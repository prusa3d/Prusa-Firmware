/* -*- c++ -*- */
/**
 * @file
 */

/**
 * @mainpage Reprap 3D printer firmware based on Sprinter and grbl.
 *
 * @section intro_sec Introduction
 *
 * This firmware is a mashup between Sprinter and grbl.
 * https://github.com/kliment/Sprinter
 * https://github.com/simen/grbl/tree
 *
 * It has preliminary support for Matthew Roberts advance algorithm
 * http://reprap.org/pipermail/reprap-dev/2011-May/003323.html
 *
 * Prusa Research s.r.o. https://www.prusa3d.cz
 *
 * @section copyright_sec Copyright
 *
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section notes_sec Notes
 *
 * * Do not create static objects in global functions.
 *   Otherwise constructor guard against concurrent calls is generated costing
 *   about 8B RAM and 14B flash.
 *
 *
 */

//-//
#include "Configuration.h"
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

#include "printers.h"

#include "menu.h"
#include "ultralcd.h"
#include "backlight.h"

#include "planner.h"
#include "stepper.h"
#include "temperature.h"
#include "motion_control.h"
#include "cardreader.h"
#include "ConfigurationStore.h"
#include "language.h"
#include "pins_arduino.h"
#include "math.h"
#include "util.h"
#include "Timer.h"

#include <avr/wdt.h>
#include <avr/pgmspace.h>

#include "Dcodes.h"
#include "AutoDeplete.h"

#ifndef LA_NOCOMPAT
#include "la10compat.h"
#endif

#ifdef SWSPI
#include "swspi.h"
#endif //SWSPI

#include "spi.h"

#ifdef SWI2C
#include "swi2c.h"
#endif //SWI2C

#ifdef FILAMENT_SENSOR
#include "fsensor.h"
#endif //FILAMENT_SENSOR

#ifdef TMC2130
#include "tmc2130.h"
#endif //TMC2130

#ifdef W25X20CL
#include "w25x20cl.h"
#include "optiboot_w25x20cl.h"
#endif //W25X20CL

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

#include "mmu.h"

#define VERSION_STRING  "1.0.2"


#include "ultralcd.h"
#include "sound.h"

#include "cmdqueue.h"
#include "io_atmega2560.h"

// Macros for bit masks
#define BIT(b) (1<<(b))
#define TEST(n,b) (((n)&BIT(b))!=0)
#define SET_BIT(n,b,value) (n) ^= ((-value)^(n)) & (BIT(b))

//Macro for print fan speed
#define FAN_PULSE_WIDTH_LIMIT ((fanSpeed > 100) ? 3 : 4) //time in ms

//filament types 
#define FILAMENT_DEFAULT 0
#define FILAMENT_FLEX 1
#define FILAMENT_PVA 2
#define FILAMENT_UNDEFINED 255

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

unsigned long PingTime = _millis();
unsigned long NcTime;

uint8_t mbl_z_probe_nr = 3; //numer of Z measurements for each point in mesh bed leveling calibration

//used for PINDA temp calibration and pause print
#define DEFAULT_RETRACTION    1
#define DEFAULT_RETRACTION_MM 4 //MM

float default_retraction = DEFAULT_RETRACTION;


float homing_feedrate[] = HOMING_FEEDRATE;
// Currently only the extruder axis may be switched to a relative mode.
// Other axes are always absolute or relative based on the common relative_mode flag.
bool axis_relative_modes[] = AXIS_RELATIVE_MODES;
int feedmultiply=100; //100->1 200->2
int extrudemultiply=100; //100->1 200->2
int extruder_multiply[EXTRUDERS] = {100
  #if EXTRUDERS > 1
    , 100
    #if EXTRUDERS > 2
      , 100
    #endif
  #endif
};

int bowden_length[4] = {385, 385, 385, 385};

bool is_usb_printing = false;
bool homing_flag = false;

bool temp_cal_active = false;

unsigned long kicktime = _millis()+100000;

unsigned int  usb_printing_counter;

int8_t lcd_change_fil_state = 0;

unsigned long pause_time = 0;
unsigned long start_pause_print = _millis();
unsigned long t_fan_rising_edge = _millis();
LongTimer safetyTimer;
static LongTimer crashDetTimer;

//unsigned long load_filament_time;

bool mesh_bed_leveling_flag = false;
bool mesh_bed_run_from_menu = false;

bool prusa_sd_card_upload = false;

unsigned int status_number = 0;

unsigned long total_filament_used;
unsigned int heating_status;
unsigned int heating_status_counter;
bool loading_flag = false;



char snmm_filaments_used = 0;


bool fan_state[2];
int fan_edge_counter[2];
int fan_speed[2];

char dir_names[3][9];

bool sortAlpha = false;


float extruder_multiplier[EXTRUDERS] = {1.0
  #if EXTRUDERS > 1
    , 1.0
    #if EXTRUDERS > 2
      , 1.0
    #endif
  #endif
};

float current_position[NUM_AXIS] = { 0.0, 0.0, 0.0, 0.0 };
//shortcuts for more readable code
#define _x current_position[X_AXIS]
#define _y current_position[Y_AXIS]
#define _z current_position[Z_AXIS]
#define _e current_position[E_AXIS]

float min_pos[3] = { X_MIN_POS, Y_MIN_POS, Z_MIN_POS };
float max_pos[3] = { X_MAX_POS, Y_MAX_POS, Z_MAX_POS };
bool axis_known_position[3] = {false, false, false};

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

  float retract_length_swap = RETRACT_LENGTH_SWAP;
  float retract_recover_length_swap = RETRACT_RECOVER_LENGTH_SWAP;
#endif

  #ifdef PS_DEFAULT_OFF
    bool powersupply = false;
  #else
	  bool powersupply = true;
  #endif

bool cancel_heatup = false ;

int8_t busy_state = NOT_BUSY;
static long prev_busy_signal_ms = -1;
uint8_t host_keepalive_interval = HOST_KEEPALIVE_INTERVAL;

const char errormagic[] PROGMEM = "Error:";
const char echomagic[] PROGMEM = "echo:";

bool no_response = false;
uint8_t important_status;
uint8_t saved_filament_type;

#define SAVED_TARGET_UNSET (X_MIN_POS-1)
float saved_target[NUM_AXIS] = {SAVED_TARGET_UNSET, 0, 0, 0};

// save/restore printing in case that mmu was not responding 
bool mmu_print_saved = false;

// storing estimated time to end of print counted by slicer
uint8_t print_percent_done_normal = PRINT_PERCENT_DONE_INIT;
uint16_t print_time_remaining_normal = PRINT_TIME_REMAINING_INIT; //estimated remaining print time in minutes
uint8_t print_percent_done_silent = PRINT_PERCENT_DONE_INIT;
uint16_t print_time_remaining_silent = PRINT_TIME_REMAINING_INIT; //estimated remaining print time in minutes

//===========================================================================
//=============================Private Variables=============================
//===========================================================================
#define MSG_BED_LEVELING_FAILED_TIMEOUT 30

const char axis_codes[NUM_AXIS] = {'X', 'Y', 'Z', 'E'};
float destination[NUM_AXIS] = {  0.0, 0.0, 0.0, 0.0};

// For tracing an arc
static float offset[3] = {0.0, 0.0, 0.0};

// Current feedrate
float feedrate = 1500.0;

// Feedrate for the next move
static float next_feedrate;

// Original feedrate saved during homing moves
static float saved_feedrate;

const int sensitive_pins[] = SENSITIVE_PINS; // Sensitive pin list for M42

//static float tt = 0;
//static float bt = 0;

//Inactivity shutdown variables
static unsigned long previous_millis_cmd = 0;
unsigned long max_inactive_time = 0;
static unsigned long stepper_inactive_time = DEFAULT_STEPPER_DEACTIVE_TIME*1000l;
static unsigned long safetytimer_inactive_time = DEFAULT_SAFETYTIMER_TIME_MINS*60*1000ul;

unsigned long starttime=0;
unsigned long stoptime=0;
unsigned long _usb_timer = 0;

bool Stopped=false;

#if NUM_SERVOS > 0
  Servo servos[NUM_SERVOS];
#endif

bool target_direction;

//Insert variables if CHDK is defined
#ifdef CHDK
unsigned long chdkHigh = 0;
boolean chdkActive = false;
#endif

//! @name RAM save/restore printing
//! @{
bool saved_printing = false; //!< Print is paused and saved in RAM
static uint32_t saved_sdpos = 0; //!< SD card position, or line number in case of USB printing
uint8_t saved_printing_type = PRINTING_TYPE_SD;
static float saved_pos[4] = { 0, 0, 0, 0 };
static uint16_t saved_feedrate2 = 0; //!< Default feedrate (truncated from float)
static int saved_feedmultiply2 = 0;
static uint8_t saved_active_extruder = 0;
static float saved_extruder_temperature = 0.0; //!< Active extruder temperature
static bool saved_extruder_relative_mode = false;
static int saved_fanSpeed = 0; //!< Print fan speed
//! @}

static int saved_feedmultiply_mm = 100;

//===========================================================================
//=============================Routines======================================
//===========================================================================

static void get_arc_coordinates();
static bool setTargetedHotend(int code, uint8_t &extruder);
static void print_time_remaining_init();
static void wait_for_heater(long codenum, uint8_t extruder);
static void gcode_G28(bool home_x_axis, bool home_y_axis, bool home_z_axis);
static void temp_compensation_start();
static void temp_compensation_apply();


uint16_t gcode_in_progress = 0;
uint16_t mcode_in_progress = 0;

void serial_echopair_P(const char *s_P, float v)
    { serialprintPGM(s_P); SERIAL_ECHO(v); }
void serial_echopair_P(const char *s_P, double v)
    { serialprintPGM(s_P); SERIAL_ECHO(v); }
void serial_echopair_P(const char *s_P, unsigned long v)
    { serialprintPGM(s_P); SERIAL_ECHO(v); }

/*FORCE_INLINE*/ void serialprintPGM(const char *str)
{
#if 0
  char ch=pgm_read_byte(str);
  while(ch)
  {
    MYSERIAL.write(ch);
    ch=pgm_read_byte(++str);
  }
#else
	// hmm, same size as the above version, the compiler did a good job optimizing the above
	while( uint8_t ch = pgm_read_byte(str) ){
	  MYSERIAL.write((char)ch);
	  ++str;
	}
#endif
}

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


bool fans_check_enabled = true;

#ifdef TMC2130

void crashdet_stop_and_save_print()
{
	stop_and_save_print_to_ram(10, -default_retraction); //XY - no change, Z 10mm up, E -1mm retract
}

void crashdet_restore_print_and_continue()
{
	restore_print_from_ram_and_continue(default_retraction); //XYZ = orig, E +1mm unretract
//	babystep_apply();
}


void crashdet_stop_and_save_print2()
{
	cli();
	planner_abort_hard(); //abort printing
	cmdqueue_reset(); //empty cmdqueue
	card.sdprinting = false;
	card.closefile();
  // Reset and re-enable the stepper timer just before the global interrupts are enabled.
  st_reset_timer();
	sei();
}

void crashdet_detected(uint8_t mask)
{
	st_synchronize();
	static uint8_t crashDet_counter = 0;
	bool automatic_recovery_after_crash = true;

	if (crashDet_counter++ == 0) {
		crashDetTimer.start();
	}
	else if (crashDetTimer.expired(CRASHDET_TIMER * 1000ul)){
		crashDetTimer.stop();
		crashDet_counter = 0;
	}
	else if(crashDet_counter == CRASHDET_COUNTER_MAX){
		automatic_recovery_after_crash = false;
		crashDetTimer.stop();
		crashDet_counter = 0;
	}
	else {
		crashDetTimer.start();
	}

	lcd_update_enable(true);
	lcd_clear();
	lcd_update(2);

	if (mask & X_AXIS_MASK)
	{
		eeprom_update_byte((uint8_t*)EEPROM_CRASH_COUNT_X, eeprom_read_byte((uint8_t*)EEPROM_CRASH_COUNT_X) + 1);
		eeprom_update_word((uint16_t*)EEPROM_CRASH_COUNT_X_TOT, eeprom_read_word((uint16_t*)EEPROM_CRASH_COUNT_X_TOT) + 1);
	}
	if (mask & Y_AXIS_MASK)
	{
		eeprom_update_byte((uint8_t*)EEPROM_CRASH_COUNT_Y, eeprom_read_byte((uint8_t*)EEPROM_CRASH_COUNT_Y) + 1);
		eeprom_update_word((uint16_t*)EEPROM_CRASH_COUNT_Y_TOT, eeprom_read_word((uint16_t*)EEPROM_CRASH_COUNT_Y_TOT) + 1);
	}
    


	lcd_update_enable(true);
	lcd_update(2);
	lcd_setstatuspgm(_T(MSG_CRASH_DETECTED));
	gcode_G28(true, true, false); //home X and Y
	st_synchronize();

	if (automatic_recovery_after_crash) {
		enquecommand_P(PSTR("CRASH_RECOVER"));
	}else{
		setTargetHotend(0, active_extruder);
		bool yesno = lcd_show_fullscreen_message_yes_no_and_wait_P(_i("Crash detected. Resume print?"), false);
		lcd_update_enable(true);
		if (yesno)
		{
			enquecommand_P(PSTR("CRASH_RECOVER"));
		}
		else
		{
			enquecommand_P(PSTR("CRASH_CANCEL"));
		}
	}
}

void crashdet_recover()
{
	crashdet_restore_print_and_continue();
	if (lcd_crash_detect_enabled()) tmc2130_sg_stop_on_crash = true;
}

void crashdet_cancel()
{
	saved_printing = false;
	tmc2130_sg_stop_on_crash = true;
	if (saved_printing_type == PRINTING_TYPE_SD) {
		lcd_print_stop();
	}else if(saved_printing_type == PRINTING_TYPE_USB){
		SERIAL_ECHOLNRPGM(MSG_OCTOPRINT_CANCEL); //for Octoprint: works the same as clicking "Abort" button in Octoprint GUI
		SERIAL_PROTOCOLLNRPGM(MSG_OK);
	}
}

#endif //TMC2130

void failstats_reset_print()
{
	eeprom_update_byte((uint8_t *)EEPROM_CRASH_COUNT_X, 0);
	eeprom_update_byte((uint8_t *)EEPROM_CRASH_COUNT_Y, 0);
	eeprom_update_byte((uint8_t *)EEPROM_FERROR_COUNT, 0);
	eeprom_update_byte((uint8_t *)EEPROM_POWER_COUNT, 0);
	eeprom_update_byte((uint8_t *)EEPROM_MMU_FAIL, 0);
	eeprom_update_byte((uint8_t *)EEPROM_MMU_LOAD_FAIL, 0);
#if defined(FILAMENT_SENSOR) && defined(PAT9125)
    fsensor_softfail = 0;
#endif
}



#ifdef MESH_BED_LEVELING
   enum MeshLevelingState { MeshReport, MeshStart, MeshNext, MeshSet };
#endif


// Factory reset function
// This function is used to erase parts or whole EEPROM memory which is used for storing calibration and and so on.
// Level input parameter sets depth of reset
int  er_progress = 0;
static void factory_reset(char level)
{	
	lcd_clear();
    switch (level) {
                   
        // Level 0: Language reset
        case 0:
      Sound_MakeCustom(100,0,false);
			lang_reset();
            break;
         
		//Level 1: Reset statistics
		case 1:
      Sound_MakeCustom(100,0,false);
			eeprom_update_dword((uint32_t *)EEPROM_TOTALTIME, 0);
			eeprom_update_dword((uint32_t *)EEPROM_FILAMENTUSED, 0);

			eeprom_update_byte((uint8_t *)EEPROM_CRASH_COUNT_X, 0);
			eeprom_update_byte((uint8_t *)EEPROM_CRASH_COUNT_Y, 0);
			eeprom_update_byte((uint8_t *)EEPROM_FERROR_COUNT, 0);
			eeprom_update_byte((uint8_t *)EEPROM_POWER_COUNT, 0);

			eeprom_update_word((uint16_t *)EEPROM_CRASH_COUNT_X_TOT, 0);
			eeprom_update_word((uint16_t *)EEPROM_CRASH_COUNT_Y_TOT, 0);
			eeprom_update_word((uint16_t *)EEPROM_FERROR_COUNT_TOT, 0);
			eeprom_update_word((uint16_t *)EEPROM_POWER_COUNT_TOT, 0);

			eeprom_update_word((uint16_t *)EEPROM_MMU_FAIL_TOT, 0);
			eeprom_update_word((uint16_t *)EEPROM_MMU_LOAD_FAIL_TOT, 0);
			eeprom_update_byte((uint8_t *)EEPROM_MMU_FAIL, 0);
			eeprom_update_byte((uint8_t *)EEPROM_MMU_LOAD_FAIL, 0);


			lcd_menu_statistics();
            
			break;

        // Level 2: Prepare for shipping
        case 2:
			//lcd_puts_P(PSTR("Factory RESET"));
            //lcd_puts_at_P(1,2,PSTR("Shipping prep"));
            
            // Force language selection at the next boot up.
			lang_reset();
            // Force the "Follow calibration flow" message at the next boot up.
            calibration_status_store(CALIBRATION_STATUS_Z_CALIBRATION);
			eeprom_write_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 1); //run wizard
            farm_no = 0;
			farm_mode = false;
			eeprom_update_byte((uint8_t*)EEPROM_FARM_MODE, farm_mode);
            EEPROM_save_B(EEPROM_FARM_NUMBER, &farm_no);

            eeprom_update_dword((uint32_t *)EEPROM_TOTALTIME, 0);
            eeprom_update_dword((uint32_t *)EEPROM_FILAMENTUSED, 0);
            eeprom_update_word((uint16_t *)EEPROM_CRASH_COUNT_X_TOT, 0);
            eeprom_update_word((uint16_t *)EEPROM_CRASH_COUNT_Y_TOT, 0);
            eeprom_update_word((uint16_t *)EEPROM_FERROR_COUNT_TOT, 0);
            eeprom_update_word((uint16_t *)EEPROM_POWER_COUNT_TOT, 0);

			eeprom_update_word((uint16_t *)EEPROM_MMU_FAIL_TOT, 0);
			eeprom_update_word((uint16_t *)EEPROM_MMU_LOAD_FAIL_TOT, 0);
			eeprom_update_byte((uint8_t *)EEPROM_MMU_FAIL, 0);
			eeprom_update_byte((uint8_t *)EEPROM_MMU_LOAD_FAIL, 0);

#ifdef FILAMENT_SENSOR
			fsensor_enable();
            fsensor_autoload_set(true);
#endif //FILAMENT_SENSOR
      Sound_MakeCustom(100,0,false);   
			//_delay_ms(2000);
            break;

			// Level 3: erase everything, whole EEPROM will be set to 0xFF

		case 3:
			lcd_puts_P(PSTR("Factory RESET"));
			lcd_puts_at_P(1, 2, PSTR("ERASING all data"));

      Sound_MakeCustom(100,0,false);
			er_progress = 0;
			lcd_puts_at_P(3, 3, PSTR("      "));
			lcd_set_cursor(3, 3);
			lcd_print(er_progress);

			// Erase EEPROM
			for (int i = 0; i < 4096; i++) {
				eeprom_update_byte((uint8_t*)i, 0xFF);

				if (i % 41 == 0) {
					er_progress++;
					lcd_puts_at_P(3, 3, PSTR("      "));
					lcd_set_cursor(3, 3);
					lcd_print(er_progress);
					lcd_puts_P(PSTR("%"));
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

extern "C" {
FILE _uartout; //= {0}; Global variable is always zero initialized. No need to explicitly state this.
}

int uart_putchar(char c, FILE *)
{
	MYSERIAL.write(c);
	return 0;
}


void lcd_splash()
{
	lcd_clear(); // clears display and homes screen
	lcd_puts_P(PSTR("\n Original Prusa i3\n   Prusa Research"));
}


void factory_reset() 
{
	KEEPALIVE_STATE(PAUSED_FOR_USER);
	if (!READ(BTN_ENC))
	{
		_delay_ms(1000);
		if (!READ(BTN_ENC))
		{
			lcd_clear();


			lcd_puts_P(PSTR("Factory RESET"));


			SET_OUTPUT(BEEPER);
  if(eSoundMode!=e_SOUND_MODE_SILENT)
			WRITE(BEEPER, HIGH);

			while (!READ(BTN_ENC));

			WRITE(BEEPER, LOW);



			_delay_ms(2000);

			char level = reset_menu();
			factory_reset(level);

			switch (level) {
			case 0: _delay_ms(0); break;
			case 1: _delay_ms(0); break;
			case 2: _delay_ms(0); break;
			case 3: _delay_ms(0); break;
			}

		}
	}
	KEEPALIVE_STATE(IN_HANDLER);
}

void show_fw_version_warnings() {
	if (FW_DEV_VERSION == FW_VERSION_GOLD || FW_DEV_VERSION == FW_VERSION_RC) return;
	switch (FW_DEV_VERSION) {
	case(FW_VERSION_ALPHA):   lcd_show_fullscreen_message_and_wait_P(_i("You are using firmware alpha version. This is development version. Using this version is not recommended and may cause printer damage."));   break;////MSG_FW_VERSION_ALPHA c=20 r=8
	case(FW_VERSION_BETA):    lcd_show_fullscreen_message_and_wait_P(_i("You are using firmware beta version. This is development version. Using this version is not recommended and may cause printer damage."));    break;////MSG_FW_VERSION_BETA c=20 r=8
  case(FW_VERSION_DEVEL):
	case(FW_VERSION_DEBUG):
    lcd_update_enable(false);
    lcd_clear();
  #if FW_DEV_VERSION == FW_VERSION_DEVEL
    lcd_puts_at_P(0, 0, PSTR("Development build !!"));
  #else
    lcd_puts_at_P(0, 0, PSTR("Debbugging build !!!"));
  #endif
    lcd_puts_at_P(0, 1, PSTR("May destroy printer!"));
    lcd_puts_at_P(0, 2, PSTR("ver ")); lcd_puts_P(PSTR(FW_VERSION_FULL));
    lcd_puts_at_P(0, 3, PSTR(FW_REPOSITORY));
    lcd_wait_for_click();
    break;
//	default: lcd_show_fullscreen_message_and_wait_P(_i("WARNING: This is an unofficial, unsupported build. Use at your own risk!")); break;////MSG_FW_VERSION_UNKNOWN c=20 r=8
	}
	lcd_update_enable(true);
}

//! @brief try to check if firmware is on right type of printer
static void check_if_fw_is_on_right_printer(){
#ifdef FILAMENT_SENSOR
  if((PRINTER_TYPE == PRINTER_MK3) || (PRINTER_TYPE == PRINTER_MK3S)){
    #ifdef IR_SENSOR
    swi2c_init();
    const uint8_t pat9125_detected = swi2c_readByte_A8(PAT9125_I2C_ADDR,0x00,NULL);
      if (pat9125_detected){
        lcd_show_fullscreen_message_and_wait_P(_i("MK3S firmware detected on MK3 printer"));}
    #endif //IR_SENSOR

    #ifdef PAT9125
      //will return 1 only if IR can detect filament in bondtech extruder so this may fail even when we have IR sensor
      const uint8_t ir_detected = !(PIN_GET(IR_SENSOR_PIN));
      if (ir_detected){
        lcd_show_fullscreen_message_and_wait_P(_i("MK3 firmware detected on MK3S printer"));}
    #endif //PAT9125
  }
#endif //FILAMENT_SENSOR
}

uint8_t check_printer_version()
{
	uint8_t version_changed = 0;
	uint16_t printer_type = eeprom_read_word((uint16_t*)EEPROM_PRINTER_TYPE);
	uint16_t motherboard = eeprom_read_word((uint16_t*)EEPROM_BOARD_TYPE);

	if (printer_type != PRINTER_TYPE) {
		if (printer_type == 0xffff) eeprom_write_word((uint16_t*)EEPROM_PRINTER_TYPE, PRINTER_TYPE);
		else version_changed |= 0b10;
	}
	if (motherboard != MOTHERBOARD) {
		if(motherboard == 0xffff) eeprom_write_word((uint16_t*)EEPROM_BOARD_TYPE, MOTHERBOARD);
		else version_changed |= 0b01;
	}
	return version_changed;
}

#ifdef BOOTAPP
#include "bootapp.h" //bootloader support
#endif //BOOTAPP

#if (LANG_MODE != 0) //secondary language support

#ifdef W25X20CL


// language update from external flash
#define LANGBOOT_BLOCKSIZE 0x1000u
#define LANGBOOT_RAMBUFFER 0x0800

void update_sec_lang_from_external_flash()
{
	if ((boot_app_magic == BOOT_APP_MAGIC) && (boot_app_flags & BOOT_APP_FLG_USER0))
	{
		uint8_t lang = boot_reserved >> 4;
		uint8_t state = boot_reserved & 0xf;
		lang_table_header_t header;
		uint32_t src_addr;
		if (lang_get_header(lang, &header, &src_addr))
		{
			lcd_puts_at_P(1,3,PSTR("Language update."));
			for (uint8_t i = 0; i < state; i++) fputc('.', lcdout);
			_delay(100);
			boot_reserved = (state + 1) | (lang << 4);
			if ((state * LANGBOOT_BLOCKSIZE) < header.size)
			{
				cli();
				uint16_t size = header.size - state * LANGBOOT_BLOCKSIZE;
				if (size > LANGBOOT_BLOCKSIZE) size = LANGBOOT_BLOCKSIZE;
				w25x20cl_rd_data(src_addr + state * LANGBOOT_BLOCKSIZE, (uint8_t*)LANGBOOT_RAMBUFFER, size);
				if (state == 0)
				{
					//TODO - check header integrity
				}
				bootapp_ram2flash(LANGBOOT_RAMBUFFER, _SEC_LANG_TABLE + state * LANGBOOT_BLOCKSIZE, size);
			}
			else
			{
				//TODO - check sec lang data integrity
				eeprom_update_byte((unsigned char *)EEPROM_LANG, LANG_ID_SEC);
			}
		}
	}
	boot_app_flags &= ~BOOT_APP_FLG_USER0;
}


#ifdef DEBUG_W25X20CL

uint8_t lang_xflash_enum_codes(uint16_t* codes)
{
	lang_table_header_t header;
	uint8_t count = 0;
	uint32_t addr = 0x00000;
	while (1)
	{
		printf_P(_n("LANGTABLE%d:"), count);
		w25x20cl_rd_data(addr, (uint8_t*)&header, sizeof(lang_table_header_t));
		if (header.magic != LANG_MAGIC)
		{
			printf_P(_n("NG!\n"));
			break;
		}
		printf_P(_n("OK\n"));
		printf_P(_n(" _lt_magic        = 0x%08lx %S\n"), header.magic, (header.magic==LANG_MAGIC)?_n("OK"):_n("NA"));
		printf_P(_n(" _lt_size         = 0x%04x (%d)\n"), header.size, header.size);
		printf_P(_n(" _lt_count        = 0x%04x (%d)\n"), header.count, header.count);
		printf_P(_n(" _lt_chsum        = 0x%04x\n"), header.checksum);
		printf_P(_n(" _lt_code         = 0x%04x (%c%c)\n"), header.code, header.code >> 8, header.code & 0xff);
		printf_P(_n(" _lt_sign         = 0x%08lx\n"), header.signature);

		addr += header.size;
		codes[count] = header.code;
		count ++;
	}
	return count;
}

void list_sec_lang_from_external_flash()
{
	uint16_t codes[8];
	uint8_t count = lang_xflash_enum_codes(codes);
	printf_P(_n("XFlash lang count = %hhd\n"), count);
}

#endif //DEBUG_W25X20CL

#endif //W25X20CL

#endif //(LANG_MODE != 0)


static void w25x20cl_err_msg()
{
	lcd_clear();
	lcd_puts_P(_n("External SPI flash\nW25X20CL is not res-\nponding. Language\nswitch unavailable."));
}

// "Setup" function is called by the Arduino framework on startup.
// Before startup, the Timers-functions (PWM)/Analog RW and HardwareSerial provided by the Arduino-code 
// are initialized by the main() routine provided by the Arduino framework.
void setup()
{
	mmu_init();

	ultralcd_init();

	spi_init();

	lcd_splash();
    Sound_Init();                                // also guarantee "SET_OUTPUT(BEEPER)"

#ifdef W25X20CL
    bool w25x20cl_success = w25x20cl_init();
	if (w25x20cl_success)
	{
	    optiboot_w25x20cl_enter();
#if (LANG_MODE != 0) //secondary language support
        update_sec_lang_from_external_flash();
#endif //(LANG_MODE != 0)
	}
	else
	{
	    w25x20cl_err_msg();
	}
#else
	const bool w25x20cl_success = true;
#endif //W25X20CL


	setup_killpin();
	setup_powerhold();

	farm_mode = eeprom_read_byte((uint8_t*)EEPROM_FARM_MODE); 
	EEPROM_read_B(EEPROM_FARM_NUMBER, &farm_no);
	if ((farm_mode == 0xFF && farm_no == 0) || ((uint16_t)farm_no == 0xFFFF)) 
		farm_mode = false; //if farm_mode has not been stored to eeprom yet and farm number is set to zero or EEPROM is fresh, deactivate farm mode
	if ((uint16_t)farm_no == 0xFFFF) farm_no = 0;
	
	selectedSerialPort = eeprom_read_byte((uint8_t*)EEPROM_SECOND_SERIAL_ACTIVE);
	if (selectedSerialPort == 0xFF) selectedSerialPort = 0;
	if (farm_mode)
	{
		no_response = true; //we need confirmation by recieving PRUSA thx
		important_status = 8;
		prusa_statistics(8);
		selectedSerialPort = 1;
#ifdef TMC2130
		//increased extruder current (PFW363)
		tmc2130_current_h[E_AXIS] = 36;
		tmc2130_current_r[E_AXIS] = 36;
#endif //TMC2130
#ifdef FILAMENT_SENSOR
		//disabled filament autoload (PFW360)
		fsensor_autoload_set(false);
#endif //FILAMENT_SENSOR
          // ~ FanCheck -> on
          if(!(eeprom_read_byte((uint8_t*)EEPROM_FAN_CHECK_ENABLED)))
               eeprom_update_byte((unsigned char *)EEPROM_FAN_CHECK_ENABLED,true);
	}
	MYSERIAL.begin(BAUDRATE);
	fdev_setup_stream(uartout, uart_putchar, NULL, _FDEV_SETUP_WRITE); //setup uart out stream
#ifndef W25X20CL
	SERIAL_PROTOCOLLNPGM("start");
#endif //W25X20CL
	stdout = uartout;
	SERIAL_ECHO_START;
	printf_P(PSTR(" " FW_VERSION_FULL "\n"));

	//SERIAL_ECHOPAIR("Active sheet before:", static_cast<unsigned long int>(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet))));

#ifdef DEBUG_SEC_LANG
	lang_table_header_t header;
	uint32_t src_addr = 0x00000;
	if (lang_get_header(1, &header, &src_addr))
	{
//this is comparsion of some printing-methods regarding to flash space usage and code size/readability
#define LT_PRINT_TEST 2
//  flash usage
//  total   p.test
//0 252718  t+c  text code
//1 253142  424  170  254
//2 253040  322  164  158
//3 253248  530  135  395
#if (LT_PRINT_TEST==1) //not optimized printf
		printf_P(_n(" _src_addr = 0x%08lx\n"), src_addr);
		printf_P(_n(" _lt_magic = 0x%08lx %S\n"), header.magic, (header.magic==LANG_MAGIC)?_n("OK"):_n("NA"));
		printf_P(_n(" _lt_size  = 0x%04x (%d)\n"), header.size, header.size);
		printf_P(_n(" _lt_count = 0x%04x (%d)\n"), header.count, header.count);
		printf_P(_n(" _lt_chsum = 0x%04x\n"), header.checksum);
		printf_P(_n(" _lt_code  = 0x%04x (%c%c)\n"), header.code, header.code >> 8, header.code & 0xff);
		printf_P(_n(" _lt_sign = 0x%08lx\n"), header.signature);
#elif (LT_PRINT_TEST==2) //optimized printf
		printf_P(
		 _n(
		  " _src_addr = 0x%08lx\n"
		  " _lt_magic = 0x%08lx %S\n"
		  " _lt_size  = 0x%04x (%d)\n"
		  " _lt_count = 0x%04x (%d)\n"
		  " _lt_chsum = 0x%04x\n"
		  " _lt_code  = 0x%04x (%c%c)\n"
		  " _lt_resv1 = 0x%08lx\n"
		 ),
		 src_addr,
		 header.magic, (header.magic==LANG_MAGIC)?_n("OK"):_n("NA"),
		 header.size, header.size,
		 header.count, header.count,
		 header.checksum,
		 header.code, header.code >> 8, header.code & 0xff,
		 header.signature
		);
#elif (LT_PRINT_TEST==3) //arduino print/println (leading zeros not solved)
		MYSERIAL.print(" _src_addr = 0x");
		MYSERIAL.println(src_addr, 16);
		MYSERIAL.print(" _lt_magic = 0x");
		MYSERIAL.print(header.magic, 16);
		MYSERIAL.println((header.magic==LANG_MAGIC)?" OK":" NA");
		MYSERIAL.print(" _lt_size  = 0x");
		MYSERIAL.print(header.size, 16);
		MYSERIAL.print(" (");
		MYSERIAL.print(header.size, 10);
		MYSERIAL.println(")");
		MYSERIAL.print(" _lt_count = 0x");
		MYSERIAL.print(header.count, 16);
		MYSERIAL.print(" (");
		MYSERIAL.print(header.count, 10);
		MYSERIAL.println(")");
		MYSERIAL.print(" _lt_chsum = 0x");
		MYSERIAL.println(header.checksum, 16);
		MYSERIAL.print(" _lt_code  = 0x");
		MYSERIAL.print(header.code, 16);
		MYSERIAL.print(" (");
		MYSERIAL.print((char)(header.code >> 8), 0);
		MYSERIAL.print((char)(header.code & 0xff), 0);
		MYSERIAL.println(")");
		MYSERIAL.print(" _lt_resv1 = 0x");
		MYSERIAL.println(header.signature, 16);
#endif //(LT_PRINT_TEST==)
#undef LT_PRINT_TEST

#if 0
		w25x20cl_rd_data(0x25ba, (uint8_t*)&block_buffer, 1024);
		for (uint16_t i = 0; i < 1024; i++)
		{
			if ((i % 16) == 0) printf_P(_n("%04x:"), 0x25ba+i);
			printf_P(_n(" %02x"), ((uint8_t*)&block_buffer)[i]);
			if ((i % 16) == 15) putchar('\n');
		}
#endif
		uint16_t sum = 0;
		for (uint16_t i = 0; i < header.size; i++)
			sum += (uint16_t)pgm_read_byte((uint8_t*)(_SEC_LANG_TABLE + i)) << ((i & 1)?0:8);
		printf_P(_n("_SEC_LANG_TABLE checksum = %04x\n"), sum);
		sum -= header.checksum; //subtract checksum
		printf_P(_n("_SEC_LANG_TABLE checksum = %04x\n"), sum);
		sum = (sum >> 8) | ((sum & 0xff) << 8); //swap bytes
		if (sum == header.checksum)
			printf_P(_n("Checksum OK\n"), sum);
		else
			printf_P(_n("Checksum NG\n"), sum);
	}
	else
		printf_P(_n("lang_get_header failed!\n"));

#if 0
		for (uint16_t i = 0; i < 1024*10; i++)
		{
			if ((i % 16) == 0) printf_P(_n("%04x:"), _SEC_LANG_TABLE+i);
			printf_P(_n(" %02x"), pgm_read_byte((uint8_t*)(_SEC_LANG_TABLE+i)));
			if ((i % 16) == 15) putchar('\n');
		}
#endif

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

#endif //DEBUG_SEC_LANG

	// Check startup - does nothing if bootloader sets MCUSR to 0
	byte mcu = MCUSR;
/*	if (mcu & 1) SERIAL_ECHOLNRPGM(MSG_POWERUP);
	if (mcu & 2) SERIAL_ECHOLNRPGM(MSG_EXTERNAL_RESET);
	if (mcu & 4) SERIAL_ECHOLNRPGM(MSG_BROWNOUT_RESET);
	if (mcu & 8) SERIAL_ECHOLNRPGM(MSG_WATCHDOG_RESET);
	if (mcu & 32) SERIAL_ECHOLNRPGM(MSG_SOFTWARE_RESET);*/
	if (mcu & 1) puts_P(MSG_POWERUP);
	if (mcu & 2) puts_P(MSG_EXTERNAL_RESET);
	if (mcu & 4) puts_P(MSG_BROWNOUT_RESET);
	if (mcu & 8) puts_P(MSG_WATCHDOG_RESET);
	if (mcu & 32) puts_P(MSG_SOFTWARE_RESET);
	MCUSR = 0;

	//SERIAL_ECHORPGM(MSG_MARLIN);
	//SERIAL_ECHOLNRPGM(VERSION_STRING);

#ifdef STRING_VERSION_CONFIG_H
#ifdef STRING_CONFIG_H_AUTHOR
	SERIAL_ECHO_START;
	SERIAL_ECHORPGM(_n(" Last Updated: "));////MSG_CONFIGURATION_VER
	SERIAL_ECHOPGM(STRING_VERSION_CONFIG_H);
	SERIAL_ECHORPGM(_n(" | Author: "));////MSG_AUTHOR
	SERIAL_ECHOLNPGM(STRING_CONFIG_H_AUTHOR);
	SERIAL_ECHOPGM("Compiled: ");
	SERIAL_ECHOLNPGM(__DATE__);
#endif
#endif

	SERIAL_ECHO_START;
	SERIAL_ECHORPGM(_n(" Free Memory: "));////MSG_FREE_MEMORY
	SERIAL_ECHO(freeMemory());
	SERIAL_ECHORPGM(_n("  PlannerBufferBytes: "));////MSG_PLANNER_BUFFER_BYTES
	SERIAL_ECHOLN((int)sizeof(block_t)*BLOCK_BUFFER_SIZE);
	//lcd_update_enable(false); // why do we need this?? - andre
	// loads data from EEPROM if available else uses defaults (and resets step acceleration rate)
	
	bool previous_settings_retrieved = false; 
	uint8_t hw_changed = check_printer_version();
	if (!(hw_changed & 0b10)) { //if printer version wasn't changed, check for eeprom version and retrieve settings from eeprom in case that version wasn't changed
		previous_settings_retrieved = Config_RetrieveSettings();
	} 
	else { //printer version was changed so use default settings 
		Config_ResetDefault();
	}
	SdFatUtil::set_stack_guard(); //writes magic number at the end of static variables to protect against overwriting static memory by stack

	tp_init();    // Initialize temperature loop

	if (w25x20cl_success) lcd_splash(); // we need to do this again, because tp_init() kills lcd
	else
	{
	    w25x20cl_err_msg();
	    printf_P(_n("W25X20CL not responding.\n"));
	}

	plan_init();  // Initialize planner;

	factory_reset();
	if (eeprom_read_dword((uint32_t*)(EEPROM_TOP - 4)) == 0x0ffffffff &&
	        eeprom_read_dword((uint32_t*)(EEPROM_TOP - 8)) == 0x0ffffffff)
	{
        // Maiden startup. The firmware has been loaded and first started on a virgin RAMBo board,
        // where all the EEPROM entries are set to 0x0ff.
        // Once a firmware boots up, it forces at least a language selection, which changes
        // EEPROM_LANG to number lower than 0x0ff.
        // 1) Set a high power mode.
	    eeprom_update_byte((uint8_t*)EEPROM_SILENT, SILENT_MODE_OFF);
#ifdef TMC2130
        tmc2130_mode = TMC2130_MODE_NORMAL;
#endif //TMC2130
        eeprom_write_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 1); //run wizard
    }

    lcd_encoder_diff=0;

#ifdef TMC2130
	uint8_t silentMode = eeprom_read_byte((uint8_t*)EEPROM_SILENT);
	if (silentMode == 0xff) silentMode = 0;
	tmc2130_mode = TMC2130_MODE_NORMAL;

	if (lcd_crash_detect_enabled() && !farm_mode)
	{
		lcd_crash_detect_enable();
	    puts_P(_N("CrashDetect ENABLED!"));
	}
	else
	{
	    lcd_crash_detect_disable();
	    puts_P(_N("CrashDetect DISABLED"));
	}

#ifdef TMC2130_LINEARITY_CORRECTION
#ifdef TMC2130_LINEARITY_CORRECTION_XYZ
	tmc2130_wave_fac[X_AXIS] = eeprom_read_byte((uint8_t*)EEPROM_TMC2130_WAVE_X_FAC);
	tmc2130_wave_fac[Y_AXIS] = eeprom_read_byte((uint8_t*)EEPROM_TMC2130_WAVE_Y_FAC);
	tmc2130_wave_fac[Z_AXIS] = eeprom_read_byte((uint8_t*)EEPROM_TMC2130_WAVE_Z_FAC);
#endif //TMC2130_LINEARITY_CORRECTION_XYZ
	tmc2130_wave_fac[E_AXIS] = eeprom_read_byte((uint8_t*)EEPROM_TMC2130_WAVE_E_FAC);
	if (tmc2130_wave_fac[X_AXIS] == 0xff) tmc2130_wave_fac[X_AXIS] = 0;
	if (tmc2130_wave_fac[Y_AXIS] == 0xff) tmc2130_wave_fac[Y_AXIS] = 0;
	if (tmc2130_wave_fac[Z_AXIS] == 0xff) tmc2130_wave_fac[Z_AXIS] = 0;
	if (tmc2130_wave_fac[E_AXIS] == 0xff) tmc2130_wave_fac[E_AXIS] = 0;
#endif //TMC2130_LINEARITY_CORRECTION

#ifdef TMC2130_VARIABLE_RESOLUTION
	tmc2130_mres[X_AXIS] = tmc2130_usteps2mres(cs.axis_ustep_resolution[X_AXIS]);
	tmc2130_mres[Y_AXIS] = tmc2130_usteps2mres(cs.axis_ustep_resolution[Y_AXIS]);
	tmc2130_mres[Z_AXIS] = tmc2130_usteps2mres(cs.axis_ustep_resolution[Z_AXIS]);
	tmc2130_mres[E_AXIS] = tmc2130_usteps2mres(cs.axis_ustep_resolution[E_AXIS]);
#else //TMC2130_VARIABLE_RESOLUTION
	tmc2130_mres[X_AXIS] = tmc2130_usteps2mres(TMC2130_USTEPS_XY);
	tmc2130_mres[Y_AXIS] = tmc2130_usteps2mres(TMC2130_USTEPS_XY);
	tmc2130_mres[Z_AXIS] = tmc2130_usteps2mres(TMC2130_USTEPS_Z);
	tmc2130_mres[E_AXIS] = tmc2130_usteps2mres(TMC2130_USTEPS_E);
#endif //TMC2130_VARIABLE_RESOLUTION

#endif //TMC2130

	st_init();    // Initialize stepper, this enables interrupts!
  
#ifdef TMC2130
	tmc2130_mode = silentMode?TMC2130_MODE_SILENT:TMC2130_MODE_NORMAL;
	update_mode_profile();
	tmc2130_init();
#endif //TMC2130
#ifdef PSU_Delta
     init_force_z();                              // ! important for correct Z-axis initialization
#endif // PSU_Delta
    
	setup_photpin();

	servo_init();
	// Reset the machine correction matrix.
	// It does not make sense to load the correction matrix until the machine is homed.
	world2machine_reset();
    
#ifdef FILAMENT_SENSOR
	fsensor_init();
#endif //FILAMENT_SENSOR


#if defined(CONTROLLERFAN_PIN) && (CONTROLLERFAN_PIN > -1)
	SET_OUTPUT(CONTROLLERFAN_PIN); //Set pin used for driver cooling fan
#endif

	setup_homepin();

#if defined(Z_AXIS_ALWAYS_ON)
    enable_z();
#endif

	farm_mode = eeprom_read_byte((uint8_t*)EEPROM_FARM_MODE);
	EEPROM_read_B(EEPROM_FARM_NUMBER, &farm_no);
	if ((farm_mode == 0xFF && farm_no == 0) || (farm_no == static_cast<int>(0xFFFF))) farm_mode = false; //if farm_mode has not been stored to eeprom yet and farm number is set to zero or EEPROM is fresh, deactivate farm mode
	if (farm_no == static_cast<int>(0xFFFF)) farm_no = 0;
	if (farm_mode)
	{
		prusa_statistics(8);
	}

	// Enable Toshiba FlashAir SD card / WiFi enahanced card.
	card.ToshibaFlashAir_enable(eeprom_read_byte((unsigned char*)EEPROM_TOSHIBA_FLASH_AIR_COMPATIBLITY) == 1);

	// Force SD card update. Otherwise the SD card update is done from loop() on card.checkautostart(false), 
	// but this times out if a blocking dialog is shown in setup().
	card.initsd();
#ifdef DEBUG_SD_SPEED_TEST
	if (card.cardOK)
	{
		uint8_t* buff = (uint8_t*)block_buffer;
		uint32_t block = 0;
		uint32_t sumr = 0;
		uint32_t sumw = 0;
		for (int i = 0; i < 1024; i++)
		{
			uint32_t u = _micros();
			bool res = card.card.readBlock(i, buff);
			u = _micros() - u;
			if (res)
			{
				printf_P(PSTR("readBlock %4d 512 bytes %lu us\n"), i, u);
				sumr += u;
				u = _micros();
				res = card.card.writeBlock(i, buff);
				u = _micros() - u;
				if (res)
				{
					printf_P(PSTR("writeBlock %4d 512 bytes %lu us\n"), i, u);
					sumw += u;
				}
				else
				{
					printf_P(PSTR("writeBlock %4d error\n"), i);
					break;
				}
			}
			else
			{
				printf_P(PSTR("readBlock %4d error\n"), i);
				break;
			}
		}
		uint32_t avg_rspeed = (1024 * 1000000) / (sumr / 512);
		uint32_t avg_wspeed = (1024 * 1000000) / (sumw / 512);
		printf_P(PSTR("avg read speed %lu bytes/s\n"), avg_rspeed);
		printf_P(PSTR("avg write speed %lu bytes/s\n"), avg_wspeed);
	}
	else
		printf_P(PSTR("Card NG!\n"));
#endif //DEBUG_SD_SPEED_TEST

    eeprom_init();
#ifdef SNMM
	if (eeprom_read_dword((uint32_t*)EEPROM_BOWDEN_LENGTH) == 0x0ffffffff) { //bowden length used for SNMM
	  int _z = BOWDEN_LENGTH;
	  for(int i = 0; i<4; i++) EEPROM_save_B(EEPROM_BOWDEN_LENGTH + i * 2, &_z);
	}
#endif

  // In the future, somewhere here would one compare the current firmware version against the firmware version stored in the EEPROM.
  // If they differ, an update procedure may need to be performed. At the end of this block, the current firmware version
  // is being written into the EEPROM, so the update procedure will be triggered only once.


#if (LANG_MODE != 0) //secondary language support

#ifdef DEBUG_W25X20CL
	W25X20CL_SPI_ENTER();
	uint8_t uid[8]; // 64bit unique id
	w25x20cl_rd_uid(uid);
	puts_P(_n("W25X20CL UID="));
	for (uint8_t i = 0; i < 8; i ++)
		printf_P(PSTR("%02hhx"), uid[i]);
	putchar('\n');
	list_sec_lang_from_external_flash();
#endif //DEBUG_W25X20CL

//	lang_reset();
	if (!lang_select(eeprom_read_byte((uint8_t*)EEPROM_LANG)))
		lcd_language();

#ifdef DEBUG_SEC_LANG

	uint16_t sec_lang_code = lang_get_code(1);
	uint16_t ui = _SEC_LANG_TABLE; //table pointer
	printf_P(_n("lang_selected=%d\nlang_table=0x%04x\nSEC_LANG_CODE=0x%04x (%c%c)\n"), lang_selected, ui, sec_lang_code, sec_lang_code >> 8, sec_lang_code & 0xff);

	lang_print_sec_lang(uartout);
#endif //DEBUG_SEC_LANG

#endif //(LANG_MODE != 0)

	if (eeprom_read_byte((uint8_t*)EEPROM_TEMP_CAL_ACTIVE) == 255) {
		eeprom_write_byte((uint8_t*)EEPROM_TEMP_CAL_ACTIVE, 0);
		temp_cal_active = false;
	} else temp_cal_active = eeprom_read_byte((uint8_t*)EEPROM_TEMP_CAL_ACTIVE);

	if (eeprom_read_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA) == 255) {
		//eeprom_write_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, 0);
		eeprom_write_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, 1);
		int16_t z_shift = 0;
		for (uint8_t i = 0; i < 5; i++) EEPROM_save_B(EEPROM_PROBE_TEMP_SHIFT + i * 2, &z_shift);
		eeprom_write_byte((uint8_t*)EEPROM_TEMP_CAL_ACTIVE, 0);
		temp_cal_active = false;
	}
	if (eeprom_read_byte((uint8_t*)EEPROM_UVLO) == 255) {
		eeprom_write_byte((uint8_t*)EEPROM_UVLO, 0);
	}
	if (eeprom_read_byte((uint8_t*)EEPROM_SD_SORT) == 255) {
		eeprom_write_byte((uint8_t*)EEPROM_SD_SORT, 0);
	}
	//mbl_mode_init();
	mbl_settings_init();
	SilentModeMenu_MMU = eeprom_read_byte((uint8_t*)EEPROM_MMU_STEALTH);
	if (SilentModeMenu_MMU == 255) {
		SilentModeMenu_MMU = 1;
		eeprom_write_byte((uint8_t*)EEPROM_MMU_STEALTH, SilentModeMenu_MMU);
	}

#if !defined(DEBUG_DISABLE_FANCHECK) && defined(FANCHECK) && defined(TACH_1) && TACH_1 >-1
	setup_fan_interrupt();
#endif //DEBUG_DISABLE_FANCHECK

#ifdef PAT9125
	fsensor_setup_interrupt();
#endif //PAT9125
	for (int i = 0; i<4; i++) EEPROM_read_B(EEPROM_BOWDEN_LENGTH + i * 2, &bowden_length[i]); 
	
#ifndef DEBUG_DISABLE_STARTMSGS
  KEEPALIVE_STATE(PAUSED_FOR_USER);

  if (!farm_mode) {
    check_if_fw_is_on_right_printer();
    show_fw_version_warnings();    
  }

  switch (hw_changed) { 
	  //if motherboard or printer type was changed inform user as it can indicate flashing wrong firmware version
	  //if user confirms with knob, new hw version (printer and/or motherboard) is written to eeprom and message will be not shown next time
	case(0b01): 
		lcd_show_fullscreen_message_and_wait_P(_i("Warning: motherboard type changed.")); ////MSG_CHANGED_MOTHERBOARD c=20 r=4
		eeprom_write_word((uint16_t*)EEPROM_BOARD_TYPE, MOTHERBOARD); 
		break;
	case(0b10): 
		lcd_show_fullscreen_message_and_wait_P(_i("Warning: printer type changed.")); ////MSG_CHANGED_PRINTER c=20 r=4
		eeprom_write_word((uint16_t*)EEPROM_PRINTER_TYPE, PRINTER_TYPE); 
		break;
	case(0b11): 
		lcd_show_fullscreen_message_and_wait_P(_i("Warning: both printer type and motherboard type changed.")); ////MSG_CHANGED_BOTH c=20 r=4
		eeprom_write_word((uint16_t*)EEPROM_PRINTER_TYPE, PRINTER_TYPE);
		eeprom_write_word((uint16_t*)EEPROM_BOARD_TYPE, MOTHERBOARD); 
		break;
	default: break; //no change, show no message
  }

  if (!previous_settings_retrieved) {
	  lcd_show_fullscreen_message_and_wait_P(_i("Old settings found. Default PID, Esteps etc. will be set.")); //if EEPROM version or printer type was changed, inform user that default setting were loaded////MSG_DEFAULT_SETTINGS_LOADED c=20 r=4
	  Config_StoreSettings();
  }
  if (eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE) == 1) {
	  lcd_wizard(WizState::Run);
  }
  if (eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE) == 0) { //dont show calibration status messages if wizard is currently active
	  if (calibration_status() == CALIBRATION_STATUS_ASSEMBLED ||
		  calibration_status() == CALIBRATION_STATUS_UNKNOWN || 
		  calibration_status() == CALIBRATION_STATUS_XYZ_CALIBRATION) {
		  // Reset the babystepping values, so the printer will not move the Z axis up when the babystepping is enabled.
            eeprom_update_word(reinterpret_cast<uint16_t *>(&(EEPROM_Sheets_base->s[(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)))].z_offset)),0);
		  // Show the message.
		  lcd_show_fullscreen_message_and_wait_P(_T(MSG_FOLLOW_CALIBRATION_FLOW));
	  }
	  else if (calibration_status() == CALIBRATION_STATUS_LIVE_ADJUST) {
		  // Show the message.
		  lcd_show_fullscreen_message_and_wait_P(_T(MSG_BABYSTEP_Z_NOT_SET));
		  lcd_update_enable(true);
	  }
	  else if (calibration_status() == CALIBRATION_STATUS_CALIBRATED && temp_cal_active == true && calibration_status_pinda() == false) {
		  //lcd_show_fullscreen_message_and_wait_P(_i("Temperature calibration has not been run yet"));////MSG_PINDA_NOT_CALIBRATED c=20 r=4
		  lcd_update_enable(true);
	  }
	  else if (calibration_status() == CALIBRATION_STATUS_Z_CALIBRATION) {
		  // Show the message.
		  lcd_show_fullscreen_message_and_wait_P(_T(MSG_FOLLOW_Z_CALIBRATION_FLOW));
	  }
  }

#if !defined (DEBUG_DISABLE_FORCE_SELFTEST) && defined (TMC2130)
  if (force_selftest_if_fw_version() && calibration_status() < CALIBRATION_STATUS_ASSEMBLED) {
	  lcd_show_fullscreen_message_and_wait_P(_i("Selftest will be run to calibrate accurate sensorless rehoming."));////MSG_FORCE_SELFTEST c=20 r=8
	  update_current_firmware_version_to_eeprom();
	  lcd_selftest();
  }
#endif //TMC2130 && !DEBUG_DISABLE_FORCE_SELFTEST

  KEEPALIVE_STATE(IN_PROCESS);
#endif //DEBUG_DISABLE_STARTMSGS
  lcd_update_enable(true);
  lcd_clear();
  lcd_update(2);
  // Store the currently running firmware into an eeprom,
  // so the next time the firmware gets updated, it will know from which version it has been updated.
  update_current_firmware_version_to_eeprom();

#ifdef TMC2130
  	tmc2130_home_origin[X_AXIS] = eeprom_read_byte((uint8_t*)EEPROM_TMC2130_HOME_X_ORIGIN);
	tmc2130_home_bsteps[X_AXIS] = eeprom_read_byte((uint8_t*)EEPROM_TMC2130_HOME_X_BSTEPS);
	tmc2130_home_fsteps[X_AXIS] = eeprom_read_byte((uint8_t*)EEPROM_TMC2130_HOME_X_FSTEPS);
	if (tmc2130_home_origin[X_AXIS] == 0xff) tmc2130_home_origin[X_AXIS] = 0;
	if (tmc2130_home_bsteps[X_AXIS] == 0xff) tmc2130_home_bsteps[X_AXIS] = 48;
	if (tmc2130_home_fsteps[X_AXIS] == 0xff) tmc2130_home_fsteps[X_AXIS] = 48;

	tmc2130_home_origin[Y_AXIS] = eeprom_read_byte((uint8_t*)EEPROM_TMC2130_HOME_Y_ORIGIN);
	tmc2130_home_bsteps[Y_AXIS] = eeprom_read_byte((uint8_t*)EEPROM_TMC2130_HOME_Y_BSTEPS);
	tmc2130_home_fsteps[Y_AXIS] = eeprom_read_byte((uint8_t*)EEPROM_TMC2130_HOME_Y_FSTEPS);
	if (tmc2130_home_origin[Y_AXIS] == 0xff) tmc2130_home_origin[Y_AXIS] = 0;
	if (tmc2130_home_bsteps[Y_AXIS] == 0xff) tmc2130_home_bsteps[Y_AXIS] = 48;
	if (tmc2130_home_fsteps[Y_AXIS] == 0xff) tmc2130_home_fsteps[Y_AXIS] = 48;

	tmc2130_home_enabled = eeprom_read_byte((uint8_t*)EEPROM_TMC2130_HOME_ENABLED);
	if (tmc2130_home_enabled == 0xff) tmc2130_home_enabled = 0;
#endif //TMC2130

#ifdef UVLO_SUPPORT
  if (eeprom_read_byte((uint8_t*)EEPROM_UVLO) != 0) { //previous print was terminated by UVLO
/*
	  if (lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_RECOVER_PRINT), false))	recover_print();
	  else {
		  eeprom_update_byte((uint8_t*)EEPROM_UVLO, 0);
		  lcd_update_enable(true);
		  lcd_update(2);
		  lcd_setstatuspgm(_T(WELCOME_MSG));
	  }
*/
      manage_heater(); // Update temperatures 
#ifdef DEBUG_UVLO_AUTOMATIC_RECOVER 
		printf_P(_N("Power panic detected!\nCurrent bed temp:%d\nSaved bed temp:%d\n"), (int)degBed(), eeprom_read_byte((uint8_t*)EEPROM_UVLO_TARGET_BED));
#endif 
     if ( degBed() > ( (float)eeprom_read_byte((uint8_t*)EEPROM_UVLO_TARGET_BED) - AUTOMATIC_UVLO_BED_TEMP_OFFSET) ){ 
          #ifdef DEBUG_UVLO_AUTOMATIC_RECOVER 
        puts_P(_N("Automatic recovery!")); 
          #endif 
         recover_print(1); 
      } 
      else{ 
          #ifdef DEBUG_UVLO_AUTOMATIC_RECOVER 
        puts_P(_N("Normal recovery!")); 
          #endif 
          if ( lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_RECOVER_PRINT), false) ) recover_print(0); 
          else { 
              eeprom_update_byte((uint8_t*)EEPROM_UVLO, 0); 
              lcd_update_enable(true); 
              lcd_update(2); 
              lcd_setstatuspgm(_T(WELCOME_MSG)); 
          } 
      }
  }

  // Only arm the uvlo interrupt _after_ a recovering print has been initialized and
  // the entire state machine initialized.
  setup_uvlo_interrupt();
#endif //UVLO_SUPPORT

  fCheckModeInit();
  fSetMmuMode(mmu_enabled);
  KEEPALIVE_STATE(NOT_BUSY);
#ifdef WATCHDOG
  wdt_enable(WDTO_4S);
#endif //WATCHDOG
}


void trace();

#define CHUNK_SIZE 64 // bytes
#define SAFETY_MARGIN 1
char chunk[CHUNK_SIZE+SAFETY_MARGIN];
int chunkHead = 0;

void serial_read_stream() {

    setAllTargetHotends(0);
    setTargetBed(0);

    lcd_clear();
    lcd_puts_P(PSTR(" Upload in progress"));

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
        }
    }
}

/**
* Output a "busy" message at regular intervals
* while the machine is not accepting commands.
*/
void host_keepalive() {
#ifndef HOST_KEEPALIVE_FEATURE
  return;
#endif //HOST_KEEPALIVE_FEATURE
  if (farm_mode) return;
  long ms = _millis();
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


// The loop() function is called in an endless loop by the Arduino framework from the default main() routine.
// Before loop(), the setup() function is called by the main() routine.
void loop()
{
	KEEPALIVE_STATE(NOT_BUSY);

	if ((usb_printing_counter > 0) && ((_millis()-_usb_timer) > 1000))
	{
		is_usb_printing = true;
		usb_printing_counter--;
		_usb_timer = _millis();
	}
	if (usb_printing_counter == 0)
	{
		is_usb_printing = false;
	}
    if (isPrintPaused && saved_printing_type == PRINTING_TYPE_USB) //keep believing that usb is being printed. Prevents accessing dangerous menus while pausing.
	{
		is_usb_printing = true;
	}
    
#ifdef FANCHECK
    if (fan_check_error && isPrintPaused)
    {
        KEEPALIVE_STATE(PAUSED_FOR_USER);
        host_keepalive(); //prevent timeouts since usb processing is disabled until print is resumed. This is for a crude way of pausing a print on all hosts.
    }
#endif

    if (prusa_sd_card_upload)
    {
        //we read byte-by byte
        serial_read_stream();
    } 
    else 
    {

        get_command();

  #ifdef SDSUPPORT
  card.checkautostart(false);
  #endif
  if(buflen)
  {
    cmdbuffer_front_already_processed = false;
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

    if (! cmdbuffer_front_already_processed && buflen)
    {
      // ptr points to the start of the block currently being processed.
      // The first character in the block is the block type.      
      char *ptr = cmdbuffer + bufindr;
      if (*ptr == CMDBUFFER_CURRENT_TYPE_SDCARD) {
        // To support power panic, move the lenght of the command on the SD card to a planner buffer.
        union {
          struct {
              char lo;
              char hi;
          } lohi;
          uint16_t value;
        } sdlen;
        sdlen.value = 0;
        {
          // This block locks the interrupts globally for 3.25 us,
          // which corresponds to a maximum repeat frequency of 307.69 kHz.
          // This blocking is safe in the context of a 10kHz stepper driver interrupt
          // or a 115200 Bd serial line receive interrupt, which will not trigger faster than 12kHz.
          cli();
          // Reset the command to something, which will be ignored by the power panic routine,
          // so this buffer length will not be counted twice.
          *ptr ++ = CMDBUFFER_CURRENT_TYPE_TO_BE_REMOVED;
          // Extract the current buffer length.
          sdlen.lohi.lo = *ptr ++;
          sdlen.lohi.hi = *ptr;
          // and pass it to the planner queue.
          planner_add_sd_length(sdlen.value);
          sei();
        }
	  }
	  else if((*ptr == CMDBUFFER_CURRENT_TYPE_USB_WITH_LINENR) && !IS_SD_PRINTING){ 
		  
		  cli();
          *ptr ++ = CMDBUFFER_CURRENT_TYPE_TO_BE_REMOVED;
          // and one for each command to previous block in the planner queue.
          planner_add_sd_length(1);
          sei();
	  }
      // Now it is safe to release the already processed command block. If interrupted by the power panic now,
      // this block's SD card length will not be counted twice as its command type has been replaced 
      // by CMDBUFFER_CURRENT_TYPE_TO_BE_REMOVED.
      cmdqueue_pop_front();
    }
	host_keepalive();
  }
}
  //check heater every n milliseconds
  manage_heater();
  isPrintPaused ? manage_inactivity(true) : manage_inactivity(false);
  checkHitEndstops();
  lcd_update(0);
#ifdef TMC2130
	tmc2130_check_overtemp();
	if (tmc2130_sg_crash)
	{
		uint8_t crash = tmc2130_sg_crash;
		tmc2130_sg_crash = 0;
//		crashdet_stop_and_save_print();
		switch (crash)
		{
		case 1: enquecommand_P((PSTR("CRASH_DETECTEDX"))); break;
		case 2: enquecommand_P((PSTR("CRASH_DETECTEDY"))); break;
		case 3: enquecommand_P((PSTR("CRASH_DETECTEDXY"))); break;
		}
	}
#endif //TMC2130
	mmu_loop();
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
  current_position[axis] = base_home_pos(axis) + cs.add_homing[axis];
  min_pos[axis] =          base_min_pos(axis) + cs.add_homing[axis];
  max_pos[axis] =          base_max_pos(axis) + cs.add_homing[axis];
}


inline void set_current_to_destination() { memcpy(current_position, destination, sizeof(current_position)); }
inline void set_destination_to_current() { memcpy(destination, current_position, sizeof(destination)); }

//! @return original feedmultiply
static int setup_for_endstop_move(bool enable_endstops_now = true) {
    saved_feedrate = feedrate;
    int l_feedmultiply = feedmultiply;
    feedmultiply = 100;
    previous_millis_cmd = _millis();
    
    enable_endstops(enable_endstops_now);
    return l_feedmultiply;
}

//! @param original_feedmultiply feedmultiply to restore
static void clean_up_after_endstop_move(int original_feedmultiply) {
#ifdef ENDSTOPS_ONLY_FOR_HOMING
    enable_endstops(false);
#endif
    
    feedrate = saved_feedrate;
    feedmultiply = original_feedmultiply;
    previous_millis_cmd = _millis();
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
    current_position[Z_AXIS] = cs.zprobe_zoffset; // in the lsq we reach here after raising the extruder due to the loop structure

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
    current_position[Z_AXIS] = cs.zprobe_zoffset;

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
    plan_buffer_line_curposXYZE(feedrate/60, active_extruder);
    st_synchronize();

    feedrate = XY_TRAVEL_SPEED;

    current_position[X_AXIS] = x;
    current_position[Y_AXIS] = y;
    plan_buffer_line_curposXYZE(feedrate/60, active_extruder);
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

  SERIAL_PROTOCOLRPGM(_T(MSG_BED));
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
    float newK = code_seen('K') ? code_value_float() : -2;
#ifdef LA_NOCOMPAT
    if (newK >= 0 && newK < 10)
        extruder_advance_K = newK;
    else
        SERIAL_ECHOLNPGM("K out of allowed range!");
#else
    if (newK == 0)
        extruder_advance_K = 0;
    else if (newK == -1)
        la10c_reset();
    else
    {
        newK = la10c_value(newK);
        if (newK < 0)
            SERIAL_ECHOLNPGM("K out of allowed range!");
        else
            extruder_advance_K = newK;
    }
#endif

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


// raise_z_above: slowly raise Z to the requested height
//
// contrarily to a simple move, this function will carefully plan a move
// when the current Z position is unknown. In such cases, stallguard is
// enabled and will prevent prolonged pushing against the Z tops
void raise_z_above(float target, bool plan)
{
    if (current_position[Z_AXIS] >= target)
        return;

    // Z needs raising
    current_position[Z_AXIS] = target;

#if defined(Z_MIN_PIN) && (Z_MIN_PIN > -1) && !defined(DEBUG_DISABLE_ZMINLIMIT)
    bool z_min_endstop = (READ(Z_MIN_PIN) != Z_MIN_ENDSTOP_INVERTING);
#else
    bool z_min_endstop = false;
#endif

    if (axis_known_position[Z_AXIS] || z_min_endstop)
    {
        // current position is known or very low, it's safe to raise Z
        if(plan) plan_buffer_line_curposXYZE(max_feedrate[Z_AXIS], active_extruder);
        return;
    }

    // ensure Z is powered in normal mode to overcome initial load
    enable_z();
    st_synchronize();

    // rely on crashguard to limit damage
    bool z_endstop_enabled = enable_z_endstop(true);
#ifdef TMC2130
    tmc2130_home_enter(Z_AXIS_MASK);
#endif //TMC2130
    plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS] / 60, active_extruder);
    st_synchronize();
#ifdef TMC2130
    if (endstop_z_hit_on_purpose())
    {
        // not necessarily exact, but will avoid further vertical moves
        current_position[Z_AXIS] = max_pos[Z_AXIS];
        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS],
                          current_position[Z_AXIS], current_position[E_AXIS]);
    }
    tmc2130_home_exit();
#endif //TMC2130
    enable_z_endstop(z_endstop_enabled);
}


#ifdef TMC2130
bool calibrate_z_auto()
{
	//lcd_display_message_fullscreen_P(_T(MSG_CALIBRATE_Z_AUTO));
	lcd_clear();
	lcd_puts_at_P(0, 1, _T(MSG_CALIBRATE_Z_AUTO));
	bool endstops_enabled = enable_endstops(true);
	int axis_up_dir = -home_dir(Z_AXIS);
	tmc2130_home_enter(Z_AXIS_MASK);
	current_position[Z_AXIS] = 0;
	plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
	set_destination_to_current();
	destination[Z_AXIS] += (1.1 * max_length(Z_AXIS) * axis_up_dir);
	feedrate = homing_feedrate[Z_AXIS];
	plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
	st_synchronize();
	//	current_position[axis] = 0;
	//	plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
	tmc2130_home_exit();
	enable_endstops(false);
	current_position[Z_AXIS] = 0;
	plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
	set_destination_to_current();
	destination[Z_AXIS] += 10 * axis_up_dir; //10mm up
	feedrate = homing_feedrate[Z_AXIS] / 2;
	plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
	st_synchronize();
	enable_endstops(endstops_enabled);
	if (PRINTER_TYPE == PRINTER_MK3) {
		current_position[Z_AXIS] = Z_MAX_POS + 2.0;
	}
	else {
		current_position[Z_AXIS] = Z_MAX_POS + 9.0;
	}
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
	return true;
}
#endif //TMC2130

#ifdef TMC2130
void homeaxis(int axis, uint8_t cnt, uint8_t* pstep)
#else
void homeaxis(int axis, uint8_t cnt)
#endif //TMC2130
{
	bool endstops_enabled  = enable_endstops(true); //RP: endstops should be allways enabled durring homing
#define HOMEAXIS_DO(LETTER) \
((LETTER##_MIN_PIN > -1 && LETTER##_HOME_DIR==-1) || (LETTER##_MAX_PIN > -1 && LETTER##_HOME_DIR==1))
    if ((axis==X_AXIS)?HOMEAXIS_DO(X):(axis==Y_AXIS)?HOMEAXIS_DO(Y):0)
	{
        int axis_home_dir = home_dir(axis);
        feedrate = homing_feedrate[axis];

#ifdef TMC2130
    	tmc2130_home_enter(X_AXIS_MASK << axis);
#endif //TMC2130


        // Move away a bit, so that the print head does not touch the end position,
        // and the following movement to endstop has a chance to achieve the required velocity
        // for the stall guard to work.
        current_position[axis] = 0;
        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		set_destination_to_current();
//        destination[axis] = 11.f;
        destination[axis] = -3.f * axis_home_dir;
        plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
        st_synchronize();
        // Move away from the possible collision with opposite endstop with the collision detection disabled.
        endstops_hit_on_purpose();
        enable_endstops(false);
        current_position[axis] = 0;
        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        destination[axis] = 1. * axis_home_dir;
        plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
        st_synchronize();
        // Now continue to move up to the left end stop with the collision detection enabled.
        enable_endstops(true);
        destination[axis] = 1.1 * axis_home_dir * max_length(axis);
        plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
        st_synchronize();
		for (uint8_t i = 0; i < cnt; i++)
		{
			// Move away from the collision to a known distance from the left end stop with the collision detection disabled.
			endstops_hit_on_purpose();
			enable_endstops(false);
			current_position[axis] = 0;
			plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
			destination[axis] = -10.f * axis_home_dir;
			plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
			st_synchronize();
			endstops_hit_on_purpose();
			// Now move left up to the collision, this time with a repeatable velocity.
			enable_endstops(true);
			destination[axis] = 11.f * axis_home_dir;
#ifdef TMC2130
			feedrate = homing_feedrate[axis];
#else //TMC2130
			feedrate = homing_feedrate[axis] / 2;
#endif //TMC2130
			plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
			st_synchronize();
#ifdef TMC2130
			uint16_t mscnt = tmc2130_rd_MSCNT(axis);
			if (pstep) pstep[i] = mscnt >> 4;
			printf_P(PSTR("%3d step=%2d mscnt=%4d\n"), i, mscnt >> 4, mscnt);
#endif //TMC2130
		}
		endstops_hit_on_purpose();
		enable_endstops(false);

#ifdef TMC2130
		uint8_t orig = tmc2130_home_origin[axis];
		uint8_t back = tmc2130_home_bsteps[axis];
		if (tmc2130_home_enabled && (orig <= 63))
		{
			tmc2130_goto_step(axis, orig, 2, 1000, tmc2130_get_res(axis));
			if (back > 0)
				tmc2130_do_steps(axis, back, -axis_home_dir, 1000);
		}
		else
			tmc2130_do_steps(axis, 8, -axis_home_dir, 1000);
		tmc2130_home_exit();
#endif //TMC2130

        axis_is_at_home(axis);
        axis_known_position[axis] = true;
        // Move from minimum
#ifdef TMC2130
        float dist = - axis_home_dir * 0.01f * tmc2130_home_fsteps[axis];
#else //TMC2130
        float dist = - axis_home_dir * 0.01f * 64;
#endif //TMC2130
        current_position[axis] -= dist;
        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        current_position[axis] += dist;
        destination[axis] = current_position[axis];
        plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], 0.5f*feedrate/60, active_extruder);
        st_synchronize();

   		feedrate = 0.0;
    }
    else if ((axis==Z_AXIS)?HOMEAXIS_DO(Z):0)
	{
#ifdef TMC2130
		FORCE_HIGH_POWER_START;
#endif	
        int axis_home_dir = home_dir(axis);
        current_position[axis] = 0;
        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        destination[axis] = 1.5 * max_length(axis) * axis_home_dir;
        feedrate = homing_feedrate[axis];
        plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
        st_synchronize();
#ifdef TMC2130
		if (READ(Z_TMC2130_DIAG) != 0) { //Z crash
			FORCE_HIGH_POWER_END;
			kill(_T(MSG_BED_LEVELING_FAILED_POINT_LOW));
			return; 
		}
#endif //TMC2130
        current_position[axis] = 0;
        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        destination[axis] = -home_retract_mm(axis) * axis_home_dir;
        plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
        st_synchronize();
        destination[axis] = 2*home_retract_mm(axis) * axis_home_dir;
        feedrate = homing_feedrate[axis]/2 ;
        plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
        st_synchronize();
#ifdef TMC2130
		if (READ(Z_TMC2130_DIAG) != 0) { //Z crash
			FORCE_HIGH_POWER_END;
			kill(_T(MSG_BED_LEVELING_FAILED_POINT_LOW));
			return; 
		}
#endif //TMC2130
        axis_is_at_home(axis);
        destination[axis] = current_position[axis];
        feedrate = 0.0;
        endstops_hit_on_purpose();
        axis_known_position[axis] = true;
#ifdef TMC2130
		FORCE_HIGH_POWER_END;
#endif	
    }
    enable_endstops(endstops_enabled);
}

/**/
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
  previous_millis_cmd = _millis();
}

#ifdef FWRETRACT
  void retract(bool retracting, bool swapretract = false) {
    if(retracting && !retracted[active_extruder]) {
      destination[X_AXIS]=current_position[X_AXIS];
      destination[Y_AXIS]=current_position[Y_AXIS];
      destination[Z_AXIS]=current_position[Z_AXIS];
      destination[E_AXIS]=current_position[E_AXIS];
      current_position[E_AXIS]+=(swapretract?retract_length_swap:cs.retract_length)*float(extrudemultiply)*0.01f;
      plan_set_e_position(current_position[E_AXIS]);
      float oldFeedrate = feedrate;
      feedrate=cs.retract_feedrate*60;
      retracted[active_extruder]=true;
      prepare_move();
      current_position[Z_AXIS]-=cs.retract_zlift;
      plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
      prepare_move();
      feedrate = oldFeedrate;
    } else if(!retracting && retracted[active_extruder]) {
      destination[X_AXIS]=current_position[X_AXIS];
      destination[Y_AXIS]=current_position[Y_AXIS];
      destination[Z_AXIS]=current_position[Z_AXIS];
      destination[E_AXIS]=current_position[E_AXIS];
      current_position[Z_AXIS]+=cs.retract_zlift;
      plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
      current_position[E_AXIS]-=(swapretract?(retract_length_swap+retract_recover_length_swap):(cs.retract_length+cs.retract_recover_length))*float(extrudemultiply)*0.01f;
      plan_set_e_position(current_position[E_AXIS]);
      float oldFeedrate = feedrate;
      feedrate=cs.retract_recover_feedrate*60;
      retracted[active_extruder]=false;
      prepare_move();
      feedrate = oldFeedrate;
    }
  } //retract
#endif //FWRETRACT

void trace() {
    Sound_MakeCustom(25,440,true);
}
/*
void ramming() {
//	  float tmp[4] = DEFAULT_MAX_FEEDRATE;
	if (current_temperature[0] < 230) {
		//PLA

		max_feedrate[E_AXIS] = 50;
		//current_position[E_AXIS] -= 8;
		//plan_buffer_line_curposXYZE(2100 / 60, active_extruder);
		//current_position[E_AXIS] += 8;
		//plan_buffer_line_curposXYZE(2100 / 60, active_extruder);
		current_position[E_AXIS] += 5.4;
		plan_buffer_line_curposXYZE(2800 / 60, active_extruder);
		current_position[E_AXIS] += 3.2;
		plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
		current_position[E_AXIS] += 3;
		plan_buffer_line_curposXYZE(3400 / 60, active_extruder);
		st_synchronize();
		max_feedrate[E_AXIS] = 80;
		current_position[E_AXIS] -= 82;
		plan_buffer_line_curposXYZE(9500 / 60, active_extruder);
		max_feedrate[E_AXIS] = 50;//tmp[E_AXIS];
		current_position[E_AXIS] -= 20;
		plan_buffer_line_curposXYZE(1200 / 60, active_extruder);
		current_position[E_AXIS] += 5;
		plan_buffer_line_curposXYZE(400 / 60, active_extruder);
		current_position[E_AXIS] += 5;
		plan_buffer_line_curposXYZE(600 / 60, active_extruder);
		current_position[E_AXIS] -= 10;
		st_synchronize();
		plan_buffer_line_curposXYZE(600 / 60, active_extruder);
		current_position[E_AXIS] += 10;
		plan_buffer_line_curposXYZE(600 / 60, active_extruder);
		current_position[E_AXIS] -= 10;
		plan_buffer_line_curposXYZE(800 / 60, active_extruder);
		current_position[E_AXIS] += 10;
		plan_buffer_line_curposXYZE(800 / 60, active_extruder);
		current_position[E_AXIS] -= 10;
		plan_buffer_line_curposXYZE(800 / 60, active_extruder);
		st_synchronize();
	}
	else {
		//ABS
		max_feedrate[E_AXIS] = 50;
		//current_position[E_AXIS] -= 8;
		//plan_buffer_line_curposXYZE(2100 / 60, active_extruder);
		//current_position[E_AXIS] += 8;
		//plan_buffer_line_curposXYZE(2100 / 60, active_extruder);
		current_position[E_AXIS] += 3.1;
		plan_buffer_line_curposXYZE(2000 / 60, active_extruder);
		current_position[E_AXIS] += 3.1;
		plan_buffer_line_curposXYZE(2500 / 60, active_extruder);
		current_position[E_AXIS] += 4;
		plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
		st_synchronize();
		//current_position[X_AXIS] += 23; //delay
		//plan_buffer_line_curposXYZE(600/60, active_extruder); //delay
		//current_position[X_AXIS] -= 23; //delay
		//plan_buffer_line_curposXYZE(600/60, active_extruder); //delay
		_delay(4700);
		max_feedrate[E_AXIS] = 80;
		current_position[E_AXIS] -= 92;
		plan_buffer_line_curposXYZE(9900 / 60, active_extruder);
		max_feedrate[E_AXIS] = 50;//tmp[E_AXIS];
		current_position[E_AXIS] -= 5;
		plan_buffer_line_curposXYZE(800 / 60, active_extruder);
		current_position[E_AXIS] += 5;
		plan_buffer_line_curposXYZE(400 / 60, active_extruder);
		current_position[E_AXIS] -= 5;
		plan_buffer_line_curposXYZE(600 / 60, active_extruder);
		st_synchronize();
		current_position[E_AXIS] += 5;
		plan_buffer_line_curposXYZE(600 / 60, active_extruder);
		current_position[E_AXIS] -= 5;
		plan_buffer_line_curposXYZE(600 / 60, active_extruder);
		current_position[E_AXIS] += 5;
		plan_buffer_line_curposXYZE(600 / 60, active_extruder);
		current_position[E_AXIS] -= 5;
		plan_buffer_line_curposXYZE(600 / 60, active_extruder);
		st_synchronize();

	}
  }
*/

#ifdef TMC2130
void force_high_power_mode(bool start_high_power_section) {
#ifdef PSU_Delta
	if (start_high_power_section == true) enable_force_z();
#endif //PSU_Delta
	uint8_t silent;
	silent = eeprom_read_byte((uint8_t*)EEPROM_SILENT);
	if (silent == 1) {
		//we are in silent mode, set to normal mode to enable crash detection

    // Wait for the planner queue to drain and for the stepper timer routine to reach an idle state.
		st_synchronize();
		cli();
		tmc2130_mode = (start_high_power_section == true) ? TMC2130_MODE_NORMAL : TMC2130_MODE_SILENT;
		update_mode_profile();
		tmc2130_init();
    // We may have missed a stepper timer interrupt due to the time spent in the tmc2130_init() routine.
    // Be safe than sorry, reset the stepper timer before re-enabling interrupts.
    st_reset_timer();
		sei();
	}
}
#endif //TMC2130

#ifdef TMC2130
static void gcode_G28(bool home_x_axis, long home_x_value, bool home_y_axis, long home_y_value, bool home_z_axis, long home_z_value, bool calib, bool without_mbl)
#else
static void gcode_G28(bool home_x_axis, long home_x_value, bool home_y_axis, long home_y_value, bool home_z_axis, long home_z_value, bool without_mbl)
#endif //TMC2130
{
	st_synchronize();

#if 0
	SERIAL_ECHOPGM("G28, initial ");  print_world_coordinates();
	SERIAL_ECHOPGM("G28, initial ");  print_physical_coordinates();
#endif

	// Flag for the display update routine and to disable the print cancelation during homing.
	homing_flag = true;

	// Which axes should be homed?
	bool home_x = home_x_axis;
	bool home_y = home_y_axis;
	bool home_z = home_z_axis;

	// Either all X,Y,Z codes are present, or none of them.
	bool home_all_axes = home_x == home_y && home_x == home_z;
	if (home_all_axes)
		// No X/Y/Z code provided means to home all axes.
		home_x = home_y = home_z = true;

	//if we are homing all axes, first move z higher to protect heatbed/steel sheet
	if (home_all_axes) {
        raise_z_above(MESH_HOME_Z_SEARCH);
		st_synchronize();
	}
#ifdef ENABLE_AUTO_BED_LEVELING
      plan_bed_level_matrix.set_to_identity();  //Reset the plane ("erase" all leveling data)
#endif //ENABLE_AUTO_BED_LEVELING
            
      // Reset world2machine_rotation_and_skew and world2machine_shift, therefore
      // the planner will not perform any adjustments in the XY plane. 
      // Wait for the motors to stop and update the current position with the absolute values.
      world2machine_revert_to_uncorrected();

      // For mesh bed leveling deactivate the matrix temporarily.
      // It is necessary to disable the bed leveling for the X and Y homing moves, so that the move is performed
      // in a single axis only.
      // In case of re-homing the X or Y axes only, the mesh bed leveling is restored after G28.
#ifdef MESH_BED_LEVELING
      uint8_t mbl_was_active = mbl.active;
      mbl.active = 0;
      current_position[Z_AXIS] = st_get_position_mm(Z_AXIS);
#endif

      // Reset baby stepping to zero, if the babystepping has already been loaded before. The babystepsTodo value will be
      // consumed during the first movements following this statement.
      if (home_z)
        babystep_undo();

      saved_feedrate = feedrate;
      int l_feedmultiply = feedmultiply;
      feedmultiply = 100;
      previous_millis_cmd = _millis();

      enable_endstops(true);

      memcpy(destination, current_position, sizeof(destination));
      feedrate = 0.0;

      #if Z_HOME_DIR > 0                      // If homing away from BED do Z first
      if(home_z)
        homeaxis(Z_AXIS);
      #endif

      #ifdef QUICK_HOME
      // In the quick mode, if both x and y are to be homed, a diagonal move will be performed initially.
      if(home_x && home_y)  //first diagonal move
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

#ifdef TMC2130	 
      if(home_x)
	  {
		if (!calib)
			homeaxis(X_AXIS);
		else
			tmc2130_home_calibrate(X_AXIS);
	  }

      if(home_y)
	  {
		if (!calib)
	        homeaxis(Y_AXIS);
		else
			tmc2130_home_calibrate(Y_AXIS);
	  }
#else //TMC2130
      if(home_x) homeaxis(X_AXIS);
      if(home_y) homeaxis(Y_AXIS);
#endif //TMC2130


      if(home_x_axis && home_x_value != 0)
        current_position[X_AXIS]=home_x_value+cs.add_homing[X_AXIS];

      if(home_y_axis && home_y_value != 0)
        current_position[Y_AXIS]=home_y_value+cs.add_homing[Y_AXIS];

      #if Z_HOME_DIR < 0                      // If homing towards BED do Z last
        #ifndef Z_SAFE_HOMING
          if(home_z) {
            #if defined (Z_RAISE_BEFORE_HOMING) && (Z_RAISE_BEFORE_HOMING > 0)
              raise_z_above(Z_RAISE_BEFORE_HOMING);
              st_synchronize();
            #endif // defined (Z_RAISE_BEFORE_HOMING) && (Z_RAISE_BEFORE_HOMING > 0)
            #if (defined(MESH_BED_LEVELING) && !defined(MK1BP))  // If Mesh bed leveling, move X&Y to safe position for home
              raise_z_above(MESH_HOME_Z_SEARCH);
              st_synchronize();
              if (!axis_known_position[X_AXIS]) homeaxis(X_AXIS);
              if (!axis_known_position[Y_AXIS]) homeaxis(Y_AXIS);
              // 1st mesh bed leveling measurement point, corrected.
              world2machine_initialize();
              world2machine(pgm_read_float(bed_ref_points_4), pgm_read_float(bed_ref_points_4+1), destination[X_AXIS], destination[Y_AXIS]);
              world2machine_reset();
              if (destination[Y_AXIS] < Y_MIN_POS)
                  destination[Y_AXIS] = Y_MIN_POS;
              feedrate = homing_feedrate[X_AXIS] / 20;
              enable_endstops(false);
#ifdef DEBUG_BUILD
              SERIAL_ECHOLNPGM("plan_set_position()");
              MYSERIAL.println(current_position[X_AXIS]);MYSERIAL.println(current_position[Y_AXIS]);
              MYSERIAL.println(current_position[Z_AXIS]);MYSERIAL.println(current_position[E_AXIS]);
#endif
              plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
#ifdef DEBUG_BUILD
              SERIAL_ECHOLNPGM("plan_buffer_line()");
              MYSERIAL.println(destination[X_AXIS]);MYSERIAL.println(destination[Y_AXIS]);
              MYSERIAL.println(destination[Z_AXIS]);MYSERIAL.println(destination[E_AXIS]);
              MYSERIAL.println(feedrate);MYSERIAL.println(active_extruder);
#endif
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
          if(home_all_axes) {
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
          if(home_z) {
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

      if(home_z_axis && home_z_value != 0)
        current_position[Z_AXIS]=home_z_value+cs.add_homing[Z_AXIS];
      #ifdef ENABLE_AUTO_BED_LEVELING
        if(home_z)
          current_position[Z_AXIS] += cs.zprobe_zoffset;  //Add Z_Probe offset (the distance is negative)
      #endif
      
      // Set the planner and stepper routine positions.
      // At this point the mesh bed leveling and world2machine corrections are disabled and current_position
      // contains the machine coordinates.
      plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);

      #ifdef ENDSTOPS_ONLY_FOR_HOMING
        enable_endstops(false);
      #endif

      feedrate = saved_feedrate;
      feedmultiply = l_feedmultiply;
      previous_millis_cmd = _millis();
      endstops_hit_on_purpose();
#ifndef MESH_BED_LEVELING
//-// Oct 2019 :: this part of code is (from) now probably un-compilable
      // If MESH_BED_LEVELING is not active, then it is the original Prusa i3.
      // Offer the user to load the baby step value, which has been adjusted at the previous print session.
      if(card.sdprinting && eeprom_read_word((uint16_t *)EEPROM_BABYSTEP_Z))
          lcd_adjust_z();
#endif

    // Load the machine correction matrix
    world2machine_initialize();
    // and correct the current_position XY axes to match the transformed coordinate system.
    world2machine_update_current();

#if (defined(MESH_BED_LEVELING) && !defined(MK1BP))
	if (home_x_axis || home_y_axis || without_mbl || home_z_axis)
		{
      if (! home_z && mbl_was_active) {
        // Re-enable the mesh bed leveling if only the X and Y axes were re-homed.
        mbl.active = true;
        // and re-adjust the current logical Z axis with the bed leveling offset applicable at the current XY position.
        current_position[Z_AXIS] -= mbl.get_z(st_get_position_mm(X_AXIS), st_get_position_mm(Y_AXIS));
      }
		}
	else
		{
			st_synchronize();
			homing_flag = false;
	  }
#endif

	  if (farm_mode) { prusa_statistics(20); };

	  homing_flag = false;
#if 0
      SERIAL_ECHOPGM("G28, final ");  print_world_coordinates();
      SERIAL_ECHOPGM("G28, final ");  print_physical_coordinates();
      SERIAL_ECHOPGM("G28, final ");  print_mesh_bed_leveling_table();
#endif
}

static void gcode_G28(bool home_x_axis, bool home_y_axis, bool home_z_axis)
{
#ifdef TMC2130
    gcode_G28(home_x_axis, 0, home_y_axis, 0, home_z_axis, 0, false, true);
#else
    gcode_G28(home_x_axis, 0, home_y_axis, 0, home_z_axis, 0, true);
#endif //TMC2130
}

void adjust_bed_reset()
{
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_VALID, 1);
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_LEFT, 0);
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_RIGHT, 0);
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_FRONT, 0);
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_REAR, 0);
}

//! @brief Calibrate XYZ
//! @param onlyZ if true, calibrate only Z axis
//! @param verbosity_level
//! @retval true Succeeded
//! @retval false Failed
bool gcode_M45(bool onlyZ, int8_t verbosity_level)
{
	bool final_result = false;
	#ifdef TMC2130
	FORCE_HIGH_POWER_START;
	#endif // TMC2130
    
    FORCE_BL_ON_START;
	
    // Only Z calibration?
	if (!onlyZ)
	{
		setTargetBed(0);
		setAllTargetHotends(0);
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

	// Home in the XY plane.
	//set_destination_to_current();
	int l_feedmultiply = setup_for_endstop_move();
	lcd_display_message_fullscreen_P(_T(MSG_AUTO_HOME));
	home_xy();

	enable_endstops(false);
	current_position[X_AXIS] += 5;
	current_position[Y_AXIS] += 5;
	plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS] / 40, active_extruder);
	st_synchronize();

	// Let the user move the Z axes up to the end stoppers.
#ifdef TMC2130
	if (calibrate_z_auto())
	{
#else //TMC2130
	if (lcd_calibrate_z_end_stop_manual(onlyZ))
	{
#endif //TMC2130
		
		lcd_show_fullscreen_message_and_wait_P(_T(MSG_CONFIRM_NOZZLE_CLEAN));
		if(onlyZ){
			lcd_display_message_fullscreen_P(_T(MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1));
			lcd_set_cursor(0, 3);
			lcd_print(1);
			lcd_puts_P(_T(MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2));
		}else{
			//lcd_show_fullscreen_message_and_wait_P(_T(MSG_PAPER));
			lcd_display_message_fullscreen_P(_T(MSG_FIND_BED_OFFSET_AND_SKEW_LINE1));
			lcd_set_cursor(0, 2);
			lcd_print(1);
			lcd_puts_P(_T(MSG_FIND_BED_OFFSET_AND_SKEW_LINE2));
		}

		refresh_cmd_timeout();
		#ifndef STEEL_SHEET
		if (((degHotend(0) > MAX_HOTEND_TEMP_CALIBRATION) || (degBed() > MAX_BED_TEMP_CALIBRATION)) && (!onlyZ))
		{
			lcd_wait_for_cool_down();
		}
		#endif //STEEL_SHEET
		if(!onlyZ)
		{
			KEEPALIVE_STATE(PAUSED_FOR_USER);
			#ifdef STEEL_SHEET
			bool result = lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_STEEL_SHEET_CHECK), false, false);
			if(result) lcd_show_fullscreen_message_and_wait_P(_T(MSG_REMOVE_STEEL_SHEET));
			#endif //STEEL_SHEET
		    lcd_show_fullscreen_message_and_wait_P(_T(MSG_PAPER));
			KEEPALIVE_STATE(IN_HANDLER);
			lcd_display_message_fullscreen_P(_T(MSG_FIND_BED_OFFSET_AND_SKEW_LINE1));
			lcd_set_cursor(0, 2);
			lcd_print(1);
			lcd_puts_P(_T(MSG_FIND_BED_OFFSET_AND_SKEW_LINE2));
		}
			
		bool endstops_enabled  = enable_endstops(false);
        current_position[Z_AXIS] -= 1; //move 1mm down with disabled endstop
        plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS] / 40, active_extruder);
        st_synchronize();

		// Move the print head close to the bed.
		current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;

		enable_endstops(true);
#ifdef TMC2130
		tmc2130_home_enter(Z_AXIS_MASK);
#endif //TMC2130

		plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS] / 40, active_extruder);

		st_synchronize();
#ifdef TMC2130
		tmc2130_home_exit();
#endif //TMC2130
		enable_endstops(endstops_enabled);

		if ((st_get_position_mm(Z_AXIS) <= (MESH_HOME_Z_SEARCH + HOME_Z_SEARCH_THRESHOLD)) &&
		    (st_get_position_mm(Z_AXIS) >= (MESH_HOME_Z_SEARCH - HOME_Z_SEARCH_THRESHOLD)))
		{
			if (onlyZ)
			{
				clean_up_after_endstop_move(l_feedmultiply);
				// Z only calibration.
				// Load the machine correction matrix
				world2machine_initialize();
				// and correct the current_position to match the transformed coordinate system.
				world2machine_update_current();
				//FIXME
				bool result = sample_mesh_and_store_reference();
				if (result)
				{
					if (calibration_status() == CALIBRATION_STATUS_Z_CALIBRATION)
						// Shipped, the nozzle height has been set already. The user can start printing now.
						calibration_status_store(CALIBRATION_STATUS_CALIBRATED);
						final_result = true;
					// babystep_apply();
				}
			}
			else
			{
				// Reset the baby step value and the baby step applied flag.
				calibration_status_store(CALIBRATION_STATUS_XYZ_CALIBRATION);
                    eeprom_update_word(reinterpret_cast<uint16_t *>(&(EEPROM_Sheets_base->s[(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)))].z_offset)),0);
				// Complete XYZ calibration.
				uint8_t point_too_far_mask = 0;
				BedSkewOffsetDetectionResultType result = find_bed_offset_and_skew(verbosity_level, point_too_far_mask);
				clean_up_after_endstop_move(l_feedmultiply);
				// Print head up.
				current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
				plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS] / 40, active_extruder);
				st_synchronize();
//#ifndef NEW_XYZCAL
				if (result >= 0)
				{
					#ifdef HEATBED_V2
					sample_z();
					#else //HEATBED_V2
					point_too_far_mask = 0;
					// Second half: The fine adjustment.
					// Let the planner use the uncorrected coordinates.
					mbl.reset();
					world2machine_reset();
					// Home in the XY plane.
					int l_feedmultiply = setup_for_endstop_move();
					home_xy();
					result = improve_bed_offset_and_skew(1, verbosity_level, point_too_far_mask);
					clean_up_after_endstop_move(l_feedmultiply);
					// Print head up.
					current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
					plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS] / 40, active_extruder);
					st_synchronize();
					// if (result >= 0) babystep_apply();					
					#endif //HEATBED_V2
				}
//#endif //NEW_XYZCAL
				lcd_update_enable(true);
				lcd_update(2);

				lcd_bed_calibration_show_result(result, point_too_far_mask);
				if (result >= 0)
				{
					// Calibration valid, the machine should be able to print. Advise the user to run the V2Calibration.gcode.
					calibration_status_store(CALIBRATION_STATUS_LIVE_ADJUST);
					if (eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE) != 1) lcd_show_fullscreen_message_and_wait_P(_T(MSG_BABYSTEP_Z_NOT_SET));
					final_result = true;
				}
			}
#ifdef TMC2130
			tmc2130_home_exit();
#endif
		}
		else
		{
			lcd_show_fullscreen_message_and_wait_P(PSTR("Calibration failed! Check the axes and run again."));
			final_result = false;
		}
	}
	else
	{
		// Timeouted.
	}
	lcd_update_enable(true);
#ifdef TMC2130
	FORCE_HIGH_POWER_END;
#endif // TMC2130
    
    FORCE_BL_ON_END;
    
	return final_result;
}

void gcode_M114()
{
	SERIAL_PROTOCOLPGM("X:");
	SERIAL_PROTOCOL(current_position[X_AXIS]);
	SERIAL_PROTOCOLPGM(" Y:");
	SERIAL_PROTOCOL(current_position[Y_AXIS]);
	SERIAL_PROTOCOLPGM(" Z:");
	SERIAL_PROTOCOL(current_position[Z_AXIS]);
	SERIAL_PROTOCOLPGM(" E:");
	SERIAL_PROTOCOL(current_position[E_AXIS]);

	SERIAL_PROTOCOLRPGM(_n(" Count X: "));////MSG_COUNT_X
	SERIAL_PROTOCOL(float(st_get_position(X_AXIS)) / cs.axis_steps_per_unit[X_AXIS]);
	SERIAL_PROTOCOLPGM(" Y:");
	SERIAL_PROTOCOL(float(st_get_position(Y_AXIS)) / cs.axis_steps_per_unit[Y_AXIS]);
	SERIAL_PROTOCOLPGM(" Z:");
	SERIAL_PROTOCOL(float(st_get_position(Z_AXIS)) / cs.axis_steps_per_unit[Z_AXIS]);
	SERIAL_PROTOCOLPGM(" E:");
	SERIAL_PROTOCOL(float(st_get_position(E_AXIS)) / cs.axis_steps_per_unit[E_AXIS]);

	SERIAL_PROTOCOLLN("");
}

//! extracted code to compute z_shift for M600 in case of filament change operation 
//! requested from fsensors.
//! The function ensures, that the printhead lifts to at least 25mm above the heat bed
//! unlike the previous implementation, which was adding 25mm even when the head was
//! printing at e.g. 24mm height.
//! A safety margin of FILAMENTCHANGE_ZADD is added in all cases to avoid touching
//! the printout.
//! This function is templated to enable fast change of computation data type.
//! @return new z_shift value
template<typename T>
static T gcode_M600_filament_change_z_shift()
{
#ifdef FILAMENTCHANGE_ZADD
	static_assert(Z_MAX_POS < (255 - FILAMENTCHANGE_ZADD), "Z-range too high, change the T type from uint8_t to uint16_t");
	// avoid floating point arithmetics when not necessary - results in shorter code
	T ztmp = T( current_position[Z_AXIS] );
	T z_shift = 0;
	if(ztmp < T(25)){
		z_shift = T(25) - ztmp; // make sure to be at least 25mm above the heat bed
	} 
	return z_shift + T(FILAMENTCHANGE_ZADD); // always move above printout
#else
	return T(0);
#endif
}	

static void gcode_M600(bool automatic, float x_position, float y_position, float z_shift, float e_shift, float /*e_shift_late*/)
{
    st_synchronize();
    float lastpos[4];

    if (farm_mode)
    {
        prusa_statistics(22);
    }

    //First backup current position and settings
    int feedmultiplyBckp = feedmultiply;
    float HotendTempBckp = degTargetHotend(active_extruder);
    int fanSpeedBckp = fanSpeed;

    lastpos[X_AXIS] = current_position[X_AXIS];
    lastpos[Y_AXIS] = current_position[Y_AXIS];
    lastpos[Z_AXIS] = current_position[Z_AXIS];
    lastpos[E_AXIS] = current_position[E_AXIS];

    //Retract E
    current_position[E_AXIS] += e_shift;
    plan_buffer_line_curposXYZE(FILAMENTCHANGE_RFEED, active_extruder);
    st_synchronize();

    //Lift Z
    current_position[Z_AXIS] += z_shift;
    plan_buffer_line_curposXYZE(FILAMENTCHANGE_ZFEED, active_extruder);
    st_synchronize();

    //Move XY to side
    current_position[X_AXIS] = x_position;
    current_position[Y_AXIS] = y_position;
    plan_buffer_line_curposXYZE(FILAMENTCHANGE_XYFEED, active_extruder);
    st_synchronize();

    //Beep, manage nozzle heater and wait for user to start unload filament
    if(!mmu_enabled) M600_wait_for_user(HotendTempBckp);

    lcd_change_fil_state = 0;

    // Unload filament
    if (mmu_enabled) extr_unload();	//unload just current filament for multimaterial printers (used also in M702)
    else unload_filament(); //unload filament for single material (used also in M702)
    //finish moves
    st_synchronize();

    if (!mmu_enabled)
    {
        KEEPALIVE_STATE(PAUSED_FOR_USER);
        lcd_change_fil_state = lcd_show_fullscreen_message_yes_no_and_wait_P(_i("Was filament unload successful?"),
                false, true); ////MSG_UNLOAD_SUCCESSFUL c=20 r=2
        if (lcd_change_fil_state == 0)
        {
			lcd_clear();
			lcd_set_cursor(0, 2);
			lcd_puts_P(_T(MSG_PLEASE_WAIT));
			current_position[X_AXIS] -= 100;
			plan_buffer_line_curposXYZE(FILAMENTCHANGE_XYFEED, active_extruder);
			st_synchronize();
			lcd_show_fullscreen_message_and_wait_P(_i("Please open idler and remove filament manually."));////MSG_CHECK_IDLER c=20 r=4
        }
    }

    if (mmu_enabled)
    {
        if (!automatic) {
            if (saved_printing) mmu_eject_filament(mmu_extruder, false); //if M600 was invoked by filament senzor (FINDA) eject filament so user can easily remove it
            mmu_M600_wait_and_beep();
            if (saved_printing) {

                lcd_clear();
                lcd_set_cursor(0, 2);
                lcd_puts_P(_T(MSG_PLEASE_WAIT));

                mmu_command(MmuCmd::R0);
                manage_response(false, false);
            }
        }
        mmu_M600_load_filament(automatic, HotendTempBckp);
    }
    else
        M600_load_filament();

    if (!automatic) M600_check_state(HotendTempBckp);

		lcd_update_enable(true);

    //Not let's go back to print
    fanSpeed = fanSpeedBckp;

    //Feed a little of filament to stabilize pressure
    if (!automatic)
    {
        current_position[E_AXIS] += FILAMENTCHANGE_RECFEED;
        plan_buffer_line_curposXYZE(FILAMENTCHANGE_EXFEED, active_extruder);
    }

    //Move XY back
    plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS],
            FILAMENTCHANGE_XYFEED, active_extruder);
    st_synchronize();
    //Move Z back
    plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], lastpos[Z_AXIS], current_position[E_AXIS],
            FILAMENTCHANGE_ZFEED, active_extruder);
    st_synchronize();

    //Set E position to original
    plan_set_e_position(lastpos[E_AXIS]);

    memcpy(current_position, lastpos, sizeof(lastpos));
    memcpy(destination, current_position, sizeof(current_position));

    //Recover feed rate
    feedmultiply = feedmultiplyBckp;
    char cmd[9];
    sprintf_P(cmd, PSTR("M220 S%i"), feedmultiplyBckp);
    enquecommand(cmd);

#ifdef IR_SENSOR
	//this will set fsensor_watch_autoload to correct value and prevent possible M701 gcode enqueuing when M600 is finished
	fsensor_check_autoload();
#endif //IR_SENSOR

    lcd_setstatuspgm(_T(WELCOME_MSG));
    custom_message_type = CustomMsg::Status;
}

void gcode_M701()
{
	printf_P(PSTR("gcode_M701 begin\n"));

	if (farm_mode)
	{
		prusa_statistics(22);
	}

	if (mmu_enabled) 
	{
		extr_adj(tmp_extruder);//loads current extruder
		mmu_extruder = tmp_extruder;
	}
	else
	{
		enable_z();
		custom_message_type = CustomMsg::FilamentLoading;

#ifdef FSENSOR_QUALITY
		fsensor_oq_meassure_start(40);
#endif //FSENSOR_QUALITY

		lcd_setstatuspgm(_T(MSG_LOADING_FILAMENT));
		current_position[E_AXIS] += 40;
		plan_buffer_line_curposXYZE(400 / 60, active_extruder); //fast sequence
		st_synchronize();

        raise_z_above(MIN_Z_FOR_LOAD, false);
		current_position[E_AXIS] += 30;
		plan_buffer_line_curposXYZE(400 / 60, active_extruder); //fast sequence
		
		load_filament_final_feed(); //slow sequence
		st_synchronize();

    Sound_MakeCustom(50,500,false);

		if (!farm_mode && loading_flag) {
			lcd_load_filament_color_check();
		}
		lcd_update_enable(true);
		lcd_update(2);
		lcd_setstatuspgm(_T(WELCOME_MSG));
		disable_z();
		loading_flag = false;
		custom_message_type = CustomMsg::Status;

#ifdef FSENSOR_QUALITY
        fsensor_oq_meassure_stop();

        if (!fsensor_oq_result())
        {
            bool disable = lcd_show_fullscreen_message_yes_no_and_wait_P(_i("Fil. sensor response is poor, disable it?"), false, true);
            lcd_update_enable(true);
            lcd_update(2);
            if (disable)
                fsensor_disable();
        }
#endif //FSENSOR_QUALITY
	}
}
/**
 * @brief Get serial number from 32U2 processor
 *
 * Typical format of S/N is:CZPX0917X003XC13518
 *
 * Command operates only in farm mode, if not in farm mode, "Not in farm mode." is written to MYSERIAL.
 *
 * Send command ;S to serial port 0 to retrieve serial number stored in 32U2 processor,
 * reply is transmitted to serial port 1 character by character.
 * Operation takes typically 23 ms. If the retransmit is not finished until 100 ms,
 * it is interrupted, so less, or no characters are retransmitted, only newline character is send
 * in any case.
 */
static void gcode_PRUSA_SN()
{
    if (farm_mode) {
        selectedSerialPort = 0;
        putchar(';');
        putchar('S');
        int numbersRead = 0;
        ShortTimer timeout;
        timeout.start();

        while (numbersRead < 19) {
            while (MSerial.available() > 0) {
                uint8_t serial_char = MSerial.read();
                selectedSerialPort = 1;
                putchar(serial_char);
                numbersRead++;
                selectedSerialPort = 0;
            }
            if (timeout.expired(100u)) break;
        }
        selectedSerialPort = 1;
        putchar('\n');
#if 0
        for (int b = 0; b < 3; b++) {
            _tone(BEEPER, 110);
            _delay(50);
            _noTone(BEEPER);
            _delay(50);
        }
#endif
    } else {
        puts_P(_N("Not in farm mode."));
    }
}
//! Detection of faulty RAMBo 1.1b boards equipped with bigger capacitors
//! at the TACH_1 pin, which causes bad detection of print fan speed.
//! Warning: This function is not to be used by ordinary users, it is here only for automated testing purposes,
//!   it may even interfere with other functions of the printer! You have been warned!
//! The test idea is to measure the time necessary to charge the capacitor.
//! So the algorithm is as follows:
//! 1. Set TACH_1 pin to INPUT mode and LOW
//! 2. Wait a few ms
//! 3. disable interrupts and measure the time until the TACH_1 pin reaches HIGH
//! Repeat 1.-3. several times
//! Good RAMBo's times are in the range of approx. 260-320 us
//! Bad RAMBo's times are approx. 260-1200 us
//! So basically we are interested in maximum time, the minima are mostly the same.
//! May be that's why the bad RAMBo's still produce some fan RPM reading, but not corresponding to reality
static void gcode_PRUSA_BadRAMBoFanTest(){
    //printf_P(PSTR("Enter fan pin test\n"));
#if !defined(DEBUG_DISABLE_FANCHECK) && defined(FANCHECK) && defined(TACH_1) && TACH_1 >-1
	fan_measuring = false; // prevent EXTINT7 breaking into the measurement
	unsigned long tach1max = 0;
	uint8_t tach1cntr = 0;
	for( /* nothing */; tach1cntr < 100; ++tach1cntr){
		//printf_P(PSTR("TACH_1: %d\n"), tach1cntr);
		SET_OUTPUT(TACH_1);
		WRITE(TACH_1, LOW);
		_delay(20); // the delay may be lower
		unsigned long tachMeasure = _micros();
		cli();
		SET_INPUT(TACH_1);
		// just wait brutally in an endless cycle until we reach HIGH
		// if this becomes a problem it may be improved to non-endless cycle
		while( READ(TACH_1) == 0 ) ;
		sei();
		tachMeasure = _micros() - tachMeasure;
		if( tach1max < tachMeasure )
		tach1max = tachMeasure;
		//printf_P(PSTR("TACH_1: %d: capacitor check time=%lu us\n"), (int)tach1cntr, tachMeasure);
	}	
	//printf_P(PSTR("TACH_1: max=%lu us\n"), tach1max);
	SERIAL_PROTOCOLPGM("RAMBo FAN ");
	if( tach1max > 500 ){
		// bad RAMBo
		SERIAL_PROTOCOLLNPGM("BAD");
	} else {
		SERIAL_PROTOCOLLNPGM("OK");
    }
	// cleanup after the test function
	SET_INPUT(TACH_1);
	WRITE(TACH_1, HIGH);
#endif
}


// G92 - Set current position to coordinates given
static void gcode_G92()
{
    bool codes[NUM_AXIS];
    float values[NUM_AXIS];

    // Check which axes need to be set
    for(uint8_t i = 0; i < NUM_AXIS; ++i)
    {
        codes[i] = code_seen(axis_codes[i]);
        if(codes[i])
            values[i] = code_value();
    }

    if((codes[E_AXIS] && values[E_AXIS] == 0) &&
       (!codes[X_AXIS] && !codes[Y_AXIS] && !codes[Z_AXIS]))
    {
        // As a special optimization, when _just_ clearing the E position
        // we schedule a flag asynchronously along with the next block to
        // reset the starting E position instead of stopping the planner
        current_position[E_AXIS] = 0;
        plan_reset_next_e();
    }
    else
    {
        // In any other case we're forced to synchronize
        st_synchronize();
        for(uint8_t i = 0; i < 3; ++i)
        {
            if(codes[i])
                current_position[i] = values[i] + cs.add_homing[i];
        }
        if(codes[E_AXIS])
            current_position[E_AXIS] = values[E_AXIS];

        // Set all at once
        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS],
                          current_position[Z_AXIS], current_position[E_AXIS]);
    }
}


#ifdef BACKLASH_X
extern uint8_t st_backlash_x;
#endif //BACKLASH_X
#ifdef BACKLASH_Y
extern uint8_t st_backlash_y;
#endif //BACKLASH_Y

//! \ingroup marlin_main

//! @brief Parse and process commands
//!
//! look here for descriptions of G-codes: https://reprap.org/wiki/G-code
//!
//!
//! Implemented Codes 
//! -------------------
//!
//! * _This list is not updated. Current documentation is maintained inside the process_cmd function._ 
//!
//!@n PRUSA CODES
//!@n P F - Returns FW versions
//!@n P R - Returns revision of printer
//!
//!@n G0  -> G1
//!@n G1  - Coordinated Movement X Y Z E
//!@n G2  - CW ARC
//!@n G3  - CCW ARC
//!@n G4  - Dwell S<seconds> or P<milliseconds>
//!@n G10 - retract filament according to settings of M207
//!@n G11 - retract recover filament according to settings of M208
//!@n G28 - Home all Axes
//!@n G29 - Detailed Z-Probe, probes the bed at 3 or more points.  Will fail if you haven't homed yet.
//!@n G30 - Single Z Probe, probes bed at current XY location.
//!@n G31 - Dock sled (Z_PROBE_SLED only)
//!@n G32 - Undock sled (Z_PROBE_SLED only)
//!@n G80 - Automatic mesh bed leveling
//!@n G81 - Print bed profile
//!@n G90 - Use Absolute Coordinates
//!@n G91 - Use Relative Coordinates
//!@n G92 - Set current position to coordinates given
//!
//!@n M Codes
//!@n M0   - Unconditional stop - Wait for user to press a button on the LCD
//!@n M1   - Same as M0
//!@n M17  - Enable/Power all stepper motors
//!@n M18  - Disable all stepper motors; same as M84
//!@n M20  - List SD card
//!@n M21  - Init SD card
//!@n M22  - Release SD card
//!@n M23  - Select SD file (M23 filename.g)
//!@n M24  - Start/resume SD print
//!@n M25  - Pause SD print
//!@n M26  - Set SD position in bytes (M26 S12345)
//!@n M27  - Report SD print status
//!@n M28  - Start SD write (M28 filename.g)
//!@n M29  - Stop SD write
//!@n M30  - Delete file from SD (M30 filename.g)
//!@n M31  - Output time since last M109 or SD card start to serial
//!@n M32  - Select file and start SD print (Can be used _while_ printing from SD card files):
//!          syntax "M32 /path/filename#", or "M32 S<startpos bytes> !filename#"
//!          Call gcode file : "M32 P !filename#" and return to caller file after finishing (similar to #include).
//!          The '#' is necessary when calling from within sd files, as it stops buffer prereading
//!@n M42  - Change pin status via gcode Use M42 Px Sy to set pin x to value y, when omitting Px the onboard led will be used.
//!@n M73  - Show percent done and print time remaining
//!@n M80  - Turn on Power Supply
//!@n M81  - Turn off Power Supply
//!@n M82  - Set E codes absolute (default)
//!@n M83  - Set E codes relative while in Absolute Coordinates (G90) mode
//!@n M84  - Disable steppers until next move,
//!          or use S<seconds> to specify an inactivity timeout, after which the steppers will be disabled.  S0 to disable the timeout.
//!@n M85  - Set inactivity shutdown timer with parameter S<seconds>. To disable set zero (default)
//!@n M86  - Set safety timer expiration time with parameter S<seconds>; M86 S0 will disable safety timer
//!@n M92  - Set axis_steps_per_unit - same syntax as G92
//!@n M104 - Set extruder target temp
//!@n M105 - Read current temp
//!@n M106 - Fan on
//!@n M107 - Fan off
//!@n M109 - Sxxx Wait for extruder current temp to reach target temp. Waits only when heating
//!          Rxxx Wait for extruder current temp to reach target temp. Waits when heating and cooling
//!        IF AUTOTEMP is enabled, S<mintemp> B<maxtemp> F<factor>. Exit autotemp by any M109 without F
//!@n M112 - Emergency stop
//!@n M113 - Get or set the timeout interval for Host Keepalive "busy" messages
//!@n M114 - Output current position to serial port
//!@n M115 - Capabilities string
//!@n M117 - display message
//!@n M119 - Output Endstop status to serial port
//!@n M126 - Solenoid Air Valve Open (BariCUDA support by jmil)
//!@n M127 - Solenoid Air Valve Closed (BariCUDA vent to atmospheric pressure by jmil)
//!@n M128 - EtoP Open (BariCUDA EtoP = electricity to air pressure transducer by jmil)
//!@n M129 - EtoP Closed (BariCUDA EtoP = electricity to air pressure transducer by jmil)
//!@n M140 - Set bed target temp
//!@n M150 - Set BlinkM Color Output R: Red<0-255> U(!): Green<0-255> B: Blue<0-255> over i2c, G for green does not work.
//!@n M190 - Sxxx Wait for bed current temp to reach target temp. Waits only when heating
//!          Rxxx Wait for bed current temp to reach target temp. Waits when heating and cooling
//!@n M200 D<millimeters>- set filament diameter and set E axis units to cubic millimeters (use S0 to set back to millimeters).
//!@n M201 - Set max acceleration in units/s^2 for print moves (M201 X1000 Y1000)
//!@n M202 - Set max acceleration in units/s^2 for travel moves (M202 X1000 Y1000) Unused in Marlin!!
//!@n M203 - Set maximum feedrate that your machine can sustain (M203 X200 Y200 Z300 E10000) in mm/sec
//!@n M204 - Set default acceleration: S normal moves T filament only moves (M204 S3000 T7000) in mm/sec^2  also sets minimum segment time in ms (B20000) to prevent buffer under-runs and M20 minimum feedrate
//!@n M205 -  advanced settings:  minimum travel speed S=while printing T=travel only,  B=minimum segment time X= maximum xy jerk, Z=maximum Z jerk, E=maximum E jerk
//!@n M206 - set additional homing offset
//!@n M207 - set retract length S[positive mm] F[feedrate mm/min] Z[additional zlift/hop], stays in mm regardless of M200 setting
//!@n M208 - set recover=unretract length S[positive mm surplus to the M207 S*] F[feedrate mm/sec]
//!@n M209 - S<1=true/0=false> enable automatic retract detect if the slicer did not support G10/11: every normal extrude-only move will be classified as retract depending on the direction.
//!@n M218 - set hotend offset (in mm): T<extruder_number> X<offset_on_X> Y<offset_on_Y>
//!@n M220 S<factor in percent>- set speed factor override percentage
//!@n M221 S<factor in percent>- set extrude factor override percentage
//!@n M226 P<pin number> S<pin state>- Wait until the specified pin reaches the state required
//!@n M240 - Trigger a camera to take a photograph
//!@n M250 - Set LCD contrast C<contrast value> (value 0..63)
//!@n M280 - set servo position absolute. P: servo index, S: angle or microseconds
//!@n M300 - Play beep sound S<frequency Hz> P<duration ms>
//!@n M301 - Set PID parameters P I and D
//!@n M302 - Allow cold extrudes, or set the minimum extrude S<temperature>.
//!@n M303 - PID relay autotune S<temperature> sets the target temperature. (default target temperature = 150C)
//!@n M304 - Set bed PID parameters P I and D
//!@n M400 - Finish all moves
//!@n M401 - Lower z-probe if present
//!@n M402 - Raise z-probe if present
//!@n M404 - N<dia in mm> Enter the nominal filament width (3mm, 1.75mm ) or will display nominal filament width without parameters
//!@n M405 - Turn on Filament Sensor extrusion control.  Optional D<delay in cm> to set delay in centimeters between sensor and extruder
//!@n M406 - Turn off Filament Sensor extrusion control
//!@n M407 - Displays measured filament diameter
//!@n M500 - stores parameters in EEPROM
//!@n M501 - reads parameters from EEPROM (if you need reset them after you changed them temporarily).
//!@n M502 - reverts to the default "factory settings".  You still need to store them in EEPROM afterwards if you want to.
//!@n M503 - print the current settings (from memory not from EEPROM)
//!@n M509 - force language selection on next restart
//!@n M540 - Use S[0|1] to enable or disable the stop SD card print on endstop hit (requires ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED)
//!@n M600 - Pause for filament change X[pos] Y[pos] Z[relative lift] E[initial retract] L[later retract distance for removal]
//!@n M605 - Set dual x-carriage movement mode: S<mode> [ X<duplication x-offset> R<duplication temp offset> ]
//!@n M860 - Wait for PINDA thermistor to reach target temperature.
//!@n M861 - Set / Read PINDA temperature compensation offsets
//!@n M900 - Set LIN_ADVANCE options, if enabled. See Configuration_adv.h for details.
//!@n M907 - Set digital trimpot motor current using axis codes.
//!@n M908 - Control digital trimpot directly.
//!@n M350 - Set microstepping mode.
//!@n M351 - Toggle MS1 MS2 pins directly.
//!
//!@n M928 - Start SD logging (M928 filename.g) - ended by M29
//!@n M999 - Restart after being stopped by error
//! <br><br>

/** @defgroup marlin_main Marlin main */

/** \ingroup GCodes */

//! _This is a list of currently implemented G Codes in Prusa firmware (dynamically generated from doxygen)._ 
/**
They are shown in order of appearance in the code.
There are reasons why some G Codes aren't in numerical order.
*/


void process_commands()
{
#ifdef FANCHECK
    if(fan_check_error){
        if(fan_check_error == EFCE_DETECTED){
            fan_check_error = EFCE_REPORTED;
            // SERIAL_PROTOCOLLNRPGM(MSG_OCTOPRINT_PAUSED);
            lcd_pause_print();
        } // otherwise it has already been reported, so just ignore further processing
        return; //ignore usb stream. It is reenabled by selecting resume from the lcd.
    }
#endif

	if (!buflen) return; //empty command
  #ifdef FILAMENT_RUNOUT_SUPPORT
    SET_INPUT(FR_SENS);
  #endif

#ifdef CMDBUFFER_DEBUG
  SERIAL_ECHOPGM("Processing a GCODE command: ");
  SERIAL_ECHO(cmdbuffer+bufindr+CMDHDRSIZE);
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

#ifdef SNMM
  float tmp_motor[3] = DEFAULT_PWM_MOTOR_CURRENT;
  float tmp_motor_loud[3] = DEFAULT_PWM_MOTOR_CURRENT_LOUD;
  int8_t SilentMode;
#endif
  /*!
  
  ---------------------------------------------------------------------------------
  ### M117 - Display Message <a href="https://reprap.org/wiki/G-code#M117:_Display_Message">M117: Display Message</a>
  This causes the given message to be shown in the status line on an attached LCD.
  It is processed early as to allow printing messages that contain G, M, N or T.
  
  ---------------------------------------------------------------------------------
  ### Special internal commands
  These are used by internal functions to process certain actions in the right order. Some of these are also usable by the user.
  They are processed early as the commands are complex (strings).
  These are only available on the MK3(S) as these require TMC2130 drivers:
    - CRASH DETECTED
    - CRASH RECOVER
    - CRASH_CANCEL
    - TMC_SET_WAVE
    - TMC_SET_STEP
    - TMC_SET_CHOP
 */
  if (code_seen("M117")) { //moved to highest priority place to be able to to print strings which includes "G", "PRUSA" and "^"
	  starpos = (strchr(strchr_pointer + 5, '*'));
	  if (starpos != NULL)
		  *(starpos) = '\0';
	  lcd_setstatus(strchr_pointer + 5);
  }

#ifdef TMC2130
	else if (strncmp_P(CMDBUFFER_CURRENT_STRING, PSTR("CRASH_"), 6) == 0)
	{

    // ### CRASH_DETECTED - TMC2130
    // ---------------------------------
	  if(code_seen("CRASH_DETECTED"))
	  {
		  uint8_t mask = 0;
		  if (code_seen('X')) mask |= X_AXIS_MASK;
		  if (code_seen('Y')) mask |= Y_AXIS_MASK;
		  crashdet_detected(mask);
	  }

    // ### CRASH_RECOVER - TMC2130
    // ----------------------------------
	  else if(code_seen("CRASH_RECOVER"))
		  crashdet_recover();

    // ### CRASH_CANCEL - TMC2130
    // ----------------------------------
	  else if(code_seen("CRASH_CANCEL"))
		  crashdet_cancel();
	}
	else if (strncmp_P(CMDBUFFER_CURRENT_STRING, PSTR("TMC_"), 4) == 0)
	{
    
    // ### TMC_SET_WAVE_ 
    // --------------------
		if (strncmp_P(CMDBUFFER_CURRENT_STRING + 4, PSTR("SET_WAVE_"), 9) == 0)
		{
			uint8_t axis = *(CMDBUFFER_CURRENT_STRING + 13);
			axis = (axis == 'E')?3:(axis - 'X');
			if (axis < 4)
			{
				uint8_t fac = (uint8_t)strtol(CMDBUFFER_CURRENT_STRING + 14, NULL, 10);
				tmc2130_set_wave(axis, 247, fac);
			}
		}
    
    // ### TMC_SET_STEP_
    //  ------------------
		else if (strncmp_P(CMDBUFFER_CURRENT_STRING + 4, PSTR("SET_STEP_"), 9) == 0)
		{
			uint8_t axis = *(CMDBUFFER_CURRENT_STRING + 13);
			axis = (axis == 'E')?3:(axis - 'X');
			if (axis < 4)
			{
				uint8_t step = (uint8_t)strtol(CMDBUFFER_CURRENT_STRING + 14, NULL, 10);
				uint16_t res = tmc2130_get_res(axis);
				tmc2130_goto_step(axis, step & (4*res - 1), 2, 1000, res);
			}
		}

    // ### TMC_SET_CHOP_
    //  -------------------
		else if (strncmp_P(CMDBUFFER_CURRENT_STRING + 4, PSTR("SET_CHOP_"), 9) == 0)
		{
			uint8_t axis = *(CMDBUFFER_CURRENT_STRING + 13);
			axis = (axis == 'E')?3:(axis - 'X');
			if (axis < 4)
			{
				uint8_t chop0 = tmc2130_chopper_config[axis].toff;
				uint8_t chop1 = tmc2130_chopper_config[axis].hstr;
				uint8_t chop2 = tmc2130_chopper_config[axis].hend;
				uint8_t chop3 = tmc2130_chopper_config[axis].tbl;
				char* str_end = 0;
				if (CMDBUFFER_CURRENT_STRING[14])
				{
					chop0 = (uint8_t)strtol(CMDBUFFER_CURRENT_STRING + 14, &str_end, 10) & 15;
					if (str_end && *str_end)
					{
						chop1 = (uint8_t)strtol(str_end, &str_end, 10) & 7;
						if (str_end && *str_end)
						{
							chop2 = (uint8_t)strtol(str_end, &str_end, 10) & 15;
							if (str_end && *str_end)
								chop3 = (uint8_t)strtol(str_end, &str_end, 10) & 3;
						}
					}
				}
				tmc2130_chopper_config[axis].toff = chop0;
				tmc2130_chopper_config[axis].hstr = chop1 & 7;
				tmc2130_chopper_config[axis].hend = chop2 & 15;
				tmc2130_chopper_config[axis].tbl = chop3 & 3;
				tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);
				//printf_P(_N("TMC_SET_CHOP_%c %hhd %hhd %hhd %hhd\n"), "xyze"[axis], chop0, chop1, chop2, chop3);
			}
		}
	}
#ifdef BACKLASH_X
	else if (strncmp_P(CMDBUFFER_CURRENT_STRING, PSTR("BACKLASH_X"), 10) == 0)
	{
		uint8_t bl = (uint8_t)strtol(CMDBUFFER_CURRENT_STRING + 10, NULL, 10);
		st_backlash_x = bl;
		printf_P(_N("st_backlash_x = %hhd\n"), st_backlash_x);
	}
#endif //BACKLASH_X
#ifdef BACKLASH_Y
	else if (strncmp_P(CMDBUFFER_CURRENT_STRING, PSTR("BACKLASH_Y"), 10) == 0)
	{
		uint8_t bl = (uint8_t)strtol(CMDBUFFER_CURRENT_STRING + 10, NULL, 10);
		st_backlash_y = bl;
		printf_P(_N("st_backlash_y = %hhd\n"), st_backlash_y);
	}
#endif //BACKLASH_Y
#endif //TMC2130
  else if(code_seen("PRUSA")){ 
    /*!
    ---------------------------------------------------------------------------------
    ### PRUSA - Internal command set <a href="https://reprap.org/wiki/G-code#G98:_Activate_farm_mode">G98: Activate farm mode - Notes</a>
    
    Set of internal PRUSA commands
    #### Usage
         PRUSA [ Ping | PRN | FAN | fn | thx | uvlo | MMURES | RESET | fv | M28 | SN | Fir | Rev | Lang | Lz | Beat | FR ]
    
    #### Parameters
      - `Ping` 
      - `PRN` - Prints revision of the printer
      - `FAN` - Prints fan details
      - `fn` - Prints farm no.
      - `thx` 
      - `uvlo` 
      - `MMURES` - Reset MMU
      - `RESET` - (Careful!)
      - `fv`  - ?
      - `M28` 
      - `SN` 
      - `Fir` - Prints firmware version
      - `Rev`- Prints filament size, elelectronics, nozzle type
      - `Lang` - Reset the language
      - `Lz` 
      - `Beat` - Kick farm link timer
      - `FR` - Full factory reset
      - `nozzle set <diameter>` - set nozzle diameter (farm mode only), e.g. `PRUSA nozzle set 0.4`
      - `nozzle D<diameter>` - check the nozzle diameter (farm mode only), works like M862.1 P, e.g. `PRUSA nozzle D0.4`
      - `nozzle` - prints nozzle diameter (farm mode only), works like M862.1 P, e.g. `PRUSA nozzle`
    */


		if (code_seen("Ping")) {  // PRUSA Ping
			if (farm_mode) {
				PingTime = _millis();
				//MYSERIAL.print(farm_no); MYSERIAL.println(": OK");
			}	  
		}
		else if (code_seen("PRN")) { // PRUSA PRN
		  printf_P(_N("%d"), status_number);

        } else if( code_seen("FANPINTST") ){
            gcode_PRUSA_BadRAMBoFanTest();
        }else if (code_seen("FAN")) { // PRUSA FAN
			printf_P(_N("E0:%d RPM\nPRN0:%d RPM\n"), 60*fan_speed[0], 60*fan_speed[1]);
		}else if (code_seen("fn")) { // PRUSA fn
		  if (farm_mode) {
			printf_P(_N("%d"), farm_no);
		  }
		  else {
			  puts_P(_N("Not in farm mode."));
		  }
		  
		}
		else if (code_seen("thx")) // PRUSA thx
		{
			no_response = false;
		}	
		else if (code_seen("uvlo")) // PRUSA uvlo
		{
               eeprom_update_byte((uint8_t*)EEPROM_UVLO,0); 
               enquecommand_P(PSTR("M24")); 
		}	
		else if (code_seen("MMURES")) // PRUSA MMURES
		{
			mmu_reset();
		}
		else if (code_seen("RESET")) { // PRUSA RESET
            // careful!
            if (farm_mode) {
#if (defined(WATCHDOG) && (MOTHERBOARD == BOARD_EINSY_1_0a))
                boot_app_magic = BOOT_APP_MAGIC;
                boot_app_flags = BOOT_APP_FLG_RUN;
				wdt_enable(WDTO_15MS);
				cli();
				while(1);
#else //WATCHDOG
                asm volatile("jmp 0x3E000");
#endif //WATCHDOG
            }
            else {
                MYSERIAL.println("Not in farm mode.");
            }
		}else if (code_seen("fv")) { // PRUSA fv
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

    } else if (code_seen("M28")) { // PRUSA M28
        trace();
        prusa_sd_card_upload = true;
        card.openFile(strchr_pointer+4,false);

	} else if (code_seen("SN")) { // PRUSA SN
        gcode_PRUSA_SN();

	} else if(code_seen("Fir")){ // PRUSA Fir

      SERIAL_PROTOCOLLN(FW_VERSION_FULL);

    } else if(code_seen("Rev")){ // PRUSA Rev

      SERIAL_PROTOCOLLN(FILAMENT_SIZE "-" ELECTRONICS "-" NOZZLE_TYPE );

    } else if(code_seen("Lang")) { // PRUSA Lang
	  lang_reset();

	} else if(code_seen("Lz")) { // PRUSA Lz
      eeprom_update_word(reinterpret_cast<uint16_t *>(&(EEPROM_Sheets_base->s[(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)))].z_offset)),0);

	} else if(code_seen("Beat")) { // PRUSA Beat
        // Kick farm link timer
        kicktime = _millis();

    } else if(code_seen("FR")) { // PRUSA FR
        // Factory full reset
        factory_reset(0);
    } else if(code_seen("MBL")) { // PRUSA MBL
        // Change the MBL status without changing the logical Z position.
        if(code_seen("V")) {
            bool value = code_value_short();
            st_synchronize();
            if(value != mbl.active) {
                mbl.active = value;
                // Use plan_set_z_position to reset the physical values
                plan_set_z_position(current_position[Z_AXIS]);
            }
        }

//-//
/*
    } else if(code_seen("rrr")) {
MYSERIAL.println("=== checking ===");
MYSERIAL.println(eeprom_read_byte((uint8_t*)EEPROM_CHECK_MODE),DEC);
MYSERIAL.println(eeprom_read_byte((uint8_t*)EEPROM_NOZZLE_DIAMETER),DEC);
MYSERIAL.println(eeprom_read_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM),DEC);
MYSERIAL.println(farm_mode,DEC);
MYSERIAL.println(eCheckMode,DEC);
    } else if(code_seen("www")) {
MYSERIAL.println("=== @ FF ===");
eeprom_update_byte((uint8_t*)EEPROM_CHECK_MODE,0xFF);
eeprom_update_byte((uint8_t*)EEPROM_NOZZLE_DIAMETER,0xFF);
eeprom_update_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM,0xFFFF);
*/
    } else if (code_seen("nozzle")) { // PRUSA nozzle
          uint16_t nDiameter;
          if(code_seen('D'))
               {
               nDiameter=(uint16_t)(code_value()*1000.0+0.5); // [,um]
               nozzle_diameter_check(nDiameter);
               }
          else if(code_seen("set") && farm_mode)
               {
               strchr_pointer++;                  // skip 1st char (~ 's')
               strchr_pointer++;                  // skip 2nd char (~ 'e')
               nDiameter=(uint16_t)(code_value()*1000.0+0.5); // [,um]
               eeprom_update_byte((uint8_t*)EEPROM_NOZZLE_DIAMETER,(uint8_t)ClNozzleDiameter::_Diameter_Undef); // for correct synchronization after farm-mode exiting
               eeprom_update_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM,nDiameter);
               }
          else SERIAL_PROTOCOLLN((float)eeprom_read_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM)/1000.0);

//-// !!! SupportMenu
/*
// musi byt PRED "PRUSA model"
    } else if (code_seen("smodel")) { //! PRUSA smodel
          size_t nOffset;
// ! -> "l"
          strchr_pointer+=5*sizeof(*strchr_pointer); // skip 1st - 5th char (~ 'smode')
          nOffset=strspn(strchr_pointer+1," \t\n\r\v\f");
          if(*(strchr_pointer+1+nOffset))
               printer_smodel_check(strchr_pointer);
          else SERIAL_PROTOCOLLN(PRINTER_NAME);
    } else if (code_seen("model")) { //! PRUSA model
          uint16_t nPrinterModel;
          strchr_pointer+=4*sizeof(*strchr_pointer); // skip 1st - 4th char (~ 'mode')
          nPrinterModel=(uint16_t)code_value_long();
          if(nPrinterModel!=0)
               printer_model_check(nPrinterModel);
          else SERIAL_PROTOCOLLN(PRINTER_TYPE);
    } else if (code_seen("version")) { //! PRUSA version
          strchr_pointer+=7*sizeof(*strchr_pointer); // skip 1st - 7th char (~ 'version')
          while(*strchr_pointer==' ')             // skip leading spaces
               strchr_pointer++;
          if(*strchr_pointer!=0)
               fw_version_check(strchr_pointer);
          else SERIAL_PROTOCOLLN(FW_VERSION);
    } else if (code_seen("gcode")) { //! PRUSA gcode
          uint16_t nGcodeLevel;
          strchr_pointer+=4*sizeof(*strchr_pointer); // skip 1st - 4th char (~ 'gcod')
          nGcodeLevel=(uint16_t)code_value_long();
          if(nGcodeLevel!=0)
               gcode_level_check(nGcodeLevel);
          else SERIAL_PROTOCOLLN(GCODE_LEVEL);
*/
	}	
    //else if (code_seen('Cal')) {
		//  lcd_calibration();
	  // }

  } 
  // This prevents reading files with "^" in their names.
  // Since it is unclear, if there is some usage of this construct,
  // it will be deprecated in 3.9 alpha a possibly completely removed in the future:
  // else if (code_seen('^')) {
  //  // nothing, this is a version line
  // }
  else if(code_seen('G'))
  {
	gcode_in_progress = (int)code_value();
//	printf_P(_N("BEGIN G-CODE=%u\n"), gcode_in_progress);
    switch (gcode_in_progress)
    {

    /*!
    ---------------------------------------------------------------------------------
	 # G Codes
	### G0, G1 - Coordinated movement X Y Z E <a href="https://reprap.org/wiki/G-code#G0_.26_G1:_Move">G0 & G1: Move</a> 
	In Prusa Firmware G0 and G1 are the same.
	#### Usage
	
	      G0 [ X | Y | Z | E | F | S ]
		  G1 [ X | Y | Z | E | F | S ]
	
	#### Parameters
	  - `X` - The position to move to on the X axis
	  - `Y` - The position to move to on the Y axis
	  - `Z` - The position to move to on the Z axis
	  - `E` - The amount to extrude between the starting point and ending point
	  - `F` - The feedrate per minute of the move between the starting point and ending point (if supplied)
	  
    */
    case 0: // G0 -> G1
    case 1: // G1
      if(Stopped == false) {

        #ifdef FILAMENT_RUNOUT_SUPPORT
            
            if(READ(FR_SENS)){

                        int feedmultiplyBckp=feedmultiply;
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
                        _delay(100);
                        
                        //LCD_ALERTMESSAGEPGM(_T(MSG_FILAMENTCHANGE));
                        uint8_t cnt=0;
                        int counterBeep = 0;
                        lcd_wait_interact();
                        while(!lcd_clicked()){
                          cnt++;
                          manage_heater();
                          manage_inactivity(true);
                          //lcd_update(0);
                          if(cnt==0)
                          {
                          #if BEEPER > 0
                          
                            if (counterBeep== 500){
                              counterBeep = 0;
                              
                            }
                          
                            
                            SET_OUTPUT(BEEPER);
                            if (counterBeep== 0){
if(eSoundMode!=e_SOUND_MODE_SILENT)
                              WRITE(BEEPER,HIGH);
                            }
                            
                            if (counterBeep== 20){
                              WRITE(BEEPER,LOW);
                            }
                            
                            
                            
                          
                            counterBeep++;
                          #else
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

            }



        #endif

            get_coordinates(); // For X Y Z E F

            // When recovering from a previous print move, restore the originally
            // calculated target position on the first USB/SD command. This accounts
            // properly for relative moves
            if ((saved_target[0] != SAVED_TARGET_UNSET) &&
                ((CMDBUFFER_CURRENT_TYPE == CMDBUFFER_CURRENT_TYPE_SDCARD) ||
                 (CMDBUFFER_CURRENT_TYPE == CMDBUFFER_CURRENT_TYPE_USB_WITH_LINENR)))
            {
                memcpy(destination, saved_target, sizeof(destination));
                saved_target[0] = SAVED_TARGET_UNSET;
            }

		if (total_filament_used > ((current_position[E_AXIS] - destination[E_AXIS]) * 100)) { //protection against total_filament_used overflow
			total_filament_used = total_filament_used + ((destination[E_AXIS] - current_position[E_AXIS]) * 100);
		}
          #ifdef FWRETRACT
            if(cs.autoretract_enabled)
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

    /*!
	### G2, G3 - Controlled Arc Move <a href="https://reprap.org/wiki/G-code#G2_.26_G3:_Controlled_Arc_Move">G2 & G3: Controlled Arc Move</a>
	
    These commands don't propperly work with MBL enabled. The compensation only happens at the end of the move, so avoid long arcs.
    
	#### Usage
	
	      G2 [ X | Y | I | E | F ] (Clockwise Arc)
		  G3 [ X | Y | I | E | F ] (Counter-Clockwise Arc)
	
	#### Parameters
	  - `X` - The position to move to on the X axis
	  - `Y` - The position to move to on the Y axis
	  - `I` - The point in X space from the current X position to maintain a constant distance from
	  - `J` - The point in Y space from the current Y position to maintain a constant distance from
	  - `E` - The amount to extrude between the starting point and ending point
	  - `F` - The feedrate per minute of the move between the starting point and ending point (if supplied)
	
    */
    case 2: 
      if(Stopped == false) {
        get_arc_coordinates();
        prepare_arc_move(true);
      }
      break;
 
    // -------------------------------
    case 3: 
      if(Stopped == false) {
        get_arc_coordinates();
        prepare_arc_move(false);
      }
      break;


    /*!
	### G4 - Dwell <a href="https://reprap.org/wiki/G-code#G4:_Dwell">G4: Dwell</a>
	Pause the machine for a period of time.
	
	#### Usage
	
	    G4 [ P | S ]
	
	#### Parameters
	  - `P` - Time to wait, in milliseconds
	  - `S` - Time to wait, in seconds
	
    */
    case 4: 
      codenum = 0;
      if(code_seen('P')) codenum = code_value(); // milliseconds to wait
      if(code_seen('S')) codenum = code_value() * 1000; // seconds to wait
	  if(codenum != 0) LCD_MESSAGERPGM(_n("Sleep..."));////MSG_DWELL
      st_synchronize();
      codenum += _millis();  // keep track of when we started waiting
      previous_millis_cmd = _millis();
      while(_millis() < codenum) {
        manage_heater();
        manage_inactivity();
        lcd_update(0);
      }
      break;
      #ifdef FWRETRACT
      

    /*!
	### G10 - Retract <a href="https://reprap.org/wiki/G-code#G10:_Retract">G10: Retract</a>
	Retracts filament according to settings of `M207`
    */
    case 10: 
       #if EXTRUDERS > 1
        retracted_swap[active_extruder]=(code_seen('S') && code_value_long() == 1); // checks for swap retract argument
        retract(true,retracted_swap[active_extruder]);
       #else
        retract(true);
       #endif
      break;
      

    /*!
	### G11 - Retract recover <a href="https://reprap.org/wiki/G-code#G11:_Unretract">G11: Unretract</a>
	Unretracts/recovers filament according to settings of `M208`
    */
    case 11: 
       #if EXTRUDERS > 1
        retract(false,retracted_swap[active_extruder]);
       #else
        retract(false);
       #endif 
      break;
      #endif //FWRETRACT
    

    /*!
    ### G28 - Home all Axes one at a time <a href="https://reprap.org/wiki/G-code#G28:_Move_to_Origin_.28Home.29">G28: Move to Origin (Home)</a>
    Using `G28` without any parameters will perfom homing of all axes AND mesh bed leveling, while `G28 W` will just home all axes (no mesh bed leveling).
    #### Usage
	
         G28 [ X | Y | Z | W | C ]
    
	#### Parameters
     - `X` - Flag to go back to the X axis origin
     - `Y` - Flag to go back to the Y axis origin
     - `Z` - Flag to go back to the Z axis origin
     - `W` - Suppress mesh bed leveling if `X`, `Y` or `Z` are not provided
     - `C` - Calibrate X and Y origin (home) - Only on MK3/s
	*/
    case 28: 
    {
      long home_x_value = 0;
      long home_y_value = 0;
      long home_z_value = 0;
      // Which axes should be homed?
      bool home_x = code_seen(axis_codes[X_AXIS]);
      home_x_value = code_value_long();
      bool home_y = code_seen(axis_codes[Y_AXIS]);
      home_y_value = code_value_long();
      bool home_z = code_seen(axis_codes[Z_AXIS]);
      home_z_value = code_value_long();
      bool without_mbl = code_seen('W');
      // calibrate?
#ifdef TMC2130
      bool calib = code_seen('C');
      gcode_G28(home_x, home_x_value, home_y, home_y_value, home_z, home_z_value, calib, without_mbl);
#else
      gcode_G28(home_x, home_x_value, home_y, home_y_value, home_z, home_z_value, without_mbl);
#endif //TMC2130
      if ((home_x || home_y || without_mbl || home_z) == false) {
         // Push the commands to the front of the message queue in the reverse order!
         // There shall be always enough space reserved for these commands.
         goto case_G80;
      }
      break;
    }

#ifdef ENABLE_AUTO_BED_LEVELING
    

    /*!
	### G29 - Detailed Z-Probe <a href="https://reprap.org/wiki/G-code#G29:_Detailed_Z-Probe">G29: Detailed Z-Probe</a>
	In Prusa Firmware this G-code is deactivated by default, must be turned on in the source code.
	
	See `G81`
    */
    case 29: 
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
            int l_feedmultiply = setup_for_endstop_move();

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
            clean_up_after_endstop_move(l_feedmultiply);

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

            clean_up_after_endstop_move(l_feedmultiply);

            set_bed_level_equation_3pts(z_at_pt_1, z_at_pt_2, z_at_pt_3);


#endif // AUTO_BED_LEVELING_GRID
            st_synchronize();

            // The following code correct the Z height difference from z-probe position and hotend tip position.
            // The Z height on homing is measured by Z-Probe, but the probe is quite far from the hotend.
            // When the bed is uneven, this height must be corrected.
            real_z = float(st_get_position(Z_AXIS))/cs.axis_steps_per_unit[Z_AXIS];  //get the real Z (since the auto bed leveling is already correcting the plane)
            x_tmp = current_position[X_AXIS] + X_PROBE_OFFSET_FROM_EXTRUDER;
            y_tmp = current_position[Y_AXIS] + Y_PROBE_OFFSET_FROM_EXTRUDER;
            z_tmp = current_position[Z_AXIS];

            apply_rotation_xyz(plan_bed_level_matrix, x_tmp, y_tmp, z_tmp);         //Apply the correction sending the probe offset
            current_position[Z_AXIS] = z_tmp - real_z + current_position[Z_AXIS];   //The difference is added to current position and sent to planner.
            plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        }
        break;
#ifndef Z_PROBE_SLED

    /*!
	### G30 - Single Z Probe <a href="https://reprap.org/wiki/G-code#G30:_Single_Z-Probe">G30: Single Z-Probe</a>
	In Prusa Firmware this G-code is deactivated by default, must be turned on in the source code.
    */
    case 30: 
        {
            st_synchronize();
            // TODO: make sure the bed_level_rotation_matrix is identity or the planner will get set incorectly
            int l_feedmultiply = setup_for_endstop_move();

            feedrate = homing_feedrate[Z_AXIS];

            run_z_probe();
            SERIAL_PROTOCOLPGM(_T(MSG_BED));
            SERIAL_PROTOCOLPGM(" X: ");
            SERIAL_PROTOCOL(current_position[X_AXIS]);
            SERIAL_PROTOCOLPGM(" Y: ");
            SERIAL_PROTOCOL(current_position[Y_AXIS]);
            SERIAL_PROTOCOLPGM(" Z: ");
            SERIAL_PROTOCOL(current_position[Z_AXIS]);
            SERIAL_PROTOCOLPGM("\n");

            clean_up_after_endstop_move(l_feedmultiply);
        }
        break;
#else

    /*!
	### G31 - Dock the sled <a href="https://reprap.org/wiki/G-code#G31:_Dock_Z_Probe_sled">G31: Dock Z Probe sled</a>
	In Prusa Firmware this G-code is deactivated by default, must be turned on in the source code.
    */
    case 31: 
        dock_sled(true);
        break;


    /*!
	### G32 - Undock the sled <a href="https://reprap.org/wiki/G-code#G32:_Undock_Z_Probe_sled">G32: Undock Z Probe sled</a>
	In Prusa Firmware this G-code is deactivated by default, must be turned on in the source code.
    */
    case 32: 
        dock_sled(false);
        break;
#endif // Z_PROBE_SLED
#endif // ENABLE_AUTO_BED_LEVELING
            
#ifdef MESH_BED_LEVELING

    /*!
	### G30 - Single Z Probe <a href="https://reprap.org/wiki/G-code#G30:_Single_Z-Probe">G30: Single Z-Probe</a>
    Sensor must be over the bed.
    The maximum travel distance before an error is triggered is 10mm.
    */
    case 30: 
        {
            st_synchronize();
            // TODO: make sure the bed_level_rotation_matrix is identity or the planner will get set incorectly
            int l_feedmultiply = setup_for_endstop_move();

            feedrate = homing_feedrate[Z_AXIS];

            find_bed_induction_sensor_point_z(-10.f, 3);

			printf_P(_N("%S X: %.5f Y: %.5f Z: %.5f\n"), _T(MSG_BED), _x, _y, _z);

            clean_up_after_endstop_move(l_feedmultiply);
        }
        break;
	
  /*!
  ### G75 - Print temperature interpolation <a href="https://reprap.org/wiki/G-code#G75:_Print_temperature_interpolation">G75: Print temperature interpolation</a>
  Show/print PINDA temperature interpolating.
  */
	case 75:
	{
		for (int i = 40; i <= 110; i++)
			printf_P(_N("%d  %.2f"), i, temp_comp_interpolation(i));
	}
	break;

  /*!
  ### G76 - PINDA probe temperature calibration <a href="https://reprap.org/wiki/G-code#G76:_PINDA_probe_temperature_calibration">G76: PINDA probe temperature calibration</a>
  This G-code is used to calibrate the temperature drift of the PINDA (inductive Sensor).
  
  The PINDAv2 sensor has a built-in thermistor which has the advantage that the calibration can be done once for all materials.
  
  The Original i3 Prusa MK2/s uses PINDAv1 and this calibration improves the temperature drift, but not as good as the PINDAv2.

  #### Example
  
  ```
  G76
  
  echo PINDA probe calibration start
  echo start temperature: 35.0°
  echo ...
  echo PINDA temperature -- Z shift (mm): 0.---
  ```
  */
  case 76: 
	{
#ifdef PINDA_THERMISTOR
		if (true)
		{

			if (calibration_status() >= CALIBRATION_STATUS_XYZ_CALIBRATION) {
				//we need to know accurate position of first calibration point
				//if xyz calibration was not performed yet, interrupt temperature calibration and inform user that xyz cal. is needed
				lcd_show_fullscreen_message_and_wait_P(_i("Please run XYZ calibration first."));
				break;
			}
			
			if (!(axis_known_position[X_AXIS] && axis_known_position[Y_AXIS] && axis_known_position[Z_AXIS]))
			{
				// We don't know where we are! HOME!
				// Push the commands to the front of the message queue in the reverse order!
				// There shall be always enough space reserved for these commands.
				repeatcommand_front(); // repeat G76 with all its parameters
				enquecommand_front_P((PSTR("G28 W0")));
				break;
			}
			lcd_show_fullscreen_message_and_wait_P(_i("Stable ambient temperature 21-26C is needed a rigid stand is required."));////MSG_TEMP_CAL_WARNING c=20 r=4
			bool result = lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_STEEL_SHEET_CHECK), false, false);
			
			if (result)
			{
				current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
				plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
				current_position[Z_AXIS] = 50;
				current_position[Y_AXIS] = 180;
				plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
				st_synchronize();
				lcd_show_fullscreen_message_and_wait_P(_T(MSG_REMOVE_STEEL_SHEET));
				current_position[Y_AXIS] = pgm_read_float(bed_ref_points_4 + 1);
				current_position[X_AXIS] = pgm_read_float(bed_ref_points_4);
				plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
				st_synchronize();
				gcode_G28(false, false, true);

			}
			if ((current_temperature_pinda > 35) && (farm_mode == false)) {
				//waiting for PIDNA probe to cool down in case that we are not in farm mode
				current_position[Z_AXIS] = 100;
				plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
				if (lcd_wait_for_pinda(35) == false) { //waiting for PINDA probe to cool, if this takes more then time expected, temp. cal. fails
					lcd_temp_cal_show_result(false);
					break;
				}
			}
			lcd_update_enable(true);
			KEEPALIVE_STATE(NOT_BUSY); //no need to print busy messages as we print current temperatures periodicaly
			SERIAL_ECHOLNPGM("PINDA probe calibration start");

			float zero_z;
			int z_shift = 0; //unit: steps
			float start_temp = 5 * (int)(current_temperature_pinda / 5);
			if (start_temp < 35) start_temp = 35;
			if (start_temp < current_temperature_pinda) start_temp += 5;
			printf_P(_N("start temperature: %.1f\n"), start_temp);

//			setTargetHotend(200, 0);
			setTargetBed(70 + (start_temp - 30));

			custom_message_type = CustomMsg::TempCal;
			custom_message_state = 1;
			lcd_setstatuspgm(_T(MSG_TEMP_CALIBRATION));
			current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
			plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
			current_position[X_AXIS] = PINDA_PREHEAT_X;
			current_position[Y_AXIS] = PINDA_PREHEAT_Y;
			plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
			current_position[Z_AXIS] = PINDA_PREHEAT_Z;
			plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
			st_synchronize();

			while (current_temperature_pinda < start_temp)
			{
				delay_keep_alive(1000);
				serialecho_temperatures();
			}

			eeprom_update_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, 0); //invalidate temp. calibration in case that in will be aborted during the calibration process 

			current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
			plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
			current_position[X_AXIS] = pgm_read_float(bed_ref_points_4);
			current_position[Y_AXIS] = pgm_read_float(bed_ref_points_4 + 1);
			plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
			st_synchronize();

			bool find_z_result = find_bed_induction_sensor_point_z(-1.f);
			if (find_z_result == false) {
				lcd_temp_cal_show_result(find_z_result);
				break;
			}
			zero_z = current_position[Z_AXIS];

			printf_P(_N("\nZERO: %.3f\n"), current_position[Z_AXIS]);

			int i = -1; for (; i < 5; i++)
			{
				float temp = (40 + i * 5);
				printf_P(_N("\nStep: %d/6 (skipped)\nPINDA temperature: %d Z shift (mm):0\n"), i + 2, (40 + i*5));
				if (i >= 0) EEPROM_save_B(EEPROM_PROBE_TEMP_SHIFT + i * 2, &z_shift);
				if (start_temp <= temp) break;
			}

			for (i++; i < 5; i++)
			{
				float temp = (40 + i * 5);
				printf_P(_N("\nStep: %d/6\n"), i + 2);
				custom_message_state = i + 2;
				setTargetBed(50 + 10 * (temp - 30) / 5);
//				setTargetHotend(255, 0);
				current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
				plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
				current_position[X_AXIS] = PINDA_PREHEAT_X;
				current_position[Y_AXIS] = PINDA_PREHEAT_Y;
				plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
				current_position[Z_AXIS] = PINDA_PREHEAT_Z;
				plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
				st_synchronize();
				while (current_temperature_pinda < temp)
				{
					delay_keep_alive(1000);
					serialecho_temperatures();
				}
				current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
				plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
				current_position[X_AXIS] = pgm_read_float(bed_ref_points_4);
				current_position[Y_AXIS] = pgm_read_float(bed_ref_points_4 + 1);
				plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
				st_synchronize();
				find_z_result = find_bed_induction_sensor_point_z(-1.f);
				if (find_z_result == false) {
					lcd_temp_cal_show_result(find_z_result);
					break;
				}
				z_shift = (int)((current_position[Z_AXIS] - zero_z)*cs.axis_steps_per_unit[Z_AXIS]);

				printf_P(_N("\nPINDA temperature: %.1f Z shift (mm): %.3f"), current_temperature_pinda, current_position[Z_AXIS] - zero_z);

				EEPROM_save_B(EEPROM_PROBE_TEMP_SHIFT + i * 2, &z_shift);

			}
			lcd_temp_cal_show_result(true);

			break;
		}
#endif //PINDA_THERMISTOR

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
		puts_P(_N("PINDA probe calibration start"));
		custom_message_type = CustomMsg::TempCal;
		custom_message_state = 1;
		lcd_setstatuspgm(_T(MSG_TEMP_CALIBRATION));
		current_position[X_AXIS] = PINDA_PREHEAT_X;
		current_position[Y_AXIS] = PINDA_PREHEAT_Y;
		current_position[Z_AXIS] = PINDA_PREHEAT_Z;
		plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
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
		plan_buffer_line_curposXYZE(3000 / 60, active_extruder);

		current_position[X_AXIS] = BED_X0;
		current_position[Y_AXIS] = BED_Y0;
		plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
		st_synchronize();
		
		find_bed_induction_sensor_point_z(-1.f);
		zero_z = current_position[Z_AXIS];

		printf_P(_N("\nZERO: %.3f\n"), current_position[Z_AXIS]);

		for (int i = 0; i<5; i++) {
			printf_P(_N("\nStep: %d/6\n"), i + 2);
			custom_message_state = i + 2;
			t_c = 60 + i * 10;

			setTargetBed(t_c);
			current_position[X_AXIS] = PINDA_PREHEAT_X;
			current_position[Y_AXIS] = PINDA_PREHEAT_Y;
			current_position[Z_AXIS] = PINDA_PREHEAT_Z;
			plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
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
			plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
			current_position[X_AXIS] = BED_X0;
			current_position[Y_AXIS] = BED_Y0;
			plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
			st_synchronize();
			find_bed_induction_sensor_point_z(-1.f);
			z_shift = (int)((current_position[Z_AXIS] - zero_z)*cs.axis_steps_per_unit[Z_AXIS]);

			printf_P(_N("\nTemperature: %d  Z shift (mm): %.3f\n"), t_c, current_position[Z_AXIS] - zero_z);

			EEPROM_save_B(EEPROM_PROBE_TEMP_SHIFT + i*2, &z_shift);
			
		
		}
		custom_message_type = CustomMsg::Status;

		eeprom_update_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, 1);
		puts_P(_N("Temperature calibration done."));
			disable_x();
			disable_y();
			disable_z();
			disable_e0();
			disable_e1();
			disable_e2();
			setTargetBed(0); //set bed target temperature back to 0
		lcd_show_fullscreen_message_and_wait_P(_T(MSG_TEMP_CALIBRATION_DONE));
		temp_cal_active = true;
		eeprom_update_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE, 1);
		lcd_update_enable(true);
		lcd_update(2);		

		

	}
	break;


    /*!
    ### G80 - Mesh-based Z probe <a href="https://reprap.org/wiki/G-code#G80:_Mesh-based_Z_probe">G80: Mesh-based Z probe</a>
    Default 3x3 grid can be changed on MK2.5/s and MK3/s to 7x7 grid.
    #### Usage
	  
          G80 [ N | R | V | L | R | F | B ]
      
	#### Parameters
      - `N` - Number of mesh points on x axis. Default is 3. Valid values are 3 and 7.
      - `R` - Probe retries. Default 3 max. 10
      - `V` - Verbosity level 1=low, 10=mid, 20=high. It only can be used if the firmware has been compiled with SUPPORT_VERBOSITY active.
      
      Using the following parameters enables additional "manual" bed leveling correction. Valid values are -100 microns to 100 microns.
    #### Additional Parameters
      - `L` - Left Bed Level correct value in um.
      - `R` - Right Bed Level correct value in um.
      - `F` - Front Bed Level correct value in um.
      - `B` - Back Bed Level correct value in um.
    */
  
	/*
    * Probes a grid and produces a mesh to compensate for variable bed height
	* The S0 report the points as below
	*  +----> X-axis
	*  |
	*  |
	*  v Y-axis
	*/

	case 80:

#ifdef MK1BP
		break;
#endif //MK1BP
	case_G80:
	{
		mesh_bed_leveling_flag = true;
#ifndef LA_NOCOMPAT
        // When printing via USB there's no clear boundary between prints. Abuse MBL to indicate
        // the beginning of a new print, allowing a new autodetected setting just after G80.
        la10c_reset();
#endif
#ifndef PINDA_THERMISTOR
        static bool run = false; // thermistor-less PINDA temperature compensation is running
#endif // ndef PINDA_THERMISTOR

#ifdef SUPPORT_VERBOSITY
		int8_t verbosity_level = 0;
		if (code_seen('V')) {
			// Just 'V' without a number counts as V1.
			char c = strchr_pointer[1];
			verbosity_level = (c == ' ' || c == '\t' || c == 0) ? 1 : code_value_short();
		}
#endif //SUPPORT_VERBOSITY
		// Firstly check if we know where we are
		if (!(axis_known_position[X_AXIS] && axis_known_position[Y_AXIS] && axis_known_position[Z_AXIS])) {
			// We don't know where we are! HOME!
			// Push the commands to the front of the message queue in the reverse order!
			// There shall be always enough space reserved for these commands.
			repeatcommand_front(); // repeat G80 with all its parameters
			enquecommand_front_P((PSTR("G28 W0")));
			break;
		} 
		
		uint8_t nMeasPoints = MESH_MEAS_NUM_X_POINTS;
		if (code_seen('N')) {
			nMeasPoints = code_value_uint8();
			if (nMeasPoints != 7) {
				nMeasPoints = 3;
			}
		}
		else {
			nMeasPoints = eeprom_read_byte((uint8_t*)EEPROM_MBL_POINTS_NR);
		}

		uint8_t nProbeRetry = 3;
		if (code_seen('R')) {
			nProbeRetry = code_value_uint8();
			if (nProbeRetry > 10) {
				nProbeRetry = 10;
			}
		}
		else {
			nProbeRetry = eeprom_read_byte((uint8_t*)EEPROM_MBL_PROBE_NR);
		}
		bool magnet_elimination = (eeprom_read_byte((uint8_t*)EEPROM_MBL_MAGNET_ELIMINATION) > 0);
		
#ifndef PINDA_THERMISTOR
		if (run == false && temp_cal_active == true && calibration_status_pinda() == true && target_temperature_bed >= 50)
		{
			temp_compensation_start();
			run = true;
			repeatcommand_front(); // repeat G80 with all its parameters
			enquecommand_front_P((PSTR("G28 W0")));
			break;
		}
        run = false;
#endif //PINDA_THERMISTOR
		// Save custom message state, set a new custom message state to display: Calibrating point 9.
		CustomMsg custom_message_type_old = custom_message_type;
		unsigned int custom_message_state_old = custom_message_state;
		custom_message_type = CustomMsg::MeshBedLeveling;
		custom_message_state = (nMeasPoints * nMeasPoints) + 10;
		lcd_update(1);

		mbl.reset(); //reset mesh bed leveling

					 // Reset baby stepping to zero, if the babystepping has already been loaded before. The babystepsTodo value will be
					 // consumed during the first movements following this statement.
		babystep_undo();

		// Cycle through all points and probe them
		// First move up. During this first movement, the babystepping will be reverted.
		current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
		plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS] / 60, active_extruder);
		// The move to the first calibration point.
		current_position[X_AXIS] = BED_X0;
		current_position[Y_AXIS] = BED_Y0;

		#ifdef SUPPORT_VERBOSITY
		if (verbosity_level >= 1)
		{
		    bool clamped = world2machine_clamp(current_position[X_AXIS], current_position[Y_AXIS]);
			clamped ? SERIAL_PROTOCOLPGM("First calibration point clamped.\n") : SERIAL_PROTOCOLPGM("No clamping for first calibration point.\n");
		}
		#else //SUPPORT_VERBOSITY
			world2machine_clamp(current_position[X_AXIS], current_position[Y_AXIS]);
		#endif //SUPPORT_VERBOSITY

		plan_buffer_line_curposXYZE(homing_feedrate[X_AXIS] / 30, active_extruder);
		// Wait until the move is finished.
		st_synchronize();

		uint8_t mesh_point = 0; //index number of calibration point

		int XY_AXIS_FEEDRATE = homing_feedrate[X_AXIS] / 20;
		int Z_LIFT_FEEDRATE = homing_feedrate[Z_AXIS] / 40;
		bool has_z = is_bed_z_jitter_data_valid(); //checks if we have data from Z calibration (offsets of the Z heiths of the 8 calibration points from the first point)
		#ifdef SUPPORT_VERBOSITY
		if (verbosity_level >= 1) {
			has_z ? SERIAL_PROTOCOLPGM("Z jitter data from Z cal. valid.\n") : SERIAL_PROTOCOLPGM("Z jitter data from Z cal. not valid.\n");
		}
		#endif // SUPPORT_VERBOSITY
		int l_feedmultiply = setup_for_endstop_move(false); //save feedrate and feedmultiply, sets feedmultiply to 100
		const char *kill_message = NULL;
		while (mesh_point != nMeasPoints * nMeasPoints) {
			// Get coords of a measuring point.
			uint8_t ix = mesh_point % nMeasPoints; // from 0 to MESH_NUM_X_POINTS - 1
			uint8_t iy = mesh_point / nMeasPoints;
			/*if (!mbl_point_measurement_valid(ix, iy, nMeasPoints, true)) {
				printf_P(PSTR("Skipping point [%d;%d] \n"), ix, iy);
				custom_message_state--;
				mesh_point++;
				continue; //skip
			}*/
			if (iy & 1) ix = (nMeasPoints - 1) - ix; // Zig zag
			if (nMeasPoints == 7) //if we have 7x7 mesh, compare with Z-calibration for points which are in 3x3 mesh
			{
				has_z = ((ix % 3 == 0) && (iy % 3 == 0)) && is_bed_z_jitter_data_valid(); 
			}
			float z0 = 0.f;
			if (has_z && (mesh_point > 0)) {
				uint16_t z_offset_u = 0;
				if (nMeasPoints == 7) {
					z_offset_u = eeprom_read_word((uint16_t*)(EEPROM_BED_CALIBRATION_Z_JITTER + 2 * ((ix/3) + iy - 1)));
				}
				else {
					z_offset_u = eeprom_read_word((uint16_t*)(EEPROM_BED_CALIBRATION_Z_JITTER + 2 * (ix + iy * 3 - 1)));
				}
				z0 = mbl.z_values[0][0] + *reinterpret_cast<int16_t*>(&z_offset_u) * 0.01;
				#ifdef SUPPORT_VERBOSITY
				if (verbosity_level >= 1) {
					printf_P(PSTR("Bed leveling, point: %d, calibration Z stored in eeprom: %d, calibration z: %f \n"), mesh_point, z_offset_u, z0);
				}
				#endif // SUPPORT_VERBOSITY
			}

			// Move Z up to MESH_HOME_Z_SEARCH.
			if((ix == 0) && (iy == 0)) current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
			else current_position[Z_AXIS] += 2.f / nMeasPoints; //use relative movement from Z coordinate where PINDa triggered on previous point. This makes calibration faster.
			float init_z_bckp = current_position[Z_AXIS];
			plan_buffer_line_curposXYZE(Z_LIFT_FEEDRATE, active_extruder);
			st_synchronize();

			// Move to XY position of the sensor point.
			current_position[X_AXIS] = BED_X(ix, nMeasPoints);
			current_position[Y_AXIS] = BED_Y(iy, nMeasPoints);

			//printf_P(PSTR("[%f;%f]\n"), current_position[X_AXIS], current_position[Y_AXIS]);

			
			#ifdef SUPPORT_VERBOSITY
			if (verbosity_level >= 1) {
				clamped = world2machine_clamp(current_position[X_AXIS], current_position[Y_AXIS]);
				SERIAL_PROTOCOL(mesh_point);
				clamped ? SERIAL_PROTOCOLPGM(": xy clamped.\n") : SERIAL_PROTOCOLPGM(": no xy clamping\n");
			}
			#else //SUPPORT_VERBOSITY
				world2machine_clamp(current_position[X_AXIS], current_position[Y_AXIS]);
			#endif // SUPPORT_VERBOSITY

			//printf_P(PSTR("after clamping: [%f;%f]\n"), current_position[X_AXIS], current_position[Y_AXIS]);
			plan_buffer_line_curposXYZE(XY_AXIS_FEEDRATE, active_extruder);
			st_synchronize();

			// Go down until endstop is hit
			const float Z_CALIBRATION_THRESHOLD = 1.f;
			if (!find_bed_induction_sensor_point_z((has_z && mesh_point > 0) ? z0 - Z_CALIBRATION_THRESHOLD : -10.f, nProbeRetry)) { //if we have data from z calibration max allowed difference is 1mm for each point, if we dont have data max difference is 10mm from initial point  
				printf_P(_T(MSG_BED_LEVELING_FAILED_POINT_LOW));
				break;
			}
			if (init_z_bckp - current_position[Z_AXIS] < 0.1f) { //broken cable or initial Z coordinate too low. Go to MESH_HOME_Z_SEARCH and repeat last step (z-probe) again to distinguish between these two cases.
				//printf_P(PSTR("Another attempt! Current Z position: %f\n"), current_position[Z_AXIS]);
				current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
				plan_buffer_line_curposXYZE(Z_LIFT_FEEDRATE, active_extruder);
				st_synchronize();

				if (!find_bed_induction_sensor_point_z((has_z && mesh_point > 0) ? z0 - Z_CALIBRATION_THRESHOLD : -10.f, nProbeRetry)) { //if we have data from z calibration max allowed difference is 1mm for each point, if we dont have data max difference is 10mm from initial point  
					printf_P(_T(MSG_BED_LEVELING_FAILED_POINT_LOW));
					break;
				}
				if (MESH_HOME_Z_SEARCH - current_position[Z_AXIS] < 0.1f) {
					printf_P(PSTR("Bed leveling failed. Sensor disconnected or cable broken.\n"));
					break;
				}
			}
			if (has_z && fabs(z0 - current_position[Z_AXIS]) > Z_CALIBRATION_THRESHOLD) { //if we have data from z calibration, max. allowed difference is 1mm for each point
				printf_P(PSTR("Bed leveling failed. Sensor triggered too high.\n"));
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
			#endif // SUPPORT_VERBOSITY
			float offset_z = 0;

#ifdef PINDA_THERMISTOR
			offset_z = temp_compensation_pinda_thermistor_offset(current_temperature_pinda);
#endif //PINDA_THERMISTOR
//			#ifdef SUPPORT_VERBOSITY
/*			if (verbosity_level >= 1)
			{
				SERIAL_ECHOPGM("mesh bed leveling: ");
				MYSERIAL.print(current_position[Z_AXIS], 5);
				SERIAL_ECHOPGM(" offset: ");
				MYSERIAL.print(offset_z, 5);
				SERIAL_ECHOLNPGM("");
			}*/
//			#endif // SUPPORT_VERBOSITY
			mbl.set_z(ix, iy, current_position[Z_AXIS] - offset_z); //store measured z values z_values[iy][ix] = z - offset_z;

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
		plan_buffer_line_curposXYZE(Z_LIFT_FEEDRATE, active_extruder);
		st_synchronize();
		if (mesh_point != nMeasPoints * nMeasPoints) {
               Sound_MakeSound(e_SOUND_TYPE_StandardAlert);
               bool bState;
               do   {                             // repeat until Z-leveling o.k.
                    lcd_display_message_fullscreen_P(_i("Some problem encountered, Z-leveling enforced ..."));
#ifdef TMC2130
                    lcd_wait_for_click_delay(MSG_BED_LEVELING_FAILED_TIMEOUT);
                    calibrate_z_auto();           // Z-leveling (X-assembly stay up!!!)
#else // TMC2130
                    lcd_wait_for_click_delay(0);  // ~ no timeout
                    lcd_calibrate_z_end_stop_manual(true); // Z-leveling (X-assembly stay up!!!)
#endif // TMC2130
                    // ~ Z-homing (can not be used "G28", because X & Y-homing would have been done before (Z-homing))
                    bState=enable_z_endstop(false);
                    current_position[Z_AXIS] -= 1;
                    plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS] / 40, active_extruder);
                    st_synchronize();
                    enable_z_endstop(true);
#ifdef TMC2130
                    tmc2130_home_enter(Z_AXIS_MASK);
#endif // TMC2130
                    current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
                    plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS] / 40, active_extruder);
                    st_synchronize();
#ifdef TMC2130
                    tmc2130_home_exit();
#endif // TMC2130
                    enable_z_endstop(bState);
                    } while (st_get_position_mm(Z_AXIS) > MESH_HOME_Z_SEARCH); // i.e. Z-leveling not o.k.
//               plan_set_z_position(MESH_HOME_Z_SEARCH); // is not necessary ('do-while' loop always ends at the expected Z-position)
               custom_message_type=CustomMsg::Status; // display / status-line recovery
               lcd_update_enable(true);           // display / status-line recovery
               gcode_G28(true, true, true);       // X & Y & Z-homing (must be after individual Z-homing (problem with spool-holder)!)
               repeatcommand_front();             // re-run (i.e. of "G80")
               break;
		}
		clean_up_after_endstop_move(l_feedmultiply);
//		SERIAL_ECHOLNPGM("clean up finished ");

#ifndef PINDA_THERMISTOR
		if(temp_cal_active == true && calibration_status_pinda() == true) temp_compensation_apply(); //apply PINDA temperature compensation
#endif
		babystep_apply(); // Apply Z height correction aka baby stepping before mesh bed leveing gets activated.
//		SERIAL_ECHOLNPGM("babystep applied");
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
			
			if (labs(correction) > BED_ADJUSTMENT_UM_MAX) {
				SERIAL_ERROR_START;
				SERIAL_ECHOPGM("Excessive bed leveling correction: ");
				SERIAL_ECHO(correction);
				SERIAL_ECHOLNPGM(" microns");
			}
			else {
				float offset = float(correction) * 0.001f;
				switch (i) {
				case 0:
					for (uint8_t row = 0; row < nMeasPoints; ++row) {						
						for (uint8_t col = 0; col < nMeasPoints - 1; ++col) {
							mbl.z_values[row][col] += offset * (nMeasPoints - 1 - col) / (nMeasPoints - 1);
						}
					}
					break;
				case 1:
					for (uint8_t row = 0; row < nMeasPoints; ++row) {					
						for (uint8_t col = 1; col < nMeasPoints; ++col) {
							mbl.z_values[row][col] += offset * col / (nMeasPoints - 1);
						}
					}
					break;
				case 2:
					for (uint8_t col = 0; col < nMeasPoints; ++col) {						
						for (uint8_t row = 0; row < nMeasPoints; ++row) {
							mbl.z_values[row][col] += offset * (nMeasPoints - 1 - row) / (nMeasPoints - 1);
						}
					}
					break;
				case 3:
					for (uint8_t col = 0; col < nMeasPoints; ++col) {						
						for (uint8_t row = 1; row < nMeasPoints; ++row) {
							mbl.z_values[row][col] += offset * row / (nMeasPoints - 1);
						}
					}
					break;
				}
			}
		}
//		SERIAL_ECHOLNPGM("Bed leveling correction finished");
		if (nMeasPoints == 3) {
			mbl.upsample_3x3(); //interpolation from 3x3 to 7x7 points using largrangian polynomials while using the same array z_values[iy][ix] for storing (just coppying measured data to new destination and interpolating between them)
		}
/*
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
*/
		if (nMeasPoints == 7 && magnet_elimination) {
			mbl_interpolation(nMeasPoints);
		}
/*
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
*/
//		SERIAL_ECHOLNPGM("Upsample finished");
		mbl.active = 1; //activate mesh bed leveling
//		SERIAL_ECHOLNPGM("Mesh bed leveling activated");
		go_home_with_z_lift();
//		SERIAL_ECHOLNPGM("Go home finished");
		//unretract (after PINDA preheat retraction)
		if (degHotend(active_extruder) > EXTRUDE_MINTEMP && temp_cal_active == true && calibration_status_pinda() == true && target_temperature_bed >= 50) {
			current_position[E_AXIS] += default_retraction;
			plan_buffer_line_curposXYZE(400, active_extruder);
		}
		KEEPALIVE_STATE(NOT_BUSY);
		// Restore custom message state
		lcd_setstatuspgm(_T(WELCOME_MSG));
		custom_message_type = custom_message_type_old;
		custom_message_state = custom_message_state_old;
		mesh_bed_leveling_flag = false;
		mesh_bed_run_from_menu = false;
		lcd_update(2);
		
	}
	break;

        /*!
		### G81 - Mesh bed leveling status <a href="https://reprap.org/wiki/G-code#G81:_Mesh_bed_leveling_status">G81: Mesh bed leveling status</a>
		Prints mesh bed leveling status and bed profile if activated.
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
        /*!
        ### G82: Single Z probe at current location - Not active <a href="https://reprap.org/wiki/G-code#G82:_Single_Z_probe_at_current_location">G82: Single Z probe at current location</a>
        
        WARNING! USE WITH CAUTION! If you'll try to probe where is no leveling pad, nasty things can happen!
		In Prusa Firmware this G-code is deactivated by default, must be turned on in the source code.
		*/
        case 82:
            SERIAL_PROTOCOLLNPGM("Finding bed ");
            int l_feedmultiply = setup_for_endstop_move();
            find_bed_induction_sensor_point_z();
            clean_up_after_endstop_move(l_feedmultiply);
            SERIAL_PROTOCOLPGM("Bed found at: ");
            SERIAL_PROTOCOL_F(current_position[Z_AXIS], 5);
            SERIAL_PROTOCOLPGM("\n");
            break;

        /*!
        ### G83: Babystep in Z and store to EEPROM - Not active <a href="https://reprap.org/wiki/G-code#G83:_Babystep_in_Z_and_store_to_EEPROM">G83: Babystep in Z and store to EEPROM</a>
		In Prusa Firmware this G-code is deactivated by default, must be turned on in the source code.
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
        /*!
        ### G84: UNDO Babystep Z (move Z axis back) - Not active <a href="https://reprap.org/wiki/G-code#G84:_UNDO_Babystep_Z_.28move_Z_axis_back.29">G84: UNDO Babystep Z (move Z axis back)</a>
		In Prusa Firmware this G-code is deactivated by default, must be turned on in the source code.
		*/
        case 84:
            babystepsTodoZsubtract(babystepLoadZ);
            // babystepLoadZ = 0;
            break;
            
        /*!
        ### G85: Pick best babystep - Not active <a href="https://reprap.org/wiki/G-code#G85:_Pick_best_babystep">G85: Pick best babystep</a>
		In Prusa Firmware this G-code is deactivated by default, must be turned on in the source code.
		*/
        case 85:
            lcd_pick_babystep();
            break;
#endif
            
        /*!
        ### G86 - Disable babystep correction after home <a href="https://reprap.org/wiki/G-code#G86:_Disable_babystep_correction_after_home">G86: Disable babystep correction after home</a>
        
        This G-code will be performed at the start of a calibration script.
        (Prusa3D specific)
        */
        case 86:
            calibration_status_store(CALIBRATION_STATUS_LIVE_ADJUST);
            break;
           

        /*!
        ### G87 - Enable babystep correction after home <a href="https://reprap.org/wiki/G-code#G87:_Enable_babystep_correction_after_home">G87: Enable babystep correction after home</a>
        
		This G-code will be performed at the end of a calibration script.
        (Prusa3D specific)
        */
        case 87:
			calibration_status_store(CALIBRATION_STATUS_CALIBRATED);
            break;

        /*!
        ### G88 - Reserved <a href="https://reprap.org/wiki/G-code#G88:_Reserved">G88: Reserved</a>
        
        Currently has no effect. 
        */

        // Prusa3D specific: Don't know what it is for, it is in V2Calibration.gcode

		    case 88:
			      break;


#endif  // ENABLE_MESH_BED_LEVELING
            

    /*!
	### G90 - Switch off relative mode <a href="https://reprap.org/wiki/G-code#G90:_Set_to_Absolute_Positioning">G90: Set to Absolute Positioning</a>
	All coordinates from now on are absolute relative to the origin of the machine. E axis is also switched to absolute mode.
    */
    case 90: {
        for(uint8_t i = 0; i != NUM_AXIS; ++i)
            axis_relative_modes[i] = false;
    }
    break;

    /*!
	### G91 - Switch on relative mode <a href="https://reprap.org/wiki/G-code#G91:_Set_to_Relative_Positioning">G91: Set to Relative Positioning</a>
    All coordinates from now on are relative to the last position. E axis is also switched to relative mode.
	*/
    case 91: {
        for(uint8_t i = 0; i != NUM_AXIS; ++i)
            axis_relative_modes[i] = true;
    }
    break;

    /*!
	### G92 - Set position <a href="https://reprap.org/wiki/G-code#G92:_Set_Position">G92: Set Position</a>
    
    It is used for setting the current position of each axis. The parameters are always absolute to the origin.
    If a parameter is omitted, that axis will not be affected.
    If `X`, `Y`, or `Z` axis are specified, the move afterwards might stutter because of Mesh Bed Leveling. `E` axis is not affected if the target position is 0 (`G92 E0`).
	A G92 without coordinates will reset all axes to zero on some firmware. This is not the case for Prusa-Firmware!
    
    #### Usage
	
	      G92 [ X | Y | Z | E ]
	
	#### Parameters
	  - `X` - new X axis position
	  - `Y` - new Y axis position
	  - `Z` - new Z axis position
	  - `E` - new extruder position
	
    */
    case 92: {
        gcode_G92();
    }
    break;

    /*!
    ### G98 - Activate farm mode <a href="https://reprap.org/wiki/G-code#G98:_Activate_farm_mode">G98: Activate farm mode</a>
	Enable Prusa-specific Farm functions and g-code.
    See Internal Prusa commands.
    */
	case 98:
		farm_mode = 1;
		PingTime = _millis();
		eeprom_update_byte((unsigned char *)EEPROM_FARM_MODE, farm_mode);
		EEPROM_save_B(EEPROM_FARM_NUMBER, &farm_no);
          SilentModeMenu = SILENT_MODE_OFF;
          eeprom_update_byte((unsigned char *)EEPROM_SILENT, SilentModeMenu);
          fCheckModeInit();                       // alternatively invoke printer reset
		break;

    /*! ### G99 - Deactivate farm mode <a href="https://reprap.org/wiki/G-code#G99:_Deactivate_farm_mode">G99: Deactivate farm mode</a>
 	Disables Prusa-specific Farm functions and g-code.
   */
	case 99:
		farm_mode = 0;
		lcd_printer_connected();
		eeprom_update_byte((unsigned char *)EEPROM_FARM_MODE, farm_mode);
		lcd_update(2);
          fCheckModeInit();                       // alternatively invoke printer reset
		break;
	default:
		printf_P(PSTR("Unknown G code: %s \n"), cmdbuffer + bufindr + CMDHDRSIZE);
    }
//	printf_P(_N("END G-CODE=%u\n"), gcode_in_progress);
	gcode_in_progress = 0;
  } // end if(code_seen('G'))
  /*!
  ### End of G-Codes
  */

  /*!
  ---------------------------------------------------------------------------------
  # M Commands
  
  */

  else if(code_seen('M'))
  {

	  int index;
	  for (index = 1; *(strchr_pointer + index) == ' ' || *(strchr_pointer + index) == '\t'; index++);
	   
	 /*for (++strchr_pointer; *strchr_pointer == ' ' || *strchr_pointer == '\t'; ++strchr_pointer);*/
	  if (*(strchr_pointer+index) < '0' || *(strchr_pointer+index) > '9') {
		  printf_P(PSTR("Invalid M code: %s \n"), cmdbuffer + bufindr + CMDHDRSIZE);

	  } else
	  {
	  mcode_in_progress = (int)code_value();
//	printf_P(_N("BEGIN M-CODE=%u\n"), mcode_in_progress);

    switch(mcode_in_progress)
    {

    /*!
	### M0, M1 - Stop the printer <a href="https://reprap.org/wiki/G-code#M0:_Stop_or_Unconditional_stop">M0: Stop or Unconditional stop</a>
    */
    case 0: // M0 - Unconditional stop - Wait for user button press on LCD
    case 1: // M1 - Conditional stop - Wait for user button press on LCD
    {
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
        LCD_MESSAGERPGM(_i("Wait for user..."));////MSG_USERWAIT
      }

      lcd_ignore_click();				//call lcd_ignore_click aslo for else ???
      st_synchronize();
      previous_millis_cmd = _millis();
      if (codenum > 0){
        codenum += _millis();  // keep track of when we started waiting
		KEEPALIVE_STATE(PAUSED_FOR_USER);
        while(_millis() < codenum && !lcd_clicked()){
          manage_heater();
          manage_inactivity(true);
          lcd_update(0);
        }
		KEEPALIVE_STATE(IN_HANDLER);
        lcd_ignore_click(false);
      }else{
        marlin_wait_for_click();
      }
      if (IS_SD_PRINTING)
        LCD_MESSAGERPGM(_T(MSG_RESUMING_PRINT));
      else
        LCD_MESSAGERPGM(_T(WELCOME_MSG));
    }
    break;

    /*!
	### M17 - Enable all axes <a href="https://reprap.org/wiki/G-code#M17:_Enable.2FPower_all_stepper_motors">M17: Enable/Power all stepper motors</a>
    */
    case 17:
        LCD_MESSAGERPGM(_i("No move."));////MSG_NO_MOVE
        enable_x();
        enable_y();
        enable_z();
        enable_e0();
        enable_e1();
        enable_e2();
      break;

#ifdef SDSUPPORT

    /*!
	### M20 - SD Card file list <a href="https://reprap.org/wiki/G-code#M20:_List_SD_card">M20: List SD card</a>
    */
    case 20:
      SERIAL_PROTOCOLLNRPGM(_N("Begin file list"));////MSG_BEGIN_FILE_LIST
      card.ls();
      SERIAL_PROTOCOLLNRPGM(_N("End file list"));////MSG_END_FILE_LIST
      break;

    /*!
	### M21 - Init SD card <a href="https://reprap.org/wiki/G-code#M21:_Initialize_SD_card">M21: Initialize SD card</a>
    */
    case 21:
      card.initsd();
      break;

    /*!
	### M22 - Release SD card <a href="https://reprap.org/wiki/G-code#M22:_Release_SD_card">M22: Release SD card</a>
    */
    case 22: 
      card.release();
      break;

    /*!
	### M23 - Select file <a href="https://reprap.org/wiki/G-code#M23:_Select_SD_file">M23: Select SD file</a>
    #### Usage
    
        M23 [filename]
    
    */
    case 23: 
      starpos = (strchr(strchr_pointer + 4,'*'));
	  if(starpos!=NULL)
        *(starpos)='\0';
      card.openFile(strchr_pointer + 4,true);
      break;

    /*!
	### M24 - Start SD print <a href="https://reprap.org/wiki/G-code#M24:_Start.2Fresume_SD_print">M24: Start/resume SD print</a>
    */
    case 24:
	  if (isPrintPaused)
          lcd_resume_print();
      else
      {
          if (!card.get_sdpos())
          {
              // A new print has started from scratch, reset stats
              failstats_reset_print();
#ifndef LA_NOCOMPAT
              la10c_reset();
#endif
          }

          card.startFileprint();
          starttime=_millis();
      }
	  break;

    /*!
	### M26 - Set SD index <a href="https://reprap.org/wiki/G-code#M26:_Set_SD_position">M26: Set SD position</a>
    Set position in SD card file to index in bytes.
    This command is expected to be called after M23 and before M24.
    Otherwise effect of this command is undefined.
    #### Usage
	
	      M26 [ S ]
	
	#### Parameters
	  - `S` - Index in bytes
    */
    case 26: 
      if(card.cardOK && code_seen('S')) {
        long index = code_value_long();
        card.setIndex(index);
        // We don't disable interrupt during update of sdpos_atomic
        // as we expect, that SD card print is not active in this moment
        sdpos_atomic = index;
      }
      break;

    /*!
	### M27 - Get SD status <a href="https://reprap.org/wiki/G-code#M27:_Report_SD_print_status">M27: Report SD print status</a>
    */
    case 27:
      card.getStatus();
      break;

    /*!
	### M28 - Start SD write <a href="https://reprap.org/wiki/G-code#M28:_Begin_write_to_SD_card">M28: Begin write to SD card</a>
    */
    case 28: 
      starpos = (strchr(strchr_pointer + 4,'*'));
      if(starpos != NULL){
        char* npos = strchr(CMDBUFFER_CURRENT_STRING, 'N');
        strchr_pointer = strchr(npos,' ') + 1;
        *(starpos) = '\0';
      }
      card.openFile(strchr_pointer+4,false);
      break;

    /*! ### M29 - Stop SD write <a href="https://reprap.org/wiki/G-code#M29:_Stop_writing_to_SD_card">M29: Stop writing to SD card</a>
	Stops writing to the SD file signaling the end of the uploaded file. It is processed very early and it's not written to the card.
    */
    case 29:
      //processed in write to file routine above
      //card,saving = false;
      break;

    /*!
	### M30 - Delete file <a href="https://reprap.org/wiki/G-code#M30:_Delete_a_file_on_the_SD_card">M30: Delete a file on the SD card</a>
    #### Usage
    
        M30 [filename]
    
    */
    case 30:
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

    /*!
	### M32 - Select file and start SD print <a href="https://reprap.org/wiki/G-code#M32:_Select_file_and_start_SD_print">M32: Select file and start SD print</a>
	@todo What are the parameters P and S for in M32?
    */
    case 32:
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
        {
            if(!card.get_sdpos())
            {
                // A new print has started from scratch, reset stats
                failstats_reset_print();
#ifndef LA_NOCOMPAT
                la10c_reset();
#endif
            }
            starttime=_millis(); // procedure calls count as normal print time.
        }
      }
    } break;

    /*!
	### M928 - Start SD logging <a href="https://reprap.org/wiki/G-code#M928:_Start_SD_logging">M928: Start SD logging</a>
    #### Usage
    
        M928 [filename]
    
    */
    case 928: 
      starpos = (strchr(strchr_pointer + 5,'*'));
      if(starpos != NULL){
        char* npos = strchr(CMDBUFFER_CURRENT_STRING, 'N');
        strchr_pointer = strchr(npos,' ') + 1;
        *(starpos) = '\0';
      }
      card.openLogFile(strchr_pointer+5);
      break;

#endif //SDSUPPORT

    /*!
	### M31 - Report current print time <a href="https://reprap.org/wiki/G-code#M31:_Output_time_since_last_M109_or_SD_card_start_to_serial">M31: Output time since last M109 or SD card start to serial</a>
    */
    case 31: //M31 take time since the start of the SD print or an M109 command
      {
      stoptime=_millis();
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

    /*!
	### M42 - Set pin state <a href="https://reprap.org/wiki/G-code#M42:_Switch_I.2FO_pin">M42: Switch I/O pin</a>
    #### Usage
    
        M42 [ P | S ]
        
    #### Parameters
    - `P` - Pin number.
    - `S` - Pin value. If the pin is analog, values are from 0 to 255. If the pin is digital, values are from 0 to 1.
    
    */
    case 42:
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


    /*!
	### M44 - Reset the bed skew and offset calibration <a href="https://reprap.org/wiki/G-code#M44:_Reset_the_bed_skew_and_offset_calibration">M44: Reset the bed skew and offset calibration</a>
    */
    case 44: // M44: Prusa3D: Reset the bed skew and offset calibration.

		// Reset the baby step value and the baby step applied flag.
		calibration_status_store(CALIBRATION_STATUS_ASSEMBLED);
          eeprom_update_word(reinterpret_cast<uint16_t *>(&(EEPROM_Sheets_base->s[(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)))].z_offset)),0);

        // Reset the skew and offset in both RAM and EEPROM.
        reset_bed_offset_and_skew();
        // Reset world2machine_rotation_and_skew and world2machine_shift, therefore
        // the planner will not perform any adjustments in the XY plane. 
        // Wait for the motors to stop and update the current position with the absolute values.
        world2machine_revert_to_uncorrected();
        break;

    /*!
	### M45 - Bed skew and offset with manual Z up <a href="https://reprap.org/wiki/G-code#M45:_Bed_skew_and_offset_with_manual_Z_up">M45: Bed skew and offset with manual Z up</a>
	#### Usage
    
        M45 [ V ]
    #### Parameters
	- `V` - Verbosity level 1, 10 and 20 (low, mid, high). Only when SUPPORT_VERBOSITY is defined. Optional.
    - `Z` - If it is provided, only Z calibration will run. Otherwise full calibration is executed.
    */
    case 45: // M45: Prusa3D: bed skew and offset with manual Z up
    {
		int8_t verbosity_level = 0;
		bool only_Z = code_seen('Z');
		#ifdef SUPPORT_VERBOSITY
		if (code_seen('V'))
		{
			// Just 'V' without a number counts as V1.
			char c = strchr_pointer[1];
			verbosity_level = (c == ' ' || c == '\t' || c == 0) ? 1 : code_value_short();
		}
		#endif //SUPPORT_VERBOSITY
		gcode_M45(only_Z, verbosity_level);
    }
	break;

    /*!
	### M46 - Show the assigned IP address <a href="https://reprap.org/wiki/G-code#M46:_Show_the_assigned_IP_address">M46: Show the assigned IP address.</a>
    */
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

    /*!
	### M47 - Show end stops dialog on the display <a href="https://reprap.org/wiki/G-code#M47:_Show_end_stops_dialog_on_the_display">M47: Show end stops dialog on the display</a>
    */
    case 47:
        
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
        int l_feedmultiply = setup_for_endstop_move();
        home_xy();
        int8_t verbosity_level = 0;
        if (code_seen('V')) {
            // Just 'V' without a number counts as V1.
            char c = strchr_pointer[1];
            verbosity_level = (c == ' ' || c == '\t' || c == 0) ? 1 : code_value_short();
        }
        bool success = scan_bed_induction_points(verbosity_level);
        clean_up_after_endstop_move(l_feedmultiply);
        // Print head up.
        current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
        plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS],current_position[Z_AXIS] , current_position[E_AXIS], homing_feedrate[Z_AXIS]/40, active_extruder);
        st_synchronize();
        lcd_update_enable(true);
        break;
    }
#endif


#ifdef ENABLE_AUTO_BED_LEVELING
#ifdef Z_PROBE_REPEATABILITY_TEST 

    /*!
	### M48 - Z-Probe repeatability measurement function <a href="https://reprap.org/wiki/G-code#M48:_Measure_Z-Probe_repeatability">M48: Measure Z-Probe repeatability</a>
    
     This function assumes the bed has been homed.  Specifically, that a G28 command as been issued prior to invoking the M48 Z-Probe repeatability measurement function. Any information generated by a prior G29 Bed leveling command will be lost and needs to be regenerated.
     
     The number of samples will default to 10 if not specified.  You can use upper or lower case letters for any of the options EXCEPT n.  n must be in lower case because Marlin uses a capital N for its communication protocol and will get horribly confused if you send it a capital N.
     @todo Why would you check for both uppercase and lowercase? Seems wasteful.
	 
     #### Usage
     
	     M48 [ n | X | Y | V | L ]
     
     #### Parameters
       - `n` - Number of samples. Valid values 4-50
	   - `X` - X position for samples
	   - `Y` - Y position for samples
	   - `V` - Verbose level. Valid values 1-4
	   - `L` - Legs of movementprior to doing probe. Valid values 1-15
    */
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

	int l_feedmultiply = setup_for_endstop_move();
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

			rotational_direction = (unsigned long) _millis() & 0x0001;			// clockwise or counter clockwise
			radius = (unsigned long) _millis() % (long) (X_MAX_LENGTH/4); 			// limit how far out to go 
			theta = (float) ((unsigned long) _millis() % (long) 360) / (360./(2*3.1415926));	// turn into radians

//SERIAL_ECHOPAIR("starting radius: ",radius);
//SERIAL_ECHOPAIR("   theta: ",theta);
//SERIAL_ECHOPAIR("   direction: ",rotational_direction);
//SERIAL_PROTOCOLLNPGM("");

			for( l=0; l<n_legs-1; l++) {
				if (rotational_direction==1)
					theta += (float) ((unsigned long) _millis() % (long) 20) / (360.0/(2*3.1415926)); // turn into radians
				else
					theta -= (float) ((unsigned long) _millis() % (long) 20) / (360.0/(2*3.1415926)); // turn into radians

				radius += (float) ( ((long) ((unsigned long) _millis() % (long) 10)) - 5);
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

		int l_feedmultiply = setup_for_endstop_move();
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

	_delay(1000);

    clean_up_after_endstop_move(l_feedmultiply);

//  enable_endstops(true);

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

	/*!
	### M73 - Set/get print progress <a href="https://reprap.org/wiki/G-code#M73:_Set.2FGet_build_percentage">M73: Set/Get build percentage</a>
	#### Usage
    
	    M73 [ P | R | Q | S ]
    
	#### Parameters
    - `P` - Percent in normal mode
    - `R` - Time remaining in normal mode
    - `Q` - Percent in silent mode
    - `S` - Time in silent mode
   */
	case 73: //M73 show percent done and time remaining
		if(code_seen('P')) print_percent_done_normal = code_value();
		if(code_seen('R')) print_time_remaining_normal = code_value();
		if(code_seen('Q')) print_percent_done_silent = code_value();
		if(code_seen('S')) print_time_remaining_silent = code_value();

		{
			const char* _msg_mode_done_remain = _N("%S MODE: Percent done: %d; print time remaining in mins: %d\n");
			printf_P(_msg_mode_done_remain, _N("NORMAL"), int(print_percent_done_normal), print_time_remaining_normal);
			printf_P(_msg_mode_done_remain, _N("SILENT"), int(print_percent_done_silent), print_time_remaining_silent);
		}
		break;

    /*!
	### M104 - Set hotend temperature <a href="https://reprap.org/wiki/G-code#M104:_Set_Extruder_Temperature">M104: Set Extruder Temperature</a>
	#### Usage
    
	    M104 [ S ]
    
	#### Parameters
       - `S` - Target temperature
    */
    case 104: // M104
    {
          uint8_t extruder;
          if(setTargetedHotend(104,extruder)){
            break;
          }
          if (code_seen('S'))
          {
              setTargetHotendSafe(code_value(), extruder);
          }
          break;
    }

    /*!
	### M112 - Emergency stop <a href="https://reprap.org/wiki/G-code#M112:_Full_.28Emergency.29_Stop">M112: Full (Emergency) Stop</a>
    It is processed much earlier as to bypass the cmdqueue.
    */
    case 112: 
      kill(MSG_M112_KILL, 3);
      break;

    /*!
	### M140 - Set bed temperature <a href="https://reprap.org/wiki/G-code#M140:_Set_Bed_Temperature_.28Fast.29">M140: Set Bed Temperature (Fast)</a>
    #### Usage
    
	    M140 [ S ]
    
	#### Parameters
       - `S` - Target temperature
    */
    case 140: 
      if (code_seen('S')) setTargetBed(code_value());
      break;

    /*!
	### M105 - Report temperatures <a href="https://reprap.org/wiki/G-code#M105:_Get_Extruder_Temperature">M105: Get Extruder Temperature</a>
	Prints temperatures:
	
	  - `T:`  - Hotend (actual / target)
	  - `B:`  - Bed (actual / target)
	  - `Tx:` - x Tool (actual / target)
	  - `@:`  - Hotend power
	  - `B@:` - Bed power
	  - `P:`  - PINDAv2 actual (only MK2.5/s and MK3/s)
	  - `A:`  - Ambient actual (only MK3/s)
	
	_Example:_
	
	    ok T:20.2 /0.0 B:19.1 /0.0 T0:20.2 /0.0 @:0 B@:0 P:19.8 A:26.4
	
    */
    case 105:
    {
      uint8_t extruder;
      if(setTargetedHotend(105, extruder)){
        break;
        }
      #if defined(TEMP_0_PIN) && TEMP_0_PIN > -1
        SERIAL_PROTOCOLPGM("ok T:");
        SERIAL_PROTOCOL_F(degHotend(extruder),1);
        SERIAL_PROTOCOLPGM(" /");
        SERIAL_PROTOCOL_F(degTargetHotend(extruder),1);
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
        SERIAL_ERRORLNRPGM(_i("No thermistors - no temperature"));////MSG_ERR_NO_THERMISTORS
      #endif

        SERIAL_PROTOCOLPGM(" @:");
      #ifdef EXTRUDER_WATTS
        SERIAL_PROTOCOL((EXTRUDER_WATTS * getHeaterPower(tmp_extruder))/127);
        SERIAL_PROTOCOLPGM("W");
      #else
        SERIAL_PROTOCOL(getHeaterPower(extruder));
      #endif

        SERIAL_PROTOCOLPGM(" B@:");
      #ifdef BED_WATTS
        SERIAL_PROTOCOL((BED_WATTS * getHeaterPower(-1))/127);
        SERIAL_PROTOCOLPGM("W");
      #else
        SERIAL_PROTOCOL(getHeaterPower(-1));
      #endif

#ifdef PINDA_THERMISTOR
		SERIAL_PROTOCOLPGM(" P:");
		SERIAL_PROTOCOL_F(current_temperature_pinda,1);
#endif //PINDA_THERMISTOR

#ifdef AMBIENT_THERMISTOR
		SERIAL_PROTOCOLPGM(" A:");
		SERIAL_PROTOCOL_F(current_temperature_ambient,1);
#endif //AMBIENT_THERMISTOR


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
    }

    /*!
	### M109 - Wait for extruder temperature <a href="https://reprap.org/wiki/G-code#M109:_Set_Extruder_Temperature_and_Wait">M109: Set Extruder Temperature and Wait</a>
    #### Usage
    
	    M104 [ B | R | S ]
    
    #### Parameters (not mandatory)
     
	  - `S` - Set extruder temperature
      - `R` - Set extruder temperature
	  - `B` - Set max. extruder temperature, while `S` is min. temperature. Not active in default, only if AUTOTEMP is defined in source code.
    
    Parameters S and R are treated identically.
    Command always waits for both cool down and heat up.
    If no parameters are supplied waits for previously set extruder temperature.
    */
    case 109:
    {
      uint8_t extruder;
      if(setTargetedHotend(109, extruder)){
        break;
      }
      LCD_MESSAGERPGM(_T(MSG_HEATING));
	  heating_status = 1;
	  if (farm_mode) { prusa_statistics(1); };

#ifdef AUTOTEMP
        autotemp_enabled=false;
      #endif
      if (code_seen('S')) {
          setTargetHotendSafe(code_value(), extruder);
            } else if (code_seen('R')) {
                setTargetHotendSafe(code_value(), extruder);
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

      codenum = _millis();

      /* See if we are heating up or cooling down */
      target_direction = isHeatingHotend(extruder); // true if heating, false if cooling
	  
	  KEEPALIVE_STATE(NOT_BUSY);

      cancel_heatup = false;

	  wait_for_heater(codenum, extruder); //loops until target temperature is reached

        LCD_MESSAGERPGM(_T(MSG_HEATING_COMPLETE));
		KEEPALIVE_STATE(IN_HANDLER);
		heating_status = 2;
		if (farm_mode) { prusa_statistics(2); };
        
        //starttime=_millis();
        previous_millis_cmd = _millis();
      }
      break;

    /*!
	### M190 - Wait for bed temperature <a href="https://reprap.org/wiki/G-code#M190:_Wait_for_bed_temperature_to_reach_target_temp">M190: Wait for bed temperature to reach target temp</a>
    #### Usage
    
        M190 [ R | S ]
    
    #### Parameters (not mandatory)
    
	  - `S` - Set extruder temperature and wait for heating
      - `R` - Set extruder temperature and wait for heating or cooling
    
    If no parameter is supplied, waits for heating or cooling to previously set temperature.
	*/
    case 190: 
    #if defined(TEMP_BED_PIN) && TEMP_BED_PIN > -1
    {
        bool CooldownNoWait = false;
        LCD_MESSAGERPGM(_T(MSG_BED_HEATING));
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
        }
        codenum = _millis();
        
        cancel_heatup = false;
        target_direction = isHeatingBed(); // true if heating, false if cooling

		KEEPALIVE_STATE(NOT_BUSY);
        while ( (target_direction)&&(!cancel_heatup) ? (isHeatingBed()) : (isCoolingBed()&&(CooldownNoWait==false)) )
        {
          if(( _millis() - codenum) > 1000 ) //Print Temp Reading every 1 second while heating up.
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
				  codenum = _millis();
			  
          }
          manage_heater();
          manage_inactivity();
          lcd_update(0);
        }
        LCD_MESSAGERPGM(_T(MSG_BED_DONE));
		KEEPALIVE_STATE(IN_HANDLER);
		heating_status = 4;

        previous_millis_cmd = _millis();
    }
    #endif
        break;

    #if defined(FAN_PIN) && FAN_PIN > -1

      /*!
	  ### M106 - Set fan speed <a href="https://reprap.org/wiki/G-code#M106:_Fan_On">M106: Fan On</a>
      #### Usage
      
        M106 [ S ]
        
      #### Parameters
      - `S` - Specifies the duty cycle of the print fan. Allowed values are 0-255. If it's omitted, a value of 255 is used.
      */
      case 106: // M106 Sxxx Fan On S<speed> 0 .. 255
        if (code_seen('S')){
           fanSpeed=constrain(code_value(),0,255);
        }
        else {
          fanSpeed=255;
        }
        break;

      /*!
	  ### M107 - Fan off <a href="https://reprap.org/wiki/G-code#M107:_Fan_Off">M107: Fan Off</a>
      */
      case 107:
        fanSpeed = 0;
        break;
    #endif //FAN_PIN

    #if defined(PS_ON_PIN) && PS_ON_PIN > -1

      /*!
	  ### M80 - Turn on the Power Supply <a href="https://reprap.org/wiki/G-code#M80:_ATX_Power_On">M80: ATX Power On</a>
      Only works if the firmware is compiled with PS_ON_PIN defined.
      */
      case 80:
        SET_OUTPUT(PS_ON_PIN); //GND
        WRITE(PS_ON_PIN, PS_ON_AWAKE);

        // If you have a switch on suicide pin, this is useful
        // if you want to start another print with suicide feature after
        // a print without suicide...
        #if defined SUICIDE_PIN && SUICIDE_PIN > -1
            SET_OUTPUT(SUICIDE_PIN);
            WRITE(SUICIDE_PIN, HIGH);
        #endif

          powersupply = true;
          LCD_MESSAGERPGM(_T(WELCOME_MSG));
          lcd_update(0);
        break;

      /*!
	  ### M81 - Turn off Power Supply <a href="https://reprap.org/wiki/G-code#M81:_ATX_Power_Off">M81: ATX Power Off</a>
      Only works if the firmware is compiled with PS_ON_PIN defined.
      */
      case 81: 
        disable_heater();
        st_synchronize();
        disable_e0();
        disable_e1();
        disable_e2();
        finishAndDisableSteppers();
        fanSpeed = 0;
        _delay(1000); // Wait a little before to switch off
      #if defined(SUICIDE_PIN) && SUICIDE_PIN > -1
        st_synchronize();
        suicide();
      #elif defined(PS_ON_PIN) && PS_ON_PIN > -1
        SET_OUTPUT(PS_ON_PIN);
        WRITE(PS_ON_PIN, PS_ON_ASLEEP);
      #endif
        powersupply = false;
        LCD_MESSAGERPGM(CAT4(CUSTOM_MENDEL_NAME,PSTR(" "),MSG_OFF,PSTR(".")));
        lcd_update(0);
	  break;
    #endif

    /*!
	### M82 - Set E axis to absolute mode <a href="https://reprap.org/wiki/G-code#M82:_Set_extruder_to_absolute_mode">M82: Set extruder to absolute mode</a>
	Makes the extruder interpret extrusion as absolute positions.
    */
    case 82:
      axis_relative_modes[E_AXIS] = false;
      break;

    /*!
	### M83 - Set E axis to relative mode <a href="https://reprap.org/wiki/G-code#M83:_Set_extruder_to_relative_mode">M83: Set extruder to relative mode</a>
	Makes the extruder interpret extrusion values as relative positions.
    */
    case 83:
      axis_relative_modes[E_AXIS] = true;
      break;

    /*!
	### M84 - Disable steppers <a href="https://reprap.org/wiki/G-code#M84:_Stop_idle_hold">M84: Stop idle hold</a>
    This command can be used to set the stepper inactivity timeout (`S`) or to disable steppers (`X`,`Y`,`Z`,`E`)
	This command can be used without any additional parameters. In that case all steppers are disabled.
    
    The file completeness check uses this parameter to detect an incomplete file. It has to be present at the end of a file with no parameters.
	
        M84 [ S | X | Y | Z | E ]
	
	  - `S` - Seconds
	  - `X` - X axis
	  - `Y` - Y axis
	  - `Z` - Z axis
	  - `E` - Exruder

	### M18 - Disable steppers <a href="https://reprap.org/wiki/G-code#M18:_Disable_all_stepper_motors">M18: Disable all stepper motors</a>
	Equal to M84 (compatibility)
    */
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
	  //in the end of print set estimated time to end of print and extruders used during print to default values for next print
	  print_time_remaining_init();
	  snmm_filaments_used = 0;
      break;

    /*!
	### M85 - Set max inactive time <a href="https://reprap.org/wiki/G-code#M85:_Set_Inactivity_Shutdown_Timer">M85: Set Inactivity Shutdown Timer</a>
    #### Usage
    
        M85 [ S ]
    
    #### Parameters
    - `S` - specifies the time in seconds. If a value of 0 is specified, the timer is disabled.
    */
    case 85: // M85
      if(code_seen('S')) {
        max_inactive_time = code_value() * 1000;
      }
      break;
#ifdef SAFETYTIMER

    /*!
    ### M86 - Set safety timer expiration time <a href="https://reprap.org/wiki/G-code#M86:_Set_Safety_Timer_expiration_time">M86: Set Safety Timer expiration time</a>	
    When safety timer expires, heatbed and nozzle target temperatures are set to zero.
    #### Usage
    
        M86 [ S ]
    
    #### Parameters
    - `S` - specifies the time in seconds. If a value of 0 is specified, the timer is disabled.
    */
	case 86: 
	  if (code_seen('S')) {
	    safetytimer_inactive_time = code_value() * 1000;
		safetyTimer.start();
	  }
	  break;
#endif

    /*!
	### M92 Set Axis steps-per-unit <a href="https://reprap.org/wiki/G-code#M92:_Set_axis_steps_per_unit">M92: Set axis_steps_per_unit</a>
	Allows programming of steps per unit (usually mm) for motor drives. These values are reset to firmware defaults on power on, unless saved to EEPROM if available (M500 in Marlin)
	#### Usage
    
	    M92 [ X | Y | Z | E ]
	
    #### Parameters
	- `X` - Steps per unit for the X drive
	- `Y` - Steps per unit for the Y drive
	- `Z` - Steps per unit for the Z drive
	- `E` - Steps per unit for the extruder drive
    */
    case 92:
      for(int8_t i=0; i < NUM_AXIS; i++)
      {
        if(code_seen(axis_codes[i]))
        {
          if(i == E_AXIS) { // E
            float value = code_value();
            if(value < 20.0) {
              float factor = cs.axis_steps_per_unit[i] / value; // increase e constants if M92 E14 is given for netfab.
              cs.max_jerk[E_AXIS] *= factor;
              max_feedrate[i] *= factor;
              axis_steps_per_sqr_second[i] *= factor;
            }
            cs.axis_steps_per_unit[i] = value;
#if defined(FILAMENT_SENSOR) && defined(PAT9125)
            fsensor_set_axis_steps_per_unit(value);
#endif
          }
          else {
            cs.axis_steps_per_unit[i] = code_value();
          }
        }
      }
      break;

    /*!
	### M110 - Set Line number <a href="https://reprap.org/wiki/G-code#M110:_Set_Current_Line_Number">M110: Set Current Line Number</a>
	Sets the line number in G-code
	#### Usage
    
	    M110 [ N ]
	
    #### Parameters
	- `N` - Line number
    */
    case 110:
      if (code_seen('N'))
	    gcode_LastN = code_value_long();
    break;

    /*!
    ### M113 - Get or set host keep-alive interval <a href="https://reprap.org/wiki/G-code#M113:_Host_Keepalive">M113: Host Keepalive</a>
    During some lengthy processes, such as G29, Marlin may appear to the host to have “gone away.” The “host keepalive” feature will send messages to the host when Marlin is busy or waiting for user response so the host won’t try to reconnect (or disconnect).
    #### Usage
    
        M113 [ S ]
	
    #### Parameters
	- `S` - Seconds. Default is 2 seconds between "busy" messages
    */
	case 113:
		if (code_seen('S')) {
			host_keepalive_interval = (uint8_t)code_value_short();
//			NOMORE(host_keepalive_interval, 60);
		}
		else {
			SERIAL_ECHO_START;
			SERIAL_ECHOPAIR("M113 S", (unsigned long)host_keepalive_interval);
			SERIAL_PROTOCOLLN("");
		}
		break;

    /*!
	### M115 - Firmware info <a href="https://reprap.org/wiki/G-code#M115:_Get_Firmware_Version_and_Capabilities">M115: Get Firmware Version and Capabilities</a>
    Print the firmware info and capabilities
    Without any arguments, prints Prusa firmware version number, machine type, extruder count and UUID.
    `M115 U` Checks the firmware version provided. If the firmware version provided by the U code is higher than the currently running firmware, it will pause the print for 30s and ask the user to upgrade the firmware.
	
	_Examples:_
	
	`M115` results:
	
	`FIRMWARE_NAME:Prusa-Firmware 3.8.1 based on Marlin FIRMWARE_URL:https://github.com/prusa3d/Prusa-Firmware PROTOCOL_VERSION:1.0 MACHINE_TYPE:Prusa i3 MK3S EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000`
	
	`M115 V` results:
	
	`3.8.1`
	
	`M115 U3.8.2-RC1` results on LCD display for 30s or user interaction:
	
	`New firmware version available: 3.8.2-RC1 Please upgrade.`
    #### Usage
    
        M115 [ V | U ]
	
    #### Parameters
	- V - Report current installed firmware version
	- U - Firmware version provided by G-code to be compared to current one.  
	*/
	case 115: // M115
      if (code_seen('V')) {
          // Report the Prusa version number.
          SERIAL_PROTOCOLLNRPGM(FW_VERSION_STR_P());
      } else if (code_seen('U')) {
          // Check the firmware version provided. If the firmware version provided by the U code is higher than the currently running firmware,
          // pause the print for 30s and ask the user to upgrade the firmware.
          show_upgrade_dialog_if_version_newer(++ strchr_pointer);
      } else {
          SERIAL_ECHOPGM("FIRMWARE_NAME:Prusa-Firmware ");
          SERIAL_ECHORPGM(FW_VERSION_STR_P());
          SERIAL_ECHOPGM(" based on Marlin FIRMWARE_URL:https://github.com/prusa3d/Prusa-Firmware PROTOCOL_VERSION:");
          SERIAL_ECHOPGM(PROTOCOL_VERSION);
          SERIAL_ECHOPGM(" MACHINE_TYPE:");
          SERIAL_ECHOPGM(CUSTOM_MENDEL_NAME); 
          SERIAL_ECHOPGM(" EXTRUDER_COUNT:"); 
          SERIAL_ECHOPGM(STRINGIFY(EXTRUDERS)); 
          SERIAL_ECHOPGM(" UUID:"); 
          SERIAL_ECHOLNPGM(MACHINE_UUID);
      }
      break;

    /*!
	### M114 - Get current position <a href="https://reprap.org/wiki/G-code#M114:_Get_Current_Position">M114: Get Current Position</a>
    */
    case 114:
		gcode_M114();
      break;

      
      /*
        M117 moved up to get the high priority

    case 117: // M117 display message
      starpos = (strchr(strchr_pointer + 5,'*'));
      if(starpos!=NULL)
        *(starpos)='\0';
      lcd_setstatus(strchr_pointer + 5);
      break;*/

    /*!
	### M120 - Enable endstops <a href="https://reprap.org/wiki/G-code#M120:_Enable_endstop_detection">M120: Enable endstop detection</a>
    */
    case 120:
      enable_endstops(false) ;
      break;

    /*!
	### M121 - Disable endstops <a href="https://reprap.org/wiki/G-code#M121:_Disable_endstop_detection">M121: Disable endstop detection</a>
    */
    case 121:
      enable_endstops(true) ;
      break;

    /*!
	### M119 - Get endstop states <a href="https://reprap.org/wiki/G-code#M119:_Get_Endstop_Status">M119: Get Endstop Status</a>
	Returns the current state of the configured X, Y, Z endstops. Takes into account any 'inverted endstop' settings, so one can confirm that the machine is interpreting the endstops correctly.
    */
    case 119:
    SERIAL_PROTOCOLRPGM(_N("Reporting endstop status"));////MSG_M119_REPORT
    SERIAL_PROTOCOLLN("");
      #if defined(X_MIN_PIN) && X_MIN_PIN > -1
        SERIAL_PROTOCOLRPGM(_n("x_min: "));////MSG_X_MIN
        if(READ(X_MIN_PIN)^X_MIN_ENDSTOP_INVERTING){
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_HIT);
        }else{
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_OPEN);
        }
        SERIAL_PROTOCOLLN("");
      #endif
      #if defined(X_MAX_PIN) && X_MAX_PIN > -1
        SERIAL_PROTOCOLRPGM(_n("x_max: "));////MSG_X_MAX
        if(READ(X_MAX_PIN)^X_MAX_ENDSTOP_INVERTING){
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_HIT);
        }else{
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_OPEN);
        }
        SERIAL_PROTOCOLLN("");
      #endif
      #if defined(Y_MIN_PIN) && Y_MIN_PIN > -1
        SERIAL_PROTOCOLRPGM(_n("y_min: "));////MSG_Y_MIN
        if(READ(Y_MIN_PIN)^Y_MIN_ENDSTOP_INVERTING){
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_HIT);
        }else{
          SERIAL_PROTOCOLRPGM(MSG_ENDSTOP_OPEN);
        }
        SERIAL_PROTOCOLLN("");
      #endif
      #if defined(Y_MAX_PIN) && Y_MAX_PIN > -1
        SERIAL_PROTOCOLRPGM(_n("y_max: "));////MSG_Y_MAX
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
      //!@todo update for all axes, use for loop
    

    #ifdef BLINKM
    /*!
	### M150 - Set RGB(W) Color <a href="https://reprap.org/wiki/G-code#M150:_Set_LED_color">M150: Set LED color</a>
	In Prusa Firmware this G-code is deactivated by default, must be turned on in the source code by defining BLINKM and its dependencies.
    #### Usage
    
        M150 [ R | U | B ]
    
    #### Parameters
    - `R` - Red color value
    - `U` - Green color value. It is NOT `G`!
    - `B` - Blue color value
    */
    case 150:
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

    /*!
	### M200 - Set filament diameter <a href="https://reprap.org/wiki/G-code#M200:_Set_filament_diameter">M200: Set filament diameter</a>
	#### Usage
    
	    M200 [ D | T ]
	
    #### Parameters
	  - `D` - Diameter in mm
	  - `T` - Number of extruder (MMUs)
    */
    case 200: // M200 D<millimeters> set filament diameter and set E axis units to cubic millimeters (use S0 to set back to millimeters).
      {

        uint8_t extruder = active_extruder;
        if(code_seen('T')) {
          extruder = code_value();
		  if(extruder >= EXTRUDERS) {
            SERIAL_ECHO_START;
            SERIAL_ECHO(_n("M200 Invalid extruder "));////MSG_M200_INVALID_EXTRUDER
            break;
          }
        }
        if(code_seen('D')) {
		  float diameter = (float)code_value();
		  if (diameter == 0.0) {
			// setting any extruder filament size disables volumetric on the assumption that
			// slicers either generate in extruder values as cubic mm or as as filament feeds
			// for all extruders
		    cs.volumetric_enabled = false;
		  } else {
            cs.filament_size[extruder] = (float)code_value();
			// make sure all extruders have some sane value for the filament size
			cs.filament_size[0] = (cs.filament_size[0] == 0.0 ? DEFAULT_NOMINAL_FILAMENT_DIA : cs.filament_size[0]);
            #if EXTRUDERS > 1
				cs.filament_size[1] = (cs.filament_size[1] == 0.0 ? DEFAULT_NOMINAL_FILAMENT_DIA : cs.filament_size[1]);
				#if EXTRUDERS > 2
					cs.filament_size[2] = (cs.filament_size[2] == 0.0 ? DEFAULT_NOMINAL_FILAMENT_DIA : cs.filament_size[2]);
				#endif
            #endif
			cs.volumetric_enabled = true;
		  }
        } else {
          //reserved for setting filament diameter via UFID or filament measuring device
          break;
        }
		calculate_extruder_multipliers();
      }
      break;

    /*!
	### M201 - Set Print Max Acceleration <a href="https://reprap.org/wiki/G-code#M201:_Set_max_printing_acceleration">M201: Set max printing acceleration</a>
    For each axis individually.
    */
    case 201:
		for (int8_t i = 0; i < NUM_AXIS; i++)
		{
			if (code_seen(axis_codes[i]))
			{
				unsigned long val = code_value();
#ifdef TMC2130
				unsigned long val_silent = val;
				if ((i == X_AXIS) || (i == Y_AXIS))
				{
					if (val > NORMAL_MAX_ACCEL_XY)
						val = NORMAL_MAX_ACCEL_XY;
					if (val_silent > SILENT_MAX_ACCEL_XY)
						val_silent = SILENT_MAX_ACCEL_XY;
				}
				cs.max_acceleration_units_per_sq_second_normal[i] = val;
				cs.max_acceleration_units_per_sq_second_silent[i] = val_silent;
#else //TMC2130
				max_acceleration_units_per_sq_second[i] = val;
#endif //TMC2130
			}
		}
		// steps per sq second need to be updated to agree with the units per sq second (as they are what is used in the planner)
		reset_acceleration_rates();
		break;
    #if 0 // Not used for Sprinter/grbl gen6
    case 202: // M202
      for(int8_t i=0; i < NUM_AXIS; i++) {
        if(code_seen(axis_codes[i])) axis_travel_steps_per_sqr_second[i] = code_value() * cs.axis_steps_per_unit[i];
      }
      break;
    #endif

    /*!
	### M203 - Set Max Feedrate <a href="https://reprap.org/wiki/G-code#M203:_Set_maximum_feedrate">M203: Set maximum feedrate</a>
    For each axis individually.
    */
    case 203: // M203 max feedrate mm/sec
		for (int8_t i = 0; i < NUM_AXIS; i++)
		{
			if (code_seen(axis_codes[i]))
			{
				float val = code_value();
#ifdef TMC2130
				float val_silent = val;
				if ((i == X_AXIS) || (i == Y_AXIS))
				{
					if (val > NORMAL_MAX_FEEDRATE_XY)
						val = NORMAL_MAX_FEEDRATE_XY;
					if (val_silent > SILENT_MAX_FEEDRATE_XY)
						val_silent = SILENT_MAX_FEEDRATE_XY;
				}
				cs.max_feedrate_normal[i] = val;
				cs.max_feedrate_silent[i] = val_silent;
#else //TMC2130
				max_feedrate[i] = val;
#endif //TMC2130
			}
		}
		break;

    /*!
	### M204 - Acceleration settings <a href="https://reprap.org/wiki/G-code#M204:_Set_default_acceleration">M204: Set default acceleration</a>

    #### Old format:
    ##### Usage
    
        M204 [ S | T ]
        
    ##### Parameters
    - `S` - normal moves
    - `T` - filmanent only moves
    
    #### New format:
    ##### Usage
    
        M204 [ P | R | T ]
    
    ##### Parameters
    - `P` - printing moves
    - `R` - filmanent only moves
    - `T` - travel moves (as of now T is ignored)
	*/
    case 204:
      {
        if(code_seen('S')) {
          // Legacy acceleration format. This format is used by the legacy Marlin, MK2 or MK3 firmware,
          // and it is also generated by Slic3r to control acceleration per extrusion type
          // (there is a separate acceleration settings in Slicer for perimeter, first layer etc).
          cs.acceleration = code_value();
          // Interpret the T value as retract acceleration in the old Marlin format.
          if(code_seen('T'))
            cs.retract_acceleration = code_value();
        } else {
          // New acceleration format, compatible with the upstream Marlin.
          if(code_seen('P'))
            cs.acceleration = code_value();
          if(code_seen('R'))
            cs.retract_acceleration = code_value();
          if(code_seen('T')) {
            // Interpret the T value as the travel acceleration in the new Marlin format.
            /*!
            @todo Prusa3D firmware currently does not support travel acceleration value independent from the extruding acceleration value.
            */
            // travel_acceleration = code_value();
          }
        }
      }
      break;

    /*!
	### M205 - Set advanced settings <a href="https://reprap.org/wiki/G-code#M205:_Advanced_settings">M205: Advanced settings</a>
    Set some advanced settings related to movement.
    #### Usage
    
        M205 [ S | T | B | X | Y | Z | E ]
        
    #### Parameters
    - `S` - Minimum feedrate for print moves (unit/s)
    - `T` - Minimum feedrate for travel moves (units/s)
    - `B` - Minimum segment time (us)
    - `X` - Maximum X jerk (units/s)
    - `Y` - Maximum Y jerk (units/s)
    - `Z` - Maximum Z jerk (units/s)
    - `E` - Maximum E jerk (units/s)
    */
    case 205: 
    {
      if(code_seen('S')) cs.minimumfeedrate = code_value();
      if(code_seen('T')) cs.mintravelfeedrate = code_value();
      if(code_seen('B')) cs.minsegmenttime = code_value() ;
      if(code_seen('X')) cs.max_jerk[X_AXIS] = cs.max_jerk[Y_AXIS] = code_value();
      if(code_seen('Y')) cs.max_jerk[Y_AXIS] = code_value();
      if(code_seen('Z')) cs.max_jerk[Z_AXIS] = code_value();
      if(code_seen('E'))
      {
          float e = code_value();
#ifndef LA_NOCOMPAT

          e = la10c_jerk(e);
#endif
          cs.max_jerk[E_AXIS] = e;
      }
      if (cs.max_jerk[X_AXIS] > DEFAULT_XJERK) cs.max_jerk[X_AXIS] = DEFAULT_XJERK;
      if (cs.max_jerk[Y_AXIS] > DEFAULT_YJERK) cs.max_jerk[Y_AXIS] = DEFAULT_YJERK;
    }
    break;

    /*!
	### M206 - Set additional homing offsets <a href="https://reprap.org/wiki/G-code#M206:_Offset_axes">M206: Offset axes</a>
    #### Usage
    
        M206 [ X | Y | Z ]
    
    #### Parameters
    - `X` - X axis offset
    - `Y` - Y axis offset
    - `Z` - Z axis offset
	*/
    case 206:
      for(int8_t i=0; i < 3; i++)
      {
        if(code_seen(axis_codes[i])) cs.add_homing[i] = code_value();
      }
      break;
    #ifdef FWRETRACT

    /*!
	### M207 - Set firmware retraction <a href="https://reprap.org/wiki/G-code#M207:_Set_retract_length">M207: Set retract length</a>
	#### Usage
    
        M207 [ S | F | Z ]
    
    #### Parameters
    - `S` - positive length to retract, in mm
    - `F` - retraction feedrate, in mm/min
    - `Z` - additional zlift/hop
    */
    case 207: //M207 - set retract length S[positive mm] F[feedrate mm/min] Z[additional zlift/hop]
    {
      if(code_seen('S'))
      {
        cs.retract_length = code_value() ;
      }
      if(code_seen('F'))
      {
        cs.retract_feedrate = code_value()/60 ;
      }
      if(code_seen('Z'))
      {
        cs.retract_zlift = code_value() ;
      }
    }break;

    /*!
	### M208 - Set retract recover length <a href="https://reprap.org/wiki/G-code#M208:_Set_unretract_length">M208: Set unretract length</a>
	#### Usage
    
        M208 [ S | F ]
    
    #### Parameters
    - `S` - positive length surplus to the M207 Snnn, in mm
    - `F` - feedrate, in mm/sec
    */
    case 208: // M208 - set retract recover length S[positive mm surplus to the M207 S*] F[feedrate mm/min]
    {
      if(code_seen('S'))
      {
        cs.retract_recover_length = code_value() ;
      }
      if(code_seen('F'))
      {
        cs.retract_recover_feedrate = code_value()/60 ;
      }
    }break;

    /*!
	### M209 - Enable/disable automatict retract <a href="https://reprap.org/wiki/G-code#M209:_Enable_automatic_retract">M209: Enable automatic retract</a>
	This boolean value S 1=true or 0=false enables automatic retract detect if the slicer did not support G10/G11: every normal extrude-only move will be classified as retract depending on the direction.
    #### Usage
    
        M209 [ S ]
        
    #### Parameters
    - `S` - 1=true or 0=false
    */
    case 209: // M209 - S<1=true/0=false> enable automatic retract detect if the slicer did not support G10/11: every normal extrude-only move will be classified as retract depending on the direction.
    {
      if(code_seen('S'))
      {
        int t= code_value() ;
        switch(t)
        {
          case 0: 
          {
            cs.autoretract_enabled=false;
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
            cs.autoretract_enabled=true;
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
            SERIAL_ECHOLNPGM("\"(1)");
        }
      }

    }break;
    #endif // FWRETRACT
    #if EXTRUDERS > 1

    /*!
	### M218 - Set hotend offset <a href="https://reprap.org/wiki/G-code#M218:_Set_Hotend_Offset">M218: Set Hotend Offset</a>
	In Prusa Firmware this G-code is only active if `EXTRUDERS` is higher then 1 in the source code. On Original i3 Prusa MK2/s MK2.5/s MK3/s it is not active.
    #### Usage
    
        M218 [ X | Y ]
        
    #### Parameters
    - `X` - X offset
    - `Y` - Y offset
    */
    case 218: // M218 - set hotend offset (in mm), T<extruder_number> X<offset_on_X> Y<offset_on_Y>
    {
      uint8_t extruder;
      if(setTargetedHotend(218, extruder)){
        break;
      }
      if(code_seen('X'))
      {
        extruder_offset[X_AXIS][extruder] = code_value();
      }
      if(code_seen('Y'))
      {
        extruder_offset[Y_AXIS][extruder] = code_value();
      }
      SERIAL_ECHO_START;
      SERIAL_ECHORPGM(MSG_HOTEND_OFFSET);
      for(extruder = 0; extruder < EXTRUDERS; extruder++)
      {
         SERIAL_ECHO(" ");
         SERIAL_ECHO(extruder_offset[X_AXIS][extruder]);
         SERIAL_ECHO(",");
         SERIAL_ECHO(extruder_offset[Y_AXIS][extruder]);
      }
      SERIAL_ECHOLN("");
    }break;
    #endif

    /*!
	### M220 Set feedrate percentage <a href="https://reprap.org/wiki/G-code#M220:_Set_speed_factor_override_percentage">M220: Set speed factor override percentage</a>
	#### Usage
    
        M220 [ B | S | R ]
    
    #### Parameters
    - `B` - Backup current speed factor
	- `S` - Speed factor override percentage (0..100 or higher)
	- `R` - Restore previous speed factor
    */
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

    /*!
	### M221 - Set extrude factor override percentage <a href="https://reprap.org/wiki/G-code#M221:_Set_extrude_factor_override_percentage">M221: Set extrude factor override percentage</a>
	#### Usage
    
        M221 [ S | T ]
    
    #### Parameters
	- `S` - Extrude factor override percentage (0..100 or higher), default 100%
	- `T` - Extruder drive number (Prusa Firmware only), default 0 if not set.
    */
    case 221: // M221 S<factor in percent>- set extrude factor override percentage
    {
      if(code_seen('S'))
      {
        int tmp_code = code_value();
        if (code_seen('T'))
        {
          uint8_t extruder;
          if(setTargetedHotend(221, extruder)){
            break;
          }
          extruder_multiply[extruder] = tmp_code;
        }
        else
        {
          extrudemultiply = tmp_code ;
        }
      }
      calculate_extruder_multipliers();
    }
    break;

    /*!
    ### M226 - Wait for Pin state <a href="https://reprap.org/wiki/G-code#M226:_Wait_for_pin_state">M226: Wait for pin state</a>
    Wait until the specified pin reaches the state required
    #### Usage
    
        M226 [ P | S ]
    
    #### Parameters
    - `P` - pin number
    - `S` - pin state
    */
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
              lcd_update(0);
            }
          }
        }
      }
    }
    break;

    #if NUM_SERVOS > 0

    /*!
	### M280 - Set/Get servo position <a href="https://reprap.org/wiki/G-code#M280:_Set_servo_position">M280: Set servo position</a>
	In Prusa Firmware this G-code is deactivated by default, must be turned on in the source code.
    #### Usage
    
        M280 [ P | S ]
    
    #### Parameters
    - `P` - Servo index (id)
    - `S` - Target position
    */
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
              _delay(PROBE_SERVO_DEACTIVATION_DELAY);
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
    
    /*!
	### M300 - Play tone <a href="https://reprap.org/wiki/G-code#M300:_Play_beep_sound">M300: Play beep sound</a>
	In Prusa Firmware the defaults are `100Hz` and `1000ms`, so that `M300` without parameters will beep for a second.
    #### Usage
    
        M300 [ S | P ]
    
    #### Parameters
    - `S` - frequency in Hz. Not all firmware versions support this parameter
    - `P` - duration in milliseconds
    */
    case 300: // M300
    {
      int beepS = code_seen('S') ? code_value() : 110;
      int beepP = code_seen('P') ? code_value() : 1000;
      if (beepS > 0)
      {
        #if BEEPER > 0
          Sound_MakeCustom(beepP,beepS,false);
        #endif
      }
      else
      {
        _delay(beepP);
      }
    }
    break;
    #endif // M300

    #ifdef PIDTEMP

    /*!
	### M301 - Set hotend PID <a href="https://reprap.org/wiki/G-code#M301:_Set_PID_parameters">M301: Set PID parameters</a>
	Sets Proportional (P), Integral (I) and Derivative (D) values for hot end.
    See also <a href="https://reprap.org/wiki/PID_Tuning">PID Tuning.</a>
    #### Usage
    
        M301 [ P | I | D | C ]
    
    #### Parameters
    - `P` - proportional (Kp)
    - `I` - integral (Ki)
    - `D` - derivative (Kd)
    - `C` - heating power=Kc*(e_speed0)  
    */
    case 301:
      {
        if(code_seen('P')) cs.Kp = code_value();
        if(code_seen('I')) cs.Ki = scalePID_i(code_value());
        if(code_seen('D')) cs.Kd = scalePID_d(code_value());

        #ifdef PID_ADD_EXTRUSION_RATE
        if(code_seen('C')) Kc = code_value();
        #endif

        updatePID();
        SERIAL_PROTOCOLRPGM(MSG_OK);
        SERIAL_PROTOCOL(" p:");
        SERIAL_PROTOCOL(cs.Kp);
        SERIAL_PROTOCOL(" i:");
        SERIAL_PROTOCOL(unscalePID_i(cs.Ki));
        SERIAL_PROTOCOL(" d:");
        SERIAL_PROTOCOL(unscalePID_d(cs.Kd));
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

    /*!
	### M304 - Set bed PID  <a href="https://reprap.org/wiki/G-code#M304:_Set_PID_parameters_-_Bed">M304: Set PID parameters - Bed</a>
	Sets Proportional (P), Integral (I) and Derivative (D) values for bed.
    See also <a href="https://reprap.org/wiki/PID_Tuning">PID Tuning.</a>
    #### Usage
    
        M304 [ P | I | D ]
    
    #### Parameters
    - `P` - proportional (Kp)
    - `I` - integral (Ki)
    - `D` - derivative (Kd)
    */
    case 304:
      {
        if(code_seen('P')) cs.bedKp = code_value();
        if(code_seen('I')) cs.bedKi = scalePID_i(code_value());
        if(code_seen('D')) cs.bedKd = scalePID_d(code_value());

        updatePID();
       	SERIAL_PROTOCOLRPGM(MSG_OK);
        SERIAL_PROTOCOL(" p:");
        SERIAL_PROTOCOL(cs.bedKp);
        SERIAL_PROTOCOL(" i:");
        SERIAL_PROTOCOL(unscalePID_i(cs.bedKi));
        SERIAL_PROTOCOL(" d:");
        SERIAL_PROTOCOL(unscalePID_d(cs.bedKd));
        SERIAL_PROTOCOLLN("");
      }
      break;
    #endif //PIDTEMP

    /*!
	### M240 - Trigger camera <a href="https://reprap.org/wiki/G-code#M240:_Trigger_camera">M240: Trigger camera</a>
	
	In Prusa Firmware this G-code is deactivated by default, must be turned on in the source code.
	
	You need to (re)define and assign `CHDK` or `PHOTOGRAPH_PIN` the correct pin number to be able to use the feature.
    */
    case 240: // M240  Triggers a camera by emulating a Canon RC-1 : http://www.doc-diy.net/photo/rc-1_hacked/
     {
     	#ifdef CHDK
       
         SET_OUTPUT(CHDK);
         WRITE(CHDK, HIGH);
         chdkHigh = _millis();
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
        _delay(7.33);
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
    #ifdef PREVENT_DANGEROUS_EXTRUDE

    /*!
	### M302 - Allow cold extrude, or set minimum extrude temperature <a href="https://reprap.org/wiki/G-code#M302:_Allow_cold_extrudes">M302: Allow cold extrudes</a>
    This tells the printer to allow movement of the extruder motor above a certain temperature, or if disabled, to allow extruder movement when the hotend is below a safe printing temperature.
    #### Usage
    
        M302 [ S ]
    
    #### Parameters
    - `S` - Cold extrude minimum temperature
    */
    case 302:
    {
	  float temp = .0;
	  if (code_seen('S')) temp=code_value();
      set_extrude_min_temp(temp);
    }
    break;
	#endif

    /*!
	### M303 - PID autotune <a href="https://reprap.org/wiki/G-code#M303:_Run_PID_tuning">M303: Run PID tuning</a>
    PID Tuning refers to a control algorithm used in some repraps to tune heating behavior for hot ends and heated beds. This command generates Proportional (Kp), Integral (Ki), and Derivative (Kd) values for the hotend or bed. Send the appropriate code and wait for the output to update the firmware values.
    #### Usage
    
        M303 [ E | S | C ]
    
    #### Parameters
      - `E` - Extruder, default `E0`. Use `E-1` to calibrate the bed PID
      - `S` - Target temperature, default `210°C` for hotend, 70 for bed
      - `C` - Cycles, default `5`
	*/
    case 303:
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
    
    /*!
	### M400 - Wait for all moves to finish <a href="https://reprap.org/wiki/G-code#M400:_Wait_for_current_moves_to_finish">M400: Wait for current moves to finish</a>
	Finishes all current moves and and thus clears the buffer.
    Equivalent to `G4` with no parameters.
    */
    case 400:
    {
      st_synchronize();
    }
    break;

    /*!
	### M403 - Set filament type (material) for particular extruder and notify the MMU <a href="https://reprap.org/wiki/G-code#M403:_Set_filament_type_.28material.29_for_particular_extruder_and_notify_the_MMU.">M403 - Set filament type (material) for particular extruder and notify the MMU</a>
    Currently three different materials are needed (default, flex and PVA).  
    And storing this information for different load/unload profiles etc. in the future firmware does not have to wait for "ok" from MMU.
    #### Usage
    
        M403 [ E | F ]
    
    #### Parameters
    - `E` - Extruder number. 0-indexed.
    - `F` - Filament type
	*/
    case 403:
	{
		// currently three different materials are needed (default, flex and PVA)
		// add storing this information for different load/unload profiles etc. in the future
		// firmware does not wait for "ok" from mmu
		if (mmu_enabled)
		{
			uint8_t extruder = 255;
			uint8_t filament = FILAMENT_UNDEFINED;
			if(code_seen('E')) extruder = code_value();
			if(code_seen('F')) filament = code_value();
			mmu_set_filament_type(extruder, filament);
		}
	}
	break;

    /*!
	### M500 - Store settings in EEPROM <a href="https://reprap.org/wiki/G-code#M500:_Store_parameters_in_non-volatile_storage">M500: Store parameters in non-volatile storage</a>
	Save current parameters to EEPROM.
    */
    case 500:
    {
        Config_StoreSettings();
    }
    break;

    /*!
	### M501 - Read settings from EEPROM <a href="https://reprap.org/wiki/G-code#M501:_Read_parameters_from_EEPROM">M501: Read parameters from EEPROM</a>
	Set the active parameters to those stored in the EEPROM. This is useful to revert parameters after experimenting with them.
    */
    case 501:
    {
        Config_RetrieveSettings();
    }
    break;

    /*!
	### M502 - Revert all settings to factory default <a href="https://reprap.org/wiki/G-code#M502:_Restore_Default_Settings">M502: Restore Default Settings</a>
	This command resets all tunable parameters to their default values, as set in the firmware's configuration files. This doesn't reset any parameters stored in the EEPROM, so it must be followed by M500 to write the default settings.
    */
    case 502:
    {
        Config_ResetDefault();
    }
    break;

    /*!
	### M503 - Repport all settings currently in memory <a href="https://reprap.org/wiki/G-code#M503:_Report_Current_Settings">M503: Report Current Settings</a>
	This command asks the firmware to reply with the current print settings as set in memory. Settings will differ from EEPROM contents if changed since the last load / save. The reply output includes the G-Code commands to produce each setting. For example, Steps-Per-Unit values are displayed as an M92 command.
    */
    case 503:
    {
        Config_PrintSettings();
    }
    break;

    /*!
	### M509 - Force language selection <a href="https://reprap.org/wiki/G-code#M509:_Force_language_selection">M509: Force language selection</a>
	Resets the language to English.
	Only on Original Prusa i3 MK2.5/s and MK3/s with multiple languages.
	*/
    case 509:
    {
		lang_reset();
        SERIAL_ECHO_START;
        SERIAL_PROTOCOLPGM(("LANG SEL FORCED"));
    }
    break;
    #ifdef ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED

    /*!
	### M540 - Abort print on endstop hit (enable/disable) <a href="https://reprap.org/wiki/G-code#M540_in_Marlin:_Enable.2FDisable_.22Stop_SD_Print_on_Endstop_Hit.22">M540 in Marlin: Enable/Disable "Stop SD Print on Endstop Hit"</a>
	In Prusa Firmware this G-code is deactivated by default, must be turned on in the source code. You must define `ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED`.
    #### Usage
    
        M540 [ S ]
    
    #### Parameters
    - `S` - disabled=0, enabled=1
	*/
    case 540:
    {
        if(code_seen('S')) abort_on_endstop_hit = code_value() > 0;
    }
    break;
    #endif

	/*!
	### M851 - Set Z-Probe Offset <a href="https://reprap.org/wiki/G-code#M851:_Set_Z-Probe_Offset">M851: Set Z-Probe Offset"</a>
    Sets the Z-probe Z offset. This offset is used to determine the actual Z position of the nozzle when using a probe to home Z with G28. This value may also be used by G81 (Prusa) / G29 (Marlin) to apply correction to the Z position.
	This value represents the distance from nozzle to the bed surface at the point where the probe is triggered. This value will be negative for typical switch probes, inductive probes, and setups where the nozzle makes a circuit with a raised metal contact. This setting will be greater than zero on machines where the nozzle itself is used as the probe, pressing down on the bed to press a switch. (This is a common setup on delta machines.)
    #### Usage
    
        M851 [ Z ]
    
    #### Parameters
    - `Z` - Z offset probe to nozzle.
	*/
    #ifdef CUSTOM_M_CODE_SET_Z_PROBE_OFFSET
    case CUSTOM_M_CODE_SET_Z_PROBE_OFFSET:
    {
      float value;
      if (code_seen('Z'))
      {
        value = code_value();
        if ((Z_PROBE_OFFSET_RANGE_MIN <= value) && (value <= Z_PROBE_OFFSET_RANGE_MAX))
        {
          cs.zprobe_zoffset = -value; // compare w/ line 278 of ConfigurationStore.cpp
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
          SERIAL_ECHO(-cs.zprobe_zoffset);
          SERIAL_PROTOCOLLN("");
      }
      break;
    }
    #endif // CUSTOM_M_CODE_SET_Z_PROBE_OFFSET

    #ifdef FILAMENTCHANGEENABLE

    /*!
	### M600 - Initiate Filament change procedure <a href="https://reprap.org/wiki/G-code#M600:_Filament_change_pause">M600: Filament change pause</a>
    Initiates Filament change, it is also used during Filament Runout Sensor process.
	If the `M600` is triggered under 25mm it will do a Z-lift of 25mm to prevent a filament blob.
    #### Usage
    
        M600 [ X | Y | Z | E | L | AUTO ]
      
    - `X`    - X position, default 211
    - `Y`    - Y position, default 0
    - `Z`    - relative lift Z, default 2.
    - `E`    - initial retract, default -2
    - `L`    - later retract distance for removal, default -80
    - `AUTO` - Automatically (only with MMU)
    */
    case 600: //Pause for filament change X[pos] Y[pos] Z[relative lift] E[initial retract] L[later retract distance for removal]
	{
		st_synchronize();

		float x_position = current_position[X_AXIS];
		float y_position = current_position[Y_AXIS];
		float z_shift = 0; // is it necessary to be a float?
		float e_shift_init = 0;
		float e_shift_late = 0;
		bool automatic = false;
		
        //Retract extruder
        if(code_seen('E'))
        {
          e_shift_init = code_value();
        }
        else
        {
          #ifdef FILAMENTCHANGE_FIRSTRETRACT
            e_shift_init = FILAMENTCHANGE_FIRSTRETRACT ;
          #endif
        }

		//currently don't work as we are using the same unload sequence as in M702, needs re-work 
		if (code_seen('L'))
		{
			e_shift_late = code_value();
		}
		else
		{
		  #ifdef FILAMENTCHANGE_FINALRETRACT
			e_shift_late = FILAMENTCHANGE_FINALRETRACT;
		  #endif	
		}

        //Lift Z
        if(code_seen('Z'))
        {
          z_shift = code_value();
        }
        else
        {
			z_shift = gcode_M600_filament_change_z_shift<uint8_t>();
        }
		//Move XY to side
        if(code_seen('X'))
        {
          x_position = code_value();
        }
        else
        {
          #ifdef FILAMENTCHANGE_XPOS
			x_position = FILAMENTCHANGE_XPOS;
          #endif
        }
        if(code_seen('Y'))
        {
          y_position = code_value();
        }
        else
        {
          #ifdef FILAMENTCHANGE_YPOS
            y_position = FILAMENTCHANGE_YPOS ;
          #endif
        }

		if (mmu_enabled && code_seen("AUTO"))
			automatic = true;

		gcode_M600(automatic, x_position, y_position, z_shift, e_shift_init, e_shift_late);
	
	}
    break;
    #endif //FILAMENTCHANGEENABLE

    /*!
    ### M601 - Pause print <a href="https://reprap.org/wiki/G-code#M601:_Pause_print">M601: Pause print</a>
    */
    /*!
    ### M125 - Pause print (TODO: not implemented)
    */
    /*!
    ### M25 - Pause SD print <a href="https://reprap.org/wiki/G-code#M25:_Pause_SD_print">M25: Pause SD print</a>
    */
	case 25:
	case 601:
	{
        if (!isPrintPaused)
        {
            st_synchronize();
            cmdqueue_pop_front(); //trick because we want skip this command (M601) after restore
            lcd_pause_print();
        }
	}
	break;

    /*!
	### M602 - Resume print <a href="https://reprap.org/wiki/G-code#M602:_Resume_print">M602: Resume print</a>
    */
	case 602: {
	  if (isPrintPaused)
          lcd_resume_print();
	}
	break;

    /*!
    ### M603 - Stop print <a href="https://reprap.org/wiki/G-code#M603:_Stop_print">M603: Stop print</a>
    */
	case 603: {
		lcd_print_stop();
	}
	break;

#ifdef PINDA_THERMISTOR
    /*!
	### M860 - Wait for extruder temperature (PINDA) <a href="https://reprap.org/wiki/G-code#M860_Wait_for_Probe_Temperature">M860 Wait for Probe Temperature</a>
    Wait for PINDA thermistor to reach target temperature
    #### Usage
    
        M860 [ S ]
    
    #### Parameters
    - `S` - Target temperature
    */
	case 860: 
	{
		int set_target_pinda = 0;

		if (code_seen('S')) {
			set_target_pinda = code_value();
		}
		else {
			break;
		}

		LCD_MESSAGERPGM(_T(MSG_PLEASE_WAIT));

		SERIAL_PROTOCOLPGM("Wait for PINDA target temperature:");
		SERIAL_PROTOCOL(set_target_pinda);
		SERIAL_PROTOCOLLN("");

		codenum = _millis();
		cancel_heatup = false;

		bool is_pinda_cooling = false;
		if ((degTargetBed() == 0) && (degTargetHotend(0) == 0)) {
		    is_pinda_cooling = true;
		}

		while ( ((!is_pinda_cooling) && (!cancel_heatup) && (current_temperature_pinda < set_target_pinda)) || (is_pinda_cooling && (current_temperature_pinda > set_target_pinda)) ) {
			if ((_millis() - codenum) > 1000) //Print Temp Reading every 1 second while waiting.
			{
				SERIAL_PROTOCOLPGM("P:");
				SERIAL_PROTOCOL_F(current_temperature_pinda, 1);
				SERIAL_PROTOCOLPGM("/");
				SERIAL_PROTOCOL(set_target_pinda);
				SERIAL_PROTOCOLLN("");
				codenum = _millis();
			}
			manage_heater();
			manage_inactivity();
			lcd_update(0);
		}
		LCD_MESSAGERPGM(MSG_OK);

		break;
	}
 
    /*!
    ### M861 - Set/Get PINDA temperature compensation offsets <a href="https://reprap.org/wiki/G-code#M861_Set_Probe_Thermal_Compensation">M861 Set Probe Thermal Compensation</a>
    Set compensation ustep value `S` for compensation table index `I`.
    #### Usage
    
        M861 [ ? | ! | Z | S | I ]
    
    #### Parameters
    - `?` - Print current EEPROM offset values
    - `!` - Set factory default values
    - `Z` - Set all values to 0 (effectively disabling PINDA temperature compensation)
    - `S` - Microsteps
    - `I` - Table index
    */
	case 861:
		if (code_seen('?')) { // ? - Print out current EEPROM offset values
			uint8_t cal_status = calibration_status_pinda();
			int16_t usteps = 0;
			cal_status ? SERIAL_PROTOCOLLN("PINDA cal status: 1") : SERIAL_PROTOCOLLN("PINDA cal status: 0");
			SERIAL_PROTOCOLLN("index, temp, ustep, um");
			for (uint8_t i = 0; i < 6; i++)
			{
				if(i>0) EEPROM_read_B(EEPROM_PROBE_TEMP_SHIFT + (i-1) * 2, &usteps);
				float mm = ((float)usteps) / cs.axis_steps_per_unit[Z_AXIS];
				i == 0 ? SERIAL_PROTOCOLPGM("n/a") : SERIAL_PROTOCOL(i - 1);
				SERIAL_PROTOCOLPGM(", ");
				SERIAL_PROTOCOL(35 + (i * 5));
				SERIAL_PROTOCOLPGM(", ");
				SERIAL_PROTOCOL(usteps);
				SERIAL_PROTOCOLPGM(", ");
				SERIAL_PROTOCOL(mm * 1000);
				SERIAL_PROTOCOLLN("");
			}
		}
		else if (code_seen('!')) { // ! - Set factory default values
			eeprom_write_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, 1);
			int16_t z_shift = 8;    //40C -  20um -   8usteps
			EEPROM_save_B(EEPROM_PROBE_TEMP_SHIFT, &z_shift);
			z_shift = 24;   //45C -  60um -  24usteps
			EEPROM_save_B(EEPROM_PROBE_TEMP_SHIFT + 2, &z_shift);
			z_shift = 48;   //50C - 120um -  48usteps
			EEPROM_save_B(EEPROM_PROBE_TEMP_SHIFT + 4, &z_shift);
			z_shift = 80;   //55C - 200um -  80usteps
			EEPROM_save_B(EEPROM_PROBE_TEMP_SHIFT + 6, &z_shift);
			z_shift = 120;  //60C - 300um - 120usteps
			EEPROM_save_B(EEPROM_PROBE_TEMP_SHIFT + 8, &z_shift);
			SERIAL_PROTOCOLLN("factory restored");
		}
		else if (code_seen('Z')) { // Z - Set all values to 0 (effectively disabling PINDA temperature compensation)
			eeprom_write_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, 1);
			int16_t z_shift = 0;
			for (uint8_t i = 0; i < 5; i++) EEPROM_save_B(EEPROM_PROBE_TEMP_SHIFT + i * 2, &z_shift);
			SERIAL_PROTOCOLLN("zerorized");
		}
		else if (code_seen('S')) { // Sxxx Iyyy - Set compensation ustep value S for compensation table index I
			int16_t usteps = code_value();
			if (code_seen('I')) {
			    uint8_t index = code_value();
				if (index < 5) {
					EEPROM_save_B(EEPROM_PROBE_TEMP_SHIFT + index * 2, &usteps);
					SERIAL_PROTOCOLLN("OK");
					SERIAL_PROTOCOLLN("index, temp, ustep, um");
					for (uint8_t i = 0; i < 6; i++)
					{
						usteps = 0;
						if (i>0) EEPROM_read_B(EEPROM_PROBE_TEMP_SHIFT + (i - 1) * 2, &usteps);
						float mm = ((float)usteps) / cs.axis_steps_per_unit[Z_AXIS];
						i == 0 ? SERIAL_PROTOCOLPGM("n/a") : SERIAL_PROTOCOL(i - 1);
						SERIAL_PROTOCOLPGM(", ");
						SERIAL_PROTOCOL(35 + (i * 5));
						SERIAL_PROTOCOLPGM(", ");
						SERIAL_PROTOCOL(usteps);
						SERIAL_PROTOCOLPGM(", ");
						SERIAL_PROTOCOL(mm * 1000);
						SERIAL_PROTOCOLLN("");
					}
				}
			}
		}
		else {
			SERIAL_PROTOCOLPGM("no valid command");
		}
		break;

#endif //PINDA_THERMISTOR
   
    /*!
	### M862 - Print checking <a href="https://reprap.org/wiki/G-code#M862:_Print_checking">M862: Print checking</a>
    Checks the parameters of the printer and gcode and performs compatibility check
	
      - M862.1 { P<nozzle_diameter> | Q } 0.25/0.40/0.60
      - M862.2 { P<model_code> | Q }
      - M862.3 { P"<model_name>" | Q }
      - M862.4 { P<fw_version> | Q }
      - M862.5 { P<gcode_level> | Q }
    
    When run with P<> argument, the check is performed against the input value.
    When run with Q argument, the current value is shown.
	
    M862.3 accepts text identifiers of printer types too.
    The syntax of M862.3 is (note the quotes around the type):
	  
          M862.3 P "MK3S"
	  
    Accepted printer type identifiers and their numeric counterparts:
	
      - MK1         (100)
      - MK2         (200)       
      - MK2MM       (201)     
      - MK2S        (202)      
      - MK2SMM      (203)    
      - MK2.5       (250)     
      - MK2.5MMU2   (20250) 
      - MK2.5S      (252)    
      - MK2.5SMMU2S (20252)
      - MK3         (300)
      - MK3MMU2     (20300)
      - MK3S        (302)
      - MK3SMMU2S   (20302)
	
    */
    case 862: // M862: print checking
          float nDummy;
          uint8_t nCommand;
          nCommand=(uint8_t)(modff(code_value_float(),&nDummy)*10.0+0.5);
          switch((ClPrintChecking)nCommand)
               {
               case ClPrintChecking::_Nozzle:     // ~ .1
                    uint16_t nDiameter;
                    if(code_seen('P'))
                         {
                         nDiameter=(uint16_t)(code_value()*1000.0+0.5); // [,um]
                         nozzle_diameter_check(nDiameter);
                         }
/*
                    else if(code_seen('S')&&farm_mode)
                         {
                         nDiameter=(uint16_t)(code_value()*1000.0+0.5); // [,um]
                         eeprom_update_byte((uint8_t*)EEPROM_NOZZLE_DIAMETER,(uint8_t)ClNozzleDiameter::_Diameter_Undef); // for correct synchronization after farm-mode exiting
                         eeprom_update_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM,nDiameter);
                         }
*/
                    else if(code_seen('Q'))
                         SERIAL_PROTOCOLLN((float)eeprom_read_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM)/1000.0);
                    break;
               case ClPrintChecking::_Model:      // ~ .2
                    if(code_seen('P'))
                         {
                         uint16_t nPrinterModel;
                         nPrinterModel=(uint16_t)code_value_long();
                         printer_model_check(nPrinterModel);
                         }
                    else if(code_seen('Q'))
                         SERIAL_PROTOCOLLN(nPrinterType);
                    break;
               case ClPrintChecking::_Smodel:     // ~ .3
                    if(code_seen('P'))
                         printer_smodel_check(strchr_pointer);
                    else if(code_seen('Q'))
                         SERIAL_PROTOCOLLNRPGM(sPrinterName);
                    break;
               case ClPrintChecking::_Version:    // ~ .4
                    if(code_seen('P'))
                         fw_version_check(++strchr_pointer);
                    else if(code_seen('Q'))
                         SERIAL_PROTOCOLLN(FW_VERSION);
                    break;
               case ClPrintChecking::_Gcode:      // ~ .5
                    if(code_seen('P'))
                         {
                         uint16_t nGcodeLevel;
                         nGcodeLevel=(uint16_t)code_value_long();
                         gcode_level_check(nGcodeLevel);
                         }
                    else if(code_seen('Q'))
                         SERIAL_PROTOCOLLN(GCODE_LEVEL);
                    break;
               }
    break;

#ifdef LIN_ADVANCE
    /*!
	### M900 - Set Linear advance options <a href="https://reprap.org/wiki/G-code#M900_Set_Linear_Advance_Scaling_Factors">M900 Set Linear Advance Scaling Factors</a>
	Sets the advance extrusion factors for Linear Advance. If any of the R, W, H, or D parameters are set to zero the ratio will be computed dynamically during printing.
	#### Usage
    
        M900 [ K | R | W | H | D]
    
    #### Parameters
    - `K` -  Advance K factor
    - `R` - Set ratio directly (overrides WH/D)
    - `W` - Width
    - `H` - Height
    - `D` - Diameter Set ratio from WH/D
    */
    case 900:
        gcode_M900();
    break;
#endif

    /*!
	### M907 - Set digital trimpot motor current in mA using axis codes <a href="https://reprap.org/wiki/G-code#M907:_Set_digital_trimpot_motor">M907: Set digital trimpot motor</a>
	Set digital trimpot motor current using axis codes (X, Y, Z, E, B, S).
	#### Usage
    
        M907 [ X | Y | Z | E | B | S ]
	
    #### Parameters
    - `X` - X motor driver
    - `Y` - Y motor driver
    - `Z` - Z motor driver
    - `E` - Extruder motor driver
    - `B` - Second Extruder motor driver
    - `S` - All motors
    */
    case 907:
    {
#ifdef TMC2130
        // See tmc2130_cur2val() for translation to 0 .. 63 range
        for (int i = 0; i < NUM_AXIS; i++)
			if(code_seen(axis_codes[i]))
			{
				long cur_mA = code_value_long();
				uint8_t val = tmc2130_cur2val(cur_mA);
				tmc2130_set_current_h(i, val);
				tmc2130_set_current_r(i, val);
				//if (i == E_AXIS) printf_P(PSTR("E-axis current=%ldmA\n"), cur_mA);
			}

#else //TMC2130
      #if defined(DIGIPOTSS_PIN) && DIGIPOTSS_PIN > -1
        for(int i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) st_current_set(i,code_value());
        if(code_seen('B')) st_current_set(4,code_value());
        if(code_seen('S')) for(int i=0;i<=4;i++) st_current_set(i,code_value());
      #endif
      #ifdef MOTOR_CURRENT_PWM_XY_PIN
        if(code_seen('X')) st_current_set(0, code_value());
      #endif
      #ifdef MOTOR_CURRENT_PWM_Z_PIN
        if(code_seen('Z')) st_current_set(1, code_value());
      #endif
      #ifdef MOTOR_CURRENT_PWM_E_PIN
        if(code_seen('E')) st_current_set(2, code_value());
      #endif
#endif //TMC2130
    }
    break;

    /*!
	### M908 - Control digital trimpot directly <a href="https://reprap.org/wiki/G-code#M908:_Control_digital_trimpot_directly">M908: Control digital trimpot directly</a>
	In Prusa Firmware this G-code is deactivated by default, must be turned on in the source code. Not usable on Prusa printers.
    #### Usage
    
        M908 [ P | S ]
    
    #### Parameters
    - `P` - channel
    - `S` - current
    */
    case 908:
    {
      #if defined(DIGIPOTSS_PIN) && DIGIPOTSS_PIN > -1
        uint8_t channel,current;
        if(code_seen('P')) channel=code_value();
        if(code_seen('S')) current=code_value();
        digitalPotWrite(channel, current);
      #endif
    }
    break;

#ifdef TMC2130_SERVICE_CODES_M910_M918

    /*!
	### M910 - TMC2130 init <a href="https://reprap.org/wiki/G-code#M910:_TMC2130_init">M910: TMC2130 init</a>
	Not active in default, only if `TMC2130_SERVICE_CODES_M910_M918` is defined in source code.
	
    */
	case 910:
    {
		tmc2130_init();
    }
    break;

    /*!
    ### M911 - Set TMC2130 holding currents <a href="https://reprap.org/wiki/G-code#M911:_Set_TMC2130_holding_currents">M911: Set TMC2130 holding currents</a>
	Not active in default, only if `TMC2130_SERVICE_CODES_M910_M918` is defined in source code.
    #### Usage
    
        M911 [ X | Y | Z | E ]
    
    #### Parameters
    - `X` - X stepper driver holding current value
    - `Y` - Y stepper driver holding current value
    - `Z` - Z stepper driver holding current value
    - `E` - Extruder stepper driver holding current value
    */
	case 911: 
    {
		if (code_seen('X')) tmc2130_set_current_h(0, code_value());
		if (code_seen('Y')) tmc2130_set_current_h(1, code_value());
        if (code_seen('Z')) tmc2130_set_current_h(2, code_value());
        if (code_seen('E')) tmc2130_set_current_h(3, code_value());
    }
    break;

    /*!
	### M912 - Set TMC2130 running currents <a href="https://reprap.org/wiki/G-code#M912:_Set_TMC2130_running_currents">M912: Set TMC2130 running currents</a>
	Not active in default, only if `TMC2130_SERVICE_CODES_M910_M918` is defined in source code.
    #### Usage
    
        M912 [ X | Y | Z | E ]
    
    #### Parameters
    - `X` - X stepper driver running current value
    - `Y` - Y stepper driver running current value
    - `Z` - Z stepper driver running current value
    - `E` - Extruder stepper driver running current value
    */
	case 912: 
    {
		if (code_seen('X')) tmc2130_set_current_r(0, code_value());
		if (code_seen('Y')) tmc2130_set_current_r(1, code_value());
        if (code_seen('Z')) tmc2130_set_current_r(2, code_value());
        if (code_seen('E')) tmc2130_set_current_r(3, code_value());
    }
    break;

    /*!
	### M913 - Print TMC2130 currents <a href="https://reprap.org/wiki/G-code#M913:_Print_TMC2130_currents">M913: Print TMC2130 currents</a>
	Not active in default, only if `TMC2130_SERVICE_CODES_M910_M918` is defined in source code.
	Shows TMC2130 currents.
    */
	case 913:
    {
		tmc2130_print_currents();
    }
    break;

    /*!
	### M914 - Set TMC2130 normal mode <a href="https://reprap.org/wiki/G-code#M914:_Set_TMC2130_normal_mode">M914: Set TMC2130 normal mode</a>
	Not active in default, only if `TMC2130_SERVICE_CODES_M910_M918` is defined in source code.
    */
    case 914:
    {
		tmc2130_mode = TMC2130_MODE_NORMAL;
		update_mode_profile();
		tmc2130_init();
    }
    break;

    /*!
	### M915 - Set TMC2130 silent mode <a href="https://reprap.org/wiki/G-code#M915:_Set_TMC2130_silent_mode">M915: Set TMC2130 silent mode</a>
	Not active in default, only if `TMC2130_SERVICE_CODES_M910_M918` is defined in source code.
    */
    case 915:
    {
		tmc2130_mode = TMC2130_MODE_SILENT;
		update_mode_profile();
		tmc2130_init();
    }
    break;

    /*!
	### M916 - Set TMC2130 Stallguard sensitivity threshold <a href="https://reprap.org/wiki/G-code#M916:_Set_TMC2130_Stallguard_sensitivity_threshold">M916: Set TMC2130 Stallguard sensitivity threshold</a>
	Not active in default, only if `TMC2130_SERVICE_CODES_M910_M918` is defined in source code.
    #### Usage
    
        M916 [ X | Y | Z | E ]
    
    #### Parameters
    - `X` - X stepper driver stallguard sensitivity threshold value
    - `Y` - Y stepper driver stallguard sensitivity threshold value
    - `Z` - Z stepper driver stallguard sensitivity threshold value
    - `E` - Extruder stepper driver stallguard sensitivity threshold value
    */
	case 916:
    {
		if (code_seen('X')) tmc2130_sg_thr[X_AXIS] = code_value();
		if (code_seen('Y')) tmc2130_sg_thr[Y_AXIS] = code_value();
		if (code_seen('Z')) tmc2130_sg_thr[Z_AXIS] = code_value();
		if (code_seen('E')) tmc2130_sg_thr[E_AXIS] = code_value();
		for (uint8_t a = X_AXIS; a <= E_AXIS; a++)
			printf_P(_N("tmc2130_sg_thr[%c]=%d\n"), "XYZE"[a], tmc2130_sg_thr[a]);
    }
    break;

    /*!
	### M917 - Set TMC2130 PWM amplitude offset (pwm_ampl) <a href="https://reprap.org/wiki/G-code#M917:_Set_TMC2130_PWM_amplitude_offset_.28pwm_ampl.29">M917: Set TMC2130 PWM amplitude offset (pwm_ampl)</a>
	Not active in default, only if `TMC2130_SERVICE_CODES_M910_M918` is defined in source code.
    #### Usage
    
        M917 [ X | Y | Z | E ]
    
    #### Parameters
    - `X` - X stepper driver PWM amplitude offset value
    - `Y` - Y stepper driver PWM amplitude offset value
    - `Z` - Z stepper driver PWM amplitude offset value
    - `E` - Extruder stepper driver PWM amplitude offset value
    */
	case 917:
    {
		if (code_seen('X')) tmc2130_set_pwm_ampl(0, code_value());
		if (code_seen('Y')) tmc2130_set_pwm_ampl(1, code_value());
        if (code_seen('Z')) tmc2130_set_pwm_ampl(2, code_value());
        if (code_seen('E')) tmc2130_set_pwm_ampl(3, code_value());
    }
    break;

    /*!
	### M918 - Set TMC2130 PWM amplitude gradient (pwm_grad) <a href="https://reprap.org/wiki/G-code#M918:_Set_TMC2130_PWM_amplitude_gradient_.28pwm_grad.29">M918: Set TMC2130 PWM amplitude gradient (pwm_grad)</a>
	Not active in default, only if `TMC2130_SERVICE_CODES_M910_M918` is defined in source code.
    #### Usage
    
        M918 [ X | Y | Z | E ]
    
    #### Parameters
    - `X` - X stepper driver PWM amplitude gradient value
    - `Y` - Y stepper driver PWM amplitude gradient value
    - `Z` - Z stepper driver PWM amplitude gradient value
    - `E` - Extruder stepper driver PWM amplitude gradient value
    */
	case 918:
    {
		if (code_seen('X')) tmc2130_set_pwm_grad(0, code_value());
		if (code_seen('Y')) tmc2130_set_pwm_grad(1, code_value());
        if (code_seen('Z')) tmc2130_set_pwm_grad(2, code_value());
        if (code_seen('E')) tmc2130_set_pwm_grad(3, code_value());
    }
    break;

#endif //TMC2130_SERVICE_CODES_M910_M918

    /*!
	### M350 - Set microstepping mode <a href="https://reprap.org/wiki/G-code#M350:_Set_microstepping_mode">M350: Set microstepping mode</a>
    Printers with TMC2130 drivers have `X`, `Y`, `Z` and `E` as options. The steps-per-unit value is updated accordingly. Not all resolutions are valid!
    Printers without TMC2130 drivers also have `B` and `S` options. In this case, the steps-per-unit value in not changed!
    #### Usage
    
        M350 [ X | Y | Z | E | B | S ]
    
    #### Parameters
    - `X` - X new resolution
    - `Y` - Y new resolution
    - `Z` - Z new resolution
    - `E` - E new resolution
    
    Only valid for MK2.5(S) or printers without TMC2130 drivers
    - `B` - Second extruder new resolution
    - `S` - All axes new resolution
    */
    case 350: 
    {
	#ifdef TMC2130
		for (int i=0; i<NUM_AXIS; i++) 
		{
			if(code_seen(axis_codes[i]))
			{
				uint16_t res_new = code_value();
				bool res_valid = (res_new == 8) || (res_new == 16) || (res_new == 32); // resolutions valid for all axis
				res_valid |= (i != E_AXIS) && ((res_new == 1) || (res_new == 2) || (res_new == 4)); // resolutions valid for X Y Z only
				res_valid |= (i == E_AXIS) && ((res_new == 64) || (res_new == 128)); // resolutions valid for E only
				if (res_valid)
				{
					st_synchronize();
					uint16_t res = tmc2130_get_res(i);
					tmc2130_set_res(i, res_new);
					cs.axis_ustep_resolution[i] = res_new;
					if (res_new > res)
					{
						uint16_t fac = (res_new / res);
						cs.axis_steps_per_unit[i] *= fac;
						position[i] *= fac;
					}
					else
					{
						uint16_t fac = (res / res_new);
						cs.axis_steps_per_unit[i] /= fac;
						position[i] /= fac;
					}
#if defined(FILAMENT_SENSOR) && defined(PAT9125)
                    if (i == E_AXIS)
                        fsensor_set_axis_steps_per_unit(cs.axis_steps_per_unit[i]);
#endif
				}
			}
		}
	#else //TMC2130
      #if defined(X_MS1_PIN) && X_MS1_PIN > -1
        if(code_seen('S')) for(int i=0;i<=4;i++) microstep_mode(i,code_value());
        for(int i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) microstep_mode(i,(uint8_t)code_value());
        if(code_seen('B')) microstep_mode(4,code_value());
        microstep_readings();
      #endif
	#endif //TMC2130
    }
    break;

    /*!
	### M351 - Toggle Microstep Pins <a href="https://reprap.org/wiki/G-code#M351:_Toggle_MS1_MS2_pins_directly">M351: Toggle MS1 MS2 pins directly</a>
    Toggle MS1 MS2 pins directly.
    #### Usage
    
        M351 [B<0|1>] [E<0|1>] S<1|2> [X<0|1>] [Y<0|1>] [Z<0|1>]
    
    #### Parameters
    - `X` - Update X axis
    - `Y` - Update Y axis
    - `Z` - Update Z axis
    - `E` - Update E axis
    - `S` - which MSx pin to toggle
    - `B` - new pin value
    */
    case 351:
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

  /*!
  ### M701 - Load filament <a href="https://reprap.org/wiki/G-code#M701:_Load_filament">M701: Load filament</a>
  
  */
	case 701:
	{
		if (mmu_enabled && code_seen('E'))
			tmp_extruder = code_value();
		gcode_M701();
	}
	break;

    /*!
    ### M702 - Unload filament <a href="https://reprap.org/wiki/G-code#M702:_Unload_filament">G32: Undock Z Probe sled</a>
    #### Usage
    
        M702 [ U | C ]
    
    #### Parameters
    - `U` - Unload all filaments used in current print
    - `C` - Unload just current filament
    - without any parameters unload all filaments
    */
	case 702:
	{
#ifdef SNMM
		if (code_seen('U'))
			extr_unload_used(); //! if "U" unload all filaments which were used in current print
		else if (code_seen('C'))
			extr_unload(); //! if "C" unload just current filament
		else
			extr_unload_all(); //! otherwise unload all filaments
#else
		if (code_seen('C')) {
			if(mmu_enabled) extr_unload(); //! if "C" unload current filament; if mmu is not present no action is performed
		}
		else {
			if(mmu_enabled) extr_unload(); //! unload current filament
			else unload_filament();
		}

#endif //SNMM
	}
	break;

    /*!
	### M999 - Restart after being stopped <a href="https://reprap.org/wiki/G-code#M999:_Restart_after_being_stopped_by_error">M999: Restart after being stopped by error</a>
    @todo Usually doesn't work. Should be fixed or removed. Most of the time, if `Stopped` it set, the print fails and is unrecoverable.
    */
    case 999:
      Stopped = false;
      lcd_reset_alert_level();
      gcode_LastN = Stopped_gcode_LastN;
      FlushSerialRequestResend();
    break;
	/*!
	#### End of M-Commands
    */
	default: 
		printf_P(PSTR("Unknown M code: %s \n"), cmdbuffer + bufindr + CMDHDRSIZE);
    }
//	printf_P(_N("END M-CODE=%u\n"), mcode_in_progress);
	mcode_in_progress = 0;
	}
  }
  // end if(code_seen('M')) (end of M codes)
  /*!
  -----------------------------------------------------------------------------------------
  # T Codes
  T<extruder nr.> - select extruder in case of multi extruder printer. select filament in case of MMU_V2.
  #### For MMU_V2:
  T<n> Gcode to extrude at least 38.10 mm at feedrate 19.02 mm/s must follow immediately to load to extruder wheels.
  @n T? Gcode to extrude shouldn't have to follow, load to extruder wheels is done automatically
  @n Tx Same as T?, except nozzle doesn't have to be preheated. Tc must be placed after extruder nozzle is preheated to finish filament load.
  @n Tc Load to nozzle after filament was prepared by Tc and extruder nozzle is already heated.
  */
  else if(code_seen('T'))
  {
      int index;
      bool load_to_nozzle = false;
      for (index = 1; *(strchr_pointer + index) == ' ' || *(strchr_pointer + index) == '\t'; index++);

	  *(strchr_pointer + index) = tolower(*(strchr_pointer + index));

      if ((*(strchr_pointer + index) < '0' || *(strchr_pointer + index) > '4') && *(strchr_pointer + index) != '?' && *(strchr_pointer + index) != 'x' && *(strchr_pointer + index) != 'c') {
          SERIAL_ECHOLNPGM("Invalid T code.");
      }
	  else if (*(strchr_pointer + index) == 'x'){ //load to bondtech gears; if mmu is not present do nothing
		if (mmu_enabled)
		{
			tmp_extruder = choose_menu_P(_T(MSG_CHOOSE_FILAMENT), _T(MSG_FILAMENT));
			if ((tmp_extruder == mmu_extruder) && mmu_fil_loaded) //dont execute the same T-code twice in a row
			{
				printf_P(PSTR("Duplicate T-code ignored.\n"));
			}
			else
			{
				st_synchronize();
				mmu_command(MmuCmd::T0 + tmp_extruder);
				manage_response(true, true, MMU_TCODE_MOVE);
			}
		}
	  }
	  else if (*(strchr_pointer + index) == 'c') { //load to from bondtech gears to nozzle (nozzle should be preheated)
	  	if (mmu_enabled) 
		{
			st_synchronize();
			mmu_continue_loading(is_usb_printing  || (lcd_commands_type == LcdCommands::Layer1Cal));
			mmu_extruder = tmp_extruder; //filament change is finished
			mmu_load_to_nozzle();
		}
	  }
      else {
          if (*(strchr_pointer + index) == '?')
          {
              if(mmu_enabled)
              {
                  tmp_extruder = choose_menu_P(_T(MSG_CHOOSE_FILAMENT), _T(MSG_FILAMENT));
                  load_to_nozzle = true;
              } else
              {
                  tmp_extruder = choose_menu_P(_T(MSG_CHOOSE_EXTRUDER), _T(MSG_EXTRUDER));
              }
          }
          else {
              tmp_extruder = code_value();
              if (mmu_enabled && lcd_autoDepleteEnabled())
              {
                  tmp_extruder = ad_getAlternative(tmp_extruder);
              }
          }
          st_synchronize();
          snmm_filaments_used |= (1 << tmp_extruder); //for stop print

          if (mmu_enabled)
          {
              if ((tmp_extruder == mmu_extruder) && mmu_fil_loaded) //dont execute the same T-code twice in a row
              {
                  printf_P(PSTR("Duplicate T-code ignored.\n"));
              }
			  else
			  {
#if defined(MMU_HAS_CUTTER) && defined(MMU_ALWAYS_CUT)
			      if (EEPROM_MMU_CUTTER_ENABLED_always == eeprom_read_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED))
                  {
                      mmu_command(MmuCmd::K0 + tmp_extruder);
                      manage_response(true, true, MMU_UNLOAD_MOVE);
                  }
#endif //defined(MMU_HAS_CUTTER) && defined(MMU_ALWAYS_CUT)
				  mmu_command(MmuCmd::T0 + tmp_extruder);
				  manage_response(true, true, MMU_TCODE_MOVE);
		          mmu_continue_loading(is_usb_printing  || (lcd_commands_type == LcdCommands::Layer1Cal));

				  mmu_extruder = tmp_extruder; //filament change is finished

				  if (load_to_nozzle)// for single material usage with mmu
				  {
					  mmu_load_to_nozzle();
				  }
			  }
          }
          else
          {
#ifdef SNMM
              mmu_extruder = tmp_extruder;

              _delay(100);

              disable_e0();
              disable_e1();
              disable_e2();

              pinMode(E_MUX0_PIN, OUTPUT);
              pinMode(E_MUX1_PIN, OUTPUT);

              _delay(100);
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
              _delay(100);

#else //SNMM
              if (tmp_extruder >= EXTRUDERS) {
                  SERIAL_ECHO_START;
                  SERIAL_ECHOPGM("T");
                  SERIAL_PROTOCOLLN((int)tmp_extruder);
                  SERIAL_ECHOLNRPGM(_n("Invalid extruder"));////MSG_INVALID_EXTRUDER
              }
              else {
#if EXTRUDERS > 1
                  boolean make_move = false;
#endif
                  if (code_seen('F')) {
#if EXTRUDERS > 1
                      make_move = true;
#endif
                      next_feedrate = code_value();
                      if (next_feedrate > 0.0) {
                          feedrate = next_feedrate;
                      }
                  }
#if EXTRUDERS > 1
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
                  SERIAL_ECHORPGM(_n("Active Extruder: "));////MSG_ACTIVE_EXTRUDER
                  SERIAL_PROTOCOLLN((int)active_extruder);
              }

#endif //SNMM
          }
      }
  } // end if(code_seen('T')) (end of T codes)
  /*!
  #### End of T-Codes
  */

  /**
  *---------------------------------------------------------------------------------
  *# D codes
  */
  else if (code_seen('D')) // D codes (debug)
  {
    switch((int)code_value())
    {

    /*!
    ### D-1 - Endless Loop <a href="https://reprap.org/wiki/G-code#D-1:_Endless_Loop">D-1: Endless Loop</a>
    */
	case -1:
		dcode__1(); break;
#ifdef DEBUG_DCODES

    /*!
    ### D0 - Reset <a href="https://reprap.org/wiki/G-code#D0:_Reset">D0: Reset</a>
    #### Usage
    
        D0 [ B ]
    
    #### Parameters
    - `B` - Bootloader
    */
	case 0:
		dcode_0(); break;

    /*!
    *
    ### D1 - Clear EEPROM and RESET <a href="https://reprap.org/wiki/G-code#D1:_Clear_EEPROM_and_RESET">D1: Clear EEPROM and RESET</a>
      
          D1
      
    *
    */
	case 1:
		dcode_1(); break;

    /*!
    ### D2 - Read/Write RAM <a href="https://reprap.org/wiki/G-code#D2:_Read.2FWrite_RAM">D3: Read/Write RAM</a>
    This command can be used without any additional parameters. It will read the entire RAM.
    #### Usage
    
        D3 [ A | C | X ]
    
    #### Parameters
    - `A` - Address (0x0000-0x1fff)
    - `C` - Count (0x0001-0x2000)
    - `X` - Data
    */
	case 2:
		dcode_2(); break;
#endif //DEBUG_DCODES
#ifdef DEBUG_DCODE3

    /*!
    ### D3 - Read/Write EEPROM <a href="https://reprap.org/wiki/G-code#D3:_Read.2FWrite_EEPROM">D3: Read/Write EEPROM</a>
    This command can be used without any additional parameters. It will read the entire eeprom.
    #### Usage
    
        D3 [ A | C | X ]
    
    #### Parameters
    - `A` - Address (0x0000-0x0fff)
    - `C` - Count (0x0001-0x1000)
    - `X` - Data
    */
	case 3:
		dcode_3(); break;
#endif //DEBUG_DCODE3
#ifdef DEBUG_DCODES

    /*!
    
    ### D4 - Read/Write PIN <a href="https://reprap.org/wiki/G-code#D4:_Read.2FWrite_PIN">D4: Read/Write PIN</a>
    To read the digital value of a pin you need only to define the pin number.
    #### Usage
    
        D4 [ P | F | V ]
    
    #### Parameters
    - `P` - Pin (0-255)
    - `F` - Function in/out (0/1)
    - `V` - Value (0/1)
    */
	case 4:
		dcode_4(); break;
#endif //DEBUG_DCODES
#ifdef DEBUG_DCODE5

    /*!
    ### D5 - Read/Write FLASH <a href="https://reprap.org/wiki/G-code#D5:_Read.2FWrite_FLASH">D5: Read/Write Flash</a>
    This command can be used without any additional parameters. It will read the 1kb FLASH.
    #### Usage
    
        D3 [ A | C | X | E ]
    
    #### Parameters
    - `A` - Address (0x00000-0x3ffff)
    - `C` - Count (0x0001-0x2000)
    - `X` - Data
    - `E` - Erase
    */
	case 5:
		dcode_5(); break;
		break;
#endif //DEBUG_DCODE5
#ifdef DEBUG_DCODES

    /*!
    ### D6 - Read/Write external FLASH <a href="https://reprap.org/wiki/G-code#D6:_Read.2FWrite_external_FLASH">D6: Read/Write external Flash</a>
    Reserved
    */
	case 6:
		dcode_6(); break;

    /*!
    ### D7 - Read/Write Bootloader <a href="https://reprap.org/wiki/G-code#D7:_Read.2FWrite_Bootloader">D7: Read/Write Bootloader</a>
    Reserved
    */
	case 7:
		dcode_7(); break;

    /*!
    ### D8 - Read/Write PINDA <a href="https://reprap.org/wiki/G-code#D8:_Read.2FWrite_PINDA">D8: Read/Write PINDA</a>
    #### Usage
    
        D8 [ ? | ! | P | Z ]
    
    #### Parameters
    - `?` - Read PINDA temperature shift values
    - `!` - Reset PINDA temperature shift values to default
    - `P` - Pinda temperature [C]
    - `Z` - Z Offset [mm]
    */
	case 8:
		dcode_8(); break;

    /*!
    ### D9 - Read ADC <a href="https://reprap.org/wiki/G-code#D9:_Read.2FWrite_ADC">D9: Read ADC</a>
    #### Usage
    
        D9 [ I | V ]
    
    #### Parameters
    - `I` - ADC channel index 
        - `0` - Heater 0 temperature
        - `1` - Heater 1 temperature
        - `2` - Bed temperature
        - `3` - PINDA temperature
        - `4` - PWR voltage
        - `5` - Ambient temperature
        - `6` - BED voltage
    - `V` Value to be written as simulated
    */
	case 9:
		dcode_9(); break;

    /*!
    ### D10 - Set XYZ calibration = OK <a href="https://reprap.org/wiki/G-code#D10:_Set_XYZ_calibration_.3D_OK">D10: Set XYZ calibration = OK</a>
    */
	case 10:
		dcode_10(); break;

    /*!
    ### D12 - Time <a href="https://reprap.org/wiki/G-code#D12:_Time">D12: Time</a>
    Writes the actual time in the log file.
    */

#endif //DEBUG_DCODES
#ifdef HEATBED_ANALYSIS

    /*!
    ### D80 - Bed check <a href="https://reprap.org/wiki/G-code#D80:_Bed_check">D80: Bed check</a>
    This command will log data to SD card file "mesh.txt".
    #### Usage
    
        D80 [ E | F | G | H | I | J ]
    
    #### Parameters
    - `E` - Dimension X (default 40)
    - `F` - Dimention Y (default 40)
    - `G` - Points X (default 40)
    - `H` - Points Y (default 40)
    - `I` - Offset X (default 74)
    - `J` - Offset Y (default 34)
  */
	case 80:
	{
		float dimension_x = 40;
		float dimension_y = 40;
		int points_x = 40;
		int points_y = 40;
		float offset_x = 74;
		float offset_y = 33;

		if (code_seen('E')) dimension_x = code_value();
		if (code_seen('F')) dimension_y = code_value();
		if (code_seen('G')) {points_x = code_value(); }
		if (code_seen('H')) {points_y = code_value(); }
		if (code_seen('I')) {offset_x = code_value(); }
		if (code_seen('J')) {offset_y = code_value(); }
		printf_P(PSTR("DIM X: %f\n"), dimension_x);
		printf_P(PSTR("DIM Y: %f\n"), dimension_y);
		printf_P(PSTR("POINTS X: %d\n"), points_x);
		printf_P(PSTR("POINTS Y: %d\n"), points_y);
		printf_P(PSTR("OFFSET X: %f\n"), offset_x);
		printf_P(PSTR("OFFSET Y: %f\n"), offset_y);
 		bed_check(dimension_x,dimension_y,points_x,points_y,offset_x,offset_y);
	}break;

    /*!
    ### D81 - Bed analysis <a href="https://reprap.org/wiki/G-code#D81:_Bed_analysis">D80: Bed analysis</a>
    This command will log data to SD card file "wldsd.txt".
    #### Usage
    
        D81 [ E | F | G | H | I | J ]
    
    #### Parameters
    - `E` - Dimension X (default 40)
    - `F` - Dimention Y (default 40)
    - `G` - Points X (default 40)
    - `H` - Points Y (default 40)
    - `I` - Offset X (default 74)
    - `J` - Offset Y (default 34)
  */
	case 81:
	{
		float dimension_x = 40;
		float dimension_y = 40;
		int points_x = 40;
		int points_y = 40;
		float offset_x = 74;
		float offset_y = 33;

		if (code_seen('E')) dimension_x = code_value();
		if (code_seen('F')) dimension_y = code_value();
		if (code_seen("G")) { strchr_pointer+=1; points_x = code_value(); }
		if (code_seen("H")) { strchr_pointer+=1; points_y = code_value(); }
		if (code_seen("I")) { strchr_pointer+=1; offset_x = code_value(); }
		if (code_seen("J")) { strchr_pointer+=1; offset_y = code_value(); }
		
		bed_analysis(dimension_x,dimension_y,points_x,points_y,offset_x,offset_y);
		
	} break;
	
#endif //HEATBED_ANALYSIS
#ifdef DEBUG_DCODES

    /*!
    ### D106 - Print measured fan speed for different pwm values <a href="https://reprap.org/wiki/G-code#D106:_Print_measured_fan_speed_for_different_pwm_values">D106: Print measured fan speed for different pwm values</a>
    */
	case 106:
	{
		for (int i = 255; i > 0; i = i - 5) {
			fanSpeed = i;
			//delay_keep_alive(2000);
			for (int j = 0; j < 100; j++) {
				delay_keep_alive(100);

			}
			printf_P(_N("%d: %d\n"), i, fan_speed[1]);
		}
	}break;

#ifdef TMC2130
    /*!
    ### D2130 - Trinamic stepper controller <a href="https://reprap.org/wiki/G-code#D2130:_Trinamic_stepper_controller">D2130: Trinamic stepper controller</a>
    @todo Please review by owner of the code. RepRap Wiki Gcode needs to be updated after review of owner as well.
    
    #### Usage
    
        D2130 [ Axis | Command | Subcommand | Value ]
    
    #### Parameters
    - Axis
      - `X` - X stepper driver
      - `Y` - Y stepper driver
      - `Z` - Z stepper driver
      - `E` - Extruder stepper driver
    - Commands
      - `0`   - Current off
      - `1`   - Current on
      - `+`   - Single step
      - `-`   - Single step oposite direction
      - `NNN` - Value sereval steps
      - `?`   - Read register
      - Subcommands for read register
        - `mres`     - Micro step resolution. More information in datasheet '5.5.2 CHOPCONF – Chopper Configuration'
        - `step`     - Step
        - `mscnt`    - Microstep counter. More information in datasheet '5.5 Motor Driver Registers'
        - `mscuract` - Actual microstep current for motor. More information in datasheet '5.5 Motor Driver Registers'
        - `wave`     - Microstep linearity compensation curve
      - `!`   - Set register
      - Subcommands for set register
        - `mres`     - Micro step resolution
        - `step`     - Step
        - `wave`     - Microstep linearity compensation curve
        - Values for set register
          - `0, 180 --> 250` - Off
          - `0.9 --> 1.25`   - Valid values (recommended is 1.1)
      - `@`   - Home calibrate axis
    
    Examples:
      
          D2130E?wave
      
      Print extruder microstep linearity compensation curve
      
          D2130E!wave0
      
      Disable extruder linearity compensation curve, (sine curve is used)
      
          D2130E!wave220
      
      (sin(x))^1.1 extruder microstep compensation curve used
    
    Notes:
      For more information see https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC2130_datasheet.pdf
    *
	*/

	case 2130:
		dcode_2130(); break;
#endif //TMC2130

#if (defined (FILAMENT_SENSOR) && defined(PAT9125))

    /*!
    ### D9125 - PAT9125 filament sensor <a href="https://reprap.org/wiki/G-code#D9:_Read.2FWrite_ADC">D9125: PAT9125 filament sensor</a>
    #### Usage
    
        D9125 [ ? | ! | R | X | Y | L ]
    
    #### Parameters
    - `?` - Print values
    - `!` - Print values
    - `R` - Resolution. Not active in code
    - `X` - X values
    - `Y` - Y values
    - `L` - Activate filament sensor log
    */
	case 9125:
		dcode_9125(); break;
#endif //FILAMENT_SENSOR

#endif //DEBUG_DCODES
	}
  }

  else
  {
    SERIAL_ECHO_START;
    SERIAL_ECHORPGM(MSG_UNKNOWN_COMMAND);
    SERIAL_ECHO(CMDBUFFER_CURRENT_STRING);
    SERIAL_ECHOLNPGM("\"(2)");
  }
  KEEPALIVE_STATE(NOT_BUSY);
  ClearToSend();
}

/*!
#### End of D-Codes
*/

  /** @defgroup GCodes G-Code List 
  */

// ---------------------------------------------------

void FlushSerialRequestResend()
{
  //char cmdbuffer[bufindr][100]="Resend:";
  MYSERIAL.flush();
  printf_P(_N("%S: %ld\n%S\n"), _n("Resend"), gcode_LastN + 1, MSG_OK);
}

// Confirm the execution of a command, if sent from a serial line.
// Execution of a command from a SD card will not be confirmed.
void ClearToSend()
{
    previous_millis_cmd = _millis();
	if ((CMDBUFFER_CURRENT_TYPE == CMDBUFFER_CURRENT_TYPE_USB) || (CMDBUFFER_CURRENT_TYPE == CMDBUFFER_CURRENT_TYPE_USB_WITH_LINENR)) 
		SERIAL_PROTOCOLLNRPGM(MSG_OK);
}

#if MOTHERBOARD == BOARD_RAMBO_MINI_1_0 || MOTHERBOARD == BOARD_RAMBO_MINI_1_3
void update_currents() {
	float current_high[3] = DEFAULT_PWM_MOTOR_CURRENT_LOUD;
	float current_low[3] = DEFAULT_PWM_MOTOR_CURRENT;
	float tmp_motor[3];
	
	//SERIAL_ECHOLNPGM("Currents updated: ");

	if (destination[Z_AXIS] < Z_SILENT) {
		//SERIAL_ECHOLNPGM("LOW");
		for (uint8_t i = 0; i < 3; i++) {
			st_current_set(i, current_low[i]);		
			/*MYSERIAL.print(int(i));
			SERIAL_ECHOPGM(": ");
			MYSERIAL.println(current_low[i]);*/
		}		
	}
	else if (destination[Z_AXIS] > Z_HIGH_POWER) {
		//SERIAL_ECHOLNPGM("HIGH");
		for (uint8_t i = 0; i < 3; i++) {
			st_current_set(i, current_high[i]);
			/*MYSERIAL.print(int(i));
			SERIAL_ECHOPGM(": ");
			MYSERIAL.println(current_high[i]);*/
		}		
	}
	else {
		for (uint8_t i = 0; i < 3; i++) {
			float q = current_low[i] - Z_SILENT*((current_high[i] - current_low[i]) / (Z_HIGH_POWER - Z_SILENT));
			tmp_motor[i] = ((current_high[i] - current_low[i]) / (Z_HIGH_POWER - Z_SILENT))*destination[Z_AXIS] + q;
			st_current_set(i, tmp_motor[i]);			
			/*MYSERIAL.print(int(i));
			SERIAL_ECHOPGM(": ");
			MYSERIAL.println(tmp_motor[i]);*/
		}
	}
}
#endif //MOTHERBOARD == BOARD_RAMBO_MINI_1_0 || MOTHERBOARD == BOARD_RAMBO_MINI_1_3

void get_coordinates()
{
  bool seen[4]={false,false,false,false};
  for(int8_t i=0; i < NUM_AXIS; i++) {
    if(code_seen(axis_codes[i]))
    {
      bool relative = axis_relative_modes[i];
      destination[i] = (float)code_value();
      if (i == E_AXIS) {
        float emult = extruder_multiplier[active_extruder];
        if (emult != 1.) {
          if (! relative) {
            destination[i] -= current_position[i];
            relative = true;
          }
          destination[i] *= emult;
        }
      }
      if (relative)
        destination[i] += current_position[i];
      seen[i]=true;
#if MOTHERBOARD == BOARD_RAMBO_MINI_1_0 || MOTHERBOARD == BOARD_RAMBO_MINI_1_3
	  if (i == Z_AXIS && SilentModeMenu == SILENT_MODE_AUTO) update_currents();
#endif //MOTHERBOARD == BOARD_RAMBO_MINI_1_0 || MOTHERBOARD == BOARD_RAMBO_MINI_1_3
    }
    else destination[i] = current_position[i]; //Are these else lines really needed?
  }
  if(code_seen('F')) {
    next_feedrate = code_value();
#ifdef MAX_SILENT_FEEDRATE
	if (tmc2130_mode == TMC2130_MODE_SILENT)
		if (next_feedrate > MAX_SILENT_FEEDRATE) next_feedrate = MAX_SILENT_FEEDRATE;
#endif //MAX_SILENT_FEEDRATE
    if(next_feedrate > 0.0) feedrate = next_feedrate;
	if (!seen[0] && !seen[1] && !seen[2] && seen[3])
	{
//		float e_max_speed = 
//		printf_P(PSTR("E MOVE speed %7.3f\n"), feedrate / 60)
	}
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
            if (cs.add_homing[Z_AXIS] < 0) negative_z_offset = negative_z_offset + cs.add_homing[Z_AXIS];
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
        int n_segments = 0;

        if (mbl.active) {
            float len = abs(dx) + abs(dy);
            if (len > 0)
                // Split to 3cm segments or shorter.
                n_segments = int(ceil(len / 30.f));
        }

        if (n_segments > 1) {
            // In a multi-segment move explicitly set the final target in the plan
            // as the move will be recalculated in it's entirety
            float gcode_target[NUM_AXIS];
            gcode_target[X_AXIS] = x;
            gcode_target[Y_AXIS] = y;
            gcode_target[Z_AXIS] = z;
            gcode_target[E_AXIS] = e;

            float dz = z - current_position[Z_AXIS];
            float de = e - current_position[E_AXIS];

            for (int i = 1; i < n_segments; ++ i) {
                float t = float(i) / float(n_segments);
                plan_buffer_line(current_position[X_AXIS] + t * dx,
                                 current_position[Y_AXIS] + t * dy,
                                 current_position[Z_AXIS] + t * dz,
                                 current_position[E_AXIS] + t * de,
                                 feed_rate, extruder, gcode_target);
                if (waiting_inside_plan_buffer_line_print_aborted)
                    return;
            }
        }
        // The rest of the path.
        plan_buffer_line(x, y, z, e, feed_rate, extruder);
    }
#endif  // MESH_BED_LEVELING
    
void prepare_move()
{
  clamp_to_software_endstops(destination);
  previous_millis_cmd = _millis();

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

  set_current_to_destination();
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
  previous_millis_cmd = _millis();
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
  if ((_millis() - lastMotorCheck) >= 2500) //Not a time critical function, so we only check every 2500ms
  {
    lastMotorCheck = _millis();

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
      lastMotor = _millis(); //... set time to NOW so the fan will turn on
    }

    if ((_millis() - lastMotor) >= (CONTROLLERFAN_SECS*1000UL) || lastMotor == 0) //If the last time any driver was enabled, is longer since than CONTROLLERSEC...
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
  if(_millis() > stat_update) {
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

#ifdef SAFETYTIMER
/**
 * @brief Turn off heating after safetytimer_inactive_time milliseconds of inactivity
 *
 * Full screen blocking notification message is shown after heater turning off.
 * Paused print is not considered inactivity, as nozzle is cooled anyway and bed cooling would
 * damage print.
 *
 * If safetytimer_inactive_time is zero, feature is disabled (heating is never turned off because of inactivity)
 */
static void handleSafetyTimer()
{
#if (EXTRUDERS > 1)
#error Implemented only for one extruder.
#endif //(EXTRUDERS > 1)
    if ((PRINTER_ACTIVE) || (!degTargetBed() && !degTargetHotend(0)) || (!safetytimer_inactive_time))
    {
        safetyTimer.stop();
    }
    else if ((degTargetBed() || degTargetHotend(0)) && (!safetyTimer.running()))
    {
        safetyTimer.start();
    }
    else if (safetyTimer.expired(farm_mode?FARM_DEFAULT_SAFETYTIMER_TIME_ms:safetytimer_inactive_time))
    {
        setTargetBed(0);
        setAllTargetHotends(0);
        lcd_show_fullscreen_message_and_wait_P(_i("Heating disabled by safety timer."));////MSG_BED_HEATING_SAFETY_DISABLED
    }
}
#endif //SAFETYTIMER

void manage_inactivity(bool ignore_stepper_queue/*=false*/) //default argument set in Marlin.h
{
bool bInhibitFlag;
#ifdef FILAMENT_SENSOR
	if (mmu_enabled == false)
	{
//-//		if (mcode_in_progress != 600) //M600 not in progress
#ifdef PAT9125
          bInhibitFlag=(menu_menu==lcd_menu_extruder_info); // Support::ExtruderInfo menu active
#endif // PAT9125
#ifdef IR_SENSOR
          bInhibitFlag=(menu_menu==lcd_menu_show_sensors_state); // Support::SensorInfo menu active
#endif // IR_SENSOR
          if ((mcode_in_progress != 600) && (eFilamentAction != FilamentAction::AutoLoad) && (!bInhibitFlag)) //M600 not in progress, preHeat @ autoLoad menu not active, Support::ExtruderInfo/SensorInfo menu not active
		{
			if (!moves_planned() && !IS_SD_PRINTING && !is_usb_printing && (lcd_commands_type != LcdCommands::Layer1Cal) && ! eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE))
			{
				if (fsensor_check_autoload())
				{
#ifdef PAT9125
					fsensor_autoload_check_stop();
#endif //PAT9125
//-//					if (degHotend0() > EXTRUDE_MINTEMP)
if(0)
					{
            Sound_MakeCustom(50,1000,false);
						loading_flag = true;
						enquecommand_front_P((PSTR("M701")));
					}
					else
					{
/*
						lcd_update_enable(false);
						show_preheat_nozzle_warning();
						lcd_update_enable(true);
*/
                              eFilamentAction=FilamentAction::AutoLoad;
                              bFilamentFirstRun=false;
                              if(target_temperature[0]>=EXTRUDE_MINTEMP)
                              {
                                   bFilamentPreheatState=true;
//                                   mFilamentItem(target_temperature[0],target_temperature_bed);
                                   menu_submenu(mFilamentItemForce);
                              }
                              else
                              {
                                   menu_submenu(lcd_generic_preheat_menu);
                                   lcd_timeoutToStatus.start();
                              }
                         }
				}
			}
			else
			{
#ifdef PAT9125
				fsensor_autoload_check_stop();
#endif //PAT9125
                if (fsensor_enabled && !saved_printing)
                    fsensor_update();
			}
		}
	}
#endif //FILAMENT_SENSOR

#ifdef SAFETYTIMER
	handleSafetyTimer();
#endif //SAFETYTIMER

#if defined(KILL_PIN) && KILL_PIN > -1
	static int killCount = 0;   // make the inactivity button a bit less responsive
   const int KILL_DELAY = 10000;
#endif
	
    if(buflen < (BUFSIZE-1)){
        get_command();
    }

  if( (_millis() - previous_millis_cmd) >  max_inactive_time )
    if(max_inactive_time)
      kill(_n("Inactivity Shutdown"), 4);
  if(stepper_inactive_time)  {
    if( (_millis() - previous_millis_cmd) >  stepper_inactive_time )
    {
      if(blocks_queued() == false && ignore_stepper_queue == false) {
        disable_x();
        disable_y();
        disable_z();
        disable_e0();
        disable_e1();
        disable_e2();
      }
    }
  }
  
  #ifdef CHDK //Check if pin should be set to LOW after M240 set it to HIGH
    if (chdkActive && (_millis() - chdkHigh > CHDK_DELAY))
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
       kill(NULL, 5);
    }
  #endif
    
  #if defined(CONTROLLERFAN_PIN) && CONTROLLERFAN_PIN > -1
    controllerFan(); //Check if fan should be turned on to cool stepper drivers down
  #endif
  #ifdef EXTRUDER_RUNOUT_PREVENT
    if( (_millis() - previous_millis_cmd) >  EXTRUDER_RUNOUT_SECONDS*1000 )
    if(degHotend(active_extruder)>EXTRUDER_RUNOUT_MINTEMP)
    {
     bool oldstatus=READ(E0_ENABLE_PIN);
     enable_e0();
     float oldepos=current_position[E_AXIS];
     float oldedes=destination[E_AXIS];
     plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS],
                      destination[E_AXIS]+EXTRUDER_RUNOUT_EXTRUDE*EXTRUDER_RUNOUT_ESTEPS/cs.axis_steps_per_unit[E_AXIS],
                      EXTRUDER_RUNOUT_SPEED/60.*EXTRUDER_RUNOUT_ESTEPS/cs.axis_steps_per_unit[E_AXIS], active_extruder);
     current_position[E_AXIS]=oldepos;
     destination[E_AXIS]=oldedes;
     plan_set_e_position(oldepos);
     previous_millis_cmd=_millis();
     st_synchronize();
     WRITE(E0_ENABLE_PIN,oldstatus);
    }
  #endif
  #ifdef TEMP_STAT_LEDS
      handle_status_leds();
  #endif
  check_axes_activity();
  mmu_loop();
}

void kill(const char *full_screen_message, unsigned char id)
{
	printf_P(_N("KILL: %d\n"), id);
	//return;
  cli(); // Stop interrupts
  disable_heater();

  disable_x();
//  SERIAL_ECHOLNPGM("kill - disable Y");
  disable_y();
  poweroff_z();
  disable_e0();
  disable_e1();
  disable_e2();

#if defined(PS_ON_PIN) && PS_ON_PIN > -1
  pinMode(PS_ON_PIN,INPUT);
#endif
  SERIAL_ERROR_START;
  SERIAL_ERRORLNRPGM(_n("Printer halted. kill() called!"));////MSG_ERR_KILLED
  if (full_screen_message != NULL) {
      SERIAL_ERRORLNRPGM(full_screen_message);
      lcd_display_message_fullscreen_P(full_screen_message);
  } else {
      LCD_ALERTMESSAGERPGM(_n("KILLED. "));////MSG_KILLED
  }

  // FMC small patch to update the LCD before ending
  sei();   // enable interrupts
  for ( int i=5; i--; lcd_update(0))
  {
     _delay(200);	
  }
  cli();   // disable interrupts
  suicide();
  while(1)
  {
#ifdef WATCHDOG
    wdt_reset();
#endif //WATCHDOG
	  /* Intentionally left empty */
	
  } // Wait for reset
}

// Stop: Emergency stop used by overtemp functions which allows recovery
//
//   In addition to stopping the print, this prevents subsequent G[0-3] commands to be
//   processed via USB (using "Stopped") until the print is resumed via M999 or
//   manually started from scratch with the LCD.
//
//   Note that the current instruction is completely discarded, so resuming from Stop()
//   will introduce either over/under extrusion on the current segment, and will not
//   survive a power panic. Switching Stop() to use the pause machinery instead (with
//   the addition of disabling the headers) could allow true recovery in the future.
void Stop()
{
  disable_heater();
  if(Stopped == false) {
    Stopped = true;
    lcd_print_stop();
    Stopped_gcode_LastN = gcode_LastN; // Save last g_code for restart
    SERIAL_ERROR_START;
    SERIAL_ERRORLNRPGM(MSG_ERR_STOPPED);
    LCD_MESSAGERPGM(_T(MSG_STOPPED));
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

//! @brief Get and validate extruder number
//!
//! If it is not specified, active_extruder is returned in parameter extruder.
//! @param [in] code M code number
//! @param [out] extruder
//! @return error
//! @retval true Invalid extruder specified in T code
//! @retval false Valid extruder specified in T code, or not specifiead

bool setTargetedHotend(int code, uint8_t &extruder)
{
  extruder = active_extruder;
  if(code_seen('T')) {
      extruder = code_value();
    if(extruder >= EXTRUDERS) {
      SERIAL_ECHO_START;
      switch(code){
        case 104:
          SERIAL_ECHORPGM(_n("M104 Invalid extruder "));////MSG_M104_INVALID_EXTRUDER
          break;
        case 105:
          SERIAL_ECHO(_n("M105 Invalid extruder "));////MSG_M105_INVALID_EXTRUDER
          break;
        case 109:
          SERIAL_ECHO(_n("M109 Invalid extruder "));////MSG_M109_INVALID_EXTRUDER
          break;
        case 218:
          SERIAL_ECHO(_n("M218 Invalid extruder "));////MSG_M218_INVALID_EXTRUDER
          break;
        case 221:
          SERIAL_ECHO(_n("M221 Invalid extruder "));////MSG_M221_INVALID_EXTRUDER
          break;
      }
      SERIAL_PROTOCOLLN((int)extruder);
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

float calculate_extruder_multiplier(float diameter) {
  float out = 1.f;
  if (cs.volumetric_enabled && diameter > 0.f) {
    float area = M_PI * diameter * diameter * 0.25;
    out = 1.f / area;
  }
  if (extrudemultiply != 100)
    out *= float(extrudemultiply) * 0.01f;
  return out;
}

void calculate_extruder_multipliers() {
	extruder_multiplier[0] = calculate_extruder_multiplier(cs.filament_size[0]);
#if EXTRUDERS > 1
	extruder_multiplier[1] = calculate_extruder_multiplier(cs.filament_size[1]);
#if EXTRUDERS > 2
	extruder_multiplier[2] = calculate_extruder_multiplier(cs.filament_size[2]);
#endif
#endif
}

void delay_keep_alive(unsigned int ms)
{
    for (;;) {
        manage_heater();
        // Manage inactivity, but don't disable steppers on timeout.
        manage_inactivity(true);
        lcd_update(0);
        if (ms == 0)
            break;
        else if (ms >= 50) {
            _delay(50);
            ms -= 50;
        } else {
			_delay(ms);
            ms = 0;
        }
    }
}

static void wait_for_heater(long codenum, uint8_t extruder) {
    if (!degTargetHotend(extruder))
        return;

#ifdef TEMP_RESIDENCY_TIME
	long residencyStart;
	residencyStart = -1;
	/* continue to loop until we have reached the target temp
	_and_ until TEMP_RESIDENCY_TIME hasn't passed since we reached it */
    cancel_heatup = false;
	while ((!cancel_heatup) && ((residencyStart == -1) ||
		(residencyStart >= 0 && (((unsigned int)(_millis() - residencyStart)) < (TEMP_RESIDENCY_TIME * 1000UL))))) {
#else
	while (target_direction ? (isHeatingHotend(tmp_extruder)) : (isCoolingHotend(tmp_extruder) && (CooldownNoWait == false))) {
#endif //TEMP_RESIDENCY_TIME
		if ((_millis() - codenum) > 1000UL)
		{ //Print Temp Reading and remaining time every 1 second while heating up/cooling down
			if (!farm_mode) {
				SERIAL_PROTOCOLPGM("T:");
				SERIAL_PROTOCOL_F(degHotend(extruder), 1);
				SERIAL_PROTOCOLPGM(" E:");
				SERIAL_PROTOCOL((int)extruder);

#ifdef TEMP_RESIDENCY_TIME
				SERIAL_PROTOCOLPGM(" W:");
				if (residencyStart > -1)
				{
					codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (_millis() - residencyStart)) / 1000UL;
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
				codenum = _millis();
		}
			manage_heater();
			manage_inactivity(true); //do not disable steppers
			lcd_update(0);
#ifdef TEMP_RESIDENCY_TIME
			/* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
			or when current temp falls outside the hysteresis after target temp was reached */
			if ((residencyStart == -1 && target_direction && (degHotend(extruder) >= (degTargetHotend(extruder) - TEMP_WINDOW))) ||
				(residencyStart == -1 && !target_direction && (degHotend(extruder) <= (degTargetHotend(extruder) + TEMP_WINDOW))) ||
				(residencyStart > -1 && labs(degHotend(extruder) - degTargetHotend(extruder)) > TEMP_HYSTERESIS))
			{
				residencyStart = _millis();
			}
#endif //TEMP_RESIDENCY_TIME
	}
}

void check_babystep()
{
	int babystep_z = eeprom_read_word(reinterpret_cast<uint16_t *>(&(EEPROM_Sheets_base->
            s[(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)))].z_offset)));

	if ((babystep_z < Z_BABYSTEP_MIN) || (babystep_z > Z_BABYSTEP_MAX)) {
		babystep_z = 0; //if babystep value is out of min max range, set it to 0
		SERIAL_ECHOLNPGM("Z live adjust out of range. Setting to 0");
		eeprom_write_word(reinterpret_cast<uint16_t *>(&(EEPROM_Sheets_base->
            s[(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)))].z_offset)),
                    babystep_z);
		lcd_show_fullscreen_message_and_wait_P(PSTR("Z live adjust out of range. Setting to 0. Click to continue."));
		lcd_update_enable(true);		
	}	
}
#ifdef HEATBED_ANALYSIS
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

void bed_check(float x_dimension, float y_dimension, int x_points_num, int y_points_num, float shift_x, float shift_y) {
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
	float measure_z_height = 0.2f;
	float row[x_points_num];
	int ix = 0;
	int iy = 0;

	const char* filename_wldsd = "mesh.txt";
	char data_wldsd[x_points_num * 7 + 1]; //6 chars(" -A.BCD")for each measurement + null 
	char numb_wldsd[8]; // (" -A.BCD" + null)
#ifdef MICROMETER_LOGGING
	d_setup();
#endif //MICROMETER_LOGGING

	int XY_AXIS_FEEDRATE = homing_feedrate[X_AXIS] / 20;
	int Z_LIFT_FEEDRATE = homing_feedrate[Z_AXIS] / 40;

	unsigned int custom_message_type_old = custom_message_type;
	unsigned int custom_message_state_old = custom_message_state;
	custom_message_type = CustomMsg::MeshBedLeveling;
	custom_message_state = (x_points_num * y_points_num) + 10;
	lcd_update(1);

	//mbl.reset();
	babystep_undo();

	card.openFile(filename_wldsd, false);

	/*destination[Z_AXIS] = mesh_home_z_search;
	//plan_buffer_line_curposXYZE(Z_LIFT_FEEDRATE, active_extruder);

	plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], Z_LIFT_FEEDRATE, active_extruder);
	for(int8_t i=0; i < NUM_AXIS; i++) {
		current_position[i] = destination[i];
	}
	st_synchronize();
	*/
		destination[Z_AXIS] = measure_z_height;
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], Z_LIFT_FEEDRATE, active_extruder);
		for(int8_t i=0; i < NUM_AXIS; i++) {
			current_position[i] = destination[i];
		}
		st_synchronize();
	/*int l_feedmultiply = */setup_for_endstop_move(false);

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
		/*destination[Z_AXIS] = mesh_home_z_search;
		//plan_buffer_line_curposXYZE(Z_LIFT_FEEDRATE, active_extruder);

		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], Z_LIFT_FEEDRATE, active_extruder);
		for(int8_t i=0; i < NUM_AXIS; i++) {
			current_position[i] = destination[i];
		}
		st_synchronize();*/


		//current_position[X_AXIS] = 13.f + ix * (x_dimension / (x_points_num - 1)) - bed_zero_ref_x + shift_x;
		//current_position[Y_AXIS] = 6.4f + iy * (y_dimension / (y_points_num - 1)) - bed_zero_ref_y + shift_y;

		destination[X_AXIS] = ix * (x_dimension / (x_points_num - 1)) + shift_x;
		destination[Y_AXIS] = iy * (y_dimension / (y_points_num - 1)) + shift_y;

        mesh_plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], XY_AXIS_FEEDRATE/6, active_extruder);
        set_current_to_destination();
		st_synchronize();

	//	printf_P(PSTR("X = %f; Y= %f \n"), current_position[X_AXIS], current_position[Y_AXIS]);

		delay_keep_alive(1000);
#ifdef MICROMETER_LOGGING

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
				//printf_P(PSTR("Done %d\n"), j);
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


		
		//row[ix] = d_ReadData();
		
		row[ix] = output;

		if (iy % 2 == 1 ? ix == 0 : ix == x_points_num - 1) {
			memset(data_wldsd, 0, sizeof(data_wldsd));
			for (int i = 0; i < x_points_num; i++) {
				SERIAL_PROTOCOLPGM(" ");
				SERIAL_PROTOCOL_F(row[i], 5);
				memset(numb_wldsd, 0, sizeof(numb_wldsd));
				dtostrf(row[i], 7, 3, numb_wldsd);
				strcat(data_wldsd, numb_wldsd);
			}
			card.write_command(data_wldsd);
			SERIAL_PROTOCOLPGM("\n");

		}

		custom_message_state--;
		mesh_point++;
		lcd_update(1);

	}
	#endif //MICROMETER_LOGGING
	card.closefile();
	//clean_up_after_endstop_move(l_feedmultiply);

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

	const char* filename_wldsd = "wldsd.txt";
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
	unsigned int custom_message_type_old = custom_message_type;
	unsigned int custom_message_state_old = custom_message_state;
	custom_message_type = CustomMsg::MeshBedLeveling;
	custom_message_state = (x_points_num * y_points_num) + 10;
	lcd_update(1);

	mbl.reset();
	babystep_undo();

	card.openFile(filename_wldsd, false);

	current_position[Z_AXIS] = mesh_home_z_search;
	plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS] / 60, active_extruder);

	int XY_AXIS_FEEDRATE = homing_feedrate[X_AXIS] / 20;
	int Z_LIFT_FEEDRATE = homing_feedrate[Z_AXIS] / 40;

	int l_feedmultiply = setup_for_endstop_move(false);

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
		plan_buffer_line_curposXYZE(Z_LIFT_FEEDRATE, active_extruder);
		st_synchronize();


		current_position[X_AXIS] = 13.f + ix * (x_dimension / (x_points_num - 1)) - bed_zero_ref_x + shift_x;
		current_position[Y_AXIS] = 6.4f + iy * (y_dimension / (y_points_num - 1)) - bed_zero_ref_y + shift_y;

		plan_buffer_line_curposXYZE(XY_AXIS_FEEDRATE, active_extruder);
		st_synchronize();

		if (!find_bed_induction_sensor_point_z(-10.f)) { //if we have data from z calibration max allowed difference is 1mm for each point, if we dont have data max difference is 10mm from initial point  
			break;
			card.closefile();
		}


		//memset(numb_wldsd, 0, sizeof(numb_wldsd));
		//dtostrf(d_ReadData(), 8, 5, numb_wldsd);
		//strcat(data_wldsd, numb_wldsd);


		
		//MYSERIAL.println(data_wldsd);
		//_delay(1000);
		//_delay(3000);
		//t1 = _millis();
		
		//while (digitalRead(D_DATACLOCK) == LOW) {}
		//while (digitalRead(D_DATACLOCK) == HIGH) {}
		memset(digit, 0, sizeof(digit));
		//cli();
		digitalWrite(D_REQUIRE, LOW);	
		
		for (int i = 0; i<13; i++)
		{
			//t1 = _millis();
			for (int j = 0; j < 4; j++)
			{
				while (digitalRead(D_DATACLOCK) == LOW) {}				
				while (digitalRead(D_DATACLOCK) == HIGH) {}
				bitWrite(digit[i], j, digitalRead(D_DATA));
			}
			//t_delay = (_millis() - t1);
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
	clean_up_after_endstop_move(l_feedmultiply);
}
#endif //HEATBED_ANALYSIS

#ifndef PINDA_THERMISTOR
static void temp_compensation_start() {
	
	custom_message_type = CustomMsg::TempCompPreheat;
	custom_message_state = PINDA_HEAT_T + 1;
	lcd_update(2);
	if (degHotend(active_extruder) > EXTRUDE_MINTEMP) {
		current_position[E_AXIS] -= default_retraction;
	}
	plan_buffer_line_curposXYZE(400, active_extruder);
	
	current_position[X_AXIS] = PINDA_PREHEAT_X;
	current_position[Y_AXIS] = PINDA_PREHEAT_Y;
	current_position[Z_AXIS] = PINDA_PREHEAT_Z;
	plan_buffer_line_curposXYZE(3000 / 60, active_extruder);
	st_synchronize();
	while (fabs(degBed() - target_temperature_bed) > 1) delay_keep_alive(1000);

	for (int i = 0; i < PINDA_HEAT_T; i++) {
		delay_keep_alive(1000);
		custom_message_state = PINDA_HEAT_T - i;
		if (custom_message_state == 99 || custom_message_state == 9) lcd_update(2); //force whole display redraw if number of digits changed
		else lcd_update(1);
	}	
	custom_message_type = CustomMsg::Status;
	custom_message_state = 0;
}

static void temp_compensation_apply() {
	int i_add;
	int z_shift = 0;
	float z_shift_mm;

	if (calibration_status() == CALIBRATION_STATUS_CALIBRATED) {
		if (target_temperature_bed % 10 == 0 && target_temperature_bed >= 60 && target_temperature_bed <= 100) {
			i_add = (target_temperature_bed - 60) / 10;
			EEPROM_read_B(EEPROM_PROBE_TEMP_SHIFT + i_add * 2, &z_shift);
			z_shift_mm = z_shift / cs.axis_steps_per_unit[Z_AXIS];
		}else {
			//interpolation
			z_shift_mm = temp_comp_interpolation(target_temperature_bed) / cs.axis_steps_per_unit[Z_AXIS];
		}
		printf_P(_N("\nZ shift applied:%.3f\n"), z_shift_mm);
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS] - z_shift_mm, current_position[E_AXIS], homing_feedrate[Z_AXIS] / 40, active_extruder);
		st_synchronize();
		plan_set_z_position(current_position[Z_AXIS]);
	}
	else {		
		//we have no temp compensation data
	}
}
#endif //ndef PINDA_THERMISTOR

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
#ifdef PINDA_THERMISTOR
		temp_C[i] = 35 + i * 5; //temperature in C
#else
		temp_C[i] = 50 + i * 10; //temperature in C
#endif
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

#ifdef PINDA_THERMISTOR
float temp_compensation_pinda_thermistor_offset(float temperature_pinda)
{
	if (!temp_cal_active) return 0;
	if (!calibration_status_pinda()) return 0;
	return temp_comp_interpolation(temperature_pinda) / cs.axis_steps_per_unit[Z_AXIS];
}
#endif //PINDA_THERMISTOR

void long_pause() //long pause print
{
	st_synchronize();
	start_pause_print = _millis();

    // Stop heaters
    setAllTargetHotends(0);

	//lift z
	current_position[Z_AXIS] += Z_PAUSE_LIFT;
	if (current_position[Z_AXIS] > Z_MAX_POS) current_position[Z_AXIS] = Z_MAX_POS;
	plan_buffer_line_curposXYZE(15, active_extruder);

	//Move XY to side
	current_position[X_AXIS] = X_PAUSE_POS;
	current_position[Y_AXIS] = Y_PAUSE_POS;
	plan_buffer_line_curposXYZE(50, active_extruder);

	// Turn off the print fan
	fanSpeed = 0;
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

#ifdef UVLO_SUPPORT
void uvlo_drain_reset()
{
    // burn all that residual power
    wdt_enable(WDTO_1S);
    WRITE(BEEPER,HIGH);
    lcd_clear();
    lcd_puts_at_P(0, 1, MSG_POWERPANIC_DETECTED);
    while(1);
}


void uvlo_()
{
	unsigned long time_start = _millis();
	bool sd_print = card.sdprinting;
    // Conserve power as soon as possible.
#ifdef LCD_BL_PIN
    backlightMode = BACKLIGHT_MODE_DIM;
    backlightLevel_LOW = 0;
    backlight_update();
#endif //LCD_BL_PIN
    disable_x();
    disable_y();
    
#ifdef TMC2130
	tmc2130_set_current_h(Z_AXIS, 20);
	tmc2130_set_current_r(Z_AXIS, 20);
	tmc2130_set_current_h(E_AXIS, 20);
	tmc2130_set_current_r(E_AXIS, 20);
#endif //TMC2130

    // Stop all heaters
    uint8_t saved_target_temperature_bed = target_temperature_bed;
    uint16_t saved_target_temperature_ext = target_temperature[active_extruder];
    setAllTargetHotends(0);
    setTargetBed(0);

    // Calculate the file position, from which to resume this print.
    long sd_position = sdpos_atomic; //atomic sd position of last command added in queue
    {
      uint16_t sdlen_planner = planner_calc_sd_length(); //length of sd commands in planner
      sd_position -= sdlen_planner;
      uint16_t sdlen_cmdqueue = cmdqueue_calc_sd_length(); //length of sd commands in cmdqueue
      sd_position -= sdlen_cmdqueue;
      if (sd_position < 0) sd_position = 0;
    }

    // save the global state at planning time
    uint16_t feedrate_bckp;
    if (current_block)
    {
        memcpy(saved_target, current_block->gcode_target, sizeof(saved_target));
        feedrate_bckp = current_block->gcode_feedrate;
    }
    else
    {
        saved_target[0] = SAVED_TARGET_UNSET;
        feedrate_bckp = feedrate;
    }

    // From this point on and up to the print recovery, Z should not move during X/Y travels and
    // should be controlled precisely. Reset the MBL status before planner_abort_hard in order to
    // get the physical Z for further manipulation.
    bool mbl_was_active = mbl.active;
    mbl.active = false;

    // After this call, the planner queue is emptied and the current_position is set to a current logical coordinate.
    // The logical coordinate will likely differ from the machine coordinate if the skew calibration and mesh bed leveling
    // are in action.
    planner_abort_hard();

    // Store the print logical Z position, which we need to recover (a slight error here would be
    // recovered on the next Gcode instruction, while a physical location error would not)
    float logical_z = current_position[Z_AXIS];
    if(mbl_was_active) logical_z -= mbl.get_z(st_get_position_mm(X_AXIS), st_get_position_mm(Y_AXIS));
    eeprom_update_float((float*)EEPROM_UVLO_CURRENT_POSITION_Z, logical_z);

    // Store the print E position before we lose track
	eeprom_update_float((float*)(EEPROM_UVLO_CURRENT_POSITION_E), current_position[E_AXIS]);
	eeprom_update_byte((uint8_t*)EEPROM_UVLO_E_ABS, axis_relative_modes[3]?0:1);

    // Clean the input command queue, inhibit serial processing using saved_printing
    cmdqueue_reset();
    card.sdprinting = false;
    saved_printing = true;

    // Enable stepper driver interrupt to move Z axis. This should be fine as the planner and
    // command queues are empty, SD card printing is disabled, usb is inhibited.
    sei();

    // Retract
    current_position[E_AXIS] -= default_retraction;
    plan_buffer_line_curposXYZE(95, active_extruder);
    st_synchronize();
    disable_e0();

    // Read out the current Z motor microstep counter to move the axis up towards
    // a full step before powering off. NOTE: we need to ensure to schedule more
    // than "dropsegments" steps in order to move (this is always the case here
    // due to UVLO_Z_AXIS_SHIFT being used)
    uint16_t z_res = tmc2130_get_res(Z_AXIS);
    uint16_t z_microsteps = tmc2130_rd_MSCNT(Z_AXIS);
    current_position[Z_AXIS] += float(1024 - z_microsteps)
                                / (z_res * cs.axis_steps_per_unit[Z_AXIS])
                                + UVLO_Z_AXIS_SHIFT;
    plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS]/60, active_extruder);
    st_synchronize();
    poweroff_z();

    // Write the file position.
    eeprom_update_dword((uint32_t*)(EEPROM_FILE_POSITION), sd_position);

    // Store the mesh bed leveling offsets. This is 2*7*7=98 bytes, which takes 98*3.4us=333us in worst case.
    for (int8_t mesh_point = 0; mesh_point < MESH_NUM_X_POINTS * MESH_NUM_Y_POINTS; ++ mesh_point) {
      uint8_t ix = mesh_point % MESH_NUM_X_POINTS; // from 0 to MESH_NUM_X_POINTS - 1
      uint8_t iy = mesh_point / MESH_NUM_X_POINTS;
      // Scale the z value to 1u resolution.
      int16_t v = mbl_was_active ? int16_t(floor(mbl.z_values[iy][ix] * 1000.f + 0.5f)) : 0;
      eeprom_update_word((uint16_t*)(EEPROM_UVLO_MESH_BED_LEVELING_FULL +2*mesh_point), *reinterpret_cast<uint16_t*>(&v));
    }

    // Write the _final_ Z position and motor microstep counter (unused).
    eeprom_update_float((float*)EEPROM_UVLO_TINY_CURRENT_POSITION_Z, current_position[Z_AXIS]);
    z_microsteps = tmc2130_rd_MSCNT(Z_AXIS);
    eeprom_update_word((uint16_t*)(EEPROM_UVLO_Z_MICROSTEPS), z_microsteps);

    // Store the current position.
    eeprom_update_float((float*)(EEPROM_UVLO_CURRENT_POSITION + 0), current_position[X_AXIS]);
    eeprom_update_float((float*)(EEPROM_UVLO_CURRENT_POSITION + 4), current_position[Y_AXIS]);

    // Store the current feed rate, temperatures, fan speed and extruder multipliers (flow rates)
	eeprom_update_word((uint16_t*)EEPROM_UVLO_FEEDRATE, feedrate_bckp);
    eeprom_update_word((uint16_t*)EEPROM_UVLO_FEEDMULTIPLY, feedmultiply);
    eeprom_update_word((uint16_t*)EEPROM_UVLO_TARGET_HOTEND, saved_target_temperature_ext);
    eeprom_update_byte((uint8_t*)EEPROM_UVLO_TARGET_BED, saved_target_temperature_bed);
    eeprom_update_byte((uint8_t*)EEPROM_UVLO_FAN_SPEED, fanSpeed);
	eeprom_update_float((float*)(EEPROM_EXTRUDER_MULTIPLIER_0), extruder_multiplier[0]);
#if EXTRUDERS > 1
	eeprom_update_float((float*)(EEPROM_EXTRUDER_MULTIPLIER_1), extruder_multiplier[1]);
#if EXTRUDERS > 2
	eeprom_update_float((float*)(EEPROM_EXTRUDER_MULTIPLIER_2), extruder_multiplier[2]);
#endif
#endif
	eeprom_update_word((uint16_t*)(EEPROM_EXTRUDEMULTIPLY), (uint16_t)extrudemultiply);

    // Store the saved target
    eeprom_update_float((float*)(EEPROM_UVLO_SAVED_TARGET+0*4), saved_target[X_AXIS]);
    eeprom_update_float((float*)(EEPROM_UVLO_SAVED_TARGET+1*4), saved_target[Y_AXIS]);
    eeprom_update_float((float*)(EEPROM_UVLO_SAVED_TARGET+2*4), saved_target[Z_AXIS]);
    eeprom_update_float((float*)(EEPROM_UVLO_SAVED_TARGET+3*4), saved_target[E_AXIS]);

#ifdef LIN_ADVANCE
	eeprom_update_float((float*)(EEPROM_UVLO_LA_K), extruder_advance_K);
#endif

    // Finaly store the "power outage" flag.
	if(sd_print) eeprom_update_byte((uint8_t*)EEPROM_UVLO, 1);

    // Increment power failure counter
	eeprom_update_byte((uint8_t*)EEPROM_POWER_COUNT, eeprom_read_byte((uint8_t*)EEPROM_POWER_COUNT) + 1);
	eeprom_update_word((uint16_t*)EEPROM_POWER_COUNT_TOT, eeprom_read_word((uint16_t*)EEPROM_POWER_COUNT_TOT) + 1);

    printf_P(_N("UVLO - end %d\n"), _millis() - time_start);
    WRITE(BEEPER,HIGH);

    // All is set: with all the juice left, try to move extruder away to detach the nozzle completely from the print
    poweron_z();
    current_position[X_AXIS] = (current_position[X_AXIS] < 0.5f * (X_MIN_POS + X_MAX_POS)) ? X_MIN_POS : X_MAX_POS;
    plan_buffer_line_curposXYZE(500, active_extruder);
    st_synchronize();

    wdt_enable(WDTO_1S);
    while(1);
}


void uvlo_tiny()
{
    unsigned long time_start = _millis();

    // Conserve power as soon as possible.
    disable_x();
    disable_y();
    disable_e0();

#ifdef TMC2130
    tmc2130_set_current_h(Z_AXIS, 20);
    tmc2130_set_current_r(Z_AXIS, 20);
#endif //TMC2130

    // Stop all heaters
    setAllTargetHotends(0);
    setTargetBed(0);

    // When power is interrupted on the _first_ recovery an attempt can be made to raise the
    // extruder, causing the Z position to change. Similarly, when recovering, the Z position is
    // lowered. In such cases we cannot just save Z, we need to re-align the steppers to a fullstep.
    // Disable MBL (if not already) to work with physical coordinates.
    mbl.active = false;
    planner_abort_hard();

    // Allow for small roundoffs to be ignored
    if(abs(current_position[Z_AXIS] - eeprom_read_float((float*)(EEPROM_UVLO_TINY_CURRENT_POSITION_Z))) >= 1.f/cs.axis_steps_per_unit[Z_AXIS])
    {
        // Clean the input command queue, inhibit serial processing using saved_printing
        cmdqueue_reset();
        card.sdprinting = false;
        saved_printing = true;

        // Enable stepper driver interrupt to move Z axis. This should be fine as the planner and
        // command queues are empty, SD card printing is disabled, usb is inhibited.
        sei();

        // The axis was moved: adjust Z as done on a regular UVLO.
        uint16_t z_res = tmc2130_get_res(Z_AXIS);
        uint16_t z_microsteps = tmc2130_rd_MSCNT(Z_AXIS);
        current_position[Z_AXIS] += float(1024 - z_microsteps)
                                    / (z_res * cs.axis_steps_per_unit[Z_AXIS])
                                    + UVLO_TINY_Z_AXIS_SHIFT;
        plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS]/60, active_extruder);
        st_synchronize();
        poweroff_z();

        // Update Z position
        eeprom_update_float((float*)(EEPROM_UVLO_TINY_CURRENT_POSITION_Z), current_position[Z_AXIS]);

        // Update the _final_ Z motor microstep counter (unused).
        z_microsteps = tmc2130_rd_MSCNT(Z_AXIS);
        eeprom_update_word((uint16_t*)(EEPROM_UVLO_Z_MICROSTEPS), z_microsteps);
    }

    // Update the the "power outage" flag.
    eeprom_update_byte((uint8_t*)EEPROM_UVLO,2);

    // Increment power failure counter
    eeprom_update_byte((uint8_t*)EEPROM_POWER_COUNT, eeprom_read_byte((uint8_t*)EEPROM_POWER_COUNT) + 1);
    eeprom_update_word((uint16_t*)EEPROM_POWER_COUNT_TOT, eeprom_read_word((uint16_t*)EEPROM_POWER_COUNT_TOT) + 1);

    printf_P(_N("UVLO_TINY - end %d\n"), _millis() - time_start);
    uvlo_drain_reset();
}
#endif //UVLO_SUPPORT

#if (defined(FANCHECK) && defined(TACH_1) && (TACH_1 >-1))

void setup_fan_interrupt() {
//INT7
	DDRE &= ~(1 << 7); //input pin
	PORTE &= ~(1 << 7); //no internal pull-up

	//start with sensing rising edge
	EICRB &= ~(1 << 6);
	EICRB |= (1 << 7);

	//enable INT7 interrupt
	EIMSK |= (1 << 7);
}

// The fan interrupt is triggered at maximum 325Hz (may be a bit more due to component tollerances),
// and it takes 4.24 us to process (the interrupt invocation overhead not taken into account).
ISR(INT7_vect) {
	//measuring speed now works for fanSpeed > 18 (approximately), which is sufficient because MIN_PRINT_FAN_SPEED is higher
#ifdef FAN_SOFT_PWM
	if (!fan_measuring || (fanSpeedSoftPwm < MIN_PRINT_FAN_SPEED)) return;
#else //FAN_SOFT_PWM
	if (fanSpeed < MIN_PRINT_FAN_SPEED) return;
#endif //FAN_SOFT_PWM

	if ((1 << 6) & EICRB) { //interrupt was triggered by rising edge
		t_fan_rising_edge = millis_nc();
	}
	else { //interrupt was triggered by falling edge
		if ((millis_nc() - t_fan_rising_edge) >= FAN_PULSE_WIDTH_LIMIT) {//this pulse was from sensor and not from pwm
			fan_edge_counter[1] += 2; //we are currently counting all edges so lets count two edges for one pulse
		}
	}	
	EICRB ^= (1 << 6); //change edge
}

#endif

#ifdef UVLO_SUPPORT
void setup_uvlo_interrupt() {
	DDRE &= ~(1 << 4); //input pin
	PORTE &= ~(1 << 4); //no internal pull-up

    // sensing falling edge
	EICRB |= (1 << 0);
	EICRB &= ~(1 << 1);

	// enable INT4 interrupt
	EIMSK |= (1 << 4);

    // check if power was lost before we armed the interrupt
    if(!(PINE & (1 << 4)) && eeprom_read_byte((uint8_t*)EEPROM_UVLO))
    {
        SERIAL_ECHOLNPGM("INT4");
        uvlo_drain_reset();
    }
}

ISR(INT4_vect) {
	EIMSK &= ~(1 << 4); //disable INT4 interrupt to make sure that this code will be executed just once 
	SERIAL_ECHOLNPGM("INT4");
    //fire normal uvlo only in case where EEPROM_UVLO is 0 or if IS_SD_PRINTING is 1. 
     if(PRINTER_ACTIVE && (!(eeprom_read_byte((uint8_t*)EEPROM_UVLO)))) uvlo_();
     if(eeprom_read_byte((uint8_t*)EEPROM_UVLO)) uvlo_tiny();
}

void recover_print(uint8_t automatic) {
	char cmd[30];
	lcd_update_enable(true);
	lcd_update(2);
  lcd_setstatuspgm(_i("Recovering print    "));////MSG_RECOVERING_PRINT c=20 r=1

  // Recover position, temperatures and extrude_multipliers
  bool mbl_was_active = recover_machine_state_after_power_panic();

  // Lift the print head 25mm, first to avoid collisions with oozed material with the print,
  // and second also so one may remove the excess priming material.
  if(eeprom_read_byte((uint8_t*)EEPROM_UVLO) == 1)
  {
      sprintf_P(cmd, PSTR("G1 Z%.3f F800"), current_position[Z_AXIS] + 25);
      enquecommand(cmd);
  }

  // Home X and Y axes. Homing just X and Y shall not touch the babystep and the world2machine
  // transformation status. G28 will not touch Z when MBL is off.
	enquecommand_P(PSTR("G28 X Y"));
  // Set the target bed and nozzle temperatures and wait.
	sprintf_P(cmd, PSTR("M104 S%d"), target_temperature[active_extruder]);
	enquecommand(cmd);
	sprintf_P(cmd, PSTR("M190 S%d"), target_temperature_bed);
	enquecommand(cmd);
	sprintf_P(cmd, PSTR("M109 S%d"), target_temperature[active_extruder]);
	enquecommand(cmd);

	enquecommand_P(PSTR("M83")); //E axis relative mode

    // If not automatically recoreverd (long power loss)
    if(automatic == 0){
        //Extrude some filament to stabilize the pressure
        enquecommand_P(PSTR("G1 E5 F120"));
        // Retract to be consistent with a short pause
        sprintf_P(cmd, PSTR("G1 E%-0.3f F2700"), default_retraction);
        enquecommand(cmd);
    }

	printf_P(_N("After waiting for temp:\nCurrent pos X_AXIS:%.3f\nCurrent pos Y_AXIS:%.3f\n"), current_position[X_AXIS], current_position[Y_AXIS]);

  // Restart the print.
  restore_print_from_eeprom(mbl_was_active);
  printf_P(_N("Current pos Z_AXIS:%.3f\nCurrent pos E_AXIS:%.3f\n"), current_position[Z_AXIS], current_position[E_AXIS]);
}

bool recover_machine_state_after_power_panic()
{
  // 1) Preset some dummy values for the XY axes
  current_position[X_AXIS] = 0;
  current_position[Y_AXIS] = 0;

  // 2) Restore the mesh bed leveling offsets, but not the MBL status.
  // This is 2*7*7=98 bytes, which takes 98*3.4us=333us in worst case.
  bool mbl_was_active = false;
  for (int8_t mesh_point = 0; mesh_point < MESH_NUM_X_POINTS * MESH_NUM_Y_POINTS; ++ mesh_point) {
    uint8_t ix = mesh_point % MESH_NUM_X_POINTS; // from 0 to MESH_NUM_X_POINTS - 1
    uint8_t iy = mesh_point / MESH_NUM_X_POINTS;
    // Scale the z value to 10u resolution.
    int16_t v;
    eeprom_read_block(&v, (void*)(EEPROM_UVLO_MESH_BED_LEVELING_FULL+2*mesh_point), 2);
    if (v != 0)
      mbl_was_active = true;
    mbl.z_values[iy][ix] = float(v) * 0.001f;
  }

  // Recover the physical coordinate of the Z axis at the time of the power panic.
  // The current position after power panic is moved to the next closest 0th full step.
  current_position[Z_AXIS] = eeprom_read_float((float*)(EEPROM_UVLO_TINY_CURRENT_POSITION_Z));

  // Recover last E axis position
  current_position[E_AXIS] = eeprom_read_float((float*)(EEPROM_UVLO_CURRENT_POSITION_E));

  memcpy(destination, current_position, sizeof(destination));

  SERIAL_ECHOPGM("recover_machine_state_after_power_panic, initial ");
  print_world_coordinates();

  // 3) Initialize the logical to physical coordinate system transformation.
  world2machine_initialize();
//  SERIAL_ECHOPGM("recover_machine_state_after_power_panic, initial ");
//  print_mesh_bed_leveling_table();

  // 4) Load the baby stepping value, which is expected to be active at the time of power panic.
  // The baby stepping value is used to reset the physical Z axis when rehoming the Z axis.
  babystep_load();

  // 5) Set the physical positions from the logical positions using the world2machine transformation
  // This is only done to inizialize Z/E axes with physical locations, since X/Y are unknown.
  plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);

  // 6) Power up the Z motors, mark their positions as known.
  axis_known_position[Z_AXIS] = true;
  enable_z();

  // 7) Recover the target temperatures.
  target_temperature[active_extruder] = eeprom_read_word((uint16_t*)EEPROM_UVLO_TARGET_HOTEND);
  target_temperature_bed = eeprom_read_byte((uint8_t*)EEPROM_UVLO_TARGET_BED);

  // 8) Recover extruder multipilers
  extruder_multiplier[0] = eeprom_read_float((float*)(EEPROM_EXTRUDER_MULTIPLIER_0));
#if EXTRUDERS > 1
  extruder_multiplier[1] = eeprom_read_float((float*)(EEPROM_EXTRUDER_MULTIPLIER_1));
#if EXTRUDERS > 2
  extruder_multiplier[2] = eeprom_read_float((float*)(EEPROM_EXTRUDER_MULTIPLIER_2));
#endif
#endif
  extrudemultiply = (int)eeprom_read_word((uint16_t*)(EEPROM_EXTRUDEMULTIPLY));

  // 9) Recover the saved target
  saved_target[X_AXIS] = eeprom_read_float((float*)(EEPROM_UVLO_SAVED_TARGET+0*4));
  saved_target[Y_AXIS] = eeprom_read_float((float*)(EEPROM_UVLO_SAVED_TARGET+1*4));
  saved_target[Z_AXIS] = eeprom_read_float((float*)(EEPROM_UVLO_SAVED_TARGET+2*4));
  saved_target[E_AXIS] = eeprom_read_float((float*)(EEPROM_UVLO_SAVED_TARGET+3*4));

#ifdef LIN_ADVANCE
  extruder_advance_K = eeprom_read_float((float*)EEPROM_UVLO_LA_K);
#endif

  return mbl_was_active;
}

void restore_print_from_eeprom(bool mbl_was_active) {
	int feedrate_rec;
	int feedmultiply_rec;
	uint8_t fan_speed_rec;
	char cmd[30];
	char filename[13];
	uint8_t depth = 0;
	char dir_name[9];

	fan_speed_rec = eeprom_read_byte((uint8_t*)EEPROM_UVLO_FAN_SPEED);
    feedrate_rec = eeprom_read_word((uint16_t*)EEPROM_UVLO_FEEDRATE);
    feedmultiply_rec = eeprom_read_word((uint16_t*)EEPROM_UVLO_FEEDMULTIPLY);
	SERIAL_ECHOPGM("Feedrate:");
	MYSERIAL.print(feedrate_rec);
	SERIAL_ECHOPGM(", feedmultiply:");
	MYSERIAL.println(feedmultiply_rec);

	depth = eeprom_read_byte((uint8_t*)EEPROM_DIR_DEPTH);
	
	MYSERIAL.println(int(depth));
	for (int i = 0; i < depth; i++) {
		for (int j = 0; j < 8; j++) {
			dir_name[j] = eeprom_read_byte((uint8_t*)EEPROM_DIRS + j + 8 * i);
		}
		dir_name[8] = '\0';
		MYSERIAL.println(dir_name);
		strcpy(dir_names[i], dir_name);
		card.chdir(dir_name);
	}

	for (int i = 0; i < 8; i++) {
		filename[i] = eeprom_read_byte((uint8_t*)EEPROM_FILENAME + i);
	}
	filename[8] = '\0';

	MYSERIAL.print(filename);
	strcat_P(filename, PSTR(".gco"));
	sprintf_P(cmd, PSTR("M23 %s"), filename);
	enquecommand(cmd);
	uint32_t position = eeprom_read_dword((uint32_t*)(EEPROM_FILE_POSITION));
	SERIAL_ECHOPGM("Position read from eeprom:");
	MYSERIAL.println(position);

    // Move to the XY print position in logical coordinates, where the print has been killed, but
    // without shifting Z along the way. This requires performing the move without mbl.
	sprintf_P(cmd, PSTR("G1 X%f Y%f F3000"),
              eeprom_read_float((float*)(EEPROM_UVLO_CURRENT_POSITION + 0)),
              eeprom_read_float((float*)(EEPROM_UVLO_CURRENT_POSITION + 4)));
	enquecommand(cmd);

    // Enable MBL and switch to logical positioning
    if (mbl_was_active)
        enquecommand_P(PSTR("PRUSA MBL V1"));

    // Move the Z axis down to the print, in logical coordinates.
    sprintf_P(cmd, PSTR("G1 Z%f"), eeprom_read_float((float*)(EEPROM_UVLO_CURRENT_POSITION_Z)));
	enquecommand(cmd);

  // Unretract.
    sprintf_P(cmd, PSTR("G1 E%0.3f F2700"), default_retraction);
    enquecommand(cmd);
  // Recover final E axis position and mode
    float pos_e = eeprom_read_float((float*)(EEPROM_UVLO_CURRENT_POSITION_E));
    sprintf_P(cmd, PSTR("G92 E"));
    dtostrf(pos_e, 6, 3, cmd + strlen(cmd));
    enquecommand(cmd);
    if (eeprom_read_byte((uint8_t*)EEPROM_UVLO_E_ABS))
        enquecommand_P(PSTR("M82")); //E axis abslute mode
  // Set the feedrates saved at the power panic.
	sprintf_P(cmd, PSTR("G1 F%d"), feedrate_rec);
	enquecommand(cmd);
	sprintf_P(cmd, PSTR("M220 S%d"), feedmultiply_rec);
	enquecommand(cmd);
  // Set the fan speed saved at the power panic.
	strcpy_P(cmd, PSTR("M106 S"));
	strcat(cmd, itostr3(int(fan_speed_rec)));
	enquecommand(cmd);

  // Set a position in the file.
  sprintf_P(cmd, PSTR("M26 S%lu"), position);
  enquecommand(cmd);
  enquecommand_P(PSTR("G4 S0")); 
  enquecommand_P(PSTR("PRUSA uvlo"));
}
#endif //UVLO_SUPPORT


//! @brief Immediately stop print moves
//!
//! Immediately stop print moves, save current extruder temperature and position to RAM.
//! If printing from sd card, position in file is saved.
//! If printing from USB, line number is saved.
//!
//! @param z_move
//! @param e_move
void stop_and_save_print_to_ram(float z_move, float e_move)
{
	if (saved_printing) return;
#if 0
	unsigned char nplanner_blocks;
#endif
	unsigned char nlines;
	uint16_t sdlen_planner;
	uint16_t sdlen_cmdqueue;
	

	cli();
	if (card.sdprinting) {
#if 0
		nplanner_blocks = number_of_blocks();
#endif
		saved_sdpos = sdpos_atomic; //atomic sd position of last command added in queue
		sdlen_planner = planner_calc_sd_length(); //length of sd commands in planner
		saved_sdpos -= sdlen_planner;
		sdlen_cmdqueue = cmdqueue_calc_sd_length(); //length of sd commands in cmdqueue
		saved_sdpos -= sdlen_cmdqueue;
		saved_printing_type = PRINTING_TYPE_SD;

	}
	else if (is_usb_printing) { //reuse saved_sdpos for storing line number
		 saved_sdpos = gcode_LastN; //start with line number of command added recently to cmd queue
		 //reuse planner_calc_sd_length function for getting number of lines of commands in planner:
		 nlines = planner_calc_sd_length(); //number of lines of commands in planner 
		 saved_sdpos -= nlines;
		 saved_sdpos -= buflen; //number of blocks in cmd buffer
		 saved_printing_type = PRINTING_TYPE_USB;
	}
	else {
		 saved_printing_type = PRINTING_TYPE_NONE;
		 //not sd printing nor usb printing
	}

#if 0
  SERIAL_ECHOPGM("SDPOS_ATOMIC="); MYSERIAL.println(sdpos_atomic, DEC);
  SERIAL_ECHOPGM("SDPOS="); MYSERIAL.println(card.get_sdpos(), DEC);
  SERIAL_ECHOPGM("SDLEN_PLAN="); MYSERIAL.println(sdlen_planner, DEC);
  SERIAL_ECHOPGM("SDLEN_CMDQ="); MYSERIAL.println(sdlen_cmdqueue, DEC);
  SERIAL_ECHOPGM("PLANNERBLOCKS="); MYSERIAL.println(int(nplanner_blocks), DEC);
  SERIAL_ECHOPGM("SDSAVED="); MYSERIAL.println(saved_sdpos, DEC);
  //SERIAL_ECHOPGM("SDFILELEN="); MYSERIAL.println(card.fileSize(), DEC);


  {
    card.setIndex(saved_sdpos);
    SERIAL_ECHOLNPGM("Content of planner buffer: ");
    for (unsigned int idx = 0; idx < sdlen_planner; ++ idx)
      MYSERIAL.print(char(card.get()));
    SERIAL_ECHOLNPGM("Content of command buffer: ");
    for (unsigned int idx = 0; idx < sdlen_cmdqueue; ++ idx)
      MYSERIAL.print(char(card.get()));
    SERIAL_ECHOLNPGM("End of command buffer");
  }
  {
    // Print the content of the planner buffer, line by line:
    card.setIndex(saved_sdpos);
    int8_t iline = 0;
    for (unsigned char idx = block_buffer_tail; idx != block_buffer_head; idx = (idx + 1) & (BLOCK_BUFFER_SIZE - 1), ++ iline) {
      SERIAL_ECHOPGM("Planner line (from file): ");
      MYSERIAL.print(int(iline), DEC);
      SERIAL_ECHOPGM(", length: ");
      MYSERIAL.print(block_buffer[idx].sdlen, DEC);
      SERIAL_ECHOPGM(", steps: (");
      MYSERIAL.print(block_buffer[idx].steps_x, DEC);
      SERIAL_ECHOPGM(",");
      MYSERIAL.print(block_buffer[idx].steps_y, DEC);
      SERIAL_ECHOPGM(",");
      MYSERIAL.print(block_buffer[idx].steps_z, DEC);
      SERIAL_ECHOPGM(",");
      MYSERIAL.print(block_buffer[idx].steps_e, DEC);
      SERIAL_ECHOPGM("), events: ");
      MYSERIAL.println(block_buffer[idx].step_event_count, DEC);
      for (int len = block_buffer[idx].sdlen; len > 0; -- len)
        MYSERIAL.print(char(card.get()));
    }
  }
  {
    // Print the content of the command buffer, line by line:
    int8_t iline = 0;
    union {
        struct {
            char lo;
            char hi;
        } lohi;
        uint16_t value;
    } sdlen_single;
    int _bufindr = bufindr;
	for (int _buflen  = buflen; _buflen > 0; ++ iline) {
        if (cmdbuffer[_bufindr] == CMDBUFFER_CURRENT_TYPE_SDCARD) {
            sdlen_single.lohi.lo = cmdbuffer[_bufindr + 1];
            sdlen_single.lohi.hi = cmdbuffer[_bufindr + 2];
        }		 
        SERIAL_ECHOPGM("Buffer line (from buffer): ");
        MYSERIAL.print(int(iline), DEC);
        SERIAL_ECHOPGM(", type: ");
        MYSERIAL.print(int(cmdbuffer[_bufindr]), DEC);
        SERIAL_ECHOPGM(", len: ");
        MYSERIAL.println(sdlen_single.value, DEC);
        // Print the content of the buffer line.
        MYSERIAL.println(cmdbuffer + _bufindr + CMDHDRSIZE);

        SERIAL_ECHOPGM("Buffer line (from file): ");
        MYSERIAL.println(int(iline), DEC);
        for (; sdlen_single.value > 0; -- sdlen_single.value)
          MYSERIAL.print(char(card.get()));

        if (-- _buflen == 0)
          break;
        // First skip the current command ID and iterate up to the end of the string.
        for (_bufindr += CMDHDRSIZE; cmdbuffer[_bufindr] != 0; ++ _bufindr) ;
        // Second, skip the end of string null character and iterate until a nonzero command ID is found.
        for (++ _bufindr; _bufindr < sizeof(cmdbuffer) && cmdbuffer[_bufindr] == 0; ++ _bufindr) ;
        // If the end of the buffer was empty,
        if (_bufindr == sizeof(cmdbuffer)) {
            // skip to the start and find the nonzero command.
            for (_bufindr = 0; cmdbuffer[_bufindr] == 0; ++ _bufindr) ;
        }
    }
  }
#endif

  // save the global state at planning time
  if (current_block)
  {
      memcpy(saved_target, current_block->gcode_target, sizeof(saved_target));
      saved_feedrate2 = current_block->gcode_feedrate;
  }
  else
  {
      saved_target[0] = SAVED_TARGET_UNSET;
      saved_feedrate2 = feedrate;
  }

	planner_abort_hard(); //abort printing
	memcpy(saved_pos, current_position, sizeof(saved_pos));
    saved_feedmultiply2 = feedmultiply; //save feedmultiply
	saved_active_extruder = active_extruder; //save active_extruder
	saved_extruder_temperature = degTargetHotend(active_extruder);
	saved_extruder_relative_mode = axis_relative_modes[E_AXIS];
	saved_fanSpeed = fanSpeed;
	cmdqueue_reset(); //empty cmdqueue
	card.sdprinting = false;
//	card.closefile();
	saved_printing = true;
  // We may have missed a stepper timer interrupt. Be safe than sorry, reset the stepper timer before re-enabling interrupts.
  st_reset_timer();
	sei();
	if ((z_move != 0) || (e_move != 0)) { // extruder or z move
#if 1
    // Rather than calling plan_buffer_line directly, push the move into the command queue so that
    // the caller can continue processing. This is used during powerpanic to save the state as we
    // move away from the print.
    char buf[48];

    if(e_move)
    {
        // First unretract (relative extrusion)
        if(!saved_extruder_relative_mode){
            enquecommand(PSTR("M83"), true);
        }
        //retract 45mm/s
        // A single sprintf may not be faster, but is definitely 20B shorter
        // than a sequence of commands building the string piece by piece
        // A snprintf would have been a safer call, but since it is not used
        // in the whole program, its implementation would bring more bytes to the total size
        // The behavior of dtostrf 8,3 should be roughly the same as %-0.3
        sprintf_P(buf, PSTR("G1 E%-0.3f F2700"), e_move);
        enquecommand(buf, false);
    }

    if(z_move)
    {
        // Then lift Z axis
        sprintf_P(buf, PSTR("G1 Z%-0.3f F%-0.3f"), saved_pos[Z_AXIS] + z_move, homing_feedrate[Z_AXIS]);
        enquecommand(buf, false);
    }

    // If this call is invoked from the main Arduino loop() function, let the caller know that the command
    // in the command queue is not the original command, but a new one, so it should not be removed from the queue.
    repeatcommand_front();
#else
		plan_buffer_line(saved_pos[X_AXIS], saved_pos[Y_AXIS], saved_pos[Z_AXIS] + z_move, saved_pos[E_AXIS] + e_move, homing_feedrate[Z_AXIS], active_extruder);
    st_synchronize(); //wait moving
    memcpy(current_position, saved_pos, sizeof(saved_pos));
    memcpy(destination, current_position, sizeof(destination));
#endif
    waiting_inside_plan_buffer_line_print_aborted = true; //unroll the stack
  }
}

//! @brief Restore print from ram
//!
//! Restore print saved by stop_and_save_print_to_ram(). Is blocking, restores
//! print fan speed, waits for extruder temperature restore, then restores
//! position and continues print moves.
//!
//! Internally lcd_update() is called by wait_for_heater().
//!
//! @param e_move
void restore_print_from_ram_and_continue(float e_move)
{
	if (!saved_printing) return;
	
#ifdef FANCHECK
	// Do not allow resume printing if fans are still not ok
	if ((fan_check_error != EFCE_OK) && (fan_check_error != EFCE_FIXED)) return;
    if (fan_check_error == EFCE_FIXED) fan_check_error = EFCE_OK; //reenable serial stream processing if printing from usb
#endif
	
//	for (int axis = X_AXIS; axis <= E_AXIS; axis++)
//	    current_position[axis] = st_get_position_mm(axis);
	active_extruder = saved_active_extruder; //restore active_extruder
	fanSpeed = saved_fanSpeed;
	if (degTargetHotend(saved_active_extruder) != saved_extruder_temperature)
	{
		setTargetHotendSafe(saved_extruder_temperature, saved_active_extruder);
		heating_status = 1;
		wait_for_heater(_millis(), saved_active_extruder);
		heating_status = 2;
	}
	axis_relative_modes[E_AXIS] = saved_extruder_relative_mode;
	float e = saved_pos[E_AXIS] - e_move;
	plan_set_e_position(e);
  
  #ifdef FANCHECK
    fans_check_enabled = false;
  #endif

	//first move print head in XY to the saved position:
	plan_buffer_line(saved_pos[X_AXIS], saved_pos[Y_AXIS], current_position[Z_AXIS], saved_pos[E_AXIS] - e_move, homing_feedrate[Z_AXIS]/13, active_extruder);
	//then move Z
	plan_buffer_line(saved_pos[X_AXIS], saved_pos[Y_AXIS], saved_pos[Z_AXIS], saved_pos[E_AXIS] - e_move, homing_feedrate[Z_AXIS]/13, active_extruder);
	//and finaly unretract (35mm/s)
	plan_buffer_line(saved_pos[X_AXIS], saved_pos[Y_AXIS], saved_pos[Z_AXIS], saved_pos[E_AXIS], FILAMENTCHANGE_RFEED, active_extruder);
	st_synchronize();

  #ifdef FANCHECK
    fans_check_enabled = true;
  #endif

    // restore original feedrate/feedmultiply _after_ restoring the extruder position
	feedrate = saved_feedrate2;
	feedmultiply = saved_feedmultiply2;

	memcpy(current_position, saved_pos, sizeof(saved_pos));
	memcpy(destination, current_position, sizeof(destination));
	if (saved_printing_type == PRINTING_TYPE_SD) { //was sd printing
		card.setIndex(saved_sdpos);
		sdpos_atomic = saved_sdpos;
		card.sdprinting = true;
	}
	else if (saved_printing_type == PRINTING_TYPE_USB) { //was usb printing
		gcode_LastN = saved_sdpos; //saved_sdpos was reused for storing line number when usb printing
		serial_count = 0; 
		FlushSerialRequestResend();
	}
	else {
		//not sd printing nor usb printing
	}

	SERIAL_PROTOCOLLNRPGM(MSG_OK); //dummy response because of octoprint is waiting for this
	lcd_setstatuspgm(_T(WELCOME_MSG));
    saved_printing_type = PRINTING_TYPE_NONE;
	saved_printing = false;
    waiting_inside_plan_buffer_line_print_aborted = true; //unroll the stack
}

// Cancel the state related to a currently saved print
void cancel_saved_printing()
{
    eeprom_update_byte((uint8_t*)EEPROM_UVLO, 0);
    saved_target[0] = SAVED_TARGET_UNSET;
    saved_printing_type = PRINTING_TYPE_NONE;
    saved_printing = false;
}

void print_world_coordinates()
{
	printf_P(_N("world coordinates: (%.3f, %.3f, %.3f)\n"), current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS]);
}

void print_physical_coordinates()
{
	printf_P(_N("physical coordinates: (%.3f, %.3f, %.3f)\n"), st_get_position_mm(X_AXIS), st_get_position_mm(Y_AXIS), st_get_position_mm(Z_AXIS));
}

void print_mesh_bed_leveling_table()
{
  SERIAL_ECHOPGM("mesh bed leveling: ");
  for (int8_t y = 0; y < MESH_NUM_Y_POINTS; ++ y)
    for (int8_t x = 0; x < MESH_NUM_Y_POINTS; ++ x) {
      MYSERIAL.print(mbl.z_values[y][x], 3);
      SERIAL_ECHOPGM(" ");
    }
  SERIAL_ECHOLNPGM("");
}

uint16_t print_time_remaining() {
	uint16_t print_t = PRINT_TIME_REMAINING_INIT;
#ifdef TMC2130 
	if (SilentModeMenu == SILENT_MODE_OFF) print_t = print_time_remaining_normal;
	else print_t = print_time_remaining_silent;
#else
	print_t = print_time_remaining_normal;
#endif //TMC2130
	if ((print_t != PRINT_TIME_REMAINING_INIT) && (feedmultiply != 0)) print_t = 100ul * print_t / feedmultiply;
	return print_t;
}

uint8_t calc_percent_done()
{
	//in case that we have information from M73 gcode return percentage counted by slicer, else return percentage counted as byte_printed/filesize
	uint8_t percent_done = 0;
#ifdef TMC2130
	if (SilentModeMenu == SILENT_MODE_OFF && print_percent_done_normal <= 100) {
		percent_done = print_percent_done_normal;
	}
	else if (print_percent_done_silent <= 100) {
		percent_done = print_percent_done_silent;
	}
#else
	if (print_percent_done_normal <= 100) {
		percent_done = print_percent_done_normal;
	}
#endif //TMC2130
	else {
		percent_done = card.percentDone();
	}
	return percent_done;
}

static void print_time_remaining_init()
{
	print_time_remaining_normal = PRINT_TIME_REMAINING_INIT;
	print_time_remaining_silent = PRINT_TIME_REMAINING_INIT;
	print_percent_done_normal = PRINT_PERCENT_DONE_INIT;
	print_percent_done_silent = PRINT_PERCENT_DONE_INIT;
}

void load_filament_final_feed()
{
	current_position[E_AXIS]+= FILAMENTCHANGE_FINALFEED;
	plan_buffer_line_curposXYZE(FILAMENTCHANGE_EFEED_FINAL, active_extruder);
}

//! @brief Wait for user to check the state
//! @par nozzle_temp nozzle temperature to load filament
void M600_check_state(float nozzle_temp)
{
    lcd_change_fil_state = 0;
    while (lcd_change_fil_state != 1)
    {
        lcd_change_fil_state = 0;
        KEEPALIVE_STATE(PAUSED_FOR_USER);
        lcd_alright();
        KEEPALIVE_STATE(IN_HANDLER);
        switch(lcd_change_fil_state)
        {
        // Filament failed to load so load it again
        case 2:
            if (mmu_enabled)
                mmu_M600_load_filament(false, nozzle_temp); //nonautomatic load; change to "wrong filament loaded" option?
            else
                M600_load_filament_movements();
            break;

        // Filament loaded properly but color is not clear
        case 3:
            st_synchronize();
            load_filament_final_feed();
            lcd_loading_color();
            st_synchronize();
            break;

        // Everything good
        default:
            lcd_change_success();
            break;
        }
    }
}

//! @brief Wait for user action
//!
//! Beep, manage nozzle heater and wait for user to start unload filament
//! If times out, active extruder temperature is set to 0.
//!
//! @param HotendTempBckp Temperature to be restored for active extruder, after user resolves MMU problem.
void M600_wait_for_user(float HotendTempBckp) {

		KEEPALIVE_STATE(PAUSED_FOR_USER);

		int counterBeep = 0;
		unsigned long waiting_start_time = _millis();
		uint8_t wait_for_user_state = 0;
		lcd_display_message_fullscreen_P(_T(MSG_PRESS_TO_UNLOAD));
		bool bFirst=true;

		while (!(wait_for_user_state == 0 && lcd_clicked())){
			manage_heater();
			manage_inactivity(true);

			#if BEEPER > 0
			if (counterBeep == 500) {
				counterBeep = 0;
			}
			SET_OUTPUT(BEEPER);
			if (counterBeep == 0) {
				if((eSoundMode==e_SOUND_MODE_BLIND)|| (eSoundMode==e_SOUND_MODE_LOUD)||((eSoundMode==e_SOUND_MODE_ONCE)&&bFirst))
				{
					bFirst=false;
					WRITE(BEEPER, HIGH);
				}
			}
			if (counterBeep == 20) {
				WRITE(BEEPER, LOW);
			}
				
			counterBeep++;
			#endif //BEEPER > 0
			
			switch (wait_for_user_state) {
			case 0: //nozzle is hot, waiting for user to press the knob to unload filament
				delay_keep_alive(4);

				if (_millis() > waiting_start_time + (unsigned long)M600_TIMEOUT * 1000) {
					lcd_display_message_fullscreen_P(_i("Press knob to preheat nozzle and continue."));////MSG_PRESS_TO_PREHEAT c=20 r=4
					wait_for_user_state = 1;
					setAllTargetHotends(0);
					st_synchronize();
					disable_e0();
					disable_e1();
					disable_e2();
				}
				break;
			case 1: //nozzle target temperature is set to zero, waiting for user to start nozzle preheat
				delay_keep_alive(4);
		
				if (lcd_clicked()) {
					setTargetHotend(HotendTempBckp, active_extruder);
					lcd_wait_for_heater();

					wait_for_user_state = 2;
				}
				break;
			case 2: //waiting for nozzle to reach target temperature

				if (abs(degTargetHotend(active_extruder) - degHotend(active_extruder)) < 1) {
					lcd_display_message_fullscreen_P(_T(MSG_PRESS_TO_UNLOAD));
					waiting_start_time = _millis();
					wait_for_user_state = 0;
				}
				else {
					counterBeep = 20; //beeper will be inactive during waiting for nozzle preheat
					lcd_set_cursor(1, 4);
					lcd_print(ftostr3(degHotend(active_extruder)));
				}
				break;

			}

		}
		WRITE(BEEPER, LOW);
}

void M600_load_filament_movements()
{
#ifdef SNMM
	display_loading();
	do
	{
		current_position[E_AXIS] += 0.002;
		plan_buffer_line_curposXYZE(500, active_extruder);
		delay_keep_alive(2);
	}
	while (!lcd_clicked());
	st_synchronize();
	current_position[E_AXIS] += bowden_length[mmu_extruder];
	plan_buffer_line_curposXYZE(3000, active_extruder);
	current_position[E_AXIS] += FIL_LOAD_LENGTH - 60;
	plan_buffer_line_curposXYZE(1400, active_extruder);
	current_position[E_AXIS] += 40;
	plan_buffer_line_curposXYZE(400, active_extruder);
	current_position[E_AXIS] += 10;
	plan_buffer_line_curposXYZE(50, active_extruder);
#else
	current_position[E_AXIS]+= FILAMENTCHANGE_FIRSTFEED ;
	plan_buffer_line_curposXYZE(FILAMENTCHANGE_EFEED_FIRST, active_extruder); 
#endif                
	load_filament_final_feed();
	lcd_loading_filament();
	st_synchronize();
}

void M600_load_filament() {
	//load filament for single material and SNMM 
	lcd_wait_interact();

	//load_filament_time = _millis();
	KEEPALIVE_STATE(PAUSED_FOR_USER);

#ifdef PAT9125
	fsensor_autoload_check_start();
#endif //PAT9125
	while(!lcd_clicked())
	{
		manage_heater();
		manage_inactivity(true);
#ifdef FILAMENT_SENSOR
		if (fsensor_check_autoload())
		{
      Sound_MakeCustom(50,1000,false);
			break;
		}
#endif //FILAMENT_SENSOR
	}
#ifdef PAT9125
	fsensor_autoload_check_stop();
#endif //PAT9125
	KEEPALIVE_STATE(IN_HANDLER);

#ifdef FSENSOR_QUALITY
	fsensor_oq_meassure_start(70);
#endif //FSENSOR_QUALITY

	M600_load_filament_movements();

      Sound_MakeCustom(50,1000,false);

#ifdef FSENSOR_QUALITY
	fsensor_oq_meassure_stop();

	if (!fsensor_oq_result())
	{
		bool disable = lcd_show_fullscreen_message_yes_no_and_wait_P(_i("Fil. sensor response is poor, disable it?"), false, true);
		lcd_update_enable(true);
		lcd_update(2);
		if (disable)
			fsensor_disable();
	}
#endif //FSENSOR_QUALITY
	lcd_update_enable(false);
}


//! @brief Wait for click
//!
//! Set
void marlin_wait_for_click()
{
    int8_t busy_state_backup = busy_state;
    KEEPALIVE_STATE(PAUSED_FOR_USER);
    lcd_consume_click();
    while(!lcd_clicked())
    {
        manage_heater();
        manage_inactivity(true);
        lcd_update(0);
    }
    KEEPALIVE_STATE(busy_state_backup);
}

#define FIL_LOAD_LENGTH 60

#ifdef PSU_Delta
bool bEnableForce_z;

void init_force_z()
{
WRITE(Z_ENABLE_PIN,Z_ENABLE_ON);
bEnableForce_z=true;                              // "true"-value enforce "disable_force_z()" executing
disable_force_z();
}

void check_force_z()
{
if(!(bEnableForce_z||eeprom_read_byte((uint8_t*)EEPROM_SILENT)))
     init_force_z();                              // causes enforced switching into disable-state
}

void disable_force_z()
{
    if(!bEnableForce_z) return;   // motor already disabled (may be ;-p )

    bEnableForce_z=false;

    // switching to silent mode
#ifdef TMC2130
    tmc2130_mode=TMC2130_MODE_SILENT;
    update_mode_profile();
    tmc2130_init(true);
#endif // TMC2130
}


void enable_force_z()
{
if(bEnableForce_z)
     return;                                      // motor already enabled (may be ;-p )
bEnableForce_z=true;

// mode recovering
#ifdef TMC2130
tmc2130_mode=eeprom_read_byte((uint8_t*)EEPROM_SILENT)?TMC2130_MODE_SILENT:TMC2130_MODE_NORMAL;
update_mode_profile();
tmc2130_init(true);
#endif // TMC2130

WRITE(Z_ENABLE_PIN,Z_ENABLE_ON);                  // slightly redundant ;-p
}
#endif // PSU_Delta
