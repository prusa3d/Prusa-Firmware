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
//#include <Arduino.h>
#include "Marlin.h"
#include "fastio.h"
//-//
#include "sound.h"


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


FILE _lcdout; // = {0}; Global variable is always zero initialized, no need to explicitly state that.


uint8_t lcd_rs_pin; // LOW: command.  HIGH: character.
uint8_t lcd_rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
uint8_t lcd_enable_pin; // activated by a HIGH pulse.
uint8_t lcd_data_pins[8];

uint8_t lcd_displayfunction;
uint8_t lcd_displaycontrol;
uint8_t lcd_displaymode;

uint8_t lcd_numlines;
uint8_t lcd_currline;

uint8_t lcd_escape[8];


void lcd_pulseEnable(void)
{
	digitalWrite(lcd_enable_pin, LOW);
	delayMicroseconds(1);    
	digitalWrite(lcd_enable_pin, HIGH);
	delayMicroseconds(1);    // enable pulse must be >450ns
	digitalWrite(lcd_enable_pin, LOW);
	delayMicroseconds(100);   // commands need > 37us to settle
}

void lcd_write4bits(uint8_t value)
{
	for (int i = 0; i < 4; i++)
	{
		pinMode(lcd_data_pins[i], OUTPUT);
		digitalWrite(lcd_data_pins[i], (value >> i) & 0x01);
	}
	lcd_pulseEnable();
}

void lcd_write8bits(uint8_t value)
{
	for (int i = 0; i < 8; i++)
	{
		pinMode(lcd_data_pins[i], OUTPUT);
		digitalWrite(lcd_data_pins[i], (value >> i) & 0x01);
	}
	lcd_pulseEnable();
}

// write either command or data, with automatic 4/8-bit selection
void lcd_send(uint8_t value, uint8_t mode)
{
	digitalWrite(lcd_rs_pin, mode);
	// if there is a RW pin indicated, set it low to Write
	if (lcd_rw_pin != 255) digitalWrite(lcd_rw_pin, LOW);
	if (lcd_displayfunction & LCD_8BITMODE)
		lcd_write8bits(value); 
	else
	{
		lcd_write4bits(value>>4);
		lcd_write4bits(value);
	}
}

void lcd_command(uint8_t value)
{
	lcd_send(value, LOW);
}

void lcd_clear(void);
void lcd_home(void);
void lcd_no_display(void);
void lcd_display(void);
void lcd_no_cursor(void);
void lcd_cursor(void);
void lcd_no_blink(void);
void lcd_blink(void);
void lcd_scrollDisplayLeft(void);
void lcd_scrollDisplayRight(void);
void lcd_leftToRight(void);
void lcd_rightToLeft(void);
void lcd_autoscroll(void);
void lcd_no_autoscroll(void);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_createChar_P(uint8_t location, const uint8_t* charmap);

uint8_t lcd_escape_write(uint8_t chr);

uint8_t lcd_write(uint8_t value)
{
	if (value == '\n')
	{
		if (lcd_currline > 3) lcd_currline = -1;
		lcd_set_cursor(0, lcd_currline + 1); // LF
		return 1;
	}
	if (lcd_escape[0] || (value == 0x1b))
		return lcd_escape_write(value);
	lcd_send(value, HIGH);
	return 1; // assume sucess
}

static void lcd_begin(uint8_t lines, uint8_t dotsize, uint8_t clear)
{
	if (lines > 1) lcd_displayfunction |= LCD_2LINE;
	lcd_numlines = lines;
	lcd_currline = 0;
	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) lcd_displayfunction |= LCD_5x10DOTS;
	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	_delay_us(50000); 
	// Now we pull both RS and R/W low to begin commands
	digitalWrite(lcd_rs_pin, LOW);
	digitalWrite(lcd_enable_pin, LOW);
	if (lcd_rw_pin != 255)
		digitalWrite(lcd_rw_pin, LOW);
	//put the LCD into 4 bit or 8 bit mode
	if (!(lcd_displayfunction & LCD_8BITMODE))
	{
		// this is according to the hitachi HD44780 datasheet
		// figure 24, pg 46
		// we start in 8bit mode, try to set 4 bit mode
		lcd_write4bits(0x03);
		_delay_us(4500); // wait min 4.1ms
		// second try
		lcd_write4bits(0x03);
		_delay_us(4500); // wait min 4.1ms
		// third go!
		lcd_write4bits(0x03); 
		_delay_us(150);
		// finally, set to 4-bit interface
		lcd_write4bits(0x02); 
	}
	else
	{
		// this is according to the hitachi HD44780 datasheet
		// page 45 figure 23
		// Send function set command sequence
		lcd_command(LCD_FUNCTIONSET | lcd_displayfunction);
		_delay_us(4500);  // wait more than 4.1ms
		// second try
		lcd_command(LCD_FUNCTIONSET | lcd_displayfunction);
		_delay_us(150);
		// third go
		lcd_command(LCD_FUNCTIONSET | lcd_displayfunction);
	}
	// finally, set # lines, font size, etc.
	lcd_command(LCD_FUNCTIONSET | lcd_displayfunction);  
	_delay_us(60);
	// turn the display on with no cursor or blinking default
	lcd_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
	lcd_display();
	_delay_us(60);
	// clear it off
	if (clear) lcd_clear();
	_delay_us(3000);
	// Initialize to default text direction (for romance languages)
	lcd_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	lcd_command(LCD_ENTRYMODESET | lcd_displaymode);
	_delay_us(60);
	lcd_escape[0] = 0;
}

