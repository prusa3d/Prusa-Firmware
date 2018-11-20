//! @file

#include "main.h"
#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include "shr16.h"
#include "adc.h"
#include "uart.h"
#include "spi.h"
#include "tmc2130.h"
#include "abtn3.h"
#include "mmctl.h"
#include "motion.h"
#include "Buttons.h"
#include <avr/wdt.h>
#include "permanent_storage.h"


// public variables:
int8_t sys_state = 0;
uint8_t sys_signals = 0;
bool fsensor_triggered = false;
bool unloadatBoot = false;
bool mmuFSensorLoading = false;
bool duplicateTCmd = false;
bool load_filament_at_toolChange = false;

uint8_t tmc2130_mode = NORMAL_MODE; // STEALTH_MODE;

static char echo[32];

#if (UART_COM == 0)
FILE *uart_com = uart0io;
#elif (UART_COM == 1)
FILE *uart_com = uart1io;
#endif //(UART_COM == 0)

extern "C" void process_commands(FILE *inout);

//! @brief Initialization after reset
//!
//! button | action
//! ------ | ------
//! middle | enter setup
//! right  | continue after error
//!
//! LED indication of states
//!
//! RG | RG | RG | RG | RG | meaning
//! -- | -- | -- | -- | -- | ------------------------
//! 00 | 00 | 00 | 00 | 0b | Shift register initialized
//! 00 | 00 | 00 | 0b | 00 | uart initialized
//! 00 | 00 | 0b | 00 | 00 | spi initialized
//! 00 | 0b | 00 | 00 | 00 | tmc2130 initialized
//! 0b | 00 | 00 | 00 | 00 | A/D converter initialized
//! b0 | b0 | b0 | b0 | b0 | Error, filament detected, still present
//! 0b | 0b | 0b | 0b | 0b | Error, filament detected, no longer present, continue by right button click
//!
//! @n R - Red LED
//! @n G - Green LED
//! @n 1 - active
//! @n 0 - inactive
//! @n b - blinking
void setup()
{

    shr16_init(); // shift register
    led_blink(0);
    delay(1000);  // wait for boot ok printer

    uart0_init(); //uart0
    uart1_init(); //uart1
    led_blink(1);


#if (UART_STD == 0)
    stdin = uart0io;  // stdin = uart0
    stdout = uart0io; // stdout = uart0
#elif(UART_STD == 1)
    stdin = uart1io;  // stdin = uart1
    stdout = uart1io; // stdout = uart1
#endif //(UART_STD == 1)


    bool requestMenu = false;

    fprintf_P(uart_com, PSTR("start\n")); //startup message

    spi_init();
    led_blink(2);

    //tmc2130_init(HOMING_MODE); // trinamic, homing
    led_blink(3);


    adc_init(); // ADC
    led_blink(4);

    init_Pulley();

    if (buttonClicked() == Btn::middle) {
        requestMenu = true;
    }

    // if FINDA is sensing filament do not home
    while (digitalRead(A1)) {
        while (Btn::right != buttonClicked()) {
            if (digitalRead(A1)) {
                shr16_set_led(0x2aa);
            } else {
                shr16_set_led(0x155);
            }
            delay(300);
            shr16_set_led(0x000);
            delay(300);
        }
    }

    home();
    // TODO 2: add reading previously stored mode (stealth/normal) from eeprom

    //tmc2130_init(tmc2130_mode); // trinamic, initialize all axes


    // check if to goto the settings menu
    if (requestMenu) {
        setupMenu();
    }
}

//! @brief Select filament menu
//!
//! Select filament by pushing left and right button, park position can be also selected.
//!
//! button | action
//! ------ | ------
//! left   | select previous filament
//! right  | select next filament
//!
//! LED indication of states
//!
//! RG | RG | RG | RG | RG | meaning
//! -- | -- | -- | -- | -- | ------------------------
//! 01 | 00 | 00 | 00 | 00 | filament 1
//! 00 | 01 | 00 | 00 | 00 | filament 2
//! 00 | 00 | 01 | 00 | 00 | filament 3
//! 00 | 00 | 00 | 01 | 00 | filament 4
//! 00 | 00 | 00 | 00 | 01 | filament 5
//! 00 | 00 | 00 | 00 | bb | park position
//!
//! @n R - Red LED
//! @n G - Green LED
//! @n 1 - active
//! @n 0 - inactive
//! @n b - blinking
void manual_extruder_selector()
{
    shr16_set_led(1 << 2 * (4 - active_extruder));

#ifdef TESTING_STEALTH
    if (buttonClicked() != Btn::none) {
        switch (buttonClicked()) {
        case Btn::right:
            if (active_extruder < EXTRUDERS) {
                select_extruder(active_extruder + 1);
            }
            break;
        case Btn::left:
            if (active_extruder > 0) {
                select_extruder(active_extruder - 1);
            }
            break;
        default:
            break;
        }
    }
#else
    if ((Btn::left | Btn::right) & buttonClicked()) {
        switch (buttonClicked()) {
        case Btn::right:
            if (active_extruder < EXTRUDERS) {
                select_extruder(active_extruder + 1);
            }
            break;
        case Btn::left:
            if (active_extruder > 0) {
                select_extruder(active_extruder - 1);
            }
            break;
        default:
            break;
        }
    }
#endif

    if (active_extruder == 5) {
        shr16_set_led(2 << 2 * 0);
        delay(50);
        shr16_set_led(1 << 2 * 0);
        delay(50);
    }
}


