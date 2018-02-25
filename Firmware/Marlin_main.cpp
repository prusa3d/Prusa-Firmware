/* -*- c++ -*- */

/*
    Reprap firmware based on Sprinter and grbl.
 Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 This firmware is a mashup between Sprinter and grbl.
  (https://github.com/kliment/Sprinter)
  (https://github.com/simen/grbl/tree)

 It has preliminary support for Matthew Roberts advance algorithm
    http://reprap.org/pipermail/reprap-dev/2011-May/003323.html
 */

#include "Marlin.h"

#ifdef ENABLE_AUTO_BED_LEVELING
#include "vector_3.h"
  #ifdef AUTO_BED_LEVELING_GRID
    #include "qr_solve.h"
  #endif
#endif // ENABLE_AUTO_BED_LEVELING

#ifdef MESH_BED_LEVELING
  #include "mesh_bed_leveling.h"
  #include "mesh_bed_calibration.h"
#endif

#include "ultralcd.h"
#include "Configuration_prusa.h"
#include "planner.h"
#include "stepper.h"
#include "temperature.h"
#include "motion_control.h"
#include "cardreader.h"
#include "watchdog.h"
#include "ConfigurationStore.h"
#include "language.h"
#include "pins_arduino.h"
#include "math.h"
#include "util.h"

#include <avr/wdt.h>

#ifdef BLINKM
#include "BlinkM.h"
#include "Wire.h"
#endif

#ifdef ULTRALCD
#include "ultralcd.h"
#endif

#if NUM_SERVOS > 0
#include "Servo.h"
#endif

#if defined(DIGIPOTSS_PIN) && DIGIPOTSS_PIN > -1
#include <SPI.h>
#endif

#define VERSION_STRING  "1.0.2"


#include "ultralcd.h"

// Macros for bit masks
#define BIT(b) (1<<(b))
#define TEST(n,b) (((n)&BIT(b))!=0)
#define SET_BIT(n,b,value) (n) ^= ((-value)^(n)) & (BIT(b))

// look here for descriptions of G-codes: http://linuxcnc.org/handbook/gcode/g-code.html
// http://objects.reprap.org/wiki/Mendel_User_Manual:_RepRapGCodes

//Implemented Codes
//-------------------

// PRUSA CODES
// P F - Returns FW versions
// P R - Returns revision of printer

// G0  -> G1
// G1  - Coordinated Movement X Y Z E
// G2  - CW ARC
// G3  - CCW ARC
// G4  - Dwell S<seconds> or P<milliseconds>
// G10 - retract filament according to settings of M207
// G11 - retract recover filament according to settings of M208
// G28 - Home all Axis
// G29 - Detailed Z-Probe, probes the bed at 3 or more points.  Will fail if you haven't homed yet.
// G30 - Single Z Probe, probes bed at current XY location.
// G31 - Dock sled (Z_PROBE_SLED only)
// G32 - Undock sled (Z_PROBE_SLED only)
// G80 - Automatic mesh bed leveling
// G81 - Print bed profile
// G90 - Use Absolute Coordinates
// G91 - Use Relative Coordinates
// G92 - Set current position to coordinates given

// M Codes
// M0   - Unconditional stop - Wait for user to press a button on the LCD (Only if ULTRA_LCD is enabled)
// M1   - Same as M0
// M17  - Enable/Power all stepper motors
// M18  - Disable all stepper motors; same as M84
// M20  - List SD card
// M21  - Init SD card
// M22  - Release SD card
// M23  - Select SD file (M23 filename.g)
// M24  - Start/resume SD print
// M25  - Pause SD print
// M26  - Set SD position in bytes (M26 S12345)
// M27  - Report SD print status
// M28  - Start SD write (M28 filename.g)
// M29  - Stop SD write
// M30  - Delete file from SD (M30 filename.g)
// M31  - Output time since last M109 or SD card start to serial
// M32  - Select file and start SD print (Can be used _while_ printing from SD card files):
//        syntax "M32 /path/filename#", or "M32 S<startpos bytes> !filename#"
//        Call gcode file : "M32 P !filename#" and return to caller file after finishing (similar to #include).
//        The '#' is necessary when calling from within sd files, as it stops buffer prereading
// M42  - Change pin status via gcode Use M42 Px Sy to set pin x to value y, when omitting Px the onboard led will be used.
// M80  - Turn on Power Supply
// M81  - Turn off Power Supply
// M82  - Set E codes absolute (default)
// M83  - Set E codes relative while in Absolute Coordinates (G90) mode
// M84  - Disable steppers until next move,
//        or use S<seconds> to specify an inactivity timeout, after which the steppers will be disabled.  S0 to disable the timeout.
// M85  - Set inactivity shutdown timer with parameter S<seconds>. To disable set zero (default)
// M92  - Set axis_steps_per_unit - same syntax as G92
// M104 - Set extruder target temp
// M105 - Read current temp
// M106 - Fan on
// M107 - Fan off
// M109 - Sxxx Wait for extruder current temp to reach target temp. Waits only when heating
//        Rxxx Wait for extruder current temp to reach target temp. Waits when heating and cooling
//        IF AUTOTEMP is enabled, S<mintemp> B<maxtemp> F<factor>. Exit autotemp by any M109 without F
// M112 - Emergency stop
// M113 - Get or set the timeout interval for Host Keepalive "busy" messages
// M114 - Output current position to serial port
// M115 - Capabilities string
// M117 - display message
// M119 - Output Endstop status to serial port
// M126 - Solenoid Air Valve Open (BariCUDA support by jmil)
// M127 - Solenoid Air Valve Closed (BariCUDA vent to atmospheric pressure by jmil)
// M128 - EtoP Open (BariCUDA EtoP = electricity to air pressure transducer by jmil)
// M129 - EtoP Closed (BariCUDA EtoP = electricity to air pressure transducer by jmil)
// M140 - Set bed target temp
// M150 - Set BlinkM Color Output R: Red<0-255> U(!): Green<0-255> B: Blue<0-255> over i2c, G for green does not work.
// M190 - Sxxx Wait for bed current temp to reach target temp. Waits only when heating
//        Rxxx Wait for bed current temp to reach target temp. Waits when heating and cooling
// M200 D<millimeters>- set filament diameter and set E axis units to cubic millimeters (use S0 to set back to millimeters).
// M201 - Set max acceleration in units/s^2 for print moves (M201 X1000 Y1000)
// M202 - Set max acceleration in units/s^2 for travel moves (M202 X1000 Y1000) Unused in Marlin!!
// M203 - Set maximum feedrate that your machine can sustain (M203 X200 Y200 Z300 E10000) in mm/sec
// M204 - Set default acceleration: S normal moves T filament only moves (M204 S3000 T7000) in mm/sec^2  also sets minimum segment time in ms (B20000) to prevent buffer under-runs and M20 minimum feedrate
// M205 -  advanced settings:  minimum travel speed S=while printing T=travel only,  B=minimum segment time X= maximum xy jerk, Z=maximum Z jerk, E=maximum E jerk
// M206 - set additional homing offset
// M207 - set retract length S[positive mm] F[feedrate mm/min] Z[additional zlift/hop], stays in mm regardless of M200 setting
// M208 - set recover=unretract length S[positive mm surplus to the M207 S*] F[feedrate mm/sec]
// M209 - S<1=true/0=false> enable automatic retract detect if the slicer did not support G10/11: every normal extrude-only move will be classified as retract depending on the direction.
// M218 - set hotend offset (in mm): T<extruder_number> X<offset_on_X> Y<offset_on_Y>
// M220 S<factor in percent>- set speed factor override percentage
// M221 S<factor in percent>- set extrude factor override percentage
// M226 P<pin number> S<pin state>- Wait until the specified pin reaches the state required
// M240 - Trigger a camera to take a photograph
// M250 - Set LCD contrast C<contrast value> (value 0..63)
// M280 - set servo position absolute. P: servo index, S: angle or microseconds
// M300 - Play beep sound S<frequency Hz> P<duration ms>
// M301 - Set PID parameters P I and D
// M302 - Allow cold extrudes, or set the minimum extrude S<temperature>.
// M303 - PID relay autotune S<temperature> sets the target temperature. (default target temperature = 150C)
// M304 - Set bed PID parameters P I and D
// M400 - Finish all moves
// M401 - Lower z-probe if present
// M402 - Raise z-probe if present
// M404 - N<dia in mm> Enter the nominal filament width (3mm, 1.75mm ) or will display nominal filament width without parameters
// M405 - Turn on Filament Sensor extrusion control.  Optional D<delay in cm> to set delay in centimeters between sensor and extruder 
// M406 - Turn off Filament Sensor extrusion control 
// M407 - Displays measured filament diameter 
// M500 - stores parameters in EEPROM
// M501 - reads parameters from EEPROM (if you need reset them after you changed them temporarily).
// M502 - reverts to the default "factory settings".  You still need to store them in EEPROM afterwards if you want to.
// M503 - print the current settings (from memory not from EEPROM)
// M509 - force language selection on next restart
// M540 - Use S[0|1] to enable or disable the stop SD card print on endstop hit (requires ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED)
// M600 - Pause for filament change X[pos] Y[pos] Z[relative lift] E[initial retract] L[later retract distance for removal]
// M605 - Set dual x-carriage movement mode: S<mode> [ X<duplication x-offset> R<duplication temp offset> ]
// M900 - Set LIN_ADVANCE options, if enabled. See Configuration_adv.h for details.
// M907 - Set digital trimpot motor current using axis codes.
// M908 - Control digital trimpot directly.
// M350 - Set microstepping mode.
// M351 - Toggle MS1 MS2 pins directly.

// M928 - Start SD logging (M928 filename.g) - ended by M29
// M999 - Restart after being stopped by error

//Stepper Movement Variables

//===========================================================================
//=============================imported variables============================
//===========================================================================


//===========================================================================
//=============================public variables=============================
//===========================================================================
#ifdef SDSUPPORT
CardReader card;
#endif

unsigned long TimeSent = millis();
unsigned long TimeNow = millis();
unsigned long PingTime = millis();
unsigned long NcTime;

union Data
{
byte b[2];
int value;
};

float homing_feedrate[] = HOMING_FEEDRATE;
// Currently only the extruder axis may be switched to a relative mode.
// Other axes are always absolute or relative based on the common relative_mode flag.
bool axis_relative_modes[] = AXIS_RELATIVE_MODES;
int feedmultiply=100; //100->1 200->2
int saved_feedmultiply;
int extrudemultiply=100; //100->1 200->2
int extruder_multiply[EXTRUDERS] = {100
  #if EXTRUDERS > 1
    , 100
    #if EXTRUDERS > 2
      , 100
    #endif
  #endif
};

int bowden_length[4];

bool is_usb_printing = false;
bool homing_flag = false;

bool temp_cal_active = false;

unsigned long kicktime = millis()+100000;

unsigned int  usb_printing_counter;

int lcd_change_fil_state = 0;

int feedmultiplyBckp = 100;
float HotendTempBckp = 0;
int fanSpeedBckp = 0;
float pause_lastpos[4];
unsigned long pause_time = 0;
unsigned long start_pause_print = millis();

unsigned long load_filament_time;

bool mesh_bed_leveling_flag = false;
bool mesh_bed_run_from_menu = false;

unsigned char lang_selected = 0;

bool prusa_sd_card_upload = false;

unsigned int status_number = 0;

unsigned long total_filament_used;
unsigned int heating_status;
unsigned int heating_status_counter;
bool custom_message;
bool loading_flag = false;
unsigned int custom_message_type;
unsigned int custom_message_state;
char snmm_filaments_used = 0;

uint8_t selectedSerialPort;

float distance_from_min[3];

bool sortAlpha = false;

bool volumetric_enabled = false;
float filament_size[EXTRUDERS] = { DEFAULT_NOMINAL_FILAMENT_DIA
  #if EXTRUDERS > 1
      , DEFAULT_NOMINAL_FILAMENT_DIA
    #if EXTRUDERS > 2
       , DEFAULT_NOMINAL_FILAMENT_DIA
    #endif
  #endif
};
float volumetric_multiplier[EXTRUDERS] = {1.0
  #if EXTRUDERS > 1
    , 1.0
    #if EXTRUDERS > 2
      , 1.0
    #endif
  #endif
};
float current_position[NUM_AXIS] = { 0.0, 0.0, 0.0, 0.0 };
float add_homing[3]={0,0,0};

float min_pos[3] = { X_MIN_POS, Y_MIN_POS, Z_MIN_POS };
float max_pos[3] = { X_MAX_POS, Y_MAX_POS, Z_MAX_POS };
bool axis_known_position[3] = {false, false, false};
float zprobe_zoffset;

// Extruder offset
#if EXTRUDERS > 1
  #define NUM_EXTRUDER_OFFSETS 2 // only in XY plane
float extruder_offset[NUM_EXTRUDER_OFFSETS][EXTRUDERS] = {
#if defined(EXTRUDER_OFFSET_X) && defined(EXTRUDER_OFFSET_Y)
  EXTRUDER_OFFSET_X, EXTRUDER_OFFSET_Y
#endif
};
#endif

uint8_t active_extruder = 0;
int fanSpeed=0;

#ifdef FWRETRACT
  bool autoretract_enabled=false;
  bool retracted[EXTRUDERS]={false
    #if EXTRUDERS > 1
    , false
     #if EXTRUDERS > 2
      , false
     #endif
  #endif
  };
  bool retracted_swap[EXTRUDERS]={false
    #if EXTRUDERS > 1
    , false
     #if EXTRUDERS > 2
      , false
     #endif
  #endif
  };

  float retract_length = RETRACT_LENGTH;
  float retract_length_swap = RETRACT_LENGTH_SWAP;
  float retract_feedrate = RETRACT_FEEDRATE;
  float retract_zlift = RETRACT_ZLIFT;
  float retract_recover_length = RETRACT_RECOVER_LENGTH;
  float retract_recover_length_swap = RETRACT_RECOVER_LENGTH_SWAP;
  float retract_recover_feedrate = RETRACT_RECOVER_FEEDRATE;
#endif

#ifdef ULTIPANEL
  #ifdef PS_DEFAULT_OFF
    bool powersupply = false;
  #else
	  bool powersupply = true;
  #endif
#endif

bool cancel_heatup = false ;

#ifdef HOST_KEEPALIVE_FEATURE
  
  MarlinBusyState busy_state = NOT_BUSY;
  static long prev_busy_signal_ms = -1;
  uint8_t host_keepalive_interval = HOST_KEEPALIVE_INTERVAL;
#else
  #define host_keepalive();
  #define KEEPALIVE_STATE(n);
#endif

#ifdef FILAMENT_SENSOR
  //Variables for Filament Sensor input 
  float filament_width_nominal=DEFAULT_NOMINAL_FILAMENT_DIA;  //Set nominal filament width, can be changed with M404 
  bool filament_sensor=false;  //M405 turns on filament_sensor control, M406 turns it off 
  float filament_width_meas=DEFAULT_MEASURED_FILAMENT_DIA; //Stores the measured filament diameter 
  signed char measurement_delay[MAX_MEASUREMENT_DELAY+1];  //ring buffer to delay measurement  store extruder factor after subtracting 100 
  int delay_index1=0;  //index into ring buffer
  int delay_index2=-1;  //index into ring buffer - set to -1 on startup to indicate ring buffer needs to be initialized
  float delay_dist=0; //delay distance counter  
  int meas_delay_cm = MEASUREMENT_DELAY_CM;  //distance delay setting
#endif

const char errormagic[] PROGMEM = "Error:";
const char echomagic[] PROGMEM = "echo:";

bool no_response = false;
uint8_t important_status;
uint8_t saved_filament_type;

//===========================================================================
//=============================Private Variables=============================
//===========================================================================
const char axis_codes[NUM_AXIS] = {'X', 'Y', 'Z', 'E'};
float destination[NUM_AXIS] = {  0.0, 0.0, 0.0, 0.0};

// For tracing an arc
static float offset[3] = {0.0, 0.0, 0.0};
static bool home_all_axis = true;
static float feedrate = 1500.0, next_feedrate, saved_feedrate;
static long gcode_N, gcode_LastN, Stopped_gcode_LastN = 0;

// Determines Absolute or Relative Coordinates.
// Also there is bool axis_relative_modes[] per axis flag.
static bool relative_mode = false;  

// String circular buffer. Commands may be pushed to the buffer from both sides:
// Chained commands will be pushed to the front, interactive (from LCD menu) 
// and printing commands (from serial line or from SD card) are pushed to the tail.
// First character of each entry indicates the type of the entry: 
#define CMDBUFFER_CURRENT_TYPE_UNKNOWN  0
// Command in cmdbuffer was sent over USB.
#define CMDBUFFER_CURRENT_TYPE_USB      1
// Command in cmdbuffer was read from SDCARD.
#define CMDBUFFER_CURRENT_TYPE_SDCARD   2
// Command in cmdbuffer was generated by the UI.
#define CMDBUFFER_CURRENT_TYPE_UI       3
// Command in cmdbuffer was generated by another G-code.
#define CMDBUFFER_CURRENT_TYPE_CHAINED  4

// How much space to reserve for the chained commands
// of type CMDBUFFER_CURRENT_TYPE_CHAINED,
// which are pushed to the front of the queue?
// Maximum 5 commands of max length 20 + null terminator.
#define CMDBUFFER_RESERVE_FRONT       (5*21)
// Reserve BUFSIZE lines of length MAX_CMD_SIZE plus CMDBUFFER_RESERVE_FRONT.
static char cmdbuffer[BUFSIZE * (MAX_CMD_SIZE + 1) + CMDBUFFER_RESERVE_FRONT];
// Head of the circular buffer, where to read.
static int bufindr = 0;
// Tail of the buffer, where to write.
static int bufindw = 0;
// Number of lines in cmdbuffer.
static int buflen = 0;
// Flag for processing the current command inside the main Arduino loop().
// If a new command was pushed to the front of a command buffer while
// processing another command, this replaces the command on the top.
// Therefore don't remove the command from the queue in the loop() function.
static bool cmdbuffer_front_already_processed = false;

// Type of a command, which is to be executed right now.
#define CMDBUFFER_CURRENT_TYPE   (cmdbuffer[bufindr])
// String of a command, which is to be executed right now.
#define CMDBUFFER_CURRENT_STRING (cmdbuffer+bufindr+1)

// Enable debugging of the command buffer.
// Debugging information will be sent to serial line.
// #define CMDBUFFER_DEBUG

static int serial_count = 0;  //index of character read from serial line
static boolean comment_mode = false;
static char *strchr_pointer; // just a pointer to find chars in the command string like X, Y, Z, E, etc

const int sensitive_pins[] = SENSITIVE_PINS; // Sensitive pin list for M42

//static float tt = 0;
//static float bt = 0;

//Inactivity shutdown variables
static unsigned long previous_millis_cmd = 0;
unsigned long max_inactive_time = 0;
static unsigned long stepper_inactive_time = DEFAULT_STEPPER_DEACTIVE_TIME*1000l;

unsigned long starttime=0;
unsigned long stoptime=0;
unsigned long _usb_timer = 0;

static uint8_t tmp_extruder;


bool Stopped=false;

#if NUM_SERVOS > 0
  Servo servos[NUM_SERVOS];
#endif

bool CooldownNoWait = true;
bool target_direction;

//Insert variables if CHDK is defined
#ifdef CHDK
unsigned long chdkHigh = 0;
boolean chdkActive = false;
#endif

static int saved_feedmultiply_mm = 100;

//===========================================================================
//=============================Routines======================================
//===========================================================================

void get_arc_coordinates();
bool setTargetedHotend(int code);

void serial_echopair_P(const char *s_P, float v)
    { serialprintPGM(s_P); SERIAL_ECHO(v); }
void serial_echopair_P(const char *s_P, double v)
    { serialprintPGM(s_P); SERIAL_ECHO(v); }
void serial_echopair_P(const char *s_P, unsigned long v)
    { serialprintPGM(s_P); SERIAL_ECHO(v); }

#ifdef SDSUPPORT
  #include "SdFatUtil.h"
  int freeMemory() { return SdFatUtil::FreeRam(); }
#else
  extern "C" {
    extern unsigned int __bss_end;
    extern unsigned int __heap_start;
    extern void *__brkval;

    int freeMemory() {
      int free_memory;

      if ((int)__brkval == 0)
        free_memory = ((int)&free_memory) - ((int)&__bss_end);
      else
        free_memory = ((int)&free_memory) - ((int)__brkval);

      return free_memory;
    }
  }
#endif //!SDSUPPORT

// Pop the currently processed command from the queue.
// It is expected, that there is at least one command in the queue.
bool cmdqueue_pop_front()
{
    if (buflen > 0) {
#ifdef CMDBUFFER_DEBUG
        SERIAL_ECHOPGM("Dequeing ");
        SERIAL_ECHO(cmdbuffer+bufindr+1);
        SERIAL_ECHOLNPGM("");
        SERIAL_ECHOPGM("Old indices: buflen ");
        SERIAL_ECHO(buflen);
        SERIAL_ECHOPGM(", bufindr ");
        SERIAL_ECHO(bufindr);
        SERIAL_ECHOPGM(", bufindw ");
        SERIAL_ECHO(bufindw);
        SERIAL_ECHOPGM(", serial_count ");
        SERIAL_ECHO(serial_count);
        SERIAL_ECHOPGM(", bufsize ");
        SERIAL_ECHO(sizeof(cmdbuffer));
        SERIAL_ECHOLNPGM("");
#endif /* CMDBUFFER_DEBUG */
        if (-- buflen == 0) {
            // Empty buffer.
            if (serial_count == 0)
                // No serial communication is pending. Reset both pointers to zero.
                bufindw = 0;
            bufindr = bufindw;
        } else {
            // There is at least one ready line in the buffer.
            // First skip the current command ID and iterate up to the end of the string.
            for (++ bufindr; cmdbuffer[bufindr] != 0; ++ bufindr) ;
            // Second, skip the end of string null character and iterate until a nonzero command ID is found.
            for (++ bufindr; (bufindr < (int)sizeof(cmdbuffer)) && (cmdbuffer[bufindr] == 0); ++ bufindr) ;
            // If the end of the buffer was empty,
            if (bufindr == sizeof(cmdbuffer)) {
                // skip to the start and find the nonzero command.
                for (bufindr = 0; cmdbuffer[bufindr] == 0; ++ bufindr) ;
            }
#ifdef CMDBUFFER_DEBUG
            SERIAL_ECHOPGM("New indices: buflen ");
            SERIAL_ECHO(buflen);
            SERIAL_ECHOPGM(", bufindr ");
            SERIAL_ECHO(bufindr);
            SERIAL_ECHOPGM(", bufindw ");
            SERIAL_ECHO(bufindw);
            SERIAL_ECHOPGM(", serial_count ");
            SERIAL_ECHO(serial_count);
            SERIAL_ECHOPGM(" new command on the top: ");
            SERIAL_ECHO(cmdbuffer+bufindr+1);
            SERIAL_ECHOLNPGM("");
#endif /* CMDBUFFER_DEBUG */
        }
        return true;
    }
    return false;
}

void cmdqueue_reset()
{
    while (cmdqueue_pop_front()) ;
}

// How long a string could be pushed to the front of the command queue?
// If yes, adjust bufindr to the new position, where the new command could be enqued.
// len_asked does not contain the zero terminator size.
bool cmdqueue_could_enqueue_front(int len_asked)
{
    // MAX_CMD_SIZE has to accommodate the zero terminator.
    if (len_asked >= MAX_CMD_SIZE)
        return false;
    // Remove the currently processed command from the queue.
    if (! cmdbuffer_front_already_processed) {
        cmdqueue_pop_front();
        cmdbuffer_front_already_processed = true;
    }
	if (bufindr == bufindw && buflen > 0)
		// Full buffer.
        return false;
    // Adjust the end of the write buffer based on whether a partial line is in the receive buffer.
    int endw = (serial_count > 0) ? (bufindw + MAX_CMD_SIZE + 1) : bufindw;
    if (bufindw < bufindr) {
        int bufindr_new = bufindr - len_asked - 2;
        // Simple case. There is a contiguous space between the write buffer and the read buffer.
        if (endw <= bufindr_new) {
            bufindr = bufindr_new;
            return true;
        }
    } else {
        // Otherwise the free space is split between the start and end.
        if (len_asked + 2 <= bufindr) {
            // Could fit at the start.
            bufindr -= len_asked + 2;
            return true;
        }
        int bufindr_new = sizeof(cmdbuffer) - len_asked - 2;
        if (endw <= bufindr_new) {
            memset(cmdbuffer, 0, bufindr);
            bufindr = bufindr_new;
            return true;
        }
    }
    return false;
}

// Could one enqueue a command of lenthg len_asked into the buffer,
// while leaving CMDBUFFER_RESERVE_FRONT at the start?
// If yes, adjust bufindw to the new position, where the new command could be enqued.
// len_asked does not contain the zero terminator size.
bool cmdqueue_could_enqueue_back(int len_asked)
{
    // MAX_CMD_SIZE has to accommodate the zero terminator.
    if (len_asked >= MAX_CMD_SIZE)
        return false;

	if (bufindr == bufindw && buflen > 0)
		// Full buffer.
		return false;

    if (serial_count > 0) {
        // If there is some data stored starting at bufindw, len_asked is certainly smaller than
        // the allocated data buffer. Try to reserve a new buffer and to move the already received
        // serial data.
        // How much memory to reserve for the commands pushed to the front?
        // End of the queue, when pushing to the end.
        int endw = bufindw + len_asked + 2;
        if (bufindw < bufindr)
            // Simple case. There is a contiguous space between the write buffer and the read buffer.
            return ((endw + CMDBUFFER_RESERVE_FRONT) <= bufindr);
        // Otherwise the free space is split between the start and end.
        if (// Could one fit to the end, including the reserve?
            (endw + CMDBUFFER_RESERVE_FRONT <= (int)sizeof(cmdbuffer)) ||
            // Could one fit to the end, and the reserve to the start?
            ((endw <= (int)sizeof(cmdbuffer)) && (CMDBUFFER_RESERVE_FRONT <= bufindr)))
            return true;
        // Could one fit both to the start?
        if (len_asked + 2 + CMDBUFFER_RESERVE_FRONT <= bufindr) {
            // Mark the rest of the buffer as used.
            memset(cmdbuffer+bufindw, 0, sizeof(cmdbuffer)-bufindw);
            // and point to the start.
            bufindw = 0;
            return true;
        }
    } else {
        // How much memory to reserve for the commands pushed to the front?
        // End of the queue, when pushing to the end.
        int endw = bufindw + len_asked + 2;
        if (bufindw < bufindr)
            // Simple case. There is a contiguous space between the write buffer and the read buffer.
            return ((endw + CMDBUFFER_RESERVE_FRONT) <= bufindr);
        // Otherwise the free space is split between the start and end.
        if (// Could one fit to the end, including the reserve?
            (endw + CMDBUFFER_RESERVE_FRONT <= (int)sizeof(cmdbuffer)) ||
            // Could one fit to the end, and the reserve to the start?
            ((endw <= (int)sizeof(cmdbuffer)) && (CMDBUFFER_RESERVE_FRONT <= bufindr)))
            return true;
        // Could one fit both to the start?
        if (len_asked + 2 + CMDBUFFER_RESERVE_FRONT <= bufindr) {
            // Mark the rest of the buffer as used.
            memset(cmdbuffer+bufindw, 0, sizeof(cmdbuffer)-bufindw);
            // and point to the start.
            bufindw = 0;
            return true;
        }
    }
    return false;
}

#ifdef CMDBUFFER_DEBUG
static void cmdqueue_dump_to_serial_single_line(int nr, const char *p)
{
    SERIAL_ECHOPGM("Entry nr: ");
    SERIAL_ECHO(nr);
    SERIAL_ECHOPGM(", type: ");
    SERIAL_ECHO(int(*p));
    SERIAL_ECHOPGM(", cmd: ");
    SERIAL_ECHO(p+1);  
    SERIAL_ECHOLNPGM("");
}

static void cmdqueue_dump_to_serial()
{
    if (buflen == 0) {
        SERIAL_ECHOLNPGM("The command buffer is empty.");
    } else {
        SERIAL_ECHOPGM("Content of the buffer: entries ");
        SERIAL_ECHO(buflen);
        SERIAL_ECHOPGM(", indr ");
        SERIAL_ECHO(bufindr);
        SERIAL_ECHOPGM(", indw ");
        SERIAL_ECHO(bufindw);
        SERIAL_ECHOLNPGM("");
        int nr = 0;
        if (bufindr < bufindw) {
            for (const char *p = cmdbuffer + bufindr; p < cmdbuffer + bufindw; ++ nr) {
                cmdqueue_dump_to_serial_single_line(nr, p);
                // Skip the command.
                for (++p; *p != 0; ++ p);
                // Skip the gaps.
                for (++p; p < cmdbuffer + bufindw && *p == 0; ++ p);
            }
        } else {
            for (const char *p = cmdbuffer + bufindr; p < cmdbuffer + sizeof(cmdbuffer); ++ nr) {
                cmdqueue_dump_to_serial_single_line(nr, p);
                // Skip the command.
                for (++p; *p != 0; ++ p);
                // Skip the gaps.
                for (++p; p < cmdbuffer + sizeof(cmdbuffer) && *p == 0; ++ p);
            }
            for (const char *p = cmdbuffer; p < cmdbuffer + bufindw; ++ nr) {
                cmdqueue_dump_to_serial_single_line(nr, p);
                // Skip the command.
                for (++p; *p != 0; ++ p);
                // Skip the gaps.
                for (++p; p < cmdbuffer + bufindw && *p == 0; ++ p);
            }
        }
        SERIAL_ECHOLNPGM("End of the buffer.");
    }
}
#endif /* CMDBUFFER_DEBUG */

//adds an command to the main command buffer
//thats really done in a non-safe way.
//needs overworking someday
// Currently the maximum length of a command piped through this function is around 20 characters
void enquecommand(const char *cmd, bool from_progmem)
{
    int len = from_progmem ? strlen_P(cmd) : strlen(cmd);
    // Does cmd fit the queue while leaving sufficient space at the front for the chained commands?
    // If it fits, it may move bufindw, so it points to a contiguous buffer, which fits cmd.
    if (cmdqueue_could_enqueue_back(len)) {
        // This is dangerous if a mixing of serial and this happens
        // This may easily be tested: If serial_count > 0, we have a problem.
        cmdbuffer[bufindw] = CMDBUFFER_CURRENT_TYPE_UI;
        if (from_progmem)
            strcpy_P(cmdbuffer + bufindw + 1, cmd);
        else
            strcpy(cmdbuffer + bufindw + 1, cmd);
		if (!farm_mode) {
			SERIAL_ECHO_START;
			SERIAL_ECHORPGM(MSG_Enqueing);
			SERIAL_ECHO(cmdbuffer + bufindw + 1);
			SERIAL_ECHOLNPGM("\"");
		}
        bufindw += len + 2;
        if (bufindw == sizeof(cmdbuffer))
            bufindw = 0;
        ++ buflen;
#ifdef CMDBUFFER_DEBUG
        cmdqueue_dump_to_serial();
#endif /* CMDBUFFER_DEBUG */
    } else {
        SERIAL_ERROR_START;
        SERIAL_ECHORPGM(MSG_Enqueing);
        if (from_progmem)
            SERIAL_PROTOCOLRPGM(cmd);
        else
            SERIAL_ECHO(cmd);
        SERIAL_ECHOLNPGM("\" failed: Buffer full!");
#ifdef CMDBUFFER_DEBUG
        cmdqueue_dump_to_serial();
#endif /* CMDBUFFER_DEBUG */
    }
}

bool cmd_buffer_empty()
{
	return (buflen == 0);
}

