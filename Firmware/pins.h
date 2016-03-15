#ifndef PINS_H
#define PINS_H

#include "boards.h"

#if !MB(5DPRINT)
#define X_MS1_PIN -1
#define X_MS2_PIN -1
#define Y_MS1_PIN -1
#define Y_MS2_PIN -1
#define Z_MS1_PIN -1
#define Z_MS2_PIN -1
#define E0_MS1_PIN -1
#define E0_MS2_PIN -1
#define E1_MS1_PIN -1
#define E1_MS2_PIN -1
#define DIGIPOTSS_PIN -1
#endif

#define LARGE_FLASH true

/*****************************************************************
 * RamboBig Pin Assignments
 ******************************************************************/
#if MOTHERBOARD == 301

#define ELECTRONICS "RAMBoBig"

#define KNOWN_BOARD

#ifndef __AVR_ATmega2560__
  #error Oops!  Make sure you have 'Arduino Mega 2560' selected from the 'Tools -> Boards' menu.
#endif

#define FR_SENS 21
#define X_STEP_PIN 37
#define X_DIR_PIN 48
#define X_MIN_PIN 12
#define X_MAX_PIN 24
#define X_ENABLE_PIN 29
#define X_MS1_PIN 40
#define X_MS2_PIN 41
#define Y_STEP_PIN 36
#define Y_DIR_PIN 49
#define Y_MIN_PIN 11
#define Y_MAX_PIN 23
#define Y_ENABLE_PIN 28
#define Y_MS1_PIN 69
#define Y_MS2_PIN 39
#define Z_STEP_PIN 35
#define Z_DIR_PIN 47
#define Z_MIN_PIN 10
#define Z_MAX_PIN 30
#define Z_ENABLE_PIN 27
#define Z_MS1_PIN 68
#define Z_MS2_PIN 67
#define TEMP_BED_PIN 2
#define TEMP_0_PIN 0
#define HEATER_1_PIN 7
#define TEMP_1_PIN 1
#define TEMP_2_PIN -1

// The SDSS pin uses a different pin mapping from file Sd2PinMap.h
#define SDSS               53

#ifndef SDSUPPORT
  // these pins are defined in the SD library if building with SD support
  #define SCK_PIN           52
  #define MISO_PIN         50
  #define MOSI_PIN         51
#endif

#define E0_STEP_PIN         34
#define E0_DIR_PIN          43
#define E0_ENABLE_PIN       26
#define E0_MS1_PIN 65
#define E0_MS2_PIN 66
#define LED_PIN            13

#ifdef THREEMM_PRINTER
  #define FAN_PIN            8
#else
  #define FAN_PIN            6
#endif

#define KILL_PIN           -1 //80 with Smart Controller LCD
#define SUICIDE_PIN        -1  //PIN that has to be turned on right after start, to keep power flowing.
#define SDPOWER            -1

#ifdef BARICUDA
  #define HEATER_2_PIN 6
#else
  #define HEATER_2_PIN -1
#endif

#define E1_STEP_PIN         33
#define E1_DIR_PIN          42
#define E1_ENABLE_PIN       25
#define E1_MS1_PIN 63
#define E1_MS2_PIN 64
#define DIGIPOTSS_PIN 38
#define DIGIPOT_CHANNELS {4,5,3,0,1} // X Y Z E0 E1 digipot channels to stepper driver mapping
#define HEATER_0_PIN  9
#define HEATER_BED_PIN 3
#define PS_ON_PIN          4
#define SDSS               53

#define BEEPER 79
#define LCD_PINS_RS 70
#define LCD_PINS_ENABLE 71
#define LCD_PINS_D4 72
#define LCD_PINS_D5 73
#define LCD_PINS_D6 74
#define LCD_PINS_D7 75
#define BTN_EN1 76
#define BTN_EN2 77
#define BTN_ENC 78
#define BLEN_C 2
#define BLEN_B 1
#define BLEN_A 0
#define SDCARDDETECT 81
#define encrot0 0
#define encrot1 2
#define encrot2 3
#define encrot3 1

#endif


/*****************************************************************
* Rambo Pin Assignments 1.3
******************************************************************/
#if MOTHERBOARD == 302
#define ELECTRONICS "RAMBo13a"

#define KNOWN_BOARD

#ifndef __AVR_ATmega2560__
    #error Oops!  Make sure you have 'Arduino Mega 2560' selected from the 'Tools -> Boards' menu.