//! @brief main loop
//!
//! It is possible to manually select filament and feed it when not printing.
//!
//! button | action
//! ------ | ------
//! middle | feed filament
//!
//! @copydoc manual_extruder_selector()
void loop()
{
    process_commands(uart_com);

    if (!isPrinting) {
        manual_extruder_selector();
#ifndef TESTING_STEALTH
        if (Btn::middle == buttonClicked() && active_extruder < 5) {
            shr16_set_led(2 << 2 * (4 - active_extruder));
            if (Btn::middle == buttonClicked()) {
                feed_filament();
            }
        }
    }
#endif
}

extern "C" {
    void process_commands(FILE *inout)
    {
        static char line[32];
        static int count = 0;
        int c = -1;
        if (count < 32) {
            if ((c = getc(inout)) >= 0) {
                if (c == '\r') {
                    c = 0;
                }
                if (c == '\n') {
                    c = 0;
                }
                line[count++] = c;
            }
        } else {
            count = 0;
            //overflow
        }
        int value = 0;
        int value0 = 0;

        if ((count > 0) && (c == 0)) {
            //line received
            //printf_P(PSTR("line received: '%s' %d\n"), line, count);
            if (strstr(line, "EE") != NULL) {
                for (int i = 0; i < 32; i++) {
                    line[i] = echo[i];
                }
                count = 0;
            } else if (sscanf_P(line, PSTR("P%d"), &value) > 0) {
                if (value == 0) { // Read finda
                    fprintf_P(inout, PSTR("%dok\n"), digitalRead(A1));
                }
            } else { //if (strstr(line, "P0") == NULL) {
                for (int i = 0; i < 32; i++) {
                    echo[i] = line[i];
                }
                count = 0;
                //delay(10); // delay so MK3 comms is not floaded
                if (strstr(line, "T0") != NULL) {
                    fprintf_P(inout, PSTR("T0\n"));
                    return;
                }
                if (strstr(line, "T1") != NULL) {
                    fprintf_P(inout, PSTR("T1\n"));
                    return;
                }
                if (strstr(line, "T2") != NULL) {
                    fprintf_P(inout, PSTR("T2\n"));
                    return;
                }
                if (strstr(line, "T3") != NULL) {
                    fprintf_P(inout, PSTR("T3\n"));
                    return;
                }
                if (strstr(line, "T4") != NULL) {
                    fprintf_P(inout, PSTR("T4\n"));
                    return;
                }
                if (strstr(line, "L0") != NULL) {
                    fprintf_P(inout, PSTR("L0\n"));
                    return;
                }
                if (strstr(line, "L1") != NULL) {
                    fprintf_P(inout, PSTR("L1\n"));
                    return;
                }
                if (strstr(line, "L2") != NULL) {
                    fprintf_P(inout, PSTR("L2\n"));
                    return;
                }
                if (strstr(line, "L3") != NULL) {
                    fprintf_P(inout, PSTR("L3\n"));
                    return;
                }
                if (strstr(line, "L4") != NULL) {
                    fprintf_P(inout, PSTR("L4\n"));
                    return;
                }
                if (strstr(line, "C0") != NULL) {
                    fprintf_P(inout, PSTR("C0\n"));
                    return;
                }
                if (strstr(line, "U0") != NULL) {
                    fprintf_P(inout, PSTR("U0\n"));
                    return;
                }
                if (strstr(line, "E0") != NULL) {
                    fprintf_P(inout, PSTR("E0\n"));
                    return;
                }
                if (strstr(line, "E1") != NULL) {
                    fprintf_P(inout, PSTR("E1\n"));
                    return;
                }
                if (strstr(line, "E2") != NULL) {
                    fprintf_P(inout, PSTR("E2\n"));
                    return;
                }
                if (strstr(line, "E3") != NULL) {
                    fprintf_P(inout, PSTR("E3\n"));
                    return;
                }
                if (strstr(line, "E4") != NULL) {
                    fprintf_P(inout, PSTR("E4\n"));
                    return;
                }
                if (strstr(line, "R0") != NULL) {
                    fprintf_P(inout, PSTR("R0\n"));
                    return;
                }
                if (strstr(line, "FS") != NULL) {
                    fprintf_P(inout, PSTR("FS\n"));
                    return;
                }
            }
            count = 0;

            if (sscanf_P(line, PSTR("T%d"), &value) > 0x00) {
                //T-code scanned
                if ((value >= 0) && (value < EXTRUDERS)) {
                    if ((active_extruder == value) & (isFilamentLoaded)) {
                        duplicateTCmd = true;
                        fprintf_P(inout, PSTR("ok\n"));
                    } else {
                        mmuFSensorLoading = true;
                        duplicateTCmd = false;
                        toolChange(value);
                        if (load_filament_at_toolChange) {
                            fprintf_P(inout, PSTR("fl\n"));
                            load_filament_withSensor();
                            load_filament_at_toolChange = false;
                            fprintf_P(inout, PSTR("ok\n"));
                        } //else fprintf_P(inout, PSTR("nk\n"));
                    }
                }
            } else if (sscanf_P(line, PSTR("L%d"), &value) > 0) {
                // Load filament
                if ((value >= 0) && (value < EXTRUDERS) && !isFilamentLoaded) {

                    select_extruder(value);
                    delay(10);
                    feed_filament();
                    delay(100);
                    fprintf_P(inout, PSTR("ok\n"));
                }
            } else if (sscanf_P(line, PSTR("M%d"), &value) > 0) {
                // M0: set to normal mode; M1: set to stealth mode
                switch (value) {
                case 0:
                    tmc2130_mode = NORMAL_MODE;
                    break;
                case 1:
                    tmc2130_mode = STEALTH_MODE;
                    break;
                default:
                    return;
                }
                //init all axes
                tmc2130_init(tmc2130_mode);
                fprintf_P(inout, PSTR("ok\n"));
            } else if (sscanf_P(line, PSTR("U%d"), &value) > 0) { // Unload filament
                unload_filament_withSensor();
                delay(200);
                fprintf_P(inout, PSTR("ok\n"));
                isPrinting = false;
                trackToolChanges = 0;
            } else if (sscanf_P(line, PSTR("X%d"), &value) > 0) {
                if (value == 0) { // MMU reset
                    wdt_enable(WDTO_15MS);
                }
            } else if (sscanf_P(line, PSTR("S%d"), &value) > 0) {
                if (value == 0) { // return ok
                    fprintf_P(inout, PSTR("ok\n"));
                } else if (value == 1) { // Read version
                    fprintf_P(inout, PSTR("%dok\n"), FW_VERSION);
                } else if (value == 2) { // Read build nr
                    fprintf_P(inout, PSTR("%dok\n"), FW_BUILDNR);
                }
            } else if (strstr(line, "FS") > 0) {
                fsensor_triggered = true;
                //fprintf_P(inout, PSTR("ok\n"));
            } else if (sscanf_P(line, PSTR("F%d %d"), &value, &value0) > 0) {
                if (((value >= 0) && (value < EXTRUDERS)) && ((value0 >= 0) && (value0 <= 2))) {
                    filament_type[value] = value0;
                    fprintf_P(inout, PSTR("ok\n"));
                }
            } else if (sscanf_P(line, PSTR("C%d"), &value) > 0) {
                if (value == 0) // C0 continue loading current filament (used after T-code), maybe add different code for
                    // each extruder (the same way as T-codes) in the future?
                {
                    if (!duplicateTCmd) {
                        load_filament_into_extruder();
                        fprintf_P(inout, PSTR("ok\n"));
                    } else fprintf_P(inout, PSTR("ok\n"));
                }
            } else if (sscanf_P(line, PSTR("E%d"), &value) > 0) {
                if ((value >= 0) && (value < EXTRUDERS)) { // Ex: eject filament
                    eject_filament(value);
                    fprintf_P(inout, PSTR("ok\n"));
                }
            } else if (sscanf_P(line, PSTR("R%d"), &value) > 0) {
                if (value == 0) { // R0: recover after eject filament
                    recover_after_eject();
                    fprintf_P(inout, PSTR("ok\n"));
                }
            } else if (!mmuFSensorLoading && fsensor_triggered) {
              fsensor_triggered = false;
              fprintf_P(inout, PSTR("ok\n"));
            }
        }
    }
} // extern C

