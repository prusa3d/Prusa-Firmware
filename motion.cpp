#include "motion.h"
#include "shr16.h"
#include "tmc2130.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <Arduino.h>
#include "main.h"
#include "uart.h"
#include "mmctl.h"
#include "Buttons.h"
#include "permanent_storage.h"
#include "config.h"

// public variables:
int8_t filament_type[EXTRUDERS] = { -1, -1, -1, -1, -1};

// private constants:
// selector homes on the right end. afterwards it is moved to extruder 0
static const int SELECTOR_STEPS_AFTER_HOMING = -3700;
static const int IDLER_STEPS_AFTER_HOMING = -138;

static const int IDLER_FULL_TRAVEL_STEPS = 1420; // 16th micro steps
// after homing: 1420 into negative direction
// and 130 steps into positive direction

static const int SELECTOR_STEPS = 2800 / (EXTRUDERS - 1);
static const int IDLER_STEPS = 1420 / (EXTRUDERS - 1); // full travel = 1420 16th micro steps
const int IDLER_PARKING_STEPS = (IDLER_STEPS / 2) + 40; // 217

const int BOWDEN_LENGTH = 8000;
const int STEPS_MK3FSensor_To_Bondtech = 390;
const int FILAMENT_PARKING_STEPS = -320;
const int EXTRA_STEPS_SELECTOR_SERVICE = 150;

static const int EJECT_PULLEY_STEPS = 2500;

// private variables:

static int selector_steps_for_eject = 0;

static int idler_steps_for_eject = 0;

// private functions:
static int set_idler_direction(int steps);
static int set_selector_direction(int steps);
static int set_pulley_direction(int steps);

void set_positions(int _current_extruder, int _next_extruder)
{
    delay(50);
    int _idler_steps = (_current_extruder - _next_extruder) * IDLER_STEPS;
    if (_next_extruder == EXTRUDERS)    _idler_steps = (_current_extruder - (_next_extruder - 1)) * IDLER_STEPS;
    if (_current_extruder == EXTRUDERS) _idler_steps = ((_current_extruder - 1) - _next_extruder) * IDLER_STEPS;
    // steps to move to new position of idler and selector
    move_idler(_idler_steps); // remove this, when abs coordinates are implemented!

    if (_next_extruder > 0) {
        int _selector_steps = ((_current_extruder - _next_extruder) * SELECTOR_STEPS) * -1;
        if (_next_extruder == EXTRUDERS)    _selector_steps += EXTRA_STEPS_SELECTOR_SERVICE;
        if (_current_extruder == EXTRUDERS) _selector_steps -= EXTRA_STEPS_SELECTOR_SERVICE;
        move_selector(_selector_steps);
    } else {
        moveSmooth(AX_SEL, 100, 2000, false);
        for (int c = 2; c > 0; c--) { // touch end 2 times
            moveSmooth(AX_SEL, -4000, 2000, false);
            if (c > 1) {
                moveSmooth(AX_SEL, 100, 2000, false);
            }
        }
        moveSmooth(AX_SEL, 33, 2000, false);
    }
}

/**
 * @brief Eject Filament
 * move selector sideways and push filament forward little bit, so user can catch it,
 * unpark idler at the end to user can pull filament out
 * @param extruder: extruder channel (0..4)
 */
void eject_filament(int extruder)
{
    int selector_position = 0;

    int8_t selector_offset_for_eject = 0;
    int8_t idler_offset_for_eject = 0;

    // if there is still filament detected by PINDA unload it first
    if (isFilamentLoaded) {
        unload_filament_withSensor();
    }


    engage_filament_pulley(true);
    tmc2130_init_axis(AX_PUL, tmc2130_mode);


    // if we are want to eject fil 0-2, move seelctor to position 4 (right), if we want to eject filament 3 - 4, move
    // selector to position 0 (left)
    // maybe we can also move selector to service position in the future?
    if (extruder <= 2) {
        selector_position = 4;
    } else {
        selector_position = 0;
    }

    // count offset (number of positions) for desired selector and idler position for ejecting
    selector_offset_for_eject = active_extruder - selector_position;
    idler_offset_for_eject = active_extruder - extruder;

    // count number of desired steps for selector and idler and store it in static variable
    selector_steps_for_eject = (selector_offset_for_eject * SELECTOR_STEPS) * -1;
    idler_steps_for_eject = idler_offset_for_eject * IDLER_STEPS;

    // move selector and idler to new position
    move_idler(idler_steps_for_eject); // remove this, with when abs coordinates are implemented!
    move_selector(selector_steps_for_eject);

    // push filament forward
    move_pulley(EJECT_PULLEY_STEPS, 666);

    // unpark idler so user can easily remove filament
    engage_filament_pulley(false);
    tmc2130_disable_axis(AX_PUL, tmc2130_mode);
    isFilamentLoaded = false; // ensure MMU knows it doesn't have filament loaded so next T? command works
}

