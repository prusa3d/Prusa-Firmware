#ifndef CONFIGURATION_PRUSA_H
#define CONFIGURATION_PRUSA_H

#include <limits.h>
#include "printers.h"
/*------------------------------------
 GENERAL SETTINGS
 *------------------------------------*/

// Printer revision
#define PRINTER_TYPE PRINTER_MK3S
#define PRINTER_NAME PRINTER_MK3S_NAME
#define PRINTER_NAME_ALTERNATE PRINTER_MK3_NAME //the other similar printer to this.
#define PRINTER_MMU_TYPE PRINTER_MK3S_MMU3
#define PRINTER_MMU_NAME PRINTER_MK3S_MMU3_NAME
#define FILAMENT_SIZE "1_75mm_MK3S"
#define NOZZLE_TYPE "E3DREVO_HF_60W"

// Printer name
#define CUSTOM_MENDEL_NAME "Prusa MK3S+RHF60"

// Electronics
#define MOTHERBOARD BOARD_EINSY_1_0a
#define STEEL_SHEET
#define HAS_SECOND_SERIAL_PORT

// PSU
// #define PSU_Delta                                 // uncomment if DeltaElectronics PSU installed


// Uncomment the below for the E3D PT100 temperature sensor (with or without PT100 Amplifier)
//#define E3D_PT100_EXTRUDER_WITH_AMP
//#define E3D_PT100_EXTRUDER_NO_AMP
//#define E3D_PT100_BED_WITH_AMP
//#define E3D_PT100_BED_NO_AMP


/*------------------------------------
 AXIS SETTINGS
 *------------------------------------*/

// Steps per unit {X,Y,Z,E}
#define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,3200/8,280}

// Endstop inverting
#define X_MIN_ENDSTOP_INVERTING 0 // set to 1 to invert the logic of the endstop.
#define Y_MIN_ENDSTOP_INVERTING 0 // set to 1 to invert the logic of the endstop.
#define Z_MIN_ENDSTOP_INVERTING 0 // set to 1 to invert the logic of the endstop.

// Direction inverting
#define INVERT_X_DIR 1    // for Mendel set to 0, for Orca set to 1
#define INVERT_Y_DIR 0    // for Mendel set to 1, for Orca set to 0
#define INVERT_Z_DIR 1     // for Mendel set to 0, for Orca set to 1
#define INVERT_E0_DIR 0   // for direct drive extruder v9 set to 1, for geared extruder set to 0

// Home position
#define MANUAL_X_HOME_POS 0
#define MANUAL_Y_HOME_POS -2.2
#define MANUAL_Z_HOME_POS 0.2

// Travel limits after homing
#define X_MAX_POS 255
#define X_MIN_POS 0
#define Y_MAX_POS 212.5
#define Y_MIN_POS -4 //orig -4
#define Z_MAX_POS 210
#define Z_MIN_POS 0.15

// Canceled home position
#define X_CANCEL_POS 50
#define Y_CANCEL_POS 190
#define Z_CANCEL_LIFT 50

//Pause print position
#define X_PAUSE_POS 50
#define Y_PAUSE_POS 190
#define Z_PAUSE_LIFT 20

#define NUM_AXIS 4 // The axis order in all axis related arrays is X, Y, Z, E
#define HOMING_FEEDRATE {3000, 3000, 800, 0}  // set the homing speeds (mm/min) // 3000 is also valid for stallGuard homing. Valid range: 2200 - 3000

//#define DEFAULT_Y_OFFSET    4.f // Default distance of Y_MIN_POS point from endstop, when the printer is not calibrated.
/**
 * [0,0] steel sheet print area point X coordinate in bed print area coordinates
 */
#define SHEET_PRINT_ZERO_REF_X 0.f
/**
 * [0,0] steel sheet print area point Y coordinate in bed print area coordinates
 */
#define SHEET_PRINT_ZERO_REF_Y -2.f

#define DEFAULT_MAX_FEEDRATE                {200, 200, 12, 120}      // (mm/sec)   max feedrate (M203)
#define DEFAULT_MAX_FEEDRATE_SILENT         {100, 100, 12, 120}      // (mm/sec)   max feedrate (M203), silent mode

