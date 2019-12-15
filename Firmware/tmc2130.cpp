//! @file

#include "Marlin.h"

#ifdef TMC2130

#include "tmc2130.h"
#include "ultralcd.h"
#include "language.h"
#include "spi.h"



#define TMC2130_GCONF_NORMAL 0x00000000 // spreadCycle
#define TMC2130_GCONF_SGSENS 0x00003180 // spreadCycle with stallguard (stall activates DIAG0 and DIAG1 [pushpull])
#define TMC2130_GCONF_SILENT 0x00000004 // stealthChop


//mode
uint8_t tmc2130_mode = TMC2130_MODE_NORMAL;
//holding currents
uint8_t tmc2130_current_h[4] = TMC2130_CURRENTS_H;
//running currents
uint8_t tmc2130_current_r[4] = TMC2130_CURRENTS_R;

//running currents for homing
uint8_t tmc2130_current_r_home[4] = TMC2130_CURRENTS_R_HOME;


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
uint8_t tmc2130_sg_thr_home[4] = TMC2130_SG_THRS_HOME;


uint8_t tmc2130_sg_homing_axes_mask = 0x00;

uint8_t tmc2130_sg_meassure = 0xff;
uint32_t tmc2130_sg_meassure_cnt = 0;
uint32_t tmc2130_sg_meassure_val = 0;

uint8_t tmc2130_home_enabled = 0;
uint8_t tmc2130_home_origin[2] = {0, 0};
uint8_t tmc2130_home_bsteps[2] = {48, 48};
uint8_t tmc2130_home_fsteps[2] = {48, 48};

uint8_t tmc2130_wave_fac[4] = {0, 0, 0, 0};

tmc2130_chopper_config_t tmc2130_chopper_config[4] = {
	{TMC2130_TOFF_XYZ, 5, 1, 2, 0},
	{TMC2130_TOFF_XYZ, 5, 1, 2, 0},
	{TMC2130_TOFF_XYZ, 5, 1, 2, 0},
	{TMC2130_TOFF_E, 5, 1, 2, 0}
};

bool tmc2130_sg_stop_on_crash = true;
uint8_t tmc2130_sg_diag_mask = 0x00;
uint8_t tmc2130_sg_crash = 0;
uint16_t tmc2130_sg_err[4] = {0, 0, 0, 0};
uint16_t tmc2130_sg_cnt[4] = {0, 0, 0, 0};
bool tmc2130_sg_change = false;


bool skip_debug_msg = false;

#define DBG(args...)
//printf_P(args)
#ifndef _n
#define _n PSTR
#endif //_n
#ifndef _i
#define _i PSTR
#endif //_i

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


uint16_t tmc2130_rd_TSTEP(uint8_t axis);
uint16_t tmc2130_rd_MSCNT(uint8_t axis);
uint32_t tmc2130_rd_MSCURACT(uint8_t axis);

void tmc2130_wr_CHOPCONF(uint8_t axis, uint8_t toff = 3, uint8_t hstrt = 4, uint8_t hend = 1, uint8_t fd3 = 0, uint8_t disfdcc = 0, uint8_t rndtf = 0, uint8_t chm = 0, uint8_t tbl = 2, uint8_t vsense = 0, uint8_t vhighfs = 0, uint8_t vhighchm = 0, uint8_t sync = 0, uint8_t mres = 0b0100, uint8_t intpol = 1, uint8_t dedge = 0, uint8_t diss2g = 0);
void tmc2130_wr_PWMCONF(uint8_t axis, uint8_t pwm_ampl, uint8_t pwm_grad, uint8_t pwm_freq, uint8_t pwm_auto, uint8_t pwm_symm, uint8_t freewheel);
void tmc2130_wr_TPWMTHRS(uint8_t axis, uint32_t val32);
void tmc2130_wr_THIGH(uint8_t axis, uint32_t val32);

#define tmc2130_rd(axis, addr, rval) tmc2130_rx(axis, addr, rval)
#define tmc2130_wr(axis, addr, wval) tmc2130_tx(axis, (addr) | 0x80, wval)

static void tmc2130_tx(uint8_t axis, uint8_t addr, uint32_t wval);
static uint8_t tmc2130_rx(uint8_t axis, uint8_t addr, uint32_t* rval);


void tmc2130_setup_chopper(uint8_t axis, uint8_t mres, uint8_t current_h, uint8_t current_r);

uint16_t __tcoolthrs(uint8_t axis)
{
	switch (axis)
	{
	case X_AXIS: return TMC2130_TCOOLTHRS_X;
	case Y_AXIS: return TMC2130_TCOOLTHRS_Y;
	case Z_AXIS: return TMC2130_TCOOLTHRS_Z;
	}
	return 0;
}
#ifdef PSU_Delta
void tmc2130_init(bool bSupressFlag)
#else
void tmc2130_init()
#endif
{
//	DBG(_n("tmc2130_init(), mode=%S\n"), tmc2130_mode?_n("STEALTH"):_n("NORMAL"));
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
	for (uint_least8_t axis = 0; axis < 2; axis++) // X Y axes
	{
		tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);
		tmc2130_wr(axis, TMC2130_REG_TPOWERDOWN, 0x00000000);
		tmc2130_wr(axis, TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[axis]) << 16));
		tmc2130_wr(axis, TMC2130_REG_TCOOLTHRS, (tmc2130_mode == TMC2130_MODE_SILENT)?0:__tcoolthrs(axis));
		tmc2130_wr(axis, TMC2130_REG_GCONF, (tmc2130_mode == TMC2130_MODE_SILENT)?TMC2130_GCONF_SILENT:TMC2130_GCONF_SGSENS);
		tmc2130_wr_PWMCONF(axis, tmc2130_pwm_ampl[axis], tmc2130_pwm_grad[axis], tmc2130_pwm_freq[axis], tmc2130_pwm_auto[axis], 0, 0);
		tmc2130_wr_TPWMTHRS(axis, TMC2130_TPWMTHRS);
		//tmc2130_wr_THIGH(axis, TMC2130_THIGH);
	}
	for (uint_least8_t axis = 2; axis < 3; axis++) // Z axis
	{
		tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);
		tmc2130_wr(axis, TMC2130_REG_TPOWERDOWN, 0x00000000);
