#pragma once

#include <inttypes.h>
#include "config.h"

/*
http://resnet.uoregon.edu/~gurney_j/jmpc/bitwise.html
*/
#define BITCOUNT(x) (((BX_(x)+(BX_(x)>>4)) & 0x0F0F0F0F) % 255)
#define BX_(x) ((x) - (((x)>>1)&0x77777777) - (((x)>>2)&0x33333333) - (((x)>>3)&0x11111111))

#define ADC_PIN_IDX(pin) BITCOUNT(ADC_CHAN_MSK & ((1 << (pin)) - 1))

#if BITCOUNT(ADC_CHAN_MSK) != ADC_CHAN_CNT
# error "ADC_CHAN_MSK oes not match ADC_CHAN_CNT"
#endif

#define VOLT_DIV_REF 5 //[V]

extern volatile uint8_t adc_channel;
extern volatile uint16_t adc_values[ADC_CHAN_CNT];

extern void adc_init();
extern void adc_start_cycle(); //should be called from an atomic context only
static inline bool adc_cycle_done() { return adc_channel >= ADC_CHAN_CNT; }