void recover_after_eject()
{
    while (digitalRead(A1)) fixTheProblem();
    move_idler(-idler_steps_for_eject); // TODO 1: remove this, when abs coordinates are implemented!
    move_selector(-selector_steps_for_eject);
}

/**
 * @brief load_filament_intoExtruder
 * loads filament after confirmed by printer into the Bontech
 * pulley gears so they can grab them.
 * We reduce here stepwise the motor current, to prevent grinding into the
 * filament as good as possible.
 *
 * TODO 1: this procedure is most important for high reliability.
 * The speed must be set accordingly to the settings in the slicer
 */
void load_filament_into_extruder()
{
    uint8_t current_running_normal[3] = CURRENT_RUNNING_NORMAL;
    uint8_t current_running_stealth[3] = CURRENT_RUNNING_STEALTH;
    uint8_t current_holding_normal[3] = CURRENT_HOLDING_NORMAL;
    uint8_t current_holding_stealth[3] = CURRENT_HOLDING_STEALTH;

    engage_filament_pulley(true); // if idler is in parked position un-park him get in contact with filament

    tmc2130_init_axis(AX_PUL, tmc2130_mode);
    move_pulley(150, 385);

    // set current to 75%
    if (tmc2130_mode == NORMAL_MODE) {
        tmc2130_init_axis_current_normal(AX_PUL, current_holding_normal[AX_PUL],
                                         current_running_normal[AX_PUL] - (current_running_normal[AX_PUL] / 4) );
    } else {
        tmc2130_init_axis_current_stealth(AX_PUL, current_holding_stealth[AX_PUL],
                                          current_running_stealth[AX_PUL] - (current_running_stealth[AX_PUL] / 4) );
    }
    move_pulley(170, 385);

    // set current to 25%
    if (tmc2130_mode == NORMAL_MODE) {
        tmc2130_init_axis_current_normal(AX_PUL, current_holding_normal[AX_PUL],
                                         current_running_normal[AX_PUL] / 4);
    } else {
        tmc2130_init_axis_current_stealth(AX_PUL, current_holding_stealth[AX_PUL],
                                          current_running_stealth[AX_PUL] / 4);
    }
    moveSmooth(AX_PUL, 452, 455, true, true, ACC_NORMAL, false, true);


    // reset currents
    if (tmc2130_mode == NORMAL_MODE) {
        tmc2130_init_axis_current_normal(AX_PUL, current_holding_normal[AX_PUL],
                                         current_running_normal[AX_PUL]);
    } else {
        tmc2130_init_axis_current_stealth(AX_PUL, current_holding_stealth[AX_PUL],
                                          current_running_stealth[AX_PUL]);
    }
    tmc2130_disable_axis(AX_PUL, tmc2130_mode);
}

void init_Pulley()
{
    float _speed = 3000;

    // TODO 1: replace with move-commands

    for (int i = 50; i > 0; i--) {
        moveSmooth(AX_PUL, 1, 0, false);
        delayMicroseconds(_speed);
        shr16_set_led(1 << 2 * (int)(i / 50)); // TODO 2: What the heck?
    }

    for (int i = 50; i > 0; i--) {
        moveSmooth(AX_PUL, -1, 0, false);
        delayMicroseconds(_speed);
        shr16_set_led(1 << 2 * (4 - (int)(i / 50))); // TODO 2: What the heck?
    }
}

/**
 * @brief engage_filament_pulley
 * Turns the idler drum to engage or disengage the filament pully
 * @param engage
 * If true, pully can drive the filament afterwards
 * if false, idler will be parked, so the filament can move freely
 */
void engage_filament_pulley(bool engage)
{
    if (isIdlerParked && engage) { // get idler in contact with filament
        move_idler(IDLER_PARKING_STEPS);
        isIdlerParked = false;
    } else if (!isIdlerParked && !engage) { // park idler so filament can move freely
        move_idler(IDLER_PARKING_STEPS * -1);
        isIdlerParked = true;
    }
}