#ifndef TMC2130_STEALTH_Z
		tmc2130_wr(axis, TMC2130_REG_GCONF, TMC2130_GCONF_SGSENS);
#else //TMC2130_STEALTH_Z
		tmc2130_wr(axis, TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[axis]) << 16));
		tmc2130_wr(axis, TMC2130_REG_TCOOLTHRS, (tmc2130_mode == TMC2130_MODE_SILENT)?0:__tcoolthrs(axis));
		tmc2130_wr(axis, TMC2130_REG_GCONF, (tmc2130_mode == TMC2130_MODE_SILENT)?TMC2130_GCONF_SILENT:TMC2130_GCONF_SGSENS);
		tmc2130_wr_PWMCONF(axis, tmc2130_pwm_ampl[axis], tmc2130_pwm_grad[axis], tmc2130_pwm_freq[axis], tmc2130_pwm_auto[axis], 0, 0);
		tmc2130_wr_TPWMTHRS(axis, TMC2130_TPWMTHRS);
#endif //TMC2130_STEALTH_Z
	}
	for (uint_least8_t axis = 3; axis < 4; axis++) // E axis
	{
		tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);
		tmc2130_wr(axis, TMC2130_REG_TPOWERDOWN, 0x00000000);
#ifndef TMC2130_STEALTH_E
		tmc2130_wr(axis, TMC2130_REG_GCONF, TMC2130_GCONF_SGSENS);
#else //TMC2130_STEALTH_E
		tmc2130_wr(axis, TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[axis]) << 16));
		tmc2130_wr(axis, TMC2130_REG_TCOOLTHRS, 0);
		tmc2130_wr(axis, TMC2130_REG_GCONF, TMC2130_GCONF_SILENT);
		tmc2130_wr_PWMCONF(axis, tmc2130_pwm_ampl[axis], tmc2130_pwm_grad[axis], tmc2130_pwm_freq[axis], tmc2130_pwm_auto[axis], 0, 0);
		tmc2130_wr_TPWMTHRS(axis, TMC2130_TPWMTHRS);
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

#ifdef TMC2130_LINEARITY_CORRECTION
#ifdef TMC2130_LINEARITY_CORRECTION_XYZ
	tmc2130_set_wave(X_AXIS, 247, tmc2130_wave_fac[X_AXIS]);
	tmc2130_set_wave(Y_AXIS, 247, tmc2130_wave_fac[Y_AXIS]);
	tmc2130_set_wave(Z_AXIS, 247, tmc2130_wave_fac[Z_AXIS]);
#endif //TMC2130_LINEARITY_CORRECTION_XYZ
	tmc2130_set_wave(E_AXIS, 247, tmc2130_wave_fac[E_AXIS]);
#endif //TMC2130_LINEARITY_CORRECTION

#ifdef PSU_Delta
     if(!bSupressFlag)
          check_force_z();
#endif // PSU_Delta

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

void tmc2130_st_isr()
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
	if (tmc2130_sg_homing_axes_mask == 0)
	{
		if (tmc2130_sg_stop_on_crash && crash)
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
		uint32_t val32 = 0;
		tmc2130_rd(tmc2130_sg_meassure, TMC2130_REG_DRV_STATUS, &val32);
		tmc2130_sg_meassure_val += (val32 & 0x3ff);
		tmc2130_sg_meassure_cnt++;
		return true;
	}
	return false;
}

void tmc2130_home_enter(uint8_t axes_mask)
{
	printf_P(PSTR("tmc2130_home_enter(axes_mask=0x%02x)\n"), axes_mask);
#ifdef TMC2130_SG_HOMING
	if (axes_mask & 0x03) //X or Y
		tmc2130_wait_standstill_xy(1000);
	for (uint8_t axis = X_AXIS; axis <= Z_AXIS; axis++) //X Y and Z axes
	{
		uint8_t mask = (X_AXIS_MASK << axis);
		if (axes_mask & mask)
		{
			tmc2130_sg_homing_axes_mask |= mask;
			//Configuration to spreadCycle
			tmc2130_wr(axis, TMC2130_REG_GCONF, TMC2130_GCONF_NORMAL);
			tmc2130_wr(axis, TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr_home[axis]) << 16));
//			tmc2130_wr(axis, TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[axis]) << 16) | ((uint32_t)1 << 24));
			tmc2130_wr(axis, TMC2130_REG_TCOOLTHRS, __tcoolthrs(axis));
			tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r_home[axis]);
			if (mask & (X_AXIS_MASK | Y_AXIS_MASK | Z_AXIS_MASK))
				tmc2130_wr(axis, TMC2130_REG_GCONF, TMC2130_GCONF_SGSENS); //stallguard output DIAG1, DIAG1 = pushpull
		}
	}
#endif //TMC2130_SG_HOMING
}

