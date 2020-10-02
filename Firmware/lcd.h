//! @file
#ifndef _LCD_H
#define _LCD_H

#include <inttypes.h>
#include <stdio.h>


// #define VT100
// #define LCD_DEBUG


extern FILE _lcdout;

#define lcdout (&_lcdout)

extern uint8_t vga_get_char(int i, int j);

extern void lcd_init(void);

extern void lcd_timer_enable(void);

extern void lcd_timer_disable(void);

extern void lcd_redraw(bool forceRedraw = false);

extern void lcd_clear(void);

extern void lcd_home(void);

/*extern void lcd_no_display(void);
extern void lcd_display(void);*/

extern void lcd_set_cursor(uint8_t col, uint8_t row);


extern int lcd_putc(int c);
extern int lcd_puts_P(const char* str);
extern int lcd_puts_at_P(uint8_t c, uint8_t r, const char* str);
extern int lcd_printf_P(const char* format, ...);
extern void lcd_space(uint8_t n);

extern void lcd_printNumber(unsigned long n, uint8_t base);
extern void lcd_printFloat(double number, uint8_t digits);

extern void lcd_print(const char*);
extern void lcd_print(char, int = 0);
extern void lcd_print(unsigned char, int = 0);
extern void lcd_print(int, int = 10);
extern void lcd_print(unsigned int, int = 10);
extern void lcd_print(long, int = 10);
extern void lcd_print(unsigned long, int = 10);
extern void lcd_print(double, int = 2);

//! @brief Clear screen
#define ESC_2J     "\x1b[2J"
//! @brief Show cursor
#define ESC_25h    "\x1b[?25h"
//! @brief Hide cursor
#define ESC_25l    "\x1b[?25l"
//! @brief Set cursor to
//! @param c column
//! @param r row
#define ESC_H(c,r) "\x1b["#r";"#c"H"


#ifdef LCD_DEBUG
extern void lcd_debug();
#endif //LCD_DEBUG


//Custom characters defined in the first 8 characters of the LCD

//It is split into two memory banks. On the actual LCD, only one is in memory at a time.
//Data from the two banks can be mixed together, but one must make sure the symbols of the same 3 bit address don't overlap.
//for example, you can't have LCD_STR_BEDTEMP[0] and LCD_STR_ARROW_DOWN[0] on the screen at the same time,
//but LCD_STR_THERMOMETER[0], LCD_STR_DEGREE[0] and LCD_STR_ARROW_DOWN[0] can all be used together.
//Also, since we are overwriting the standard ASCII table with the custom characters, 0x0A must remain the NL character.

//BANK 0:
#define LCD_STR_BEDTEMP       "\x00"
#define LCD_STR_DEGREE        "\x01"
#define LCD_STR_THERMOMETER   "\x02"
#define LCD_STR_UPLEVEL       "\x03"
#define LCD_STR_REFRESH       "\x04"
#define LCD_STR_FOLDER        "\x05"
#define LCD_STR_FEEDRATE      "\x06"
#define LCD_STR_CLOCK         "\x07"
//BANK 1:
#define LCD_STR_ARROW_DOWN    "\x08"
#define LCD_STR_ARROW_2_DOWN  "\x09"
#define LCD_STR_RESERVED_NL   "\x0A" //this is the newLine character. It is reserved for it's original purpose.
#define LCD_STR_CONFIRM       "\x0B"

//BANK DEFAULT - from the default character set
#define LCD_STR_ARROW_RIGHT   "\x7E"
#define LCD_STR_PROGRESS      "\xFF"

#ifdef LCD_DEBUG
extern uint8_t lcd_custom_character_bank;
#endif //LCD_DEBUG

#endif //_LCD_H
