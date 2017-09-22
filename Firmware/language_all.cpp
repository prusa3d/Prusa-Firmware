
#include "Configuration_prusa.h"
#include "language_all.h"

#define LCD_WIDTH 20
extern unsigned char lang_selected;

const char MSG_ACTIVE_EXTRUDER_EN[] PROGMEM = "Active Extruder: ";
const char * const MSG_ACTIVE_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ACTIVE_EXTRUDER_EN
};

const char MSG_ADJUSTZ_EN[] PROGMEM = "Auto adjust Z?";
const char * const MSG_ADJUSTZ_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ADJUSTZ_EN
};

const char MSG_ALL_EN[] PROGMEM = "All";
const char * const MSG_ALL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ALL_EN
};

const char MSG_AMAX_EN[] PROGMEM = "Amax ";
const char * const MSG_AMAX_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AMAX_EN
};

const char MSG_AUTHOR_EN[] PROGMEM = " | Author: ";
const char * const MSG_AUTHOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AUTHOR_EN
};

const char MSG_AUTO_HOME_EN[] PROGMEM = "Auto home";
const char * const MSG_AUTO_HOME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AUTO_HOME_EN
};

const char MSG_A_RETRACT_EN[] PROGMEM = "A-retract";
const char * const MSG_A_RETRACT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_A_RETRACT_EN
};

const char MSG_BABYSTEPPING_X_EN[] PROGMEM = "Babystepping X";
const char * const MSG_BABYSTEPPING_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEPPING_X_EN
};

const char MSG_BABYSTEPPING_Y_EN[] PROGMEM = "Babystepping Y";
const char * const MSG_BABYSTEPPING_Y_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEPPING_Y_EN
};

const char MSG_BABYSTEPPING_Z_EN[] PROGMEM = "Adjusting Z";
const char * const MSG_BABYSTEPPING_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEPPING_Z_EN
};

const char MSG_BABYSTEP_X_EN[] PROGMEM = "Babystep X";
const char * const MSG_BABYSTEP_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEP_X_EN
};

const char MSG_BABYSTEP_Y_EN[] PROGMEM = "Babystep Y";
const char * const MSG_BABYSTEP_Y_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEP_Y_EN
};

const char MSG_BABYSTEP_Z_EN[] PROGMEM = "Live adjust Z";
const char * const MSG_BABYSTEP_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEP_Z_EN
};

const char MSG_BABYSTEP_Z_NOT_SET_EN[] PROGMEM = "Distance between tip of the nozzle and the bed surface has not been set yet. Please follow the manual, chapter First steps, section First layer calibration.";
const char * const MSG_BABYSTEP_Z_NOT_SET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEP_Z_NOT_SET_EN
};

const char MSG_BED_EN[] PROGMEM = "Bed";
const char * const MSG_BED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_EN
};

const char MSG_BED_CORRECTION_FRONT_EN[] PROGMEM = "Front side[um]";
const char * const MSG_BED_CORRECTION_FRONT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_FRONT_EN
};

const char MSG_BED_CORRECTION_LEFT_EN[] PROGMEM = "Left side [um]";
const char * const MSG_BED_CORRECTION_LEFT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_LEFT_EN
};

const char MSG_BED_CORRECTION_MENU_EN[] PROGMEM = "Bed level correct";
const char * const MSG_BED_CORRECTION_MENU_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_MENU_EN
};

const char MSG_BED_CORRECTION_REAR_EN[] PROGMEM = "Rear side [um]";
const char * const MSG_BED_CORRECTION_REAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_REAR_EN
};

const char MSG_BED_CORRECTION_RESET_EN[] PROGMEM = "Reset";
const char * const MSG_BED_CORRECTION_RESET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_RESET_EN
};

const char MSG_BED_CORRECTION_RIGHT_EN[] PROGMEM = "Right side[um]";
const char * const MSG_BED_CORRECTION_RIGHT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_RIGHT_EN
};

const char MSG_BED_DONE_EN[] PROGMEM = "Bed done";
const char * const MSG_BED_DONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_DONE_EN
};

const char MSG_BED_HEATING_EN[] PROGMEM = "Bed Heating";
const char * const MSG_BED_HEATING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_HEATING_EN
};

const char MSG_BED_LEVELING_FAILED_POINT_HIGH_EN[] PROGMEM = "Bed leveling failed. Sensor triggered too high. Waiting for reset.";
const char * const MSG_BED_LEVELING_FAILED_POINT_HIGH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_LEVELING_FAILED_POINT_HIGH_EN
};

const char MSG_BED_LEVELING_FAILED_POINT_LOW_EN[] PROGMEM = "Bed leveling failed. Sensor didnt trigger. Debris on nozzle? Waiting for reset.";
const char * const MSG_BED_LEVELING_FAILED_POINT_LOW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_LEVELING_FAILED_POINT_LOW_EN
};

const char MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_EN[] PROGMEM = "Bed leveling failed. Sensor disconnected or cable broken. Waiting for reset.";
const char * const MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_EN
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_EN[] PROGMEM = "XYZ calibration failed. Front calibration points not reachable.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_EN
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_EN[] PROGMEM = "XYZ calibration failed. Left front calibration point not reachable.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_EN
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_EN[] PROGMEM = "XYZ calibration failed. Right front calibration point not reachable.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_EN
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_EN[] PROGMEM = "XYZ calibration failed. Please consult the manual.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_EN
};

const char MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_EN[] PROGMEM = "XYZ calibration ok. X/Y axes are perpendicular. Congratulations!";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_EN
};

const char MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_EN[] PROGMEM = "XYZ calibration failed. Bed calibration point was not found.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_EN
};

const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_EN[] PROGMEM = "XYZ calibration all right. Skew will be corrected automatically.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_EN
};

const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_EN[] PROGMEM = "XYZ calibration all right. X/Y axes are slightly skewed. Good job!";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_EN
};

const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_EN[] PROGMEM = "XYZ calibration compromised. Front calibration points not reachable.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_EN
};

const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_EN[] PROGMEM = "XYZ calibration compromised. Left front calibration point not reachable.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_EN
};

const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_EN[] PROGMEM = "XYZ calibration compromised. Right front calibration point not reachable.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_EN
};

const char MSG_BEGIN_FILE_LIST_EN[] PROGMEM = "Begin file list";
const char * const MSG_BEGIN_FILE_LIST_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BEGIN_FILE_LIST_EN
};

const char MSG_BROWNOUT_RESET_EN[] PROGMEM = " Brown out Reset";
const char * const MSG_BROWNOUT_RESET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BROWNOUT_RESET_EN
};

const char MSG_CALIBRATE_BED_EN[] PROGMEM = "Calibrate XYZ";
const char * const MSG_CALIBRATE_BED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_BED_EN
};

const char MSG_CALIBRATE_BED_RESET_EN[] PROGMEM = "Reset XYZ calibr.";
const char * const MSG_CALIBRATE_BED_RESET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_BED_RESET_EN
};

const char MSG_CALIBRATE_E_EN[] PROGMEM = "Calibrate E";
const char * const MSG_CALIBRATE_E_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_E_EN
};