#define DEFAULT_MAX_ACCELERATION            {1000, 1000, 200, 5000}  // (mm/sec^2) max acceleration (M201)
#define DEFAULT_MAX_ACCELERATION_SILENT     {960, 960, 200, 5000}    // (mm/sec^2) max acceleration (M201), silent mode


#define DEFAULT_ACCELERATION          1250   // X, Y, Z and E max acceleration in mm/s^2 for printing moves (M204P)
#define DEFAULT_RETRACT_ACCELERATION  1250   // X, Y, Z and E max acceleration in mm/s^2 for retracts (M204R)
#define DEFAULT_TRAVEL_ACCELERATION   1250   // X, Y, Z and E max acceleration in mm/s^2 for travels (M204T)

#define MANUAL_FEEDRATE {2700, 2700, 1000, 100}   // set the speeds for manual moves (mm/min)

//Silent mode limits
#define SILENT_MAX_ACCEL_XY      960ul  // max acceleration in silent mode in mm/s^2
#define SILENT_MAX_FEEDRATE_XY   100  // max feedrate in mm/s

//Normal mode limits
#define NORMAL_MAX_ACCEL_XY     2500ul  // max acceleration in normal mode in mm/s^2
#define NORMAL_MAX_FEEDRATE_XY   200  // max feedrate in mm/s

//number of bytes from end of the file to start check
#define END_FILE_SECTION 30720

#define Z_AXIS_ALWAYS_ON 1

//Crash detection
#define CRASHDET_TIMER 45 //seconds
#define CRASHDET_COUNTER_MAX 3 

// New XYZ calibration
#define NEW_XYZCAL

// Watchdog support
#define WATCHDOG

// Power panic
#define UVLO_SUPPORT

// Fan check
#define FANCHECK

// Safety timer
#define SAFETYTIMER
#define DEFAULT_SAFETYTIMER_TIME_MINS 30

// Offline crash dumper
#define XFLASH_DUMP     // enable dump functionality (including D20/D21/D22)
#define MENU_DUMP       // enable "Memory dump" in Settings menu
#define EMERGENCY_DUMP  // trigger crash on stack corruption and WDR

// Online crash dumper
//#define EMERGENCY_SERIAL_DUMP   // Request dump via serial on stack corruption and WDR
//#define MENU_SERIAL_DUMP        // Enable "Memory dump" in Settings menu

// Filament sensor
#define FILAMENT_SENSOR
#define FILAMENT_SENSOR_TYPE FSENSOR_IR_ANALOG
#define FSENSOR_PROBING

// Backlash - 
//#define BACKLASH_X
//#define BACKLASH_Y


// Minimum ambient temperature limit to start triggering MINTEMP errors [C]
// this value is litlebit higher that real limit, because ambient termistor is on the board and is temperated from it,
// temperature inside the case is around 31C for ambient temperature 25C, when the printer is powered on long time and idle
// the real limit is 15C (same as MINTEMP limit), this is because 15C is end of scale for both used thermistors (bed, heater)
#define MINTEMP_MINAMBIENT      10
#define MINTEMP_MINAMBIENT_RAW  1002

#define DEBUG_DCODE2
#define DEBUG_DCODE3
#define DEBUG_DCODE6

//#define DEBUG_PULLUP_CRASH //Test Pullup crash

