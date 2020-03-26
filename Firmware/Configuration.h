#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "boards.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

//-//
#include <avr/pgmspace.h>
extern const uint16_t _nPrinterType;
extern const char _sPrinterName[] PROGMEM;
extern const uint16_t _nPrinterMmuType;
extern const char _sPrinterMmuName[] PROGMEM;
extern uint16_t nPrinterType;
extern PGM_P sPrinterName;

// Firmware version
#define FW_VERSION "3.9.0-RC1"
#define FW_COMMIT_NR   3272
// FW_VERSION_UNKNOWN means this is an unofficial build.
// The firmware should only be checked into github with this symbol.
#define FW_DEV_VERSION FW_VERSION_UNKNOWN
#define FW_REPOSITORY "Unknown"
#define FW_VERSION_FULL FW_VERSION "-" STR(FW_COMMIT_NR)

// G-code language level
#define GCODE_LEVEL 1

// Debug version has debugging enabled (the symbol DEBUG_BUILD is set).
// The debug build may be a bit slower than the non-debug build, therefore the debug build should
// not be shipped to a customer.
#define FW_VERSION_DEBUG    6
// This is a development build. A development build is either built from an unofficial git repository, 
// or from an unofficial branch, or it does not have a label set. Only the build server should set this build type.
#define FW_VERSION_DEVEL    5
// This is an alpha release. Only the build server should set this build type.
#define FW_VERSION_ALPHA    4
// This is a beta release. Only the build server should set this build type.
#define FW_VERSION_BETA     3
// This is a release candidate build. Only the build server should set this build type.
#define FW_VERSION_RC       2
// This is a final release. Only the build server should set this build type.
#define FW_VERSION_GOLD     1
// This is an unofficial build. The firmware should only be checked into github with this symbol,
// the build server shall never produce builds with this build type.
#define FW_VERSION_UNKNOWN  0

#if FW_DEV_VERSION == FW_VERSION_DEBUG
#define DEBUG_BUILD
#else
#undef DEBUG_BUILD
#endif

#include "Configuration_prusa.h"

#define FW_PRUSA3D_MAGIC "PRUSA3DFW"
#define FW_PRUSA3D_MAGIC_LEN 10

#include "eeprom.h"

// This configuration file contains the basic settings.
// Advanced settings can be found in Configuration_adv.h
// BASIC SETTINGS: select your board type, temperature sensor type, axis scaling, and endstop configuration

// User-specified version info of this build to display in [Pronterface, etc] terminal window during
// startup. Implementation of an idea by Prof Braino to inform user that any changes made to this
// build by the user have been successfully uploaded into firmware.

//#define STRING_VERSION "1.0.2"

#define STRING_VERSION_CONFIG_H __DATE__ " " __TIME__ // build date and time
#define STRING_CONFIG_H_AUTHOR "(none, default config)" // Who made the changes.

// SERIAL_PORT selects which serial port should be used for communication with the host.
// This allows the connection of wireless adapters (for instance) to non-default port pins.
// Serial port 0 is still used by the Arduino bootloader regardless of this setting.
#define SERIAL_PORT 0

// This determines the communication speed of the printer
#define BAUDRATE 115200

// This enables the serial port associated to the Bluetooth interface
//#define BTENABLED              // Enable BT interface on AT90USB devices

// The following define selects which electronics board you have.
// Please choose the name from boards.h that matches your setup







// Define this to set a unique identifier for this printer, (Used by some programs to differentiate between machines)
// You can use an online service to generate a random UUID. (eg http://www.uuidgenerator.net/version4)
// #define MACHINE_UUID "00000000-0000-0000-0000-000000000000"

// This defines the number of extruders
#define EXTRUDERS 1

//// The following define selects which power supply you have. Please choose the one that matches your setup
// 1 = ATX
// 2 = X-Box 360 203Watts (the blue wire connected to PS_ON and the red wire to VCC)

#define POWER_SUPPLY 1





// Define this to have the electronics keep the power supply off on startup. If you don't know what this is leave it.
// #define PS_DEFAULT_OFF


// Actual temperature must be close to target for this long before M109 returns success
#define TEMP_RESIDENCY_TIME 3  // (seconds)
#define TEMP_HYSTERESIS 5       // (degC) range of +/- temperatures considered "close" to the target one
#define TEMP_WINDOW     1       // (degC) Window around target to start the residency timer x degC early.



