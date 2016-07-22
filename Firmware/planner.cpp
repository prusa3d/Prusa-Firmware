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

#ifdef MESH_BED_LEVELING
#include "mesh_bed_leveling.h"
#include "mesh_bed_calibration.h"
#endif

//===========================================================================
//=============================public variables ============================
//===========================================================================

unsigned long minsegmenttime;
float max_feedrate[NUM_AXIS]; // set the max speeds
float axis_steps_per_unit[NUM_AXIS];
unsigned long max_acceleration_units_per_sq_second[NUM_AXIS]; // Use M201 to override by software
float minimumfeedrate;
float acceleration;         // Normal acceleration mm/s^2  THIS IS THE DEFAULT ACCELERATION for all moves. M204 SXXXX
float retract_acceleration; //  mm/s^2   filament pull-pack and push-forward  while standing still in the other axis M204 TXXXX
// Jerk is a maximum immediate velocity change.
float max_jerk[NUM_AXIS];
float mintravelfeedrate;
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

//===========================================================================
//=============================private variables ============================
//===========================================================================
#ifdef PREVENT_DANGEROUS_EXTRUDE
float extrude_min_temp=EXTRUDE_MINTEMP;
#endif

#ifdef FILAMENT_SENSOR
 static char meas_sample; //temporary variable to hold filament measurement sample
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

// Calculates trapezoid parameters so that the entry- and exit-speed is compensated by the provided factors.

