/*
  planner.h - buffers movement commands and manages the acceleration profile plan
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

// This module is to be considered a sub-module of stepper.c. Please don't include 
// this file from any other module.

#ifndef planner_h
#define planner_h

#include "Marlin.h"

#ifdef ENABLE_AUTO_BED_LEVELING
#include "vector_3.h"
#endif // ENABLE_AUTO_BED_LEVELING

enum BlockFlag {
    // Planner flag to recalculate trapezoids on entry junction.
    // This flag has an optimization purpose only.
    BLOCK_FLAG_RECALCULATE = 1,
    // Planner flag for nominal speed always reached. That means, the segment is long enough, that the nominal speed
    // may be reached if accelerating from a safe speed (in the regard of jerking from zero speed).
    BLOCK_FLAG_NOMINAL_LENGTH = 2,
    // If set, the machine will start from a halt at the start of this block,
    // respecting the maximum allowed jerk.
    BLOCK_FLAG_START_FROM_FULL_HALT = 4,
    // If set, the stepper interrupt expects, that the number of steps to tick will be lower
    // than 32767, therefore the DDA algorithm may run with 16bit resolution only.
    // In addition, the stepper routine will not do any end stop checking for higher performance.
    BLOCK_FLAG_DDA_LOWRES = 8,
    // Block starts with Zeroed E counter
    BLOCK_FLAG_E_RESET = 16,
};

union dda_isteps_t
{
  int32_t     wide;
  struct {
    int16_t   lo;
    int16_t   hi;
  };
};

union dda_usteps_t
{
  uint32_t    wide;
  struct {
    uint16_t  lo;
    uint16_t  hi;
  };
};

// This struct is used when buffering the setup for each linear movement "nominal" values are as specified in 
// the source g-code and may never actually be reached if acceleration management is active.
typedef struct {
  // Fields used by the bresenham algorithm for tracing the line
  // steps_x.y,z, step_event_count, acceleration_rate, direction_bits and active_extruder are set by plan_buffer_line().
  dda_isteps_t steps_x, steps_y, steps_z, steps_e;  // Step count along each axis
  dda_usteps_t step_event_count;            // The number of step events required to complete this block
  long acceleration_rate;                   // The acceleration rate used for acceleration calculation
  unsigned char direction_bits;             // The direction bit set for this block (refers to *_DIRECTION_BIT in config.h)
  unsigned char active_extruder;            // Selects the active extruder
  // accelerate_until and decelerate_after are set by calculate_trapezoid_for_block() and they need to be synchronized with the stepper interrupt controller.
  long accelerate_until;                    // The index of the step event on which to stop acceleration
  long decelerate_after;                    // The index of the step event on which to start decelerating

  // Fields used by the motion planner to manage acceleration
//  float speed_x, speed_y, speed_z, speed_e;        // Nominal mm/sec for each axis
  // The nominal speed for this block in mm/sec.
  // This speed may or may not be reached due to the jerk and acceleration limits.
  float nominal_speed;
  // Entry speed at previous-current junction in mm/sec, respecting the acceleration and jerk limits.
  // The entry speed limit of the current block equals the exit speed of the preceding block.
  float entry_speed;
  // Maximum allowable junction entry speed in mm/sec. This value is also a maximum exit speed of the previous block.
  float max_entry_speed;
  // The total travel of this block in mm
  float millimeters;
  // acceleration mm/sec^2
  float acceleration;

  // Bit flags defined by the BlockFlag enum.
  uint8_t flag;

  // Settings for the trapezoid generator (runs inside an interrupt handler).
  // Changing the following values in the planner needs to be synchronized with the interrupt handler by disabling the interrupts.
  //FIXME nominal_rate, initial_rate and final_rate are limited to uint16_t by MultiU24X24toH16 in the stepper interrupt anyway!
  unsigned long nominal_rate;                        // The nominal step rate for this block in step_events/sec 
  unsigned long initial_rate;                        // The jerk-adjusted step rate at start of block  
  unsigned long final_rate;                          // The minimal rate at exit
  unsigned long acceleration_st;                     // acceleration steps/sec^2
  //FIXME does it have to be unsigned long? Probably uint8_t would be just fine.
  unsigned long fan_speed;
  volatile char busy;


  // Pre-calculated division for the calculate_trapezoid_for_block() routine to run faster.
  float speed_factor;

#ifdef LIN_ADVANCE
  bool use_advance_lead;            // Whether the current block uses LA
  uint16_t advance_rate,            // Step-rate for extruder speed
           max_adv_steps,           // max. advance steps to get cruising speed pressure (not always nominal_speed!)
           final_adv_steps;         // advance steps due to exit speed
  uint8_t advance_step_loops;       // Number of stepper ticks for each advance isr
  float adv_comp;                   // Precomputed E compression factor
#endif

  // Save/recovery state data
  float gcode_target[NUM_AXIS];     // Target (abs mm) of the original Gcode instruction
  uint16_t gcode_feedrate;          // Default and/or move feedrate
  uint16_t sdlen;                   // Length of the Gcode instruction
} block_t;

#ifdef LIN_ADVANCE
extern float extruder_advance_K;    // Linear-advance K factor
#endif

#ifdef ENABLE_AUTO_BED_LEVELING
// this holds the required transform to compensate for bed level
extern matrix_3x3 plan_bed_level_matrix;
#endif // #ifdef ENABLE_AUTO_BED_LEVELING

// Initialize the motion plan subsystem      
void plan_init();

// Add a new linear movement to the buffer. x, y and z is the signed, absolute target position in 
// millimaters. Feed rate specifies the speed of the motion.

#ifdef ENABLE_AUTO_BED_LEVELING
void plan_buffer_line(float x, float y, float z, const float &e, float feed_rate, const uint8_t &extruder);

// Get the position applying the bed level matrix if enabled
vector_3 plan_get_position();
#else

/// Extracting common call of 
/// plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], ...
/// saves almost 5KB.
/// The performance penalty is negligible, since these planned lines are usually maintenance moves with the extruder.
void plan_buffer_line_curposXYZE(float feed_rate);

void plan_buffer_line_destinationXYZE(float feed_rate);

void plan_set_position_curposXYZE();

void plan_buffer_line(float x, float y, float z, const float &e, float feed_rate, uint8_t extruder, const float* gcode_target = NULL);
//void plan_buffer_line(const float &x, const float &y, const float &z, const float &e, float feed_rate, const uint8_t &extruder);
#endif // ENABLE_AUTO_BED_LEVELING

// Set position. Used for G92 instructions.
//#ifdef ENABLE_AUTO_BED_LEVELING
void plan_set_position(float x, float y, float z, const float &e);
//#else
//void plan_set_position(const float &x, const float &y, const float &z, const float &e);
//#endif // ENABLE_AUTO_BED_LEVELING

void plan_set_z_position(const float &z);
void plan_set_e_position(const float &e);

// Reset the E position to zero at the start of the next segment
void plan_reset_next_e();

inline void set_current_to_destination() { memcpy(current_position, destination, sizeof(current_position)); }
inline void set_destination_to_current() { memcpy(destination, current_position, sizeof(destination)); }

extern bool e_active();

void check_axes_activity();

// Use M203 to override by software
extern float* max_feedrate;


// Use M201 to override by software
extern unsigned long* max_acceleration_units_per_sq_second; 
extern unsigned long axis_steps_per_sqr_second[NUM_AXIS];

extern long position[NUM_AXIS];
extern uint8_t maxlimit_status;


#ifdef AUTOTEMP
    extern bool autotemp_enabled;
    extern float autotemp_max;
    extern float autotemp_min;
    extern float autotemp_factor;
#endif

    


extern block_t block_buffer[BLOCK_BUFFER_SIZE];            // A ring buffer for motion instfructions
// Index of the next block to be pushed into the planner queue.
extern volatile unsigned char block_buffer_head;
// Index of the first block in the planner queue.
// This is the block, which is being currently processed by the stepper routine, 
// or which is first to be processed by the stepper routine.
extern volatile unsigned char block_buffer_tail; 
// Called when the current block is no longer needed. Discards the block and makes the memory
// available for new blocks.    
FORCE_INLINE void plan_discard_current_block()  
{
  if (block_buffer_head != block_buffer_tail) {
    block_buffer_tail = (block_buffer_tail + 1) & (BLOCK_BUFFER_SIZE - 1);  
  }
}

// Gets the current block. This is the block to be exectuted by the stepper routine.
// Mark this block as busy, so its velocities and acceperations will be no more recalculated
// by the planner routine.
// Returns NULL if buffer empty
FORCE_INLINE block_t *plan_get_current_block() 
{
  if (block_buffer_head == block_buffer_tail) { 
    return(NULL); 
  }
  block_t *block = &block_buffer[block_buffer_tail];
  block->busy = true;
  return(block);
}

// Returns true if the buffer has a queued block, false otherwise
FORCE_INLINE bool blocks_queued() { 
	return (block_buffer_head != block_buffer_tail); 
}

//return the nr of buffered moves
FORCE_INLINE uint8_t moves_planned() {
    return (block_buffer_head + BLOCK_BUFFER_SIZE - block_buffer_tail) & (BLOCK_BUFFER_SIZE - 1);
}

FORCE_INLINE bool planner_queue_full() {
    unsigned char next_block_index = block_buffer_head;
    if (++ next_block_index == BLOCK_BUFFER_SIZE)
        next_block_index = 0; 
    return block_buffer_tail == next_block_index;
}

// Abort the stepper routine, clean up the block queue,
// wait for the steppers to stop,
// update planner's current position and the current_position of the front end.
extern void planner_abort_hard();
extern bool waiting_inside_plan_buffer_line_print_aborted;

#ifdef PREVENT_DANGEROUS_EXTRUDE
void set_extrude_min_temp(float temp);
#endif

void reset_acceleration_rates();
#endif

void update_mode_profile();

unsigned char number_of_blocks();

// #define PLANNER_DIAGNOSTICS
#ifdef PLANNER_DIAGNOSTICS
// Diagnostic functions to display planner buffer underflow on the display.
extern uint8_t planner_queue_min();
// Diagnostic function: Reset the minimum planner segments.
extern void planner_queue_min_reset();
#endif /* PLANNER_DIAGNOSTICS */

extern void planner_add_sd_length(uint16_t sdlen);

extern uint16_t planner_calc_sd_length();
