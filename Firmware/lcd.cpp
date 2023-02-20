//menu.cpp

#include "lcd.h"
#include <stdio.h>
#include <stdarg.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "Timer.h"

#include "Configuration.h"
#include "pins.h"
#include <binary.h>
#include <Arduino.h>
#include "Marlin.h"
#include "fastio.h"
//-//
#include "sound.h"
#include "backlight.h"

#define LCD_DEFAULT_DELAY 100

#if (defined(LCD_PINS_D0) && defined(LCD_PINS_D1) && defined(LCD_PINS_D2) && defined(LCD_PINS_D3))
	#define LCD_8BIT
#endif

// #define VT100

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// bitmasks for flag argument settings
#define LCD_RS_FLAG 0x01
#define LCD_HALF_FLAG 0x02

constexpr uint8_t row_offsets[] PROGMEM = { 0x00, 0x40, 0x14, 0x54 };

FILE _lcdout; // = {0}; Global variable is always zero initialized, no need to explicitly state that.

uint8_t lcd_displayfunction = 0;
uint8_t lcd_displaycontrol = 0;
uint8_t lcd_displaymode = 0;

uint8_t lcd_currline;

#ifdef VT100
uint8_t lcd_escape[8];
#endif

static void lcd_display(void);

#if 0
static void lcd_no_display(void);
static void lcd_no_cursor(void);
static void lcd_cursor(void);
static void lcd_no_blink(void);
static void lcd_blink(void);
static void lcd_scrollDisplayLeft(void);
static void lcd_scrollDisplayRight(void);
static void lcd_leftToRight(void);
static void lcd_rightToLeft(void);
static void lcd_autoscroll(void);
static void lcd_no_autoscroll(void);
#endif

#ifdef VT100
void lcd_escape_write(uint8_t chr);
#endif

static void lcd_pulseEnable(void)
{  
	WRITE(LCD_PINS_ENABLE,HIGH);
	_delay_us(1);    // enable pulse must be >450ns
	WRITE(LCD_PINS_ENABLE,LOW);
}

static void lcd_writebits(uint8_t value)
{
#ifdef LCD_8BIT
	WRITE(LCD_PINS_D0, value & 0x01);
	WRITE(LCD_PINS_D1, value & 0x02);
	WRITE(LCD_PINS_D2, value & 0x04);
	WRITE(LCD_PINS_D3, value & 0x08);
#endif
	WRITE(LCD_PINS_D4, value & 0x10);
	WRITE(LCD_PINS_D5, value & 0x20);
	WRITE(LCD_PINS_D6, value & 0x40);
	WRITE(LCD_PINS_D7, value & 0x80);
	
	lcd_pulseEnable();
}

static void lcd_send(uint8_t data, uint8_t flags, uint16_t duration = LCD_DEFAULT_DELAY)
{
	WRITE(LCD_PINS_RS,flags&LCD_RS_FLAG);
	_delay_us(5);
	lcd_writebits(data);
#ifndef LCD_8BIT
	if (!(flags & LCD_HALF_FLAG))
	{
		_delay_us(LCD_DEFAULT_DELAY);
		lcd_writebits(data<<4);
	}
#endif
	delayMicroseconds(duration);
}

static void lcd_command(uint8_t value, uint16_t delayExtra = 0)
{
	lcd_send(value, LOW, LCD_DEFAULT_DELAY + delayExtra);
}

static void lcd_write(uint8_t value)
{
	if (value == '\n')
	{
		if (lcd_currline > 3) lcd_currline = -1;
		lcd_set_cursor(0, lcd_currline + 1); // LF
		return;
	}
	#ifdef VT100
	if (lcd_escape[0] || (value == 0x1b)){
		lcd_escape_write(value);
		return;
	}
	#endif
	lcd_send(value, HIGH);
}

