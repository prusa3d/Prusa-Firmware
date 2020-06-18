//tone04.h
// use atmega timer4 as main tone timer instead of timer2
// timer2 is used for System timer.
#ifndef TIMER04_H
#define TIMER04_H

#include <inttypes.h>

#if defined(__cplusplus)
extern "C" {
#endif //defined(__cplusplus)

extern void timer4_init(void);

extern void timer4_set_fan0(uint8_t duty);

extern void tone4(uint8_t _pin, uint16_t frequency);

extern void noTone4(uint8_t _pin);

#if defined(__cplusplus)
}
#endif //defined(__cplusplus)

#endif //TIMER02_H
