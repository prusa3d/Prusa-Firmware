#define KNOWN_BOARD
#ifndef __AVR_ATmega2560__
  #error Oops!  Make sure you have 'Arduino Mega 2560' selected from the 'Tools -> Boards' menu.
#endif
#define ELECTRONICS "RAMPS14"


#define FR_SENS -1

#define X_STEP_PIN 54
#define X_DIR_PIN 55
#define X_MIN_PIN 3
#define X_MAX_PIN 2
#define X_ENABLE_PIN 38
#define X_MS1_PIN -1
#define X_MS2_PIN -1
#define X_CS_PIN 53


#define Y_STEP_PIN 60
#define Y_DIR_PIN 61
#define Y_MIN_PIN 14
#define Y_MAX_PIN 15
#define Y_ENABLE_PIN 56
#define Y_MS1_PIN -1
#define Y_MS2_PIN -1
#define Y_CS_PIN 49


#define Z_STEP_PIN 46
#define Z_DIR_PIN 48
#define Z_MIN_PIN 18
#define Z_MAX_PIN 19
#define Z_ENABLE_PIN 62
#define Z_MS1_PIN -1
#define Z_MS2_PIN -1
#define Z_CS_PIN 40

#define E0_STEP_PIN 26
#define E0_DIR_PIN  28
#define E0_ENABLE_PIN 24
#define E0_MS1_PIN -1
#define E0_MS2_PIN -1
#define E0_CS_PIN 42



#define TEMP_BED_PIN 14
#define TEMP_0_PIN 13
#define HEATER_1_PIN 7
#define TEMP_1_PIN 15
#define TEMP_2_PIN -1

  
#ifdef SNMM 
  #define E_MUX0_PIN 40 
  #define E_MUX1_PIN 42 
  #define E_MUX2_PIN 44 
#endif

#ifdef DIS
//  #define D_REQUIRE 2
//  #define D_DATA 20
//  #define D_DATACLOCK 21
#endif

// The SDSS pin uses a different pin mapping from file Sd2PinMap.h
#define SDSS               53

#ifndef SDSUPPORT
  // these pins are defined in the SD library if building with SD support
  #define SCK_PIN           52
  #define MISO_PIN         50
  #define MOSI_PIN         51
#endif

#define BEEPER_PIN 84

#define BTN_EN1 31
#define BTN_EN2 33
#define BTN_ENC 35

#define SDCARDDETECT 49

#define LCD_PINS_RS 16
#define LCD_PINS_ENABLE 17
#define LCD_PINS_D4 23
#define LCD_PINS_D5 25
#define LCD_PINS_D6 27
#define LCD_PINS_D7 29



#define LED_PIN            13
#define KILL_PIN          41 //80 with Smart Controller LCD
#define SUICIDE_PIN        -1  //PIN that has to be turned on right after start, to keep power flowing.
#define SDPOWER            -1
#define HEATER_2_PIN -1


#define HEATER_0_PIN 10
#define HEATER_BED_PIN 8
#define FAN_1_PIN -1 
#define PS_ON_PIN 12

#define FAN_PIN            4

#define EXTRUDER_0_AUTO_FAN_PIN   5
#define EXTRUDER_1_AUTO_FAN_PIN   -1
#define EXTRUDER_2_AUTO_FAN_PIN   -1





