/*
  planner.c - buffers movement commands and manages the acceleration profile plan
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

/* The ring buffer implementation gleaned from the wiring_serial library by David A. Mellis. */

/*  
 Reasoning behind the mathematics in this module (in the key of 'Mathematica'):
 
 s == speed, a == acceleration, t == time, d == distance
 
 Basic definitions:
 
 Speed[s_, a_, t_] := s + (a*t) 
 Travel[s_, a_, t_] := Integrate[Speed[s, a, t], t]
 
 Distance to reach a specific speed with a constant acceleration:
 
 Solve[{Speed[s, a, t] == m, Travel[s, a, t] == d}, d, t]
 d -> (m^2 - s^2)/(2 a) --> estimate_acceleration_distance()
 
 Speed after a given distance of travel with constant acceleration:
 
 Solve[{Speed[s, a, t] == m, Travel[s, a, t] == d}, m, t]
 m -> Sqrt[2 a d + s^2]    
 
 DestinationSpeed[s_, a_, d_] := Sqrt[2 a d + s^2]
 
 When to start braking (di) to reach a specified destionation speed (s2) after accelerating
 from initial speed s1 without ever stopping at a plateau:
 
 Solve[{DestinationSpeed[s1, a, di] == DestinationSpeed[s2, a, d - di]}, di]
 di -> (2 a d - s1^2 + s2^2)/(4 a) --> intersection_distance()
 
 IntersectionDistance[s1_, s2_, a_, d_] := (2 a d - s1^2 + s2^2)/(4 a)
 */

#include "Marlin.h"
#include "planner.h"
#include "stepper.h"
#include "temperature.h"
#include "fancheck.h"
#include "ultralcd.h"
#include "language.h"
#include "ConfigurationStore.h"

#ifdef MESH_BED_LEVELING
#include "mesh_bed_leveling.h"
#include "mesh_bed_calibration.h"
#endif

#ifdef TMC2130
#include "tmc2130.h"
#endif //TMC2130

#include <util/atomic.h>


//===========================================================================
//=============================public variables ============================
//===========================================================================

// Use M203 to override by software
float* max_feedrate = cs.max_feedrate_normal;


// Use M201 to override by software
uint32_t* max_acceleration_mm_per_s2 = cs.max_acceleration_mm_per_s2_normal;
uint32_t max_acceleration_steps_per_s2[NUM_AXIS];

#ifdef ENABLE_AUTO_BED_LEVELING
// this holds the required transform to compensate for bed level
matrix_3x3 plan_bed_level_matrix = {
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0,
};
#endif // #ifdef ENABLE_AUTO_BED_LEVELING

// The current position of the tool in absolute steps
long position[NUM_AXIS];   //rescaled from extern when axis_steps_per_mm are changed by gcode
static float previous_speed[NUM_AXIS]; // Speed of previous path line segment
static float previous_nominal_speed; // Nominal speed of previous path line segment
static float previous_safe_speed; // Exit speed limited by a jerk to full halt of a previous last segment.

#ifdef AUTOTEMP
float autotemp_max=250;
float autotemp_min=210;
float autotemp_factor=0.1;
bool autotemp_enabled=false;
#endif

//===========================================================================
//=================semi-private variables, used in inline  functions    =====
//===========================================================================
block_t block_buffer[BLOCK_BUFFER_SIZE];    // A ring buffer for motion instfructions
volatile uint8_t block_buffer_head;         // Index of the next block to be pushed
volatile uint8_t block_buffer_tail;         // Index of the block to process now

#ifdef PLANNER_DIAGNOSTICS
// Diagnostic function: Minimum number of planned moves since the last 
static uint8_t g_cntr_planner_queue_min = 0;
#endif /* PLANNER_DIAGNOSTICS */

//===========================================================================
//=============================private variables ============================
//===========================================================================
#ifdef PREVENT_DANGEROUS_EXTRUDE
int extrude_min_temp = EXTRUDE_MINTEMP;
#endif

#ifdef LIN_ADVANCE
float extruder_advance_K = LA_K_DEF;
float position_float[NUM_AXIS];
#endif

// Request the next block to start at zero E count
static bool plan_reset_next_e_queue;
static bool plan_reset_next_e_sched;

// Returns the index of the next block in the ring buffer
// NOTE: Removed modulo (%) operator, which uses an expensive divide and multiplication.
static inline uint8_t next_block_index(uint8_t block_index) {
  if (++ block_index == BLOCK_BUFFER_SIZE)
    block_index = 0;
  return block_index;
}


// Returns the index of the previous block in the ring buffer
static inline uint8_t prev_block_index(uint8_t block_index) {
  if (block_index == 0)
    block_index = BLOCK_BUFFER_SIZE;
  -- block_index;
  return block_index;
}

//===========================================================================
//=============================functions         ============================
//===========================================================================

// Calculates the distance (not time) it takes to accelerate from initial_rate to target_rate using the 
// given acceleration:
FORCE_INLINE float estimate_acceleration_distance(float initial_rate, float target_rate, float acceleration)
{
  if (acceleration!=0) {
    return((target_rate*target_rate-initial_rate*initial_rate)/
      (2.0*acceleration));
  }
  else {
    return 0.0;  // acceleration was 0, set acceleration distance to 0
  }
}

// This function gives you the point at which you must start braking (at the rate of -acceleration) if 
// you started at speed initial_rate and accelerated until this point and want to end at the final_rate after
// a total travel of distance. This can be used to compute the intersection point between acceleration and
// deceleration in the cases where the trapezoid has no plateau (i.e. never reaches maximum speed)

FORCE_INLINE float intersection_distance(float initial_rate, float final_rate, float acceleration, float distance) 
{
  if (acceleration!=0) {
    return((2.0*acceleration*distance-initial_rate*initial_rate+final_rate*final_rate)/
      (4.0*acceleration) );
  }
  else {
    return 0.0;  // acceleration was 0, set intersection distance to 0
  }
}

// Minimum stepper rate 120Hz.
#define MINIMAL_STEP_RATE 120

