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
 * stepper_indirection.cpp
 *
 * Stepper motor driver indirection to allow some stepper functions to
 * be done via SPI/I2c instead of direct pin manipulation.
 *
 * Part of Marlin
 *
 * Copyright (c) 2015 Dominik Wenger
 */

#include "stepper_indirection.h"

#include "Marlin.h"

//
// TMC2130 Driver objects and inits
//
#if defined(HAVE_TMC2130)

  #include <SPI.h>
  #include <TMC2130Stepper.h>

  #define _TMC2130_DEFINE(ST) TMC2130Stepper stepper##ST(ST##_ENABLE_PIN, ST##_DIR_PIN, ST##_STEP_PIN, ST##_CS_PIN)

  // Stepper objects of TMC2130 steppers used
  #if defined(X_IS_TMC2130)
    _TMC2130_DEFINE(X);
  #endif

  #if defined(Y_IS_TMC2130)
    _TMC2130_DEFINE(Y);
  #endif

  #if defined(Z_IS_TMC2130)
    _TMC2130_DEFINE(Z);
  #endif

  #if defined(E0_IS_TMC2130)
    _TMC2130_DEFINE(E0);
  #endif

  // Use internal reference voltage for current calculations. This is the default.
  // Following values from Trinamic's spreadsheet with values for a NEMA17 (42BYGHW609)
  // https://www.trinamic.com/products/integrated-circuits/details/tmc2130/
  void tmc2130_init(TMC2130Stepper &st, const uint16_t microsteps, const uint32_t thrs, const uint32_t spmm) {
    st.begin();
    st.setCurrent(st.getCurrent(), R_SENSE, HOLD_MULTIPLIER);
    st.microsteps(microsteps);
    st.blank_time(36);
    st.off_time(5); // Only enables the driver if used with stealthChop
    st.interpolate(INTERPOLATE);
    st.power_down_delay(128); // ~2s until driver lowers to hold current
    st.hysterisis_start(0); // HSTRT = 1
    st.hysterisis_low(1); // HEND = -2
    st.diag1_active_high(1); // For sensorless homing
    #if defined(STEALTHCHOP)
      st.stealth_freq(1); // f_pwm = 2/683 f_clk
      st.stealth_autoscale(1);
      st.stealth_gradient(5);
      st.stealth_amplitude(255);
      st.stealthChop(1);
      #if defined(HYBRID_THRESHOLD)
        st.stealth_max_speed(12650000UL*st.microsteps()/(256*thrs*spmm));
      #endif
    #elif defined(SENSORLESS_HOMING)
      st.coolstep_min_speed(1024UL * 1024UL - 1UL);
    #endif
  }

  #define _TMC2130_INIT(ST, SPMM) tmc2130_init(stepper##ST, ST##_MICROSTEPS, ST##_HYBRID_THRESHOLD, SPMM)

  void tmc2130_init() {
    constexpr uint16_t steps_per_mm[] = DEFAULT_AXIS_STEPS_PER_UNIT;
    #if defined(X_IS_TMC2130)
      _TMC2130_INIT( X, steps_per_mm[X_AXIS]);
      #if defined(SENSORLESS_HOMING)
        stepperX.sg_stall_value(X_HOMING_SENSITIVITY);
      #endif
    #endif

    #if defined(Y_IS_TMC2130)
      _TMC2130_INIT( Y, steps_per_mm[Y_AXIS]);
      #if defined(SENSORLESS_HOMING)
        stepperY.sg_stall_value(Y_HOMING_SENSITIVITY);
      #endif
    #endif

    #if defined(Z_IS_TMC2130)
      _TMC2130_INIT( Z, steps_per_mm[Z_AXIS]);
    #endif

    #if defined(E0_IS_TMC2130)
      _TMC2130_INIT(E0, steps_per_mm[E_AXIS]);
    #endif

    TMC2130_ADV()
  }
#endif // HAVE_TMC2130