/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
  stepper_indirection.h - stepper motor driver indirection macros
  to allow some stepper functions to be done via SPI/I2c instead of direct pin manipulation
  Part of Marlin

  Copyright (c) 2015 Dominik Wenger

  Marlin is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Marlin is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Marlin.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef STEPPER_INDIRECTION_H
#define STEPPER_INDIRECTION_H

#if defined(HAVE_TMC2130)
  #include "TMC2130Stepper.h"
  void tmc2130_init();
#endif

// X Stepper
    #if defined(HAVE_TMC2130) && defined(X_IS_TMC2130)
      extern TMC2130Stepper stepperX;
    #endif

#define X_ENABLE_INIT SET_OUTPUT(X_ENABLE_PIN)
#define X_ENABLE_WRITE(STATE) WRITE(X_ENABLE_PIN,STATE)
#define X_ENABLE_READ READ(X_ENABLE_PIN)

#define X_DIR_INIT SET_OUTPUT(X_DIR_PIN)
#define X_DIR_WRITE(STATE) WRITE(X_DIR_PIN,STATE)
#define X_DIR_READ READ(X_DIR_PIN)

#define X_STEP_INIT SET_OUTPUT(X_STEP_PIN)
#define X_STEP_WRITE(STATE) WRITE(X_STEP_PIN,STATE)
#define X_STEP_READ READ(X_STEP_PIN)

// Y Stepper
    #if defined(HAVE_TMC2130) && defined(Y_IS_TMC2130)
      extern TMC2130Stepper stepperY;
    #endif
	
#define Y_ENABLE_INIT SET_OUTPUT(Y_ENABLE_PIN)
#define Y_ENABLE_WRITE(STATE) WRITE(Y_ENABLE_PIN,STATE)
#define Y_ENABLE_READ READ(Y_ENABLE_PIN)

#define Y_DIR_INIT SET_OUTPUT(Y_DIR_PIN)
#define Y_DIR_WRITE(STATE) WRITE(Y_DIR_PIN,STATE)
#define Y_DIR_READ READ(Y_DIR_PIN)

#define Y_STEP_INIT SET_OUTPUT(Y_STEP_PIN)
#define Y_STEP_WRITE(STATE) WRITE(Y_STEP_PIN,STATE)
#define Y_STEP_READ READ(Y_STEP_PIN)

// Z Stepper
    #if defined(HAVE_TMC2130) && defined(Z_IS_TMC2130)
      extern TMC2130Stepper stepperZ;
    #endif
	
#define Z_ENABLE_INIT SET_OUTPUT(Z_ENABLE_PIN)
#define Z_ENABLE_WRITE(STATE) WRITE(Z_ENABLE_PIN,STATE)
#define Z_ENABLE_READ READ(Z_ENABLE_PIN)

#define Z_DIR_INIT SET_OUTPUT(Z_DIR_PIN)
#define Z_DIR_WRITE(STATE) WRITE(Z_DIR_PIN,STATE)
#define Z_DIR_READ READ(Z_DIR_PIN)

#define Z_STEP_INIT SET_OUTPUT(Z_STEP_PIN)
#define Z_STEP_WRITE(STATE) WRITE(Z_STEP_PIN,STATE)
#define Z_STEP_READ READ(Z_STEP_PIN)

// E0 Stepper
    #if defined(HAVE_TMC2130) && defined(E0_IS_TMC2130)
      extern TMC2130Stepper stepperE0;
    #endif
#define E0_ENABLE_INIT SET_OUTPUT(E0_ENABLE_PIN)
#define E0_ENABLE_WRITE(STATE) WRITE(E0_ENABLE_PIN,STATE)
#define E0_ENABLE_READ READ(E0_ENABLE_PIN)

#define E0_DIR_INIT SET_OUTPUT(E0_DIR_PIN)
#define E0_DIR_WRITE(STATE) WRITE(E0_DIR_PIN,STATE)
#define E0_DIR_READ READ(E0_DIR_PIN)

#define E0_STEP_INIT SET_OUTPUT(E0_STEP_PIN)
#define E0_STEP_WRITE(STATE) WRITE(E0_STEP_PIN,STATE)
#define E0_STEP_READ READ(E0_STEP_PIN)

#endif // STEPPER_INDIRECTION_H
