#ifndef TMC2130_H
#define TMC2130_H

#include "Configuration_prusa.h"

/*------------------------------------
 TMC2130 default settings
 *------------------------------------*/

#define TMC2130_FCLK 12000000       // fclk = 12MHz

#ifdef EINSY_HIGH_SAMPLE_RATE
	#define TMC2130_USTEPS_XY   32        // microstep resolution for XY axes
	#define TMC2130_USTEPS_Z    32        // microstep resolution for Z axis
	#define TMC2130_USTEPS_E    64        // microstep resolution for E axis
#else
	#define TMC2130_USTEPS_XY   16        // microstep resolution for XY axes
	#define TMC2130_USTEPS_Z    16        // microstep resolution for Z axis
	#define TMC2130_USTEPS_E    16        // microstep resolution for E axis
#endif

#define TMC2130_PWM_GRAD_X  2         // PWMCONF
#define TMC2130_PWM_AMPL_X  230       // PWMCONF
#define TMC2130_PWM_AUTO_X  1         // PWMCONF
#define TMC2130_PWM_FREQ_X  2         // PWMCONF

#define TMC2130_PWM_GRAD_Y  2         // PWMCONF
#define TMC2130_PWM_AMPL_Y  235       // PWMCONF
#define TMC2130_PWM_AUTO_Y  1         // PWMCONF
#define TMC2130_PWM_FREQ_Y  2         // PWMCONF

#define TMC2130_PWM_GRAD_E  2         // PWMCONF
#define TMC2130_PWM_AMPL_E  235       // PWMCONF
#define TMC2130_PWM_AUTO_E  1         // PWMCONF
#define TMC2130_PWM_FREQ_E  2         // PWMCONF

#define TMC2130_PWM_GRAD_Z  4         // PWMCONF
#define TMC2130_PWM_AMPL_Z  200       // PWMCONF
#define TMC2130_PWM_AUTO_Z  1         // PWMCONF
#define TMC2130_PWM_FREQ_Z  2         // PWMCONF

#define TMC2130_PWM_GRAD_E  4         // PWMCONF
#define TMC2130_PWM_AMPL_E  240       // PWMCONF
#define TMC2130_PWM_AUTO_E  1         // PWMCONF
#define TMC2130_PWM_FREQ_E  2         // PWMCONF

#define TMC2130_TOFF_XYZ    3         // CHOPCONF // fchop = 27.778kHz
#define TMC2130_TOFF_E      3         // CHOPCONF // fchop = 27.778kHz
//#define TMC2130_TOFF_E      4         // CHOPCONF // fchop = 21.429kHz
//#define TMC2130_TOFF_E      5         // CHOPCONF // fchop = 17.442kHz

//#define TMC2130_STEALTH_E // Extruder stealthChop mode
//#define TMC2130_CNSTOFF_E // Extruder constant-off-time mode (similar to MK2)

//#define TMC2130_PWM_DIV   683         // PWM frequency divider (1024, 683, 512, 410)
#define TMC2130_PWM_DIV   512         // PWM frequency divider (1024, 683, 512, 410)
#define TMC2130_PWM_CLK   (2 * TMC2130_FCLK / TMC2130_PWM_DIV) // PWM frequency (23.4kHz, 35.1kHz, 46.9kHz, 58.5kHz for 12MHz fclk)

#define TMC2130_TPWMTHRS  0         // TPWMTHRS - Sets the switching speed threshold based on TSTEP from stealthChop to spreadCycle mode
#define TMC2130_THIGH     0         // THIGH - unused

//#define TMC2130_TCOOLTHRS_X 450       // TCOOLTHRS - coolstep treshold
//#define TMC2130_TCOOLTHRS_Y 450       // TCOOLTHRS - coolstep treshold
#define TMC2130_TCOOLTHRS_X 430       // TCOOLTHRS - coolstep treshold
#define TMC2130_TCOOLTHRS_Y 430       // TCOOLTHRS - coolstep treshold
#define TMC2130_TCOOLTHRS_Z 500       // TCOOLTHRS - coolstep treshold
#define TMC2130_TCOOLTHRS_E 500       // TCOOLTHRS - coolstep treshold