void tmc2130_home_exit()
{
	printf_P(PSTR("tmc2130_home_exit tmc2130_sg_homing_axes_mask=0x%02x\n"), tmc2130_sg_homing_axes_mask);
#ifdef TMC2130_SG_HOMING
	if (tmc2130_sg_homing_axes_mask & 0x03) //X or Y
		tmc2130_wait_standstill_xy(1000);
	if (tmc2130_sg_homing_axes_mask)
	{
		for (uint8_t axis = X_AXIS; axis <= Z_AXIS; axis++) //X Y and Z axes
		{
			uint8_t mask = (X_AXIS_MASK << axis);
			if (tmc2130_sg_homing_axes_mask & mask & (X_AXIS_MASK | Y_AXIS_MASK | Z_AXIS_MASK))
			{
#ifndef TMC2130_STEALTH_Z
				if ((tmc2130_mode == TMC2130_MODE_SILENT) && (axis != Z_AXIS))
#else //TMC2130_STEALTH_Z
				if (tmc2130_mode == TMC2130_MODE_SILENT)
#endif //TMC2130_STEALTH_Z
				{
					tmc2130_wr(axis, TMC2130_REG_GCONF, TMC2130_GCONF_SILENT); // Configuration back to stealthChop
					tmc2130_wr(axis, TMC2130_REG_TCOOLTHRS, 0);
//					tmc2130_wr_PWMCONF(i, tmc2130_pwm_ampl[i], tmc2130_pwm_grad[i], tmc2130_pwm_freq[i], tmc2130_pwm_auto[i], 0, 0);
				}
				else
				{
//					tmc2130_wr(axis, TMC2130_REG_GCONF, TMC2130_GCONF_NORMAL);
					tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);
//					tmc2130_wr(axis, TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[axis]) << 16) | ((uint32_t)1 << 24));
					tmc2130_wr(axis, TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[axis]) << 16));
					tmc2130_wr(axis, TMC2130_REG_TCOOLTHRS, __tcoolthrs(axis));
					tmc2130_wr(axis, TMC2130_REG_GCONF, TMC2130_GCONF_SGSENS);
				}
			}
		}
		tmc2130_sg_homing_axes_mask = 0x00;
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
		tmc2130_rd(X_AXIS, TMC2130_REG_DRV_STATUS, &drv_status_x);
		tmc2130_rd(Y_AXIS, TMC2130_REG_DRV_STATUS, &drv_status_y);
//		DBG(_n("\tdrv_status_x=0x%08x drv_status_x=0x%08x\n"), drv_status_x, drv_status_y);
		standstill = (drv_status_x & 0x80000000) && (drv_status_y & 0x80000000);
		tmc2130_check_overtemp();
		timeout--;
	}
	return standstill;
}


void tmc2130_check_overtemp()
{
	static uint32_t checktime = 0;
	if (_millis() - checktime > 1000 )
	{
		for (uint_least8_t i = 0; i < 4; i++)
		{
			uint32_t drv_status = 0;
			skip_debug_msg = true;
			tmc2130_rd(i, TMC2130_REG_DRV_STATUS, &drv_status);
			if (drv_status & ((uint32_t)1 << 26))
			{ // BIT 26 - over temp prewarning ~120C (+-20C)
				SERIAL_ERRORRPGM(MSG_TMC_OVERTEMP);
				SERIAL_ECHOLN(i);
				for (uint_least8_t j = 0; j < 4; j++)
					tmc2130_wr(j, TMC2130_REG_CHOPCONF, 0x00010000);
				kill(MSG_TMC_OVERTEMP);
			}

		}
		checktime = _millis();
		tmc2130_sg_change = true;
	}
#ifdef DEBUG_CRASHDET_COUNTERS
	if (tmc2130_sg_change)
	{
		for (int i = 0; i < 4; i++)
		{
			tmc2130_sg_change = false;
			lcd_set_cursor(0 + i*4, 3);
			lcd_print(itostr3(tmc2130_sg_cnt[i]));
			lcd_print(' ');
		}
	}
#endif //DEBUG_CRASHDET_COUNTERS
}

void tmc2130_setup_chopper(uint8_t axis, uint8_t mres, uint8_t current_h, uint8_t current_r)
{
	uint8_t intpol = (mres != 0); // intpol to 256 only if microsteps aren't 256
	uint8_t toff = tmc2130_chopper_config[axis].toff; // toff = 3 (fchop = 27.778kHz)
	uint8_t hstrt = tmc2130_chopper_config[axis].hstr; //initial 4, modified to 5
	uint8_t hend = tmc2130_chopper_config[axis].hend; //original value = 1
	uint8_t fd3 = 0;
	uint8_t rndtf = 0; //random off time
	uint8_t chm = 0; //spreadCycle
	uint8_t tbl = tmc2130_chopper_config[axis].tbl; //blanking time, original value = 2
	if (axis == E_AXIS)
	{
#ifdef TMC2130_CNSTOFF_E
		// fd = 0 (slow decay only)
		hstrt = 0; //fd0..2
		fd3 = 0; //fd3
		hend = 0; //sine wave offset
		chm = 1; // constant off time mod
#endif //TMC2130_CNSTOFF_E
//		toff = TMC2130_TOFF_E; // toff = 3-5
//		rndtf = 1;
	}
//	DBG(_n("tmc2130_setup_chopper(axis=%hhd, mres=%hhd, curh=%hhd, curr=%hhd\n"), axis, mres, current_h, current_r);
//	DBG(_n(" toff=%hhd, hstr=%hhd, hend=%hhd, tbl=%hhd\n"), toff, hstrt, hend, tbl);
	if (current_r <= 31)
	{
		tmc2130_wr_CHOPCONF(axis, toff, hstrt, hend, fd3, 0, rndtf, chm, tbl, 1, 0, 0, 0, mres, intpol, 0, 0);
		tmc2130_wr(axis, TMC2130_REG_IHOLD_IRUN, 0x000f0000 | ((current_r & 0x1f) << 8) | (current_h & 0x1f));
	}
	else
	{
		tmc2130_wr_CHOPCONF(axis, toff, hstrt, hend, fd3, 0, rndtf, chm, tbl, 0, 0, 0, 0, mres, intpol, 0, 0);
		tmc2130_wr(axis, TMC2130_REG_IHOLD_IRUN, 0x000f0000 | (((current_r >> 1) & 0x1f) << 8) | ((current_h >> 1) & 0x1f));
	}
}

