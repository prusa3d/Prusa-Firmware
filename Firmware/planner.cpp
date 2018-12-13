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

//===========================================================================
//=============================public variables ============================
//===========================================================================

// Use M203 to override by software
float* max_feedrate = cs.max_feedrate_normal;


// Use M201 to override by software
unsigned long* max_acceleration_units_per_sq_second = cs.max_acceleration_units_per_sq_second_normal;
unsigned long axis_steps_per_sqr_second[NUM_AXIS];

#ifdef ENABLE_AUTO_BED_LEVELING
// this holds the required transform to compensate for bed level
matrix_3x3 plan_bed_level_matrix = {
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0,
};
#endif // #ifdef ENABLE_AUTO_BED_LEVELING

// The current position of the tool in absolute steps
long position[NUM_AXIS];   //rescaled from extern when axis_steps_per_unit are changed by gcode
static float previous_speed[NUM_AXIS]; // Speed of previous path line segment
static float previous_nominal_speed; // Nominal speed of previous path line segment
static float previous_safe_speed; // Exit speed limited by a jerk to full halt of a previous last segment.

uint8_t maxlimit_status;

#ifdef AUTOTEMP
float autotemp_max=250;
float autotemp_min=210;
float autotemp_factor=0.1;
bool autotemp_enabled=false;
#endif

unsigned char g_uc_extruder_last_move[3] = {0,0,0};

//===========================================================================
//=================semi-private variables, used in inline  functions    =====
//===========================================================================
block_t block_buffer[BLOCK_BUFFER_SIZE];            // A ring buffer for motion instfructions
volatile unsigned char block_buffer_head;           // Index of the next block to be pushed
volatile unsigned char block_buffer_tail;           // Index of the block to process now

#ifdef PLANNER_DIAGNOSTICS
// Diagnostic function: Minimum number of planned moves since the last 
static uint8_t g_cntr_planner_queue_min = 0;
#endif /* PLANNER_DIAGNOSTICS */

//===========================================================================
//=============================private variables ============================
//===========================================================================
#ifdef PREVENT_DANGEROUS_EXTRUDE
float extrude_min_temp=EXTRUDE_MINTEMP;
#endif

#ifdef LIN_ADVANCE
    float extruder_advance_k = LIN_ADVANCE_K,
    advance_ed_ratio = LIN_ADVANCE_E_D_RATIO,
    position_float[NUM_AXIS] = { 0 };
#endif

// Returns the index of the next block in the ring buffer
// NOTE: Removed modulo (%) operator, which uses an expensive divide and multiplication.
static inline int8_t next_block_index(int8_t block_index) {
  if (++ block_index == BLOCK_BUFFER_SIZE)
    block_index = 0; 
  return block_index;
}


// Returns the index of the previous block in the ring buffer
static inline int8_t prev_block_index(int8_t block_index) {
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

  uint32_t acceleration      = block->acceleration_st;
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

  // Is the Plateau of Nominal Rate smaller than nothing? That means no cruising, and we will
  // have to use intersection_distance() to calculate when to abort acceleration and start braking
  // in order to reach the final_rate exactly at the end of this block.
  if (accel_decel_steps < block->step_event_count.wide) {
    plateau_steps = block->step_event_count.wide - accel_decel_steps;
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
  }

  CRITICAL_SECTION_START;  // Fill variables used by the stepper in a critical section
  // This block locks the interrupts globally for 4.38 us,
  // which corresponds to a maximum repeat frequency of 228.57 kHz.
  // This blocking is safe in the context of a 10kHz stepper driver interrupt
  // or a 115200 Bd serial line receive interrupt, which will not trigger faster than 12kHz.
  if (! block->busy) { // Don't update variables if block is busy.
    block->accelerate_until = accelerate_steps;
    block->decelerate_after = accelerate_steps+plateau_steps;
    block->initial_rate = initial_rate;
    block->final_rate = final_rate;
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
    unsigned char tail = block_buffer_tail;
    uint8_t block_index;
    block_t *prev, *current, *next;

//    SERIAL_ECHOLNPGM("planner_recalculate - 1");

    // At least three blocks are in the queue?
    unsigned char n_blocks = (block_buffer_head + BLOCK_BUFFER_SIZE - tail) & (BLOCK_BUFFER_SIZE - 1);
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
  memset(position_float, 0, sizeof(position)); // clear position
#endif
  previous_speed[0] = 0.0;
  previous_speed[1] = 0.0;
  previous_speed[2] = 0.0;
  previous_speed[3] = 0.0;
  previous_nominal_speed = 0.0;
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
    if((block_buffer[block_index].steps_x.wide != 0) ||
      (block_buffer[block_index].steps_y.wide != 0) ||
      (block_buffer[block_index].steps_z.wide != 0)) {
      float se=(float(block_buffer[block_index].steps_e.wide)/float(block_buffer[block_index].step_event_count.wide))*block_buffer[block_index].nominal_speed;
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
  setTargetHotend0(t);
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
      if(block->steps_e.wide != 0) e_active++;
      block_index = (block_index+1) & (BLOCK_BUFFER_SIZE - 1);
    }
  }
  return (e_active > 0) ? true : false ;
}

