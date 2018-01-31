#include "Marlin.h"

#ifdef TMC2130

#include "tmc2130.h"
#include <SPI.h>
#include "LiquidCrystal.h"
#include "ultralcd.h"

extern LiquidCrystal lcd;

#define TMC2130_GCONF_NORMAL 0x00000000 // spreadCycle
#define TMC2130_GCONF_SGSENS 0x00003180 // spreadCycle with stallguard (stall activates DIAG0 and DIAG1 [pushpull])
#define TMC2130_GCONF_SILENT 0x00000004 // stealthChop

//externals for debuging
extern float current_position[4];
extern void st_get_position_xy(long &x, long &y);
extern long st_get_position(uint8_t axis);
extern void crashdet_stop_and_save_print();
extern void crashdet_stop_and_save_print2();

//chipselect pins
uint8_t tmc2130_cs[4] = { X_TMC2130_CS, Y_TMC2130_CS, Z_TMC2130_CS, E0_TMC2130_CS };
//mode
uint8_t tmc2130_mode = TMC2130_MODE_NORMAL;
//holding currents
uint8_t tmc2130_current_h[4] = TMC2130_CURRENTS_H;
//running currents
uint8_t tmc2130_current_r[4] = TMC2130_CURRENTS_R;

//running currents for homing
uint8_t tmc2130_current_r_home[4] = {10, 10, 20, 10};


//pwm_ampl
uint8_t tmc2130_pwm_ampl[4] = {TMC2130_PWM_AMPL_X, TMC2130_PWM_AMPL_Y, TMC2130_PWM_AMPL_Z, TMC2130_PWM_AMPL_E};
//pwm_grad
uint8_t tmc2130_pwm_grad[4] = {TMC2130_PWM_GRAD_X, TMC2130_PWM_GRAD_Y, TMC2130_PWM_GRAD_Z, TMC2130_PWM_GRAD_E};
//pwm_auto
uint8_t tmc2130_pwm_auto[4] = {TMC2130_PWM_AUTO_X, TMC2130_PWM_AUTO_Y, TMC2130_PWM_AUTO_Z, TMC2130_PWM_AUTO_E};
//pwm_freq
uint8_t tmc2130_pwm_freq[4] = {TMC2130_PWM_FREQ_X, TMC2130_PWM_FREQ_Y, TMC2130_PWM_FREQ_Z, TMC2130_PWM_FREQ_E};

uint8_t tmc2130_mres[4] = {0, 0, 0, 0}; //will be filed at begin of init


uint8_t tmc2130_sg_thr[4] = {TMC2130_SG_THRS_X, TMC2130_SG_THRS_Y, TMC2130_SG_THRS_Z, TMC2130_SG_THRS_E};
uint8_t tmc2130_sg_thr_home[4] = {3, 3, TMC2130_SG_THRS_Z, TMC2130_SG_THRS_E};


uint8_t sg_homing_axes_mask = 0x00;

uint8_t tmc2130_sg_meassure = 0xff;
uint16_t tmc2130_sg_meassure_cnt = 0;
uint32_t tmc2130_sg_meassure_val = 0;


bool tmc2130_sg_stop_on_crash = true;
uint8_t tmc2130_sg_diag_mask = 0x00;
uint8_t tmc2130_sg_crash = 0;
uint16_t tmc2130_sg_err[4] = {0, 0, 0, 0};
uint16_t tmc2130_sg_cnt[4] = {0, 0, 0, 0};
bool tmc2130_sg_change = false;


bool skip_debug_msg = false;

#define DBG(args...) printf_P(args)
#define _n PSTR
#define _i PSTR

