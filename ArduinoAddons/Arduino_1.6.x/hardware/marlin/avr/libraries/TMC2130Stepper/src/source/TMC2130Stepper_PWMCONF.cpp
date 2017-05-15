#include "TMC2130Stepper.h"
#include "TMC2130Stepper_MACROS.h"


// PWMCONF
uint32_t TMC2130Stepper::PWMCONF() { return PWMCONF_sr; }
void TMC2130Stepper::PWMCONF(uint32_t input) {
	PWMCONF_sr = input;
	WRITE_REG(PWMCONF);
}

void TMC2130Stepper::pwm_ampl(		uint8_t B )	{ MOD_REG(PWMCONF, PWM_AMPL);		}
void TMC2130Stepper::pwm_grad(		uint8_t B )	{ MOD_REG(PWMCONF, PWM_GRAD);		}
void TMC2130Stepper::pwm_freq(		uint8_t B )	{ MOD_REG(PWMCONF, PWM_FREQ);		}
void TMC2130Stepper::pwm_autoscale(	bool 	B )	{ MOD_REG(PWMCONF, PWM_AUTOSCALE);	}
void TMC2130Stepper::pwm_symmetric(	bool 	B )	{ MOD_REG(PWMCONF, PWM_SYMMETRIC);	}
void TMC2130Stepper::freewheel(		uint8_t B )	{ MOD_REG(PWMCONF, FREEWHEEL);		}

uint8_t TMC2130Stepper::pwm_ampl()		{ GET_BYTE(PWMCONF, PWM_AMPL);		}
uint8_t TMC2130Stepper::pwm_grad()		{ GET_BYTE(PWMCONF, PWM_GRAD);		}
uint8_t TMC2130Stepper::pwm_freq()		{ GET_BYTE(PWMCONF, PWM_FREQ);		}
bool 	TMC2130Stepper::pwm_autoscale()	{ GET_BYTE(PWMCONF, PWM_AUTOSCALE);	}
bool 	TMC2130Stepper::pwm_symmetric()	{ GET_BYTE(PWMCONF, PWM_SYMMETRIC);	}
uint8_t TMC2130Stepper::freewheel()		{ GET_BYTE(PWMCONF, FREEWHEEL);		}
