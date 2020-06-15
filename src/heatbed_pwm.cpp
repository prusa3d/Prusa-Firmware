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

// 2019-08-14 update: the original algorithm worked very well, however there were 2 regressions:
// 1. 62kHz ISR requires considerable amount of processing power, 
//    USB transfer speed dropped by 20%, which was most notable when doing short G-code segments.
// 2. Some users reported TLed PSU started clicking when running at 120V/60Hz. 
//    This looks like the original algorithm didn't maintain base PWM 30Hz, but only 15Hz
// To address both issues, there is an improved approach based on the idea of leveraging
// different CLK prescalers in some automaton states - i.e. when holding LOW or HIGH on the output pin,
// we don't have to clock 62kHz, but we can increase the CLK prescaler for these states to 8 (or even 64).
// That shall result in the ISR not being called that much resulting in regained performance
// Theoretically this is relatively easy, however one must be very carefull handling the AVR's timer
// control registers correctly, especially setting them in a correct order.
// Some registers are double buffered, some changes are applied in next cycles etc.
// The biggest problem was with the CLK prescaler itself - this circuit is shared among almost all timers,
// we don't want to reset the prescaler counted value when transiting among automaton states.
// Resetting the prescaler would make the PWM more precise, right now there are temporal segments
// of variable period ranging from 0 to 7 62kHz ticks - that's logical, the timer must "sync"
// to the new slower CLK after setting the slower prescaler value.
// In our application, this isn't any significant problem and may be ignored.
// Doing changes in timer's registers non-correctly results in artefacts on the output pin
// - it can toggle unnoticed, which will result in bed clicking again.
// That's why there are special transition states ZERO_TO_RISE and ONE_TO_FALL, which enable the
// counter change its operation atomically and without artefacts on the output pin.
// The resulting signal on the output pin was checked with an osciloscope. 
// If there are any change requirements in the future, the signal must be checked with an osciloscope again,
// ad-hoc changes may completely screw things up!

// 2020-01-29 update: we are introducing a new option to the automaton that will allow us to force the output state
// to either full ON or OFF. This is so that interference during the MBL probing is minimal.
// To accomplish this goal we use bedPWMDisabled. It is only supposed to be used for brief periods of time as to
// not make the bed temperature too unstable. Also, careful consideration should be used when using this
// option as leaving this enabled will also keep the bed output in the state it stopped in.

///! Definition off finite automaton states
enum class States : uint8_t {
	ZERO_START = 0,///< entry point of the automaton - reads the soft_pwm_bed value for the next whole PWM cycle
	ZERO,          ///< steady 0 (OFF), no change for the whole period
	ZERO_TO_RISE,  ///< metastate allowing the timer change its state atomically without artefacts on the output pin
	RISE,          ///< 16 fast PWM cycles with increasing duty up to steady ON
	RISE_TO_ONE,   ///< metastate allowing the timer change its state atomically without artefacts on the output pin
	ONE,           ///< steady 1 (ON), no change for the whole period 
	FALL,          ///< 16 fast PWM cycles with decreasing duty down to steady OFF
	FALL_TO_ZERO   ///< metastate allowing the timer change its state atomically without artefacts on the output pin
};

///! Inner states of the finite automaton
static States state = States::ZERO_START;

bool bedPWMDisabled = 0;

///! Fast PWM counter is used in the RISE and FALL states (62.5kHz)
static uint8_t slowCounter = 0;
///! Slow PWM counter is used in the ZERO and ONE states (62.5kHz/8 or 64)
static uint8_t fastCounter = 0;
///! PWM counter for the whole cycle - a cache for soft_pwm_bed
static uint8_t pwm = 0;

///! The slow PWM duty for the next 30Hz cycle
///! Set in the whole firmware at various places
extern unsigned char soft_pwm_bed;

/// fastMax - how many fast PWM steps to do in RISE and FALL states
/// 16 is a good compromise between silenced bed ("smooth" edges)
/// and not burning the switching MOSFET
static const uint8_t fastMax = 16;

/// Scaler 16->256 for fast PWM
static const uint8_t fastShift = 4;

/// Increment slow PWM counter by slowInc every ZERO or ONE state
/// This allows for fine-tuning the basic PWM switching frequency
/// A possible further optimization - use a 64 prescaler (instead of 8)
/// increment slowCounter by 1
/// but use less bits of soft PWM - something like soft_pwm_bed >> 2
/// that may further reduce the CPU cycles required by the bed heating automaton
/// Due to the nature of bed heating the reduced PID precision may not be a major issue, however doing 8x less ISR(timer0_ovf) may significantly improve the performance 
static const uint8_t slowInc = 1;

