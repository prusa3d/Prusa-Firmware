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

static_assert(sizeof(menu_data)>= sizeof(menu_data_edit_t),"menu_data_edit_t doesn't fit into menu_data");

void menu_data_reset(void)
{
	// Resets the global shared C union.
	// This ensures, that the menu entered will find out, that it shall initialize itself.
	memset(&menu_data, 0, sizeof(menu_data));
}

void menu_goto(menu_func_t menu, const int16_t encoder, bool reset_menu_state, const bool feedback)
{
	CRITICAL_SECTION_START;
	if (menu_menu != menu)
	{
		menu_menu = menu;
		lcd_encoder = encoder;
		menu_top = 0; //reset menu view. Needed if menu_back() is called from deep inside a menu, such as Support
		lcd_draw_update = 2; // Full LCD re-draw
		CRITICAL_SECTION_END;
		if (feedback) lcd_beeper_quick_feedback();
		if (reset_menu_state) menu_data_reset();
	}
	else
		CRITICAL_SECTION_END;
}

void menu_start(void)
{
    if (lcd_encoder < 0)
    {
        lcd_encoder = 0;
		Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
    }
    if (lcd_encoder < menu_top)
		menu_top = lcd_encoder;
    menu_line = menu_top;
    menu_clicked = lcd_clicked(); // Consume click event
}

void menu_end(void)
{
	if (menu_row >= LCD_HEIGHT)
	{
		// Early abort if the menu was clicked. The current menu might have changed because of the click event
		return;
	}
	if (lcd_encoder >= menu_item)
	{
		lcd_encoder = menu_item - 1;
		Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
	}
	if (((uint8_t)lcd_encoder) >= menu_top + LCD_HEIGHT)
	{
		menu_top = lcd_encoder - LCD_HEIGHT + 1;
		menu_line = menu_top - 1;
		menu_row = -1;
	}
}

void menu_back(uint8_t nLevel)
{
     menu_depth = ((menu_depth > nLevel) ? (menu_depth - nLevel) : 0);
     menu_goto(menu_stack[menu_depth].menu, menu_stack[menu_depth].position, true);
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
		menu_goto(menu_stack[menu_depth].menu, menu_stack[menu_depth].position, false);
	}
}

void menu_back_if_clicked(void)
{
	if (lcd_clicked())
		menu_back();
}

void menu_submenu(menu_func_t submenu, const bool feedback)
{
	if (menu_depth < MENU_DEPTH_MAX)
	{
		menu_stack[menu_depth].menu = menu_menu;
		menu_stack[menu_depth++].position = lcd_encoder;
		menu_goto(submenu, 0, true, feedback);
	}
}

void menu_submenu_no_reset(menu_func_t submenu, const bool feedback)
{
	if (menu_depth < MENU_DEPTH_MAX)
	{
		menu_stack[menu_depth].menu = menu_menu;
		menu_stack[menu_depth++].position = lcd_encoder;
		menu_goto(submenu, 0, false, feedback);
	}
}

void menu_item_ret(void)
{
	lcd_draw_update = 2;
	menu_item++;

	//prevent the rest of the menu items from rendering or getting clicked
	menu_row = LCD_HEIGHT; // early exit from the MENU_BEGIN() for loop at the end of the current cycle
	menu_line = 0; // prevent subsequent menu items from rendering at all in the current MENU_BEGIN() for loop cycle
	menu_clicked = 0; // prevent subsequent items from being able to be clicked in case the current menu or position was changed by the clicked menu item
}

static char menu_selection_mark(){
	return (lcd_encoder == menu_item)?'>':' ';
}

static void menu_draw_item_puts_P(char type_char, const char* str)
{
    lcd_putc_at(0, menu_row, menu_selection_mark());
    lcd_print_pad_P(str, LCD_WIDTH - 2);
    lcd_putc(type_char);
}

static void menu_draw_toggle_puts_P(const char* str, const char* toggle, const uint8_t settings)
{
    //settings:
    //xxxxxcba
    //a = selection mark. If it's set(1), then '>' will be used as the first character on the line. Else leave blank
    //b = toggle string is from progmem
    uint8_t is_progmem = settings & 0x02;
    const char eol = (toggle == NULL) ? LCD_STR_ARROW_RIGHT[0] : ' ';
    if (toggle == NULL) toggle = _T(MSG_NA);
    uint8_t len = 4 + (is_progmem ? strlen_P(toggle) : strlen(toggle));
	lcd_putc_at(0, menu_row, (settings & 0x01) ? '>' : ' ');
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
    SheetFormatBuffer buffer;
    menu_format_sheet_select_E(sheet, buffer);
    lcd_putc_at(0, menu_row, menu_selection_mark());
    lcd_print_pad(buffer.c, LCD_WIDTH - 2);
    lcd_putc(type_char);
}


static void menu_draw_item_puts_E(char type_char, const Sheet &sheet)
{
    SheetFormatBuffer buffer;
    menu_format_sheet_E(sheet, buffer);
    lcd_putc_at(0, menu_row, menu_selection_mark());
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
		{
			menu_item_ret();
			return 1;
		}
	}
	menu_item++;
	return 0;
}

void menu_item_submenu_P(const char* str, menu_func_t submenu)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update) menu_draw_item_puts_P(LCD_STR_ARROW_RIGHT[0], str);
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			menu_submenu(submenu);
			menu_item_ret();
			return;
		}
	}
	menu_item++;
}

