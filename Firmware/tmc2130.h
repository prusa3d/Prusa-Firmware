#ifndef TMC2130_H
#define TMC2130_H

#include <stdint.h>
#include "Configuration_var.h"

//mode
extern uint8_t tmc2130_mode;
//microstep resolution (0 means 256usteps, 8 means 1ustep
extern uint8_t tmc2130_mres[4];

//flags for axis stall detection
extern uint8_t tmc2130_sg_thr[4];

extern bool tmc2130_sg_stop_on_crash;
extern uint8_t tmc2130_sg_crash; //crash mask

extern uint8_t tmc2130_sg_homing_axes_mask;

extern const char eMotorCurrentScalingEnabled[];

#define TMC2130_MODE_NORMAL 0
#define TMC2130_MODE_SILENT 1

#define TMC2130_WAVE_FAC1000_MIN  30
#define TMC2130_WAVE_FAC1000_MAX 200
#define TMC2130_WAVE_FAC1000_STP   1

#define TMC2130_MINIMUM_PULSE 0   // minimum pulse width in uS
#define TMC2130_SET_DIR_DELAY 20  // minimum delay after setting direction in uS
#define TMC2130_SET_PWR_DELAY 0   // minimum delay after changing pwr mode in uS

#ifdef TMC2130_DEDGE_STEPPING
#define TMC2130_MINIMUM_DELAY //NOP
#elif TMC2130_MINIMUM_PULSE == 0
#define TMC2130_MINIMUM_DELAY asm("nop")
#else
#define TMC2130_MINIMUM_DELAY delayMicroseconds(TMC2130_MINIMUM_PULSE)
#endif

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

extern tmc2130_chopper_config_t tmc2130_chopper_config[NUM_AXIS];

struct MotorCurrents {
    // Refresh the vSense flag
    // If the vSense flag changes then both Run and Hold current values
    // must be shifted accordingly. This is done especially to handle
    // the edge case where only either of the current values are changed at runtime.
    // See M911 and M912
    void refreshCurrentScaling() {
        // IMPORTANT: iRun must have range 0 to 63 (2^6) so we can properly
        //            update the current scaling back and forth

        // Detect new vSense value
        const bool newvSense = (iRun < 32);
        if (vSense != newvSense) {
            // Update currents to match current scaling
            if (vSense) {
                // vSense was 1 [V_FS = 0.32V] but is changing to 0 [V_FS = 0.18V]
                // Half both current values to be in sync with current scale range
                iHold >>= 1;
                iRun >>= 1;
            } else {
                // vSense was 0 [V_FS = 0.18V], but is changing to 1 [V_FS = 0.32V]
                // double the Hold current value
                // iRun is expected to already be correct so no shift needed.
                // Keep in mind, only a change in iRun can change vSense.
                iHold <<= 1;
            }

            // Update vSense
            vSense = newvSense;
        } else if (!vSense) {
            // No change in vSense, but vSense = 0, which means we must scale down the iRun value
            // from range [0, 63] to range [0, 31]
            iRun >>= 1;
        }
    }

    // PROGMEM initializer
    inline __attribute__((always_inline)) MotorCurrents(const MotorCurrents &curr_P) { memcpy_P(this, &curr_P, sizeof(*this)); }

    constexpr inline __attribute__((always_inline)) MotorCurrents(uint8_t ir, uint8_t ih)
        : vSense((ir < 32) ? 1 : 0)
        , iRun((ir < 32) ? ir : (ir >> 1))
        , iHold((ir < 32) ? ih : (ih >> 1)) {}

    inline uint8_t getiRun() const { return iRun; }
    inline uint8_t getiHold() const { return min(iHold, iRun); }
    inline uint8_t getOriginaliRun() const { return vSense ? iRun : iRun << 1; }
    inline uint8_t getOriginaliHold() const { return min(vSense ? iHold : iHold << 1, getOriginaliRun()); }
    inline bool iHoldIsClamped() const { return iHold > iRun; }
    inline uint8_t getvSense() const { return vSense; }

    void __attribute__((noinline)) setiRun(uint8_t ir) {
        iRun = ir;

        // Refresh the vSense bit and take care of updating Hold/Run currents
        // accordingly
        refreshCurrentScaling();
    }

    void __attribute__((noinline)) setiHold(uint8_t ih) {
        iHold = vSense ? ih : ih >> 1;
        // Note that iHold cannot change the vSense bit. If iHold is larger
        // than iRun, then iHold is truncated later in SetCurrents()
    }

    private:
        // These members are protected in order to ensure that
        // the struct methods are used always to update these values at runtime.
        bool vSense; ///< VSense current scaling
        uint8_t iRun; ///< Running current
        uint8_t iHold; ///< Holding current
};

extern MotorCurrents currents[NUM_AXIS];

//initialize tmc2130

struct TMCInitParams {
    uint8_t bSuppressFlag : 1; // only relevant on MK3S with PSU_Delta
    uint8_t enableECool : 1;  // experimental support for E-motor cooler operation
    inline TMCInitParams():bSuppressFlag(0), enableECool(0) { }
    inline explicit TMCInitParams(bool bSuppressFlag, bool enableECool):bSuppressFlag(bSuppressFlag), enableECool(enableECool) { }
    inline explicit TMCInitParams(bool enableECool)
        : bSuppressFlag(
#ifdef PSU_Delta
        1
#else
        0
#endif
        )
        , enableECool(enableECool) { }
};
extern void tmc2130_init(TMCInitParams params);
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

//start stallguard measuring for single axis
extern void tmc2130_sg_measure_start(uint8_t axis);
//stop current stallguard measuring and report result
extern uint16_t tmc2130_sg_measure_stop();

// Enable or Disable crash detection according to EEPROM
void crashdet_use_eeprom_setting();

extern void tmc2130_setup_chopper(uint8_t axis, uint8_t mres, const MotorCurrents *curr = nullptr);

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
extern void tmc2130_get_wave(uint8_t axis, uint8_t* data);
extern void tmc2130_set_wave(uint8_t axis, uint8_t amp, uint8_t fac1000);

extern bool tmc2130_home_calibrate(uint8_t axis);

extern uint8_t tmc2130_cur2val(float cur);
extern float tmc2130_val2cur(uint8_t val);

#endif //TMC2130_H
