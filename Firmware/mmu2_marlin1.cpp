/// @file
/// MK3 / Marlin1 implementation of support routines for the MMU2
#include "mmu2_marlin.h"
#include "Marlin.h"
#include "stepper.h"
#include "planner.h"
#include "mmu2_config.h"
#include "temperature.h"

namespace MMU2 {

void MoveE(float delta, float feedRate) {
    current_position[E_AXIS] += delta;
    planner_line_to_current_position(feedRate);
}

float MoveRaiseZ(float delta) {
    return raise_z(delta);
}

void planner_synchronize() {
    st_synchronize();
}

bool planner_any_moves() {
    return blocks_queued();
}

float planner_get_machine_position_E_mm(){
    return current_position[E_AXIS];
}

float planner_get_current_position_E(){
    return current_position[E_AXIS];
}

void planner_set_current_position_E(float e){
        current_position[E_AXIS] = e;
}

void planner_line_to_current_position(float feedRate_mm_s){
     plan_buffer_line_curposXYZE(feedRate_mm_s);
}

void planner_line_to_current_position_sync(float feedRate_mm_s){
    planner_line_to_current_position(feedRate_mm_s);
    planner_synchronize();
}

pos3d planner_current_position(){
    return pos3d(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS]);
}

void motion_do_blocking_move_to_xy(float rx, float ry, float feedRate_mm_s){
    current_position[X_AXIS] = rx;
    current_position[Y_AXIS] = ry;
    planner_line_to_current_position_sync(feedRate_mm_s);
}

void motion_do_blocking_move_to_z(float z, float feedRate_mm_s){
    current_position[Z_AXIS] = z;
    planner_line_to_current_position_sync(feedRate_mm_s);
}

void nozzle_park() {
    current_position[X_AXIS] = MMU_ERR_X_PAUSE_POS;
    current_position[Y_AXIS] = MMU_ERR_Y_PAUSE_POS;
    planner_line_to_current_position_sync(NOZZLE_PARK_XY_FEEDRATE);
}

bool marlin_printingIsActive() {
    // return IS_SD_PRINTING || usb_timer_running();
    return printer_active();
}

void marlin_manage_heater(){
    manage_heater();
}

void marlin_manage_inactivity(bool b){
    manage_inactivity(b);
}

void marlin_idle(bool b){
    manage_heater();
    manage_inactivity(b);
}

int16_t thermal_degTargetHotend() {
    return degTargetHotend(0);
}

int16_t thermal_degHotend() {
    return degHotend(0);
}

void thermal_setExtrudeMintemp(int16_t t) {
    set_extrude_min_temp(t);
}

void thermal_setTargetHotend(int16_t t) {
    setTargetHotend(t);
}

void safe_delay_keep_alive(uint16_t t) {
    delay_keep_alive(t);
}

void gcode_reset_stepper_timeout(){
    // empty
}

void Enable_E0(){ enable_e0(); }
void Disable_E0(){ disable_e0(); }

bool all_axes_homed(){
    return axis_known_position[X_AXIS] && axis_known_position[Y_AXIS];
}

bool cutter_enabled(){
    return eeprom_read_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED) == EEPROM_MMU_CUTTER_ENABLED_enabled;
}

} // namespace MMU2