// If your bed has low resistance e.g. .6 ohm and throws the fuse you can duty cycle it to reduce the
// average current. The value should be an integer and the heat bed will be turned on for 1 interval of
// HEATER_BED_DUTY_CYCLE_DIVIDER intervals.
//#define HEATER_BED_DUTY_CYCLE_DIVIDER 4

// If you want the M105 heater power reported in watts, define the BED_WATTS, and (shared for all extruders) EXTRUDER_WATTS
//#define EXTRUDER_WATTS (12.0*12.0/6.7) //  P=I^2/R
//#define BED_WATTS (12.0*12.0/1.1)      // P=I^2/R

// PID settings:
// Comment the following line to disable PID and enable bang-bang.
#define PIDTEMP
#define BANG_MAX 255 // limits current to nozzle while in bang-bang mode; 255=full current
#define PID_MAX BANG_MAX // limits current to nozzle while PID is active; 255=full current
#ifdef PIDTEMP
  //#define PID_DEBUG // Sends debug data to the serial port.
  //#define PID_OPENLOOP 1 // Puts PID in open loop. M104/M140 sets the output power from 0 to PID_MAX
  //#define SLOW_PWM_HEATERS // PWM with very low frequency (roughly 0.125Hz=8s) and minimum state time of approximately 1s useful for heaters driven by a relay
  #define PID_INTEGRAL_DRIVE_MAX PID_MAX  //limit for the integral term
  #define PID_K1 0.95 //smoothing factor within the PID
  #define PID_dT ((OVERSAMPLENR * 10.0)/(F_CPU / 64.0 / 256.0)) //sampling period of the temperature routine

// If you are using a pre-configured hotend then you can use one of the value sets by uncommenting it
// Ultimaker
    

// MakerGear
//    #define  DEFAULT_Kp 7.0
//    #define  DEFAULT_Ki 0.1
//    #define  DEFAULT_Kd 12

// Mendel Parts V9 on 12V
//    #define  DEFAULT_Kp 63.0
//    #define  DEFAULT_Ki 2.25
//    #define  DEFAULT_Kd 440
#endif // PIDTEMP


//this prevents dangerous Extruder moves, i.e. if the temperature is under the limit
//can be software-disabled for whatever purposes by
#define PREVENT_DANGEROUS_EXTRUDE
//if PREVENT_DANGEROUS_EXTRUDE is on, you can still disable (uncomment) very long bits of extrusion separately.
#define PREVENT_LENGTHY_EXTRUDE

#ifdef DEBUG_DISABLE_PREVENT_EXTRUDER
#undef PREVENT_DANGEROUS_EXTRUDE
#undef PREVENT_LENGTHY_EXTRUDE
#endif //DEBUG_DISABLE_PREVENT_EXTRUDER

#define EXTRUDE_MAXLENGTH (X_MAX_LENGTH+Y_MAX_LENGTH) //prevent extrusion of very large distances.

/*================== Thermal Runaway Protection ==============================
This is a feature to protect your printer from burn up in flames if it has
a thermistor coming off place (this happened to a friend of mine recently and
motivated me writing this feature).

The issue: If a thermistor come off, it will read a lower temperature than actual.
The system will turn the heater on forever, burning up the filament and anything
else around.

After the temperature reaches the target for the first time, this feature will 
start measuring for how long the current temperature stays below the target 
minus _HYSTERESIS (set_temperature - THERMAL_RUNAWAY_PROTECTION_HYSTERESIS).

If it stays longer than _PERIOD, it means the thermistor temperature
cannot catch up with the target, so something *may be* wrong. Then, to be on the
safe side, the system will he halt.

Bear in mind the count down will just start AFTER the first time the 
thermistor temperature is over the target, so you will have no problem if
your extruder heater takes 2 minutes to hit the target on heating.

*/
// If you want to enable this feature for all your extruder heaters,
// uncomment the 2 defines below:

// Parameters for all extruder heaters
//#define THERMAL_RUNAWAY_PROTECTION_PERIOD 40 //in seconds
//#define THERMAL_RUNAWAY_PROTECTION_HYSTERESIS 4 // in degree Celsius

// If you want to enable this feature for your bed heater,
// uncomment the 2 defines below:

// Parameters for the bed heater
//#define THERMAL_RUNAWAY_PROTECTION_BED_PERIOD 20 //in seconds
//#define THERMAL_RUNAWAY_PROTECTION_BED_HYSTERESIS 2 // in degree Celsius
//===========================================================================


