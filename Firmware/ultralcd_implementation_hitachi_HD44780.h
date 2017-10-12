#ifndef ULTRA_LCD_IMPLEMENTATION_HITACHI_HD44780_H
#define ULTRA_LCD_IMPLEMENTATION_HITACHI_HD44780_H


int scrollstuff = 0;
char longFilenameOLD[LONG_FILENAME_LENGTH];

#include "Configuration_prusa.h"
#include "Marlin.h"
/**
* Implementation of the LCD display routines for a Hitachi HD44780 display. These are common LCD character displays.
* When selecting the Russian language, a slightly different LCD implementation is used to handle UTF8 characters.
**/

#ifndef REPRAPWORLD_KEYPAD
extern volatile uint8_t buttons;  //the last checked buttons in a bit array.
#else
extern volatile uint16_t buttons;  //an extended version of the last checked buttons in a bit array.
#endif

////////////////////////////////////
// Setup button and encode mappings for each panel (into 'buttons' variable
//
// This is just to map common functions (across different panels) onto the same 
// macro name. The mapping is independent of whether the button is directly connected or 
// via a shift/i2c register.

#ifdef ULTIPANEL
// All UltiPanels might have an encoder - so this is always be mapped onto first two bits
#define BLEN_B 1
#define BLEN_A 0

#define EN_B (1<<BLEN_B) // The two encoder pins are connected through BTN_EN1 and BTN_EN2
#define EN_A (1<<BLEN_A)

#if defined(BTN_ENC) && BTN_ENC > -1
  // encoder click is directly connected
  #define BLEN_C 2 
  #define EN_C (1<<BLEN_C) 
#endif 
  
//
// Setup other button mappings of each panel
//
#if defined(LCD_I2C_VIKI)
  #define B_I2C_BTN_OFFSET 3 // (the first three bit positions reserved for EN_A, EN_B, EN_C)
  
  // button and encoder bit positions within 'buttons'
  #define B_LE (BUTTON_LEFT<<B_I2C_BTN_OFFSET)    // The remaining normalized buttons are all read via I2C
  #define B_UP (BUTTON_UP<<B_I2C_BTN_OFFSET)
  #define B_MI (BUTTON_SELECT<<B_I2C_BTN_OFFSET)
  #define B_DW (BUTTON_DOWN<<B_I2C_BTN_OFFSET)
  #define B_RI (BUTTON_RIGHT<<B_I2C_BTN_OFFSET)

  #if defined(BTN_ENC) && BTN_ENC > -1 
    // the pause/stop/restart button is connected to BTN_ENC when used
    #define B_ST (EN_C)                            // Map the pause/stop/resume button into its normalized functional name 
    #define LCD_CLICKED (buttons&(B_MI|B_RI|B_ST)) // pause/stop button also acts as click until we implement proper pause/stop.
  #else
    #define LCD_CLICKED (buttons&(B_MI|B_RI))
  #endif  

  // I2C buttons take too long to read inside an interrupt context and so we read them during lcd_update
  #define LCD_HAS_SLOW_BUTTONS

#elif defined(LCD_I2C_PANELOLU2)
  // encoder click can be read through I2C if not directly connected
  #if BTN_ENC <= 0 
    #define B_I2C_BTN_OFFSET 3 // (the first three bit positions reserved for EN_A, EN_B, EN_C)
  
    #define B_MI (PANELOLU2_ENCODER_C<<B_I2C_BTN_OFFSET) // requires LiquidTWI2 library v1.2.3 or later

    #define LCD_CLICKED (buttons&B_MI)

    // I2C buttons take too long to read inside an interrupt context and so we read them during lcd_update
    #define LCD_HAS_SLOW_BUTTONS
  #else
    #define LCD_CLICKED (buttons&EN_C)  
  #endif

#elif defined(REPRAPWORLD_KEYPAD)
    // define register bit values, don't change it
    #define BLEN_REPRAPWORLD_KEYPAD_F3 0
    #define BLEN_REPRAPWORLD_KEYPAD_F2 1
    #define BLEN_REPRAPWORLD_KEYPAD_F1 2
    #define BLEN_REPRAPWORLD_KEYPAD_UP 3
    #define BLEN_REPRAPWORLD_KEYPAD_RIGHT 4
    #define BLEN_REPRAPWORLD_KEYPAD_MIDDLE 5
    #define BLEN_REPRAPWORLD_KEYPAD_DOWN 6
    #define BLEN_REPRAPWORLD_KEYPAD_LEFT 7
    
    #define REPRAPWORLD_BTN_OFFSET 3 // bit offset into buttons for shift register values

    #define EN_REPRAPWORLD_KEYPAD_F3 (1<<(BLEN_REPRAPWORLD_KEYPAD_F3+REPRAPWORLD_BTN_OFFSET))
    #define EN_REPRAPWORLD_KEYPAD_F2 (1<<(BLEN_REPRAPWORLD_KEYPAD_F2+REPRAPWORLD_BTN_OFFSET))
    #define EN_REPRAPWORLD_KEYPAD_F1 (1<<(BLEN_REPRAPWORLD_KEYPAD_F1+REPRAPWORLD_BTN_OFFSET))
    #define EN_REPRAPWORLD_KEYPAD_UP (1<<(BLEN_REPRAPWORLD_KEYPAD_UP+REPRAPWORLD_BTN_OFFSET))
    #define EN_REPRAPWORLD_KEYPAD_RIGHT (1<<(BLEN_REPRAPWORLD_KEYPAD_RIGHT+REPRAPWORLD_BTN_OFFSET))
    #define EN_REPRAPWORLD_KEYPAD_MIDDLE (1<<(BLEN_REPRAPWORLD_KEYPAD_MIDDLE+REPRAPWORLD_BTN_OFFSET))
    #define EN_REPRAPWORLD_KEYPAD_DOWN (1<<(BLEN_REPRAPWORLD_KEYPAD_DOWN+REPRAPWORLD_BTN_OFFSET))
    #define EN_REPRAPWORLD_KEYPAD_LEFT (1<<(BLEN_REPRAPWORLD_KEYPAD_LEFT+REPRAPWORLD_BTN_OFFSET))

    #define LCD_CLICKED ((buttons&EN_C) || (buttons&EN_REPRAPWORLD_KEYPAD_F1))
    #define REPRAPWORLD_KEYPAD_MOVE_Y_DOWN (buttons&EN_REPRAPWORLD_KEYPAD_DOWN)
    #define REPRAPWORLD_KEYPAD_MOVE_Y_UP (buttons&EN_REPRAPWORLD_KEYPAD_UP)
    #define REPRAPWORLD_KEYPAD_MOVE_HOME (buttons&EN_REPRAPWORLD_KEYPAD_MIDDLE)

