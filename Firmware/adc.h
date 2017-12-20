//adc.h
#ifndef _ADC_H
#define _ADC_H

#include <inttypes.h>
#include "config.h"


#if defined(__cplusplus)
extern "C" {
#endif //defined(__cplusplus)


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