#define TMC2130_SG_HOMING       1     // stallguard homing
#define TMC2130_SG_THRS_X       3     // stallguard sensitivity for X axis
#define TMC2130_SG_THRS_Y       3     // stallguard sensitivity for Y axis
#define TMC2130_SG_THRS_Z       3     // stallguard sensitivity for Z axis
#define TMC2130_SG_THRS_E       3     // stallguard sensitivity for E axis

//new settings is possible for vsense = 1, running current value > 31 set vsense to zero and shift both currents by 1 bit right (Z axis only)
//#define TMC2130_CURRENTS {25, 25, 31, 25}  // default running currents for all axes
#define TMC2130_CURRENTS {25, 25, 39, 26}  // default running currents for all axes

//mode
extern uint8_t tmc2130_mode;
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


//initialize tmc2130
extern void tmc2130_init();
//check diag pins (called from stepper isr)
extern void tmc2130_st_isr(uint8_t last_step_mask);
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

#pragma pack(push)
#pragma pack(1)
struct
{
	uint8_t mres:5;             // mres       - byte 0, bit 0..4     microstep resolution
	uint8_t reserved_0_0:2;     // reserved   - byte 0, bit 5..6
	uint8_t intpol:1;           // intpol     - byte 0, bit 7        linear interpolation to 255 usteps
	uint8_t pwm_ampl:8;         // pwm_ampl   - byte 1, bit 0..7     pwm amplitude for silent mode
	uint8_t pwm_grad:4;         // pwm_grad   - byte 2, bit 0..3     pwm gradient for silent mode
	uint8_t pwm_freq:2;         // pwm_freq   - byte 2, bit 4..5     pwm frequency for silent mode
	uint8_t reserved_2_0:2;     // reserved   - byte 2, bit 6..7
	uint16_t tcoolthrs:16;      // tcoolthrs  - byte 3..4            coolstep threshold / middle sensitivity
	int8_t  sg_thrs:8;          // sg_thrs    - byte 5, bit 0..7     stallguard sensitivity in high power / middle sensitivity
	int8_t  current_h:6;        // current_h  - byte 6, bit 0..5     holding current for high power mode
	uint8_t reserved_6_0:2;     // reserved   - byte 6, bit 6..7
	int8_t  current_r:6;        // current_r  - byte 7, bit 0..5     running current for high power mode
	uint8_t reserved_7_0:2;     // reserved   - byte 7, bit 6..7
	int8_t  home_sg_thrs:8;     // sg_thrs    - byte 8, bit 0..7     stallguard sensitivity for homing
	int8_t  home_current:6;     // current_r  - byte 9, bit 0..5     running current for homing
	uint8_t reserved_9_0:2;     // reserved   - byte 9, bit 6..7
	int8_t  home_dtcoolthrs:8;  // dtcoolthrs - byte 10, bit 0..7    delta tcoolthrs for homing
	int8_t  dtcoolthrs_low:8;   // dtcoolthrs - byte 11, bit 0..7    delta tcoolthrs for low sensitivity (based on value for middle sensitivity)
	int8_t  dtcoolthrs_high:8;  // dtcoolthrs - byte 12, bit 0..7    delta tcoolthrs for high sensitivity (based on value for middle sensitivity)
	int8_t  sg_thrs_low:8;      // sg_thrs    - byte 13, bit 0..7    stallguard sensitivity in high power / low sensitivity
	int8_t  sg_thrs_high:8;     // sg_thrs    - byte 14, bit 0..7    stallguard sensitivity in high power / high sensitivity
} tmc2130_axis_config;
#pragma pack(pop)

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

#endif //TMC2130_H