// Calculates trapezoid parameters so that the entry- and exit-speed is compensated by the provided factors.
void calculate_trapezoid_for_block(block_t *block, float entry_speed, float exit_speed) 
{
  // These two lines are the only floating point calculations performed in this routine.
  // initial_rate, final_rate in Hz.
  // Minimum stepper rate 120Hz, maximum 40kHz. If the stepper rate goes above 10kHz,
  // the stepper interrupt routine groups the pulses by 2 or 4 pulses per interrupt tick.
  uint32_t initial_rate = ceil(entry_speed * block->speed_factor); // (step/min)
  uint32_t final_rate   = ceil(exit_speed  * block->speed_factor); // (step/min)

  // Limit minimal step rate (Otherwise the timer will overflow.)
  if (initial_rate < MINIMAL_STEP_RATE)
      initial_rate = MINIMAL_STEP_RATE;
  if (initial_rate > block->nominal_rate)
      initial_rate = block->nominal_rate;
  if (final_rate < MINIMAL_STEP_RATE)
      final_rate = MINIMAL_STEP_RATE;
  if (final_rate > block->nominal_rate)
      final_rate = block->nominal_rate;

  uint32_t acceleration      = block->acceleration_steps_per_s2;
  if (acceleration == 0)
      // Don't allow zero acceleration.
      acceleration = 1;
  // estimate_acceleration_distance(float initial_rate, float target_rate, float acceleration)
  // (target_rate*target_rate-initial_rate*initial_rate)/(2.0*acceleration));
  uint32_t initial_rate_sqr  = initial_rate*initial_rate;
  //FIXME assert that this result fits a 64bit unsigned int.
  uint32_t nominal_rate_sqr  = block->nominal_rate*block->nominal_rate;
  uint32_t final_rate_sqr    = final_rate*final_rate;
  uint32_t acceleration_x2   = acceleration << 1;
  // ceil(estimate_acceleration_distance(initial_rate, block->nominal_rate, acceleration));
  uint32_t accelerate_steps  = (nominal_rate_sqr - initial_rate_sqr + acceleration_x2 - 1) / acceleration_x2;
  // floor(estimate_acceleration_distance(block->nominal_rate, final_rate, -acceleration));
  uint32_t decelerate_steps  = (nominal_rate_sqr - final_rate_sqr) / acceleration_x2;
  uint32_t accel_decel_steps = accelerate_steps + decelerate_steps;
  // Size of Plateau of Nominal Rate.
  uint32_t plateau_steps     = 0;

#ifdef LIN_ADVANCE
  uint16_t final_adv_steps = 0;
  uint16_t max_adv_steps = 0;
  if (block->flag & BLOCK_FLAG_USE_ADVANCE_LEAD) {
      final_adv_steps = final_rate * block->adv_comp;
  }
#endif

  // Is the Plateau of Nominal Rate smaller than nothing? That means no cruising, and we will
  // have to use intersection_distance() to calculate when to abort acceleration and start braking
  // in order to reach the final_rate exactly at the end of this block.
  if (accel_decel_steps < block->step_event_count.wide) {
    plateau_steps = block->step_event_count.wide - accel_decel_steps;
#ifdef LIN_ADVANCE
    if (block->flag & BLOCK_FLAG_USE_ADVANCE_LEAD)
        max_adv_steps = block->nominal_rate * block->adv_comp;
#endif
  } else {
    uint32_t acceleration_x4  = acceleration << 2;
    // Avoid negative numbers
    if (final_rate_sqr >= initial_rate_sqr) {
        // accelerate_steps = ceil(intersection_distance(initial_rate, final_rate, acceleration, block->step_event_count));
        // intersection_distance(float initial_rate, float final_rate, float acceleration, float distance) 
        // (2.0*acceleration*distance-initial_rate*initial_rate+final_rate*final_rate)/(4.0*acceleration);
#if 0
        accelerate_steps = (block->step_event_count >> 1) + (final_rate_sqr - initial_rate_sqr + acceleration_x4 - 1 + (block->step_event_count & 1) * acceleration_x2) / acceleration_x4;
#else
        accelerate_steps = final_rate_sqr - initial_rate_sqr + acceleration_x4 - 1;
        if (block->step_event_count.wide & 1)
            accelerate_steps += acceleration_x2;
        accelerate_steps /= acceleration_x4;
        accelerate_steps += (block->step_event_count.wide >> 1);
#endif
        if (accelerate_steps > block->step_event_count.wide)
            accelerate_steps = block->step_event_count.wide;
    } else {
#if 0
        decelerate_steps = (block->step_event_count >> 1) + (initial_rate_sqr - final_rate_sqr + (block->step_event_count & 1) * acceleration_x2) / acceleration_x4;
#else
        decelerate_steps = initial_rate_sqr - final_rate_sqr;
        if (block->step_event_count.wide & 1)
            decelerate_steps += acceleration_x2;
        decelerate_steps /= acceleration_x4;
        decelerate_steps += (block->step_event_count.wide >> 1);
#endif
        if (decelerate_steps > block->step_event_count.wide)
            decelerate_steps = block->step_event_count.wide;
        accelerate_steps = block->step_event_count.wide - decelerate_steps;
    }

#ifdef LIN_ADVANCE
    if (block->flag & BLOCK_FLAG_USE_ADVANCE_LEAD) {
        if(!accelerate_steps || !decelerate_steps) {
            // accelerate_steps=0: deceleration-only ramp, max_rate is effectively unused
            // decelerate_steps=0: acceleration-only ramp, max_rate _is_ final_rate
            max_adv_steps = final_adv_steps;
        } else {
            float max_rate = sqrt(acceleration_x2 * accelerate_steps + initial_rate_sqr);
            max_adv_steps = max_rate * block->adv_comp;
        }
    }
#endif
  }

  CRITICAL_SECTION_START;  // Fill variables used by the stepper in a critical section
  // This block locks the interrupts globally for 4.38 us,
  // which corresponds to a maximum repeat frequency of 228.57 kHz.
  // This blocking is safe in the context of a 10kHz stepper driver interrupt
  // or a 115200 Bd serial line receive interrupt, which will not trigger faster than 12kHz.
  if (!(block->flag & BLOCK_FLAG_BUSY)) { // Don't update variables if block is busy.
    block->accelerate_until = accelerate_steps;
    block->decelerate_after = accelerate_steps+plateau_steps;
    block->initial_rate = initial_rate;
    block->final_rate = final_rate;
#ifdef LIN_ADVANCE
    block->final_adv_steps = final_adv_steps;
    block->max_adv_steps = max_adv_steps;
#endif
  }
  CRITICAL_SECTION_END;
}

// Calculates the maximum allowable entry speed, when you must be able to reach target_velocity using the 
// decceleration within the allotted distance.
FORCE_INLINE float max_allowable_entry_speed(float decceleration, float target_velocity, float distance) 
{
    // assert(decceleration < 0);
    return  sqrt(target_velocity*target_velocity-2*decceleration*distance);
}

