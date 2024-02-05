#include "Configuration.h"
#include "config.h"

#ifdef UVLO_SUPPORT
#include <avr/wdt.h>
#include <Arduino.h> // For HIGH and LOW macros
#include "backlight.h"
#include "cardreader.h"
#include "cmdqueue.h"
#include "eeprom.h"
#include "fastio.h"
#include "language.h"
#include "lcd.h"
#include "mesh_bed_leveling.h"
#include "mesh_bed_calibration.h"
#include "messages.h"
#include "planner.h"
#include "power_panic.h"
#include "stepper.h"
#include "system_timer.h"
#include "tmc2130.h"
#include "temperature.h"
#include "ultralcd.h"

static const char MSG_INT4[] PROGMEM = "INT4";

static bool recover_machine_state_after_power_panic();
static void restore_print_from_eeprom(bool mbl_was_active);

static void uvlo_drain_reset() {
    // burn all that residual power
    wdt_enable(WDTO_1S);
    WRITE(BEEPER,HIGH);
    lcd_clear();
    lcd_puts_at_P(0, 1, MSG_POWERPANIC_DETECTED);
    while(1);
}


void uvlo_() {
    unsigned long time_start = _millis();

    // True if a print is already saved to RAM
    const bool sd_print_saved_in_ram = saved_printing && (saved_printing_type == PowerPanic::PRINT_TYPE_SD);
    const bool pos_invalid = mesh_bed_leveling_flag || homing_flag;

    // Conserve as much power as soon as possible
    // Turn off the LCD backlight
#ifdef LCD_BL_PIN
    backlightMode = BACKLIGHT_MODE_DIM;
    backlightLevel_LOW = 0;
    backlight_update();
#endif //LCD_BL_PIN

    // Disable X and Y motors to conserve power
    disable_x();
    disable_y();

    // Minimise Z and E motor currents (Hold and Run)
#ifdef TMC2130
    currents[Z_AXIS].setiHold(20);
    currents[Z_AXIS].setiRun(20);
    tmc2130_setup_chopper(Z_AXIS, tmc2130_mres[Z_AXIS]);
    currents[E_AXIS].setiHold(20);
    currents[E_AXIS].setiRun(20);
    tmc2130_setup_chopper(E_AXIS, tmc2130_mres[E_AXIS]);
#endif //TMC2130

    if (!sd_print_saved_in_ram && !isPartialBackupAvailable)
    {
        saved_bed_temperature = target_temperature_bed;
        saved_extruder_temperature = target_temperature[active_extruder];
        saved_extruder_relative_mode = axis_relative_modes & E_AXIS_MASK;
        saved_fan_speed = fanSpeed;
    }

    // Stop all heaters before continuing
    disable_heater();

    // Fetch data not included in a partial back-up
    if (!sd_print_saved_in_ram) {
        // Calculate the file position, from which to resume this print.
        save_print_file_state();

        // save the global state at planning time
        save_planner_global_state();
    }

    // From this point on and up to the print recovery, Z should not move during X/Y travels and
    // should be controlled precisely. Reset the MBL status before planner_abort_hard in order to
    // get the physical Z for further manipulation.
    bool mbl_was_active = mbl.active;
    mbl.active = false;

    // After this call, the planner queue is emptied and the current_position is set to a current logical coordinate.
    // The logical coordinate will likely differ from the machine coordinate if the skew calibration and mesh bed leveling
    // are in action.
    planner_abort_hard();

    // When there is no position already saved, then we must grab whatever the current position is.
    // This is most likely a position where the printer is in the middle of a G-code move
    if (!sd_print_saved_in_ram && !isPartialBackupAvailable)
    {
        memcpy(saved_pos, current_position, sizeof(saved_pos));
        if (pos_invalid) saved_pos[X_AXIS] = X_COORD_INVALID;
    }

    // Store the print logical Z position, which we need to recover (a slight error here would be
    // recovered on the next Gcode instruction, while a physical location error would not)
    float logical_z = saved_pos[Z_AXIS];
    if(mbl_was_active) {
        // Mesh bed leveling was being actively applied to the Z-position. Revert the
        // mesh bed leveling offset value.
        logical_z -= mbl.get_z(saved_pos[X_AXIS], saved_pos[Y_AXIS]);
    }
    eeprom_update_float((float*)EEPROM_UVLO_CURRENT_POSITION_Z, logical_z);

    // Store the print E position before we lose track
    eeprom_update_float((float*)(EEPROM_UVLO_CURRENT_POSITION_E), saved_pos[E_AXIS]);
    eeprom_update_byte((uint8_t*)EEPROM_UVLO_E_ABS, !saved_extruder_relative_mode);

    // Clean the input command queue, inhibit serial processing using saved_printing
    cmdqueue_reset();
    card.sdprinting = false;
    saved_printing = true;

    // Enable stepper driver interrupt to move Z axis. This should be fine as the planner and
    // command queues are empty, SD card printing is disabled, usb is inhibited.
    planner_aborted = false;
    sei();

    // Retract
    current_position[E_AXIS] -= default_retraction;
    plan_buffer_line_curposXYZE(95);
    st_synchronize();
    disable_e0();

    // Read out the current Z motor microstep counter to move the axis up towards
    // a full step before powering off. NOTE: we need to ensure to schedule more
    // than "dropsegments" steps in order to move (this is always the case here
    // due to UVLO_Z_AXIS_SHIFT being used)
    uint16_t z_res = tmc2130_get_res(Z_AXIS);
    uint16_t z_microsteps = tmc2130_rd_MSCNT(Z_AXIS);
    current_position[Z_AXIS] += float(1024 - z_microsteps)
                                / (z_res * cs.axis_steps_per_mm[Z_AXIS])
                                + UVLO_Z_AXIS_SHIFT;
    plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS]/60);
    st_synchronize();
    poweroff_z();

    // Write the file position.
    eeprom_update_dword((uint32_t*)(EEPROM_FILE_POSITION), saved_sdpos);

    // Store the mesh bed leveling offsets. This is 2*7*7=98 bytes, which takes 98*3.4us=333us in worst case.
    for (uint8_t mesh_point = 0; mesh_point < MESH_NUM_X_POINTS * MESH_NUM_Y_POINTS; ++ mesh_point)
    {
        uint8_t ix = mesh_point % MESH_NUM_X_POINTS; // from 0 to MESH_NUM_X_POINTS - 1
        uint8_t iy = mesh_point / MESH_NUM_X_POINTS;
        // Scale the z value to 1u resolution.
        int16_t v = mbl_was_active ? int16_t(floor(mbl.z_values[iy][ix] * 1000.f + 0.5f)) : 0;
        eeprom_update_word((uint16_t*)(EEPROM_UVLO_MESH_BED_LEVELING_FULL +2*mesh_point), *reinterpret_cast<uint16_t*>(&v));
    }