const char MSG_CALIBRATE_PINDA_EN[] PROGMEM = "Calibrate";
const char * const MSG_CALIBRATE_PINDA_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_PINDA_EN
};

const char MSG_CALIBRATE_Z_AUTO_EN[] PROGMEM = "Calibrating Z...";
const char * const MSG_CALIBRATE_Z_AUTO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_Z_AUTO_EN
};

const char MSG_CALIBRATION_PINDA_MENU_EN[] PROGMEM = "Temp. calibration";
const char * const MSG_CALIBRATION_PINDA_MENU_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATION_PINDA_MENU_EN
};

const char MSG_CARD_MENU_EN[] PROGMEM = "Print from SD";
const char * const MSG_CARD_MENU_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CARD_MENU_EN
};

const char MSG_CHANGE_EXTR_EN[] PROGMEM = "Change extruder";
const char * const MSG_CHANGE_EXTR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHANGE_EXTR_EN
};

const char MSG_CHANGE_SUCCESS_EN[] PROGMEM = "Change success!";
const char * const MSG_CHANGE_SUCCESS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHANGE_SUCCESS_EN
};

const char MSG_CHANGING_FILAMENT_EN[] PROGMEM = "Changing filament!";
const char * const MSG_CHANGING_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHANGING_FILAMENT_EN
};

const char MSG_CHOOSE_EXTRUDER_EN[] PROGMEM = "Choose extruder:";
const char * const MSG_CHOOSE_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHOOSE_EXTRUDER_EN
};

const char MSG_CLEAN_NOZZLE_E_EN[] PROGMEM = "E calibration finished. Please clean the nozzle. Click when done.";
const char * const MSG_CLEAN_NOZZLE_E_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CLEAN_NOZZLE_E_EN
};

const char MSG_CNG_SDCARD_EN[] PROGMEM = "Change SD card";
const char * const MSG_CNG_SDCARD_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CNG_SDCARD_EN
};

const char MSG_CONFIGURATION_VER_EN[] PROGMEM = " Last Updated: ";
const char * const MSG_CONFIGURATION_VER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONFIGURATION_VER_EN
};

const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_EN[] PROGMEM = "Are left and right Z~carriages all up?";
const char * const MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_EN
};

const char MSG_CONFIRM_NOZZLE_CLEAN_EN[] PROGMEM = "Please clean the nozzle for calibration. Click when done.";
const char * const MSG_CONFIRM_NOZZLE_CLEAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONFIRM_NOZZLE_CLEAN_EN
};

const char MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_EN[] PROGMEM = "Filaments are now adjusted. Please clean the nozzle for calibration. Click when done.";
const char * const MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_EN
};

const char MSG_CONTROL_EN[] PROGMEM = "Control";
const char * const MSG_CONTROL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONTROL_EN
};

const char MSG_COOLDOWN_EN[] PROGMEM = "Cooldown";
const char * const MSG_COOLDOWN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_COOLDOWN_EN
};

const char MSG_CORRECTLY_EN[] PROGMEM = "Changed correctly?";
const char * const MSG_CORRECTLY_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CORRECTLY_EN
};

const char MSG_COUNT_X_EN[] PROGMEM = " Count X: ";
const char * const MSG_COUNT_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_COUNT_X_EN
};

const char MSG_CRASHDETECT_OFF_EN[] PROGMEM = "Crash det.  [off]";
const char * const MSG_CRASHDETECT_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CRASHDETECT_OFF_EN
};

const char MSG_CRASHDETECT_ON_EN[] PROGMEM = "Crash det.   [on]";
const char * const MSG_CRASHDETECT_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CRASHDETECT_ON_EN
};

const char MSG_CURRENT_EN[] PROGMEM = "Current";
const char * const MSG_CURRENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CURRENT_EN
};

const char MSG_DISABLE_STEPPERS_EN[] PROGMEM = "Disable steppers";
const char * const MSG_DISABLE_STEPPERS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_DISABLE_STEPPERS_EN
};

const char MSG_DWELL_EN[] PROGMEM = "Sleep...";
const char * const MSG_DWELL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_DWELL_EN
};

const char MSG_ENDSTOPS_HIT_EN[] PROGMEM = "endstops hit: ";
const char * const MSG_ENDSTOPS_HIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ENDSTOPS_HIT_EN
};

const char MSG_ENDSTOP_HIT_EN[] PROGMEM = "TRIGGERED";
const char * const MSG_ENDSTOP_HIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ENDSTOP_HIT_EN
};

const char MSG_ENDSTOP_OPEN_EN[] PROGMEM = "open";
const char * const MSG_ENDSTOP_OPEN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ENDSTOP_OPEN_EN
};

const char MSG_END_FILE_LIST_EN[] PROGMEM = "End file list";
const char * const MSG_END_FILE_LIST_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_END_FILE_LIST_EN
};

const char MSG_ERROR_EN[] PROGMEM = "ERROR:";
const char * const MSG_ERROR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERROR_EN
};

const char MSG_ERR_CHECKSUM_MISMATCH_EN[] PROGMEM = "checksum mismatch, Last Line: ";
const char * const MSG_ERR_CHECKSUM_MISMATCH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_CHECKSUM_MISMATCH_EN
};

const char MSG_ERR_COLD_EXTRUDE_STOP_EN[] PROGMEM = " cold extrusion prevented";
const char * const MSG_ERR_COLD_EXTRUDE_STOP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_COLD_EXTRUDE_STOP_EN
};

const char MSG_ERR_KILLED_EN[] PROGMEM = "Printer halted. kill() called!";
const char * const MSG_ERR_KILLED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_KILLED_EN
};

const char MSG_ERR_LINE_NO_EN[] PROGMEM = "Line Number is not Last Line Number+1, Last Line: ";
const char * const MSG_ERR_LINE_NO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_LINE_NO_EN
};

const char MSG_ERR_LONG_EXTRUDE_STOP_EN[] PROGMEM = " too long extrusion prevented";
const char * const MSG_ERR_LONG_EXTRUDE_STOP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_LONG_EXTRUDE_STOP_EN
};

const char MSG_ERR_NO_CHECKSUM_EN[] PROGMEM = "No Checksum with line number, Last Line: ";
const char * const MSG_ERR_NO_CHECKSUM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_NO_CHECKSUM_EN
};

const char MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_EN[] PROGMEM = "No Line Number with checksum, Last Line: ";
const char * const MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_EN
};

const char MSG_ERR_NO_THERMISTORS_EN[] PROGMEM = "No thermistors - no temperature";
const char * const MSG_ERR_NO_THERMISTORS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_NO_THERMISTORS_EN
};

const char MSG_ERR_STOPPED_EN[] PROGMEM = "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)";
const char * const MSG_ERR_STOPPED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_STOPPED_EN
};

const char MSG_EXTERNAL_RESET_EN[] PROGMEM = " External Reset";
const char * const MSG_EXTERNAL_RESET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_EXTERNAL_RESET_EN
};

const char MSG_EXTRUDER_EN[] PROGMEM = "Extruder";
const char * const MSG_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_EXTRUDER_EN
};