//===========================================================================
//=============================Mechanical Settings===========================
//===========================================================================

// Uncomment the following line to enable CoreXY kinematics
// #define COREXY

// coarse Endstop Settings
#define ENDSTOPPULLUPS // Comment this out (using // at the start of the line) to disable the endstop pullup resistors

#ifndef ENDSTOPPULLUPS
  // fine endstop settings: Individual pullups. will be ignored if ENDSTOPPULLUPS is defined
  // #define ENDSTOPPULLUP_XMAX
  // #define ENDSTOPPULLUP_YMAX
  // #define ENDSTOPPULLUP_ZMAX
  // #define ENDSTOPPULLUP_XMIN
  // #define ENDSTOPPULLUP_YMIN
  // #define ENDSTOPPULLUP_ZMIN
#endif

#ifdef ENDSTOPPULLUPS
  #define ENDSTOPPULLUP_XMAX
  #define ENDSTOPPULLUP_YMAX
  #define ENDSTOPPULLUP_ZMAX
  #define ENDSTOPPULLUP_XMIN
  #define ENDSTOPPULLUP_YMIN
  #define ENDSTOPPULLUP_ZMIN
#endif

// The pullups are needed if you directly connect a mechanical endswitch between the signal and ground pins.

#define X_MAX_ENDSTOP_INVERTING 0 // set to 1 to invert the logic of the endstop.
#define Y_MAX_ENDSTOP_INVERTING 0 // set to 1 to invert the logic of the endstop.
#define Z_MAX_ENDSTOP_INVERTING 1 // set to 1 to invert the logic of the endstop.
//#define DISABLE_MAX_ENDSTOPS
//#define DISABLE_MIN_ENDSTOPS

// Disable max endstops for compatibility with endstop checking routine
#if defined(COREXY) && !defined(DISABLE_MAX_ENDSTOPS)
  #define DISABLE_MAX_ENDSTOPS
#endif

// For Inverting Stepper Enable Pins (Active Low) use 0, Non Inverting (Active High) use 1
#define X_ENABLE_ON 0
#define Y_ENABLE_ON 0
#define Z_ENABLE_ON 0
#define E_ENABLE_ON 0 // For all extruders

// Disables axis when it's not being used.
#define DISABLE_X 0
#define DISABLE_Y 0
#define DISABLE_Z 0
#define DISABLE_E 0// For all extruders
#define DISABLE_INACTIVE_EXTRUDER 1 //disable only inactive extruders and keep active extruder enabled


// ENDSTOP SETTINGS:
// Sets direction of endstops when homing; 1=MAX, -1=MIN
#define X_HOME_DIR -1
#define Y_HOME_DIR -1
#define Z_HOME_DIR -1

#ifdef DEBUG_DISABLE_SWLIMITS
#define min_software_endstops 0
#define max_software_endstops 0
#else
#define min_software_endstops 1 // If true, axis won't move to coordinates less than HOME_POS.
#define max_software_endstops 1  // If true, axis won't move to coordinates greater than the defined lengths below.
#endif //DEBUG_DISABLE_SWLIMITS


#define X_MAX_LENGTH (X_MAX_POS - X_MIN_POS)
#define Y_MAX_LENGTH (Y_MAX_POS - Y_MIN_POS) 
#define Z_MAX_LENGTH (Z_MAX_POS - Z_MIN_POS)

#define Z_HEIGHT_HIDE_LIVE_ADJUST_MENU 2.0f

#define HOME_Z_SEARCH_THRESHOLD 0.15f             // Threshold of the Z height in calibration

//============================= Bed Auto Leveling ===========================

//#define ENABLE_AUTO_BED_LEVELING // Delete the comment to enable (remove // at the start of the line)
#define Z_PROBE_REPEATABILITY_TEST  // If not commented out, Z-Probe Repeatability test will be included if Auto Bed Leveling is Enabled.

#ifdef ENABLE_AUTO_BED_LEVELING

// There are 2 different ways to pick the X and Y locations to probe:

//  - "grid" mode
//    Probe every point in a rectangular grid
//    You must specify the rectangle, and the density of sample points
//    This mode is preferred because there are more measurements.
//    It used to be called ACCURATE_BED_LEVELING but "grid" is more descriptive

