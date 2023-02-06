//menu.cpp

#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <avr/pgmspace.h>
#include "lcd.h"
#include "Configuration.h"
#include "Marlin.h"
#include "cmdqueue.h"
#include "ultralcd.h"
#include "language.h"
#include "static_assert.h"
#include "sound.h"

#define MENU_DEPTH_MAX       7

static menu_record_t menu_stack[MENU_DEPTH_MAX];

uint8_t menu_data[MENU_DATA_SIZE];
#ifndef __AVR__
#error "menu_data is non-portable to non 8bit processor"
#endif

uint8_t menu_depth = 0;
uint8_t menu_block_mask = MENU_BLOCK_NONE;
uint8_t menu_line = 0;
uint8_t menu_item = 0;
uint8_t menu_row = 0;
uint8_t menu_top = 0;

uint8_t menu_clicked = 0;

uint8_t menu_leaving = 0;

menu_func_t menu_menu = 0;

static_assert(sizeof(menu_data)>= sizeof(menu_data_edit_t<int16_t>),"menu_data_edit_t doesn't fit into menu_data");

void menu_data_reset(void)
{
	// Resets the global shared C union.
	// This ensures, that the menu entered will find out, that it shall initialize itself.
	memset(&menu_data, 0, sizeof(menu_data));
}

void menu_goto(menu_func_t menu, const int8_t encoder, const bool feedback, bool reset_menu_state)
{
	CRITICAL_SECTION_START;
	if (menu_menu != menu)
	{
		menu_menu = menu;
		lcd_encoder = encoder;
		menu_top = 0; //reset menu view. Needed if menu_back() is called from deep inside a menu, such as Support
		CRITICAL_SECTION_END;
		if (reset_menu_state)
			menu_data_reset();

		if (feedback) lcd_quick_feedback();
	}
	else
		CRITICAL_SECTION_END;
}

void menu_start(void)
{
	limit_lcd_encoder = 1;
    if (lcd_encoder < menu_top)
		menu_top = lcd_encoder;
    menu_line = menu_top;
    menu_clicked = LCD_CLICKED;
	if (menu_clicked)
	{
		// if a menu item is clicked, then remove the limit
		// if we enter a submenu, menu_start will re-enable the limit
		limit_lcd_encoder = 0;
	}
}

void menu_end(void)
{
	if (lcd_encoder >= menu_item)
	{
		lcd_encoder = menu_item - 1;
		Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
	}
	if (lcd_encoder >= (menu_top + LCD_HEIGHT))
	{
		menu_top = lcd_encoder - LCD_HEIGHT + 1;
		lcd_draw_update = 1;
		menu_line = menu_top - 1;
		menu_row = -1;
	}
	
}

void menu_back(uint8_t nLevel)
{
     menu_depth = ((menu_depth > nLevel) ? (menu_depth - nLevel) : 0);
     menu_goto(menu_stack[menu_depth].menu, menu_stack[menu_depth].position, true, true);
}

void menu_back(void)
{
menu_back(1);
}

void menu_back_no_reset(void)
{
	if (menu_depth > 0)
	{
		menu_depth--;		
		menu_goto(menu_stack[menu_depth].menu, menu_stack[menu_depth].position, true, false);
	}
}

void menu_back_if_clicked(void)
{
	if (lcd_clicked())
		menu_back();
}

void menu_back_if_clicked_fb(void)
{
	if (lcd_clicked())
	{
        lcd_quick_feedback();
		menu_back();
	}
}

void menu_submenu(menu_func_t submenu)
{
	if (menu_depth < MENU_DEPTH_MAX)
	{
		menu_stack[menu_depth].menu = menu_menu;
		menu_stack[menu_depth++].position = lcd_encoder;
		menu_goto(submenu, 0, true, true);
	}
}

