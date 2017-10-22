#ifndef FSENSOR_H
#define FSENSOR_H


extern void fsensor_stop_and_save_print();
extern void fsensor_restore_print_and_continue();
extern void fsensor_enable();
extern void fsensor_disable();
extern void fsensor_setup_interrupt();
extern void fsensor_update();


extern bool fsensor_M600;
extern int16_t fsensor_steps_e;
extern int16_t fsensor_y_old;



#endif //FSENSOR_H