//  - "3-point" mode
//    Probe 3 arbitrary points on the bed (that aren't colinear)
//    You must specify the X & Y coordinates of all 3 points

  #define AUTO_BED_LEVELING_GRID
  // with AUTO_BED_LEVELING_GRID, the bed is sampled in a
  // AUTO_BED_LEVELING_GRID_POINTSxAUTO_BED_LEVELING_GRID_POINTS grid
  // and least squares solution is calculated
  // Note: this feature occupies 10'206 byte
  #ifdef AUTO_BED_LEVELING_GRID

    // set the rectangle in which to probe
    #define LEFT_PROBE_BED_POSITION 15
    #define RIGHT_PROBE_BED_POSITION 170
    #define BACK_PROBE_BED_POSITION 180
    #define FRONT_PROBE_BED_POSITION 20

     // set the number of grid points per dimension
     // I wouldn't see a reason to go above 3 (=9 probing points on the bed)
    #define AUTO_BED_LEVELING_GRID_POINTS 2


  #else  // not AUTO_BED_LEVELING_GRID
    // with no grid, just probe 3 arbitrary points.  A simple cross-product
    // is used to esimate the plane of the print bed

      #define ABL_PROBE_PT_1_X 15
      #define ABL_PROBE_PT_1_Y 180
      #define ABL_PROBE_PT_2_X 15
      #define ABL_PROBE_PT_2_Y 20
      #define ABL_PROBE_PT_3_X 170
      #define ABL_PROBE_PT_3_Y 20

  #endif // AUTO_BED_LEVELING_GRID


  // these are the offsets to the probe relative to the extruder tip (Hotend - Probe)
  // X and Y offsets must be integers
  #define X_PROBE_OFFSET_FROM_EXTRUDER -25
  #define Y_PROBE_OFFSET_FROM_EXTRUDER -29
  #define Z_PROBE_OFFSET_FROM_EXTRUDER -12.35

  #define Z_RAISE_BEFORE_HOMING 5       // (in mm) Raise Z before homing (G28) for Probe Clearance.
                                        // Be sure you have this distance over your Z_MAX_POS in case

  #define XY_TRAVEL_SPEED 8000         // X and Y axis travel speed between probes, in mm/min

  #define Z_RAISE_BEFORE_PROBING 15    //How much the extruder will be raised before traveling to the first probing point.
  #define Z_RAISE_BETWEEN_PROBINGS 5  //How much the extruder will be raised when traveling from between next probing points

  //#define Z_PROBE_SLED // turn on if you have a z-probe mounted on a sled like those designed by Charles Bell
  //#define SLED_DOCKING_OFFSET 5 // the extra distance the X axis must travel to pickup the sled. 0 should be fine but you can push it further if you'd like.

  //If defined, the Probe servo will be turned on only during movement and then turned off to avoid jerk
  //The value is the delay to turn the servo off after powered on - depends on the servo speed; 300ms is good value, but you can try lower it.
  // You MUST HAVE the SERVO_ENDSTOPS defined to use here a value higher than zero otherwise your code will not compile.

//  #define PROBE_SERVO_DEACTIVATION_DELAY 300


