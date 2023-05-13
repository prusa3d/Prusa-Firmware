//! @file
#ifndef _LCD_H
#define _LCD_H

#include <inttypes.h>
#include <stdio.h>
#include "Timer.h"



extern FILE _lcdout;

#define lcdout (&_lcdout)

extern void lcd_init(void);

extern void lcd_refresh(void);

extern void lcd_refresh_noclear(void);

extern void lcd_clear(void);

extern void lcd_home(void);

/*extern void lcd_no_display(void);
extern void lcd_display(void);
extern void lcd_no_blink(void);
extern void lcd_blink(void);
extern void lcd_no_cursor(void);
extern void lcd_cursor(void);
extern void lcd_scrollDisplayLeft(void);
extern void lcd_scrollDisplayRight(void);
extern void lcd_leftToRight(void);
extern void lcd_rightToLeft(void);
extern void lcd_autoscroll(void);
extern void lcd_no_autoscroll(void);*/

extern void lcd_set_cursor(uint8_t col, uint8_t row);

/// @brief Change the cursor column position while preserving the current row position
/// @param col column number, ranges from 0 to LCD_WIDTH - 1
void lcd_set_cursor_column(uint8_t col);

extern void lcd_createChar_P(uint8_t, const uint8_t*);


// char c is non-standard, however it saves 1B on stack
extern int lcd_putc(char c);
extern int lcd_putc_at(uint8_t c, uint8_t r, char ch);

extern int lcd_puts_P(const char* str);
extern int lcd_puts_at_P(uint8_t c, uint8_t r, const char* str);
extern int lcd_printf_P(const char* format, ...);
extern void lcd_space(uint8_t n);

extern void lcd_printNumber(unsigned long n, uint8_t base);

extern void lcd_print(const char*);
extern uint8_t lcd_print_pad(const char* s, uint8_t len);

/// @brief print a string from PROGMEM with left-adjusted padding
/// @param s string from PROGMEM.
/// @param len maximum number of characters to print, including padding. Ranges from 0 to LCD_WIDTH.
/// @return number of padded bytes. 0 means there was no padding.
uint8_t lcd_print_pad_P(const char* s, uint8_t len);
extern void lcd_print(char, int = 0);
extern void lcd_print(unsigned char, int = 0);
extern void lcd_print(int, int = 10);
extern void lcd_print(unsigned int, int = 10);
extern void lcd_print(long, int = 10);
extern void lcd_print(unsigned long, int = 10);

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



#define LCD_UPDATE_INTERVAL    100
#define LCD_TIMEOUT_TO_STATUS 30000ul //!< Generic timeout to status screen in ms, when no user action.
#define LCD_TIMEOUT_TO_STATUS_BABYSTEP_Z 90000ul //!< Specific timeout for lcd_babystep_z screen in ms.



typedef void (*lcd_longpress_func_t)(void);

typedef void (*lcd_charsetup_func_t)(void);

typedef void (*lcd_lcdupdate_func_t)(void);

//Set to none-zero when the LCD needs to draw, decreased after every draw. Set to 2 in LCD routines so the LCD gets at least 1 full redraw (first redraw is partial)
extern uint8_t lcd_draw_update;

extern int16_t lcd_encoder;

extern uint8_t lcd_click_trigger;

extern uint8_t lcd_update_enabled;

extern LongTimer lcd_timeoutToStatus;

extern uint32_t lcd_next_update_millis;

extern lcd_longpress_func_t lcd_longpress_func;
extern bool lcd_longpress_trigger;

extern lcd_lcdupdate_func_t lcd_lcdupdate_func;



extern uint8_t lcd_clicked(void);

extern void lcd_beeper_quick_feedback(void);

//Cause an LCD refresh, and give the user visual or audible feedback that something has happened
extern void lcd_quick_feedback(void);

/// @brief Check whether knob is rotated or clicked and update relevant
///variables. Flags are set by lcd_buttons_update in ISR context.
extern void lcd_knob_update();

extern void lcd_update(uint8_t lcdDrawUpdateOverride);

extern void lcd_update_enable(uint8_t enabled);

extern void lcd_buttons_update(void);

//! @brief Helper class to temporarily disable LCD updates
//!
//! When constructed (on stack), original state state of lcd_update_enabled is stored
//! and LCD updates are disabled.
//! When destroyed (gone out of scope), original state of LCD update is restored.
//! It has zero overhead compared to storing bool saved = lcd_update_enabled
//! and calling lcd_update_enable(false) and lcd_update_enable(saved).
struct LCDUpdateEnableRAII
{
public:
    explicit inline __attribute__((always_inline)) LCDUpdateEnableRAII(): m_updateEnabled(lcd_update_enabled)
    {
        lcd_update_enabled = 0;
    }
    inline __attribute__((always_inline)) ~LCDUpdateEnableRAII() {
        lcd_update_enabled = m_updateEnabled;
    }

private:
    uint8_t m_updateEnabled;
};

static_assert(sizeof(LCDUpdateEnableRAII) == 1);

////////////////////////////////////

//! @brief Was button clicked?
//!
//! Doesn't consume button click event. See lcd_clicked()
//! for function consuming the event.
//!
//! Generally is used in non-modal menus.
//!
//! @retval 0 button was not clicked
//! @retval 1 button was clicked
#define LCD_CLICKED (lcd_click_trigger)

////////////////////////////////////

//Custom characters defined in the first 8 characters of the LCD
#define LCD_STR_ARROW_RIGHT  "\x7E" //from the default character set
#define LCD_STR_ARROW_LEFT   "\x7F" //from the default character set
#define LCD_STR_BEDTEMP      "\x80"
#define LCD_STR_DEGREE       "\x81"
#define LCD_STR_THERMOMETER  "\x82"
#define LCD_STR_UPLEVEL      "\x83"
#define LCD_STR_REFRESH      "\x84"
#define LCD_STR_FOLDER       "\x85"
#define LCD_STR_FEEDRATE     "\x86"
#define LCD_STR_CLOCK        "\x87"
#define LCD_STR_ARROW_2_DOWN "\x88"
#define LCD_STR_CONFIRM      "\x89"
#define LCD_STR_SOLID_BLOCK  "\xFF"  //from the default character set

extern void lcd_frame_start();

//! @brief Consume click and longpress event
inline void lcd_consume_click()
{
    lcd_click_trigger = 0;
    lcd_longpress_trigger = 0;
}


#endif //_LCD_H