// Recalculates the motion plan according to the following algorithm:
//
//   1. Go over every block in reverse order and calculate a junction speed reduction (i.e. block_t.entry_factor) 
//      so that:
//     a. The junction jerk is within the set limit
//     b. No speed reduction within one block requires faster deceleration than the one, true constant 
//        acceleration.
//   2. Go over every block in chronological order and dial down junction speed reduction values if 
//     a. The speed increase within one block would require faster accelleration than the one, true 
//        constant acceleration.
//
// When these stages are complete all blocks have an entry_factor that will allow all speed changes to 
// be performed using only the one, true constant acceleration, and where no junction jerk is jerkier than 
// the set limit. Finally it will:
//
//   3. Recalculate trapezoids for all blocks.
//
//FIXME This routine is called 15x every time a new line is added to the planner,
// therefore it is a bottle neck and it shall be rewritten into a Fixed Point arithmetics,
// if the CPU is found lacking computational power.
//
// Following sources may be used to optimize the 8-bit AVR code:
// http://www.mikrocontroller.net/articles/AVR_Arithmetik
// http://darcy.rsgc.on.ca/ACES/ICE4M/FixedPoint/avrfix.pdf
// 
// https://github.com/gcc-mirror/gcc/blob/master/libgcc/config/avr/lib1funcs-fixed.S
// https://gcc.gnu.org/onlinedocs/gcc/Fixed-Point.html
// https://gcc.gnu.org/onlinedocs/gccint/Fixed-point-fractional-library-routines.html
// 
// https://ucexperiment.wordpress.com/2015/04/04/arduino-s15-16-fixed-point-math-routines/
// https://mekonik.wordpress.com/2009/03/18/arduino-avr-gcc-multiplication/
// https://github.com/rekka/avrmultiplication
// 
// https://people.ece.cornell.edu/land/courses/ece4760/Math/Floating_point/
// https://courses.cit.cornell.edu/ee476/Math/
// https://courses.cit.cornell.edu/ee476/Math/GCC644/fixedPt/multASM.S
//
void planner_recalculate(const float &safe_final_speed) 
{
    // Reverse pass
    // Make a local copy of block_buffer_tail, because the interrupt can alter it
    // by consuming the blocks, therefore shortening the queue.
    uint8_t tail = block_buffer_tail;
    uint8_t block_index;
    block_t *prev, *current, *next;

//    SERIAL_ECHOLNPGM("planner_recalculate - 1");

    // At least three blocks are in the queue?
    uint8_t n_blocks = (block_buffer_head + BLOCK_BUFFER_SIZE - tail) & (BLOCK_BUFFER_SIZE - 1);
    if (n_blocks >= 3) {
        // Initialize the last tripple of blocks.
        block_index = prev_block_index(block_buffer_head);
        next        = block_buffer + block_index;
        current     = block_buffer + (block_index = prev_block_index(block_index));
        // No need to recalculate the last block, it has already been set by the plan_buffer_line() function.
        // Vojtech thinks, that one shall not touch the entry speed of the very first block as well, because
        // 1) it may already be running at the stepper interrupt,
        // 2) there is no way to limit it when going in the forward direction.
        while (block_index != tail) {
            if (current->flag & BLOCK_FLAG_START_FROM_FULL_HALT) {
                // Don't modify the entry velocity of the starting block.
                // Also don't modify the trapezoids before this block, they are finalized already, prepared
                // for the stepper interrupt routine to use them.
                tail = block_index;
                // Update the number of blocks to process.
                n_blocks = (block_buffer_head + BLOCK_BUFFER_SIZE - tail) & (BLOCK_BUFFER_SIZE - 1);
                // SERIAL_ECHOLNPGM("START");
                break;
            }
            // If entry speed is already at the maximum entry speed, no need to recheck. Block is cruising.
            // If not, block in state of acceleration or deceleration. Reset entry speed to maximum and
            // check for maximum allowable speed reductions to ensure maximum possible planned speed.
            if (current->entry_speed != current->max_entry_speed) {
                // assert(current->entry_speed < current->max_entry_speed);
                // Entry speed could be increased up to the max_entry_speed, limited by the length of the current
                // segment and the maximum acceleration allowed for this segment.
                // If nominal length true, max junction speed is guaranteed to be reached even if decelerating to a jerk-from-zero velocity.
                // Only compute for max allowable speed if block is decelerating and nominal length is false.
                // entry_speed is uint16_t, 24 bits would be sufficient for block->acceleration and block->millimiteres, if scaled to um.
                // therefore an optimized assembly 24bit x 24bit -> 32bit multiply would be more than sufficient
                // together with an assembly 32bit->16bit sqrt function.
                current->entry_speed = ((current->flag & BLOCK_FLAG_NOMINAL_LENGTH) || current->max_entry_speed <= next->entry_speed) ?
                    current->max_entry_speed :
                    // min(current->max_entry_speed, sqrt(next->entry_speed*next->entry_speed+2*current->acceleration*current->millimeters));
                    min(current->max_entry_speed, max_allowable_entry_speed(-current->acceleration,next->entry_speed,current->millimeters));
                current->flag |= BLOCK_FLAG_RECALCULATE;
            }
            next = current;
            current = block_buffer + (block_index = prev_block_index(block_index));
        }
    }

//    SERIAL_ECHOLNPGM("planner_recalculate - 2");

    // Forward pass and recalculate the trapezoids.
    if (n_blocks >= 2) {
        // Better to limit the velocities using the already processed block, if it is available, so rather use the saved tail.
        block_index = tail;
        prev    = block_buffer + block_index;
        current = block_buffer + (block_index = next_block_index(block_index));
        do {
            // If the previous block is an acceleration block, but it is not long enough to complete the
            // full speed change within the block, we need to adjust the entry speed accordingly. Entry
            // speeds have already been reset, maximized, and reverse planned by reverse planner.
            // If nominal length is true, max junction speed is guaranteed to be reached. No need to recheck.
            if (! (prev->flag & BLOCK_FLAG_NOMINAL_LENGTH) && prev->entry_speed < current->entry_speed) {
                float entry_speed = min(current->entry_speed, max_allowable_entry_speed(-prev->acceleration,prev->entry_speed,prev->millimeters));
                // Check for junction speed change
                if (current->entry_speed != entry_speed) {
                    current->entry_speed = entry_speed;
                    current->flag |= BLOCK_FLAG_RECALCULATE;
                }
            }
            // Recalculate if current block entry or exit junction speed has changed.
            if ((prev->flag | current->flag) & BLOCK_FLAG_RECALCULATE) {
                // NOTE: Entry and exit factors always > 0 by all previous logic operations.
                calculate_trapezoid_for_block(prev, prev->entry_speed, current->entry_speed);
                // Reset current only to ensure next trapezoid is computed.
                prev->flag &= ~BLOCK_FLAG_RECALCULATE;
            }
            prev = current;
            current = block_buffer + (block_index = next_block_index(block_index));
        } while (block_index != block_buffer_head);
    }

//    SERIAL_ECHOLNPGM("planner_recalculate - 3");

    // Last/newest block in buffer. Exit speed is set with safe_final_speed. Always recalculated.
    current = block_buffer + prev_block_index(block_buffer_head);
    calculate_trapezoid_for_block(current, current->entry_speed, safe_final_speed);
    current->flag &= ~BLOCK_FLAG_RECALCULATE;

//    SERIAL_ECHOLNPGM("planner_recalculate - 4");
}

void plan_init() {
  block_buffer_head = 0;
  block_buffer_tail = 0;
  memset(position, 0, sizeof(position)); // clear position
  #ifdef LIN_ADVANCE
  memset(position_float, 0, sizeof(position_float)); // clear position
  #endif
  memset(previous_speed, 0, sizeof(previous_speed));
  previous_nominal_speed = 0.0;
  plan_reset_next_e_queue = false;
  plan_reset_next_e_sched = false;
}




#ifdef AUTOTEMP
void getHighESpeed()
{
  static float oldt=0;
  if(!autotemp_enabled){
    return;
  }
  if(degTargetHotend0()+2<autotemp_min) {  //probably temperature set to zero.
    return; //do nothing
  }

  float high=0.0;
  uint8_t block_index = block_buffer_tail;

  while(block_index != block_buffer_head) {
    if((block_buffer[block_index].steps[X_AXIS].wide != 0) ||
      (block_buffer[block_index].steps[Y_AXIS].wide != 0) ||
      (block_buffer[block_index].steps[Z_AXIS].wide != 0)) {
      float se=(float(block_buffer[block_index].steps[E_AXIS].wide)/float(block_buffer[block_index].step_event_count.wide))*block_buffer[block_index].nominal_speed;
      //se; mm/sec;
      if(se>high)
      {
        high=se;
      }
    }
    block_index = (block_index+1) & (BLOCK_BUFFER_SIZE - 1);
  }

  float g=autotemp_min+high*autotemp_factor;
  float t=g;
  if(t<autotemp_min)
    t=autotemp_min;
  if(t>autotemp_max)
    t=autotemp_max;
  if(oldt>t)
  {
    t=AUTOTEMP_OLDWEIGHT*oldt+(1-AUTOTEMP_OLDWEIGHT)*t;
  }
  oldt=t;
  setTargetHotend(t);
}
#endif

bool e_active()
{
	unsigned char e_active = 0;
	block_t *block;
  if(block_buffer_tail != block_buffer_head)
  {
    uint8_t block_index = block_buffer_tail;
    while(block_index != block_buffer_head)
    {
      block = &block_buffer[block_index];
      if(block->steps[E_AXIS].wide != 0) e_active++;
      block_index = (block_index+1) & (BLOCK_BUFFER_SIZE - 1);
    }
  }
  return (e_active > 0) ? true : false ;
}

