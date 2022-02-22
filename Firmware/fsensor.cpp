//! @file

#include "Marlin.h"

#include "fsensor.h"
#include <avr/pgmspace.h>
#include "pat9125.h"
#include "stepper.h"
#include "cmdqueue.h"
#include "ultralcd.h"
#include "mmu.h"
#include "cardreader.h"

#include "adc.h"
#include "temperature.h"
#include "config.h"

//! @name Basic parameters
//! @{
#define FSENSOR_CHUNK_LEN      1.25 //!< filament sensor chunk length (mm)
#define FSENSOR_ERR_MAX           4 //!< filament sensor maximum error/chunk count for runout detection

#define FSENSOR_SOFTERR_CMAX      3 //!< number of contiguous soft failures before a triggering a runout
#define FSENSOR_SOFTERR_DELTA 30000 //!< maximum interval (ms) to consider soft failures contiguous
//! @}

const char ERRMSG_PAT9125_NOT_RESP[] PROGMEM = "PAT9125 not responding (%d)!\n";

// PJ7 can not be used (does not have PinChangeInterrupt possibility)
#define FSENSOR_INT_PIN          75 //!< filament sensor interrupt pin PJ4
#define FSENSOR_INT_PIN_MASK   0x10 //!< filament sensor interrupt pin mask (bit4)
#define FSENSOR_INT_PIN_PIN_REG PINJ              // PIN register @ PJ4
#define FSENSOR_INT_PIN_VECT PCINT1_vect          // PinChange ISR @ PJ4
#define FSENSOR_INT_PIN_PCMSK_REG PCMSK1          // PinChangeMaskRegister @ PJ4
#define FSENSOR_INT_PIN_PCMSK_BIT PCINT13         // PinChange Interrupt / PinChange Enable Mask @ PJ4
#define FSENSOR_INT_PIN_PCICR_BIT PCIE1           // PinChange Interrupt Enable / Flag @ PJ4

//! enabled = initialized and sampled every chunk event
bool fsensor_enabled = true;
//! runout watching is done in fsensor_update (called from main loop)
bool fsensor_watch_runout = true;
//! not responding - is set if any communication error occurred during initialization or readout
bool fsensor_not_responding = false;

#ifdef PAT9125
uint8_t fsensor_int_pin_old = 0;
//! optical checking "chunk lenght" (already in steps)
int16_t fsensor_chunk_len = 0;
//! number of errors, updated in ISR
uint8_t fsensor_err_cnt = 0;
//! variable for accumulating step count (updated callbacks from stepper and ISR)
int16_t fsensor_st_cnt = 0;
//! count of total sensor "soft" failures (filament status checks)
uint8_t fsensor_softfail = 0;
//! timestamp of last soft failure
unsigned long fsensor_softfail_last = 0;
//! count of soft failures within the configured time
uint8_t fsensor_softfail_ccnt = 0;
#endif

#ifdef DEBUG_FSENSOR_LOG
//! log flag: 0=log disabled, 1=log enabled
uint8_t fsensor_log = 1;
#endif //DEBUG_FSENSOR_LOG


//! @name filament autoload variables
//! @{

//! autoload feature enabled
bool fsensor_autoload_enabled = true;
//! autoload watching enable/disable flag
bool fsensor_watch_autoload = false;

#ifdef PAT9125
//
uint16_t fsensor_autoload_y;
//
uint8_t fsensor_autoload_c;
//
uint32_t fsensor_autoload_last_millis;
//
uint8_t fsensor_autoload_sum;
//! @}
#endif


#ifdef IR_SENSOR_ANALOG
ClFsensorPCB oFsensorPCB;
ClFsensorActionNA oFsensorActionNA;
ShortTimer tIRsensorCheckTimer;
#endif //IR_SENSOR_ANALOG

void fsensor_stop_and_save_print(void)
{
    puts_P(PSTR("fsensor_stop_and_save_print"));
    stop_and_save_print_to_ram(0, 0);
    fsensor_watch_runout = false;
}

#ifdef PAT9125
// Reset all internal counters to zero, including stepper callbacks
void fsensor_reset_err_cnt()
{
    fsensor_err_cnt = 0;
    pat9125_y = 0;
    st_reset_fsensor();
}

