//menu.cpp

#include "lcd.h"
#include <stdio.h>
#include <stdarg.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "Timer.h"

#include "Configuration.h"
#include "pins.h"
#include "system_timer.h"
#include "static_assert.h"
#include "fastio.h"
#include "macros.h"

#define LCD_DEFAULT_DELAY 100 //us
#define LCD_REDRAW_PERIOD (30 * 1000) //ms
#define LCD_ISR_DELAY_MULTIPLIER 1 //increase this to make the drawing slower

#if (defined(LCD_PINS_D0) && defined(LCD_PINS_D1) && defined(LCD_PINS_D2) && defined(LCD_PINS_D3))
	#define LCD_8BIT
#endif

static_assert(LCD_WIDTH == 20 && LCD_HEIGHT == 4, "Only 20x4 LCDs are supported");

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
#define LCD_ENTRYMODESET_DIRECTION 0x02 //else decrement cursor address.
#define LCD_ENTRYMODESET_SHIFT 0x01 //moves display. Cursor remains stationary. Direction dictated by increment flag.

// flags for display on/off control
#define LCD_DISPLAYCONTROL_DISPLAYON 0x04
#define LCD_DISPLAYCONTROL_CURSORON 0x02
#define LCD_DISPLAYCONTROL_BLINKON 0x01

// flags for display/cursor shift
#define LCD_CURSORSHIFT_DISPLAYMOVE 0x08
#define LCD_CURSORSHIFT_DIRECTION 0x04

// flags for function set
#define LCD_FUNCTIONSET_8BITMODE 0x10
#define LCD_FUNCTIONSET_2LINE 0x08
#define LCD_FUNCTIONSET_5x10DOTS 0x04

// bitmasks for flag argument settings
#define LCD_RS_FLAG 0x01
#define LCD_HALF_FLAG 0x02

FILE _lcdout; // = {0}; Global variable is always zero initialized, no need to explicitly state that.

//lcd
uint8_t lcd_displayfunction = 0;
uint8_t lcd_displaycontrol = 0;
uint8_t lcd_displaymode = 0;

uint8_t lcd_curpos;
//xbbbbaaa: cursor position from 0 to (LCD_WIDTH * LCD_HEIGHT)-1
//bbbb: index of the cluster in vga_map to search. From 0 to sizeof(vga_map)-1
//aaa: Rshift value for the bit to search. From 0 to 7
//x: unused. Leave it clear.

uint8_t vga_map[DIVIDE_ROUND_UP(LCD_WIDTH * LCD_HEIGHT, 8)]; //bitmap for changes on the display. Individual bits are set when lcd_write() is used
//    01234567890123456789
  
// 0  00000000111111112222
// 1  22223333333344444444
// 2  55555555666666667777
// 3  77778888888899999999

volatile uint8_t lcd_status = 0;
//xxxxdxba
//a: timer enabled status
//b: bit is set only if the timer is enabled and is used in the ISR to disable itself after required time has passed to send commands to it. ie: lcd_redraw(); 
//d: current command type. 1=lcd_set_cursor (jump); 0=character. Used by the ISR for the second nibble and also to decide whether a jump command is necessary (eg. next character is on the next line).
//x: unused. Do not change.

//vga
uint8_t vga_currline; //these two are used for puting data into the vga buffer.
uint8_t vga_currcol;

uint8_t vga[LCD_WIDTH][LCD_HEIGHT]; //vga buffer.

//dictates the bank to use for each custom character cell. Two banks are available.
uint8_t lcd_custom_character_bank = 0;

LongTimer lcd_redraw_timer; // Software timer used for redrawing the lcd at a set interval (LCD_REDRAW_PERIOD).

static void lcd_init_custom_characters(void);
static void lcd_set_custom_character(uint8_t charID);

static void lcd_display(void);
#if 0
static void lcd_no_display(void);
#endif

#ifdef VT100
uint8_t lcd_escape[8];
#endif

#define LCD_TIMER_REGNAME(registerbase,number,suffix) _REGNAME(registerbase,number,suffix)
#undef B0 //Necessary hack because of "binary.h" included in "Arduino.h" included in "system_timer.h" included in this file...

