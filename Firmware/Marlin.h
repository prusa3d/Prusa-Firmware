// Tonokip RepRap firmware rewrite based off of Hydra-mmm firmware.
// License: GPL

#ifndef MARLIN_H
#define MARLIN_H

#define  FORCE_INLINE __attribute__((always_inline)) inline

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>


#include "fastio.h"
#include "Configuration.h"
#include "pins.h"

#ifndef AT90USB
#define  HardwareSerial_h // trick to disable the standard HWserial
#endif

#if (ARDUINO >= 100)
# include "Arduino.h"
#else
# include "WProgram.h"
#endif

// Arduino < 1.0.0 does not define this, so we need to do it ourselves
#ifndef analogInputToDigitalPin
# define analogInputToDigitalPin(p) ((p) + A0)
#endif

#ifdef AT90USB
#include "HardwareSerial.h"
#endif

#include "MarlinSerial.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#include "WString.h"

#ifdef AT90USB
   #ifdef BTENABLED
         #define MYSERIAL bt
   #else
         #define MYSERIAL Serial
   #endif // BTENABLED
#else
  #define MYSERIAL MSerial
#endif

#define SERIAL_PROTOCOL(x) (MYSERIAL.print(x))
#define SERIAL_PROTOCOL_F(x,y) (MYSERIAL.print(x,y))
#define SERIAL_PROTOCOLPGM(x) (serialprintPGM(PSTR(x)))
#define SERIAL_PROTOCOLRPGM(x) (serialprintPGM((x)))
#define SERIAL_PROTOCOLLN(x) (MYSERIAL.print(x),MYSERIAL.write('\n'))
#define SERIAL_PROTOCOLLNPGM(x) (serialprintPGM(PSTR(x)),MYSERIAL.write('\n'))
#define SERIAL_PROTOCOLLNRPGM(x) (serialprintPGM((x)),MYSERIAL.write('\n'))


extern const char errormagic[] PROGMEM;
extern const char echomagic[] PROGMEM;

#define SERIAL_ERROR_START (serialprintPGM(errormagic))
#define SERIAL_ERROR(x) SERIAL_PROTOCOL(x)
#define SERIAL_ERRORPGM(x) SERIAL_PROTOCOLPGM(x)
#define SERIAL_ERRORRPGM(x) SERIAL_PROTOCOLRPGM(x)
#define SERIAL_ERRORLN(x) SERIAL_PROTOCOLLN(x)
#define SERIAL_ERRORLNPGM(x) SERIAL_PROTOCOLLNPGM(x)
#define SERIAL_ERRORLNRPGM(x) SERIAL_PROTOCOLLNRPGM(x)

#define SERIAL_ECHO_START (serialprintPGM(echomagic))
#define SERIAL_ECHO(x) SERIAL_PROTOCOL(x)
#define SERIAL_ECHOPGM(x) SERIAL_PROTOCOLPGM(x)
#define SERIAL_ECHORPGM(x) SERIAL_PROTOCOLRPGM(x)
#define SERIAL_ECHOLN(x) SERIAL_PROTOCOLLN(x)
#define SERIAL_ECHOLNPGM(x) SERIAL_PROTOCOLLNPGM(x)
#define SERIAL_ECHOLNRPGM(x) SERIAL_PROTOCOLLNRPGM(x)

#define SERIAL_ECHOPAIR(name,value) (serial_echopair_P(PSTR(name),(value)))

void serial_echopair_P(const char *s_P, float v);
void serial_echopair_P(const char *s_P, double v);
void serial_echopair_P(const char *s_P, unsigned long v);

extern uint8_t selectedSerialPort;

//Things to write to serial from Program memory. Saves 400 to 2k of RAM.
FORCE_INLINE void serialprintPGM(const char *str)
{
  char ch=pgm_read_byte(str);
  while(ch)
  {
    MYSERIAL.write(ch);
    ch=pgm_read_byte(++str);
  }
}

