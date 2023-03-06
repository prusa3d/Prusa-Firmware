// fan control and check
#pragma once

#include "Configuration.h"
#include "config.h"

#if (defined(FANCHECK) && defined(TACH_0) && (TACH_0 > -1))
enum {
	EFCE_OK = 0,   //!< normal operation, both fans are ok
	EFCE_FIXED,    //!< previous fan error was fixed
	EFCE_REPORTED  //!< fan error detected and reported to LCD and serial
};
extern volatile uint8_t fan_check_error;

void readFanTach();
#endif //(defined(TACH_0))

#ifdef EXTRUDER_ALTFAN_DETECT
extern bool extruder_altfan_detect();
extern void altfanOverride_toggle();
extern bool altfanOverride_get();
#endif //EXTRUDER_ALTFAN_DETECT

#if (defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1)
#ifdef FAN_SOFT_PWM
extern bool fan_measuring;
#endif //FAN_SOFT_PWM

extern unsigned long extruder_autofan_last_check;
void setExtruderAutoFanState(uint8_t state);
void checkExtruderAutoFans();
#endif

void checkFans();
void resetFanCheck(); // resets the fan measuring state

void hotendFanSetFullSpeed();
void hotendDefaultAutoFanState();
