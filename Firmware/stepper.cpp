/*
  stepper.c - stepper motor driver: executes motion plans using stepper motors
  Part of Grbl

  Copyright (c) 2009-2011 Simen Svale Skogsrud

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

/* The timer calculations of this module informed by the 'RepRap cartesian firmware' by Zack Smith
   and Philipp Tiefenbacher. */

#include "Marlin.h"
#include "stepper.h"
#include "planner.h"
#include "temperature.h"
#include "ultralcd.h"
#include "language.h"
#include "cardreader.h"
#include "speed_lookuptable.h"
#if defined(DIGIPOTSS_PIN) && DIGIPOTSS_PIN > -1
#include <SPI.h>
#endif


//===========================================================================
//=============================public variables  ============================
//===========================================================================
block_t *current_block;  // A pointer to the block currently being traced


//===========================================================================
//=============================private variables ============================
//===========================================================================
//static makes it inpossible to be called from outside of this file by extern.!

// Variables used by The Stepper Driver Interrupt
static unsigned char out_bits;        // The next stepping-bits to be output
static int32_t counter_x,       // Counter variables for the bresenham line tracer
               counter_y,
               counter_z,
               counter_e;
volatile static uint32_t step_events_completed; // The number of step events executed in the current block
static int32_t  acceleration_time, deceleration_time;
//static unsigned long accelerate_until, decelerate_after, acceleration_rate, initial_rate, final_rate, nominal_rate;
static uint16_t acc_step_rate; // needed for deccelaration start point
static uint8_t  step_loops;
static uint16_t OCR1A_nominal;
static uint8_t  step_loops_nominal;

volatile long endstops_trigsteps[3]={0,0,0};
volatile long endstops_stepsTotal,endstops_stepsDone;
static volatile bool endstop_x_hit=false;
static volatile bool endstop_y_hit=false;
static volatile bool endstop_z_hit=false;
#ifdef ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED
bool abort_on_endstop_hit = false;
#endif
#ifdef MOTOR_CURRENT_PWM_XY_PIN
  int motor_current_setting[3] = DEFAULT_PWM_MOTOR_CURRENT;
  int motor_current_setting_silent[3] = DEFAULT_PWM_MOTOR_CURRENT;
  int motor_current_setting_loud[3] = DEFAULT_PWM_MOTOR_CURRENT_LOUD;
#endif

static bool old_x_min_endstop=false;
static bool old_y_min_endstop=false;
static bool old_z_min_endstop=false;

#if defined(X_MAX_PIN) && (X_MAX_PIN > -1)
static bool old_x_max_endstop=false;
#endif
#if defined(Y_MAX_PIN) && (Y_MAX_PIN > -1)
static bool old_y_max_endstop=false;
#endif
#if defined(Z_MAX_PIN) && (Z_MAX_PIN > -1)
static bool old_z_max_endstop=false;
#endif

static bool check_endstops = true;
static bool check_z_endstop = false;

int8_t SilentMode;

volatile long count_position[NUM_AXIS] = { 0, 0, 0, 0};
volatile signed char count_direction[NUM_AXIS] = { 1, 1, 1, 1};

#ifdef LIN_ADVANCE

  uint16_t ADV_NEVER = 65535;

  static uint16_t nextMainISR = 0;
  static uint16_t nextAdvanceISR = ADV_NEVER;
  static uint16_t eISR_Rate = ADV_NEVER;

  static volatile int e_steps; //Extrusion steps to be executed by the stepper
  static int final_estep_rate; //Speed of extruder at cruising speed
  static int current_estep_rate; //The current speed of the extruder
  static int current_adv_steps; //The current pretension of filament expressed in steps

  #define ADV_RATE(T, L) (e_steps ? (T) * (L) / abs(e_steps) : ADV_NEVER)
  #define _NEXT_ISR(T) nextMainISR = T

#else
  #define _NEXT_ISR(T) OCR1A = T    
#endif

//===========================================================================
//=============================functions         ============================
//===========================================================================

#define CHECK_ENDSTOPS  if(check_endstops)