void menu_submenu_no_reset(menu_func_t submenu)
{
	if (menu_depth < MENU_DEPTH_MAX)
	{
		menu_stack[menu_depth].menu = menu_menu;
		menu_stack[menu_depth++].position = lcd_encoder;
		menu_goto(submenu, 0, true, false);
	}
}

uint8_t menu_item_ret(void)
{
	lcd_quick_feedback();
	return 1;
}

static char menu_selection_mark(){
	return (lcd_encoder == menu_item)?'>':' ';
}

static void menu_draw_item_puts_P(char type_char, const char* str)
{
    lcd_set_cursor(0, menu_row);
    lcd_putc(menu_selection_mark());
    lcd_print_pad_P(str, LCD_WIDTH - 2);
    lcd_putc(type_char);
}

static void menu_draw_toggle_puts_P(const char* str, const char* toggle, const uint8_t settings)
{
    //settings:
    //xxxxxcba
    //a = selection mark. If it's set(1), then '>' will be used as the first character on the line. Else leave blank
    //b = toggle string is from progmem
    //c = do not set cursor at all. Must be handled externally.
    uint8_t is_progmem = settings & 0x02;
    const char eol = (toggle == NULL) ? LCD_STR_ARROW_RIGHT[0] : ' ';
    if (toggle == NULL) toggle = _T(MSG_NA);
    uint8_t len = 4 + (is_progmem ? strlen_P(toggle) : strlen(toggle));
    if (!(settings & 0x04)) lcd_set_cursor(0, menu_row);
    lcd_putc((settings & 0x01) ? '>' : ' ');
    lcd_print_pad_P(str, LCD_WIDTH - len);
    lcd_putc('[');
    if (is_progmem)
    {
        lcd_puts_P(toggle);
    } else {
        lcd_print(toggle);
    }
    lcd_putc(']');
    lcd_putc(eol);
}

//! @brief Format sheet name
//!
//! @param[in] sheet_E Sheet in EEPROM
//! @param[out] buffer for formatted output
void menu_format_sheet_E(const Sheet &sheet_E, SheetFormatBuffer &buffer)
{
    uint_least8_t index = sprintf_P(buffer.c, PSTR("%.10S "), _T(MSG_SHEET));
    eeprom_read_block(&(buffer.c[index]), sheet_E.name, 7);
    //index += 7;
    buffer.c[index + 7] = '\0';
}

//! @brief Format sheet name in select menu
//!
//! @param[in] sheet_E Sheet in EEPROM
//! @param[out] buffer for formatted output
void menu_format_sheet_select_E(const Sheet &sheet_E, SheetFormatBuffer &buffer)
{
    uint_least8_t index = sprintf_P(buffer.c,PSTR("%-9.9S["), _T(MSG_SHEET));
    eeprom_read_block(&(buffer.c[index]), sheet_E.name, sizeof(sheet_E.name)/sizeof(sheet_E.name[0]));
    for (const uint_least8_t start = index; static_cast<uint_least8_t>(index - start) < sizeof(sheet_E.name)/sizeof(sheet_E.name[0]); ++index)
    {
        if (buffer.c[index] == '\0') break;
    }
	buffer.c[index] = ']';
    buffer.c[index + 1] = '\0';
}

static void menu_draw_item_select_sheet_E(char type_char, const Sheet &sheet)
{
    lcd_set_cursor(0, menu_row);
    SheetFormatBuffer buffer;
    menu_format_sheet_select_E(sheet, buffer);
    lcd_putc(menu_selection_mark());
    lcd_print_pad(buffer.c, LCD_WIDTH - 2);
    lcd_putc(type_char);
}


static void menu_draw_item_puts_E(char type_char, const Sheet &sheet)
{
    lcd_set_cursor(0, menu_row);
    SheetFormatBuffer buffer;
    menu_format_sheet_E(sheet, buffer);
    lcd_putc(menu_selection_mark());
    lcd_print_pad(buffer.c, LCD_WIDTH - 2);
    lcd_putc(type_char);
}