static void lcd_begin(uint8_t clear)
{
	lcd_currline = 0;

	lcd_send(LCD_FUNCTIONSET | LCD_8BITMODE, LOW | LCD_HALF_FLAG, 4500); // wait min 4.1ms
	// second try
	lcd_send(LCD_FUNCTIONSET | LCD_8BITMODE, LOW | LCD_HALF_FLAG, 150);
	// third go!
	lcd_send(LCD_FUNCTIONSET | LCD_8BITMODE, LOW | LCD_HALF_FLAG, 150);
#ifndef LCD_8BIT
	// set to 4-bit interface
	lcd_send(LCD_FUNCTIONSET | LCD_4BITMODE, LOW | LCD_HALF_FLAG, 150);
#endif

	// finally, set # lines, font size, etc.0
	lcd_command(LCD_FUNCTIONSET | lcd_displayfunction);
	// turn the display on with no cursor or blinking default
	lcd_displaycontrol = LCD_CURSOROFF | LCD_BLINKOFF;  
	lcd_display();
	// clear it off
	if (clear) lcd_clear();
	// Initialize to default text direction (for romance languages)
	lcd_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	lcd_command(LCD_ENTRYMODESET | lcd_displaymode);
	
	#ifdef VT100
	lcd_escape[0] = 0;
	#endif
}

static int lcd_putchar(char c, FILE *)
{
	lcd_write(c);
	return 0;
}

void lcd_init(void)
{
	WRITE(LCD_PINS_ENABLE,LOW);
	SET_OUTPUT(LCD_PINS_RS);
	SET_OUTPUT(LCD_PINS_ENABLE);

#ifdef LCD_8BIT
	SET_OUTPUT(LCD_PINS_D0);
	SET_OUTPUT(LCD_PINS_D1);
	SET_OUTPUT(LCD_PINS_D2);
	SET_OUTPUT(LCD_PINS_D3);
#endif
	SET_OUTPUT(LCD_PINS_D4);
	SET_OUTPUT(LCD_PINS_D5);
	SET_OUTPUT(LCD_PINS_D6);
	SET_OUTPUT(LCD_PINS_D7);
	
#ifdef LCD_8BIT
	lcd_displayfunction |= LCD_8BITMODE;
#endif
	lcd_displayfunction |= LCD_2LINE;
	_delay_us(50000); 
	lcd_begin(1); //first time init
	fdev_setup_stream(lcdout, lcd_putchar, NULL, _FDEV_SETUP_WRITE); //setup lcdout stream
}

void lcd_refresh(void)
{
    lcd_begin(1);
    lcd_set_custom_characters();
}

void lcd_refresh_noclear(void)
{
    lcd_begin(0);
    lcd_set_custom_characters();
}

void lcd_clear(void)
{
	lcd_command(LCD_CLEARDISPLAY, 1600);  // clear display, set cursor position to zero
	lcd_currline = 0;
}

void lcd_home(void)
{
	lcd_command(LCD_RETURNHOME, 1600);  // set cursor position to zero
	lcd_currline = 0;
}

