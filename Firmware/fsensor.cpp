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
