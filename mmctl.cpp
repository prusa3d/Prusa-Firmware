// mmctl.cpp - multimaterial switcher control
#include "main.h"
#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include "shr16.h"
#include "spi.h"
#include "tmc2130.h"
#include "mmctl.h"
#include "motion.h"
#include "Buttons.h"

// public variables:
int active_extruder = -1;  // extruder channel, 0...4
int previous_extruder = -1;
bool isFilamentLoaded = false;
bool isIdlerParked = false;
bool isPrinting = false;
bool isHomed = false;
bool homedOnUnload = false;

// private variables:
static int toolChanges = 0;
int trackToolChanges = 0;

bool feed_filament(void)
{
    bool _loaded = false;

    int _c = 0;
    engage_filament_pulley(true);

    while (!_loaded) {

        if (moveSmooth(AX_PUL, 4000, 650, false, true, ACC_NORMAL, true) == MR_Success) {
            moveSmooth(AX_PUL, FILAMENT_PARKING_STEPS, 650, false, false, ACC_NORMAL);
            shr16_set_led(1 << 2 * (4 - active_extruder));
            _loaded = true;
            break;
        } else {
            if (_c < 2) {
                fixTheProblem();
                engage_filament_pulley(true);
            } else {
                _loaded = false;
                break;
            }
            _c++;
        }
    }
    tmc2130_disable_axis(AX_PUL, tmc2130_mode);
    engage_filament_pulley(false);
    return _loaded;
}

bool toolChange(int new_extruder)
{
    bool _return = false;
    isPrinting = true;

    shr16_set_led(2 << 2 * (4 - active_extruder));

    previous_extruder = active_extruder;
    active_extruder = new_extruder;

    if (previous_extruder == active_extruder) {
        if (!isFilamentLoaded) {
            shr16_set_led(2 << 2 * (4 - active_extruder));
            load_filament_at_toolChange = true;
            _return = true;
        } else {
            _return = true; // nothing really happened
        }
    } else {
        if (isFilamentLoaded) {
            unload_filament_withSensor(); //failed unload. unload filament first
        }
        if (!isFilamentLoaded) {
            if (trackToolChanges == TOOLSYNC) { // Home every period TOOLSYNC
                home(true);
            // move idler and selector to new filament position
            } else if (!homedOnUnload) set_positions(previous_extruder, active_extruder);
            toolChanges++;
            trackToolChanges ++;
            shr16_set_led(2 << 2 * (4 - active_extruder));
            load_filament_at_toolChange = true;
            homedOnUnload = false;
            _return = true;
        }
    }

    shr16_set_led(0x000);
    shr16_set_led(1 << 2 * (4 - active_extruder));
    return _return;
}

//! @brief select extruder
//!
//! Known limitation is, that if extruder 5 - service position was selected before
//! it is not possible to select any other extruder than extruder 4.
//!
//! @param new_extruder Extruder to be selected
//! @return
bool select_extruder(int new_extruder)
{
    if (digitalRead(A1)) return false;

    int previous_extruder = active_extruder;
    active_extruder = new_extruder;

    bool _return = false;
    if (!isHomed) {
        home();
    }

    shr16_set_led(2 << 2 * (4 - active_extruder));

    if (previous_extruder == active_extruder) {
        if (!isFilamentLoaded) {
            _return = true;
        }
    } else {
        set_positions(previous_extruder, active_extruder); // move idler and selector to new filament position
        engage_filament_pulley(false);
        _return = true;
    }


    shr16_set_led(0x000);
    shr16_set_led(1 << 2 * (4 - active_extruder));
    return _return;
}

void led_blink(int _no)
{
    shr16_set_led(1 << 2 * _no);
    delay(40);
    shr16_set_led(0x000);
    delay(20);
    shr16_set_led(1 << 2 * _no);
    delay(40);

    shr16_set_led(0x000);
    delay(10);
}