void enquecommand_front(const char *cmd, bool from_progmem)
{
    int len = from_progmem ? strlen_P(cmd) : strlen(cmd);
    // Does cmd fit the queue? This call shall move bufindr, so the command may be copied.
    if (cmdqueue_could_enqueue_front(len)) {
        cmdbuffer[bufindr] = CMDBUFFER_CURRENT_TYPE_UI;
        if (from_progmem)
            strcpy_P(cmdbuffer + bufindr + 1, cmd);
        else
            strcpy(cmdbuffer + bufindr + 1, cmd);
        ++ buflen;
		if (!farm_mode) {
			SERIAL_ECHO_START;
			SERIAL_ECHOPGM("Enqueing to the front: \"");
			SERIAL_ECHO(cmdbuffer + bufindr + 1);
			SERIAL_ECHOLNPGM("\"");
		}
#ifdef CMDBUFFER_DEBUG
        cmdqueue_dump_to_serial();
#endif /* CMDBUFFER_DEBUG */
    } else {
        SERIAL_ERROR_START;
        SERIAL_ECHOPGM("Enqueing to the front: \"");
        if (from_progmem)
            SERIAL_PROTOCOLRPGM(cmd);
        else
            SERIAL_ECHO(cmd);
        SERIAL_ECHOLNPGM("\" failed: Buffer full!");
#ifdef CMDBUFFER_DEBUG
        cmdqueue_dump_to_serial();
#endif /* CMDBUFFER_DEBUG */
    }
}

// Mark the command at the top of the command queue as new.
// Therefore it will not be removed from the queue.
void repeatcommand_front()
{
    cmdbuffer_front_already_processed = true;
} 

bool is_buffer_empty()
{
	if (buflen == 0) return true;
	else return false;
}

void setup_killpin()
{
  #if defined(KILL_PIN) && KILL_PIN > -1
    SET_INPUT(KILL_PIN);
    WRITE(KILL_PIN,HIGH);
  #endif
}

// Set home pin
void setup_homepin(void)
{
#if defined(HOME_PIN) && HOME_PIN > -1
   SET_INPUT(HOME_PIN);
   WRITE(HOME_PIN,HIGH);
#endif
}

void setup_photpin()
{
  #if defined(PHOTOGRAPH_PIN) && PHOTOGRAPH_PIN > -1
    SET_OUTPUT(PHOTOGRAPH_PIN);
    WRITE(PHOTOGRAPH_PIN, LOW);
  #endif
}

void setup_powerhold()
{
  #if defined(SUICIDE_PIN) && SUICIDE_PIN > -1
    SET_OUTPUT(SUICIDE_PIN);
    WRITE(SUICIDE_PIN, HIGH);
  #endif
  #if defined(PS_ON_PIN) && PS_ON_PIN > -1
    SET_OUTPUT(PS_ON_PIN);
	#if defined(PS_DEFAULT_OFF)
	  WRITE(PS_ON_PIN, PS_ON_ASLEEP);
    #else
	  WRITE(PS_ON_PIN, PS_ON_AWAKE);
	#endif
  #endif
}

void suicide()
{
  #if defined(SUICIDE_PIN) && SUICIDE_PIN > -1
    SET_OUTPUT(SUICIDE_PIN);
    WRITE(SUICIDE_PIN, LOW);
  #endif
}

void servo_init()
{
  #if (NUM_SERVOS >= 1) && defined(SERVO0_PIN) && (SERVO0_PIN > -1)
    servos[0].attach(SERVO0_PIN);
  #endif
  #if (NUM_SERVOS >= 2) && defined(SERVO1_PIN) && (SERVO1_PIN > -1)
    servos[1].attach(SERVO1_PIN);
  #endif
  #if (NUM_SERVOS >= 3) && defined(SERVO2_PIN) && (SERVO2_PIN > -1)
    servos[2].attach(SERVO2_PIN);
  #endif
  #if (NUM_SERVOS >= 4) && defined(SERVO3_PIN) && (SERVO3_PIN > -1)
    servos[3].attach(SERVO3_PIN);
  #endif
  #if (NUM_SERVOS >= 5)
    #error "TODO: enter initalisation code for more servos"
  #endif
}


#ifdef MESH_BED_LEVELING
   enum MeshLevelingState { MeshReport, MeshStart, MeshNext, MeshSet };
#endif


// Factory reset function
// This function is used to erase parts or whole EEPROM memory which is used for storing calibration and and so on.
// Level input parameter sets depth of reset
// Quiet parameter masks all waitings for user interact.
int  er_progress = 0;
void factory_reset(char level, bool quiet)
{	
    lcd_implementation_clear();
    switch (level) {
                   
        // Level 0: Language reset
        case 0:
            WRITE(BEEPER, HIGH);
            _delay_ms(100);
            WRITE(BEEPER, LOW);
            
            lcd_force_language_selection();
            break;
         
		//Level 1: Reset statistics
		case 1:
			WRITE(BEEPER, HIGH);
			_delay_ms(100);
			WRITE(BEEPER, LOW);
			eeprom_update_dword((uint32_t *)EEPROM_TOTALTIME, 0);
			eeprom_update_dword((uint32_t *)EEPROM_FILAMENTUSED, 0);
			lcd_menu_statistics();
            
			break;

        // Level 2: Prepare for shipping
        case 2:
			//lcd_printPGM(PSTR("Factory RESET"));
            //lcd_print_at_PGM(1,2,PSTR("Shipping prep"));
            
            // Force language selection at the next boot up.
            lcd_force_language_selection();
            // Force the "Follow calibration flow" message at the next boot up.
            calibration_status_store(CALIBRATION_STATUS_Z_CALIBRATION);
            farm_no = 0;
            farm_mode = 0;
            eeprom_update_byte((uint8_t*)EEPROM_FARM_MODE, farm_mode);
            EEPROM_save_B(EEPROM_FARM_NUMBER, &farm_no);
                       
            WRITE(BEEPER, HIGH);
            _delay_ms(100);
            WRITE(BEEPER, LOW);
			//_delay_ms(2000);
            break;

			// Level 3: erase everything, whole EEPROM will be set to 0xFF

		case 3:
			lcd_printPGM(PSTR("Factory RESET"));
			lcd_print_at_PGM(1, 2, PSTR("ERASING all data"));

			WRITE(BEEPER, HIGH);
			_delay_ms(100);
			WRITE(BEEPER, LOW);

			er_progress = 0;
			lcd_print_at_PGM(3, 3, PSTR("      "));
			lcd_implementation_print_at(3, 3, er_progress);

			// Erase EEPROM
			for (int i = 0; i < 4096; i++) {
				eeprom_write_byte((uint8_t*)i, 0xFF);

				if (i % 41 == 0) {
					er_progress++;
					lcd_print_at_PGM(3, 3, PSTR("      "));
					lcd_implementation_print_at(3, 3, er_progress);
					lcd_printPGM(PSTR("%"));
				}

			}


			break;
		case 4:
			bowden_menu();
			break;
        
        default:
            break;
    }
    

}


// "Setup" function is called by the Arduino framework on startup.
// Before startup, the Timers-functions (PWM)/Analog RW and HardwareSerial provided by the Arduino-code 
// are initialized by the main() routine provided by the Arduino framework.
void setup()
{
	lcd_init();
	lcd_print_at_PGM(0, 1, PSTR("   Original Prusa   "));
	lcd_print_at_PGM(0, 2, PSTR("    3D  Printers    "));
	setup_killpin();
	setup_powerhold();
	farm_mode = eeprom_read_byte((uint8_t*)EEPROM_FARM_MODE);
	EEPROM_read_B(EEPROM_FARM_NUMBER, &farm_no);
	if ((farm_mode == 0xFF && farm_no == 0) || ((uint16_t)farm_no == 0xFFFF))
		farm_mode = false; //if farm_mode has not been stored to eeprom yet and farm number is set to zero or EEPROM is fresh, deactivate farm mode 
	if ((uint16_t)farm_no == 0xFFFF) farm_no = 0;
	if (farm_mode)
	{
		prusa_statistics(8);
		no_response = true; //we need confirmation by recieving PRUSA thx
		important_status = 8;
        selectedSerialPort = 1;
	} else {
        selectedSerialPort = 0;
    }
	MYSERIAL.begin(BAUDRATE);
	SERIAL_PROTOCOLLNPGM("start");
	SERIAL_ECHO_START;


#if 0
	SERIAL_ECHOLN("Reading eeprom from 0 to 100: start");
	for (int i = 0; i < 4096; ++i) {
		int b = eeprom_read_byte((unsigned char*)i);
		if (b != 255) {
			SERIAL_ECHO(i);
			SERIAL_ECHO(":");
			SERIAL_ECHO(b);
			SERIAL_ECHOLN("");
		}
	}
	SERIAL_ECHOLN("Reading eeprom from 0 to 100: done");
#endif

	// Check startup - does nothing if bootloader sets MCUSR to 0
	byte mcu = MCUSR;
	if (mcu & 1) SERIAL_ECHOLNRPGM(MSG_POWERUP);
	if (mcu & 2) SERIAL_ECHOLNRPGM(MSG_EXTERNAL_RESET);
	if (mcu & 4) SERIAL_ECHOLNRPGM(MSG_BROWNOUT_RESET);
	if (mcu & 8) SERIAL_ECHOLNRPGM(MSG_WATCHDOG_RESET);
	if (mcu & 32) SERIAL_ECHOLNRPGM(MSG_SOFTWARE_RESET);
	MCUSR = 0;

	//SERIAL_ECHORPGM(MSG_MARLIN);
	//SERIAL_ECHOLNRPGM(VERSION_STRING);

#ifdef STRING_VERSION_CONFIG_H
#ifdef STRING_CONFIG_H_AUTHOR
	SERIAL_ECHO_START;
	SERIAL_ECHORPGM(MSG_CONFIGURATION_VER);
	SERIAL_ECHOPGM(STRING_VERSION_CONFIG_H);
	SERIAL_ECHORPGM(MSG_AUTHOR);
	SERIAL_ECHOLNPGM(STRING_CONFIG_H_AUTHOR);
	SERIAL_ECHOPGM("Compiled: ");
	SERIAL_ECHOLNPGM(__DATE__);
#endif
#endif

	SERIAL_ECHO_START;
	SERIAL_ECHORPGM(MSG_FREE_MEMORY);
	SERIAL_ECHO(freeMemory());
	SERIAL_ECHORPGM(MSG_PLANNER_BUFFER_BYTES);
	SERIAL_ECHOLN((int)sizeof(block_t)*BLOCK_BUFFER_SIZE);
	lcd_update_enable(false);
	// loads data from EEPROM if available else uses defaults (and resets step acceleration rate)
	bool previous_settings_retrieved = Config_RetrieveSettings();
	SdFatUtil::set_stack_guard(); //writes magic number at the end of static variables to protect against overwriting static memory by stack
	tp_init();    // Initialize temperature loop
	plan_init();  // Initialize planner;
	watchdog_init();
	st_init();    // Initialize stepper, this enables interrupts!
	setup_photpin();
	servo_init();
	// Reset the machine correction matrix.
	// It does not make sense to load the correction matrix until the machine is homed.
	world2machine_reset();

	lcd_init();
	KEEPALIVE_STATE(PAUSED_FOR_USER);
	if (!READ(BTN_ENC))
	{
		_delay_ms(1000);
		if (!READ(BTN_ENC))
		{
			lcd_implementation_clear();


			lcd_printPGM(PSTR("Factory RESET"));


			SET_OUTPUT(BEEPER);
			WRITE(BEEPER, HIGH);

			while (!READ(BTN_ENC));

			WRITE(BEEPER, LOW);



			_delay_ms(2000);
			
			char level = reset_menu();
			factory_reset(level, false);

			switch (level) {
			case 0: _delay_ms(0); break;
			case 1: _delay_ms(0); break;
			case 2: _delay_ms(0); break;
			case 3: _delay_ms(0); break;
			}
			// _delay_ms(100);
  /*
  #ifdef MESH_BED_LEVELING
			_delay_ms(2000);

			if (!READ(BTN_ENC))
			{
				WRITE(BEEPER, HIGH);
				_delay_ms(100);
				WRITE(BEEPER, LOW);
				_delay_ms(200);
				WRITE(BEEPER, HIGH);
				_delay_ms(100);
				WRITE(BEEPER, LOW);

				int _z = 0;
				calibration_status_store(CALIBRATION_STATUS_CALIBRATED);
				EEPROM_save_B(EEPROM_BABYSTEP_X, &_z);
				EEPROM_save_B(EEPROM_BABYSTEP_Y, &_z);
				EEPROM_save_B(EEPROM_BABYSTEP_Z, &_z);
			}
			else
			{

				WRITE(BEEPER, HIGH);
				_delay_ms(100);
				WRITE(BEEPER, LOW);
			}
  #endif // mesh */

		}
	}
	else
	{
		//_delay_ms(1000);  // wait 1sec to display the splash screen // what's this and why do we need it?? - andre
	}



#if defined(CONTROLLERFAN_PIN) && CONTROLLERFAN_PIN > -1
	SET_OUTPUT(CONTROLLERFAN_PIN); //Set pin used for driver cooling fan
#endif

#ifdef DIGIPOT_I2C
	digipot_i2c_init();
#endif
	setup_homepin();

#if defined(Z_AXIS_ALWAYS_ON)
	enable_z();
#endif
	
	// Enable Toshiba FlashAir SD card / WiFi enahanced card.
	card.ToshibaFlashAir_enable(eeprom_read_byte((unsigned char*)EEPROM_TOSHIBA_FLASH_AIR_COMPATIBLITY) == 1);
	// Force SD card update. Otherwise the SD card update is done from loop() on card.checkautostart(false), 
	// but this times out if a blocking dialog is shown in setup().
	lcd_print_at_PGM(0, 1, PSTR("   Original Prusa   ")); // we need to do this again for some reason, no time to research
	lcd_print_at_PGM(0, 2, PSTR("    3D  Printers    "));
	card.initsd();

	if (eeprom_read_dword((uint32_t*)(EEPROM_TOP - 4)) == 0x0ffffffff &&
		eeprom_read_dword((uint32_t*)(EEPROM_TOP - 8)) == 0x0ffffffff &&
		eeprom_read_dword((uint32_t*)(EEPROM_TOP - 12)) == 0x0ffffffff) {
		// Maiden startup. The firmware has been loaded and first started on a virgin RAMBo board,
		// where all the EEPROM entries are set to 0x0ff.
		// Once a firmware boots up, it forces at least a language selection, which changes
		// EEPROM_LANG to number lower than 0x0ff.
		// 1) Set a high power mode.
		eeprom_write_byte((uint8_t*)EEPROM_SILENT, 0);
		eeprom_write_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 1); //run wizard

	}
#ifdef SNMM
	if (eeprom_read_dword((uint32_t*)EEPROM_BOWDEN_LENGTH) == 0x0ffffffff) { //bowden length used for SNMM
	  int _z = BOWDEN_LENGTH;
	  for(int i = 0; i<4; i++) EEPROM_save_B(EEPROM_BOWDEN_LENGTH + i * 2, &_z);
	}
#endif

  // In the future, somewhere here would one compare the current firmware version against the firmware version stored in the EEPROM.
  // If they differ, an update procedure may need to be performed. At the end of this block, the current firmware version
  // is being written into the EEPROM, so the update procedure will be triggered only once.
    lang_selected = eeprom_read_byte((uint8_t*)EEPROM_LANG);
    if (lang_selected >= LANG_NUM){
      lcd_mylang();
    }
	
	if (eeprom_read_byte((uint8_t*)EEPROM_TEMP_CAL_ACTIVE) == 255) {
		eeprom_write_byte((uint8_t*)EEPROM_TEMP_CAL_ACTIVE, 0);
		temp_cal_active = false;
	} else temp_cal_active = eeprom_read_byte((uint8_t*)EEPROM_TEMP_CAL_ACTIVE);

	if (eeprom_read_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA) == 255) {
		eeprom_write_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, 0);
	}
	if (eeprom_read_byte((uint8_t*)EEPROM_SD_SORT) == 255) {
		eeprom_write_byte((uint8_t*)EEPROM_SD_SORT, 0);
	}

#ifndef DEBUG_DISABLE_STARTMSGS
	check_babystep(); //checking if Z babystep is in allowed range

  for (int i = 0; i < 4; i++) EEPROM_read_B(EEPROM_BOWDEN_LENGTH + i * 2, &bowden_length[i]);
  if (eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE) == 1) {
	  lcd_wizard(0);
  }
  else if (eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE) == 0) { //dont show calibration status messages if wizard is currently active

	  if (calibration_status() == CALIBRATION_STATUS_ASSEMBLED ||
		  calibration_status() == CALIBRATION_STATUS_UNKNOWN ||
	      calibration_status() == CALIBRATION_STATUS_XYZ_CALIBRATION){
		  // Reset the babystepping values, so the printer will not move the Z axis up when the babystepping is enabled.
		  eeprom_update_word((uint16_t*)EEPROM_BABYSTEP_Z, 0);
		  // Show the message.
		  lcd_show_fullscreen_message_and_wait_P(MSG_FOLLOW_CALIBRATION_FLOW);
	  }
	  else if (calibration_status() == CALIBRATION_STATUS_LIVE_ADJUST) {
		  // Show the message.
		  lcd_show_fullscreen_message_and_wait_P(MSG_BABYSTEP_Z_NOT_SET);
		  lcd_update_enable(true);
	  }
	  else if (calibration_status() == CALIBRATION_STATUS_CALIBRATED && temp_cal_active == true && calibration_status_pinda() == false) {
		  lcd_show_fullscreen_message_and_wait_P(MSG_PINDA_NOT_CALIBRATED);
		  lcd_update_enable(true);
	  }
	  else if (calibration_status() == CALIBRATION_STATUS_Z_CALIBRATION) {
		  // Show the message.
		  lcd_show_fullscreen_message_and_wait_P(MSG_FOLLOW_CALIBRATION_FLOW);
	  }
	  //If eeprom version for storing parameters to eeprom using M500 changed, default settings are used. Inform user in this case
	  if (!previous_settings_retrieved) {
		  lcd_show_fullscreen_message_and_wait_P(MSG_DEFAULT_SETTINGS_LOADED);
	  }
  }  
  
#endif //DEBUG_DISABLE_STARTMSGS
  lcd_update_enable(true);

  // Store the currently running firmware into an eeprom,
  // so the next time the firmware gets updated, it will know from which version it has been updated.
  update_current_firmware_version_to_eeprom();
  KEEPALIVE_STATE(NOT_BUSY);
}

void trace();

#define CHUNK_SIZE 64 // bytes
#define SAFETY_MARGIN 1
char chunk[CHUNK_SIZE+SAFETY_MARGIN];
int chunkHead = 0;

void serial_read_stream() {

    setTargetHotend(0, 0);
    setTargetBed(0);

    lcd_implementation_clear();
    lcd_printPGM(PSTR(" Upload in progress"));

    // first wait for how many bytes we will receive
    uint32_t bytesToReceive;

    // receive the four bytes
    char bytesToReceiveBuffer[4];
    for (int i=0; i<4; i++) {
        int data;
        while ((data = MYSERIAL.read()) == -1) {};
        bytesToReceiveBuffer[i] = data;

    }

    // make it a uint32
    memcpy(&bytesToReceive, &bytesToReceiveBuffer, 4);

    // we're ready, notify the sender
    MYSERIAL.write('+');

    // lock in the routine
    uint32_t receivedBytes = 0;
    while (prusa_sd_card_upload) {
        int i;
        for (i=0; i<CHUNK_SIZE; i++) {
            int data;

            // check if we're not done
            if (receivedBytes == bytesToReceive) {
                break;
            }

            // read the next byte
            while ((data = MYSERIAL.read()) == -1) {};
            receivedBytes++;

            // save it to the chunk
            chunk[i] = data;
        }

        // write the chunk to SD
        card.write_command_no_newline(&chunk[0]);

        // notify the sender we're ready for more data
        MYSERIAL.write('+');

        // for safety
        manage_heater();

        // check if we're done
        if(receivedBytes == bytesToReceive) {
            trace(); // beep
            card.closefile();
            prusa_sd_card_upload = false;
            SERIAL_PROTOCOLLNRPGM(MSG_FILE_SAVED);
            return;
        }

    }
}

#ifdef HOST_KEEPALIVE_FEATURE
/**
* Output a "busy" message at regular intervals
* while the machine is not accepting commands.
*/
void host_keepalive() {
  if (farm_mode) return;
  long ms = millis();
  if (host_keepalive_interval && busy_state != NOT_BUSY) {
    if ((ms - prev_busy_signal_ms) < (long)(1000L * host_keepalive_interval)) return;
     switch (busy_state) {
      case IN_HANDLER:
      case IN_PROCESS:
        SERIAL_ECHO_START;
        SERIAL_ECHOLNPGM("busy: processing");
        break;
      case PAUSED_FOR_USER:
        SERIAL_ECHO_START;
        SERIAL_ECHOLNPGM("busy: paused for user");
        break;
      case PAUSED_FOR_INPUT:
        SERIAL_ECHO_START;
        SERIAL_ECHOLNPGM("busy: paused for input");
        break;
      default:
	break;
    }
  }
  prev_busy_signal_ms = ms;
}
#endif

// The loop() function is called in an endless loop by the Arduino framework from the default main() routine.
// Before loop(), the setup() function is called by the main() routine.
void loop()
{
	KEEPALIVE_STATE(NOT_BUSY);
	if (usb_printing_counter > 0 && millis()-_usb_timer > 1000)
	{
		is_usb_printing = true;
		usb_printing_counter--;
		_usb_timer = millis();
	}
	if (usb_printing_counter == 0)
	{
		is_usb_printing = false;
	}

    if (prusa_sd_card_upload)
    {
        //we read byte-by byte
        serial_read_stream();
    } else 
    {

        get_command();

	#ifdef SDSUPPORT
	card.checkautostart(false);
	#endif
	if(buflen)
	{
	  #ifdef SDSUPPORT
	      if(card.saving)
	      {
	        // Saving a G-code file onto an SD-card is in progress.
	        // Saving starts with M28, saving until M29 is seen.
	        if(strstr_P(CMDBUFFER_CURRENT_STRING, PSTR("M29")) == NULL) {
	          card.write_command(CMDBUFFER_CURRENT_STRING);
	          if(card.logging)
	            process_commands();
	          else
	           SERIAL_PROTOCOLLNRPGM(MSG_OK);
	        } else {
	          card.closefile();
	          SERIAL_PROTOCOLLNRPGM(MSG_FILE_SAVED);
	        }
	      } else {
	        process_commands();
	      }
	  #else
	      process_commands();
	  #endif //SDSUPPORT
	  if (! cmdbuffer_front_already_processed)
	      cmdqueue_pop_front();
	  cmdbuffer_front_already_processed = false;
	  host_keepalive();
	}
    }
    //check heater every n milliseconds
    manage_heater();
    isPrintPaused ? manage_inactivity(true) : manage_inactivity(false);
    checkHitEndstops();
    lcd_update();
}

void proc_commands() {
	if (buflen)
	{
		process_commands();
		if (!cmdbuffer_front_already_processed)
			cmdqueue_pop_front();
		cmdbuffer_front_already_processed = false;
	}
}

void get_command()
{
    // Test and reserve space for the new command string.
	if (!cmdqueue_could_enqueue_back(MAX_CMD_SIZE - 1)) 
		return;
	
	bool rx_buffer_full = false; //flag that serial rx buffer is full

  while (MYSERIAL.available() > 0) {
	  if (MYSERIAL.available() == RX_BUFFER_SIZE - 1) { //compare number of chars buffered in rx buffer with rx buffer size
		  SERIAL_ECHOLNPGM("Full RX Buffer");   //if buffer was full, there is danger that reading of last gcode will not be completed
		  rx_buffer_full = true;				//sets flag that buffer was full	
	  }
    char serial_char = MYSERIAL.read();
    if (selectedSerialPort == 1) {
        selectedSerialPort = 0;
        MYSERIAL.write(serial_char);
        selectedSerialPort = 1;
    }
      TimeSent = millis();
      TimeNow = millis();

    if (serial_char < 0)
        // Ignore extended ASCII characters. These characters have no meaning in the G-code apart from the file names
        // and Marlin does not support such file names anyway.
        // Serial characters with a highest bit set to 1 are generated when the USB cable is unplugged, leading
        // to a hang-up of the print process from an SD card.
        continue;
    if(serial_char == '\n' ||
       serial_char == '\r' ||
		(serial_char == ':' && comment_mode == false) ||
       serial_count >= (MAX_CMD_SIZE - 1) )
    {
      if(!serial_count) { //if empty line
        comment_mode = false; //for new command
        return;
      }
      cmdbuffer[bufindw+serial_count+1] = 0; //terminate string
      if(!comment_mode){
	    // Line numbers must be first in buffer
	    if ((strstr(cmdbuffer+bufindw+1, "PRUSA") == NULL) && 
		(cmdbuffer[bufindw + 1] == 'N')) {

            // Line number met. When sending a G-code over a serial line, each line may be stamped with its index,
            // and Marlin tests, whether the successive lines are stamped with an increasing line number ID.
            gcode_N = (strtol(cmdbuffer + bufindw + 2, NULL, 10));
            if ((gcode_N != gcode_LastN + 1) &&
                (strstr_P(cmdbuffer+bufindw+1, PSTR("M110")) == NULL))
            {
                // Line numbers not sent in succession.
                SERIAL_ERROR_START;
                SERIAL_ERRORRPGM(MSG_ERR_LINE_NO);
                SERIAL_ERRORLN(gcode_LastN);
                //Serial.println(gcode_N);
                FlushSerialRequestResend();
                serial_count = 0;
                return;
            }

            if((strchr_pointer = strchr(cmdbuffer+bufindw+1, '*')) != NULL)
            {
                byte checksum = 0;
                char *p = cmdbuffer+bufindw+1;
                while (p != strchr_pointer)
                    checksum = checksum^(*p++);
                if (int(strtol(strchr_pointer+1, NULL, 10)) != int(checksum)) {
                SERIAL_ERROR_START;
                SERIAL_ERRORRPGM(MSG_ERR_CHECKSUM_MISMATCH);
                SERIAL_ERRORLN(gcode_LastN);
                FlushSerialRequestResend();
                serial_count = 0;
                return;
                }
                // If no errors, remove the checksum and continue parsing.
                *strchr_pointer = 0;
            }
            else
            {
                SERIAL_ERROR_START;
                SERIAL_ERRORRPGM(MSG_ERR_NO_CHECKSUM);
                SERIAL_ERRORLN(gcode_LastN);
                FlushSerialRequestResend();
                serial_count = 0;
                return;
            }

            // Don't parse N again with code_seen('N')
            cmdbuffer[bufindw + 1] = '$';
            //if no errors, continue parsing
            gcode_LastN = gcode_N;
         }

        // if we don't receive 'N' but still see '*'
        if((cmdbuffer[bufindw + 1] != 'N') && (cmdbuffer[bufindw + 1] != '$') &&
	   (strchr(cmdbuffer + bufindw + 1, '*') != NULL))
        {
            SERIAL_ERROR_START;
            SERIAL_ERRORRPGM(MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM);
            SERIAL_ERRORLN(gcode_LastN);
            serial_count = 0;
            return;
        }

        if ((strchr_pointer = strchr(cmdbuffer+bufindw+1, 'G')) != NULL) {
      		  if (! IS_SD_PRINTING) {
        			  usb_printing_counter = 10;
        			  is_usb_printing = true;
      		  }
            if (Stopped == true) {
                int gcode = strtol(strchr_pointer+1, NULL, 10);
                if (gcode >= 0 && gcode <= 3) {
                    SERIAL_ERRORLNRPGM(MSG_ERR_STOPPED);
                    LCD_MESSAGERPGM(MSG_STOPPED);
                }
            }
        } // end of 'G' command

        //If command was e-stop process now
        if(strcmp(cmdbuffer+bufindw+1, "M112") == 0)
          kill();
        
        // Store the current line into buffer, move to the next line.
        cmdbuffer[bufindw] = CMDBUFFER_CURRENT_TYPE_USB;
#ifdef CMDBUFFER_DEBUG
        SERIAL_ECHO_START;
        SERIAL_ECHOPGM("Storing a command line to buffer: ");
        SERIAL_ECHO(cmdbuffer+bufindw+1);
        SERIAL_ECHOLNPGM("");
#endif /* CMDBUFFER_DEBUG */
        bufindw += strlen(cmdbuffer+bufindw+1) + 2;
        if (bufindw == sizeof(cmdbuffer))
            bufindw = 0;
        ++ buflen;
#ifdef CMDBUFFER_DEBUG
        SERIAL_ECHOPGM("Number of commands in the buffer: ");
        SERIAL_ECHO(buflen);
        SERIAL_ECHOLNPGM("");
#endif /* CMDBUFFER_DEBUG */
      } // end of 'not comment mode'
      serial_count = 0; //clear buffer
      // Don't call cmdqueue_could_enqueue_back if there are no characters waiting
      // in the queue, as this function will reserve the memory.
      if (MYSERIAL.available() == 0 || ! cmdqueue_could_enqueue_back(MAX_CMD_SIZE-1))
          return;
    } // end of "end of line" processing
    else {
      // Not an "end of line" symbol. Store the new character into a buffer.
      if(serial_char == ';') comment_mode = true;
      if(!comment_mode) cmdbuffer[bufindw+1+serial_count++] = serial_char;
    }
  } // end of serial line processing loop

    if(farm_mode){
        TimeNow = millis();
        if ( ((TimeNow - TimeSent) > 800) && (serial_count > 0) ) {
            cmdbuffer[bufindw+serial_count+1] = 0;
            
            bufindw += strlen(cmdbuffer+bufindw+1) + 2;
            if (bufindw == sizeof(cmdbuffer))
                bufindw = 0;
            ++ buflen;
            
            serial_count = 0;
            
            SERIAL_ECHOPGM("TIMEOUT:");
            //memset(cmdbuffer, 0 , sizeof(cmdbuffer));
            return;
        }
    }

	//add comment
	if (rx_buffer_full == true && serial_count > 0) {   //if rx buffer was full and string was not properly terminated
		rx_buffer_full = false;
		bufindw = bufindw - serial_count;				//adjust tail of the buffer to prepare buffer for writing new command
		serial_count = 0;
	}

  #ifdef SDSUPPORT
  if(!card.sdprinting || serial_count!=0){
    // If there is a half filled buffer from serial line, wait until return before
    // continuing with the serial line.
     return;
  }

  //'#' stops reading from SD to the buffer prematurely, so procedural macro calls are possible
  // if it occurs, stop_buffering is triggered and the buffer is ran dry.
  // this character _can_ occur in serial com, due to checksums. however, no checksums are used in SD printing

  static bool stop_buffering=false;
  if(buflen==0) stop_buffering=false;

  // Reads whole lines from the SD card. Never leaves a half-filled line in the cmdbuffer.
  while( !card.eof() && !stop_buffering) {
    int16_t n=card.get();
    char serial_char = (char)n;
    if(serial_char == '\n' ||
       serial_char == '\r' ||
       (serial_char == '#' && comment_mode == false) ||
       (serial_char == ':' && comment_mode == false) ||
       serial_count >= (MAX_CMD_SIZE - 1)||n==-1)
    {
      if(card.eof()){
        SERIAL_PROTOCOLLNRPGM(MSG_FILE_PRINTED);
        stoptime=millis();
        char time[30];
        unsigned long t=(stoptime-starttime-pause_time)/1000;
		pause_time = 0;
        int hours, minutes;
        minutes=(t/60)%60;
        hours=t/60/60;
		save_statistics(total_filament_used, t);
		sprintf_P(time, PSTR("%i hours %i minutes"),hours, minutes);
        SERIAL_ECHO_START;
        SERIAL_ECHOLN(time);
        lcd_setstatus(time);
        card.printingHasFinished();
        card.checkautostart(true);

		if (farm_mode)
		{
			prusa_statistics(6);
			lcd_commands_type = LCD_COMMAND_FARM_MODE_CONFIRM;
		}

      }
      if(serial_char=='#')
        stop_buffering=true;

      if(!serial_count)
      {
        comment_mode = false; //for new command
        return; //if empty line
      }
      cmdbuffer[bufindw+serial_count+1] = 0; //terminate string
      cmdbuffer[bufindw] = CMDBUFFER_CURRENT_TYPE_SDCARD;
      ++ buflen;
      bufindw += strlen(cmdbuffer+bufindw+1) + 2;
      if (bufindw == sizeof(cmdbuffer))
          bufindw = 0;
      comment_mode = false; //for new command
      serial_count = 0; //clear buffer
      // The following line will reserve buffer space if available.
      if (! cmdqueue_could_enqueue_back(MAX_CMD_SIZE-1))
          return;
    }
    else
    {
      if(serial_char == ';') comment_mode = true;
      if(!comment_mode) cmdbuffer[bufindw+1+serial_count++] = serial_char;
    }
  }

  #endif //SDSUPPORT
}