#define TCCRxA LCD_TIMER_REGNAME(TCCR, LCD_TIMER, A)
#define TCCRxB LCD_TIMER_REGNAME(TCCR, LCD_TIMER, B)
#define TCCRxC LCD_TIMER_REGNAME(TCCR, LCD_TIMER, C)
#define TCNTx LCD_TIMER_REGNAME(TCNT, LCD_TIMER,)
#define OCRxA LCD_TIMER_REGNAME(OCR, LCD_TIMER, A)
#define TIMSKx LCD_TIMER_REGNAME(TIMSK, LCD_TIMER,)
#define TIFRx LCD_TIMER_REGNAME(TIFR, LCD_TIMER,)
#define TIMERx_COMPA_vect LCD_TIMER_REGNAME(TIMER, LCD_TIMER, _COMPA_vect)
#define CSx0 LCD_TIMER_REGNAME(CS, LCD_TIMER, 0)
#define CSx1 LCD_TIMER_REGNAME(CS, LCD_TIMER, 1)
#define CSx2 LCD_TIMER_REGNAME(CS, LCD_TIMER, 2)
#define WGMx0 LCD_TIMER_REGNAME(WGM, LCD_TIMER, 0)
#define WGMx1 LCD_TIMER_REGNAME(WGM, LCD_TIMER, 1)
#define WGMx2 LCD_TIMER_REGNAME(WGM, LCD_TIMER, 2)
#define WGMx3 LCD_TIMER_REGNAME(WGM, LCD_TIMER, 3)
#define COMxA0 LCD_TIMER_REGNAME(COM, LCD_TIMER, A0)
#define COMxB0 LCD_TIMER_REGNAME(COM, LCD_TIMER, B0)
#define COMxC0 LCD_TIMER_REGNAME(COM, LCD_TIMER, C0)
#define OCIExA LCD_TIMER_REGNAME(OCIE, LCD_TIMER, A)
#define OCFxA LCD_TIMER_REGNAME(OCF, LCD_TIMER, A)

#define LCD_TIMER_IS_ENABLED() (TIMSKx & _BV(OCIExA))

#ifdef LCD_DEBUG
void lcd_debug(){
	puts_P(PSTR("VGA:"));
	for (int i = 0; i < LCD_HEIGHT; i++){
		for (int j = 0; j < LCD_WIDTH; j++){
			putchar(vga[j][i]);
		}
		putchar('\n');
	}
	
	for (uint8_t i = 0; i < sizeof(vga_map); i++)
	{
		printf_P(PSTR("%02hX "), vga_map[i]);
	}
	putchar('\n');
	
	printf_P(PSTR("lcd_curpos: %02hX\n"), lcd_curpos);
	printf_P(PSTR("lcd_status: %02hX\n"), lcd_status);
	printf_P(PSTR("TCCRxB: %02hX\n"), TCCRxB);
}
#endif //LCD_DEBUG

void lcd_timer_enable(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		lcd_status |= 0x01; //set timer enabled flag
		lcd_status &= ~0x02; //clear timer force disable flag. Shouldn't be needed, but just to be safe.
		TCNTx = 0; //clear timer value
		TIMSKx |= _BV(OCIExA); // enable interrupt
		TIFRx |= _BV(OCFxA); //clear interrupt flag by writing 1 in register.
	}
}

void lcd_timer_disable(void)
{
	if ((lcd_status & 0x01) && !(lcd_status & 0x02)) //do not wait recursively
	{
		lcd_status |= 0x02;
		while(lcd_status & 0x02) //wait for isr to finish what it has to do. The ISR might run a second time to make sure the nibble doesn't get out of sync.
		{
			// lcd_debug();
		}
		return; //the ISR disables itself. no need to also disable it here.
	}
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		lcd_status &= ~0x03; //clear both timer flags
		TIMSKx &= ~_BV(OCIExA); // disable interrupt
	}
}

class LcdTimerDisabler
{
public:
	LcdTimerDisabler(): m_updateEnabled(LCD_TIMER_IS_ENABLED())
	{
		lcd_timer_disable();
	}
	~LcdTimerDisabler()
	{
		if (m_updateEnabled)
			lcd_timer_enable();
	}
private:
	bool m_updateEnabled;
};

static void lcd_pulseEnable(void) //lcd
{
	WRITE(LCD_PINS_ENABLE, 1);
	_delay_us(1); // enable pulse must be >450ns
	WRITE(LCD_PINS_ENABLE, 0);
}

static void lcd_writebits(uint8_t value) //lcd
{
#ifdef LCD_8BIT
	WRITE(LCD_PINS_D0, value & _BV(0));
	WRITE(LCD_PINS_D1, value & _BV(1));
	WRITE(LCD_PINS_D2, value & _BV(2));
	WRITE(LCD_PINS_D3, value & _BV(3));
#endif
	WRITE(LCD_PINS_D4, value & _BV(4));
	WRITE(LCD_PINS_D5, value & _BV(5));
	WRITE(LCD_PINS_D6, value & _BV(6));
	WRITE(LCD_PINS_D7, value & _BV(7));
	
	lcd_pulseEnable();
}