const char MSG_EXTRUDER_1_EN[] PROGMEM = "Extruder 1";
const char * const MSG_EXTRUDER_1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_EXTRUDER_1_EN
};

const char MSG_EXTRUDER_2_EN[] PROGMEM = "Extruder 2";
const char * const MSG_EXTRUDER_2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_EXTRUDER_2_EN
};

const char MSG_EXTRUDER_3_EN[] PROGMEM = "Extruder 3";
const char * const MSG_EXTRUDER_3_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_EXTRUDER_3_EN
};

const char MSG_EXTRUDER_4_EN[] PROGMEM = "Extruder 4";
const char * const MSG_EXTRUDER_4_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_EXTRUDER_4_EN
};

const char MSG_E_CAL_KNOB_EN[] PROGMEM = "Rotate knob until mark reaches extruder body. Click when done.";
const char * const MSG_E_CAL_KNOB_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_E_CAL_KNOB_EN
};

const char MSG_Enqueing_EN[] PROGMEM = "enqueing \"";
const char * const MSG_Enqueing_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_Enqueing_EN
};

const char MSG_FACTOR_EN[] PROGMEM = " \002 Fact";
const char * const MSG_FACTOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FACTOR_EN
};

const char MSG_FAN_SPEED_EN[] PROGMEM = "Fan speed";
const char * const MSG_FAN_SPEED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FAN_SPEED_EN
};

const char MSG_FARM_CARD_MENU_EN[] PROGMEM = "Farm mode print";
const char * const MSG_FARM_CARD_MENU_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FARM_CARD_MENU_EN
};

const char MSG_FILAMENTCHANGE_EN[] PROGMEM = "Change filament";
const char * const MSG_FILAMENTCHANGE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENTCHANGE_EN
};

const char MSG_FILAMENT_CLEAN_EN[] PROGMEM = "Is color clear?";
const char * const MSG_FILAMENT_CLEAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_CLEAN_EN
};

const char MSG_FILAMENT_LOADING_T0_EN[] PROGMEM = "Insert filament into extruder 1. Click when done.";
const char * const MSG_FILAMENT_LOADING_T0_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_LOADING_T0_EN
};

const char MSG_FILAMENT_LOADING_T1_EN[] PROGMEM = "Insert filament into extruder 2. Click when done.";
const char * const MSG_FILAMENT_LOADING_T1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_LOADING_T1_EN
};

const char MSG_FILAMENT_LOADING_T2_EN[] PROGMEM = "Insert filament into extruder 3. Click when done.";
const char * const MSG_FILAMENT_LOADING_T2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_LOADING_T2_EN
};

const char MSG_FILAMENT_LOADING_T3_EN[] PROGMEM = "Insert filament into extruder 4. Click when done.";
const char * const MSG_FILAMENT_LOADING_T3_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_LOADING_T3_EN
};

const char MSG_FILE_PRINTED_EN[] PROGMEM = "Done printing file";
const char * const MSG_FILE_PRINTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILE_PRINTED_EN
};

const char MSG_FILE_SAVED_EN[] PROGMEM = "Done saving file.";
const char * const MSG_FILE_SAVED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILE_SAVED_EN
};

const char MSG_FIL_ADJUSTING_EN[] PROGMEM = "Adjusting filaments. Please wait.";
const char * const MSG_FIL_ADJUSTING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIL_ADJUSTING_EN
};

const char MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_EN[] PROGMEM = "Iteration ";
const char * const MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_EN
};

const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_EN[] PROGMEM = "Searching bed calibration point";
const char * const MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_EN
};

const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_EN[] PROGMEM = " of 4";
const char * const MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_EN
};

const char MSG_FINISHING_MOVEMENTS_EN[] PROGMEM = "Finishing movements";
const char * const MSG_FINISHING_MOVEMENTS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FINISHING_MOVEMENTS_EN
};

const char MSG_FLOW_EN[] PROGMEM = "Flow";
const char * const MSG_FLOW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FLOW_EN
};

const char MSG_FLOW0_EN[] PROGMEM = "Flow 0";
const char * const MSG_FLOW0_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FLOW0_EN
};

const char MSG_FLOW1_EN[] PROGMEM = "Flow 1";
const char * const MSG_FLOW1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FLOW1_EN
};

const char MSG_FLOW2_EN[] PROGMEM = "Flow 2";
const char * const MSG_FLOW2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FLOW2_EN
};

const char MSG_FOLLOW_CALIBRATION_FLOW_EN[] PROGMEM = "Printer has not been calibrated yet. Please follow the manual, chapter First steps, section Calibration flow.";
const char * const MSG_FOLLOW_CALIBRATION_FLOW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FOLLOW_CALIBRATION_FLOW_EN
};

const char MSG_FREE_MEMORY_EN[] PROGMEM = " Free Memory: ";
const char * const MSG_FREE_MEMORY_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FREE_MEMORY_EN
};

const char MSG_FSENSOR_OFF_EN[] PROGMEM = "Fil. sensor [off]";
const char * const MSG_FSENSOR_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FSENSOR_OFF_EN
};

const char MSG_FSENSOR_ON_EN[] PROGMEM = "Fil. sensor  [on]";
const char * const MSG_FSENSOR_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FSENSOR_ON_EN
};

const char MSG_HEATING_EN[] PROGMEM = "Heating";
const char * const MSG_HEATING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HEATING_EN
};

const char MSG_HEATING_COMPLETE_EN[] PROGMEM = "Heating done.";
const char * const MSG_HEATING_COMPLETE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HEATING_COMPLETE_EN
};

const char MSG_HOMEYZ_EN[] PROGMEM = "Calibrate Z";
const char * const MSG_HOMEYZ_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HOMEYZ_EN
};

const char MSG_HOMEYZ_DONE_EN[] PROGMEM = "Calibration done";
const char * const MSG_HOMEYZ_DONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HOMEYZ_DONE_EN
};

const char MSG_HOMEYZ_PROGRESS_EN[] PROGMEM = "Calibrating Z";
const char * const MSG_HOMEYZ_PROGRESS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HOMEYZ_PROGRESS_EN
};

const char MSG_HOTEND_OFFSET_EN[] PROGMEM = "Hotend offsets:";
const char * const MSG_HOTEND_OFFSET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HOTEND_OFFSET_EN
};

const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_EN[] PROGMEM = "Improving bed calibration point";
const char * const MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_EN
};

const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_EN[] PROGMEM = " of 9";
const char * const MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_EN
};

const char MSG_INFO_EXTRUDER_EN[] PROGMEM = "Extruder info";
const char * const MSG_INFO_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INFO_EXTRUDER_EN
};

const char MSG_INFO_FILAMENT_XDIFF_EN[] PROGMEM = "Fil. Xd:";
const char * const MSG_INFO_FILAMENT_XDIFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INFO_FILAMENT_XDIFF_EN
};

const char MSG_INFO_FILAMENT_YDIFF_EN[] PROGMEM = "Fil. Ydiff:";
const char * const MSG_INFO_FILAMENT_YDIFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INFO_FILAMENT_YDIFF_EN
};

