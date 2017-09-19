/*****************************************************************
* Rambo mini 1.0 Pin Assignments
******************************************************************/

#define ELECTRONICS "RAMBo10a"

#define KNOWN_BOARD
#ifndef __AVR_ATmega2560__
  #error Oops!  Make sure you have 'Arduino Mega 2560' selected from the 'Tools -> Boards' menu.
#endif

#define FR_SENS                21

#define X_STEP_PIN             37
#define X_DIR_PIN              48
#define X_MIN_PIN              12
#define X_MAX_PIN              30
#define X_ENABLE_PIN           29
#define X_MS1_PIN              40
#define X_MS2_PIN              41
#define Y_STEP_PIN             36
#define Y_DIR_PIN              49
#define Y_MIN_PIN              11
#define Y_MAX_PIN              24
#define Y_ENABLE_PIN           28
#define Y_MS1_PIN              69
#define Y_MS2_PIN              39
#define Z_STEP_PIN             35
#define Z_DIR_PIN              47
#define Z_MIN_PIN              10
#define Z_MAX_PIN              23
#define Z_ENABLE_PIN           27
#define Z_MS1_PIN              68
#define Z_MS2_PIN              67
#define TEMP_BED_PIN            2
#define TEMP_0_PIN              0
#define HEATER_1_PIN            7
#define TEMP_1_PIN              1
#define TEMP_2_PIN             -1

#ifdef SNMM
  #define E_MUX0_PIN           17
  #define E_MUX1_PIN           16
  #define E_MUX2_PIN           84
#endif

// The SDSS pin uses a different pin mapping from file Sd2PinMap.h
#define SDSS                   53

#ifndef SDSUPPORT
// these pins are defined in the SD library if building with SD support
  #define SCK_PIN              52
  #define MISO_PIN             50
  #define MOSI_PIN             51
#endif
  
#define BEEPER                 78

#define BTN_EN1                80
#define BTN_EN2                73
#define BTN_ENC                21

#define SDCARDDETECT           72

#define LCD_PINS_RS            38
#define LCD_PINS_ENABLE         5
#define LCD_PINS_D4            14
#define LCD_PINS_D5            15
#define LCD_PINS_D6            32
#define LCD_PINS_D7            31

#define E0_STEP_PIN            34
#define E0_DIR_PIN             43
#define E0_ENABLE_PIN          26
#define E0_MS1_PIN             65
#define E0_MS2_PIN             66
#define LED_PIN                13
#ifdef THREEMM_PRINTER
  #define FAN_PIN               8
#else
  #define FAN_PIN               6
#endif
#define KILL_PIN               -1 //80 with Smart Controller LCD
#define SUICIDE_PIN            -1  //PIN that has to be turned on right after start, to keep power flowing.
#define SDPOWER                -1
#define HEATER_2_PIN           -1

#define HEATER_0_PIN            3
#define HEATER_BED_PIN          4
#define FAN_1_PIN              -1 //6
#define PS_ON_PIN              71
#define MOTOR_CURRENT_PWM_XY_PIN   46
#define MOTOR_CURRENT_PWM_Z_PIN    45
#define MOTOR_CURRENT_PWM_E_PIN    44