static void lcd_send(uint8_t data, uint8_t flags) //lcd
{
	WRITE(LCD_PINS_RS,flags&LCD_RS_FLAG);
	lcd_writebits(data);
#ifndef LCD_8BIT
	if (!(flags & LCD_HALF_FLAG))
	{
		_delay_us(1);
		lcd_writebits(data<<4);
	}
#endif
#ifdef LCD_DEBUG
	printf_P(PSTR("lcd_send: %02hX, %c\n"), data, (flags&LCD_RS_FLAG)?1:0);
#endif //LCD_DEBUG
}

static void lcd_command(uint8_t value) //lcd
{
	lcd_send(value, 0);
	_delay_us(LCD_DEFAULT_DELAY);
}

static void vga_linefeed(void) //vga
{
	if (vga_currline > 3) vga_currline = -1;
	lcd_set_cursor(0, vga_currline + 1);
}

static void lcd_write(uint8_t value) //vga
{
	if (value == '\n')
	{
		vga_linefeed();
		return;
	}
	if (value <= 0x0F) //custom character
		if ((value / 8) != bool(lcd_custom_character_bank & (1 << (value & 0x07)))) //if value is different in the bank cache.
			lcd_set_custom_character(value);
	
	if (vga_currcol == LCD_WIDTH) vga_linefeed();
	#ifdef VT100
	if (lcd_escape[0] || (value == 0x1b)){
		lcd_escape_write(value);
		return;
	}
	#endif
	if (vga[vga_currcol][vga_currline] != value)
	{
		vga[vga_currcol][vga_currline] = value;
		vga_map[((vga_currline * LCD_WIDTH) + vga_currcol) / 8] |= (1 << (7 - (((vga_currline * LCD_WIDTH) + vga_currcol) % 8)));
		if (!(lcd_status & 0x01)) lcd_timer_enable();
	}
	vga_currcol++;
}

static void lcd_clear_hardware(void);

static void lcd_begin()
{
	LcdTimerDisabler _LcdTimerDisabler;
	lcd_send(LCD_FUNCTIONSET | LCD_FUNCTIONSET_8BITMODE, 0 | LCD_HALF_FLAG); // wait min 4.1ms
	_delay_us(4500);
	// second try
	lcd_send(LCD_FUNCTIONSET | LCD_FUNCTIONSET_8BITMODE, 0 | LCD_HALF_FLAG);
	_delay_us(150);
	// third go!
	lcd_send(LCD_FUNCTIONSET | LCD_FUNCTIONSET_8BITMODE, 0 | LCD_HALF_FLAG);
	_delay_us(150);
#ifndef LCD_8BIT
	// set to 4-bit interface
	lcd_send(LCD_FUNCTIONSET, 0 | LCD_HALF_FLAG);
	_delay_us(150);
#endif

	// finally, set # lines, font size, etc.0
	lcd_command(LCD_FUNCTIONSET | lcd_displayfunction);
	// turn the display on with no cursor or blinking default
	lcd_displaycontrol = 0;  
	lcd_display();
	// clear lcd and set all bits to be updated
	lcd_clear_hardware();
	// Initialize to default text direction (for romance languages)
	lcd_displaymode = LCD_ENTRYMODESET_DIRECTION;
	// set the entry mode
	lcd_command(LCD_ENTRYMODESET | lcd_displaymode);
	
	#ifdef VT100
	lcd_escape[0] = 0;
	#endif
}

static int vga_putchar(char c, FILE *) //vga
{
	lcd_write(c);
	return 0;
}

static void vga_init(void) //vga
{
	lcd_clear(); //fill buffer with ' ' and home
	
	//setup lcd_timer
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		TCCRxA = 0;
		TCCRxB = 0;
		
		// waveform generation = 0100 = CTC
		TCCRxB |= _BV(WGMx2);
		OCRxA = (LCD_DEFAULT_DELAY * LCD_ISR_DELAY_MULTIPLIER * (F_CPU/1000000/8)) - 1; //set timer TOP value with an 8x prescaler.
		
		lcd_status = 0;
		lcd_timer_disable();
		
		//start timer. Set clock source
		TCCRxB |= _BV(CSx1);
	}
	
	fdev_setup_stream(lcdout, vga_putchar, NULL, _FDEV_SETUP_WRITE); //setup lcdout stream
	lcd_redraw_timer.start();
}

