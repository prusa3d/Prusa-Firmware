#ifndef CONFIGURATION_PRUSA_H
#define CONFIGURATION_PRUSA_H

/*------------------------------------
GENERAL SETTINGS
*------------------------------------*/

// Printer revision
#define FILAMENT_SIZE "1_75mm_MK2"
#define NOZZLE_TYPE "E3Dv6full"

// Developer flag
//#define DEVELOPER

// Printer name
#define CUSTOM_MENDEL_NAME "Prusa i3 MK2"

// Electronics
//#define MOTHERBOARD BOARD_RAMBO_MINI_1_3
#define MOTHERBOARD BOARD_RAMBO_EINSY_0 

// Prusa Single extruder multiple material suport
#define SNMM

// Uncomment the below for the E3D PT100 temperature sensor (with or without PT100 Amplifier)
#define E3D_PT100_EXTRUDER_WITH_AMP
//#define E3D_PT100_EXTRUDER_NO_AMP
//#define E3D_PT100_BED_WITH_AMP
//#define E3D_PT100_BED_NO_AMP


// Linear Advance feature - EXPERIMENTAL!
/**
 * Implementation of linear pressure control
 *
 * Assumption: advance = k * (delta velocity)
 * K=0 means advance disabled.
 * See Marlin documentation for calibration instructions.
 */
#define LIN_ADVANCE

#if defined(LIN_ADVANCE)
  #define LIN_ADVANCE_K 75

  /**
   * Some Slicers produce Gcode with randomly jumping extrusion widths occasionally.
   * For example within a 0.4mm perimeter it may produce a single segment of 0.05mm width.
   * While this is harmless for normal printing (the fluid nature of the filament will
   * close this very, very tiny gap), it throws off the LIN_ADVANCE pressure adaption.
   *
   * For this case LIN_ADVANCE_E_D_RATIO can be used to set the extrusion:distance ratio
   * to a fixed value. Note that using a fixed ratio will lead to wrong nozzle pressures
   * if the slicer is using variable widths or layer heights within one print!
   *
   * This option sets the default E:D ratio at startup. Use `M900` to override this value.
   *
   * Example: `M900 W0.4 H0.2 D1.75`, where:
   *   - W is the extrusion width in mm
   *   - H is the layer height in mm
   *   - D is the filament diameter in mm
   *
   * Example: `M900 R0.0458` to set the ratio directly.
   *
   * Set to 0 to auto-detect the ratio based on given Gcode G1 print moves.
   *
   * Slic3r (including Prusa Slic3r) produces Gcode compatible with the automatic mode.
   * Cura (as of this writing) may produce Gcode incompatible with the automatic mode.
   */
  #define LIN_ADVANCE_E_D_RATIO 0.033260135 // The calculated ratio (or 0) according to the formula W * H / ((D / 2) ^ 2 * PI)
                                  // Example: 0.4 * 0.2 / ((1.75 / 2) ^ 2 * PI) = 0.033260135
#endif



/*------------------------------------
AXIS SETTINGS
*------------------------------------*/

// Steps per unit {X,Y,Z,E}
#ifdef SNMM
#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200/8,140}
#else
#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200/8,161.3}
#endif


// Endstop inverting
const bool X_MIN_ENDSTOP_INVERTING = false; // set to true to invert the logic of the endstop.
const bool Y_MIN_ENDSTOP_INVERTING = false; // set to true to invert the logic of the endstop.
const bool Z_MIN_ENDSTOP_INVERTING = false; // set to true to invert the logic of the endstop.

// Home position
#define MANUAL_X_HOME_POS 0
#define MANUAL_Y_HOME_POS -2.2
#define MANUAL_Z_HOME_POS 0.15

// Travel limits after homing
#define X_MAX_POS 250
#define X_MIN_POS 0
#define Y_MAX_POS 210
#define Y_MIN_POS -2.2
#define Z_MAX_POS 210
#define Z_MIN_POS 0.15

// Canceled home position
#define X_CANCEL_POS 50
#define Y_CANCEL_POS 190

//Pause print position
#define X_PAUSE_POS 50
#define Y_PAUSE_POS 190
#define Z_PAUSE_LIFT 20

