//! @file
#ifndef FSENSOR_H
#define FSENSOR_H

#include <inttypes.h>


//! minimum meassured chunk length in steps
extern int16_t fsensor_chunk_len;
// enable/disable flag
extern bool fsensor_enabled;
// not responding flag
extern bool fsensor_not_responding;
//enable/disable quality meassurement
extern bool fsensor_oq_meassure_enabled;


//! @name save restore printing
//! @{
extern void fsensor_stop_and_save_print(void);
extern void fsensor_restore_print_and_continue(void);
//! @}

//! initialize
extern void fsensor_init(void);

//! @name enable/disable
//! @{
extern bool fsensor_enable(void);
extern void fsensor_disable(void);
//! @}

//autoload feature enabled
extern bool fsensor_autoload_enabled;
extern void fsensor_autoload_set(bool State);

extern void fsensor_update(void);

//! setup pin-change interrupt
extern void fsensor_setup_interrupt(void);

//! @name autoload support
//! @{
extern void fsensor_autoload_check_start(void);
extern void fsensor_autoload_check_stop(void);
extern bool fsensor_check_autoload(void);
//! @}

//! @name optical quality measurement support
//! @{
extern void fsensor_oq_meassure_set(bool State);
extern void fsensor_oq_meassure_start(uint8_t skip);
extern void fsensor_oq_meassure_stop(void);
extern bool fsensor_oq_result(void);
//! @}


#include "planner.h"
//! @name callbacks from stepper
//! @{
extern void fsensor_st_block_begin(block_t* bl);
extern void fsensor_st_block_chunk(block_t* bl, int cnt);
//! @}

#endif //FSENSOR_H