//TMC2130 registers
#define TMC2130_REG_GCONF      0x00 // 17 bits
#define TMC2130_REG_GSTAT      0x01 // 3 bits
#define TMC2130_REG_IOIN       0x04 // 8+8 bits
#define TMC2130_REG_IHOLD_IRUN 0x10 // 5+5+4 bits
#define TMC2130_REG_TPOWERDOWN 0x11 // 8 bits
#define TMC2130_REG_TSTEP      0x12 // 20 bits
#define TMC2130_REG_TPWMTHRS   0x13 // 20 bits
#define TMC2130_REG_TCOOLTHRS  0x14 // 20 bits
#define TMC2130_REG_THIGH      0x15 // 20 bits
#define TMC2130_REG_XDIRECT    0x2d // 32 bits
#define TMC2130_REG_VDCMIN     0x33 // 23 bits
#define TMC2130_REG_MSLUT0     0x60 // 32 bits
#define TMC2130_REG_MSLUT1     0x61 // 32 bits
#define TMC2130_REG_MSLUT2     0x62 // 32 bits
#define TMC2130_REG_MSLUT3     0x63 // 32 bits
#define TMC2130_REG_MSLUT4     0x64 // 32 bits
#define TMC2130_REG_MSLUT5     0x65 // 32 bits
#define TMC2130_REG_MSLUT6     0x66 // 32 bits
#define TMC2130_REG_MSLUT7     0x67 // 32 bits
#define TMC2130_REG_MSLUTSEL   0x68 // 32 bits
#define TMC2130_REG_MSLUTSTART 0x69 // 8+8 bits
#define TMC2130_REG_MSCNT      0x6a // 10 bits
#define TMC2130_REG_MSCURACT   0x6b // 9+9 bits
#define TMC2130_REG_CHOPCONF   0x6c // 32 bits
#define TMC2130_REG_COOLCONF   0x6d // 25 bits
#define TMC2130_REG_DCCTRL     0x6e // 24 bits
#define TMC2130_REG_DRV_STATUS 0x6f // 32 bits
#define TMC2130_REG_PWMCONF    0x70 // 22 bits
#define TMC2130_REG_PWM_SCALE  0x71 // 8 bits
#define TMC2130_REG_ENCM_CTRL  0x72 // 2 bits
#define TMC2130_REG_LOST_STEPS 0x73 // 20 bits


uint16_t tmc2130_rd_TSTEP(uint8_t cs);
uint16_t tmc2130_rd_MSCNT(uint8_t cs);
uint16_t tmc2130_rd_DRV_STATUS(uint8_t cs);

void tmc2130_wr_CHOPCONF(uint8_t cs, uint8_t toff = 3, uint8_t hstrt = 4, uint8_t hend = 1, uint8_t fd3 = 0, uint8_t disfdcc = 0, uint8_t rndtf = 0, uint8_t chm = 0, uint8_t tbl = 2, uint8_t vsense = 0, uint8_t vhighfs = 0, uint8_t vhighchm = 0, uint8_t sync = 0, uint8_t mres = 0b0100, uint8_t intpol = 1, uint8_t dedge = 0, uint8_t diss2g = 0);
void tmc2130_wr_PWMCONF(uint8_t cs, uint8_t pwm_ampl, uint8_t pwm_grad, uint8_t pwm_freq, uint8_t pwm_auto, uint8_t pwm_symm, uint8_t freewheel);
void tmc2130_wr_TPWMTHRS(uint8_t cs, uint32_t val32);
void tmc2130_wr_THIGH(uint8_t cs, uint32_t val32);

uint8_t tmc2130_axis_by_cs(uint8_t cs);
uint8_t tmc2130_calc_mres(uint16_t microstep_resolution);

uint8_t tmc2130_wr(uint8_t cs, uint8_t addr, uint32_t wval);
uint8_t tmc2130_rd(uint8_t cs, uint8_t addr, uint32_t* rval);
uint8_t tmc2130_txrx(uint8_t cs, uint8_t addr, uint32_t wval, uint32_t* rval);


void tmc2130_setup_chopper(uint8_t axis, uint8_t mres, uint8_t current_h, uint8_t current_r);



