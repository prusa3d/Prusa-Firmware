//! @file
#ifndef FSENSOR_H
#define FSENSOR_H

#include <inttypes.h>
#include "config.h"


// enable/disable flag
extern bool fsensor_enabled;
// not responding flag
extern bool fsensor_not_responding;
#ifdef PAT9125
// optical checking "chunk lenght" (already in steps)
extern int16_t fsensor_chunk_len;
// count of soft failures
extern uint8_t fsensor_softfail;
#endif

//! @name save restore printing
//! @{
//! split the current gcode stream to insert new instructions
extern void fsensor_checkpoint_print(void);
//! @}

#ifdef PAT9125
//! update axis resolution
extern void fsensor_set_axis_steps_per_unit(float u);
#endif

//! @name enable/disable
//! @{
extern bool fsensor_enable(bool bUpdateEEPROM=true);
extern void fsensor_disable(bool bUpdateEEPROM=true);
//! @}

//autoload feature enabled
extern bool fsensor_autoload_enabled;
extern void fsensor_autoload_set(bool State);

#ifdef PAT9125
//! setup pin-change interrupt
extern void fsensor_setup_interrupt(void);

//! @name autoload support
//! @{

extern void fsensor_autoload_check_start(void);
extern void fsensor_autoload_check_stop(void);
#endif //PAT9125
//! @}

#ifdef PAT9125
//! @name callbacks from stepper
//! @{
extern void fsensor_st_block_chunk(int cnt);

// debugging
extern uint8_t fsensor_log;

// There's really nothing to do in block_begin: the stepper ISR likely has
// called us already at the end of the last block, making this integration
// redundant. LA1.5 might not always do that during a coasting move, so attempt
// to drain fsensor_st_cnt anyway at the beginning of the new block.
#define fsensor_st_block_begin(rev) fsensor_st_block_chunk(0)
//! @}
#endif //PAT9125

#ifdef IR_SENSOR_ANALOG

enum class ClFsensorActionNA:uint_least8_t
{
    _Continue=0,
    _Pause=1,
    _Undef=EEPROM_EMPTY_VALUE
};

extern ClFsensorActionNA oFsensorActionNA;

#endif //IR_SENSOR_ANALOG

#endif //FSENSOR_H