ISR(TIMER0_OVF_vect)          // timer compare interrupt service routine
{
	switch(state){
	case States::ZERO_START:
		if (bedPWMDisabled) return; // stay in the OFF state and do not change the output pin
		pwm = soft_pwm_bed << 1;// expecting soft_pwm_bed to be 7bit!
		if( pwm != 0 ){
			state = States::ZERO;     // do nothing, let it tick once again after the 30Hz period
		}
		break;
	case States::ZERO: // end of state ZERO - we'll either stay in ZERO or change to RISE
		// In any case update our cache of pwm value for the next whole cycle from soft_pwm_bed
		slowCounter += slowInc; // this does software timer_clk/256 or less (depends on slowInc)
		if( slowCounter > pwm ){
			return;
		} // otherwise moving towards RISE
		state = States::ZERO_TO_RISE; // and finalize the change in a transitional state RISE0
		break;
	// even though it may look like the ZERO state may be glued together with the ZERO_TO_RISE, don't do it
	// the timer must tick once more in order to get rid of occasional output pin toggles.
	case States::ZERO_TO_RISE:  // special state for handling transition between prescalers and switching inverted->non-inverted fast-PWM without toggling the output pin.
		// It must be done in consequent steps, otherwise the pin will get flipped up and down during one PWM cycle.
		// Also beware of the correct sequence of the following timer control registers initialization - it really matters!
		state = States::RISE;     // prepare for standard RISE cycles
		fastCounter = fastMax - 1;// we'll do 16-1 cycles of RISE
		TCNT0 = 255;              // force overflow on the next clock cycle
		TCCR0B = (1 << CS00);     // change prescaler to 1, i.e. 62.5kHz
		TCCR0A &= ~(1 << COM0B0); // Clear OC0B on Compare Match, set OC0B at BOTTOM (non-inverting mode)
		break;
	case States::RISE:
		OCR0B = (fastMax - fastCounter) << fastShift;
		if( fastCounter ){
			--fastCounter;
		} else { // end of RISE cycles, changing into state ONE
			state = States::RISE_TO_ONE;
			OCR0B = 255;          // full duty
			TCNT0 = 254;          // make the timer overflow in the next cycle
			// @@TODO these constants are still subject to investigation
		}
		break;
	case States::RISE_TO_ONE:
		state = States::ONE;
		OCR0B = 255;              // full duty
		TCNT0 = 255;              // make the timer overflow in the next cycle
		TCCR0B = (1 << CS01);     // change prescaler to 8, i.e. 7.8kHz
		break;
	case States::ONE:             // state ONE - we'll either stay in ONE or change to FALL
		OCR0B = 255;
		if (bedPWMDisabled) return; // stay in the ON state and do not change the output pin
		slowCounter += slowInc;   // this does software timer_clk/256 or less
		if( slowCounter < pwm ){
			return;
		}
		if( (soft_pwm_bed << 1) >= (255 - slowInc - 1) ){  //@@TODO simplify & explain
			// if slowInc==2, soft_pwm == 251 will be the first to do short drops to zero. 252 will keep full heating
			return;           // want full duty for the next ONE cycle again - so keep on heating and just wait for the next timer ovf
		}
		// otherwise moving towards FALL
		state = States::ONE;//_TO_FALL;
		state=States::FALL;
		fastCounter = fastMax - 1;// we'll do 16-1 cycles of RISE
		TCNT0 = 255;              // force overflow on the next clock cycle
		TCCR0B = (1 << CS00);     // change prescaler to 1, i.e. 62.5kHz
		// must switch to inverting mode already here, because it takes a whole PWM cycle and it would make a "1" at the end of this pwm cycle
		// COM0B1 remains set both in inverting and non-inverting mode
		TCCR0A |= (1 << COM0B0);  // inverting mode
		break;
	case States::FALL:
		OCR0B = (fastMax - fastCounter) << fastShift; // this is the same as in RISE, because now we are setting the zero part of duty due to inverting mode
		//TCCR0A |= (1 << COM0B0); // already set in ONE_TO_FALL
		if( fastCounter ){
			--fastCounter;
		} else {   // end of FALL cycles, changing into state ZERO
			state = States::FALL_TO_ZERO;
			TCNT0 = 128; //@@TODO again - need to wait long enough to propagate the timer state changes
			OCR0B = 255;
		}
		break;
	case States::FALL_TO_ZERO:
		state = States::ZERO_START; // go to read new soft_pwm_bed value for the next cycle
		TCNT0 = 128;
		OCR0B = 255;
		TCCR0B = (1 << CS01); // change prescaler to 8, i.e. 7.8kHz
		break;		
    }
}