#endif

#define FR_SENS 21
#define X_STEP_PIN 37
#define X_DIR_PIN 48
#define X_MIN_PIN 12
#define X_MAX_PIN 24
#define X_ENABLE_PIN 29
#define X_MS1_PIN 40
#define X_MS2_PIN 41
#define Y_STEP_PIN 36
#define Y_DIR_PIN 49
#define Y_MIN_PIN 11
#define Y_MAX_PIN 23
#define Y_ENABLE_PIN 28
#define Y_MS1_PIN 69
#define Y_MS2_PIN 39
#define Z_STEP_PIN 35
#define Z_DIR_PIN 47
#define Z_MIN_PIN 10
#define Z_MAX_PIN 30
#define Z_ENABLE_PIN 27
#define Z_MS1_PIN 68
#define Z_MS2_PIN 67
#define TEMP_BED_PIN 2
#define TEMP_0_PIN 0
#define HEATER_1_PIN 7
#define TEMP_1_PIN 1
#define TEMP_2_PIN -1

// The SDSS pin uses a different pin mapping from file Sd2PinMap.h
#define SDSS               53

#ifndef SDSUPPORT
    // these pins are defined in the SD library if building with SD support
    #define SCK_PIN           52
    #define MISO_PIN         50
    #define MOSI_PIN         51
#endif

#define E0_STEP_PIN         34
#define E0_DIR_PIN          43
#define E0_ENABLE_PIN       26
#define E0_MS1_PIN 65
#define E0_MS2_PIN 66
#define LED_PIN            13

#ifdef THREEMM_PRINTER
    #define FAN_PIN            8
#else
    #define FAN_PIN            6
#endif

#define KILL_PIN           -1 //80 with Smart Controller LCD
#define SUICIDE_PIN        -1  //PIN that has to be turned on right after start, to keep power flowing.
#define SDPOWER            -1

#ifdef BARICUDA
    #define HEATER_2_PIN 6
#else
    #define HEATER_2_PIN -1
#endif

#define BEEPER 84
#define BTN_EN1 72
#define BTN_EN2 14
#define BTN_ENC 9
#define SDCARDDETECT 15
#define LCD_PINS_RS 82
#define LCD_PINS_ENABLE 18
#define LCD_PINS_D4 19
#define LCD_PINS_D5 70
#define LCD_PINS_D6 85
#define LCD_PINS_D7 71
#define HEATER_0_PIN 3
#define HEATER_BED_PIN 4
#define FAN_1_PIN -1 //6
#define PS_ON_PIN 71

#define MOTOR_CURRENT_PWM_XY_PIN 46
#define MOTOR_CURRENT_PWM_Z_PIN 45
#define MOTOR_CURRENT_PWM_E_PIN 44

#endif


/*****************************************************************
* Rambo mini Pin Assignments 1.0
******************************************************************/
#if MOTHERBOARD == 102
#define ELECTRONICS "RAMBo10a"

#define KNOWN_BOARD

#ifndef __AVR_ATmega2560__
    #error Oops!  Make sure you have 'Arduino Mega 2560' selected from the 'Tools -> Boards' menu.
#endif

#define FR_SENS 21
#define X_STEP_PIN 37
#define X_DIR_PIN 48
#define X_MIN_PIN 12
#define X_MAX_PIN 24
#define X_ENABLE_PIN 29
#define X_MS1_PIN 40
#define X_MS2_PIN 41
#define Y_STEP_PIN 36
#define Y_DIR_PIN 49
#define Y_MIN_PIN 11
#define Y_MAX_PIN 23
#define Y_ENABLE_PIN 28
#define Y_MS1_PIN 69
#define Y_MS2_PIN 39
#define Z_STEP_PIN 35
#define Z_DIR_PIN 47
#define Z_MIN_PIN 10
#define Z_MAX_PIN 30
#define Z_ENABLE_PIN 27
#define Z_MS1_PIN 68
#define Z_MS2_PIN 67
#define TEMP_BED_PIN 2
#define TEMP_0_PIN 0
#define HEATER_1_PIN 7
#define TEMP_1_PIN 1
#define TEMP_2_PIN -1

// The SDSS pin uses a different pin mapping from file Sd2PinMap.h
#define SDSS               53

#ifndef SDSUPPORT
    // these pins are defined in the SD library if building with SD support
    #define SCK_PIN           52
    #define MISO_PIN         50
    #define MOSI_PIN         51
#endif