static void menu_draw_item_puts_P(char type_char, const char* str, char num)
{
    const uint8_t max_strlen = LCD_WIDTH - 3;
    lcd_putc_at(0, menu_row, menu_selection_mark());
    uint8_t len = lcd_print_pad_P(str, max_strlen);
    lcd_putc_at((max_strlen - len) + 2, menu_row, num);
    lcd_putc_at(LCD_WIDTH - 1, menu_row, type_char);
}

void menu_item_dummy(void)
{
	menu_item++;
}

uint8_t menu_item_text_P(const char* str)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update) menu_draw_item_puts_P(' ', str);
		if (menu_clicked && (lcd_encoder == menu_item))
			return menu_item_ret();
	}
	menu_item++;
	return 0;
}

uint8_t menu_item_submenu_P(const char* str, menu_func_t submenu)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update) menu_draw_item_puts_P(LCD_STR_ARROW_RIGHT[0], str);
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			menu_submenu(submenu);
			return menu_item_ret();
		}
	}
	menu_item++;
	return 0;
}

uint8_t menu_item_submenu_E(const Sheet &sheet, menu_func_t submenu)
{
    if (menu_item == menu_line)
    {
        if (lcd_draw_update) menu_draw_item_puts_E(LCD_STR_ARROW_RIGHT[0], sheet);
        if (menu_clicked && (lcd_encoder == menu_item))
        {
            menu_submenu(submenu);
            return menu_item_ret();
        }
    }
    menu_item++;
    return 0;
}

uint8_t __attribute__((noinline)) menu_item_function_E(const Sheet &sheet, menu_func_t func)
{
    if (menu_item == menu_line)
    {
        if (lcd_draw_update) menu_draw_item_select_sheet_E(' ', sheet);
        if (menu_clicked && (lcd_encoder == menu_item))
        {
            menu_clicked = false;
            lcd_consume_click();
            lcd_update_enabled = 0;
            if (func) func();
            lcd_update_enabled = 1;
            return menu_item_ret();
        }
    }
    menu_item++;
    return 0;
}

uint8_t menu_item_back_P(const char* str)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update) menu_draw_item_puts_P(LCD_STR_UPLEVEL[0], str);
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			menu_back();
			return menu_item_ret();
		}
	}
	menu_item++;
	return 0;
}

bool __attribute__((noinline)) menu_item_leave(){
    return ((menu_item == menu_line) && menu_clicked && (lcd_encoder == menu_item)) || menu_leaving;
}

uint8_t menu_item_function_P(const char* str, menu_func_t func)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update) menu_draw_item_puts_P(' ', str);
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			menu_clicked = false;
			lcd_consume_click();
			lcd_update_enabled = 0;
			if (func) func();
			lcd_update_enabled = 1;
			return menu_item_ret();
		}
	}
	menu_item++;
	return 0;
}

//! @brief Menu item function taking single parameter
//!
//! Ideal for numbered lists calling functions with number parameter.
//! @param str Item caption
//! @param number aditional character to be added after str, e.g. number
//! @param func pointer to function taking uint8_t with no return value
//! @param fn_par value to be passed to function
//! @retval 0
//! @retval 1 Item was clicked
uint8_t menu_item_function_P(const char* str, char number, void (*func)(uint8_t), uint8_t fn_par)
{
    if (menu_item == menu_line)
    {
        if (lcd_draw_update) menu_draw_item_puts_P(' ', str, number);
        if (menu_clicked && (lcd_encoder == menu_item))
        {
            menu_clicked = false;
            lcd_consume_click();
            lcd_update_enabled = 0;
            if (func) func(fn_par);
            lcd_update_enabled = 1;
            return menu_item_ret();
        }
    }
    menu_item++;
    return 0;
}