void calculate_trapezoid_for_block(block_t *block, float entry_factor, float exit_factor) {
  unsigned long initial_rate = ceil(block->nominal_rate*entry_factor); // (step/min)
  unsigned long final_rate = ceil(block->nominal_rate*exit_factor); // (step/min)

  // Limit minimal step rate (Otherwise the timer will overflow.)
  if(initial_rate <120) {
    initial_rate=120; 
  }
  if(final_rate < 120) {
    final_rate=120;  
  }

  long acceleration = block->acceleration_st;
  int32_t accelerate_steps =
    ceil(estimate_acceleration_distance(initial_rate, block->nominal_rate, acceleration));
  int32_t decelerate_steps =
    floor(estimate_acceleration_distance(block->nominal_rate, final_rate, -acceleration));

  // Calculate the size of Plateau of Nominal Rate.
  int32_t plateau_steps = block->step_event_count-accelerate_steps-decelerate_steps;

  // Is the Plateau of Nominal Rate smaller than nothing? That means no cruising, and we will
  // have to use intersection_distance() to calculate when to abort acceleration and start braking
  // in order to reach the final_rate exactly at the end of this block.
  if (plateau_steps < 0) {
    accelerate_steps = ceil(intersection_distance(initial_rate, final_rate, acceleration, block->step_event_count));
    accelerate_steps = max(accelerate_steps,0); // Check limits due to numerical round-off
    accelerate_steps = min((uint32_t)accelerate_steps,block->step_event_count);//(We can cast here to unsigned, because the above line ensures that we are above zero)
    plateau_steps = 0;
  }

#ifdef ADVANCE
  volatile long initial_advance = block->advance*entry_factor*entry_factor; 
  volatile long final_advance = block->advance*exit_factor*exit_factor;
#endif // ADVANCE

  // block->accelerate_until = accelerate_steps;
  // block->decelerate_after = accelerate_steps+plateau_steps;
  CRITICAL_SECTION_START;  // Fill variables used by the stepper in a critical section
  if (! block->busy) { // Don't update variables if block is busy.
    block->accelerate_until = accelerate_steps;
    block->decelerate_after = accelerate_steps+plateau_steps;
    block->initial_rate = initial_rate;
    block->final_rate = final_rate;
#ifdef ADVANCE
    block->initial_advance = initial_advance;
    block->final_advance = final_advance;
#endif //ADVANCE
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
                current->entry_speed = ((current->flag & BLOCK_FLAG_NOMINAL_LENGTH) || current->max_entry_speed <= next->entry_speed) ?
                    current->max_entry_speed :
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
                calculate_trapezoid_for_block(prev, prev->entry_speed/prev->nominal_speed, current->entry_speed/prev->nominal_speed);
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
    calculate_trapezoid_for_block(current, current->entry_speed/current->nominal_speed, safe_final_speed/current->nominal_speed);
    current->flag &= ~BLOCK_FLAG_RECALCULATE;

//    SERIAL_ECHOLNPGM("planner_recalculate - 4");
}

void plan_init() {
  block_buffer_head = 0;
  block_buffer_tail = 0;
  memset(position, 0, sizeof(position)); // clear position
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
    if((block_buffer[block_index].steps_x != 0) ||
      (block_buffer[block_index].steps_y != 0) ||
      (block_buffer[block_index].steps_z != 0)) {
      float se=(float(block_buffer[block_index].steps_e)/float(block_buffer[block_index].step_event_count))*block_buffer[block_index].nominal_speed;
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
      if(block->steps_x != 0) x_active++;
      if(block->steps_y != 0) y_active++;
      if(block->steps_z != 0) z_active++;
      if(block->steps_e != 0) e_active++;
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
  while(block_buffer_tail == next_buffer_head)
  {
    manage_heater(); 
    // Vojtech: Don't disable motors inside the planner!
    manage_inactivity(false); 
    lcd_update();
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
  target[X_AXIS] = lround(x*axis_steps_per_unit[X_AXIS]);
  target[Y_AXIS] = lround(y*axis_steps_per_unit[Y_AXIS]);
#ifdef MESH_BED_LEVELING
    if (mbl.active){
        target[Z_AXIS] = lround((z+mbl.get_z(x, y))*axis_steps_per_unit[Z_AXIS]);
    }else{
        target[Z_AXIS] = lround(z*axis_steps_per_unit[Z_AXIS]);
    }
#else
    target[Z_AXIS] = lround(z*axis_steps_per_unit[Z_AXIS]);
#endif // ENABLE_MESH_BED_LEVELING
  target[E_AXIS] = lround(e*axis_steps_per_unit[E_AXIS]);
  #ifdef PREVENT_DANGEROUS_EXTRUDE
  if(target[E_AXIS]!=position[E_AXIS])
  {
    if(degHotend(active_extruder)<extrude_min_temp)
    {
      position[E_AXIS]=target[E_AXIS]; //behave as if the move really took place, but ignore E part
      SERIAL_ECHO_START;
      SERIAL_ECHOLNRPGM(MSG_ERR_COLD_EXTRUDE_STOP);
    }
    
    #ifdef PREVENT_LENGTHY_EXTRUDE
    if(labs(target[E_AXIS]-position[E_AXIS])>axis_steps_per_unit[E_AXIS]*EXTRUDE_MAXLENGTH)
    {
      position[E_AXIS]=target[E_AXIS]; //behave as if the move really took place, but ignore E part
      SERIAL_ECHO_START;
      SERIAL_ECHOLNRPGM(MSG_ERR_LONG_EXTRUDE_STOP);
    }
    #endif
  }
  #endif

  // Prepare to set up new block
  block_t *block = &block_buffer[block_buffer_head];

  // Mark block as not busy (Not executed by the stepper interrupt, could be still tinkered with.)
  block->busy = false;

  // Number of steps for each axis
#ifndef COREXY
// default non-h-bot planning
block->steps_x = labs(target[X_AXIS]-position[X_AXIS]);
block->steps_y = labs(target[Y_AXIS]-position[Y_AXIS]);
#else
// corexy planning
// these equations follow the form of the dA and dB equations on http://www.corexy.com/theory.html
block->steps_x = labs((target[X_AXIS]-position[X_AXIS]) + (target[Y_AXIS]-position[Y_AXIS]));
block->steps_y = labs((target[X_AXIS]-position[X_AXIS]) - (target[Y_AXIS]-position[Y_AXIS]));
#endif
  block->steps_z = labs(target[Z_AXIS]-position[Z_AXIS]);
  block->steps_e = labs(target[E_AXIS]-position[E_AXIS]);
  block->steps_e *= volumetric_multiplier[active_extruder];
  block->steps_e *= extrudemultiply;
  block->steps_e /= 100;
  block->step_event_count = max(block->steps_x, max(block->steps_y, max(block->steps_z, block->steps_e)));

  // Bail if this is a zero-length block
  if (block->step_event_count <= dropsegments)
  { 
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
  if((block->steps_x != 0) || (block->steps_y != 0))
  {
    enable_x();
    enable_y();
  }
  #else
  if(block->steps_x != 0) enable_x();
  if(block->steps_y != 0) enable_y();
  #endif
#ifndef Z_LATE_ENABLE
  if(block->steps_z != 0) enable_z();
#endif

  // Enable extruder(s)
  if(block->steps_e != 0)
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
          
          if(g_uc_extruder_last_move[1] == 0) disable_e1(); 
          if(g_uc_extruder_last_move[2] == 0) disable_e2(); 
        break;
        case 1:
          enable_e1(); 
          g_uc_extruder_last_move[1] = BLOCK_BUFFER_SIZE*2;
          
          if(g_uc_extruder_last_move[0] == 0) disable_e0(); 
          if(g_uc_extruder_last_move[2] == 0) disable_e2(); 
        break;
        case 2:
          enable_e2(); 
          g_uc_extruder_last_move[2] = BLOCK_BUFFER_SIZE*2;
          
          if(g_uc_extruder_last_move[0] == 0) disable_e0(); 
          if(g_uc_extruder_last_move[1] == 0) disable_e1(); 
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

  if (block->steps_e == 0)
  {
    if(feed_rate<mintravelfeedrate) feed_rate=mintravelfeedrate;
  }
  else
  {
    if(feed_rate<minimumfeedrate) feed_rate=minimumfeedrate;
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
    delta_mm[X_AXIS] = (target[X_AXIS]-position[X_AXIS])/axis_steps_per_unit[X_AXIS];
    delta_mm[Y_AXIS] = (target[Y_AXIS]-position[Y_AXIS])/axis_steps_per_unit[Y_AXIS];
  #else
    float delta_mm[6];
    delta_mm[X_HEAD] = (target[X_AXIS]-position[X_AXIS])/axis_steps_per_unit[X_AXIS];
    delta_mm[Y_HEAD] = (target[Y_AXIS]-position[Y_AXIS])/axis_steps_per_unit[Y_AXIS];
    delta_mm[X_AXIS] = ((target[X_AXIS]-position[X_AXIS]) + (target[Y_AXIS]-position[Y_AXIS]))/axis_steps_per_unit[X_AXIS];
    delta_mm[Y_AXIS] = ((target[X_AXIS]-position[X_AXIS]) - (target[Y_AXIS]-position[Y_AXIS]))/axis_steps_per_unit[Y_AXIS];
  #endif
  delta_mm[Z_AXIS] = (target[Z_AXIS]-position[Z_AXIS])/axis_steps_per_unit[Z_AXIS];
  delta_mm[E_AXIS] = ((target[E_AXIS]-position[E_AXIS])/axis_steps_per_unit[E_AXIS])*volumetric_multiplier[active_extruder]*extrudemultiply/100.0;
  if ( block->steps_x <=dropsegments && block->steps_y <=dropsegments && block->steps_z <=dropsegments )
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
      if (segment_time < minsegmenttime)
          // buffer is draining, add extra time.  The amount of time added increases if the buffer is still emptied more.
          inverse_second=1000000.0/(segment_time+lround(2*(minsegmenttime-segment_time)/moves_queued));
  }
#endif // SLOWDOWN

  block->nominal_speed = block->millimeters * inverse_second; // (mm/sec) Always > 0
  block->nominal_rate = ceil(block->step_event_count * inverse_second); // (step/sec) Always > 0

#ifdef FILAMENT_SENSOR
  //FMM update ring buffer used for delay with filament measurements
  
  
    if((extruder==FILAMENT_SENSOR_EXTRUDER_NUM) && (delay_index2 > -1))  //only for extruder with filament sensor and if ring buffer is initialized
  	  {
    delay_dist = delay_dist + delta_mm[E_AXIS];  //increment counter with next move in e axis
  
    while (delay_dist >= (10*(MAX_MEASUREMENT_DELAY+1)))  //check if counter is over max buffer size in mm
      	  delay_dist = delay_dist - 10*(MAX_MEASUREMENT_DELAY+1);  //loop around the buffer
    while (delay_dist<0)
    	  delay_dist = delay_dist + 10*(MAX_MEASUREMENT_DELAY+1); //loop around the buffer
      
    delay_index1=delay_dist/10.0;  //calculate index
    
    //ensure the number is within range of the array after converting from floating point
    if(delay_index1<0)
    	delay_index1=0;
    else if (delay_index1>MAX_MEASUREMENT_DELAY)
    	delay_index1=MAX_MEASUREMENT_DELAY;
    	
    if(delay_index1 != delay_index2)  //moved index
  	  {
    	meas_sample=widthFil_to_size_ratio()-100;  //subtract off 100 to reduce magnitude - to store in a signed char
  	  }
    while( delay_index1 != delay_index2)
  	  {
  	  delay_index2 = delay_index2 + 1;
  	if(delay_index2>MAX_MEASUREMENT_DELAY)
  			  delay_index2=delay_index2-(MAX_MEASUREMENT_DELAY+1);  //loop around buffer when incrementing
  	  if(delay_index2<0)
  		delay_index2=0;
  	  else if (delay_index2>MAX_MEASUREMENT_DELAY)
  		delay_index2=MAX_MEASUREMENT_DELAY;  
  	  
  	  measurement_delay[delay_index2]=meas_sample;
  	  }
    	
    
  	  }
#endif


  // Calculate and limit speed in mm/sec for each axis
  float current_speed[4];
  float speed_factor = 1.0; //factor <=1 do decrease speed
  for(int i=0; i < 4; i++)
  {
    current_speed[i] = delta_mm[i] * inverse_second;
    if(fabs(current_speed[i]) > max_feedrate[i])
      speed_factor = min(speed_factor, max_feedrate[i] / fabs(current_speed[i]));
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
  float steps_per_mm = block->step_event_count/block->millimeters;
  if(block->steps_x == 0 && block->steps_y == 0 && block->steps_z == 0)
  {
    block->acceleration_st = ceil(retract_acceleration * steps_per_mm); // convert to: acceleration steps/sec^2
  }
  else
  {
    block->acceleration_st = ceil(acceleration * steps_per_mm); // convert to: acceleration steps/sec^2
    // Limit acceleration per axis
    //FIXME Vojtech: One shall rather limit a projection of the acceleration vector instead of using the limit.
    if(((float)block->acceleration_st * (float)block->steps_x / (float)block->step_event_count) > axis_steps_per_sqr_second[X_AXIS])
      block->acceleration_st = axis_steps_per_sqr_second[X_AXIS];
    if(((float)block->acceleration_st * (float)block->steps_y / (float)block->step_event_count) > axis_steps_per_sqr_second[Y_AXIS])
      block->acceleration_st = axis_steps_per_sqr_second[Y_AXIS];
    if(((float)block->acceleration_st * (float)block->steps_e / (float)block->step_event_count) > axis_steps_per_sqr_second[E_AXIS])
      block->acceleration_st = axis_steps_per_sqr_second[E_AXIS];
    if(((float)block->acceleration_st * (float)block->steps_z / (float)block->step_event_count ) > axis_steps_per_sqr_second[Z_AXIS])
      block->acceleration_st = axis_steps_per_sqr_second[Z_AXIS];
  }
  block->acceleration = block->acceleration_st / steps_per_mm;
  block->acceleration_rate = (long)((float)block->acceleration_st * (16777216.0 / (F_CPU / 8.0)));

#if 0  // Use old jerk for now
  // Compute path unit vector
  double unit_vec[3];

  unit_vec[X_AXIS] = delta_mm[X_AXIS]*inverse_millimeters;
  unit_vec[Y_AXIS] = delta_mm[Y_AXIS]*inverse_millimeters;
  unit_vec[Z_AXIS] = delta_mm[Z_AXIS]*inverse_millimeters;

  // Compute maximum allowable entry speed at junction by centripetal acceleration approximation.
  // Let a circle be tangent to both previous and current path line segments, where the junction
  // deviation is defined as the distance from the junction to the closest edge of the circle,
  // colinear with the circle center. The circular segment joining the two paths represents the
  // path of centripetal acceleration. Solve for max velocity based on max acceleration about the
  // radius of the circle, defined indirectly by junction deviation. This may be also viewed as
  // path width or max_jerk in the previous grbl version. This approach does not actually deviate
  // from path, but used as a robust way to compute cornering speeds, as it takes into account the
  // nonlinearities of both the junction angle and junction velocity.
  double vmax_junction = MINIMUM_PLANNER_SPEED; // Set default max junction speed

  // Skip first block or when previous_nominal_speed is used as a flag for homing and offset cycles.
  if ((block_buffer_head != block_buffer_tail) && (previous_nominal_speed > 0.0)) {
    // Compute cosine of angle between previous and current path. (prev_unit_vec is negative)
    // NOTE: Max junction velocity is computed without sin() or acos() by trig half angle identity.
    double cos_theta = - previous_unit_vec[X_AXIS] * unit_vec[X_AXIS]
      - previous_unit_vec[Y_AXIS] * unit_vec[Y_AXIS]
      - previous_unit_vec[Z_AXIS] * unit_vec[Z_AXIS] ;

    // Skip and use default max junction speed for 0 degree acute junction.
    if (cos_theta < 0.95) {
      vmax_junction = min(previous_nominal_speed,block->nominal_speed);
      // Skip and avoid divide by zero for straight junctions at 180 degrees. Limit to min() of nominal speeds.
      if (cos_theta > -0.95) {
        // Compute maximum junction velocity based on maximum acceleration and junction deviation
        double sin_theta_d2 = sqrt(0.5*(1.0-cos_theta)); // Trig half angle identity. Always positive.
        vmax_junction = min(vmax_junction,
        sqrt(block->acceleration * junction_deviation * sin_theta_d2/(1.0-sin_theta_d2)) );
      }
    }
  }
#endif
  // Start with a safe speed.
  // Safe speed is the speed, from which the machine may halt to stop immediately.
  float safe_speed = block->nominal_speed;
  bool  limited = false;
  for (uint8_t axis = 0; axis < 4; ++ axis) {
      float jerk = fabs(current_speed[axis]);
      if (jerk > max_jerk[axis]) {
          // The actual jerk is lower, if it has been limited by the XY jerk.
          if (limited) {
              // Spare one division by a following gymnastics:
              // Instead of jerk *= safe_speed / block->nominal_speed,
              // multiply max_jerk[axis] by the divisor.
              jerk *= safe_speed;
              float mjerk = max_jerk[axis] * block->nominal_speed;
              if (jerk > mjerk) {
                  safe_speed *= mjerk / jerk;
                  limited = true;
              }
          } else {
              safe_speed = max_jerk[axis];
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
          if (jerk > max_jerk[axis]) {
              v_factor *= max_jerk[axis] / jerk;
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

#ifdef ADVANCE
  // Calculate advance rate
  if((block->steps_e == 0) || (block->steps_x == 0 && block->steps_y == 0 && block->steps_z == 0)) {
    block->advance_rate = 0;
    block->advance = 0;
  }
  else {
    long acc_dist = estimate_acceleration_distance(0, block->nominal_rate, block->acceleration_st);
    float advance = (STEPS_PER_CUBIC_MM_E * EXTRUDER_ADVANCE_K) * 
      (current_speed[E_AXIS] * current_speed[E_AXIS] * EXTRUSION_AREA * EXTRUSION_AREA)*256;
    block->advance = advance;
    if(acc_dist == 0) {
      block->advance_rate = 0;
    } 
    else {
      block->advance_rate = advance / (float)acc_dist;
    }
  }
  /*
    SERIAL_ECHO_START;
   SERIAL_ECHOPGM("advance :");
   SERIAL_ECHO(block->advance/256.0);
   SERIAL_ECHOPGM("advance rate :");
   SERIAL_ECHOLN(block->advance_rate/256.0);
   */
#endif // ADVANCE

  calculate_trapezoid_for_block(block, block->entry_speed/block->nominal_speed, safe_speed/block->nominal_speed);

  // Move the buffer head. From now the block may be picked up by the stepper interrupt controller.
  block_buffer_head = next_buffer_head;

  // Update position
  memcpy(position, target, sizeof(target)); // position[] = target[]

  // Recalculate the trapezoids to maximize speed at the segment transitions while respecting
  // the machine limits (maximum acceleration and maximum jerk).
  // This runs asynchronously with the stepper interrupt controller, which may
  // interfere with the process.
  planner_recalculate(safe_speed);

//  SERIAL_ECHOPGM("Q");
//  SERIAL_ECHO(int(moves_planned()));
//  SERIAL_ECHOLNPGM("");

  st_wake_up();
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
    {
        float tmpx = x;
        float tmpy = y;
        x = world2machine_rotation_and_skew[0][0] * tmpx + world2machine_rotation_and_skew[0][1] * tmpy + world2machine_shift[0];
        y = world2machine_rotation_and_skew[1][0] * tmpx + world2machine_rotation_and_skew[1][1] * tmpy + world2machine_shift[1];
    }

  position[X_AXIS] = lround(x*axis_steps_per_unit[X_AXIS]);
  position[Y_AXIS] = lround(y*axis_steps_per_unit[Y_AXIS]);
#ifdef MESH_BED_LEVELING
    if (mbl.active){
      position[Z_AXIS] = lround((z+mbl.get_z(x, y))*axis_steps_per_unit[Z_AXIS]);
    }else{
        position[Z_AXIS] = lround(z*axis_steps_per_unit[Z_AXIS]);
    }
#else
  position[Z_AXIS] = lround(z*axis_steps_per_unit[Z_AXIS]);
#endif // ENABLE_MESH_BED_LEVELING
  position[E_AXIS] = lround(e*axis_steps_per_unit[E_AXIS]);  
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
    position[Z_AXIS] = lround(z*axis_steps_per_unit[Z_AXIS]);
    st_set_position(position[X_AXIS], position[Y_AXIS], position[Z_AXIS], position[E_AXIS]);
}

void plan_set_e_position(const float &e)
{
  position[E_AXIS] = lround(e*axis_steps_per_unit[E_AXIS]);  
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
        {
        axis_steps_per_sqr_second[i] = max_acceleration_units_per_sq_second[i] * axis_steps_per_unit[i];
        }
}