void tmc2130_init()
{
	DBG(_n("tmc2130_init(), mode=%S\n"), tmc2130_mode?_n("STEALTH"):_n("NORMAL"));
	tmc2130_mres[0] = tmc2130_calc_mres(TMC2130_USTEPS_XY);
	tmc2130_mres[1] = tmc2130_calc_mres(TMC2130_USTEPS_XY);
	tmc2130_mres[2] = tmc2130_calc_mres(TMC2130_USTEPS_Z);
	tmc2130_mres[3] = tmc2130_calc_mres(TMC2130_USTEPS_E);
	WRITE(X_TMC2130_CS, HIGH);
	WRITE(Y_TMC2130_CS, HIGH);
	WRITE(Z_TMC2130_CS, HIGH);
	WRITE(E0_TMC2130_CS, HIGH);
	SET_OUTPUT(X_TMC2130_CS);
	SET_OUTPUT(Y_TMC2130_CS);
	SET_OUTPUT(Z_TMC2130_CS);
	SET_OUTPUT(E0_TMC2130_CS);
	SET_INPUT(X_TMC2130_DIAG);
	SET_INPUT(Y_TMC2130_DIAG);
	SET_INPUT(Z_TMC2130_DIAG);
	SET_INPUT(E0_TMC2130_DIAG);
	SPI.begin();
	for (int axis = 0; axis < 2; axis++) // X Y axes
	{
/*		if (tmc2130_current_r[axis] <= 31)
		{
			tmc2130_wr_CHOPCONF(tmc2130_cs[axis], 3, 5, 1, 0, 0, 0, 0, 2, 1, 0, 0, 0, mres, TMC2130_INTPOL_XY, 0, 0);
			tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_IHOLD_IRUN, 0x000f0000 | ((tmc2130_current_r[axis] & 0x1f) << 8) | (tmc2130_current_h[axis] & 0x1f));
		}
		else
		{
			tmc2130_wr_CHOPCONF(tmc2130_cs[axis], 3, 5, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, mres, TMC2130_INTPOL_XY, 0, 0);
			tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_IHOLD_IRUN, 0x000f0000 | (((tmc2130_current_r[axis] >> 1) & 0x1f) << 8) | ((tmc2130_current_h[axis] >> 1) & 0x1f));
		}*/
		tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);

//		tmc2130_wr_CHOPCONF(tmc2130_cs[axis], 3, 5, 1, 0, 0, 0, 0, 2, 1, 0, 0, 0, mres, TMC2130_INTPOL_XY, 0, 0);
//		tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_IHOLD_IRUN, 0x000f0000 | ((tmc2130_current_r[axis] & 0x1f) << 8) | (tmc2130_current_h[axis] & 0x1f));
		tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_TPOWERDOWN, 0x00000000);
//		tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[axis]) << 16) | ((uint32_t)1 << 24));
		tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[axis]) << 16));
		tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_TCOOLTHRS, (tmc2130_mode == TMC2130_MODE_SILENT)?0:((axis==X_AXIS)?TMC2130_TCOOLTHRS_X:TMC2130_TCOOLTHRS_Y));
		tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_GCONF, (tmc2130_mode == TMC2130_MODE_SILENT)?TMC2130_GCONF_SILENT:TMC2130_GCONF_SGSENS);
		tmc2130_wr_PWMCONF(tmc2130_cs[axis], tmc2130_pwm_ampl[axis], tmc2130_pwm_grad[axis], tmc2130_pwm_freq[axis], tmc2130_pwm_auto[axis], 0, 0);
		tmc2130_wr_TPWMTHRS(tmc2130_cs[axis], TMC2130_TPWMTHRS);
		//tmc2130_wr_THIGH(tmc2130_cs[axis], TMC2130_THIGH);
	}
	for (int axis = 2; axis < 3; axis++) // Z axis
	{
//		uint8_t mres = tmc2130_mres(TMC2130_USTEPS_Z);
/*		if (tmc2130_current_r[axis] <= 31)
		{
			tmc2130_wr_CHOPCONF(tmc2130_cs[axis], 3, 5, 1, 0, 0, 0, 0, 2, 1, 0, 0, 0, mres, TMC2130_INTPOL_Z, 0, 0);
			tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_IHOLD_IRUN, 0x000f0000 | ((tmc2130_current_r[axis] & 0x1f) << 8) | (tmc2130_current_h[axis] & 0x1f));
		}
		else
		{
			tmc2130_wr_CHOPCONF(tmc2130_cs[axis], 3, 5, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, mres, TMC2130_INTPOL_Z, 0, 0);
			tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_IHOLD_IRUN, 0x000f0000 | (((tmc2130_current_r[axis] >> 1) & 0x1f) << 8) | ((tmc2130_current_h[axis] >> 1) & 0x1f));
		}*/
		tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);

		tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_TPOWERDOWN, 0x00000000);
		tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_GCONF, TMC2130_GCONF_SGSENS);

	}
	for (int axis = 3; axis < 4; axis++) // E axis
	{
//		uint8_t mres = tmc2130_mres(TMC2130_USTEPS_E);
		tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);

//		tmc2130_wr_CHOPCONF(tmc2130_cs[axis], 3, 5, 1, 0, 0, 0, 0, 2, 1, 0, 0, 0, mres, TMC2130_INTPOL_E, 0, 0);
//		tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_IHOLD_IRUN, 0x000f0000 | ((tmc2130_current_r[axis] & 0x1f) << 8) | (tmc2130_current_h[axis] & 0x1f));

		tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_TPOWERDOWN, 0x00000000);