void menu_item_submenu_E(const Sheet &sheet, menu_func_t submenu)
{
    if (menu_item == menu_line)
    {
        if (lcd_draw_update) menu_draw_item_puts_E(LCD_STR_ARROW_RIGHT[0], sheet);
        if (menu_clicked && (lcd_encoder == menu_item))
        {
            menu_submenu(submenu);
            menu_item_ret();
            return;
        }
    }
    menu_item++;
}

void __attribute__((noinline)) menu_item_function_E(const Sheet &sheet, menu_func_t func)
{
    if (menu_item == menu_line)
    {
        if (lcd_draw_update) menu_draw_item_select_sheet_E(' ', sheet);
        if (menu_clicked && (lcd_encoder == menu_item))
        {
            lcd_update_enabled = 0;
            if (func) func();
            lcd_update_enabled = 1;
            menu_item_ret();
            return;
        }
    }
    menu_item++;
}

void menu_item_back_P(const char* str)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update) menu_draw_item_puts_P(LCD_STR_UPLEVEL[0], str);
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			menu_back();
			menu_item_ret();
			return;
		}
	}
	menu_item++;
}

bool __attribute__((noinline)) menu_item_leave(){
    return ((menu_item == menu_line) && menu_clicked && (lcd_encoder == menu_item)) || menu_leaving;
}

void menu_item_function_P(const char* str, menu_func_t func)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update) menu_draw_item_puts_P(' ', str);
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			lcd_update_enabled = 0;
			if (func) func();
			lcd_update_enabled = 1;
			menu_item_ret();
			return;
		}
	}
	menu_item++;
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
void menu_item_function_P(const char* str, char number, void (*func)(uint8_t), uint8_t fn_par)
{
    if (menu_item == menu_line)
    {
        if (lcd_draw_update) menu_draw_item_puts_P(' ', str, number);
        if (menu_clicked && (lcd_encoder == menu_item))
        {
            lcd_update_enabled = 0;
            if (func) func(fn_par);
            lcd_update_enabled = 1;
            menu_item_ret();
            return;
        }
    }
    menu_item++;
}

void menu_item_toggle_P(const char* str, const char* toggle, menu_func_t func, const uint8_t settings)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update) menu_draw_toggle_puts_P(str, toggle, settings | (menu_selection_mark()=='>'));
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			if (toggle == NULL) // print N/A warning message
			{
				menu_submenu(func);
			}
			else // do the actual toggling
			{
				lcd_update_enabled = 0;
				if (func) func();
				lcd_update_enabled = 1;
			}
			menu_item_ret();
			return;
		}
	}
	menu_item++;
}

void menu_item_gcode_P(const char* str, const char* str_gcode)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update) menu_draw_item_puts_P(' ', str);
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			if (str_gcode) enquecommand_P(str_gcode);
			menu_item_ret();
			return;
		}
	}
	menu_item++;
}

const char menu_fmt_float31[] PROGMEM = "%-12.12S%+8.1f";
const char menu_fmt_float13[] PROGMEM = "%c%-13.13S%+5.3f";

/// @brief Draw the label and value for a menu edit item
/// @param chr 1 byte character
/// @param str String residing in program memory (PROGMEM)
/// @param val value to render, ranges from -999 to 9999
static void menu_draw_P(const char chr, const char* str, const int16_t val)
{
	lcd_putc(chr);
	lcd_puts_P(str);
	lcd_putc(':');

	// Padding to compensate variable string length
	const uint8_t len = strlen_P(str);
	lcd_space((LCD_WIDTH - 4) - (2 + len));

	// Right adjusted value
	lcd_printf_P(PSTR("%4d"), val);
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

static void _menu_edit_P()
{
	menu_data_edit_t* _md = (menu_data_edit_t*)&(menu_data[0]);
	if (lcd_draw_update)
	{
        // handle initial value jumping
        if (_md->minJumpValue && lcd_encoder) {
            if (lcd_encoder > 0 && _md->currentValue == _md->minEditValue) {
                _md->currentValue = _md->minJumpValue;
                lcd_encoder = 0;
            }
            // disable after first use and/or if the initial value is not minEditValue
            _md->minJumpValue = 0;
        }

		_md->currentValue += lcd_encoder;
		lcd_encoder = 0; // Consume knob rotation event

		// Constrain the value in case it's outside the allowed limits
		_md->currentValue = constrain(_md->currentValue, _md->minEditValue, _md->maxEditValue);
		lcd_set_cursor(0, 1);
		menu_draw_P(' ', _md->editLabel, _md->currentValue);
	}
	if (lcd_clicked())
	{
        if (_md->editValueBits == 8)
            *((uint8_t*)(_md->editValuePtr)) = _md->currentValue;
        else
            *((int16_t*)(_md->editValuePtr)) = _md->currentValue;
		menu_back_no_reset();
	}
}

void menu_item_edit_P(const char* str, void* pval, uint8_t pbits, int16_t min_val, int16_t max_val, int16_t jmp_val)
{
	menu_data_edit_t* _md = (menu_data_edit_t*)&(menu_data[0]);
	if (menu_item == menu_line)
	{
        int16_t cur_val = (pbits == 8 ? *((uint8_t*)pval) : *((int16_t*)pval));

		if (lcd_draw_update)
		{
			lcd_set_cursor(0, menu_row);
			menu_draw_P(menu_selection_mark(), str, cur_val);
		}
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			menu_submenu_no_reset(_menu_edit_P);
			_md->editLabel = str;
			_md->editValuePtr = pval;
			_md->editValueBits = pbits;
			_md->currentValue = cur_val;
			_md->minEditValue = min_val;
			_md->maxEditValue = max_val;
			_md->minJumpValue = jmp_val;
			menu_item_ret();
			return;
		}
	}
	menu_item++;
}

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