#elif defined(NEWPANEL)
  #define LCD_CLICKED (buttons&EN_C)
  
#else // old style ULTIPANEL
  //bits in the shift register that carry the buttons for:
  // left up center down right red(stop)
  #define BL_LE 7
  #define BL_UP 6
  #define BL_MI 5
  #define BL_DW 4
  #define BL_RI 3
  #define BL_ST 2

  //automatic, do not change
  #define B_LE (1<<BL_LE)
  #define B_UP (1<<BL_UP)
  #define B_MI (1<<BL_MI)
  #define B_DW (1<<BL_DW)
  #define B_RI (1<<BL_RI)
  #define B_ST (1<<BL_ST)
  
  #define LCD_CLICKED (buttons&(B_MI|B_ST))
#endif

////////////////////////
// Setup Rotary Encoder Bit Values (for two pin encoders to indicate movement)
// These values are independent of which pins are used for EN_A and EN_B indications
// The rotary encoder part is also independent to the chipset used for the LCD
#if defined(EN_A) && defined(EN_B)
    #define encrot0 0
    #define encrot1 2
    #define encrot2 3
    #define encrot3 1
#endif 

#endif //ULTIPANEL

////////////////////////////////////
// Create LCD class instance and chipset-specific information
#if defined(LCD_I2C_TYPE_PCF8575)
  // note: these are register mapped pins on the PCF8575 controller not Arduino pins
  #define LCD_I2C_PIN_BL  3
  #define LCD_I2C_PIN_EN  2
  #define LCD_I2C_PIN_RW  1
  #define LCD_I2C_PIN_RS  0
  #define LCD_I2C_PIN_D4  4
  #define LCD_I2C_PIN_D5  5
  #define LCD_I2C_PIN_D6  6
  #define LCD_I2C_PIN_D7  7

  #include <Wire.h>
  #include <LCD.h>
  #include <LiquidCrystal_I2C.h>
  #define LCD_CLASS LiquidCrystal_I2C
  LCD_CLASS lcd(LCD_I2C_ADDRESS,LCD_I2C_PIN_EN,LCD_I2C_PIN_RW,LCD_I2C_PIN_RS,LCD_I2C_PIN_D4,LCD_I2C_PIN_D5,LCD_I2C_PIN_D6,LCD_I2C_PIN_D7);
  
#elif defined(LCD_I2C_TYPE_MCP23017)
  //for the LED indicators (which maybe mapped to different things in lcd_implementation_update_indicators())
  #define LED_A 0x04 //100
  #define LED_B 0x02 //010
  #define LED_C 0x01 //001

  #define LCD_HAS_STATUS_INDICATORS

  #include <Wire.h>
  #include <LiquidTWI2.h>
  #define LCD_CLASS LiquidTWI2
  #if defined(DETECT_DEVICE)
     LCD_CLASS lcd(LCD_I2C_ADDRESS, 1);
  #else
     LCD_CLASS lcd(LCD_I2C_ADDRESS);
  #endif
  
#elif defined(LCD_I2C_TYPE_MCP23008)
  #include <Wire.h>
  #include <LiquidTWI2.h>
  #define LCD_CLASS LiquidTWI2
  #if defined(DETECT_DEVICE)
     LCD_CLASS lcd(LCD_I2C_ADDRESS, 1);
  #else
     LCD_CLASS lcd(LCD_I2C_ADDRESS);
  #endif

#elif defined(LCD_I2C_TYPE_PCA8574)
    #include <LiquidCrystal_I2C.h>
    #define LCD_CLASS LiquidCrystal_I2C
    LCD_CLASS lcd(LCD_I2C_ADDRESS, LCD_WIDTH, LCD_HEIGHT);
    