#define NUM_AXIS 4 // The axis order in all axis related arrays is X, Y, Z, E
#define HOMING_FEEDRATE {3000, 3000, 800, 0}  // set the homing speeds (mm/min)

#define DEFAULT_MAX_FEEDRATE          {500, 500, 12, 120}    // (mm/sec)
#define DEFAULT_MAX_ACCELERATION      {9000,9000,500,10000}    // X, Y, Z, E maximum start speed for accelerated moves. E default values are good for Skeinforge 40+, for older versions raise them a lot.

#define DEFAULT_ACCELERATION          1500    // X, Y, Z and E max acceleration in mm/s^2 for printing moves
#define DEFAULT_RETRACT_ACCELERATION  1500   // X, Y, Z and E max acceleration in mm/s^2 for retracts


#define MANUAL_FEEDRATE {3000, 3000, 1000, 100}   // set the speeds for manual moves (mm/min)

#define Z_AXIS_ALWAYS_ON 1

/*------------------------------------
EXTRUDER SETTINGS
*------------------------------------*/

// Mintemps
#define HEATER_0_MINTEMP 15
#define HEATER_1_MINTEMP 5
#define HEATER_2_MINTEMP 5
#define BED_MINTEMP 15

// Maxtemps
#if defined(E3D_PT100_EXTRUDER_WITH_AMP) || defined(E3D_PT100_EXTRUDER_NO_AMP)
#define HEATER_0_MAXTEMP 410
#else
#define HEATER_0_MAXTEMP 305
#endif
#define HEATER_1_MAXTEMP 305
#define HEATER_2_MAXTEMP 305
#define BED_MAXTEMP 150

#if defined(E3D_PT100_EXTRUDER_WITH_AMP) || defined(E3D_PT100_EXTRUDER_NO_AMP)
// Define PID constants for extruder with PT100
#define  DEFAULT_Kp 21.70
#define  DEFAULT_Ki 1.60
#define  DEFAULT_Kd 73.76
#else
// Define PID constants for extruder
#define  DEFAULT_Kp 40.925
#define  DEFAULT_Ki 4.875
#define  DEFAULT_Kd 86.085
#endif

// Extrude mintemp
#define EXTRUDE_MINTEMP 130

// Extruder cooling fans
#define EXTRUDER_0_AUTO_FAN_PIN   8
#define EXTRUDER_1_AUTO_FAN_PIN   -1
#define EXTRUDER_2_AUTO_FAN_PIN   -1
#define EXTRUDER_AUTO_FAN_TEMPERATURE 50
#define EXTRUDER_AUTO_FAN_SPEED   255  // == full speed






#ifdef SNMM
//#define BOWDEN_LENGTH	408
#define BOWDEN_LENGTH 433 //default total length for filament fast loading part; max length for extrusion is 465 mm!; this length can be adjusted in service menu
#define FIL_LOAD_LENGTH 102 //length for loading filament into the nozzle
#define FIL_COOLING 10 //length for cooling moves
#define E_MOTOR_LOW_CURRENT 350 // current for PRUSAY code
#define E_MOTOR_HIGH_CURRENT 700 //current for unloading filament, stop print, PRUSAY ramming
#endif //SNMM

#define DIS //for measuring bed heigth and PINDa detection heigth relative to auto home point, experimental function


/*------------------------------------
CHANGE FILAMENT SETTINGS
*------------------------------------*/

// Filament change configuration
#define FILAMENTCHANGEENABLE
#ifdef FILAMENTCHANGEENABLE
#define FILAMENTCHANGE_XPOS 211
#define FILAMENTCHANGE_YPOS 0
#define FILAMENTCHANGE_ZADD 2
#define FILAMENTCHANGE_FIRSTRETRACT -2
#define FILAMENTCHANGE_FINALRETRACT -80

#define FILAMENTCHANGE_FIRSTFEED 70
#define FILAMENTCHANGE_FINALFEED 50
#define FILAMENTCHANGE_RECFEED 5

#define FILAMENTCHANGE_XYFEED 50
#define FILAMENTCHANGE_EFEED 20
#define FILAMENTCHANGE_RFEED 400
#define FILAMENTCHANGE_EXFEED 2
#define FILAMENTCHANGE_ZFEED 15