//#define DEBUG_EEPROM_CHANGES //Uses +1188 bytes Flash +6 bytes SRAM
//#define DEBUG_BUILD
//#define DEBUG_SEC_LANG   //secondary language debug output at startup
//#define DEBUG_XFLASH   //debug external spi flash
#ifdef DEBUG_BUILD
//#define _NO_ASM
#define DEBUG_DCODES //D codes
#define DEBUG_STACK_MONITOR        //Stack monitor in stepper ISR
//#define DEBUG_CRASHDET_COUNTERS  //Display crash-detection counters on LCD
//#define DEBUG_RESUME_PRINT       //Resume/save print debug enable 
//#define DEBUG_UVLO_AUTOMATIC_RECOVER // Power panic automatic recovery debug output 
//#define DEBUG_DISABLE_XMINLIMIT  //x min limit ignored
//#define DEBUG_DISABLE_XMAXLIMIT  //x max limit ignored
//#define DEBUG_DISABLE_YMINLIMIT  //y min limit ignored
//#define DEBUG_DISABLE_YMAXLIMIT  //y max limit ignored
//#define DEBUG_DISABLE_ZMINLIMIT  //z min limit ignored
//#define DEBUG_DISABLE_ZMAXLIMIT  //z max limit ignored
#define DEBUG_DISABLE_STARTMSGS //no startup messages 
//#define DEBUG_DISABLE_MINTEMP   //mintemp error ignored
//#define DEBUG_DISABLE_SWLIMITS  //sw limits ignored
//#define DEBUG_DISABLE_LCD_STATUS_LINE  //empty four lcd line
//#define DEBUG_DISABLE_PREVENT_EXTRUDER //cold extrusion and long extrusion allowed
//#define DEBUG_DISABLE_FORCE_SELFTEST //disable force selftest
//#define DEBUG_XSTEP_DUP_PIN 21   //duplicate x-step output to pin 21 (SCL on P3)
//#define DEBUG_YSTEP_DUP_PIN 21   //duplicate y-step output to pin 21 (SCL on P3)
//#define DEBUG_DISABLE_FANCHECK     //disable fan check (no ISR INT7, check disabled)
//#define DEBUG_DISABLE_FSENSORCHECK //disable fsensor check (no ISR INT7, check disabled)
#define DEBUG_DUMP_TO_2ND_SERIAL   //dump received characters to 2nd serial line
#define DEBUG_STEPPER_TIMER_MISSED // Stop on stepper timer overflow, beep and display a message.
#define PLANNER_DIAGNOSTICS // Show the planner queue status on printer display.
#define CMD_DIAGNOSTICS //Show cmd queue length on printer display
#endif /* DEBUG_BUILD */


#define LINEARITY_CORRECTION
#define TMC2130_LINEARITY_CORRECTION
#define TMC2130_LINEARITY_CORRECTION_XYZ
#define TMC2130_VARIABLE_RESOLUTION



/*------------------------------------
 TMC2130 default settings
 *------------------------------------*/

#define TMC2130_FCLK 12000000       // fclk = 12MHz

#define TMC2130_USTEPS_XY   16        // microstep resolution for XY axes
#define TMC2130_USTEPS_Z    16        // microstep resolution for Z axis
#define TMC2130_USTEPS_E    32        // microstep resolution for E axis
#define TMC2130_INTPOL_XY   1         // extrapolate 256 for XY axes
#define TMC2130_INTPOL_Z    1         // extrapolate 256 for Z axis
#define TMC2130_INTPOL_E    1         // extrapolate 256 for E axis
// #define ALLOW_ALL_MRES

#define TMC2130_PWM_GRAD_X  2         // PWMCONF
#define TMC2130_PWM_AMPL_X  230       // PWMCONF
#define TMC2130_PWM_AUTO_X  1         // PWMCONF
#define TMC2130_PWM_FREQ_X  2         // PWMCONF

#define TMC2130_PWM_GRAD_Y  2         // PWMCONF
#define TMC2130_PWM_AMPL_Y  235       // PWMCONF
#define TMC2130_PWM_AUTO_Y  1         // PWMCONF
#define TMC2130_PWM_FREQ_Y  2         // PWMCONF

#define TMC2130_PWM_GRAD_Z  4         // PWMCONF
#define TMC2130_PWM_AMPL_Z  200       // PWMCONF
#define TMC2130_PWM_AUTO_Z  1         // PWMCONF
#define TMC2130_PWM_FREQ_Z  2         // PWMCONF

#define TMC2130_PWM_GRAD_E  4         // PWMCONF
#define TMC2130_PWM_AMPL_E  240       // PWMCONF
#define TMC2130_PWM_AUTO_E  1         // PWMCONF
#define TMC2130_PWM_FREQ_E  2         // PWMCONF

// experimental setting for E-motor cooler operation
#define TMC2130_PWM_GRAD_Ecool  84        // PWMCONF 730mA @ 375mm/min  970mA phase peak at feedrate 900mm/min
#define TMC2130_PWM_AMPL_Ecool  43        // PWMCONF 500mA phase peak at feedrate 10 mm/min
#define TMC2130_PWM_AUTO_Ecool  0         // PWMCONF

#define TMC2130_TOFF_XYZ    3         // CHOPCONF // fchop = 27.778kHz
#define TMC2130_TOFF_E      3         // CHOPCONF // fchop = 27.778kHz
//#define TMC2130_TOFF_E      4         // CHOPCONF // fchop = 21.429kHz
//#define TMC2130_TOFF_E      5         // CHOPCONF // fchop = 17.442kHz

