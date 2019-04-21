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


extern uint8_t timer02_pwm0;

extern void timer02_set_pwm0(uint8_t pwm0);

extern void timer02_init(void);

extern unsigned long millis2(void);

extern unsigned long micros2(void);

extern void delay2(unsigned long ms);

extern void tone2(uint8_t _pin, unsigned int frequency/*, unsigned long duration*/);

extern void noTone2(uint8_t _pin);


#if defined(__cplusplus)
}
#endif //defined(__cplusplus)

#endif //TIMER02_H
