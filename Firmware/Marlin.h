// Tonokip RepRap firmware rewrite based off of Hydra-mmm firmware.
// License: GPL

#ifndef MARLIN_H
#define MARLIN_H

#include "macros.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "system_timer.h"
#include "fastio.h"
#include "Configuration.h"
#include "pins.h"
#include "Timer.h"
#include "mmu2.h"
extern uint8_t mbl_z_probe_nr;

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

//#include "WString.h"

#ifdef AT90USB
   #ifdef BTENABLED
         #define MYSERIAL bt
   #else
         #define MYSERIAL Serial
   #endif // BTENABLED
#else
  #define MYSERIAL MSerial
#endif

#include "lcd.h"

#ifdef __cplusplus
extern "C" {
#endif
extern FILE _uartout;
#ifdef __cplusplus
}
#endif

#define uartout (&_uartout)

#define SERIAL_PROTOCOL(x) (MYSERIAL.print(x))
#define SERIAL_PROTOCOL_F(x,y) (MYSERIAL.print(x,y))
#define SERIAL_PROTOCOLPGM(x) (serialprintPGM(PSTR(x)))
#define SERIAL_PROTOCOLRPGM(x) (serialprintPGM((x)))
#define SERIAL_PROTOCOLLN(x) (MYSERIAL.println(x))
#define SERIAL_PROTOCOLLNPGM(x) (serialprintlnPGM(PSTR(x)))
#define SERIAL_PROTOCOLLNRPGM(x) (serialprintlnPGM((x)))


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


//Things to write to serial from Program memory. Saves 400 to 2k of RAM.
// Making this FORCE_INLINE is not a good idea when running out of FLASH
// I'd rather skip a few CPU ticks than 5.5KB (!!) of FLASH
void serialprintPGM(const char *str);

//The "ln" variant of the function above.
void serialprintlnPGM(const char *str);

bool is_buffer_empty();
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
			#define  poweron_z() { WRITE(Z_ENABLE_PIN, Z_ENABLE_ON); WRITE(Z2_ENABLE_PIN, Z_ENABLE_ON); }
			#define poweroff_z() { WRITE(Z_ENABLE_PIN,!Z_ENABLE_ON); WRITE(Z2_ENABLE_PIN,!Z_ENABLE_ON); axis_known_position[Z_AXIS] = false; }
		  #else
			#define  poweron_z() WRITE(Z_ENABLE_PIN, Z_ENABLE_ON)
			#define poweroff_z() {}
		  #endif
	#else
		#ifdef Z_DUAL_STEPPER_DRIVERS
			#define  poweron_z() { WRITE(Z_ENABLE_PIN, Z_ENABLE_ON); WRITE(Z2_ENABLE_PIN, Z_ENABLE_ON); }
			#define poweroff_z() { WRITE(Z_ENABLE_PIN,!Z_ENABLE_ON); WRITE(Z2_ENABLE_PIN,!Z_ENABLE_ON); axis_known_position[Z_AXIS] = false; }
		#else
			#define  poweron_z() WRITE(Z_ENABLE_PIN, Z_ENABLE_ON)
			#define poweroff_z() { WRITE(Z_ENABLE_PIN,!Z_ENABLE_ON); axis_known_position[Z_AXIS] = false; }
		#endif
	#endif
#else
    #define  poweron_z() {}
    #define poweroff_z() {}
#endif

#ifndef PSU_Delta
    #define  enable_z()  poweron_z()
    #define disable_z() poweroff_z()
#else
    void init_force_z();
    void check_force_z();
    void enable_force_z();
    void disable_force_z();
    #define  enable_z()  enable_force_z()
    #define disable_z() disable_force_z()
#endif // PSU_Delta


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
#define X_AXIS_MASK  1
#define Y_AXIS_MASK  2
#define Z_AXIS_MASK  4
#define E_AXIS_MASK  8
#define X_HEAD_MASK 16
#define Y_HEAD_MASK 32


void FlushSerialRequestResend();
void ClearToSend();
void update_currents();

void kill(const char *full_screen_message = NULL, unsigned char id = 0);
void finishAndDisableSteppers();

