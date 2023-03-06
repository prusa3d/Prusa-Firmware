#include "adc.h"
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "pins.h"

static uint8_t adc_count; //used for oversampling
static uint8_t adc_channel_idx; //bitmask index
volatile uint8_t adc_channel; //regular index
volatile uint16_t adc_values[ADC_CHAN_CNT];

static void adc_reset();
static void adc_setmux(uint8_t ch);

void adc_init()
{
	puts_P(PSTR("adc_init"));
    DIDR0 = ((ADC_CHAN_MSK & ADC_DIDR_MSK) & 0xff); //disable digital inputs PORTF
    DIDR2 = ((ADC_CHAN_MSK & ADC_DIDR_MSK) >> 8); //disable digital inputs PORTK
    ADMUX |= (1 << REFS0); //use AVCC as reference

    //enable ADC, set prescaler/128, enable interrupt
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADIF) | (1 << ADIE);
}

static void adc_reset()
{
    static const uint8_t first_channel_idx = 0;
    static_assert((1 << first_channel_idx) & ADC_CHAN_MSK);

    ADCSRA &= ~(1 << ADSC); //stop conversion just in case
    adc_count = 0;
    adc_channel = 0;
    adc_channel_idx = first_channel_idx;
    adc_setmux(adc_channel_idx);
    memset((void*)adc_values, 0, sizeof(adc_values));
}

static void adc_setmux(uint8_t ch)
{
	ch &= 0x0f;
	if (ch & 0x08) ADCSRB |= (1 << MUX5);
	else ADCSRB &= ~(1 << MUX5);
	ADMUX = (ADMUX & ~(0x07)) | (ch & 0x07);
}

void adc_start_cycle() {
	adc_reset();
	ADCSRA |= (1 << ADSC); //start conversion
}

#ifdef ADC_CALLBACK
extern void ADC_CALLBACK();
#endif //ADC_CALLBACK

ISR(ADC_vect)
{
    adc_values[adc_channel] += ADC;
    if (++adc_count == ADC_OVRSAMPL)
    {
        // go to the next channel
        if (++adc_channel == ADC_CHAN_CNT) {
#ifdef ADC_CALLBACK
            ADC_CALLBACK();
#endif
            return; // do not start the next measurement since there are no channels remaining
        }

        // find the next channel
        while (++adc_channel_idx) {
            if (ADC_CHAN_MSK & (1 << adc_channel_idx)) {
                adc_setmux(adc_channel_idx);
                adc_count = 0;
                break;
            }
        }
    }
    ADCSRA |= (1 << ADSC); //start conversion
}