//#define TMC2130_STEALTH_E // Extruder stealthChop mode
//#define TMC2130_CNSTOFF_E // Extruder constant-off-time mode (similar to MK2)

//#define TMC2130_PWM_DIV   683         // PWM frequency divider (1024, 683, 512, 410)
#define TMC2130_PWM_DIV   512         // PWM frequency divider (1024, 683, 512, 410)
#define TMC2130_PWM_CLK   (2 * TMC2130_FCLK / TMC2130_PWM_DIV) // PWM frequency (23.4kHz, 35.1kHz, 46.9kHz, 58.5kHz for 12MHz fclk)

#define TMC2130_TPWMTHRS  0         // TPWMTHRS - Sets the switching speed threshold based on TSTEP from stealthChop to spreadCycle mode
#define TMC2130_TPWMTHRS_E 403      // Switch extruder from StealthChop to SpreadCycle at around 900mm/min
#define TMC2130_THIGH     0         // THIGH - unused

//#define TMC2130_TCOOLTHRS_X 450       // TCOOLTHRS - coolstep treshold
//#define TMC2130_TCOOLTHRS_Y 450       // TCOOLTHRS - coolstep treshold
#define TMC2130_TCOOLTHRS_X 430       // TCOOLTHRS - coolstep treshold
#define TMC2130_TCOOLTHRS_Y 430       // TCOOLTHRS - coolstep treshold
#define TMC2130_TCOOLTHRS_Z 500       // TCOOLTHRS - coolstep treshold
#define TMC2130_TCOOLTHRS_E 500       // TCOOLTHRS - coolstep treshold

#define TMC2130_SG_HOMING       1     // stallguard homing
#define TMC2130_SG_THRS_X       3     // stallguard sensitivity for X axis
#define TMC2130_SG_THRS_Y       3     // stallguard sensitivity for Y axis
#define TMC2130_SG_THRS_Z       4     // stallguard sensitivity for Z axis
#define TMC2130_SG_THRS_E       3     // stallguard sensitivity for E axis
#define TMC2130_SG_THRS_HOME {3, 3, TMC2130_SG_THRS_Z, TMC2130_SG_THRS_E}

//new settings is possible for vsense = 1, running current value > 31 set vsense to zero and shift both currents by 1 bit right (Z axis only)
#define TMC2130_CURRENTS_H {16, 20, 35, 30}  // default holding currents for all axes
#define TMC2130_CURRENTS_FARM 36             // E 805 mA peak for ECool/farm mode
#define TMC2130_CURRENTS_R {16, 20, 35, 30}  // default running currents for all axes
#define TMC2130_CURRENTS_R_HOME {8, 10, 20, 18}  // homing running currents for all axes

#define TMC2130_STEALTH_Z
#define TMC2130_DEDGE_STEPPING

//#define TMC2130_SERVICE_CODES_M910_M918

//#define TMC2130_DEBUG
//#define TMC2130_DEBUG_WR
//#define TMC2130_DEBUG_RD


/*------------------------------------
 EXTRUDER SETTINGS
 *------------------------------------*/

// Mintemps
#define HEATER_0_MINTEMP 10
#define HEATER_MINTEMP_DELAY 15000                // [ms] ! if changed, check maximal allowed value @ ShortTimer
#if HEATER_MINTEMP_DELAY>USHRT_MAX
#error "Check maximal allowed value @ ShortTimer (see HEATER_MINTEMP_DELAY definition)"
#endif
#define BED_MINTEMP 10
#define BED_MINTEMP_DELAY 50000                   // [ms] ! if changed, check maximal allowed value @ ShortTimer
#if BED_MINTEMP_DELAY>USHRT_MAX
#error "Check maximal allowed value @ ShortTimer (see BED_MINTEMP_DELAY definition)"
#endif
#define SUPERPINDA_SUPPORT
#define PINDA_MINTEMP 10
//#define PINDA_TEMP_COMP //Used to enable SuperPINDA toggle menu/function
#define AMBIENT_MINTEMP -30