// intRes = intIn1 * intIn2 >> 16
// uses:
// r26 to store 0
// r27 to store the byte 1 of the 24 bit result
#define MultiU16X8toH16(intRes, charIn1, intIn2) \
asm volatile ( \
"clr r26 \n\t" \
"mul %A1, %B2 \n\t" \
"movw %A0, r0 \n\t" \
"mul %A1, %A2 \n\t" \
"add %A0, r1 \n\t" \
"adc %B0, r26 \n\t" \
"lsr r0 \n\t" \
"adc %A0, r26 \n\t" \
"adc %B0, r26 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (intRes) \
: \
"d" (charIn1), \
"d" (intIn2) \
: \
"r26" \
)

// intRes = longIn1 * longIn2 >> 24
// uses:
// r26 to store 0
// r27 to store the byte 1 of the 48bit result
#define MultiU24X24toH16(intRes, longIn1, longIn2) \
asm volatile ( \
"clr r26 \n\t" \
"mul %A1, %B2 \n\t" \
"mov r27, r1 \n\t" \
"mul %B1, %C2 \n\t" \
"movw %A0, r0 \n\t" \
"mul %C1, %C2 \n\t" \
"add %B0, r0 \n\t" \
"mul %C1, %B2 \n\t" \
"add %A0, r0 \n\t" \
"adc %B0, r1 \n\t" \
"mul %A1, %C2 \n\t" \
"add r27, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, r26 \n\t" \
"mul %B1, %B2 \n\t" \
"add r27, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, r26 \n\t" \
"mul %C1, %A2 \n\t" \
"add r27, r0 \n\t" \
"adc %A0, r1 \n\t" \
"adc %B0, r26 \n\t" \
"mul %B1, %A2 \n\t" \
"add r27, r1 \n\t" \
"adc %A0, r26 \n\t" \
"adc %B0, r26 \n\t" \
"lsr r27 \n\t" \
"adc %A0, r26 \n\t" \
"adc %B0, r26 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (intRes) \
: \
"d" (longIn1), \
"d" (longIn2) \
: \
"r26" , "r27" \
)

// Some useful constants

#define ENABLE_STEPPER_DRIVER_INTERRUPT()  TIMSK1 |= (1<<OCIE1A)
#define DISABLE_STEPPER_DRIVER_INTERRUPT() TIMSK1 &= ~(1<<OCIE1A)


void checkHitEndstops()
{
 if( endstop_x_hit || endstop_y_hit || endstop_z_hit) {
   SERIAL_ECHO_START;
   SERIAL_ECHORPGM(MSG_ENDSTOPS_HIT);
   if(endstop_x_hit) {
     SERIAL_ECHOPAIR(" X:",(float)endstops_trigsteps[X_AXIS]/axis_steps_per_unit[X_AXIS]);
     LCD_MESSAGERPGM(CAT2(MSG_ENDSTOPS_HIT, PSTR("X")));
   }
   if(endstop_y_hit) {
     SERIAL_ECHOPAIR(" Y:",(float)endstops_trigsteps[Y_AXIS]/axis_steps_per_unit[Y_AXIS]);
     LCD_MESSAGERPGM(CAT2(MSG_ENDSTOPS_HIT, PSTR("Y")));
   }
   if(endstop_z_hit) {
     SERIAL_ECHOPAIR(" Z:",(float)endstops_trigsteps[Z_AXIS]/axis_steps_per_unit[Z_AXIS]);
     LCD_MESSAGERPGM(CAT2(MSG_ENDSTOPS_HIT,PSTR("Z")));
   }
   SERIAL_ECHOLN("");
   endstop_x_hit=false;
   endstop_y_hit=false;
   endstop_z_hit=false;
#if defined(ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED) && defined(SDSUPPORT)
   if (abort_on_endstop_hit)
   {
     card.sdprinting = false;
     card.closefile();
     quickStop();
     setTargetHotend0(0);
     setTargetHotend1(0);
     setTargetHotend2(0);
   }
#endif
 }
}

bool endstops_hit_on_purpose()
{
  bool hit = endstop_x_hit || endstop_y_hit || endstop_z_hit;
  endstop_x_hit=false;
  endstop_y_hit=false;
  endstop_z_hit=false;
  return hit;
}

bool endstop_z_hit_on_purpose()
{
  bool hit = endstop_z_hit;
  endstop_z_hit=false;
  return hit;
}

bool enable_endstops(bool check)
{
  bool old = check_endstops;
  check_endstops = check;
  return old;
}

bool enable_z_endstop(bool check)
{
  bool old = check_z_endstop;
  check_z_endstop = check;
  endstop_z_hit=false;
  return old;
}

//         __________________________
//        /|                        |\     _________________         ^
//       / |                        | \   /|               |\        |
//      /  |                        |  \ / |               | \       s
//     /   |                        |   |  |               |  \      p
//    /    |                        |   |  |               |   \     e
//   +-----+------------------------+---+--+---------------+----+    e
//   |               BLOCK 1            |      BLOCK 2          |    d
//
//                           time ----->
//
//  The trapezoid is the shape the speed curve over time. It starts at block->initial_rate, accelerates
//  first block->accelerate_until step_events_completed, then keeps going at constant speed until
//  step_events_completed reaches block->decelerate_after after which it decelerates until the trapezoid generator is reset.
//  The slope of acceleration is calculated with the leib ramp alghorithm.

void st_wake_up() {
  //  TCNT1 = 0;
  ENABLE_STEPPER_DRIVER_INTERRUPT();
}

void step_wait(){
    for(int8_t i=0; i < 6; i++){
    }
}


FORCE_INLINE unsigned short calc_timer(unsigned short step_rate) {
  unsigned short timer;
  if(step_rate > MAX_STEP_FREQUENCY) step_rate = MAX_STEP_FREQUENCY;

  if(step_rate > 20000) { // If steprate > 20kHz >> step 4 times
    step_rate = (step_rate >> 2)&0x3fff;
    step_loops = 4;
  }
  else if(step_rate > 10000) { // If steprate > 10kHz >> step 2 times
    step_rate = (step_rate >> 1)&0x7fff;
    step_loops = 2;
  }
  else {
    step_loops = 1;
  }

  if(step_rate < (F_CPU/500000)) step_rate = (F_CPU/500000);
  step_rate -= (F_CPU/500000); // Correct for minimal speed
  if(step_rate >= (8*256)){ // higher step rate
    unsigned short table_address = (unsigned short)&speed_lookuptable_fast[(unsigned char)(step_rate>>8)][0];
    unsigned char tmp_step_rate = (step_rate & 0x00ff);
    unsigned short gain = (unsigned short)pgm_read_word_near(table_address+2);
    MultiU16X8toH16(timer, tmp_step_rate, gain);
    timer = (unsigned short)pgm_read_word_near(table_address) - timer;
  }
  else { // lower step rates
    unsigned short table_address = (unsigned short)&speed_lookuptable_slow[0][0];
    table_address += ((step_rate)>>1) & 0xfffc;
    timer = (unsigned short)pgm_read_word_near(table_address);
    timer -= (((unsigned short)pgm_read_word_near(table_address+2) * (unsigned char)(step_rate & 0x0007))>>3);
  }
  if(timer < 100) { timer = 100; MYSERIAL.print(MSG_STEPPER_TOO_HIGH); MYSERIAL.println(step_rate); }//(20kHz this should never happen)
  return timer;
}

// Initializes the trapezoid generator from the current block. Called whenever a new
// block begins.
FORCE_INLINE void trapezoid_generator_reset() {
  deceleration_time = 0;
  // step_rate to timer interval
  OCR1A_nominal = calc_timer(current_block->nominal_rate);
  // make a note of the number of step loops required at nominal speed
  step_loops_nominal = step_loops;
  acc_step_rate = current_block->initial_rate;
  acceleration_time = calc_timer(acc_step_rate);
  _NEXT_ISR(acceleration_time);
  
  #ifdef LIN_ADVANCE
    if (current_block->use_advance_lead) {
      current_estep_rate = ((unsigned long)acc_step_rate * current_block->abs_adv_steps_multiplier8) >> 17;
      final_estep_rate = (current_block->nominal_rate * current_block->abs_adv_steps_multiplier8) >> 17;
    }
  #endif
}

// "The Stepper Driver Interrupt" - This timer interrupt is the workhorse.
// It pops blocks from the block_buffer and executes them by pulsing the stepper pins appropriately.
ISR(TIMER1_COMPA_vect) {
  #ifdef LIN_ADVANCE
    advance_isr_scheduler();
  #else
    isr();
  #endif
}

void isr() {
  // If there is no current block, attempt to pop one from the buffer
  if (current_block == NULL) {
    // Anything in the buffer?
    current_block = plan_get_current_block();
    if (current_block != NULL) {
      // The busy flag is set by the plan_get_current_block() call.
      // current_block->busy = true;
      trapezoid_generator_reset();
      counter_x = -(current_block->step_event_count >> 1);
      counter_y = counter_x;
      counter_z = counter_x;
      counter_e = counter_x;
      step_events_completed = 0;

      #ifdef Z_LATE_ENABLE
        if(current_block->steps_z > 0) {
          enable_z();
          _NEXT_ISR(2000); //1ms wait
          return;
        }
      #endif
    }
    else {
        _NEXT_ISR(2000); // 1kHz.
    }
  }

  if (current_block != NULL) {
    // Set directions TO DO This should be done once during init of trapezoid. Endstops -> interrupt
    out_bits = current_block->direction_bits;


    // Set the direction bits (X_AXIS=A_AXIS and Y_AXIS=B_AXIS for COREXY)
    if((out_bits & (1<<X_AXIS))!=0){
        WRITE(X_DIR_PIN, INVERT_X_DIR);
      count_direction[X_AXIS]=-1;
    }
    else{
        WRITE(X_DIR_PIN, !INVERT_X_DIR);
      count_direction[X_AXIS]=1;
    }
    if((out_bits & (1<<Y_AXIS))!=0){
      WRITE(Y_DIR_PIN, INVERT_Y_DIR);
	  
	  #ifdef Y_DUAL_STEPPER_DRIVERS
	    WRITE(Y2_DIR_PIN, !(INVERT_Y_DIR == INVERT_Y2_VS_Y_DIR));
	  #endif
	  
      count_direction[Y_AXIS]=-1;
    }
    else{
      WRITE(Y_DIR_PIN, !INVERT_Y_DIR);
	  
	  #ifdef Y_DUAL_STEPPER_DRIVERS
	    WRITE(Y2_DIR_PIN, (INVERT_Y_DIR == INVERT_Y2_VS_Y_DIR));
	  #endif
	  
      count_direction[Y_AXIS]=1;
    }

    // Set direction en check limit switches
    #ifndef COREXY
    if ((out_bits & (1<<X_AXIS)) != 0) {   // stepping along -X axis
    #else
    if ((((out_bits & (1<<X_AXIS)) != 0)&&(out_bits & (1<<Y_AXIS)) != 0)) {   //-X occurs for -A and -B
    #endif
      CHECK_ENDSTOPS
      {
        {
          #if defined(X_MIN_PIN) && (X_MIN_PIN > -1) && !defined(DEBUG_DISABLE_XMINLIMIT)
            bool x_min_endstop=(READ(X_MIN_PIN) != X_MIN_ENDSTOP_INVERTING);
            if(x_min_endstop && old_x_min_endstop && (current_block->steps_x > 0)) {
              endstops_trigsteps[X_AXIS] = count_position[X_AXIS];
              endstop_x_hit=true;
              step_events_completed = current_block->step_event_count;
            }
            old_x_min_endstop = x_min_endstop;
          #endif
        }
      }
    }
    else { // +direction
      CHECK_ENDSTOPS
      {
        {
          #if defined(X_MAX_PIN) && (X_MAX_PIN > -1) && !defined(DEBUG_DISABLE_XMAXLIMIT)
            bool x_max_endstop=(READ(X_MAX_PIN) != X_MAX_ENDSTOP_INVERTING);
            if(x_max_endstop && old_x_max_endstop && (current_block->steps_x > 0)){
              endstops_trigsteps[X_AXIS] = count_position[X_AXIS];
              endstop_x_hit=true;
              step_events_completed = current_block->step_event_count;
            }
            old_x_max_endstop = x_max_endstop;
          #endif
        }
      }
    }

    #ifndef COREXY
    if ((out_bits & (1<<Y_AXIS)) != 0) {   // -direction
    #else
    if ((((out_bits & (1<<X_AXIS)) != 0)&&(out_bits & (1<<Y_AXIS)) == 0)) {   // -Y occurs for -A and +B
    #endif
      CHECK_ENDSTOPS
      {
        #if defined(Y_MIN_PIN) && (Y_MIN_PIN > -1) && !defined(DEBUG_DISABLE_YMINLIMIT)
          bool y_min_endstop=(READ(Y_MIN_PIN) != Y_MIN_ENDSTOP_INVERTING);
          if(y_min_endstop && old_y_min_endstop && (current_block->steps_y > 0)) {
            endstops_trigsteps[Y_AXIS] = count_position[Y_AXIS];
            endstop_y_hit=true;
            step_events_completed = current_block->step_event_count;
          }
          old_y_min_endstop = y_min_endstop;
        #endif
      }
    }
    else { // +direction
      CHECK_ENDSTOPS
      {
        #if defined(Y_MAX_PIN) && (Y_MAX_PIN > -1) && !defined(DEBUG_DISABLE_YMAXLIMIT)
          bool y_max_endstop=(READ(Y_MAX_PIN) != Y_MAX_ENDSTOP_INVERTING);
          if(y_max_endstop && old_y_max_endstop && (current_block->steps_y > 0)){
            endstops_trigsteps[Y_AXIS] = count_position[Y_AXIS];
            endstop_y_hit=true;
            step_events_completed = current_block->step_event_count;
          }
          old_y_max_endstop = y_max_endstop;
        #endif
      }
    }

    if ((out_bits & (1<<Z_AXIS)) != 0) {   // -direction
      WRITE(Z_DIR_PIN,INVERT_Z_DIR);
      
      #ifdef Z_DUAL_STEPPER_DRIVERS
        WRITE(Z2_DIR_PIN,INVERT_Z_DIR);
      #endif

      count_direction[Z_AXIS]=-1;
      if(check_endstops && ! check_z_endstop)
      {
        #if defined(Z_MIN_PIN) && (Z_MIN_PIN > -1) && !defined(DEBUG_DISABLE_ZMINLIMIT)
          bool z_min_endstop=(READ(Z_MIN_PIN) != Z_MIN_ENDSTOP_INVERTING);
          if(z_min_endstop && old_z_min_endstop && (current_block->steps_z > 0)) {
            endstops_trigsteps[Z_AXIS] = count_position[Z_AXIS];
            endstop_z_hit=true;
            step_events_completed = current_block->step_event_count;
          }
          old_z_min_endstop = z_min_endstop;
        #endif
      }
    }
    else { // +direction
      WRITE(Z_DIR_PIN,!INVERT_Z_DIR);

      #ifdef Z_DUAL_STEPPER_DRIVERS
        WRITE(Z2_DIR_PIN,!INVERT_Z_DIR);
      #endif

      count_direction[Z_AXIS]=1;
      CHECK_ENDSTOPS
      {
        #if defined(Z_MAX_PIN) && (Z_MAX_PIN > -1) && !defined(DEBUG_DISABLE_ZMAXLIMIT)
          bool z_max_endstop=(READ(Z_MAX_PIN) != Z_MAX_ENDSTOP_INVERTING);
          if(z_max_endstop && old_z_max_endstop && (current_block->steps_z > 0)) {
            endstops_trigsteps[Z_AXIS] = count_position[Z_AXIS];
            endstop_z_hit=true;
            step_events_completed = current_block->step_event_count;
          }
          old_z_max_endstop = z_max_endstop;
        #endif
      }
    }

    // Supporting stopping on a trigger of the Z-stop induction sensor, not only for the Z-minus movements.
    #if defined(Z_MIN_PIN) && (Z_MIN_PIN > -1) && !defined(DEBUG_DISABLE_ZMINLIMIT)
    if(check_z_endstop) {
        // Check the Z min end-stop no matter what.
        // Good for searching for the center of an induction target.
        bool z_min_endstop=(READ(Z_MIN_PIN) != Z_MIN_ENDSTOP_INVERTING);
        if(z_min_endstop && old_z_min_endstop) {
          endstops_trigsteps[Z_AXIS] = count_position[Z_AXIS];
          endstop_z_hit=true;
          step_events_completed = current_block->step_event_count;
        }
        old_z_min_endstop = z_min_endstop;
    }
    #endif

	if ((out_bits & (1 << E_AXIS)) != 0)
	{	// -direction
		//AKU
#ifdef SNMM
		if (snmm_extruder == 0 || snmm_extruder == 2)
		{
			NORM_E_DIR();
		}
		else
		{
			REV_E_DIR();
		}
#else
		REV_E_DIR();
#endif // SNMM
		count_direction[E_AXIS] = -1;
	}
	else
	{	// +direction
#ifdef SNMM
		if (snmm_extruder == 0 || snmm_extruder == 2)
		{
			REV_E_DIR();
		}
		else
		{
			NORM_E_DIR();
		}
#else
		NORM_E_DIR();
#endif // SNMM
		count_direction[E_AXIS] = 1;
	}

    for(uint8_t i=0; i < step_loops; i++) { // Take multiple steps per interrupt (For high speed moves)
      #ifndef AT90USB
      MSerial.checkRx(); // Check for serial chars.
      #endif
      
      #ifdef LIN_ADVANCE
        counter_e += current_block->steps_e;
        if (counter_e > 0) {
          counter_e -= current_block->step_event_count;
          count_position[E_AXIS] += count_direction[E_AXIS];
          ((out_bits&(1<<E_AXIS))!=0) ? --e_steps : ++e_steps;
        }
      #endif

        counter_x += current_block->steps_x;
        if (counter_x > 0) {
          WRITE(X_STEP_PIN, !INVERT_X_STEP_PIN);
          counter_x -= current_block->step_event_count;
          count_position[X_AXIS]+=count_direction[X_AXIS];   
          WRITE(X_STEP_PIN, INVERT_X_STEP_PIN);
        }

        counter_y += current_block->steps_y;
        if (counter_y > 0) {
          WRITE(Y_STEP_PIN, !INVERT_Y_STEP_PIN);
		  
		  #ifdef Y_DUAL_STEPPER_DRIVERS
			WRITE(Y2_STEP_PIN, !INVERT_Y_STEP_PIN);
		  #endif
		  
          counter_y -= current_block->step_event_count;
          count_position[Y_AXIS]+=count_direction[Y_AXIS];
          WRITE(Y_STEP_PIN, INVERT_Y_STEP_PIN);
		  
		  #ifdef Y_DUAL_STEPPER_DRIVERS
			WRITE(Y2_STEP_PIN, INVERT_Y_STEP_PIN);
		  #endif
        }

      counter_z += current_block->steps_z;
      if (counter_z > 0) {
        WRITE(Z_STEP_PIN, !INVERT_Z_STEP_PIN);
        
        #ifdef Z_DUAL_STEPPER_DRIVERS
          WRITE(Z2_STEP_PIN, !INVERT_Z_STEP_PIN);
        #endif

        counter_z -= current_block->step_event_count;
        count_position[Z_AXIS]+=count_direction[Z_AXIS];
        WRITE(Z_STEP_PIN, INVERT_Z_STEP_PIN);
        
        #ifdef Z_DUAL_STEPPER_DRIVERS
          WRITE(Z2_STEP_PIN, INVERT_Z_STEP_PIN);
        #endif
      }

        #ifndef LIN_ADVANCE
        counter_e += current_block->steps_e;
        if (counter_e > 0) {
          WRITE_E_STEP(!INVERT_E_STEP_PIN);
          counter_e -= current_block->step_event_count;
          count_position[E_AXIS]+=count_direction[E_AXIS];
          WRITE_E_STEP(INVERT_E_STEP_PIN);
        }
        #endif
      step_events_completed += 1;
      if(step_events_completed >= current_block->step_event_count) break;
    }
    
    #ifdef LIN_ADVANCE
      if (current_block->use_advance_lead) {
        const int delta_adv_steps = current_estep_rate - current_adv_steps;
        current_adv_steps += delta_adv_steps;
        e_steps += delta_adv_steps;
      }
      // If we have esteps to execute, fire the next advance_isr "now"
      if (e_steps) nextAdvanceISR = 0;
    #endif
  
    // Calculare new timer value
    unsigned short timer;
    unsigned short step_rate;
    if (step_events_completed <= (unsigned long int)current_block->accelerate_until) {
      // v = t * a   ->   acc_step_rate = acceleration_time * current_block->acceleration_rate
      MultiU24X24toH16(acc_step_rate, acceleration_time, current_block->acceleration_rate);
      acc_step_rate += current_block->initial_rate;

      // upper limit
      if(acc_step_rate > current_block->nominal_rate)
        acc_step_rate = current_block->nominal_rate;

      // step_rate to timer interval
      timer = calc_timer(acc_step_rate);
      _NEXT_ISR(timer);
      acceleration_time += timer;
      
      #ifdef LIN_ADVANCE
        if (current_block->use_advance_lead) {
          current_estep_rate = ((uint32_t)acc_step_rate * current_block->abs_adv_steps_multiplier8) >> 17;
        }
        eISR_Rate = ADV_RATE(timer, step_loops);
      #endif
    }
    else if (step_events_completed > (unsigned long int)current_block->decelerate_after) {
      MultiU24X24toH16(step_rate, deceleration_time, current_block->acceleration_rate);

      if(step_rate > acc_step_rate) { // Check step_rate stays positive
        step_rate = current_block->final_rate;
      }
      else {
        step_rate = acc_step_rate - step_rate; // Decelerate from aceleration end point.
      }

      // lower limit
      if(step_rate < current_block->final_rate)
        step_rate = current_block->final_rate;

      // step_rate to timer interval
      timer = calc_timer(step_rate);
      _NEXT_ISR(timer);
      deceleration_time += timer;
      
      #ifdef LIN_ADVANCE
        if (current_block->use_advance_lead) {
          current_estep_rate = ((uint32_t)step_rate * current_block->abs_adv_steps_multiplier8) >> 17;
        }
        eISR_Rate = ADV_RATE(timer, step_loops);
      #endif
    }
    else {
      #ifdef LIN_ADVANCE
        if (current_block->use_advance_lead)
          current_estep_rate = final_estep_rate;

        eISR_Rate = ADV_RATE(OCR1A_nominal, step_loops_nominal);
      #endif
      
      _NEXT_ISR(OCR1A_nominal);
      // ensure we're running at the correct step rate, even if we just came off an acceleration
      step_loops = step_loops_nominal;
    }

    // If current block is finished, reset pointer
    if (step_events_completed >= current_block->step_event_count) {
      current_block = NULL;
      plan_discard_current_block();
    }
  }
}

#ifdef LIN_ADVANCE

  // Timer interrupt for E. e_steps is set in the main routine.

  void advance_isr() {

    if (e_steps) {
      bool dir =
      #ifdef SNMM
        ((e_steps < 0) == (snmm_extruder & 1))
      #else
        (e_steps < 0)
      #endif
      ? INVERT_E0_DIR : !INVERT_E0_DIR; //If we have SNMM, reverse every second extruder.
      WRITE(E0_DIR_PIN, dir);
  
      for (uint8_t i = step_loops; e_steps && i--;) {
        WRITE(E0_STEP_PIN, !INVERT_E_STEP_PIN);
        e_steps < 0 ? ++e_steps : --e_steps;
        WRITE(E0_STEP_PIN, INVERT_E_STEP_PIN);
      }
	}
	else{
		eISR_Rate = ADV_NEVER;
	}
	nextAdvanceISR = eISR_Rate;
  }

  void advance_isr_scheduler() {
    // Run main stepping ISR if flagged
    if (!nextMainISR) isr();

    // Run Advance stepping ISR if flagged
    if (!nextAdvanceISR) advance_isr();

    // Is the next advance ISR scheduled before the next main ISR?
    if (nextAdvanceISR <= nextMainISR) {
      // Set up the next interrupt
      OCR1A = nextAdvanceISR;
      // New interval for the next main ISR
      if (nextMainISR) nextMainISR -= nextAdvanceISR;
      // Will call Stepper::advance_isr on the next interrupt
      nextAdvanceISR = 0;
    }
    else {
      // The next main ISR comes first
      OCR1A = nextMainISR;
      // New interval for the next advance ISR, if any
      if (nextAdvanceISR && nextAdvanceISR != ADV_NEVER)
        nextAdvanceISR -= nextMainISR;
      // Will call Stepper::isr on the next interrupt
      nextMainISR = 0;
    }

    // Don't run the ISR faster than possible
    if (OCR1A < TCNT1 + 16) OCR1A = TCNT1 + 16;
  }
  
  void clear_current_adv_vars() {
    e_steps = 0; //Should be already 0 at an filament change event, but just to be sure..
    current_adv_steps = 0;
  }

#endif // LIN_ADVANCE

void st_init()
{
  digipot_init(); //Initialize Digipot Motor Current
  microstep_init(); //Initialize Microstepping Pins

  //Initialize Dir Pins
  #if defined(X_DIR_PIN) && X_DIR_PIN > -1
    SET_OUTPUT(X_DIR_PIN);
  #endif
  #if defined(X2_DIR_PIN) && X2_DIR_PIN > -1
    SET_OUTPUT(X2_DIR_PIN);
  #endif
  #if defined(Y_DIR_PIN) && Y_DIR_PIN > -1
    SET_OUTPUT(Y_DIR_PIN);
		
	#if defined(Y_DUAL_STEPPER_DRIVERS) && defined(Y2_DIR_PIN) && (Y2_DIR_PIN > -1)
	  SET_OUTPUT(Y2_DIR_PIN);
	#endif
  #endif
  #if defined(Z_DIR_PIN) && Z_DIR_PIN > -1
    SET_OUTPUT(Z_DIR_PIN);

    #if defined(Z_DUAL_STEPPER_DRIVERS) && defined(Z2_DIR_PIN) && (Z2_DIR_PIN > -1)
      SET_OUTPUT(Z2_DIR_PIN);
    #endif
  #endif
  #if defined(E0_DIR_PIN) && E0_DIR_PIN > -1
    SET_OUTPUT(E0_DIR_PIN);
  #endif
  #if defined(E1_DIR_PIN) && (E1_DIR_PIN > -1)
    SET_OUTPUT(E1_DIR_PIN);
  #endif
  #if defined(E2_DIR_PIN) && (E2_DIR_PIN > -1)
    SET_OUTPUT(E2_DIR_PIN);
  #endif

  //Initialize Enable Pins - steppers default to disabled.

  #if defined(X_ENABLE_PIN) && X_ENABLE_PIN > -1
    SET_OUTPUT(X_ENABLE_PIN);
    if(!X_ENABLE_ON) WRITE(X_ENABLE_PIN,HIGH);
  #endif
  #if defined(X2_ENABLE_PIN) && X2_ENABLE_PIN > -1
    SET_OUTPUT(X2_ENABLE_PIN);
    if(!X_ENABLE_ON) WRITE(X2_ENABLE_PIN,HIGH);
  #endif
  #if defined(Y_ENABLE_PIN) && Y_ENABLE_PIN > -1
    SET_OUTPUT(Y_ENABLE_PIN);
    if(!Y_ENABLE_ON) WRITE(Y_ENABLE_PIN,HIGH);
	
	#if defined(Y_DUAL_STEPPER_DRIVERS) && defined(Y2_ENABLE_PIN) && (Y2_ENABLE_PIN > -1)
	  SET_OUTPUT(Y2_ENABLE_PIN);
	  if(!Y_ENABLE_ON) WRITE(Y2_ENABLE_PIN,HIGH);
	#endif
  #endif
  #if defined(Z_ENABLE_PIN) && Z_ENABLE_PIN > -1
    SET_OUTPUT(Z_ENABLE_PIN);
    if(!Z_ENABLE_ON) WRITE(Z_ENABLE_PIN,HIGH);

    #if defined(Z_DUAL_STEPPER_DRIVERS) && defined(Z2_ENABLE_PIN) && (Z2_ENABLE_PIN > -1)
      SET_OUTPUT(Z2_ENABLE_PIN);
      if(!Z_ENABLE_ON) WRITE(Z2_ENABLE_PIN,HIGH);
    #endif
  #endif
  #if defined(E0_ENABLE_PIN) && (E0_ENABLE_PIN > -1)
    SET_OUTPUT(E0_ENABLE_PIN);
    if(!E_ENABLE_ON) WRITE(E0_ENABLE_PIN,HIGH);
  #endif
  #if defined(E1_ENABLE_PIN) && (E1_ENABLE_PIN > -1)
    SET_OUTPUT(E1_ENABLE_PIN);
    if(!E_ENABLE_ON) WRITE(E1_ENABLE_PIN,HIGH);
  #endif
  #if defined(E2_ENABLE_PIN) && (E2_ENABLE_PIN > -1)
    SET_OUTPUT(E2_ENABLE_PIN);
    if(!E_ENABLE_ON) WRITE(E2_ENABLE_PIN,HIGH);
  #endif

  //endstops and pullups

  #if defined(X_MIN_PIN) && X_MIN_PIN > -1
    SET_INPUT(X_MIN_PIN);
    #ifdef ENDSTOPPULLUP_XMIN
      WRITE(X_MIN_PIN,HIGH);
    #endif
  #endif

  #if defined(Y_MIN_PIN) && Y_MIN_PIN > -1
    SET_INPUT(Y_MIN_PIN);
    #ifdef ENDSTOPPULLUP_YMIN
      WRITE(Y_MIN_PIN,HIGH);
    #endif
  #endif

  #if defined(Z_MIN_PIN) && Z_MIN_PIN > -1
    SET_INPUT(Z_MIN_PIN);
    #ifdef ENDSTOPPULLUP_ZMIN
      WRITE(Z_MIN_PIN,HIGH);
    #endif
  #endif

  #if defined(X_MAX_PIN) && X_MAX_PIN > -1
    SET_INPUT(X_MAX_PIN);
    #ifdef ENDSTOPPULLUP_XMAX
      WRITE(X_MAX_PIN,HIGH);
    #endif
  #endif

  #if defined(Y_MAX_PIN) && Y_MAX_PIN > -1
    SET_INPUT(Y_MAX_PIN);
    #ifdef ENDSTOPPULLUP_YMAX
      WRITE(Y_MAX_PIN,HIGH);
    #endif
  #endif

  #if defined(Z_MAX_PIN) && Z_MAX_PIN > -1
    SET_INPUT(Z_MAX_PIN);
    #ifdef ENDSTOPPULLUP_ZMAX
      WRITE(Z_MAX_PIN,HIGH);
    #endif
  #endif


  //Initialize Step Pins
  #if defined(X_STEP_PIN) && (X_STEP_PIN > -1)
    SET_OUTPUT(X_STEP_PIN);
    WRITE(X_STEP_PIN,INVERT_X_STEP_PIN);
    disable_x();
  #endif
  #if defined(X2_STEP_PIN) && (X2_STEP_PIN > -1)
    SET_OUTPUT(X2_STEP_PIN);
    WRITE(X2_STEP_PIN,INVERT_X_STEP_PIN);
    disable_x();
  #endif
  #if defined(Y_STEP_PIN) && (Y_STEP_PIN > -1)
    SET_OUTPUT(Y_STEP_PIN);
    WRITE(Y_STEP_PIN,INVERT_Y_STEP_PIN);
    #if defined(Y_DUAL_STEPPER_DRIVERS) && defined(Y2_STEP_PIN) && (Y2_STEP_PIN > -1)
      SET_OUTPUT(Y2_STEP_PIN);
      WRITE(Y2_STEP_PIN,INVERT_Y_STEP_PIN);
    #endif
    disable_y();
  #endif
  #if defined(Z_STEP_PIN) && (Z_STEP_PIN > -1)
    SET_OUTPUT(Z_STEP_PIN);
    WRITE(Z_STEP_PIN,INVERT_Z_STEP_PIN);
    #if defined(Z_DUAL_STEPPER_DRIVERS) && defined(Z2_STEP_PIN) && (Z2_STEP_PIN > -1)
      SET_OUTPUT(Z2_STEP_PIN);
      WRITE(Z2_STEP_PIN,INVERT_Z_STEP_PIN);
    #endif
    disable_z();
  #endif
  #if defined(E0_STEP_PIN) && (E0_STEP_PIN > -1)
    SET_OUTPUT(E0_STEP_PIN);
    WRITE(E0_STEP_PIN,INVERT_E_STEP_PIN);
    disable_e0();
  #endif
  #if defined(E1_STEP_PIN) && (E1_STEP_PIN > -1)
    SET_OUTPUT(E1_STEP_PIN);
    WRITE(E1_STEP_PIN,INVERT_E_STEP_PIN);
    disable_e1();
  #endif
  #if defined(E2_STEP_PIN) && (E2_STEP_PIN > -1)
    SET_OUTPUT(E2_STEP_PIN);
    WRITE(E2_STEP_PIN,INVERT_E_STEP_PIN);
    disable_e2();
  #endif

  // waveform generation = 0100 = CTC
  TCCR1B &= ~(1<<WGM13);
  TCCR1B |=  (1<<WGM12);
  TCCR1A &= ~(1<<WGM11);
  TCCR1A &= ~(1<<WGM10);

  // output mode = 00 (disconnected)
  TCCR1A &= ~(3<<COM1A0);
  TCCR1A &= ~(3<<COM1B0);

  // Set the timer pre-scaler
  // Generally we use a divider of 8, resulting in a 2MHz timer
  // frequency on a 16MHz MCU. If you are going to change this, be
  // sure to regenerate speed_lookuptable.h with
  // create_speed_lookuptable.py
  TCCR1B = (TCCR1B & ~(0x07<<CS10)) | (2<<CS10);

  OCR1A = 0x4000;
  TCNT1 = 0;
  ENABLE_STEPPER_DRIVER_INTERRUPT();
  
  #ifdef LIN_ADVANCE
    e_steps = 0;
    current_adv_steps = 0;
  #endif

  enable_endstops(true); // Start with endstops active. After homing they can be disabled
  sei();
}


// Block until all buffered steps are executed
void st_synchronize()
{
    while( blocks_queued()) {
    manage_heater();
    // Vojtech: Don't disable motors inside the planner!
    manage_inactivity(true);
    lcd_update();
  }
}

void st_set_position(const long &x, const long &y, const long &z, const long &e)
{
  CRITICAL_SECTION_START;
  count_position[X_AXIS] = x;
  count_position[Y_AXIS] = y;
  count_position[Z_AXIS] = z;
  count_position[E_AXIS] = e;
  CRITICAL_SECTION_END;
}

void st_set_e_position(const long &e)
{
  CRITICAL_SECTION_START;
  count_position[E_AXIS] = e;
  CRITICAL_SECTION_END;
}

long st_get_position(uint8_t axis)
{
  long count_pos;
  CRITICAL_SECTION_START;
  count_pos = count_position[axis];
  CRITICAL_SECTION_END;
  return count_pos;
}


float st_get_position_mm(uint8_t axis)
{
  float steper_position_in_steps = st_get_position(axis);
  return steper_position_in_steps / axis_steps_per_unit[axis];
}


void finishAndDisableSteppers()
{
  st_synchronize();
  disable_x();
  disable_y();
  disable_z();
  disable_e0();
  disable_e1();
  disable_e2();
}

void quickStop()
{
  DISABLE_STEPPER_DRIVER_INTERRUPT();
  while (blocks_queued()) plan_discard_current_block(); 
  current_block = NULL;
  ENABLE_STEPPER_DRIVER_INTERRUPT();
}

#ifdef BABYSTEPPING


void babystep(const uint8_t axis,const bool direction)
{
  //MUST ONLY BE CALLED BY A ISR, it depends on that no other ISR interrupts this
    //store initial pin states
  switch(axis)
  {
  case X_AXIS:
  {
    enable_x();   
    uint8_t old_x_dir_pin= READ(X_DIR_PIN);  //if dualzstepper, both point to same direction.
   
    //setup new step
    WRITE(X_DIR_PIN,(INVERT_X_DIR)^direction);
    
    //perform step 
    WRITE(X_STEP_PIN, !INVERT_X_STEP_PIN); 
    delayMicroseconds(3);
    WRITE(X_STEP_PIN, INVERT_X_STEP_PIN);

    //get old pin state back.
    WRITE(X_DIR_PIN,old_x_dir_pin);
  }
  break;
  case Y_AXIS:
  {
    enable_y();   
    uint8_t old_y_dir_pin= READ(Y_DIR_PIN);  //if dualzstepper, both point to same direction.
   
    //setup new step
    WRITE(Y_DIR_PIN,(INVERT_Y_DIR)^direction);
    
    //perform step 
    WRITE(Y_STEP_PIN, !INVERT_Y_STEP_PIN); 
    delayMicroseconds(3);
    WRITE(Y_STEP_PIN, INVERT_Y_STEP_PIN);

    //get old pin state back.
    WRITE(Y_DIR_PIN,old_y_dir_pin);

  }
  break;
 
  case Z_AXIS:
  {
    enable_z();
    uint8_t old_z_dir_pin= READ(Z_DIR_PIN);  //if dualzstepper, both point to same direction.
    //setup new step
    WRITE(Z_DIR_PIN,(INVERT_Z_DIR)^direction^BABYSTEP_INVERT_Z);
    #ifdef Z_DUAL_STEPPER_DRIVERS
      WRITE(Z2_DIR_PIN,(INVERT_Z_DIR)^direction^BABYSTEP_INVERT_Z);
    #endif
    //perform step 
    WRITE(Z_STEP_PIN, !INVERT_Z_STEP_PIN); 
    #ifdef Z_DUAL_STEPPER_DRIVERS
      WRITE(Z2_STEP_PIN, !INVERT_Z_STEP_PIN);
      delayMicroseconds(2);
    #else
      delayMicroseconds(3);
    #endif

    WRITE(Z_STEP_PIN, INVERT_Z_STEP_PIN);
    #ifdef Z_DUAL_STEPPER_DRIVERS
      WRITE(Z2_STEP_PIN, INVERT_Z_STEP_PIN);
    #endif

    //get old pin state back.
    WRITE(Z_DIR_PIN,old_z_dir_pin);
    #ifdef Z_DUAL_STEPPER_DRIVERS
      WRITE(Z2_DIR_PIN,old_z_dir_pin);
    #endif

  }
  break;
 
  default:    break;
  }
}
#endif //BABYSTEPPING

void digitalPotWrite(int address, int value) // From Arduino DigitalPotControl example
{
  #if defined(DIGIPOTSS_PIN) && DIGIPOTSS_PIN > -1
    digitalWrite(DIGIPOTSS_PIN,LOW); // take the SS pin low to select the chip
    SPI.transfer(address); //  send in the address and value via SPI:
    SPI.transfer(value);
    digitalWrite(DIGIPOTSS_PIN,HIGH); // take the SS pin high to de-select the chip:
    //delay(10);
  #endif
}

void EEPROM_read_st(int pos, uint8_t* value, uint8_t size)
{
    do
    {
        *value = eeprom_read_byte((unsigned char*)pos);
        pos++;
        value++;
    }while(--size);
}


void digipot_init() //Initialize Digipot Motor Current
{  
  EEPROM_read_st(EEPROM_SILENT,(uint8_t*)&SilentMode,sizeof(SilentMode));
  SilentModeMenu = SilentMode;
  #if defined(DIGIPOTSS_PIN) && DIGIPOTSS_PIN > -1
    if(SilentMode == 0){
    const uint8_t digipot_motor_current[] = DIGIPOT_MOTOR_CURRENT_LOUD;
    }else{
      const uint8_t digipot_motor_current[] = DIGIPOT_MOTOR_CURRENT;
    }
    SPI.begin();
    pinMode(DIGIPOTSS_PIN, OUTPUT);
    for(int i=0;i<=4;i++)
      //digitalPotWrite(digipot_ch[i], digipot_motor_current[i]);
      digipot_current(i,digipot_motor_current[i]);
  #endif
  #ifdef MOTOR_CURRENT_PWM_XY_PIN
    pinMode(MOTOR_CURRENT_PWM_XY_PIN, OUTPUT);
    pinMode(MOTOR_CURRENT_PWM_Z_PIN, OUTPUT);
    pinMode(MOTOR_CURRENT_PWM_E_PIN, OUTPUT);
    if((SilentMode == 0) || (farm_mode) ){

     motor_current_setting[0] = motor_current_setting_loud[0];
     motor_current_setting[1] = motor_current_setting_loud[1];
     motor_current_setting[2] = motor_current_setting_loud[2];

    }else{

     motor_current_setting[0] = motor_current_setting_silent[0];
     motor_current_setting[1] = motor_current_setting_silent[1];
     motor_current_setting[2] = motor_current_setting_silent[2];

    }
    digipot_current(0, motor_current_setting[0]);
    digipot_current(1, motor_current_setting[1]);
    digipot_current(2, motor_current_setting[2]);
    //Set timer5 to 31khz so the PWM of the motor power is as constant as possible. (removes a buzzing noise)
    TCCR5B = (TCCR5B & ~(_BV(CS50) | _BV(CS51) | _BV(CS52))) | _BV(CS50);
  #endif
}




void digipot_current(uint8_t driver, int current)
{
  #if defined(DIGIPOTSS_PIN) && DIGIPOTSS_PIN > -1
    const uint8_t digipot_ch[] = DIGIPOT_CHANNELS;
    digitalPotWrite(digipot_ch[driver], current);
  #endif
  #ifdef MOTOR_CURRENT_PWM_XY_PIN
  if (driver == 0) analogWrite(MOTOR_CURRENT_PWM_XY_PIN, (long)current * 255L / (long)MOTOR_CURRENT_PWM_RANGE);
  if (driver == 1) analogWrite(MOTOR_CURRENT_PWM_Z_PIN, (long)current * 255L / (long)MOTOR_CURRENT_PWM_RANGE);
  if (driver == 2) analogWrite(MOTOR_CURRENT_PWM_E_PIN, (long)current * 255L / (long)MOTOR_CURRENT_PWM_RANGE);
  #endif
}

void microstep_init()
{
  const uint8_t microstep_modes[] = MICROSTEP_MODES;

  #if defined(E1_MS1_PIN) && E1_MS1_PIN > -1
  pinMode(E1_MS1_PIN,OUTPUT);
  pinMode(E1_MS2_PIN,OUTPUT); 
  #endif

  #if defined(X_MS1_PIN) && X_MS1_PIN > -1
  pinMode(X_MS1_PIN,OUTPUT);
  pinMode(X_MS2_PIN,OUTPUT);  
  pinMode(Y_MS1_PIN,OUTPUT);
  pinMode(Y_MS2_PIN,OUTPUT);
  pinMode(Z_MS1_PIN,OUTPUT);
  pinMode(Z_MS2_PIN,OUTPUT);
  pinMode(E0_MS1_PIN,OUTPUT);
  pinMode(E0_MS2_PIN,OUTPUT);
  for(int i=0;i<=4;i++) microstep_mode(i,microstep_modes[i]);
  #endif
}

void microstep_ms(uint8_t driver, int8_t ms1, int8_t ms2)
{
  if(ms1 > -1) switch(driver)
  {
    case 0: digitalWrite( X_MS1_PIN,ms1); break;
    case 1: digitalWrite( Y_MS1_PIN,ms1); break;
    case 2: digitalWrite( Z_MS1_PIN,ms1); break;
    case 3: digitalWrite(E0_MS1_PIN,ms1); break;
    #if defined(E1_MS1_PIN) && E1_MS1_PIN > -1
    case 4: digitalWrite(E1_MS1_PIN,ms1); break;
    #endif
  }
  if(ms2 > -1) switch(driver)
  {
    case 0: digitalWrite( X_MS2_PIN,ms2); break;
    case 1: digitalWrite( Y_MS2_PIN,ms2); break;
    case 2: digitalWrite( Z_MS2_PIN,ms2); break;
    case 3: digitalWrite(E0_MS2_PIN,ms2); break;
    #if defined(E1_MS2_PIN) && E1_MS2_PIN > -1
    case 4: digitalWrite(E1_MS2_PIN,ms2); break;
    #endif
  }
}

void microstep_mode(uint8_t driver, uint8_t stepping_mode)
{
  switch(stepping_mode)
  {
    case 1: microstep_ms(driver,MICROSTEP1); break;
    case 2: microstep_ms(driver,MICROSTEP2); break;
    case 4: microstep_ms(driver,MICROSTEP4); break;
    case 8: microstep_ms(driver,MICROSTEP8); break;
    case 16: microstep_ms(driver,MICROSTEP16); break;
  }
}

void microstep_readings()
{
      SERIAL_PROTOCOLPGM("MS1,MS2 Pins\n");
      SERIAL_PROTOCOLPGM("X: ");
      SERIAL_PROTOCOL(   digitalRead(X_MS1_PIN));
      SERIAL_PROTOCOLLN( digitalRead(X_MS2_PIN));
      SERIAL_PROTOCOLPGM("Y: ");
      SERIAL_PROTOCOL(   digitalRead(Y_MS1_PIN));
      SERIAL_PROTOCOLLN( digitalRead(Y_MS2_PIN));
      SERIAL_PROTOCOLPGM("Z: ");
      SERIAL_PROTOCOL(   digitalRead(Z_MS1_PIN));
      SERIAL_PROTOCOLLN( digitalRead(Z_MS2_PIN));
      SERIAL_PROTOCOLPGM("E0: ");
      SERIAL_PROTOCOL(   digitalRead(E0_MS1_PIN));
      SERIAL_PROTOCOLLN( digitalRead(E0_MS2_PIN));
      #if defined(E1_MS1_PIN) && E1_MS1_PIN > -1
      SERIAL_PROTOCOLPGM("E1: ");
      SERIAL_PROTOCOL(   digitalRead(E1_MS1_PIN));
      SERIAL_PROTOCOLLN( digitalRead(E1_MS2_PIN));
      #endif
}

