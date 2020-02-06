/*
  stepper.h - stepper motor driver: executes motion plans of planner.c using the stepper motors
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

#ifndef stepper_h
#define stepper_h 

#include "planner.h"

#define ENABLE_STEPPER_DRIVER_INTERRUPT()  TIMSK1 |= (1<<OCIE1A)
#define DISABLE_STEPPER_DRIVER_INTERRUPT() TIMSK1 &= ~(1<<OCIE1A)

#ifdef ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED
extern bool abort_on_endstop_hit;
#endif

// Initialize and start the stepper motor subsystem
void st_init();

// Interrupt Service Routines

void isr();

// Block until all buffered steps are executed
void st_synchronize();

// Set current position in steps
void st_set_position(const long &x, const long &y, const long &z, const long &e);
void st_set_e_position(const long &e);

// Get current position in steps
long st_get_position(uint8_t axis);

// Get current x and y position in steps
void st_get_position_xy(long &x, long &y);

// Get current position in mm
float st_get_position_mm(uint8_t axis);


// Call this function just before re-enabling the stepper driver interrupt and the global interrupts
// to avoid a stepper timer overflow.
void st_reset_timer();

void checkHitEndstops(); //call from somewhere to create an serial error message with the locations the endstops where hit, in case they were triggered
bool endstops_hit_on_purpose(); //avoid creation of the message, i.e. after homing and before a routine call of checkHitEndstops();
bool endstop_z_hit_on_purpose();


bool enable_endstops(bool check); // Enable/disable endstop checking. Return the old value.
bool enable_z_endstop(bool check);
void invert_z_endstop(bool endstop_invert);

void checkStepperErrors(); //Print errors detected by the stepper

void finishAndDisableSteppers();

extern block_t *current_block;  // A pointer to the block currently being traced
extern bool x_min_endstop;
extern bool x_max_endstop;
extern bool y_min_endstop;
extern bool y_max_endstop;
extern volatile long count_position[NUM_AXIS];

void quickStop();
#if defined(DIGIPOTSS_PIN) && DIGIPOTSS_PIN > -1
void digitalPotWrite(int address, int value);
#endif //defined(DIGIPOTSS_PIN) && DIGIPOTSS_PIN > -1
void microstep_ms(uint8_t driver, int8_t ms1, int8_t ms2);
void microstep_mode(uint8_t driver, uint8_t stepping);
void st_current_init();
void st_current_set(uint8_t driver, int current);
void microstep_init();
void microstep_readings();

#ifdef BABYSTEPPING
  void babystep(const uint8_t axis,const bool direction); // perform a short step with a single stepper motor, outside of any convention
#endif

#if defined(FILAMENT_SENSOR) && defined(PAT9125)
// reset the internal filament sensor state
void st_reset_fsensor();
#endif

#endif