// Maxtemps
#if defined(E3D_PT100_EXTRUDER_WITH_AMP) || defined(E3D_PT100_EXTRUDER_NO_AMP)
#define HEATER_0_MAXTEMP 410
#else
#define HEATER_0_MAXTEMP 305
#endif
#define BED_MAXTEMP 125
#define AMBIENT_MAXTEMP 100

#if defined(E3D_PT100_EXTRUDER_WITH_AMP) || defined(E3D_PT100_EXTRUDER_NO_AMP)
// Define PID constants for extruder with PT100
#define  DEFAULT_Kp 21.70
#define  DEFAULT_Ki 1.60
#define  DEFAULT_Kd 73.76
#else
// Define PID constants for E3D REVO HF 60W
#define  MAX_OVERSHOOT_PID_AUTOTUNE 30
#define  DEFAULT_Kp 15.00
#define  DEFAULT_Ki 2.9
#define  DEFAULT_Kd 19.2
#endif

// Extrude mintemp
#define EXTRUDE_MINTEMP 175

// Quick nozzle change supported
#define QUICK_NOZZLE_CHANGE

// Extruder cooling fans
#define EXTRUDER_0_AUTO_FAN_PIN   8
#define EXTRUDER_AUTO_FAN_TEMPERATURE 50
#define EXTRUDER_AUTO_FAN_SPEED   255  // == full speed
#define EXTRUDER_ALTFAN_DETECT
#define EXTRUDER_ALTFAN_SPEED_SILENT 128

#define FANCHECK_AUTO_PRINT_FAN_THRS 70 //[RPS] - Used during selftest to identify swapped fans automatically
#define FANCHECK_AUTO_FAIL_THRS 20 //[RPS] - Used during selftest to identify a faulty fan

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
#define FILAMENTCHANGE_FINALRETRACT 0

#define FILAMENTCHANGE_FIRSTFEED 70 //E distance in mm for fast filament loading sequence used used in filament change (M600)
#define FILAMENTCHANGE_FINALFEED 25 //E distance in mm for slow filament loading sequence used used in filament change (M600) and filament load (M701) 
#define FILAMENTCHANGE_RECFEED 5

#define FILAMENTCHANGE_XYFEED 50
#define FILAMENTCHANGE_EFEED_FIRST 20 // feedrate in mm/s for fast filament loading sequence used in filament change (M600)
#define FILAMENTCHANGE_EFEED_FINAL 3.3f // feedrate in mm/s for slow filament loading sequence used in filament change (M600) and filament load (M701) 
//#define FILAMENTCHANGE_RFEED 400
#define FILAMENTCHANGE_RFEED 7000 / 60
#define FILAMENTCHANGE_EXFEED 2
#define FILAMENTCHANGE_ZFEED 15

//Retract and then extrude some filament to prevent oozing.
//After the loading sequence and after a print is canceled, the filament is retracted to get it out of the heat zone of the nozzle.
//Then a small extrusion is performed to make sure the filament is close enough for the next print without oozing.
//#define COMMUNITY_PREVENT_OOZE
#ifdef COMMUNITY_PREVENT_OOZE
#define FILAMENTCHANGE_COMMUNITY_ROOZEFEED -10 //E retract distance in mm for ooze prevention
#define FILAMENTCHANGE_COMMUNITY_EOOZEFEED 4 //E extrude distance in mm for ooze prevention
#endif //End COMMUNITY_PREVENT_OOZE

#endif

/*------------------------------------
 ADDITIONAL FEATURES SETTINGS
 *------------------------------------*/

// temperature runaway
#define TEMP_RUNAWAY_BED_HYSTERESIS 5
#define TEMP_RUNAWAY_BED_TIMEOUT 360

#define TEMP_RUNAWAY_EXTRUDER_HYSTERESIS 15
#define TEMP_RUNAWAY_EXTRUDER_TIMEOUT 45

// model-based temperature check
#define THERMAL_MODEL 1              // enable model-based temperature checks
#define THERMAL_MODEL_DEBUG 1        // extended runtime logging

#define THERMAL_MODEL_CAL_C_low 5    // C estimation lower limit
#define THERMAL_MODEL_CAL_C_high 20  // C estimation upper limit
#define THERMAL_MODEL_CAL_C_thr 0.01 // C estimation iteration threshold
#define THERMAL_MODEL_CAL_C_itr 30   // C estimation iteration limit