#define NOMORE(v,n) do{ if (v > n) v = n; }while(0)

bool is_buffer_empty();
void get_command();
void process_commands();
void ramming();

void manage_inactivity(bool ignore_stepper_queue=false);

#if defined(X_ENABLE_PIN) && X_ENABLE_PIN > -1
  #define  enable_x() WRITE(X_ENABLE_PIN, X_ENABLE_ON)
  #define disable_x() { WRITE(X_ENABLE_PIN,!X_ENABLE_ON); axis_known_position[X_AXIS] = false; }
#else
  #define enable_x() ;
  #define disable_x() ;
#endif

#if defined(Y_ENABLE_PIN) && Y_ENABLE_PIN > -1
  #ifdef Y_DUAL_STEPPER_DRIVERS
    #define  enable_y() { WRITE(Y_ENABLE_PIN, Y_ENABLE_ON); WRITE(Y2_ENABLE_PIN,  Y_ENABLE_ON); }
    #define disable_y() { WRITE(Y_ENABLE_PIN,!Y_ENABLE_ON); WRITE(Y2_ENABLE_PIN, !Y_ENABLE_ON); axis_known_position[Y_AXIS] = false; }
  #else
    #define  enable_y() WRITE(Y_ENABLE_PIN, Y_ENABLE_ON)
    #define disable_y() { WRITE(Y_ENABLE_PIN,!Y_ENABLE_ON); axis_known_position[Y_AXIS] = false; }
  #endif
#else
  #define enable_y() ;
  #define disable_y() ;
#endif

#if defined(Z_ENABLE_PIN) && Z_ENABLE_PIN > -1 
	#if defined(Z_AXIS_ALWAYS_ON)
		  #ifdef Z_DUAL_STEPPER_DRIVERS
			#define  enable_z() { WRITE(Z_ENABLE_PIN, Z_ENABLE_ON); WRITE(Z2_ENABLE_PIN, Z_ENABLE_ON); }
			#define disable_z() { WRITE(Z_ENABLE_PIN,!Z_ENABLE_ON); WRITE(Z2_ENABLE_PIN,!Z_ENABLE_ON); axis_known_position[Z_AXIS] = false; }
		  #else
			#define  enable_z() WRITE(Z_ENABLE_PIN, Z_ENABLE_ON)
			#define  disable_z() ;
		  #endif
	#else
		#ifdef Z_DUAL_STEPPER_DRIVERS
			#define  enable_z() { WRITE(Z_ENABLE_PIN, Z_ENABLE_ON); WRITE(Z2_ENABLE_PIN, Z_ENABLE_ON); }
			#define disable_z() { WRITE(Z_ENABLE_PIN,!Z_ENABLE_ON); WRITE(Z2_ENABLE_PIN,!Z_ENABLE_ON); axis_known_position[Z_AXIS] = false; }
		#else
			#define  enable_z() WRITE(Z_ENABLE_PIN, Z_ENABLE_ON)
			#define disable_z() { WRITE(Z_ENABLE_PIN,!Z_ENABLE_ON); axis_known_position[Z_AXIS] = false; }
		#endif
	#endif
#else
  #define enable_z() ;
  #define disable_z() ;
#endif




//#if defined(Z_ENABLE_PIN) && Z_ENABLE_PIN > -1
//#ifdef Z_DUAL_STEPPER_DRIVERS
//#define  enable_z() { WRITE(Z_ENABLE_PIN, Z_ENABLE_ON); WRITE(Z2_ENABLE_PIN, Z_ENABLE_ON); }
//#define disable_z() { WRITE(Z_ENABLE_PIN,!Z_ENABLE_ON); WRITE(Z2_ENABLE_PIN,!Z_ENABLE_ON); axis_known_position[Z_AXIS] = false; }
//#else
//#define  enable_z() WRITE(Z_ENABLE_PIN, Z_ENABLE_ON)
//#define disable_z() { WRITE(Z_ENABLE_PIN,!Z_ENABLE_ON); axis_known_position[Z_AXIS] = false; }
//#endif
//#else
//#define enable_z() ;
//#define disable_z() ;
//#endif