#define BEEPER 78
#define BTN_EN1 80
#define BTN_EN2 73
#define BTN_ENC 21
#define SDCARDDETECT 72
#define LCD_PINS_RS 38
#define LCD_PINS_ENABLE 5
#define LCD_PINS_D4 14
#define LCD_PINS_D5 15
#define LCD_PINS_D6 32
#define LCD_PINS_D7 31

#define E0_STEP_PIN         34
#define E0_DIR_PIN          43
#define E0_ENABLE_PIN       26
#define E0_MS1_PIN 65
#define E0_MS2_PIN 66
#define LED_PIN            13

#ifdef THREEMM_PRINTER
    #define FAN_PIN            8
#else
    #define FAN_PIN            6
#endif

#define KILL_PIN           -1 //80 with Smart Controller LCD
#define SUICIDE_PIN        -1  //PIN that has to be turned on right after start, to keep power flowing.
#define SDPOWER            -1

#ifdef BARICUDA
    #define HEATER_2_PIN 6
#else
    #define HEATER_2_PIN -1
#endif

#define HEATER_0_PIN 3
#define HEATER_BED_PIN 4
#define FAN_1_PIN -1 //6
#define PS_ON_PIN 71

#define MOTOR_CURRENT_PWM_XY_PIN 46
#define MOTOR_CURRENT_PWM_Z_PIN 45
#define MOTOR_CURRENT_PWM_E_PIN 44

#endif





#ifndef KNOWN_BOARD
#error Unknown MOTHERBOARD value in configuration.h
#endif

//List of pins which to ignore when asked to change by gcode, 0 and 1 are RX and TX, do not mess with those!
#define _E0_PINS E0_STEP_PIN, E0_DIR_PIN, E0_ENABLE_PIN, HEATER_0_PIN,
#if EXTRUDERS > 1
  #define _E1_PINS E1_STEP_PIN, E1_DIR_PIN, E1_ENABLE_PIN, HEATER_1_PIN,
#else
  #define _E1_PINS
#endif
#if EXTRUDERS > 2
  #define _E2_PINS E2_STEP_PIN, E2_DIR_PIN, E2_ENABLE_PIN, HEATER_2_PIN,
#else
  #define _E2_PINS
#endif

#ifdef X_STOP_PIN
  #if X_HOME_DIR < 0
    #define X_MIN_PIN X_STOP_PIN
    #define X_MAX_PIN -1
  #else
    #define X_MIN_PIN -1
    #define X_MAX_PIN X_STOP_PIN
  #endif
#endif

#ifdef Y_STOP_PIN
  #if Y_HOME_DIR < 0
    #define Y_MIN_PIN Y_STOP_PIN
    #define Y_MAX_PIN -1
  #else
    #define Y_MIN_PIN -1
    #define Y_MAX_PIN Y_STOP_PIN
  #endif
#endif

#ifdef Z_STOP_PIN
  #if Z_HOME_DIR < 0
    #define Z_MIN_PIN Z_STOP_PIN
    #define Z_MAX_PIN -1
  #else
    #define Z_MIN_PIN -1
    #define Z_MAX_PIN Z_STOP_PIN
  #endif
#endif

#ifdef DISABLE_MAX_ENDSTOPS
#define X_MAX_PIN          -1
#define Y_MAX_PIN          -1
#define Z_MAX_PIN          -1
#endif

#ifdef DISABLE_MIN_ENDSTOPS
#define X_MIN_PIN          -1
#define Y_MIN_PIN          -1
#define Z_MIN_PIN          -1
#endif

#define SENSITIVE_PINS {0, 1, X_STEP_PIN, X_DIR_PIN, X_ENABLE_PIN, X_MIN_PIN, X_MAX_PIN, Y_STEP_PIN, Y_DIR_PIN, Y_ENABLE_PIN, Y_MIN_PIN, Y_MAX_PIN, Z_STEP_PIN, Z_DIR_PIN, Z_ENABLE_PIN, Z_MIN_PIN, Z_MAX_PIN, PS_ON_PIN, \
                        HEATER_BED_PIN, FAN_PIN,                  \
                        _E0_PINS _E1_PINS _E2_PINS             \
                        analogInputToDigitalPin(TEMP_0_PIN), analogInputToDigitalPin(TEMP_1_PIN), analogInputToDigitalPin(TEMP_2_PIN), analogInputToDigitalPin(TEMP_BED_PIN) }

#endif //__PINS_H
