//lcd.h
#ifndef _LCD_H
#define _LCD_H

#include <inttypes.h>


#define LCD_UPDATE_INTERVAL    100
#define LCD_TIMEOUT_TO_STATUS  30000


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

extern uint32_t lcd_timeoutToStatus;

extern uint32_t lcd_next_update_millis;

extern uint8_t lcd_status_update_delay;

extern uint8_t lcd_long_press_active;

extern lcd_longpress_func_t lcd_longpress_func;

extern lcd_charsetup_func_t lcd_charsetup_func;

extern lcd_lcdupdate_func_t lcd_lcdupdate_func;

extern uint8_t lcd_clicked(void);

extern void lcd_set_cursor(uint8_t c, uint8_t r);

extern void lcd_implementation_quick_feedback(void);

//Cause an LCD refresh, and give the user visual or audible feedback that something has happened
extern void lcd_quick_feedback(void);

extern int lcd_puts_P(const char* str);
extern int lcd_putc(int c);
extern int lcd_printf_P(const char* format, ...);






extern void lcd_update(uint8_t lcdDrawUpdateOverride);

extern void lcd_update_enable(uint8_t enabled);

extern void lcd_buttons_update(void);



























#include "Configuration_prusa.h"
#include "Marlin.h"



/**
* Implementation of the LCD display routines for a Hitachi HD44780 display. These are common LCD character displays.
* When selecting the Russian language, a slightly different LCD implementation is used to handle UTF8 characters.
**/


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
  
#define LCD_CLICKED (lcd_buttons&EN_C)

////////////////////////
// Setup Rotary Encoder Bit Values (for two pin encoders to indicate movement)
// These values are independent of which pins are used for EN_A and EN_B indications
// The rotary encoder part is also independent to the chipset used for the LCD
#define encrot0 0
#define encrot1 2
#define encrot2 3
#define encrot3 1


////////////////////////////////////
// Create LCD class instance and chipset-specific information
#include "LiquidCrystal_Prusa.h"
#define LCD_CLASS LiquidCrystal_Prusa
extern LCD_CLASS lcd;


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

extern void lcd_implementation_init(void);
extern void lcd_implementation_init_noclear(void);
extern void lcd_implementation_nodisplay(void);
extern void lcd_implementation_display(void);
extern void lcd_implementation_clear(void);

// Arduino < 1.0.0 is missing a function to print PROGMEM strings, so we need to implement our own
extern void lcd_printPGM(const char* str);
extern void lcd_print_at_PGM(uint8_t x, uint8_t y, const char* str);
extern void lcd_implementation_write(char c);
extern void lcd_implementation_print(int8_t i);
extern void lcd_implementation_print_at(uint8_t x, uint8_t y, int8_t i);
extern void lcd_implementation_print(int i);
extern void lcd_implementation_print_at(uint8_t x, uint8_t y, int i);
extern void lcd_implementation_print(float f);
extern void lcd_implementation_print(const char *str);
extern void lcd_implementation_print_at(uint8_t x, uint8_t y, const char *str);

extern void lcd_implementation_drawmenu_generic(uint8_t row, const char* pstr, char pre_char, char post_char);
extern void lcd_implementation_drawmenu_generic_RAM(uint8_t row, const char* str, char pre_char, char post_char);
extern void lcd_implementation_drawmenu_setting_edit_generic(uint8_t row, const char* pstr, char pre_char, char* data);
extern void lcd_implementation_drawmenu_setting_edit_generic_P(uint8_t row, const char* pstr, char pre_char, const char* data);


extern char *wfac_to_str5(const uint8_t &x);
extern char *mres_to_str3(const uint8_t &x);

#define lcd_implementation_drawmenu_setting_edit_wfac_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', wfac_to_str5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_wfac(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', wfac_to_str5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_mres_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', mres_to_str3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_mres(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', mres_to_str3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_byte3_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', itostr3((uint8_t)*(data)))
#define lcd_implementation_drawmenu_setting_edit_byte3(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', itostr3((uint8_t)*(data)))
#define lcd_implementation_drawmenu_setting_edit_int3_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_int3(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float3_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float3(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float32_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float32(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float43_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr43(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float43(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr43(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float5_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float5(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float52_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float52(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float51_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_float51(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_long5_selected(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_long5(row, pstr, pstr2, data, minValue, maxValue) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_bool_selected(row, pstr, pstr2, data) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, '>', (*(data))?PSTR(MSG_ON):PSTR(MSG_OFF))
#define lcd_implementation_drawmenu_setting_edit_bool(row, pstr, pstr2, data) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, ' ', (*(data))?PSTR(MSG_ON):PSTR(MSG_OFF))

//Add version for callback functions
#define lcd_implementation_drawmenu_setting_edit_callback_int3_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_int3(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', itostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float3_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float3(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr3(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float32_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float32(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr32(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float43_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr43(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float43(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr43(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float5_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float5(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float52_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float52(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr52(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float51_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_float51(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr51(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_long5_selected(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, '>', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_long5(row, pstr, pstr2, data, minValue, maxValue, callback) lcd_implementation_drawmenu_setting_edit_generic(row, pstr, ' ', ftostr5(*(data)))
#define lcd_implementation_drawmenu_setting_edit_callback_bool_selected(row, pstr, pstr2, data, callback) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, '>', (*(data))?PSTR(MSG_ON):PSTR(MSG_OFF))
#define lcd_implementation_drawmenu_setting_edit_callback_bool(row, pstr, pstr2, data, callback) lcd_implementation_drawmenu_setting_edit_generic_P(row, pstr, ' ', (*(data))?PSTR(MSG_ON):PSTR(MSG_OFF))


extern void lcd_implementation_drawedit(const char* pstr, char* value);
extern void lcd_implementation_drawedit_2(const char* pstr, char* value);



#endif //_LCD_H