#ifndef TMC2130_STEALTH_E
		tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_GCONF, TMC2130_GCONF_SGSENS);
#else //TMC2130_STEALTH_E
		tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[axis]) << 16));
		tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_TCOOLTHRS, 0);
		tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_GCONF, TMC2130_GCONF_SILENT);
		tmc2130_wr_PWMCONF(tmc2130_cs[axis], tmc2130_pwm_ampl[axis], tmc2130_pwm_grad[axis], tmc2130_pwm_freq[axis], tmc2130_pwm_auto[axis], 0, 0);
		tmc2130_wr_TPWMTHRS(tmc2130_cs[axis], TMC2130_TPWMTHRS);
#endif //TMC2130_STEALTH_E
	}

	tmc2130_sg_err[0] = 0;
	tmc2130_sg_err[1] = 0;
	tmc2130_sg_err[2] = 0;
	tmc2130_sg_err[3] = 0;
	tmc2130_sg_cnt[0] = 0;
	tmc2130_sg_cnt[1] = 0;
	tmc2130_sg_cnt[2] = 0;
	tmc2130_sg_cnt[3] = 0;
}

uint8_t tmc2130_sample_diag()
{
	uint8_t mask = 0;
	if (READ(X_TMC2130_DIAG)) mask |= X_AXIS_MASK;
	if (READ(Y_TMC2130_DIAG)) mask |= Y_AXIS_MASK;
//	if (READ(Z_TMC2130_DIAG)) mask |= Z_AXIS_MASK;
//	if (READ(E0_TMC2130_DIAG)) mask |= E_AXIS_MASK;
	return mask;
}

extern bool is_usb_printing;

void tmc2130_st_isr(uint8_t last_step_mask)
{
	if (tmc2130_mode == TMC2130_MODE_SILENT || tmc2130_sg_stop_on_crash == false) return;
	uint8_t crash = 0;
	uint8_t diag_mask = tmc2130_sample_diag();
//	for (uint8_t axis = X_AXIS; axis <= E_AXIS; axis++)
	for (uint8_t axis = X_AXIS; axis <= Z_AXIS; axis++)
	{
		uint8_t mask = (X_AXIS_MASK << axis);
		if (diag_mask & mask) tmc2130_sg_err[axis]++;
		else
			if (tmc2130_sg_err[axis] > 0) tmc2130_sg_err[axis]--;
		if (tmc2130_sg_cnt[axis] < tmc2130_sg_err[axis])
		{
			tmc2130_sg_cnt[axis] = tmc2130_sg_err[axis];
			tmc2130_sg_change = true;
			uint8_t sg_thr = 64;
//			if (axis == Y_AXIS) sg_thr = 64;
			if (tmc2130_sg_err[axis] >= sg_thr)
			{
				tmc2130_sg_err[axis] = 0;
				crash |= mask;
			}
		}
	}
	if (sg_homing_axes_mask == 0)
	{
/*		if (crash)
		{
			if (diag_mask & 0x01) tmc2130_sg_cnt[0]++;
			if (diag_mask & 0x02) tmc2130_sg_cnt[1]++;
			if (diag_mask & 0x04) tmc2130_sg_cnt[2]++;
			if (diag_mask & 0x08) tmc2130_sg_cnt[3]++;
		}*/
		if (/*!is_usb_printing && */tmc2130_sg_stop_on_crash && crash)
		{
			tmc2130_sg_crash = crash;
			tmc2130_sg_stop_on_crash = false;
			crashdet_stop_and_save_print();
		}
	}
}


bool tmc2130_update_sg()
{
	if (tmc2130_sg_meassure <= E_AXIS)
	{
		uint8_t cs = tmc2130_cs[tmc2130_sg_meassure];
		uint16_t sg = tmc2130_rd_DRV_STATUS(cs) & 0x3ff;
		tmc2130_sg_meassure_val += sg;
		tmc2130_sg_meassure_cnt++;
//		printf_P(PSTR("tmc2130_update_sg - meassure - sg=%d\n"), sg);
		return true;
	}
	return false;
}

