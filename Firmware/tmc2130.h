#ifndef TMC2130_H
#define TMC2130_H


//mode
extern uint8_t tmc2130_mode;
//holding and running currents
extern uint8_t tmc2130_current_h[4];
extern uint8_t tmc2130_current_r[4];
//microstep resolution (0 means 256usteps, 8 means 1ustep
extern uint8_t tmc2130_mres[4];

//flags for axis stall detection
extern uint8_t tmc2130_sg_thr[4];

extern bool tmc2130_sg_stop_on_crash;
extern uint8_t tmc2130_sg_crash; //crash mask

extern uint8_t tmc2130_sg_meassure;
extern uint32_t tmc2130_sg_meassure_cnt;
extern uint32_t tmc2130_sg_meassure_val;

extern uint8_t tmc2130_sg_homing_axes_mask;

#define TMC2130_MODE_NORMAL 0
#define TMC2130_MODE_SILENT 1

#define TMC2130_WAVE_FAC1000_MIN  30
#define TMC2130_WAVE_FAC1000_MAX 200
#define TMC2130_WAVE_FAC1000_STP   1

extern uint8_t tmc2130_home_enabled;
extern uint8_t tmc2130_home_origin[2];
extern uint8_t tmc2130_home_bsteps[2];
extern uint8_t tmc2130_home_fsteps[2];

extern uint8_t tmc2130_wave_fac[4];

#pragma pack(push)
#pragma pack(1)
typedef struct
{
	uint8_t toff:4;
	uint8_t hstr:3;
	uint8_t hend:4;
	uint8_t tbl:2;
	uint8_t res:3;
} tmc2130_chopper_config_t;
#pragma pack(pop)

extern tmc2130_chopper_config_t tmc2130_chopper_config[4];

//initialize tmc2130
#ifdef PSU_Delta
extern void tmc2130_init(bool bSupressFlag=false);
#else
extern void tmc2130_init();
#endif
//check diag pins (called from stepper isr)
extern void tmc2130_st_isr();
//update stall guard (called from st_synchronize inside the loop)
extern bool tmc2130_update_sg();
//temperature watching (called from )
extern void tmc2130_check_overtemp();
//enter homing (called from homeaxis before homing starts)
extern void tmc2130_home_enter(uint8_t axes_mask);
//exit homing (called from homeaxis after homing ends)
extern void tmc2130_home_exit();

//start stallguard meassuring for single axis
extern void tmc2130_sg_meassure_start(uint8_t axis);
//stop current stallguard meassuring and report result
extern uint16_t tmc2130_sg_meassure_stop();

extern void tmc2130_setup_chopper(uint8_t axis, uint8_t mres, uint8_t current_h, uint8_t current_r);

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


extern uint16_t tmc2130_rd_MSCNT(uint8_t axis);
extern uint32_t tmc2130_rd_MSCURACT(uint8_t axis);

extern uint8_t tmc2130_usteps2mres(uint16_t usteps);
#define tmc2130_mres2usteps(mres) ((uint16_t)256 >> mres)

extern bool tmc2130_wait_standstill_xy(int timeout);

extern uint16_t tmc2130_get_res(uint8_t axis);
extern void tmc2130_set_res(uint8_t axis, uint16_t res);
extern uint8_t tmc2130_get_pwr(uint8_t axis);
extern void tmc2130_set_pwr(uint8_t axis, uint8_t pwr);
extern uint8_t tmc2130_get_inv(uint8_t axis);
extern uint8_t tmc2130_get_dir(uint8_t axis);
extern void tmc2130_set_dir(uint8_t axis, uint8_t dir);
extern void tmc2130_do_step(uint8_t axis);
extern void tmc2130_do_steps(uint8_t axis, uint16_t steps, uint8_t dir, uint16_t delay_us);
extern void tmc2130_goto_step(uint8_t axis, uint8_t step, uint8_t dir, uint16_t delay_us, uint16_t microstep_resolution);
extern void tmc2130_get_wave(uint8_t axis, uint8_t* data, FILE* stream);
extern void tmc2130_set_wave(uint8_t axis, uint8_t amp, uint8_t fac1000);

extern bool tmc2130_home_calibrate(uint8_t axis);

extern uint8_t tmc2130_cur2val(float cur);
extern float tmc2130_val2cur(uint8_t val);

#endif //TMC2130_H
