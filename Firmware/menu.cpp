//menu.cpp

#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <avr/pgmspace.h>
#include "lcd.h"
#include "Configuration.h"
#include "Marlin.h"
#include "ultralcd.h"



extern int32_t lcd_encoder;


menu_record_t menu_stack[MENU_DEPTH_MAX];

uint8_t menu_data[MENU_DATA_SIZE];

uint8_t menu_depth = 0;

uint8_t menu_line = 0;
uint8_t menu_item = 0;
uint8_t menu_row = 0;
uint8_t menu_top = 0;

uint8_t menu_clicked = 0;

menu_func_t menu_menu = 0;


void menu_goto(menu_func_t menu, const uint32_t encoder, const bool feedback, bool reset_menu_state)
{
	asm("cli");
	if (menu_menu != menu)
	{
		menu_menu = menu;
		lcd_encoder = encoder;
		asm("sei");
		if (reset_menu_state)
		{
			// Resets the global shared C union.
			// This ensures, that the menu entered will find out, that it shall initialize itself.
			memset(&menu_data, 0, sizeof(menu_data));
		}
		if (feedback) lcd_quick_feedback();
	}
	else
		asm("sei");
}

void menu_start(void)
{
    if (lcd_encoder > 0x8000) lcd_encoder = 0;
    if (lcd_encoder < 0) lcd_encoder = 0;
    if (lcd_encoder < menu_top)
		menu_top = lcd_encoder;
    menu_line = menu_top;
    menu_clicked = LCD_CLICKED;
}

void menu_end(void)
{
	if (lcd_encoder >= menu_item)
		lcd_encoder = menu_item - 1;
	if (((uint8_t)lcd_encoder) >= menu_top + LCD_HEIGHT)
	{
		menu_top = lcd_encoder - LCD_HEIGHT + 1;
		lcd_draw_update = 1;
		menu_line = menu_top - 1;
		menu_row = -1;
	}
}

void menu_back(void)
{
	if (menu_depth > 0) {
		menu_depth--;		
		menu_goto(menu_stack[menu_depth].menu, menu_stack[menu_depth].position, true, true);
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
	if (menu_depth <= MENU_DEPTH_MAX)
	{
		menu_stack[menu_depth].menu = menu_menu;
		menu_stack[menu_depth++].position = lcd_encoder;
		menu_goto(submenu, 0, true, true);
	}
}

uint8_t menu_item_ret(void)
{
	lcd_beeper_quick_feedback();
	lcd_draw_update = 2;
	lcd_button_pressed = false;
	return 1;
}

/*
int menu_draw_item_printf_P(char type_char, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int ret = 0;
    lcd_set_cursor(0, menu_row);
	if (lcd_encoder == menu_item)
		lcd_print('>');
	else
		lcd_print(' ');
	int cnt = vfprintf_P(lcdout, format, args);
	for (int i = cnt; i < 18; i++)
		lcd_print(' ');
	lcd_print(type_char);
	va_end(args);
	return ret;
}
*/

int menu_draw_item_puts_P(char type_char, const char* str)
{
    lcd_set_cursor(0, menu_row);
	int cnt = lcd_printf_P(PSTR("%c%-18S%c"), (lcd_encoder == menu_item)?'>':' ', str, type_char);
	return cnt;
}

/*
int menu_draw_item_puts_P_int16(char type_char, const char* str, int16_t val, )
{
    lcd_set_cursor(0, menu_row);
	int cnt = lcd_printf_P(PSTR("%c%-18S%c"), (lcd_encoder == menu_item)?'>':' ', str, type_char);
	return cnt;
}
*/

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

uint8_t menu_item_function_P(const char* str, menu_func_t func)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update) menu_draw_item_puts_P(' ', str);
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			menu_clicked = false;
			lcd_update_enabled = 0;
			if (func) func();
			lcd_update_enabled = 1;
			return menu_item_ret();
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


const char menu_20x_space[] PROGMEM = "                    ";

const char menu_fmt_int3[] PROGMEM = "%c%.15S:%s%3d";

const char menu_fmt_float31[] PROGMEM = "%c%.12S:%s%+06.1f";

const char menu_fmt_float13[] PROGMEM = "%c%.12S:%s%+06.3f";


void menu_draw_int3(char chr, const char* str, int16_t val)
{
	int text_len = strlen_P(str);
	if (text_len > 15) text_len = 15;
	char spaces[21];
	strcpy_P(spaces, menu_20x_space);
	spaces[15 - text_len] = 0;
	lcd_printf_P(menu_fmt_int3, chr, str, spaces, val);
}

//draw up to 12 chars of text, ':' and float number in format +123.0
void menu_draw_float31(char chr, const char* str, float val)
{
	int text_len = strlen_P(str);
	if (text_len > 12) text_len = 12;
	char spaces[21];
	strcpy_P(spaces, menu_20x_space);
	spaces[12 - text_len] = 0;
	lcd_printf_P(menu_fmt_float31, chr, str, spaces, val);
}

//draw up to 12 chars of text, ':' and float number in format +1.234
void menu_draw_float13(char chr, const char* str, float val)
{
	int text_len = strlen_P(str);
	if (text_len > 12) text_len = 12;
	char spaces[21];
	strcpy_P(spaces, menu_20x_space);
	spaces[12 - text_len] = 0;
	lcd_printf_P(menu_fmt_float13, chr, str, spaces, val);
}

#define _menu_data menuData.edit_menu
void _menu_edit_int3(void)
{
	if (lcd_draw_update)
	{
		if (lcd_encoder < _menu_data.minEditValue) lcd_encoder = _menu_data.minEditValue;
		if (lcd_encoder > _menu_data.maxEditValue) lcd_encoder = _menu_data.maxEditValue;
		lcd_set_cursor(0, 1);
		menu_draw_int3(' ', _menu_data.editLabel, (int)lcd_encoder);
	}
	if (LCD_CLICKED)
	{
		*((int*)(_menu_data.editValue)) = (int)lcd_encoder;
		menu_back();
	}
}

uint8_t menu_item_edit_int3(const char* str, int16_t* pval, int16_t min_val, int16_t max_val)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update) 
		{
			lcd_set_cursor(0, menu_row);
			menu_draw_int3((lcd_encoder == menu_item)?'>':' ', str, *pval);
		}
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			menu_submenu(_menu_edit_int3);
			_menu_data.editLabel = str;
			_menu_data.editValue = pval;
			_menu_data.minEditValue = min_val;
			_menu_data.maxEditValue = max_val;
			lcd_encoder = *pval;
			return menu_item_ret();
		}
	}
	menu_item++;
	return 0;
}

#undef _menu_data





