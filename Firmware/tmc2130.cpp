//! @file

#include "Marlin.h"

#ifdef TMC2130

#include "tmc2130.h"
#include "language.h"
#include "spi.h"
#include "Timer.h"

#define TMC2130_GCONF_NORMAL 0x00000000 // spreadCycle
#define TMC2130_GCONF_SGSENS 0x00000180 // spreadCycle with stallguard (stall activates DIAG0 and DIAG1 [open collector])
#define TMC2130_GCONF_DYNAMIC_SGSENS 0x00000184 // stealthChop/spreadCycle (dynamic) with stallguard (stall activates DIAG0 and DIAG1 [open collector])
#define TMC2130_GCONF_SILENT 0x00000004 // stealthChop

#ifdef TMC2130_DEDGE_STEPPING
static constexpr uint8_t default_dedge_bit = 1;
#define _DO_STEP_X      TOGGLE(X_STEP_PIN)
#define _DO_STEP_Y      TOGGLE(Y_STEP_PIN)
#define _DO_STEP_Z      TOGGLE(Z_STEP_PIN)
#define _DO_STEP_E      TOGGLE(E0_STEP_PIN)
#else // !TMC2130_DEDGE_STEPPING
static constexpr uint8_t default_dedge_bit = 0;
#define _DO_STEP_X      { WRITE(X_STEP_PIN, !INVERT_X_STEP_PIN); TMC2130_MINIMUM_DELAY; WRITE(X_STEP_PIN, INVERT_X_STEP_PIN); }
#define _DO_STEP_Y      { WRITE(Y_STEP_PIN, !INVERT_Y_STEP_PIN); TMC2130_MINIMUM_DELAY; WRITE(Y_STEP_PIN, INVERT_Y_STEP_PIN); }
#define _DO_STEP_Z      { WRITE(Z_STEP_PIN, !INVERT_Z_STEP_PIN); TMC2130_MINIMUM_DELAY; WRITE(Z_STEP_PIN, INVERT_Z_STEP_PIN); }
#define _DO_STEP_E      { WRITE(E0_STEP_PIN, !INVERT_E_STEP_PIN); TMC2130_MINIMUM_DELAY; WRITE(E0_STEP_PIN, INVERT_E_STEP_PIN); }
#endif // TMC2130_DEDGE_STEPPING

//mode
uint8_t tmc2130_mode = TMC2130_MODE_NORMAL;

static constexpr uint8_t tmc2130_default_current_h[4] = TMC2130_CURRENTS_H;
//running currents
static constexpr uint8_t tmc2130_default_current_r[4] = TMC2130_CURRENTS_R;
//running currents for homing
static constexpr uint8_t tmc2130_current_r_home[4] = TMC2130_CURRENTS_R_HOME;

static constexpr MotorCurrents homing_currents_P[NUM_AXIS] PROGMEM = {
	MotorCurrents(tmc2130_current_r_home[0], tmc2130_current_r_home[0]),
	MotorCurrents(tmc2130_current_r_home[1], tmc2130_current_r_home[1]),
	MotorCurrents(tmc2130_current_r_home[2], tmc2130_current_r_home[2]),
	MotorCurrents(tmc2130_current_r_home[3], tmc2130_current_r_home[3])
};

MotorCurrents currents[NUM_AXIS] = {
	MotorCurrents(tmc2130_default_current_r[0], tmc2130_default_current_h[0]),
	MotorCurrents(tmc2130_default_current_r[1], tmc2130_default_current_h[1]),
	MotorCurrents(tmc2130_default_current_r[2], tmc2130_default_current_h[2]),
	MotorCurrents(tmc2130_default_current_r[3], tmc2130_default_current_h[3])
};

