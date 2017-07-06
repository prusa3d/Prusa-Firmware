#ifndef TMC2130_H
#define TMC2130_H

//mode
extern uint8_t tmc2130_mode;
//holding and running currents
extern uint8_t tmc2130_current_h[4];
extern uint8_t tmc2130_current_r[4];
//flags for axis stall detection
extern uint8_t tmc2130_axis_stalled[2];

extern uint8_t sg_thrs_x;
extern uint8_t sg_thrs_y;

extern uint8_t sg_homing_delay;

#define TMC2130_MODE_NORMAL 0
#define TMC2130_MODE_SILENT 1

//initialize tmc2130
extern void tmc2130_init();
//update stall guard (called from st_synchronize inside the loop)
extern bool tmc2130_update_sg();
//temperature watching (called from )
extern void tmc2130_check_overtemp();
//enter homing (called from homeaxis before homing starts)
extern void tmc2130_home_enter(uint8_t axis);
//exit homing (called from homeaxis after homing ends)
extern void tmc2130_home_exit();
//
extern uint8_t tmc2130_didLastHomingStall();

//set holding current for any axis (M911)
extern void tmc2130_set_current_h(uint8_t axis, uint8_t current);
//set running current for any axis (M912)
extern void tmc2130_set_current_r(uint8_t axis, uint8_t current);
//print currents (M913)
extern void tmc2130_print_currents();

//set PWM_AMPL for any axis (M917)
extern void tmc2130_set_pwm_ampl(uint8_t axis, uint8_t pwm_ampl);
//set PWM_GRAD for any axis (M918)
extern void tmc2130_set_pwm_grad(uint8_t axis, uint8_t pwm_ampl);


#endif //TMC2130_H