int lcd_putchar(char c, FILE *)
{
	lcd_write(c);
	return 0;
}

void lcd_init(void)
{
	uint8_t fourbitmode = 1;
	lcd_rs_pin = LCD_PINS_RS;
	lcd_rw_pin = 255;
	lcd_enable_pin = LCD_PINS_ENABLE;
	lcd_data_pins[0] = LCD_PINS_D4;
	lcd_data_pins[1] = LCD_PINS_D5;
	lcd_data_pins[2] = LCD_PINS_D6;
	lcd_data_pins[3] = LCD_PINS_D7; 
	lcd_data_pins[4] = 0;
	lcd_data_pins[5] = 0;
	lcd_data_pins[6] = 0;
	lcd_data_pins[7] = 0;
	pinMode(lcd_rs_pin, OUTPUT);
	// we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
	if (lcd_rw_pin != 255) pinMode(lcd_rw_pin, OUTPUT);
	pinMode(lcd_enable_pin, OUTPUT);
	if (fourbitmode) lcd_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	else lcd_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
	lcd_begin(LCD_HEIGHT, LCD_5x8DOTS, 1);
	//lcd_clear();
	fdev_setup_stream(lcdout, lcd_putchar, NULL, _FDEV_SETUP_WRITE); //setup lcdout stream
}

void lcd_refresh(void)
{
    lcd_begin(LCD_HEIGHT, LCD_5x8DOTS, 1);
    lcd_set_custom_characters();
}

void lcd_refresh_noclear(void)
{
    lcd_begin(LCD_HEIGHT, LCD_5x8DOTS, 0);
    lcd_set_custom_characters();
}



void lcd_clear(void)
{
	lcd_command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
	_delay_us(1600);  // this command takes a long time
}

void lcd_home(void)
{
	lcd_command(LCD_RETURNHOME);  // set cursor position to zero
	_delay_us(1600);  // this command takes a long time!
}

// Turn the display on/off (quickly)
void lcd_no_display(void)
{
	lcd_displaycontrol &= ~LCD_DISPLAYON;
	lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

void lcd_display(void)
{
	lcd_displaycontrol |= LCD_DISPLAYON;
	lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

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

void lcd_set_cursor(uint8_t col, uint8_t row)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row >= lcd_numlines )
		row = lcd_numlines-1;    // we count rows starting w/0
	lcd_currline = row;  
	lcd_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void lcd_createChar_P(uint8_t location, const uint8_t* charmap)
{
  location &= 0x7; // we only have 8 locations 0-7
  lcd_command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++)
    lcd_send(pgm_read_byte(&charmap[i]), HIGH);
}

//Supported VT100 escape codes:
//EraseScreen  "\x1b[2J"
//CursorHome   "\x1b[%d;%dH"
//CursorShow   "\x1b[?25h"
//CursorHide   "\x1b[?25l"
uint8_t lcd_escape_write(uint8_t chr)
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
		if (chr == 0x1b) return 1;  // escape = "\x1b"
		break;
	case 1:
		is_num_msk = 0x00; // reset 'is number' bit mask
		if (chr == '[') return 1; // escape = "\x1b["
		break;
	case 2:
		switch (chr)
		{
		case '2': return 1; // escape = "\x1b[2"
		case '?': return 1; // escape = "\x1b[?"
		default:
			if (chr_is_num) return 1; // escape = "\x1b[%1d"
		}
		break;
	case 3:
		switch (lcd_escape[2])
		{
		case '?': // escape = "\x1b[?"
			if (chr == '2') return 1; // escape = "\x1b[?2"
			break;
		case '2':
			if (chr == 'J') // escape = "\x1b[2J"
				{ lcd_clear(); lcd_currline = 0; break; } // EraseScreen
		default:
			if (e_2_is_num && // escape = "\x1b[%1d"
				((chr == ';') || // escape = "\x1b[%1d;"
				chr_is_num)) // escape = "\x1b[%2d"
				return 1;
		}
		break;
	case 4:
		switch (lcd_escape[2])
		{
		case '?': // "\x1b[?"
			if ((lcd_escape[3] == '2') && (chr == '5')) return 1; // escape = "\x1b[?25"
			break;
		default:
			if (e_2_is_num) // escape = "\x1b[%1d"
			{
				if ((lcd_escape[3] == ';') && chr_is_num) return 1; // escape = "\x1b[%1d;%1d"
				else if (e_3_is_num && (chr == ';')) return 1; // escape = "\x1b[%2d;"
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
						return 1; // escape = "\x1b%1d;%2d"
				}
				else if (e_3_is_num && (lcd_escape[4] == ';') && chr_is_num)
					return 1; // escape = "\x1b%2d;%1d"
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
					return 1;
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
	return 1; // assume sucess
}




