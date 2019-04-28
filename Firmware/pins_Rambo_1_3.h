/*****************************************************************
* Rambo mini 1.3 Pin Assignments
******************************************************************/

#define ELECTRONICS "RAMBo13a"

#define KNOWN_BOARD
#ifndef __AVR_ATmega2560__
  #error Oops!  Make sure you have 'Arduino Mega 2560 or Rambo' selected from the 'Tools -> Boards' menu.
#endif

#define PINDA_THERMISTOR

#define SWI2C_SDA      20 //SDA on P3
#define SWI2C_SCL      21 //SCL on P3

#ifdef MICROMETER_LOGGING
#define D_DATACLOCK		24	//Y_MAX (green)
#define D_DATA			30	//X_MAX (blue)
#define D_REQUIRE		23	//Z_MAX (white)
#endif //MICROMETER_LOGGING



#define X_STEP_PIN             37
#define X_DIR_PIN              48
#define X_MIN_PIN              12
#define X_MAX_PIN              -1
#define X_ENABLE_PIN           29
#define X_MS1_PIN              40
#define X_MS2_PIN              41

#define Y_STEP_PIN             36
#define Y_DIR_PIN              49
#define Y_MIN_PIN              11
#define Y_MAX_PIN              -1
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

#define HEATER_BED_PIN          4 //PG5
#define TEMP_BED_PIN            2 //A2

#define HEATER_0_PIN            3 //PE5
#define TEMP_0_PIN              0 //A0

#define HEATER_1_PIN           -1
#define TEMP_1_PIN             -1 //A1

#define HEATER_2_PIN           -1
#define TEMP_2_PIN             -1

#define TEMP_AMBIENT_PIN        6 //A6

#define TEMP_PINDA_PIN          1 //A1



#define E0_STEP_PIN            34
#define E0_DIR_PIN             43
#define E0_ENABLE_PIN          26
#define E0_MS1_PIN             65
#define E0_MS2_PIN             66

#ifdef SNMM 
  #define E_MUX0_PIN 17
  #define E_MUX1_PIN 16
#endif


#define MOTOR_CURRENT_PWM_XY_PIN 46
#define MOTOR_CURRENT_PWM_Z_PIN  45
#define MOTOR_CURRENT_PWM_E_PIN  44
#define SDPOWER             -1
#define SDSS                53
#define LED_PIN             13
#define FAN_PIN              6
#define FAN_1_PIN           -1
#define PS_ON_PIN           -1
#define KILL_PIN            -1  // 80 with Smart Controller LCD
#define SUICIDE_PIN         -1  // PIN that has to be turned on right after start, to keep power flowing.
#define TACH_0				30	// noctua extruder fan


//#define KILL_PIN            32


#define BEEPER              84  // Beeper on AUX-4
#define LCD_PINS_RS         82
#define LCD_PINS_ENABLE     18
#define LCD_PINS_D4	        19
#define LCD_PINS_D5         70
#define LCD_PINS_D6         85
#define LCD_PINS_D7         71

//buttons are directly attached using AUX-2
#define BTN_EN1                72
#define BTN_EN2                14
#define BTN_ENC                 9  // the click

#define SDCARDDETECT           15

#define IR_SENSOR_PIN 20 //idler sensor

// Support for an 8 bit logic analyzer, for example the Saleae.
// Channels 0-2 are fast, they could generate 2.667Mhz waveform with a software loop.
#define LOGIC_ANALYZER_CH0		X_MIN_PIN		// PB6
#define LOGIC_ANALYZER_CH1		Y_MIN_PIN		// PB5
#define LOGIC_ANALYZER_CH2		53				// PB0 (PROC_nCS)
// Channels 3-7 are slow, they could generate
// 0.889Mhz waveform with a software loop and interrupt locking,
// 1.333MHz waveform without interrupt locking.
#define LOGIC_ANALYZER_CH3 		73				// PJ3
// PK0 has no Arduino digital pin assigned, so we set it directly.
#define WRITE_LOGIC_ANALYZER_CH4(value) if (value) PORTK |= (1 << 0); else PORTK &= ~(1 << 0) // PK0
#define LOGIC_ANALYZER_CH5		16				// PH0 (RXD2)
#define LOGIC_ANALYZER_CH6		17				// PH1 (TXD2)
#define LOGIC_ANALYZER_CH7 		76				// PJ5

#define LOGIC_ANALYZER_CH0_ENABLE SET_OUTPUT(LOGIC_ANALYZER_CH0)
#define LOGIC_ANALYZER_CH1_ENABLE SET_OUTPUT(LOGIC_ANALYZER_CH1)
#define LOGIC_ANALYZER_CH2_ENABLE SET_OUTPUT(LOGIC_ANALYZER_CH2)
#define LOGIC_ANALYZER_CH3_ENABLE SET_OUTPUT(LOGIC_ANALYZER_CH3)
#define LOGIC_ANALYZER_CH4_ENABLE do { DDRK |= 1 << 0; } while (0)
#define LOGIC_ANALYZER_CH5_ENABLE do { cbi(UCSR2B, TXEN2); cbi(UCSR2B, RXEN2); cbi(UCSR2B, RXCIE2); SET_OUTPUT(LOGIC_ANALYZER_CH5); } while (0)
#define LOGIC_ANALYZER_CH6_ENABLE do { cbi(UCSR2B, TXEN2); cbi(UCSR2B, RXEN2); cbi(UCSR2B, RXCIE2); SET_OUTPUT(LOGIC_ANALYZER_CH6); } while (0)
#define LOGIC_ANALYZER_CH7_ENABLE SET_OUTPUT(LOGIC_ANALYZER_CH7)

