//timer02.c
// use atmega timer2 as main system timer instead of timer0
// timer0 is used for fast pwm (OC0B output)
// original OVF handler is disabled
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>


uint8_t timer02_pwm0 = 0;

void timer02_set_pwm0(uint8_t pwm0)
{
	if (timer02_pwm0 == pwm0) return;
	if (pwm0)
	{
		TCCR0A |= (2 << COM0B0);
		OCR0B = pwm0 - 1;
	}
	else
	{
		TCCR0A &= ~(2 << COM0B0);
		OCR0B = 0;
	}
}

void timer02_init(void)
{
	//save sreg
	uint8_t _sreg = SREG;
	//disable interrupts for sure
	cli();
	//mask timer0 interrupts - disable all
	TIMSK0 &= ~(1<<TOIE0);
	TIMSK0 &= ~(1<<OCIE0A);
	TIMSK0 &= ~(1<<OCIE0B);
	//setup timer0
	TCCR0A = 0x00; //COM_A-B=00, WGM_0-1=00
	TCCR0B = (1 << CS00); //WGM_2=0, CS_0-2=011
	//switch timer0 to fast pwm mode
	TCCR0A |= (3 << WGM00); //WGM_0-1=11
	//set OCR0B register to zero
	OCR0B = 0;
	//disable OCR0B output (will be enabled in timer02_set_pwm0)
	TCCR0A &= ~(2 << COM0B0);
	//setup timer2
	TCCR2A = 0x00; //COM_A-B=00, WGM_0-1=00
	TCCR2B = (3 << CS20); //WGM_2=0, CS_0-2=011
	//mask timer2 interrupts - enable OVF, disable others
	TIMSK2 |= (1<<TOIE2);
	TIMSK2 &= ~(1<<OCIE2A);
	TIMSK2 &= ~(1<<OCIE2B);
	//set timer2 OCR registers (OCRB interrupt generated 0.5ms after OVF interrupt)
	OCR2A = 0;
	OCR2B = 128;
	//restore sreg (enable interrupts)
	SREG = _sreg;
}


//following code is OVF handler for timer 2
//it is copy-paste from wiring.c and modified for timer2
//variables timer0_overflow_count and timer0_millis are declared in wiring.c



// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

extern volatile unsigned long timer0_overflow_count;
extern volatile unsigned long timer0_millis;
unsigned char timer0_fract = 0;

ISR(TIMER2_OVF_vect)
{
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer0_millis;
	unsigned char f = timer0_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX)
	{
		f -= FRACT_MAX;
		m += 1;
	}

	timer0_fract = f;
	timer0_millis = m;
	timer0_overflow_count++;
}