void tmc2130_home_enter(uint8_t axes_mask)
{
//	printf_P(PSTR("tmc2130_home_enter(axes_mask=0x%02x)\n"), axes_mask);
#ifdef TMC2130_SG_HOMING
	for (uint8_t axis = X_AXIS; axis <= Z_AXIS; axis++) //X Y and Z axes
	{
		uint8_t mask = (X_AXIS_MASK << axis);
		uint8_t cs = tmc2130_cs[axis];
		if (axes_mask & mask)
		{
			sg_homing_axes_mask |= mask;
			//Configuration to spreadCycle
			tmc2130_wr(cs, TMC2130_REG_GCONF, TMC2130_GCONF_NORMAL);
			tmc2130_wr(cs, TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr_home[axis]) << 16));
//			tmc2130_wr(cs, TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[axis]) << 16) | ((uint32_t)1 << 24));
			tmc2130_wr(cs, TMC2130_REG_TCOOLTHRS, (axis==X_AXIS)?TMC2130_TCOOLTHRS_X:TMC2130_TCOOLTHRS_Y);
			tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r_home[axis]);
			if (mask & (X_AXIS_MASK | Y_AXIS_MASK | Z_AXIS_MASK))
				tmc2130_wr(cs, TMC2130_REG_GCONF, TMC2130_GCONF_SGSENS); //stallguard output DIAG1, DIAG1 = pushpull
		}
	}
#endif //TMC2130_SG_HOMING
}

void tmc2130_home_exit()
{
//	printf_P(PSTR("tmc2130_home_exit sg_homing_axes_mask=0x%02x\n"), sg_homing_axes_mask);
#ifdef TMC2130_SG_HOMING
	if (sg_homing_axes_mask)
	{
		for (uint8_t axis = X_AXIS; axis <= Z_AXIS; axis++) //X Y and Z axes
		{
			uint8_t mask = (X_AXIS_MASK << axis);
			if (sg_homing_axes_mask & mask & (X_AXIS_MASK | Y_AXIS_MASK))
			{
				if (tmc2130_mode == TMC2130_MODE_SILENT)
				{
					tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_GCONF, TMC2130_GCONF_SILENT); // Configuration back to stealthChop
					tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_TCOOLTHRS, 0);
//					tmc2130_wr_PWMCONF(tmc2130_cs[i], tmc2130_pwm_ampl[i], tmc2130_pwm_grad[i], tmc2130_pwm_freq[i], tmc2130_pwm_auto[i], 0, 0);
				}
				else
				{
//					tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_GCONF, TMC2130_GCONF_NORMAL);
					tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);
//					tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[axis]) << 16) | ((uint32_t)1 << 24));
					tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[axis]) << 16));
					tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_TCOOLTHRS, (tmc2130_mode == TMC2130_MODE_SILENT)?0:((axis==X_AXIS)?TMC2130_TCOOLTHRS_X:TMC2130_TCOOLTHRS_Y));
					tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_GCONF, TMC2130_GCONF_SGSENS);
				}
			}
		}
		sg_homing_axes_mask = 0x00;
	}
	tmc2130_sg_crash = false;
#endif
}

void tmc2130_sg_meassure_start(uint8_t axis)
{
	tmc2130_sg_meassure = axis;
	tmc2130_sg_meassure_cnt = 0;
	tmc2130_sg_meassure_val = 0;
}

uint16_t tmc2130_sg_meassure_stop()
{
	tmc2130_sg_meassure = 0xff;
	return tmc2130_sg_meassure_val / tmc2130_sg_meassure_cnt;
}


bool tmc2130_wait_standstill_xy(int timeout)
{
//	DBG(_n("tmc2130_wait_standstill_xy(timeout=%d)\n"), timeout);
	bool standstill = false;
	while (!standstill && (timeout > 0))
	{
		uint32_t drv_status_x = 0;
		uint32_t drv_status_y = 0;
		tmc2130_rd(tmc2130_cs[X_AXIS], TMC2130_REG_DRV_STATUS, &drv_status_x);
		tmc2130_rd(tmc2130_cs[Y_AXIS], TMC2130_REG_DRV_STATUS, &drv_status_y);
//		DBG(_n("\tdrv_status_x=0x%08x drv_status_x=0x%08x\n"), drv_status_x, drv_status_y);
		standstill = (drv_status_x & 0x80000000) && (drv_status_y & 0x80000000);
		tmc2130_check_overtemp();
		timeout--;
	}
	return standstill;
}