// 2 wire Non-latching LCD SR from:
// https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/schematics#!shiftregister-connection 
#elif defined(SR_LCD_2W_NL)

  extern "C" void __cxa_pure_virtual() { while (1); }
  #include <LCD.h>
  #include <LiquidCrystal_SR.h>
  #define LCD_CLASS LiquidCrystal_SR
  LCD_CLASS lcd(SR_DATA_PIN, SR_CLK_PIN);

#else
  // Standard directly connected LCD implementations
  #ifdef LANGUAGE_RU
    #include "LiquidCrystalRus.h"
    #define LCD_CLASS LiquidCrystalRus
  #else 
    #include "LiquidCrystal.h"
    #define LCD_CLASS LiquidCrystal
  #endif  
  LCD_CLASS lcd(LCD_PINS_RS, LCD_PINS_ENABLE, LCD_PINS_D4, LCD_PINS_D5,LCD_PINS_D6,LCD_PINS_D7);  //RS,Enable,D4,D5,D6,D7
#endif

#if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
  static uint16_t progressBarTick = 0;
  #if PROGRESS_MSG_EXPIRE > 0
    static uint16_t messageTick = 0;
  #endif
  #define LCD_STR_PROGRESS  "\x03\x04\x05"
#endif

/* Custom characters defined in the first 8 characters of the LCD */
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
#define LCD_STR_ARROW_RIGHT "\x7E"  /* from the default character set */

static void lcd_set_custom_characters(
  #if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
    bool progress_bar_set=true
  #endif
) {
  byte bedTemp[8] = {
    B00000,
    B11111,
    B10101,
    B10001,
    B10101,
    B11111,
    B00000,
    B00000
  }; //thanks Sonny Mounicou
  byte degree[8] = {
    B01100,
    B10010,
    B10010,
    B01100,
    B00000,
    B00000,
    B00000,
    B00000
  };
  byte thermometer[8] = {
    B00100,
    B01010,
    B01010,
    B01010,
    B01010,
    B10001,
    B10001,
    B01110
  };
  byte uplevel[8] = {
    B00100,
    B01110,
    B11111,
    B00100,
    B11100,
    B00000,
    B00000,
    B00000
  }; //thanks joris
  byte refresh[8] = {
    B00000,
    B00110,
    B11001,
    B11000,
    B00011,
    B10011,
    B01100,
    B00000,
  }; //thanks joris
  byte folder[8] = {
    B00000,
    B11100,
    B11111,
    B10001,
    B10001,
    B11111,
    B00000,
    B00000
  }; //thanks joris
  #ifdef LANGUAGE_EN_H
  byte feedrate[8] = {
    B11100,
    B10000,
    B11000,
    B10111,
    B00101,
    B00110,
    B00101,
    B00000
  }; //thanks Sonny Mounicou
  #else
 /*
  byte feedrate[8] = {
        B11100,
        B10100,
        B11000,
        B10100,
        B00000,
        B00111,
        B00010,
        B00010
  };
  */
  /*
  byte feedrate[8] = {
        B01100,
        B10011,
        B00000,
        B01100,
        B10011,
        B00000,
        B01100,
        B10011
  };
  */
  byte feedrate[8] = {
        B00000,
        B00100,
        B10010,
        B01001,
        B10010,
        B00100,
        B00000,
        B00000
  };
  #endif

  byte clock[8] = {
    B00000,
    B01110,
    B10011,
    B10101,
    B10001,
    B01110,
    B00000,
    B00000
  }; //thanks Sonny Mounicou

#if 0	// Unused
  byte arrup[8] = {
    B00100,
    B01110,
    B11111,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000
  }; 

  byte arrdown[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B10001,
    B01010,
    B00100
  }; 
#endif

  #if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
    static bool char_mode = false;
    byte progress[3][8] = { {
      B00000,
      B10000,
      B10000,
      B10000,
      B10000,
      B10000,
      B10000,
      B00000
    }, {
      B00000,
      B10100,
      B10100,
      B10100,
      B10100,
      B10100,
      B10100,
      B00000
    }, {
      B00000,
      B10101,
      B10101,
      B10101,
      B10101,
      B10101,
      B10101,
      B00000
    } };
    if (progress_bar_set != char_mode) {
      char_mode = progress_bar_set;
      lcd.createChar(LCD_STR_BEDTEMP[0], bedTemp);
      lcd.createChar(LCD_STR_DEGREE[0], degree);
      lcd.createChar(LCD_STR_THERMOMETER[0], thermometer);
      lcd.createChar(LCD_STR_FEEDRATE[0], feedrate);
      lcd.createChar(LCD_STR_CLOCK[0], clock);
      if (progress_bar_set) {
        // Progress bar characters for info screen
        for (int i=3; i--;) lcd.createChar(LCD_STR_PROGRESS[i], progress[i]);
      }
      else {
        // Custom characters for submenus
        lcd.createChar(LCD_STR_UPLEVEL[0], uplevel);
        lcd.createChar(LCD_STR_REFRESH[0], refresh);
        lcd.createChar(LCD_STR_FOLDER[0], folder);
      }
    }
  #else
    lcd.createChar(LCD_STR_BEDTEMP[0], bedTemp);
    lcd.createChar(LCD_STR_DEGREE[0], degree);
    lcd.createChar(LCD_STR_THERMOMETER[0], thermometer);
    lcd.createChar(LCD_STR_UPLEVEL[0], uplevel);
    lcd.createChar(LCD_STR_REFRESH[0], refresh);
    lcd.createChar(LCD_STR_FOLDER[0], folder);
    lcd.createChar(LCD_STR_FEEDRATE[0], feedrate);
    lcd.createChar(LCD_STR_CLOCK[0], clock);
    //lcd.createChar(LCD_STR_ARROW_UP[0], arrup);
    //lcd.createChar(LCD_STR_ARROW_DOWN[0], arrdown);
  #endif
}

