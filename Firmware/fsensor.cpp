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

#include "Filament_sensor.h" //temporary

//! @name Basic parameters
//! @{
#define FSENSOR_CHUNK_LEN      1.25 //!< filament sensor chunk length (mm)
#define FSENSOR_ERR_MAX           4 //!< filament sensor maximum error/chunk count for runout detection

#define FSENSOR_SOFTERR_CMAX      3 //!< number of contiguous soft failures before a triggering a runout
#define FSENSOR_SOFTERR_DELTA 30000 //!< maximum interval (ms) to consider soft failures contiguous
//! @}

const char ERRMSG_PAT9125_NOT_RESP[] PROGMEM = "PAT9125 not responding (%d)!\n";

//! enabled = initialized and sampled every chunk event
bool fsensor_enabled = true;
//! runout watching is done in fsensor_update (called from main loop)
bool fsensor_watch_runout = true;
//! not responding - is set if any communication error occurred during initialization or readout
bool fsensor_not_responding = false;

#ifdef PAT9125
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


#ifdef IR_SENSOR_ANALOG
ClFsensorActionNA oFsensorActionNA;
#endif //IR_SENSOR_ANALOG

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


// fsensor_checkpoint_print cuts the current print job at the current position,
// allowing new instructions to be inserted in the middle
void fsensor_checkpoint_print(void)
{
    puts_P(PSTR("fsensor_checkpoint_print"));
    stop_and_save_print_to_ram(0, 0);
    restore_print_from_ram_and_continue(0);
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
     if(!fsensor.checkVoltage(fsensor.getVoltRaw()))
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
