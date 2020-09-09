//! @file
#ifndef FSENSOR_H
#define FSENSOR_H

#include <inttypes.h>
#include "config.h"


// enable/disable flag
extern bool fsensor_enabled;
// not responding flag
extern bool fsensor_not_responding;
#ifdef PAT9125
// optical checking "chunk lenght" (already in steps)
extern int16_t fsensor_chunk_len;
// count of soft failures
extern uint8_t fsensor_softfail;
#endif

//! @name save restore printing
//! @{
extern void fsensor_stop_and_save_print(void);
//! restore print - restore position and heatup to original temperature
extern void fsensor_restore_print_and_continue(void);
//! split the current gcode stream to insert new instructions
extern void fsensor_checkpoint_print(void);
//! @}

//! initialize
extern void fsensor_init(void);

#ifdef PAT9125
//! update axis resolution
extern void fsensor_set_axis_steps_per_unit(float u);
#endif

//! @name enable/disable
//! @{
extern bool fsensor_enable(bool bUpdateEEPROM=true);
extern void fsensor_disable(bool bUpdateEEPROM=true);
//! @}

//autoload feature enabled
extern bool fsensor_autoload_enabled;
extern void fsensor_autoload_set(bool State);

extern void fsensor_update(void);
#ifdef PAT9125
//! setup pin-change interrupt
extern void fsensor_setup_interrupt(void);

//! @name autoload support
//! @{

extern void fsensor_autoload_check_start(void);
extern void fsensor_autoload_check_stop(void);
#endif //PAT9125
extern bool fsensor_check_autoload(void);
//! @}

#ifdef PAT9125
//! @name optical quality measurement support
//! @{
extern bool fsensor_oq_meassure_enabled;
extern void fsensor_oq_meassure_set(bool State);
extern void fsensor_oq_meassure_start(uint8_t skip);
extern void fsensor_oq_meassure_stop(void);
extern bool fsensor_oq_result(void);
//! @}

//! @name callbacks from stepper
//! @{
extern void fsensor_st_block_chunk(int cnt);

// debugging
extern uint8_t fsensor_log;

// There's really nothing to do in block_begin: the stepper ISR likely has
// called us already at the end of the last block, making this integration
// redundant. LA1.5 might not always do that during a coasting move, so attempt
// to drain fsensor_st_cnt anyway at the beginning of the new block.
#define fsensor_st_block_begin(rev) fsensor_st_block_chunk(0)
//! @}
#endif //PAT9125

#define VOLT_DIV_REF 5

#ifdef IR_SENSOR_ANALOG
#define IR_SENSOR_STEADY 10                       // [ms]

enum class ClFsensorPCB:uint_least8_t
{
    _Old=0,
    _Rev04=1,
    _Undef=EEPROM_EMPTY_VALUE
};

enum class ClFsensorActionNA:uint_least8_t
{
    _Continue=0,
    _Pause=1,
    _Undef=EEPROM_EMPTY_VALUE
};

extern ClFsensorPCB oFsensorPCB;
extern ClFsensorActionNA oFsensorActionNA;
extern const char* FsensorIRVersionText();

extern bool fsensor_IR_check();
constexpr uint16_t Voltage2Raw(float V){
	return ( V * 1023 * OVERSAMPLENR / VOLT_DIV_REF ) + 0.5F;
}
constexpr float Raw2Voltage(uint16_t raw){
	return VOLT_DIV_REF*(raw / (1023.F * OVERSAMPLENR) );
}
constexpr uint16_t IRsensor_Ldiode_TRESHOLD = Voltage2Raw(0.3F); // ~0.3V, raw value=982
constexpr uint16_t IRsensor_Lmax_TRESHOLD = Voltage2Raw(1.5F); // ~1.5V (0.3*Vcc), raw value=4910
constexpr uint16_t IRsensor_Hmin_TRESHOLD = Voltage2Raw(3.0F); // ~3.0V (0.6*Vcc), raw value=9821
constexpr uint16_t IRsensor_Hopen_TRESHOLD = Voltage2Raw(4.6F); // ~4.6V (N.C. @ Ru~20-50k, Rd'=56k, Ru'=10k), raw value=15059
constexpr uint16_t IRsensor_VMax_TRESHOLD = Voltage2Raw(5.F); // ~5V, raw value=16368

#endif //IR_SENSOR_ANALOG

#endif //FSENSOR_H
