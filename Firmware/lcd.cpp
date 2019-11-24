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

#define LCD_DEFAULT_DELAY 100

#if (defined(LCD_PINS_D0) && defined(LCD_PINS_D1) && defined(LCD_PINS_D2) && defined(LCD_PINS_D3))
	#define LCD_8BIT
#endif

// #define VT100
// #define LCD_DEBUG

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

FILE _lcdout; // = {0}; Global variable is always zero initialized, no need to explicitly state that.

//lcd
uint8_t lcd_displayfunction = 0;
uint8_t lcd_displaycontrol = 0;
uint8_t lcd_displaymode = 0;

#define VGA_MAP_SIZE ((LCD_WIDTH * LCD_HEIGHT) / 8)
#ifdef LCD_DEBUG
	#define DEBUG(x) MYSERIAL.println(x);
#else
	#define DEBUG(x)
#endif

volatile uint8_t lcd_curpos;
//xbbbbaaa: cursor position from 0 to (LCD_WIDTH * LCD_HEIGHT)-1
//bbbb: index of the cluster in vga_map to search. From 0 to VGA_MAP_SIZE-1
//aaa: Rshift value for the bit to search. From 0 to 7
//x: unused. Leave it clear.

volatile uint8_t vga_map[VGA_MAP_SIZE]; //bitmap for changes on the display. Individual bits are set when lcd_write() is used
//    01234567890123456789
  
// 0  00000000111111112222
// 1  22223333333344444444
// 2  55555555666666667777
// 3  77778888888899999999

volatile uint8_t lcd_status = 0;
//xxxxdxba
//a: timer enabled status
//b: bit is set only if the timer is enabled and is used in the ISR to disable itself after required time has passed to send commands to it. ie: lcd_refresh(); 
//d: current command type. 1=lcd_set_cursor (jump); 0=character. Used by the ISR for the second nibble and also to decide whether a jump command is necessary (eg. next character is on the next line).
//x: unused. Do not change.

//vga
uint8_t vga_currline; //these two are used for puting data into the vga buffer.
uint8_t vga_currcol;

uint8_t vga[LCD_WIDTH][LCD_HEIGHT]; //vga buffer.

static void lcd_display(void);
#if 0
static void lcd_no_display(void);
#endif

#ifdef VT100
uint8_t lcd_escape[8];
#endif

#if ((MOTHERBOARD == BOARD_RAMBO_MINI_1_0) || (MOTHERBOARD == BOARD_RAMBO_MINI_1_3))
	#define TCCRxA TCCR3A
	#define TCCRxB TCCR3B
	#define TCCRxC TCCR3C
	#define TCNTx TCNT3
	#define OCRxA OCR3A
	#define TIMSKx TIMSK3
	#define TIFRx TIFR3
	#define TIMERx_COMPA_vect TIMER3_COMPA_vect
#elif ((MOTHERBOARD == BOARD_EINSY_1_0a))
	#define TCCRxA TCCR5A
	#define TCCRxB TCCR5B
	#define TCCRxC TCCR5C
	#define TCNTx TCNT5
	#define OCRxA OCR5A
	#define TIMSKx TIMSK5
	#define TIFRx TIFR5
	#define TIMERx_COMPA_vect TIMER5_COMPA_vect
#endif

void lcd_debug(){
	MYSERIAL.println("VGA:");
	for (int i = 0; i < LCD_HEIGHT; i++){
		for (int j = 0; j < LCD_WIDTH; j++){
			MYSERIAL.print(vga[j][i]);
		}
		MYSERIAL.print("\n");
	}
	
	for (int i = 0; i < 10; i++)
	{
		MYSERIAL.print(vga_map[i], HEX);
		MYSERIAL.print(' ');
	}
	MYSERIAL.print('\n');
	
	MYSERIAL.print("curpos:"); MYSERIAL.println(lcd_curpos, DEC);
	MYSERIAL.print("timer_status:"); MYSERIAL.println(lcd_status, BIN);
	MYSERIAL.print("TCCRxB:"); MYSERIAL.println(TCCRxB, BIN);
}

void lcd_timer_enable(void)
{	
	CRITICAL_SECTION_START; //prevent unwanted timer interrupts while messing with the timer.
	lcd_status |= 0x01; //set timer enabled flag
	lcd_status &= ~0x02; //clear timer force disable flag. Shouldn't be needed, but just to be safe.
	TCCRxB |= 0x02; //start timer. Set clock source
	TCNTx = 0; //clear timer value
	TIFRx |= (1 << OCF3A); //clear interrupt flag by writing 1 in register.
	CRITICAL_SECTION_END;
}