union ChopConfU {
	struct __attribute__((packed)) S {
		uint32_t toff : 4;     // Off time and driver enable
		uint32_t hstrt : 3;    // Hysteresis start value added to HEND
		uint32_t hend : 4;     // HEND hysteresis low value (chm = 0) or OFFSET sine wave offset (chm = 1)
		uint32_t fd : 1;
		uint32_t disfdcc : 1;  // Fast decay mode
		uint32_t rndtf : 1;    // Random TOFF time 
		uint32_t chm : 1;      // Chopper mode
		uint32_t tbl : 2;      // Blank time select
		uint32_t vsense : 1;   // Sense resistor voltage based current scaling
		uint32_t vhighfs : 1;  // High velocity fullstep selection
		uint32_t vhighchm : 1; // High velocity chopper mode
		uint32_t sync : 4;     // PWM synchronization clock
		uint32_t mres : 4;     // Micro step resolution
		uint32_t intpol : 1;   // Interpolation to 256 microsteps
		uint32_t dedge : 1;    // Enable double edgestep pulses
		uint32_t diss2g : 1;   // Short to GND protection disable
		uint32_t reserved : 1; // Reserved, set to 0
		constexpr S(bool vsense, uint8_t mres)
			: toff(TMC2130_TOFF_XYZ)
			, hstrt(5)
			, hend(1)
			, fd(0)
			, disfdcc(0)
			, rndtf(0)   // Chopper off time is fixed as set by TOFF
			, chm(0)     // Standard mode (spreadCycle)
			, tbl(2)
			, vsense(vsense)
			, vhighfs(0)
			, vhighchm(0)
			, sync(0)
			, mres(mres)
			, intpol(0)
			, dedge(default_dedge_bit)
			, diss2g(0)  // Short to GND protection is on
			, reserved(0) {}
	} s;
	uint32_t dw;
	constexpr ChopConfU(bool vsense, uint8_t mres)
		: s(vsense, mres) {}
};
static_assert(sizeof(ChopConfU::S) == 4);
static_assert(sizeof(ChopConfU) == 4);

union PWMConfU {
    struct __attribute__((packed)) S {
        uint32_t pwm_ampl : 8; // User defined amplitude (offset)
        uint32_t pwm_grad : 8; // User defined amplitude (gradient) or regulation loop gradient
        uint32_t pwm_freq0 : 1; // PWM frequency selection
        uint32_t pwm_freq1 : 1;
        uint32_t pwm_autoscale : 1; // PWM automatic amplitude scaling
        uint32_t pwm_symmetric : 1; // Currently always zero
        uint32_t freewheel0 : 1; // Currently always zero
        uint32_t freewheel1 : 1; // Currently always zero
        uint32_t reserved : 10; // Set to zero
    } s;
    uint32_t dw;
    constexpr PWMConfU(uint32_t val)
            : dw(val) {}
};
static_assert(sizeof(PWMConfU::S) == 4);
static_assert(sizeof(PWMConfU) == 4);

/// Helper function to set bit shifts in one line
constexpr uint32_t PWMCONF_REG(uint32_t PWM_AMPL, uint32_t PWM_GRAD, uint32_t PWM_FREQ, uint32_t PWM_AUTO)
{
    return uint32_t((PWM_AMPL << 0U) | (PWM_GRAD<< 8U) | (PWM_FREQ << 16U) | (PWM_AUTO << 18U));
}

static constexpr uint32_t PWM_AMPL[NUM_AXIS] = {TMC2130_PWM_AMPL_X, TMC2130_PWM_AMPL_Y, TMC2130_PWM_AMPL_Z, TMC2130_PWM_AMPL_E};
static constexpr uint32_t PWM_GRAD[NUM_AXIS] = {TMC2130_PWM_GRAD_X, TMC2130_PWM_GRAD_Y, TMC2130_PWM_GRAD_Z, TMC2130_PWM_GRAD_E};
static constexpr uint32_t PWM_FREQ[NUM_AXIS] = {TMC2130_PWM_FREQ_X, TMC2130_PWM_FREQ_Y, TMC2130_PWM_FREQ_Z, TMC2130_PWM_FREQ_E};
static constexpr uint32_t PWM_AUTO[NUM_AXIS] = {TMC2130_PWM_AUTO_X, TMC2130_PWM_AUTO_Y, TMC2130_PWM_AUTO_Z, TMC2130_PWM_AUTO_E};