const char MSG_INFO_NOZZLE_FAN_EN[] PROGMEM = "Nozzle FAN:";
const char * const MSG_INFO_NOZZLE_FAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INFO_NOZZLE_FAN_EN
};

const char MSG_INFO_PRINT_FAN_EN[] PROGMEM = "Print FAN: ";
const char * const MSG_INFO_PRINT_FAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INFO_PRINT_FAN_EN
};

const char MSG_INIT_SDCARD_EN[] PROGMEM = "Init. SD card";
const char * const MSG_INIT_SDCARD_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INIT_SDCARD_EN
};

const char MSG_INSERT_FILAMENT_EN[] PROGMEM = "Insert filament";
const char * const MSG_INSERT_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INSERT_FILAMENT_EN
};

const char MSG_INVALID_EXTRUDER_EN[] PROGMEM = "Invalid extruder";
const char * const MSG_INVALID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INVALID_EXTRUDER_EN
};

const char MSG_KILLED_EN[] PROGMEM = "KILLED. ";
const char * const MSG_KILLED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_KILLED_EN
};

const char MSG_LANGUAGE_NAME_EN[] PROGMEM = "English";
const char * const MSG_LANGUAGE_NAME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LANGUAGE_NAME_EN
};

const char MSG_LANGUAGE_SELECT_EN[] PROGMEM = "Select language";
const char * const MSG_LANGUAGE_SELECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LANGUAGE_SELECT_EN
};

const char MSG_LOADING_COLOR_EN[] PROGMEM = "Loading color";
const char * const MSG_LOADING_COLOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOADING_COLOR_EN
};

const char MSG_LOADING_FILAMENT_EN[] PROGMEM = "Loading filament";
const char * const MSG_LOADING_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOADING_FILAMENT_EN
};

const char MSG_LOAD_ALL_EN[] PROGMEM = "Load all";
const char * const MSG_LOAD_ALL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_ALL_EN
};

const char MSG_LOAD_EPROM_EN[] PROGMEM = "Load memory";
const char * const MSG_LOAD_EPROM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_EPROM_EN
};

const char MSG_LOAD_FILAMENT_EN[] PROGMEM = "Load filament";
const char * const MSG_LOAD_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_EN
};

const char MSG_LOAD_FILAMENT_1_EN[] PROGMEM = "Load filament 1";
const char * const MSG_LOAD_FILAMENT_1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_1_EN
};

const char MSG_LOAD_FILAMENT_2_EN[] PROGMEM = "Load filament 2";
const char * const MSG_LOAD_FILAMENT_2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_2_EN
};

const char MSG_LOAD_FILAMENT_3_EN[] PROGMEM = "Load filament 3";
const char * const MSG_LOAD_FILAMENT_3_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_3_EN
};

const char MSG_LOAD_FILAMENT_4_EN[] PROGMEM = "Load filament 4";
const char * const MSG_LOAD_FILAMENT_4_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_4_EN
};

const char MSG_LOOSE_PULLEY_EN[] PROGMEM = "Loose pulley";
const char * const MSG_LOOSE_PULLEY_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOOSE_PULLEY_EN
};

const char MSG_M104_INVALID_EXTRUDER_EN[] PROGMEM = "M104 Invalid extruder ";
const char * const MSG_M104_INVALID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M104_INVALID_EXTRUDER_EN
};

const char MSG_M105_INVALID_EXTRUDER_EN[] PROGMEM = "M105 Invalid extruder ";
const char * const MSG_M105_INVALID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M105_INVALID_EXTRUDER_EN
};

const char MSG_M109_INVALID_EXTRUDER_EN[] PROGMEM = "M109 Invalid extruder ";
const char * const MSG_M109_INVALID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M109_INVALID_EXTRUDER_EN
};

const char MSG_M115_REPORT_EN[] PROGMEM = "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:https://github.com/prusa3d/Prusa-i3-Plus/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:" CUSTOM_MENDEL_NAME " EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000\n";
const char * const MSG_M115_REPORT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M115_REPORT_EN
};

const char MSG_M119_REPORT_EN[] PROGMEM = "Reporting endstop status";
const char * const MSG_M119_REPORT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M119_REPORT_EN
};

const char MSG_M200_INVALID_EXTRUDER_EN[] PROGMEM = "M200 Invalid extruder ";
const char * const MSG_M200_INVALID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M200_INVALID_EXTRUDER_EN
};

const char MSG_M218_INVALID_EXTRUDER_EN[] PROGMEM = "M218 Invalid extruder ";
const char * const MSG_M218_INVALID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M218_INVALID_EXTRUDER_EN
};

const char MSG_M221_INVALID_EXTRUDER_EN[] PROGMEM = "M221 Invalid extruder ";
const char * const MSG_M221_INVALID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M221_INVALID_EXTRUDER_EN
};

const char MSG_MAIN_EN[] PROGMEM = "Main";
const char * const MSG_MAIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MAIN_EN
};

const char MSG_MARK_FIL_EN[] PROGMEM = "Mark filament 100mm from extruder body. Click when done.";
const char * const MSG_MARK_FIL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MARK_FIL_EN
};

const char MSG_MAX_EN[] PROGMEM = " \002 Max";
const char * const MSG_MAX_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MAX_EN
};

const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_EN[] PROGMEM = "Measuring reference height of calibration point";
const char * const MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_EN
};

const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_EN[] PROGMEM = " of 9";
const char * const MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_EN
};

const char MSG_MENU_CALIBRATION_EN[] PROGMEM = "Calibration";
const char * const MSG_MENU_CALIBRATION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MENU_CALIBRATION_EN
};

const char MSG_MESH_BED_LEVELING_EN[] PROGMEM = "Mesh Bed Leveling";
const char * const MSG_MESH_BED_LEVELING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MESH_BED_LEVELING_EN
};

const char MSG_MIN_EN[] PROGMEM = " \002 Min";
const char * const MSG_MIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MIN_EN
};

const char MSG_MOTION_EN[] PROGMEM = "Motion";
const char * const MSG_MOTION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOTION_EN
};

const char MSG_MOVE_01MM_EN[] PROGMEM = "Move 0.1mm";
const char * const MSG_MOVE_01MM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_01MM_EN
};

const char MSG_MOVE_10MM_EN[] PROGMEM = "Move 10mm";
const char * const MSG_MOVE_10MM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_10MM_EN
};

const char MSG_MOVE_1MM_EN[] PROGMEM = "Move 1mm";
const char * const MSG_MOVE_1MM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_1MM_EN
};

const char MSG_MOVE_AXIS_EN[] PROGMEM = "Move axis";
const char * const MSG_MOVE_AXIS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_AXIS_EN
};

const char MSG_MOVE_CARRIAGE_TO_THE_TOP_EN[] PROGMEM = "Calibrating XYZ. Rotate the knob to move the Z carriage up to the end stoppers. Click when done.";
const char * const MSG_MOVE_CARRIAGE_TO_THE_TOP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_CARRIAGE_TO_THE_TOP_EN
};

const char MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_EN[] PROGMEM = "Calibrating Z. Rotate the knob to move the Z carriage up to the end stoppers. Click when done.";
const char * const MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_EN
};

