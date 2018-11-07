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

// private variables:
static int toolChanges = 0;
int trackToolChanges = 0;

bool feed_filament()
{
    bool _loaded = false;

    uint8_t current_loading_normal[3] = CURRENT_LOADING_NORMAL;
    uint8_t current_loading_stealth[3] = CURRENT_LOADING_STEALTH;
    uint8_t current_running_normal[3] = CURRENT_RUNNING_NORMAL;
    uint8_t current_running_stealth[3] = CURRENT_RUNNING_STEALTH;
    uint8_t current_holding_normal[3] = CURRENT_HOLDING_NORMAL;
    uint8_t current_holding_stealth[3] = CURRENT_HOLDING_STEALTH;


    int _c = 0;
    engage_filament_pulley(true);
    while (!_loaded) {
        if (tmc2130_mode == NORMAL_MODE) {
            tmc2130_init_axis_current_normal(AX_PUL, current_holding_normal[AX_PUL],
                                             current_loading_normal[AX_PUL]);
        } else {
            tmc2130_init_axis_current_normal(AX_PUL, current_holding_stealth[AX_PUL],
                                             current_loading_stealth[AX_PUL]);
        }

        if (moveSmooth(AX_PUL, 4000, 650, false, true, ACC_NORMAL, true) == MR_Success) {
            if (tmc2130_mode == NORMAL_MODE) {
                tmc2130_init_axis_current_normal(AX_PUL, current_holding_normal[AX_PUL],
                                                 current_running_normal[AX_PUL]);
            } else {
                tmc2130_init_axis_current_normal(AX_PUL, current_holding_stealth[AX_PUL],
                                                 current_running_stealth[AX_PUL]);
            }
            moveSmooth(AX_PUL, -600, 650, false, false, ACC_NORMAL);
            shr16_set_led(1 << 2 * (4 - active_extruder));
            _loaded = true;
            break;
        } else {
            if (_c < 2) fixTheProblem();
            else break;
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

    if (active_extruder == 5) {
        active_extruder = 4;
        move_selector(-700); // service position
    }

    shr16_set_led(2 << 2 * (4 - active_extruder));

    previous_extruder = active_extruder;
    active_extruder = new_extruder;

    if (previous_extruder == active_extruder) {
        if (!isFilamentLoaded) {
            shr16_set_led(2 << 2 * (4 - active_extruder));
            load_filament_withSensor(); // just load filament if not loaded
            _return = true;
        } else {
            _return = true; // nothing really happened
        }
    } else {
        if (isFilamentLoaded) {
            unload_filament_withSensor(); //failed unload. unload filament first
        }
        if (!isFilamentLoaded) {
            if (trackToolChanges == TOOLSYNC) {
                home(true);
                set_positions(0, active_extruder); // move idler and selector to new filament position
                delay(50);
                engage_filament_pulley(true);
            } else {
                set_positions(previous_extruder, active_extruder); // move idler and selector to new filament position
            }
            toolChanges++;
            trackToolChanges ++;
            shr16_set_led(2 << 2 * (4 - active_extruder));
            //load_filament_withSensor();
            load_filament_at_toolChange = true;
            _return = true;
        }
    }

    shr16_set_led(0x000);
    shr16_set_led(2 << 2 * (4 - active_extruder));
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
    if (digitalRead(A1) == 1) return false;

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
        if (new_extruder == EXTRUDERS) {
            move_selector(700); // move to service position
        } else {
            if (previous_extruder == EXTRUDERS) {
                move_selector(-700); // move back from service position
            } else {
                set_positions(previous_extruder,
                              active_extruder); // move idler and selector to new filament position
                engage_filament_pulley(false);
            }
        }
        _return = true;
    }


    shr16_set_led(0x000);
    shr16_set_led(1 << 2 * (4 - active_extruder));
    return _return;
}

bool service_position()
{
    // TODO 2: fixme, when abs-coords are implemented
    move_selector(600); // TODO 1: check if 600 is ok!
    return true;
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