void lcd_set_custom_characters_arrows()
 {

  byte arrdown[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B10001,
    B01010,
    B00100
  }; 

    lcd.createChar(1, arrdown);
}

void lcd_set_custom_characters_progress()
{
	byte progress[8] = {
		B11111,
		B11111,
		B11111,
		B11111,
		B11111,
		B11111,
		B11111,
		B11111,
	};

	lcd.createChar(1, progress);
}

void lcd_set_custom_characters_nextpage()
 {

  byte arrdown[8] = {
    B00000,
    B00000,
    B10001,
    B01010,
    B00100,
    B10001,
    B01010,
    B00100
  }; 
  byte confirm[8] = {
	B00000,
	B00001,
	B00011,
	B10110,
	B11100,
	B01000,
	B00000,
	B00000
  };

    lcd.createChar(1, arrdown);
	lcd.createChar(2, confirm);
}

void lcd_set_custom_characters_degree()
 {
  byte degree[8] = {
    B01100,
    B10010,
    B10010,
    B01100,
    B00000,
    B00000,
    B00000,
    B00000
  };

    lcd.createChar(1, degree);
}


static void lcd_implementation_init(
  #if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
    bool progress_bar_set=true
  #endif
) {

#if defined(LCD_I2C_TYPE_PCF8575)
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  #ifdef LCD_I2C_PIN_BL
    lcd.setBacklightPin(LCD_I2C_PIN_BL,POSITIVE);
    lcd.setBacklight(HIGH);
  #endif
  
#elif defined(LCD_I2C_TYPE_MCP23017)
    lcd.setMCPType(LTI_TYPE_MCP23017);
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);
    lcd.setBacklight(0); //set all the LEDs off to begin with
    
#elif defined(LCD_I2C_TYPE_MCP23008)
    lcd.setMCPType(LTI_TYPE_MCP23008);
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);

#elif defined(LCD_I2C_TYPE_PCA8574)
      lcd.init();
      lcd.backlight();
    
#else
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);
#endif

    lcd_set_custom_characters(
        #if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
            progress_bar_set
        #endif
    );

    lcd.clear();
}


static void lcd_implementation_init_noclear(
  #if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
    bool progress_bar_set=true
  #endif
) {

#if defined(LCD_I2C_TYPE_PCF8575)
    lcd.begin_noclear(LCD_WIDTH, LCD_HEIGHT);
  #ifdef LCD_I2C_PIN_BL
    lcd.setBacklightPin(LCD_I2C_PIN_BL,POSITIVE);
    lcd.setBacklight(HIGH);
  #endif
  
#elif defined(LCD_I2C_TYPE_MCP23017)
    lcd.setMCPType(LTI_TYPE_MCP23017);
    lcd.begin_noclear(LCD_WIDTH, LCD_HEIGHT);
    lcd.setBacklight(0); //set all the LEDs off to begin with
    
#elif defined(LCD_I2C_TYPE_MCP23008)
    lcd.setMCPType(LTI_TYPE_MCP23008);
    lcd.begin_noclear(LCD_WIDTH, LCD_HEIGHT);

#elif defined(LCD_I2C_TYPE_PCA8574)
      lcd.init();
      lcd.backlight();
    
#else
    lcd.begin_noclear(LCD_WIDTH, LCD_HEIGHT);
#endif

    lcd_set_custom_characters(
        #if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
            progress_bar_set
        #endif
    );


}


inline void lcd_implementation_nodisplay()
{
    lcd.noDisplay();
}
inline void lcd_implementation_display()
{
    lcd.display();
}
inline void lcd_implementation_clear()
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

void lcd_implementation_print(int8_t i)
{
    lcd.print(i);
}

void lcd_implementation_print_at(uint8_t x, uint8_t y, int8_t i)
{
    lcd.setCursor(x, y);
    lcd.print(i);
}

void lcd_implementation_print(int i)
{
    lcd.print(i);
}

void lcd_implementation_print_at(uint8_t x, uint8_t y, int i)
{
    lcd.setCursor(x, y);
    lcd.print(i);
}

void lcd_implementation_print(float f)
{
    lcd.print(f);
}

void lcd_implementation_print(const char *str)
{
    lcd.print(str);
}

void lcd_implementation_print_at(uint8_t x, uint8_t y, const char *str)
{
    lcd.setCursor(x, y);
    lcd.print(str);
}

