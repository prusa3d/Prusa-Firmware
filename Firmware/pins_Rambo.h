/*****************************************************************
* Rambo Pin Assignments
******************************************************************/

#define ELECTRONICS "RAMBoBig"

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

#ifdef DIS
  #define D_REQUIRE            30
  #define D_DATA               20
  #define D_DATACLOCK          21
#endif

// The SDSS pin uses a different pin mapping from file Sd2PinMap.h
#define SDSS                   53

#ifndef SDSUPPORT
// these pins are defined in the SD library if building with SD support
  #define SCK_PIN              52
  #define MISO_PIN             50
  #define MOSI_PIN             51
#endif

#define BEEPER                 84

#define BTN_EN1                72
#define BTN_EN2                14
#define BTN_ENC                 9

#define SDCARDDETECT           15

#define LCD_PINS_RS            82
#define LCD_PINS_ENABLE        18
#define LCD_PINS_D4            19
#define LCD_PINS_D5            70
#define LCD_PINS_D6            85
#define LCD_PINS_D7            71

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

#define E1_STEP_PIN            33
#define E1_DIR_PIN             42
#define E1_ENABLE_PIN          25
#define E1_MS1_PIN             63
#define E1_MS2_PIN             64
#define DIGIPOTSS_PIN          38
#define DIGIPOT_CHANNELS       {4,5,3,0,1} // X Y Z E0 E1 digipot channels to stepper driver mapping
#define HEATER_0_PIN            9
#define HEATER_BED_PIN          3
#define PS_ON_PIN               4
#define SDSS                   53
#ifdef ULTRA_LCD
 #define KILL_PIN              80
 #ifdef NEWPANEL
  //arduino pin which triggers an piezzo beeper
  #define BEEPER               84      // Beeper on AUX-4
  #define LCD_PINS_RS          82
  #define LCD_PINS_ENABLE      18
  #define LCD_PINS_D4          19
  #define LCD_PINS_D5          70
  #define LCD_PINS_D6          85
  #define LCD_PINS_D7          71
  //buttons are directly attached using AUX-2
  #define BTN_EN1              76
  #define BTN_EN2              77
  #define BTN_ENC              78  //the click
  #define BLEN_C                2
  #define BLEN_B                1
  #define BLEN_A                0
  #define SDCARDDETECT         81    // Ramps does not use this port
  //encoder rotation values
  #define encrot0               0
  #define encrot1               2
  #define encrot2               3
  #define encrot3               1
 #else //old style panel with shift register
  //arduino pin witch triggers an piezzo beeper
  #define BEEPER               84    //No Beeper added
  //buttons are attached to a shift register
  // Not wired this yet
  // #define SHIFT_CLK 38
  // #define SHIFT_LD 42
  // #define SHIFT_OUT 40
  // #define SHIFT_EN 17
  #define LCD_PINS_RS          82
  #define LCD_PINS_ENABLE      18
  #define LCD_PINS_D4          19
  #define LCD_PINS_D5          70
  #define LCD_PINS_D6          85
  #define LCD_PINS_D7          71
  //encoder rotation values
  #define encrot0               0
  #define encrot1               2
  #define encrot2               3
  #define encrot3               1
  //bits in the shift register that carry the buttons for:
  // left up center down right red
  #define BL_LE                 7
  #define BL_UP                 6
  #define BL_MI                 5
  #define BL_DW                 4
  #define BL_RI                 3
  #define BL_ST                 2
  #define BLEN_B                1
  #define BLEN_A                0
 #endif
#endif //ULTRA_LCD
