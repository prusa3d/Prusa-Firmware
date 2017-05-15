#include "TMC2130Stepper.h"
#include "TMC2130Stepper_MACROS.h"

// COOLCONF
uint32_t TMC2130Stepper::COOLCONF() { return COOLCONF_sr; }
void TMC2130Stepper::COOLCONF(uint32_t input) {
	COOLCONF_sr = input;
	WRITE_REG(COOLCONF);
}

void TMC2130Stepper::semin(		uint8_t B )	{ MOD_REG(COOLCONF, SEMIN);		}
void TMC2130Stepper::seup(		uint8_t B )	{ MOD_REG(COOLCONF, SEUP);		}
void TMC2130Stepper::semax(		uint8_t B )	{ MOD_REG(COOLCONF, SEMAX);		}
void TMC2130Stepper::sedn(		uint8_t B )	{ MOD_REG(COOLCONF, SEDN);		}
void TMC2130Stepper::seimin(	bool 	B )	{ MOD_REG(COOLCONF, SEIMIN);	}
void TMC2130Stepper::sgt(		uint8_t B )	{ MOD_REG(COOLCONF, SGT);		}
void TMC2130Stepper::sfilt(		bool 	B )	{ MOD_REG(COOLCONF, SFILT);		}

uint8_t TMC2130Stepper::semin()	{ GET_BYTE(COOLCONF, SEMIN);	}
uint8_t TMC2130Stepper::seup()	{ GET_BYTE(COOLCONF, SEUP);		}
uint8_t TMC2130Stepper::semax()	{ GET_BYTE(COOLCONF, SEMAX);	}
uint8_t TMC2130Stepper::sedn()	{ GET_BYTE(COOLCONF, SEDN);		}
bool TMC2130Stepper::seimin()	{ GET_BYTE(COOLCONF, SEIMIN);	}
uint8_t TMC2130Stepper::sgt()	{ GET_BYTE(COOLCONF, SGT);		}
bool TMC2130Stepper::sfilt()	{ GET_BYTE(COOLCONF, SFILT);	}
