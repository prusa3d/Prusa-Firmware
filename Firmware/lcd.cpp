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

#define LCD_BLOCK_BUFFER_SIZE 8
#define LCD_COMMAND_DELAY 100

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

// bitmasks for flag argument settings for lcd_planner
#define LCD_RS_FLAG 0x01
#define LCD_HALF_FLAG 0x02
#define LCD_WAIT_FLAG 0x04
#define LCD_WAIT_UPDATE_BACKUP_FLAG 0x08

FILE _lcdout; // = {0}; Global variable is always zero initialized, no need to explicitly state that.

uint8_t lcd_displayfunction;
uint8_t lcd_displaycontrol;
uint8_t lcd_displaymode;

uint8_t lcd_currline;

#ifdef VT100
uint8_t lcd_escape[8];
#endif

void lcd_writebits(uint8_t value);

typedef struct {
	uint8_t data;
	uint8_t flag;
	uint16_t command_delay_us;
} lcd_block_t;

#define ENABLE_LCD_TIMER() do {\
	TCNT3 = 0;\
	TCCR3B |= 0x02;\
} while(0)

#define DISABLE_LCD_TIMER() TCCR3B = (TCCR3B & ~(0x07<<CS30))

lcd_block_t lcd_block_buffer[LCD_BLOCK_BUFFER_SIZE];          // A ring buffer for lcd data blocks
volatile unsigned char lcd_block_buffer_head;                 // Index of the next block to be pushed
volatile unsigned char lcd_block_buffer_tail;                 // Index of the block to process now
lcd_block_t *lcd_current_block;  // A pointer to the block currently being traced

static int8_t lcd_next_block_index(int8_t block_index) {
	if (++ block_index == LCD_BLOCK_BUFFER_SIZE)
		block_index = 0;
	return block_index;
}

lcd_block_t *lcd_plan_get_current_block() 
{
	if (lcd_block_buffer_head == lcd_block_buffer_tail)
		return(NULL);
	lcd_block_t *block = &lcd_block_buffer[lcd_block_buffer_tail];
	return(block);
}

void lcd_plan_discard_current_block()
{
	if (lcd_block_buffer_head != lcd_block_buffer_tail)
		lcd_block_buffer_tail = (lcd_block_buffer_tail + 1) & (LCD_BLOCK_BUFFER_SIZE - 1); //tail++
}

uint16_t lcd_plan_calculate_timer_delay(uint32_t us) {
	return (us*(F_CPU/1000000/8)) - 1;
}

void lcd_plan_init() {
	lcd_block_buffer_head = 0;
	lcd_block_buffer_tail = 0;
	
	//block interrupts
	uint8_t _sreg = SREG;
	cli();
	
	// waveform generation = 0100 = CTC
	TCCR3B &= ~(1<<WGM33);
	TCCR3B |=  (1<<WGM32);
	TCCR3A &= ~(1<<WGM31);
	TCCR3A &= ~(1<<WGM30);
	
	// output mode = 00 (disconnected)
	TCCR3A &= ~(3<<COM3A0);
	TCCR3A &= ~(3<<COM3B0);
	TCCR3A &= ~(3<<COM3C0);
	
	DISABLE_LCD_TIMER();
	
	// enable interrupt
	TIMSK3 = 0x02;
	
	SREG = _sreg;
}

ISR(TIMER3_COMPA_vect)
{
	DISABLE_LCD_TIMER();
	lcd_current_block = lcd_plan_get_current_block();
	if(lcd_current_block == NULL)
		return;
	uint8_t flag = lcd_current_block->flag;
	if (flag & LCD_WAIT_FLAG) {
		if (!lcd_update_enabled) // in case the update was enabled while the lcd was initializing so not to disable it
			lcd_update_enabled = lcd_current_block->flag & LCD_WAIT_UPDATE_BACKUP_FLAG;
	}
	else
	{
		WRITE(LCD_PINS_RS, flag & LCD_RS_FLAG);
		_delay_us(5);
		lcd_writebits(lcd_current_block->data);
	}
#ifndef LCD_8BIT
	if (flag & LCD_HALF_FLAG){
		lcd_current_block->data = lcd_current_block->data<<4;
		flag &= ~LCD_HALF_FLAG;
		lcd_current_block->flag = flag;
		OCR3A = lcd_plan_calculate_timer_delay(LCD_COMMAND_DELAY);
	}
	else
#endif
	{
		OCR3A = lcd_plan_calculate_timer_delay(lcd_current_block->command_delay_us);
		lcd_current_block = NULL;
		lcd_plan_discard_current_block();
	}
	ENABLE_LCD_TIMER();
}