void fsensor_set_axis_steps_per_unit(float u)
{
    fsensor_chunk_len = (int16_t)(FSENSOR_CHUNK_LEN * u);
}
#endif


void fsensor_restore_print_and_continue(void)
{
    puts_P(PSTR("fsensor_restore_print_and_continue"));
    fsensor_watch_runout = true;
#ifdef PAT9125
    fsensor_reset_err_cnt();
#endif
    restore_print_from_ram_and_continue(0);
}

// fsensor_checkpoint_print cuts the current print job at the current position,
// allowing new instructions to be inserted in the middle
void fsensor_checkpoint_print(void)
{
    puts_P(PSTR("fsensor_checkpoint_print"));
    stop_and_save_print_to_ram(0, 0);
    restore_print_from_ram_and_continue(0);
}

#ifdef IR_SENSOR_ANALOG
const char* FsensorIRVersionText()
{
	switch(oFsensorPCB)
	{
		case ClFsensorPCB::_Old:
			return _T(MSG_IR_03_OR_OLDER);
		case ClFsensorPCB::_Rev04:
			return _T(MSG_IR_04_OR_NEWER);
		default:
			return _T(MSG_IR_UNKNOWN);
	}
}
#endif //IR_SENSOR_ANALOG

void fsensor_init(void)
{
#ifdef PAT9125
	uint8_t pat9125 = pat9125_init();
	printf_P(PSTR("PAT9125_init:%u\n"), pat9125);
#endif //PAT9125
	uint8_t fsensor_enabled = eeprom_read_byte((uint8_t*)EEPROM_FSENSOR);
	fsensor_autoload_enabled=eeprom_read_byte((uint8_t*)EEPROM_FSENS_AUTOLOAD_ENABLED);
	fsensor_not_responding = false;
#ifdef PAT9125
	fsensor_set_axis_steps_per_unit(cs.axis_steps_per_unit[E_AXIS]);
	
	if (!pat9125){
		fsensor_enabled = 0; //disable sensor
		fsensor_not_responding = true;
	}
#endif //PAT9125
#ifdef IR_SENSOR_ANALOG
	oFsensorPCB = (ClFsensorPCB)eeprom_read_byte((uint8_t*)EEPROM_FSENSOR_PCB);
	oFsensorActionNA = (ClFsensorActionNA)eeprom_read_byte((uint8_t*)EEPROM_FSENSOR_ACTION_NA);

	// If the fsensor is not responding even at the start of the printer,
	// set this flag accordingly to show N/A in Settings->Filament sensor.
	// This is even valid for both fsensor board revisions (0.3 or older and 0.4).
	// Must be done after reading what type of fsensor board we have
	fsensor_not_responding = ! fsensor_IR_check();
#endif //IR_SENSOR_ANALOG
	if (fsensor_enabled){
		fsensor_enable(false);                  // (in this case) EEPROM update is not necessary
	} else {
		fsensor_disable(false);                 // (in this case) EEPROM update is not necessary
	}
	printf_P(PSTR("FSensor %S"), (fsensor_enabled?PSTR("ENABLED"):PSTR("DISABLED")));
#ifdef IR_SENSOR_ANALOG
	printf_P(PSTR(" (sensor board revision:%S)\n"), FsensorIRVersionText());
#else //IR_SENSOR_ANALOG
	MYSERIAL.println();
#endif //IR_SENSOR_ANALOG
	if (check_for_ir_sensor()){
		ir_sensor_detected = true;
	}
}