void check_axes_activity()
{
  uint8_t x_active = 0;
  uint8_t y_active = 0;  
  uint8_t z_active = 0;
  uint8_t e_active = 0;
  uint8_t tail_fan_speed = fanSpeed;
  block_t *block;

  if(block_buffer_tail != block_buffer_head)
  {
    uint8_t block_index = block_buffer_tail;
    tail_fan_speed = block_buffer[block_index].fan_speed;
    while(block_index != block_buffer_head)
    {
      block = &block_buffer[block_index];
      if(block->steps[X_AXIS].wide != 0) x_active++;
      if(block->steps[Y_AXIS].wide != 0) y_active++;
      if(block->steps[Z_AXIS].wide != 0) z_active++;
      if(block->steps[E_AXIS].wide != 0) e_active++;
      block_index = (block_index+1) & (BLOCK_BUFFER_SIZE - 1);
    }
  }
  if((DISABLE_X) && (x_active == 0)) disable_x();
  if((DISABLE_Y) && (y_active == 0)) disable_y();
  if((DISABLE_Z) && (z_active == 0)) disable_z();
  if((DISABLE_E) && (e_active == 0)) disable_e0();
#if defined(FAN_PIN) && FAN_PIN > -1
  #ifdef FAN_KICKSTART_TIME
    static unsigned long fan_kick_end;
    if (tail_fan_speed) {
      if (fan_kick_end == 0) {
        // Just starting up fan - run at full power.
        fan_kick_end = _millis() + FAN_KICKSTART_TIME;
        tail_fan_speed = 255;
      } else if (fan_kick_end > _millis())
        // Fan still spinning up.
        tail_fan_speed = 255;
    } else {
      fan_kick_end = 0;
    }
  #endif//FAN_KICKSTART_TIME
  #ifdef FAN_SOFT_PWM
	if (fan_measuring) { //if measurement is currently in process, fanSpeedSoftPwm must remain set to 255, but we must update fanSpeedBckp value
		fanSpeedBckp = tail_fan_speed;
	}
	else {
		fanSpeedSoftPwm = tail_fan_speed;
	}
  //printf_P(PSTR("fanspeedsoftPWM %d \n"), fanSpeedSoftPwm);
  #else
  analogWrite(FAN_PIN,tail_fan_speed);
  #endif//!FAN_SOFT_PWM
#endif//FAN_PIN > -1
#ifdef AUTOTEMP
  getHighESpeed();
#endif
}

bool planner_aborted = false;

#ifdef PLANNER_DIAGNOSTICS
static inline void planner_update_queue_min_counter()
{
  uint8_t new_counter = moves_planned();
  if (new_counter < g_cntr_planner_queue_min)
    g_cntr_planner_queue_min = new_counter;
}
#endif /* PLANNER_DIAGNOSTICS */

extern volatile uint32_t step_events_completed; // The number of step events executed in the current block

void planner_reset_position()
{
    // First update the planner's current position in the physical motor steps.
    position[X_AXIS] = st_get_position(X_AXIS);
    position[Y_AXIS] = st_get_position(Y_AXIS);
    position[Z_AXIS] = st_get_position(Z_AXIS);
    position[E_AXIS] = st_get_position(E_AXIS);

    // Second update the current position of the front end.
    current_position[X_AXIS] = st_get_position_mm(X_AXIS);
    current_position[Y_AXIS] = st_get_position_mm(Y_AXIS);
    current_position[Z_AXIS] = st_get_position_mm(Z_AXIS);
    current_position[E_AXIS] = st_get_position_mm(E_AXIS);

    // Apply the mesh bed leveling correction to the Z axis.
#ifdef MESH_BED_LEVELING
    if (mbl.active) {
#if 1
        // Undo the bed level correction so the current Z position is reversible wrt. the machine coordinates.
        // This does not necessary mean that the Z position will be the same as linearly interpolated from the source G-code line.
        current_position[Z_AXIS] -= mbl.get_z(current_position[X_AXIS], current_position[Y_AXIS]);
#else
        // Undo the bed level correction so that the current Z position is the same as linearly interpolated from the source G-code line.
        if (current_block == NULL || (current_block->steps_x == 0 && current_block->steps_y == 0))
            current_position[Z_AXIS] -= mbl.get_z(current_position[X_AXIS], current_position[Y_AXIS]);
        else {
            float t = float(step_events_completed) / float(current_block->step_event_count);
            float vec[3] = { 
              current_block->steps_x / cs.axis_steps_per_mm[X_AXIS],
              current_block->steps_y / cs.axis_steps_per_mm[Y_AXIS],
              current_block->steps_z / cs.axis_steps_per_mm[Z_AXIS]
            };
            float pos1[3], pos2[3];
            for (int8_t i = 0; i < 3; ++ i) {
              if (current_block->direction_bits & (1<<i))
                vec[i] = - vec[i];
              pos1[i] = current_position[i] - vec[i] * t;
              pos2[i] = current_position[i] + vec[i] * (1.f - t);
            }
            pos1[Z_AXIS] -= mbl.get_z(pos1[X_AXIS], pos1[Y_AXIS]);
            pos2[Z_AXIS] -= mbl.get_z(pos2[X_AXIS], pos2[Y_AXIS]);
            current_position[Z_AXIS] = pos1[Z_AXIS] * t + pos2[Z_AXIS] * (1.f - t);
        }
#endif
    }
#endif

    // Apply inverse world correction matrix.
    machine2world(current_position[X_AXIS], current_position[Y_AXIS]);
    set_destination_to_current();
#ifdef LIN_ADVANCE
    memcpy(position_float, current_position, sizeof(position_float));
#endif
}

void planner_abort_hard()
{
    // Abort the stepper routine and flush the planner queue.
    DISABLE_STEPPER_DRIVER_INTERRUPT();

    // Now the front-end (the Marlin_main.cpp with its current_position) is out of sync.
    planner_reset_position();

    // Relay to planner wait routine that the current line shall be canceled.
    planner_aborted = true;

    // Clear the planner queue, reset and re-enable the stepper timer.
    quickStop();

    // Resets planner junction speeds. Assumes start from rest.
    previous_nominal_speed = 0.0;
    memset(previous_speed, 0, sizeof(previous_speed));

    // Reset position sync requests
    plan_reset_next_e_queue = false;
    plan_reset_next_e_sched = false;
}

void plan_buffer_line_curposXYZE(float feed_rate) {
    plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], feed_rate);
}

void plan_buffer_line_destinationXYZE(float feed_rate) {
    plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feed_rate);
}

void plan_set_position_curposXYZE(){
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
}