#ifdef DEBUG_UVLO
    print_world_coordinates();
    print_physical_coordinates();
#endif //End DEBUG_UVLO

    // Write the _final_ Z position
    eeprom_update_float((float*)EEPROM_UVLO_TINY_CURRENT_POSITION_Z, current_position[Z_AXIS]);

    // Store the current position.
    eeprom_update_float((float*)(EEPROM_UVLO_CURRENT_POSITION + 0), saved_pos[X_AXIS]);
    eeprom_update_float((float*)(EEPROM_UVLO_CURRENT_POSITION + 4), saved_pos[Y_AXIS]);

    // Store the current feed rate, temperatures, fan speed and extruder multipliers (flow rates)
    eeprom_update_word((uint16_t*)EEPROM_UVLO_FEEDRATE, saved_feedrate2);
    eeprom_update_word((uint16_t*)EEPROM_UVLO_FEEDMULTIPLY, feedmultiply);
    eeprom_update_word((uint16_t*)EEPROM_UVLO_TARGET_HOTEND, saved_extruder_temperature);
    eeprom_update_byte((uint8_t*)EEPROM_UVLO_TARGET_BED, saved_bed_temperature);
    eeprom_update_byte((uint8_t*)EEPROM_UVLO_FAN_SPEED, saved_fan_speed);
    eeprom_update_float((float*)(EEPROM_EXTRUDER_MULTIPLIER_0), extruder_multiplier[0]);
    eeprom_update_word((uint16_t*)(EEPROM_EXTRUDEMULTIPLY), (uint16_t)extrudemultiply);

    eeprom_update_float((float*)(EEPROM_UVLO_ACCELL), cs.acceleration);
    eeprom_update_float((float*)(EEPROM_UVLO_RETRACT_ACCELL), cs.retract_acceleration);
    eeprom_update_float((float*)(EEPROM_UVLO_TRAVEL_ACCELL), cs.travel_acceleration);

    // Store the saved target
    eeprom_update_block(saved_start_position, (float *)EEPROM_UVLO_SAVED_START_POSITION, sizeof(saved_start_position));

    eeprom_update_word((uint16_t*)EEPROM_UVLO_SAVED_SEGMENT_IDX, saved_segment_idx);
    eeprom_update_byte((uint8_t*)EEPROM_UVLO_PRINT_TYPE, saved_printing_type);