#endif

/*------------------------------------
ADDITIONAL FEATURES SETTINGS
*------------------------------------*/

// Define Prusa filament runout sensor
//#define FILAMENT_RUNOUT_SUPPORT

#ifdef FILAMENT_RUNOUT_SUPPORT
#define FILAMENT_RUNOUT_SENSOR 1
#endif

// temperature runaway
#define TEMP_RUNAWAY_BED_HYSTERESIS 5
#define TEMP_RUNAWAY_BED_TIMEOUT 360

#define TEMP_RUNAWAY_EXTRUDER_HYSTERESIS 15
#define TEMP_RUNAWAY_EXTRUDER_TIMEOUT 45

// @section TMC2130

/**
 * Enable this for SilentStepStick Trinamic TMC2130 SPI-configurable stepper drivers.
 *
 * You'll also need the TMC2130Stepper Arduino library
 * (https://github.com/teemuatlut/TMC2130Stepper).
 *
 * To use TMC2130 stepper drivers in SPI mode connect your SPI2130 pins to
 * the hardware SPI interface on your board and define the required CS pins
 * in your `pins_MYBOARD.h` file. (e.g., RAMPS 1.4 uses AUX3 pins `X_CS_PIN 53`, `Y_CS_PIN 49`, etc.).
 */
#define HAVE_TMC2130

#if defined(HAVE_TMC2130)

  // CHOOSE YOUR MOTORS HERE, THIS IS MANDATORY
  #define X_IS_TMC2130
  #define Y_IS_TMC2130
  #define Z_IS_TMC2130
  #define E0_IS_TMC2130

  /**
   * Stepper driver settings
   */

  #define R_SENSE           0.10  // R_sense resistor for SilentStepStick2130
  #define HOLD_MULTIPLIER    0.5  // Scales down the holding current from run current
  #define INTERPOLATE          1  // Interpolate X/Y/Z_MICROSTEPS to 256

  #define X_CURRENT         1000  // rms current in mA. Multiply by 1.41 for peak current.
  #define X_MICROSTEPS        16  // 0..256

  #define Y_CURRENT         1000
  #define Y_MICROSTEPS        16

  #define Z_CURRENT         1000
  #define Z_MICROSTEPS        16

  #define E0_CURRENT      1000
  #define E0_MICROSTEPS     16

  /**
   * Use Trinamic's ultra quiet stepping mode.
   * When disabled, Marlin will use spreadCycle stepping mode.
   */
  #define STEALTHCHOP

  /**
   * Let Marlin automatically control stepper current.
   * This is still an experimental feature.
   * Increase current every 5s by CURRENT_STEP until stepper temperature prewarn gets triggered,
   * then decrease current by CURRENT_STEP until temperature prewarn is cleared.
   * Adjusting starts from X/Y/Z/E_CURRENT but will not increase over AUTO_ADJUST_MAX
   * Relevant g-codes:
   * M906 - Set or get motor current in milliamps using axis codes X, Y, Z, E. Report values if no axis codes given.
   * M906 S1 - Start adjusting current
   * M906 S0 - Stop adjusting current
   * M911 - Report stepper driver overtemperature pre-warn condition.
   * M912 - Clear stepper driver overtemperature pre-warn condition flag.
   */
  #define AUTOMATIC_CURRENT_CONTROL

  #if defined(AUTOMATIC_CURRENT_CONTROL)
    #define CURRENT_STEP          50  // [mA]
    #define AUTO_ADJUST_MAX     1300  // [mA], 1300mA_rms = 1840mA_peak
    #define REPORT_CURRENT_CHANGE
  #endif

  /**
   * The driver will switch to spreadCycle when stepper speed is over HYBRID_THRESHOLD.
   * This mode allows for faster movements at the expense of higher noise levels.
   * STEALTHCHOP needs to be enabled.
   * M913 X/Y/Z/E to live tune the setting
   */
  #define HYBRID_THRESHOLD

  #define X_HYBRID_THRESHOLD     100  // [mm/s]
  #define Y_HYBRID_THRESHOLD     100
  #define Z_HYBRID_THRESHOLD       4
  #define E0_HYBRID_THRESHOLD     30

  /**
   * Use stallGuard2 to sense an obstacle and trigger an endstop.
   * You need to place a wire from the driver's DIAG1 pin to the X/Y endstop pin.
   * If used along with STEALTHCHOP, the movement will be louder when homing. This is normal.
   *
   * X/Y_HOMING_SENSITIVITY is used for tuning the trigger sensitivity.
   * Higher values make the system LESS sensitive.
   * Lower value make the system MORE sensitive.
   * Too low values can lead to false positives, while too high values will collide the axis without triggering.
   * It is advised to set X/Y_HOME_BUMP_MM to 0.
   * M914 X/Y to live tune the setting
   */
  //#define SENSORLESS_HOMING

  #if defined(SENSORLESS_HOMING)
    #define X_HOMING_SENSITIVITY  19
    #define Y_HOMING_SENSITIVITY  19
  #endif

  /**
   * You can set your own advanced settings by filling in predefined functions.
   * A list of available functions can be found on the library github page
   * https://github.com/teemuatlut/TMC2130Stepper
   *
   * Example:
   * #define TMC2130_ADV() { \
   *   stepperX.diag0_temp_prewarn(1); \
   *   stepperX.interpolate(0); \
   * }
   */
  #define  TMC2130_ADV() {  }