// Add a new linear movement to the buffer. steps_x, _y and _z is the absolute position in 
// mm. Microseconds specify how many microseconds the move should take to perform. To aid acceleration
// calculation the caller must also provide the physical length of the line in millimeters.
void plan_buffer_line(float x, float y, float z, const float &e, float feed_rate, const float* gcode_start_position, uint16_t segment_idx)
{
  // CRITICAL_SECTION_START; //prevent stack overflow in ISR
  // printf_P(PSTR("plan_buffer_line(%f, %f, %f, %f, %f, %u, [%f,%f,%f,%f], %u)\n"), x, y, z, e, feed_rate, extruder, gcode_start_position[0], gcode_start_position[1], gcode_start_position[2], gcode_start_position[3], segment_idx);
  // CRITICAL_SECTION_END;

  // Calculate the buffer head after we push this byte
  uint8_t next_buffer_head = next_block_index(block_buffer_head);

  // If the buffer is full: good! That means we are well ahead of the robot.
  // Rest here until there is room in the buffer.
  if (block_buffer_tail == next_buffer_head) {
      do {
          manage_heater(); 
          // Vojtech: Don't disable motors inside the planner!
          manage_inactivity(false); 
          lcd_update(0);
      } while (block_buffer_tail == next_buffer_head);
  }
#ifdef PLANNER_DIAGNOSTICS
  planner_update_queue_min_counter();
#endif /* PLANNER_DIAGNOSTICS */
  if(planner_aborted) {
      // avoid planning the block early if aborted
      SERIAL_ECHO_START;
      SERIAL_ECHOLNRPGM(_n("Move aborted"));
      return;
  }

  // Prepare to set up new block
  block_t *block = &block_buffer[block_buffer_head];

  // Mark block as not busy (Not executed by the stepper interrupt, could be still tinkered with.)
  // Also reset the block flag.
  block->flag = 0;

  // Set sdlen for calculating sd position
  block->sdlen = 0;

  // Save original start position of the move
  if (gcode_start_position)
      memcpy(block->gcode_start_position, gcode_start_position, sizeof(block_t::gcode_start_position));
  else
      memcpy(block->gcode_start_position, current_position, sizeof(block_t::gcode_start_position));
  
  // Save the index of this segment (when a single G0/1/2/3 command plans multiple segments)
  block->segment_idx = segment_idx;

  // Save the global feedrate at scheduling time
  block->gcode_feedrate = feedrate;

  // Reset the starting E position when requested
  if (plan_reset_next_e_queue)
  {
      position[E_AXIS] = 0;
#ifdef LIN_ADVANCE
      position_float[E_AXIS] = 0;
#endif

      // the block might still be discarded later, but we need to ensure the lower-level
      // count_position is also reset correctly for consistent results!
      plan_reset_next_e_queue = false;
      plan_reset_next_e_sched = true;
  }

#ifdef ENABLE_AUTO_BED_LEVELING
  apply_rotation_xyz(plan_bed_level_matrix, x, y, z);
#endif // ENABLE_AUTO_BED_LEVELING

    // Apply the machine correction matrix.
    {
      #if 0
        SERIAL_ECHOPGM("Planner, current position - servos: ");
        MYSERIAL.print(st_get_position_mm(X_AXIS), 5);
        SERIAL_ECHOPGM(", ");
        MYSERIAL.print(st_get_position_mm(Y_AXIS), 5);
        SERIAL_ECHOPGM(", ");
        MYSERIAL.print(st_get_position_mm(Z_AXIS), 5);
        SERIAL_ECHOLNPGM("");

        SERIAL_ECHOPGM("Planner, target position, initial: ");
        MYSERIAL.print(x, 5);
        SERIAL_ECHOPGM(", ");
        MYSERIAL.print(y, 5);
        SERIAL_ECHOLNPGM("");

        SERIAL_ECHOPGM("Planner, world2machine: ");
        MYSERIAL.print(world2machine_rotation_and_skew[0][0], 5);
        SERIAL_ECHOPGM(", ");
        MYSERIAL.print(world2machine_rotation_and_skew[0][1], 5);
        SERIAL_ECHOPGM(", ");
        MYSERIAL.print(world2machine_rotation_and_skew[1][0], 5);
        SERIAL_ECHOPGM(", ");
        MYSERIAL.print(world2machine_rotation_and_skew[1][1], 5);
        SERIAL_ECHOLNPGM("");
        SERIAL_ECHOPGM("Planner, offset: ");
        MYSERIAL.print(world2machine_shift[0], 5);
        SERIAL_ECHOPGM(", ");
        MYSERIAL.print(world2machine_shift[1], 5);
        SERIAL_ECHOLNPGM("");
      #endif

        world2machine(x, y);

      #if 0
        SERIAL_ECHOPGM("Planner, target position, corrected: ");
        MYSERIAL.print(x, 5);
        SERIAL_ECHOPGM(", ");
        MYSERIAL.print(y, 5);
        SERIAL_ECHOLNPGM("");
      #endif
    }

  // The target position of the tool in absolute steps
  // Calculate target position in absolute steps
  //this should be done after the wait, because otherwise a M92 code within the gcode disrupts this calculation somehow
  long target[4];
  target[X_AXIS] = lround(x*cs.axis_steps_per_mm[X_AXIS]);
  target[Y_AXIS] = lround(y*cs.axis_steps_per_mm[Y_AXIS]);
#ifdef MESH_BED_LEVELING
    if (mbl.active){
        target[Z_AXIS] = lround((z+mbl.get_z(x, y))*cs.axis_steps_per_mm[Z_AXIS]);
    }else{
        target[Z_AXIS] = lround(z*cs.axis_steps_per_mm[Z_AXIS]);
    }
#else
    target[Z_AXIS] = lround(z*cs.axis_steps_per_mm[Z_AXIS]);
#endif // ENABLE_MESH_BED_LEVELING
  target[E_AXIS] = lround(e*cs.axis_steps_per_mm[E_AXIS]);

  // Calculate subtraction to re-use result in many places
  // This saves memory and speeds up calculations
  int32_t de = target[E_AXIS] - position[E_AXIS];
  int32_t dx = target[X_AXIS] - position[X_AXIS];
  int32_t dy = target[Y_AXIS] - position[Y_AXIS];
  int32_t dz = target[Z_AXIS] - position[Z_AXIS];
  
  #ifdef PREVENT_DANGEROUS_EXTRUDE
  if(de)
  {
    if((int)degHotend(active_extruder)<extrude_min_temp)
    {
      position[E_AXIS]=target[E_AXIS]; //behave as if the move really took place, but ignore E part
      #ifdef LIN_ADVANCE
      position_float[E_AXIS] = e;
      #endif
      de = 0; // no difference
      SERIAL_ECHO_START;
      SERIAL_ECHOLNRPGM(_n(" cold extrusion prevented"));////MSG_ERR_COLD_EXTRUDE_STOP
    }
    
    #ifdef PREVENT_LENGTHY_EXTRUDE
    if(labs(de) > cs.axis_steps_per_mm[E_AXIS]*EXTRUDE_MAXLENGTH)
    {
      position[E_AXIS]=target[E_AXIS]; //behave as if the move really took place, but ignore E part
      #ifdef LIN_ADVANCE
      position_float[E_AXIS] = e;
      #endif
      de = 0; // no difference
      SERIAL_ECHO_START;
      SERIAL_ECHOLNRPGM(_n(" too long extrusion prevented"));////MSG_ERR_LONG_EXTRUDE_STOP
    }
    #endif
  }
  #endif

  // Number of steps for each axis
#ifndef COREXY
// default non-h-bot planning
block->steps[X_AXIS].wide = labs(dx);
block->steps[Y_AXIS].wide = labs(dy);
#else
// corexy planning
// these equations follow the form of the dA and dB equations on http://www.corexy.com/theory.html
block->steps[X_AXIS].wide = labs(dx + dy);
block->steps[Y_AXIS].wide = labs(dx - dy);
#endif
  block->steps[Z_AXIS].wide = labs(dz);
  block->steps[E_AXIS].wide = labs(de);
  block->step_event_count.wide = max(block->steps[X_AXIS].wide, max(block->steps[Y_AXIS].wide, max(block->steps[Z_AXIS].wide, block->steps[E_AXIS].wide)));

  // Bail if this is a zero-length block
  if (block->step_event_count.wide <= dropsegments)
  { 
#ifdef PLANNER_DIAGNOSTICS
    planner_update_queue_min_counter();
#endif /* PLANNER_DIAGNOSTICS */
    return; 
  }

  block->fan_speed = fanSpeed;

  // Compute direction bits for this block
  block->direction_bits = 0;
#ifndef COREXY
  if (dx < 0) block->direction_bits |= _BV(X_AXIS);
  if (dy < 0) block->direction_bits |= _BV(Y_AXIS);
#else
  if (dx + dy < 0) block->direction_bits |= _BV(X_AXIS);
  if (dx - dy < 0) block->direction_bits |= _BV(Y_AXIS);
#endif
  if (dz < 0) block->direction_bits |= _BV(Z_AXIS);
  if (de < 0) block->direction_bits |= _BV(E_AXIS);

  //enable active axes
  #ifdef COREXY
  if((block->steps[X_AXIS].wide != 0) || (block->steps[Y_AXIS].wide != 0))
  {
    enable_x();
    enable_y();
  }
  #else
  if(block->steps[X_AXIS].wide != 0) enable_x();
  if(block->steps[Y_AXIS].wide != 0) enable_y();
  #endif
  if(block->steps[Z_AXIS].wide != 0) enable_z();
  if(block->steps[E_AXIS].wide != 0) enable_e0();

  if (block->steps[E_AXIS].wide == 0)
  {
    if(feed_rate<cs.mintravelfeedrate) feed_rate=cs.mintravelfeedrate;
  }
  else
  {
    if(feed_rate<cs.minimumfeedrate) feed_rate=cs.minimumfeedrate;
  } 

/* This part of the code calculates the total length of the movement. 
For cartesian bots, the X_AXIS is the real X movement and same for Y_AXIS.
But for corexy bots, that is not true. The "X_AXIS" and "Y_AXIS" motors (that should be named to A_AXIS
and B_AXIS) cannot be used for X and Y length, because A=X+Y and B=X-Y.
So we need to create other 2 "AXIS", named X_HEAD and Y_HEAD, meaning the real displacement of the Head. 
Having the real displacement of the head, we can calculate the total movement length and apply the desired speed.
*/ 
  #ifndef COREXY
    float delta_mm[4];
    delta_mm[X_AXIS] = dx / cs.axis_steps_per_mm[X_AXIS];
    delta_mm[Y_AXIS] = dy / cs.axis_steps_per_mm[Y_AXIS];
  #else
    float delta_mm[6];
    delta_mm[X_HEAD] = dx / cs.axis_steps_per_mm[X_AXIS];
    delta_mm[Y_HEAD] = dy / cs.axis_steps_per_mm[Y_AXIS];
    delta_mm[X_AXIS] = (dx + dy) / cs.axis_steps_per_mm[X_AXIS];
    delta_mm[Y_AXIS] = (dx - dy) / cs.axis_steps_per_mm[Y_AXIS];
  #endif
  delta_mm[Z_AXIS] = dz / cs.axis_steps_per_mm[Z_AXIS];
  delta_mm[E_AXIS] = de / cs.axis_steps_per_mm[E_AXIS];
  if ( block->steps[X_AXIS].wide <=dropsegments && block->steps[Y_AXIS].wide <=dropsegments && block->steps[Z_AXIS].wide <=dropsegments )
  {
    block->millimeters = fabs(delta_mm[E_AXIS]);
  } 
  else
  {
    #ifndef COREXY
      block->millimeters = sqrt(square(delta_mm[X_AXIS]) + square(delta_mm[Y_AXIS]) + square(delta_mm[Z_AXIS]));
	#else
	  block->millimeters = sqrt(square(delta_mm[X_HEAD]) + square(delta_mm[Y_HEAD]) + square(delta_mm[Z_AXIS]));
    #endif	
  }
  float inverse_millimeters = 1.0/block->millimeters;  // Inverse millimeters to remove multiple divides 

    // Calculate speed in mm/second for each axis. No divide by zero due to previous checks.
  float inverse_second = feed_rate * inverse_millimeters;

  uint8_t moves_queued = moves_planned();

  // slow down when de buffer starts to empty, rather than wait at the corner for a buffer refill
#ifdef SLOWDOWN
  //FIXME Vojtech: Why moves_queued > 1? Why not >=1?
  // Can we somehow differentiate the filling of the buffer at the start of a g-code from a buffer draining situation?
  if (moves_queued > 1 && moves_queued < (BLOCK_BUFFER_SIZE >> 1)) {
      // segment time in micro seconds
      unsigned long segment_time = lround(1000000.0/inverse_second);
      if (segment_time < cs.min_segment_time_us)
          // buffer is draining, add extra time.  The amount of time added increases if the buffer is still emptied more.
          inverse_second=1000000.0/(segment_time+lround(2*(cs.min_segment_time_us-segment_time)/moves_queued));
  }
#endif // SLOWDOWN

  block->nominal_speed = block->millimeters * inverse_second; // (mm/sec) Always > 0
  block->nominal_rate = ceil(block->step_event_count.wide * inverse_second); // (step/sec) Always > 0

  // Calculate and limit speed in mm/sec for each axis
  float current_speed[4];
  float speed_factor = 1.0; //factor <=1 do decrease speed
  for(int i=0; i < 4; i++)
  {
    current_speed[i] = delta_mm[i] * inverse_second;
	if(fabs(current_speed[i]) > max_feedrate[i])
	{
      speed_factor = min(speed_factor, max_feedrate[i] / fabs(current_speed[i]));
	}
  }

  // Correct the speed  
  if( speed_factor < 1.0)
  {
    for(unsigned char i=0; i < 4; i++)
    {
      current_speed[i] *= speed_factor;
    }
    block->nominal_speed *= speed_factor;
    block->nominal_rate *= speed_factor;
  }

#ifdef LIN_ADVANCE
  float e_D_ratio = 0;
#endif
  // Compute and limit the acceleration rate for the trapezoid generator.  
  // block->step_event_count ... event count of the fastest axis
  // block->millimeters ... Euclidian length of the XYZ movement or the E length, if no XYZ movement.
  float steps_per_mm = block->step_event_count.wide/block->millimeters;
  uint32_t accel;
  if(block->steps[X_AXIS].wide == 0 && block->steps[Y_AXIS].wide == 0 && block->steps[Z_AXIS].wide == 0)
  {
    accel = ceil(cs.retract_acceleration * steps_per_mm); // convert to: acceleration steps/sec^2
    #ifdef LIN_ADVANCE
    block->flag &= ~BLOCK_FLAG_USE_ADVANCE_LEAD;
    #endif
  }
  else
  {
    accel = ceil((block->steps[E_AXIS].wide ? cs.acceleration : cs.travel_acceleration) * steps_per_mm); // convert to: acceleration steps/sec^2

    #ifdef LIN_ADVANCE
    /**
     * Use LIN_ADVANCE within this block if all these are true:
     *
     * extruder_advance_K       : There is an advance factor set.
     * delta_mm[E_AXIS] >= 0    : Extruding or traveling, but _not_ retracting.
     * |delta_mm[Z_AXIS]| < 0.5 : Z is only moved for leveling (_not_ for priming)
     */
    if (extruder_advance_K > 0
            && delta_mm[E_AXIS] >= 0
            && fabs(delta_mm[Z_AXIS]) < 0.5) {
        block->flag |= BLOCK_FLAG_USE_ADVANCE_LEAD;
#ifdef LA_FLOWADJ
        // M221/FLOW should change uniformly the extrusion thickness
        float delta_e = (e - position_float[E_AXIS]) / extruder_multiplier[extruder];
#else
        // M221/FLOW only adjusts for an incorrect source diameter
        float delta_e = (e - position_float[E_AXIS]);
#endif
        float delta_D = sqrt(sq(x - position_float[X_AXIS])
                             + sq(y - position_float[Y_AXIS])
                             + sq(z - position_float[Z_AXIS]));

        // all extrusion moves with LA require a compression which is proportional to the
        // extrusion_length to distance ratio (e/D)
        e_D_ratio = delta_e / delta_D;

        // Check for unusual high e_D ratio to detect if a retract move was combined with the last
        // print move due to min. steps per segment. Never execute this with advance! This assumes
        // no one will use a retract length of 0mm < retr_length < ~0.2mm and no one will print
        // 100mm wide lines using 3mm filament or 35mm wide lines using 1.75mm filament.
        if (e_D_ratio > 3.0)
            block->flag &= ~BLOCK_FLAG_USE_ADVANCE_LEAD;
        else if (e_D_ratio > 0) {
            const uint32_t max_accel_steps_per_s2 = ceil(cs.max_jerk[E_AXIS] / (extruder_advance_K * e_D_ratio) * steps_per_mm);
            if (accel > max_accel_steps_per_s2) {
                accel = max_accel_steps_per_s2;
                #ifdef LA_DEBUG
                SERIAL_ECHOLNPGM("LA: Block acceleration limited due to max E-jerk");
                #endif
            }
        }
    }
    #endif

    // Limit acceleration per axis
    for (uint8_t axis = 0; axis < NUM_AXIS; axis++)
    {
      if(block->steps[axis].wide && max_acceleration_steps_per_s2[axis] < accel)
      {
        const float max_possible = float(max_acceleration_steps_per_s2[axis]) * float(block->step_event_count.wide) / float(block->steps[axis].wide);
        if (max_possible < accel) accel = max_possible;
      }
    }
  }
  // Acceleration of the segment, in mm/sec^2
  block->acceleration_steps_per_s2 = accel;
  block->acceleration = accel / steps_per_mm;
  block->acceleration_rate = (uint32_t)(accel * (float(1UL << 24) / ((F_CPU) / 8.0f)));

  // Start with a safe speed.
  // Safe speed is the speed, from which the machine may halt to stop immediately.
  float safe_speed = block->nominal_speed;
  bool  limited = false;
  for (uint8_t axis = 0; axis < 4; ++ axis) {
      float jerk = fabs(current_speed[axis]);
      if (jerk > cs.max_jerk[axis]) {
          // The actual jerk is lower, if it has been limited by the XY jerk.
          if (limited) {
              // Spare one division by a following gymnastics:
              // Instead of jerk *= safe_speed / block->nominal_speed,
              // multiply max_jerk[axis] by the divisor.
              jerk *= safe_speed;
              float mjerk = cs.max_jerk[axis] * block->nominal_speed;
              if (jerk > mjerk) {
                  safe_speed *= mjerk / jerk;
                  limited = true;
              }
          } else {
              safe_speed = cs.max_jerk[axis];
              limited = true;
          }
      }
  }

  if (plan_reset_next_e_sched)
  {
      // finally propagate a pending reset
      block->flag |= BLOCK_FLAG_E_RESET;
      plan_reset_next_e_sched = false;
  }

  // Initial limit on the segment entry velocity.
  float vmax_junction;

  //FIXME Vojtech: Why only if at least two lines are planned in the queue?
  // Is it because we don't want to tinker with the first buffer line, which
  // is likely to be executed by the stepper interrupt routine soon?
  if (moves_queued > 1 && previous_nominal_speed > 0.0001f) {
      // Estimate a maximum velocity allowed at a joint of two successive segments.
      // If this maximum velocity allowed is lower than the minimum of the entry / exit safe velocities,
      // then the machine is not coasting anymore and the safe entry / exit velocities shall be used.

      // The junction velocity will be shared between successive segments. Limit the junction velocity to their minimum.
      bool prev_speed_larger = previous_nominal_speed > block->nominal_speed;
      float smaller_speed_factor = prev_speed_larger ? (block->nominal_speed / previous_nominal_speed) : (previous_nominal_speed / block->nominal_speed);
      // Pick the smaller of the nominal speeds. Higher speed shall not be achieved at the junction during coasting.
      vmax_junction = prev_speed_larger ? block->nominal_speed : previous_nominal_speed;
      // Factor to multiply the previous / current nominal velocities to get componentwise limited velocities.
      float v_factor = 1.f;
      limited = false;
      // Now limit the jerk in all axes.
      for (uint8_t axis = 0; axis < 4; ++ axis) {
          // Limit an axis. We have to differentiate coasting from the reversal of an axis movement, or a full stop.
          float v_exit  = previous_speed[axis];
          float v_entry = current_speed [axis];
          if (prev_speed_larger)
              v_exit *= smaller_speed_factor;
          if (limited) {
              v_exit  *= v_factor;
              v_entry *= v_factor;
          }
          // Calculate the jerk depending on whether the axis is coasting in the same direction or reversing a direction.
          float jerk = 
              (v_exit > v_entry) ?
                  ((v_entry > 0.f || v_exit < 0.f) ?
                      // coasting
                      (v_exit - v_entry) : 
                      // axis reversal
                      max(v_exit, - v_entry)) :
                  // v_exit <= v_entry
                  ((v_entry < 0.f || v_exit > 0.f) ?
                      // coasting
                      (v_entry - v_exit) :
                      // axis reversal
                      max(- v_exit, v_entry));
          if (jerk > cs.max_jerk[axis]) {
              v_factor *= cs.max_jerk[axis] / jerk;
              limited = true;
          }
      }
      if (limited)
          vmax_junction *= v_factor;
      // Now the transition velocity is known, which maximizes the shared exit / entry velocity while
      // respecting the jerk factors, it may be possible, that applying separate safe exit / entry velocities will achieve faster prints.
      float vmax_junction_threshold = vmax_junction * 0.99f;
      if (previous_safe_speed > vmax_junction_threshold && safe_speed > vmax_junction_threshold) {
          // Not coasting. The machine will stop and start the movements anyway,
          // better to start the segment from start.
          block->flag |= BLOCK_FLAG_START_FROM_FULL_HALT;
          vmax_junction = safe_speed;
      }
  } else {
      block->flag |= BLOCK_FLAG_START_FROM_FULL_HALT;
      vmax_junction = safe_speed;
  }

  // Max entry speed of this block equals the max exit speed of the previous block.
  block->max_entry_speed = vmax_junction;

  // Initialize block entry speed. Compute based on deceleration to safe_speed.
  double v_allowable = max_allowable_entry_speed(-block->acceleration,safe_speed,block->millimeters);
  block->entry_speed = min(vmax_junction, v_allowable);

  // Initialize planner efficiency flags
  // Set flag if block will always reach maximum junction speed regardless of entry/exit speeds.
  // If a block can de/ac-celerate from nominal speed to zero within the length of the block, then
  // the current block and next block junction speeds are guaranteed to always be at their maximum
  // junction speeds in deceleration and acceleration, respectively. This is due to how the current
  // block nominal speed limits both the current and next maximum junction speeds. Hence, in both
  // the reverse and forward planners, the corresponding block junction speed will always be at the
  // the maximum junction speed and may always be ignored for any speed reduction checks.
  // Always calculate trapezoid for new block
  block->flag |= (block->nominal_speed <= v_allowable) ? (BLOCK_FLAG_NOMINAL_LENGTH | BLOCK_FLAG_RECALCULATE) : BLOCK_FLAG_RECALCULATE;

  // Update previous path unit_vector and nominal speed
  memcpy(previous_speed, current_speed, sizeof(previous_speed)); // previous_speed[] = current_speed[]
  previous_nominal_speed = block->nominal_speed;
  previous_safe_speed = safe_speed;

  // Precalculate the division, so when all the trapezoids in the planner queue get recalculated, the division is not repeated.
  block->speed_factor = block->nominal_rate / block->nominal_speed;

#ifdef LIN_ADVANCE
  if (block->flag & BLOCK_FLAG_USE_ADVANCE_LEAD) {
      // calculate the compression ratio for the segment (the required advance steps are computed
      // during trapezoid planning)
      float adv_comp = extruder_advance_K * e_D_ratio * cs.axis_steps_per_mm[E_AXIS]; // (step/(mm/s))
      block->adv_comp = adv_comp / block->speed_factor; // step/(step/min)

      float advance_speed;
      if (e_D_ratio > 0)
          advance_speed = (extruder_advance_K * e_D_ratio * block->acceleration * cs.axis_steps_per_mm[E_AXIS]);
      else
          advance_speed = cs.max_jerk[E_AXIS] * cs.axis_steps_per_mm[E_AXIS];

      // to save more space we avoid another copy of calc_timer and go through slow division, but we
      // still need to replicate the *exact* same step grouping policy (see below)
      if (advance_speed > MAX_STEP_FREQUENCY) advance_speed = MAX_STEP_FREQUENCY;
      float advance_rate = (F_CPU / 8.0) / advance_speed;
      if (advance_speed > 20000) {
          block->advance_rate = advance_rate * 4;
          block->advance_step_loops = 4;
      }
      else if (advance_speed > 10000) {
          block->advance_rate = advance_rate * 2;
          block->advance_step_loops = 2;
      }
      else
      {
          // never overflow the internal accumulator with very low rates
          if (advance_rate < UINT16_MAX)
              block->advance_rate = advance_rate;
          else
              block->advance_rate = UINT16_MAX;
          block->advance_step_loops = 1;
      }

      #ifdef LA_DEBUG
      if (block->advance_step_loops > 2)
          // @wavexx: we should really check for the difference between step_loops and
          //          advance_step_loops instead. A difference of more than 1 will lead
          //          to uneven speed and *should* be adjusted here by furthermore
          //          reducing the speed.
          SERIAL_ECHOLNPGM("LA: More than 2 steps per eISR loop executed.");
      #endif
  }
#endif

  calculate_trapezoid_for_block(block, block->entry_speed, safe_speed);

  if (block->step_event_count.wide <= 32767)
    block->flag |= BLOCK_FLAG_DDA_LOWRES;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      // Move the buffer head ensuring the current block hasn't been cancelled from an isr context
      // (this is possible both during crash detection *and* uvlo, thus needing a global cli)
      if(planner_aborted) return;
      block_buffer_head = next_buffer_head;
  }

  // Update position
  memcpy(position, target, sizeof(target)); // position[] = target[]

  #ifdef LIN_ADVANCE
  position_float[X_AXIS] = x;
  position_float[Y_AXIS] = y;
  position_float[Z_AXIS] = z;
  position_float[E_AXIS] = e;
  #endif
    
  // Recalculate the trapezoids to maximize speed at the segment transitions while respecting
  // the machine limits (maximum acceleration and maximum jerk).
  // This runs asynchronously with the stepper interrupt controller, which may
  // interfere with the process.
  planner_recalculate(safe_speed);

