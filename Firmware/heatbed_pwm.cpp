#include <avr/io.h>
#include <avr/interrupt.h>
#include "io_atmega2560.h"

// All this is about silencing the heat bed, as it behaves like a loudspeaker.
// Basically, we want the PWM heating switched at 30Hz (or so) which is a well ballanced
// frequency for both power supply units (i.e. both PSUs are reasonably silent).
// The only trouble is the rising or falling edge of bed heating - that creates an audible click.
// This audible click may be suppressed by making the rising or falling edge NOT sharp.
// Of course, making non-sharp edges in digital technology is not easy, but there is a solution.
// It is possible to do a fast PWM sequence with duty starting from 0 to 255.
// Doing this at higher frequency than the bed "loudspeaker" can handle makes the click barely audible.
// Technically:
// timer0 is set to fast PWM mode at 62.5kHz (timer0 is linked to the bed heating pin) (zero prescaler)
// To keep the bed switching at 30Hz - we don't want the PWM running at 62kHz all the time 
// since it would burn the heatbed's MOSFET:
// 16MHz/256 levels of PWM duty gives us 62.5kHz
// 62.5kHz/256 gives ~244Hz, that is still too fast - 244/8 gives ~30Hz, that's what we need
// So the automaton runs atop of inner 8 (or 16) cycles.
// The finite automaton is running in the ISR(TIMER0_OVF_vect)

///! Definition off finite automaton states
enum class States : uint8_t {
  ZERO = 0,
  RISE = 1,
  ONE = 2,
  FALL = 3
};


///! Inner states of the finite automaton
static States state = States::ZERO;

///! Inner and outer PWM counters
static uint8_t outer = 0;
static uint8_t inner = 0;
static uint8_t pwm = 0;

///! the slow PWM duty for the next 30Hz cycle
///! Set in the whole firmware at various places
extern unsigned char soft_pwm_bed;

/// Fine tuning of automaton cycles
#if 1
static const uint8_t innerMax = 16;
static const uint8_t innerShift = 4;
#else
static const uint8_t innerMax = 8;
static const uint8_t innerShift = 5;
#endif

ISR(TIMER0_OVF_vect)          // timer compare interrupt service routine
{
  if( inner ){
    switch(state){
    case States::ZERO:
      OCR0B = 255;
	  // Commenting the following code saves 6B, but it is left here for reference
	  // It is not necessary to set it all over again, because we can only get into the ZERO state from the FALL state (which sets this register)
//       TCCR0A |= (1 << COM0B1) | (1 << COM0B0);
      break;
    case States::RISE:
      OCR0B = (innerMax - inner) << innerShift;
//       TCCR0A |= (1 << COM0B1); // this bit is always 1
      TCCR0A &= ~(1 << COM0B0);
      break;  
    case States::ONE:
      OCR0B = 255;
	  // again - may be skipped, because we get into the ONE state only from RISE (which sets this register)
//       TCCR0A |= (1 << COM0B1);
       TCCR0A &= ~(1 << COM0B0);
      break;
    case States::FALL:
      OCR0B = (innerMax - inner) << innerShift; // this is the same as in RISE, because now we are setting the zero part of duty due to inverting mode
      // must switch to inverting mode already here, because it takes a whole PWM cycle and it would make a "1" at the end of this pwm cycle
	  TCCR0A |= /*(1 << COM0B1) |*/ (1 << COM0B0); 
      break;
    }
    --inner;
  } else {
    if( ! outer ){ // at the end of 30Hz PWM period
      // synchro is not needed (almost), soft_pwm_bed is just 1 byte, 1-byte write instruction is atomic
      pwm = soft_pwm_bed << 1;
    }
	if( pwm > outer || pwm >= 254 ){
	  // switch ON
      // soft_pwm_bed has a range of 0-127, that why a <<1 is done here. That also means that we may get only up to 254 which we want to be full-time 1 (ON)
      if (state == States::ZERO)
      {
          state = States::RISE;
      }
      else if (state == States::RISE)
      {
          state = States::ONE;
      }
    } else {
      // switch OFF
        if (state == States::ONE)
        {
            state = States::FALL;
        }
        else if (state == States::FALL)
        {
            state = States::ZERO;
        }
    }
    ++outer;
    inner = innerMax;
  }
}