void UnconditionalStop();                   // Stop heaters, motion and clear current print status
void ThermalStop(bool allow_pause = false); // Emergency stop used by overtemp functions which allows
                                            // recovery (with pause=true)
bool IsStopped();                           // Returns true if the print has been stopped

//put an ASCII command at the end of the current buffer, read from flash
#define enquecommand_P(cmd) enquecommand(cmd, true)

//put an ASCII command at the begin of the current buffer, read from flash
#define enquecommand_front_P(cmd) enquecommand_front(cmd, true)

void clamp_to_software_endstops(float target[3]);
void refresh_cmd_timeout(void);

#ifdef FAST_PWM_FAN
void setPwmFrequency(uint8_t pin, int val);
#endif

enum class HeatingStatus : uint8_t
{
    NO_HEATING = 0,
    EXTRUDER_HEATING = 1,
    EXTRUDER_HEATING_COMPLETE = 2,
    BED_HEATING = 3,
    BED_HEATING_COMPLETE = 4,
};

extern HeatingStatus heating_status;

extern bool fans_check_enabled;
extern float homing_feedrate[];
extern uint8_t axis_relative_modes;
extern float feedrate;
extern int feedmultiply;
extern int extrudemultiply; // Sets extrude multiply factor (in percent) for all extruders
extern float extruder_multiplier[EXTRUDERS]; // reciprocal of cross-sectional area of filament (in square millimeters), stored this way to reduce computational burden in planner
extern float current_position[NUM_AXIS] ;
extern float destination[NUM_AXIS] ;
extern float min_pos[3];
extern float max_pos[3];
extern bool axis_known_position[3];
extern int fanSpeed;
extern uint8_t newFanSpeed;
extern float default_retraction;

void get_coordinates();
void prepare_move(uint16_t start_segment_idx = 0);
void prepare_arc_move(bool isclockwise, uint16_t start_segment_idx = 0);
uint16_t restore_interrupted_gcode();

#ifdef TMC2130
void homeaxis(uint8_t axis, uint8_t cnt = 1, uint8_t* pstep = 0);
#else
void homeaxis(uint8_t axis, uint8_t cnt = 1);
#endif //TMC2130

#ifdef FWRETRACT
extern bool retracted[EXTRUDERS];
extern float retract_length_swap;
extern float retract_recover_length_swap;
#endif

extern uint8_t host_keepalive_interval;

extern unsigned long starttime;
extern unsigned long stoptime;
extern ShortTimer usb_timer;
extern bool processing_tcode;
extern bool homing_flag;
extern bool loading_flag;
extern unsigned long total_filament_used;
void save_statistics(unsigned long _total_filament_used, unsigned long _total_print_time);
extern uint8_t heating_status_counter;

extern bool fan_state[2];
extern int fan_edge_counter[2];
extern int fan_speed[2];

// Active extruder becomes a #define to make the whole firmware compilable.
// We may even remove the references to it wherever possible in the future
#define active_extruder 0

//Long pause
extern unsigned long pause_time;
extern unsigned long start_pause_print;
extern unsigned long t_fan_rising_edge;

extern bool mesh_bed_leveling_flag;

// save/restore printing
extern bool saved_printing;
extern uint8_t saved_printing_type;
#define PRINTING_TYPE_SD 0
#define PRINTING_TYPE_USB 1
#define PRINTING_TYPE_NONE 2

extern float saved_extruder_temperature; //!< Active extruder temperature
extern float saved_bed_temperature; //!< Bed temperature
extern int saved_fan_speed; //!< Print fan speed

//estimated time to end of the print
extern uint8_t print_percent_done_normal;
extern uint8_t print_percent_done_silent;
extern uint16_t print_time_remaining_normal;
extern uint16_t print_time_remaining_silent;
extern uint16_t print_time_to_change_normal;
extern uint16_t print_time_to_change_silent;

#define PRINT_TIME_REMAINING_INIT 0xffff

extern uint16_t mcode_in_progress;
extern uint16_t gcode_in_progress;

extern LongTimer safetyTimer;

#define PRINT_PERCENT_DONE_INIT 0xff

extern bool printer_active();