// Return True if a character was found
static inline bool    code_seen(char code) { return (strchr_pointer = strchr(CMDBUFFER_CURRENT_STRING, code)) != NULL; }
static inline bool    code_seen(const char *code) { return (strchr_pointer = strstr(CMDBUFFER_CURRENT_STRING, code)) != NULL; }
static inline float   code_value()      { return strtod(strchr_pointer+1, NULL);}
static inline long    code_value_long()    { return strtol(strchr_pointer+1, NULL, 10); }
static inline int16_t code_value_short()   { return int16_t(strtol(strchr_pointer+1, NULL, 10)); };
static inline uint8_t code_value_uint8()   { return uint8_t(strtol(strchr_pointer+1, NULL, 10)); };

static inline float code_value_float() {
  char* e = strchr(strchr_pointer, 'E');
  if (!e) return strtod(strchr_pointer + 1, NULL);
  *e = 0;
  float ret = strtod(strchr_pointer + 1, NULL);
  *e = 'E';
  return ret;
}

#define DEFINE_PGM_READ_ANY(type, reader)       \
    static inline type pgm_read_any(const type *p)  \
    { return pgm_read_##reader##_near(p); }

DEFINE_PGM_READ_ANY(float,       float);
DEFINE_PGM_READ_ANY(signed char, byte);

#define XYZ_CONSTS_FROM_CONFIG(type, array, CONFIG) \
static const PROGMEM type array##_P[3] =        \
    { X_##CONFIG, Y_##CONFIG, Z_##CONFIG };     \
static inline type array(int axis)              \
    { return pgm_read_any(&array##_P[axis]); }  \
type array##_ext(int axis)                      \
    { return pgm_read_any(&array##_P[axis]); }

XYZ_CONSTS_FROM_CONFIG(float, base_min_pos,    MIN_POS);
XYZ_CONSTS_FROM_CONFIG(float, base_max_pos,    MAX_POS);
XYZ_CONSTS_FROM_CONFIG(float, base_home_pos,   HOME_POS);
XYZ_CONSTS_FROM_CONFIG(float, max_length,      MAX_LENGTH);
XYZ_CONSTS_FROM_CONFIG(float, home_retract_mm, HOME_RETRACT_MM);
XYZ_CONSTS_FROM_CONFIG(signed char, home_dir,  HOME_DIR);

static void axis_is_at_home(int axis) {
  current_position[axis] = base_home_pos(axis) + add_homing[axis];
  min_pos[axis] =          base_min_pos(axis) + add_homing[axis];
  max_pos[axis] =          base_max_pos(axis) + add_homing[axis];
}


inline void set_current_to_destination() { memcpy(current_position, destination, sizeof(current_position)); }
inline void set_destination_to_current() { memcpy(destination, current_position, sizeof(destination)); }


static void setup_for_endstop_move(bool enable_endstops_now = true) {
    saved_feedrate = feedrate;
    saved_feedmultiply = feedmultiply;
    feedmultiply = 100;
    previous_millis_cmd = millis();
    
    enable_endstops(enable_endstops_now);
}

static void clean_up_after_endstop_move() {
#ifdef ENDSTOPS_ONLY_FOR_HOMING
    enable_endstops(false);
#endif
    
    feedrate = saved_feedrate;
    feedmultiply = saved_feedmultiply;
    previous_millis_cmd = millis();
}



#ifdef ENABLE_AUTO_BED_LEVELING
#ifdef AUTO_BED_LEVELING_GRID
static void set_bed_level_equation_lsq(double *plane_equation_coefficients)
{
    vector_3 planeNormal = vector_3(-plane_equation_coefficients[0], -plane_equation_coefficients[1], 1);
    planeNormal.debug("planeNormal");
    plan_bed_level_matrix = matrix_3x3::create_look_at(planeNormal);
    //bedLevel.debug("bedLevel");

    //plan_bed_level_matrix.debug("bed level before");
    //vector_3 uncorrected_position = plan_get_position_mm();
    //uncorrected_position.debug("position before");

    vector_3 corrected_position = plan_get_position();
//    corrected_position.debug("position after");
    current_position[X_AXIS] = corrected_position.x;
    current_position[Y_AXIS] = corrected_position.y;
    current_position[Z_AXIS] = corrected_position.z;

    // put the bed at 0 so we don't go below it.
    current_position[Z_AXIS] = zprobe_zoffset; // in the lsq we reach here after raising the extruder due to the loop structure

    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
}

#else // not AUTO_BED_LEVELING_GRID

static void set_bed_level_equation_3pts(float z_at_pt_1, float z_at_pt_2, float z_at_pt_3) {

    plan_bed_level_matrix.set_to_identity();

    vector_3 pt1 = vector_3(ABL_PROBE_PT_1_X, ABL_PROBE_PT_1_Y, z_at_pt_1);
    vector_3 pt2 = vector_3(ABL_PROBE_PT_2_X, ABL_PROBE_PT_2_Y, z_at_pt_2);
    vector_3 pt3 = vector_3(ABL_PROBE_PT_3_X, ABL_PROBE_PT_3_Y, z_at_pt_3);

    vector_3 from_2_to_1 = (pt1 - pt2).get_normal();
    vector_3 from_2_to_3 = (pt3 - pt2).get_normal();
    vector_3 planeNormal = vector_3::cross(from_2_to_1, from_2_to_3).get_normal();
    planeNormal = vector_3(planeNormal.x, planeNormal.y, abs(planeNormal.z));

    plan_bed_level_matrix = matrix_3x3::create_look_at(planeNormal);

    vector_3 corrected_position = plan_get_position();
    current_position[X_AXIS] = corrected_position.x;
    current_position[Y_AXIS] = corrected_position.y;
    current_position[Z_AXIS] = corrected_position.z;

    // put the bed at 0 so we don't go below it.
    current_position[Z_AXIS] = zprobe_zoffset;

    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);

}

#endif // AUTO_BED_LEVELING_GRID

static void run_z_probe() {
    plan_bed_level_matrix.set_to_identity();
    feedrate = homing_feedrate[Z_AXIS];

    // move down until you find the bed
    float zPosition = -10;
    plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], zPosition, current_position[E_AXIS], feedrate/60, active_extruder);
    st_synchronize();

        // we have to let the planner know where we are right now as it is not where we said to go.
    zPosition = st_get_position_mm(Z_AXIS);
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], zPosition, current_position[E_AXIS]);

    // move up the retract distance
    zPosition += home_retract_mm(Z_AXIS);
    plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], zPosition, current_position[E_AXIS], feedrate/60, active_extruder);
    st_synchronize();

    // move back down slowly to find bed
    feedrate = homing_feedrate[Z_AXIS]/4;
    zPosition -= home_retract_mm(Z_AXIS) * 2;
    plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], zPosition, current_position[E_AXIS], feedrate/60, active_extruder);
    st_synchronize();

    current_position[Z_AXIS] = st_get_position_mm(Z_AXIS);
    // make sure the planner knows where we are as it may be a bit different than we last said to move to
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
}

static void do_blocking_move_to(float x, float y, float z) {
    float oldFeedRate = feedrate;

    feedrate = homing_feedrate[Z_AXIS];

    current_position[Z_AXIS] = z;
    plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], feedrate/60, active_extruder);
    st_synchronize();

    feedrate = XY_TRAVEL_SPEED;

    current_position[X_AXIS] = x;
    current_position[Y_AXIS] = y;
    plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], feedrate/60, active_extruder);
    st_synchronize();

    feedrate = oldFeedRate;
}

static void do_blocking_move_relative(float offset_x, float offset_y, float offset_z) {
    do_blocking_move_to(current_position[X_AXIS] + offset_x, current_position[Y_AXIS] + offset_y, current_position[Z_AXIS] + offset_z);
}


/// Probe bed height at position (x,y), returns the measured z value
static float probe_pt(float x, float y, float z_before) {
  // move to right place
  do_blocking_move_to(current_position[X_AXIS], current_position[Y_AXIS], z_before);
  do_blocking_move_to(x - X_PROBE_OFFSET_FROM_EXTRUDER, y - Y_PROBE_OFFSET_FROM_EXTRUDER, current_position[Z_AXIS]);

  run_z_probe();
  float measured_z = current_position[Z_AXIS];

  SERIAL_PROTOCOLRPGM(MSG_BED);
  SERIAL_PROTOCOLPGM(" x: ");
  SERIAL_PROTOCOL(x);
  SERIAL_PROTOCOLPGM(" y: ");
  SERIAL_PROTOCOL(y);
  SERIAL_PROTOCOLPGM(" z: ");
  SERIAL_PROTOCOL(measured_z);
  SERIAL_PROTOCOLPGM("\n");
  return measured_z;
}

#endif // #ifdef ENABLE_AUTO_BED_LEVELING

#ifdef LIN_ADVANCE
  /**
   * M900: Set and/or Get advance K factor
   *
   *  K<factor>                  Set advance K factor
   */
  inline void gcode_M900() {
    st_synchronize();

    const float newK = code_seen('K') ? code_value_float() : -1;
    if (newK >= 0 && newK < 10)
      extruder_advance_K = newK;
    else
      SERIAL_ECHOLNPGM("K out of allowed range!");

    SERIAL_ECHO_START;
    SERIAL_ECHOPGM("Advance K=");
    SERIAL_ECHOLN(extruder_advance_K);
  }
#endif // LIN_ADVANCE

bool check_commands() {
	  bool end_command_found = false;

	  while (buflen)
	  {
		  if ((code_seen("M84")) || (code_seen("M 84"))) end_command_found = true;
		  if (!cmdbuffer_front_already_processed)
			  cmdqueue_pop_front();
		  cmdbuffer_front_already_processed = false;
	  }
	  return end_command_found;
}

void homeaxis(int axis) {
#define HOMEAXIS_DO(LETTER) \
  ((LETTER##_MIN_PIN > -1 && LETTER##_HOME_DIR==-1) || (LETTER##_MAX_PIN > -1 && LETTER##_HOME_DIR==1))

  if (axis==X_AXIS ? HOMEAXIS_DO(X) :
      axis==Y_AXIS ? HOMEAXIS_DO(Y) :
      axis==Z_AXIS ? HOMEAXIS_DO(Z) :
      0) {
    int axis_home_dir = home_dir(axis);

    current_position[axis] = 0;
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);

    destination[axis] = 1.5 * max_length(axis) * axis_home_dir;
    feedrate = homing_feedrate[axis];
    plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
    st_synchronize();

    current_position[axis] = 0;
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
    destination[axis] = -home_retract_mm(axis) * axis_home_dir;
    plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
    st_synchronize();

    destination[axis] = 2*home_retract_mm(axis) * axis_home_dir;
    feedrate = homing_feedrate[axis]/2 ;
    plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
    st_synchronize();
    axis_is_at_home(axis);
    destination[axis] = current_position[axis];
    feedrate = 0.0;
    endstops_hit_on_purpose();
    axis_known_position[axis] = true;
  }
}

void home_xy()
{
    set_destination_to_current();
    homeaxis(X_AXIS);
    homeaxis(Y_AXIS);
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
    endstops_hit_on_purpose();
}

void refresh_cmd_timeout(void)
{
  previous_millis_cmd = millis();
}

#ifdef FWRETRACT
  void retract(bool retracting, bool swapretract = false) {
    if(retracting && !retracted[active_extruder]) {
      destination[X_AXIS]=current_position[X_AXIS];
      destination[Y_AXIS]=current_position[Y_AXIS];
      destination[Z_AXIS]=current_position[Z_AXIS];
      destination[E_AXIS]=current_position[E_AXIS];
      if (swapretract) {
        current_position[E_AXIS]+=retract_length_swap/volumetric_multiplier[active_extruder];
      } else {
        current_position[E_AXIS]+=retract_length/volumetric_multiplier[active_extruder];
      }
      plan_set_e_position(current_position[E_AXIS]);
      float oldFeedrate = feedrate;
      feedrate=retract_feedrate*60;
      retracted[active_extruder]=true;
      prepare_move();
      current_position[Z_AXIS]-=retract_zlift;
      plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
      prepare_move();
      feedrate = oldFeedrate;
    } else if(!retracting && retracted[active_extruder]) {
      destination[X_AXIS]=current_position[X_AXIS];
      destination[Y_AXIS]=current_position[Y_AXIS];
      destination[Z_AXIS]=current_position[Z_AXIS];
      destination[E_AXIS]=current_position[E_AXIS];
      current_position[Z_AXIS]+=retract_zlift;
      plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
      //prepare_move();
      if (swapretract) {
        current_position[E_AXIS]-=(retract_length_swap+retract_recover_length_swap)/volumetric_multiplier[active_extruder]; 
      } else {
        current_position[E_AXIS]-=(retract_length+retract_recover_length)/volumetric_multiplier[active_extruder]; 
      }
      plan_set_e_position(current_position[E_AXIS]);
      float oldFeedrate = feedrate;
      feedrate=retract_recover_feedrate*60;
      retracted[active_extruder]=false;
      prepare_move();
      feedrate = oldFeedrate;
    }
  } //retract
#endif //FWRETRACT

void trace() {
    tone(BEEPER, 440);
    delay(25);
    noTone(BEEPER);
    delay(20);
}
/*
void ramming() {
//	  float tmp[4] = DEFAULT_MAX_FEEDRATE;
	if (current_temperature[0] < 230) {
		//PLA

		max_feedrate[E_AXIS] = 50;
		//current_position[E_AXIS] -= 8;
		//plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 2100 / 60, active_extruder);
		//current_position[E_AXIS] += 8;
		//plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 2100 / 60, active_extruder);
		current_position[E_AXIS] += 5.4;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 2800 / 60, active_extruder);
		current_position[E_AXIS] += 3.2;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 3000 / 60, active_extruder);
		current_position[E_AXIS] += 3;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 3400 / 60, active_extruder);
		st_synchronize();
		max_feedrate[E_AXIS] = 80;
		current_position[E_AXIS] -= 82;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 9500 / 60, active_extruder);
		max_feedrate[E_AXIS] = 50;//tmp[E_AXIS];
		current_position[E_AXIS] -= 20;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 1200 / 60, active_extruder);
		current_position[E_AXIS] += 5;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 400 / 60, active_extruder);
		current_position[E_AXIS] += 5;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 600 / 60, active_extruder);
		current_position[E_AXIS] -= 10;
		st_synchronize();
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 600 / 60, active_extruder);
		current_position[E_AXIS] += 10;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 600 / 60, active_extruder);
		current_position[E_AXIS] -= 10;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 800 / 60, active_extruder);
		current_position[E_AXIS] += 10;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 800 / 60, active_extruder);
		current_position[E_AXIS] -= 10;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 800 / 60, active_extruder);
		st_synchronize();
	}
	else {
		//ABS
		max_feedrate[E_AXIS] = 50;
		//current_position[E_AXIS] -= 8;
		//plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 2100 / 60, active_extruder);
		//current_position[E_AXIS] += 8;
		//plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 2100 / 60, active_extruder);
		current_position[E_AXIS] += 3.1;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 2000 / 60, active_extruder);
		current_position[E_AXIS] += 3.1;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 2500 / 60, active_extruder);
		current_position[E_AXIS] += 4;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 3000 / 60, active_extruder);
		st_synchronize();
		//current_position[X_AXIS] += 23; //delay
		//plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 600/60, active_extruder); //delay
		//current_position[X_AXIS] -= 23; //delay
		//plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 600/60, active_extruder); //delay
		delay(4700);
		max_feedrate[E_AXIS] = 80;
		current_position[E_AXIS] -= 92;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 9900 / 60, active_extruder);
		max_feedrate[E_AXIS] = 50;//tmp[E_AXIS];
		current_position[E_AXIS] -= 5;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 800 / 60, active_extruder);
		current_position[E_AXIS] += 5;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 400 / 60, active_extruder);
		current_position[E_AXIS] -= 5;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 600 / 60, active_extruder);
		st_synchronize();
		current_position[E_AXIS] += 5;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 600 / 60, active_extruder);
		current_position[E_AXIS] -= 5;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 600 / 60, active_extruder);
		current_position[E_AXIS] += 5;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 600 / 60, active_extruder);
		current_position[E_AXIS] -= 5;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 600 / 60, active_extruder);
		st_synchronize();

	}
  }
*/


void gcode_M701() {
#ifdef SNMM
	extr_adj(snmm_extruder);//loads current extruder
#else
	enable_z();
	custom_message = true;
	custom_message_type = 2;

	lcd_setstatuspgm(MSG_LOADING_FILAMENT);
	current_position[E_AXIS] += 70;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 400 / 60, active_extruder); //fast sequence

	current_position[E_AXIS] += 25;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 100 / 60, active_extruder); //slow sequence
	st_synchronize();

	if (!farm_mode && loading_flag) {
		bool clean = lcd_show_fullscreen_message_yes_no_and_wait_P(MSG_FILAMENT_CLEAN, false, true);

		while (!clean) {
			lcd_update_enable(true);
			lcd_update(2);
			current_position[E_AXIS] += 25;
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 100 / 60, active_extruder); //slow sequence
			st_synchronize();
			clean = lcd_show_fullscreen_message_yes_no_and_wait_P(MSG_FILAMENT_CLEAN, false, true);
		}
	}
	lcd_update_enable(true);
	lcd_update(2);
	lcd_setstatuspgm(WELCOME_MSG);
	disable_z();
	loading_flag = false;
	custom_message = false;
	custom_message_type = 0;
#endif
}

bool gcode_M45(bool onlyZ) {
	bool final_result = false;
	if (!onlyZ) {
		setTargetBed(0);
		setTargetHotend(0, 0);
		setTargetHotend(0, 1);
		setTargetHotend(0, 2);
		adjust_bed_reset(); //reset bed level correction
	}

	// Disable the default update procedure of the display. We will do a modal dialog.
	lcd_update_enable(false);
	// Let the planner use the uncorrected coordinates.
	mbl.reset();
	// Reset world2machine_rotation_and_skew and world2machine_shift, therefore
	// the planner will not perform any adjustments in the XY plane. 
	// Wait for the motors to stop and update the current position with the absolute values.
	world2machine_revert_to_uncorrected();
	// Reset the baby step value applied without moving the axes.
	babystep_reset();
	// Mark all axes as in a need for homing.
	memset(axis_known_position, 0, sizeof(axis_known_position));

	// Let the user move the Z axes up to the end stoppers.
	KEEPALIVE_STATE(PAUSED_FOR_USER);
	if (lcd_calibrate_z_end_stop_manual(onlyZ)) {
		KEEPALIVE_STATE(IN_HANDLER);
		refresh_cmd_timeout();
		if (((degHotend(0) > MAX_HOTEND_TEMP_CALIBRATION) || (degBed() > MAX_BED_TEMP_CALIBRATION)) && (!onlyZ)) {
			lcd_wait_for_cool_down();
			lcd_show_fullscreen_message_and_wait_P(MSG_PAPER);
			lcd_display_message_fullscreen_P(MSG_FIND_BED_OFFSET_AND_SKEW_LINE1);
			lcd_implementation_print_at(0, 3, 1);
			lcd_printPGM(MSG_FIND_BED_OFFSET_AND_SKEW_LINE2);
		}

		// Move the print head close to the bed.
		current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], homing_feedrate[Z_AXIS] / 40, active_extruder);
		st_synchronize();

		// Home in the XY plane.
		set_destination_to_current();
		setup_for_endstop_move();
		home_xy();

		int8_t verbosity_level = 0;
		if (code_seen('V')) {
			// Just 'V' without a number counts as V1.
			char c = strchr_pointer[1];
			verbosity_level = (c == ' ' || c == '\t' || c == 0) ? 1 : code_value_short();
		}

		if (onlyZ) {
			clean_up_after_endstop_move();
			// Z only calibration.
			// Load the machine correction matrix
			world2machine_initialize();
			// and correct the current_position to match the transformed coordinate system.
			world2machine_update_current();
			//FIXME
			bool result = sample_mesh_and_store_reference();
			if (result) {
				if (calibration_status() == CALIBRATION_STATUS_Z_CALIBRATION)
					// Shipped, the nozzle height has been set already. The user can start printing now.
					calibration_status_store(CALIBRATION_STATUS_CALIBRATED);
				// babystep_apply();
				final_result = true;
			}
		}
		else {
			//if wizard is active and selftest was succefully completed, we dont want to loose information about it
			if (calibration_status() != 250 || eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE) == 0) {				
				calibration_status_store(CALIBRATION_STATUS_ASSEMBLED);
			}
			// Reset the baby step value and the baby step applied flag.
			eeprom_update_word((uint16_t*)EEPROM_BABYSTEP_Z, 0);
			// Complete XYZ calibration.
			uint8_t point_too_far_mask = 0;
			BedSkewOffsetDetectionResultType result = find_bed_offset_and_skew(verbosity_level, point_too_far_mask);
			clean_up_after_endstop_move();
			// Print head up.
			current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], homing_feedrate[Z_AXIS] / 40, active_extruder);
			st_synchronize();
			if (result >= 0) {
				point_too_far_mask = 0;
				// Second half: The fine adjustment.
				// Let the planner use the uncorrected coordinates.
				mbl.reset();
				world2machine_reset();
				// Home in the XY plane.
				setup_for_endstop_move();
				home_xy();
				result = improve_bed_offset_and_skew(1, verbosity_level, point_too_far_mask);
				clean_up_after_endstop_move();
				// Print head up.
				current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
				plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], homing_feedrate[Z_AXIS] / 40, active_extruder);
				st_synchronize();
				// if (result >= 0) babystep_apply();
			}
			lcd_bed_calibration_show_result(result, point_too_far_mask);
			if (result >= 0) {
				// Calibration valid, the machine should be able to print. Advise the user to run the V2Calibration.gcode.
				calibration_status_store(CALIBRATION_STATUS_LIVE_ADJUST);
				if(eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE) != 1) lcd_show_fullscreen_message_and_wait_P(MSG_BABYSTEP_Z_NOT_SET);
				final_result = true;
			}
		}
	}
	else {
		// Timeouted.
		KEEPALIVE_STATE(IN_HANDLER);
	}
	lcd_update_enable(true);
	return final_result;
}