void process_signals()
{
    // what to do here?
}

void fault_handler(Fault id)
{
    while (1) {
        shr16_set_led(id + 1);
        delay(1000);
        shr16_set_led(0);
        delay(2000);
    }
}

//****************************************************************************************************
//* this routine is the common routine called for fixing the filament issues (loading or unloading)
//****************************************************************************************************
void fixTheProblem(void) {
  
    engage_filament_pulley(false);                    // park the idler stepper motor
    tmc2130_disable_axis(AX_SEL, tmc2130_mode);       // turn OFF the selector stepper motor
    tmc2130_disable_axis(AX_IDL, tmc2130_mode);       // turn OFF the idler stepper motor

    while ((Btn::middle != buttonClicked()) || digitalRead(A1)) {
        //  wait until key is entered to proceed  (this is to allow for operator intervention)
        delay(100);
        shr16_set_led(0x000);
        delay(100);
        if (digitalRead(A1)) {
            shr16_set_led(2 << 2 * (4 - active_extruder));
        } else shr16_set_led(1 << 2 * (4 - active_extruder));
    }

    tmc2130_init_axis(AX_SEL, tmc2130_mode);           // turn ON the selector stepper motor
    tmc2130_init_axis(AX_IDL, tmc2130_mode);           // turn ON the idler stepper motor
    home(true); // Home and return to previous active extruder
}