void tmc2130_set_current_h(uint8_t axis, uint8_t current)
{
//	DBG(_n("tmc2130_set_current_h(axis=%d, current=%d\n"), axis, current);
	tmc2130_current_h[axis] = current;
	tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);
}

void tmc2130_set_current_r(uint8_t axis, uint8_t current)
{
//	DBG(_n("tmc2130_set_current_r(axis=%d, current=%d\n"), axis, current);
	tmc2130_current_r[axis] = current;
	tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);
}

void tmc2130_print_currents()
{
	printf_P(_n("tmc2130_print_currents()\n\tH\tR\nX\t%d\t%d\nY\t%d\t%d\nZ\t%d\t%d\nE\t%d\t%d\n"),
		tmc2130_current_h[0], tmc2130_current_r[0],
		tmc2130_current_h[1], tmc2130_current_r[1],
		tmc2130_current_h[2], tmc2130_current_r[2],
		tmc2130_current_h[3], tmc2130_current_r[3]
	);
}

void tmc2130_set_pwm_ampl(uint8_t axis, uint8_t pwm_ampl)
{
//	DBG(_n("tmc2130_set_pwm_ampl(axis=%hhd, pwm_ampl=%hhd\n"), axis, pwm_ampl);
	tmc2130_pwm_ampl[axis] = pwm_ampl;
	if (((axis == 0) || (axis == 1)) && (tmc2130_mode == TMC2130_MODE_SILENT))
		tmc2130_wr_PWMCONF(axis, tmc2130_pwm_ampl[axis], tmc2130_pwm_grad[axis], tmc2130_pwm_freq[axis], tmc2130_pwm_auto[axis], 0, 0);
}

void tmc2130_set_pwm_grad(uint8_t axis, uint8_t pwm_grad)
{
//	DBG(_n("tmc2130_set_pwm_grad(axis=%hhd, pwm_grad=%hhd\n"), axis, pwm_grad);
	tmc2130_pwm_grad[axis] = pwm_grad;
	if (((axis == 0) || (axis == 1)) && (tmc2130_mode == TMC2130_MODE_SILENT))
		tmc2130_wr_PWMCONF(axis, tmc2130_pwm_ampl[axis], tmc2130_pwm_grad[axis], tmc2130_pwm_freq[axis], tmc2130_pwm_auto[axis], 0, 0);
}

uint16_t tmc2130_rd_TSTEP(uint8_t axis)
{
	uint32_t val32 = 0;
	tmc2130_rd(axis, TMC2130_REG_TSTEP, &val32);
	if (val32 & 0x000f0000) return 0xffff;
	return val32 & 0xffff;
}

uint16_t tmc2130_rd_MSCNT(uint8_t axis)
{
	uint32_t val32 = 0;
	tmc2130_rd(axis, TMC2130_REG_MSCNT, &val32);
	return val32 & 0x3ff;
}

uint32_t tmc2130_rd_MSCURACT(uint8_t axis)
{
	uint32_t val32 = 0;
	tmc2130_rd(axis, TMC2130_REG_MSCURACT, &val32);
	return val32;
}

void tmc2130_wr_MSLUTSTART(uint8_t axis, uint8_t start_sin, uint8_t start_sin90)
{
	uint32_t val = 0;
	val |= (uint32_t)start_sin;
	val |= ((uint32_t)start_sin90) << 16;
	tmc2130_wr(axis, TMC2130_REG_MSLUTSTART, val);
	//printf_P(PSTR("MSLUTSTART=%08lx (start_sin=%d start_sin90=%d)\n"), val, start_sin, start_sin90);
}

void tmc2130_wr_MSLUTSEL(uint8_t axis, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t w0, uint8_t w1, uint8_t w2, uint8_t w3)
{
	uint32_t val = 0;
	val |= ((uint32_t)w0);
	val |= ((uint32_t)w1) << 2;
	val |= ((uint32_t)w2) << 4;
	val |= ((uint32_t)w3) << 6;
	val |= ((uint32_t)x1) << 8;
	val |= ((uint32_t)x2) << 16;
	val |= ((uint32_t)x3) << 24;
	tmc2130_wr(axis, TMC2130_REG_MSLUTSEL, val);
	//printf_P(PSTR("MSLUTSEL=%08lx (x1=%d x2=%d x3=%d w0=%d w1=%d w2=%d w3=%d)\n"), val, x1, x2, x3, w0, w1, w2, w3);
}

void tmc2130_wr_MSLUT(uint8_t axis, uint8_t i, uint32_t val)
{
	tmc2130_wr(axis, TMC2130_REG_MSLUT0 + (i & 7), val);
	//printf_P(PSTR("MSLUT[%d]=%08lx\n"), i, val);
}

void tmc2130_wr_CHOPCONF(uint8_t axis, uint8_t toff, uint8_t hstrt, uint8_t hend, uint8_t fd3, uint8_t disfdcc, uint8_t rndtf, uint8_t chm, uint8_t tbl, uint8_t vsense, uint8_t vhighfs, uint8_t vhighchm, uint8_t sync, uint8_t mres, uint8_t intpol, uint8_t dedge, uint8_t diss2g)
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
	tmc2130_wr(axis, TMC2130_REG_CHOPCONF, val);
}

