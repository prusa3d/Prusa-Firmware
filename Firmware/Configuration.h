#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdint.h>
#include "boards.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define _CONCAT(x,y) x##y
#define CONCAT(x,y) _CONCAT(x,y)

#include <avr/pgmspace.h>
extern const uint16_t _nPrinterType;
extern const char _sPrinterName[] PROGMEM;
extern const uint16_t _nPrinterMmuType;
extern const char _sPrinterMmuName[] PROGMEM;

// Firmware version.
// NOTE: These are ONLY used if you are not building via cmake and/or not in a git repository.
// Otherwise the repository information takes precedence.
#ifndef CMAKE_CONTROL
#define FW_MAJOR 3
#define FW_MINOR 14
#define FW_REVISION 1
#define FW_COMMITNR 8225
#define FW_FLAVOR RC      //uncomment if DEV, ALPHA, BETA or RC
#define FW_FLAVERSION 1     //uncomment if FW_FLAVOR is defined and versioning is needed. Limited to max 8.
#endif

#ifndef FW_FLAVOR
    #define FW_TWEAK (FIRMWARE_REVISION_RELEASED)
    #define FW_VERSION STR(FW_MAJOR) "." STR(FW_MINOR) "." STR(FW_REVISION)
    #define FW_VERSION_FULL STR(FW_MAJOR) "." STR(FW_MINOR) "." STR(FW_REVISION) "-" STR(FW_COMMITNR)
#else
    // Construct the TWEAK value as it is expected from the enum.
    #define FW_TWEAK (CONCAT(FIRMWARE_REVISION_,FW_FLAVOR) + FW_FLAVERSION)
    #define FW_VERSION STR(FW_MAJOR) "." STR(FW_MINOR) "." STR(FW_REVISION) "-" STR(FW_FLAVOR) "" STR(FW_FLAVERSION)
    #define FW_VERSION_FULL STR(FW_MAJOR) "." STR(FW_MINOR) "." STR(FW_REVISION) "-" STR(FW_FLAVOR) "" STR(FW_FLAVERSION) "+" STR(FW_COMMITNR)
#endif

// The full version string and repository source are set via cmake
#ifndef CMAKE_CONTROL
#define FW_COMMIT_HASH_LENGTH 1
#define FW_COMMIT_HASH "0"
#define FW_REPOSITORY "Unknown"
#endif

// G-code language level
#define GCODE_LEVEL 1

#ifndef SOURCE_DATE_EPOCH
#define SOURCE_DATE_EPOCH __DATE__
#endif
#ifndef SOURCE_TIME_EPOCH
#define SOURCE_TIME_EPOCH __TIME__
#endif

#include "Configuration_var.h"

#define FW_PRUSA3D_MAGIC "PRUSA3DFW"
#define FW_PRUSA3D_MAGIC_LEN 10

#include "eeprom.h"

// This configuration file contains the basic settings.
// Advanced settings can be found in Configuration_adv.h
// BASIC SETTINGS: select your board type, temperature sensor type, axis scaling, and endstop configuration

// User-specified version info of this build to display in [Pronterface, etc] terminal window during
// startup. Implementation of an idea by Prof Braino to inform user that any changes made to this
// build by the user have been successfully uploaded into firmware.

#define STRING_VERSION_CONFIG_H SOURCE_DATE_EPOCH " " SOURCE_TIME_EPOCH // build date and time
#define STRING_CONFIG_H_AUTHOR FW_REPOSITORY // Who made the changes.

// SERIAL_PORT selects which serial port should be used for communication with the host.
// This allows the connection of wireless adapters (for instance) to non-default port pins.
// Serial port 0 is still used by the Arduino bootloader regardless of this setting.
#define SERIAL_PORT 0

// This determines the communication speed of the printer
#define BAUDRATE 115200

// Enable g-code compression (see https://github.com/scottmudge/OctoPrint-MeatPack)
#define ENABLE_MEATPACK

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

// The position of the homing switches
//#define MANUAL_HOME_POSITIONS  // If defined, MANUAL_*_HOME_POS below will be used
//#define BED_CENTER_AT_0_0  // If defined, the center of the bed is at (X=0, Y=0)

//Manual homing switch locations:
// For deltabots this means top and center of the Cartesian print volume.

// The speed change that does not require acceleration (i.e. the software might assume it can be done instantaneously)
#define DEFAULT_XJERK                10       // (mm/sec)
#define DEFAULT_YJERK                10       // (mm/sec)
#define DEFAULT_ZJERK                 0.4     // (mm/sec)
#define DEFAULT_EJERK                 4.5     // (mm/sec)

//===========================================================================
//=============================Additional Features===========================
//===========================================================================

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

#define DEFAULT_NOMINAL_FILAMENT_DIA  1.75  //Enter the diameter (in mm) of the filament generally used (3.0 mm or 1.75 mm). Used by the volumetric extrusion.

// Try to maintain a minimum distance from the bed even when Z is
// unknown when doing the following operations
#define MIN_Z_FOR_LOAD    50 // lcd filament loading or autoload
#define MIN_Z_FOR_UNLOAD  20 // lcd filament unloading
#define MIN_Z_FOR_SWAP    27 // filament change (including M600)
#define MIN_Z_FOR_PREHEAT 10 // lcd preheat

#include "Configuration_adv.h"
#include "thermistortables.h"

#endif //__CONFIGURATION_H