/*

20x4   |01234567890123456789|
       |T 000/000D  Z000.0  |
       |B 000/000D  F100%   |
       |SD100%      T--:--  |
       |Status line.........|
*/
static void lcd_implementation_status_screen()
{
   
    int tHotend=int(degHotend(0) + 0.5);
    int tTarget=int(degTargetHotend(0) + 0.5);

    //Print the hotend temperature
    lcd.setCursor(0, 0);
    lcd.print(LCD_STR_THERMOMETER[0]);
    lcd.print(itostr3(tHotend));
    lcd.print('/');
    lcd.print(itostr3left(tTarget));
    lcd_printPGM(PSTR(LCD_STR_DEGREE " "));
    lcd_printPGM(PSTR("  "));

    //Print the Z coordinates
    lcd.setCursor(LCD_WIDTH - 8-2, 0);
#if 1
    lcd_printPGM(PSTR("  Z"));
    if (custom_message_type == 1) {
        // In a bed calibration mode.
        lcd_printPGM(PSTR("   --- "));
    } else {
        lcd.print(ftostr32sp(current_position[Z_AXIS] + 0.00001));
        lcd.print(' ');
    }
#else
    lcd_printPGM(PSTR(" Queue:"));
    lcd.print(int(moves_planned()));
    lcd.print(' ');
#endif

    //Print the Bedtemperature
    lcd.setCursor(0, 1);
    tHotend=int(degBed() + 0.5);
    tTarget=int(degTargetBed() + 0.5);
    lcd.print(LCD_STR_BEDTEMP[0]);
    lcd.print(itostr3(tHotend));
    lcd.print('/');
    lcd.print(itostr3left(tTarget));
    lcd_printPGM(PSTR(LCD_STR_DEGREE " "));
    lcd_printPGM(PSTR("  "));

#if 1
    //Print Feedrate
    lcd.setCursor(LCD_WIDTH - 8-2, 1);
    lcd_printPGM(PSTR("  "));
    lcd.print(LCD_STR_FEEDRATE[0]);
    lcd.print(itostr3(feedmultiply));
    lcd_printPGM(PSTR("%     "));
#else
    //Print Feedrate
    lcd.setCursor(LCD_WIDTH - 8-2, 1);
    lcd.print(LCD_STR_FEEDRATE[0]);
    lcd.print(itostr3(feedmultiply));
    lcd_printPGM(PSTR("%  Q"));
    {
      uint8_t queue = planner_queue_min();
      if (queue < (BLOCK_BUFFER_SIZE >> 1)) {
        lcd.print('!');
      } else {
        lcd.print((char)(queue / 10) + '0');
        queue %= 10;
      }
      lcd.print((char)queue + '0');
      planner_queue_min_reset();
    }
#endif
	
    //Print SD status
    lcd.setCursor(0, 2);
	if (is_usb_printing)
	{
		lcd_printPGM(PSTR("--"));
	}
	else
	{
		lcd_printPGM(PSTR("SD"));
	}
	if (IS_SD_PRINTING)
	{
		lcd.print(itostr3(card.percentDone()));
		lcd.print('%');
	}
	else
	{
		if (is_usb_printing)
		{
			lcd_printPGM(PSTR(">USB"));
		}
		else
		{
			lcd_printPGM(PSTR("---"));
			lcd.print('%');
		}
	}
    
	// Farm number display
	if (farm_mode)
	{
		lcd_printPGM(PSTR(" F"));
		lcd.print(farm_no);
		lcd_printPGM(PSTR("  "));
        
        // Beat display
        lcd.setCursor(LCD_WIDTH - 1, 0);
        if ( (millis() - kicktime) < 60000 ) {
        
            lcd_printPGM(PSTR("L"));
        
        }else{
            lcd_printPGM(PSTR(" "));
        }
        
	}
	else {
#ifdef SNMM
		lcd_printPGM(PSTR(" E"));
		lcd.print(get_ext_nr() + 1);

#else
		lcd.setCursor(LCD_WIDTH - 8 - 2, 2);
		lcd_printPGM(PSTR(" "));
#endif
	}



    //Print time elapsed
    lcd.setCursor(LCD_WIDTH - 8 -1, 2);
    lcd_printPGM(PSTR(" "));
    lcd.print(LCD_STR_CLOCK[0]);
    if(starttime != 0)
    {
		uint16_t time = millis() / 60000 - starttime / 60000;
        lcd.print(itostr2(time/60));
        lcd.print(':');
        lcd.print(itostr2(time%60));
    }else{
        lcd_printPGM(PSTR("--:--"));
    }
    lcd_printPGM(PSTR("  "));


#ifdef DEBUG_DISABLE_LCD_STATUS_LINE
	return;
#endif //DEBUG_DISABLE_LCD_STATUS_LINE

	//Print status line
    lcd.setCursor(0, 3);

    // If heating in progress, set flag
	if (heating_status != 0) { custom_message = true; }

	if (IS_SD_PRINTING) {
		if (strcmp(longFilenameOLD, card.longFilename) != 0)
		{
			memset(longFilenameOLD, '\0', strlen(longFilenameOLD));
			sprintf_P(longFilenameOLD, PSTR("%s"), card.longFilename);
			scrollstuff = 0;
		}
	}

    // If printing from SD, show what we are printing
	if (IS_SD_PRINTING && !custom_message)
	{
		  if (strlen(card.longFilename) > LCD_WIDTH)
		  {
			  int inters = 0;
			  int gh = scrollstuff;
			  while (((gh - scrollstuff) < LCD_WIDTH) && (inters == 0))
			  {

				  if (card.longFilename[gh] == '\0')
				  {
					  lcd.setCursor(gh - scrollstuff, 3);
					  lcd.print(card.longFilename[gh - 1]);
					  scrollstuff = 0;
					  gh = scrollstuff;
					  inters = 1;
				  }
				  else
				  {
					  lcd.setCursor(gh - scrollstuff, 3);
					  lcd.print(card.longFilename[gh - 1]);
					  gh++;
				  }


			  }
			  scrollstuff++;
		  }
		  else
		  {
			  lcd.print(longFilenameOLD);
		  }
    }
    // If not, check for other special events
	else
	{
        
		if (custom_message)
		{
            // If heating flag, show progress of heating.
			if (heating_status != 0)
			{
				heating_status_counter++;
				if (heating_status_counter > 13)
				{
					heating_status_counter = 0;
				}
				lcd.setCursor(7, 3);
				lcd_printPGM(PSTR("             "));

				for (uint16_t dots = 0; dots < heating_status_counter; dots++)
				{
					lcd.setCursor(7 + dots, 3);
					lcd.print('.');
				}

				switch (heating_status)
				{
				case 1:
					lcd.setCursor(0, 3);
					lcd_printPGM(MSG_HEATING);
					break;
				case 2:
					lcd.setCursor(0, 3);
					lcd_printPGM(MSG_HEATING_COMPLETE);
					heating_status = 0;
					heating_status_counter = 0;
					custom_message = false;
					break;
				case 3:
					lcd.setCursor(0, 3);
					lcd_printPGM(MSG_BED_HEATING);
					break;
				case 4:
					lcd.setCursor(0, 3);
					lcd_printPGM(MSG_BED_DONE);
					heating_status = 0;
					heating_status_counter = 0;
					custom_message = false;
					break;
				default:
					break;
				}
			}
            
            // If mesh bed leveling in progress, show the status
            
			if (custom_message_type == 1)
			{
				if (custom_message_state > 10)
				{
					lcd.setCursor(0, 3);
					lcd_printPGM(PSTR("                    "));
					lcd.setCursor(0, 3);
					lcd_printPGM(MSG_HOMEYZ_PROGRESS);
					lcd_printPGM(PSTR(" : "));
					lcd.print(custom_message_state-10);
				}
				else
				{
					if (custom_message_state == 3)
					{
						lcd_printPGM(WELCOME_MSG);
						lcd_setstatuspgm(WELCOME_MSG);
						custom_message = false;
						custom_message_type = 0;
					}
					if (custom_message_state > 3 && custom_message_state <= 10 )
					{
						lcd.setCursor(0, 3);
						lcd_printPGM(PSTR("                   "));
						lcd.setCursor(0, 3);
						lcd_printPGM(MSG_HOMEYZ_DONE);
						custom_message_state--;
					}
				}

			}
            // If loading filament, print status
			if (custom_message_type == 2)
			{
				lcd.print(lcd_status_message);
			}
			// PID tuning in progress
			if (custom_message_type == 3) {
				lcd.print(lcd_status_message);
				if (pid_cycle <= pid_number_of_cycles && custom_message_state > 0) {
					lcd.setCursor(10, 3);
					lcd.print(itostr3(pid_cycle));
					
					lcd.print('/');
					lcd.print(itostr3left(pid_number_of_cycles));
				}
			}
			// PINDA temp calibration in progress
			if (custom_message_type == 4) {
				char progress[4];
				lcd.setCursor(0, 3);
				lcd_printPGM(MSG_TEMP_CALIBRATION);
				lcd.setCursor(12, 3);
				sprintf(progress, "%d/6", custom_message_state);
				lcd.print(progress);
			}
			// temp compensation preheat
			if (custom_message_type == 5) {
				lcd.setCursor(0, 3);
				lcd_printPGM(MSG_PINDA_PREHEAT);
				if (custom_message_state <= PINDA_HEAT_T) {
					lcd_printPGM(PSTR(": "));
					lcd.print(custom_message_state); //seconds
					lcd.print(' ');
					
				}
			}


		}
	else
		{
            // Nothing special, print status message normally
			lcd.print(lcd_status_message);
		}
	}
    
    // Fill the rest of line to have nice and clean output
    for(int fillspace = 0; fillspace<20;fillspace++)
	{
      if((lcd_status_message[fillspace] > 31 ))
	  {
      }
	  else
	  {
        lcd.print(' ');
      }
    }
	
}