void tmc2130_check_overtemp()
{
	const static char TMC_OVERTEMP_MSG[] PROGMEM = "TMC DRIVER OVERTEMP ";
	static uint32_t checktime = 0;
	if (millis() - checktime > 1000 )
	{
		for (int i = 0; i < 4; i++)
		{
			uint32_t drv_status = 0;
			skip_debug_msg = true;
			tmc2130_rd(tmc2130_cs[i], TMC2130_REG_DRV_STATUS, &drv_status);
			if (drv_status & ((uint32_t)1 << 26))
			{ // BIT 26 - over temp prewarning ~120C (+-20C)
				SERIAL_ERRORRPGM(TMC_OVERTEMP_MSG);
				SERIAL_ECHOLN(i);
				for (int j = 0; j < 4; j++)
					tmc2130_wr(tmc2130_cs[j], TMC2130_REG_CHOPCONF, 0x00010000);
				kill(TMC_OVERTEMP_MSG);
			}

		}
		checktime = millis();
		tmc2130_sg_change = true;
	}
#ifdef DEBUG_CRASHDET_COUNTERS
	if (tmc2130_sg_change)
	{
		for (int i = 0; i < 4; i++)
		{
			tmc2130_sg_change = false;
			lcd.setCursor(0 + i*4, 3);
			lcd.print(itostr3(tmc2130_sg_cnt[i]));
			lcd.print(' ');
		}
	}
#endif DEBUG_CRASHDET_COUNTERS
}

void tmc2130_setup_chopper(uint8_t axis, uint8_t mres, uint8_t current_h, uint8_t current_r)
{
	uint8_t cs = tmc2130_cs[axis];
	uint8_t intpol = 1;
	uint8_t toff = TMC2130_TOFF_XYZ; // toff = 3 (fchop = 27.778kHz)
	uint8_t hstrt = 5; //initial 4, modified to 5
	uint8_t hend = 1;
	uint8_t fd3 = 0;
	uint8_t rndtf = 0; //random off time
	uint8_t chm = 0; //spreadCycle
	uint8_t tbl = 2; //blanking time
	if (axis == E_AXIS)
	{
#ifdef TMC2130_CNSTOFF_E
		// fd = 0 (slow decay only)
		hstrt = 0; //fd0..2
		fd3 = 0; //fd3
		hend = 0; //sine wave offset
		chm = 1; // constant off time mod
#endif //TMC2130_CNSTOFF_E
		toff = TMC2130_TOFF_E; // toff = 3-5
//		rndtf = 1;
	}
	if (current_r <= 31)
	{
		tmc2130_wr_CHOPCONF(cs, toff, hstrt, hend, fd3, 0, rndtf, chm, tbl, 1, 0, 0, 0, mres, intpol, 0, 0);
		tmc2130_wr(cs, TMC2130_REG_IHOLD_IRUN, 0x000f0000 | ((current_r & 0x1f) << 8) | (current_h & 0x1f));
	}
	else
	{
		tmc2130_wr_CHOPCONF(cs, toff, hstrt, hend, fd3, 0, 0, 0, tbl, 0, 0, 0, 0, mres, intpol, 0, 0);
		tmc2130_wr(cs, TMC2130_REG_IHOLD_IRUN, 0x000f0000 | (((current_r >> 1) & 0x1f) << 8) | ((current_h >> 1) & 0x1f));
	}
}

void tmc2130_set_current_h(uint8_t axis, uint8_t current)
{
	DBG(_n("tmc2130_set_current_h(axis=%d, current=%d\n"), axis, current);
	tmc2130_current_h[axis] = current;
	tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);
}

void tmc2130_set_current_r(uint8_t axis, uint8_t current)
{
	DBG(_n("tmc2130_set_current_r(axis=%d, current=%d\n"), axis, current);
	tmc2130_current_r[axis] = current;
	tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);
}

void tmc2130_print_currents()
{
	DBG(_n("tmc2130_print_currents()\n\tH\tR\nX\t%d\t%d\nY\t%d\t%d\nZ\t%d\t%d\nE\t%d\t%d\n"),
		tmc2130_current_h[0], tmc2130_current_r[0],
		tmc2130_current_h[1], tmc2130_current_r[1],
		tmc2130_current_h[2], tmc2130_current_r[2],
		tmc2130_current_h[3], tmc2130_current_r[3]
	);
}

void tmc2130_set_pwm_ampl(uint8_t axis, uint8_t pwm_ampl)
{
	MYSERIAL.print("tmc2130_set_pwm_ampl ");
	MYSERIAL.print((int)axis);
	MYSERIAL.print(" ");
	MYSERIAL.println((int)pwm_ampl);
	tmc2130_pwm_ampl[axis] = pwm_ampl;
	if (((axis == 0) || (axis == 1)) && (tmc2130_mode == TMC2130_MODE_SILENT))
		tmc2130_wr_PWMCONF(tmc2130_cs[axis], tmc2130_pwm_ampl[axis], tmc2130_pwm_grad[axis], tmc2130_pwm_freq[axis], tmc2130_pwm_auto[axis], 0, 0);
}