//void tmc2130_wr_PWMCONF(uint8_t axis, uint8_t PWMautoScale, uint8_t PWMfreq, uint8_t PWMgrad, uint8_t PWMampl)
void tmc2130_wr_PWMCONF(uint8_t axis, uint8_t pwm_ampl, uint8_t pwm_grad, uint8_t pwm_freq, uint8_t pwm_auto, uint8_t pwm_symm, uint8_t freewheel)
{
	uint32_t val = 0;
	val |= (uint32_t)(pwm_ampl & 255);
	val |= (uint32_t)(pwm_grad & 255) << 8;
	val |= (uint32_t)(pwm_freq & 3) << 16;
	val |= (uint32_t)(pwm_auto & 1) << 18;
	val |= (uint32_t)(pwm_symm & 1) << 19;
	val |= (uint32_t)(freewheel & 3) << 20;
	tmc2130_wr(axis, TMC2130_REG_PWMCONF, val);
//	tmc2130_wr(axis, TMC2130_REG_PWMCONF, ((uint32_t)(PWMautoScale+PWMfreq) << 16) | ((uint32_t)PWMgrad << 8) | PWMampl); // TMC LJ -> For better readability changed to 0x00 and added PWMautoScale and PWMfreq
}

void tmc2130_wr_TPWMTHRS(uint8_t axis, uint32_t val32)
{
	tmc2130_wr(axis, TMC2130_REG_TPWMTHRS, val32);
}

void tmc2130_wr_THIGH(uint8_t axis, uint32_t val32)
{
	tmc2130_wr(axis, TMC2130_REG_THIGH, val32);
}

uint8_t tmc2130_usteps2mres(uint16_t usteps)
{
	uint8_t mres = 8; while (usteps >>= 1) mres--;
	return mres;
}


inline void tmc2130_cs_low(uint8_t axis)
{
	switch (axis)
	{
	case X_AXIS: WRITE(X_TMC2130_CS, LOW); break;
	case Y_AXIS: WRITE(Y_TMC2130_CS, LOW); break;
	case Z_AXIS: WRITE(Z_TMC2130_CS, LOW); break;
	case E_AXIS: WRITE(E0_TMC2130_CS, LOW); break;
	}
}

inline void tmc2130_cs_high(uint8_t axis)
{
	switch (axis)
	{
	case X_AXIS: WRITE(X_TMC2130_CS, HIGH); break;
	case Y_AXIS: WRITE(Y_TMC2130_CS, HIGH); break;
	case Z_AXIS: WRITE(Z_TMC2130_CS, HIGH); break;
	case E_AXIS: WRITE(E0_TMC2130_CS, HIGH); break;
	}
}

//spi
#define TMC2130_SPI_ENTER()    spi_setup(TMC2130_SPCR, TMC2130_SPSR)
#define TMC2130_SPI_TXRX       spi_txrx
#define TMC2130_SPI_LEAVE()

static void tmc2130_tx(uint8_t axis, uint8_t addr, uint32_t wval)
{
	//datagram1 - request
	TMC2130_SPI_ENTER();
	tmc2130_cs_low(axis);
	TMC2130_SPI_TXRX(addr); // address
	TMC2130_SPI_TXRX((wval >> 24) & 0xff); // MSB
	TMC2130_SPI_TXRX((wval >> 16) & 0xff);
	TMC2130_SPI_TXRX((wval >> 8) & 0xff);
	TMC2130_SPI_TXRX(wval & 0xff); // LSB
	tmc2130_cs_high(axis);
	TMC2130_SPI_LEAVE();
}

static uint8_t tmc2130_rx(uint8_t axis, uint8_t addr, uint32_t* rval)
{
	//datagram1 - request
	TMC2130_SPI_ENTER();
	tmc2130_cs_low(axis);
	TMC2130_SPI_TXRX(addr); // address
	TMC2130_SPI_TXRX(0); // MSB
	TMC2130_SPI_TXRX(0);
	TMC2130_SPI_TXRX(0);
	TMC2130_SPI_TXRX(0); // LSB
	tmc2130_cs_high(axis);
	TMC2130_SPI_LEAVE();
	//datagram2 - response
	TMC2130_SPI_ENTER();
	tmc2130_cs_low(axis);
	uint8_t stat = TMC2130_SPI_TXRX(0); // status
	uint32_t val32 = 0;
	val32 = TMC2130_SPI_TXRX(0); // MSB
	val32 = (val32 << 8) | TMC2130_SPI_TXRX(0);
	val32 = (val32 << 8) | TMC2130_SPI_TXRX(0);
	val32 = (val32 << 8) | TMC2130_SPI_TXRX(0); // LSB
	tmc2130_cs_high(axis);
	TMC2130_SPI_LEAVE();
	if (rval != 0) *rval = val32;
	return stat;
}

#define _GET_PWR_X      (READ(X_ENABLE_PIN) == X_ENABLE_ON)
#define _GET_PWR_Y      (READ(Y_ENABLE_PIN) == Y_ENABLE_ON)
#define _GET_PWR_Z      (READ(Z_ENABLE_PIN) == Z_ENABLE_ON)
#define _GET_PWR_E      (READ(E0_ENABLE_PIN) == E_ENABLE_ON)

#define _SET_PWR_X(ena) { WRITE(X_ENABLE_PIN, ena?X_ENABLE_ON:!X_ENABLE_ON); asm("nop"); }
#define _SET_PWR_Y(ena) { WRITE(Y_ENABLE_PIN, ena?Y_ENABLE_ON:!Y_ENABLE_ON); asm("nop"); }
#define _SET_PWR_Z(ena) { WRITE(Z_ENABLE_PIN, ena?Z_ENABLE_ON:!Z_ENABLE_ON); asm("nop"); }
#define _SET_PWR_E(ena) { WRITE(E0_ENABLE_PIN, ena?E_ENABLE_ON:!E_ENABLE_ON); asm("nop"); }

#define _GET_DIR_X      (READ(X_DIR_PIN) == INVERT_X_DIR)
#define _GET_DIR_Y      (READ(Y_DIR_PIN) == INVERT_Y_DIR)
#define _GET_DIR_Z      (READ(Z_DIR_PIN) == INVERT_Z_DIR)
#define _GET_DIR_E      (READ(E0_DIR_PIN) == INVERT_E0_DIR)