static void lcd_implementation_drawmenu_generic(uint8_t row, const char* pstr, char pre_char, char post_char)
{
    char c;
    //Use all characters in narrow LCDs
  #if LCD_WIDTH < 20
      uint8_t n = LCD_WIDTH - 1 - 1;
    #else
      uint8_t n = LCD_WIDTH - 1 - 2;
  #endif
    lcd.setCursor(0, row);
    lcd.print(pre_char);
    while( ((c = pgm_read_byte(pstr)) != '\0') && (n>0) )
    {
        lcd.print(c);
        pstr++;
        n--;
    }
    while(n--)
        lcd.print(' ');
    lcd.print(post_char);
    lcd.print(' ');
}

static void lcd_implementation_drawmenu_generic_RAM(uint8_t row, const char* str, char pre_char, char post_char)
{
    char c;
    //Use all characters in narrow LCDs
  #if LCD_WIDTH < 20
      uint8_t n = LCD_WIDTH - 1 - 1;
    #else
      uint8_t n = LCD_WIDTH - 1 - 2;
  #endif
    lcd.setCursor(0, row);
    lcd.print(pre_char);
    while( ((c = *str) != '\0') && (n>0) )
    {
        lcd.print(c);
        str++;
        n--;
    }
    while(n--)
        lcd.print(' ');
    lcd.print(post_char);
    lcd.print(' ');
}

