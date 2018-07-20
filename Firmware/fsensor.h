#ifndef FSENSOR_H
#define FSENSOR_H

#include "planner.h"

#define FSENSOR_CHUNK_LEN      180  //filament sensor chunk length in steps - 0.64mm
#define FSENSOR_ERR_MAX         10  //filament sensor maximum error count for runout detection

//Optical quality meassurement params
#define FSENSOR_OQ_MAX_ER      5    //maximum error count for loading (~150mm)
#define FSENSOR_OQ_MIN_YD      2    //minimum yd per chunk
#define FSENSOR_OQ_MAX_YD      200  //maximum yd per chunk
#define FSENSOR_OQ_MAX_PD      3    //maximum positive deviation (= yd_max/yd_avg)
#define FSENSOR_OQ_MAX_ND      5    //maximum negative deviation (= yd_avg/yd_min)


//save restore printing
extern void fsensor_stop_and_save_print(void);
extern void fsensor_restore_print_and_continue(void);

//initialize
extern void fsensor_init(void);

//enable/disable
extern bool fsensor_enable(void);
extern void fsensor_disable(void);

//autoload feature enabled
extern bool fsensor_autoload_enabled;
extern void fsensor_autoload_set(bool State);

//update (perform M600 on filament runout)
extern void fsensor_update(void);

//setup pin-change interrupt
extern void fsensor_setup_interrupt(void);

//autoload support
extern void fsensor_autoload_check_start(void);
extern void fsensor_autoload_check_stop(void);
extern bool fsensor_check_autoload(void);

//optical quality meassurement support
extern void fsensor_oq_meassure_start(void);
extern void fsensor_oq_meassure_stop(void);
extern bool fsensor_oq_result(void);

//callbacks from stepper
extern void fsensor_st_block_begin(block_t* bl);
extern void fsensor_st_block_chunk(block_t* bl, int cnt);

//minimum meassured chunk length in steps
extern int16_t fsensor_chunk_len;
//enable/disable flag
extern bool fsensor_enabled;
//watch runout flag
extern bool fsensor_watch_runout;
//not responding flag
extern bool fsensor_not_responding;

//error counter
extern uint8_t fsensor_err_cnt;

//autoload enable/disable flag
extern bool fsensor_watch_autoload;


#endif //FSENSOR_H