#define _SET_DIR_X(dir) { WRITE(X_DIR_PIN, dir?INVERT_X_DIR:!INVERT_X_DIR); asm("nop"); }
#define _SET_DIR_Y(dir) { WRITE(Y_DIR_PIN, dir?INVERT_Y_DIR:!INVERT_Y_DIR); asm("nop"); }
#define _SET_DIR_Z(dir) { WRITE(Z_DIR_PIN, dir?INVERT_Z_DIR:!INVERT_Z_DIR); asm("nop"); }
#define _SET_DIR_E(dir) { WRITE(E0_DIR_PIN, dir?INVERT_E0_DIR:!INVERT_E0_DIR); asm("nop"); }

#define _DO_STEP_X      { WRITE(X_STEP_PIN, !INVERT_X_STEP_PIN); asm("nop"); WRITE(X_STEP_PIN, INVERT_X_STEP_PIN); asm("nop"); }
#define _DO_STEP_Y      { WRITE(Y_STEP_PIN, !INVERT_Y_STEP_PIN); asm("nop"); WRITE(Y_STEP_PIN, INVERT_Y_STEP_PIN); asm("nop"); }
#define _DO_STEP_Z      { WRITE(Z_STEP_PIN, !INVERT_Z_STEP_PIN); asm("nop"); WRITE(Z_STEP_PIN, INVERT_Z_STEP_PIN); asm("nop"); }
#define _DO_STEP_E      { WRITE(E0_STEP_PIN, !INVERT_E_STEP_PIN); asm("nop"); WRITE(E0_STEP_PIN, INVERT_E_STEP_PIN); asm("nop"); }


uint16_t tmc2130_get_res(uint8_t axis)
{
	return tmc2130_mres2usteps(tmc2130_mres[axis]);
}

void tmc2130_set_res(uint8_t axis, uint16_t res)
{
	tmc2130_mres[axis] = tmc2130_usteps2mres(res);
//	uint32_t u = _micros();
	tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);
//	u = _micros() - u;
//	printf_P(PSTR("tmc2130_setup_chopper %c %lu us"), "XYZE"[axis], u);
}

uint8_t tmc2130_get_pwr(uint8_t axis)
{
	switch (axis)
	{
	case X_AXIS: return _GET_PWR_X;
	case Y_AXIS: return _GET_PWR_Y;
	case Z_AXIS: return _GET_PWR_Z;
	case E_AXIS: return _GET_PWR_E;
	}
	return 0;
}

//! @par pwr motor power
//!  * 0 disabled
//!  * non-zero enabled
void tmc2130_set_pwr(uint8_t axis, uint8_t pwr)
{
	switch (axis)
	{
	case X_AXIS: _SET_PWR_X(pwr); break;
	case Y_AXIS: _SET_PWR_Y(pwr); break;
	case Z_AXIS: _SET_PWR_Z(pwr); break;
	case E_AXIS: _SET_PWR_E(pwr); break;
	}
}

uint8_t tmc2130_get_inv(uint8_t axis)
{
	switch (axis)
	{
	case X_AXIS: return INVERT_X_DIR;
	case Y_AXIS: return INVERT_Y_DIR;
	case Z_AXIS: return INVERT_Z_DIR;
	case E_AXIS: return INVERT_E0_DIR;
	}
	return 0;
}

uint8_t tmc2130_get_dir(uint8_t axis)
{
	switch (axis)
	{
	case X_AXIS: return _GET_DIR_X;
	case Y_AXIS: return _GET_DIR_Y;
	case Z_AXIS: return _GET_DIR_Z;
	case E_AXIS: return _GET_DIR_E;
	}
	return 0;
}


void tmc2130_set_dir(uint8_t axis, uint8_t dir)
{
	switch (axis)
	{
	case X_AXIS: _SET_DIR_X(dir); break;
	case Y_AXIS: _SET_DIR_Y(dir); break;
	case Z_AXIS: _SET_DIR_Z(dir); break;
	case E_AXIS: _SET_DIR_E(dir); break;
	}
}

void tmc2130_do_step(uint8_t axis)
{
	switch (axis)
	{
	case X_AXIS: _DO_STEP_X; break;
	case Y_AXIS: _DO_STEP_Y; break;
	case Z_AXIS: _DO_STEP_Z; break;
	case E_AXIS: _DO_STEP_E; break;
	}
}

void tmc2130_do_steps(uint8_t axis, uint16_t steps, uint8_t dir, uint16_t delay_us)
{
	tmc2130_set_dir(axis, dir);
	delayMicroseconds(100);
	while (steps--)
	{
		tmc2130_do_step(axis);
		delayMicroseconds(delay_us);
	}
}

void tmc2130_goto_step(uint8_t axis, uint8_t step, uint8_t dir, uint16_t delay_us, uint16_t microstep_resolution)
{
	printf_P(PSTR("tmc2130_goto_step %d %d %d %d \n"), axis, step, dir, delay_us, microstep_resolution);
	uint8_t shift; for (shift = 0; shift < 8; shift++) if (microstep_resolution == (256u >> shift)) break;
	uint16_t cnt = 4 * (1 << (8 - shift));
	uint16_t mscnt = tmc2130_rd_MSCNT(axis);
	if (dir == 2)
	{
		dir = tmc2130_get_inv(axis)?0:1;
		int steps = (int)step - (int)(mscnt >> shift);
		if (steps > static_cast<int>(cnt / 2))
		{
			dir ^= 1;
			steps = cnt - steps; // This can create a negative step value
		}
        if (steps < 0)
		{
			dir ^= 1;
			steps = -steps;
		}
		cnt = steps;
	}
	tmc2130_set_dir(axis, dir);
	delayMicroseconds(100);
	mscnt = tmc2130_rd_MSCNT(axis);
	while ((cnt--) && ((mscnt >> shift) != step))
	{
		tmc2130_do_step(axis);
		delayMicroseconds(delay_us);
		mscnt = tmc2130_rd_MSCNT(axis);
	}
}