bool fsensor_enable(bool bUpdateEEPROM)
{
#ifdef PAT9125
    (void)bUpdateEEPROM; // silence unused warning in this variant

	if (mmu_enabled == false) { //filament sensor is pat9125, enable only if it is working
		uint8_t pat9125 = pat9125_init();
		printf_P(PSTR("PAT9125_init:%u\n"), pat9125);
		if (pat9125)
			fsensor_not_responding = false;
		else
			fsensor_not_responding = true;
		fsensor_enabled = pat9125 ? true : false;
		fsensor_watch_runout = true;
        fsensor_reset_err_cnt();
		eeprom_update_byte((uint8_t*)EEPROM_FSENSOR, fsensor_enabled ? 0x01 : 0x00);
		FSensorStateMenu = fsensor_enabled ? 1 : 0;
	}
	else //filament sensor is FINDA, always enable 
	{
		fsensor_enabled = true;
		eeprom_update_byte((uint8_t*)EEPROM_FSENSOR, 0x01);
		FSensorStateMenu = 1;
	}
#else // PAT9125
#ifdef IR_SENSOR_ANALOG
     if(!fsensor_IR_check())
          {
          bUpdateEEPROM=true;
          fsensor_enabled=false;
          fsensor_not_responding=true;
          FSensorStateMenu=0;
          }
     else {
#endif //IR_SENSOR_ANALOG
     fsensor_enabled=true;
     fsensor_not_responding=false;
     FSensorStateMenu=1;
#ifdef IR_SENSOR_ANALOG
          }
#endif //IR_SENSOR_ANALOG
     if(bUpdateEEPROM)
          eeprom_update_byte((uint8_t*)EEPROM_FSENSOR, FSensorStateMenu);
#endif //PAT9125
	return fsensor_enabled;
}

void fsensor_disable(bool bUpdateEEPROM)
{ 
	fsensor_enabled = false;
	FSensorStateMenu = 0;
     if(bUpdateEEPROM)
          eeprom_update_byte((uint8_t*)EEPROM_FSENSOR, 0x00); 
}

void fsensor_autoload_set(bool State)
{
#ifdef PAT9125
	if (!State) fsensor_autoload_check_stop();
#endif //PAT9125
	fsensor_autoload_enabled = State;
	eeprom_update_byte((unsigned char *)EEPROM_FSENS_AUTOLOAD_ENABLED, fsensor_autoload_enabled);
}

#ifdef IR_SENSOR_ANALOG
/// This is called only upon start of the printer or when switching the fsensor ON in the menu
/// We cannot do temporal window checks here (aka the voltage has been in some range for a period of time)
bool fsensor_IR_check(){
    if( IRsensor_Lmax_TRESHOLD <= current_voltage_raw_IR && current_voltage_raw_IR <= IRsensor_Hmin_TRESHOLD ){
        /// If the voltage is in forbidden range, the fsensor is ok, but the lever is mounted improperly.
        /// Or the user is so creative so that he can hold a piece of fillament in the hole in such a genius way,
        /// that the IR fsensor reading is within 1.5 and 3V ... this would have been highly unusual
        /// and would have been considered more like a sabotage than normal printer operation
        puts_P(PSTR("fsensor in forbidden range 1.5-3V - check sensor"));
        return false; 
    }
    if( oFsensorPCB == ClFsensorPCB::_Rev04 ){
        /// newer IR sensor cannot normally produce 4.6-5V, this is considered a failure/bad mount
        if( IRsensor_Hopen_TRESHOLD <= current_voltage_raw_IR && current_voltage_raw_IR <= IRsensor_VMax_TRESHOLD ){
            puts_P(PSTR("fsensor v0.4 in fault range 4.6-5V - unconnected"));
            return false;
        }
        /// newer IR sensor cannot normally produce 0-0.3V, this is considered a failure 
#if 0	//Disabled as it has to be decided if we gonna use this or not.
        if( IRsensor_Hopen_TRESHOLD <= current_voltage_raw_IR && current_voltage_raw_IR <= IRsensor_VMax_TRESHOLD ){
            puts_P(PSTR("fsensor v0.4 in fault range 0.0-0.3V - wrong IR sensor"));
            return false;
        }
#endif
    }
    /// If IR sensor is "uknown state" and filament is not loaded > 1.5V return false
#if 0
    if( (oFsensorPCB == ClFsensorPCB::_Undef) && ( current_voltage_raw_IR > IRsensor_Lmax_TRESHOLD ) ){
        puts_P(PSTR("Unknown IR sensor version and no filament loaded detected."));
        return false;
    }
#endif
    // otherwise the IR fsensor is considered working correctly
    return true;
}
#endif //IR_SENSOR_ANALOG
