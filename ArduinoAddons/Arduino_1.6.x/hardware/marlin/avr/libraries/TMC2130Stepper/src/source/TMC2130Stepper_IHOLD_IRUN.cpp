#include "TMC2130Stepper.h"
#include "TMC2130Stepper_MACROS.h"

// IHOLD_IRUN
void TMC2130Stepper::IHOLD_IRUN(uint32_t input) {
	IHOLD_IRUN_sr = input;
	WRITE_REG(IHOLD_IRUN);
}
uint32_t TMC2130Stepper::IHOLD_IRUN() { return IHOLD_IRUN_sr; }

void 	TMC2130Stepper::ihold(uint8_t B) 		{ MOD_REG(IHOLD_IRUN, IHOLD);		}
void 	TMC2130Stepper::irun(uint8_t B)  		{ MOD_REG(IHOLD_IRUN, IRUN); 		}
void 	TMC2130Stepper::iholddelay(uint8_t B)	{ MOD_REG(IHOLD_IRUN, IHOLDDELAY); 	}
uint8_t TMC2130Stepper::ihold() 				{ GET_BYTE(IHOLD_IRUN, IHOLD);		}
uint8_t TMC2130Stepper::irun()  				{ GET_BYTE(IHOLD_IRUN, IRUN); 		}
uint8_t TMC2130Stepper::iholddelay()  			{ GET_BYTE(IHOLD_IRUN, IHOLDDELAY);	}