#ifdef LIN_ADVANCE
    eeprom_update_float((float*)(EEPROM_UVLO_LA_K), extruder_advance_K);
#endif

    // Finally store the "power outage" flag.
    eeprom_update_byte((uint8_t*)EEPROM_UVLO, PowerPanic::PENDING_RECOVERY);

    // Increment power failure counter
    eeprom_increment_byte((uint8_t*)EEPROM_POWER_COUNT);
    eeprom_increment_word((uint16_t*)EEPROM_POWER_COUNT_TOT);
#ifdef DEBUG_UVLO
    printf_P(_N("EEPROM_UVLO: %d\n"), uint8_t(eeprom_read_byte((uint8_t*)EEPROM_UVLO)));
    printf_P(_N("EEPROM_UVLO_PRINT_TYPE: %d\n"), uint8_t(eeprom_read_byte((uint8_t*)EEPROM_UVLO_PRINT_TYPE)));
#endif //End DEBUG_UVLO
    printf_P(_N("UVLO - end %d\n"), _millis() - time_start);

    WRITE(BEEPER,HIGH);

    // All is set: with all the juice left, try to move extruder away to detach the nozzle completely from the print
    poweron_z();
    current_position[X_AXIS] = (current_position[X_AXIS] < 0.5f * (X_MIN_POS + X_MAX_POS)) ? X_MIN_POS : X_MAX_POS;
    plan_buffer_line_curposXYZE(500);
    st_synchronize();

    wdt_enable(WDTO_1S);
    while(1);
}


static void uvlo_tiny() {
    unsigned long time_start = _millis();

    // Conserve power as soon as possible.
    disable_x();
    disable_y();
    disable_e0();

#ifdef TMC2130
    currents[Z_AXIS].setiHold(20);
    currents[Z_AXIS].setiRun(20);
    tmc2130_setup_chopper(Z_AXIS, tmc2130_mres[Z_AXIS]);
#endif //TMC2130

    // Stop all heaters
    disable_heater();

    // When power is interrupted on the _first_ recovery an attempt can be made to raise the
    // extruder, causing the Z position to change. Similarly, when recovering, the Z position is
    // lowered. In such cases we cannot just save Z, we need to re-align the steppers to a fullstep.
    // Disable MBL (if not already) to work with physical coordinates.

#ifdef DEBUG_UVLO
    print_world_coordinates();
    print_physical_coordinates();
#endif //End DEBUG_UVLO
    mbl.active = false;
    planner_abort_hard();

    // Allow for small roundoffs to be ignored
    if(fabs(current_position[Z_AXIS] - eeprom_read_float((float*)(EEPROM_UVLO_TINY_CURRENT_POSITION_Z))) >= 1.f/cs.axis_steps_per_mm[Z_AXIS])
    {
        // Clean the input command queue, inhibit serial processing using saved_printing
        cmdqueue_reset();
        card.sdprinting = false;
        saved_printing = true;

        // Enable stepper driver interrupt to move Z axis. This should be fine as the planner and
        // command queues are empty, SD card printing is disabled, usb is inhibited.
        planner_aborted = false;
        sei();

        // The axis was moved: adjust Z as done on a regular UVLO.
        uint16_t z_res = tmc2130_get_res(Z_AXIS);
        uint16_t z_microsteps = tmc2130_rd_MSCNT(Z_AXIS);
        current_position[Z_AXIS] += float(1024 - z_microsteps)
                                    / (z_res * cs.axis_steps_per_mm[Z_AXIS])
                                    + UVLO_TINY_Z_AXIS_SHIFT;
        plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS]/60);
        st_synchronize();
        poweroff_z();

        // Update Z position
        eeprom_update_float((float*)(EEPROM_UVLO_TINY_CURRENT_POSITION_Z), current_position[Z_AXIS]);
    }

    // Update the the "power outage" flag.
    eeprom_update_byte((uint8_t*)EEPROM_UVLO, PowerPanic::PENDING_RECOVERY_RETRY);

    // Increment power failure counter
    eeprom_increment_byte((uint8_t*)EEPROM_POWER_COUNT);
    eeprom_increment_word((uint16_t*)EEPROM_POWER_COUNT_TOT);