void tmc2130_set_pwm_grad(uint8_t axis, uint8_t pwm_grad)
{
	MYSERIAL.print("tmc2130_set_pwm_grad ");
	MYSERIAL.print((int)axis);
	MYSERIAL.print(" ");
	MYSERIAL.println((int)pwm_grad);
	tmc2130_pwm_grad[axis] = pwm_grad;
	if (((axis == 0) || (axis == 1)) && (tmc2130_mode == TMC2130_MODE_SILENT))
		tmc2130_wr_PWMCONF(tmc2130_cs[axis], tmc2130_pwm_ampl[axis], tmc2130_pwm_grad[axis], tmc2130_pwm_freq[axis], tmc2130_pwm_auto[axis], 0, 0);
}

uint16_t tmc2130_rd_TSTEP(uint8_t cs)
{
	uint32_t val32 = 0;
	tmc2130_rd(cs, TMC2130_REG_TSTEP, &val32);
	if (val32 & 0x000f0000) return 0xffff;
	return val32 & 0xffff;
}

uint16_t tmc2130_rd_MSCNT(uint8_t cs)
{
	uint32_t val32 = 0;
	tmc2130_rd(cs, TMC2130_REG_MSCNT, &val32);
	return val32 & 0x3ff;
}

uint16_t tmc2130_rd_DRV_STATUS(uint8_t cs)
{
	uint32_t val32 = 0;
	tmc2130_rd(cs, TMC2130_REG_DRV_STATUS, &val32);
	return val32;
}

void tmc2130_wr_CHOPCONF(uint8_t cs, uint8_t toff, uint8_t hstrt, uint8_t hend, uint8_t fd3, uint8_t disfdcc, uint8_t rndtf, uint8_t chm, uint8_t tbl, uint8_t vsense, uint8_t vhighfs, uint8_t vhighchm, uint8_t sync, uint8_t mres, uint8_t intpol, uint8_t dedge, uint8_t diss2g)
{
	uint32_t val = 0;
	val |= (uint32_t)(toff & 15);
	val |= (uint32_t)(hstrt & 7) << 4;
	val |= (uint32_t)(hend & 15) << 7;
	val |= (uint32_t)(fd3 & 1) << 11;
	val |= (uint32_t)(disfdcc & 1) << 12;
	val |= (uint32_t)(rndtf & 1) << 13;
	val |= (uint32_t)(chm & 1) << 14;
	val |= (uint32_t)(tbl & 3) << 15;
	val |= (uint32_t)(vsense & 1) << 17;
	val |= (uint32_t)(vhighfs & 1) << 18;
	val |= (uint32_t)(vhighchm & 1) << 19;
	val |= (uint32_t)(sync & 15) << 20;
	val |= (uint32_t)(mres & 15) << 24;
	val |= (uint32_t)(intpol & 1) << 28;
	val |= (uint32_t)(dedge & 1) << 29;
	val |= (uint32_t)(diss2g & 1) << 30;
	tmc2130_wr(cs, TMC2130_REG_CHOPCONF, val);
}

//void tmc2130_wr_PWMCONF(uint8_t cs, uint8_t PWMautoScale, uint8_t PWMfreq, uint8_t PWMgrad, uint8_t PWMampl)
void tmc2130_wr_PWMCONF(uint8_t cs, uint8_t pwm_ampl, uint8_t pwm_grad, uint8_t pwm_freq, uint8_t pwm_auto, uint8_t pwm_symm, uint8_t freewheel)
{
	uint32_t val = 0;
	val |= (uint32_t)(pwm_ampl & 255);
	val |= (uint32_t)(pwm_grad & 255) << 8;
	val |= (uint32_t)(pwm_freq & 3) << 16;
	val |= (uint32_t)(pwm_auto & 1) << 18;
	val |= (uint32_t)(pwm_symm & 1) << 19;
	val |= (uint32_t)(freewheel & 3) << 20;
	tmc2130_wr(cs, TMC2130_REG_PWMCONF, val);
//	tmc2130_wr(cs, TMC2130_REG_PWMCONF, ((uint32_t)(PWMautoScale+PWMfreq) << 16) | ((uint32_t)PWMgrad << 8) | PWMampl); // TMC LJ -> For better readability changed to 0x00 and added PWMautoScale and PWMfreq
}

