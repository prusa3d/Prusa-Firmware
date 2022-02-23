//! @file
#ifndef FSENSOR_H
#define FSENSOR_H

#include <inttypes.h>
#include "config.h"


#ifdef PAT9125
// optical checking "chunk lenght" (already in steps)
extern int16_t fsensor_chunk_len;
// count of soft failures
extern uint8_t fsensor_softfail;

//! update axis resolution
extern void fsensor_set_axis_steps_per_unit(float u);

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

#endif //FSENSOR_H
