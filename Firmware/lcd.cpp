//menu.cpp

#include "lcd.h"
#include <stdio.h>
#include <stdarg.h>
#include <avr/pgmspace.h>
#include "Timer.h"

extern FILE _lcdout;
#define lcdout (&_lcdout)

uint8_t lcd_draw_update = 2;
int32_t lcd_encoder = 0;
uint8_t lcd_encoder_bits = 0;
int8_t lcd_encoder_diff = 0;

uint8_t lcd_buttons = 0;
uint8_t lcd_button_pressed = 0;
uint8_t lcd_update_enabled = 1;
uint32_t lcd_timeoutToStatus = 0;

uint32_t lcd_next_update_millis = 0;
uint8_t lcd_status_update_delay = 0;

uint8_t lcd_long_press_active = 0;

lcd_longpress_func_t lcd_longpress_func = 0;

lcd_charsetup_func_t lcd_charsetup_func = 0;

lcd_lcdupdate_func_t lcd_lcdupdate_func = 0;

uint32_t lcd_button_blanking_time = millis();
ShortTimer longPressTimer;


uint8_t lcd_clicked(void)
{
	bool clicked = LCD_CLICKED;
	if(clicked) lcd_button_pressed = 1;
    return clicked;
}

void lcd_set_cursor(uint8_t c, uint8_t r)
{
	lcd_printf_P(PSTR("\x1b[%hhu;%hhuH"), r, c);
}

void lcd_beeper_quick_feedback(void)
{
	SET_OUTPUT(BEEPER);
	for(int8_t i = 0; i < 10; i++)
	{
		WRITE(BEEPER,HIGH);
		delayMicroseconds(100);
		WRITE(BEEPER,LOW);
		delayMicroseconds(100);
	}
}

void lcd_quick_feedback(void)
{
  lcd_draw_update = 2;
  lcd_button_pressed = false;  
  lcd_beeper_quick_feedback();
}

int lcd_puts_P(const char* str)
{
	return fputs_P(str, lcdout);
}

int lcd_putc(int c)
{
	return fputc(c, lcdout);
}

int lcd_printf_P(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int ret = vfprintf_P(lcdout, format, args);
	va_end(args);
	return ret;
}