void process_commands()
{
  #ifdef FILAMENT_RUNOUT_SUPPORT
    SET_INPUT(FR_SENS);
  #endif

#ifdef CMDBUFFER_DEBUG
  SERIAL_ECHOPGM("Processing a GCODE command: ");
  SERIAL_ECHO(cmdbuffer+bufindr+1);
  SERIAL_ECHOLNPGM("");
  SERIAL_ECHOPGM("In cmdqueue: ");
  SERIAL_ECHO(buflen);
  SERIAL_ECHOLNPGM("");
#endif /* CMDBUFFER_DEBUG */
  
  unsigned long codenum; //throw away variable
  char *starpos = NULL;
#ifdef ENABLE_AUTO_BED_LEVELING
  float x_tmp, y_tmp, z_tmp, real_z;
#endif

  // PRUSA GCODES

  KEEPALIVE_STATE(IN_HANDLER);

  if (code_seen("M117")) { //moved to highest priority place to be able to to print strings which includes "G", "PRUSA" and "^"
	  custom_message = true; //fixes using M117 during SD print, but needs to be be updated in future
	  custom_message_type = 2; //fixes using M117 during SD print, but needs to be be updated in future
	  starpos = (strchr(strchr_pointer + 5, '*'));
	  if (starpos != NULL)
		  *(starpos) = '\0';
	  lcd_setstatus(strchr_pointer + 5);
	  custom_message = false;
	  custom_message_type = 0;
  }
  else if(code_seen("PRUSA")){
		if (code_seen("Ping")) {  //PRUSA Ping
			if (farm_mode) {
				PingTime = millis();
				//MYSERIAL.print(farm_no); MYSERIAL.println(": OK");
			}	  
		} else if (code_seen("PRN")) {
		  MYSERIAL.println(status_number);

		} else if (code_seen("RESET")) {
            // careful!
            if (farm_mode) {
                asm volatile("  jmp 0x3E000");
            }
            else {
                MYSERIAL.println("Not in farm mode.");
            }
        } else if (code_seen("fn")) {
		  if (farm_mode) {
			  MYSERIAL.println(farm_no);
		  }
		  else {
			  MYSERIAL.println("Not in farm mode.");
		  }
		  
		}
		else if (code_seen("thx")) {
			no_response = false;
		}else if (code_seen("fv")) {
        // get file version
        #ifdef SDSUPPORT
        card.openFile(strchr_pointer + 3,true);
        while (true) {
            uint16_t readByte = card.get();
            MYSERIAL.write(readByte);
            if (readByte=='\n') {
                break;
            }
        }
        card.closefile();

        #endif // SDSUPPORT

    } else if (code_seen("M28")) {
        trace();
        prusa_sd_card_upload = true;
        card.openFile(strchr_pointer+4,false);
    } else if (code_seen("SN")) {
        if (farm_mode) {
            selectedSerialPort = 0;
            MSerial.write(";S");
            // S/N is:CZPX0917X003XC13518
            int numbersRead = 0;

            while (numbersRead < 19) {
                while (MSerial.available() > 0) {
                    uint8_t serial_char = MSerial.read();
                    selectedSerialPort = 1;
                    MSerial.write(serial_char);
                    numbersRead++;
                    selectedSerialPort = 0;
                }
            }
            selectedSerialPort = 1;
            MSerial.write('\n');
            /*for (int b = 0; b < 3; b++) {
                tone(BEEPER, 110);
                delay(50);
                noTone(BEEPER);
                delay(50);
            }*/
        } else {
            MYSERIAL.println("Not in farm mode.");
        }
    } else if(code_seen("Fir")){

      SERIAL_PROTOCOLLN(FW_version);

    } else if(code_seen("Rev")){

      SERIAL_PROTOCOLLN(FILAMENT_SIZE "-" ELECTRONICS "-" NOZZLE_TYPE );

    } else if(code_seen("Lang")) {
      lcd_force_language_selection();
    } else if(code_seen("Lz")) {
      EEPROM_save_B(EEPROM_BABYSTEP_Z,0);
      
    } else if (code_seen("SERIAL LOW")) {
        MYSERIAL.println("SERIAL LOW");
        MYSERIAL.begin(BAUDRATE);
        return;
    } else if (code_seen("SERIAL HIGH")) {
        MYSERIAL.println("SERIAL HIGH");
        MYSERIAL.begin(115200);
        return;
    } else if(code_seen("Beat")) {
        // Kick farm link timer
        kicktime = millis();

    } else if(code_seen("FR")) {
        // Factory full reset
        factory_reset(0,true);        
    }
    //else if (code_seen('Cal')) {
		//  lcd_calibration();
	  // }

  }  
  else if (code_seen('^')) {
    // nothing, this is a version line
  } else if(code_seen('G'))
  {
    switch((int)code_value())
    {
    case 0: // G0 -> G1
    case 1: // G1
      if(Stopped == false) {

        #ifdef FILAMENT_RUNOUT_SUPPORT
            
            if(READ(FR_SENS)){
			enquecommand_front_P((PSTR(FILAMENT_RUNOUT_SCRIPT)));

/*                        feedmultiplyBckp=feedmultiply;
                        float target[4];
                        float lastpos[4];
                        target[X_AXIS]=current_position[X_AXIS];
                        target[Y_AXIS]=current_position[Y_AXIS];
                        target[Z_AXIS]=current_position[Z_AXIS];
                        target[E_AXIS]=current_position[E_AXIS];
                        lastpos[X_AXIS]=current_position[X_AXIS];
                        lastpos[Y_AXIS]=current_position[Y_AXIS];
                        lastpos[Z_AXIS]=current_position[Z_AXIS];
                        lastpos[E_AXIS]=current_position[E_AXIS];
                        //retract by E
                        
                        target[E_AXIS]+= FILAMENTCHANGE_FIRSTRETRACT ;
                        
                        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 400, active_extruder);


                        target[Z_AXIS]+= FILAMENTCHANGE_ZADD ;

                        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 300, active_extruder);

                        target[X_AXIS]= FILAMENTCHANGE_XPOS ;
                        
                        target[Y_AXIS]= FILAMENTCHANGE_YPOS ;
                         
                 
                        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 70, active_extruder);

                        target[E_AXIS]+= FILAMENTCHANGE_FINALRETRACT ;
                          

                        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 20, active_extruder);

                        //finish moves
                        st_synchronize();
                        //disable extruder steppers so filament can be removed
                        disable_e0();
                        disable_e1();
                        disable_e2();
                        delay(100);
                        
                        //LCD_ALERTMESSAGEPGM(MSG_FILAMENTCHANGE);
                        uint8_t cnt=0;
                        int counterBeep = 0;
                        lcd_wait_interact();
                        while(!lcd_clicked()){
                          cnt++;
                          manage_heater();
                          manage_inactivity(true);
                          //lcd_update();
                          if(cnt==0)
                          {
                          #if BEEPER > 0
                          
                            if (counterBeep== 500){
                              counterBeep = 0;
                              
                            }
                          
                            
                            SET_OUTPUT(BEEPER);
                            if (counterBeep== 0){
                              WRITE(BEEPER,HIGH);
                            }
                            
                            if (counterBeep== 20){
                              WRITE(BEEPER,LOW);
                            }
                            
                            
                            
                          
                            counterBeep++;
                          #else
                      #if !defined(LCD_FEEDBACK_FREQUENCY_HZ) || !defined(LCD_FEEDBACK_FREQUENCY_DURATION_MS)
                              lcd_buzz(1000/6,100);
                      #else
                        lcd_buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS,LCD_FEEDBACK_FREQUENCY_HZ);
                      #endif
                          #endif
                          }
                        }
                        
                        WRITE(BEEPER,LOW);
                        
                        target[E_AXIS]+= FILAMENTCHANGE_FIRSTFEED ;
                        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 20, active_extruder); 
                        
                        
                        target[E_AXIS]+= FILAMENTCHANGE_FINALFEED ;
                        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 2, active_extruder); 
                        
                 
                        
                        
                        
                        lcd_change_fil_state = 0;
                        lcd_loading_filament();
                        while ((lcd_change_fil_state == 0)||(lcd_change_fil_state != 1)){
                        
                          lcd_change_fil_state = 0;
                          lcd_alright();
                          switch(lcd_change_fil_state){
                          
                             case 2:
                                     target[E_AXIS]+= FILAMENTCHANGE_FIRSTFEED ;
                                     plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 20, active_extruder); 
                        
                        
                                     target[E_AXIS]+= FILAMENTCHANGE_FINALFEED ;
                                     plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 2, active_extruder); 
                                      
                                     
                                     lcd_loading_filament();
                                     break;
                             case 3:
                                     target[E_AXIS]+= FILAMENTCHANGE_FINALFEED ;
                                     plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 2, active_extruder); 
                                     lcd_loading_color();
                                     break;
                                          
                             default:
                                     lcd_change_success();
                                     break;
                          }
                          
                        }
                        

                        
                      target[E_AXIS]+= 5;
                      plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 2, active_extruder);
                        
                      target[E_AXIS]+= FILAMENTCHANGE_FIRSTRETRACT;
                      plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 400, active_extruder);
                        

                        //current_position[E_AXIS]=target[E_AXIS]; //the long retract of L is compensated by manual filament feeding
                        //plan_set_e_position(current_position[E_AXIS]);
                        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 70, active_extruder); //should do nothing
                        plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], target[Z_AXIS], target[E_AXIS], 70, active_extruder); //move xy back
                        plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], lastpos[Z_AXIS], target[E_AXIS], 200, active_extruder); //move z back
                        
                        
                        target[E_AXIS]= target[E_AXIS] - FILAMENTCHANGE_FIRSTRETRACT;
                        
                      
                             
                        plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], lastpos[Z_AXIS], target[E_AXIS], 5, active_extruder); //final untretract
                        
                        
                        plan_set_e_position(lastpos[E_AXIS]);
                        
                        feedmultiply=feedmultiplyBckp;
                        
                     
                        
                        char cmd[9];

                        sprintf_P(cmd, PSTR("M220 S%i"), feedmultiplyBckp);
                        enquecommand(cmd);
*/
            }



        #endif


        get_coordinates(); // For X Y Z E F
		if (total_filament_used > ((current_position[E_AXIS] - destination[E_AXIS]) * 100)) { //protection against total_filament_used overflow
			total_filament_used = total_filament_used + ((destination[E_AXIS] - current_position[E_AXIS]) * 100);
		}
          #ifdef FWRETRACT
            if(autoretract_enabled)
            if( !(code_seen('X') || code_seen('Y') || code_seen('Z')) && code_seen('E')) {
              float echange=destination[E_AXIS]-current_position[E_AXIS];

              if((echange<-MIN_RETRACT && !retracted[active_extruder]) || (echange>MIN_RETRACT && retracted[active_extruder])) { //move appears to be an attempt to retract or recover
                  current_position[E_AXIS] = destination[E_AXIS]; //hide the slicer-generated retract/recover from calculations
                  plan_set_e_position(current_position[E_AXIS]); //AND from the planner
                  retract(!retracted[active_extruder]);
                  return;
              }


            }
          #endif //FWRETRACT
        prepare_move();
        //ClearToSend();
      }
      break;
    case 2: // G2  - CW ARC
      if(Stopped == false) {
        get_arc_coordinates();
        prepare_arc_move(true);
      }
      break;
    case 3: // G3  - CCW ARC
      if(Stopped == false) {
        get_arc_coordinates();
        prepare_arc_move(false);
      }
      break;
    case 4: // G4 dwell   
      codenum = 0;
      if(code_seen('P')) codenum = code_value(); // milliseconds to wait
      if(code_seen('S')) codenum = code_value() * 1000; // seconds to wait
	  if(codenum != 0) LCD_MESSAGERPGM(MSG_DWELL);
      st_synchronize();
      codenum += millis();  // keep track of when we started waiting
      previous_millis_cmd = millis();
      while(millis() < codenum) {
        manage_heater();
        manage_inactivity();
        lcd_update();
      }
      break;
      #ifdef FWRETRACT
      case 10: // G10 retract
       #if EXTRUDERS > 1
        retracted_swap[active_extruder]=(code_seen('S') && code_value_long() == 1); // checks for swap retract argument
        retract(true,retracted_swap[active_extruder]);
       #else
        retract(true);
       #endif
      break;
      case 11: // G11 retract_recover
       #if EXTRUDERS > 1
        retract(false,retracted_swap[active_extruder]);
       #else
        retract(false);
       #endif 
      break;
      #endif //FWRETRACT
    case 28: //G28 Home all Axis one at a time
		homing_flag = true;
		#ifdef ENABLE_AUTO_BED_LEVELING
      plan_bed_level_matrix.set_to_identity();  //Reset the plane ("erase" all leveling data)
#endif //ENABLE_AUTO_BED_LEVELING
            
	      
        // For mesh bed leveling deactivate the matrix temporarily
        #ifdef MESH_BED_LEVELING
            mbl.active = 0;
        #endif

      // Reset world2machine_rotation_and_skew and world2machine_shift, therefore
      // the planner will not perform any adjustments in the XY plane. 
      // Wait for the motors to stop and update the current position with the absolute values.
      world2machine_revert_to_uncorrected();

      // Reset baby stepping to zero, if the babystepping has already been loaded before. The babystepsTodo value will be
      // consumed during the first movements following this statement.
      babystep_undo();

      saved_feedrate = feedrate;
      saved_feedmultiply = feedmultiply;
      feedmultiply = 100;
      previous_millis_cmd = millis();

      enable_endstops(true);

      for(int8_t i=0; i < NUM_AXIS; i++)
          destination[i] = current_position[i];
      feedrate = 0.0;

      home_all_axis = !((code_seen(axis_codes[X_AXIS])) || (code_seen(axis_codes[Y_AXIS])) || (code_seen(axis_codes[Z_AXIS])));

      #if Z_HOME_DIR > 0                      // If homing away from BED do Z first
      if((home_all_axis) || (code_seen(axis_codes[Z_AXIS]))) {
        homeaxis(Z_AXIS);
      }
      #endif

      #ifdef QUICK_HOME
      // In the quick mode, if both x and y are to be homed, a diagonal move will be performed initially.
      if((home_all_axis)||( code_seen(axis_codes[X_AXIS]) && code_seen(axis_codes[Y_AXIS])) )  //first diagonal move
      {
        current_position[X_AXIS] = 0;current_position[Y_AXIS] = 0;

        int x_axis_home_dir = home_dir(X_AXIS);

        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        destination[X_AXIS] = 1.5 * max_length(X_AXIS) * x_axis_home_dir;destination[Y_AXIS] = 1.5 * max_length(Y_AXIS) * home_dir(Y_AXIS);
        feedrate = homing_feedrate[X_AXIS];
        if(homing_feedrate[Y_AXIS]<feedrate)
          feedrate = homing_feedrate[Y_AXIS];
        if (max_length(X_AXIS) > max_length(Y_AXIS)) {
          feedrate *= sqrt(pow(max_length(Y_AXIS) / max_length(X_AXIS), 2) + 1);
        } else {
          feedrate *= sqrt(pow(max_length(X_AXIS) / max_length(Y_AXIS), 2) + 1);
        }
        plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
        st_synchronize();

        axis_is_at_home(X_AXIS);
        axis_is_at_home(Y_AXIS);
        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        destination[X_AXIS] = current_position[X_AXIS];
        destination[Y_AXIS] = current_position[Y_AXIS];
        plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
        feedrate = 0.0;
        st_synchronize();
        endstops_hit_on_purpose();

        current_position[X_AXIS] = destination[X_AXIS];
        current_position[Y_AXIS] = destination[Y_AXIS];
        current_position[Z_AXIS] = destination[Z_AXIS];
      }
      #endif /* QUICK_HOME */

	 
      if((home_all_axis) || (code_seen(axis_codes[X_AXIS])))
        homeaxis(X_AXIS);

      if((home_all_axis) || (code_seen(axis_codes[Y_AXIS])))
        homeaxis(Y_AXIS);

      if(code_seen(axis_codes[X_AXIS]) && code_value_long() != 0)
        current_position[X_AXIS]=code_value()+add_homing[X_AXIS];

      if(code_seen(axis_codes[Y_AXIS]) && code_value_long() != 0)
		    current_position[Y_AXIS]=code_value()+add_homing[Y_AXIS];

      #if Z_HOME_DIR < 0                      // If homing towards BED do Z last
        #ifndef Z_SAFE_HOMING
          if((home_all_axis) || (code_seen(axis_codes[Z_AXIS]))) {
            #if defined (Z_RAISE_BEFORE_HOMING) && (Z_RAISE_BEFORE_HOMING > 0)
              destination[Z_AXIS] = Z_RAISE_BEFORE_HOMING * home_dir(Z_AXIS) * (-1);    // Set destination away from bed
              feedrate = max_feedrate[Z_AXIS];
              plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate, active_extruder);
              st_synchronize();
            #endif // defined (Z_RAISE_BEFORE_HOMING) && (Z_RAISE_BEFORE_HOMING > 0)
            #if (defined(MESH_BED_LEVELING) && !defined(MK1BP))  // If Mesh bed leveling, moxve X&Y to safe position for home
      			  if (!(axis_known_position[X_AXIS] && axis_known_position[Y_AXIS] )) 
      			  {
                homeaxis(X_AXIS);
                homeaxis(Y_AXIS);
      			  } 
              // 1st mesh bed leveling measurement point, corrected.
              world2machine_initialize();
              world2machine(pgm_read_float(bed_ref_points), pgm_read_float(bed_ref_points+1), destination[X_AXIS], destination[Y_AXIS]);
              world2machine_reset();
              if (destination[Y_AXIS] < Y_MIN_POS)
                  destination[Y_AXIS] = Y_MIN_POS;
              destination[Z_AXIS] = MESH_HOME_Z_SEARCH;    // Set destination away from bed
              feedrate = homing_feedrate[Z_AXIS]/10;
              current_position[Z_AXIS] = 0;
              enable_endstops(false);
              plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
              plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate, active_extruder);
              st_synchronize();
              current_position[X_AXIS] = destination[X_AXIS];
              current_position[Y_AXIS] = destination[Y_AXIS];
              enable_endstops(true);
              endstops_hit_on_purpose();
              homeaxis(Z_AXIS);
            #else // MESH_BED_LEVELING
              homeaxis(Z_AXIS);
            #endif // MESH_BED_LEVELING
          }
        #else // defined(Z_SAFE_HOMING): Z Safe mode activated.
          if(home_all_axis) {
            destination[X_AXIS] = round(Z_SAFE_HOMING_X_POINT - X_PROBE_OFFSET_FROM_EXTRUDER);
            destination[Y_AXIS] = round(Z_SAFE_HOMING_Y_POINT - Y_PROBE_OFFSET_FROM_EXTRUDER);
            destination[Z_AXIS] = Z_RAISE_BEFORE_HOMING * home_dir(Z_AXIS) * (-1);    // Set destination away from bed
            feedrate = XY_TRAVEL_SPEED/60;
            current_position[Z_AXIS] = 0;

            plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
            plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate, active_extruder);
            st_synchronize();
            current_position[X_AXIS] = destination[X_AXIS];
            current_position[Y_AXIS] = destination[Y_AXIS];

            homeaxis(Z_AXIS);
          }
                                                // Let's see if X and Y are homed and probe is inside bed area.
          if(code_seen(axis_codes[Z_AXIS])) {
            if ( (axis_known_position[X_AXIS]) && (axis_known_position[Y_AXIS]) \
              && (current_position[X_AXIS]+X_PROBE_OFFSET_FROM_EXTRUDER >= X_MIN_POS) \
              && (current_position[X_AXIS]+X_PROBE_OFFSET_FROM_EXTRUDER <= X_MAX_POS) \
              && (current_position[Y_AXIS]+Y_PROBE_OFFSET_FROM_EXTRUDER >= Y_MIN_POS) \
              && (current_position[Y_AXIS]+Y_PROBE_OFFSET_FROM_EXTRUDER <= Y_MAX_POS)) {

              current_position[Z_AXIS] = 0;
              plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
              destination[Z_AXIS] = Z_RAISE_BEFORE_HOMING * home_dir(Z_AXIS) * (-1);    // Set destination away from bed
              feedrate = max_feedrate[Z_AXIS];
              plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate, active_extruder);
              st_synchronize();

              homeaxis(Z_AXIS);
            } else if (!((axis_known_position[X_AXIS]) && (axis_known_position[Y_AXIS]))) {
                LCD_MESSAGERPGM(MSG_POSITION_UNKNOWN);
                SERIAL_ECHO_START;
                SERIAL_ECHOLNRPGM(MSG_POSITION_UNKNOWN);
            } else {
                LCD_MESSAGERPGM(MSG_ZPROBE_OUT);
                SERIAL_ECHO_START;
                SERIAL_ECHOLNRPGM(MSG_ZPROBE_OUT);
            }
          }
        #endif // Z_SAFE_HOMING
      #endif // Z_HOME_DIR < 0

      if(code_seen(axis_codes[Z_AXIS])) {
        if(code_value_long() != 0) {
          current_position[Z_AXIS]=code_value()+add_homing[Z_AXIS];
        }
      }
      #ifdef ENABLE_AUTO_BED_LEVELING
        if((home_all_axis) || (code_seen(axis_codes[Z_AXIS]))) {
          current_position[Z_AXIS] += zprobe_zoffset;  //Add Z_Probe offset (the distance is negative)
        }
      #endif
  
      plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);

      #ifdef ENDSTOPS_ONLY_FOR_HOMING
        enable_endstops(false);
      #endif

      feedrate = saved_feedrate;
      feedmultiply = saved_feedmultiply;
      previous_millis_cmd = millis();
      endstops_hit_on_purpose();
#ifndef MESH_BED_LEVELING
      // If MESH_BED_LEVELING is not active, then it is the original Prusa i3.
      // Offer the user to load the baby step value, which has been adjusted at the previous print session.
      if(card.sdprinting && eeprom_read_word((uint16_t *)EEPROM_BABYSTEP_Z))
          lcd_adjust_z();
#endif

    // Load the machine correction matrix
    world2machine_initialize();
    // and correct the current_position to match the transformed coordinate system.
    world2machine_update_current();

#if (defined(MESH_BED_LEVELING) && !defined(MK1BP))
	if (code_seen(axis_codes[X_AXIS]) || code_seen(axis_codes[Y_AXIS]) || code_seen('W') || code_seen(axis_codes[Z_AXIS]))
		{
		}
	else
		{
			st_synchronize();
			homing_flag = false;
			// Push the commands to the front of the message queue in the reverse order!
			// There shall be always enough space reserved for these commands.
			// enquecommand_front_P((PSTR("G80")));
			goto case_G80;
	  }
#endif

	  if (farm_mode) { prusa_statistics(20); };

	  homing_flag = false;

      break;

#ifdef ENABLE_AUTO_BED_LEVELING
    case 29: // G29 Detailed Z-Probe, probes the bed at 3 or more points.
        {
            #if Z_MIN_PIN == -1
            #error "You must have a Z_MIN endstop in order to enable Auto Bed Leveling feature! Z_MIN_PIN must point to a valid hardware pin."
            #endif

            // Prevent user from running a G29 without first homing in X and Y
            if (! (axis_known_position[X_AXIS] && axis_known_position[Y_AXIS]) )
            {
                LCD_MESSAGERPGM(MSG_POSITION_UNKNOWN);
                SERIAL_ECHO_START;
                SERIAL_ECHOLNRPGM(MSG_POSITION_UNKNOWN);
                break; // abort G29, since we don't know where we are
            }

            st_synchronize();
            // make sure the bed_level_rotation_matrix is identity or the planner will get it incorectly
            //vector_3 corrected_position = plan_get_position_mm();
            //corrected_position.debug("position before G29");
            plan_bed_level_matrix.set_to_identity();
            vector_3 uncorrected_position = plan_get_position();
            //uncorrected_position.debug("position durring G29");
            current_position[X_AXIS] = uncorrected_position.x;
            current_position[Y_AXIS] = uncorrected_position.y;
            current_position[Z_AXIS] = uncorrected_position.z;
            plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
            setup_for_endstop_move();

            feedrate = homing_feedrate[Z_AXIS];
#ifdef AUTO_BED_LEVELING_GRID
            // probe at the points of a lattice grid

            int xGridSpacing = (RIGHT_PROBE_BED_POSITION - LEFT_PROBE_BED_POSITION) / (AUTO_BED_LEVELING_GRID_POINTS-1);
            int yGridSpacing = (BACK_PROBE_BED_POSITION - FRONT_PROBE_BED_POSITION) / (AUTO_BED_LEVELING_GRID_POINTS-1);


            // solve the plane equation ax + by + d = z
            // A is the matrix with rows [x y 1] for all the probed points
            // B is the vector of the Z positions
            // the normal vector to the plane is formed by the coefficients of the plane equation in the standard form, which is Vx*x+Vy*y+Vz*z+d = 0
            // so Vx = -a Vy = -b Vz = 1 (we want the vector facing towards positive Z

            // "A" matrix of the linear system of equations
            double eqnAMatrix[AUTO_BED_LEVELING_GRID_POINTS*AUTO_BED_LEVELING_GRID_POINTS*3];
            // "B" vector of Z points
            double eqnBVector[AUTO_BED_LEVELING_GRID_POINTS*AUTO_BED_LEVELING_GRID_POINTS];


            int probePointCounter = 0;
            bool zig = true;

            for (int yProbe=FRONT_PROBE_BED_POSITION; yProbe <= BACK_PROBE_BED_POSITION; yProbe += yGridSpacing)
            {
              int xProbe, xInc;
              if (zig)
              {
                xProbe = LEFT_PROBE_BED_POSITION;
                //xEnd = RIGHT_PROBE_BED_POSITION;
                xInc = xGridSpacing;
                zig = false;
              } else // zag
              {
                xProbe = RIGHT_PROBE_BED_POSITION;
                //xEnd = LEFT_PROBE_BED_POSITION;
                xInc = -xGridSpacing;
                zig = true;
              }

              for (int xCount=0; xCount < AUTO_BED_LEVELING_GRID_POINTS; xCount++)
              {
                float z_before;
                if (probePointCounter == 0)
                {
                  // raise before probing
                  z_before = Z_RAISE_BEFORE_PROBING;
                } else
                {
                  // raise extruder
                  z_before = current_position[Z_AXIS] + Z_RAISE_BETWEEN_PROBINGS;
                }

                float measured_z = probe_pt(xProbe, yProbe, z_before);

                eqnBVector[probePointCounter] = measured_z;

                eqnAMatrix[probePointCounter + 0*AUTO_BED_LEVELING_GRID_POINTS*AUTO_BED_LEVELING_GRID_POINTS] = xProbe;
                eqnAMatrix[probePointCounter + 1*AUTO_BED_LEVELING_GRID_POINTS*AUTO_BED_LEVELING_GRID_POINTS] = yProbe;
                eqnAMatrix[probePointCounter + 2*AUTO_BED_LEVELING_GRID_POINTS*AUTO_BED_LEVELING_GRID_POINTS] = 1;
                probePointCounter++;
                xProbe += xInc;
              }
            }
            clean_up_after_endstop_move();

            // solve lsq problem
            double *plane_equation_coefficients = qr_solve(AUTO_BED_LEVELING_GRID_POINTS*AUTO_BED_LEVELING_GRID_POINTS, 3, eqnAMatrix, eqnBVector);

            SERIAL_PROTOCOLPGM("Eqn coefficients: a: ");
            SERIAL_PROTOCOL(plane_equation_coefficients[0]);
            SERIAL_PROTOCOLPGM(" b: ");
            SERIAL_PROTOCOL(plane_equation_coefficients[1]);
            SERIAL_PROTOCOLPGM(" d: ");
            SERIAL_PROTOCOLLN(plane_equation_coefficients[2]);


            set_bed_level_equation_lsq(plane_equation_coefficients);

            free(plane_equation_coefficients);

#else // AUTO_BED_LEVELING_GRID not defined

            // Probe at 3 arbitrary points
            // probe 1
            float z_at_pt_1 = probe_pt(ABL_PROBE_PT_1_X, ABL_PROBE_PT_1_Y, Z_RAISE_BEFORE_PROBING);

            // probe 2
            float z_at_pt_2 = probe_pt(ABL_PROBE_PT_2_X, ABL_PROBE_PT_2_Y, current_position[Z_AXIS] + Z_RAISE_BETWEEN_PROBINGS);

            // probe 3
            float z_at_pt_3 = probe_pt(ABL_PROBE_PT_3_X, ABL_PROBE_PT_3_Y, current_position[Z_AXIS] + Z_RAISE_BETWEEN_PROBINGS);

            clean_up_after_endstop_move();

            set_bed_level_equation_3pts(z_at_pt_1, z_at_pt_2, z_at_pt_3);


#endif // AUTO_BED_LEVELING_GRID
            st_synchronize();

            // The following code correct the Z height difference from z-probe position and hotend tip position.
            // The Z height on homing is measured by Z-Probe, but the probe is quite far from the hotend.
            // When the bed is uneven, this height must be corrected.
            real_z = float(st_get_position(Z_AXIS))/axis_steps_per_unit[Z_AXIS];  //get the real Z (since the auto bed leveling is already correcting the plane)
            x_tmp = current_position[X_AXIS] + X_PROBE_OFFSET_FROM_EXTRUDER;
            y_tmp = current_position[Y_AXIS] + Y_PROBE_OFFSET_FROM_EXTRUDER;
            z_tmp = current_position[Z_AXIS];

            apply_rotation_xyz(plan_bed_level_matrix, x_tmp, y_tmp, z_tmp);         //Apply the correction sending the probe offset
            current_position[Z_AXIS] = z_tmp - real_z + current_position[Z_AXIS];   //The difference is added to current position and sent to planner.
            plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        }
        break;
#ifndef Z_PROBE_SLED
    case 30: // G30 Single Z Probe
        {
            st_synchronize();
            // TODO: make sure the bed_level_rotation_matrix is identity or the planner will get set incorectly
            setup_for_endstop_move();

            feedrate = homing_feedrate[Z_AXIS];

            run_z_probe();
            SERIAL_PROTOCOLPGM(MSG_BED);
            SERIAL_PROTOCOLPGM(" X: ");
            SERIAL_PROTOCOL(current_position[X_AXIS]);
            SERIAL_PROTOCOLPGM(" Y: ");
            SERIAL_PROTOCOL(current_position[Y_AXIS]);
            SERIAL_PROTOCOLPGM(" Z: ");
            SERIAL_PROTOCOL(current_position[Z_AXIS]);
            SERIAL_PROTOCOLPGM("\n");

            clean_up_after_endstop_move();
        }
        break;
#else
    case 31: // dock the sled
        dock_sled(true);
        break;
    case 32: // undock the sled
        dock_sled(false);
        break;
#endif // Z_PROBE_SLED
#endif // ENABLE_AUTO_BED_LEVELING
            
#ifdef MESH_BED_LEVELING
    case 30: // G30 Single Z Probe
        {
            st_synchronize();
            // TODO: make sure the bed_level_rotation_matrix is identity or the planner will get set incorectly
            setup_for_endstop_move();

            feedrate = homing_feedrate[Z_AXIS];

            find_bed_induction_sensor_point_z(-10.f, 3);
            SERIAL_PROTOCOLRPGM(MSG_BED);
            SERIAL_PROTOCOLPGM(" X: ");
            MYSERIAL.print(current_position[X_AXIS], 5);
            SERIAL_PROTOCOLPGM(" Y: ");
            MYSERIAL.print(current_position[Y_AXIS], 5);
            SERIAL_PROTOCOLPGM(" Z: ");
            MYSERIAL.print(current_position[Z_AXIS], 5);
            SERIAL_PROTOCOLPGM("\n");
            clean_up_after_endstop_move();
        }
        break;
	

	case 75:
	{
		for (int i = 40; i <= 110; i++) {
			MYSERIAL.print(i);
			MYSERIAL.print("  ");
			MYSERIAL.println(temp_comp_interpolation(i));// / axis_steps_per_unit[Z_AXIS]);
		}
	}
	break;

	case 76: //PINDA probe temperature calibration
	{
		setTargetBed(PINDA_MIN_T);
		float zero_z;
		int z_shift = 0; //unit: steps
		int t_c; // temperature

		if (!(axis_known_position[X_AXIS] && axis_known_position[Y_AXIS] && axis_known_position[Z_AXIS])) {
			// We don't know where we are! HOME!
			// Push the commands to the front of the message queue in the reverse order!
			// There shall be always enough space reserved for these commands.
			repeatcommand_front(); // repeat G76 with all its parameters
			enquecommand_front_P((PSTR("G28 W0")));
			break;
		}
		KEEPALIVE_STATE(NOT_BUSY); //no need to print busy messages as we print current temperatures periodicaly
		SERIAL_ECHOLNPGM("PINDA probe calibration start");
		custom_message = true;
		custom_message_type = 4;
		custom_message_state = 1;
		custom_message = MSG_TEMP_CALIBRATION;
		current_position[X_AXIS] = PINDA_PREHEAT_X;
		current_position[Y_AXIS] = PINDA_PREHEAT_Y;
		current_position[Z_AXIS] = PINDA_PREHEAT_Z;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 3000 / 60, active_extruder);
		st_synchronize();
		
		while (abs(degBed() - PINDA_MIN_T) > 1) {
			delay_keep_alive(1000);
			serialecho_temperatures();
		}
		
		//enquecommand_P(PSTR("M190 S50"));
		for (int i = 0; i < PINDA_HEAT_T; i++) {
			delay_keep_alive(1000);
			serialecho_temperatures();
		}
		eeprom_update_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, 0); //invalidate temp. calibration in case that in will be aborted during the calibration process 

		current_position[Z_AXIS] = 5;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 3000 / 60, active_extruder);

		current_position[X_AXIS] = pgm_read_float(bed_ref_points);
		current_position[Y_AXIS] = pgm_read_float(bed_ref_points + 1);
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 3000 / 60, active_extruder);
		st_synchronize();
		
		find_bed_induction_sensor_point_z(-1.f);
		zero_z = current_position[Z_AXIS];

		//current_position[Z_AXIS]
		SERIAL_ECHOLNPGM("");
		SERIAL_ECHOPGM("ZERO: ");
		MYSERIAL.print(current_position[Z_AXIS]);
		SERIAL_ECHOLNPGM("");

		for (int i = 0; i<5; i++) {
			SERIAL_ECHOPGM("Step: ");
			MYSERIAL.print(i+2);
			SERIAL_ECHOLNPGM("/6");
			custom_message_state = i + 2;
			t_c = 60 + i * 10;

			setTargetBed(t_c);
			current_position[X_AXIS] = PINDA_PREHEAT_X;
			current_position[Y_AXIS] = PINDA_PREHEAT_Y;
			current_position[Z_AXIS] = PINDA_PREHEAT_Z;
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 3000 / 60, active_extruder);
			st_synchronize();
			while (degBed() < t_c) {
				delay_keep_alive(1000);
				serialecho_temperatures();
			}
			for (int i = 0; i < PINDA_HEAT_T; i++) {
				delay_keep_alive(1000);
				serialecho_temperatures();
			}
			current_position[Z_AXIS] = 5;
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 3000 / 60, active_extruder);
			current_position[X_AXIS] = pgm_read_float(bed_ref_points);
			current_position[Y_AXIS] = pgm_read_float(bed_ref_points + 1);
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 3000 / 60, active_extruder);
			st_synchronize();
			find_bed_induction_sensor_point_z(-1.f);
			z_shift = (int)((current_position[Z_AXIS] - zero_z)*axis_steps_per_unit[Z_AXIS]);

			SERIAL_ECHOLNPGM("");
			SERIAL_ECHOPGM("Temperature: ");
			MYSERIAL.print(t_c);
			SERIAL_ECHOPGM(" Z shift (mm):");
			MYSERIAL.print(current_position[Z_AXIS] - zero_z);
			SERIAL_ECHOLNPGM("");

			EEPROM_save_B(EEPROM_PROBE_TEMP_SHIFT + i*2, &z_shift);
			
		
		}
		custom_message_type = 0;
		custom_message = false;

		eeprom_update_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, 1);
		SERIAL_ECHOLNPGM("Temperature calibration done. Continue with pressing the knob.");
			disable_x();
			disable_y();
			disable_z();
			disable_e0();
			disable_e1();
			disable_e2();
			setTargetBed(0); //set bed target temperature back to 0
		lcd_show_fullscreen_message_and_wait_P(MSG_TEMP_CALIBRATION_DONE);
		lcd_update_enable(true);
		lcd_update(2);		

		

	}
	break;

#ifdef DIS
	case 77:
	{
		//G77 X200 Y150 XP100 YP15 XO10 Y015

		//for 9 point mesh bed leveling G77 X203 Y196 XP3 YP3 XO0 YO0


		//G77 X232 Y218 XP116 YP109 XO-11 YO0 

		float dimension_x = 40;
		float dimension_y = 40;
		int points_x = 40;
		int points_y = 40;
		float offset_x = 74;
		float offset_y = 33;

		if (code_seen('X')) dimension_x = code_value();
		if (code_seen('Y')) dimension_y = code_value();
		if (code_seen('XP')) points_x = code_value();
		if (code_seen('YP')) points_y = code_value();
		if (code_seen('XO')) offset_x = code_value();
		if (code_seen('YO')) offset_y = code_value();
		
		bed_analysis(dimension_x,dimension_y,points_x,points_y,offset_x,offset_y);
		
	} break;
	
#endif

	/**
	* G80: Mesh-based Z probe, probes a grid and produces a
	*      mesh to compensate for variable bed height
	*
	* The S0 report the points as below
	*
	*  +----> X-axis
	*  |
	*  |
	*  v Y-axis
	*
	*/

	case 80:
#ifdef MK1BP
		break;