void tmc2130_get_wave(uint8_t axis, uint8_t* data, FILE* stream)
{
	uint8_t pwr = tmc2130_get_pwr(axis);
	tmc2130_set_pwr(axis, 0);
	tmc2130_setup_chopper(axis, tmc2130_usteps2mres(256), tmc2130_current_h[axis], tmc2130_current_r[axis]);
	tmc2130_goto_step(axis, 0, 2, 100, 256);
	tmc2130_set_dir(axis, tmc2130_get_inv(axis)?0:1);
	for (unsigned int i = 0; i <= 255; i++)
	{
		uint32_t val = tmc2130_rd_MSCURACT(axis);
		uint16_t mscnt = tmc2130_rd_MSCNT(axis);
		int curA = (val & 0xff) | ((val << 7) & 0x8000);
		if (stream)
		{
			if (mscnt == i)
				fprintf_P(stream, PSTR("%d\t%d\n"), i, curA);
			else //TODO - remove this check
				fprintf_P(stream, PSTR("!! (i=%d MSCNT=%d)\n"), i, mscnt);
		}
		if (data) *(data++) = curA;
		tmc2130_do_step(axis);
		delayMicroseconds(100);
	}
	tmc2130_setup_chopper(axis, tmc2130_mres[axis], tmc2130_current_h[axis], tmc2130_current_r[axis]);
	tmc2130_set_pwr(axis, pwr);
}

void tmc2130_set_wave(uint8_t axis, uint8_t amp, uint8_t fac1000)
{
// TMC2130 wave compression algorithm
// optimized for minimal memory requirements
//	printf_P(PSTR("tmc2130_set_wave %hhd %hhd\n"), axis, fac1000);
	if (fac1000 < TMC2130_WAVE_FAC1000_MIN) fac1000 = 0;
	if (fac1000 > TMC2130_WAVE_FAC1000_MAX) fac1000 = TMC2130_WAVE_FAC1000_MAX;
	float fac = 0;
	if (fac1000) fac = ((float)((uint16_t)fac1000 + 1000) / 1000); //correction factor
//	printf_P(PSTR(" factor: %s\n"), ftostr43(fac));
	uint8_t vA = 0;                //value of currentA
	uint8_t va = 0;                //previous vA
	int8_t d0 = 0;                //delta0
	int8_t d1 = 1;                //delta1
	uint8_t w[4] = {1,1,1,1};      //W bits (MSLUTSEL)
	uint8_t x[3] = {255,255,255};  //X segment bounds (MSLUTSEL)
	uint8_t s = 0;                 //current segment
	int8_t b;                      //encoded bit value
    int8_t dA;                     //delta value
	int i;                         //microstep index
	uint32_t reg = 0;              //tmc2130 register
	tmc2130_wr_MSLUTSTART(axis, 0, amp);
	for (i = 0; i < 256; i++)
	{
		if ((i & 0x1f) == 0)
			reg = 0;
		// calculate value
		if (fac == 0) // default TMC wave
			vA = (uint8_t)((amp+1) * sin((2*PI*i + PI)/1024) + 0.5) - 1;
		else // corrected wave
			vA = (uint8_t)(amp * pow(sin(2*PI*i/1024), fac) + 0.5);
		dA = vA - va; // calculate delta
		va = vA;
		b = -1;
		if (dA == d0) b = 0;      //delta == delta0 => bit=0
		else if (dA == d1) b = 1; //delta == delta1 => bit=1
		else
		{
			if (dA < d0) // delta < delta0 => switch wbit down
			{
				//printf("dn\n");
				b = 0;
				switch (dA)
				{
				case -1: d0 = -1; d1 = 0; w[s+1] = 0; break;
				case  0: d0 =  0; d1 = 1; w[s+1] = 1; break;
				case  1: d0 =  1; d1 = 2; w[s+1] = 2; break;
				default: b = -1; break;
				}
				if (b >= 0) { x[s] = i; s++; }
			}
			else if (dA > d1) // delta > delta0 => switch wbit up
			{
				//printf("up\n");
				b = 1;
				switch (dA)
				{
				case  1: d0 =  0; d1 = 1; w[s+1] = 1; break;
				case  2: d0 =  1; d1 = 2; w[s+1] = 2; break;
				case  3: d0 =  2; d1 = 3; w[s+1] = 3; break;
				default: b = -1; break;
				}
			    if (b >= 0) { x[s] = i; s++; }
			}
		}
		if (b < 0) break; // delta out of range (<-1 or >3)
		if (s > 3) break; // segment out of range (> 3)
		//printf("%d\n", vA);
		if (b == 1) reg |= 0x80000000;
		if ((i & 31) == 31)
			tmc2130_wr_MSLUT(axis, (uint8_t)(i >> 5), reg);
		else
			reg >>= 1;
//		printf("%3d\t%3d\t%2d\t%2d\t%2d\t%2d    %08x\n", i, vA, dA, b, w[s], s, reg);
	}
	tmc2130_wr_MSLUTSEL(axis, x[0], x[1], x[2], w[0], w[1], w[2], w[3]);
}

void bubblesort_uint8(uint8_t* data, uint8_t size, uint8_t* data2)
{
	uint8_t changed = 1;
	while (changed)
	{
		changed = 0;
		for (uint8_t i = 0; i < (size - 1); i++)
			if (data[i] > data[i+1])
			{
				uint8_t register d = data[i];
				data[i] = data[i+1];
				data[i+1] = d;
				if (data2)
				{
					d = data2[i];
					data2[i] = data2[i+1];
					data2[i+1] = d;
				}
				changed = 1;
			}
	}
}