void _lcd_plan_data(uint8_t data, uint8_t flag, uint16_t command_delay_us) {
	int next_buffer_head = lcd_next_block_index(lcd_block_buffer_head);
	while (lcd_block_buffer_tail == next_buffer_head);
	lcd_block_t *block = &lcd_block_buffer[lcd_block_buffer_head];
	block->data = data;
	block->flag = flag;
	block->command_delay_us = command_delay_us;
	if (flag & LCD_WAIT_FLAG) {
		block->flag ^= (-lcd_update_enabled ^ block->flag) & LCD_WAIT_UPDATE_BACKUP_FLAG;
		lcd_update_enabled = 0;
	}
	lcd_block_buffer_head = next_buffer_head;
	if ((TCCR3B & 0x02) == 0) {
		OCR3A = lcd_plan_calculate_timer_delay(command_delay_us);
		ENABLE_LCD_TIMER();
	}
}

void lcd_plan_data
(
	uint8_t data
	,bool RS
	,uint16_t command_delay_us = LCD_COMMAND_DELAY
#ifndef LCD_8BIT
	,bool half = 1
#endif
	,uint8_t flag = 0
)
{
	flag ^= (-RS ^ flag) & LCD_RS_FLAG;
#ifndef LCD_8BIT
	flag ^= (-half ^ flag) & LCD_HALF_FLAG;
#endif

	_lcd_plan_data(data, flag, command_delay_us);
}

void lcd_pulseEnable(void)
{  
	WRITE(LCD_PINS_ENABLE,HIGH);
	_delay_us(1);    // enable pulse must be >450ns
	WRITE(LCD_PINS_ENABLE,LOW);
}

void lcd_writebits(uint8_t value)
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

void lcd_command(uint8_t value, uint16_t delayTime = 0)
{
	lcd_plan_data(value, LOW, LCD_COMMAND_DELAY + delayTime);
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

#ifdef VT100
void lcd_escape_write(uint8_t chr);
#endif

void lcd_write(uint8_t value)
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
	lcd_plan_data(value, HIGH);
}

static void lcd_begin(uint8_t clear)
{
	lcd_displayfunction |= LCD_2LINE;
	lcd_currline = 0;

	lcd_plan_data(LCD_FUNCTIONSET | LCD_8BITMODE, LOW, 4500, 0); // wait min 4.1ms
	// second try
	lcd_plan_data(LCD_FUNCTIONSET | LCD_8BITMODE, LOW, 150, 0);
	// third go!
	lcd_plan_data(LCD_FUNCTIONSET | LCD_8BITMODE, LOW, 150, 0);
	
#ifndef LCD_8BIT
	// set to 4-bit interface
	lcd_plan_data(LCD_FUNCTIONSET | LCD_4BITMODE, LOW, LCD_COMMAND_DELAY + 150, 0);
#endif

	// finally, set # lines, font size, etc.
	lcd_command(LCD_FUNCTIONSET | lcd_displayfunction, 60);
	// turn the display on with no cursor or blinking default
	lcd_displaycontrol = LCD_CURSOROFF | LCD_BLINKOFF;  
	lcd_display();
	// clear it off
	if (clear) lcd_clear();
	// Initialize to default text direction (for romance languages)
	lcd_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	lcd_command(LCD_ENTRYMODESET | lcd_displaymode, 60);
	
	#ifdef VT100
	lcd_escape[0] = 0;
	#endif
	
	lcd_plan_data(0x00, LOW, 0, 0, LCD_WAIT_FLAG); // special flag that disables lcd_update() to not block the loop with the status screen update during lcd_begin()
}

int lcd_putchar(char c, FILE *)
{
	lcd_write(c);
	return 0;
}

void lcd_init(void)
{
	SET_OUTPUT(LCD_PINS_RS);
	SET_OUTPUT(LCD_PINS_ENABLE);
	#ifndef LCD_8BIT
		lcd_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	#else
		lcd_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
	#endif
	lcd_plan_init();
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
	if (row >= LCD_HEIGHT)
		row = LCD_HEIGHT - 1;    // we count rows starting w/0
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
    lcd_plan_data(pgm_read_byte(&charmap[i]), HIGH);
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
//-//
Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
/*
	for(int8_t i = 0; i < 10; i++)
	{
		Sound_MakeCustom(100,0,false);
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
			lcd_next_update_millis = _millis() - 1;
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