static PWMConfU pwmconf[NUM_AXIS] = {
    PWMConfU(PWMCONF_REG(PWM_AMPL[X_AXIS], PWM_GRAD[X_AXIS], PWM_FREQ[X_AXIS], PWM_AUTO[X_AXIS])),
    PWMConfU(PWMCONF_REG(PWM_AMPL[Y_AXIS], PWM_GRAD[Y_AXIS], PWM_FREQ[Y_AXIS], PWM_AUTO[Y_AXIS])),
    PWMConfU(PWMCONF_REG(PWM_AMPL[Z_AXIS], PWM_GRAD[Z_AXIS], PWM_FREQ[Z_AXIS], PWM_AUTO[Z_AXIS])),
    PWMConfU(PWMCONF_REG(PWM_AMPL[E_AXIS], PWM_GRAD[E_AXIS], PWM_FREQ[E_AXIS], PWM_AUTO[E_AXIS]))
};

// E-axis PWMCONF setting when using E-cool mode. Can be disabled/enabled at run time.
static constexpr PWMConfU pwmconf_Ecool = PWMConfU(PWMCONF_REG(TMC2130_PWM_AMPL_Ecool, TMC2130_PWM_GRAD_Ecool, PWM_FREQ[E_AXIS], TMC2130_PWM_AUTO_Ecool));

uint8_t tmc2130_mres[4] = {0, 0, 0, 0}; //will be filed at begin of init

uint8_t tmc2130_sg_thr[4] = {TMC2130_SG_THRS_X, TMC2130_SG_THRS_Y, TMC2130_SG_THRS_Z, TMC2130_SG_THRS_E};
static uint8_t tmc2130_sg_thr_home[4] = TMC2130_SG_THRS_HOME;


uint8_t tmc2130_sg_homing_axes_mask = 0x00;

const char eMotorCurrentScalingEnabled[] PROGMEM = "E-motor current scaling enabled";

static uint8_t tmc2130_sg_measure = 0xff;
static uint32_t tmc2130_sg_measure_cnt = 0;
static uint32_t tmc2130_sg_measure_val = 0;

uint8_t tmc2130_home_enabled = 0;
uint8_t tmc2130_home_origin[2] = {0, 0};
uint8_t tmc2130_home_bsteps[2] = {48, 48};
uint8_t tmc2130_home_fsteps[2] = {48, 48};

uint8_t tmc2130_wave_fac[4] = {0, 0, 0, 0};

tmc2130_chopper_config_t tmc2130_chopper_config[NUM_AXIS] = {
	{ // X axis
		.toff = TMC2130_TOFF_XYZ,
		.hstr = 5,
		.hend = 1,
		.tbl = 2,
		.res = 0
	},
	{ // Y axis
		.toff = TMC2130_TOFF_XYZ,
		.hstr = 5,
		.hend = 1,
		.tbl = 2,
		.res = 0
	},
	{ // Z axis
		.toff = TMC2130_TOFF_XYZ,
		.hstr = 5,
		.hend = 1,
		.tbl = 2,
		.res = 0
	},
#ifdef TMC2130_CNSTOFF_E
	{ // E axis
		.toff = TMC2130_TOFF_E,
		.hstr = 0,
		.hend = 0,
		.tbl = 2,
		.res = 0
	}
#else // !TMC2130_CNSTOFF_E
	{ // E axis
		.toff = TMC2130_TOFF_E,
		.hstr = 5,
		.hend = 1,
		.tbl = 2,
		.res = 0
	}
#endif
};

bool tmc2130_sg_stop_on_crash = true;
uint8_t tmc2130_sg_crash = 0;

//used for triggering a periodic check (1s) of the overtemperature pre-warning flag at ~120C (+-20C)
static ShortTimer tmc2130_overtemp_timer;

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

