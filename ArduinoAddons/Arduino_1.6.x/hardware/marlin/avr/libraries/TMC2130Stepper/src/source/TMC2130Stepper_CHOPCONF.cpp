#include "TMC2130Stepper.h"
#include "TMC2130Stepper_MACROS.h"

// CHOPCONF
uint32_t TMC2130Stepper::CHOPCONF() { READ_REG(CHOPCONF); }
void TMC2130Stepper::CHOPCONF(uint32_t input) {
	CHOPCONF_sr = input;
	WRITE_REG(CHOPCONF);
}

void TMC2130Stepper::toff(		uint8_t B )	{ MOD_REG(CHOPCONF, TOFF);		}
void TMC2130Stepper::hstrt(		uint8_t B )	{ MOD_REG(CHOPCONF, HSTRT);		}
void TMC2130Stepper::hend(		int8_t  B )	{ MOD_REG(CHOPCONF, HEND);		}
void TMC2130Stepper::fd(		uint8_t B )	{ MOD_REG(CHOPCONF, FD);		}
void TMC2130Stepper::disfdcc(	bool 	B )	{ MOD_REG(CHOPCONF, DISFDCC);	}
void TMC2130Stepper::rndtf(		bool 	B )	{ MOD_REG(CHOPCONF, RNDTF);		}
void TMC2130Stepper::chm(		bool 	B )	{ MOD_REG(CHOPCONF, CHM);		}
void TMC2130Stepper::tbl(		uint8_t B )	{ MOD_REG(CHOPCONF, TBL);		}
void TMC2130Stepper::vsense(	bool 	B )	{ MOD_REG(CHOPCONF, VSENSE);	}
void TMC2130Stepper::vhighfs(	bool 	B )	{ MOD_REG(CHOPCONF, VHIGHFS);	}
void TMC2130Stepper::vhighchm(	bool 	B )	{ MOD_REG(CHOPCONF, VHIGHCHM);	}
void TMC2130Stepper::sync(		uint8_t B )	{ MOD_REG(CHOPCONF, SYNC);		}
void TMC2130Stepper::mres(		uint8_t B )	{ MOD_REG(CHOPCONF, MRES);		}
void TMC2130Stepper::intpol(	bool 	B )	{ MOD_REG(CHOPCONF, INTPOL);	}
void TMC2130Stepper::dedge(		bool 	B )	{ MOD_REG(CHOPCONF, DEDGE);		}
void TMC2130Stepper::diss2g(	bool 	B )	{ MOD_REG(CHOPCONF, DISS2G);	}

uint8_t TMC2130Stepper::toff()		{ GET_BYTE(CHOPCONF, TOFF);		}
uint8_t TMC2130Stepper::hstrt()		{ GET_BYTE(CHOPCONF, HSTRT);	}
int8_t  TMC2130Stepper::hend()		{ GET_BYTE(CHOPCONF, HEND);		}
uint8_t TMC2130Stepper::fd()		{ GET_BYTE(CHOPCONF, FD);		}
bool 	TMC2130Stepper::disfdcc()	{ GET_BYTE(CHOPCONF, DISFDCC);	}
bool 	TMC2130Stepper::rndtf()		{ GET_BYTE(CHOPCONF, RNDTF);	}
bool 	TMC2130Stepper::chm()		{ GET_BYTE(CHOPCONF, CHM);		}
uint8_t TMC2130Stepper::tbl()		{ GET_BYTE(CHOPCONF, TBL);		}
bool 	TMC2130Stepper::vsense()	{ GET_BIT( CHOPCONF, VSENSE);	}
bool 	TMC2130Stepper::vhighfs()	{ GET_BYTE(CHOPCONF, VHIGHFS);	}
bool 	TMC2130Stepper::vhighchm()	{ GET_BYTE(CHOPCONF, VHIGHCHM);	}
uint8_t TMC2130Stepper::sync()		{ GET_BYTE(CHOPCONF, SYNC);		}
uint8_t TMC2130Stepper::mres()		{ GET_BYTE(CHOPCONF, MRES);		}
bool 	TMC2130Stepper::intpol()	{ GET_BYTE(CHOPCONF, INTPOL);	}
bool 	TMC2130Stepper::dedge()		{ GET_BYTE(CHOPCONF, DEDGE);	}
bool 	TMC2130Stepper::diss2g()	{ GET_BYTE(CHOPCONF, DISS2G);	}