#ifdef DEBUG_UVLO
    printf_P(_N("EEPROM_UVLO: %d\n"), uint8_t(eeprom_read_byte((uint8_t*)EEPROM_UVLO)));
    printf_P(_N("EEPROM_UVLO_PRINT_TYPE: %d\n"), uint8_t(eeprom_read_byte((uint8_t*)EEPROM_UVLO_PRINT_TYPE)));
#endif //End DEBUG_UVLO
    printf_P(_N("UVLO_TINY - end %d\n"), _millis() - time_start);
    uvlo_drain_reset();
}

void setup_uvlo_interrupt() {
    DDRE &= ~(1 << 4); //input pin
    PORTE &= ~(1 << 4); //no internal pull-up

    // sensing falling edge
    EICRB |= (1 << 0);
    EICRB &= ~(1 << 1);

    // enable INT4 interrupt
    EIMSK |= (1 << 4);

    // check if power was lost before we armed the interrupt
    if(!(PINE & (1 << 4)) && eeprom_read_byte((uint8_t*)EEPROM_UVLO) != PowerPanic::NO_PENDING_RECOVERY)
    {
        SERIAL_ECHOLNRPGM(MSG_INT4);
        uvlo_drain_reset();
    }
}

ISR(INT4_vect) {
    EIMSK &= ~(1 << 4); //disable INT4 interrupt to make sure that this code will be executed just once
    SERIAL_ECHOLNRPGM(MSG_INT4);
    if (eeprom_read_byte((uint8_t*)EEPROM_UVLO) == PowerPanic::NO_PENDING_RECOVERY)
    {
        if(printer_active()) {
            uvlo_();
        }
    } else {
        // There is already a pending recovery waiting. Power outage in this scenario
        // arrives before we can fully recover the print. In that case call a 'tiny'
        // version of uvlo_() which doesn't overwrite the print state already waiting in EEPROM
        uvlo_tiny();
    }
}

void recover_print(uint8_t automatic) {
    lcd_setstatuspgm(_i("Recovering print"));////MSG_RECOVERING_PRINT c=20

    // Recover position, temperatures and extrude_multipliers
    bool mbl_was_active = recover_machine_state_after_power_panic();

    // Lift the print head ONCE plus Z_PAUSE_LIFT, first to avoid collisions with oozed material with the print,
    // and second also so one may remove the excess priming material.

#ifdef DEBUG_UVLO
    print_world_coordinates();
    print_physical_coordinates();
#endif //End DEBUG_UVLO

    if(eeprom_read_byte((uint8_t*)EEPROM_UVLO) == PowerPanic::PENDING_RECOVERY)
    {
        enquecommandf_P(PSTR("G1 Z%.3f F800"), current_position[Z_AXIS] + Z_PAUSE_LIFT);
        eeprom_update_byte((uint8_t*)EEPROM_UVLO, PowerPanic::PENDING_RECOVERY_RETRY);
    }

#ifdef DEBUG_UVLO
    print_world_coordinates();
    print_physical_coordinates();
#endif //End DEBUG_UVLO

    // Home X and Y axes. Homing just X and Y shall not touch the babystep and the world2machine
    // transformation status. G28 will not touch Z when MBL is off.
    enquecommand_P(PSTR("G28 X Y"));
    // Set the target bed and nozzle temperatures and wait.
    enquecommandf_P(PSTR("M104 S%d"), target_temperature[active_extruder]);
    enquecommandf_P(PSTR("M140 S%d"), target_temperature_bed);
    enquecommandf_P(PSTR("M109 S%d"), target_temperature[active_extruder]);
    enquecommand_P(MSG_M83); //E axis relative mode

    // If not automatically recoreverd (long power loss)
    if(automatic == 0){
        //Extrude some filament to stabilize the pressure
        enquecommand_P(PSTR("G1 E5 F120"));
        // Retract to be consistent with a short pause
        enquecommandf_P(G1_E_F2700, default_retraction);
    }

    puts_P(_N("Temperature Restored\n"));
    gcode_M114();

    // Restart the print.
    restore_print_from_eeprom(mbl_was_active);
    puts_P(_N("Done reading EEPROM\n"));
    gcode_M114();
#ifdef DEBUG_UVLO
    printf_P(_N("EEPROM_UVLO: %d\n"), uint8_t(eeprom_read_byte((uint8_t*)EEPROM_UVLO)));
    printf_P(_N("EEPROM_UVLO_PRINT_TYPE: %d\n"), uint8_t(eeprom_read_byte((uint8_t*)EEPROM_UVLO_PRINT_TYPE)));
#endif //End DEBUG_UVLO
}