#define _GET_PWR_X      (READ(X_ENABLE_PIN) == X_ENABLE_ON)
#define _GET_PWR_Y      (READ(Y_ENABLE_PIN) == Y_ENABLE_ON)
#define _GET_PWR_Z      (READ(Z_ENABLE_PIN) == Z_ENABLE_ON)
#define _GET_PWR_E      (READ(E0_ENABLE_PIN) == E_ENABLE_ON)

#define _SET_PWR_X(ena) WRITE(X_ENABLE_PIN, ena?X_ENABLE_ON:!X_ENABLE_ON)
#define _SET_PWR_Y(ena) WRITE(Y_ENABLE_PIN, ena?Y_ENABLE_ON:!Y_ENABLE_ON)
#define _SET_PWR_Z(ena) WRITE(Z_ENABLE_PIN, ena?Z_ENABLE_ON:!Z_ENABLE_ON)
#define _SET_PWR_E(ena) WRITE(E0_ENABLE_PIN, ena?E_ENABLE_ON:!E_ENABLE_ON)

#define _GET_DIR_X      (READ(X_DIR_PIN) == INVERT_X_DIR)
#define _GET_DIR_Y      (READ(Y_DIR_PIN) == INVERT_Y_DIR)
#define _GET_DIR_Z      (READ(Z_DIR_PIN) == INVERT_Z_DIR)
#define _GET_DIR_E      (READ(E0_DIR_PIN) == INVERT_E0_DIR)

#define _SET_DIR_X(dir) WRITE(X_DIR_PIN, dir?INVERT_X_DIR:!INVERT_X_DIR)
#define _SET_DIR_Y(dir) WRITE(Y_DIR_PIN, dir?INVERT_Y_DIR:!INVERT_Y_DIR)
#define _SET_DIR_Z(dir) WRITE(Z_DIR_PIN, dir?INVERT_Z_DIR:!INVERT_Z_DIR)
#define _SET_DIR_E(dir) WRITE(E0_DIR_PIN, dir?INVERT_E0_DIR:!INVERT_E0_DIR)

uint16_t tmc2130_rd_TSTEP(uint8_t axis);
uint16_t tmc2130_rd_MSCNT(uint8_t axis);
uint32_t tmc2130_rd_MSCURACT(uint8_t axis);

#define tmc2130_rd(axis, addr, rval) tmc2130_rx(axis, addr, rval)
#define tmc2130_wr(axis, addr, wval) tmc2130_tx(axis, (addr) | 0x80, wval)

static void tmc2130_tx(uint8_t axis, uint8_t addr, uint32_t wval);
static uint8_t tmc2130_rx(uint8_t axis, uint8_t addr, uint32_t* rval);

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

static void tmc2130_XYZ_reg_init(uint8_t axis)
{
	tmc2130_setup_chopper(axis, tmc2130_mres[axis]);
	tmc2130_wr(axis, TMC2130_REG_TPOWERDOWN, 0x00000000);
	const bool isStealth = (tmc2130_mode == TMC2130_MODE_SILENT);
	if (axis == Z_AXIS) {
#ifdef TMC2130_STEALTH_Z
		tmc2130_wr(axis, TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[axis]) << 16) | ((uint32_t)1 << 24));
		tmc2130_wr(axis, TMC2130_REG_TCOOLTHRS, isStealth ? 0 : __tcoolthrs(axis));
		tmc2130_wr(axis, TMC2130_REG_GCONF, isStealth ? TMC2130_GCONF_SILENT : TMC2130_GCONF_DYNAMIC_SGSENS);
		tmc2130_wr(axis, TMC2130_REG_PWMCONF, pwmconf[axis].dw);
		tmc2130_wr(axis, TMC2130_REG_TPWMTHRS, isStealth ? 0 : 0xFFFF0);
#else // TMC2130_STEALTH_Z
		tmc2130_wr(axis, TMC2130_REG_GCONF, TMC2130_GCONF_SGSENS);
