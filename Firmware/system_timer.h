//! @file

#ifndef FIRMWARE_SYSTEM_TIMER_H_
#define FIRMWARE_SYSTEM_TIMER_H_

#include "Arduino.h"
#include "macros.h"
#define SYSTEM_TIMER_2

#ifdef SYSTEM_TIMER_2
#include "timer02.h"
#include "tone04.h"
#define _millis millis2
#define _micros micros2
#define _delay delay2
#define _tone tone4
#define _noTone noTone4

#define timer02_set_pwm0(pwm0)

#else //SYSTEM_TIMER_2
#define _millis millis
#define _micros micros
#define _delay delay
#define _tone tone
#define _noTone noTone
#define timer02_set_pwm0(pwm0)
#endif //SYSTEM_TIMER_2

// Timer counter, incremented by the 1ms Arduino timer.
// The standard Arduino timer() function returns this value atomically
// by disabling / enabling interrupts. This is costly, if the interrupts are known
// to be disabled.
#ifdef SYSTEM_TIMER_2
extern volatile unsigned long timer2_millis;
#else //SYSTEM_TIMER_2
extern volatile unsigned long timer0_millis;
#endif //SYSTEM_TIMER_2

// An unsynchronized equivalent to a standard Arduino _millis() function.
// To be used inside an interrupt routine.
FORCE_INLINE unsigned long millis_nc() {
#ifdef SYSTEM_TIMER_2
	return timer2_millis;
#else //SYSTEM_TIMER_2
	return timer0_millis;
#endif //SYSTEM_TIMER_2
}

#endif /* FIRMWARE_SYSTEM_TIMER_H_ */