const char MSG_MOVE_E_EN[] PROGMEM = "Extruder";
const char * const MSG_MOVE_E_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_E_EN
};

const char MSG_MOVE_X_EN[] PROGMEM = "Move X";
const char * const MSG_MOVE_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_X_EN
};

const char MSG_MOVE_Y_EN[] PROGMEM = "Move Y";
const char * const MSG_MOVE_Y_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_Y_EN
};

const char MSG_MOVE_Z_EN[] PROGMEM = "Move Z";
const char * const MSG_MOVE_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_Z_EN
};

const char MSG_NEW_FIRMWARE_AVAILABLE_EN[] PROGMEM = "New firmware version available:";
const char * const MSG_NEW_FIRMWARE_AVAILABLE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NEW_FIRMWARE_AVAILABLE_EN
};

const char MSG_NEW_FIRMWARE_PLEASE_UPGRADE_EN[] PROGMEM = "Please upgrade.";
const char * const MSG_NEW_FIRMWARE_PLEASE_UPGRADE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NEW_FIRMWARE_PLEASE_UPGRADE_EN
};

const char MSG_NO_EN[] PROGMEM = "No";
const char * const MSG_NO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NO_EN
};

const char MSG_NOT_COLOR_EN[] PROGMEM = "Color not clear";
const char * const MSG_NOT_COLOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOT_COLOR_EN
};

const char MSG_NOT_LOADED_EN[] PROGMEM = "Filament not loaded";
const char * const MSG_NOT_LOADED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOT_LOADED_EN
};

const char MSG_NOZZLE_EN[] PROGMEM = "Nozzle";
const char * const MSG_NOZZLE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOZZLE_EN
};

const char MSG_NOZZLE1_EN[] PROGMEM = "Nozzle2";
const char * const MSG_NOZZLE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOZZLE1_EN
};

const char MSG_NOZZLE2_EN[] PROGMEM = "Nozzle3";
const char * const MSG_NOZZLE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOZZLE2_EN
};

const char MSG_NO_CARD_EN[] PROGMEM = "No SD card";
const char * const MSG_NO_CARD_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NO_CARD_EN
};

const char MSG_NO_MOVE_EN[] PROGMEM = "No move.";
const char * const MSG_NO_MOVE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NO_MOVE_EN
};

const char MSG_OFF_EN[] PROGMEM = "Off";
const char * const MSG_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_OFF_EN
};

const char MSG_OK_EN[] PROGMEM = "ok";
const char * const MSG_OK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_OK_EN
};

const char MSG_ON_EN[] PROGMEM = "On ";
const char * const MSG_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ON_EN
};

const char MSG_PAPER_EN[] PROGMEM = "Place a sheet of paper under the nozzle during the calibration of first 4 points. If the nozzle catches the paper, power off the printer immediately.";
const char * const MSG_PAPER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PAPER_EN
};

const char MSG_PAUSE_PRINT_EN[] PROGMEM = "Pause print";
const char * const MSG_PAUSE_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PAUSE_PRINT_EN
};

const char MSG_PICK_Z_EN[] PROGMEM = "Pick print";
const char * const MSG_PICK_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PICK_Z_EN
};

const char MSG_PID_EXTRUDER_EN[] PROGMEM = "PID calibration";
const char * const MSG_PID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PID_EXTRUDER_EN
};

const char MSG_PID_FINISHED_EN[] PROGMEM = "PID cal. finished";
const char * const MSG_PID_FINISHED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PID_FINISHED_EN
};

const char MSG_PID_RUNNING_EN[] PROGMEM = "PID cal.           ";
const char * const MSG_PID_RUNNING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PID_RUNNING_EN
};

const char MSG_PINDA_NOT_CALIBRATED_EN[] PROGMEM = "Temperature calibration has not been run yet";
const char * const MSG_PINDA_NOT_CALIBRATED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PINDA_NOT_CALIBRATED_EN
};

const char MSG_PINDA_PREHEAT_EN[] PROGMEM = "PINDA Heating";
const char * const MSG_PINDA_PREHEAT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PINDA_PREHEAT_EN
};

const char MSG_PLANNER_BUFFER_BYTES_EN[] PROGMEM = "  PlannerBufferBytes: ";
const char * const MSG_PLANNER_BUFFER_BYTES_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PLANNER_BUFFER_BYTES_EN
};

const char MSG_PLEASE_WAIT_EN[] PROGMEM = "Please wait";
const char * const MSG_PLEASE_WAIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PLEASE_WAIT_EN
};

const char MSG_POSITION_UNKNOWN_EN[] PROGMEM = "Home X/Y before Z";
const char * const MSG_POSITION_UNKNOWN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_POSITION_UNKNOWN_EN
};

const char MSG_POWERUP_EN[] PROGMEM = "PowerUp";
const char * const MSG_POWERUP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_POWERUP_EN
};

const char MSG_PREHEAT_EN[] PROGMEM = "Preheat";
const char * const MSG_PREHEAT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_EN
};

const char MSG_PREHEAT_NOZZLE_EN[] PROGMEM = "Preheat the nozzle!";
const char * const MSG_PREHEAT_NOZZLE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_NOZZLE_EN
};

const char MSG_PREPARE_FILAMENT_EN[] PROGMEM = "Prepare new filament";
const char * const MSG_PREPARE_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREPARE_FILAMENT_EN
};

const char MSG_PRESS_EN[] PROGMEM = "and press the knob";
const char * const MSG_PRESS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRESS_EN
};

const char MSG_PRINTER_DISCONNECTED_EN[] PROGMEM = "Printer disconnected";
const char * const MSG_PRINTER_DISCONNECTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRINTER_DISCONNECTED_EN
};

const char MSG_PRINT_ABORTED_EN[] PROGMEM = "Print aborted";
const char * const MSG_PRINT_ABORTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRINT_ABORTED_EN
};

const char MSG_PRINT_PAUSED_EN[] PROGMEM = "Print paused";
const char * const MSG_PRINT_PAUSED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRINT_PAUSED_EN
};

const char MSG_PRUSA3D_EN[] PROGMEM = "prusa3d.com";
const char * const MSG_PRUSA3D_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRUSA3D_EN
};

const char MSG_PRUSA3D_FORUM_EN[] PROGMEM = "forum.prusa3d.com";
const char * const MSG_PRUSA3D_FORUM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRUSA3D_FORUM_EN
};

const char MSG_PRUSA3D_HOWTO_EN[] PROGMEM = "howto.prusa3d.com";
const char * const MSG_PRUSA3D_HOWTO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRUSA3D_HOWTO_EN
};

const char MSG_REBOOT_EN[] PROGMEM = "Reboot the printer";
const char * const MSG_REBOOT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_REBOOT_EN
};

const char MSG_RECOVERING_PRINT_EN[] PROGMEM = "Recovering print    ";
const char * const MSG_RECOVERING_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RECOVERING_PRINT_EN
};

const char MSG_RECOVER_PRINT_EN[] PROGMEM = "Blackout occurred. Recover print?";
const char * const MSG_RECOVER_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RECOVER_PRINT_EN
};