bool recover_machine_state_after_power_panic() {
    // 1) Preset some dummy values for the XY axes
    current_position[X_AXIS] = 0;
    current_position[Y_AXIS] = 0;

    // 2) Restore the mesh bed leveling offsets, but not the MBL status.
    // This is 2*7*7=98 bytes, which takes 98*3.4us=333us in worst case.
    bool mbl_was_active = false;
    for (int8_t mesh_point = 0; mesh_point < MESH_NUM_X_POINTS * MESH_NUM_Y_POINTS; ++ mesh_point) {
        uint8_t ix = mesh_point % MESH_NUM_X_POINTS; // from 0 to MESH_NUM_X_POINTS - 1
        uint8_t iy = mesh_point / MESH_NUM_X_POINTS;
        // Scale the z value to 10u resolution.
        int16_t v;
        eeprom_read_block(&v, (void*)(EEPROM_UVLO_MESH_BED_LEVELING_FULL+2*mesh_point), 2);
        if (v != 0)
            mbl_was_active = true;
        mbl.z_values[iy][ix] = float(v) * 0.001f;
    }

    // Recover the physical coordinate of the Z axis at the time of the power panic.
    // The current position after power panic is moved to the next closest 0th full step.
    current_position[Z_AXIS] = eeprom_read_float((float*)(EEPROM_UVLO_TINY_CURRENT_POSITION_Z));

    // Recover last E axis position
    current_position[E_AXIS] = eeprom_read_float((float*)(EEPROM_UVLO_CURRENT_POSITION_E));

    // 3) Initialize the logical to physical coordinate system transformation.
    world2machine_initialize();

    // 4) Load the baby stepping value, which is expected to be active at the time of power panic.
    // The baby stepping value is used to reset the physical Z axis when rehoming the Z axis.
    babystep_load();

    // 5) Set the physical positions from the logical positions using the world2machine transformation
    // This is only done to inizialize Z/E axes with physical locations, since X/Y are unknown.
    clamp_to_software_endstops(current_position);
    set_destination_to_current();
    plan_set_position_curposXYZE();
#ifdef DEBUG_UVLO
    SERIAL_ECHOLNPGM("recover_machine_state_after_power_panic, initial ");
    print_world_coordinates();
    print_physical_coordinates();
#endif //End DEBUG_UVLO

    // 6) Power up the Z motors, mark their positions as known.
    axis_known_position[Z_AXIS] = true;
    enable_z();

    // 7) Recover the target temperatures.
    target_temperature[active_extruder] = eeprom_read_word((uint16_t*)EEPROM_UVLO_TARGET_HOTEND);
    target_temperature_bed = eeprom_read_byte((uint8_t*)EEPROM_UVLO_TARGET_BED);

    // 8) Recover extruder multipilers
    extruder_multiplier[0] = eeprom_read_float((float*)(EEPROM_EXTRUDER_MULTIPLIER_0));
    extrudemultiply = (int)eeprom_read_word((uint16_t*)(EEPROM_EXTRUDEMULTIPLY));

    // 9) Recover the saved target
    eeprom_read_block(saved_start_position, (float *)EEPROM_UVLO_SAVED_START_POSITION, sizeof(saved_start_position));

    saved_segment_idx = eeprom_read_word((uint16_t*)EEPROM_UVLO_SAVED_SEGMENT_IDX);

#ifdef LIN_ADVANCE
    extruder_advance_K = eeprom_read_float((float*)EEPROM_UVLO_LA_K);
#endif

     return mbl_was_active;
#ifdef DEBUG_UVLO
    printf_P(_N("EEPROM_UVLO: %d\n"), uint8_t(eeprom_read_byte((uint8_t*)EEPROM_UVLO)));
    printf_P(_N("EEPROM_UVLO_PRINT_TYPE: %d\n"), uint8_t(eeprom_read_byte((uint8_t*)EEPROM_UVLO_PRINT_TYPE)));
#endif //End DEBUG_UVLO
}