#endif // ENABLED(HAVE_TMC2130)

/*------------------------------------
MOTOR CURRENT SETTINGS
*------------------------------------*/

// Motor Current setting for BIG RAMBo
#define DIGIPOT_MOTOR_CURRENT {135,135,135,135,135} // Values 0-255 (RAMBO 135 = ~0.75A, 185 = ~1A)
#define DIGIPOT_MOTOR_CURRENT_LOUD {135,135,135,135,135}

// Motor Current settings for RAMBo mini PWM value = MotorCurrentSetting * 255 / range
#if MOTHERBOARD == 102 || MOTHERBOARD == 302 
	#define MOTOR_CURRENT_PWM_RANGE 2000
	#define DEFAULT_PWM_MOTOR_CURRENT  {270, 830, 450} // {XY,Z,E}
	#define DEFAULT_PWM_MOTOR_CURRENT_LOUD  {540, 830, 500} // {XY,Z,E}

#elif MOTHERBOARD == 304
	#define MOTOR_CURRENT_PWM_RANGE 2000
	#define DEFAULT_PWM_MOTOR_CURRENT  {1000, 1100, 1100}
	#define DEFAULT_PWM_MOTOR_CURRENT_LOUD  {1000, 1100, 1100}
#endif

/*------------------------------------
BED SETTINGS
*------------------------------------*/

// Define Mesh Bed Leveling system to enable it
#define MESH_BED_LEVELING
#ifdef MESH_BED_LEVELING

#define MBL_Z_STEP 0.01

// Mesh definitions
#define MESH_MIN_X 35
#define MESH_MAX_X 238
#define MESH_MIN_Y 6
#define MESH_MAX_Y 202

// Mesh upsample definition
#define MESH_NUM_X_POINTS 7
#define MESH_NUM_Y_POINTS 7
// Mesh measure definition
#define MESH_MEAS_NUM_X_POINTS 3
#define MESH_MEAS_NUM_Y_POINTS 3

#define MESH_HOME_Z_CALIB 0.2
#define MESH_HOME_Z_SEARCH 5 //Z lift for homing, mesh bed leveling etc.

#define X_PROBE_OFFSET_FROM_EXTRUDER 23     // Z probe to nozzle X offset: -left  +right
#define Y_PROBE_OFFSET_FROM_EXTRUDER 9     // Z probe to nozzle Y offset: -front +behind
#define Z_PROBE_OFFSET_FROM_EXTRUDER -0.4  // Z probe to nozzle Z offset: -below (always!)
#endif