//If you have enabled the Bed Auto Leveling and are using the same Z Probe for Z Homing,
//it is highly recommended you let this Z_SAFE_HOMING enabled!

  //#define Z_SAFE_HOMING   // This feature is meant to avoid Z homing with probe outside the bed area.
                          // When defined, it will:
                          // - Allow Z homing only after X and Y homing AND stepper drivers still enabled
                          // - If stepper drivers timeout, it will need X and Y homing again before Z homing
                          // - Position the probe in a defined XY point before Z Homing when homing all axis (G28)
                          // - Block Z homing only when the probe is outside bed area.

  #ifdef Z_SAFE_HOMING

    #define Z_SAFE_HOMING_X_POINT (X_MAX_LENGTH/2)    // X point for Z homing when homing all axis (G28)
    #define Z_SAFE_HOMING_Y_POINT (Y_MAX_LENGTH/2)    // Y point for Z homing when homing all axis (G28)

  #endif

  #ifdef AUTO_BED_LEVELING_GRID	// Check if Probe_Offset * Grid Points is greater than Probing Range
    #if X_PROBE_OFFSET_FROM_EXTRUDER < 0
      #if (-(X_PROBE_OFFSET_FROM_EXTRUDER * AUTO_BED_LEVELING_GRID_POINTS) >= (RIGHT_PROBE_BED_POSITION - LEFT_PROBE_BED_POSITION))
	     #error "The X axis probing range is not enough to fit all the points defined in AUTO_BED_LEVELING_GRID_POINTS"
	  #endif
	#else
      #if ((X_PROBE_OFFSET_FROM_EXTRUDER * AUTO_BED_LEVELING_GRID_POINTS) >= (RIGHT_PROBE_BED_POSITION - LEFT_PROBE_BED_POSITION))
	     #error "The X axis probing range is not enough to fit all the points defined in AUTO_BED_LEVELING_GRID_POINTS"
	  #endif
	#endif
    #if Y_PROBE_OFFSET_FROM_EXTRUDER < 0
      #if (-(Y_PROBE_OFFSET_FROM_EXTRUDER * AUTO_BED_LEVELING_GRID_POINTS) >= (BACK_PROBE_BED_POSITION - FRONT_PROBE_BED_POSITION))
	     #error "The Y axis probing range is not enough to fit all the points defined in AUTO_BED_LEVELING_GRID_POINTS"
	  #endif
	#else
      #if ((Y_PROBE_OFFSET_FROM_EXTRUDER * AUTO_BED_LEVELING_GRID_POINTS) >= (BACK_PROBE_BED_POSITION - FRONT_PROBE_BED_POSITION))
	     #error "The Y axis probing range is not enough to fit all the points defined in AUTO_BED_LEVELING_GRID_POINTS"
	  #endif
	#endif

	
  #endif
  
#endif // ENABLE_AUTO_BED_LEVELING


// The position of the homing switches
//#define MANUAL_HOME_POSITIONS  // If defined, MANUAL_*_HOME_POS below will be used
//#define BED_CENTER_AT_0_0  // If defined, the center of the bed is at (X=0, Y=0)

//Manual homing switch locations:
// For deltabots this means top and center of the Cartesian print volume.


// Offset of the extruders (uncomment if using more than one and relying on firmware to position when changing).
// The offset has to be X=0, Y=0 for the extruder 0 hotend (default extruder).
// For the other hotends it is their distance from the extruder 0 hotend.
// #define EXTRUDER_OFFSET_X {0.0, 20.00} // (in mm) for each extruder, offset of the hotend on the X axis
// #define EXTRUDER_OFFSET_Y {0.0, 5.00}  // (in mm) for each extruder, offset of the hotend on the Y axis

// The speed change that does not require acceleration (i.e. the software might assume it can be done instantaneously)
#define DEFAULT_XJERK                10       // (mm/sec)
#define DEFAULT_YJERK                10       // (mm/sec)
#define DEFAULT_ZJERK                 0.4     // (mm/sec)
#define DEFAULT_EJERK                 4.5     // (mm/sec)

//===========================================================================
//=============================Additional Features===========================
//===========================================================================

// Custom M code points
#define CUSTOM_M_CODES
#ifdef CUSTOM_M_CODES
  #define CUSTOM_M_CODE_SET_Z_PROBE_OFFSET 851
  #define Z_PROBE_OFFSET_RANGE_MIN -15
  #define Z_PROBE_OFFSET_RANGE_MAX -5
#endif


// EEPROM
// The microcontroller can store settings in the EEPROM, e.g. max velocity...
// M500 - stores parameters in EEPROM
// M501 - reads parameters from EEPROM (if you need reset them after you changed them temporarily).
// M502 - reverts to the default "factory settings".  You still need to store them in EEPROM afterwards if you want to.
//define this to enable EEPROM support
//#define EEPROM_SETTINGS
//to disable EEPROM Serial responses and decrease program space by ~1700 byte: comment this out:
// please keep turned on if you can.
//#define EEPROM_CHITCHAT

// Host Keepalive
//
// When enabled Marlin will send a busy status message to the host
// every couple of seconds when it can't accept commands.
//
#ifndef HEATBED_ANALYSIS
#define HOST_KEEPALIVE_FEATURE    // Disable this if your host doesn't like keepalive messages
#endif //HEATBED_ANALYSIS
#define HOST_KEEPALIVE_INTERVAL 2 // Number of seconds between "busy" messages. Set with M113.