void lcd_timer_disable(void)
{
	if ((lcd_status & 0x01) && !(lcd_status & 0x02)) //do not wait recursively
	{
		lcd_status |= 0x02;
		while(lcd_status & 0x02) 
			// lcd_debug();
			asm ("nop"); //wait for isr to finish what it has to do. The ISR might run a second time to make sure the nibble doesn't get out of sync.
		return; //the ISR disables itself. no need to also disable it here.
	}
	CRITICAL_SECTION_START; //prevent unwanted timer interrupts while messing with the timer.
	lcd_status &= ~0x03; //clear both timer flags
	TCCRxB &= ~(0x07); //stop timer
	TCNTx = 0; //clear timer value
	TIFRx |= (1 << OCF3A); //clear interrupt flag by writing 1 in register.
	CRITICAL_SECTION_END;
}

static void lcd_pulseEnable(void) //lcd
{  
	WRITE(LCD_PINS_ENABLE,HIGH);
	_delay_us(1);    // enable pulse must be >450ns
	WRITE(LCD_PINS_ENABLE,LOW);
}

static void lcd_writebits(uint8_t value) //lcd
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

static void lcd_send(uint8_t data, uint8_t flags, uint16_t duration = LCD_DEFAULT_DELAY) //lcd
{
	WRITE(LCD_PINS_RS,flags&LCD_RS_FLAG);
	// _delay_us(5);
	lcd_writebits(data);
#ifndef LCD_8BIT
	if (!(flags & LCD_HALF_FLAG))
	{
		_delay_us(1);
		lcd_writebits(data<<4);
	}
#endif
	delayMicroseconds(duration);
#ifdef LCD_DEBUG
	MYSERIAL.print("SEND:"); MYSERIAL.print((flags&LCD_RS_FLAG)?1:0, BIN); MYSERIAL.print(' '); MYSERIAL.println(data, HEX);
#endif
}

static void lcd_command(uint8_t value, uint16_t delayExtra = 0) //lcd
{
	lcd_send(value, LOW, LCD_DEFAULT_DELAY + delayExtra);
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
	LcdTimerDisabler_START;
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
	// clear lcd and set all bits to be updated
	lcd_clear_hardware();
	// Initialize to default text direction (for romance languages)
	lcd_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	lcd_command(LCD_ENTRYMODESET | lcd_displaymode);
	
	#ifdef VT100
	lcd_escape[0] = 0;
	#endif
	
	LcdTimerDisabler_END;
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
	
	CRITICAL_SECTION_START;
	// waveform generation = 0100 = CTC
	TCCRxB &= ~(1<<WGM33);
	TCCRxB |=  (1<<WGM32);
	TCCRxA &= ~(1<<WGM31);
	TCCRxA &= ~(1<<WGM30);
	
	// output mode = 00 (disconnected)
	TCCRxA &= ~(3<<COM3A0);
	TCCRxA &= ~(3<<COM3B0);
	TCCRxA &= ~(3<<COM3C0);
	OCRxA = (LCD_DEFAULT_DELAY * 2 * (F_CPU/1000000/8)) - 1; //set timer TOP value with an 8x prescaler. The push speed is slowed down a bit.
	
	lcd_status &= ~0x0b;
	lcd_timer_disable();
	
	// enable interrupt
	TIMSKx = 0x02;
	CRITICAL_SECTION_END;
	
	fdev_setup_stream(lcdout, vga_putchar, NULL, _FDEV_SETUP_WRITE); //setup lcdout stream
}

void lcd_init(void) //lcd
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
	vga_init();
	_delay_us(50000);
	lcd_begin();
	lcd_timer_enable();
}

void lcd_refresh(void)
{
	lcd_timer_disable();
    lcd_begin();
    lcd_set_custom_characters();
	lcd_timer_enable();
}

void lcd_clear(void) //vga
{
	for (int i = 0; i < LCD_WIDTH; i++)
		for (int j = 0; j < LCD_HEIGHT; j++)
			vga[i][j] = ' ';
	for (int i = 0; i < VGA_MAP_SIZE; i++) vga_map[i] = 0xff; //force entire screen update.
	lcd_home();
}

static void lcd_clear_hardware(void) //lcd
{
	lcd_command(LCD_CLEARDISPLAY, 1600);  // clear display, set cursor position to zero
	lcd_curpos = 0;
	for (int i = 0; i < VGA_MAP_SIZE; i++) vga_map[i] = 0xff; //force entire screen update.
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
	LcdTimerDisabler_START;
    lcd_displaycontrol |= LCD_DISPLAYON;
    lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
	LcdTimerDisabler_END;
}