#endif //MK1BP
	case_G80:
	{
		mesh_bed_leveling_flag = true;
		int8_t verbosity_level = 0;
		static bool run = false;

		if (code_seen('V')) {
			// Just 'V' without a number counts as V1.
			char c = strchr_pointer[1];
			verbosity_level = (c == ' ' || c == '\t' || c == 0) ? 1 : code_value_short();
		}
		// Firstly check if we know where we are
		if (!(axis_known_position[X_AXIS] && axis_known_position[Y_AXIS] && axis_known_position[Z_AXIS])) {
			// We don't know where we are! HOME!
			// Push the commands to the front of the message queue in the reverse order!
			// There shall be always enough space reserved for these commands.
			if (lcd_commands_type != LCD_COMMAND_STOP_PRINT) {
				repeatcommand_front(); // repeat G80 with all its parameters
				enquecommand_front_P((PSTR("G28 W0")));
			}
			else {
				mesh_bed_leveling_flag = false;
			}
			break;
		} 
		
		if (run == false && temp_cal_active == true && calibration_status_pinda() == true && target_temperature_bed >= 50) {
			if (lcd_commands_type != LCD_COMMAND_STOP_PRINT) {
				temp_compensation_start();
				run = true;
				repeatcommand_front(); // repeat G80 with all its parameters
				enquecommand_front_P((PSTR("G28 W0")));
			}
			else {
				mesh_bed_leveling_flag = false;
			}
			break;
		}
		run = false;
		if (lcd_commands_type == LCD_COMMAND_STOP_PRINT) {
			mesh_bed_leveling_flag = false;
			break;
		}
		// Save custom message state, set a new custom message state to display: Calibrating point 9.
		bool custom_message_old = custom_message;
		unsigned int custom_message_type_old = custom_message_type;
		unsigned int custom_message_state_old = custom_message_state;
		custom_message = true;
		custom_message_type = 1;
		custom_message_state = (MESH_MEAS_NUM_X_POINTS * MESH_MEAS_NUM_Y_POINTS) + 10;
		lcd_update(1);

		mbl.reset(); //reset mesh bed leveling

					 // Reset baby stepping to zero, if the babystepping has already been loaded before. The babystepsTodo value will be
					 // consumed during the first movements following this statement.
		babystep_undo();

		// Cycle through all points and probe them
		// First move up. During this first movement, the babystepping will be reverted.
		current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], homing_feedrate[Z_AXIS] / 60, active_extruder);
		// The move to the first calibration point.
		current_position[X_AXIS] = pgm_read_float(bed_ref_points);
		current_position[Y_AXIS] = pgm_read_float(bed_ref_points + 1);
		bool clamped = world2machine_clamp(current_position[X_AXIS], current_position[Y_AXIS]);

		#ifdef SUPPORT_VERBOSITY
		if (verbosity_level >= 1) {
			clamped ? SERIAL_PROTOCOLPGM("First calibration point clamped.\n") : SERIAL_PROTOCOLPGM("No clamping for first calibration point.\n");
		}
		#endif // SUPPORT_VERBOSITY
		//            mbl.get_meas_xy(0, 0, current_position[X_AXIS], current_position[Y_AXIS], false);            
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], homing_feedrate[X_AXIS] / 30, active_extruder);
		// Wait until the move is finished.
		st_synchronize();

		int mesh_point = 0; //index number of calibration point

		int ix = 0;
		int iy = 0;

		int XY_AXIS_FEEDRATE = homing_feedrate[X_AXIS] / 20;
		int Z_LIFT_FEEDRATE = homing_feedrate[Z_AXIS] / 40;
		bool has_z = is_bed_z_jitter_data_valid(); //checks if we have data from Z calibration (offsets of the Z heiths of the 8 calibration points from the first point)
		#ifdef SUPPORT_VERBOSITY
		if (verbosity_level >= 1) {
			has_z ? SERIAL_PROTOCOLPGM("Z jitter data from Z cal. valid.\n") : SERIAL_PROTOCOLPGM("Z jitter data from Z cal. not valid.\n");
		}
		#endif // SUPPORT_VERBOSITY
		setup_for_endstop_move(false); //save feedrate and feedmultiply, sets feedmultiply to 100
		const char *kill_message = NULL;
		while (mesh_point != MESH_MEAS_NUM_X_POINTS * MESH_MEAS_NUM_Y_POINTS) {
			#ifdef SUPPORT_VERBOSITY
			if (verbosity_level >= 1) SERIAL_ECHOLNPGM("");
			#endif // SUPPORT_VERBOSITY
			// Get coords of a measuring point.
			ix = mesh_point % MESH_MEAS_NUM_X_POINTS; // from 0 to MESH_NUM_X_POINTS - 1
			iy = mesh_point / MESH_MEAS_NUM_X_POINTS;
			if (iy & 1) ix = (MESH_MEAS_NUM_X_POINTS - 1) - ix; // Zig zag
			float z0 = 0.f;
			if (has_z && mesh_point > 0) {
				uint16_t z_offset_u = eeprom_read_word((uint16_t*)(EEPROM_BED_CALIBRATION_Z_JITTER + 2 * (ix + iy * 3 - 1)));
				z0 = mbl.z_values[0][0] + *reinterpret_cast<int16_t*>(&z_offset_u) * 0.01;
				#ifdef SUPPORT_VERBOSITY
				if (verbosity_level >= 1) {
					SERIAL_ECHOPGM("Bed leveling, point: ");
					MYSERIAL.print(mesh_point);
					SERIAL_ECHOPGM(", calibration z: ");
					MYSERIAL.print(z0, 5);
					SERIAL_ECHOLNPGM("");
				}
				#endif // SUPPORT_VERBOSITY
			}

			// Move Z up to MESH_HOME_Z_SEARCH.
			current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], Z_LIFT_FEEDRATE, active_extruder);
			st_synchronize();

			// Move to XY position of the sensor point.
			current_position[X_AXIS] = pgm_read_float(bed_ref_points + 2 * mesh_point);
			current_position[Y_AXIS] = pgm_read_float(bed_ref_points + 2 * mesh_point + 1);



			world2machine_clamp(current_position[X_AXIS], current_position[Y_AXIS]);
			#ifdef SUPPORT_VERBOSITY
			if (verbosity_level >= 1) {

				SERIAL_PROTOCOL(mesh_point);
				clamped ? SERIAL_PROTOCOLPGM(": xy clamped.\n") : SERIAL_PROTOCOLPGM(": no xy clamping\n");
			}
			#endif // SUPPORT_VERBOSITY


			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], XY_AXIS_FEEDRATE, active_extruder);
			st_synchronize();

			// Go down until endstop is hit
			const float Z_CALIBRATION_THRESHOLD = 1.f;
			if (!find_bed_induction_sensor_point_z((has_z && mesh_point > 0) ? z0 - Z_CALIBRATION_THRESHOLD : -10.f)) { //if we have data from z calibration max allowed difference is 1mm for each point, if we dont have data max difference is 10mm from initial point  
				kill_message = MSG_BED_LEVELING_FAILED_POINT_LOW;
				break;
			}
			if (MESH_HOME_Z_SEARCH - current_position[Z_AXIS] < 0.1f) {
				kill_message = MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED;
				break;
			}
			if (has_z && fabs(z0 - current_position[Z_AXIS]) > Z_CALIBRATION_THRESHOLD) { //if we have data from z calibration, max. allowed difference is 1mm for each point
				kill_message = MSG_BED_LEVELING_FAILED_POINT_HIGH;
				break;
			}
			#ifdef SUPPORT_VERBOSITY
			if (verbosity_level >= 10) {
				SERIAL_ECHOPGM("X: ");
				MYSERIAL.print(current_position[X_AXIS], 5);
				SERIAL_ECHOLNPGM("");
				SERIAL_ECHOPGM("Y: ");
				MYSERIAL.print(current_position[Y_AXIS], 5);
				SERIAL_PROTOCOLPGM("\n");
			}
			if (verbosity_level >= 1) {
				SERIAL_ECHOPGM("mesh bed leveling: ");
				MYSERIAL.print(current_position[Z_AXIS], 5);
				SERIAL_ECHOLNPGM("");
			}
			#endif // SUPPORT_VERBOSITY
			mbl.set_z(ix, iy, current_position[Z_AXIS]); //store measured z values z_values[iy][ix] = z;

			custom_message_state--;
			mesh_point++;
			lcd_update(1);
		}
		current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
		#ifdef SUPPORT_VERBOSITY
		if (verbosity_level >= 20) {
			SERIAL_ECHOLNPGM("Mesh bed leveling while loop finished.");
			SERIAL_ECHOLNPGM("MESH_HOME_Z_SEARCH: ");
			MYSERIAL.print(current_position[Z_AXIS], 5);
		}
		#endif // SUPPORT_VERBOSITY
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], Z_LIFT_FEEDRATE, active_extruder);
		st_synchronize();
		if (mesh_point != MESH_MEAS_NUM_X_POINTS * MESH_MEAS_NUM_Y_POINTS) {
			kill(kill_message);
			SERIAL_ECHOLNPGM("killed");
		}
		clean_up_after_endstop_move();
		SERIAL_ECHOLNPGM("clean up finished ");
		if(temp_cal_active == true && calibration_status_pinda() == true) temp_compensation_apply(); //apply PINDA temperature compensation
		babystep_apply(); // Apply Z height correction aka baby stepping before mesh bed leveing gets activated.
		SERIAL_ECHOLNPGM("babystep applied");
		bool eeprom_bed_correction_valid = eeprom_read_byte((unsigned char*)EEPROM_BED_CORRECTION_VALID) == 1;

		#ifdef SUPPORT_VERBOSITY
		if (verbosity_level >= 1) {
			eeprom_bed_correction_valid ? SERIAL_PROTOCOLPGM("Bed correction data valid\n") : SERIAL_PROTOCOLPGM("Bed correction data not valid\n");
		}
		#endif // SUPPORT_VERBOSITY

		for (uint8_t i = 0; i < 4; ++i) {
			unsigned char codes[4] = { 'L', 'R', 'F', 'B' };
			long correction = 0;
			if (code_seen(codes[i]))
				correction = code_value_long();
			else if (eeprom_bed_correction_valid) {
				unsigned char *addr = (i < 2) ?
					((i == 0) ? (unsigned char*)EEPROM_BED_CORRECTION_LEFT : (unsigned char*)EEPROM_BED_CORRECTION_RIGHT) :
					((i == 2) ? (unsigned char*)EEPROM_BED_CORRECTION_FRONT : (unsigned char*)EEPROM_BED_CORRECTION_REAR);
				correction = eeprom_read_int8(addr);
			}
			if (correction == 0)
				continue;
			float offset = float(correction) * 0.001f;
			if (fabs(offset) > 0.101f) {
				SERIAL_ERROR_START;
				SERIAL_ECHOPGM("Excessive bed leveling correction: ");
				SERIAL_ECHO(offset);
				SERIAL_ECHOLNPGM(" microns");
			}
			else {
				switch (i) {
				case 0:
					for (uint8_t row = 0; row < 3; ++row) {
						mbl.z_values[row][1] += 0.5f * offset;
						mbl.z_values[row][0] += offset;
					}
					break;
				case 1:
					for (uint8_t row = 0; row < 3; ++row) {
						mbl.z_values[row][1] += 0.5f * offset;
						mbl.z_values[row][2] += offset;
					}
					break;
				case 2:
					for (uint8_t col = 0; col < 3; ++col) {
						mbl.z_values[1][col] += 0.5f * offset;
						mbl.z_values[0][col] += offset;
					}
					break;
				case 3:
					for (uint8_t col = 0; col < 3; ++col) {
						mbl.z_values[1][col] += 0.5f * offset;
						mbl.z_values[2][col] += offset;
					}
					break;
				}
			}
		}
		SERIAL_ECHOLNPGM("Bed leveling correction finished");
		mbl.upsample_3x3(); //bilinear interpolation from 3x3 to 7x7 points while using the same array z_values[iy][ix] for storing (just coppying measured data to new destination and interpolating between them)
		SERIAL_ECHOLNPGM("Upsample finished");
		mbl.active = 1; //activate mesh bed leveling
		SERIAL_ECHOLNPGM("Mesh bed leveling activated");
		go_home_with_z_lift();
		SERIAL_ECHOLNPGM("Go home finished");
		//unretract (after PINDA preheat retraction)
		if (degHotend(active_extruder) > EXTRUDE_MINTEMP && temp_cal_active == true && calibration_status_pinda() == true && target_temperature_bed >= 50) {
			current_position[E_AXIS] += DEFAULT_RETRACTION;
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 400, active_extruder);
		}
		KEEPALIVE_STATE(NOT_BUSY);
		// Restore custom message state
		custom_message = custom_message_old;
		custom_message_type = custom_message_type_old;
		custom_message_state = custom_message_state_old;
		mesh_bed_leveling_flag = false;
		mesh_bed_run_from_menu = false;
		lcd_update(2);
		
	}
	break;

        /**
         * G81: Print mesh bed leveling status and bed profile if activated
         */
        case 81:
            if (mbl.active) {
                SERIAL_PROTOCOLPGM("Num X,Y: ");
                SERIAL_PROTOCOL(MESH_NUM_X_POINTS);
                SERIAL_PROTOCOLPGM(",");
                SERIAL_PROTOCOL(MESH_NUM_Y_POINTS);
                SERIAL_PROTOCOLPGM("\nZ search height: ");
                SERIAL_PROTOCOL(MESH_HOME_Z_SEARCH);
                SERIAL_PROTOCOLLNPGM("\nMeasured points:");
                for (int y = MESH_NUM_Y_POINTS-1; y >= 0; y--) {
                    for (int x = 0; x < MESH_NUM_X_POINTS; x++) {
                        SERIAL_PROTOCOLPGM("  ");
                        SERIAL_PROTOCOL_F(mbl.z_values[y][x], 5);
                    }
                    SERIAL_PROTOCOLPGM("\n");
                }
            }
            else
                SERIAL_PROTOCOLLNPGM("Mesh bed leveling not active.");
            break;
            
#if 0
        /**
         * G82: Single Z probe at current location
         *
         * WARNING! USE WITH CAUTION! If you'll try to probe where is no leveling pad, nasty things can happen!
         *
         */
        case 82:
            SERIAL_PROTOCOLLNPGM("Finding bed ");
            setup_for_endstop_move();
            find_bed_induction_sensor_point_z();
            clean_up_after_endstop_move();
            SERIAL_PROTOCOLPGM("Bed found at: ");
            SERIAL_PROTOCOL_F(current_position[Z_AXIS], 5);
            SERIAL_PROTOCOLPGM("\n");
            break;

            /**
             * G83: Prusa3D specific: Babystep in Z and store to EEPROM
             */
        case 83:
        {
            int babystepz = code_seen('S') ? code_value() : 0;
            int BabyPosition = code_seen('P') ? code_value() : 0;
            
            if (babystepz != 0) {
                //FIXME Vojtech: What shall be the index of the axis Z: 3 or 4?
                // Is the axis indexed starting with zero or one?
                if (BabyPosition > 4) {
                    SERIAL_PROTOCOLLNPGM("Index out of bounds");
                }else{
                    // Save it to the eeprom
                    babystepLoadZ = babystepz;
                    EEPROM_save_B(EEPROM_BABYSTEP_Z0+(BabyPosition*2),&babystepLoadZ);
                    // adjust the Z
                    babystepsTodoZadd(babystepLoadZ);
                }
            
            }
            
        }
        break;
            /**
             * G84: Prusa3D specific: UNDO Babystep Z (move Z axis back)
             */
        case 84:
            babystepsTodoZsubtract(babystepLoadZ);
            // babystepLoadZ = 0;
            break;
            
            /**
             * G85: Prusa3D specific: Pick best babystep
             */
        case 85:
            lcd_pick_babystep();
            break;
#endif
            
            /**
             * G86: Prusa3D specific: Disable babystep correction after home.
             * This G-code will be performed at the start of a calibration script.
             */
        case 86:
            calibration_status_store(CALIBRATION_STATUS_LIVE_ADJUST);
            break;
            /**
             * G87: Prusa3D specific: Enable babystep correction after home
             * This G-code will be performed at the end of a calibration script.
             */
        case 87:
			calibration_status_store(CALIBRATION_STATUS_CALIBRATED);
            break;

		/*case 88: //just for test
			SERIAL_ECHOPGM("Calibration status:");
			MYSERIAL.println(int(calibration_status()));
			if (code_seen('S')) codenum = code_value();
			calibration_status_store(codenum);
			SERIAL_ECHOPGM("Calibration status:");
			MYSERIAL.println(int(calibration_status()));
			eeprom_write_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 1); //run wizard
			break;
		*/
#endif  // ENABLE_MESH_BED_LEVELING
            
            
    case 90: // G90
      relative_mode = false;
      break;
    case 91: // G91
      relative_mode = true;
      break;
    case 92: // G92
      if(!code_seen(axis_codes[E_AXIS]))
        st_synchronize();
      for(int8_t i=0; i < NUM_AXIS; i++) {
        if(code_seen(axis_codes[i])) {
           if(i == E_AXIS) {
             current_position[i] = code_value();
             plan_set_e_position(current_position[E_AXIS]);
           }
           else {
		current_position[i] = code_value()+add_homing[i];
            plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
           }
        }
      }
      break;

	case 98: //activate farm mode
		farm_mode = 1;
		PingTime = millis();
		EEPROM_save_B(EEPROM_FARM_NUMBER, &farm_no);
		eeprom_update_byte((unsigned char *)EEPROM_FARM_MODE, farm_mode);
		
		break;

	case 99: //deactivate farm mode
		farm_mode = 0;
		lcd_printer_connected();
		eeprom_update_byte((unsigned char *)EEPROM_FARM_MODE, farm_mode);
		lcd_update(2);
		break;







    }
  } // end if(code_seen('G'))

  else if(code_seen('M'))
  {
	  int index;
	  for (index = 1; *(strchr_pointer + index) == ' ' || *(strchr_pointer + index) == '\t'; index++);
	   
	 /*for (++strchr_pointer; *strchr_pointer == ' ' || *strchr_pointer == '\t'; ++strchr_pointer);*/
	  if (*(strchr_pointer+index) < '0' || *(strchr_pointer+index) > '9') {
		  SERIAL_ECHOLNPGM("Invalid M code");
	  } else
    switch((int)code_value())
    {
#ifdef ULTIPANEL

    case 0: // M0 - Unconditional stop - Wait for user button press on LCD
    case 1: // M1 - Conditional stop - Wait for user button press on LCD
    {
	  custom_message = true;  //fixes using M1 during SD print, but needs to be be updated in future
	  custom_message_type = 2; //fixes using M1 during SD print, but needs to be be updated in future

      char *src = strchr_pointer + 2;
	  codenum = 0;

      bool hasP = false, hasS = false;
      if (code_seen('P')) {
        codenum = code_value(); // milliseconds to wait
        hasP = codenum > 0;
      }
      if (code_seen('S')) {
        codenum = code_value() * 1000; // seconds to wait
        hasS = codenum > 0;
      }
      starpos = strchr(src, '*');
      if (starpos != NULL) *(starpos) = '\0';
      while (*src == ' ') ++src;
      if (!hasP && !hasS && *src != '\0') {
        lcd_setstatus(src);
	  } else {
		  LCD_MESSAGERPGM(MSG_USERWAIT);
	  }

      lcd_ignore_click();				//call lcd_ignore_click aslo for else ???
      st_synchronize();
      previous_millis_cmd = millis();
      if (codenum > 0){
        codenum += millis();  // keep track of when we started waiting
		KEEPALIVE_STATE(PAUSED_FOR_USER);
		while(millis() < codenum && !lcd_clicked()){
          manage_heater();
          manage_inactivity(true);
          lcd_update();
        }
		KEEPALIVE_STATE(IN_HANDLER);
        lcd_ignore_click(false);
      }else{
          if (!lcd_detected())
            break;
		  KEEPALIVE_STATE(PAUSED_FOR_USER);
        while(!lcd_clicked()){
          manage_heater();
          manage_inactivity(true);
          lcd_update();
        }
		KEEPALIVE_STATE(IN_HANDLER);
      }
      if (IS_SD_PRINTING)
        LCD_MESSAGERPGM(MSG_RESUMING);
      else
        LCD_MESSAGERPGM(WELCOME_MSG);
	  custom_message = false; 
	  custom_message_type = 0;
	}
    break;
#endif
    case 17:
        LCD_MESSAGERPGM(MSG_NO_MOVE);
        enable_x();
        enable_y();
        enable_z();
        enable_e0();
        enable_e1();
        enable_e2();
      break;

#ifdef SDSUPPORT
    case 20: // M20 - list SD card
      SERIAL_PROTOCOLLNRPGM(MSG_BEGIN_FILE_LIST);
      card.ls();
      SERIAL_PROTOCOLLNRPGM(MSG_END_FILE_LIST);
      break;
    case 21: // M21 - init SD card

      card.initsd();

      break;
    case 22: //M22 - release SD card
      card.release();

      break;
    case 23: //M23 - Select file
      starpos = (strchr(strchr_pointer + 4,'*'));
      if(starpos!=NULL)
        *(starpos)='\0';
      card.openFile(strchr_pointer + 4,true);
      break;
    case 24: //M24 - Start SD print
      card.startFileprint();
      starttime=millis();
	  break;
    case 25: //M25 - Pause SD print
      card.pauseSDPrint();
      break;
    case 26: //M26 - Set SD index
      if(card.cardOK && code_seen('S')) {
        card.setIndex(code_value_long());
      }
      break;
    case 27: //M27 - Get SD status
      card.getStatus();
      break;
    case 28: //M28 - Start SD write
      starpos = (strchr(strchr_pointer + 4,'*'));
      if(starpos != NULL){
        char* npos = strchr(CMDBUFFER_CURRENT_STRING, 'N');
        strchr_pointer = strchr(npos,' ') + 1;
        *(starpos) = '\0';
      }
      card.openFile(strchr_pointer+4,false);
      break;
    case 29: //M29 - Stop SD write
      //processed in write to file routine above
      //card,saving = false;
      break;
    case 30: //M30 <filename> Delete File
      if (card.cardOK){
        card.closefile();
        starpos = (strchr(strchr_pointer + 4,'*'));
        if(starpos != NULL){
          char* npos = strchr(CMDBUFFER_CURRENT_STRING, 'N');
          strchr_pointer = strchr(npos,' ') + 1;
          *(starpos) = '\0';
        }
        card.removeFile(strchr_pointer + 4);
      }
      break;
    case 32: //M32 - Select file and start SD print
    {
      if(card.sdprinting) {
        st_synchronize();

      }
      starpos = (strchr(strchr_pointer + 4,'*'));

      char* namestartpos = (strchr(strchr_pointer + 4,'!'));   //find ! to indicate filename string start.
      if(namestartpos==NULL)
      {
        namestartpos=strchr_pointer + 4; //default name position, 4 letters after the M
      }
      else
        namestartpos++; //to skip the '!'

      if(starpos!=NULL)
        *(starpos)='\0';

      bool call_procedure=(code_seen('P'));

      if(strchr_pointer>namestartpos)
        call_procedure=false;  //false alert, 'P' found within filename

      if( card.cardOK )
      {
        card.openFile(namestartpos,true,!call_procedure);
        if(code_seen('S'))
          if(strchr_pointer<namestartpos) //only if "S" is occuring _before_ the filename
            card.setIndex(code_value_long());
        card.startFileprint();
        if(!call_procedure)
          starttime=millis(); //procedure calls count as normal print time.
      }
    } break;
    case 928: //M928 - Start SD write
      starpos = (strchr(strchr_pointer + 5,'*'));
      if(starpos != NULL){
        char* npos = strchr(CMDBUFFER_CURRENT_STRING, 'N');
        strchr_pointer = strchr(npos,' ') + 1;
        *(starpos) = '\0';
      }
      card.openLogFile(strchr_pointer+5);
      break;

#endif //SDSUPPORT

    case 31: //M31 take time since the start of the SD print or an M109 command
      {
      stoptime=millis();
      char time[30];
      unsigned long t=(stoptime-starttime)/1000;
      int sec,min;
      min=t/60;
      sec=t%60;
      sprintf_P(time, PSTR("%i min, %i sec"), min, sec);
      SERIAL_ECHO_START;
      SERIAL_ECHOLN(time);
      lcd_setstatus(time);
      autotempShutdown();
      }
      break;
    case 42: //M42 -Change pin status via gcode
      if (code_seen('S'))
      {
        int pin_status = code_value();
        int pin_number = LED_PIN;
        if (code_seen('P') && pin_status >= 0 && pin_status <= 255)
          pin_number = code_value();
        for(int8_t i = 0; i < (int8_t)(sizeof(sensitive_pins)/sizeof(int)); i++)
        {
          if (sensitive_pins[i] == pin_number)
          {
            pin_number = -1;
            break;
          }
        }
      #if defined(FAN_PIN) && FAN_PIN > -1
        if (pin_number == FAN_PIN)
          fanSpeed = pin_status;
      #endif
        if (pin_number > -1)
        {
          pinMode(pin_number, OUTPUT);
          digitalWrite(pin_number, pin_status);
          analogWrite(pin_number, pin_status);
        }
      }
     break;

    case 44: // M44: Prusa3D: Reset the bed skew and offset calibration.

		// Reset the baby step value and the baby step applied flag.
		calibration_status_store(CALIBRATION_STATUS_ASSEMBLED);
		eeprom_update_word((uint16_t*)EEPROM_BABYSTEP_Z, 0);

        // Reset the skew and offset in both RAM and EEPROM.
        reset_bed_offset_and_skew();
        // Reset world2machine_rotation_and_skew and world2machine_shift, therefore
        // the planner will not perform any adjustments in the XY plane. 
        // Wait for the motors to stop and update the current position with the absolute values.
        world2machine_revert_to_uncorrected();
        break;

    case 45: // M45: Prusa3D: bed skew and offset with manual Z up
    {
		// Only Z calibration?
		bool only_Z = code_seen('Z');
		gcode_M45(only_Z);		
        break;
    }

    /*
    case 46:
    {
        // M46: Prusa3D: Show the assigned IP address.
        uint8_t ip[4];
        bool hasIP = card.ToshibaFlashAir_GetIP(ip);
        if (hasIP) {
            SERIAL_ECHOPGM("Toshiba FlashAir current IP: ");
            SERIAL_ECHO(int(ip[0]));
            SERIAL_ECHOPGM(".");
            SERIAL_ECHO(int(ip[1]));
            SERIAL_ECHOPGM(".");
            SERIAL_ECHO(int(ip[2]));
            SERIAL_ECHOPGM(".");
            SERIAL_ECHO(int(ip[3]));
            SERIAL_ECHOLNPGM("");
        } else {
            SERIAL_ECHOLNPGM("Toshiba FlashAir GetIP failed");          
        }
        break;
    }
    */

    case 47:
        // M47: Prusa3D: Show end stops dialog on the display.
		KEEPALIVE_STATE(PAUSED_FOR_USER);
        lcd_diag_show_end_stops();
		KEEPALIVE_STATE(IN_HANDLER);
		break;

#if 0
    case 48: // M48: scan the bed induction sensor points, print the sensor trigger coordinates to the serial line for visualization on the PC.
    {
        // Disable the default update procedure of the display. We will do a modal dialog.
        lcd_update_enable(false);
        // Let the planner use the uncorrected coordinates.
        mbl.reset();
        // Reset world2machine_rotation_and_skew and world2machine_shift, therefore
        // the planner will not perform any adjustments in the XY plane. 
        // Wait for the motors to stop and update the current position with the absolute values.
        world2machine_revert_to_uncorrected();
        // Move the print head close to the bed.
        current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
        plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS],current_position[Z_AXIS] , current_position[E_AXIS], homing_feedrate[Z_AXIS]/40, active_extruder);
        st_synchronize();
        // Home in the XY plane.
        set_destination_to_current();
        setup_for_endstop_move();
        home_xy();
        int8_t verbosity_level = 0;
        if (code_seen('V')) {
            // Just 'V' without a number counts as V1.
            char c = strchr_pointer[1];
            verbosity_level = (c == ' ' || c == '\t' || c == 0) ? 1 : code_value_short();
        }
        bool success = scan_bed_induction_points(verbosity_level);
        clean_up_after_endstop_move();
        // Print head up.
        current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
        plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS],current_position[Z_AXIS] , current_position[E_AXIS], homing_feedrate[Z_AXIS]/40, active_extruder);
        st_synchronize();
        lcd_update_enable(true);
        break;
    }
#endif

// M48 Z-Probe repeatability measurement function.
//
// Usage:   M48 <n #_samples> <X X_position_for_samples> <Y Y_position_for_samples> <V Verbose_Level> <L legs_of_movement_prior_to_doing_probe>
//	
// This function assumes the bed has been homed.  Specificaly, that a G28 command
// as been issued prior to invoking the M48 Z-Probe repeatability measurement function.
// Any information generated by a prior G29 Bed leveling command will be lost and need to be
// regenerated.
//
// The number of samples will default to 10 if not specified.  You can use upper or lower case
// letters for any of the options EXCEPT n.  n must be in lower case because Marlin uses a capital
// N for its communication protocol and will get horribly confused if you send it a capital N.
//

#ifdef ENABLE_AUTO_BED_LEVELING
#ifdef Z_PROBE_REPEATABILITY_TEST 

    case 48: // M48 Z-Probe repeatability
        {
            #if Z_MIN_PIN == -1
            #error "You must have a Z_MIN endstop in order to enable calculation of Z-Probe repeatability."
            #endif

	double sum=0.0; 
	double mean=0.0; 
	double sigma=0.0;
	double sample_set[50];
	int verbose_level=1, n=0, j, n_samples = 10, n_legs=0;
	double X_current, Y_current, Z_current;
	double X_probe_location, Y_probe_location, Z_start_location, ext_position;
	
	if (code_seen('V') || code_seen('v')) {
        	verbose_level = code_value();
		if (verbose_level<0 || verbose_level>4 ) {
			SERIAL_PROTOCOLPGM("?Verbose Level not plausable.\n");
			goto Sigma_Exit;
		}
	}

	if (verbose_level > 0)   {
		SERIAL_PROTOCOLPGM("M48 Z-Probe Repeatability test.   Version 2.00\n");
		SERIAL_PROTOCOLPGM("Full support at: http://3dprintboard.com/forum.php\n");
	}

	if (code_seen('n')) {
        	n_samples = code_value();
		if (n_samples<4 || n_samples>50 ) {
			SERIAL_PROTOCOLPGM("?Specified sample size not plausable.\n");
			goto Sigma_Exit;
		}
	}

	X_current = X_probe_location = st_get_position_mm(X_AXIS);
	Y_current = Y_probe_location = st_get_position_mm(Y_AXIS);
	Z_current = st_get_position_mm(Z_AXIS);
	Z_start_location = st_get_position_mm(Z_AXIS) + Z_RAISE_BEFORE_PROBING;
	ext_position	 = st_get_position_mm(E_AXIS);

	if (code_seen('X') || code_seen('x') ) {
        	X_probe_location = code_value() -  X_PROBE_OFFSET_FROM_EXTRUDER;
		if (X_probe_location<X_MIN_POS || X_probe_location>X_MAX_POS ) {
			SERIAL_PROTOCOLPGM("?Specified X position out of range.\n");
			goto Sigma_Exit;
		}
	}

	if (code_seen('Y') || code_seen('y') ) {
        	Y_probe_location = code_value() -  Y_PROBE_OFFSET_FROM_EXTRUDER;
		if (Y_probe_location<Y_MIN_POS || Y_probe_location>Y_MAX_POS ) {
			SERIAL_PROTOCOLPGM("?Specified Y position out of range.\n");
			goto Sigma_Exit;
		}
	}

	if (code_seen('L') || code_seen('l') ) {
        	n_legs = code_value();
		if ( n_legs==1 ) 
			n_legs = 2;
		if ( n_legs<0 || n_legs>15 ) {
			SERIAL_PROTOCOLPGM("?Specified number of legs in movement not plausable.\n");
			goto Sigma_Exit;
		}
	}

//
// Do all the preliminary setup work.   First raise the probe.
//

        st_synchronize();
        plan_bed_level_matrix.set_to_identity();
	plan_buffer_line( X_current, Y_current, Z_start_location,
			ext_position,
    			homing_feedrate[Z_AXIS]/60,
			active_extruder);
        st_synchronize();

//
// Now get everything to the specified probe point So we can safely do a probe to
// get us close to the bed.  If the Z-Axis is far from the bed, we don't want to 
// use that as a starting point for each probe.
//
	if (verbose_level > 2) 
		SERIAL_PROTOCOL("Positioning probe for the test.\n");

	plan_buffer_line( X_probe_location, Y_probe_location, Z_start_location,
			ext_position,
    			homing_feedrate[X_AXIS]/60,
			active_extruder);
        st_synchronize();

	current_position[X_AXIS] = X_current = st_get_position_mm(X_AXIS);
	current_position[Y_AXIS] = Y_current = st_get_position_mm(Y_AXIS);
	current_position[Z_AXIS] = Z_current = st_get_position_mm(Z_AXIS);
	current_position[E_AXIS] = ext_position = st_get_position_mm(E_AXIS);

// 
// OK, do the inital probe to get us close to the bed.
// Then retrace the right amount and use that in subsequent probes
//

	setup_for_endstop_move();
	run_z_probe();

	current_position[Z_AXIS] = Z_current = st_get_position_mm(Z_AXIS);
	Z_start_location = st_get_position_mm(Z_AXIS) + Z_RAISE_BEFORE_PROBING;

	plan_buffer_line( X_probe_location, Y_probe_location, Z_start_location,
			ext_position,
    			homing_feedrate[X_AXIS]/60,
			active_extruder);
        st_synchronize();
	current_position[Z_AXIS] = Z_current = st_get_position_mm(Z_AXIS);

        for( n=0; n<n_samples; n++) {

		do_blocking_move_to( X_probe_location, Y_probe_location, Z_start_location); // Make sure we are at the probe location

		if ( n_legs)  {
		double radius=0.0, theta=0.0, x_sweep, y_sweep;
		int rotational_direction, l;

			rotational_direction = (unsigned long) millis() & 0x0001;			// clockwise or counter clockwise
			radius = (unsigned long) millis() % (long) (X_MAX_LENGTH/4); 			// limit how far out to go 
			theta = (float) ((unsigned long) millis() % (long) 360) / (360./(2*3.1415926));	// turn into radians

//SERIAL_ECHOPAIR("starting radius: ",radius);
//SERIAL_ECHOPAIR("   theta: ",theta);
//SERIAL_ECHOPAIR("   direction: ",rotational_direction);
//SERIAL_PROTOCOLLNPGM("");

			for( l=0; l<n_legs-1; l++) {
				if (rotational_direction==1)
					theta += (float) ((unsigned long) millis() % (long) 20) / (360.0/(2*3.1415926)); // turn into radians
				else
					theta -= (float) ((unsigned long) millis() % (long) 20) / (360.0/(2*3.1415926)); // turn into radians

				radius += (float) ( ((long) ((unsigned long) millis() % (long) 10)) - 5);
				if ( radius<0.0 )
					radius = -radius;

				X_current = X_probe_location + cos(theta) * radius;
				Y_current = Y_probe_location + sin(theta) * radius;

				if ( X_current<X_MIN_POS)		// Make sure our X & Y are sane
					 X_current = X_MIN_POS;
				if ( X_current>X_MAX_POS)
					 X_current = X_MAX_POS;

				if ( Y_current<Y_MIN_POS)		// Make sure our X & Y are sane
					 Y_current = Y_MIN_POS;
				if ( Y_current>Y_MAX_POS)
					 Y_current = Y_MAX_POS;

				if (verbose_level>3 ) {
					SERIAL_ECHOPAIR("x: ", X_current);
					SERIAL_ECHOPAIR("y: ", Y_current);
					SERIAL_PROTOCOLLNPGM("");
				}

				do_blocking_move_to( X_current, Y_current, Z_current );
			}
			do_blocking_move_to( X_probe_location, Y_probe_location, Z_start_location); // Go back to the probe location
		}

		setup_for_endstop_move();
                run_z_probe();

		sample_set[n] = current_position[Z_AXIS];

//
// Get the current mean for the data points we have so far
//
		sum=0.0; 
		for( j=0; j<=n; j++) {
			sum = sum + sample_set[j];
		}
		mean = sum / (double (n+1));
//
// Now, use that mean to calculate the standard deviation for the
// data points we have so far
//

		sum=0.0; 
		for( j=0; j<=n; j++) {
			sum = sum + (sample_set[j]-mean) * (sample_set[j]-mean);
		}
		sigma = sqrt( sum / (double (n+1)) );

		if (verbose_level > 1) {
			SERIAL_PROTOCOL(n+1);
			SERIAL_PROTOCOL(" of ");
			SERIAL_PROTOCOL(n_samples);
			SERIAL_PROTOCOLPGM("   z: ");
			SERIAL_PROTOCOL_F(current_position[Z_AXIS], 6);
		}

		if (verbose_level > 2) {
			SERIAL_PROTOCOL(" mean: ");
			SERIAL_PROTOCOL_F(mean,6);

			SERIAL_PROTOCOL("   sigma: ");
			SERIAL_PROTOCOL_F(sigma,6);
		}

		if (verbose_level > 0) 
			SERIAL_PROTOCOLPGM("\n");

		plan_buffer_line( X_probe_location, Y_probe_location, Z_start_location, 
				  current_position[E_AXIS], homing_feedrate[Z_AXIS]/60, active_extruder);
        	st_synchronize();

	}

	delay(1000);

        clean_up_after_endstop_move();

//      enable_endstops(true);

	if (verbose_level > 0) {
		SERIAL_PROTOCOLPGM("Mean: ");
		SERIAL_PROTOCOL_F(mean, 6);
		SERIAL_PROTOCOLPGM("\n");
	}

SERIAL_PROTOCOLPGM("Standard Deviation: ");
SERIAL_PROTOCOL_F(sigma, 6);
SERIAL_PROTOCOLPGM("\n\n");

Sigma_Exit:
        break;
	}