// Turn the display on/off (quickly)
void lcd_display(void)
{
    lcd_displaycontrol |= LCD_DISPLAYON;
    lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

#if 0
void lcd_no_display(void)
{
	lcd_displaycontrol &= ~LCD_DISPLAYON;
	lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}
#endif

#ifdef VT100 //required functions for VT100
// Turns the underline cursor on/off
void lcd_no_cursor(void)
{
	lcd_displaycontrol &= ~LCD_CURSORON;
	lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

void lcd_cursor(void)
{
	lcd_displaycontrol |= LCD_CURSORON;
	lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}
#endif

#if 0
// Turn on and off the blinking cursor
void lcd_no_blink(void)
{
	lcd_displaycontrol &= ~LCD_BLINKON;
	lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

void lcd_blink(void)
{
	lcd_displaycontrol |= LCD_BLINKON;
	lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

// These commands scroll the display without changing the RAM
void lcd_scrollDisplayLeft(void)
{
	lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void lcd_scrollDisplayRight(void)
{
	lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcd_leftToRight(void)
{
	lcd_displaymode |= LCD_ENTRYLEFT;
	lcd_command(LCD_ENTRYMODESET | lcd_displaymode);
}

// This is for text that flows Right to Left
void lcd_rightToLeft(void)
{
	lcd_displaymode &= ~LCD_ENTRYLEFT;
	lcd_command(LCD_ENTRYMODESET | lcd_displaymode);
}

// This will 'right justify' text from the cursor
void lcd_autoscroll(void)
{
	lcd_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	lcd_command(LCD_ENTRYMODESET | lcd_displaymode);
}

// This will 'left justify' text from the cursor
void lcd_no_autoscroll(void)
{
	lcd_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	lcd_command(LCD_ENTRYMODESET | lcd_displaymode);
}
#endif

/// @brief set the current LCD row
/// @param row LCD row number, ranges from 0 to LCD_HEIGHT - 1
static void FORCE_INLINE lcd_set_current_row(uint8_t row)
{
	lcd_currline = min(row, LCD_HEIGHT - 1);
}

/// @brief Calculate the LCD row offset
/// @param row LCD row number, ranges from 0 to LCD_HEIGHT - 1
/// @return row offset which the LCD register understands
static uint8_t __attribute__((noinline)) lcd_get_row_offset(uint8_t row)
{
	return pgm_read_byte(row_offsets + min(row, LCD_HEIGHT - 1));
}

void lcd_set_cursor(uint8_t col, uint8_t row)
{
	lcd_set_current_row(row);
	lcd_command(LCD_SETDDRAMADDR | (col + lcd_get_row_offset(lcd_currline)));
}

void lcd_set_cursor_column(uint8_t col)
{
	lcd_command(LCD_SETDDRAMADDR | (col + lcd_get_row_offset(lcd_currline)));
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void lcd_createChar_P(uint8_t location, const uint8_t* charmap)
{
  location &= 0x7; // we only have 8 locations 0-7
  lcd_command(LCD_SETCGRAMADDR | (location << 3));
  for (uint8_t i = 0; i < 8; i++)
    lcd_send(pgm_read_byte(&charmap[i]), HIGH);
}

#ifdef VT100

//Supported VT100 escape codes:
//EraseScreen  "\x1b[2J"
//CursorHome   "\x1b[%d;%dH"
//CursorShow   "\x1b[?25h"
//CursorHide   "\x1b[?25l"
void lcd_escape_write(uint8_t chr)
{
#define escape_cnt (lcd_escape[0])        //escape character counter
#define is_num_msk (lcd_escape[1])        //numeric character bit mask
#define chr_is_num (is_num_msk & 0x01) //current character is numeric
#define e_2_is_num (is_num_msk & 0x04) //escape char 2 is numeric
#define e_3_is_num (is_num_msk & 0x08) //...
#define e_4_is_num (is_num_msk & 0x10)
#define e_5_is_num (is_num_msk & 0x20)
#define e_6_is_num (is_num_msk & 0x40)
#define e_7_is_num (is_num_msk & 0x80)
#define e2_num (lcd_escape[2] - '0')      //number from character 2
#define e3_num (lcd_escape[3] - '0')      //number from character 3
#define e23_num (10*e2_num+e3_num)     //number from characters 2 and 3
#define e4_num (lcd_escape[4] - '0')      //number from character 4
#define e5_num (lcd_escape[5] - '0')      //number from character 5
#define e45_num (10*e4_num+e5_num)     //number from characters 4 and 5
#define e6_num (lcd_escape[6] - '0')      //number from character 6
#define e56_num (10*e5_num+e6_num)     //number from characters 5 and 6
	if (escape_cnt > 1) // escape length > 1 = "\x1b["
	{
		lcd_escape[escape_cnt] = chr; // store current char
		if ((chr >= '0') && (chr <= '9')) // char is numeric
			is_num_msk |= (1 | (1 << escape_cnt)); //set mask
		else
			is_num_msk &= ~1; //clear mask
	}
	switch (escape_cnt++)
	{
	case 0:
		if (chr == 0x1b) return;  // escape = "\x1b"
		break;
	case 1:
		is_num_msk = 0x00; // reset 'is number' bit mask
		if (chr == '[') return; // escape = "\x1b["
		break;
	case 2:
		switch (chr)
		{
		case '2': return; // escape = "\x1b[2"
		case '?': return; // escape = "\x1b[?"
		default:
			if (chr_is_num) return; // escape = "\x1b[%1d"
		}
		break;
	case 3:
		switch (lcd_escape[2])
		{
		case '?': // escape = "\x1b[?"
			if (chr == '2') return; // escape = "\x1b[?2"
			break;
		case '2':
			if (chr == 'J') // escape = "\x1b[2J"
				{ lcd_clear(); lcd_currline = 0; break; } // EraseScreen
		default:
			if (e_2_is_num && // escape = "\x1b[%1d"
				((chr == ';') || // escape = "\x1b[%1d;"
				chr_is_num)) // escape = "\x1b[%2d"
				return;
		}
		break;
	case 4:
		switch (lcd_escape[2])
		{
		case '?': // "\x1b[?"
			if ((lcd_escape[3] == '2') && (chr == '5')) return; // escape = "\x1b[?25"
			break;
		default:
			if (e_2_is_num) // escape = "\x1b[%1d"
			{
				if ((lcd_escape[3] == ';') && chr_is_num) return; // escape = "\x1b[%1d;%1d"
				else if (e_3_is_num && (chr == ';')) return; // escape = "\x1b[%2d;"
			}
		}
		break;
	case 5:
		switch (lcd_escape[2])
		{
		case '?':
			if ((lcd_escape[3] == '2') && (lcd_escape[4] == '5')) // escape = "\x1b[?25"
				switch (chr)
				{
				case 'h': // escape = "\x1b[?25h"
  					lcd_cursor(); // CursorShow
					break;
				case 'l': // escape = "\x1b[?25l"
					lcd_no_cursor(); // CursorHide
					break;
				}
			break;
		default:
			if (e_2_is_num) // escape = "\x1b[%1d"
			{
				if ((lcd_escape[3] == ';') && e_4_is_num) // escape = "\x1b%1d;%1dH"
				{
					if (chr == 'H') // escape = "\x1b%1d;%1dH"
						lcd_set_cursor(e4_num, e2_num); // CursorHome
					else if (chr_is_num)
						return; // escape = "\x1b%1d;%2d"
				}
				else if (e_3_is_num && (lcd_escape[4] == ';') && chr_is_num)
					return; // escape = "\x1b%2d;%1d"
			}
		}
		break;
	case 6:
		if (e_2_is_num) // escape = "\x1b[%1d"
		{
			if ((lcd_escape[3] == ';') && e_4_is_num && e_5_is_num && (chr == 'H')) // escape = "\x1b%1d;%2dH"
				lcd_set_cursor(e45_num, e2_num); // CursorHome
			else if (e_3_is_num && (lcd_escape[4] == ';') && e_5_is_num) // escape = "\x1b%2d;%1d"
			{
				if (chr == 'H') // escape = "\x1b%2d;%1dH"
					lcd_set_cursor(e5_num, e23_num); // CursorHome
				else if (chr_is_num) // "\x1b%2d;%2d"
					return;
			}
		}
		break;
	case 7:
		if (e_2_is_num && e_3_is_num && (lcd_escape[4] == ';')) // "\x1b[%2d;"
			if (e_5_is_num && e_6_is_num && (chr == 'H')) // "\x1b[%2d;%2dH"
				lcd_set_cursor(e56_num, e23_num); // CursorHome
		break;
	}
	escape_cnt = 0; // reset escape
}

#endif //VT100


int lcd_putc(char c)
{
	return fputc(c, lcdout);
}

int lcd_putc_at(uint8_t c, uint8_t r, char ch)
{
	lcd_set_cursor(c, r);
	return fputc(ch, lcdout);
}

int lcd_puts_P(const char* str)
{
	return fputs_P(str, lcdout);
}

int lcd_puts_at_P(uint8_t c, uint8_t r, const char* str)
{
	lcd_set_cursor(c, r);
	return fputs_P(str, lcdout);
}

int lcd_printf_P(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int ret = vfprintf_P(lcdout, format, args);
	va_end(args);
	return ret;
}

void lcd_space(uint8_t n)
{
	while (n--) lcd_putc(' ');
}


void lcd_print(const char* s)
{
	while (*s) lcd_write(*(s++));
}

char lcd_print_pad(const char* s, uint8_t len)
{
    while (len && *s) {
        lcd_write(*(s++));
        --len;
    }
    lcd_space(len);
    return *s;
}

uint8_t lcd_print_pad_P(const char* s, uint8_t len)
{
    while (len && pgm_read_byte(s)) {
        lcd_write(pgm_read_byte(s++));
        --len;
    }
    lcd_space(len);
    return len;
}

void lcd_print(char c, int base)
{
	lcd_print((long) c, base);
}

void lcd_print(unsigned char b, int base)
{
	lcd_print((unsigned long) b, base);
}

void lcd_print(int n, int base)
{
	lcd_print((long) n, base);
}

void lcd_print(unsigned int n, int base)
{
	lcd_print((unsigned long) n, base);
}

void lcd_print(long n, int base)
{
	if (base == 0)
		lcd_write(n);
	else if (base == 10)
	{
		if (n < 0)
		{
			lcd_print('-');
			n = -n;
		}
		lcd_printNumber(n, 10);
	}
	else
		lcd_printNumber(n, base);
}

void lcd_print(unsigned long n, int base)
{
	if (base == 0)
		lcd_write(n);
	else
		lcd_printNumber(n, base);
}

void lcd_printNumber(unsigned long n, uint8_t base)
{
	unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
	uint8_t i = 0;
	if (n == 0)
	{
		lcd_print('0');
		return;
	} 
	while (n > 0)
	{
		buf[i++] = n % base;
		n /= base;
	}
	for (; i > 0; i--)
		lcd_print((char) (buf[i - 1] < 10 ?	'0' + buf[i - 1] : 'A' + buf[i - 1] - 10));
}

uint8_t lcd_draw_update = 2;
int32_t lcd_encoder = 0;
uint8_t lcd_encoder_bits = 0;
int8_t lcd_encoder_diff = 0;

uint8_t lcd_buttons = 0;
uint8_t lcd_button_pressed = 0;
uint8_t lcd_update_enabled = 1;
static bool lcd_backlight_wake_trigger; // Flag set by interrupt when the knob is pressed or rotated

uint32_t lcd_next_update_millis = 0;



lcd_longpress_func_t lcd_longpress_func = 0;

lcd_lcdupdate_func_t lcd_lcdupdate_func = 0;

static ShortTimer buttonBlanking;
ShortTimer longPressTimer;
LongTimer lcd_timeoutToStatus;


//! @brief Was button clicked?
//!
//! Consume click event, following call would return 0.
//! See #LCD_CLICKED macro for version not consuming the event.
//!
//! Generally is used in modal dialogs.
//!
//! @retval 0 not clicked
//! @retval nonzero clicked
uint8_t lcd_clicked(void)
{
	bool clicked = LCD_CLICKED;
	if(clicked)
	{
	    lcd_consume_click();
	}
    return clicked;
}

void lcd_beeper_quick_feedback(void)
{
//-//
Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
/*
	for(int8_t i = 0; i < 10; i++)
	{
		Sound_MakeCustom(100,0,false);
		_delay_us(100);
	}
*/
}

void lcd_quick_feedback(void)
{
  lcd_draw_update = 2;
  lcd_button_pressed = false;
  lcd_beeper_quick_feedback();
}

void lcd_update(uint8_t lcdDrawUpdateOverride)
{
	if (lcd_draw_update < lcdDrawUpdateOverride)
		lcd_draw_update = lcdDrawUpdateOverride;

	if (lcd_backlight_wake_trigger) {
		lcd_backlight_wake_trigger = false;
		backlight_wake();
	}

	backlight_update();

	if (!lcd_update_enabled) return;

	if (lcd_lcdupdate_func)
		lcd_lcdupdate_func();
}

void lcd_update_enable(uint8_t enabled)
{
	// printf_P(PSTR("lcd_update_enable(%u -> %u)\n"), lcd_update_enabled, enabled);
	if (lcd_update_enabled != enabled)
	{
		lcd_update_enabled = enabled;
		if (enabled)
		{ // Reset encoder position. This is equivalent to re-entering a menu.
			lcd_encoder = 0;
			lcd_encoder_diff = 0;
			// Enabling the normal LCD update procedure.
			// Reset the timeout interval.
			lcd_timeoutToStatus.start();
			// Force the keypad update now.
			lcd_next_update_millis = _millis() - 1;
			// Full update.
			lcd_clear();
			lcd_update(2);
		} else
		{
			// Clear the LCD always, or let it to the caller?
		}
	}
}

bool lcd_longpress_trigger = 0;

// WARNING: this function is called from the temperature ISR.
//          Only update flags, but do not perform any menu/lcd operation!
void lcd_buttons_update(void)
{
    static uint8_t lcd_long_press_active = 0;
	uint8_t newbutton = 0;
	if (READ(BTN_EN1) == 0)  newbutton |= EN_A;
	if (READ(BTN_EN2) == 0)  newbutton |= EN_B;

    if (READ(BTN_ENC) == 0)
    { //button is pressed
        if (!buttonBlanking.running() || buttonBlanking.expired(BUTTON_BLANKING_TIME)) {
            buttonBlanking.start();
            safetyTimer.start();
            lcd_backlight_wake_trigger = true; // flag event, knob pressed
            if ((lcd_button_pressed == 0) && (lcd_long_press_active == 0))
            {
                longPressTimer.start();
                lcd_button_pressed = 1;
            }
            else if (longPressTimer.expired(LONG_PRESS_TIME))
            {
                lcd_long_press_active = 1;
                lcd_longpress_trigger = 1;
            }
        }
    }
    else
    { //button not pressed
        if (lcd_button_pressed)
        { //button was released
            buttonBlanking.start();
            if (lcd_long_press_active == 0)
            { //button released before long press gets activated
                newbutton |= EN_C;
            }
            //else if (menu_menu == lcd_move_z) lcd_quick_feedback();
            //lcd_button_pressed is set back to false via lcd_quick_feedback function
        }
        lcd_long_press_active = 0;
    }

	lcd_buttons = newbutton;
	//manage encoder rotation
	uint8_t enc = 0;
	if (lcd_buttons & EN_A) enc |= B01;
	if (lcd_buttons & EN_B) enc |= B10;
	if (enc != lcd_encoder_bits)
	{
		switch (enc)
		{
		case encrot0:
			if (lcd_encoder_bits == encrot3)
				lcd_encoder_diff++;
			else if (lcd_encoder_bits == encrot1)
				lcd_encoder_diff--;
			break;
		case encrot1:
			if (lcd_encoder_bits == encrot0)
				lcd_encoder_diff++;
			else if (lcd_encoder_bits == encrot2)
				lcd_encoder_diff--;
			break;
		case encrot2:
			if (lcd_encoder_bits == encrot1)
				lcd_encoder_diff++;
			else if (lcd_encoder_bits == encrot3)
				lcd_encoder_diff--;
			break;
		case encrot3:
			if (lcd_encoder_bits == encrot2)
				lcd_encoder_diff++;
			else if (lcd_encoder_bits == encrot0)
				lcd_encoder_diff--;
			break;
		}

		if (abs(lcd_encoder_diff) >= ENCODER_PULSES_PER_STEP) {
			lcd_backlight_wake_trigger = true; // flag event, knob rotated
		}
	}
	lcd_encoder_bits = enc;
}


////////////////////////////////////////////////////////////////////////////////
// Custom character data

const uint8_t lcd_chardata_bedTemp[8] PROGMEM = {
	B00000,
	B11111,
	B10101,
	B10001,
	B10101,
	B11111,
	B00000,
	B00000}; //thanks Sonny Mounicou

const uint8_t lcd_chardata_degree[8] PROGMEM = {
	B01100,
	B10010,
	B10010,
	B01100,
	B00000,
	B00000,
	B00000,
	B00000};

const uint8_t lcd_chardata_thermometer[8] PROGMEM = {
	B00100,
	B01010,
	B01010,
	B01010,
	B01010,
	B10001,
	B10001,
	B01110};

const uint8_t lcd_chardata_uplevel[8] PROGMEM = {
	B00100,
	B01110,
	B11111,
	B00100,
	B11100,
	B00000,
	B00000,
	B00000}; //thanks joris

const uint8_t lcd_chardata_refresh[8] PROGMEM = {
	B00000,
	B00110,
	B11001,
	B11000,
	B00011,
	B10011,
	B01100,
	B00000}; //thanks joris

const uint8_t lcd_chardata_folder[8] PROGMEM = {
	B00000,
	B11100,
	B11111,
	B10001,
	B10001,
	B11111,
	B00000,
	B00000}; //thanks joris

/*const uint8_t lcd_chardata_feedrate[8] PROGMEM = {
	B11100,
	B10000,
	B11000,
	B10111,
	B00101,
	B00110,
	B00101,
	B00000};*/ //thanks Sonny Mounicou

/*const uint8_t lcd_chardata_feedrate[8] PROGMEM = {
	B11100,
	B10100,
	B11000,
	B10100,
	B00000,
	B00111,
	B00010,
	B00010};*/

/*const uint8_t lcd_chardata_feedrate[8] PROGMEM = {
	B01100,
	B10011,
	B00000,
	B01100,
	B10011,
	B00000,
	B01100,
	B10011};*/

const uint8_t lcd_chardata_feedrate[8] PROGMEM = {
	B00000,
	B00100,
	B10010,
	B01001,
	B10010,
	B00100,
	B00000,
	B00000};

const uint8_t lcd_chardata_clock[8] PROGMEM = {
	B00000,
	B01110,
	B10011,
	B10101,
	B10001,
	B01110,
	B00000,
	B00000}; //thanks Sonny Mounicou

void lcd_set_custom_characters(void)
{
	lcd_createChar_P(LCD_STR_BEDTEMP[0], lcd_chardata_bedTemp);
	lcd_createChar_P(LCD_STR_DEGREE[0], lcd_chardata_degree);
	lcd_createChar_P(LCD_STR_THERMOMETER[0], lcd_chardata_thermometer);
	lcd_createChar_P(LCD_STR_UPLEVEL[0], lcd_chardata_uplevel);
	lcd_createChar_P(LCD_STR_REFRESH[0], lcd_chardata_refresh);
	lcd_createChar_P(LCD_STR_FOLDER[0], lcd_chardata_folder);
	lcd_createChar_P(LCD_STR_FEEDRATE[0], lcd_chardata_feedrate);
	lcd_createChar_P(LCD_STR_CLOCK[0], lcd_chardata_clock);
}

const uint8_t lcd_chardata_arr2down[8] PROGMEM = {
	B00000,
	B00000,
	B10001,
	B01010,
	B00100,
	B10001,
	B01010,
	B00100};

const uint8_t lcd_chardata_confirm[8] PROGMEM = {
	B00000,
	B00001,
	B00011,
	B10110,
	B11100,
	B01000,
	B00000};

void lcd_set_custom_characters_nextpage(void)
{
	lcd_createChar_P(LCD_STR_ARROW_2_DOWN[0], lcd_chardata_arr2down);
	lcd_createChar_P(LCD_STR_CONFIRM[0], lcd_chardata_confirm);
}