const char MSG_RECTRACT_EN[] PROGMEM = "Rectract";
const char * const MSG_RECTRACT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RECTRACT_EN
};

const char MSG_REFRESH_EN[] PROGMEM = "\xF8" "Refresh";
const char * const MSG_REFRESH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_REFRESH_EN
};

const char MSG_RESEND_EN[] PROGMEM = "Resend: ";
const char * const MSG_RESEND_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESEND_EN
};

const char MSG_RESET_CALIBRATE_E_EN[] PROGMEM = "Reset E Cal.";
const char * const MSG_RESET_CALIBRATE_E_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESET_CALIBRATE_E_EN
};

const char MSG_RESTORE_FAILSAFE_EN[] PROGMEM = "Restore failsafe";
const char * const MSG_RESTORE_FAILSAFE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESTORE_FAILSAFE_EN
};

const char MSG_RESUME_PRINT_EN[] PROGMEM = "Resume print";
const char * const MSG_RESUME_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESUME_PRINT_EN
};

const char MSG_RESUMING_EN[] PROGMEM = "Resuming print";
const char * const MSG_RESUMING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESUMING_EN
};

const char MSG_RESUMING_PRINT_EN[] PROGMEM = "Resuming print";
const char * const MSG_RESUMING_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESUMING_PRINT_EN
};

const char MSG_SD_CANT_ENTER_SUBDIR_EN[] PROGMEM = "Cannot enter subdir: ";
const char * const MSG_SD_CANT_ENTER_SUBDIR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_CANT_ENTER_SUBDIR_EN
};

const char MSG_SD_CANT_OPEN_SUBDIR_EN[] PROGMEM = "Cannot open subdir";
const char * const MSG_SD_CANT_OPEN_SUBDIR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_CANT_OPEN_SUBDIR_EN
};

const char MSG_SD_CARD_OK_EN[] PROGMEM = "SD card ok";
const char * const MSG_SD_CARD_OK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_CARD_OK_EN
};

const char MSG_SD_ERR_WRITE_TO_FILE_EN[] PROGMEM = "error writing to file";
const char * const MSG_SD_ERR_WRITE_TO_FILE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_ERR_WRITE_TO_FILE_EN
};

const char MSG_SD_FILE_OPENED_EN[] PROGMEM = "File opened: ";
const char * const MSG_SD_FILE_OPENED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_FILE_OPENED_EN
};

const char MSG_SD_FILE_SELECTED_EN[] PROGMEM = "File selected";
const char * const MSG_SD_FILE_SELECTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_FILE_SELECTED_EN
};

const char MSG_SD_INIT_FAIL_EN[] PROGMEM = "SD init fail";
const char * const MSG_SD_INIT_FAIL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_INIT_FAIL_EN
};

const char MSG_SD_INSERTED_EN[] PROGMEM = "Card inserted";
const char * const MSG_SD_INSERTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_INSERTED_EN
};

const char MSG_SD_NOT_PRINTING_EN[] PROGMEM = "Not SD printing";
const char * const MSG_SD_NOT_PRINTING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_NOT_PRINTING_EN
};

const char MSG_SD_OPENROOT_FAIL_EN[] PROGMEM = "openRoot failed";
const char * const MSG_SD_OPENROOT_FAIL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_OPENROOT_FAIL_EN
};

const char MSG_SD_OPEN_FILE_FAIL_EN[] PROGMEM = "open failed, File: ";
const char * const MSG_SD_OPEN_FILE_FAIL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_OPEN_FILE_FAIL_EN
};

const char MSG_SD_PRINTING_BYTE_EN[] PROGMEM = "SD printing byte ";
const char * const MSG_SD_PRINTING_BYTE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_PRINTING_BYTE_EN
};

const char MSG_SD_REMOVED_EN[] PROGMEM = "Card removed";
const char * const MSG_SD_REMOVED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_REMOVED_EN
};

const char MSG_SD_SIZE_EN[] PROGMEM = " Size: ";
const char * const MSG_SD_SIZE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_SIZE_EN
};

const char MSG_SD_VOL_INIT_FAIL_EN[] PROGMEM = "volume.init failed";
const char * const MSG_SD_VOL_INIT_FAIL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_VOL_INIT_FAIL_EN
};

const char MSG_SD_WORKDIR_FAIL_EN[] PROGMEM = "workDir open failed";
const char * const MSG_SD_WORKDIR_FAIL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_WORKDIR_FAIL_EN
};

const char MSG_SD_WRITE_TO_FILE_EN[] PROGMEM = "Writing to file: ";
const char * const MSG_SD_WRITE_TO_FILE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_WRITE_TO_FILE_EN
};

const char MSG_SELFTEST_EN[] PROGMEM = "Selftest         ";
const char * const MSG_SELFTEST_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_EN
};

const char MSG_SELFTEST_AXIS_EN[] PROGMEM = "Axis";
const char * const MSG_SELFTEST_AXIS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_AXIS_EN
};

const char MSG_SELFTEST_AXIS_LENGTH_EN[] PROGMEM = "Axis length";
const char * const MSG_SELFTEST_AXIS_LENGTH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_AXIS_LENGTH_EN
};

const char MSG_SELFTEST_BEDHEATER_EN[] PROGMEM = "Bed / Heater";
const char * const MSG_SELFTEST_BEDHEATER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_BEDHEATER_EN
};

const char MSG_SELFTEST_CHECK_ALLCORRECT_EN[] PROGMEM = "All correct      ";
const char * const MSG_SELFTEST_CHECK_ALLCORRECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_ALLCORRECT_EN
};

const char MSG_SELFTEST_CHECK_BED_EN[] PROGMEM = "Checking bed     ";
const char * const MSG_SELFTEST_CHECK_BED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_BED_EN
};

const char MSG_SELFTEST_CHECK_ENDSTOPS_EN[] PROGMEM = "Checking endstops";
const char * const MSG_SELFTEST_CHECK_ENDSTOPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_ENDSTOPS_EN
};

const char MSG_SELFTEST_CHECK_HOTEND_EN[] PROGMEM = "Checking hotend  ";
const char * const MSG_SELFTEST_CHECK_HOTEND_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_HOTEND_EN
};

const char MSG_SELFTEST_CHECK_X_EN[] PROGMEM = "Checking X axis  ";
const char * const MSG_SELFTEST_CHECK_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_X_EN
};

const char MSG_SELFTEST_CHECK_Y_EN[] PROGMEM = "Checking Y axis  ";
const char * const MSG_SELFTEST_CHECK_Y_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_Y_EN
};

const char MSG_SELFTEST_CHECK_Z_EN[] PROGMEM = "Checking Z axis  ";
const char * const MSG_SELFTEST_CHECK_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_Z_EN
};

const char MSG_SELFTEST_COOLING_FAN_EN[] PROGMEM = "Front print fan?";;
const char * const MSG_SELFTEST_COOLING_FAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_COOLING_FAN_EN
};

const char MSG_SELFTEST_ENDSTOP_EN[] PROGMEM = "Endstop";
const char * const MSG_SELFTEST_ENDSTOP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_ENDSTOP_EN
};