#endif		// Z_PROBE_REPEATABILITY_TEST 
#endif		// ENABLE_AUTO_BED_LEVELING

    case 104: // M104
      if(setTargetedHotend(104)){
        break;
      }
      if (code_seen('S')) setTargetHotend(code_value(), tmp_extruder);
      setWatch();
      break;
    case 112: //  M112 -Emergency Stop
      kill();
      break;
    case 140: // M140 set bed temp
      if (code_seen('S')) setTargetBed(code_value());
      break;
    case 105 : // M105
      if(setTargetedHotend(105)){
        break;
        }
      #if defined(TEMP_0_PIN) && TEMP_0_PIN > -1
        SERIAL_PROTOCOLPGM("ok T:");
        SERIAL_PROTOCOL_F(degHotend(tmp_extruder),1);
        SERIAL_PROTOCOLPGM(" /");
        SERIAL_PROTOCOL_F(degTargetHotend(tmp_extruder),1);
        #if defined(TEMP_BED_PIN) && TEMP_BED_PIN > -1
          SERIAL_PROTOCOLPGM(" B:");
          SERIAL_PROTOCOL_F(degBed(),1);
          SERIAL_PROTOCOLPGM(" /");
          SERIAL_PROTOCOL_F(degTargetBed(),1);
        #endif //TEMP_BED_PIN
        for (int8_t cur_extruder = 0; cur_extruder < EXTRUDERS; ++cur_extruder) {
          SERIAL_PROTOCOLPGM(" T");
          SERIAL_PROTOCOL(cur_extruder);
          SERIAL_PROTOCOLPGM(":");
          SERIAL_PROTOCOL_F(degHotend(cur_extruder),1);
          SERIAL_PROTOCOLPGM(" /");
          SERIAL_PROTOCOL_F(degTargetHotend(cur_extruder),1);
        }
      #else
        SERIAL_ERROR_START;
        SERIAL_ERRORLNRPGM(MSG_ERR_NO_THERMISTORS);
      #endif

        SERIAL_PROTOCOLPGM(" @:");
      #ifdef EXTRUDER_WATTS
        SERIAL_PROTOCOL((EXTRUDER_WATTS * getHeaterPower(tmp_extruder))/127);
        SERIAL_PROTOCOLPGM("W");
      #else
        SERIAL_PROTOCOL(getHeaterPower(tmp_extruder));
      #endif

        SERIAL_PROTOCOLPGM(" B@:");
      #ifdef BED_WATTS
        SERIAL_PROTOCOL((BED_WATTS * getHeaterPower(-1))/127);
        SERIAL_PROTOCOLPGM("W");
      #else
        SERIAL_PROTOCOL(getHeaterPower(-1));
      #endif

        #ifdef SHOW_TEMP_ADC_VALUES
          {float raw = 0.0;

          #if defined(TEMP_BED_PIN) && TEMP_BED_PIN > -1
            SERIAL_PROTOCOLPGM("    ADC B:");
            SERIAL_PROTOCOL_F(degBed(),1);
            SERIAL_PROTOCOLPGM("C->");
            raw = rawBedTemp();
            SERIAL_PROTOCOL_F(raw/OVERSAMPLENR,5);
            SERIAL_PROTOCOLPGM(" Rb->");
            SERIAL_PROTOCOL_F(100 * (1 + (PtA * (raw/OVERSAMPLENR)) + (PtB * sq((raw/OVERSAMPLENR)))), 5);
            SERIAL_PROTOCOLPGM(" Rxb->");
            SERIAL_PROTOCOL_F(raw, 5);
          #endif
          for (int8_t cur_extruder = 0; cur_extruder < EXTRUDERS; ++cur_extruder) {
            SERIAL_PROTOCOLPGM("  T");
            SERIAL_PROTOCOL(cur_extruder);
            SERIAL_PROTOCOLPGM(":");
            SERIAL_PROTOCOL_F(degHotend(cur_extruder),1);
            SERIAL_PROTOCOLPGM("C->");
            raw = rawHotendTemp(cur_extruder);
            SERIAL_PROTOCOL_F(raw/OVERSAMPLENR,5);
            SERIAL_PROTOCOLPGM(" Rt");
            SERIAL_PROTOCOL(cur_extruder);
            SERIAL_PROTOCOLPGM("->");
            SERIAL_PROTOCOL_F(100 * (1 + (PtA * (raw/OVERSAMPLENR)) + (PtB * sq((raw/OVERSAMPLENR)))), 5);
            SERIAL_PROTOCOLPGM(" Rx");
            SERIAL_PROTOCOL(cur_extruder);
            SERIAL_PROTOCOLPGM("->");
            SERIAL_PROTOCOL_F(raw, 5);
          }}
        #endif
		SERIAL_PROTOCOLLN("");
		KEEPALIVE_STATE(NOT_BUSY);
      return;
      break;
    case 109:
    {// M109 - Wait for extruder heater to reach target.
      if(setTargetedHotend(109)){
        break;
      }
      LCD_MESSAGERPGM(MSG_HEATING);
	  heating_status = 1;
	  if (farm_mode) { prusa_statistics(1); };

#ifdef AUTOTEMP
        autotemp_enabled=false;
      #endif
      if (code_seen('S')) {
        setTargetHotend(code_value(), tmp_extruder);
              CooldownNoWait = true;
            } else if (code_seen('R')) {
              setTargetHotend(code_value(), tmp_extruder);
        CooldownNoWait = false;
      }
      #ifdef AUTOTEMP
        if (code_seen('S')) autotemp_min=code_value();
        if (code_seen('B')) autotemp_max=code_value();
        if (code_seen('F'))
        {
          autotemp_factor=code_value();
          autotemp_enabled=true;
        }
      #endif

      setWatch();
      codenum = millis();

      /* See if we are heating up or cooling down */
      target_direction = isHeatingHotend(tmp_extruder); // true if heating, false if cooling

	  KEEPALIVE_STATE(NOT_BUSY);

      cancel_heatup = false;

	  wait_for_heater(codenum); //loops until target temperature is reached

        LCD_MESSAGERPGM(MSG_HEATING_COMPLETE);
		KEEPALIVE_STATE(IN_HANDLER);
		heating_status = 2;
		if (farm_mode) { prusa_statistics(2); };
        
        //starttime=millis();
        previous_millis_cmd = millis();
      }
      break;
    case 190: // M190 - Wait for bed heater to reach target.
    #if defined(TEMP_BED_PIN) && TEMP_BED_PIN > -1
        LCD_MESSAGERPGM(MSG_BED_HEATING);
		heating_status = 3;
		if (farm_mode) { prusa_statistics(1); };
        if (code_seen('S')) 
		{
          setTargetBed(code_value());
          CooldownNoWait = true;
        } 
		else if (code_seen('R')) 
		{
          setTargetBed(code_value());
          CooldownNoWait = false;
        }
        codenum = millis();
        
        cancel_heatup = false;
        target_direction = isHeatingBed(); // true if heating, false if cooling

		KEEPALIVE_STATE(NOT_BUSY);
        while ( (target_direction)&&(!cancel_heatup) ? (isHeatingBed()) : (isCoolingBed()&&(CooldownNoWait==false)) )
        {
          if(( millis() - codenum) > 1000 ) //Print Temp Reading every 1 second while heating up.
          {
			  if (!farm_mode) {
				  float tt = degHotend(active_extruder);
				  SERIAL_PROTOCOLPGM("T:");
				  SERIAL_PROTOCOL(tt);
				  SERIAL_PROTOCOLPGM(" E:");
				  SERIAL_PROTOCOL((int)active_extruder);
				  SERIAL_PROTOCOLPGM(" B:");
				  SERIAL_PROTOCOL_F(degBed(), 1);
				  SERIAL_PROTOCOLLN("");
			  }
				  codenum = millis();
			  
          }
          manage_heater();
          manage_inactivity();
          lcd_update();
        }
        LCD_MESSAGERPGM(MSG_BED_DONE);
		KEEPALIVE_STATE(IN_HANDLER);
		heating_status = 4;

        previous_millis_cmd = millis();
    #endif
        break;

    #if defined(FAN_PIN) && FAN_PIN > -1
      case 106: //M106 Fan On
        if (code_seen('S')){
           fanSpeed=constrain(code_value(),0,255);
        }
        else {
          fanSpeed=255;
        }
        break;
      case 107: //M107 Fan Off
        fanSpeed = 0;
        break;
    #endif //FAN_PIN

    #if defined(PS_ON_PIN) && PS_ON_PIN > -1
      case 80: // M80 - Turn on Power Supply
        SET_OUTPUT(PS_ON_PIN); //GND
        WRITE(PS_ON_PIN, PS_ON_AWAKE);

        // If you have a switch on suicide pin, this is useful
        // if you want to start another print with suicide feature after
        // a print without suicide...
        #if defined SUICIDE_PIN && SUICIDE_PIN > -1
            SET_OUTPUT(SUICIDE_PIN);
            WRITE(SUICIDE_PIN, HIGH);
        #endif

        #ifdef ULTIPANEL
          powersupply = true;
          LCD_MESSAGERPGM(WELCOME_MSG);
          lcd_update();
        #endif
        break;
      #endif

      case 81: // M81 - Turn off Power Supply
        disable_heater();
        st_synchronize();
        disable_e0();
        disable_e1();
        disable_e2();
        finishAndDisableSteppers();
        fanSpeed = 0;
        delay(1000); // Wait a little before to switch off
      #if defined(SUICIDE_PIN) && SUICIDE_PIN > -1
        st_synchronize();
        suicide();
      #elif defined(PS_ON_PIN) && PS_ON_PIN > -1
        SET_OUTPUT(PS_ON_PIN);
        WRITE(PS_ON_PIN, PS_ON_ASLEEP);
      #endif
      #ifdef ULTIPANEL
        powersupply = false;
        LCD_MESSAGERPGM(CAT4(CUSTOM_MENDEL_NAME,PSTR(" "),MSG_OFF,PSTR("."))); //!!
        
        /*
        MACHNAME = "Prusa i3"
        MSGOFF = "Vypnuto"
        "Prusai3"" ""vypnuto""."
        
        "Prusa i3"" "MSG_ALL[lang_selected][50]"."
        */
        lcd_update();
      #endif
	  break;

    case 82:
      axis_relative_modes[3] = false;
      break;
    case 83:
      axis_relative_modes[3] = true;
      break;
    case 18: //compatibility
    case 84: // M84
      if(code_seen('S')){
        stepper_inactive_time = code_value() * 1000;
      }
      else
      {
        bool all_axis = !((code_seen(axis_codes[X_AXIS])) || (code_seen(axis_codes[Y_AXIS])) || (code_seen(axis_codes[Z_AXIS]))|| (code_seen(axis_codes[E_AXIS])));
        if(all_axis)
        {
          st_synchronize();
          disable_e0();
          disable_e1();
          disable_e2();
          finishAndDisableSteppers();
        }
        else
        {
          st_synchronize();
		  if (code_seen('X')) disable_x();
		  if (code_seen('Y')) disable_y();
		  if (code_seen('Z')) disable_z();
#if ((E0_ENABLE_PIN != X_ENABLE_PIN) && (E1_ENABLE_PIN != Y_ENABLE_PIN)) // Only enable on boards that have seperate ENABLE_PINS
		  if (code_seen('E')) {
			  disable_e0();
			  disable_e1();
			  disable_e2();
            }
          #endif
        }
      }
	  snmm_filaments_used = 0;
      break;
    case 85: // M85
      if(code_seen('S')) {
        max_inactive_time = code_value() * 1000;
      }
      break;
    case 92: // M92
      for(int8_t i=0; i < NUM_AXIS; i++)
      {
        if(code_seen(axis_codes[i]))
        {
          if(i == 3) { // E
            float value = code_value();
            if(value < 20.0) {
              float factor = axis_steps_per_unit[i] / value; // increase e constants if M92 E14 is given for netfab.
              max_jerk[E_AXIS] *= factor;
              max_feedrate[i] *= factor;
              axis_steps_per_sqr_second[i] *= factor;
            }
            axis_steps_per_unit[i] = value;
          }
          else {
            axis_steps_per_unit[i] = code_value();
          }
        }
      }
	  break;
	case 110:   // M110 - reset line pos
		if (code_seen('N'))
			gcode_LastN = code_value_long();
		break;
#ifdef HOST_KEEPALIVE_FEATURE
	case 113: // M113 - Get or set Host Keepalive interval
      if (code_seen('S')) {
		host_keepalive_interval = (uint8_t)code_value_short();
		NOMORE(host_keepalive_interval, 60);
      } else {
        SERIAL_ECHO_START;
        SERIAL_ECHOPAIR("M113 S", (unsigned long)host_keepalive_interval);
		SERIAL_PROTOCOLLN("");
      }
	break;
#endif
    case 115: // M115
      if (code_seen('V')) {
          // Report the Prusa version number.
          SERIAL_PROTOCOLLNRPGM(FW_VERSION_STR_P());
      } else if (code_seen('U')) {
          // Check the firmware version provided. If the firmware version provided by the U code is higher than the currently running firmware,
          // pause the print and ask the user to upgrade the firmware.
          show_upgrade_dialog_if_version_newer(++ strchr_pointer);
      } else {
          SERIAL_PROTOCOLRPGM(MSG_M115_REPORT);
      }
      break;
/*    case 117: // M117 display message
      starpos = (strchr(strchr_pointer + 5,'*'));
      if(starpos!=NULL)
        *(starpos)='\0';
      lcd_setstatus(strchr_pointer + 5);
      break;*/
    case 114: // M114
      SERIAL_PROTOCOLPGM("X:");
      SERIAL_PROTOCOL(current_position[X_AXIS]);
      SERIAL_PROTOCOLPGM(" Y:");
      SERIAL_PROTOCOL(current_position[Y_AXIS]);
      SERIAL_PROTOCOLPGM(" Z:");
      SERIAL_PROTOCOL(current_position[Z_AXIS]);
      SERIAL_PROTOCOLPGM(" E:");
      SERIAL_PROTOCOL(current_position[E_AXIS]);

      SERIAL_PROTOCOLRPGM(MSG_COUNT_X);
      SERIAL_PROTOCOL(float(st_get_position(X_AXIS))/axis_steps_per_unit[X_AXIS]);
      SERIAL_PROTOCOLPGM(" Y:");
      SERIAL_PROTOCOL(float(st_get_position(Y_AXIS))/axis_steps_per_unit[Y_AXIS]);
      SERIAL_PROTOCOLPGM(" Z:");
      SERIAL_PROTOCOL(float(st_get_position(Z_AXIS))/axis_steps_per_unit[Z_AXIS]);

      SERIAL_PROTOCOLLN("");
      break;
    case 120: // M120
      enable_endstops(false) ;
      break;
    case 121: // M121
      enable_endstops(true) ;
      break;
    case 119: // M119
    SERIAL_PROTOCOLRPGM(MSG_M119_REPORT);
    SERIAL_PROTOCOLLN("");
      #if defined(X_MIN_PIN) && X_MIN_PIN > -1
        SERIAL_PROTOCOLRPGM(MSG_X_MIN);
        if(READ(X_MIN_PIN)^X_MIN_ENDSTOP_INVERTING){
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_HIT);
        }else{
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_OPEN);
        }
        SERIAL_PROTOCOLLN("");
      #endif
      #if defined(X_MAX_PIN) && X_MAX_PIN > -1
        SERIAL_PROTOCOLRPGM(MSG_X_MAX);
        if(READ(X_MAX_PIN)^X_MAX_ENDSTOP_INVERTING){
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_HIT);
        }else{
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_OPEN);
        }
        SERIAL_PROTOCOLLN("");
      #endif
      #if defined(Y_MIN_PIN) && Y_MIN_PIN > -1
        SERIAL_PROTOCOLRPGM(MSG_Y_MIN);
        if(READ(Y_MIN_PIN)^Y_MIN_ENDSTOP_INVERTING){
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_HIT);
        }else{
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_OPEN);
        }
        SERIAL_PROTOCOLLN("");
      #endif
      #if defined(Y_MAX_PIN) && Y_MAX_PIN > -1
        SERIAL_PROTOCOLRPGM(MSG_Y_MAX);
        if(READ(Y_MAX_PIN)^Y_MAX_ENDSTOP_INVERTING){
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_HIT);
        }else{
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_OPEN);
        }
        SERIAL_PROTOCOLLN("");
      #endif
      #if defined(Z_MIN_PIN) && Z_MIN_PIN > -1
        SERIAL_PROTOCOLRPGM(MSG_Z_MIN);
        if(READ(Z_MIN_PIN)^Z_MIN_ENDSTOP_INVERTING){
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_HIT);
        }else{
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_OPEN);
        }
        SERIAL_PROTOCOLLN("");
      #endif
      #if defined(Z_MAX_PIN) && Z_MAX_PIN > -1
        SERIAL_PROTOCOLRPGM(MSG_Z_MAX);
        if(READ(Z_MAX_PIN)^Z_MAX_ENDSTOP_INVERTING){
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_HIT);
        }else{
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_OPEN);
        }
        SERIAL_PROTOCOLLN("");
      #endif
      break;
      //TODO: update for all axis, use for loop
    #ifdef BLINKM
    case 150: // M150
      {
        byte red;
        byte grn;
        byte blu;

        if(code_seen('R')) red = code_value();
        if(code_seen('U')) grn = code_value();
        if(code_seen('B')) blu = code_value();

        SendColors(red,grn,blu);
      }
      break;
    #endif //BLINKM
    case 200: // M200 D<millimeters> set filament diameter and set E axis units to cubic millimeters (use S0 to set back to millimeters).
      {

        tmp_extruder = active_extruder;
        if(code_seen('T')) {
          tmp_extruder = code_value();
		  if(tmp_extruder >= EXTRUDERS) {
            SERIAL_ECHO_START;
            SERIAL_ECHO(MSG_M200_INVALID_EXTRUDER);
            break;
          }
        }

        if(code_seen('D')) {
		  float diameter = (float)code_value();
		  if (diameter == 0.0) {
			// setting any extruder filament size disables volumetric on the assumption that
			// slicers either generate in extruder values as cubic mm or as as filament feeds
			// for all extruders
		    volumetric_enabled = false;
		  } else {
            filament_size[tmp_extruder] = (float)code_value();
			// make sure all extruders have some sane value for the filament size
			filament_size[0] = (filament_size[0] == 0.0 ? DEFAULT_NOMINAL_FILAMENT_DIA : filament_size[0]);
            #if EXTRUDERS > 1
			filament_size[1] = (filament_size[1] == 0.0 ? DEFAULT_NOMINAL_FILAMENT_DIA : filament_size[1]);
            #if EXTRUDERS > 2
			filament_size[2] = (filament_size[2] == 0.0 ? DEFAULT_NOMINAL_FILAMENT_DIA : filament_size[2]);
            #endif
            #endif
			volumetric_enabled = true;
		  }
        } else {
          //reserved for setting filament diameter via UFID or filament measuring device
          break;
        }
		calculate_volumetric_multipliers();
      }
      break;
    case 201: // M201
      for(int8_t i=0; i < NUM_AXIS; i++)
      {
        if(code_seen(axis_codes[i]))
        {
          max_acceleration_units_per_sq_second[i] = code_value();
        }
      }
      // steps per sq second need to be updated to agree with the units per sq second (as they are what is used in the planner)
      reset_acceleration_rates();
      break;
    #if 0 // Not used for Sprinter/grbl gen6
    case 202: // M202
      for(int8_t i=0; i < NUM_AXIS; i++) {
        if(code_seen(axis_codes[i])) axis_travel_steps_per_sqr_second[i] = code_value() * axis_steps_per_unit[i];
      }
      break;
    #endif
    case 203: // M203 max feedrate mm/sec
      for(int8_t i=0; i < NUM_AXIS; i++) {
        if(code_seen(axis_codes[i])) max_feedrate[i] = code_value();
      }
      break;
    case 204: // M204 acclereration S normal moves T filmanent only moves
      {
        if(code_seen('S')) acceleration = code_value() ;
        if(code_seen('T')) retract_acceleration = code_value() ;
      }
      break;
    case 205: //M205 advanced settings:  minimum travel speed S=while printing T=travel only,  B=minimum segment time X= maximum xy jerk, Z=maximum Z jerk
    {
      if(code_seen('S')) minimumfeedrate = code_value();
      if(code_seen('T')) mintravelfeedrate = code_value();
      if(code_seen('B')) minsegmenttime = code_value() ;
      if(code_seen('X')) max_jerk[X_AXIS] = max_jerk[Y_AXIS] = code_value();
      if(code_seen('Y')) max_jerk[Y_AXIS] = code_value();
      if(code_seen('Z')) max_jerk[Z_AXIS] = code_value();
      if(code_seen('E')) max_jerk[E_AXIS] = code_value();
    }
    break;
    case 206: // M206 additional homing offset
      for(int8_t i=0; i < 3; i++)
      {
        if(code_seen(axis_codes[i])) add_homing[i] = code_value();
      }
      break;
    #ifdef FWRETRACT
    case 207: //M207 - set retract length S[positive mm] F[feedrate mm/min] Z[additional zlift/hop]
    {
      if(code_seen('S'))
      {
        retract_length = code_value() ;
      }
      if(code_seen('F'))
      {
        retract_feedrate = code_value()/60 ;
      }
      if(code_seen('Z'))
      {
        retract_zlift = code_value() ;
      }
    }break;
    case 208: // M208 - set retract recover length S[positive mm surplus to the M207 S*] F[feedrate mm/min]
    {
      if(code_seen('S'))
      {
        retract_recover_length = code_value() ;
      }
      if(code_seen('F'))
      {
        retract_recover_feedrate = code_value()/60 ;
      }
    }break;
    case 209: // M209 - S<1=true/0=false> enable automatic retract detect if the slicer did not support G10/11: every normal extrude-only move will be classified as retract depending on the direction.
    {
      if(code_seen('S'))
      {
        int t= code_value() ;
        switch(t)
        {
          case 0: 
          {
            autoretract_enabled=false;
            retracted[0]=false;
            #if EXTRUDERS > 1
              retracted[1]=false;
            #endif
            #if EXTRUDERS > 2
              retracted[2]=false;
            #endif
          }break;
          case 1: 
          {
            autoretract_enabled=true;
            retracted[0]=false;
            #if EXTRUDERS > 1
              retracted[1]=false;
            #endif
            #if EXTRUDERS > 2
              retracted[2]=false;
            #endif
          }break;
          default:
            SERIAL_ECHO_START;
            SERIAL_ECHORPGM(MSG_UNKNOWN_COMMAND);
            SERIAL_ECHO(CMDBUFFER_CURRENT_STRING);
            SERIAL_ECHOLNPGM("\"");
        }
      }

    }break;
    #endif // FWRETRACT
    #if EXTRUDERS > 1
    case 218: // M218 - set hotend offset (in mm), T<extruder_number> X<offset_on_X> Y<offset_on_Y>
    {
      if(setTargetedHotend(218)){
        break;
      }
      if(code_seen('X'))
      {
        extruder_offset[X_AXIS][tmp_extruder] = code_value();
      }
      if(code_seen('Y'))
      {
        extruder_offset[Y_AXIS][tmp_extruder] = code_value();
      }
      SERIAL_ECHO_START;
      SERIAL_ECHORPGM(MSG_HOTEND_OFFSET);
      for(tmp_extruder = 0; tmp_extruder < EXTRUDERS; tmp_extruder++)
      {
         SERIAL_ECHO(" ");
         SERIAL_ECHO(extruder_offset[X_AXIS][tmp_extruder]);
         SERIAL_ECHO(",");
         SERIAL_ECHO(extruder_offset[Y_AXIS][tmp_extruder]);
      }
      SERIAL_ECHOLN("");
    }break;
    #endif

    case 220: // M220 S<factor in percent>- set speed factor override percentage
    {
	  if (code_seen('B')) //backup current speed factor
	  {
		saved_feedmultiply_mm = feedmultiply;
	  }
      if(code_seen('S'))
      {		
        feedmultiply = code_value() ;
      }
	  if (code_seen('R')) { //restore previous feedmultiply
		  feedmultiply = saved_feedmultiply_mm;
	  }
    }
    break;
    case 221: // M221 S<factor in percent>- set extrude factor override percentage
    {
      if(code_seen('S'))
      {
        int tmp_code = code_value();
        if (code_seen('T'))
        {
          if(setTargetedHotend(221)){
            break;
          }
          extruder_multiply[tmp_extruder] = tmp_code;
        }
        else
        {
          extrudemultiply = tmp_code ;
        }
      }
    }
    break;

	case 226: // M226 P<pin number> S<pin state>- Wait until the specified pin reaches the state required
	{
      if(code_seen('P')){
        int pin_number = code_value(); // pin number
        int pin_state = -1; // required pin state - default is inverted

        if(code_seen('S')) pin_state = code_value(); // required pin state

        if(pin_state >= -1 && pin_state <= 1){

          for(int8_t i = 0; i < (int8_t)(sizeof(sensitive_pins)/sizeof(int)); i++)
          {
            if (sensitive_pins[i] == pin_number)
            {
              pin_number = -1;
              break;
            }
          }

          if (pin_number > -1)
          {
            int target = LOW;

            st_synchronize();

            pinMode(pin_number, INPUT);

            switch(pin_state){
            case 1:
              target = HIGH;
              break;

            case 0:
              target = LOW;
              break;

            case -1:
              target = !digitalRead(pin_number);
              break;
            }

            while(digitalRead(pin_number) != target){
              manage_heater();
              manage_inactivity();
              lcd_update();
            }
          }
        }
      }
    }
    break;

    #if NUM_SERVOS > 0
    case 280: // M280 - set servo position absolute. P: servo index, S: angle or microseconds
      {
        int servo_index = -1;
        int servo_position = 0;
        if (code_seen('P'))
          servo_index = code_value();
        if (code_seen('S')) {
          servo_position = code_value();
          if ((servo_index >= 0) && (servo_index < NUM_SERVOS)) {
#if defined (ENABLE_AUTO_BED_LEVELING) && (PROBE_SERVO_DEACTIVATION_DELAY > 0)
		      servos[servo_index].attach(0);
#endif
            servos[servo_index].write(servo_position);
#if defined (ENABLE_AUTO_BED_LEVELING) && (PROBE_SERVO_DEACTIVATION_DELAY > 0)
              delay(PROBE_SERVO_DEACTIVATION_DELAY);
              servos[servo_index].detach();
#endif
          }
          else {
            SERIAL_ECHO_START;
            SERIAL_ECHO("Servo ");
            SERIAL_ECHO(servo_index);
            SERIAL_ECHOLN(" out of range");
          }
        }
        else if (servo_index >= 0) {
          SERIAL_PROTOCOL(MSG_OK);
          SERIAL_PROTOCOL(" Servo ");
          SERIAL_PROTOCOL(servo_index);
          SERIAL_PROTOCOL(": ");
          SERIAL_PROTOCOL(servos[servo_index].read());
          SERIAL_PROTOCOLLN("");
        }
      }
      break;
    #endif // NUM_SERVOS > 0

    #if (LARGE_FLASH == true && ( BEEPER > 0 || defined(ULTRALCD) || defined(LCD_USE_I2C_BUZZER)))
    case 300: // M300
    {
      int beepS = code_seen('S') ? code_value() : 110;
      int beepP = code_seen('P') ? code_value() : 1000;
      if (beepS > 0)
      {
        #if BEEPER > 0
          tone(BEEPER, beepS);
          delay(beepP);
          noTone(BEEPER);
        #elif defined(ULTRALCD)
		  lcd_buzz(beepS, beepP);
		#elif defined(LCD_USE_I2C_BUZZER)
		  lcd_buzz(beepP, beepS);
        #endif
      }
      else
      {
        delay(beepP);
      }
    }
    break;
    #endif // M300

    #ifdef PIDTEMP
    case 301: // M301
      {
        if(code_seen('P')) Kp = code_value();
        if(code_seen('I')) Ki = scalePID_i(code_value());
        if(code_seen('D')) Kd = scalePID_d(code_value());

        #ifdef PID_ADD_EXTRUSION_RATE
        if(code_seen('C')) Kc = code_value();
        #endif

        updatePID();
        SERIAL_PROTOCOLRPGM(MSG_OK);
        SERIAL_PROTOCOL(" p:");
        SERIAL_PROTOCOL(Kp);
        SERIAL_PROTOCOL(" i:");
        SERIAL_PROTOCOL(unscalePID_i(Ki));
        SERIAL_PROTOCOL(" d:");
        SERIAL_PROTOCOL(unscalePID_d(Kd));
        #ifdef PID_ADD_EXTRUSION_RATE
        SERIAL_PROTOCOL(" c:");
        //Kc does not have scaling applied above, or in resetting defaults
        SERIAL_PROTOCOL(Kc);
        #endif
        SERIAL_PROTOCOLLN("");
      }
      break;
    #endif //PIDTEMP
    #ifdef PIDTEMPBED
    case 304: // M304
      {
        if(code_seen('P')) bedKp = code_value();
        if(code_seen('I')) bedKi = scalePID_i(code_value());
        if(code_seen('D')) bedKd = scalePID_d(code_value());

        updatePID();
       	SERIAL_PROTOCOLRPGM(MSG_OK);
        SERIAL_PROTOCOL(" p:");
        SERIAL_PROTOCOL(bedKp);
        SERIAL_PROTOCOL(" i:");
        SERIAL_PROTOCOL(unscalePID_i(bedKi));
        SERIAL_PROTOCOL(" d:");
        SERIAL_PROTOCOL(unscalePID_d(bedKd));
        SERIAL_PROTOCOLLN("");
      }
      break;
    #endif //PIDTEMP
    case 240: // M240  Triggers a camera by emulating a Canon RC-1 : http://www.doc-diy.net/photo/rc-1_hacked/
     {
     	#ifdef CHDK
       
         SET_OUTPUT(CHDK);
         WRITE(CHDK, HIGH);
         chdkHigh = millis();
         chdkActive = true;
       
       #else
     	
      	#if defined(PHOTOGRAPH_PIN) && PHOTOGRAPH_PIN > -1
	const uint8_t NUM_PULSES=16;
	const float PULSE_LENGTH=0.01524;
	for(int i=0; i < NUM_PULSES; i++) {
        WRITE(PHOTOGRAPH_PIN, HIGH);
        _delay_ms(PULSE_LENGTH);
        WRITE(PHOTOGRAPH_PIN, LOW);
        _delay_ms(PULSE_LENGTH);
        }
        delay(7.33);
        for(int i=0; i < NUM_PULSES; i++) {
        WRITE(PHOTOGRAPH_PIN, HIGH);
        _delay_ms(PULSE_LENGTH);
        WRITE(PHOTOGRAPH_PIN, LOW);
        _delay_ms(PULSE_LENGTH);
        }
      	#endif
      #endif //chdk end if
     }
    break;
