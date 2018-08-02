//adc.h
#ifndef _ADC_H
#define _ADC_H

#include <inttypes.h>
#include "config.h"


#if defined(__cplusplus)
extern "C" {
#endif //defined(__cplusplus)

/*
http://resnet.uoregon.edu/~gurney_j/jmpc/bitwise.html
*/
#define BITCOUNT(x) (((BX_(x)+(BX_(x)>>4)) & 0x0F0F0F0F) % 255)
#define BX_(x) ((x) - (((x)>>1)&0x77777777) - (((x)>>2)&0x33333333) - (((x)>>3)&0x11111111))

#define ADC_PIN_IDX(pin) BITCOUNT(ADC_CHAN_MSK & ((1 << (pin)) - 1))

#if BITCOUNT(ADC_CHAN_MSK) != ADC_CHAN_CNT
# error "ADC_CHAN_MSK oes not match ADC_CHAN_CNT"
#endif

extern uint8_t adc_state;
extern uint8_t adc_count;
extern uint16_t adc_values[ADC_CHAN_CNT];
extern uint16_t adc_sim_mask;


extern void adc_init(void);

extern void adc_reset(void);

extern void adc_setmux(uint8_t ch);

extern uint8_t adc_chan(uint8_t index);

extern void adc_cycle(void);


#if defined(__cplusplus)
}
#endif //defined(__cplusplus)
#endif //_ADC_H
