#ifndef FSENSOR_H
#define FSENSOR_H

#include "planner.h"

//save restore printing
extern void fsensor_stop_and_save_print();
extern void fsensor_restore_print_and_continue();

//enable/disable
extern void fsensor_enable();
extern void fsensor_disable();

//update (perform M600 on filament runout)
extern void fsensor_update();

//setup pin-change interrupt
extern void fsensor_setup_interrupt();

//callbacks from stepper
extern void fsensor_st_block_begin(block_t* bl);
extern void fsensor_st_block_chunk(block_t* bl, int cnt);

//minimum meassured chunk length in steps
extern int16_t fsensor_chunk_len;
//M600 in progress
extern bool fsensor_M600;


#endif //FSENSOR_H