void lcd_init(void) //lcd
{
	WRITE(LCD_PINS_ENABLE, 0);
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
	lcd_displayfunction |= LCD_FUNCTIONSET_8BITMODE;
#endif
	lcd_displayfunction |= LCD_FUNCTIONSET_2LINE;
	vga_init();
	_delay_us(50000);
	lcd_begin();
	lcd_timer_enable();
}

//uses lcd_redraw_timer and redraws the entire display every (LCD_REDRAW_PERIOD).
void lcd_redraw(bool forceRedraw)
{
	if (!lcd_redraw_timer.expired(LCD_REDRAW_PERIOD) && !forceRedraw) return;
	lcd_timer_disable();
    lcd_begin();
    lcd_init_custom_characters();
	lcd_timer_enable();
	lcd_redraw_timer.start();
}

void lcd_clear(void) //vga
{
	for (int i = 0; i < LCD_WIDTH; i++)
		for (int j = 0; j < LCD_HEIGHT; j++)
			vga[i][j] = ' ';
	for (uint8_t i = 0; i < sizeof(vga_map); i++) vga_map[i] = 0xff; //force entire screen update.
	lcd_home();
}

static void lcd_clear_hardware(void) //lcd
{
	lcd_command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
	_delay_us(1600); //extra delay according to datasheet
	lcd_curpos = 0;
	for (uint8_t i = 0; i < sizeof(vga_map); i++) vga_map[i] = 0xff; //force entire screen update.
	lcd_status &= ~0x08;
}

void lcd_home(void) //vga
{
	vga_currcol = 0;
	vga_currline = 0;
}