#define THERMAL_MODEL_CAL_R_low 5    // R estimation lower limit
#define THERMAL_MODEL_CAL_R_high 50  // R estimation upper limit
#define THERMAL_MODEL_CAL_R_thr 0.01 // R estimation iteration threshold
#define THERMAL_MODEL_CAL_R_itr 30   // R estimation iteration limit

#define THERMAL_MODEL_CAL_T_low 50   // Default calibration cooling temperature (C)
#define THERMAL_MODEL_CAL_T_high 230 // Default calibration working temperature (C)

#define THERMAL_MODEL_Ta_corr -7     // Default ambient temperature correction

#include "thermal_model/e3d_REVO_HF_60W.h"
#define THERMAL_MODEL_DEFAULT E3D_REVO_HF_60W // Default E3D REVO HF 60W model parameters

/*------------------------------------
 HOST FEATURES
 *------------------------------------*/

//#define HOST_SHUTDOWN              //Host supports "//action:shutdown" feature

/*------------------------------------
 MOTOR CURRENT SETTINGS
 *------------------------------------*/

// Motor Current settings for Einsy/tmc = 0..63
#define MOTOR_CURRENT_PWM_RANGE 63

/*------------------------------------
 BED SETTINGS
 *------------------------------------*/

// Define Mesh Bed Leveling system to enable it
#define MESH_BED_LEVELING
#ifdef MESH_BED_LEVELING

#define MBL_Z_STEP 0.01

// Mesh upsample definition
#define MESH_NUM_X_POINTS 7
#define MESH_NUM_Y_POINTS 7
// Mesh measure definition
#define MESH_MEAS_NUM_X_POINTS 3
#define MESH_MEAS_NUM_Y_POINTS 3

// Maximum bed level correction value
#define BED_ADJUSTMENT_UM_MAX 100

#define MESH_HOME_Z_CALIB 0.2
#define MESH_HOME_Z_SEARCH 5.0f           // Z lift for homing, mesh bed leveling etc.

#define X_PROBE_OFFSET_FROM_EXTRUDER 23     // Z probe to nozzle X offset: -left  +right
#define Y_PROBE_OFFSET_FROM_EXTRUDER 5     // Z probe to nozzle Y offset: -front +behind
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
#define PLA_PREHEAT_HPB_TEMP 60

#define PVB_PREHEAT_HOTEND_TEMP 215
#define PVB_PREHEAT_HPB_TEMP 75

#define ASA_PREHEAT_HOTEND_TEMP 260
#define ASA_PREHEAT_HPB_TEMP 105

#define PC_PREHEAT_HOTEND_TEMP 275
#define PC_PREHEAT_HPB_TEMP 110

#define PA_PREHEAT_HOTEND_TEMP 275
#define PA_PREHEAT_HPB_TEMP 90

#define ABS_PREHEAT_HOTEND_TEMP 255
#define ABS_PREHEAT_HPB_TEMP 100

#define HIPS_PREHEAT_HOTEND_TEMP 220
#define HIPS_PREHEAT_HPB_TEMP 100

#define PP_PREHEAT_HOTEND_TEMP 254
#define PP_PREHEAT_HPB_TEMP 100

#define PET_PREHEAT_HOTEND_TEMP 230
#define PET_PREHEAT_HPB_TEMP 85

#define FLEX_PREHEAT_HOTEND_TEMP 240
#define FLEX_PREHEAT_HPB_TEMP 50

#define LCD_JUMP_HOTEND_TEMP 200
#define LCD_JUMP_BED_TEMP 50
#define LCD_JUMP_FAN_SPEED 127

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
// 13 is 100k Hisens 3950  1% up to 300°C for hotend "Simple ONE " & "Hotend "All In ONE"
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
#if defined(E3D_PT100_BED_WITH_AMP)
#define TEMP_SENSOR_BED 247
#elif defined(E3D_PT100_BED_NO_AMP)
#define TEMP_SENSOR_BED 148
#else
#define TEMP_SENSOR_BED 1
#endif
#define TEMP_SENSOR_PINDA 1
#define TEMP_SENSOR_AMBIENT 2000

#define STACK_GUARD_TEST_VALUE 0xA2A2
#define STACK_GUARD_MARGIN     32

#define MAX_BED_TEMP_CALIBRATION 50
#define MAX_HOTEND_TEMP_CALIBRATION 50

