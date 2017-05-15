#ifndef TMC2130Stepper_MACROS_H
#define TMC2130Stepper_MACROS_H
#include "TMC2130Stepper.h"
#include "../TMC2130Stepper_REGDEFS.h"

#define WRITE_REG(R) 	send2130(TMC2130_WRITE|REG_##R, &R##_sr);

#define READ_REG(R)   	send2130(TMC2130_READ|REG_##R, &R##_sr); return R##_sr

#define READ_REG_R(R)   tmp_sr=0; send2130(TMC2130_READ|REG_##R, &tmp_sr); return tmp_sr;

#define MOD_REG(REG, SETTING) 	REG##_sr &= ~SETTING##_bm; \
								REG##_sr |= ((uint32_t)B<<SETTING##_bp)&SETTING##_bm; \
								WRITE_REG(REG);

#define GET_BYTE(REG, SETTING) 	return (REG()&SETTING##_bm) >> SETTING##_bp;

#define GET_BYTE_R(REG, SETTING) return (REG()&SETTING##_bm) >> SETTING##_bp;

#define GET_BIT(REG, SETTING) 	return (bool)((REG()&SETTING##_bm) >> SETTING##_bp);

#endif