// Turn the display on/off (quickly)
void lcd_display(void) //lcd
{
	LcdTimerDisabler _LcdTimerDisabler;
	lcd_displaycontrol |= LCD_DISPLAYCONTROL_DISPLAYON;
	lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

#if 0
void lcd_no_display(void)
{
	LcdTimerDisabler _LcdTimerDisabler;
	lcd_displaycontrol &= ~LCD_DISPLAYCONTROL_DISPLAYON;
	lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}
#endif

void lcd_set_cursor(uint8_t col, uint8_t row) //vga
{
	vga_currcol = col;
	NOMORE(vga_currcol, LCD_WIDTH - 1);
	vga_currline = row;
	NOMORE(vga_currline, LCD_HEIGHT - 1);
}

static void lcd_set_cursor_hardware(uint8_t col, uint8_t row, bool doDelay = false) //lcd
{
	constexpr uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	lcd_send(LCD_SETDDRAMADDR | (col + row_offsets[row]), 0); 
	if (doDelay)
		_delay_us(100);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
static void lcd_createChar_P(uint8_t location, const uint8_t* charmap) //lcd
{
	location &= 0x7; // we only have 8 locations 0-7
	lcd_command(LCD_SETCGRAMADDR | (location << 3));
	for (uint8_t i = 0; i < 8; i++)
	{
		lcd_send(pgm_read_byte(&charmap[i]), 1);
		_delay_us(LCD_DEFAULT_DELAY);
	}
}

ISR(TIMERx_COMPA_vect)
{
	if (lcd_status & 0x02) //if lcd_timer_disable() is waiting and the nibble is in sync we can safetly disable the ISR
	{
		lcd_status &= ~0x01;
		lcd_timer_disable();
		return;
	}
	uint8_t next_command_type = 0; //0: no char found; 1: lcd_curpos needs to be printed; 2: mandatory jump
	if (vga_map[lcd_curpos >> 3] & (1 << (7 - (lcd_curpos & 0x07)))) next_command_type = 1;
	else
	{
		for (uint8_t i = 0; (i < sizeof(vga_map)) && (next_command_type == 0); i++)
		{
			if (vga_map[i] != 0)
			{
				for (int j = 0; (j < 8) && (next_command_type == 0); j++)
				{
					if (vga_map[i] & (1 << (7 - j)))
					{
						lcd_curpos = (i << 3) + j;
						next_command_type = 2;
					}
				}
			}
		}
	}
	
	if ((next_command_type == 1) && !(!(lcd_status & 0x08) && (lcd_curpos % LCD_WIDTH == 0))) //print current char and last char was jump
	{
#ifdef LCD_DEBUG
		printf_P(PSTR("ISR:print: %c\n"), vga[lcd_curpos % LCD_WIDTH][lcd_curpos / LCD_WIDTH]);
#endif //LCD_DEBUG
		lcd_send(vga[lcd_curpos % LCD_WIDTH][lcd_curpos / LCD_WIDTH], 1);
		vga_map[lcd_curpos >> 3] &= ~(1 << (7 - (lcd_curpos & 0x07))); //clear bit in vga_map
		lcd_status &= ~0x08; //this char is data
		lcd_curpos++;
		if (lcd_curpos == LCD_WIDTH * LCD_HEIGHT) lcd_curpos = 0;
	}
	else if (next_command_type == 0) //no character to print was found. vga_map is empty. disable timer.
	{
#ifdef LCD_DEBUG
		puts_P(PSTR("ISR:disable"));
#endif //LCD_DEBUG
		lcd_status &= ~0x01;
		lcd_timer_disable();
		return;
	}
	else //a jump command is required to the destination lcd_curpos
	{
#ifdef LCD_DEBUG
		printf_P(PSTR("ISR:jump: %02hX\n"));
#endif //LCD_DEBUG
		lcd_set_cursor_hardware(lcd_curpos % LCD_WIDTH, lcd_curpos / LCD_WIDTH);
		lcd_status |= 0x08; // the data is jump
	}
	TCNTx = 0; //clear timer value to make sure timing is correct
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
				lcd_clear(); // EraseScreen
				break;
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
				switch (chr) //todo: remove lcd_cursor entirely
				{
				case 'h': // escape = "\x1b[?25h"
  					// lcd_cursor(); // CursorShow 
					break;
				case 'l': // escape = "\x1b[?25l"
					// lcd_no_cursor(); // CursorHide
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


////////////////////////////////////////////////////////////////////////////////
// Custom character data
const uint8_t lcd_custom_character_set[][8] PROGMEM = {
	{//LCD_STR_BEDTEMP
	0b00000,
	0b11111,
	0b10101,
	0b10001,
	0b10101,
	0b11111,
	0b00000,
	0b00000}, //thanks Sonny Mounicou

	{//LCD_STR_DEGREE
	0b01100,
	0b10010,
	0b10010,
	0b01100,
	0b00000,
	0b00000,
	0b00000,
	0b00000},

	{//LCD_STR_THERMOMETER
	0b00100,
	0b01010,
	0b01010,
	0b01010,
	0b01010,
	0b10001,
	0b10001,
	0b01110},

	{//LCD_STR_UPLEVEL
	0b00100,
	0b01110,
	0b11111,
	0b00100,
	0b11100,
	0b00000,
	0b00000,
	0b00000}, //thanks joris

	{//LCD_STR_REFRESH
	0b00000,
	0b00110,
	0b11001,
	0b11000,
	0b00011,
	0b10011,
	0b01100,
	0b00000}, //thanks joris

	{//LCD_STR_FOLDER
	0b00000,
	0b11100,
	0b11111,
	0b10001,
	0b10001,
	0b11111,
	0b00000,
	0b00000}, //thanks joris

	{//LCD_STR_FEEDRATE
	0b00000,
	0b00100,
	0b10010,
	0b01001,
	0b10010,
	0b00100,
	0b00000,
	0b00000},

	{//LCD_STR_CLOCK
	0b00000,
	0b01110,
	0b10011,
	0b10101,
	0b10001,
	0b01110,
	0b00000,
	0b00000}, //thanks Sonny Mounicou

	{//LCD_STR_ARROW_DOWN
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b10001,
	0b01010,
	0b00100},

	{//LCD_STR_ARROW_2_DOWN
	0b00000,
	0b00000,
	0b10001,
	0b01010,
	0b00100,
	0b10001,
	0b01010,
	0b00100},

	{/* LCD_STR_RESERVED_NL */},
	
	{//LCD_STR_CONFIRM
	0b00000,
	0b00001,
	0b00011,
	0b10110,
	0b11100,
	0b01000,
	0b00000},
};

//sends all 8 custom characters to the lcd
static void lcd_init_custom_characters(void)
{
	LcdTimerDisabler _LcdTimerDisabler;
	
	for (uint8_t i = 0; i < 8; i++)
		lcd_createChar_P(i, &lcd_custom_character_set[i + ((lcd_custom_character_bank >> i) & 1) * 8][0]);
	
	lcd_set_cursor_hardware(lcd_curpos % LCD_WIDTH, lcd_curpos / LCD_WIDTH, true);
}

static void lcd_set_custom_character(uint8_t charID)
{
	LcdTimerDisabler _LcdTimerDisabler;
	
	lcd_createChar_P(charID, &lcd_custom_character_set[charID][0]);
	lcd_custom_character_bank = (lcd_custom_character_bank & ~(1 << (charID & 0x07))) | (((charID / 8) & 1) << (charID & 0x07)); //set the bank information
	
	lcd_set_cursor_hardware(lcd_curpos % LCD_WIDTH, lcd_curpos / LCD_WIDTH, true);
}