bool load_filament_withSensor()
{
    fsensor_triggered = false;
    loop:
    {
        engage_filament_pulley(true); // get in contact with filament
        tmc2130_init_axis(AX_PUL, tmc2130_mode);

        unsigned long startTime, currentTime;
        bool tag = false;

        // load filament until FINDA senses end of the filament, means correctly loaded into the selector
        // we can expect something like 570 steps to get in sensor, try 1000 incase user is feeding to pulley

        if (moveSmooth(AX_PUL, 1000, 650, false, false, ACC_NORMAL, true) == MR_Success) {        // Check if filament makes it to the FINDA
            moveSmooth(AX_PUL, BOWDEN_LENGTH, MAX_SPEED_PUL, false, false, ACC_FEED_NORMAL);      // Load filament down to MK3-FSensor

            startTime = millis();
            process_commands(uart_com);                                           // Run through serial read buffer so fsensor_triggered can be updated

            while (tag == false) {
                currentTime = millis();
                if ((currentTime - startTime) > 5000) {      // After min bowden length load slow until MK3-FSensor trips
                    fixTheProblem();
                    goto loop;
                }

                move_pulley(1,MAX_SPEED_PUL);
                process_commands(uart_com);
                if (fsensor_triggered == true) tag = true;
            }
            moveSmooth(AX_PUL, STEPS_MK3FSensor_To_Bondtech, 385,false, false);   // Load from MK3-FSensor to Bontech gears, ready for loading into extruder with C0 command
            shr16_set_led(0x000);                                                 // Clear all 10 LEDs on MMU unit
            shr16_set_led(1 << 2 * (4 - active_extruder));
            isFilamentLoaded = true;  // filament loaded
            mmuFSensorLoading = false;
            return true;
        }
        fixTheProblem();
        goto loop;
    }
}

/**
 * @brief unload_filament_withSensor
 * unloads filament from extruder - filament is above Bondtech gears
 */
bool unload_filament_withSensor()
{
    bool _return = false;
    tmc2130_init_axis(AX_PUL, tmc2130_mode);
    //tmc2130_init_axis(AX_IDL, tmc2130_mode);
    engage_filament_pulley(true); // get in contact with filament
    
    moveSmooth(AX_PUL, (BOWDEN_LENGTH * -1), MAX_SPEED_PUL - (MAX_SPEED_PUL/5), false, false, ACC_FEED_NORMAL); // unload to before FINDA
    if (moveSmooth(AX_PUL, -2000, 650, false, false, ACC_NORMAL) == MR_Success) {                               // move to trigger FINDA
      moveSmooth(AX_PUL, FILAMENT_PARKING_STEPS, 650, false, false, ACC_NORMAL);                                // move to filament parking position
    }
    if (digitalRead(A1)) fixTheProblem();                                                                       // If -1000 steps didn't trigger FINDA
    isFilamentLoaded = false;                                                                                   // update global variable filament unloaded
    tmc2130_disable_axis(AX_PUL, tmc2130_mode);
    engage_filament_pulley(false);
    return true;
}