void check_axes_activity()
{
  unsigned char x_active = 0;
  unsigned char y_active = 0;  
  unsigned char z_active = 0;
  unsigned char e_active = 0;
  unsigned char tail_fan_speed = fanSpeed;
  block_t *block;

  if(block_buffer_tail != block_buffer_head)
  {
    uint8_t block_index = block_buffer_tail;
    tail_fan_speed = block_buffer[block_index].fan_speed;
    while(block_index != block_buffer_head)
    {
      block = &block_buffer[block_index];
      if(block->steps_x.wide != 0) x_active++;
      if(block->steps_y.wide != 0) y_active++;
      if(block->steps_z.wide != 0) z_active++;
      if(block->steps_e.wide != 0) e_active++;
      block_index = (block_index+1) & (BLOCK_BUFFER_SIZE - 1);
    }
  }
  if((DISABLE_X) && (x_active == 0)) disable_x();
  if((DISABLE_Y) && (y_active == 0)) disable_y();
  if((DISABLE_Z) && (z_active == 0)) disable_z();
  if((DISABLE_E) && (e_active == 0))
  {
    disable_e0();
    disable_e1();
    disable_e2(); 
  }
#if defined(FAN_PIN) && FAN_PIN > -1
  #ifdef FAN_KICKSTART_TIME
    static unsigned long fan_kick_end;
    if (tail_fan_speed) {
      if (fan_kick_end == 0) {
        // Just starting up fan - run at full power.
        fan_kick_end = millis() + FAN_KICKSTART_TIME;
        tail_fan_speed = 255;
      } else if (fan_kick_end > millis())
        // Fan still spinning up.
        tail_fan_speed = 255;
    } else {
      fan_kick_end = 0;
    }
  #endif//FAN_KICKSTART_TIME
  #ifdef FAN_SOFT_PWM
  fanSpeedSoftPwm = tail_fan_speed;
  #else
  analogWrite(FAN_PIN,tail_fan_speed);
  #endif//!FAN_SOFT_PWM
#endif//FAN_PIN > -1
#ifdef AUTOTEMP
  getHighESpeed();
#endif
}

bool waiting_inside_plan_buffer_line_print_aborted = false;
/*
void planner_abort_soft()
{
    // Empty the queue.
    while (blocks_queued()) plan_discard_current_block();
    // Relay to planner wait routine, that the current line shall be canceled.
    waiting_inside_plan_buffer_line_print_aborted = true;
    //current_position[i]
}
*/

#ifdef PLANNER_DIAGNOSTICS
static inline void planner_update_queue_min_counter()
{
  uint8_t new_counter = moves_planned();
  if (new_counter < g_cntr_planner_queue_min)
    g_cntr_planner_queue_min = new_counter;
}
#endif /* PLANNER_DIAGNOSTICS */

extern volatile uint32_t step_events_completed; // The number of step events executed in the current block