void reset_engage_filament_pulley(bool previouslyEngaged)  //reset after mid op homing
{
    if (isIdlerParked && previouslyEngaged) { // get idler in contact with filament
        move_idler(IDLER_PARKING_STEPS);
        isIdlerParked = false;
    } else if (!isIdlerParked && !previouslyEngaged) { // park idler so filament can move freely
        move_idler(IDLER_PARKING_STEPS * -1);
        isIdlerParked = true;
    }
}

void home(bool doToolSync)
{
    tmc2130_init(HOMING_MODE);  // trinamic, homing
    bool previouslyEngaged = isIdlerParked;
    homeIdlerSmooth();
    homeSelectorSmooth();
    tmc2130_init(tmc2130_mode); // trinamic, normal
    //tmc2130_init_axis(AX_IDL, tmc2130_mode);
    //tmc2130_init_axis(AX_SEL, tmc2130_mode);

    shr16_set_led(0x155);       // All five red

    isIdlerParked = false;
    delay(50); // delay to release the stall detection
    engage_filament_pulley(false);
    shr16_set_led(0x000);       // All five off

    isFilamentLoaded = false;
    shr16_set_led(1 << 2 * (4 - active_extruder));

    isHomed = true;

    if (doToolSync) {
        set_positions(0, active_extruder); // move idler and selector to new filament position
        reset_engage_filament_pulley(previouslyEngaged);
        trackToolChanges = 0;
    } else active_extruder = 0;
}

void move_idler(int steps, uint16_t speed)
{
    if (speed > MAX_SPEED_IDL) {
        speed = MAX_SPEED_IDL;
    }
    moveSmooth(AX_IDL, steps, MAX_SPEED_IDL, true, true, ACC_IDL_NORMAL);
}

/**
 * @brief move_selector
 * Strictly prevent selector movement, when filament is in FINDA
 * @param steps, number of micro steps
 */
void move_selector(int steps, uint16_t speed)
{
    if (speed > MAX_SPEED_SEL) {
        speed = MAX_SPEED_SEL;
    }
    if (tmc2130_mode == STEALTH_MODE) {
        if (speed > MAX_SPEED_STEALTH_SEL) {
            speed = MAX_SPEED_STEALTH_SEL;
        }
    }
    if (digitalRead(A1) == false) {
        moveSmooth(AX_SEL, steps, speed);
    }
}

void move_pulley(int steps, uint16_t speed)
{
    moveSmooth(AX_PUL, steps, speed, false, true);
}

/**
 * @brief set_idler_direction
 * @param steps: positive = towards engaging filament nr 1,
 * negative = towards engaging filament nr 5.
 * @return abs(steps)
 */
int set_idler_direction(int steps)
{
    if (steps < 0) {
        steps = steps * -1;
        shr16_set_dir(shr16_get_dir() & ~4);
    } else {
        shr16_set_dir(shr16_get_dir() | 4);
    }
    return steps;
}

/**
 * @brief set_selector_direction
 * Sets the direction bit on the motor driver and returns positive number of steps
 * @param steps: positive = to the right (towards filament 5),
 * negative = to the left (towards filament 1)
 * @return abs(steps)
 */
int set_selector_direction(int steps)
{
    if (steps < 0) {
        steps = steps * -1;
        shr16_set_dir(shr16_get_dir() & ~2);
    } else {
        shr16_set_dir(shr16_get_dir() | 2);
    }
    return steps;
}

/**
 * @brief set_pulley_direction
 * @param steps, positive (push) or negative (pull)
 * @return abs(steps)
 */
int set_pulley_direction(int steps)
{
    if (steps < 0) {
        steps = steps * -1;
        shr16_set_dir(shr16_get_dir() | 1);
    } else {
        shr16_set_dir(shr16_get_dir() & ~1);
    }
    return steps;
}

MotReturn homeSelectorSmooth()
{
    for (int c = 2; c > 0; c--) { // touch end 2 times
        moveSmooth(AX_SEL, 4000, 2000, false); // 3000 is too fast, 2500 works, decreased to 2000 for production
        if (c > 1) {
            moveSmooth(AX_SEL, -300, 2000, false);
        }
    }

    return moveSmooth(AX_SEL, SELECTOR_STEPS_AFTER_HOMING, MAX_SPEED_SEL, false);
}