#if defined(E0_ENABLE_PIN) && (E0_ENABLE_PIN > -1)
  #define enable_e0() WRITE(E0_ENABLE_PIN, E_ENABLE_ON)
  #define disable_e0() WRITE(E0_ENABLE_PIN,!E_ENABLE_ON)
#else
  #define enable_e0()  /* nothing */
  #define disable_e0() /* nothing */
#endif

#if (EXTRUDERS > 1) && defined(E1_ENABLE_PIN) && (E1_ENABLE_PIN > -1)
  #define enable_e1() WRITE(E1_ENABLE_PIN, E_ENABLE_ON)
  #define disable_e1() WRITE(E1_ENABLE_PIN,!E_ENABLE_ON)
#else
  #define enable_e1()  /* nothing */
  #define disable_e1() /* nothing */
#endif

#if (EXTRUDERS > 2) && defined(E2_ENABLE_PIN) && (E2_ENABLE_PIN > -1)
  #define enable_e2() WRITE(E2_ENABLE_PIN, E_ENABLE_ON)
  #define disable_e2() WRITE(E2_ENABLE_PIN,!E_ENABLE_ON)
#else
  #define enable_e2()  /* nothing */
  #define disable_e2() /* nothing */
#endif


enum AxisEnum {X_AXIS=0, Y_AXIS=1, Z_AXIS=2, E_AXIS=3, X_HEAD=4, Y_HEAD=5};


void FlushSerialRequestResend();
void ClearToSend();

void get_coordinates();
void prepare_move();
void kill(const char *full_screen_message = NULL);
void Stop();

bool IsStopped();

//put an ASCII command at the end of the current buffer.
void enquecommand(const char *cmd, bool from_progmem = false);
//put an ASCII command at the end of the current buffer, read from flash
#define enquecommand_P(cmd) enquecommand(cmd, true)
void enquecommand_front(const char *cmd, bool from_progmem = false);
bool cmd_buffer_empty();
//put an ASCII command at the end of the current buffer, read from flash
#define enquecommand_P(cmd) enquecommand(cmd, true)
#define enquecommand_front_P(cmd) enquecommand_front(cmd, true)
void repeatcommand_front();
// Remove all lines from the command queue.
void cmdqueue_reset();

void prepare_arc_move(char isclockwise);
void clamp_to_software_endstops(float target[3]);

void refresh_cmd_timeout(void);

#ifdef FAST_PWM_FAN
void setPwmFrequency(uint8_t pin, int val);
#endif

#ifndef CRITICAL_SECTION_START
  #define CRITICAL_SECTION_START  unsigned char _sreg = SREG; cli();
  #define CRITICAL_SECTION_END    SREG = _sreg;
#endif //CRITICAL_SECTION_START

extern float homing_feedrate[];
extern bool axis_relative_modes[];
extern int feedmultiply;
extern int extrudemultiply; // Sets extrude multiply factor (in percent) for all extruders
extern bool volumetric_enabled;
extern int extruder_multiply[EXTRUDERS]; // sets extrude multiply factor (in percent) for each extruder individually
extern float filament_size[EXTRUDERS]; // cross-sectional area of filament (in millimeters), typically around 1.75 or 2.85, 0 disables the volumetric calculations for the extruder.
extern float volumetric_multiplier[EXTRUDERS]; // reciprocal of cross-sectional area of filament (in square millimeters), stored this way to reduce computational burden in planner
extern float current_position[NUM_AXIS] ;
extern float destination[NUM_AXIS] ;
extern float add_homing[3];
extern float min_pos[3];
extern float max_pos[3];
extern bool axis_known_position[3];
extern float zprobe_zoffset;
extern int fanSpeed;
extern void homeaxis(int axis);