// Bed Temperature Control
// Select PID or bang-bang with PIDTEMPBED. If bang-bang, BED_LIMIT_SWITCHING will enable hysteresis
//
// Uncomment this to enable PID on the bed. It uses the same frequency PWM as the extruder.
// If your PID_dT above is the default, and correct for your hardware/configuration, that means 7.689Hz,
// which is fine for driving a square wave into a resistive load and does not significantly impact you FET heating.
// This also works fine on a Fotek SSR-10DA Solid State Relay into a 250W heater.
// If your configuration is significantly different than this and you don't understand the issues involved, you probably
// shouldn't use bed PID until someone else verifies your hardware works.
// If this is enabled, find your own PID constants below.
#define PIDTEMPBED
//
//#define BED_LIMIT_SWITCHING

// This sets the max power delivered to the bed, and replaces the HEATER_BED_DUTY_CYCLE_DIVIDER option.
// all forms of bed control obey this (PID, bang-bang, bang-bang with hysteresis)
// setting this to anything other than 255 enables a form of PWM to the bed just like HEATER_BED_DUTY_CYCLE_DIVIDER did,
// so you shouldn't use it unless you are OK with PWM on your bed.  (see the comment on enabling PIDTEMPBED)
#define MAX_BED_POWER 255 // limits duty cycle to bed; 255=full current

// Bed temperature compensation settings
#define BED_OFFSET 10
#define BED_OFFSET_START 40
#define BED_OFFSET_CENTER 50


#ifdef PIDTEMPBED
//120v 250W silicone heater into 4mm borosilicate (MendelMax 1.5+)
//from FOPDT model - kp=.39 Tp=405 Tdead=66, Tc set to 79.2, aggressive factor of .15 (vs .1, 1, 10)
#if defined(E3D_PT100_BED_WITH_AMP) || defined(E3D_PT100_BED_NO_AMP)
// Define PID constants for extruder with PT100
#define  DEFAULT_bedKp 21.70
#define  DEFAULT_bedKi 1.60
#define  DEFAULT_bedKd 73.76
#else
#define  DEFAULT_bedKp 126.13
#define  DEFAULT_bedKi 4.30
#define  DEFAULT_bedKd 924.76
#endif

//120v 250W silicone heater into 4mm borosilicate (MendelMax 1.5+)
//from pidautotune
//    #define  DEFAULT_bedKp 97.1
//    #define  DEFAULT_bedKi 1.41
//    #define  DEFAULT_bedKd 1675.16

// FIND YOUR OWN: "M303 E-1 C8 S90" to run autotune on the bed at 90 degreesC for 8 cycles.
#endif // PIDTEMPBED


/*-----------------------------------
PREHEAT SETTINGS
*------------------------------------*/

#define PLA_PREHEAT_HOTEND_TEMP 215
#define PLA_PREHEAT_HPB_TEMP 55
#define PLA_PREHEAT_FAN_SPEED 0  

#define ABS_PREHEAT_HOTEND_TEMP 255
#define ABS_PREHEAT_HPB_TEMP 100
#define ABS_PREHEAT_FAN_SPEED 0 

#define HIPS_PREHEAT_HOTEND_TEMP 220
#define HIPS_PREHEAT_HPB_TEMP 100
#define HIPS_PREHEAT_FAN_SPEED 0 

#define PP_PREHEAT_HOTEND_TEMP 254
#define PP_PREHEAT_HPB_TEMP 100
#define PP_PREHEAT_FAN_SPEED 0 

#define PET_PREHEAT_HOTEND_TEMP 240
#define PET_PREHEAT_HPB_TEMP 90
#define PET_PREHEAT_FAN_SPEED 0 

#define FLEX_PREHEAT_HOTEND_TEMP 230
#define FLEX_PREHEAT_HPB_TEMP 50
#define FLEX_PREHEAT_FAN_SPEED 0 

/*------------------------------------
THERMISTORS SETTINGS
*------------------------------------*/