#ifdef DOGLCD
    case 250: // M250  Set LCD contrast value: C<value> (value 0..63)
     {
	  if (code_seen('C')) {
	   lcd_setcontrast( ((int)code_value())&63 );
          }
          SERIAL_PROTOCOLPGM("lcd contrast value: ");
          SERIAL_PROTOCOL(lcd_contrast);
          SERIAL_PROTOCOLLN("");
     }
    break;
#endif
    #ifdef PREVENT_DANGEROUS_EXTRUDE
    case 302: // allow cold extrudes, or set the minimum extrude temperature
    {
	  float temp = .0;
	  if (code_seen('S')) temp=code_value();
      set_extrude_min_temp(temp);
    }
    break;
	#endif
    case 303: // M303 PID autotune
    {
      float temp = 150.0;
      int e=0;
      int c=5;
      if (code_seen('E')) e=code_value();
        if (e<0)
          temp=70;
      if (code_seen('S')) temp=code_value();
      if (code_seen('C')) c=code_value();
	  
	  PID_autotune(temp, e, c);
    }
    break;
    case 400: // M400 finish all moves
    {
      st_synchronize();
    }
    break;

#ifdef FILAMENT_SENSOR
case 404:  //M404 Enter the nominal filament width (3mm, 1.75mm ) N<3.0> or display nominal filament width 
    {
    #if (FILWIDTH_PIN > -1) 
    if(code_seen('N')) filament_width_nominal=code_value();
    else{
    SERIAL_PROTOCOLPGM("Filament dia (nominal mm):"); 
    SERIAL_PROTOCOLLN(filament_width_nominal); 
    }
    #endif
    }
    break; 
    
    case 405:  //M405 Turn on filament sensor for control 
    {
    
    
    if(code_seen('D')) meas_delay_cm=code_value();
       
       if(meas_delay_cm> MAX_MEASUREMENT_DELAY)
       	meas_delay_cm = MAX_MEASUREMENT_DELAY;
    
       if(delay_index2 == -1)  //initialize the ring buffer if it has not been done since startup
    	   {
    	   int temp_ratio = widthFil_to_size_ratio(); 
       	    
       	    for (delay_index1=0; delay_index1<(MAX_MEASUREMENT_DELAY+1); ++delay_index1 ){
       	              measurement_delay[delay_index1]=temp_ratio-100;  //subtract 100 to scale within a signed byte
       	        }
       	    delay_index1=0;
       	    delay_index2=0;	
    	   }
    
    filament_sensor = true ; 
    
    //SERIAL_PROTOCOLPGM("Filament dia (measured mm):"); 
    //SERIAL_PROTOCOL(filament_width_meas); 
    //SERIAL_PROTOCOLPGM("Extrusion ratio(%):"); 
    //SERIAL_PROTOCOL(extrudemultiply); 
    } 
    break; 
    
    case 406:  //M406 Turn off filament sensor for control 
    {      
    filament_sensor = false ; 
    } 
    break; 
  
    case 407:   //M407 Display measured filament diameter 
    { 
     
    
    
    SERIAL_PROTOCOLPGM("Filament dia (measured mm):"); 
    SERIAL_PROTOCOLLN(filament_width_meas);   
    } 
    break; 
    #endif

    case 500: // M500 Store settings in EEPROM
    {
        Config_StoreSettings();
    }
    break;
    case 501: // M501 Read settings from EEPROM
    {
        Config_RetrieveSettings();
    }
    break;
    case 502: // M502 Revert to default settings
    {
        Config_ResetDefault();
    }
    break;
    case 503: // M503 print settings currently in memory
    {
        Config_PrintSettings();
    }
    break;
    case 509: //M509 Force language selection
    {
        lcd_force_language_selection();
        SERIAL_ECHO_START;
        SERIAL_PROTOCOLPGM(("LANG SEL FORCED"));
    }
    break;
    #ifdef ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED
    case 540:
    {
        if(code_seen('S')) abort_on_endstop_hit = code_value() > 0;
    }
    break;
    #endif

    #ifdef CUSTOM_M_CODE_SET_Z_PROBE_OFFSET
    case CUSTOM_M_CODE_SET_Z_PROBE_OFFSET:
    {
      float value;
      if (code_seen('Z'))
      {
        value = code_value();
        if ((Z_PROBE_OFFSET_RANGE_MIN <= value) && (value <= Z_PROBE_OFFSET_RANGE_MAX))
        {
          zprobe_zoffset = -value; // compare w/ line 278 of ConfigurationStore.cpp
          SERIAL_ECHO_START;
          SERIAL_ECHOLNRPGM(CAT4(MSG_ZPROBE_ZOFFSET, " ", MSG_OK,PSTR("")));
          SERIAL_PROTOCOLLN("");
        }
        else
        {
          SERIAL_ECHO_START;
          SERIAL_ECHORPGM(MSG_ZPROBE_ZOFFSET);
          SERIAL_ECHORPGM(MSG_Z_MIN);
          SERIAL_ECHO(Z_PROBE_OFFSET_RANGE_MIN);
          SERIAL_ECHORPGM(MSG_Z_MAX);
          SERIAL_ECHO(Z_PROBE_OFFSET_RANGE_MAX);
          SERIAL_PROTOCOLLN("");
        }
      }
      else
      {
          SERIAL_ECHO_START;
          SERIAL_ECHOLNRPGM(CAT2(MSG_ZPROBE_ZOFFSET, PSTR(" : ")));
          SERIAL_ECHO(-zprobe_zoffset);
          SERIAL_PROTOCOLLN("");
      }
      break;
    }
    #endif // CUSTOM_M_CODE_SET_Z_PROBE_OFFSET

    #ifdef FILAMENTCHANGEENABLE
    case 600: //Pause for filament change X[pos] Y[pos] Z[relative lift] E[initial retract] L[later retract distance for removal]
    {

		st_synchronize();
		float target[4];
		float lastpos[4];

        if (farm_mode)
            
        {
            
            prusa_statistics(22);
            
        }
        
        feedmultiplyBckp=feedmultiply;

        target[X_AXIS]=current_position[X_AXIS];
        target[Y_AXIS]=current_position[Y_AXIS];
        target[Z_AXIS]=current_position[Z_AXIS];
        target[E_AXIS]=current_position[E_AXIS];
        lastpos[X_AXIS]=current_position[X_AXIS];
        lastpos[Y_AXIS]=current_position[Y_AXIS];
        lastpos[Z_AXIS]=current_position[Z_AXIS];
        lastpos[E_AXIS]=current_position[E_AXIS];

        //Retract extruder
        if(code_seen('E'))
        {
          target[E_AXIS]+= code_value();
        }
        else
        {
          #ifdef FILAMENTCHANGE_FIRSTRETRACT
            target[E_AXIS]+= FILAMENTCHANGE_FIRSTRETRACT ;
          #endif
        }
        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], FILAMENTCHANGE_RFEED, active_extruder);

        //Lift Z
        if(code_seen('Z'))
        {
          target[Z_AXIS]+= code_value();
        }
        else
        {
          #ifdef FILAMENTCHANGE_ZADD
            target[Z_AXIS]+= FILAMENTCHANGE_ZADD ;
	    // XXX: Removed unused var 'TooLowZ'
            if(target[Z_AXIS] < 10){
              target[Z_AXIS]+= 10 ;
            }
          #endif
     
          
        }
        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], FILAMENTCHANGE_ZFEED, active_extruder);

        //Move XY to side
        if(code_seen('X'))
        {
          target[X_AXIS]+= code_value();
        }
        else
        {
          #ifdef FILAMENTCHANGE_XPOS
            target[X_AXIS]= FILAMENTCHANGE_XPOS ;
          #endif
        }
        if(code_seen('Y'))
        {
          target[Y_AXIS]= code_value();
        }
        else
        {
          #ifdef FILAMENTCHANGE_YPOS
            target[Y_AXIS]= FILAMENTCHANGE_YPOS ;
          #endif
        }
        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], FILAMENTCHANGE_XYFEED, active_extruder);
		st_synchronize();
		custom_message = true;
		lcd_setstatuspgm(MSG_UNLOADING_FILAMENT);

        // Unload filament
        if(code_seen('L'))
        {
          target[E_AXIS]+= code_value();
        }
        else
        {
			#ifdef SNMM

			#else
				#ifdef FILAMENTCHANGE_FINALRETRACT
							target[E_AXIS] += FILAMENTCHANGE_FINALRETRACT;
				#endif
			#endif // SNMM
        }

#ifdef SNMM
		target[E_AXIS] += 12;
		plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 3500, active_extruder);
		target[E_AXIS] += 6;
		plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 5000, active_extruder);
		target[E_AXIS] += (FIL_LOAD_LENGTH * -1);
		plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 5000, active_extruder);
		st_synchronize();
		target[E_AXIS] += (FIL_COOLING);
		plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 50, active_extruder);
		target[E_AXIS] += (FIL_COOLING*-1);
		plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 50, active_extruder);
		target[E_AXIS] += (bowden_length[snmm_extruder] *-1);
		plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 3000, active_extruder);
		st_synchronize();

#else
		plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], FILAMENTCHANGE_RFEED, active_extruder);
#endif // SNMM
		     

        //finish moves
        st_synchronize();
        //disable extruder steppers so filament can be removed
        disable_e0();
        disable_e1();
        disable_e2();
        delay(100);
        
        //Wait for user to insert filament
        uint8_t cnt=0;
        int counterBeep = 0;
        lcd_wait_interact();
		load_filament_time = millis();
		KEEPALIVE_STATE(PAUSED_FOR_USER);
        while(!lcd_clicked()){

		  cnt++;
          manage_heater();
          manage_inactivity(true);

/*#ifdef SNMM
		  target[E_AXIS] += 0.002;
		  plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 500, active_extruder);

#endif // SNMM*/

          if(cnt==0)
          {
          #if BEEPER > 0
            if (counterBeep== 500){
              counterBeep = 0;  
            }
            SET_OUTPUT(BEEPER);
            if (counterBeep== 0){
              WRITE(BEEPER,HIGH);
            }			
            if (counterBeep== 20){
              WRITE(BEEPER,LOW);
            }
            counterBeep++;
          #else
			   #if !defined(LCD_FEEDBACK_FREQUENCY_HZ) || !defined(LCD_FEEDBACK_FREQUENCY_DURATION_MS)
              lcd_buzz(1000/6,100);
			   #else
			     lcd_buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS,LCD_FEEDBACK_FREQUENCY_HZ);
			   #endif
          #endif
          }

        }
		KEEPALIVE_STATE(IN_HANDLER);
		WRITE(BEEPER, LOW);
#ifdef SNMM
		display_loading();
		KEEPALIVE_STATE(PAUSED_FOR_USER);
		do {
			target[E_AXIS] += 0.002;
			plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 500, active_extruder);
			delay_keep_alive(2);
		} while (!lcd_clicked());
		KEEPALIVE_STATE(IN_HANDLER);
		/*if (millis() - load_filament_time > 2) {
			load_filament_time = millis();
			target[E_AXIS] += 0.001;
			plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 1000, active_extruder);
		}*/
#endif
        //Filament inserted
        
        

		//Feed the filament to the end of nozzle quickly        
#ifdef SNMM
		
		st_synchronize();
		target[E_AXIS] += bowden_length[snmm_extruder];
		plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 3000, active_extruder);
		target[E_AXIS] += FIL_LOAD_LENGTH - 60;
		plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 1400, active_extruder);
		target[E_AXIS] += 40;
		plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 400, active_extruder);
		target[E_AXIS] += 10;
		plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 50, active_extruder);
#else
		target[E_AXIS] += FILAMENTCHANGE_FIRSTFEED;
		plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], FILAMENTCHANGE_EFEED, active_extruder);
#endif // SNMM
        
        //Extrude some filament
        target[E_AXIS]+= FILAMENTCHANGE_FINALFEED ;
        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], FILAMENTCHANGE_EXFEED, active_extruder); 
        
 

        
        //Wait for user to check the state
        lcd_change_fil_state = 0;
        lcd_loading_filament();
        while ((lcd_change_fil_state == 0)||(lcd_change_fil_state != 1)){
          lcd_change_fil_state = 0;
		  KEEPALIVE_STATE(PAUSED_FOR_USER);
          lcd_alright();
		  KEEPALIVE_STATE(IN_HANDLER);
          switch(lcd_change_fil_state){
            
             // Filament failed to load so load it again
             case 2:
#ifdef SNMM
				 display_loading();
				 do {
					 target[E_AXIS] += 0.002;
					 plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 500, active_extruder);
					 delay_keep_alive(2);
				 } while (!lcd_clicked());

				 st_synchronize();
				 target[E_AXIS] += bowden_length[snmm_extruder];
				 plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 3000, active_extruder);
				 target[E_AXIS] += FIL_LOAD_LENGTH - 60;
				 plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 1400, active_extruder);
				 target[E_AXIS] += 40;
				 plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 400, active_extruder);
				 target[E_AXIS] += 10;
				 plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 50, active_extruder);

#else
                     target[E_AXIS]+= FILAMENTCHANGE_FIRSTFEED ;
                     plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], FILAMENTCHANGE_EFEED, active_extruder); 
#endif                
                     target[E_AXIS]+= FILAMENTCHANGE_FINALFEED ;
                     plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], FILAMENTCHANGE_EXFEED, active_extruder); 

                     lcd_loading_filament();

                     break;

             // Filament loaded properly but color is not clear
             case 3:
                     target[E_AXIS]+= FILAMENTCHANGE_FINALFEED ;
                     plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 2, active_extruder); 
                     lcd_loading_color();
                     break;
                 
             // Everything good             
             default:
                     lcd_change_success();
					 lcd_update_enable(true);
                     break;
          }
          
        }
        

      //Not let's go back to print

      //Feed a little of filament to stabilize pressure
      target[E_AXIS]+= FILAMENTCHANGE_RECFEED;
      plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], FILAMENTCHANGE_EXFEED, active_extruder);
        
      //Retract
      target[E_AXIS]+= FILAMENTCHANGE_FIRSTRETRACT;
      plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], FILAMENTCHANGE_RFEED, active_extruder);
        

        
      //plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], 70, active_extruder); //should do nothing
      
      //Move XY back
      plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], target[Z_AXIS], target[E_AXIS], FILAMENTCHANGE_XYFEED, active_extruder);
      
      //Move Z back
      plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], lastpos[Z_AXIS], target[E_AXIS], FILAMENTCHANGE_ZFEED, active_extruder);
        
        
      target[E_AXIS]= target[E_AXIS] - FILAMENTCHANGE_FIRSTRETRACT;
        
      //Unretract       
      plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], lastpos[Z_AXIS], target[E_AXIS], FILAMENTCHANGE_RFEED, active_extruder);
        
      //Set E position to original  
      plan_set_e_position(lastpos[E_AXIS]);
       
      //Recover feed rate 
      feedmultiply=feedmultiplyBckp;
      char cmd[9];
      sprintf_P(cmd, PSTR("M220 S%i"), feedmultiplyBckp);
      enquecommand(cmd);
      
	  lcd_setstatuspgm(WELCOME_MSG);
	  custom_message = false;
	  custom_message_type = 0;
        
    }
    break;
    #endif //FILAMENTCHANGEENABLE
	case 601: {
		if(lcd_commands_type == 0)  lcd_commands_type = LCD_COMMAND_LONG_PAUSE;
	}
	break;

	case 602: {
		if(lcd_commands_type == 0)	lcd_commands_type = LCD_COMMAND_LONG_PAUSE_RESUME;
	}
	break;

    #ifdef LIN_ADVANCE
      case 900: // M900: Set LIN_ADVANCE options.
        gcode_M900();
        break;
    #endif
      
    case 907: // M907 Set digital trimpot motor current using axis codes.
    {
      #if defined(DIGIPOTSS_PIN) && DIGIPOTSS_PIN > -1
        for(int i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) digipot_current(i,code_value());
        if(code_seen('B')) digipot_current(4,code_value());
        if(code_seen('S')) for(int i=0;i<=4;i++) digipot_current(i,code_value());
      #endif
      #ifdef MOTOR_CURRENT_PWM_XY_PIN
        if(code_seen('X')) digipot_current(0, code_value());
      #endif
      #ifdef MOTOR_CURRENT_PWM_Z_PIN
        if(code_seen('Z')) digipot_current(1, code_value());
      #endif
      #ifdef MOTOR_CURRENT_PWM_E_PIN
        if(code_seen('E')) digipot_current(2, code_value());
      #endif
      #ifdef DIGIPOT_I2C
        // this one uses actual amps in floating point
        for(int i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) digipot_i2c_set_current(i, code_value());
        // for each additional extruder (named B,C,D,E..., channels 4,5,6,7...)
        for(int i=NUM_AXIS;i<DIGIPOT_I2C_NUM_CHANNELS;i++) if(code_seen('B'+i-NUM_AXIS)) digipot_i2c_set_current(i, code_value());
      #endif
    }
    break;
    case 908: // M908 Control digital trimpot directly.
    {
      #if defined(DIGIPOTSS_PIN) && DIGIPOTSS_PIN > -1
        uint8_t channel,current;
        if(code_seen('P')) channel=code_value();
        if(code_seen('S')) current=code_value();
        digitalPotWrite(channel, current);
      #endif
    }
    break;
    case 350: // M350 Set microstepping mode. Warning: Steps per unit remains unchanged. S code sets stepping mode for all drivers.
    {
      #if defined(X_MS1_PIN) && X_MS1_PIN > -1
        if(code_seen('S')) for(int i=0;i<=4;i++) microstep_mode(i,code_value());
        for(int i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) microstep_mode(i,(uint8_t)code_value());
        if(code_seen('B')) microstep_mode(4,code_value());
        microstep_readings();
      #endif
    }
    break;
    case 351: // M351 Toggle MS1 MS2 pins directly, S# determines MS1 or MS2, X# sets the pin high/low.
    {
      #if defined(X_MS1_PIN) && X_MS1_PIN > -1
      if(code_seen('S')) switch((int)code_value())
      {
        case 1:
          for(int i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) microstep_ms(i,code_value(),-1);
          if(code_seen('B')) microstep_ms(4,code_value(),-1);
          break;
        case 2:
          for(int i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) microstep_ms(i,-1,code_value());
          if(code_seen('B')) microstep_ms(4,-1,code_value());
          break;
      }
      microstep_readings();
      #endif
    }
    break;
	case 701: //M701: load filament
	{
		gcode_M701();
	}
	break;
	case 702:
	{
#ifdef SNMM
		if (code_seen('U')) {
			extr_unload_used(); //unload all filaments which were used in current print
		}
		else if (code_seen('C')) {
			extr_unload(); //unload just current filament 
		}
		else {
			extr_unload_all(); //unload all filaments
		}
#else
		custom_message = true;
		custom_message_type = 2;
		lcd_setstatuspgm(MSG_UNLOADING_FILAMENT); 
		current_position[E_AXIS] -= 80;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 7000 / 60, active_extruder);
		st_synchronize();
		lcd_setstatuspgm(WELCOME_MSG);
		custom_message = false;
		custom_message_type = 0;
#endif	
	}
	break;

    case 999: // M999: Restart after being stopped
      Stopped = false;
      lcd_reset_alert_level();
      gcode_LastN = Stopped_gcode_LastN;
      FlushSerialRequestResend();
    break;
	default: SERIAL_ECHOLNPGM("Invalid M code.");
    }
	
  } // end if(code_seen('M')) (end of M codes)

  else if(code_seen('T'))
  {
	
	  int index;
	  st_synchronize();
	  for (index = 1; *(strchr_pointer + index) == ' ' || *(strchr_pointer + index) == '\t'; index++);
	   
	  if ((*(strchr_pointer + index) < '0' || *(strchr_pointer + index) > '9') && *(strchr_pointer + index) != '?') {
		  SERIAL_ECHOLNPGM("Invalid T code.");
	  }
	  else {
		  if (*(strchr_pointer + index) == '?') {
			  tmp_extruder = choose_extruder_menu();
		  }
		  else {
			  tmp_extruder = code_value();
		  }
		  snmm_filaments_used |= (1 << tmp_extruder); //for stop print
#ifdef SNMM
      #ifdef LIN_ADVANCE
        if (snmm_extruder != tmp_extruder)
          clear_current_adv_vars(); //Check if the selected extruder is not the active one and reset LIN_ADVANCE variables if so.
      #endif
      
      snmm_extruder = tmp_extruder;

		  delay(100);

		  disable_e0();
		  disable_e1();
		  disable_e2();

		  pinMode(E_MUX0_PIN, OUTPUT);
		  pinMode(E_MUX1_PIN, OUTPUT);

		  delay(100);
		  SERIAL_ECHO_START;
		  SERIAL_ECHO("T:");
		  SERIAL_ECHOLN((int)tmp_extruder);
		  switch (tmp_extruder) {
		  case 1:
			  WRITE(E_MUX0_PIN, HIGH);
			  WRITE(E_MUX1_PIN, LOW);
			  break;
		  case 2:
			  WRITE(E_MUX0_PIN, LOW);
			  WRITE(E_MUX1_PIN, HIGH);
			  break;
		  case 3:
			  WRITE(E_MUX0_PIN, HIGH);
			  WRITE(E_MUX1_PIN, HIGH);
			  break;
		  default:
			  WRITE(E_MUX0_PIN, LOW);
			  WRITE(E_MUX1_PIN, LOW);
			  break;
		  }
		  delay(100);

#else
		  if (tmp_extruder >= EXTRUDERS) {
			  SERIAL_ECHO_START;
			  SERIAL_ECHOPGM("T");
			  SERIAL_PROTOCOLLN((int)tmp_extruder);
			  SERIAL_ECHOLNRPGM(MSG_INVALID_EXTRUDER);
		  }
		  else {
#if EXTRUDERS == 1
			  if (code_seen('F')) {
				  next_feedrate = code_value();
				  if (next_feedrate > 0.0) {
					  feedrate = next_feedrate;
				  }
			  }
#else
			  boolean make_move = false;
			  if (code_seen('F')) {
				  make_move = true;
				  next_feedrate = code_value();
				  if (next_feedrate > 0.0) {
					  feedrate = next_feedrate;
				  }
			  }
			  if (tmp_extruder != active_extruder) {
				  // Save current position to return to after applying extruder offset
				  memcpy(destination, current_position, sizeof(destination));
				  // Offset extruder (only by XY)
				  int i;
				  for (i = 0; i < 2; i++) {
					  current_position[i] = current_position[i] -
						  extruder_offset[i][active_extruder] +
						  extruder_offset[i][tmp_extruder];
				  }
				  // Set the new active extruder and position
				  active_extruder = tmp_extruder;
				  plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
				  // Move to the old position if 'F' was in the parameters
				  if (make_move && Stopped == false) {
					  prepare_move();
				  }
			  }
#endif
			  SERIAL_ECHO_START;
			  SERIAL_ECHORPGM(MSG_ACTIVE_EXTRUDER);
			  SERIAL_PROTOCOLLN((int)active_extruder);
		  }

#endif
	  }
  } // end if(code_seen('T')) (end of T codes)

#ifdef DEBUG_DCODES
  else if (code_seen('D')) // D codes (debug)
  {
    switch((int)code_value_uint8())
    {
	case 0: // D0 - Reset
		if (*(strchr_pointer + 1) == 0) break;
		MYSERIAL.println("D0 - Reset");
		asm volatile("jmp 0x00000");
		break;
	case 1: // D1 - Clear EEPROM
		{
			MYSERIAL.println("D1 - Clear EEPROM");
			cli();
			for (int i = 0; i < 4096; i++)
				eeprom_write_byte((unsigned char*)i, (unsigned char)0);
			sei();
		}
		break;
	case 2: // D2 - Read/Write PIN
		{
			if (code_seen('P')) // Pin (0-255)
			{
				int pin = (int)code_value();
				if ((pin >= 0) && (pin <= 255))
				{
					if (code_seen('F')) // Function in/out (0/1)
					{
						int fnc = (int)code_value();
						if (fnc == 0) pinMode(pin, INPUT);
						else if (fnc == 1) pinMode(pin, OUTPUT);
					}
					if (code_seen('V')) // Value (0/1)
					{
						int val = (int)code_value();
						if (val == 0) digitalWrite(pin, LOW);
						else if (val == 1) digitalWrite(pin, HIGH);
					}
					else
					{
						int val = (digitalRead(pin) != LOW)?1:0;
						MYSERIAL.print("PIN");
						MYSERIAL.print(pin);
						MYSERIAL.print("=");
						MYSERIAL.println(val);
					}
				}
			}
		}
		break;
	}
  }
#endif //DEBUG_DCODES

  else
  {
    SERIAL_ECHO_START;
    SERIAL_ECHORPGM(MSG_UNKNOWN_COMMAND);
    SERIAL_ECHO(CMDBUFFER_CURRENT_STRING);
    SERIAL_ECHOLNPGM("\"");
  }

  KEEPALIVE_STATE(NOT_BUSY);

  ClearToSend();
}

void FlushSerialRequestResend()
{
  //char cmdbuffer[bufindr][100]="Resend:";
  MYSERIAL.flush();
  SERIAL_PROTOCOLRPGM(MSG_RESEND);
  SERIAL_PROTOCOLLN(gcode_LastN + 1);
  ClearToSend();
}

// Confirm the execution of a command, if sent from a serial line.
// Execution of a command from a SD card will not be confirmed.
void ClearToSend()
{
    previous_millis_cmd = millis();
    if (CMDBUFFER_CURRENT_TYPE == CMDBUFFER_CURRENT_TYPE_USB)
        SERIAL_PROTOCOLLNRPGM(MSG_OK);
}

void update_currents() {
	float current_high[3] = DEFAULT_PWM_MOTOR_CURRENT_LOUD;
	float current_low[3] = DEFAULT_PWM_MOTOR_CURRENT;
	float tmp_motor[3];
	
	//SERIAL_ECHOLNPGM("Currents updated: ");

	if (destination[Z_AXIS] < Z_SILENT) {
		//SERIAL_ECHOLNPGM("LOW");
		for (uint8_t i = 0; i < 3; i++) {
			digipot_current(i, current_low[i]);		
			/*MYSERIAL.print(int(i));
			SERIAL_ECHOPGM(": ");
			MYSERIAL.println(current_low[i]);*/
		}		
	}
	else if (destination[Z_AXIS] > Z_HIGH_POWER) {
		//SERIAL_ECHOLNPGM("HIGH");
		for (uint8_t i = 0; i < 3; i++) {
			digipot_current(i, current_high[i]);
			/*MYSERIAL.print(int(i));
			SERIAL_ECHOPGM(": ");
			MYSERIAL.println(current_high[i]);*/
		}		
	}
	else {
		for (uint8_t i = 0; i < 3; i++) {
			float q = current_low[i] - Z_SILENT*((current_high[i] - current_low[i]) / (Z_HIGH_POWER - Z_SILENT));
			tmp_motor[i] = ((current_high[i] - current_low[i]) / (Z_HIGH_POWER - Z_SILENT))*destination[Z_AXIS] + q;
			digipot_current(i, tmp_motor[i]);			
			/*MYSERIAL.print(int(i));
			SERIAL_ECHOPGM(": ");
			MYSERIAL.println(tmp_motor[i]);*/
		}
	}
}

void get_coordinates()
{
  // XXX: Unused var (set but not ref)
  // bool seen[4]={false,false,false,false};
  for(int8_t i=0; i < NUM_AXIS; i++) {
    if(code_seen(axis_codes[i]))
    {
	  destination[i] = (float)code_value() + (axis_relative_modes[i] || relative_mode)*current_position[i];
	  // seen[i]=true;
	  if (i == Z_AXIS && SilentModeMenu == 2) update_currents();
    }
    else destination[i] = current_position[i]; //Are these else lines really needed?
  }
  if(code_seen('F')) {
    next_feedrate = code_value();
    if(next_feedrate > 0.0) feedrate = next_feedrate;
  }
}

void get_arc_coordinates()
{
#ifdef SF_ARC_FIX
   bool relative_mode_backup = relative_mode;
   relative_mode = true;
#endif
   get_coordinates();
#ifdef SF_ARC_FIX
   relative_mode=relative_mode_backup;
#endif

   if(code_seen('I')) {
     offset[0] = code_value();
   }
   else {
     offset[0] = 0.0;
   }
   if(code_seen('J')) {
     offset[1] = code_value();
   }
   else {
     offset[1] = 0.0;
   }
}

void clamp_to_software_endstops(float target[3])
{
#ifdef DEBUG_DISABLE_SWLIMITS
	return;
#endif //DEBUG_DISABLE_SWLIMITS
    world2machine_clamp(target[0], target[1]);

    // Clamp the Z coordinate.
    if (min_software_endstops) {
        float negative_z_offset = 0;
        #ifdef ENABLE_AUTO_BED_LEVELING
            if (Z_PROBE_OFFSET_FROM_EXTRUDER < 0) negative_z_offset = negative_z_offset + Z_PROBE_OFFSET_FROM_EXTRUDER;
            if (add_homing[Z_AXIS] < 0) negative_z_offset = negative_z_offset + add_homing[Z_AXIS];
        #endif
        if (target[Z_AXIS] < min_pos[Z_AXIS]+negative_z_offset) target[Z_AXIS] = min_pos[Z_AXIS]+negative_z_offset;
    }
    if (max_software_endstops) {
        if (target[Z_AXIS] > max_pos[Z_AXIS]) target[Z_AXIS] = max_pos[Z_AXIS];
    }
}