//  SERIAL_ECHOPGM("Q");
//  SERIAL_ECHO(int(moves_planned()));
//  SERIAL_ECHOLNPGM("");

#ifdef PLANNER_DIAGNOSTICS
  planner_update_queue_min_counter();
#endif /* PLANNER_DIAGNOSTIC */

  // The stepper timer interrupt will run continuously from now on.
  // If there are no planner blocks to be executed by the stepper routine,
  // the stepper interrupt ticks at 1kHz to wake up and pick a block
  // from the planner queue if available.
  ENABLE_STEPPER_DRIVER_INTERRUPT();
}

#ifdef ENABLE_AUTO_BED_LEVELING
vector_3 plan_get_position() {
	vector_3 position = vector_3(st_get_position_mm(X_AXIS), st_get_position_mm(Y_AXIS), st_get_position_mm(Z_AXIS));

	//position.debug("in plan_get position");
	//plan_bed_level_matrix.debug("in plan_get bed_level");
	matrix_3x3 inverse = matrix_3x3::transpose(plan_bed_level_matrix);
	//inverse.debug("in plan_get inverse");
	position.apply_rotation(inverse);
	//position.debug("after rotation");

	return position;
}
#endif // ENABLE_AUTO_BED_LEVELING

void plan_set_position(float x, float y, float z, const float &e)
{
#ifdef ENABLE_AUTO_BED_LEVELING
    apply_rotation_xyz(plan_bed_level_matrix, x, y, z);
#endif // ENABLE_AUTO_BED_LEVELING

    world2machine(x, y);

  position[X_AXIS] = lround(x*cs.axis_steps_per_mm[X_AXIS]);
  position[Y_AXIS] = lround(y*cs.axis_steps_per_mm[Y_AXIS]);
#ifdef MESH_BED_LEVELING
  position[Z_AXIS] = mbl.active ? 
    lround((z+mbl.get_z(x, y))*cs.axis_steps_per_mm[Z_AXIS]) :
    lround(z*cs.axis_steps_per_mm[Z_AXIS]);
#else
  position[Z_AXIS] = lround(z*cs.axis_steps_per_mm[Z_AXIS]);
#endif // ENABLE_MESH_BED_LEVELING
  position[E_AXIS] = lround(e*cs.axis_steps_per_mm[E_AXIS]);
  #ifdef LIN_ADVANCE
  position_float[X_AXIS] = x;
  position_float[Y_AXIS] = y;
  position_float[Z_AXIS] = z;
  position_float[E_AXIS] = e;
  #endif
  st_set_position(position);
  previous_nominal_speed = 0.0; // Resets planner junction speeds. Assumes start from rest.
  memset(previous_speed, 0, sizeof(previous_speed));
}