void lcd_update(uint8_t lcdDrawUpdateOverride)
{
	if (lcd_draw_update < lcdDrawUpdateOverride)
		lcd_draw_update = lcdDrawUpdateOverride;
	if (!lcd_update_enabled)
		return;
	lcd_buttons_update();
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
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
			// Force the keypad update now.
			lcd_next_update_millis = millis() - 1;
			// Full update.
			lcd_implementation_clear();
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
	static bool _lock = false;
	if (_lock) return;
	_lock = true;
	uint8_t newbutton = 0;
	if (READ(BTN_EN1) == 0)  newbutton |= EN_A;
	if (READ(BTN_EN2) == 0)  newbutton |= EN_B;
	if (lcd_update_enabled)
	{ //if we are in non-modal mode, long press can be used and short press triggers with button release
		if (READ(BTN_ENC) == 0)
		{ //button is pressed	  
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
			if (millis() > lcd_button_blanking_time)
			{
				lcd_button_blanking_time = millis() + BUTTON_BLANKING_TIME;
				if ((lcd_button_pressed == 0) && (lcd_long_press_active == 0))
				{
					longPressTimer.start();
					lcd_button_pressed = 1;
				}
				else
				{
					if (longPressTimer.expired(LONG_PRESS_TIME))
					{
						lcd_long_press_active = 1;
						if (lcd_longpress_func)
							lcd_longpress_func();
					}
				}
			}
		}
		else
		{ //button not pressed
			if (lcd_button_pressed)
			{ //button was released
				lcd_button_blanking_time = millis() + BUTTON_BLANKING_TIME;
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
	}
	else
	{ //we are in modal mode
		if (READ(BTN_ENC) == 0)
			newbutton |= EN_C; 
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
	_lock = false;
}






LCD_CLASS lcd(LCD_PINS_RS, LCD_PINS_ENABLE, LCD_PINS_D4, LCD_PINS_D5,LCD_PINS_D6,LCD_PINS_D7);  //RS,Enable,D4,D5,D6,D7


void lcd_implementation_init(void)
{
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);
    lcd_set_custom_characters();
    lcd.clear();
}


void lcd_implementation_init_noclear(void)
{
    lcd.begin_noclear(LCD_WIDTH, LCD_HEIGHT);
    lcd_set_custom_characters();
}


void lcd_implementation_clear(void)
{
    lcd.clear();
}
/* Arduino < 1.0.0 is missing a function to print PROGMEM strings, so we need to implement our own */
void lcd_printPGM(const char* str)
{
    char c;
    while((c = pgm_read_byte(str++)) != '\0')
    {
        lcd.write(c);
    }
}

void lcd_print_at_PGM(uint8_t x, uint8_t y, const char* str)
{
    lcd.setCursor(x, y);
    char c;
    while((c = pgm_read_byte(str++)) != '\0')
    {
        lcd.write(c);
    }
}

void lcd_implementation_write(char c)
{
    lcd.write(c);
}

void lcd_print(int8_t i)
{
    lcd.print(i);
}

void lcd_print_at(uint8_t x, uint8_t y, int8_t i)
{
    lcd.setCursor(x, y);
    lcd.print(i);
}

void lcd_print(int i)
{
    lcd.print(i);
}

void lcd_print_at(uint8_t x, uint8_t y, int i)
{
    lcd.setCursor(x, y);
    lcd.print(i);
}

void lcd_print(float f)
{
    lcd.print(f);
}

void lcd_print(const char *str)
{
    lcd.print(str);
}

void lcd_print_at(uint8_t x, uint8_t y, const char *str)
{
    lcd.setCursor(x, y);
    lcd.print(str);
}










void lcd_drawedit(const char* pstr, char* value)
{
    lcd.setCursor(1, 1);
    lcd_printPGM(pstr);
    lcd.print(':');
   #if LCD_WIDTH < 20
      lcd.setCursor(LCD_WIDTH - strlen(value), 1);
    #else
      lcd.setCursor(LCD_WIDTH -1 - strlen(value), 1);
   #endif
    lcd.print(value);
}

void lcd_drawedit_2(const char* pstr, char* value)
{
    lcd.setCursor(0, 1);
    lcd_printPGM(pstr);
    lcd.print(':');

    lcd.setCursor((LCD_WIDTH - strlen(value))/2, 3);

    lcd.print(value);
    lcd.print(" mm");
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

const uint8_t lcd_chardata_feedrate[8] PROGMEM = {
	B11100,
	B10000,
	B11000,
	B10111,
	B00101,
	B00110,
	B00101,
	B00000}; //thanks Sonny Mounicou

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

/*const uint8_t lcd_chardata_feedrate[8] PROGMEM = {
	B00000,
	B00100,
	B10010,
	B01001,
	B10010,
	B00100,
	B00000,
	B00000};*/

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
	lcd.createChar_P(LCD_STR_BEDTEMP[0], lcd_chardata_bedTemp);
	lcd.createChar_P(LCD_STR_DEGREE[0], lcd_chardata_degree);
	lcd.createChar_P(LCD_STR_THERMOMETER[0], lcd_chardata_thermometer);
	lcd.createChar_P(LCD_STR_UPLEVEL[0], lcd_chardata_uplevel);
	lcd.createChar_P(LCD_STR_REFRESH[0], lcd_chardata_refresh);
	lcd.createChar_P(LCD_STR_FOLDER[0], lcd_chardata_folder);
	lcd.createChar_P(LCD_STR_FEEDRATE[0], lcd_chardata_feedrate);
	lcd.createChar_P(LCD_STR_CLOCK[0], lcd_chardata_clock);
	//lcd.createChar_P(LCD_STR_ARROW_UP[0], lcd_chardata_arrup);
	//lcd.createChar_P(LCD_STR_ARROW_DOWN[0], lcd_chardata_arrdown);
}

void lcd_set_custom_characters_arrows(void)
{
	lcd.createChar_P(1, lcd_chardata_arrdown);
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
	lcd.createChar_P(1, lcd_chardata_progress);
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
	lcd.createChar_P(1, lcd_chardata_arr2down);
	lcd.createChar_P(2, lcd_chardata_confirm);
}

void lcd_set_custom_characters_degree(void)
{
	lcd.createChar_P(1, lcd_chardata_degree);
}

