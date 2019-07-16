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

extern void lcd_createChar_P(uint8_t, const uint8_t*);


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



#define LCD_UPDATE_INTERVAL    100
#define LCD_TIMEOUT_TO_STATUS 30000ul //!< Generic timeout to status screen in ms, when no user action.
#define LCD_TIMEOUT_TO_STATUS_BABYSTEP_Z 90000ul //!< Specific timeout for lcd_babystep_z screen in ms.



typedef void (*lcd_longpress_func_t)(void);

typedef void (*lcd_charsetup_func_t)(void);

typedef void (*lcd_lcdupdate_func_t)(void);

//Set to none-zero when the LCD needs to draw, decreased after every draw. Set to 2 in LCD routines so the LCD gets at least 1 full redraw (first redraw is partial)
extern uint8_t lcd_draw_update;

extern int32_t lcd_encoder;

extern uint8_t lcd_encoder_bits;

// lcd_encoder_diff is updated from interrupt context and added to lcd_encoder every LCD update
extern int8_t lcd_encoder_diff;

//the last checked lcd_buttons in a bit array.
extern uint8_t lcd_buttons;

extern uint8_t lcd_button_pressed;

extern uint8_t lcd_update_enabled;

extern LongTimer lcd_timeoutToStatus;

extern uint32_t lcd_next_update_millis;

extern uint8_t lcd_status_update_delay;

extern lcd_longpress_func_t lcd_longpress_func;

extern lcd_charsetup_func_t lcd_charsetup_func;

extern lcd_lcdupdate_func_t lcd_lcdupdate_func;



extern uint8_t lcd_clicked(void);

extern void lcd_beeper_quick_feedback(void);

//Cause an LCD refresh, and give the user visual or audible feedback that something has happened
extern void lcd_quick_feedback(void);

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
class LcdUpdateDisabler
{
public:
    LcdUpdateDisabler(): m_updateEnabled(lcd_update_enabled)
    {
        lcd_update_enable(false);
    }
    ~LcdUpdateDisabler()
    {
        lcd_update_enable(m_updateEnabled);
    }

private:
    bool m_updateEnabled;
};


////////////////////////////////////
// Setup button and encode mappings for each panel (into 'lcd_buttons' variable
//
// This is just to map common functions (across different panels) onto the same 
// macro name. The mapping is independent of whether the button is directly connected or 
// via a shift/i2c register.

#define BLEN_B 1
#define BLEN_A 0
#define EN_B (1<<BLEN_B) // The two encoder pins are connected through BTN_EN1 and BTN_EN2
#define EN_A (1<<BLEN_A)
#define BLEN_C 2 
#define EN_C (1<<BLEN_C) 

//! @brief Was button clicked?
//!
//! Doesn't consume button click event. See lcd_clicked()
//! for function consuming the event.
//!
//! Generally is used in non-modal menus.
//!
//! @retval 0 button was not clicked
//! @retval 1 button was clicked
#define LCD_CLICKED (lcd_buttons&EN_C)

////////////////////////
// Setup Rotary Encoder Bit Values (for two pin encoders to indicate movement)
// These values are independent of which pins are used for EN_A and EN_B indications
// The rotary encoder part is also independent to the chipset used for the LCD
#define encrot0 0
#define encrot1 2
#define encrot2 3
#define encrot3 1


//Custom characters defined in the first 8 characters of the LCD
#define LCD_STR_BEDTEMP     "\x00"
#define LCD_STR_DEGREE      "\x01"
#define LCD_STR_THERMOMETER "\x02"
#define LCD_STR_UPLEVEL     "\x03"
#define LCD_STR_REFRESH     "\x04"
#define LCD_STR_FOLDER      "\x05"
#define LCD_STR_FEEDRATE    "\x06"
#define LCD_STR_CLOCK       "\x07"
#define LCD_STR_ARROW_UP    "\x0B"
#define LCD_STR_ARROW_DOWN  "\x01"
#define LCD_STR_ARROW_RIGHT "\x7E" //from the default character set

extern void lcd_set_custom_characters(void);
extern void lcd_set_custom_characters_arrows(void);
extern void lcd_set_custom_characters_progress(void);
extern void lcd_set_custom_characters_nextpage(void);
extern void lcd_set_custom_characters_degree(void);

//! @brief Consume click event
inline void lcd_consume_click()
{
    lcd_button_pressed = 0;
    lcd_buttons &= 0xff^EN_C;
}


#endif //_LCD_H