void planner_abort_hard()
{
    // Abort the stepper routine and flush the planner queue.
    DISABLE_STEPPER_DRIVER_INTERRUPT();

    // Now the front-end (the Marlin_main.cpp with its current_position) is out of sync.
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
              current_block->steps_x / cs.axis_steps_per_unit[X_AXIS],
              current_block->steps_y / cs.axis_steps_per_unit[Y_AXIS],
              current_block->steps_z / cs.axis_steps_per_unit[Z_AXIS]
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
    // Clear the planner queue, reset and re-enable the stepper timer.
    quickStop();

    // Apply inverse world correction matrix.
    machine2world(current_position[X_AXIS], current_position[Y_AXIS]);
    memcpy(destination, current_position, sizeof(destination));

    // Resets planner junction speeds. Assumes start from rest.
    previous_nominal_speed = 0.0;
    previous_speed[0] = 0.0;
    previous_speed[1] = 0.0;
    previous_speed[2] = 0.0;
    previous_speed[3] = 0.0;

    // Relay to planner wait routine, that the current line shall be canceled.
    waiting_inside_plan_buffer_line_print_aborted = true;
}

float junction_deviation = 0.1;
// Add a new linear movement to the buffer. steps_x, _y and _z is the absolute position in 
// mm. Microseconds specify how many microseconds the move should take to perform. To aid acceleration
// calculation the caller must also provide the physical length of the line in millimeters.
void plan_buffer_line(float x, float y, float z, const float &e, float feed_rate, const uint8_t &extruder)
{
    // Calculate the buffer head after we push this byte
  int next_buffer_head = next_block_index(block_buffer_head);

  // If the buffer is full: good! That means we are well ahead of the robot. 
  // Rest here until there is room in the buffer.
  if (block_buffer_tail == next_buffer_head) {
      waiting_inside_plan_buffer_line_print_aborted = false;
      do {
          manage_heater(); 
          // Vojtech: Don't disable motors inside the planner!
          manage_inactivity(false); 
          lcd_update(0);
      } while (block_buffer_tail == next_buffer_head);
      if (waiting_inside_plan_buffer_line_print_aborted) {
          // Inside the lcd_update(0) routine the print has been aborted.
          // Cancel the print, do not plan the current line this routine is waiting on.
#ifdef PLANNER_DIAGNOSTICS
          planner_update_queue_min_counter();
#endif /* PLANNER_DIAGNOSTICS */
          return;
      }
  }
#ifdef PLANNER_DIAGNOSTICS
  planner_update_queue_min_counter();
#endif /* PLANNER_DIAGNOSTICS */

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
  target[X_AXIS] = lround(x*cs.axis_steps_per_unit[X_AXIS]);
  target[Y_AXIS] = lround(y*cs.axis_steps_per_unit[Y_AXIS]);
#ifdef MESH_BED_LEVELING
    if (mbl.active){
        target[Z_AXIS] = lround((z+mbl.get_z(x, y))*cs.axis_steps_per_unit[Z_AXIS]);
    }else{
        target[Z_AXIS] = lround(z*cs.axis_steps_per_unit[Z_AXIS]);
    }
#else
    target[Z_AXIS] = lround(z*cs.axis_steps_per_unit[Z_AXIS]);
#endif // ENABLE_MESH_BED_LEVELING
  target[E_AXIS] = lround(e*cs.axis_steps_per_unit[E_AXIS]);
  
#ifdef LIN_ADVANCE
    const float mm_D_float = sqrt(sq(x - position_float[X_AXIS]) + sq(y - position_float[Y_AXIS]));
    float de_float = e - position_float[E_AXIS];
#endif
    
  #ifdef PREVENT_DANGEROUS_EXTRUDE
  if(target[E_AXIS]!=position[E_AXIS])
  {
    if(degHotend(active_extruder)<extrude_min_temp)
    {
      position[E_AXIS]=target[E_AXIS]; //behave as if the move really took place, but ignore E part
#ifdef LIN_ADVANCE
      position_float[E_AXIS] = e;
      de_float = 0;
#endif
      SERIAL_ECHO_START;
      SERIAL_ECHOLNRPGM(_n(" cold extrusion prevented"));////MSG_ERR_COLD_EXTRUDE_STOP c=0 r=0
    }
    
    #ifdef PREVENT_LENGTHY_EXTRUDE
    if(labs(target[E_AXIS]-position[E_AXIS])>cs.axis_steps_per_unit[E_AXIS]*EXTRUDE_MAXLENGTH)
    {
      position[E_AXIS]=target[E_AXIS]; //behave as if the move really took place, but ignore E part
#ifdef LIN_ADVANCE
        position_float[E_AXIS] = e;
        de_float = 0;
#endif
      SERIAL_ECHO_START;
      SERIAL_ECHOLNRPGM(_n(" too long extrusion prevented"));////MSG_ERR_LONG_EXTRUDE_STOP c=0 r=0
    }
    #endif
  }
  #endif

  // Prepare to set up new block
  block_t *block = &block_buffer[block_buffer_head];

  // Set sdlen for calculating sd position
  block->sdlen = 0;

  // Mark block as not busy (Not executed by the stepper interrupt, could be still tinkered with.)
  block->busy = false;

  // Number of steps for each axis
#ifndef COREXY
// default non-h-bot planning
block->steps_x.wide = labs(target[X_AXIS]-position[X_AXIS]);
block->steps_y.wide = labs(target[Y_AXIS]-position[Y_AXIS]);
#else
// corexy planning
// these equations follow the form of the dA and dB equations on http://www.corexy.com/theory.html
block->steps_x.wide = labs((target[X_AXIS]-position[X_AXIS]) + (target[Y_AXIS]-position[Y_AXIS]));
block->steps_y.wide = labs((target[X_AXIS]-position[X_AXIS]) - (target[Y_AXIS]-position[Y_AXIS]));
#endif
  block->steps_z.wide = labs(target[Z_AXIS]-position[Z_AXIS]);
  block->steps_e.wide = labs(target[E_AXIS]-position[E_AXIS]);
  block->step_event_count.wide = max(block->steps_x.wide, max(block->steps_y.wide, max(block->steps_z.wide, block->steps_e.wide)));

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
  if (target[X_AXIS] < position[X_AXIS])
  {
    block->direction_bits |= (1<<X_AXIS); 
  }
  if (target[Y_AXIS] < position[Y_AXIS])
  {
    block->direction_bits |= (1<<Y_AXIS); 
  }
#else
  if ((target[X_AXIS]-position[X_AXIS]) + (target[Y_AXIS]-position[Y_AXIS]) < 0)
  {
    block->direction_bits |= (1<<X_AXIS); 
  }
  if ((target[X_AXIS]-position[X_AXIS]) - (target[Y_AXIS]-position[Y_AXIS]) < 0)
  {
    block->direction_bits |= (1<<Y_AXIS); 
  }
#endif
  if (target[Z_AXIS] < position[Z_AXIS])
  {
    block->direction_bits |= (1<<Z_AXIS); 
  }
  if (target[E_AXIS] < position[E_AXIS])
  {
    block->direction_bits |= (1<<E_AXIS); 
  }

  block->active_extruder = extruder;

  //enable active axes
  #ifdef COREXY
  if((block->steps_x.wide != 0) || (block->steps_y.wide != 0))
  {
    enable_x();
    enable_y();
  }
  #else
  if(block->steps_x.wide != 0) enable_x();
  if(block->steps_y.wide != 0) enable_y();
  #endif
  if(block->steps_z.wide != 0) enable_z();

  // Enable extruder(s)
  if(block->steps_e.wide != 0)
  {
    if (DISABLE_INACTIVE_EXTRUDER) //enable only selected extruder
    {

      if(g_uc_extruder_last_move[0] > 0) g_uc_extruder_last_move[0]--;
      if(g_uc_extruder_last_move[1] > 0) g_uc_extruder_last_move[1]--;
      if(g_uc_extruder_last_move[2] > 0) g_uc_extruder_last_move[2]--;
      
      switch(extruder)
      {
        case 0: 
          enable_e0(); 
          g_uc_extruder_last_move[0] = BLOCK_BUFFER_SIZE*2;
          
          if(g_uc_extruder_last_move[1] == 0) {disable_e1();}
          if(g_uc_extruder_last_move[2] == 0) {disable_e2();}
        break;
        case 1:
          enable_e1(); 
          g_uc_extruder_last_move[1] = BLOCK_BUFFER_SIZE*2;
          
          if(g_uc_extruder_last_move[0] == 0) {disable_e0();}
          if(g_uc_extruder_last_move[2] == 0) {disable_e2();}
        break;
        case 2:
          enable_e2(); 
          g_uc_extruder_last_move[2] = BLOCK_BUFFER_SIZE*2;
          
          if(g_uc_extruder_last_move[0] == 0) {disable_e0();}
          if(g_uc_extruder_last_move[1] == 0) {disable_e1();}
        break;        
      }
    }
    else //enable all
    {
      enable_e0();
      enable_e1();
      enable_e2(); 
    }
  }

  if (block->steps_e.wide == 0)
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
    delta_mm[X_AXIS] = (target[X_AXIS]-position[X_AXIS])/cs.axis_steps_per_unit[X_AXIS];
    delta_mm[Y_AXIS] = (target[Y_AXIS]-position[Y_AXIS])/cs.axis_steps_per_unit[Y_AXIS];
  #else
    float delta_mm[6];
    delta_mm[X_HEAD] = (target[X_AXIS]-position[X_AXIS])/cs.axis_steps_per_unit[X_AXIS];
    delta_mm[Y_HEAD] = (target[Y_AXIS]-position[Y_AXIS])/cs.axis_steps_per_unit[Y_AXIS];
    delta_mm[X_AXIS] = ((target[X_AXIS]-position[X_AXIS]) + (target[Y_AXIS]-position[Y_AXIS]))/cs.axis_steps_per_unit[X_AXIS];
    delta_mm[Y_AXIS] = ((target[X_AXIS]-position[X_AXIS]) - (target[Y_AXIS]-position[Y_AXIS]))/cs.axis_steps_per_unit[Y_AXIS];
  #endif
  delta_mm[Z_AXIS] = (target[Z_AXIS]-position[Z_AXIS])/cs.axis_steps_per_unit[Z_AXIS];
  delta_mm[E_AXIS] = (target[E_AXIS]-position[E_AXIS])/cs.axis_steps_per_unit[E_AXIS];
  if ( block->steps_x.wide <=dropsegments && block->steps_y.wide <=dropsegments && block->steps_z.wide <=dropsegments )
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

  int moves_queued = moves_planned();

  // slow down when de buffer starts to empty, rather than wait at the corner for a buffer refill
#ifdef SLOWDOWN
  //FIXME Vojtech: Why moves_queued > 1? Why not >=1?
  // Can we somehow differentiate the filling of the buffer at the start of a g-code from a buffer draining situation?
  if (moves_queued > 1 && moves_queued < (BLOCK_BUFFER_SIZE >> 1)) {
      // segment time in micro seconds
      unsigned long segment_time = lround(1000000.0/inverse_second);
      if (segment_time < cs.minsegmenttime)
          // buffer is draining, add extra time.  The amount of time added increases if the buffer is still emptied more.
          inverse_second=1000000.0/(segment_time+lround(2*(cs.minsegmenttime-segment_time)/moves_queued));
  }
#endif // SLOWDOWN

  block->nominal_speed = block->millimeters * inverse_second; // (mm/sec) Always > 0
  block->nominal_rate = ceil(block->step_event_count.wide * inverse_second); // (step/sec) Always > 0

  // Calculate and limit speed in mm/sec for each axis
  float current_speed[4];
  float speed_factor = 1.0; //factor <=1 do decrease speed
//  maxlimit_status &= ~0xf;
  for(int i=0; i < 4; i++)
  {
    current_speed[i] = delta_mm[i] * inverse_second;
	if(fabs(current_speed[i]) > max_feedrate[i])
	{
      speed_factor = min(speed_factor, max_feedrate[i] / fabs(current_speed[i]));
	  maxlimit_status |= (1 << i);
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

  // Compute and limit the acceleration rate for the trapezoid generator.  
  // block->step_event_count ... event count of the fastest axis
  // block->millimeters ... Euclidian length of the XYZ movement or the E length, if no XYZ movement.
  float steps_per_mm = block->step_event_count.wide/block->millimeters;
  if(block->steps_x.wide == 0 && block->steps_y.wide == 0 && block->steps_z.wide == 0)
  {
    block->acceleration_st = ceil(cs.retract_acceleration * steps_per_mm); // convert to: acceleration steps/sec^2
  }
  else
  {
    block->acceleration_st = ceil(cs.acceleration * steps_per_mm); // convert to: acceleration steps/sec^2
    // Limit acceleration per axis
    //FIXME Vojtech: One shall rather limit a projection of the acceleration vector instead of using the limit.
    if(((float)block->acceleration_st * (float)block->steps_x.wide / (float)block->step_event_count.wide) > axis_steps_per_sqr_second[X_AXIS])
	{  block->acceleration_st = axis_steps_per_sqr_second[X_AXIS]; maxlimit_status |= (X_AXIS_MASK << 4); }
    if(((float)block->acceleration_st * (float)block->steps_y.wide / (float)block->step_event_count.wide) > axis_steps_per_sqr_second[Y_AXIS])
	{  block->acceleration_st = axis_steps_per_sqr_second[Y_AXIS]; maxlimit_status |= (Y_AXIS_MASK << 4); }
    if(((float)block->acceleration_st * (float)block->steps_e.wide / (float)block->step_event_count.wide) > axis_steps_per_sqr_second[E_AXIS])
	{  block->acceleration_st = axis_steps_per_sqr_second[E_AXIS]; maxlimit_status |= (Z_AXIS_MASK << 4); }
    if(((float)block->acceleration_st * (float)block->steps_z.wide / (float)block->step_event_count.wide ) > axis_steps_per_sqr_second[Z_AXIS])
	{  block->acceleration_st = axis_steps_per_sqr_second[Z_AXIS]; maxlimit_status |= (E_AXIS_MASK << 4); }
  }
  // Acceleration of the segment, in mm/sec^2
  block->acceleration = block->acceleration_st / steps_per_mm;

#if 0
  // Oversample diagonal movements by a power of 2 up to 8x
  // to achieve more accurate diagonal movements.
  uint8_t bresenham_oversample = 1;
  for (uint8_t i = 0; i < 3; ++ i) {
    if (block->nominal_rate >= 5000) // 5kHz
      break;
    block->nominal_rate << 1;
    bresenham_oversample << 1;
    block->step_event_count << 1;
  }
  if (bresenham_oversample > 1)
    // Lower the acceleration steps/sec^2 to account for the oversampling.
    block->acceleration_st = (block->acceleration_st + (bresenham_oversample >> 1)) / bresenham_oversample;
#endif

  block->acceleration_rate = (long)((float)block->acceleration_st * (16777216.0 / (F_CPU / 8.0)));

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

  // Reset the block flag.
  block->flag = 0;

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

#ifdef LIN_ADVANCE

    //
    // Use LIN_ADVANCE for blocks if all these are true:
    //
    // esteps                                          : We have E steps todo (a printing move)
    //
    // block->steps[X_AXIS] || block->steps[Y_AXIS]    : We have a movement in XY direction (i.e., not retract / prime).
    //
    // extruder_advance_k                              : There is an advance factor set.
    //
    // block->steps[E_AXIS] != block->step_event_count : A problem occurs if the move before a retract is too small.
    //                                                   In that case, the retract and move will be executed together.
    //                                                   This leads to too many advance steps due to a huge e_acceleration.
    //                                                   The math is good, but we must avoid retract moves with advance!
    // de_float > 0.0                                  : Extruder is running forward (e.g., for "Wipe while retracting" (Slic3r) or "Combing" (Cura) moves)
    //
    block->use_advance_lead =  block->steps_e.wide
                           && (block->steps_x.wide || block->steps_y.wide)
                           && extruder_advance_k
                           && (uint32_t)block->steps_e.wide != block->step_event_count.wide
                           && de_float > 0.0;
    if (block->use_advance_lead)
        block->abs_adv_steps_multiplier8 = lround(
                          extruder_advance_k
                          * ((advance_ed_ratio < 0.000001) ? de_float / mm_D_float : advance_ed_ratio) // Use the fixed ratio, if set
                          * (block->nominal_speed / (float)block->nominal_rate)
                          * cs.axis_steps_per_unit[E_AXIS] * 256.0
                          );
#endif
    
  // Precalculate the division, so when all the trapezoids in the planner queue get recalculated, the division is not repeated.
  block->speed_factor = block->nominal_rate / block->nominal_speed;
  calculate_trapezoid_for_block(block, block->entry_speed, safe_speed);

  if (block->step_event_count.wide <= 32767)
    block->flag |= BLOCK_FLAG_DDA_LOWRES;

  // Move the buffer head. From now the block may be picked up by the stepper interrupt controller.
  block_buffer_head = next_buffer_head;

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

    // Apply the machine correction matrix.
    if (world2machine_correction_mode != WORLD2MACHINE_CORRECTION_NONE)
    {
        float tmpx = x;
        float tmpy = y;
        x = world2machine_rotation_and_skew[0][0] * tmpx + world2machine_rotation_and_skew[0][1] * tmpy + world2machine_shift[0];
        y = world2machine_rotation_and_skew[1][0] * tmpx + world2machine_rotation_and_skew[1][1] * tmpy + world2machine_shift[1];
    }

  position[X_AXIS] = lround(x*cs.axis_steps_per_unit[X_AXIS]);
  position[Y_AXIS] = lround(y*cs.axis_steps_per_unit[Y_AXIS]);
#ifdef MESH_BED_LEVELING
  position[Z_AXIS] = mbl.active ? 
    lround((z+mbl.get_z(x, y))*cs.axis_steps_per_unit[Z_AXIS]) :
    lround(z*cs.axis_steps_per_unit[Z_AXIS]);
#else
  position[Z_AXIS] = lround(z*cs.axis_steps_per_unit[Z_AXIS]);
#endif // ENABLE_MESH_BED_LEVELING
  position[E_AXIS] = lround(e*cs.axis_steps_per_unit[E_AXIS]);
#ifdef LIN_ADVANCE
  position_float[X_AXIS] = x;
  position_float[Y_AXIS] = y;
  position_float[Z_AXIS] = z;
  position_float[E_AXIS] = e;
#endif
  st_set_position(position[X_AXIS], position[Y_AXIS], position[Z_AXIS], position[E_AXIS]);
  previous_nominal_speed = 0.0; // Resets planner junction speeds. Assumes start from rest.
  previous_speed[0] = 0.0;
  previous_speed[1] = 0.0;
  previous_speed[2] = 0.0;
  previous_speed[3] = 0.0;
}

// Only useful in the bed leveling routine, when the mesh bed leveling is off.
void plan_set_z_position(const float &z)
{
	#ifdef LIN_ADVANCE
	position_float[Z_AXIS] = z;
	#endif
    position[Z_AXIS] = lround(z*cs.axis_steps_per_unit[Z_AXIS]);
    st_set_position(position[X_AXIS], position[Y_AXIS], position[Z_AXIS], position[E_AXIS]);
}

void plan_set_e_position(const float &e)
{
  #ifdef LIN_ADVANCE
  position_float[E_AXIS] = e;
  #endif
  position[E_AXIS] = lround(e*cs.axis_steps_per_unit[E_AXIS]);  
  st_set_e_position(position[E_AXIS]);
}

#ifdef PREVENT_DANGEROUS_EXTRUDE
void set_extrude_min_temp(float temp)
{
  extrude_min_temp=temp;
}
#endif

// Calculate the steps/s^2 acceleration rates, based on the mm/s^s
void reset_acceleration_rates()
{
	for(int8_t i=0; i < NUM_AXIS; i++)
        axis_steps_per_sqr_second[i] = max_acceleration_units_per_sq_second[i] * cs.axis_steps_per_unit[i];
}

#ifdef TMC2130
void update_mode_profile()
{
	if (tmc2130_mode == TMC2130_MODE_NORMAL)
	{
		max_feedrate = cs.max_feedrate_normal;
		max_acceleration_units_per_sq_second = cs.max_acceleration_units_per_sq_second_normal;
	}
	else if (tmc2130_mode == TMC2130_MODE_SILENT)
	{
		max_feedrate = cs.max_feedrate_silent;
		max_acceleration_units_per_sq_second = cs.max_acceleration_units_per_sq_second_silent;
	}
	reset_acceleration_rates();
}
#endif //TMC2130

unsigned char number_of_blocks()
{
	return (block_buffer_head + BLOCK_BUFFER_SIZE - block_buffer_tail) & (BLOCK_BUFFER_SIZE - 1);
}
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
	unsigned char _block_buffer_head = block_buffer_head;
	unsigned char _block_buffer_tail = block_buffer_tail;
	uint16_t sdlen = 0;
	while (_block_buffer_head != _block_buffer_tail)
	{
		sdlen += block_buffer[_block_buffer_tail].sdlen;
	    _block_buffer_tail = (_block_buffer_tail + 1) & (BLOCK_BUFFER_SIZE - 1);  
	}
	return sdlen;
}