int lcd_putc(int c)
{
	return fputc(c, lcdout);
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

void lcd_print(double n, int digits)
{
  lcd_printFloat(n, digits);
}


void lcd_printNumber(unsigned long n, uint8_t base)
{
	unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
	unsigned long i = 0;
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

void lcd_printFloat(double number, uint8_t digits) 
{ 
	// Handle negative numbers
	if (number < 0.0)
	{
		lcd_print('-');
		number = -number;
	}
	// Round correctly so that print(1.999, 2) prints as "2.00"
	double rounding = 0.5;
	for (uint8_t i=0; i<digits; ++i)
		rounding /= 10.0;
	number += rounding;
	// Extract the integer part of the number and print it
	unsigned long int_part = (unsigned long)number;
	double remainder = number - (double)int_part;
	lcd_print(int_part);
	// Print the decimal point, but only if there are digits beyond
	if (digits > 0)
		lcd_print('.'); 
	// Extract digits from the remainder one at a time
	while (digits-- > 0)
	{
		remainder *= 10.0;
		int toPrint = int(remainder);
		lcd_print(toPrint);
		remainder -= toPrint; 
	} 
}












uint8_t lcd_draw_update = 2;
int32_t lcd_encoder = 0;
uint8_t lcd_encoder_bits = 0;
int8_t lcd_encoder_diff = 0;

uint8_t lcd_buttons = 0;
uint8_t lcd_button_pressed = 0;
uint8_t lcd_update_enabled = 1;

uint32_t lcd_next_update_millis = 0;
uint8_t lcd_status_update_delay = 0;



lcd_longpress_func_t lcd_longpress_func = 0;

lcd_charsetup_func_t lcd_charsetup_func = 0;

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
	SET_OUTPUT(BEEPER);
//-//
Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
/*
	for(int8_t i = 0; i < 10; i++)
	{
		WRITE(BEEPER,HIGH);
		delayMicroseconds(100);
		WRITE(BEEPER,LOW);
		delayMicroseconds(100);
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
	if (!lcd_update_enabled)
		return;
	if (lcd_lcdupdate_func)
		lcd_lcdupdate_func();
}

void lcd_update_enable(uint8_t enabled)
{
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
			lcd_next_update_millis = millis() - 1;
			// Full update.
			lcd_clear();
			if (lcd_charsetup_func)
				lcd_charsetup_func();
			lcd_update(2);
		} else
		{
			// Clear the LCD always, or let it to the caller?
		}
	}
}

void lcd_buttons_update(void)
{
    static uint8_t lcd_long_press_active = 0;
	uint8_t newbutton = 0;
	if (READ(BTN_EN1) == 0)  newbutton |= EN_A;
	if (READ(BTN_EN2) == 0)  newbutton |= EN_B;

    if (READ(BTN_ENC) == 0)
    { //button is pressed
        lcd_timeoutToStatus.start();
        if (!buttonBlanking.running() || buttonBlanking.expired(BUTTON_BLANKING_TIME)) {
            buttonBlanking.start();
            safetyTimer.start();
            if ((lcd_button_pressed == 0) && (lcd_long_press_active == 0))
            {
                longPressTimer.start();
                lcd_button_pressed = 1;
            }
            else if (longPressTimer.expired(LONG_PRESS_TIME))
            {
                lcd_long_press_active = 1;
                //long press is not possible in modal mode
                if (lcd_longpress_func && lcd_update_enabled)
                    lcd_longpress_func();
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
        else
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

const uint8_t lcd_chardata_arrup[8] PROGMEM = {
	B00100,
	B01110,
	B11111,
	B00000,
	B00000,
	B00000,
	B00000,
	B00000};

const uint8_t lcd_chardata_arrdown[8] PROGMEM = {
	B00000,
	B00000,
	B00000,
	B00000,
	B00000,
	B10001,
	B01010,
	B00100};



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
	//lcd_createChar_P(LCD_STR_ARROW_UP[0], lcd_chardata_arrup);
	//lcd_createChar_P(LCD_STR_ARROW_DOWN[0], lcd_chardata_arrdown);
}

void lcd_set_custom_characters_arrows(void)
{
	lcd_createChar_P(1, lcd_chardata_arrdown);
}

const uint8_t lcd_chardata_progress[8] PROGMEM = {
	B11111,
	B11111,
	B11111,
	B11111,
	B11111,
	B11111,
	B11111,
	B11111};

void lcd_set_custom_characters_progress(void)
{
	lcd_createChar_P(1, lcd_chardata_progress);
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
	lcd_createChar_P(1, lcd_chardata_arr2down);
	lcd_createChar_P(2, lcd_chardata_confirm);
}

void lcd_set_custom_characters_degree(void)
{
	lcd_createChar_P(1, lcd_chardata_degree);
}