const char MSG_SELFTEST_ENDSTOPS_EN[] PROGMEM = "Endstops";
const char * const MSG_SELFTEST_ENDSTOPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_ENDSTOPS_EN
};

const char MSG_SELFTEST_ENDSTOP_NOTHIT_EN[] PROGMEM = "Endstop not hit";
const char * const MSG_SELFTEST_ENDSTOP_NOTHIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_ENDSTOP_NOTHIT_EN
};

const char MSG_SELFTEST_ERROR_EN[] PROGMEM = "Selftest error !";
const char * const MSG_SELFTEST_ERROR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_ERROR_EN
};

const char MSG_SELFTEST_EXTRUDER_FAN_EN[] PROGMEM = "Left hotend fan?";;
const char * const MSG_SELFTEST_EXTRUDER_FAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_EXTRUDER_FAN_EN
};

const char MSG_SELFTEST_FAILED_EN[] PROGMEM = "Selftest failed  ";
const char * const MSG_SELFTEST_FAILED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FAILED_EN
};

const char MSG_SELFTEST_FAN_EN[] PROGMEM = "Fan test";;
const char * const MSG_SELFTEST_FAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FAN_EN
};

const char MSG_SELFTEST_FAN_NO_EN[] PROGMEM = "Not spinning";;
const char * const MSG_SELFTEST_FAN_NO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FAN_NO_EN
};

const char MSG_SELFTEST_FAN_YES_EN[] PROGMEM = "Spinning";;
const char * const MSG_SELFTEST_FAN_YES_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FAN_YES_EN
};

const char MSG_SELFTEST_HEATERTHERMISTOR_EN[] PROGMEM = "Heater/Thermistor";
const char * const MSG_SELFTEST_HEATERTHERMISTOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_HEATERTHERMISTOR_EN
};

const char MSG_SELFTEST_MOTOR_EN[] PROGMEM = "Motor";
const char * const MSG_SELFTEST_MOTOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_MOTOR_EN
};

const char MSG_SELFTEST_NOTCONNECTED_EN[] PROGMEM = "Not connected";
const char * const MSG_SELFTEST_NOTCONNECTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_NOTCONNECTED_EN
};

const char MSG_SELFTEST_OK_EN[] PROGMEM = "Self test OK";
const char * const MSG_SELFTEST_OK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_OK_EN
};

const char MSG_SELFTEST_PLEASECHECK_EN[] PROGMEM = "Please check :";
const char * const MSG_SELFTEST_PLEASECHECK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_PLEASECHECK_EN
};

const char MSG_SELFTEST_START_EN[] PROGMEM = "Self test start  ";
const char * const MSG_SELFTEST_START_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_START_EN
};

const char MSG_SELFTEST_WIRINGERROR_EN[] PROGMEM = "Wiring error";
const char * const MSG_SELFTEST_WIRINGERROR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_WIRINGERROR_EN
};

const char MSG_SERIAL_ERROR_MENU_STRUCTURE_EN[] PROGMEM = "Error in menu structure";
const char * const MSG_SERIAL_ERROR_MENU_STRUCTURE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SERIAL_ERROR_MENU_STRUCTURE_EN
};

const char MSG_SETTINGS_EN[] PROGMEM = "Settings";
const char * const MSG_SETTINGS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SETTINGS_EN
};

const char MSG_SET_HOME_OFFSETS_EN[] PROGMEM = "Set home offsets";
const char * const MSG_SET_HOME_OFFSETS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SET_HOME_OFFSETS_EN
};

const char MSG_SET_ORIGIN_EN[] PROGMEM = "Set origin";
const char * const MSG_SET_ORIGIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SET_ORIGIN_EN
};

const char MSG_SET_TEMPERATURE_EN[] PROGMEM = "Set temperature:";
const char * const MSG_SET_TEMPERATURE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SET_TEMPERATURE_EN
};

const char MSG_SHOW_END_STOPS_EN[] PROGMEM = "Show end stops";
const char * const MSG_SHOW_END_STOPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SHOW_END_STOPS_EN
};

const char MSG_SILENT_MODE_OFF_EN[] PROGMEM = "Mode [high power]";
const char * const MSG_SILENT_MODE_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SILENT_MODE_OFF_EN
};

const char MSG_SILENT_MODE_ON_EN[] PROGMEM = "Mode     [silent]";
const char * const MSG_SILENT_MODE_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SILENT_MODE_ON_EN
};

const char MSG_SOFTWARE_RESET_EN[] PROGMEM = " Software Reset";
const char * const MSG_SOFTWARE_RESET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SOFTWARE_RESET_EN
};

const char MSG_SPEED_EN[] PROGMEM = "Speed";
const char * const MSG_SPEED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SPEED_EN
};

const char MSG_STACK_ERROR_EN[] PROGMEM = "Error - static memory has been overwritten";
const char * const MSG_STACK_ERROR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STACK_ERROR_EN
};

const char MSG_STATISTICS_EN[] PROGMEM = "Statistics  ";
const char * const MSG_STATISTICS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATISTICS_EN
};

const char MSG_STATS_FILAMENTUSED_EN[] PROGMEM = "Filament used:  ";
const char * const MSG_STATS_FILAMENTUSED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_FILAMENTUSED_EN
};

const char MSG_STATS_PRINTTIME_EN[] PROGMEM = "Print time:  ";
const char * const MSG_STATS_PRINTTIME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_PRINTTIME_EN
};

const char MSG_STATS_TOTALFILAMENT_EN[] PROGMEM = "Total filament :";
const char * const MSG_STATS_TOTALFILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_TOTALFILAMENT_EN
};

const char MSG_STATS_TOTALPRINTTIME_EN[] PROGMEM = "Total print time :";
const char * const MSG_STATS_TOTALPRINTTIME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_TOTALPRINTTIME_EN
};

const char MSG_STEPPER_TOO_HIGH_EN[] PROGMEM = "Steprate too high: ";
const char * const MSG_STEPPER_TOO_HIGH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STEPPER_TOO_HIGH_EN
};

const char MSG_STOPPED_EN[] PROGMEM = "STOPPED. ";
const char * const MSG_STOPPED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STOPPED_EN
};

const char MSG_STOP_PRINT_EN[] PROGMEM = "Stop print";
const char * const MSG_STOP_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STOP_PRINT_EN
};

const char MSG_STORE_EPROM_EN[] PROGMEM = "Store memory";
const char * const MSG_STORE_EPROM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STORE_EPROM_EN
};

const char MSG_SUPPORT_EN[] PROGMEM = "Support";
const char * const MSG_SUPPORT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SUPPORT_EN
};

const char MSG_SWITCH_PS_OFF_EN[] PROGMEM = "Switch power off";
const char * const MSG_SWITCH_PS_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SWITCH_PS_OFF_EN
};

const char MSG_SWITCH_PS_ON_EN[] PROGMEM = "Switch power on";
const char * const MSG_SWITCH_PS_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SWITCH_PS_ON_EN
};