static void lcd_implementation_drawmenu_setting_edit_generic(uint8_t row, const char* pstr, char pre_char, char* data)
{
    char c;
    //Use all characters in narrow LCDs
  #if LCD_WIDTH < 20
      uint8_t n = LCD_WIDTH - 1 - 1 - strlen(data);
    #else
      uint8_t n = LCD_WIDTH - 1 - 2 - strlen(data);
  #endif
    lcd.setCursor(0, row);
    lcd.print(pre_char);
    while( ((c = pgm_read_byte(pstr)) != '\0') && (n>0) )
    {
        lcd.print(c);
        pstr++;
        n--;
    }
    lcd.print(':');
    while(n--)
        lcd.print(' ');
    lcd.print(data);
}
#if 0
static void lcd_implementation_drawmenu_setting_edit_generic_P(uint8_t row, const char* pstr, char pre_char, const char* data)
{
    char c;
    //Use all characters in narrow LCDs
  #if LCD_WIDTH < 20
      uint8_t n = LCD_WIDTH - 1 - 1 - strlen_P(data);
    #else
      uint8_t n = LCD_WIDTH - 1 - 2 - strlen_P(data);
  #endif
    lcd.setCursor(0, row);
    lcd.print(pre_char);
    while( ((c = pgm_read_byte(pstr)) != '\0') && (n>0) )
    {
        lcd.print(c);
        pstr++;
        n--;
    }
    lcd.print(':');
    while(n--)
        lcd.print(' ');
    lcd_printPGM(data);
}
#endif
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


void lcd_implementation_drawedit(const char* pstr, char* value)
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

void lcd_implementation_drawedit_2(const char* pstr, char* value)
{
    lcd.setCursor(0, 1);
    lcd_printPGM(pstr);
    lcd.print(':');

    lcd.setCursor((LCD_WIDTH - strlen(value))/2, 3);

    lcd.print(value);
    lcd.print(" mm");
}

static void lcd_implementation_drawmenu_sdfile_selected(uint8_t row, const char* pstr, const char* filename, char* longFilename)
{
    char c;
    int enc_dif = encoderDiff;
    uint8_t n = LCD_WIDTH - 1;
    for(int g = 0; g<4;g++){
      lcd.setCursor(0, g);
    lcd.print(' ');
    }

    lcd.setCursor(0, row);
    lcd.print('>');

    int i = 1;
    int j = 0;
    char* longFilenameTMP = longFilename;
	
    while((c = *longFilenameTMP) != '\0')
    {

        lcd.setCursor(i, row);
        lcd.print(c);
        i++;
        longFilenameTMP++;
        if(i==LCD_WIDTH) {
          i=1;
          j++;
          longFilenameTMP = longFilename + j;          
          n = LCD_WIDTH - 1;
          for(int g = 0; g<300 ;g++){
            if(LCD_CLICKED || ( enc_dif != encoderDiff )){
				longFilenameTMP = longFilename;
				*(longFilenameTMP + LCD_WIDTH - 2) = '\0';
				i = 1;
				j = 0;
				break;
            }else{
				if (j == 1) delay(3);	//wait around 1.2 s to start scrolling text
				delay(1);				//then scroll with redrawing every 300 ms 
            }

          }
        }
    }
    if(c!='\0'){
      lcd.setCursor(i, row);
        lcd.print(c);
        i++;
    }
    n=n-i+1;
    while(n--)
    lcd.print(' ');
}
static void lcd_implementation_drawmenu_sdfile(uint8_t row, const char* pstr, const char* filename, char* longFilename)
{
    char c;
    uint8_t n = LCD_WIDTH - 1;
    lcd.setCursor(0, row);
    lcd.print(' ');
    if (longFilename[0] != '\0')
    {
        filename = longFilename;
        longFilename[LCD_WIDTH-1] = '\0';
    }
    while( ((c = *filename) != '\0') && (n>0) )
    {
        lcd.print(c);
        filename++;
        n--;
    }
    while(n--)
        lcd.print(' ');
}
static void lcd_implementation_drawmenu_sddirectory_selected(uint8_t row, const char* pstr, const char* filename, char* longFilename)
{
    char c;
    uint8_t n = LCD_WIDTH - 2;
    lcd.setCursor(0, row);
    lcd.print('>');
    lcd.print(LCD_STR_FOLDER[0]);
    if (longFilename[0] != '\0')
    {
        filename = longFilename;
        longFilename[LCD_WIDTH-2] = '\0';
    }
    while( ((c = *filename) != '\0') && (n>0) )
    {
        lcd.print(c);
        filename++;
        n--;
    }
    while(n--)
        lcd.print(' ');
}
static void lcd_implementation_drawmenu_sddirectory(uint8_t row, const char* pstr, const char* filename, char* longFilename)
{
    char c;
    uint8_t n = LCD_WIDTH - 2;
    lcd.setCursor(0, row);
    lcd.print(' ');
    lcd.print(LCD_STR_FOLDER[0]);
    if (longFilename[0] != '\0')
    {
        filename = longFilename;
        longFilename[LCD_WIDTH-2] = '\0';
    }
    while( ((c = *filename) != '\0') && (n>0) )
    {
        lcd.print(c);
        filename++;
        n--;
    }
    while(n--)
        lcd.print(' ');
}
#define lcd_implementation_drawmenu_back_selected(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, LCD_STR_UPLEVEL[0], LCD_STR_UPLEVEL[0])
#define lcd_implementation_drawmenu_back(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, ' ', LCD_STR_UPLEVEL[0])
#define lcd_implementation_drawmenu_back_RAM_selected(row, str, data) lcd_implementation_drawmenu_generic_RAM(row, str, LCD_STR_UPLEVEL[0], LCD_STR_UPLEVEL[0])
#define lcd_implementation_drawmenu_back_RAM(row, str, data) lcd_implementation_drawmenu_generic_RAM(row, str, ' ', LCD_STR_UPLEVEL[0])
#define lcd_implementation_drawmenu_submenu_selected(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, '>', LCD_STR_ARROW_RIGHT[0])
#define lcd_implementation_drawmenu_submenu(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, ' ', LCD_STR_ARROW_RIGHT[0])
#define lcd_implementation_drawmenu_gcode_selected(row, pstr, gcode) lcd_implementation_drawmenu_generic(row, pstr, '>', ' ')
#define lcd_implementation_drawmenu_gcode(row, pstr, gcode) lcd_implementation_drawmenu_generic(row, pstr, ' ', ' ')
#define lcd_implementation_drawmenu_function_selected(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, '>', ' ')
#define lcd_implementation_drawmenu_function(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, ' ', ' ')
#define lcd_implementation_drawmenu_setlang_selected(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, '>', ' ')
#define lcd_implementation_drawmenu_setlang(row, pstr, data) lcd_implementation_drawmenu_generic(row, pstr, ' ', ' ')

