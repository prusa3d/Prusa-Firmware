#include "TMC2130Stepper.h"
#include "TMC2130Stepper_MACROS.h"

uint32_t TMC2130Stepper::DRV_STATUS() { READ_REG_R(DRV_STATUS); }

uint16_t TMC2130Stepper::sg_result(){ GET_BYTE_R(DRV_STATUS, SG_RESULT); 	}
bool TMC2130Stepper::fsactive()		{ GET_BYTE_R(DRV_STATUS, FSACTIVE); 	}
uint8_t TMC2130Stepper::cs_actual()	{ GET_BYTE_R(DRV_STATUS, CS_ACTUAL); 	}
bool TMC2130Stepper::stallguard()	{ GET_BYTE_R(DRV_STATUS, STALLGUARD); 	}
bool TMC2130Stepper::ot()			{ GET_BYTE_R(DRV_STATUS, OT); 			}
bool TMC2130Stepper::otpw()			{ GET_BYTE_R(DRV_STATUS, OTPW); 		}
bool TMC2130Stepper::s2ga()			{ GET_BYTE_R(DRV_STATUS, S2GA); 		}
bool TMC2130Stepper::s2gb()			{ GET_BYTE_R(DRV_STATUS, S2GB); 		}
bool TMC2130Stepper::ola()			{ GET_BYTE_R(DRV_STATUS, OLA); 			}
bool TMC2130Stepper::olb()			{ GET_BYTE_R(DRV_STATUS, OLB); 			}
bool TMC2130Stepper::stst()			{ GET_BYTE_R(DRV_STATUS, STST); 		}
/*
uint16_t TMC2130Stepper::sg_result()	{
	uint32_t drv_status = 0x00000000UL;
	Serial.print("drv_status=");
	Serial.print(drv_status);
	drv_status = DRV_STATUS();
	Serial.print("\tdrv_status=");
	Serial.print(drv_status);
	Serial.print("\t");

	return drv_status&SG_RESULT_bm;
}
*/