//
//--NORMAL IS 4.7kohm PULLUP!-- 1kohm pullup can be used on hotend sensor, using correct resistor and table
//
//// Temperature sensor settings:
// -2 is thermocouple with MAX6675 (only for sensor 0)
// -1 is thermocouple with AD595
// 0 is not used
// 1 is 100k thermistor - best choice for EPCOS 100k (4.7k pullup)
// 2 is 200k thermistor - ATC Semitec 204GT-2 (4.7k pullup)
// 3 is Mendel-parts thermistor (4.7k pullup)
// 4 is 10k thermistor !! do not use it for a hotend. It gives bad resolution at high temp. !!
// 5 is 100K thermistor - ATC Semitec 104GT-2 (Used in ParCan & J-Head) (4.7k pullup)
// 6 is 100k EPCOS - Not as accurate as table 1 (created using a fluke thermocouple) (4.7k pullup)
// 7 is 100k Honeywell thermistor 135-104LAG-J01 (4.7k pullup)
// 71 is 100k Honeywell thermistor 135-104LAF-J01 (4.7k pullup)
// 8 is 100k 0603 SMD Vishay NTCS0603E3104FXT (4.7k pullup)
// 9 is 100k GE Sensing AL03006-58.2K-97-G1 (4.7k pullup)
// 10 is 100k RS thermistor 198-961 (4.7k pullup)
// 11 is 100k beta 3950 1% thermistor (4.7k pullup)
// 12 is 100k 0603 SMD Vishay NTCS0603E3104FXT (4.7k pullup) (calibrated for Makibox hot bed)
// 13 is 100k Hisens 3950  1% up to 300Â°C for hotend "Simple ONE " & "Hotend "All In ONE" 
// 20 is the PT100 circuit found in the Ultimainboard V2.x
// 60 is 100k Maker's Tool Works Kapton Bed Thermistor beta=3950
//
//    1k ohm pullup tables - This is not normal, you would have to have changed out your 4.7k for 1k
//                          (but gives greater accuracy and more stable PID)
// 51 is 100k thermistor - EPCOS (1k pullup)
// 52 is 200k thermistor - ATC Semitec 204GT-2 (1k pullup)
// 55 is 100k thermistor - ATC Semitec 104GT-2 (Used in ParCan & J-Head) (1k pullup)
//
// 1047 is Pt1000 with 4k7 pullup
// 1010 is Pt1000 with 1k pullup (non standard)
// 147 is Pt100 with 4k7 pullup
// 148 is E3D Pt100 with 4k7 pullup and no PT100 Amplifier on a MiniRambo 1.3a
// 247 is Pt100 with 4k7 pullup and PT100 Amplifier
// 110 is Pt100 with 1k pullup (non standard)

#if defined(E3D_PT100_EXTRUDER_WITH_AMP)
#define TEMP_SENSOR_0 247
#elif defined(E3D_PT100_EXTRUDER_NO_AMP)
#define TEMP_SENSOR_0 148
#else
#define TEMP_SENSOR_0 5
#endif
#define TEMP_SENSOR_1 0
#define TEMP_SENSOR_2 0
#if defined(E3D_PT100_BED_WITH_AMP)
#define TEMP_SENSOR_BED 247
#elif defined(E3D_PT100_BED_NO_AMP)
#define TEMP_SENSOR_BED 148
#else
#define TEMP_SENSOR_BED 1
#endif

#define STACK_GUARD_TEST_VALUE 0xA2A2

#define MAX_BED_TEMP_CALIBRATION 50
#define MAX_HOTEND_TEMP_CALIBRATION 50

#define MAX_E_STEPS_PER_UNIT 250
#define MIN_E_STEPS_PER_UNIT 100

#define Z_BABYSTEP_MIN -3999
#define Z_BABYSTEP_MAX 0

#define PINDA_PREHEAT_X 70
#define PINDA_PREHEAT_Y -3
#define PINDA_PREHEAT_Z 1
#define PINDA_HEAT_T 120 //time in s

#define PINDA_MIN_T 50
#define PINDA_STEP_T 10
#define PINDA_MAX_T 100

#define PING_TIME 60 //time in s
#define PING_TIME_LONG 600 //10 min; used when length of commands buffer > 0 to avoid false triggering when dealing with long gcodes
#define PING_ALLERT_PERIOD 60 //time in s

#define LONG_PRESS_TIME 1000 //time in ms for button long press 
#define BUTTON_BLANKING_TIME 200 //time in ms for blanking after button release

#define PAUSE_RETRACT 1 

#define DEFAULT_PID_TEMP 210

#define DEFAULT_RETRACTION 1 //used for PINDA temp calibration

#endif //__CONFIGURATION_PRUSA_H