#endif // TMC2130_STEALTH_Z
	} else { // X Y
		tmc2130_wr(axis, TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[axis]) << 16) | ((uint32_t)1 << 24));
		tmc2130_wr(axis, TMC2130_REG_TCOOLTHRS, isStealth ? 0 : __tcoolthrs(axis));
		tmc2130_wr(axis, TMC2130_REG_GCONF, isStealth ? TMC2130_GCONF_SILENT : TMC2130_GCONF_SGSENS);
		tmc2130_wr(axis, TMC2130_REG_PWMCONF, pwmconf[axis].dw);
		tmc2130_wr(axis, TMC2130_REG_TPWMTHRS, TMC2130_TPWMTHRS);
	}
}

void tmc2130_init(TMCInitParams params)
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
	WRITE(X_TMC2130_DIAG,HIGH);
	WRITE(Y_TMC2130_DIAG,HIGH);
	WRITE(Z_TMC2130_DIAG,HIGH);
	WRITE(E0_TMC2130_DIAG,HIGH);
	
	for (uint_least8_t axis = 0; axis < E_AXIS; axis++) // X Y Z axes
	{
		tmc2130_XYZ_reg_init(axis);
	}

    // E axis
    tmc2130_setup_chopper(E_AXIS, tmc2130_mres[E_AXIS]);
    tmc2130_wr(E_AXIS, TMC2130_REG_TPOWERDOWN, 0x00000000);
#ifndef TMC2130_STEALTH_E
    if( ! params.enableECool ){
        tmc2130_wr(E_AXIS, TMC2130_REG_GCONF, TMC2130_GCONF_SGSENS);
    } else {
        tmc2130_wr(E_AXIS, TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[E_AXIS]) << 16));
        tmc2130_wr(E_AXIS, TMC2130_REG_TCOOLTHRS, 0);
        tmc2130_wr(E_AXIS, TMC2130_REG_GCONF, TMC2130_GCONF_SILENT);
        tmc2130_wr(E_AXIS, TMC2130_REG_PWMCONF, pwmconf_Ecool.dw);
        tmc2130_wr(E_AXIS, TMC2130_REG_TPWMTHRS, TMC2130_TPWMTHRS_E);
        SERIAL_ECHOLNRPGM(eMotorCurrentScalingEnabled);
    }
#else //TMC2130_STEALTH_E
    tmc2130_wr(E_AXIS, TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr[E_AXIS]) << 16));
    tmc2130_wr(E_AXIS, TMC2130_REG_TCOOLTHRS, 0);
    tmc2130_wr(E_AXIS, TMC2130_REG_GCONF, TMC2130_GCONF_SILENT);
    tmc2130_wr(E_AXIS, TMC2130_REG_PWMCONF, pwmconf[E_AXIS].dw);
    tmc2130_wr(E_AXIS, TMC2130_REG_TPWMTHRS, TMC2130_TPWMTHRS);
#endif //TMC2130_STEALTH_E

#ifdef TMC2130_LINEARITY_CORRECTION
#ifdef TMC2130_LINEARITY_CORRECTION_XYZ
	tmc2130_set_wave(X_AXIS, 247, tmc2130_wave_fac[X_AXIS]);
	tmc2130_set_wave(Y_AXIS, 247, tmc2130_wave_fac[Y_AXIS]);
	tmc2130_set_wave(Z_AXIS, 247, tmc2130_wave_fac[Z_AXIS]);
#endif //TMC2130_LINEARITY_CORRECTION_XYZ
	tmc2130_set_wave(E_AXIS, 247, tmc2130_wave_fac[E_AXIS]);
#endif //TMC2130_LINEARITY_CORRECTION

#ifdef PSU_Delta
     if(!params.bSuppressFlag)
          check_force_z();
#endif // PSU_Delta

}