MotReturn homeIdlerSmooth()
{
    for (int c = 2; c > 0; c--) { // touch end 3 times
        moveSmooth(AX_IDL, 2000, 3000, false);
        if (c > 1) {
            moveSmooth(AX_IDL, -350, MAX_SPEED_IDL, false);
        }
    }
    return moveSmooth(AX_IDL, IDLER_STEPS_AFTER_HOMING, MAX_SPEED_IDL, false);
}

/**
 * @brief moveTest
 * @param axis, index of axis, use AX_PUL, AX_SEL or AX_IDL
 * @param steps, number of micro steps to move
 * @param speed, max. speed
 * @param rehomeOnFail: flag, by default true, set to false
 *   in homing commands, to prevent endless loops and stack overflow.
 * @return
 */
// TODO 3: compensate delay for computation time, to get accurate speeds
MotReturn moveSmooth(uint8_t axis, int steps, int speed, bool rehomeOnFail, bool withStallDetection, float acc, bool withFindaDetection, bool disengageAtEnd)
{
    MotReturn ret = MR_Success;
    if (withFindaDetection) ret = MR_Failed;

    if (tmc2130_mode == STEALTH_MODE) {
        withStallDetection = false;
    }

    float vMax = speed;
    float v0 = 200; // steps/s, minimum speed
    float v = v0; // current speed
    int accSteps = 0; // number of steps for acceleration
    int stepsDone = 0;
    int stepsLeft = 0;

    switch (axis) {
    case AX_PUL:
        stepsLeft = set_pulley_direction(steps);
        if (disengageAtEnd) { set_idler_direction(IDLER_PARKING_STEPS * -1); stepsLeft += (IDLER_PARKING_STEPS/2); }// 217 and steps left plus half
        tmc2130_init_axis(AX_PUL, tmc2130_mode);
        break;
    case AX_IDL:
        stepsLeft = set_idler_direction(steps);
        break;
    case AX_SEL:
        stepsLeft = set_selector_direction(steps);
        break;
    }

    enum State {
        Accelerate = 0,
        ConstVelocity = 1,
        Decelerate = 2,
    };

    State st = Accelerate;

    while (stepsLeft) {
        switch (axis) {
        case AX_PUL:
            PIN_STP_PUL_HIGH;
            PIN_STP_PUL_LOW;
            if ((stepsLeft <= IDLER_PARKING_STEPS) && disengageAtEnd)    { PIN_STP_IDL_HIGH; PIN_STP_IDL_LOW; }   // Park AX_IDL for last parking steps
            if ((stepsLeft >= (IDLER_PARKING_STEPS/2)) && disengageAtEnd){ PIN_STP_PUL_HIGH; PIN_STP_PUL_LOW; }   // finish AX_PUL move half way through AX_IDL parking
            if (withStallDetection && digitalRead(A3)) { // stall detected
                delay(50); // delay to release the stall detection
                return MR_Failed;
            }
            if (withFindaDetection && ( steps > 0 ) && digitalRead(A1)) return MR_Success;
            if (withFindaDetection && ( steps < 0 ) && (digitalRead(A1) == false)) return MR_Success;
            break;
        case AX_IDL:
            PIN_STP_IDL_HIGH;
            PIN_STP_IDL_LOW;
            if (withStallDetection && digitalRead(A5)) { // stall detected
                delay(50); // delay to release the stall detection
                if (rehomeOnFail) fixTheProblem();
                else return MR_Failed;
            }
            break;
        case AX_SEL:
            PIN_STP_SEL_HIGH;
            PIN_STP_SEL_LOW;
            if (withStallDetection && digitalRead(A4)) { // stall detected
                delay(50); // delay to release the stall detection
                if (rehomeOnFail) fixTheProblem();
                else return MR_Failed;
            }
            break;
        }

        stepsDone++;
        stepsLeft--;

        float dt = 1 / v;
        delayMicroseconds(1e6 * dt);

        switch (st) {
        case Accelerate:
            v += acc * dt;
            if (v >= vMax) {
                accSteps = stepsDone;
                st = ConstVelocity;

                v = vMax;
            } else if (stepsDone > stepsLeft) {
                accSteps = stepsDone;
                st = Decelerate;

            }
            break;
        case ConstVelocity: {
            if (stepsLeft <= accSteps) {
                st = Decelerate;
            }
        }
        break;
        case Decelerate: {
            v -= acc * dt;
            if (v < v0) {
                v = v0;
            }
        }
        break;
        }
    }
    if (disengageAtEnd) isIdlerParked = true;
    return ret;
}