// Only useful in the bed leveling routine, when the mesh bed leveling is off.
void plan_set_z_position(const float &z)
{
  #ifdef LIN_ADVANCE
  position_float[Z_AXIS] = z;
  #endif
  position[Z_AXIS] = lround(z*cs.axis_steps_per_mm[Z_AXIS]);
  st_set_position(position);
}

void plan_set_e_position(const float &e)
{
  #ifdef LIN_ADVANCE
  position_float[E_AXIS] = e;
  #endif
  position[E_AXIS] = lround(e*cs.axis_steps_per_mm[E_AXIS]);  
  st_set_e_position(position[E_AXIS]);
}

void plan_reset_next_e()
{
    plan_reset_next_e_queue = true;
}

#ifdef PREVENT_DANGEROUS_EXTRUDE
void set_extrude_min_temp(int temp)
{
  extrude_min_temp = temp;
}
#endif

// Calculate the steps/s^2 acceleration rates, based on the mm/s^s
void reset_acceleration_rates()
{
	for(int8_t i=0; i < NUM_AXIS; i++)
        max_acceleration_steps_per_s2[i] = max_acceleration_mm_per_s2[i] * cs.axis_steps_per_mm[i];
}

#ifdef TMC2130
void update_mode_profile()
{
	if (tmc2130_mode == TMC2130_MODE_NORMAL)
	{
		max_feedrate = cs.max_feedrate_normal;
		max_acceleration_mm_per_s2 = cs.max_acceleration_mm_per_s2_normal;
	}
	else if (tmc2130_mode == TMC2130_MODE_SILENT)
	{
		max_feedrate = cs.max_feedrate_silent;
		max_acceleration_mm_per_s2 = cs.max_acceleration_mm_per_s2_silent;
	}
	reset_acceleration_rates();
}
#endif //TMC2130