uint8_t tmc2130_sample_diag()
{
	uint8_t mask = 0;
	if (!READ(X_TMC2130_DIAG)) mask |= X_AXIS_MASK;
	if (!READ(Y_TMC2130_DIAG)) mask |= Y_AXIS_MASK;
//	if (!READ(Z_TMC2130_DIAG)) mask |= Z_AXIS_MASK;
//	if (!READ(E0_TMC2130_DIAG)) mask |= E_AXIS_MASK;
	return mask;
}

void tmc2130_st_isr()
{
	if (tmc2130_mode == TMC2130_MODE_SILENT || tmc2130_sg_stop_on_crash == false || tmc2130_sg_homing_axes_mask != 0)
		return;
	uint8_t mask = tmc2130_sample_diag();
	if (tmc2130_sg_stop_on_crash && mask) {
		tmc2130_sg_crash = mask;
		tmc2130_sg_stop_on_crash = false;
		crashdet_stop_and_save_print();
	}
}


bool tmc2130_update_sg()
{
	if (tmc2130_sg_measure <= E_AXIS)
	{
		uint32_t val32 = 0;
		tmc2130_rd(tmc2130_sg_measure, TMC2130_REG_DRV_STATUS, &val32);
		tmc2130_sg_measure_val += (val32 & 0x3ff);
		tmc2130_sg_measure_cnt++;
		return true;
	}
	return false;
}

void tmc2130_home_enter(uint8_t axes_mask)
{
	printf_P(PSTR("tmc2130_home_enter(axes_mask=0x%02x)\n"), axes_mask);
#ifdef TMC2130_SG_HOMING
	if (axes_mask & (X_AXIS_MASK | Y_AXIS_MASK)) //X or Y
		tmc2130_wait_standstill_xy(1000);
	for (uint8_t axis = X_AXIS, mask = X_AXIS_MASK; axis <= Z_AXIS; axis++, mask <<= 1) //X Y and Z axes
	{
		if (axes_mask & mask)
		{
			tmc2130_sg_homing_axes_mask |= mask;
			//Configuration to spreadCycle
			tmc2130_wr(axis, TMC2130_REG_GCONF, TMC2130_GCONF_NORMAL);
			tmc2130_wr(axis, TMC2130_REG_COOLCONF, (((uint32_t)tmc2130_sg_thr_home[axis]) << 16));
			tmc2130_wr(axis, TMC2130_REG_TCOOLTHRS, __tcoolthrs(axis));
			MotorCurrents curr(homing_currents_P[axis]);
			tmc2130_setup_chopper(axis, tmc2130_mres[axis], &curr);
			tmc2130_wr(axis, TMC2130_REG_GCONF, TMC2130_GCONF_SGSENS); //stallguard output DIAG1, DIAG1 = pushpull
		}
	}
#endif //TMC2130_SG_HOMING
}

void tmc2130_home_exit()
{
	printf_P(PSTR("tmc2130_home_exit tmc2130_sg_homing_axes_mask=0x%02x\n"), tmc2130_sg_homing_axes_mask);
#ifdef TMC2130_SG_HOMING
	if (tmc2130_sg_homing_axes_mask & (X_AXIS_MASK | Y_AXIS_MASK))
		tmc2130_wait_standstill_xy(1000);
	if (tmc2130_sg_homing_axes_mask)
	{
		for (uint8_t axis = X_AXIS, mask = X_AXIS_MASK; axis <= Z_AXIS; axis++, mask <<= 1) //X Y and Z axes
		{
			if (tmc2130_sg_homing_axes_mask & mask) {
				tmc2130_XYZ_reg_init(axis);
			}
		}
		tmc2130_sg_homing_axes_mask = 0x00;
	}
	tmc2130_sg_crash = false;
#endif
}

void tmc2130_sg_measure_start(uint8_t axis)
{
	tmc2130_sg_measure = axis;
	tmc2130_sg_measure_cnt = 0;
	tmc2130_sg_measure_val = 0;
}