static void lcd_implementation_quick_feedback()
{
#ifdef LCD_USE_I2C_BUZZER
	#if !defined(LCD_FEEDBACK_FREQUENCY_HZ) || !defined(LCD_FEEDBACK_FREQUENCY_DURATION_MS)
	  lcd_buzz(1000/6,100);
	#else
	  lcd_buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS,LCD_FEEDBACK_FREQUENCY_HZ);
	#endif
#elif defined(BEEPER) && BEEPER > -1
    SET_OUTPUT(BEEPER);
	#if !defined(LCD_FEEDBACK_FREQUENCY_HZ) || !defined(LCD_FEEDBACK_FREQUENCY_DURATION_MS)
    for(int8_t i=0;i<10;i++)
    {
      WRITE(BEEPER,HIGH);
      delayMicroseconds(100);
      WRITE(BEEPER,LOW);
      delayMicroseconds(100);
    }
    #else
    for(int8_t i=0;i<(LCD_FEEDBACK_FREQUENCY_DURATION_MS / (1000 / LCD_FEEDBACK_FREQUENCY_HZ));i++)
    {
      WRITE(BEEPER,HIGH);
      delayMicroseconds(1000000 / LCD_FEEDBACK_FREQUENCY_HZ / 2);
      WRITE(BEEPER,LOW);
      delayMicroseconds(1000000 / LCD_FEEDBACK_FREQUENCY_HZ / 2);
    }
    #endif
#endif
}

#ifdef LCD_HAS_STATUS_INDICATORS
static void lcd_implementation_update_indicators()
{
  #if defined(LCD_I2C_PANELOLU2) || defined(LCD_I2C_VIKI)
    //set the LEDS - referred to as backlights by the LiquidTWI2 library 
    static uint8_t ledsprev = 0;
    uint8_t leds = 0;
    if (target_temperature_bed > 0) leds |= LED_A;
    if (target_temperature[0] > 0) leds |= LED_B;
    if (fanSpeed) leds |= LED_C;
    #if EXTRUDERS > 1  
      if (target_temperature[1] > 0) leds |= LED_C;
    #endif
    if (leds != ledsprev) {
      lcd.setBacklight(leds);
      ledsprev = leds;
    }
  #endif
}
#endif

#ifdef LCD_HAS_SLOW_BUTTONS
extern uint32_t blocking_enc;

static uint8_t lcd_implementation_read_slow_buttons()
{
  #ifdef LCD_I2C_TYPE_MCP23017
  uint8_t slow_buttons;
    // Reading these buttons this is likely to be too slow to call inside interrupt context
    // so they are called during normal lcd_update
    slow_buttons = lcd.readButtons() << B_I2C_BTN_OFFSET; 
    #if defined(LCD_I2C_VIKI)
    if(slow_buttons & (B_MI|B_RI)) { //LCD clicked
       if(blocking_enc > millis()) {
         slow_buttons &= ~(B_MI|B_RI); // Disable LCD clicked buttons if screen is updated
       }
    }
    #endif
    return slow_buttons; 
  #endif
}
#endif

#endif//ULTRA_LCD_IMPLEMENTATION_HITACHI_HD44780_H