void tmc2130_wr_TPWMTHRS(uint8_t cs, uint32_t val32)
{
	tmc2130_wr(cs, TMC2130_REG_TPWMTHRS, val32);
}

void tmc2130_wr_THIGH(uint8_t cs, uint32_t val32)
{
	tmc2130_wr(cs, TMC2130_REG_THIGH, val32);
}

#if defined(TMC2130_DEBUG_RD) || defined(TMC2130_DEBUG_WR)
uint8_t tmc2130_axis_by_cs(uint8_t cs)
{
	switch (cs)
	{
	case X_TMC2130_CS: return 0;
	case Y_TMC2130_CS: return 1;
	case Z_TMC2130_CS: return 2;
	case E0_TMC2130_CS: return 3;
	}
	return -1;
}
#endif //TMC2130_DEBUG

uint8_t tmc2130_calc_mres(uint16_t microstep_resolution)
{
	if (microstep_resolution == 256) return 0b0000;
	if (microstep_resolution == 128) return 0b0001;
	if (microstep_resolution == 64)  return 0b0010;
	if (microstep_resolution == 32)  return 0b0011;
	if (microstep_resolution == 16)  return 0b0100;
	if (microstep_resolution == 8)   return 0b0101;
	if (microstep_resolution == 4)   return 0b0110;
	if (microstep_resolution == 2)   return 0b0111;
	if (microstep_resolution == 1)   return 0b1000;
	return 0;
}

uint8_t tmc2130_wr(uint8_t cs, uint8_t addr, uint32_t wval)
{
	uint8_t stat = tmc2130_txrx(cs, addr | 0x80, wval, 0);
#ifdef TMC2130_DEBUG_WR
	MYSERIAL.print("tmc2130_wr(");
	MYSERIAL.print((unsigned char)tmc2130_axis_by_cs(cs), DEC);
	MYSERIAL.print(", 0x");
	MYSERIAL.print((unsigned char)addr, HEX);
	MYSERIAL.print(", 0x");
	MYSERIAL.print((unsigned long)wval, HEX);
	MYSERIAL.print(")=0x");
	MYSERIAL.println((unsigned char)stat, HEX);
#endif //TMC2130_DEBUG_WR
	return stat;
}

uint8_t tmc2130_rd(uint8_t cs, uint8_t addr, uint32_t* rval)
{
	uint32_t val32 = 0;
	uint8_t stat = tmc2130_txrx(cs, addr, 0x00000000, &val32);
	if (rval != 0) *rval = val32;
#ifdef TMC2130_DEBUG_RD
	if (!skip_debug_msg)
	{
		MYSERIAL.print("tmc2130_rd(");
		MYSERIAL.print((unsigned char)tmc2130_axis_by_cs(cs), DEC);
		MYSERIAL.print(", 0x");
		MYSERIAL.print((unsigned char)addr, HEX);
		MYSERIAL.print(", 0x");
		MYSERIAL.print((unsigned long)val32, HEX);
		MYSERIAL.print(")=0x");
		MYSERIAL.println((unsigned char)stat, HEX);
	}
	skip_debug_msg = false;
#endif //TMC2130_DEBUG_RD
	return stat;
}

uint8_t tmc2130_txrx(uint8_t cs, uint8_t addr, uint32_t wval, uint32_t* rval)
{
	//datagram1 - request
	SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
	digitalWrite(cs, LOW);
	SPI.transfer(addr); // address
	SPI.transfer((wval >> 24) & 0xff); // MSB
	SPI.transfer((wval >> 16) & 0xff);
	SPI.transfer((wval >> 8) & 0xff);
	SPI.transfer(wval & 0xff); // LSB
	digitalWrite(cs, HIGH);
	SPI.endTransaction();
	//datagram2 - response
	SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
	digitalWrite(cs, LOW);
	uint8_t stat = SPI.transfer(0); // status
	uint32_t val32 = 0;
	val32 = SPI.transfer(0); // MSB
	val32 = (val32 << 8) | SPI.transfer(0);
	val32 = (val32 << 8) | SPI.transfer(0);
	val32 = (val32 << 8) | SPI.transfer(0); // LSB
	digitalWrite(cs, HIGH);
	SPI.endTransaction();
	if (rval != 0) *rval = val32;
	return stat;
}

void tmc2130_eeprom_load_config()
{
}

void tmc2130_eeprom_save_config()
{

}


#endif //TMC2130