uint16_t tmc2130_sg_measure_stop()
{
	tmc2130_sg_measure = 0xff;
	return tmc2130_sg_measure_val / tmc2130_sg_measure_cnt;
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
	if (tmc2130_overtemp_timer.expired_cont(1000))
	{
		for (uint_least8_t i = 0; i < 4; i++)
		{
			uint32_t drv_status = 0;
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
		tmc2130_overtemp_timer.start();
	}
}

/// Helper function to determine the value of the CHOPCONF intpol flag
static constexpr bool getIntpolBit([[maybe_unused]]const uint8_t axis, const uint8_t mres) {
#if defined(TMC2130_INTPOL_E) && (TMC2130_INTPOL_E == 0)
    if (axis == E_AXIS) return 0;
#endif
#if defined(TMC2130_INTPOL_XY) && (TMC2130_INTPOL_XY == 0)
    if (axis == X_AXIS || axis == Y_AXIS) return 0;
#endif
#if defined(TMC2130_INTPOL_Z) && (TMC2130_INTPOL_Z == 0)
    if (axis == Z_AXIS) return 0;
#endif

    return (mres != 0); // intpol to 256 only if microsteps aren't 256
}

static void SetCurrents(const uint8_t axis, const MotorCurrents &curr) {
    uint8_t iHold = curr.getiHold();
    const uint8_t iRun = curr.getiRun();

    // Make sure iHold never exceeds iRun at runtime
    if (curr.iHoldIsClamped()) {
        // Let user know firmware modified the value
        SERIAL_ECHO_START;
        SERIAL_ECHOLNRPGM(_n("Hold current truncated to Run current"));
    }

    union IHoldRun {
        struct S {
            uint8_t iHold;
            uint8_t iRun;
            uint16_t iHoldDelay;
            constexpr S(uint8_t ih, uint8_t ir)
                : iHold(ih & 0x1F)
                , iRun(ir & 0x1F)
                , iHoldDelay(15 & 0x0F) {}
        } s;
        uint32_t dw;
        constexpr IHoldRun(uint8_t ih, uint8_t ir)
            : s(ih, ir) {}
    };

    IHoldRun ihold_irun(iHold, iRun);
#ifdef DEBUG_TMC_CURRENTS
	printf_P(PSTR("SetCurrents(axis=%u, iHold=%u, iRun=%u, vsense=%u, reg=%08lX)\n"), axis, iHold, iRun, curr.getvSense(), ihold_irun.dw);
#endif //DEBUG_TMC_CURRENTS
    tmc2130_wr(axis, TMC2130_REG_IHOLD_IRUN, ihold_irun.dw);
}

void tmc2130_setup_chopper(uint8_t axis, uint8_t mres, const MotorCurrents *curr /* = nullptr */)
{
	// Initialise the chopper configuration
	ChopConfU chopconf = ChopConfU(currents[axis].getvSense(), mres);

	chopconf.s.intpol = getIntpolBit(axis, mres);
	chopconf.s.toff = tmc2130_chopper_config[axis].toff; // toff = 3 (fchop = 27.778kHz)
	chopconf.s.hstrt = tmc2130_chopper_config[axis].hstr; // initial 4, modified to 5
	chopconf.s.hend = tmc2130_chopper_config[axis].hend; // original value = 1
	chopconf.s.tbl = tmc2130_chopper_config[axis].tbl; //blanking time, original value = 2

	tmc2130_wr(axis, TMC2130_REG_CHOPCONF, chopconf.dw);
	if (curr == nullptr) {
		curr = &currents[axis];
	}
	SetCurrents(axis, *curr);
}

void tmc2130_print_currents()
{
	printf_P(_n("tmc2130_print_currents()\n\tH\tR\nX\t%d\t%d\nY\t%d\t%d\nZ\t%d\t%d\nE\t%d\t%d\n"),
		currents[0].getiHold(), currents[0].getiRun(),
		currents[1].getiHold(), currents[1].getiRun(),
		currents[2].getiHold(), currents[2].getiRun(),
		currents[3].getiHold(), currents[3].getiRun()
	);
}

void tmc2130_set_pwm_ampl(uint8_t axis, uint8_t pwm_ampl)
{
    pwmconf[axis].s.pwm_ampl = pwm_ampl;
    if (((axis == X_AXIS) || (axis == Y_AXIS)) && (tmc2130_mode == TMC2130_MODE_SILENT))
        tmc2130_wr(axis, TMC2130_REG_PWMCONF, pwmconf[axis].dw);
}

void tmc2130_set_pwm_grad(uint8_t axis, uint8_t pwm_grad)
{
    pwmconf[axis].s.pwm_grad = pwm_grad;
    if (((axis == X_AXIS) || (axis == Y_AXIS)) && (tmc2130_mode == TMC2130_MODE_SILENT))
        tmc2130_wr(axis, TMC2130_REG_PWMCONF, pwmconf[axis].dw);
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

uint16_t tmc2130_get_res(uint8_t axis)
{
	return tmc2130_mres2usteps(tmc2130_mres[axis]);
}

void tmc2130_set_res(uint8_t axis, uint16_t res)
{
	tmc2130_mres[axis] = tmc2130_usteps2mres(res);
//	uint32_t u = _micros();
	tmc2130_setup_chopper(axis, tmc2130_mres[axis]);
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
    delayMicroseconds(TMC2130_SET_PWR_DELAY);
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
    delayMicroseconds(TMC2130_SET_DIR_DELAY);
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
    if (tmc2130_get_dir(axis) != dir)
        tmc2130_set_dir(axis, dir);
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
	mscnt = tmc2130_rd_MSCNT(axis);
	while ((cnt--) && ((mscnt >> shift) != step))
	{
		tmc2130_do_step(axis);
		delayMicroseconds(delay_us);
		mscnt = tmc2130_rd_MSCNT(axis);
	}
}

void tmc2130_get_wave(uint8_t axis, uint8_t* data)
{
	uint8_t pwr = tmc2130_get_pwr(axis);
	tmc2130_set_pwr(axis, 0);
	tmc2130_setup_chopper(axis, tmc2130_usteps2mres(256));
	tmc2130_goto_step(axis, 0, 2, 100, 256);
	tmc2130_set_dir(axis, tmc2130_get_inv(axis)?0:1);
	for (unsigned int i = 0; i <= 255; i++)
	{
		uint32_t val = tmc2130_rd_MSCURACT(axis);
		uint16_t mscnt = tmc2130_rd_MSCNT(axis);
		int curA = (val & 0xff) | ((val << 7) & 0x8000);
		if (mscnt == i)
			printf_P(PSTR("%d\t%d\n"), i, curA);
		else //TODO - remove this check
			printf_P(PSTR("! (i=%d MSCNT=%d)\n"), i, mscnt);
		if (data) *(data++) = curA;
		tmc2130_do_step(axis);
		delayMicroseconds(100);
	}
	tmc2130_setup_chopper(axis, tmc2130_mres[axis]);
	tmc2130_set_pwr(axis, pwr);
}

void tmc2130_set_wave(uint8_t axis, uint8_t amp, uint8_t fac1000)
{
// TMC2130 wave compression algorithm
// optimized for minimal memory requirements
//	printf_P(PSTR("tmc2130_set_wave %d %d\n"), axis, fac1000);
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
	uint8_t i = 0;                         //microstep index
	uint32_t reg = 0;              //tmc2130 register
	tmc2130_wr_MSLUTSTART(axis, 0, amp);
	do
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
	} while (i++ != 255);
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
				uint8_t d = data[i];
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
	puts_P(PSTR("sorted samples:"));
	for (uint8_t i = 0; i < 16; i++)
		printf_P(PSTR(" i=%2d step=%2d\n"), i, step[i]);
	uint8_t cl = clusterize_uint8(step, 16, cnt, val, 1);
	puts_P(PSTR("clusters:"));
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