#define Z_BABYSTEP_MIN -3999
#define Z_BABYSTEP_MAX 0

#define PINDA_PREHEAT_X 20
#define PINDA_PREHEAT_Y 60
#define PINDA_PREHEAT_Z 0.15
/*
#define PINDA_PREHEAT_X 70
#define PINDA_PREHEAT_Y -3
#define PINDA_PREHEAT_Z 1*/
#define PINDA_HEAT_T 120 //time in s

#define PINDA_MIN_T 50
#define PINDA_STEP_T 10
#define PINDA_MAX_T 100

#define LONG_PRESS_TIME 1000 //time in ms for button long press
#define BUTTON_BLANKING_TIME 200 //time in ms for blanking after button release

#define DEFAULT_PID_TEMP 210

#define MIN_PRINT_FAN_SPEED 75


// How much shall the print head be lifted on power panic?
// Ideally the Z axis will reach a zero phase of the stepper driver on power outage. To simplify this,
// UVLO_Z_AXIS_SHIFT shall be an integer multiply of the stepper driver cycle, that is 4x full step.
// For example, the Prusa i3 MK2 with 16 microsteps per full step has Z stepping of 400 microsteps per mm.
// At 400 microsteps per mm, a full step lifts the Z axis by 0.04mm, and a stepper driver cycle is 0.16mm.
// The following example, 12 * (4 * 16 / 400) = 12 * 0.16mm = 1.92mm.
//#define UVLO_Z_AXIS_SHIFT 1.92
#define UVLO_Z_AXIS_SHIFT 0.64
// When powered off during PP recovery, the Z axis position can still be re-adjusted. In this case
// we just need to shift to the nearest fullstep, but we need a move which is at least
// "dropsegments" steps long. All the above rules still need to apply.
#define UVLO_TINY_Z_AXIS_SHIFT 0.16
// If power panic occured, and the current temperature is higher then target temperature before interrupt minus this offset, print will be recovered automatically.
#define AUTOMATIC_UVLO_BED_TEMP_OFFSET 5 

#define HEATBED_V2

#define M600_TIMEOUT 600  //seconds

//#define SUPPORT_VERBOSITY

#define MMU_CONFIG_FILE "mmu2/variants/config_MMU2S.h"
#define MMU_FILAMENT_COUNT 5
#define MMU_HWRESET
#define MMU_DEBUG //print communication between MMU and printer on serial
#define MMU_HAS_CUTTER

// This is experimental feature requested by our test department.
// There is no known use for ordinary user. If enabled by this macro
// and enabled from printer menu (not enabled by default). It cuts filament
// every time when switching filament from gcode. MMU_HAS_CUTTER needs to be
// defined.

//#define MMU_ALWAYS_CUT

// MMU Error pause position
#define MMU_ERR_X_PAUSE_POS 125
#define MMU_ERR_Y_PAUSE_POS 0
#define MMU_ERR_Z_PAUSE_LIFT 20

// Default Arc Interpolation Settings (Now configurable via M214)
#define DEFAULT_N_ARC_CORRECTION       25 // Number of interpolated segments between corrections.
/* A value of 1 or less for N_ARC_CORRECTION will trigger the use of Sin and Cos for every arc, which will improve accuracy at the
   cost of performance*/
#define DEFAULT_MM_PER_ARC_SEGMENT     1.0f // REQUIRED - The enforced maximum length of an arc segment
#define DEFAULT_MIN_MM_PER_ARC_SEGMENT 0.5f //the enforced minimum length of an interpolated segment
   /*  MIN_MM_PER_ARC_SEGMENT Must be smaller than MM_PER_ARC_SEGMENT.  Only has an effect if MIN_ARC_SEGMENTS > 0
       or ARC_SEGMENTS_PER_SEC > 0 .  If both MIN_ARC_SEGMENTS and ARC_SEGMENTS_PER_SEC is defined, the minimum
       calculated segment length is used. */
#define DEFAULT_MIN_ARC_SEGMENTS 20 // The enforced minimum segments in a full circle of the same radius.  Set to 0 to disable
#define DEFAULT_ARC_SEGMENTS_PER_SEC 0 // Use feedrate to choose segment length. Set to 0 to disable

#endif //__CONFIGURATION_PRUSA_H