#ifdef FAN_SOFT_PWM
extern unsigned char fanSpeedSoftPwm;
#endif

#ifdef FILAMENT_SENSOR
  extern float filament_width_nominal;  //holds the theoretical filament diameter ie., 3.00 or 1.75
  extern bool filament_sensor;  //indicates that filament sensor readings should control extrusion
  extern float filament_width_meas; //holds the filament diameter as accurately measured
  extern signed char measurement_delay[];  //ring buffer to delay measurement
  extern int delay_index1, delay_index2;  //index into ring buffer
  extern float delay_dist; //delay distance counter
  extern int meas_delay_cm; //delay distance
#endif

#ifdef FWRETRACT
extern bool autoretract_enabled;
extern bool retracted[EXTRUDERS];
extern float retract_length, retract_length_swap, retract_feedrate, retract_zlift;
extern float retract_recover_length, retract_recover_length_swap, retract_recover_feedrate;
#endif

#ifdef HOST_KEEPALIVE_FEATURE
extern uint8_t host_keepalive_interval;
#endif

extern unsigned long starttime;
extern unsigned long stoptime;
extern int bowden_length[4];
extern bool is_usb_printing;
extern bool homing_flag;
extern bool temp_cal_active;
extern bool loading_flag;
extern unsigned int usb_printing_counter;

extern unsigned long kicktime;

extern unsigned long total_filament_used;
void save_statistics(unsigned long _total_filament_used, unsigned long _total_print_time);
extern unsigned int heating_status;
extern unsigned int status_number;
extern unsigned int heating_status_counter;
extern bool custom_message;
extern unsigned int custom_message_type;
extern unsigned int custom_message_state;
extern char snmm_filaments_used;
extern unsigned long PingTime;
extern unsigned long NcTime;
extern bool no_response;
extern uint8_t important_status;
extern uint8_t saved_filament_type;


// Handling multiple extruders pins
extern uint8_t active_extruder;

#ifdef DIGIPOT_I2C
extern void digipot_i2c_set_current( int channel, float current );
extern void digipot_i2c_init();
#endif

//Long pause
extern int saved_feedmultiply;
extern float HotendTempBckp;
extern int fanSpeedBckp;
extern float pause_lastpos[4];
extern unsigned long pause_time;
extern unsigned long start_pause_print;

extern bool mesh_bed_leveling_flag;
extern bool mesh_bed_run_from_menu;

extern float distance_from_min[3];

extern bool sortAlpha;

extern void calculate_volumetric_multipliers();

// Similar to the default Arduino delay function, 
// but it keeps the background tasks running.
extern void delay_keep_alive(unsigned int ms);

extern void check_babystep();

extern void long_pause();

#ifdef DIS

void d_setup();
float d_ReadData();
void bed_analysis(float x_dimension, float y_dimension, int x_points_num, int y_points_num, float shift_x, float shift_y);

#endif

float temp_comp_interpolation(float temperature);
void temp_compensation_apply();
void temp_compensation_start();
void wait_for_heater(long codenum);
void serialecho_temperatures();
void proc_commands();
bool check_commands();

#ifdef HOST_KEEPALIVE_FEATURE

// States for managing Marlin and host communication
// Marlin sends messages if blocked or busy
enum MarlinBusyState {
	NOT_BUSY,           // Not in a handler
	IN_HANDLER,         // Processing a GCode
	IN_PROCESS,         // Known to be blocking command input (as in G29)
	PAUSED_FOR_USER,    // Blocking pending any input
	PAUSED_FOR_INPUT    // Blocking pending text input (concept)
};

#define KEEPALIVE_STATE(n) do { busy_state = n;} while (0)
extern void host_keepalive();
extern MarlinBusyState busy_state;

#endif //HOST_KEEPALIVE_FEATURE

bool gcode_M45(bool onlyZ);
void gcode_M701();


#endif //ifndef marlin.h