uint8_t menu_item_toggle_P(const char* str, const char* toggle, menu_func_t func, const uint8_t settings)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update) menu_draw_toggle_puts_P(str, toggle, settings | (menu_selection_mark()=='>'));
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			if (toggle == NULL) // print N/A warning message
			{
				menu_submenu(func);
				return menu_item_ret();
			}
			else // do the actual toggling
			{
				menu_clicked = false;
				lcd_consume_click();
				lcd_update_enabled = 0;
				if (func) func();
				lcd_update_enabled = 1;
				return menu_item_ret();
			}
		}
	}
	menu_item++;
	return 0;
}

uint8_t menu_item_gcode_P(const char* str, const char* str_gcode)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update) menu_draw_item_puts_P(' ', str);
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			if (str_gcode) enquecommand_P(str_gcode);
			return menu_item_ret();
		}
	}
	menu_item++;
	return 0;
}

const char menu_fmt_float31[] PROGMEM = "%-12.12S%+8.1f";

const char menu_fmt_float13[] PROGMEM = "%c%-13.13S%+5.3f";

static float menu_edit_convert_to_float(const int16_t val, const uint8_t decimals)
{
	return ((float)val / (10 * decimals));
}

static int16_t menu_edit_convert_from_float(const float val, const uint8_t decimals)
{
	return (int16_t)(val * 10 * decimals);
}

template <typename T, typename D>
static D menu_edit_get_current_value(T val, const uint8_t decimals)
{
	if (sizeof(T) > 2) {
		return menu_edit_convert_from_float(val, decimals);
	} else {
		return val;
	}
}


void menu_draw_item_P(char chr, const char* str, int16_t val)
{
	// The LCD row position is controlled externally. We may only modify the column here
	lcd_putc(chr);
	uint8_t len = lcd_print_pad_P(str, LCD_WIDTH - 1);
	lcd_set_cursor_column((LCD_WIDTH - 1) - len + 1);
	lcd_putc(' ');

	// The value is right adjusted, set the cursor then render the value
	if (val < 10) { // 1 digit
		lcd_set_cursor_column(LCD_WIDTH - 1);
	} else if (val < 100) { // 2 digits
		lcd_set_cursor_column(LCD_WIDTH - 2);
	} else { // 3 digits
		lcd_set_cursor_column(LCD_WIDTH - 3);
	}
	lcd_print(val);
}

void __attribute__((noinline)) menu_draw_edit_P(char chr, const char* str, int16_t val, uint8_t isFloat)
{
    if (isFloat) {
        float temp = menu_edit_convert_to_float(val, 3);
        lcd_printf_P(menu_fmt_float13, chr, str, temp);
    } else {
        menu_draw_item_P(chr, str, val);
    }
}

//! @brief Draw up to 10 chars of text and a float number in format from +0.0 to +12345.0. The increased range is necessary
//! for displaying large values of extruder positions, which caused text overflow in the previous implementation.
//! 
//! @param str string label to print
//! @param val value to print aligned to the right side of the display  
//! 
//! Implementation comments:
//! The text needs to come with a colon ":", this function does not append it anymore.
//! That resulted in a much shorter implementation (234628B -> 234476B)
//! There are similar functions around which may be shortened in a similar way
void menu_draw_float31(const char* str, float val)
{
	lcd_printf_P(menu_fmt_float31, str, val);	
}

//! @brief Draw up to 14 chars of text and a float number in format +1.234
//! 
//! @param str string label to print
//! @param val value to print aligned to the right side of the display  
//! 
//! Implementation comments:
//! This function uses similar optimization principles as menu_draw_float31
//! (i.e. str must include a ':' at its end)
//! FLASH usage dropped 234476B -> 234392B
//! Moreover, this function gets inlined in the final code, so removing it doesn't really help ;)
void menu_draw_float13(const char* str, float val)
{
	lcd_printf_P(menu_fmt_float13, ' ', str, val);
}