#if 0
void lcd_no_display(void)
{
	LcdTimerDisabler_START;
	lcd_displaycontrol &= ~LCD_DISPLAYON;
	lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
	LcdTimerDisabler_END;
}
#endif

void lcd_set_cursor(uint8_t col, uint8_t row) //vga
{
	vga_currcol = (uint8_t)(constrain((int8_t)(col), 0, LCD_WIDTH-1));
	vga_currline = (uint8_t)(constrain((int8_t)(row), 0, LCD_HEIGHT-1));
}

static void lcd_set_cursor_hardware(uint8_t col, uint8_t row, bool nibbleLess = 0) //lcd
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (nibbleLess)
		lcd_send(LCD_SETDDRAMADDR | (col + row_offsets[row]), LOW, 100);
	else
		lcd_send(LCD_SETDDRAMADDR | (col + row_offsets[row]), LOW, 0);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
static void lcd_createChar_P(uint8_t location, const uint8_t* charmap) //lcd
{
  location &= 0x7; // we only have 8 locations 0-7
  lcd_command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++)
    lcd_send(pgm_read_byte(&charmap[i]), HIGH);
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
		for (int i = 0; (i < VGA_MAP_SIZE) && (next_command_type == 0); i++)
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
		MYSERIAL.print("VGA:print: "); MYSERIAL.println(vga[lcd_curpos % LCD_WIDTH][lcd_curpos / LCD_WIDTH]);
#endif
		lcd_send(vga[lcd_curpos % LCD_WIDTH][lcd_curpos / LCD_WIDTH], HIGH, 0);
		vga_map[lcd_curpos >> 3] &= ~(1 << (7 - (lcd_curpos & 0x07))); //clear bit in vga_map
		lcd_status &= ~0x08; //this char is data
		lcd_curpos++;
		if (lcd_curpos == LCD_WIDTH * LCD_HEIGHT) lcd_curpos = 0;
	}
	else if (next_command_type == 0) //no character to print was found. vga_map is empty. disable timer.
	{
#ifdef LCD_DEBUG
		MYSERIAL.println("ISR:disable");
#endif
		lcd_status &= ~0x01;
		lcd_timer_disable();
		return;
	}
	else //a jump command is required to the destination lcd_curpos
	{
#ifdef LCD_DEBUG
		MYSERIAL.print("VGA:jump: "); MYSERIAL.println(lcd_curpos, DEC);
#endif
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
	LcdTimerDisabler_START;
	lcd_createChar_P(LCD_STR_BEDTEMP[0], lcd_chardata_bedTemp);
	lcd_createChar_P(LCD_STR_DEGREE[0], lcd_chardata_degree);
	lcd_createChar_P(LCD_STR_THERMOMETER[0], lcd_chardata_thermometer);
	lcd_createChar_P(LCD_STR_UPLEVEL[0], lcd_chardata_uplevel);
	lcd_createChar_P(LCD_STR_REFRESH[0], lcd_chardata_refresh); //unused?
	lcd_createChar_P(LCD_STR_FOLDER[0], lcd_chardata_folder);
	lcd_createChar_P(LCD_STR_FEEDRATE[0], lcd_chardata_feedrate);
	lcd_createChar_P(LCD_STR_CLOCK[0], lcd_chardata_clock);
	//lcd_createChar_P(LCD_STR_ARROW_UP[0], lcd_chardata_arrup);
	//lcd_createChar_P(LCD_STR_ARROW_DOWN[0], lcd_chardata_arrdown);
	lcd_set_cursor_hardware(lcd_curpos % LCD_WIDTH, lcd_curpos / LCD_WIDTH, true);
	LcdTimerDisabler_END;
}

void lcd_set_custom_characters_arrows(void)
{
	LcdTimerDisabler_START;
	lcd_createChar_P(1, lcd_chardata_arrdown);
	lcd_set_cursor_hardware(lcd_curpos % LCD_WIDTH, lcd_curpos / LCD_WIDTH, true);
	LcdTimerDisabler_END;
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
	LcdTimerDisabler_START;
	lcd_createChar_P(1, lcd_chardata_arr2down);
	lcd_createChar_P(2, lcd_chardata_confirm);
	lcd_set_cursor_hardware(lcd_curpos % LCD_WIDTH, lcd_curpos / LCD_WIDTH, true);
	LcdTimerDisabler_END;
}

void lcd_set_custom_characters_degree(void)
{
	LcdTimerDisabler_START;
	lcd_createChar_P(1, lcd_chardata_degree);
	lcd_set_cursor_hardware(lcd_curpos % LCD_WIDTH, lcd_curpos / LCD_WIDTH, true);
	LcdTimerDisabler_END;
}

