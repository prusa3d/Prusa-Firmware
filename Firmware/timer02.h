//timer02.h
// use atmega timer2 as main system timer instead of timer0
// timer0 is used for fast pwm (OC0B output)
// original OVF handler is disabled
#ifndef TIMER02_H
#define TIMER02_H

#include <inttypes.h>

#if defined(__cplusplus)
extern "C" {
#endif //defined(__cplusplus)

///! Initializes TIMER0 for fast PWM mode-driven bed heating
extern void timer0_init(void);

///! Reimplemented original millis() using timer2
extern unsigned long millis2(void);

///! Reimplemented original micros() using timer2
extern unsigned long micros2(void);

///! Reimplemented original delay() using timer2
extern void delay2(unsigned long ms);

#if defined(__cplusplus)
}
#endif //defined(__cplusplus)

#endif //TIMER02_H
