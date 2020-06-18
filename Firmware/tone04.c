//tone04.c
// use atmega timer4 as main tone timer instead of timer2
// timer2 is used for System timer.

#include "system_timer.h"
#include "Configuration_prusa.h"

#ifdef SYSTEM_TIMER_2

#include <avr/io.h>
#include <avr/interrupt.h>
#include "pins.h"

#ifndef CRITICAL_SECTION_START
	#define CRITICAL_SECTION_START  unsigned char _sreg = SREG; cli();
	#define CRITICAL_SECTION_END    SREG = _sreg;
#endif //CRITICAL_SECTION_START


#include "fastio.h"

void timer4_init(void)
{
	CRITICAL_SECTION_START;
	
	SET_OUTPUT(BEEPER);
	WRITE(BEEPER, LOW);
	
	SET_OUTPUT(EXTRUDER_0_AUTO_FAN_PIN);
	
	// Set timer mode 9 (PWM,Phase and Frequency Correct)
	// Prescaler is CLK/1024
	// Output compare is disabled on all timer pins
	// Input capture is disabled
	// All interrupts are disabled
	TCCR4A = (1 << WGM40);
	TCCR4B = (1 << WGM43) | (1 << CS42) | (1 << CS40);
	OCR4A = 255;
	OCR4B = 255;
	OCR4C = 255;
	TIMSK4 = 0;
	
	CRITICAL_SECTION_END;
}

#ifdef EXTRUDER_0_AUTO_FAN_PIN
void timer4_set_fan0(uint8_t duty)
{
	if (duty == 0 || duty == 255)
	{
		// We use digital logic if the duty cycle is 0% or 100%
		TCCR4A &= ~(1 << COM4C1);
		OCR4C = 0;
		WRITE(EXTRUDER_0_AUTO_FAN_PIN, duty);
	}
	else
	{
		// Use the timer for fan speed. Enable the timer compare output and set the duty cycle.
		// This function also handles the impossible scenario of a fan speed change during a Tone.
		// Better be safe than sorry.
		CRITICAL_SECTION_START;
		// Enable the PWM output on the fan pin.
		TCCR4A |= (1 << COM4C1);
		OCR4C = (((uint32_t)duty) * ((uint32_t)((TIMSK4 & (1 << OCIE4A))?OCR4A:255))) / ((uint32_t)255);
		CRITICAL_SECTION_END;
	}
}
#endif //EXTRUDER_0_AUTO_FAN_PIN

// Because of the timer mode change, we need two interrupts. We could also try to assume that the frequency is x2
// and use a TOGGLE(), but this seems to work well enough so I left it as it is now.
ISR(TIMER4_COMPA_vect)
{
	WRITE(BEEPER, 1);
}

ISR(TIMER4_OVF_vect)
{
	WRITE(BEEPER, 0);
}

void tone4(__attribute__((unused)) uint8_t _pin, uint16_t frequency)
{
	//this ocr and prescalarbits calculation is taken from the Arduino core and simplified for one type of timer only
	uint8_t prescalarbits = 0b001;
	uint32_t ocr = F_CPU / frequency / 2 - 1;
	
	if (ocr > 0xffff)
	{
		ocr = F_CPU / frequency / 2 / 64 - 1;
		prescalarbits = 0b011;
	}
	
	CRITICAL_SECTION_START;
	// Set calcualted prescaler
	TCCR4B = (TCCR4B & 0b11111000) | prescalarbits;
#ifdef EXTRUDER_0_AUTO_FAN_PIN
	// Scale the fan PWM duty cycle so that it remains constant, but at the tone frequency
	OCR4C = (((uint32_t)OCR4C) * ocr) / (uint32_t)((TIMSK4 & (1 << OCIE4A))?OCR4A:255);
#endif //EXTRUDER_0_AUTO_FAN_PIN
	// Set calcualted ocr
	OCR4A = ocr;
	// Enable Output compare A interrupt and timer overflow interrupt
	TIMSK4 |= (1 << OCIE4A) | (1 << TOIE4);
	CRITICAL_SECTION_END;
}

void noTone4(__attribute__((unused)) uint8_t _pin)
{
	CRITICAL_SECTION_START;
	// Revert prescaler to CLK/1024
	TCCR4B = (TCCR4B & 0b11111000) | (1 << CS42) | (1 << CS40);
#ifdef EXTRUDER_0_AUTO_FAN_PIN
	// Scale the fan OCR back to the original value.
	OCR4C = (((uint32_t)OCR4C) * (uint32_t)255) / (uint32_t)((TIMSK4 & (1 << OCIE4A))?OCR4A:255);
#endif //EXTRUDER_0_AUTO_FAN_PIN
	OCR4A = 255;
	// Disable Output compare A interrupt and timer overflow interrupt
	TIMSK4 &= ~((1 << OCIE4A) | (1 << TOIE4));
	CRITICAL_SECTION_END;
	// Turn beeper off if it was on when noTone was called
	WRITE(BEEPER, 0);
}


#endif //SYSTEM_TIMER_2