#ifdef MESH_BED_LEVELING
    void mesh_plan_buffer_line(const float &x, const float &y, const float &z, const float &e, const float &feed_rate, const uint8_t extruder) {
        float dx = x - current_position[X_AXIS];
        float dy = y - current_position[Y_AXIS];
        float dz = z - current_position[Z_AXIS];
        int n_segments = 0;
		
        if (mbl.active) {
            float len = abs(dx) + abs(dy);
            if (len > 0)
                // Split to 3cm segments or shorter.
                n_segments = int(ceil(len / 30.f));
        }
        
        if (n_segments > 1) {
            float de = e - current_position[E_AXIS];
            for (int i = 1; i < n_segments; ++ i) {
                float t = float(i) / float(n_segments);
                plan_buffer_line(
                                 current_position[X_AXIS] + t * dx,
                                 current_position[Y_AXIS] + t * dy,
                                 current_position[Z_AXIS] + t * dz,
                                 current_position[E_AXIS] + t * de,
                                 feed_rate, extruder);
            }
        }
        // The rest of the path.
        plan_buffer_line(x, y, z, e, feed_rate, extruder);
        current_position[X_AXIS] = x;
        current_position[Y_AXIS] = y;
        current_position[Z_AXIS] = z;
        current_position[E_AXIS] = e;
    }
#endif  // MESH_BED_LEVELING
    
void prepare_move()
{
  clamp_to_software_endstops(destination);
  previous_millis_cmd = millis();

  // Do not use feedmultiply for E or Z only moves
  if( (current_position[X_AXIS] == destination [X_AXIS]) && (current_position[Y_AXIS] == destination [Y_AXIS])) {
      plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
  }
  else {
#ifdef MESH_BED_LEVELING
    mesh_plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate*feedmultiply*(1./(60.f*100.f)), active_extruder);
#else
     plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate*feedmultiply*(1./(60.f*100.f)), active_extruder);
#endif
  }

  for(int8_t i=0; i < NUM_AXIS; i++) {
    current_position[i] = destination[i];
  }
}

void prepare_arc_move(char isclockwise) {
  float r = hypot(offset[X_AXIS], offset[Y_AXIS]); // Compute arc radius for mc_arc

  // Trace the arc
  mc_arc(current_position, destination, offset, X_AXIS, Y_AXIS, Z_AXIS, feedrate*feedmultiply/60/100.0, r, isclockwise, active_extruder);

  // As far as the parser is concerned, the position is now == target. In reality the
  // motion control system might still be processing the action and the real tool position
  // in any intermediate location.
  for(int8_t i=0; i < NUM_AXIS; i++) {
    current_position[i] = destination[i];
  }
  previous_millis_cmd = millis();
}

#if defined(CONTROLLERFAN_PIN) && CONTROLLERFAN_PIN > -1

#if defined(FAN_PIN)
  #if CONTROLLERFAN_PIN == FAN_PIN
    #error "You cannot set CONTROLLERFAN_PIN equal to FAN_PIN"
  #endif
#endif

unsigned long lastMotor = 0; //Save the time for when a motor was turned on last
unsigned long lastMotorCheck = 0;

void controllerFan()
{
  if ((millis() - lastMotorCheck) >= 2500) //Not a time critical function, so we only check every 2500ms
  {
    lastMotorCheck = millis();

    if(!READ(X_ENABLE_PIN) || !READ(Y_ENABLE_PIN) || !READ(Z_ENABLE_PIN) || (soft_pwm_bed > 0)
    #if EXTRUDERS > 2
       || !READ(E2_ENABLE_PIN)
    #endif
    #if EXTRUDER > 1
      #if defined(X2_ENABLE_PIN) && X2_ENABLE_PIN > -1
       || !READ(X2_ENABLE_PIN)
      #endif
       || !READ(E1_ENABLE_PIN)
    #endif
       || !READ(E0_ENABLE_PIN)) //If any of the drivers are enabled...
    {
      lastMotor = millis(); //... set time to NOW so the fan will turn on
    }

    if ((millis() - lastMotor) >= (CONTROLLERFAN_SECS*1000UL) || lastMotor == 0) //If the last time any driver was enabled, is longer since than CONTROLLERSEC...
    {
        digitalWrite(CONTROLLERFAN_PIN, 0);
        analogWrite(CONTROLLERFAN_PIN, 0);
    }
    else
    {
        // allows digital or PWM fan output to be used (see M42 handling)
        digitalWrite(CONTROLLERFAN_PIN, CONTROLLERFAN_SPEED);
        analogWrite(CONTROLLERFAN_PIN, CONTROLLERFAN_SPEED);
    }
  }
}
#endif

#ifdef TEMP_STAT_LEDS
static bool blue_led = false;
static bool red_led = false;
static uint32_t stat_update = 0;

void handle_status_leds(void) {
  float max_temp = 0.0;
  if(millis() > stat_update) {
    stat_update += 500; // Update every 0.5s
    for (int8_t cur_extruder = 0; cur_extruder < EXTRUDERS; ++cur_extruder) {
       max_temp = max(max_temp, degHotend(cur_extruder));
       max_temp = max(max_temp, degTargetHotend(cur_extruder));
    }
    #if defined(TEMP_BED_PIN) && TEMP_BED_PIN > -1
      max_temp = max(max_temp, degTargetBed());
      max_temp = max(max_temp, degBed());
    #endif
    if((max_temp > 55.0) && (red_led == false)) {
      digitalWrite(STAT_LED_RED, 1);
      digitalWrite(STAT_LED_BLUE, 0);
      red_led = true;
      blue_led = false;
    }
    if((max_temp < 54.0) && (blue_led == false)) {
      digitalWrite(STAT_LED_RED, 0);
      digitalWrite(STAT_LED_BLUE, 1);
      red_led = false;
      blue_led = true;
    }
  }
}
#endif

void manage_inactivity(bool ignore_stepper_queue/*=false*/) //default argument set in Marlin.h
{
	
#if defined(KILL_PIN) && KILL_PIN > -1
	static int killCount = 0;   // make the inactivity button a bit less responsive
   const int KILL_DELAY = 10000;
#endif

    if(buflen < (BUFSIZE-1)){
        get_command();
    }

  if( (millis() - previous_millis_cmd) >  max_inactive_time )
    if(max_inactive_time)
      kill();
  if(stepper_inactive_time)  {
    if( (millis() - previous_millis_cmd) >  stepper_inactive_time )
    {
      if(blocks_queued() == false && ignore_stepper_queue == false) {
        disable_x();
//        SERIAL_ECHOLNPGM("manage_inactivity - disable Y");
        disable_y();
        disable_z();
        disable_e0();
        disable_e1();
        disable_e2();
      }
    }
  }
  
  #ifdef CHDK //Check if pin should be set to LOW after M240 set it to HIGH
    if (chdkActive && (millis() - chdkHigh > CHDK_DELAY))
    {
      chdkActive = false;
      WRITE(CHDK, LOW);
    }
  #endif
  
  #if defined(KILL_PIN) && KILL_PIN > -1
    
    // Check if the kill button was pressed and wait just in case it was an accidental
    // key kill key press
    // -------------------------------------------------------------------------------
    if( 0 == READ(KILL_PIN) )
    {
       killCount++;
    }
    else if (killCount > 0)
    {
       killCount--;
    }
    // Exceeded threshold and we can confirm that it was not accidental
    // KILL the machine
    // ----------------------------------------------------------------
    if ( killCount >= KILL_DELAY)
    {
       kill();
    }
  #endif
    
  #if defined(CONTROLLERFAN_PIN) && CONTROLLERFAN_PIN > -1
    controllerFan(); //Check if fan should be turned on to cool stepper drivers down
  #endif
  #ifdef EXTRUDER_RUNOUT_PREVENT
    if( (millis() - previous_millis_cmd) >  EXTRUDER_RUNOUT_SECONDS*1000 )
    if(degHotend(active_extruder)>EXTRUDER_RUNOUT_MINTEMP)
    {
     bool oldstatus=READ(E0_ENABLE_PIN);
     enable_e0();
     float oldepos=current_position[E_AXIS];
     float oldedes=destination[E_AXIS];
     plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS],
                      destination[E_AXIS]+EXTRUDER_RUNOUT_EXTRUDE*EXTRUDER_RUNOUT_ESTEPS/axis_steps_per_unit[E_AXIS],
                      EXTRUDER_RUNOUT_SPEED/60.*EXTRUDER_RUNOUT_ESTEPS/axis_steps_per_unit[E_AXIS], active_extruder);
     current_position[E_AXIS]=oldepos;
     destination[E_AXIS]=oldedes;
     plan_set_e_position(oldepos);
     previous_millis_cmd=millis();
     st_synchronize();
     WRITE(E0_ENABLE_PIN,oldstatus);
    }
  #endif
  #ifdef TEMP_STAT_LEDS
      handle_status_leds();
  #endif
  check_axes_activity();
}

void kill(const char *full_screen_message)
{
  cli(); // Stop interrupts
  disable_heater();

  disable_x();
//  SERIAL_ECHOLNPGM("kill - disable Y");
  disable_y();
  disable_z();
  disable_e0();
  disable_e1();
  disable_e2();

#if defined(PS_ON_PIN) && PS_ON_PIN > -1
  pinMode(PS_ON_PIN,INPUT);
#endif
  SERIAL_ERROR_START;
  SERIAL_ERRORLNRPGM(MSG_ERR_KILLED);
  if (full_screen_message != NULL) {
      SERIAL_ERRORLNRPGM(full_screen_message);
      lcd_display_message_fullscreen_P(full_screen_message);
  } else {
      LCD_ALERTMESSAGERPGM(MSG_KILLED);
  }

  // FMC small patch to update the LCD before ending
  sei();   // enable interrupts
  for ( int i=5; i--; lcd_update())
  {
     delay(200);	
  }
  cli();   // disable interrupts
  suicide();
  while(1) { /* Intentionally left empty */ } // Wait for reset
}

void Stop()
{
  disable_heater();
  if(Stopped == false) {
    Stopped = true;
    Stopped_gcode_LastN = gcode_LastN; // Save last g_code for restart
    SERIAL_ERROR_START;
    SERIAL_ERRORLNRPGM(MSG_ERR_STOPPED);
    LCD_MESSAGERPGM(MSG_STOPPED);
  }
}

bool IsStopped() { return Stopped; };

#ifdef FAST_PWM_FAN
void setPwmFrequency(uint8_t pin, int val)
{
  val &= 0x07;
  switch(digitalPinToTimer(pin))
  {

    #if defined(TCCR0A)
    case TIMER0A:
    case TIMER0B:
//         TCCR0B &= ~(_BV(CS00) | _BV(CS01) | _BV(CS02));
//         TCCR0B |= val;
         break;
    #endif

    #if defined(TCCR1A)
    case TIMER1A:
    case TIMER1B:
//         TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
//         TCCR1B |= val;
         break;
    #endif

    #if defined(TCCR2)
    case TIMER2:
    case TIMER2:
         TCCR2 &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
         TCCR2 |= val;
         break;
    #endif

    #if defined(TCCR2A)
    case TIMER2A:
    case TIMER2B:
         TCCR2B &= ~(_BV(CS20) | _BV(CS21) | _BV(CS22));
         TCCR2B |= val;
         break;
    #endif

    #if defined(TCCR3A)
    case TIMER3A:
    case TIMER3B:
    case TIMER3C:
         TCCR3B &= ~(_BV(CS30) | _BV(CS31) | _BV(CS32));
         TCCR3B |= val;
         break;
    #endif

    #if defined(TCCR4A)
    case TIMER4A:
    case TIMER4B:
    case TIMER4C:
         TCCR4B &= ~(_BV(CS40) | _BV(CS41) | _BV(CS42));
         TCCR4B |= val;
         break;
   #endif

    #if defined(TCCR5A)
    case TIMER5A:
    case TIMER5B:
    case TIMER5C:
         TCCR5B &= ~(_BV(CS50) | _BV(CS51) | _BV(CS52));
         TCCR5B |= val;
         break;
   #endif

  }
}
#endif //FAST_PWM_FAN

bool setTargetedHotend(int code){
  tmp_extruder = active_extruder;
  if(code_seen('T')) {
    tmp_extruder = code_value();
    if(tmp_extruder >= EXTRUDERS) {
      SERIAL_ECHO_START;
      switch(code){
        case 104:
          SERIAL_ECHORPGM(MSG_M104_INVALID_EXTRUDER);
          break;
        case 105:
          SERIAL_ECHO(MSG_M105_INVALID_EXTRUDER);
          break;
        case 109:
          SERIAL_ECHO(MSG_M109_INVALID_EXTRUDER);
          break;
        case 218:
          SERIAL_ECHO(MSG_M218_INVALID_EXTRUDER);
          break;
        case 221:
          SERIAL_ECHO(MSG_M221_INVALID_EXTRUDER);
          break;
      }
      SERIAL_PROTOCOLLN((int)tmp_extruder);
      return true;
    }
  }
  return false;
}

void save_statistics(unsigned long _total_filament_used, unsigned long _total_print_time) //_total_filament_used unit: mm/100; print time in s
{
	if (eeprom_read_byte((uint8_t *)EEPROM_TOTALTIME) == 255 && eeprom_read_byte((uint8_t *)EEPROM_TOTALTIME + 1) == 255 && eeprom_read_byte((uint8_t *)EEPROM_TOTALTIME + 2) == 255 && eeprom_read_byte((uint8_t *)EEPROM_TOTALTIME + 3) == 255)
	{
		eeprom_update_dword((uint32_t *)EEPROM_TOTALTIME, 0);
		eeprom_update_dword((uint32_t *)EEPROM_FILAMENTUSED, 0);
	}

	unsigned long _previous_filament = eeprom_read_dword((uint32_t *)EEPROM_FILAMENTUSED); //_previous_filament unit: cm
	unsigned long _previous_time = eeprom_read_dword((uint32_t *)EEPROM_TOTALTIME); //_previous_time unit: min

	eeprom_update_dword((uint32_t *)EEPROM_TOTALTIME, _previous_time + (_total_print_time/60)); //EEPROM_TOTALTIME unit: min
	eeprom_update_dword((uint32_t *)EEPROM_FILAMENTUSED, _previous_filament + (_total_filament_used / 1000));

	total_filament_used = 0;

}

float calculate_volumetric_multiplier(float diameter) {
	float area = .0;
	float radius = .0;

	radius = diameter * .5;
	if (! volumetric_enabled || radius == 0) {
		area = 1;
	}
	else {
		area = M_PI * pow(radius, 2);
	}

	return 1.0 / area;
}

void calculate_volumetric_multipliers() {
	volumetric_multiplier[0] = calculate_volumetric_multiplier(filament_size[0]);
#if EXTRUDERS > 1
	volumetric_multiplier[1] = calculate_volumetric_multiplier(filament_size[1]);
#if EXTRUDERS > 2
	volumetric_multiplier[2] = calculate_volumetric_multiplier(filament_size[2]);
#endif
#endif
}

void delay_keep_alive(unsigned int ms)
{
	for (;;) {
        manage_heater();
        // Manage inactivity, but don't disable steppers on timeout.
        manage_inactivity(true);
        lcd_update();
        if (ms == 0)
            break;
        else if (ms >= 50) {
            delay(50);
            ms -= 50;
        } else {
            delay(ms);
            ms = 0;
        }
    }
}

void wait_for_heater(long codenum) {

#ifdef TEMP_RESIDENCY_TIME
	long residencyStart;
	residencyStart = -1;
	/* continue to loop until we have reached the target temp
	_and_ until TEMP_RESIDENCY_TIME hasn't passed since we reached it */
	while ((!cancel_heatup) && ((residencyStart == -1) ||
		(residencyStart >= 0 && (((unsigned int)(millis() - residencyStart)) < (TEMP_RESIDENCY_TIME * 1000UL))))) {
#else
	while (target_direction ? (isHeatingHotend(tmp_extruder)) : (isCoolingHotend(tmp_extruder) && (CooldownNoWait == false))) {
#endif //TEMP_RESIDENCY_TIME
		if ((millis() - codenum) > 1000UL)
		{ //Print Temp Reading and remaining time every 1 second while heating up/cooling down
			if (!farm_mode) {
				SERIAL_PROTOCOLPGM("T:");
				SERIAL_PROTOCOL_F(degHotend(tmp_extruder), 1);
				SERIAL_PROTOCOLPGM(" E:");
				SERIAL_PROTOCOL((int)tmp_extruder);

#ifdef TEMP_RESIDENCY_TIME
				SERIAL_PROTOCOLPGM(" W:");
				if (residencyStart > -1)
				{
					codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
					SERIAL_PROTOCOLLN(codenum);
				}
				else
				{
					SERIAL_PROTOCOLLN("?");
				}
			}
#else
				SERIAL_PROTOCOLLN("");
#endif
				codenum = millis();
		}
			manage_heater();
			manage_inactivity();
			lcd_update();
#ifdef TEMP_RESIDENCY_TIME
			/* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
			or when current temp falls outside the hysteresis after target temp was reached */
			if ((residencyStart == -1 && target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder) - TEMP_WINDOW))) ||
				(residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder) + TEMP_WINDOW))) ||
				(residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS))
			{
				residencyStart = millis();
			}
#endif //TEMP_RESIDENCY_TIME
	}
}

void check_babystep() {
	int babystep_z;
	EEPROM_read_B(EEPROM_BABYSTEP_Z, &babystep_z);
	if ((babystep_z < Z_BABYSTEP_MIN) || (babystep_z > Z_BABYSTEP_MAX)) {
		babystep_z = 0; //if babystep value is out of min max range, set it to 0
		SERIAL_ECHOLNPGM("Z live adjust out of range. Setting to 0");
		EEPROM_save_B(EEPROM_BABYSTEP_Z, &babystep_z);
		lcd_show_fullscreen_message_and_wait_P(PSTR("Z live adjust out of range. Setting to 0. Click to continue."));
		lcd_update_enable(true);		
	}	
}
#ifdef DIS
void d_setup()
{	
	pinMode(D_DATACLOCK, INPUT_PULLUP);
	pinMode(D_DATA, INPUT_PULLUP);
	pinMode(D_REQUIRE, OUTPUT);
	digitalWrite(D_REQUIRE, HIGH);
}


float d_ReadData()
{
	int digit[13];
	String mergeOutput;
	float output;

	digitalWrite(D_REQUIRE, HIGH);
	for (int i = 0; i<13; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			while (digitalRead(D_DATACLOCK) == LOW) {}
			while (digitalRead(D_DATACLOCK) == HIGH) {}
			bitWrite(digit[i], j, digitalRead(D_DATA));
		}
	}

	digitalWrite(D_REQUIRE, LOW);
	mergeOutput = "";
	output = 0;
	for (int r = 5; r <= 10; r++) //Merge digits
	{
		mergeOutput += digit[r];
	}
	output = mergeOutput.toFloat();

	if (digit[4] == 8) //Handle sign
	{
		output *= -1;
	}

	for (int i = digit[11]; i > 0; i--) //Handle floating point
	{
		output /= 10;
	}

	return output;

}

void bed_analysis(float x_dimension, float y_dimension, int x_points_num, int y_points_num, float shift_x, float shift_y) {
	int t1 = 0;
	int t_delay = 0;
	int digit[13];
	int m;
	char str[3];
	//String mergeOutput;
	char mergeOutput[15];
	float output;

	int mesh_point = 0; //index number of calibration point
	float bed_zero_ref_x = (-22.f + X_PROBE_OFFSET_FROM_EXTRUDER); //shift between zero point on bed and target and between probe and nozzle
	float bed_zero_ref_y = (-0.6f + Y_PROBE_OFFSET_FROM_EXTRUDER);

	float mesh_home_z_search = 4;
	float row[x_points_num];
	int ix = 0;
	int iy = 0;

	char* filename_wldsd = "wldsd.txt";
	char data_wldsd[70];
	char numb_wldsd[10];

	d_setup();

	if (!(axis_known_position[X_AXIS] && axis_known_position[Y_AXIS] && axis_known_position[Z_AXIS])) {
		// We don't know where we are! HOME!
		// Push the commands to the front of the message queue in the reverse order!
		// There shall be always enough space reserved for these commands.
		repeatcommand_front(); // repeat G80 with all its parameters
		
		enquecommand_front_P((PSTR("G28 W0")));
		enquecommand_front_P((PSTR("G1 Z5")));
		return;
	}
	bool custom_message_old = custom_message;
	unsigned int custom_message_type_old = custom_message_type;
	unsigned int custom_message_state_old = custom_message_state;
	custom_message = true;
	custom_message_type = 1;
	custom_message_state = (x_points_num * y_points_num) + 10;
	lcd_update(1);

	mbl.reset();
	babystep_undo();

	card.openFile(filename_wldsd, false);

	current_position[Z_AXIS] = mesh_home_z_search;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], homing_feedrate[Z_AXIS] / 60, active_extruder);

	int XY_AXIS_FEEDRATE = homing_feedrate[X_AXIS] / 20;
	int Z_LIFT_FEEDRATE = homing_feedrate[Z_AXIS] / 40;

	setup_for_endstop_move(false);

	SERIAL_PROTOCOLPGM("Num X,Y: ");
	SERIAL_PROTOCOL(x_points_num);
	SERIAL_PROTOCOLPGM(",");
	SERIAL_PROTOCOL(y_points_num);
	SERIAL_PROTOCOLPGM("\nZ search height: ");
	SERIAL_PROTOCOL(mesh_home_z_search);
	SERIAL_PROTOCOLPGM("\nDimension X,Y: ");
	SERIAL_PROTOCOL(x_dimension);
	SERIAL_PROTOCOLPGM(",");
	SERIAL_PROTOCOL(y_dimension);
	SERIAL_PROTOCOLLNPGM("\nMeasured points:");

	while (mesh_point != x_points_num * y_points_num) {
		ix = mesh_point % x_points_num; // from 0 to MESH_NUM_X_POINTS - 1
		iy = mesh_point / x_points_num;
		if (iy & 1) ix = (x_points_num - 1) - ix; // Zig zag
		float z0 = 0.f;
		current_position[Z_AXIS] = mesh_home_z_search;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], Z_LIFT_FEEDRATE, active_extruder);
		st_synchronize();


		current_position[X_AXIS] = 13.f + ix * (x_dimension / (x_points_num - 1)) - bed_zero_ref_x + shift_x;
		current_position[Y_AXIS] = 6.4f + iy * (y_dimension / (y_points_num - 1)) - bed_zero_ref_y + shift_y;

		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], XY_AXIS_FEEDRATE, active_extruder);
		st_synchronize();

		if (!find_bed_induction_sensor_point_z(-10.f)) { //if we have data from z calibration max allowed difference is 1mm for each point, if we dont have data max difference is 10mm from initial point  
			break;
			card.closefile();
		}


		//memset(numb_wldsd, 0, sizeof(numb_wldsd));
		//dtostrf(d_ReadData(), 8, 5, numb_wldsd);
		//strcat(data_wldsd, numb_wldsd);


		
		//MYSERIAL.println(data_wldsd);
		//delay(1000);
		//delay(3000);
		//t1 = millis();
		
		//while (digitalRead(D_DATACLOCK) == LOW) {}
		//while (digitalRead(D_DATACLOCK) == HIGH) {}
		memset(digit, 0, sizeof(digit));
		//cli();
		digitalWrite(D_REQUIRE, LOW);	
		
		for (int i = 0; i<13; i++)
		{
			//t1 = millis();
			for (int j = 0; j < 4; j++)
			{
				while (digitalRead(D_DATACLOCK) == LOW) {}				
				while (digitalRead(D_DATACLOCK) == HIGH) {}
				bitWrite(digit[i], j, digitalRead(D_DATA));
			}
			//t_delay = (millis() - t1);
			//SERIAL_PROTOCOLPGM(" ");
			//SERIAL_PROTOCOL_F(t_delay, 5);
			//SERIAL_PROTOCOLPGM(" ");
		}
		//sei();
		digitalWrite(D_REQUIRE, HIGH);
		mergeOutput[0] = '\0';
		output = 0;
		for (int r = 5; r <= 10; r++) //Merge digits
		{			
			sprintf(str, "%d", digit[r]);
			strcat(mergeOutput, str);
		}
		
		output = atof(mergeOutput);

		if (digit[4] == 8) //Handle sign
		{
			output *= -1;
		}

		for (int i = digit[11]; i > 0; i--) //Handle floating point
		{
			output *= 0.1;
		}
		

		//output = d_ReadData();

		//row[ix] = current_position[Z_AXIS];

		memset(data_wldsd, 0, sizeof(data_wldsd));

		for (int i = 0; i <3; i++) {
			memset(numb_wldsd, 0, sizeof(numb_wldsd));
			dtostrf(current_position[i], 8, 5, numb_wldsd);
			strcat(data_wldsd, numb_wldsd);
			strcat(data_wldsd, ";");

		}
		memset(numb_wldsd, 0, sizeof(numb_wldsd));
		dtostrf(output, 8, 5, numb_wldsd);
		strcat(data_wldsd, numb_wldsd);
		//strcat(data_wldsd, ";");
		card.write_command(data_wldsd);

		
		//row[ix] = d_ReadData();
		
		row[ix] = output; // current_position[Z_AXIS];

		if (iy % 2 == 1 ? ix == 0 : ix == x_points_num - 1) {
			for (int i = 0; i < x_points_num; i++) {
				SERIAL_PROTOCOLPGM(" ");
				SERIAL_PROTOCOL_F(row[i], 5);


			}
			SERIAL_PROTOCOLPGM("\n");
		}
		custom_message_state--;
		mesh_point++;
		lcd_update(1);

	}
	card.closefile();

}
#endif

void temp_compensation_start() {
	
	custom_message = true;
	custom_message_type = 5;
	custom_message_state = PINDA_HEAT_T + 1;
	lcd_update(2);
	if (degHotend(active_extruder) > EXTRUDE_MINTEMP) {
		current_position[E_AXIS] -= DEFAULT_RETRACTION;
	}
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 400, active_extruder);
	
	current_position[X_AXIS] = PINDA_PREHEAT_X;
	current_position[Y_AXIS] = PINDA_PREHEAT_Y;
	current_position[Z_AXIS] = PINDA_PREHEAT_Z;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 3000 / 60, active_extruder);
	st_synchronize();
	while (fabs(degBed() - target_temperature_bed) > 1) delay_keep_alive(1000);

	for (int i = 0; i < PINDA_HEAT_T; i++) {
		delay_keep_alive(1000);
		custom_message_state = PINDA_HEAT_T - i;
		if (custom_message_state == 99 || custom_message_state == 9) lcd_update(2); //force whole display redraw if number of digits changed
		else lcd_update(1);
	}	
	custom_message_type = 0;
	custom_message_state = 0;
	custom_message = false;
}

void temp_compensation_apply() {
	int i_add;
	int z_shift = 0;
	float z_shift_mm;

	if (calibration_status() == CALIBRATION_STATUS_CALIBRATED) {
		if (target_temperature_bed % 10 == 0 && target_temperature_bed >= 60 && target_temperature_bed <= 100) {
			i_add = (target_temperature_bed - 60) / 10;
			EEPROM_read_B(EEPROM_PROBE_TEMP_SHIFT + i_add * 2, &z_shift);
			z_shift_mm = z_shift / axis_steps_per_unit[Z_AXIS];
		}else {
			//interpolation
			z_shift_mm = temp_comp_interpolation(target_temperature_bed) / axis_steps_per_unit[Z_AXIS];
		}
		SERIAL_PROTOCOLPGM("\n");
		SERIAL_PROTOCOLPGM("Z shift applied:");
		MYSERIAL.print(z_shift_mm);
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS] - z_shift_mm, current_position[E_AXIS], homing_feedrate[Z_AXIS] / 40, active_extruder);
		st_synchronize();
		plan_set_z_position(current_position[Z_AXIS]);
	}
	else {		
		//we have no temp compensation data
	}
}

float temp_comp_interpolation(float inp_temperature) {

	//cubic spline interpolation

	int n, i, j;
	float h[10], a, b, c, d, sum, s[10] = { 0 }, x[10], F[10], f[10], m[10][10] = { 0 }, temp;
	int shift[10];
	int temp_C[10];

	n = 6; //number of measured points

	shift[0] = 0;
	for (i = 0; i < n; i++) {
		if (i>0) EEPROM_read_B(EEPROM_PROBE_TEMP_SHIFT + (i-1) * 2, &shift[i]); //read shift in steps from EEPROM
		temp_C[i] = 50 + i * 10; //temperature in C
		
		x[i] = (float)temp_C[i];
		f[i] = (float)shift[i];
	}
	if (inp_temperature < x[0]) return 0;


	for (i = n - 1; i>0; i--) {
		F[i] = (f[i] - f[i - 1]) / (x[i] - x[i - 1]);
		h[i - 1] = x[i] - x[i - 1];
	}
	//*********** formation of h, s , f matrix **************
	for (i = 1; i<n - 1; i++) {
		m[i][i] = 2 * (h[i - 1] + h[i]);
		if (i != 1) {
			m[i][i - 1] = h[i - 1];
			m[i - 1][i] = h[i - 1];
		}
		m[i][n - 1] = 6 * (F[i + 1] - F[i]);
	}
	//*********** forward elimination **************
	for (i = 1; i<n - 2; i++) {
		temp = (m[i + 1][i] / m[i][i]);
		for (j = 1; j <= n - 1; j++)
			m[i + 1][j] -= temp*m[i][j];
	}
	//*********** backward substitution *********
	for (i = n - 2; i>0; i--) {
		sum = 0;
		for (j = i; j <= n - 2; j++)
			sum += m[i][j] * s[j];
		s[i] = (m[i][n - 1] - sum) / m[i][i];
	}

		for (i = 0; i<n - 1; i++)
			if ((x[i] <= inp_temperature && inp_temperature <= x[i + 1]) || (i == n-2 && inp_temperature > x[i + 1])) {
				a = (s[i + 1] - s[i]) / (6 * h[i]);
				b = s[i] / 2;
				c = (f[i + 1] - f[i]) / h[i] - (2 * h[i] * s[i] + s[i + 1] * h[i]) / 6;
				d = f[i];
				sum = a*pow((inp_temperature - x[i]), 3) + b*pow((inp_temperature - x[i]), 2) + c*(inp_temperature - x[i]) + d;
			}

		return sum;

}

void long_pause() //long pause print
{
	st_synchronize();
	
	//save currently set parameters to global variables
	saved_feedmultiply = feedmultiply; 
	HotendTempBckp = degTargetHotend(active_extruder);
	fanSpeedBckp = fanSpeed;
	start_pause_print = millis();
		

	//save position
	pause_lastpos[X_AXIS] = current_position[X_AXIS];
	pause_lastpos[Y_AXIS] = current_position[Y_AXIS];
	pause_lastpos[Z_AXIS] = current_position[Z_AXIS];
	pause_lastpos[E_AXIS] = current_position[E_AXIS];

	//retract
	current_position[E_AXIS] -= DEFAULT_RETRACTION;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 400, active_extruder);

	//lift z
	current_position[Z_AXIS] += Z_PAUSE_LIFT;
	if (current_position[Z_AXIS] > Z_MAX_POS) current_position[Z_AXIS] = Z_MAX_POS;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 15, active_extruder);

	//set nozzle target temperature to 0
	setTargetHotend(0, 0);
	setTargetHotend(0, 1);
	setTargetHotend(0, 2);

	//Move XY to side
	current_position[X_AXIS] = X_PAUSE_POS;
	current_position[Y_AXIS] = Y_PAUSE_POS;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 50, active_extruder);

	// Turn off the print fan
	fanSpeed = 0;

	st_synchronize();
}

void serialecho_temperatures() {
	float tt = degHotend(active_extruder);
	SERIAL_PROTOCOLPGM("T:");
	SERIAL_PROTOCOL(tt);
	SERIAL_PROTOCOLPGM(" E:");
	SERIAL_PROTOCOL((int)active_extruder);
	SERIAL_PROTOCOLPGM(" B:");
	SERIAL_PROTOCOL_F(degBed(), 1);
	SERIAL_PROTOCOLLN("");
}