//LCD and SD support
#define SDSUPPORT // Enable SD Card Support in Hardware Console
//#define SDSLOW // Use slower SD transfer mode (not normally needed - uncomment if you're getting volume init error)
#define SD_CHECK_AND_RETRY // Use CRC checks and retries on the SD communication
#define ENCODER_PULSES_PER_STEP 4 // Increase if you have a high resolution encoder
//#define ENCODER_STEPS_PER_MENU_ITEM 1 // Set according to ENCODER_PULSES_PER_STEP or your liking

// The RepRapDiscount Smart Controller (white PCB)
// http://reprap.org/wiki/RepRapDiscount_Smart_Controller
#define REPRAP_DISCOUNT_SMART_CONTROLLER
#define SDSUPPORT
#define LCD_WIDTH 20
#define LCD_HEIGHT 4


// Increase the FAN pwm frequency. Removes the PWM noise but increases heating in the FET/Arduino
//#define FAST_PWM_FAN

// Temperature status LEDs that display the hotend and bet temperature.
// If all hotends and bed temperature and temperature setpoint are < 54C then the BLUE led is on.
// Otherwise the RED led is on. There is 1C hysteresis.
//#define TEMP_STAT_LEDS

// Use software PWM to drive the fan, as for the heaters. This uses a very low frequency
// which is not ass annoying as with the hardware PWM. On the other hand, if this frequency
// is too low, you should also increment SOFT_PWM_SCALE.
#define FAN_SOFT_PWM
#define FAN_SOFT_PWM_BITS 4 //PWM bit resolution = 4bits, freq = 62.5Hz

// Bed soft pwm
#define HEATER_BED_SOFT_PWM_BITS 5 //PWM bit resolution = 5bits, freq = 31.25Hz

// Incrementing this by 1 will double the software PWM frequency,
// affecting heaters, and the fan if FAN_SOFT_PWM is enabled.
// However, control resolution will be halved for each increment;
// at zero value, there are 128 effective control positions.
#define SOFT_PWM_SCALE 0

// M240  Triggers a camera by emulating a Canon RC-1 Remote
// Data from: http://www.doc-diy.net/photo/rc-1_hacked/
// #define PHOTOGRAPH_PIN     23

// SF send wrong arc g-codes when using Arc Point as fillet procedure
//#define SF_ARC_FIX

//define BlinkM/CyzRgb Support
//#define BLINKM

/*********************************************************************\
* R/C SERVO support
* Sponsored by TrinityLabs, Reworked by codexmas
**********************************************************************/

// Number of servos
//
// If you select a configuration below, this will receive a default value and does not need to be set manually
// set it manually if you have more servos than extruders and wish to manually control some
// leaving it undefined or defining as 0 will disable the servo subsystem
// If unsure, leave commented / disabled
//
//#define NUM_SERVOS 3 // Servo index starts with 0 for M280 command

#define DEFAULT_NOMINAL_FILAMENT_DIA  1.75  //Enter the diameter (in mm) of the filament generally used (3.0 mm or 1.75 mm). Used by the volumetric extrusion.

// Calibration status of the machine, to be stored into the EEPROM,
// (unsigned char*)EEPROM_CALIBRATION_STATUS
enum CalibrationStatus
{
	// Freshly assembled, needs to peform a self-test and the XYZ calibration.
	CALIBRATION_STATUS_ASSEMBLED = 255,

	// For the wizard: self test has been performed, now the XYZ calibration is needed.
	CALIBRATION_STATUS_XYZ_CALIBRATION = 250,

	// For the wizard: factory assembled, needs to run Z calibration.
	CALIBRATION_STATUS_Z_CALIBRATION = 240,

	// The XYZ calibration has been performed, now it remains to run the V2Calibration.gcode.
	CALIBRATION_STATUS_LIVE_ADJUST = 230,

    // Calibrated, ready to print.
    CALIBRATION_STATUS_CALIBRATED = 1,

    // Legacy: resetted by issuing a G86 G-code.
    // This value can only be expected after an upgrade from the initial MK2 firmware releases.
    // Currently the G86 sets the calibration status to 
    CALIBRATION_STATUS_UNKNOWN = 0,
};

// Try to maintain a minimum distance from the bed even when Z is
// unknown when doing the following operations
#define MIN_Z_FOR_LOAD    50
#define MIN_Z_FOR_UNLOAD  20
#define MIN_Z_FOR_PREHEAT 10

#include "Configuration_adv.h"
#include "thermistortables.h"


#endif //__CONFIGURATION_H