#ifdef PLANNER_DIAGNOSTICS
uint8_t planner_queue_min()
{
  return g_cntr_planner_queue_min;
}

void planner_queue_min_reset()
{
  g_cntr_planner_queue_min = moves_planned();
}
#endif /* PLANNER_DIAGNOSTICS */

void planner_add_sd_length(uint16_t sdlen)
{
  if (block_buffer_head != block_buffer_tail) {
    // The planner buffer is not empty. Get the index of the last buffer line entered,
    // which is (block_buffer_head - 1) modulo BLOCK_BUFFER_SIZE.
    block_buffer[prev_block_index(block_buffer_head)].sdlen += sdlen;
  } else {
    // There is no line stored in the planner buffer, which means the last command does not need to be revertible,
    // at a power panic, so the length of this command may be forgotten.
  }
}

uint16_t planner_calc_sd_length()
{
	uint8_t _block_buffer_head = block_buffer_head;
	uint8_t _block_buffer_tail = block_buffer_tail;
	uint16_t sdlen = 0;
	while (_block_buffer_head != _block_buffer_tail)
	{
		sdlen += block_buffer[_block_buffer_tail].sdlen;
	    _block_buffer_tail = (_block_buffer_tail + 1) & (BLOCK_BUFFER_SIZE - 1);  
	}
	return sdlen;
}