//! Beware - mcode_in_progress is set as soon as the command gets really processed,
//! which is not the same as posting the M600 command into the command queue
//! There can be a considerable lag between posting M600 and its real processing which might result
//! in posting multiple M600's into the command queue
//! Instead, the fsensor uses another state variable :( , which is set to true, when the M600 command is enqued
//! and is reset to false when the fsensor returns into its filament runout finished handler
//! I'd normally change this macro, but who knows what would happen in the MMU :)
bool check_fsensor();

extern void calculate_extruder_multipliers();

// Similar to the default Arduino delay function, 
// but it keeps the background tasks running.
extern void delay_keep_alive(unsigned int ms);

extern void check_babystep();

extern void long_pause();
extern void crashdet_stop_and_save_print();

#ifdef HEATBED_ANALYSIS
void d_setup();
float d_ReadData();
void bed_analysis(float x_dimension, float y_dimension, int x_points_num, int y_points_num, float shift_x, float shift_y);
void bed_check(float x_dimension, float y_dimension, int x_points_num, int y_points_num, float shift_x, float shift_y);
#endif //HEATBED_ANALYSIS
float temp_comp_interpolation(float temperature);
#if 0
void show_fw_version_warnings();
#endif
uint8_t check_printer_version();

#ifdef PINDA_THERMISTOR
float temp_compensation_pinda_thermistor_offset(float temperature_pinda);
#endif //PINDA_THERMISTOR

void serialecho_temperatures();
bool check_commands();

void uvlo_();
void uvlo_tiny();
void recover_print(uint8_t automatic); 
void setup_uvlo_interrupt();

#if defined(TACH_1) && TACH_1 >-1
void setup_fan_interrupt();
#endif

extern bool recover_machine_state_after_power_panic();
extern void restore_print_from_eeprom(bool mbl_was_active);
extern void position_menu();

extern void print_world_coordinates();
extern void print_physical_coordinates();
extern void print_mesh_bed_leveling_table();

extern void stop_and_save_print_to_ram(float z_move, float e_move);
void restore_extruder_temperature_from_ram();
extern void restore_print_from_ram_and_continue(float e_move);
extern void cancel_saved_printing();


//estimated time to end of the print
extern uint8_t calc_percent_done();



// States for managing Marlin and host communication
// Marlin sends messages if blocked or busy
/*enum MarlinBusyState {
	NOT_BUSY,           // Not in a handler
	IN_HANDLER,         // Processing a GCode
	IN_PROCESS,         // Known to be blocking command input (as in G29)
	PAUSED_FOR_USER,    // Blocking pending any input
	PAUSED_FOR_INPUT    // Blocking pending text input (concept)
};*/

#define NOT_BUSY          1
#define IN_HANDLER        2
#define IN_PROCESS        3
#define PAUSED_FOR_USER   4
#define PAUSED_FOR_INPUT  5

#define KEEPALIVE_STATE(n) do { busy_state = n;} while (0)
extern void host_keepalive();
extern void host_autoreport();
//extern MarlinBusyState busy_state;
extern int8_t busy_state;


#ifdef TMC2130

#define FORCE_HIGH_POWER_START	force_high_power_mode(true)
#define FORCE_HIGH_POWER_END	force_high_power_mode(false)

void force_high_power_mode(bool start_high_power_section);

#endif //TMC2130

// G-codes

bool gcode_M45(bool onlyZ, int8_t verbosity_level);
void gcode_M114();
#if (defined(FANCHECK) && (((defined(TACH_0) && (TACH_0 >-1)) || (defined(TACH_1) && (TACH_1 > -1)))))
void gcode_M123();
#endif //FANCHECK and TACH_0 and TACH_1
void gcode_M701(float fastLoadLength, uint8_t mmuSlotIndex);

#define UVLO !(PINE & (1<<4))


void M600_load_filament();
void M600_load_filament_movements();
void M600_wait_for_user(float HotendTempBckp);
void M600_check_state(float nozzle_temp);
void load_filament_final_feed();
void marlin_wait_for_click();
float raise_z(float delta);
void raise_z_above(float target);

extern "C" void softReset();
void stack_error();

extern uint32_t IP_address;

#endif