const char MSG_TAKE_EFFECT_EN[] PROGMEM = " for take effect";
const char * const MSG_TAKE_EFFECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TAKE_EFFECT_EN
};

const char MSG_TEMPERATURE_EN[] PROGMEM = "Temperature";
const char * const MSG_TEMPERATURE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMPERATURE_EN
};

const char MSG_TEMP_CALIBRATION_EN[] PROGMEM = "Temp. cal.          ";
const char * const MSG_TEMP_CALIBRATION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMP_CALIBRATION_EN
};

const char MSG_TEMP_CALIBRATION_DONE_EN[] PROGMEM = "Temperature calibration is finished. Click to continue.";
const char * const MSG_TEMP_CALIBRATION_DONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMP_CALIBRATION_DONE_EN
};

const char MSG_TEMP_CALIBRATION_OFF_EN[] PROGMEM = "Temp. cal. [OFF]";
const char * const MSG_TEMP_CALIBRATION_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMP_CALIBRATION_OFF_EN
};

const char MSG_TEMP_CALIBRATION_ON_EN[] PROGMEM = "Temp. cal. [ON]";
const char * const MSG_TEMP_CALIBRATION_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMP_CALIBRATION_ON_EN
};

const char MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_EN[] PROGMEM = "SD card [normal]";
const char * const MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_EN
};

const char MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_EN[] PROGMEM = "SD card [FlshAir]";
const char * const MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_EN
};

const char MSG_TUNE_EN[] PROGMEM = "Tune";
const char * const MSG_TUNE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TUNE_EN
};

const char MSG_UNKNOWN_COMMAND_EN[] PROGMEM = "Unknown command: \"";
const char * const MSG_UNKNOWN_COMMAND_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNKNOWN_COMMAND_EN
};

const char MSG_UNLOADING_FILAMENT_EN[] PROGMEM = "Unloading filament";
const char * const MSG_UNLOADING_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOADING_FILAMENT_EN
};

const char MSG_UNLOAD_ALL_EN[] PROGMEM = "Unload all";
const char * const MSG_UNLOAD_ALL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_ALL_EN
};

const char MSG_UNLOAD_FILAMENT_EN[] PROGMEM = "Unload filament";
const char * const MSG_UNLOAD_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_EN
};

const char MSG_UNLOAD_FILAMENT_1_EN[] PROGMEM = "Unload filament 1";
const char * const MSG_UNLOAD_FILAMENT_1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_1_EN
};

const char MSG_UNLOAD_FILAMENT_2_EN[] PROGMEM = "Unload filament 2";
const char * const MSG_UNLOAD_FILAMENT_2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_2_EN
};

const char MSG_UNLOAD_FILAMENT_3_EN[] PROGMEM = "Unload filament 3";
const char * const MSG_UNLOAD_FILAMENT_3_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_3_EN
};

const char MSG_UNLOAD_FILAMENT_4_EN[] PROGMEM = "Unload filament 4";
const char * const MSG_UNLOAD_FILAMENT_4_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_4_EN
};

const char MSG_USB_PRINTING_EN[] PROGMEM = "USB printing  ";
const char * const MSG_USB_PRINTING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_USB_PRINTING_EN
};

const char MSG_USED_EN[] PROGMEM = "Used during print";
const char * const MSG_USED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_USED_EN
};

const char MSG_USERWAIT_EN[] PROGMEM = "Wait for user...";
const char * const MSG_USERWAIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_USERWAIT_EN
};

const char MSG_VMIN_EN[] PROGMEM = "Vmin";
const char * const MSG_VMIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_VMIN_EN
};

const char MSG_VOLUMETRIC_EN[] PROGMEM = "Filament";
const char * const MSG_VOLUMETRIC_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_VOLUMETRIC_EN
};

const char MSG_VOLUMETRIC_ENABLED_EN[] PROGMEM = "E in mm3";
const char * const MSG_VOLUMETRIC_ENABLED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_VOLUMETRIC_ENABLED_EN
};

const char MSG_VTRAV_MIN_EN[] PROGMEM = "VTrav min";
const char * const MSG_VTRAV_MIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_VTRAV_MIN_EN
};

const char MSG_WAITING_TEMP_EN[] PROGMEM = "Waiting for nozzle and bed cooling";
const char * const MSG_WAITING_TEMP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WAITING_TEMP_EN
};

const char MSG_WATCH_EN[] PROGMEM = "Info screen";
const char * const MSG_WATCH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WATCH_EN
};

const char MSG_WATCHDOG_RESET_EN[] PROGMEM = " Watchdog Reset";
const char * const MSG_WATCHDOG_RESET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WATCHDOG_RESET_EN
};

const char MSG_X_MAX_EN[] PROGMEM = "x_max: ";
const char * const MSG_X_MAX_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_X_MAX_EN
};

const char MSG_X_MIN_EN[] PROGMEM = "x_min: ";
const char * const MSG_X_MIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_X_MIN_EN
};

const char MSG_YES_EN[] PROGMEM = "Yes";
const char * const MSG_YES_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_YES_EN
};

const char MSG_Y_MAX_EN[] PROGMEM = "y_max: ";
const char * const MSG_Y_MAX_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_Y_MAX_EN
};

const char MSG_Y_MIN_EN[] PROGMEM = "y_min: ";
const char * const MSG_Y_MIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_Y_MIN_EN
};

const char MSG_ZPROBE_OUT_EN[] PROGMEM = "Z probe out. bed";
const char * const MSG_ZPROBE_OUT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ZPROBE_OUT_EN
};

const char MSG_ZPROBE_ZOFFSET_EN[] PROGMEM = "Z Offset";
const char * const MSG_ZPROBE_ZOFFSET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ZPROBE_ZOFFSET_EN
};

const char MSG_Z_MAX_EN[] PROGMEM = "z_max: ";
const char * const MSG_Z_MAX_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_Z_MAX_EN
};

const char MSG_Z_MIN_EN[] PROGMEM = "z_min: ";
const char * const MSG_Z_MIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_Z_MIN_EN
};

const char WELCOME_MSG_EN[] PROGMEM = CUSTOM_MENDEL_NAME " ready.";
const char * const WELCOME_MSG_LANG_TABLE[LANG_NUM] PROGMEM = {
	WELCOME_MSG_EN
};


char langbuffer[LCD_WIDTH+1];
char* CAT2(const char *s1,const char *s2) {
  unsigned char len=0;
  strncpy_P(langbuffer+len,s1,LCD_WIDTH-len);
  len+=strlen_P(s1);
  strncpy_P(langbuffer+len,s2,LCD_WIDTH-len);
  return langbuffer;
}
char* CAT4(const char *s1,const char *s2,const char *s3,const char *s4) {
  unsigned char len=0;
  strncpy_P(langbuffer+len,s1,LCD_WIDTH-len);
  len+=strlen_P(s1);
  strncpy_P(langbuffer+len,s2,LCD_WIDTH-len);
  len+=strlen_P(s2);
  strncpy_P(langbuffer+len,s3,LCD_WIDTH-len);
  len+=strlen_P(s3);
  strncpy_P(langbuffer+len,s4,LCD_WIDTH-len);
  return langbuffer;
}