uint8_t clusterize_uint8(uint8_t* data, uint8_t size, uint8_t* ccnt, uint8_t* cval, uint8_t tol)
{
	uint8_t cnt = 1;
	uint16_t sum = data[0];
	uint8_t cl = 0;
	for (uint8_t i = 1; i < size; i++)
	{
		uint8_t d = data[i];
		uint8_t val = sum / cnt;
		uint8_t dif = 0;
		if (val > d) dif = val - d;
		else dif = d - val;
		if (dif <= tol)
		{
			cnt += 1;
			sum += d;
		}
		else
		{
			if (ccnt) ccnt[cl] = cnt;
			if (cval) cval[cl] = val;
			cnt = 1;
			sum = d;
			cl += 1;
		}
	}
	if (ccnt) ccnt[cl] = cnt;
	if (cval) cval[cl] = sum / cnt;
	return ++cl;
}

bool tmc2130_home_calibrate(uint8_t axis)
{
	uint8_t step[16];
	uint8_t cnt[16];
	uint8_t val[16];
	homeaxis(axis, 16, step);
	bubblesort_uint8(step, 16, 0);
	printf_P(PSTR("sorted samples:\n"));
	for (uint8_t i = 0; i < 16; i++)
		printf_P(PSTR(" i=%2d step=%2d\n"), i, step[i]);
	uint8_t cl = clusterize_uint8(step, 16, cnt, val, 1);
	printf_P(PSTR("clusters:\n"));
	for (uint8_t i = 0; i < cl; i++)
		printf_P(PSTR(" i=%2d cnt=%2d val=%2d\n"), i, cnt[i], val[i]);
	bubblesort_uint8(cnt, cl, val);
	tmc2130_home_origin[axis] = val[cl-1];
	printf_P(PSTR("result value: %d\n"), tmc2130_home_origin[axis]);
	if (axis == X_AXIS) eeprom_update_byte((uint8_t*)EEPROM_TMC2130_HOME_X_ORIGIN, tmc2130_home_origin[X_AXIS]);
	else if (axis == Y_AXIS) eeprom_update_byte((uint8_t*)EEPROM_TMC2130_HOME_Y_ORIGIN, tmc2130_home_origin[Y_AXIS]);
	return true;
}


//! @brief Translate current to tmc2130 vsense and IHOLD or IRUN
//! @param cur current in mA
//! @return 0 .. 63
//! @n most significant bit is CHOPCONF vsense bit (sense resistor voltage based current scaling)
//! @n rest is to be used in IRUN or IHOLD register
//!
//! | mA   | trinamic register | note |
//! | ---  | ---               | ---  |
//! |    0 |  0 | doesn't mean current off, lowest current is 1/32 current with vsense low range |
//! |   30 |  1 | |
//! |   40 |  2 | |
//! |   60 |  3 | |
//! |   90 |  4 | |
//! |  100 |  5 | |
//! |  120 |  6 | |
//! |  130 |  7 | |
//! |  150 |  8 | |
//! |  180 |  9 | |
//! |  190 | 10 | |
//! |  210 | 11 | |
//! |  230 | 12 | |
//! |  240 | 13 | |
//! |  250 | 13 | |
//! |  260 | 14 | |
//! |  280 | 15 | |
//! |  300 | 16 | |
//! |  320 | 17 | |
//! |  340 | 18 | |
//! |  350 | 19 | |
//! |  370 | 20 | |
//! |  390 | 21 | |
//! |  410 | 22 | |
//! |  430 | 23 | |
//! |  450 | 24 | |
//! |  460 | 25 | |
//! |  480 | 26 | |
//! |  500 | 27 | |
//! |  520 | 28 | |
//! |  535 | 29 | |
//! |  N/D | 30 | extruder default |
//! |  540 | 33 | |
//! |  560 | 34 | |
//! |  580 | 35 | |
//! |  590 | 36 | farm mode extruder default |
//! |  610 | 37 | |
//! |  630 | 38 | |
//! |  640 | 39 | |
//! |  660 | 40 | |
//! |  670 | 41 | |
//! |  690 | 42 | |
//! |  710 | 43 | |
//! |  720 | 44 | |
//! |  730 | 45 | |
//! |  760 | 46 | |
//! |  770 | 47 | |
//! |  790 | 48 | |
//! |  810 | 49 | |
//! |  820 | 50 | |
//! |  840 | 51 | |
//! |  850 | 52 | |
//! |  870 | 53 | |
//! |  890 | 54 | |
//! |  900 | 55 | |
//! |  920 | 56 | |
//! |  940 | 57 | |
//! |  950 | 58 | |
//! |  970 | 59 | |
//! |  980 | 60 | |
//! | 1000 | 61 | |
//! | 1020 | 62 | |
//! | 1029 | 63 | |

uint8_t tmc2130_cur2val(float cur)
{
	if (cur < 0) cur = 0; //limit min
	if (cur > 1029) cur = 1029; //limit max
	//540mA is threshold for switch from high sense to low sense
	//for higher currents is maximum current 1029mA
	if (cur >= 540) return 63 * (float)cur / 1029;
	//for lower currents must be the value divided by 1.125 (= 0.18*2/0.32)
	return 63 * (float)cur / (1029 * 1.125);
}

float tmc2130_val2cur(uint8_t val)
{
	float rsense = 0.2; //0.2 ohm sense resistors
	uint8_t vsense = (val & 0x20)?0:1; //vsense bit = val>31
	float vfs = vsense?0.18:0.32; //vfs depends on vsense bit
	uint8_t val2 = vsense?val:(val >> 1); //vals 32..63 shifted right (16..31)
	// equation from datasheet (0.7071 ~= 1/sqrt(2))
	float cur = ((float)(val2 + 1)/32) * (vfs/(rsense + 0.02)) * 0.7071;
	return cur * 1000; //return current in mA
}



#endif //TMC2130