void restore_print_from_eeprom(bool mbl_was_active) {
    int feedrate_rec;
    int feedmultiply_rec;
    uint8_t fan_speed_rec;

    fan_speed_rec = eeprom_read_byte((uint8_t*)EEPROM_UVLO_FAN_SPEED);
    feedrate_rec = eeprom_read_word((uint16_t*)EEPROM_UVLO_FEEDRATE);
    feedmultiply_rec = eeprom_read_word((uint16_t*)EEPROM_UVLO_FEEDMULTIPLY);
    SERIAL_ECHOPGM("Feedrate:");
    MYSERIAL.print(feedrate_rec);
    SERIAL_ECHOPGM(", feedmultiply:");
    MYSERIAL.println(feedmultiply_rec);

    if (eeprom_read_byte((uint8_t*)EEPROM_UVLO_PRINT_TYPE) == PowerPanic::PRINT_TYPE_SD)
    { // M23
        restore_file_from_sd();
    }

    // Move to the XY print position in logical coordinates, where the print has been killed, but
    // without shifting Z along the way. This requires performing the move without mbl.
    float pos_x = eeprom_read_float((float*)(EEPROM_UVLO_CURRENT_POSITION + 0));
    float pos_y = eeprom_read_float((float*)(EEPROM_UVLO_CURRENT_POSITION + 4));
    if (pos_x != X_COORD_INVALID) {
        enquecommandf_P(PSTR("G1 X%-.3f Y%-.3f F3000"), pos_x, pos_y);
    }

    // Enable MBL and switch to logical positioning
    if (mbl_was_active)
        enquecommand_P(PSTR("PRUSA MBL V1"));

    // Move the Z axis down to the print, in logical coordinates.
    enquecommandf_P(PSTR("G1 Z%-.3f"), eeprom_read_float((float*)(EEPROM_UVLO_CURRENT_POSITION_Z)));

    // Restore acceleration settings
    float acceleration = eeprom_read_float((float*)(EEPROM_UVLO_ACCELL));
    float retract_acceleration = eeprom_read_float((float*)(EEPROM_UVLO_RETRACT_ACCELL));
    float travel_acceleration = eeprom_read_float((float*)(EEPROM_UVLO_TRAVEL_ACCELL));
    // accelerations are usually ordinary numbers, no need to keep extensive amount of decimal places
    enquecommandf_P(PSTR("M204 P%-.1f R%-.1f T%-.1f"), acceleration, retract_acceleration, travel_acceleration);

    // Unretract.
    enquecommandf_P(G1_E_F2700, default_retraction);
    // Recover final E axis position and mode
    float pos_e = eeprom_read_float((float*)(EEPROM_UVLO_CURRENT_POSITION_E));
    enquecommandf_P(PSTR("G92 E%-.3f"), pos_e);
    if (eeprom_read_byte((uint8_t*)EEPROM_UVLO_E_ABS))
        enquecommand_P(PSTR("M82")); //E axis abslute mode
    // Set the feedrates saved at the power panic.
    enquecommandf_P(PSTR("G1 F%d"), feedrate_rec);
    enquecommandf_P(MSG_M220, feedmultiply_rec);
    // Set the fan speed saved at the power panic.
    enquecommandf_P(PSTR("M106 S%u"), fan_speed_rec);

    // SD: Position in file, USB: g-code line number
    uint32_t position = eeprom_read_dword((uint32_t*)(EEPROM_FILE_POSITION));
    if (eeprom_read_byte((uint8_t*)EEPROM_UVLO_PRINT_TYPE) == PowerPanic::PRINT_TYPE_SD)
    {
        // Set a position in the file.
        enquecommandf_P(PSTR("M26 S%lu"), position);
    }
    else if (eeprom_read_byte((uint8_t*)EEPROM_UVLO_PRINT_TYPE) == PowerPanic::PRINT_TYPE_HOST)
    {
        // Set line number
        enquecommandf_P(PSTR("M110 N%lu"), position);
    }
#ifdef DEBUG_UVLO
    printf_P(_N("EEPROM_UVLO: %d\n"), uint8_t(eeprom_read_byte((uint8_t*)EEPROM_UVLO)));
    printf_P(_N("EEPROM_UVLO_PRINT_TYPE: %d\n"), uint8_t(eeprom_read_byte((uint8_t*)EEPROM_UVLO_PRINT_TYPE)));
#endif //End DEBUG_UVLO
    enquecommand_P(PSTR("G4 S0"));
    enquecommand_P(PSTR("PRUSA uvlo"));
}
#endif //UVLO_SUPPORT