template <typename T, typename D>
static void _menu_edit_P()
{
	menu_data_edit_t<D>* _md = (menu_data_edit_t<D>*)&(menu_data[0]);
	if (lcd_draw_update)
	{
		// Increment the current value
		_md->currentValue += lcd_encoder;
		lcd_encoder = 0;

		// Constrain the value in case it's outside the allowed limits
		_md->currentValue = constrain(_md->currentValue, _md->minEditValue, _md->maxEditValue);

		// Render the updated value
		lcd_set_cursor(0, 1);
		menu_draw_edit_P(' ', _md->editLabel, _md->currentValue, sizeof(T) > 2);
	}
	if (LCD_CLICKED)
	{
		// Save the value
		if (sizeof(T) > 2) {
			*(T*)_md->editValue = menu_edit_convert_to_float(_md->currentValue, _md->decimals);
		} else {
			*(T*)_md->editValue = _md->currentValue;
		}
		menu_back_no_reset();
	}
}

template <typename T, typename D>
uint8_t __attribute__((noinline)) menu_item_edit_P(const char* str, T* const pval, const D* settings, const uint8_t is_progmem, const uint8_t decimals)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update)
		{
			lcd_set_cursor(0, menu_row);
			menu_draw_edit_P(menu_selection_mark(), str, menu_edit_get_current_value<T, D>(*pval, decimals), sizeof(T) > 2);
		}
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			menu_data_edit_t<D>* _md = (menu_data_edit_t<D>*)&(menu_data[0]);
			// Initialise the menu data before opening edit menu
			_md->editLabel = str;
			_md->editValue = pval;
			_md->currentValue = menu_edit_get_current_value<T, D>(*pval, decimals);
			_md->decimals = decimals;
			if (!is_progmem) {
				memcpy(&_md->minEditValue, settings, sizeof(D)*3);
			} else { // Handle limits stored in PROGMEM
				memcpy_P(&_md->minEditValue, settings, sizeof(D)*3);
			}

			if (!_md->currentValue && _md->jumpEditValue) {
				// If current value is 0, and the jump value is defined (not zero)
				// set the current value to the jump value to save time.
				_md->currentValue = _md->jumpEditValue;
			}

			// Render the editing menu
			menu_submenu_no_reset(_menu_edit_P<T, D>);

			return menu_item_ret();
		}
	}
	menu_item++;
	return 0;
}

template uint8_t menu_item_edit_P<float, int16_t>(const char* str, float* const pval, const int16_t* settings, const uint8_t is_progmem, const uint8_t decimals);
template uint8_t menu_item_edit_P<int16_t, int16_t>(const char* str, int16_t* const pval, const int16_t* settings, const uint8_t is_progmem, const uint8_t decimals);
template uint8_t menu_item_edit_P<uint8_t, uint8_t>(const char* str, uint8_t* const pval, const uint8_t* settings, const uint8_t is_progmem, const uint8_t decimals);
template uint8_t menu_item_edit_P<int8_t, int8_t>(const char* str, int8_t* const pval, const int8_t* settings, const uint8_t is_progmem, const uint8_t decimals);

static uint8_t progressbar_block_count = 0;
static uint16_t progressbar_total = 0;
void menu_progressbar_init(uint16_t total, const char* title)
{
	lcd_clear();
	progressbar_block_count = 0;
	progressbar_total = total;
	
	lcd_set_cursor(0, 1);
	lcd_print_pad_P(title, LCD_WIDTH);
	lcd_set_cursor(0, 2);
}

void menu_progressbar_update(uint16_t newVal)
{
	uint8_t newCnt = (newVal * LCD_WIDTH) / progressbar_total;
	if (newCnt > LCD_WIDTH)
		newCnt = LCD_WIDTH;
	while (newCnt > progressbar_block_count)
	{
		lcd_print(LCD_STR_SOLID_BLOCK[0]);
		progressbar_block_count++;
	}
}

void menu_progressbar_finish(void)
{
	progressbar_total = 1;
	menu_progressbar_update(1);
	_delay(300);
}
