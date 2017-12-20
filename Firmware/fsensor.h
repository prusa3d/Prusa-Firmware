#ifndef FSENSOR_H
#define FSENSOR_H

#include "planner.h"

//save restore printing
extern void fsensor_stop_and_save_print();
extern void fsensor_restore_print_and_continue();

//temporarily enable/disable without writing to eeprom
extern void fsensor_block();
extern void fsensor_unblock();

//enable/disable
extern bool fsensor_enable();
extern void fsensor_disable();

//update (perform M600 on filament runout)
extern void fsensor_update();

//setup pin-change interrupt
extern void fsensor_setup_interrupt();

//
extern void fsensor_autoload_check_start(void);

//
extern void fsensor_autoload_check_stop(void);

//
extern bool fsensor_check_autoload(void);

//callbacks from stepper
extern void fsensor_st_block_begin(block_t* bl);
extern void fsensor_st_block_chunk(block_t* bl, int cnt);

//minimum meassured chunk length in steps
extern int16_t fsensor_chunk_len;
//M600 in progress
extern bool fsensor_M600;
//enable/disable flag
extern bool fsensor_enabled;
//not responding flag
extern bool fsensor_not_responding;

//autoload enable/disable flag
extern bool fsensor_autoload_enabled;


#endif //FSENSOR_H
