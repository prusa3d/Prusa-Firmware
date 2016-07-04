#include <avr/pgmspace.h>
#include "configuration_prusa.h"
#include "language_all.h"

#define LCD_WIDTH 20
extern unsigned char lang_selected;

const char MSG_ACC_EN[] PROGMEM = "Accel";
const char MSG_ACC_CZ[] PROGMEM = "Accel";
const char MSG_ACC_IT[] PROGMEM = "Accel";
const char MSG_ACC_ES[] PROGMEM = "Accel";
const char MSG_ACC_PL[] PROGMEM = "Accel";
const char * const MSG_ACC_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ACC_EN,
	MSG_ACC_CZ,
	MSG_ACC_IT,
	MSG_ACC_ES,
	MSG_ACC_PL
};

const char MSG_ACTIVE_EXTRUDER_EN[] PROGMEM = "Active Extruder: ";
const char MSG_ACTIVE_EXTRUDER_CZ[] PROGMEM = "Active Extruder: ";
const char MSG_ACTIVE_EXTRUDER_IT[] PROGMEM = "Active Extruder: ";
const char MSG_ACTIVE_EXTRUDER_ES[] PROGMEM = "Active Extruder: ";
const char MSG_ACTIVE_EXTRUDER_PL[] PROGMEM = "Active Extruder: ";
const char * const MSG_ACTIVE_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ACTIVE_EXTRUDER_EN,
	MSG_ACTIVE_EXTRUDER_CZ,
	MSG_ACTIVE_EXTRUDER_IT,
	MSG_ACTIVE_EXTRUDER_ES,
	MSG_ACTIVE_EXTRUDER_PL
};

const char MSG_ADJUSTZ_EN[] PROGMEM = "Auto adjust Z ?";
const char MSG_ADJUSTZ_CZ[] PROGMEM = "Auto doladit Z ?";
const char MSG_ADJUSTZ_IT[] PROGMEM = "Auto regolare Z ?";
const char MSG_ADJUSTZ_ES[] PROGMEM = "Auto Micropaso Z?";
const char MSG_ADJUSTZ_PL[] PROGMEM = "Autodostroic Z?";
const char * const MSG_ADJUSTZ_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ADJUSTZ_EN,
	MSG_ADJUSTZ_CZ,
	MSG_ADJUSTZ_IT,
	MSG_ADJUSTZ_ES,
	MSG_ADJUSTZ_PL
};

const char MSG_AMAX_EN[] PROGMEM = "Amax ";
const char MSG_AMAX_CZ[] PROGMEM = "Amax ";
const char MSG_AMAX_IT[] PROGMEM = "Amax ";
const char MSG_AMAX_ES[] PROGMEM = "Amax ";
const char MSG_AMAX_PL[] PROGMEM = "Amax ";
const char * const MSG_AMAX_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AMAX_EN,
	MSG_AMAX_CZ,
	MSG_AMAX_IT,
	MSG_AMAX_ES,
	MSG_AMAX_PL
};

const char MSG_AUTHOR_EN[] PROGMEM = " | Author: ";
const char MSG_AUTHOR_CZ[] PROGMEM = " | Author: ";
const char MSG_AUTHOR_IT[] PROGMEM = " | Author: ";
const char MSG_AUTHOR_ES[] PROGMEM = " | Author: ";
const char MSG_AUTHOR_PL[] PROGMEM = " | Author: ";
const char * const MSG_AUTHOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AUTHOR_EN,
	MSG_AUTHOR_CZ,
	MSG_AUTHOR_IT,
	MSG_AUTHOR_ES,
	MSG_AUTHOR_PL
};

const char MSG_AUTORETRACT_EN[] PROGMEM = "AutoRetr.";
const char MSG_AUTORETRACT_CZ[] PROGMEM = "AutoRetr.";
const char MSG_AUTORETRACT_IT[] PROGMEM = "AutoRetr.";
const char MSG_AUTORETRACT_ES[] PROGMEM = "AutoRetr.";
const char MSG_AUTORETRACT_PL[] PROGMEM = "AutoRetr.";
const char * const MSG_AUTORETRACT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AUTORETRACT_EN,
	MSG_AUTORETRACT_CZ,
	MSG_AUTORETRACT_IT,
	MSG_AUTORETRACT_ES,
	MSG_AUTORETRACT_PL
};

const char MSG_AUTOSTART_EN[] PROGMEM = "Autostart";
const char MSG_AUTOSTART_CZ[] PROGMEM = "Autostart";
const char MSG_AUTOSTART_IT[] PROGMEM = "Autostart";
const char MSG_AUTOSTART_ES[] PROGMEM = "Autostart";
const char MSG_AUTOSTART_PL[] PROGMEM = "Autostart";
const char * const MSG_AUTOSTART_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AUTOSTART_EN,
	MSG_AUTOSTART_CZ,
	MSG_AUTOSTART_IT,
	MSG_AUTOSTART_ES,
	MSG_AUTOSTART_PL
};

const char MSG_AUTOTEMP_EN[] PROGMEM = "Autotemp";
const char MSG_AUTOTEMP_CZ[] PROGMEM = "Autotemp";
const char MSG_AUTOTEMP_IT[] PROGMEM = "Autotemp";
const char MSG_AUTOTEMP_ES[] PROGMEM = "Autotemp";
const char MSG_AUTOTEMP_PL[] PROGMEM = "Autotemp";
const char * const MSG_AUTOTEMP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AUTOTEMP_EN,
	MSG_AUTOTEMP_CZ,
	MSG_AUTOTEMP_IT,
	MSG_AUTOTEMP_ES,
	MSG_AUTOTEMP_PL
};

const char MSG_AUTO_HOME_EN[] PROGMEM = "Auto home";
const char MSG_AUTO_HOME_CZ[] PROGMEM = "Auto home";
const char MSG_AUTO_HOME_IT[] PROGMEM = "Auto Home";
const char MSG_AUTO_HOME_ES[] PROGMEM = "Llevar al origen";
const char MSG_AUTO_HOME_PL[] PROGMEM = "Auto home";
const char * const MSG_AUTO_HOME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AUTO_HOME_EN,
	MSG_AUTO_HOME_CZ,
	MSG_AUTO_HOME_IT,
	MSG_AUTO_HOME_ES,
	MSG_AUTO_HOME_PL
};

const char MSG_A_RETRACT_EN[] PROGMEM = "A-retract";
const char MSG_A_RETRACT_CZ[] PROGMEM = "A-retract";
const char MSG_A_RETRACT_IT[] PROGMEM = "A-retract";
const char MSG_A_RETRACT_ES[] PROGMEM = "A-retract";
const char MSG_A_RETRACT_PL[] PROGMEM = "A-retract";
const char * const MSG_A_RETRACT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_A_RETRACT_EN,
	MSG_A_RETRACT_CZ,
	MSG_A_RETRACT_IT,
	MSG_A_RETRACT_ES,
	MSG_A_RETRACT_PL
};

const char MSG_BABYSTEPPING_X_EN[] PROGMEM = "Babystepping X";
const char MSG_BABYSTEPPING_X_CZ[] PROGMEM = "Babystepping X";
const char MSG_BABYSTEPPING_X_IT[] PROGMEM = "Babystepping X";
const char MSG_BABYSTEPPING_X_ES[] PROGMEM = "Babystepping X";
const char MSG_BABYSTEPPING_X_PL[] PROGMEM = "Babystepping X";
const char * const MSG_BABYSTEPPING_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEPPING_X_EN,
	MSG_BABYSTEPPING_X_CZ,
	MSG_BABYSTEPPING_X_IT,
	MSG_BABYSTEPPING_X_ES,
	MSG_BABYSTEPPING_X_PL
};

const char MSG_BABYSTEPPING_Y_EN[] PROGMEM = "Babystepping Y";
const char MSG_BABYSTEPPING_Y_CZ[] PROGMEM = "Babystepping Y";
const char MSG_BABYSTEPPING_Y_IT[] PROGMEM = "Babystepping Y";
const char MSG_BABYSTEPPING_Y_ES[] PROGMEM = "Babystepping Y";
const char MSG_BABYSTEPPING_Y_PL[] PROGMEM = "Babystepping Y";
const char * const MSG_BABYSTEPPING_Y_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEPPING_Y_EN,
	MSG_BABYSTEPPING_Y_CZ,
	MSG_BABYSTEPPING_Y_IT,
	MSG_BABYSTEPPING_Y_ES,
	MSG_BABYSTEPPING_Y_PL
};

const char MSG_BABYSTEPPING_Z_EN[] PROGMEM = "Adjusting Z";
const char MSG_BABYSTEPPING_Z_CZ[] PROGMEM = "Dostavovani Z";
const char MSG_BABYSTEPPING_Z_IT[] PROGMEM = "Adjusting Z";
const char MSG_BABYSTEPPING_Z_ES[] PROGMEM = "Adjusting Z";
const char MSG_BABYSTEPPING_Z_PL[] PROGMEM = "Dostavovani Z";
const char * const MSG_BABYSTEPPING_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEPPING_Z_EN,
	MSG_BABYSTEPPING_Z_CZ,
	MSG_BABYSTEPPING_Z_IT,
	MSG_BABYSTEPPING_Z_ES,
	MSG_BABYSTEPPING_Z_PL
};

const char MSG_BABYSTEP_X_EN[] PROGMEM = "Babystep X";
const char MSG_BABYSTEP_X_CZ[] PROGMEM = "Babystep X";
const char MSG_BABYSTEP_X_IT[] PROGMEM = "Babystep X";
const char MSG_BABYSTEP_X_ES[] PROGMEM = "Babystep X";
const char MSG_BABYSTEP_X_PL[] PROGMEM = "Babystep X";
const char * const MSG_BABYSTEP_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEP_X_EN,
	MSG_BABYSTEP_X_CZ,
	MSG_BABYSTEP_X_IT,
	MSG_BABYSTEP_X_ES,
	MSG_BABYSTEP_X_PL
};

const char MSG_BABYSTEP_Y_EN[] PROGMEM = "Babystep Y";
const char MSG_BABYSTEP_Y_CZ[] PROGMEM = "Babystep Y";
const char MSG_BABYSTEP_Y_IT[] PROGMEM = "Babystep Y";
const char MSG_BABYSTEP_Y_ES[] PROGMEM = "Babystep Y";
const char MSG_BABYSTEP_Y_PL[] PROGMEM = "Babystep Y";
const char * const MSG_BABYSTEP_Y_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEP_Y_EN,
	MSG_BABYSTEP_Y_CZ,
	MSG_BABYSTEP_Y_IT,
	MSG_BABYSTEP_Y_ES,
	MSG_BABYSTEP_Y_PL
};

const char MSG_BABYSTEP_Z_EN[] PROGMEM = "Live adjust Z";
const char MSG_BABYSTEP_Z_CZ[] PROGMEM = "Doladeni osy Z";
const char MSG_BABYSTEP_Z_IT[] PROGMEM = "Babystep Z";
const char MSG_BABYSTEP_Z_ES[] PROGMEM = "Micropaso Z";
const char MSG_BABYSTEP_Z_PL[] PROGMEM = "Dostrojenie osy Z";
const char * const MSG_BABYSTEP_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEP_Z_EN,
	MSG_BABYSTEP_Z_CZ,
	MSG_BABYSTEP_Z_IT,
	MSG_BABYSTEP_Z_ES,
	MSG_BABYSTEP_Z_PL
};

const char MSG_BED_EN[] PROGMEM = "Bed";
const char MSG_BED_CZ[] PROGMEM = "Bed";
const char MSG_BED_IT[] PROGMEM = "Piatto";
const char MSG_BED_ES[] PROGMEM = "Base";
const char MSG_BED_PL[] PROGMEM = "Stolik";
const char * const MSG_BED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_EN,
	MSG_BED_CZ,
	MSG_BED_IT,
	MSG_BED_ES,
	MSG_BED_PL
};

const char MSG_BED_DONE_EN[] PROGMEM = "Bed done";
const char MSG_BED_DONE_CZ[] PROGMEM = "Bed OK.";
const char MSG_BED_DONE_IT[] PROGMEM = "Piatto fatto.";
const char MSG_BED_DONE_ES[] PROGMEM = "Base listo.";
const char MSG_BED_DONE_PL[] PROGMEM = "Stolik OK.";
const char * const MSG_BED_DONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_DONE_EN,
	MSG_BED_DONE_CZ,
	MSG_BED_DONE_IT,
	MSG_BED_DONE_ES,
	MSG_BED_DONE_PL
};

const char MSG_BED_HEATING_EN[] PROGMEM = "Bed Heating";
const char MSG_BED_HEATING_CZ[] PROGMEM = "Zahrivani bed";
const char MSG_BED_HEATING_IT[] PROGMEM = "Piatto riscaldam.";
const char MSG_BED_HEATING_ES[] PROGMEM = "Base Calentando";
const char MSG_BED_HEATING_PL[] PROGMEM = "Grzanie stolika..";
const char * const MSG_BED_HEATING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_HEATING_EN,
	MSG_BED_HEATING_CZ,
	MSG_BED_HEATING_IT,
	MSG_BED_HEATING_ES,
	MSG_BED_HEATING_PL
};

const char MSG_BED_LEVELING_FAILED_POINT_HIGH_EN[] PROGMEM = "Bed leveling failed. Sensor triggered too high. Waiting for reset.";
const char MSG_BED_LEVELING_FAILED_POINT_HIGH_CZ[] PROGMEM = "Kalibrace Z selhala. Sensor sepnul prilis vysoko. Cekam na reset.";
const char MSG_BED_LEVELING_FAILED_POINT_HIGH_IT[] PROGMEM = "Bed leveling failed. Sensor triggered too high. Waiting for reset.";
const char MSG_BED_LEVELING_FAILED_POINT_HIGH_ES[] PROGMEM = "Bed leveling failed. Sensor triggered too high. Waiting for reset.";
const char MSG_BED_LEVELING_FAILED_POINT_HIGH_PL[] PROGMEM = "Bed leveling failed. Sensor triggered too high. Waiting for reset.";
const char * const MSG_BED_LEVELING_FAILED_POINT_HIGH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_LEVELING_FAILED_POINT_HIGH_EN,
	MSG_BED_LEVELING_FAILED_POINT_HIGH_CZ,
	MSG_BED_LEVELING_FAILED_POINT_HIGH_IT,
	MSG_BED_LEVELING_FAILED_POINT_HIGH_ES,
	MSG_BED_LEVELING_FAILED_POINT_HIGH_PL
};

const char MSG_BED_LEVELING_FAILED_POINT_LOW_EN[] PROGMEM = "Bed leveling failed. Sensor didnt trigger. Debris on nozzle? Waiting for reset.";
const char MSG_BED_LEVELING_FAILED_POINT_LOW_CZ[] PROGMEM = "Kalibrace Z selhala. Sensor nesepnul. Znecistena tryska? Cekam na reset.";
const char MSG_BED_LEVELING_FAILED_POINT_LOW_IT[] PROGMEM = "Bed leveling failed. Sensor didnt trigger. Debris on nozzle? Waiting for reset.";
const char MSG_BED_LEVELING_FAILED_POINT_LOW_ES[] PROGMEM = "Bed leveling failed. Sensor didnt trigger. Debris on nozzle? Waiting for reset.";
const char MSG_BED_LEVELING_FAILED_POINT_LOW_PL[] PROGMEM = "Bed leveling failed. Sensor didnt trigger. Debris on nozzle? Waiting for reset.";
const char * const MSG_BED_LEVELING_FAILED_POINT_LOW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_LEVELING_FAILED_POINT_LOW_EN,
	MSG_BED_LEVELING_FAILED_POINT_LOW_CZ,
	MSG_BED_LEVELING_FAILED_POINT_LOW_IT,
	MSG_BED_LEVELING_FAILED_POINT_LOW_ES,
	MSG_BED_LEVELING_FAILED_POINT_LOW_PL
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_EN[] PROGMEM = "X/Y calibration failed. Please consult manual.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_CZ[] PROGMEM = "Kalibrace X/Y selhala. Nahlednete do manualu.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_IT[] PROGMEM = "X/Y calibration failed. Please consult manual.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_ES[] PROGMEM = "X/Y calibration failed. Please consult manual.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_PL[] PROGMEM = "X/Y calibration failed. Please consult manual.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FAILED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_PL
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FRONT_LEFT_FAR_EN[] PROGMEM = "X/Y calibration bad. Left front corner not reachable. Fix the printer.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FRONT_LEFT_FAR_CZ[] PROGMEM = "Kalibrace selhala. Levy predni bod moc vpredu. Srovnejte tiskarnu.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FRONT_LEFT_FAR_IT[] PROGMEM = "X/Y calibration bad. Left front corner not reachable. Fix the printer.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FRONT_LEFT_FAR_ES[] PROGMEM = "X/Y calibration bad. Left front corner not reachable. Fix the printer.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FRONT_LEFT_FAR_PL[] PROGMEM = "X/Y calibration bad. Left front corner not reachable. Fix the printer.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FRONT_LEFT_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FRONT_LEFT_FAR_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_FRONT_LEFT_FAR_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_FRONT_LEFT_FAR_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_FRONT_LEFT_FAR_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_FRONT_LEFT_FAR_PL
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FRONT_RIGHT_FAR_EN[] PROGMEM = "X/Y calibration bad. Right front corner not reachable. Fix the printer.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FRONT_RIGHT_FAR_CZ[] PROGMEM = "Kalibrace selhala. Pravy predni bod moc vpredu. Srovnejte tiskarnu.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FRONT_RIGHT_FAR_IT[] PROGMEM = "X/Y calibration bad. Right front corner not reachable. Fix the printer.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FRONT_RIGHT_FAR_ES[] PROGMEM = "X/Y calibration bad. Right front corner not reachable. Fix the printer.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FRONT_RIGHT_FAR_PL[] PROGMEM = "X/Y calibration bad. Right front corner not reachable. Fix the printer.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FRONT_RIGHT_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FRONT_RIGHT_FAR_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_FRONT_RIGHT_FAR_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_FRONT_RIGHT_FAR_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_FRONT_RIGHT_FAR_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_FRONT_RIGHT_FAR_PL
};

const char MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_EN[] PROGMEM = "X/Y calibration ok. X/Y axes are perpendicular.";
const char MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_CZ[] PROGMEM = "Kalibrace X/Y perfektni. X/Y osy jsou kolme.";
const char MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_IT[] PROGMEM = "X/Y calibration ok. X/Y axes are perpendicular.";
const char MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_ES[] PROGMEM = "X/Y calibration ok. X/Y axes are perpendicular.";
const char MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_PL[] PROGMEM = "X/Y calibration ok. X/Y axes are perpendicular.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_PL
};

const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_EN[] PROGMEM = "X/Y skewed severly. Skew will be corrected automatically.";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_CZ[] PROGMEM = "X/Y osy jsou silne zkosene. Zkoseni bude automaticky vyrovnano pri tisku.";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_IT[] PROGMEM = "X/Y skewed severly. Skew will be corrected automatically.";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_ES[] PROGMEM = "X/Y skewed severly. Skew will be corrected automatically.";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_PL[] PROGMEM = "X/Y skewed severly. Skew will be corrected automatically.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_PL
};

const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_EN[] PROGMEM = "X/Y calibration all right. X/Y axes are slightly skewed.";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_CZ[] PROGMEM = "Kalibrace X/Y v poradku. X/Y osy mirne zkosene.";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_IT[] PROGMEM = "X/Y calibration all right. X/Y axes are slightly skewed.";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_ES[] PROGMEM = "X/Y calibration all right. X/Y axes are slightly skewed.";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_PL[] PROGMEM = "X/Y calibration all right. X/Y axes are slightly skewed.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_PL
};

const char MSG_BEGIN_FILE_LIST_EN[] PROGMEM = "Begin file list";
const char MSG_BEGIN_FILE_LIST_CZ[] PROGMEM = "Begin file list";
const char MSG_BEGIN_FILE_LIST_IT[] PROGMEM = "Begin file list";
const char MSG_BEGIN_FILE_LIST_ES[] PROGMEM = "Begin file list";
const char MSG_BEGIN_FILE_LIST_PL[] PROGMEM = "Begin file list";
const char * const MSG_BEGIN_FILE_LIST_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BEGIN_FILE_LIST_EN,
	MSG_BEGIN_FILE_LIST_CZ,
	MSG_BEGIN_FILE_LIST_IT,
	MSG_BEGIN_FILE_LIST_ES,
	MSG_BEGIN_FILE_LIST_PL
};

const char MSG_BROWNOUT_RESET_EN[] PROGMEM = " Brown out Reset";
const char MSG_BROWNOUT_RESET_CZ[] PROGMEM = " Brown out Reset";
const char MSG_BROWNOUT_RESET_IT[] PROGMEM = " Brown out Reset";
const char MSG_BROWNOUT_RESET_ES[] PROGMEM = " Brown out Reset";
const char MSG_BROWNOUT_RESET_PL[] PROGMEM = " Brown out Reset";
const char * const MSG_BROWNOUT_RESET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BROWNOUT_RESET_EN,
	MSG_BROWNOUT_RESET_CZ,
	MSG_BROWNOUT_RESET_IT,
	MSG_BROWNOUT_RESET_ES,
	MSG_BROWNOUT_RESET_PL
};

const char MSG_CALIBRATE_BED_EN[] PROGMEM = "Calibrate X/Y";
const char MSG_CALIBRATE_BED_CZ[] PROGMEM = "Kalibrace X/Y";
const char MSG_CALIBRATE_BED_IT[] PROGMEM = "Calibrate X/Y";
const char MSG_CALIBRATE_BED_ES[] PROGMEM = "Calibrate X/Y";
const char MSG_CALIBRATE_BED_PL[] PROGMEM = "Calibrate X/Y";
const char * const MSG_CALIBRATE_BED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_BED_EN,
	MSG_CALIBRATE_BED_CZ,
	MSG_CALIBRATE_BED_IT,
	MSG_CALIBRATE_BED_ES,
	MSG_CALIBRATE_BED_PL
};

const char MSG_CALIBRATE_BED_RESET_EN[] PROGMEM = "Reset X/Y calibr.";
const char MSG_CALIBRATE_BED_RESET_CZ[] PROGMEM = "Reset X/Y kalibr.";
const char MSG_CALIBRATE_BED_RESET_IT[] PROGMEM = "Reset X/Y calibr.";
const char MSG_CALIBRATE_BED_RESET_ES[] PROGMEM = "Reset X/Y calibr.";
const char MSG_CALIBRATE_BED_RESET_PL[] PROGMEM = "Reset X/Y calibr.";
const char * const MSG_CALIBRATE_BED_RESET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_BED_RESET_EN,
	MSG_CALIBRATE_BED_RESET_CZ,
	MSG_CALIBRATE_BED_RESET_IT,
	MSG_CALIBRATE_BED_RESET_ES,
	MSG_CALIBRATE_BED_RESET_PL
};

const char MSG_CARD_MENU_EN[] PROGMEM = "Print from SD";
const char MSG_CARD_MENU_CZ[] PROGMEM = "Tisk z SD";
const char MSG_CARD_MENU_IT[] PROGMEM = "Menu SD Carta";
const char MSG_CARD_MENU_ES[] PROGMEM = "Menu de SD";
const char MSG_CARD_MENU_PL[] PROGMEM = "Druk z SD";
const char * const MSG_CARD_MENU_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CARD_MENU_EN,
	MSG_CARD_MENU_CZ,
	MSG_CARD_MENU_IT,
	MSG_CARD_MENU_ES,
	MSG_CARD_MENU_PL
};

const char MSG_CHANGE_SUCCESS_EN[] PROGMEM = "Change success!";
const char MSG_CHANGE_SUCCESS_CZ[] PROGMEM = "Zmena uspesna!";
const char MSG_CHANGE_SUCCESS_IT[] PROGMEM = "Cambia. riuscito!";
const char MSG_CHANGE_SUCCESS_ES[] PROGMEM = "Cambiar bien!";
const char MSG_CHANGE_SUCCESS_PL[] PROGMEM = "Wymiana ok!";
const char * const MSG_CHANGE_SUCCESS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHANGE_SUCCESS_EN,
	MSG_CHANGE_SUCCESS_CZ,
	MSG_CHANGE_SUCCESS_IT,
	MSG_CHANGE_SUCCESS_ES,
	MSG_CHANGE_SUCCESS_PL
};

const char MSG_CHANGING_FILAMENT_EN[] PROGMEM = "Changing filament!";
const char MSG_CHANGING_FILAMENT_CZ[] PROGMEM = "Vymena filamentu!";
const char MSG_CHANGING_FILAMENT_IT[] PROGMEM = "Mutevole fil.!";
const char MSG_CHANGING_FILAMENT_ES[] PROGMEM = "Cambiando fil.!";
const char MSG_CHANGING_FILAMENT_PL[] PROGMEM = "Wymiana filamentu";
const char * const MSG_CHANGING_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHANGING_FILAMENT_EN,
	MSG_CHANGING_FILAMENT_CZ,
	MSG_CHANGING_FILAMENT_IT,
	MSG_CHANGING_FILAMENT_ES,
	MSG_CHANGING_FILAMENT_PL
};

const char MSG_CNG_SDCARD_EN[] PROGMEM = "Change SD card";
const char MSG_CNG_SDCARD_CZ[] PROGMEM = "Vymenit SD";
const char MSG_CNG_SDCARD_IT[] PROGMEM = "Change SD card";
const char MSG_CNG_SDCARD_ES[] PROGMEM = "Change SD card";
const char MSG_CNG_SDCARD_PL[] PROGMEM = "Vymenit SD";
const char * const MSG_CNG_SDCARD_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CNG_SDCARD_EN,
	MSG_CNG_SDCARD_CZ,
	MSG_CNG_SDCARD_IT,
	MSG_CNG_SDCARD_ES,
	MSG_CNG_SDCARD_PL
};

const char MSG_CONFIGURATION_VER_EN[] PROGMEM = " Last Updated: ";
const char MSG_CONFIGURATION_VER_CZ[] PROGMEM = " Last Updated: ";
const char MSG_CONFIGURATION_VER_IT[] PROGMEM = " Last Updated: ";
const char MSG_CONFIGURATION_VER_ES[] PROGMEM = " Last Updated: ";
const char MSG_CONFIGURATION_VER_PL[] PROGMEM = " Last Updated: ";
const char * const MSG_CONFIGURATION_VER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONFIGURATION_VER_EN,
	MSG_CONFIGURATION_VER_CZ,
	MSG_CONFIGURATION_VER_IT,
	MSG_CONFIGURATION_VER_ES,
	MSG_CONFIGURATION_VER_PL
};

const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE1_EN[] PROGMEM = "Are left and right";
const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE1_CZ[] PROGMEM = "Dojely oba Z voziky";
const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE1_IT[] PROGMEM = "Are left and right";
const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE1_ES[] PROGMEM = "Are left and right";
const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE1_PL[] PROGMEM = "Are left and right";
const char * const MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE1_EN,
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE1_CZ,
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE1_IT,
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE1_ES,
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE1_PL
};

const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE2_EN[] PROGMEM = "Z carriages all up?";
const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE2_CZ[] PROGMEM = "k hornimu dorazu?";
const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE2_IT[] PROGMEM = "Z carriages all up?";
const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE2_ES[] PROGMEM = "Z carriages all up?";
const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE2_PL[] PROGMEM = "Z carriages all up?";
const char * const MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE2_EN,
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE2_CZ,
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE2_IT,
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE2_ES,
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LINE2_PL
};

const char MSG_CONTRAST_EN[] PROGMEM = "LCD contrast";
const char MSG_CONTRAST_CZ[] PROGMEM = "LCD contrast";
const char MSG_CONTRAST_IT[] PROGMEM = "LCD contrast";
const char MSG_CONTRAST_ES[] PROGMEM = "LCD contrast";
const char MSG_CONTRAST_PL[] PROGMEM = "LCD contrast";
const char * const MSG_CONTRAST_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONTRAST_EN,
	MSG_CONTRAST_CZ,
	MSG_CONTRAST_IT,
	MSG_CONTRAST_ES,
	MSG_CONTRAST_PL
};

const char MSG_CONTROL_EN[] PROGMEM = "Control";
const char MSG_CONTROL_CZ[] PROGMEM = "Kontrola";
const char MSG_CONTROL_IT[] PROGMEM = "Control";
const char MSG_CONTROL_ES[] PROGMEM = "Control";
const char MSG_CONTROL_PL[] PROGMEM = "Kontrola";
const char * const MSG_CONTROL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONTROL_EN,
	MSG_CONTROL_CZ,
	MSG_CONTROL_IT,
	MSG_CONTROL_ES,
	MSG_CONTROL_PL
};

const char MSG_CONTROL_RETRACT_EN[] PROGMEM = "Retract mm";
const char MSG_CONTROL_RETRACT_CZ[] PROGMEM = "Retract mm";
const char MSG_CONTROL_RETRACT_IT[] PROGMEM = "Retract mm";
const char MSG_CONTROL_RETRACT_ES[] PROGMEM = "Retract mm";
const char MSG_CONTROL_RETRACT_PL[] PROGMEM = "Retract mm";
const char * const MSG_CONTROL_RETRACT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONTROL_RETRACT_EN,
	MSG_CONTROL_RETRACT_CZ,
	MSG_CONTROL_RETRACT_IT,
	MSG_CONTROL_RETRACT_ES,
	MSG_CONTROL_RETRACT_PL
};

const char MSG_CONTROL_RETRACTF_EN[] PROGMEM = "Retract  V";
const char MSG_CONTROL_RETRACTF_CZ[] PROGMEM = "Retract  V";
const char MSG_CONTROL_RETRACTF_IT[] PROGMEM = "Retract  V";
const char MSG_CONTROL_RETRACTF_ES[] PROGMEM = "Retract  V";
const char MSG_CONTROL_RETRACTF_PL[] PROGMEM = "Retract  V";
const char * const MSG_CONTROL_RETRACTF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONTROL_RETRACTF_EN,
	MSG_CONTROL_RETRACTF_CZ,
	MSG_CONTROL_RETRACTF_IT,
	MSG_CONTROL_RETRACTF_ES,
	MSG_CONTROL_RETRACTF_PL
};

const char MSG_CONTROL_RETRACT_RECOVER_EN[] PROGMEM = "UnRet +mm";
const char MSG_CONTROL_RETRACT_RECOVER_CZ[] PROGMEM = "UnRet +mm";
const char MSG_CONTROL_RETRACT_RECOVER_IT[] PROGMEM = "UnRet +mm";
const char MSG_CONTROL_RETRACT_RECOVER_ES[] PROGMEM = "UnRet +mm";
const char MSG_CONTROL_RETRACT_RECOVER_PL[] PROGMEM = "UnRet +mm";
const char * const MSG_CONTROL_RETRACT_RECOVER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONTROL_RETRACT_RECOVER_EN,
	MSG_CONTROL_RETRACT_RECOVER_CZ,
	MSG_CONTROL_RETRACT_RECOVER_IT,
	MSG_CONTROL_RETRACT_RECOVER_ES,
	MSG_CONTROL_RETRACT_RECOVER_PL
};

const char MSG_CONTROL_RETRACT_RECOVERF_EN[] PROGMEM = "UnRet  V";
const char MSG_CONTROL_RETRACT_RECOVERF_CZ[] PROGMEM = "UnRet  V";
const char MSG_CONTROL_RETRACT_RECOVERF_IT[] PROGMEM = "UnRet  V";
const char MSG_CONTROL_RETRACT_RECOVERF_ES[] PROGMEM = "UnRet  V";
const char MSG_CONTROL_RETRACT_RECOVERF_PL[] PROGMEM = "UnRet  V";
const char * const MSG_CONTROL_RETRACT_RECOVERF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONTROL_RETRACT_RECOVERF_EN,
	MSG_CONTROL_RETRACT_RECOVERF_CZ,
	MSG_CONTROL_RETRACT_RECOVERF_IT,
	MSG_CONTROL_RETRACT_RECOVERF_ES,
	MSG_CONTROL_RETRACT_RECOVERF_PL
};

const char MSG_CONTROL_RETRACT_RECOVER_SWAP_EN[] PROGMEM = "S UnRet+mm";
const char MSG_CONTROL_RETRACT_RECOVER_SWAP_CZ[] PROGMEM = "S UnRet+mm";
const char MSG_CONTROL_RETRACT_RECOVER_SWAP_IT[] PROGMEM = "S UnRet+mm";
const char MSG_CONTROL_RETRACT_RECOVER_SWAP_ES[] PROGMEM = "S UnRet+mm";
const char MSG_CONTROL_RETRACT_RECOVER_SWAP_PL[] PROGMEM = "S UnRet+mm";
const char * const MSG_CONTROL_RETRACT_RECOVER_SWAP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONTROL_RETRACT_RECOVER_SWAP_EN,
	MSG_CONTROL_RETRACT_RECOVER_SWAP_CZ,
	MSG_CONTROL_RETRACT_RECOVER_SWAP_IT,
	MSG_CONTROL_RETRACT_RECOVER_SWAP_ES,
	MSG_CONTROL_RETRACT_RECOVER_SWAP_PL
};

const char MSG_CONTROL_RETRACT_SWAP_EN[] PROGMEM = "Swap Re.mm";
const char MSG_CONTROL_RETRACT_SWAP_CZ[] PROGMEM = "Swap Re.mm";
const char MSG_CONTROL_RETRACT_SWAP_IT[] PROGMEM = "Swap Re.mm";
const char MSG_CONTROL_RETRACT_SWAP_ES[] PROGMEM = "Swap Re.mm";
const char MSG_CONTROL_RETRACT_SWAP_PL[] PROGMEM = "Swap Re.mm";
const char * const MSG_CONTROL_RETRACT_SWAP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONTROL_RETRACT_SWAP_EN,
	MSG_CONTROL_RETRACT_SWAP_CZ,
	MSG_CONTROL_RETRACT_SWAP_IT,
	MSG_CONTROL_RETRACT_SWAP_ES,
	MSG_CONTROL_RETRACT_SWAP_PL
};

const char MSG_CONTROL_RETRACT_ZLIFT_EN[] PROGMEM = "Hop mm";
const char MSG_CONTROL_RETRACT_ZLIFT_CZ[] PROGMEM = "Hop mm";
const char MSG_CONTROL_RETRACT_ZLIFT_IT[] PROGMEM = "Hop mm";
const char MSG_CONTROL_RETRACT_ZLIFT_ES[] PROGMEM = "Hop mm";
const char MSG_CONTROL_RETRACT_ZLIFT_PL[] PROGMEM = "Hop mm";
const char * const MSG_CONTROL_RETRACT_ZLIFT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONTROL_RETRACT_ZLIFT_EN,
	MSG_CONTROL_RETRACT_ZLIFT_CZ,
	MSG_CONTROL_RETRACT_ZLIFT_IT,
	MSG_CONTROL_RETRACT_ZLIFT_ES,
	MSG_CONTROL_RETRACT_ZLIFT_PL
};

const char MSG_COOLDOWN_EN[] PROGMEM = "Cooldown";
const char MSG_COOLDOWN_CZ[] PROGMEM = "Zchladit";
const char MSG_COOLDOWN_IT[] PROGMEM = "Raffredda";
const char MSG_COOLDOWN_ES[] PROGMEM = "Enfriar";
const char MSG_COOLDOWN_PL[] PROGMEM = "Wychlodzic";
const char * const MSG_COOLDOWN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_COOLDOWN_EN,
	MSG_COOLDOWN_CZ,
	MSG_COOLDOWN_IT,
	MSG_COOLDOWN_ES,
	MSG_COOLDOWN_PL
};

const char MSG_CORRECTLY_EN[] PROGMEM = "Changed correctly?";
const char MSG_CORRECTLY_CZ[] PROGMEM = "Vymena ok?";
const char MSG_CORRECTLY_IT[] PROGMEM = "Cambiato corr.?";
const char MSG_CORRECTLY_ES[] PROGMEM = "Cambiado correc.?";
const char MSG_CORRECTLY_PL[] PROGMEM = "Wymiana ok?";
const char * const MSG_CORRECTLY_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CORRECTLY_EN,
	MSG_CORRECTLY_CZ,
	MSG_CORRECTLY_IT,
	MSG_CORRECTLY_ES,
	MSG_CORRECTLY_PL
};

const char MSG_COUNT_X_EN[] PROGMEM = " Count X: ";
const char MSG_COUNT_X_CZ[] PROGMEM = " Count X: ";
const char MSG_COUNT_X_IT[] PROGMEM = " Count X: ";
const char MSG_COUNT_X_ES[] PROGMEM = " Count X: ";
const char MSG_COUNT_X_PL[] PROGMEM = " Count X: ";
const char * const MSG_COUNT_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_COUNT_X_EN,
	MSG_COUNT_X_CZ,
	MSG_COUNT_X_IT,
	MSG_COUNT_X_ES,
	MSG_COUNT_X_PL
};

const char MSG_DISABLE_STEPPERS_EN[] PROGMEM = "Disable steppers";
const char MSG_DISABLE_STEPPERS_CZ[] PROGMEM = "Vypnout motory";
const char MSG_DISABLE_STEPPERS_IT[] PROGMEM = "Disabilita Motori";
const char MSG_DISABLE_STEPPERS_ES[] PROGMEM = "Apagar motores";
const char MSG_DISABLE_STEPPERS_PL[] PROGMEM = "Wylaczyc silniki";
const char * const MSG_DISABLE_STEPPERS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_DISABLE_STEPPERS_EN,
	MSG_DISABLE_STEPPERS_CZ,
	MSG_DISABLE_STEPPERS_IT,
	MSG_DISABLE_STEPPERS_ES,
	MSG_DISABLE_STEPPERS_PL
};

const char MSG_DWELL_EN[] PROGMEM = "Sleep...";
const char MSG_DWELL_CZ[] PROGMEM = "Sleep...";
const char MSG_DWELL_IT[] PROGMEM = "Sospensione...";
const char MSG_DWELL_ES[] PROGMEM = "Reposo...";
const char MSG_DWELL_PL[] PROGMEM = "Sleep...";
const char * const MSG_DWELL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_DWELL_EN,
	MSG_DWELL_CZ,
	MSG_DWELL_IT,
	MSG_DWELL_ES,
	MSG_DWELL_PL
};

const char MSG_E_EN[] PROGMEM = "e";
const char MSG_E_CZ[] PROGMEM = "e";
const char MSG_E_IT[] PROGMEM = "e";
const char MSG_E_ES[] PROGMEM = "e";
const char MSG_E_PL[] PROGMEM = "e";
const char * const MSG_E_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_E_EN,
	MSG_E_CZ,
	MSG_E_IT,
	MSG_E_ES,
	MSG_E_PL
};

const char MSG_ENDSTOPS_HIT_EN[] PROGMEM = "endstops hit: ";
const char MSG_ENDSTOPS_HIT_CZ[] PROGMEM = "endstops hit: ";
const char MSG_ENDSTOPS_HIT_IT[] PROGMEM = "endstops hit: ";
const char MSG_ENDSTOPS_HIT_ES[] PROGMEM = "endstops hit: ";
const char MSG_ENDSTOPS_HIT_PL[] PROGMEM = "endstops hit: ";
const char * const MSG_ENDSTOPS_HIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ENDSTOPS_HIT_EN,
	MSG_ENDSTOPS_HIT_CZ,
	MSG_ENDSTOPS_HIT_IT,
	MSG_ENDSTOPS_HIT_ES,
	MSG_ENDSTOPS_HIT_PL
};

const char MSG_ENDSTOP_ABORT_EN[] PROGMEM = "Endstop abort";
const char MSG_ENDSTOP_ABORT_CZ[] PROGMEM = "Endstop abort";
const char MSG_ENDSTOP_ABORT_IT[] PROGMEM = "Endstop abort";
const char MSG_ENDSTOP_ABORT_ES[] PROGMEM = "Endstop abort";
const char MSG_ENDSTOP_ABORT_PL[] PROGMEM = "Endstop abort";
const char * const MSG_ENDSTOP_ABORT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ENDSTOP_ABORT_EN,
	MSG_ENDSTOP_ABORT_CZ,
	MSG_ENDSTOP_ABORT_IT,
	MSG_ENDSTOP_ABORT_ES,
	MSG_ENDSTOP_ABORT_PL
};

const char MSG_ENDSTOP_HIT_EN[] PROGMEM = "TRIGGERED";
const char MSG_ENDSTOP_HIT_CZ[] PROGMEM = "TRIGGERED";
const char MSG_ENDSTOP_HIT_IT[] PROGMEM = "TRIGGERED";
const char MSG_ENDSTOP_HIT_ES[] PROGMEM = "TRIGGERED";
const char MSG_ENDSTOP_HIT_PL[] PROGMEM = "TRIGGERED";
const char * const MSG_ENDSTOP_HIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ENDSTOP_HIT_EN,
	MSG_ENDSTOP_HIT_CZ,
	MSG_ENDSTOP_HIT_IT,
	MSG_ENDSTOP_HIT_ES,
	MSG_ENDSTOP_HIT_PL
};

const char MSG_ENDSTOP_OPEN_EN[] PROGMEM = "open";
const char MSG_ENDSTOP_OPEN_CZ[] PROGMEM = "open";
const char MSG_ENDSTOP_OPEN_IT[] PROGMEM = "open";
const char MSG_ENDSTOP_OPEN_ES[] PROGMEM = "open";
const char MSG_ENDSTOP_OPEN_PL[] PROGMEM = "open";
const char * const MSG_ENDSTOP_OPEN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ENDSTOP_OPEN_EN,
	MSG_ENDSTOP_OPEN_CZ,
	MSG_ENDSTOP_OPEN_IT,
	MSG_ENDSTOP_OPEN_ES,
	MSG_ENDSTOP_OPEN_PL
};

const char MSG_END_FILE_LIST_EN[] PROGMEM = "End file list";
const char MSG_END_FILE_LIST_CZ[] PROGMEM = "End file list";
const char MSG_END_FILE_LIST_IT[] PROGMEM = "End file list";
const char MSG_END_FILE_LIST_ES[] PROGMEM = "End file list";
const char MSG_END_FILE_LIST_PL[] PROGMEM = "End file list";
const char * const MSG_END_FILE_LIST_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_END_FILE_LIST_EN,
	MSG_END_FILE_LIST_CZ,
	MSG_END_FILE_LIST_IT,
	MSG_END_FILE_LIST_ES,
	MSG_END_FILE_LIST_PL
};

const char MSG_ERROR_EN[] PROGMEM = "ERROR:";
const char MSG_ERROR_CZ[] PROGMEM = "CHYBA:";
const char MSG_ERROR_IT[] PROGMEM = "ERROR:";
const char MSG_ERROR_ES[] PROGMEM = "ERROR:";
const char MSG_ERROR_PL[] PROGMEM = "BLAD:";
const char * const MSG_ERROR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERROR_EN,
	MSG_ERROR_CZ,
	MSG_ERROR_IT,
	MSG_ERROR_ES,
	MSG_ERROR_PL
};

const char MSG_ERR_CHECKSUM_MISMATCH_EN[] PROGMEM = "checksum mismatch, Last Line: ";
const char MSG_ERR_CHECKSUM_MISMATCH_CZ[] PROGMEM = "checksum mismatch, Last Line: ";
const char MSG_ERR_CHECKSUM_MISMATCH_IT[] PROGMEM = "checksum mismatch, Last Line: ";
const char MSG_ERR_CHECKSUM_MISMATCH_ES[] PROGMEM = "checksum mismatch, Last Line: ";
const char MSG_ERR_CHECKSUM_MISMATCH_PL[] PROGMEM = "checksum mismatch, Last Line: ";
const char * const MSG_ERR_CHECKSUM_MISMATCH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_CHECKSUM_MISMATCH_EN,
	MSG_ERR_CHECKSUM_MISMATCH_CZ,
	MSG_ERR_CHECKSUM_MISMATCH_IT,
	MSG_ERR_CHECKSUM_MISMATCH_ES,
	MSG_ERR_CHECKSUM_MISMATCH_PL
};

const char MSG_ERR_COLD_EXTRUDE_STOP_EN[] PROGMEM = " cold extrusion prevented";
const char MSG_ERR_COLD_EXTRUDE_STOP_CZ[] PROGMEM = " cold extrusion prevented";
const char MSG_ERR_COLD_EXTRUDE_STOP_IT[] PROGMEM = " cold extrusion prevented";
const char MSG_ERR_COLD_EXTRUDE_STOP_ES[] PROGMEM = " cold extrusion prevented";
const char MSG_ERR_COLD_EXTRUDE_STOP_PL[] PROGMEM = " cold extrusion prevented";
const char * const MSG_ERR_COLD_EXTRUDE_STOP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_COLD_EXTRUDE_STOP_EN,
	MSG_ERR_COLD_EXTRUDE_STOP_CZ,
	MSG_ERR_COLD_EXTRUDE_STOP_IT,
	MSG_ERR_COLD_EXTRUDE_STOP_ES,
	MSG_ERR_COLD_EXTRUDE_STOP_PL
};

const char MSG_ERR_KILLED_EN[] PROGMEM = "Printer halted. kill() called!";
const char MSG_ERR_KILLED_CZ[] PROGMEM = "Printer halted. kill() called!";
const char MSG_ERR_KILLED_IT[] PROGMEM = "Printer halted. kill() called!";
const char MSG_ERR_KILLED_ES[] PROGMEM = "Printer halted. kill() called!";
const char MSG_ERR_KILLED_PL[] PROGMEM = "Printer halted. kill() called!";
const char * const MSG_ERR_KILLED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_KILLED_EN,
	MSG_ERR_KILLED_CZ,
	MSG_ERR_KILLED_IT,
	MSG_ERR_KILLED_ES,
	MSG_ERR_KILLED_PL
};

const char MSG_ERR_LINE_NO_EN[] PROGMEM = "Line Number is not Last Line Number+1, Last Line: ";
const char MSG_ERR_LINE_NO_CZ[] PROGMEM = "Line Number is not Last Line Number+1, Last Line: ";
const char MSG_ERR_LINE_NO_IT[] PROGMEM = "Line Number is not Last Line Number+1, Last Line: ";
const char MSG_ERR_LINE_NO_ES[] PROGMEM = "Line Number is not Last Line Number+1, Last Line: ";
const char MSG_ERR_LINE_NO_PL[] PROGMEM = "Line Number is not Last Line Number+1, Last Line: ";
const char * const MSG_ERR_LINE_NO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_LINE_NO_EN,
	MSG_ERR_LINE_NO_CZ,
	MSG_ERR_LINE_NO_IT,
	MSG_ERR_LINE_NO_ES,
	MSG_ERR_LINE_NO_PL
};

const char MSG_ERR_LONG_EXTRUDE_STOP_EN[] PROGMEM = " too long extrusion prevented";
const char MSG_ERR_LONG_EXTRUDE_STOP_CZ[] PROGMEM = " too long extrusion prevented";
const char MSG_ERR_LONG_EXTRUDE_STOP_IT[] PROGMEM = " too long extrusion prevented";
const char MSG_ERR_LONG_EXTRUDE_STOP_ES[] PROGMEM = " too long extrusion prevented";
const char MSG_ERR_LONG_EXTRUDE_STOP_PL[] PROGMEM = " too long extrusion prevented";
const char * const MSG_ERR_LONG_EXTRUDE_STOP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_LONG_EXTRUDE_STOP_EN,
	MSG_ERR_LONG_EXTRUDE_STOP_CZ,
	MSG_ERR_LONG_EXTRUDE_STOP_IT,
	MSG_ERR_LONG_EXTRUDE_STOP_ES,
	MSG_ERR_LONG_EXTRUDE_STOP_PL
};

const char MSG_ERR_NO_CHECKSUM_EN[] PROGMEM = "No Checksum with line number, Last Line: ";
const char MSG_ERR_NO_CHECKSUM_CZ[] PROGMEM = "No Checksum with line number, Last Line: ";
const char MSG_ERR_NO_CHECKSUM_IT[] PROGMEM = "No Checksum with line number, Last Line: ";
const char MSG_ERR_NO_CHECKSUM_ES[] PROGMEM = "No Checksum with line number, Last Line: ";
const char MSG_ERR_NO_CHECKSUM_PL[] PROGMEM = "No Checksum with line number, Last Line: ";
const char * const MSG_ERR_NO_CHECKSUM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_NO_CHECKSUM_EN,
	MSG_ERR_NO_CHECKSUM_CZ,
	MSG_ERR_NO_CHECKSUM_IT,
	MSG_ERR_NO_CHECKSUM_ES,
	MSG_ERR_NO_CHECKSUM_PL
};

const char MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_EN[] PROGMEM = "No Line Number with checksum, Last Line: ";
const char MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_CZ[] PROGMEM = "No Line Number with checksum, Last Line: ";
const char MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_IT[] PROGMEM = "No Line Number with checksum, Last Line: ";
const char MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_ES[] PROGMEM = "No Line Number with checksum, Last Line: ";
const char MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_PL[] PROGMEM = "No Line Number with checksum, Last Line: ";
const char * const MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_EN,
	MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_CZ,
	MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_IT,
	MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_ES,
	MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_PL
};

const char MSG_ERR_NO_THERMISTORS_EN[] PROGMEM = "No thermistors - no temperature";
const char MSG_ERR_NO_THERMISTORS_CZ[] PROGMEM = "No thermistors - no temperature";
const char MSG_ERR_NO_THERMISTORS_IT[] PROGMEM = "No thermistors - no temperature";
const char MSG_ERR_NO_THERMISTORS_ES[] PROGMEM = "No thermistors - no temperature";
const char MSG_ERR_NO_THERMISTORS_PL[] PROGMEM = "No thermistors - no temperature";
const char * const MSG_ERR_NO_THERMISTORS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_NO_THERMISTORS_EN,
	MSG_ERR_NO_THERMISTORS_CZ,
	MSG_ERR_NO_THERMISTORS_IT,
	MSG_ERR_NO_THERMISTORS_ES,
	MSG_ERR_NO_THERMISTORS_PL
};

const char MSG_ERR_STOPPED_EN[] PROGMEM = "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)";
const char MSG_ERR_STOPPED_CZ[] PROGMEM = "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)";
const char MSG_ERR_STOPPED_IT[] PROGMEM = "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)";
const char MSG_ERR_STOPPED_ES[] PROGMEM = "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)";
const char MSG_ERR_STOPPED_PL[] PROGMEM = "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)";
const char * const MSG_ERR_STOPPED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERR_STOPPED_EN,
	MSG_ERR_STOPPED_CZ,
	MSG_ERR_STOPPED_IT,
	MSG_ERR_STOPPED_ES,
	MSG_ERR_STOPPED_PL
};

const char MSG_ESTEPS_EN[] PROGMEM = "Esteps/mm";
const char MSG_ESTEPS_CZ[] PROGMEM = "Esteps/mm";
const char MSG_ESTEPS_IT[] PROGMEM = "Esteps/mm";
const char MSG_ESTEPS_ES[] PROGMEM = "Esteps/mm";
const char MSG_ESTEPS_PL[] PROGMEM = "Esteps/mm";
const char * const MSG_ESTEPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ESTEPS_EN,
	MSG_ESTEPS_CZ,
	MSG_ESTEPS_IT,
	MSG_ESTEPS_ES,
	MSG_ESTEPS_PL
};

const char MSG_EXTERNAL_RESET_EN[] PROGMEM = " External Reset";
const char MSG_EXTERNAL_RESET_CZ[] PROGMEM = " External Reset";
const char MSG_EXTERNAL_RESET_IT[] PROGMEM = " External Reset";
const char MSG_EXTERNAL_RESET_ES[] PROGMEM = " External Reset";
const char MSG_EXTERNAL_RESET_PL[] PROGMEM = " External Reset";
const char * const MSG_EXTERNAL_RESET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_EXTERNAL_RESET_EN,
	MSG_EXTERNAL_RESET_CZ,
	MSG_EXTERNAL_RESET_IT,
	MSG_EXTERNAL_RESET_ES,
	MSG_EXTERNAL_RESET_PL
};

const char MSG_EXTRUDE_EN[] PROGMEM = "Extrude";
const char MSG_EXTRUDE_CZ[] PROGMEM = "Extrudovat";
const char MSG_EXTRUDE_IT[] PROGMEM = "Extrude";
const char MSG_EXTRUDE_ES[] PROGMEM = "Extrude";
const char MSG_EXTRUDE_PL[] PROGMEM = "Extrudovat";
const char * const MSG_EXTRUDE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_EXTRUDE_EN,
	MSG_EXTRUDE_CZ,
	MSG_EXTRUDE_IT,
	MSG_EXTRUDE_ES,
	MSG_EXTRUDE_PL
};

const char MSG_Enqueing_EN[] PROGMEM = "enqueing \"";
const char MSG_Enqueing_CZ[] PROGMEM = "enqueing \"";
const char MSG_Enqueing_IT[] PROGMEM = "enqueing \"";
const char MSG_Enqueing_ES[] PROGMEM = "enqueing \"";
const char MSG_Enqueing_PL[] PROGMEM = "enqueing \"";
const char * const MSG_Enqueing_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_Enqueing_EN,
	MSG_Enqueing_CZ,
	MSG_Enqueing_IT,
	MSG_Enqueing_ES,
	MSG_Enqueing_PL
};

const char MSG_FACTOR_EN[] PROGMEM = " \002 Fact";
const char MSG_FACTOR_CZ[] PROGMEM = " \002 Fact";
const char MSG_FACTOR_IT[] PROGMEM = " \002 Fact";
const char MSG_FACTOR_ES[] PROGMEM = " \002 Fact";
const char MSG_FACTOR_PL[] PROGMEM = " \002 Fact";
const char * const MSG_FACTOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FACTOR_EN,
	MSG_FACTOR_CZ,
	MSG_FACTOR_IT,
	MSG_FACTOR_ES,
	MSG_FACTOR_PL
};

const char MSG_FAN_SPEED_EN[] PROGMEM = "Fan speed";
const char MSG_FAN_SPEED_CZ[] PROGMEM = "Rychlost vent.";
const char MSG_FAN_SPEED_IT[] PROGMEM = "Ventola";
const char MSG_FAN_SPEED_ES[] PROGMEM = "Ventilador";
const char MSG_FAN_SPEED_PL[] PROGMEM = "Predkosc went.";
const char * const MSG_FAN_SPEED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FAN_SPEED_EN,
	MSG_FAN_SPEED_CZ,
	MSG_FAN_SPEED_IT,
	MSG_FAN_SPEED_ES,
	MSG_FAN_SPEED_PL
};

const char MSG_FILAMENTCHANGE_EN[] PROGMEM = "Change filament";
const char MSG_FILAMENTCHANGE_CZ[] PROGMEM = "Vymenit filament";
const char MSG_FILAMENTCHANGE_IT[] PROGMEM = "Cambiare filamento";
const char MSG_FILAMENTCHANGE_ES[] PROGMEM = "Cambiar filamento";
const char MSG_FILAMENTCHANGE_PL[] PROGMEM = "Wymienic filament";
const char * const MSG_FILAMENTCHANGE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENTCHANGE_EN,
	MSG_FILAMENTCHANGE_CZ,
	MSG_FILAMENTCHANGE_IT,
	MSG_FILAMENTCHANGE_ES,
	MSG_FILAMENTCHANGE_PL
};

const char MSG_FILAMENT_SIZE_EXTRUDER_0_EN[] PROGMEM = "Fil. Dia. 1";
const char MSG_FILAMENT_SIZE_EXTRUDER_0_CZ[] PROGMEM = "Fil. Dia. 1";
const char MSG_FILAMENT_SIZE_EXTRUDER_0_IT[] PROGMEM = "Fil. Dia. 1";
const char MSG_FILAMENT_SIZE_EXTRUDER_0_ES[] PROGMEM = "Fil. Dia. 1";
const char MSG_FILAMENT_SIZE_EXTRUDER_0_PL[] PROGMEM = "Fil. Dia. 1";
const char * const MSG_FILAMENT_SIZE_EXTRUDER_0_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_SIZE_EXTRUDER_0_EN,
	MSG_FILAMENT_SIZE_EXTRUDER_0_CZ,
	MSG_FILAMENT_SIZE_EXTRUDER_0_IT,
	MSG_FILAMENT_SIZE_EXTRUDER_0_ES,
	MSG_FILAMENT_SIZE_EXTRUDER_0_PL
};

const char MSG_FILAMENT_SIZE_EXTRUDER_1_EN[] PROGMEM = "Fil. Dia. 2";
const char MSG_FILAMENT_SIZE_EXTRUDER_1_CZ[] PROGMEM = "Fil. Dia. 2";
const char MSG_FILAMENT_SIZE_EXTRUDER_1_IT[] PROGMEM = "Fil. Dia. 2";
const char MSG_FILAMENT_SIZE_EXTRUDER_1_ES[] PROGMEM = "Fil. Dia. 2";
const char MSG_FILAMENT_SIZE_EXTRUDER_1_PL[] PROGMEM = "Fil. Dia. 2";
const char * const MSG_FILAMENT_SIZE_EXTRUDER_1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_SIZE_EXTRUDER_1_EN,
	MSG_FILAMENT_SIZE_EXTRUDER_1_CZ,
	MSG_FILAMENT_SIZE_EXTRUDER_1_IT,
	MSG_FILAMENT_SIZE_EXTRUDER_1_ES,
	MSG_FILAMENT_SIZE_EXTRUDER_1_PL
};

const char MSG_FILAMENT_SIZE_EXTRUDER_2_EN[] PROGMEM = "Fil. Dia. 3";
const char MSG_FILAMENT_SIZE_EXTRUDER_2_CZ[] PROGMEM = "Fil. Dia. 3";
const char MSG_FILAMENT_SIZE_EXTRUDER_2_IT[] PROGMEM = "Fil. Dia. 3";
const char MSG_FILAMENT_SIZE_EXTRUDER_2_ES[] PROGMEM = "Fil. Dia. 3";
const char MSG_FILAMENT_SIZE_EXTRUDER_2_PL[] PROGMEM = "Fil. Dia. 3";
const char * const MSG_FILAMENT_SIZE_EXTRUDER_2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_SIZE_EXTRUDER_2_EN,
	MSG_FILAMENT_SIZE_EXTRUDER_2_CZ,
	MSG_FILAMENT_SIZE_EXTRUDER_2_IT,
	MSG_FILAMENT_SIZE_EXTRUDER_2_ES,
	MSG_FILAMENT_SIZE_EXTRUDER_2_PL
};

const char MSG_FILE_PRINTED_EN[] PROGMEM = "Done printing file";
const char MSG_FILE_PRINTED_CZ[] PROGMEM = "Done printing file";
const char MSG_FILE_PRINTED_IT[] PROGMEM = "Done printing file";
const char MSG_FILE_PRINTED_ES[] PROGMEM = "Done printing file";
const char MSG_FILE_PRINTED_PL[] PROGMEM = "Done printing file";
const char * const MSG_FILE_PRINTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILE_PRINTED_EN,
	MSG_FILE_PRINTED_CZ,
	MSG_FILE_PRINTED_IT,
	MSG_FILE_PRINTED_ES,
	MSG_FILE_PRINTED_PL
};

const char MSG_FILE_SAVED_EN[] PROGMEM = "Done saving file.";
const char MSG_FILE_SAVED_CZ[] PROGMEM = "Done saving file.";
const char MSG_FILE_SAVED_IT[] PROGMEM = "Done saving file.";
const char MSG_FILE_SAVED_ES[] PROGMEM = "Done saving file.";
const char MSG_FILE_SAVED_PL[] PROGMEM = "Done saving file.";
const char * const MSG_FILE_SAVED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILE_SAVED_EN,
	MSG_FILE_SAVED_CZ,
	MSG_FILE_SAVED_IT,
	MSG_FILE_SAVED_ES,
	MSG_FILE_SAVED_PL
};

const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_EN[] PROGMEM = "Searching bed";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_CZ[] PROGMEM = "Hledam kalibracni";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_IT[] PROGMEM = "Searching bed";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_ES[] PROGMEM = "Searching bed";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_PL[] PROGMEM = "Searching bed";
const char * const MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_EN,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_CZ,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_IT,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_ES,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_PL
};

const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_EN[] PROGMEM = "calibration point";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_CZ[] PROGMEM = "bod podlozky";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_IT[] PROGMEM = "calibration point";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_ES[] PROGMEM = "calibration point";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_PL[] PROGMEM = "calibration point";
const char * const MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_EN,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_CZ,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_IT,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_ES,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_PL
};

const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE3_EN[] PROGMEM = " of 4";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE3_CZ[] PROGMEM = " z 4";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE3_IT[] PROGMEM = " of 4";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE3_ES[] PROGMEM = " of 4";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE3_PL[] PROGMEM = " of 4";
const char * const MSG_FIND_BED_OFFSET_AND_SKEW_LINE3_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE3_EN,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE3_CZ,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE3_IT,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE3_ES,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE3_PL
};

const char MSG_FLOW_EN[] PROGMEM = "Flow";
const char MSG_FLOW_CZ[] PROGMEM = "Prutok";
const char MSG_FLOW_IT[] PROGMEM = "Flusso";
const char MSG_FLOW_ES[] PROGMEM = "Flujo";
const char MSG_FLOW_PL[] PROGMEM = "Przeplyw";
const char * const MSG_FLOW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FLOW_EN,
	MSG_FLOW_CZ,
	MSG_FLOW_IT,
	MSG_FLOW_ES,
	MSG_FLOW_PL
};

const char MSG_FLOW0_EN[] PROGMEM = "Flow 0";
const char MSG_FLOW0_CZ[] PROGMEM = "Prutok 0";
const char MSG_FLOW0_IT[] PROGMEM = "Flow 0";
const char MSG_FLOW0_ES[] PROGMEM = "Flow 0";
const char MSG_FLOW0_PL[] PROGMEM = "Prutok 0";
const char * const MSG_FLOW0_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FLOW0_EN,
	MSG_FLOW0_CZ,
	MSG_FLOW0_IT,
	MSG_FLOW0_ES,
	MSG_FLOW0_PL
};

const char MSG_FLOW1_EN[] PROGMEM = "Flow 1";
const char MSG_FLOW1_CZ[] PROGMEM = "Prutok 1";
const char MSG_FLOW1_IT[] PROGMEM = "Flow 1";
const char MSG_FLOW1_ES[] PROGMEM = "Flow 1";
const char MSG_FLOW1_PL[] PROGMEM = "Prutok 1";
const char * const MSG_FLOW1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FLOW1_EN,
	MSG_FLOW1_CZ,
	MSG_FLOW1_IT,
	MSG_FLOW1_ES,
	MSG_FLOW1_PL
};

const char MSG_FLOW2_EN[] PROGMEM = "Flow 2";
const char MSG_FLOW2_CZ[] PROGMEM = "Prutok 2";
const char MSG_FLOW2_IT[] PROGMEM = "Flow 2";
const char MSG_FLOW2_ES[] PROGMEM = "Flow 2";
const char MSG_FLOW2_PL[] PROGMEM = "Prutok 2";
const char * const MSG_FLOW2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FLOW2_EN,
	MSG_FLOW2_CZ,
	MSG_FLOW2_IT,
	MSG_FLOW2_ES,
	MSG_FLOW2_PL
};

const char MSG_FREE_MEMORY_EN[] PROGMEM = " Free Memory: ";
const char MSG_FREE_MEMORY_CZ[] PROGMEM = " Free Memory: ";
const char MSG_FREE_MEMORY_IT[] PROGMEM = " Free Memory: ";
const char MSG_FREE_MEMORY_ES[] PROGMEM = " Free Memory: ";
const char MSG_FREE_MEMORY_PL[] PROGMEM = " Free Memory: ";
const char * const MSG_FREE_MEMORY_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FREE_MEMORY_EN,
	MSG_FREE_MEMORY_CZ,
	MSG_FREE_MEMORY_IT,
	MSG_FREE_MEMORY_ES,
	MSG_FREE_MEMORY_PL
};

const char MSG_HEATING_EN[] PROGMEM = "Heating";
const char MSG_HEATING_CZ[] PROGMEM = "Zahrivani";
const char MSG_HEATING_IT[] PROGMEM = "Riscaldamento...";
const char MSG_HEATING_ES[] PROGMEM = "Calentando...";
const char MSG_HEATING_PL[] PROGMEM = "Grzanie...";
const char * const MSG_HEATING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HEATING_EN,
	MSG_HEATING_CZ,
	MSG_HEATING_IT,
	MSG_HEATING_ES,
	MSG_HEATING_PL
};

const char MSG_HEATING_COMPLETE_EN[] PROGMEM = "Heating done.";
const char MSG_HEATING_COMPLETE_CZ[] PROGMEM = "Zahrivani OK.";
const char MSG_HEATING_COMPLETE_IT[] PROGMEM = "Riscaldamento fatto.";
const char MSG_HEATING_COMPLETE_ES[] PROGMEM = "Calentando listo.";
const char MSG_HEATING_COMPLETE_PL[] PROGMEM = "Grzanie OK.";
const char * const MSG_HEATING_COMPLETE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HEATING_COMPLETE_EN,
	MSG_HEATING_COMPLETE_CZ,
	MSG_HEATING_COMPLETE_IT,
	MSG_HEATING_COMPLETE_ES,
	MSG_HEATING_COMPLETE_PL
};

const char MSG_HOMEYZ_EN[] PROGMEM = "Calibrate Z";
const char MSG_HOMEYZ_CZ[] PROGMEM = "Kalibrovat Z";
const char MSG_HOMEYZ_IT[] PROGMEM = "Calibra Z";
const char MSG_HOMEYZ_ES[] PROGMEM = "Calibrar Z";
const char MSG_HOMEYZ_PL[] PROGMEM = "Kalibruj Z";
const char * const MSG_HOMEYZ_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HOMEYZ_EN,
	MSG_HOMEYZ_CZ,
	MSG_HOMEYZ_IT,
	MSG_HOMEYZ_ES,
	MSG_HOMEYZ_PL
};

const char MSG_HOMEYZ_DONE_EN[] PROGMEM = "Calibration done";
const char MSG_HOMEYZ_DONE_CZ[] PROGMEM = "Kalibrace OK";
const char MSG_HOMEYZ_DONE_IT[] PROGMEM = "Calibratura OK";
const char MSG_HOMEYZ_DONE_ES[] PROGMEM = "Calibracion OK";
const char MSG_HOMEYZ_DONE_PL[] PROGMEM = "Kalibracja OK";
const char * const MSG_HOMEYZ_DONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HOMEYZ_DONE_EN,
	MSG_HOMEYZ_DONE_CZ,
	MSG_HOMEYZ_DONE_IT,
	MSG_HOMEYZ_DONE_ES,
	MSG_HOMEYZ_DONE_PL
};

const char MSG_HOMEYZ_PROGRESS_EN[] PROGMEM = "Calibrating Z";
const char MSG_HOMEYZ_PROGRESS_CZ[] PROGMEM = "Kalibruji Z";
const char MSG_HOMEYZ_PROGRESS_IT[] PROGMEM = "Calibrando Z";
const char MSG_HOMEYZ_PROGRESS_ES[] PROGMEM = "Calibrando Z";
const char MSG_HOMEYZ_PROGRESS_PL[] PROGMEM = "Kalibruje Z";
const char * const MSG_HOMEYZ_PROGRESS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HOMEYZ_PROGRESS_EN,
	MSG_HOMEYZ_PROGRESS_CZ,
	MSG_HOMEYZ_PROGRESS_IT,
	MSG_HOMEYZ_PROGRESS_ES,
	MSG_HOMEYZ_PROGRESS_PL
};

const char MSG_HOTEND_OFFSET_EN[] PROGMEM = "Hotend offsets:";
const char MSG_HOTEND_OFFSET_CZ[] PROGMEM = "Hotend offsets:";
const char MSG_HOTEND_OFFSET_IT[] PROGMEM = "Hotend offsets:";
const char MSG_HOTEND_OFFSET_ES[] PROGMEM = "Hotend offsets:";
const char MSG_HOTEND_OFFSET_PL[] PROGMEM = "Hotend offsets:";
const char * const MSG_HOTEND_OFFSET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HOTEND_OFFSET_EN,
	MSG_HOTEND_OFFSET_CZ,
	MSG_HOTEND_OFFSET_IT,
	MSG_HOTEND_OFFSET_ES,
	MSG_HOTEND_OFFSET_PL
};

const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_EN[] PROGMEM = "Improving bed";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_CZ[] PROGMEM = "Zlepsuji presnost";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_IT[] PROGMEM = "Improving bed";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_ES[] PROGMEM = "Improving bed";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_PL[] PROGMEM = "Improving bed";
const char * const MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_EN,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_CZ,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_IT,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_ES,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_PL
};

const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_EN[] PROGMEM = "calibration point";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_CZ[] PROGMEM = "kalibracniho bodu";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_IT[] PROGMEM = "calibration point";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_ES[] PROGMEM = "calibration point";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_PL[] PROGMEM = "calibration point";
const char * const MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_EN,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_CZ,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_IT,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_ES,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_PL
};

const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE3_EN[] PROGMEM = " of 9";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE3_CZ[] PROGMEM = " z 9";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE3_IT[] PROGMEM = " of 9";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE3_ES[] PROGMEM = " of 9";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE3_PL[] PROGMEM = " of 9";
const char * const MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE3_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE3_EN,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE3_CZ,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE3_IT,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE3_ES,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE3_PL
};

const char MSG_INIT_SDCARD_EN[] PROGMEM = "Init. SD card";
const char MSG_INIT_SDCARD_CZ[] PROGMEM = "Inic. SD";
const char MSG_INIT_SDCARD_IT[] PROGMEM = "Init. SD card";
const char MSG_INIT_SDCARD_ES[] PROGMEM = "Init. SD card";
const char MSG_INIT_SDCARD_PL[] PROGMEM = "Inic. SD";
const char * const MSG_INIT_SDCARD_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INIT_SDCARD_EN,
	MSG_INIT_SDCARD_CZ,
	MSG_INIT_SDCARD_IT,
	MSG_INIT_SDCARD_ES,
	MSG_INIT_SDCARD_PL
};

const char MSG_INSERT_FILAMENT_EN[] PROGMEM = "Insert filament";
const char MSG_INSERT_FILAMENT_CZ[] PROGMEM = "Vlozte filament";
const char MSG_INSERT_FILAMENT_IT[] PROGMEM = "Inserire filamento";
const char MSG_INSERT_FILAMENT_ES[] PROGMEM = "Inserta filamento";
const char MSG_INSERT_FILAMENT_PL[] PROGMEM = "Wprowadz filament";
const char * const MSG_INSERT_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INSERT_FILAMENT_EN,
	MSG_INSERT_FILAMENT_CZ,
	MSG_INSERT_FILAMENT_IT,
	MSG_INSERT_FILAMENT_ES,
	MSG_INSERT_FILAMENT_PL
};

const char MSG_INVALID_EXTRUDER_EN[] PROGMEM = "Invalid extruder";
const char MSG_INVALID_EXTRUDER_CZ[] PROGMEM = "Invalid extruder";
const char MSG_INVALID_EXTRUDER_IT[] PROGMEM = "Invalid extruder";
const char MSG_INVALID_EXTRUDER_ES[] PROGMEM = "Invalid extruder";
const char MSG_INVALID_EXTRUDER_PL[] PROGMEM = "Invalid extruder";
const char * const MSG_INVALID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INVALID_EXTRUDER_EN,
	MSG_INVALID_EXTRUDER_CZ,
	MSG_INVALID_EXTRUDER_IT,
	MSG_INVALID_EXTRUDER_ES,
	MSG_INVALID_EXTRUDER_PL
};

const char MSG_KILLED_EN[] PROGMEM = "KILLED. ";
const char MSG_KILLED_CZ[] PROGMEM = "KILLED. ";
const char MSG_KILLED_IT[] PROGMEM = "UCCISO ";
const char MSG_KILLED_ES[] PROGMEM = "PARADA DE EMERG.";
const char MSG_KILLED_PL[] PROGMEM = "KILLED. ";
const char * const MSG_KILLED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_KILLED_EN,
	MSG_KILLED_CZ,
	MSG_KILLED_IT,
	MSG_KILLED_ES,
	MSG_KILLED_PL
};

const char MSG_LANGUAGE_NAME_EN[] PROGMEM = "English";
const char MSG_LANGUAGE_NAME_CZ[] PROGMEM = "Cestina";
const char MSG_LANGUAGE_NAME_IT[] PROGMEM = "Italiano";
const char MSG_LANGUAGE_NAME_ES[] PROGMEM = "Espanol";
const char MSG_LANGUAGE_NAME_PL[] PROGMEM = "Polski";
const char * const MSG_LANGUAGE_NAME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LANGUAGE_NAME_EN,
	MSG_LANGUAGE_NAME_CZ,
	MSG_LANGUAGE_NAME_IT,
	MSG_LANGUAGE_NAME_ES,
	MSG_LANGUAGE_NAME_PL
};

const char MSG_LANGUAGE_SELECT_EN[] PROGMEM = "Select language     ";
const char MSG_LANGUAGE_SELECT_CZ[] PROGMEM = "Vyber jazyka        ";
const char MSG_LANGUAGE_SELECT_IT[] PROGMEM = "Selez. la lingua";
const char MSG_LANGUAGE_SELECT_ES[] PROGMEM = "Cambia la lengua ";
const char MSG_LANGUAGE_SELECT_PL[] PROGMEM = "Wybor jezyka        ";
const char * const MSG_LANGUAGE_SELECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LANGUAGE_SELECT_EN,
	MSG_LANGUAGE_SELECT_CZ,
	MSG_LANGUAGE_SELECT_IT,
	MSG_LANGUAGE_SELECT_ES,
	MSG_LANGUAGE_SELECT_PL
};

const char MSG_LOADING_COLOR_EN[] PROGMEM = "Loading color";
const char MSG_LOADING_COLOR_CZ[] PROGMEM = "Cisteni barvy";
const char MSG_LOADING_COLOR_IT[] PROGMEM = "Cargando color";
const char MSG_LOADING_COLOR_ES[] PROGMEM = "Cargando color";
const char MSG_LOADING_COLOR_PL[] PROGMEM = "Czyszcz. koloru";
const char * const MSG_LOADING_COLOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOADING_COLOR_EN,
	MSG_LOADING_COLOR_CZ,
	MSG_LOADING_COLOR_IT,
	MSG_LOADING_COLOR_ES,
	MSG_LOADING_COLOR_PL
};

const char MSG_LOADING_FILAMENT_EN[] PROGMEM = "Loading filament";
const char MSG_LOADING_FILAMENT_CZ[] PROGMEM = "Zavadeni filamentu";
const char MSG_LOADING_FILAMENT_IT[] PROGMEM = "Cargando fil.";
const char MSG_LOADING_FILAMENT_ES[] PROGMEM = "Cargando fil.";
const char MSG_LOADING_FILAMENT_PL[] PROGMEM = "Wprow. filamentu";
const char * const MSG_LOADING_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOADING_FILAMENT_EN,
	MSG_LOADING_FILAMENT_CZ,
	MSG_LOADING_FILAMENT_IT,
	MSG_LOADING_FILAMENT_ES,
	MSG_LOADING_FILAMENT_PL
};

const char MSG_LOAD_EPROM_EN[] PROGMEM = "Load memory";
const char MSG_LOAD_EPROM_CZ[] PROGMEM = "Ulozit pamet";
const char MSG_LOAD_EPROM_IT[] PROGMEM = "Load memory";
const char MSG_LOAD_EPROM_ES[] PROGMEM = "Load memory";
const char MSG_LOAD_EPROM_PL[] PROGMEM = "Ulozit pamet";
const char * const MSG_LOAD_EPROM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_EPROM_EN,
	MSG_LOAD_EPROM_CZ,
	MSG_LOAD_EPROM_IT,
	MSG_LOAD_EPROM_ES,
	MSG_LOAD_EPROM_PL
};

const char MSG_LOAD_FILAMENT_EN[] PROGMEM = "Load filament";
const char MSG_LOAD_FILAMENT_CZ[] PROGMEM = "Zavest filament";
const char MSG_LOAD_FILAMENT_IT[] PROGMEM = "Caricare filamento";
const char MSG_LOAD_FILAMENT_ES[] PROGMEM = "Introducir filamento";
const char MSG_LOAD_FILAMENT_PL[] PROGMEM = "Wprowadz filament";
const char * const MSG_LOAD_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_EN,
	MSG_LOAD_FILAMENT_CZ,
	MSG_LOAD_FILAMENT_IT,
	MSG_LOAD_FILAMENT_ES,
	MSG_LOAD_FILAMENT_PL
};

const char MSG_M104_INVALID_EXTRUDER_EN[] PROGMEM = "M104 Invalid extruder ";
const char MSG_M104_INVALID_EXTRUDER_CZ[] PROGMEM = "M104 Invalid extruder ";
const char MSG_M104_INVALID_EXTRUDER_IT[] PROGMEM = "M104 Invalid extruder ";
const char MSG_M104_INVALID_EXTRUDER_ES[] PROGMEM = "M104 Invalid extruder ";
const char MSG_M104_INVALID_EXTRUDER_PL[] PROGMEM = "M104 Invalid extruder ";
const char * const MSG_M104_INVALID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M104_INVALID_EXTRUDER_EN,
	MSG_M104_INVALID_EXTRUDER_CZ,
	MSG_M104_INVALID_EXTRUDER_IT,
	MSG_M104_INVALID_EXTRUDER_ES,
	MSG_M104_INVALID_EXTRUDER_PL
};

const char MSG_M105_INVALID_EXTRUDER_EN[] PROGMEM = "M105 Invalid extruder ";
const char MSG_M105_INVALID_EXTRUDER_CZ[] PROGMEM = "M105 Invalid extruder ";
const char MSG_M105_INVALID_EXTRUDER_IT[] PROGMEM = "M105 Invalid extruder ";
const char MSG_M105_INVALID_EXTRUDER_ES[] PROGMEM = "M105 Invalid extruder ";
const char MSG_M105_INVALID_EXTRUDER_PL[] PROGMEM = "M105 Invalid extruder ";
const char * const MSG_M105_INVALID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M105_INVALID_EXTRUDER_EN,
	MSG_M105_INVALID_EXTRUDER_CZ,
	MSG_M105_INVALID_EXTRUDER_IT,
	MSG_M105_INVALID_EXTRUDER_ES,
	MSG_M105_INVALID_EXTRUDER_PL
};

const char MSG_M109_INVALID_EXTRUDER_EN[] PROGMEM = "M109 Invalid extruder ";
const char MSG_M109_INVALID_EXTRUDER_CZ[] PROGMEM = "M109 Invalid extruder ";
const char MSG_M109_INVALID_EXTRUDER_IT[] PROGMEM = "M109 Invalid extruder ";
const char MSG_M109_INVALID_EXTRUDER_ES[] PROGMEM = "M109 Invalid extruder ";
const char MSG_M109_INVALID_EXTRUDER_PL[] PROGMEM = "M109 Invalid extruder ";
const char * const MSG_M109_INVALID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M109_INVALID_EXTRUDER_EN,
	MSG_M109_INVALID_EXTRUDER_CZ,
	MSG_M109_INVALID_EXTRUDER_IT,
	MSG_M109_INVALID_EXTRUDER_ES,
	MSG_M109_INVALID_EXTRUDER_PL
};

const char MSG_M115_REPORT_EN[] PROGMEM = "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:https://github.com/prusa3d/Prusa-i3-Plus/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:" CUSTOM_MENDEL_NAME " EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000\n";
const char MSG_M115_REPORT_CZ[] PROGMEM = "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:https://github.com/prusa3d/Prusa-i3-Plus/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:" CUSTOM_MENDEL_NAME " EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000\n";
const char MSG_M115_REPORT_IT[] PROGMEM = "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:https://github.com/prusa3d/Prusa-i3-Plus/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:" CUSTOM_MENDEL_NAME " EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000\n";
const char MSG_M115_REPORT_ES[] PROGMEM = "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:https://github.com/prusa3d/Prusa-i3-Plus/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:" CUSTOM_MENDEL_NAME " EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000\n";
const char MSG_M115_REPORT_PL[] PROGMEM = "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:https://github.com/prusa3d/Prusa-i3-Plus/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:" CUSTOM_MENDEL_NAME " EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000\n";
const char * const MSG_M115_REPORT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M115_REPORT_EN,
	MSG_M115_REPORT_CZ,
	MSG_M115_REPORT_IT,
	MSG_M115_REPORT_ES,
	MSG_M115_REPORT_PL
};

const char MSG_M119_REPORT_EN[] PROGMEM = "Reporting endstop status";
const char MSG_M119_REPORT_CZ[] PROGMEM = "Reporting endstop status";
const char MSG_M119_REPORT_IT[] PROGMEM = "Reporting endstop status";
const char MSG_M119_REPORT_ES[] PROGMEM = "Reporting endstop status";
const char MSG_M119_REPORT_PL[] PROGMEM = "Reporting endstop status";
const char * const MSG_M119_REPORT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M119_REPORT_EN,
	MSG_M119_REPORT_CZ,
	MSG_M119_REPORT_IT,
	MSG_M119_REPORT_ES,
	MSG_M119_REPORT_PL
};

const char MSG_M200_INVALID_EXTRUDER_EN[] PROGMEM = "M200 Invalid extruder ";
const char MSG_M200_INVALID_EXTRUDER_CZ[] PROGMEM = "M200 Invalid extruder ";
const char MSG_M200_INVALID_EXTRUDER_IT[] PROGMEM = "M200 Invalid extruder ";
const char MSG_M200_INVALID_EXTRUDER_ES[] PROGMEM = "M200 Invalid extruder ";
const char MSG_M200_INVALID_EXTRUDER_PL[] PROGMEM = "M200 Invalid extruder ";
const char * const MSG_M200_INVALID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M200_INVALID_EXTRUDER_EN,
	MSG_M200_INVALID_EXTRUDER_CZ,
	MSG_M200_INVALID_EXTRUDER_IT,
	MSG_M200_INVALID_EXTRUDER_ES,
	MSG_M200_INVALID_EXTRUDER_PL
};

const char MSG_M218_INVALID_EXTRUDER_EN[] PROGMEM = "M218 Invalid extruder ";
const char MSG_M218_INVALID_EXTRUDER_CZ[] PROGMEM = "M218 Invalid extruder ";
const char MSG_M218_INVALID_EXTRUDER_IT[] PROGMEM = "M218 Invalid extruder ";
const char MSG_M218_INVALID_EXTRUDER_ES[] PROGMEM = "M218 Invalid extruder ";
const char MSG_M218_INVALID_EXTRUDER_PL[] PROGMEM = "M218 Invalid extruder ";
const char * const MSG_M218_INVALID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M218_INVALID_EXTRUDER_EN,
	MSG_M218_INVALID_EXTRUDER_CZ,
	MSG_M218_INVALID_EXTRUDER_IT,
	MSG_M218_INVALID_EXTRUDER_ES,
	MSG_M218_INVALID_EXTRUDER_PL
};

const char MSG_M221_INVALID_EXTRUDER_EN[] PROGMEM = "M221 Invalid extruder ";
const char MSG_M221_INVALID_EXTRUDER_CZ[] PROGMEM = "M221 Invalid extruder ";
const char MSG_M221_INVALID_EXTRUDER_IT[] PROGMEM = "M221 Invalid extruder ";
const char MSG_M221_INVALID_EXTRUDER_ES[] PROGMEM = "M221 Invalid extruder ";
const char MSG_M221_INVALID_EXTRUDER_PL[] PROGMEM = "M221 Invalid extruder ";
const char * const MSG_M221_INVALID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M221_INVALID_EXTRUDER_EN,
	MSG_M221_INVALID_EXTRUDER_CZ,
	MSG_M221_INVALID_EXTRUDER_IT,
	MSG_M221_INVALID_EXTRUDER_ES,
	MSG_M221_INVALID_EXTRUDER_PL
};

const char MSG_MAIN_EN[] PROGMEM = "Main";
const char MSG_MAIN_CZ[] PROGMEM = "Hlavni nabidka";
const char MSG_MAIN_IT[] PROGMEM = "Menu principale";
const char MSG_MAIN_ES[] PROGMEM = "Menu principal";
const char MSG_MAIN_PL[] PROGMEM = "Menu glowne";
const char * const MSG_MAIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MAIN_EN,
	MSG_MAIN_CZ,
	MSG_MAIN_IT,
	MSG_MAIN_ES,
	MSG_MAIN_PL
};

const char MSG_MAX_EN[] PROGMEM = " \002 Max";
const char MSG_MAX_CZ[] PROGMEM = " \002 Max";
const char MSG_MAX_IT[] PROGMEM = " \002 Max";
const char MSG_MAX_ES[] PROGMEM = " \002 Max";
const char MSG_MAX_PL[] PROGMEM = " \002 Max";
const char * const MSG_MAX_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MAX_EN,
	MSG_MAX_CZ,
	MSG_MAX_IT,
	MSG_MAX_ES,
	MSG_MAX_PL
};

const char MSG_MIN_EN[] PROGMEM = " \002 Min";
const char MSG_MIN_CZ[] PROGMEM = " \002 Min";
const char MSG_MIN_IT[] PROGMEM = " \002 Min";
const char MSG_MIN_ES[] PROGMEM = " \002 Min";
const char MSG_MIN_PL[] PROGMEM = " \002 Min";
const char * const MSG_MIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MIN_EN,
	MSG_MIN_CZ,
	MSG_MIN_IT,
	MSG_MIN_ES,
	MSG_MIN_PL
};

const char MSG_MOTION_EN[] PROGMEM = "Motion";
const char MSG_MOTION_CZ[] PROGMEM = "Pohyb";
const char MSG_MOTION_IT[] PROGMEM = "Motion";
const char MSG_MOTION_ES[] PROGMEM = "Motion";
const char MSG_MOTION_PL[] PROGMEM = "Pohyb";
const char * const MSG_MOTION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOTION_EN,
	MSG_MOTION_CZ,
	MSG_MOTION_IT,
	MSG_MOTION_ES,
	MSG_MOTION_PL
};

const char MSG_MOVE_01MM_EN[] PROGMEM = "Move 0.1mm";
const char MSG_MOVE_01MM_CZ[] PROGMEM = "Posunout o 0.1mm";
const char MSG_MOVE_01MM_IT[] PROGMEM = "Move 0.1mm";
const char MSG_MOVE_01MM_ES[] PROGMEM = "Move 0.1mm";
const char MSG_MOVE_01MM_PL[] PROGMEM = "Posunout o 0.1mm";
const char * const MSG_MOVE_01MM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_01MM_EN,
	MSG_MOVE_01MM_CZ,
	MSG_MOVE_01MM_IT,
	MSG_MOVE_01MM_ES,
	MSG_MOVE_01MM_PL
};

const char MSG_MOVE_10MM_EN[] PROGMEM = "Move 10mm";
const char MSG_MOVE_10MM_CZ[] PROGMEM = "Posunout o 10mm";
const char MSG_MOVE_10MM_IT[] PROGMEM = "Move 10mm";
const char MSG_MOVE_10MM_ES[] PROGMEM = "Move 10mm";
const char MSG_MOVE_10MM_PL[] PROGMEM = "Posunout o 10mm";
const char * const MSG_MOVE_10MM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_10MM_EN,
	MSG_MOVE_10MM_CZ,
	MSG_MOVE_10MM_IT,
	MSG_MOVE_10MM_ES,
	MSG_MOVE_10MM_PL
};

const char MSG_MOVE_1MM_EN[] PROGMEM = "Move 1mm";
const char MSG_MOVE_1MM_CZ[] PROGMEM = "Posunout o 1mm";
const char MSG_MOVE_1MM_IT[] PROGMEM = "Move 1mm";
const char MSG_MOVE_1MM_ES[] PROGMEM = "Move 1mm";
const char MSG_MOVE_1MM_PL[] PROGMEM = "Posunout o 1mm";
const char * const MSG_MOVE_1MM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_1MM_EN,
	MSG_MOVE_1MM_CZ,
	MSG_MOVE_1MM_IT,
	MSG_MOVE_1MM_ES,
	MSG_MOVE_1MM_PL
};

const char MSG_MOVE_AXIS_EN[] PROGMEM = "Move axis";
const char MSG_MOVE_AXIS_CZ[] PROGMEM = "Posunout osu";
const char MSG_MOVE_AXIS_IT[] PROGMEM = "Muovi Asse";
const char MSG_MOVE_AXIS_ES[] PROGMEM = "Mover ejes";
const char MSG_MOVE_AXIS_PL[] PROGMEM = "Ruch osi";
const char * const MSG_MOVE_AXIS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_AXIS_EN,
	MSG_MOVE_AXIS_CZ,
	MSG_MOVE_AXIS_IT,
	MSG_MOVE_AXIS_ES,
	MSG_MOVE_AXIS_PL
};

const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE1_EN[] PROGMEM = "Calibrating X/Y.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE1_CZ[] PROGMEM = "Kalibrace X/Y";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE1_IT[] PROGMEM = "Calibrating X/Y.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE1_ES[] PROGMEM = "Calibrating X/Y.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE1_PL[] PROGMEM = "Calibrating X/Y.";
const char * const MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE1_EN,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE1_CZ,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE1_IT,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE1_ES,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE1_PL
};

const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE2_EN[] PROGMEM = "Move Z carriage up";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE2_CZ[] PROGMEM = "Posunte prosim Z osu";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE2_IT[] PROGMEM = "Move Z carriage up";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE2_ES[] PROGMEM = "Move Z carriage up";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE2_PL[] PROGMEM = "Move Z carriage up";
const char * const MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE2_EN,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE2_CZ,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE2_IT,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE2_ES,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE2_PL
};

const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE3_EN[] PROGMEM = "to the end stoppers.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE3_CZ[] PROGMEM = "az k hornimu dorazu.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE3_IT[] PROGMEM = "to the end stoppers.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE3_ES[] PROGMEM = "to the end stoppers.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE3_PL[] PROGMEM = "to the end stoppers.";
const char * const MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE3_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE3_EN,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE3_CZ,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE3_IT,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE3_ES,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE3_PL
};

const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE4_EN[] PROGMEM = "Click when done.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE4_CZ[] PROGMEM = "Potvrdte tlacitkem.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE4_IT[] PROGMEM = "Click when done.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE4_ES[] PROGMEM = "Click when done.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE4_PL[] PROGMEM = "Click when done.";
const char * const MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE4_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE4_EN,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE4_CZ,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE4_IT,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE4_ES,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_LINE4_PL
};

const char MSG_MOVE_E_EN[] PROGMEM = "Extruder";
const char MSG_MOVE_E_CZ[] PROGMEM = "Extruder";
const char MSG_MOVE_E_IT[] PROGMEM = "Estrusore";
const char MSG_MOVE_E_ES[] PROGMEM = "Extrusor";
const char MSG_MOVE_E_PL[] PROGMEM = "Extruder";
const char * const MSG_MOVE_E_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_E_EN,
	MSG_MOVE_E_CZ,
	MSG_MOVE_E_IT,
	MSG_MOVE_E_ES,
	MSG_MOVE_E_PL
};

const char MSG_MOVE_E1_EN[] PROGMEM = "Extruder2";
const char MSG_MOVE_E1_CZ[] PROGMEM = "Extruder2";
const char MSG_MOVE_E1_IT[] PROGMEM = "Extruder2";
const char MSG_MOVE_E1_ES[] PROGMEM = "Extruder2";
const char MSG_MOVE_E1_PL[] PROGMEM = "Extruder2";
const char * const MSG_MOVE_E1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_E1_EN,
	MSG_MOVE_E1_CZ,
	MSG_MOVE_E1_IT,
	MSG_MOVE_E1_ES,
	MSG_MOVE_E1_PL
};

const char MSG_MOVE_E2_EN[] PROGMEM = "Extruder3";
const char MSG_MOVE_E2_CZ[] PROGMEM = "Extruder3";
const char MSG_MOVE_E2_IT[] PROGMEM = "Extruder3";
const char MSG_MOVE_E2_ES[] PROGMEM = "Extruder3";
const char MSG_MOVE_E2_PL[] PROGMEM = "Extruder3";
const char * const MSG_MOVE_E2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_E2_EN,
	MSG_MOVE_E2_CZ,
	MSG_MOVE_E2_IT,
	MSG_MOVE_E2_ES,
	MSG_MOVE_E2_PL
};

const char MSG_MOVE_X_EN[] PROGMEM = "Move X";
const char MSG_MOVE_X_CZ[] PROGMEM = "Posunout X";
const char MSG_MOVE_X_IT[] PROGMEM = "Muovi X";
const char MSG_MOVE_X_ES[] PROGMEM = "Mover X";
const char MSG_MOVE_X_PL[] PROGMEM = "Przesunac X";
const char * const MSG_MOVE_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_X_EN,
	MSG_MOVE_X_CZ,
	MSG_MOVE_X_IT,
	MSG_MOVE_X_ES,
	MSG_MOVE_X_PL
};

const char MSG_MOVE_Y_EN[] PROGMEM = "Move Y";
const char MSG_MOVE_Y_CZ[] PROGMEM = "Posunout Y";
const char MSG_MOVE_Y_IT[] PROGMEM = "Muovi Y";
const char MSG_MOVE_Y_ES[] PROGMEM = "Mover Y";
const char MSG_MOVE_Y_PL[] PROGMEM = "Przesunac Y";
const char * const MSG_MOVE_Y_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_Y_EN,
	MSG_MOVE_Y_CZ,
	MSG_MOVE_Y_IT,
	MSG_MOVE_Y_ES,
	MSG_MOVE_Y_PL
};

const char MSG_MOVE_Z_EN[] PROGMEM = "Move Z";
const char MSG_MOVE_Z_CZ[] PROGMEM = "Posunout Z";
const char MSG_MOVE_Z_IT[] PROGMEM = "Muovi Z";
const char MSG_MOVE_Z_ES[] PROGMEM = "Mover Z";
const char MSG_MOVE_Z_PL[] PROGMEM = "Przesunac Z";
const char * const MSG_MOVE_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_Z_EN,
	MSG_MOVE_Z_CZ,
	MSG_MOVE_Z_IT,
	MSG_MOVE_Z_ES,
	MSG_MOVE_Z_PL
};

const char MSG_NO_EN[] PROGMEM = "No";
const char MSG_NO_CZ[] PROGMEM = "Ne";
const char MSG_NO_IT[] PROGMEM = "No";
const char MSG_NO_ES[] PROGMEM = "No";
const char MSG_NO_PL[] PROGMEM = "Nie";
const char * const MSG_NO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NO_EN,
	MSG_NO_CZ,
	MSG_NO_IT,
	MSG_NO_ES,
	MSG_NO_PL
};

const char MSG_NOT_COLOR_EN[] PROGMEM = "Color not clear";
const char MSG_NOT_COLOR_CZ[] PROGMEM = "Barva neni cista";
const char MSG_NOT_COLOR_IT[] PROGMEM = "Color no claro";
const char MSG_NOT_COLOR_ES[] PROGMEM = "Color no claro";
const char MSG_NOT_COLOR_PL[] PROGMEM = "Kolor zanieczysz.";
const char * const MSG_NOT_COLOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOT_COLOR_EN,
	MSG_NOT_COLOR_CZ,
	MSG_NOT_COLOR_IT,
	MSG_NOT_COLOR_ES,
	MSG_NOT_COLOR_PL
};

const char MSG_NOT_LOADED_EN[] PROGMEM = "Filament not loaded";
const char MSG_NOT_LOADED_CZ[] PROGMEM = "Filament nezaveden";
const char MSG_NOT_LOADED_IT[] PROGMEM = "Fil. no cargado";
const char MSG_NOT_LOADED_ES[] PROGMEM = "Fil. no cargado";
const char MSG_NOT_LOADED_PL[] PROGMEM = "Brak filamentu";
const char * const MSG_NOT_LOADED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOT_LOADED_EN,
	MSG_NOT_LOADED_CZ,
	MSG_NOT_LOADED_IT,
	MSG_NOT_LOADED_ES,
	MSG_NOT_LOADED_PL
};

const char MSG_NOZZLE_EN[] PROGMEM = "Nozzle";
const char MSG_NOZZLE_CZ[] PROGMEM = "Tryska";
const char MSG_NOZZLE_IT[] PROGMEM = "Ugello";
const char MSG_NOZZLE_ES[] PROGMEM = "Fusor";
const char MSG_NOZZLE_PL[] PROGMEM = "Dysza";
const char * const MSG_NOZZLE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOZZLE_EN,
	MSG_NOZZLE_CZ,
	MSG_NOZZLE_IT,
	MSG_NOZZLE_ES,
	MSG_NOZZLE_PL
};

const char MSG_NOZZLE1_EN[] PROGMEM = "Nozzle2";
const char MSG_NOZZLE1_CZ[] PROGMEM = "Tryska2";
const char MSG_NOZZLE1_IT[] PROGMEM = "Nozzle2";
const char MSG_NOZZLE1_ES[] PROGMEM = "Nozzle2";
const char MSG_NOZZLE1_PL[] PROGMEM = "Tryska2";
const char * const MSG_NOZZLE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOZZLE1_EN,
	MSG_NOZZLE1_CZ,
	MSG_NOZZLE1_IT,
	MSG_NOZZLE1_ES,
	MSG_NOZZLE1_PL
};

const char MSG_NOZZLE2_EN[] PROGMEM = "Nozzle3";
const char MSG_NOZZLE2_CZ[] PROGMEM = "Tryska3";
const char MSG_NOZZLE2_IT[] PROGMEM = "Nozzle3";
const char MSG_NOZZLE2_ES[] PROGMEM = "Nozzle3";
const char MSG_NOZZLE2_PL[] PROGMEM = "Tryska3";
const char * const MSG_NOZZLE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOZZLE2_EN,
	MSG_NOZZLE2_CZ,
	MSG_NOZZLE2_IT,
	MSG_NOZZLE2_ES,
	MSG_NOZZLE2_PL
};

const char MSG_NO_CARD_EN[] PROGMEM = "No SD card";
const char MSG_NO_CARD_CZ[] PROGMEM = "Zadna SD karta";
const char MSG_NO_CARD_IT[] PROGMEM = "No SD Carta";
const char MSG_NO_CARD_ES[] PROGMEM = "No hay tarjeta SD";
const char MSG_NO_CARD_PL[] PROGMEM = "Brak karty SD";
const char * const MSG_NO_CARD_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NO_CARD_EN,
	MSG_NO_CARD_CZ,
	MSG_NO_CARD_IT,
	MSG_NO_CARD_ES,
	MSG_NO_CARD_PL
};

const char MSG_NO_MOVE_EN[] PROGMEM = "No move.";
const char MSG_NO_MOVE_CZ[] PROGMEM = "No move.";
const char MSG_NO_MOVE_IT[] PROGMEM = "Nessun Movimento";
const char MSG_NO_MOVE_ES[] PROGMEM = "Sin movimiento";
const char MSG_NO_MOVE_PL[] PROGMEM = "No move.";
const char * const MSG_NO_MOVE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NO_MOVE_EN,
	MSG_NO_MOVE_CZ,
	MSG_NO_MOVE_IT,
	MSG_NO_MOVE_ES,
	MSG_NO_MOVE_PL
};

const char MSG_OFF_EN[] PROGMEM = "Off";
const char MSG_OFF_CZ[] PROGMEM = "Off";
const char MSG_OFF_IT[] PROGMEM = "Off";
const char MSG_OFF_ES[] PROGMEM = "Off";
const char MSG_OFF_PL[] PROGMEM = "Off";
const char * const MSG_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_OFF_EN,
	MSG_OFF_CZ,
	MSG_OFF_IT,
	MSG_OFF_ES,
	MSG_OFF_PL
};

const char MSG_OK_EN[] PROGMEM = "ok";
const char MSG_OK_CZ[] PROGMEM = "ok";
const char MSG_OK_IT[] PROGMEM = "ok";
const char MSG_OK_ES[] PROGMEM = "ok";
const char MSG_OK_PL[] PROGMEM = "ok";
const char * const MSG_OK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_OK_EN,
	MSG_OK_CZ,
	MSG_OK_IT,
	MSG_OK_ES,
	MSG_OK_PL
};

const char MSG_ON_EN[] PROGMEM = "On ";
const char MSG_ON_CZ[] PROGMEM = "On ";
const char MSG_ON_IT[] PROGMEM = "On ";
const char MSG_ON_ES[] PROGMEM = "On ";
const char MSG_ON_PL[] PROGMEM = "On ";
const char * const MSG_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ON_EN,
	MSG_ON_CZ,
	MSG_ON_IT,
	MSG_ON_ES,
	MSG_ON_PL
};

const char MSG_PAUSE_PRINT_EN[] PROGMEM = "Pause print";
const char MSG_PAUSE_PRINT_CZ[] PROGMEM = "Pozastavit tisk";
const char MSG_PAUSE_PRINT_IT[] PROGMEM = "Pausa";
const char MSG_PAUSE_PRINT_ES[] PROGMEM = "Pausar impresion";
const char MSG_PAUSE_PRINT_PL[] PROGMEM = "Przerwac druk";
const char * const MSG_PAUSE_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PAUSE_PRINT_EN,
	MSG_PAUSE_PRINT_CZ,
	MSG_PAUSE_PRINT_IT,
	MSG_PAUSE_PRINT_ES,
	MSG_PAUSE_PRINT_PL
};

const char MSG_PICK_Z_EN[] PROGMEM = "Pick print";
const char MSG_PICK_Z_CZ[] PROGMEM = "Vyberte vytisk";
const char MSG_PICK_Z_IT[] PROGMEM = "Vyberte vytisk";
const char MSG_PICK_Z_ES[] PROGMEM = "Vyberte vytisk";
const char MSG_PICK_Z_PL[] PROGMEM = "Vyberte vytisk";
const char * const MSG_PICK_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PICK_Z_EN,
	MSG_PICK_Z_CZ,
	MSG_PICK_Z_IT,
	MSG_PICK_Z_ES,
	MSG_PICK_Z_PL
};

const char MSG_PID_C_EN[] PROGMEM = "PID-C";
const char MSG_PID_C_CZ[] PROGMEM = "PID-C";
const char MSG_PID_C_IT[] PROGMEM = "PID-C";
const char MSG_PID_C_ES[] PROGMEM = "PID-C";
const char MSG_PID_C_PL[] PROGMEM = "PID-C";
const char * const MSG_PID_C_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PID_C_EN,
	MSG_PID_C_CZ,
	MSG_PID_C_IT,
	MSG_PID_C_ES,
	MSG_PID_C_PL
};

const char MSG_PID_D_EN[] PROGMEM = "PID-D";
const char MSG_PID_D_CZ[] PROGMEM = "PID-D";
const char MSG_PID_D_IT[] PROGMEM = "PID-D";
const char MSG_PID_D_ES[] PROGMEM = "PID-D";
const char MSG_PID_D_PL[] PROGMEM = "PID-D";
const char * const MSG_PID_D_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PID_D_EN,
	MSG_PID_D_CZ,
	MSG_PID_D_IT,
	MSG_PID_D_ES,
	MSG_PID_D_PL
};

const char MSG_PID_I_EN[] PROGMEM = "PID-I";
const char MSG_PID_I_CZ[] PROGMEM = "PID-I";
const char MSG_PID_I_IT[] PROGMEM = "PID-I";
const char MSG_PID_I_ES[] PROGMEM = "PID-I";
const char MSG_PID_I_PL[] PROGMEM = "PID-I";
const char * const MSG_PID_I_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PID_I_EN,
	MSG_PID_I_CZ,
	MSG_PID_I_IT,
	MSG_PID_I_ES,
	MSG_PID_I_PL
};

const char MSG_PID_P_EN[] PROGMEM = "PID-P";
const char MSG_PID_P_CZ[] PROGMEM = "PID-P";
const char MSG_PID_P_IT[] PROGMEM = "PID-P";
const char MSG_PID_P_ES[] PROGMEM = "PID-P";
const char MSG_PID_P_PL[] PROGMEM = "PID-P";
const char * const MSG_PID_P_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PID_P_EN,
	MSG_PID_P_CZ,
	MSG_PID_P_IT,
	MSG_PID_P_ES,
	MSG_PID_P_PL
};

const char MSG_PLANNER_BUFFER_BYTES_EN[] PROGMEM = "  PlannerBufferBytes: ";
const char MSG_PLANNER_BUFFER_BYTES_CZ[] PROGMEM = "  PlannerBufferBytes: ";
const char MSG_PLANNER_BUFFER_BYTES_IT[] PROGMEM = "  PlannerBufferBytes: ";
const char MSG_PLANNER_BUFFER_BYTES_ES[] PROGMEM = "  PlannerBufferBytes: ";
const char MSG_PLANNER_BUFFER_BYTES_PL[] PROGMEM = "  PlannerBufferBytes: ";
const char * const MSG_PLANNER_BUFFER_BYTES_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PLANNER_BUFFER_BYTES_EN,
	MSG_PLANNER_BUFFER_BYTES_CZ,
	MSG_PLANNER_BUFFER_BYTES_IT,
	MSG_PLANNER_BUFFER_BYTES_ES,
	MSG_PLANNER_BUFFER_BYTES_PL
};

const char MSG_PLEASE_WAIT_EN[] PROGMEM = "Please wait";
const char MSG_PLEASE_WAIT_CZ[] PROGMEM = "Prosim cekejte";
const char MSG_PLEASE_WAIT_IT[] PROGMEM = "Aspetta";
const char MSG_PLEASE_WAIT_ES[] PROGMEM = "Espera";
const char MSG_PLEASE_WAIT_PL[] PROGMEM = "Prosze czekac";
const char * const MSG_PLEASE_WAIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PLEASE_WAIT_EN,
	MSG_PLEASE_WAIT_CZ,
	MSG_PLEASE_WAIT_IT,
	MSG_PLEASE_WAIT_ES,
	MSG_PLEASE_WAIT_PL
};

const char MSG_POSITION_UNKNOWN_EN[] PROGMEM = "Home X/Y before Z";
const char MSG_POSITION_UNKNOWN_CZ[] PROGMEM = "Home X/Y before Z";
const char MSG_POSITION_UNKNOWN_IT[] PROGMEM = "Home X/Y before Z";
const char MSG_POSITION_UNKNOWN_ES[] PROGMEM = "Home X/Y before Z";
const char MSG_POSITION_UNKNOWN_PL[] PROGMEM = "Home X/Y before Z";
const char * const MSG_POSITION_UNKNOWN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_POSITION_UNKNOWN_EN,
	MSG_POSITION_UNKNOWN_CZ,
	MSG_POSITION_UNKNOWN_IT,
	MSG_POSITION_UNKNOWN_ES,
	MSG_POSITION_UNKNOWN_PL
};

const char MSG_POWERUP_EN[] PROGMEM = "PowerUp";
const char MSG_POWERUP_CZ[] PROGMEM = "PowerUp";
const char MSG_POWERUP_IT[] PROGMEM = "PowerUp";
const char MSG_POWERUP_ES[] PROGMEM = "PowerUp";
const char MSG_POWERUP_PL[] PROGMEM = "PowerUp";
const char * const MSG_POWERUP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_POWERUP_EN,
	MSG_POWERUP_CZ,
	MSG_POWERUP_IT,
	MSG_POWERUP_ES,
	MSG_POWERUP_PL
};

const char MSG_PREHEAT_EN[] PROGMEM = "Preheat";
const char MSG_PREHEAT_CZ[] PROGMEM = "Predehrev";
const char MSG_PREHEAT_IT[] PROGMEM = "Preriscalda";
const char MSG_PREHEAT_ES[] PROGMEM = "Precalentar";
const char MSG_PREHEAT_PL[] PROGMEM = "Grzanie";
const char * const MSG_PREHEAT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_EN,
	MSG_PREHEAT_CZ,
	MSG_PREHEAT_IT,
	MSG_PREHEAT_ES,
	MSG_PREHEAT_PL
};

const char MSG_PREHEAT_ABS_EN[] PROGMEM = "Preheat ABS";
const char MSG_PREHEAT_ABS_CZ[] PROGMEM = "Predehrev ABS";
const char MSG_PREHEAT_ABS_IT[] PROGMEM = "Preheat ABS";
const char MSG_PREHEAT_ABS_ES[] PROGMEM = "Preheat ABS";
const char MSG_PREHEAT_ABS_PL[] PROGMEM = "Predehrev ABS";
const char * const MSG_PREHEAT_ABS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_ABS_EN,
	MSG_PREHEAT_ABS_CZ,
	MSG_PREHEAT_ABS_IT,
	MSG_PREHEAT_ABS_ES,
	MSG_PREHEAT_ABS_PL
};

const char MSG_PREHEAT_ABS0_EN[] PROGMEM = "Preheat ABS 1";
const char MSG_PREHEAT_ABS0_CZ[] PROGMEM = "Predehrev ABS 1";
const char MSG_PREHEAT_ABS0_IT[] PROGMEM = "Preheat ABS 1";
const char MSG_PREHEAT_ABS0_ES[] PROGMEM = "Preheat ABS 1";
const char MSG_PREHEAT_ABS0_PL[] PROGMEM = "Predehrev ABS 1";
const char * const MSG_PREHEAT_ABS0_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_ABS0_EN,
	MSG_PREHEAT_ABS0_CZ,
	MSG_PREHEAT_ABS0_IT,
	MSG_PREHEAT_ABS0_ES,
	MSG_PREHEAT_ABS0_PL
};

const char MSG_PREHEAT_ABS012_EN[] PROGMEM = "Preheat ABS All";
const char MSG_PREHEAT_ABS012_CZ[] PROGMEM = "Predehrev ABS All";
const char MSG_PREHEAT_ABS012_IT[] PROGMEM = "Preheat ABS All";
const char MSG_PREHEAT_ABS012_ES[] PROGMEM = "Preheat ABS All";
const char MSG_PREHEAT_ABS012_PL[] PROGMEM = "Predehrev ABS All";
const char * const MSG_PREHEAT_ABS012_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_ABS012_EN,
	MSG_PREHEAT_ABS012_CZ,
	MSG_PREHEAT_ABS012_IT,
	MSG_PREHEAT_ABS012_ES,
	MSG_PREHEAT_ABS012_PL
};

const char MSG_PREHEAT_ABS1_EN[] PROGMEM = "Preheat ABS 2";
const char MSG_PREHEAT_ABS1_CZ[] PROGMEM = "Predehrev ABS 2";
const char MSG_PREHEAT_ABS1_IT[] PROGMEM = "Preheat ABS 2";
const char MSG_PREHEAT_ABS1_ES[] PROGMEM = "Preheat ABS 2";
const char MSG_PREHEAT_ABS1_PL[] PROGMEM = "Predehrev ABS 2";
const char * const MSG_PREHEAT_ABS1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_ABS1_EN,
	MSG_PREHEAT_ABS1_CZ,
	MSG_PREHEAT_ABS1_IT,
	MSG_PREHEAT_ABS1_ES,
	MSG_PREHEAT_ABS1_PL
};

const char MSG_PREHEAT_ABS2_EN[] PROGMEM = "Preheat ABS 3";
const char MSG_PREHEAT_ABS2_CZ[] PROGMEM = "Predehrev ABS 3";
const char MSG_PREHEAT_ABS2_IT[] PROGMEM = "Preheat ABS 3";
const char MSG_PREHEAT_ABS2_ES[] PROGMEM = "Preheat ABS 3";
const char MSG_PREHEAT_ABS2_PL[] PROGMEM = "Predehrev ABS 3";
const char * const MSG_PREHEAT_ABS2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_ABS2_EN,
	MSG_PREHEAT_ABS2_CZ,
	MSG_PREHEAT_ABS2_IT,
	MSG_PREHEAT_ABS2_ES,
	MSG_PREHEAT_ABS2_PL
};

const char MSG_PREHEAT_ABS_BEDONLY_EN[] PROGMEM = "Preheat ABS Bed";
const char MSG_PREHEAT_ABS_BEDONLY_CZ[] PROGMEM = "Predehrev ABS Bed";
const char MSG_PREHEAT_ABS_BEDONLY_IT[] PROGMEM = "Preheat ABS Bed";
const char MSG_PREHEAT_ABS_BEDONLY_ES[] PROGMEM = "Preheat ABS Bed";
const char MSG_PREHEAT_ABS_BEDONLY_PL[] PROGMEM = "Predehrev ABS Bed";
const char * const MSG_PREHEAT_ABS_BEDONLY_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_ABS_BEDONLY_EN,
	MSG_PREHEAT_ABS_BEDONLY_CZ,
	MSG_PREHEAT_ABS_BEDONLY_IT,
	MSG_PREHEAT_ABS_BEDONLY_ES,
	MSG_PREHEAT_ABS_BEDONLY_PL
};

const char MSG_PREHEAT_ABS_SETTINGS_EN[] PROGMEM = "Preheat ABS conf";
const char MSG_PREHEAT_ABS_SETTINGS_CZ[] PROGMEM = "Predehrev ABS conf";
const char MSG_PREHEAT_ABS_SETTINGS_IT[] PROGMEM = "Preheat ABS conf";
const char MSG_PREHEAT_ABS_SETTINGS_ES[] PROGMEM = "Preheat ABS conf";
const char MSG_PREHEAT_ABS_SETTINGS_PL[] PROGMEM = "Predehrev ABS conf";
const char * const MSG_PREHEAT_ABS_SETTINGS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_ABS_SETTINGS_EN,
	MSG_PREHEAT_ABS_SETTINGS_CZ,
	MSG_PREHEAT_ABS_SETTINGS_IT,
	MSG_PREHEAT_ABS_SETTINGS_ES,
	MSG_PREHEAT_ABS_SETTINGS_PL
};

const char MSG_PREHEAT_NOZZLE_EN[] PROGMEM = "Preheat the nozzle!";
const char MSG_PREHEAT_NOZZLE_CZ[] PROGMEM = "Predehrejte trysku!";
const char MSG_PREHEAT_NOZZLE_IT[] PROGMEM = "Preris. ugello!";
const char MSG_PREHEAT_NOZZLE_ES[] PROGMEM = "Precal. extrusor!";
const char MSG_PREHEAT_NOZZLE_PL[] PROGMEM = "Nagrzej dysze!";
const char * const MSG_PREHEAT_NOZZLE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_NOZZLE_EN,
	MSG_PREHEAT_NOZZLE_CZ,
	MSG_PREHEAT_NOZZLE_IT,
	MSG_PREHEAT_NOZZLE_ES,
	MSG_PREHEAT_NOZZLE_PL
};

const char MSG_PREHEAT_PLA_EN[] PROGMEM = "Preheat PLA";
const char MSG_PREHEAT_PLA_CZ[] PROGMEM = "Predehrev PLA";
const char MSG_PREHEAT_PLA_IT[] PROGMEM = "Preheat PLA";
const char MSG_PREHEAT_PLA_ES[] PROGMEM = "Preheat PLA";
const char MSG_PREHEAT_PLA_PL[] PROGMEM = "Predehrev PLA";
const char * const MSG_PREHEAT_PLA_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_PLA_EN,
	MSG_PREHEAT_PLA_CZ,
	MSG_PREHEAT_PLA_IT,
	MSG_PREHEAT_PLA_ES,
	MSG_PREHEAT_PLA_PL
};

const char MSG_PREHEAT_PLA0_EN[] PROGMEM = "Preheat PLA 1";
const char MSG_PREHEAT_PLA0_CZ[] PROGMEM = "Predehrev PLA 1";
const char MSG_PREHEAT_PLA0_IT[] PROGMEM = "Preheat PLA 1";
const char MSG_PREHEAT_PLA0_ES[] PROGMEM = "Preheat PLA 1";
const char MSG_PREHEAT_PLA0_PL[] PROGMEM = "Predehrev PLA 1";
const char * const MSG_PREHEAT_PLA0_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_PLA0_EN,
	MSG_PREHEAT_PLA0_CZ,
	MSG_PREHEAT_PLA0_IT,
	MSG_PREHEAT_PLA0_ES,
	MSG_PREHEAT_PLA0_PL
};

const char MSG_PREHEAT_PLA012_EN[] PROGMEM = "Preheat PLA All";
const char MSG_PREHEAT_PLA012_CZ[] PROGMEM = "Predehrev PLA All";
const char MSG_PREHEAT_PLA012_IT[] PROGMEM = "Preheat PLA All";
const char MSG_PREHEAT_PLA012_ES[] PROGMEM = "Preheat PLA All";
const char MSG_PREHEAT_PLA012_PL[] PROGMEM = "Predehrev PLA All";
const char * const MSG_PREHEAT_PLA012_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_PLA012_EN,
	MSG_PREHEAT_PLA012_CZ,
	MSG_PREHEAT_PLA012_IT,
	MSG_PREHEAT_PLA012_ES,
	MSG_PREHEAT_PLA012_PL
};

const char MSG_PREHEAT_PLA1_EN[] PROGMEM = "Preheat PLA 2";
const char MSG_PREHEAT_PLA1_CZ[] PROGMEM = "Predehrev PLA 2";
const char MSG_PREHEAT_PLA1_IT[] PROGMEM = "Preheat PLA 2";
const char MSG_PREHEAT_PLA1_ES[] PROGMEM = "Preheat PLA 2";
const char MSG_PREHEAT_PLA1_PL[] PROGMEM = "Predehrev PLA 2";
const char * const MSG_PREHEAT_PLA1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_PLA1_EN,
	MSG_PREHEAT_PLA1_CZ,
	MSG_PREHEAT_PLA1_IT,
	MSG_PREHEAT_PLA1_ES,
	MSG_PREHEAT_PLA1_PL
};

const char MSG_PREHEAT_PLA2_EN[] PROGMEM = "Preheat PLA 3";
const char MSG_PREHEAT_PLA2_CZ[] PROGMEM = "Predehrev PLA 3";
const char MSG_PREHEAT_PLA2_IT[] PROGMEM = "Preheat PLA 3";
const char MSG_PREHEAT_PLA2_ES[] PROGMEM = "Preheat PLA 3";
const char MSG_PREHEAT_PLA2_PL[] PROGMEM = "Predehrev PLA 3";
const char * const MSG_PREHEAT_PLA2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_PLA2_EN,
	MSG_PREHEAT_PLA2_CZ,
	MSG_PREHEAT_PLA2_IT,
	MSG_PREHEAT_PLA2_ES,
	MSG_PREHEAT_PLA2_PL
};

const char MSG_PREHEAT_PLA_BEDONLY_EN[] PROGMEM = "Preheat PLA Bed";
const char MSG_PREHEAT_PLA_BEDONLY_CZ[] PROGMEM = "Predehrev PLA Bed";
const char MSG_PREHEAT_PLA_BEDONLY_IT[] PROGMEM = "Preheat PLA Bed";
const char MSG_PREHEAT_PLA_BEDONLY_ES[] PROGMEM = "Preheat PLA Bed";
const char MSG_PREHEAT_PLA_BEDONLY_PL[] PROGMEM = "Predehrev PLA Bed";
const char * const MSG_PREHEAT_PLA_BEDONLY_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_PLA_BEDONLY_EN,
	MSG_PREHEAT_PLA_BEDONLY_CZ,
	MSG_PREHEAT_PLA_BEDONLY_IT,
	MSG_PREHEAT_PLA_BEDONLY_ES,
	MSG_PREHEAT_PLA_BEDONLY_PL
};

const char MSG_PREHEAT_PLA_SETTINGS_EN[] PROGMEM = "Preheat PLA conf";
const char MSG_PREHEAT_PLA_SETTINGS_CZ[] PROGMEM = "Predehrev PLA conf";
const char MSG_PREHEAT_PLA_SETTINGS_IT[] PROGMEM = "Preheat PLA conf";
const char MSG_PREHEAT_PLA_SETTINGS_ES[] PROGMEM = "Preheat PLA conf";
const char MSG_PREHEAT_PLA_SETTINGS_PL[] PROGMEM = "Predehrev PLA conf";
const char * const MSG_PREHEAT_PLA_SETTINGS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_PLA_SETTINGS_EN,
	MSG_PREHEAT_PLA_SETTINGS_CZ,
	MSG_PREHEAT_PLA_SETTINGS_IT,
	MSG_PREHEAT_PLA_SETTINGS_ES,
	MSG_PREHEAT_PLA_SETTINGS_PL
};

const char MSG_PREPARE_EN[] PROGMEM = "Prepare";
const char MSG_PREPARE_CZ[] PROGMEM = "Priprava";
const char MSG_PREPARE_IT[] PROGMEM = "Prepare";
const char MSG_PREPARE_ES[] PROGMEM = "Prepare";
const char MSG_PREPARE_PL[] PROGMEM = "Priprava";
const char * const MSG_PREPARE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREPARE_EN,
	MSG_PREPARE_CZ,
	MSG_PREPARE_IT,
	MSG_PREPARE_ES,
	MSG_PREPARE_PL
};

const char MSG_PRESS_EN[] PROGMEM = "And press the knob";
const char MSG_PRESS_CZ[] PROGMEM = "A stisknete tlacitko";
const char MSG_PRESS_IT[] PROGMEM = "Y pulse el mando";
const char MSG_PRESS_ES[] PROGMEM = "Y pulse el mando";
const char MSG_PRESS_PL[] PROGMEM = "Nacisnij przycisk";
const char * const MSG_PRESS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRESS_EN,
	MSG_PRESS_CZ,
	MSG_PRESS_IT,
	MSG_PRESS_ES,
	MSG_PRESS_PL
};

const char MSG_PRINT_ABORTED_EN[] PROGMEM = "Print aborted";
const char MSG_PRINT_ABORTED_CZ[] PROGMEM = "Tisk prerusen";
const char MSG_PRINT_ABORTED_IT[] PROGMEM = "Stampa abortita";
const char MSG_PRINT_ABORTED_ES[] PROGMEM = "Print aborted";
const char MSG_PRINT_ABORTED_PL[] PROGMEM = "Druk przerwany";
const char * const MSG_PRINT_ABORTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRINT_ABORTED_EN,
	MSG_PRINT_ABORTED_CZ,
	MSG_PRINT_ABORTED_IT,
	MSG_PRINT_ABORTED_ES,
	MSG_PRINT_ABORTED_PL
};

const char MSG_PRUSA3D_EN[] PROGMEM = "prusa3d.com";
const char MSG_PRUSA3D_CZ[] PROGMEM = "prusa3d.cz";
const char MSG_PRUSA3D_IT[] PROGMEM = "prusa3d.com";
const char MSG_PRUSA3D_ES[] PROGMEM = "prusa3d.com";
const char MSG_PRUSA3D_PL[] PROGMEM = "prusa3d.cz";
const char * const MSG_PRUSA3D_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRUSA3D_EN,
	MSG_PRUSA3D_CZ,
	MSG_PRUSA3D_IT,
	MSG_PRUSA3D_ES,
	MSG_PRUSA3D_PL
};

const char MSG_PRUSA3D_FORUM_EN[] PROGMEM = "forum.prusa3d.com";
const char MSG_PRUSA3D_FORUM_CZ[] PROGMEM = "forum.prusa3d.cz";
const char MSG_PRUSA3D_FORUM_IT[] PROGMEM = "forum.prusa3d.com";
const char MSG_PRUSA3D_FORUM_ES[] PROGMEM = "forum.prusa3d.com";
const char MSG_PRUSA3D_FORUM_PL[] PROGMEM = "forum.prusa3d.cz";
const char * const MSG_PRUSA3D_FORUM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRUSA3D_FORUM_EN,
	MSG_PRUSA3D_FORUM_CZ,
	MSG_PRUSA3D_FORUM_IT,
	MSG_PRUSA3D_FORUM_ES,
	MSG_PRUSA3D_FORUM_PL
};

const char MSG_PRUSA3D_HOWTO_EN[] PROGMEM = "howto.prusa3d.com";
const char MSG_PRUSA3D_HOWTO_CZ[] PROGMEM = "howto.prusa3d.cz";
const char MSG_PRUSA3D_HOWTO_IT[] PROGMEM = "howto.prusa3d.com";
const char MSG_PRUSA3D_HOWTO_ES[] PROGMEM = "howto.prusa3d.com";
const char MSG_PRUSA3D_HOWTO_PL[] PROGMEM = "howto.prusa3d.cz";
const char * const MSG_PRUSA3D_HOWTO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRUSA3D_HOWTO_EN,
	MSG_PRUSA3D_HOWTO_CZ,
	MSG_PRUSA3D_HOWTO_IT,
	MSG_PRUSA3D_HOWTO_ES,
	MSG_PRUSA3D_HOWTO_PL
};

const char MSG_REBOOT_EN[] PROGMEM = "Reboot the printer";
const char MSG_REBOOT_CZ[] PROGMEM = "Restartujte tiskarnu";
const char MSG_REBOOT_IT[] PROGMEM = "Riavvio la stamp.";
const char MSG_REBOOT_ES[] PROGMEM = "Reiniciar la imp.";
const char MSG_REBOOT_PL[] PROGMEM = "Restart drukarki";
const char * const MSG_REBOOT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_REBOOT_EN,
	MSG_REBOOT_CZ,
	MSG_REBOOT_IT,
	MSG_REBOOT_ES,
	MSG_REBOOT_PL
};

const char MSG_RECTRACT_EN[] PROGMEM = "Rectract";
const char MSG_RECTRACT_CZ[] PROGMEM = "Rectract";
const char MSG_RECTRACT_IT[] PROGMEM = "Rectract";
const char MSG_RECTRACT_ES[] PROGMEM = "Rectract";
const char MSG_RECTRACT_PL[] PROGMEM = "Rectract";
const char * const MSG_RECTRACT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RECTRACT_EN,
	MSG_RECTRACT_CZ,
	MSG_RECTRACT_IT,
	MSG_RECTRACT_ES,
	MSG_RECTRACT_PL
};

const char MSG_REFRESH_EN[] PROGMEM = "\xF8" "Refresh";
const char MSG_REFRESH_CZ[] PROGMEM = "\xF8" "Obnovit";
const char MSG_REFRESH_IT[] PROGMEM = "\xF8" "Refresh";
const char MSG_REFRESH_ES[] PROGMEM = "\xF8" "Refresh";
const char MSG_REFRESH_PL[] PROGMEM = "\xF8" "Obnovit";
const char * const MSG_REFRESH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_REFRESH_EN,
	MSG_REFRESH_CZ,
	MSG_REFRESH_IT,
	MSG_REFRESH_ES,
	MSG_REFRESH_PL
};

const char MSG_RESEND_EN[] PROGMEM = "Resend: ";
const char MSG_RESEND_CZ[] PROGMEM = "Resend: ";
const char MSG_RESEND_IT[] PROGMEM = "Resend: ";
const char MSG_RESEND_ES[] PROGMEM = "Resend: ";
const char MSG_RESEND_PL[] PROGMEM = "Resend: ";
const char * const MSG_RESEND_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESEND_EN,
	MSG_RESEND_CZ,
	MSG_RESEND_IT,
	MSG_RESEND_ES,
	MSG_RESEND_PL
};

const char MSG_RESTORE_FAILSAFE_EN[] PROGMEM = "Restore failsafe";
const char MSG_RESTORE_FAILSAFE_CZ[] PROGMEM = "Obnovit vychozi";
const char MSG_RESTORE_FAILSAFE_IT[] PROGMEM = "Restore failsafe";
const char MSG_RESTORE_FAILSAFE_ES[] PROGMEM = "Restore failsafe";
const char MSG_RESTORE_FAILSAFE_PL[] PROGMEM = "Obnovit vychozi";
const char * const MSG_RESTORE_FAILSAFE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESTORE_FAILSAFE_EN,
	MSG_RESTORE_FAILSAFE_CZ,
	MSG_RESTORE_FAILSAFE_IT,
	MSG_RESTORE_FAILSAFE_ES,
	MSG_RESTORE_FAILSAFE_PL
};

const char MSG_RESUME_PRINT_EN[] PROGMEM = "Resume print";
const char MSG_RESUME_PRINT_CZ[] PROGMEM = "Pokracovat";
const char MSG_RESUME_PRINT_IT[] PROGMEM = "Riprendi stampa";
const char MSG_RESUME_PRINT_ES[] PROGMEM = "Reanudar impres.";
const char MSG_RESUME_PRINT_PL[] PROGMEM = "Kontynuowac";
const char * const MSG_RESUME_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESUME_PRINT_EN,
	MSG_RESUME_PRINT_CZ,
	MSG_RESUME_PRINT_IT,
	MSG_RESUME_PRINT_ES,
	MSG_RESUME_PRINT_PL
};

const char MSG_RESUMING_EN[] PROGMEM = "Resuming print";
const char MSG_RESUMING_CZ[] PROGMEM = "Obnoveni tisku";
const char MSG_RESUMING_IT[] PROGMEM = "Riprendi Stampa";
const char MSG_RESUMING_ES[] PROGMEM = "Resumiendo impre.";
const char MSG_RESUMING_PL[] PROGMEM = "Wznowienie druku";
const char * const MSG_RESUMING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESUMING_EN,
	MSG_RESUMING_CZ,
	MSG_RESUMING_IT,
	MSG_RESUMING_ES,
	MSG_RESUMING_PL
};

const char MSG_RETRACT_EN[] PROGMEM = "Retract";
const char MSG_RETRACT_CZ[] PROGMEM = "Retract";
const char MSG_RETRACT_IT[] PROGMEM = "Retract";
const char MSG_RETRACT_ES[] PROGMEM = "Retract";
const char MSG_RETRACT_PL[] PROGMEM = "Retract";
const char * const MSG_RETRACT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RETRACT_EN,
	MSG_RETRACT_CZ,
	MSG_RETRACT_IT,
	MSG_RETRACT_ES,
	MSG_RETRACT_PL
};

const char MSG_SD_CANT_ENTER_SUBDIR_EN[] PROGMEM = "Cannot enter subdir: ";
const char MSG_SD_CANT_ENTER_SUBDIR_CZ[] PROGMEM = "Cannot enter subdir: ";
const char MSG_SD_CANT_ENTER_SUBDIR_IT[] PROGMEM = "Cannot enter subdir: ";
const char MSG_SD_CANT_ENTER_SUBDIR_ES[] PROGMEM = "Cannot enter subdir: ";
const char MSG_SD_CANT_ENTER_SUBDIR_PL[] PROGMEM = "Cannot enter subdir: ";
const char * const MSG_SD_CANT_ENTER_SUBDIR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_CANT_ENTER_SUBDIR_EN,
	MSG_SD_CANT_ENTER_SUBDIR_CZ,
	MSG_SD_CANT_ENTER_SUBDIR_IT,
	MSG_SD_CANT_ENTER_SUBDIR_ES,
	MSG_SD_CANT_ENTER_SUBDIR_PL
};

const char MSG_SD_CANT_OPEN_SUBDIR_EN[] PROGMEM = "Cannot open subdir";
const char MSG_SD_CANT_OPEN_SUBDIR_CZ[] PROGMEM = "Cannot open subdir";
const char MSG_SD_CANT_OPEN_SUBDIR_IT[] PROGMEM = "Cannot open subdir";
const char MSG_SD_CANT_OPEN_SUBDIR_ES[] PROGMEM = "Cannot open subdir";
const char MSG_SD_CANT_OPEN_SUBDIR_PL[] PROGMEM = "Cannot open subdir";
const char * const MSG_SD_CANT_OPEN_SUBDIR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_CANT_OPEN_SUBDIR_EN,
	MSG_SD_CANT_OPEN_SUBDIR_CZ,
	MSG_SD_CANT_OPEN_SUBDIR_IT,
	MSG_SD_CANT_OPEN_SUBDIR_ES,
	MSG_SD_CANT_OPEN_SUBDIR_PL
};

const char MSG_SD_CARD_OK_EN[] PROGMEM = "SD card ok";
const char MSG_SD_CARD_OK_CZ[] PROGMEM = "SD card ok";
const char MSG_SD_CARD_OK_IT[] PROGMEM = "SD card ok";
const char MSG_SD_CARD_OK_ES[] PROGMEM = "SD card ok";
const char MSG_SD_CARD_OK_PL[] PROGMEM = "SD card ok";
const char * const MSG_SD_CARD_OK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_CARD_OK_EN,
	MSG_SD_CARD_OK_CZ,
	MSG_SD_CARD_OK_IT,
	MSG_SD_CARD_OK_ES,
	MSG_SD_CARD_OK_PL
};

const char MSG_SD_ERR_WRITE_TO_FILE_EN[] PROGMEM = "error writing to file";
const char MSG_SD_ERR_WRITE_TO_FILE_CZ[] PROGMEM = "error writing to file";
const char MSG_SD_ERR_WRITE_TO_FILE_IT[] PROGMEM = "error writing to file";
const char MSG_SD_ERR_WRITE_TO_FILE_ES[] PROGMEM = "error writing to file";
const char MSG_SD_ERR_WRITE_TO_FILE_PL[] PROGMEM = "error writing to file";
const char * const MSG_SD_ERR_WRITE_TO_FILE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_ERR_WRITE_TO_FILE_EN,
	MSG_SD_ERR_WRITE_TO_FILE_CZ,
	MSG_SD_ERR_WRITE_TO_FILE_IT,
	MSG_SD_ERR_WRITE_TO_FILE_ES,
	MSG_SD_ERR_WRITE_TO_FILE_PL
};

const char MSG_SD_FILE_OPENED_EN[] PROGMEM = "File opened: ";
const char MSG_SD_FILE_OPENED_CZ[] PROGMEM = "File opened: ";
const char MSG_SD_FILE_OPENED_IT[] PROGMEM = "File opened: ";
const char MSG_SD_FILE_OPENED_ES[] PROGMEM = "File opened: ";
const char MSG_SD_FILE_OPENED_PL[] PROGMEM = "File opened: ";
const char * const MSG_SD_FILE_OPENED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_FILE_OPENED_EN,
	MSG_SD_FILE_OPENED_CZ,
	MSG_SD_FILE_OPENED_IT,
	MSG_SD_FILE_OPENED_ES,
	MSG_SD_FILE_OPENED_PL
};

const char MSG_SD_FILE_SELECTED_EN[] PROGMEM = "File selected";
const char MSG_SD_FILE_SELECTED_CZ[] PROGMEM = "File selected";
const char MSG_SD_FILE_SELECTED_IT[] PROGMEM = "File selected";
const char MSG_SD_FILE_SELECTED_ES[] PROGMEM = "File selected";
const char MSG_SD_FILE_SELECTED_PL[] PROGMEM = "File selected";
const char * const MSG_SD_FILE_SELECTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_FILE_SELECTED_EN,
	MSG_SD_FILE_SELECTED_CZ,
	MSG_SD_FILE_SELECTED_IT,
	MSG_SD_FILE_SELECTED_ES,
	MSG_SD_FILE_SELECTED_PL
};

const char MSG_SD_INIT_FAIL_EN[] PROGMEM = "SD init fail";
const char MSG_SD_INIT_FAIL_CZ[] PROGMEM = "SD init fail";
const char MSG_SD_INIT_FAIL_IT[] PROGMEM = "SD init fail";
const char MSG_SD_INIT_FAIL_ES[] PROGMEM = "SD init fail";
const char MSG_SD_INIT_FAIL_PL[] PROGMEM = "SD init fail";
const char * const MSG_SD_INIT_FAIL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_INIT_FAIL_EN,
	MSG_SD_INIT_FAIL_CZ,
	MSG_SD_INIT_FAIL_IT,
	MSG_SD_INIT_FAIL_ES,
	MSG_SD_INIT_FAIL_PL
};

const char MSG_SD_INSERTED_EN[] PROGMEM = "Card inserted";
const char MSG_SD_INSERTED_CZ[] PROGMEM = "Karta vlozena";
const char MSG_SD_INSERTED_IT[] PROGMEM = "SD Card inserita";
const char MSG_SD_INSERTED_ES[] PROGMEM = "Tarjeta colocada";
const char MSG_SD_INSERTED_PL[] PROGMEM = "Karta wlozona";
const char * const MSG_SD_INSERTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_INSERTED_EN,
	MSG_SD_INSERTED_CZ,
	MSG_SD_INSERTED_IT,
	MSG_SD_INSERTED_ES,
	MSG_SD_INSERTED_PL
};

const char MSG_SD_NOT_PRINTING_EN[] PROGMEM = "Not SD printing";
const char MSG_SD_NOT_PRINTING_CZ[] PROGMEM = "Not SD printing";
const char MSG_SD_NOT_PRINTING_IT[] PROGMEM = "Not SD printing";
const char MSG_SD_NOT_PRINTING_ES[] PROGMEM = "Not SD printing";
const char MSG_SD_NOT_PRINTING_PL[] PROGMEM = "Not SD printing";
const char * const MSG_SD_NOT_PRINTING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_NOT_PRINTING_EN,
	MSG_SD_NOT_PRINTING_CZ,
	MSG_SD_NOT_PRINTING_IT,
	MSG_SD_NOT_PRINTING_ES,
	MSG_SD_NOT_PRINTING_PL
};

const char MSG_SD_OPENROOT_FAIL_EN[] PROGMEM = "openRoot failed";
const char MSG_SD_OPENROOT_FAIL_CZ[] PROGMEM = "openRoot failed";
const char MSG_SD_OPENROOT_FAIL_IT[] PROGMEM = "openRoot failed";
const char MSG_SD_OPENROOT_FAIL_ES[] PROGMEM = "openRoot failed";
const char MSG_SD_OPENROOT_FAIL_PL[] PROGMEM = "openRoot failed";
const char * const MSG_SD_OPENROOT_FAIL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_OPENROOT_FAIL_EN,
	MSG_SD_OPENROOT_FAIL_CZ,
	MSG_SD_OPENROOT_FAIL_IT,
	MSG_SD_OPENROOT_FAIL_ES,
	MSG_SD_OPENROOT_FAIL_PL
};

const char MSG_SD_OPEN_FILE_FAIL_EN[] PROGMEM = "open failed, File: ";
const char MSG_SD_OPEN_FILE_FAIL_CZ[] PROGMEM = "open failed, File: ";
const char MSG_SD_OPEN_FILE_FAIL_IT[] PROGMEM = "open failed, File: ";
const char MSG_SD_OPEN_FILE_FAIL_ES[] PROGMEM = "open failed, File: ";
const char MSG_SD_OPEN_FILE_FAIL_PL[] PROGMEM = "open failed, File: ";
const char * const MSG_SD_OPEN_FILE_FAIL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_OPEN_FILE_FAIL_EN,
	MSG_SD_OPEN_FILE_FAIL_CZ,
	MSG_SD_OPEN_FILE_FAIL_IT,
	MSG_SD_OPEN_FILE_FAIL_ES,
	MSG_SD_OPEN_FILE_FAIL_PL
};

const char MSG_SD_PRINTING_BYTE_EN[] PROGMEM = "SD printing byte ";
const char MSG_SD_PRINTING_BYTE_CZ[] PROGMEM = "SD printing byte ";
const char MSG_SD_PRINTING_BYTE_IT[] PROGMEM = "SD printing byte ";
const char MSG_SD_PRINTING_BYTE_ES[] PROGMEM = "SD printing byte ";
const char MSG_SD_PRINTING_BYTE_PL[] PROGMEM = "SD printing byte ";
const char * const MSG_SD_PRINTING_BYTE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_PRINTING_BYTE_EN,
	MSG_SD_PRINTING_BYTE_CZ,
	MSG_SD_PRINTING_BYTE_IT,
	MSG_SD_PRINTING_BYTE_ES,
	MSG_SD_PRINTING_BYTE_PL
};

const char MSG_SD_REMOVED_EN[] PROGMEM = "Card removed";
const char MSG_SD_REMOVED_CZ[] PROGMEM = "Karta vyjmuta";
const char MSG_SD_REMOVED_IT[] PROGMEM = "SD Card rimossa";
const char MSG_SD_REMOVED_ES[] PROGMEM = "Tarjeta retirada";
const char MSG_SD_REMOVED_PL[] PROGMEM = "Karta wyjeta";
const char * const MSG_SD_REMOVED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_REMOVED_EN,
	MSG_SD_REMOVED_CZ,
	MSG_SD_REMOVED_IT,
	MSG_SD_REMOVED_ES,
	MSG_SD_REMOVED_PL
};

const char MSG_SD_SIZE_EN[] PROGMEM = " Size: ";
const char MSG_SD_SIZE_CZ[] PROGMEM = " Size: ";
const char MSG_SD_SIZE_IT[] PROGMEM = " Size: ";
const char MSG_SD_SIZE_ES[] PROGMEM = " Size: ";
const char MSG_SD_SIZE_PL[] PROGMEM = " Size: ";
const char * const MSG_SD_SIZE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_SIZE_EN,
	MSG_SD_SIZE_CZ,
	MSG_SD_SIZE_IT,
	MSG_SD_SIZE_ES,
	MSG_SD_SIZE_PL
};

const char MSG_SD_VOL_INIT_FAIL_EN[] PROGMEM = "volume.init failed";
const char MSG_SD_VOL_INIT_FAIL_CZ[] PROGMEM = "volume.init failed";
const char MSG_SD_VOL_INIT_FAIL_IT[] PROGMEM = "volume.init failed";
const char MSG_SD_VOL_INIT_FAIL_ES[] PROGMEM = "volume.init failed";
const char MSG_SD_VOL_INIT_FAIL_PL[] PROGMEM = "volume.init failed";
const char * const MSG_SD_VOL_INIT_FAIL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_VOL_INIT_FAIL_EN,
	MSG_SD_VOL_INIT_FAIL_CZ,
	MSG_SD_VOL_INIT_FAIL_IT,
	MSG_SD_VOL_INIT_FAIL_ES,
	MSG_SD_VOL_INIT_FAIL_PL
};

const char MSG_SD_WORKDIR_FAIL_EN[] PROGMEM = "workDir open failed";
const char MSG_SD_WORKDIR_FAIL_CZ[] PROGMEM = "workDir open failed";
const char MSG_SD_WORKDIR_FAIL_IT[] PROGMEM = "workDir open failed";
const char MSG_SD_WORKDIR_FAIL_ES[] PROGMEM = "workDir open failed";
const char MSG_SD_WORKDIR_FAIL_PL[] PROGMEM = "workDir open failed";
const char * const MSG_SD_WORKDIR_FAIL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_WORKDIR_FAIL_EN,
	MSG_SD_WORKDIR_FAIL_CZ,
	MSG_SD_WORKDIR_FAIL_IT,
	MSG_SD_WORKDIR_FAIL_ES,
	MSG_SD_WORKDIR_FAIL_PL
};

const char MSG_SD_WRITE_TO_FILE_EN[] PROGMEM = "Writing to file: ";
const char MSG_SD_WRITE_TO_FILE_CZ[] PROGMEM = "Writing to file: ";
const char MSG_SD_WRITE_TO_FILE_IT[] PROGMEM = "Writing to file: ";
const char MSG_SD_WRITE_TO_FILE_ES[] PROGMEM = "Writing to file: ";
const char MSG_SD_WRITE_TO_FILE_PL[] PROGMEM = "Writing to file: ";
const char * const MSG_SD_WRITE_TO_FILE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_WRITE_TO_FILE_EN,
	MSG_SD_WRITE_TO_FILE_CZ,
	MSG_SD_WRITE_TO_FILE_IT,
	MSG_SD_WRITE_TO_FILE_ES,
	MSG_SD_WRITE_TO_FILE_PL
};

const char MSG_SELFTEST_EN[] PROGMEM = "Selftest         ";
const char MSG_SELFTEST_CZ[] PROGMEM = "Selftest         ";
const char MSG_SELFTEST_IT[] PROGMEM = "Autotest";
const char MSG_SELFTEST_ES[] PROGMEM = "Autotest";
const char MSG_SELFTEST_PL[] PROGMEM = "Selftest         ";
const char * const MSG_SELFTEST_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_EN,
	MSG_SELFTEST_CZ,
	MSG_SELFTEST_IT,
	MSG_SELFTEST_ES,
	MSG_SELFTEST_PL
};

const char MSG_SELFTEST_BEDHEATER_EN[] PROGMEM = "Bed / Heater";
const char MSG_SELFTEST_BEDHEATER_CZ[] PROGMEM = "Bed / Heater";
const char MSG_SELFTEST_BEDHEATER_IT[] PROGMEM = "Piastra/Riscaldatore";
const char MSG_SELFTEST_BEDHEATER_ES[] PROGMEM = "Cama/Calentador";
const char MSG_SELFTEST_BEDHEATER_PL[] PROGMEM = "Bed / Heater";
const char * const MSG_SELFTEST_BEDHEATER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_BEDHEATER_EN,
	MSG_SELFTEST_BEDHEATER_CZ,
	MSG_SELFTEST_BEDHEATER_IT,
	MSG_SELFTEST_BEDHEATER_ES,
	MSG_SELFTEST_BEDHEATER_PL
};

const char MSG_SELFTEST_CHECK_ALLCORRECT_EN[] PROGMEM = "All correct      ";
const char MSG_SELFTEST_CHECK_ALLCORRECT_CZ[] PROGMEM = "Vse OK           ";
const char MSG_SELFTEST_CHECK_ALLCORRECT_IT[] PROGMEM = "Nessun errore";
const char MSG_SELFTEST_CHECK_ALLCORRECT_ES[] PROGMEM = "Todo bie ";
const char MSG_SELFTEST_CHECK_ALLCORRECT_PL[] PROGMEM = "Wszystko OK      ";
const char * const MSG_SELFTEST_CHECK_ALLCORRECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_ALLCORRECT_EN,
	MSG_SELFTEST_CHECK_ALLCORRECT_CZ,
	MSG_SELFTEST_CHECK_ALLCORRECT_IT,
	MSG_SELFTEST_CHECK_ALLCORRECT_ES,
	MSG_SELFTEST_CHECK_ALLCORRECT_PL
};

const char MSG_SELFTEST_CHECK_BED_EN[] PROGMEM = "Checking bed     ";
const char MSG_SELFTEST_CHECK_BED_CZ[] PROGMEM = "Kontrola bed     ";
const char MSG_SELFTEST_CHECK_BED_IT[] PROGMEM = "Verifica piastra";
const char MSG_SELFTEST_CHECK_BED_ES[] PROGMEM = "Control de cama";
const char MSG_SELFTEST_CHECK_BED_PL[] PROGMEM = "Kontrola bed     ";
const char * const MSG_SELFTEST_CHECK_BED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_BED_EN,
	MSG_SELFTEST_CHECK_BED_CZ,
	MSG_SELFTEST_CHECK_BED_IT,
	MSG_SELFTEST_CHECK_BED_ES,
	MSG_SELFTEST_CHECK_BED_PL
};

const char MSG_SELFTEST_CHECK_ENDSTOPS_EN[] PROGMEM = "Checking endstops";
const char MSG_SELFTEST_CHECK_ENDSTOPS_CZ[] PROGMEM = "Kontrola endstops";
const char MSG_SELFTEST_CHECK_ENDSTOPS_IT[] PROGMEM = "Verifica limiti";
const char MSG_SELFTEST_CHECK_ENDSTOPS_ES[] PROGMEM = "Cont. topes final";
const char MSG_SELFTEST_CHECK_ENDSTOPS_PL[] PROGMEM = "Kontrola endstops";
const char * const MSG_SELFTEST_CHECK_ENDSTOPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_ENDSTOPS_EN,
	MSG_SELFTEST_CHECK_ENDSTOPS_CZ,
	MSG_SELFTEST_CHECK_ENDSTOPS_IT,
	MSG_SELFTEST_CHECK_ENDSTOPS_ES,
	MSG_SELFTEST_CHECK_ENDSTOPS_PL
};

const char MSG_SELFTEST_CHECK_HOTEND_EN[] PROGMEM = "Checking hotend  ";
const char MSG_SELFTEST_CHECK_HOTEND_CZ[] PROGMEM = "Kontrola hotend  ";
const char MSG_SELFTEST_CHECK_HOTEND_IT[] PROGMEM = "Verifica lim temp";
const char MSG_SELFTEST_CHECK_HOTEND_ES[] PROGMEM = "Control hotend ";
const char MSG_SELFTEST_CHECK_HOTEND_PL[] PROGMEM = "Kontrola hotend  ";
const char * const MSG_SELFTEST_CHECK_HOTEND_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_HOTEND_EN,
	MSG_SELFTEST_CHECK_HOTEND_CZ,
	MSG_SELFTEST_CHECK_HOTEND_IT,
	MSG_SELFTEST_CHECK_HOTEND_ES,
	MSG_SELFTEST_CHECK_HOTEND_PL
};

const char MSG_SELFTEST_CHECK_X_EN[] PROGMEM = "Checking X axis  ";
const char MSG_SELFTEST_CHECK_X_CZ[] PROGMEM = "Kontrola X axis  ";
const char MSG_SELFTEST_CHECK_X_IT[] PROGMEM = "Verifica asse X";
const char MSG_SELFTEST_CHECK_X_ES[] PROGMEM = "Control del eje X";
const char MSG_SELFTEST_CHECK_X_PL[] PROGMEM = "Kontrola X axis  ";
const char * const MSG_SELFTEST_CHECK_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_X_EN,
	MSG_SELFTEST_CHECK_X_CZ,
	MSG_SELFTEST_CHECK_X_IT,
	MSG_SELFTEST_CHECK_X_ES,
	MSG_SELFTEST_CHECK_X_PL
};

const char MSG_SELFTEST_CHECK_Y_EN[] PROGMEM = "Checking Y axis  ";
const char MSG_SELFTEST_CHECK_Y_CZ[] PROGMEM = "Kontrola Y axis  ";
const char MSG_SELFTEST_CHECK_Y_IT[] PROGMEM = "Verifica asse Y";
const char MSG_SELFTEST_CHECK_Y_ES[] PROGMEM = "Control del eje Y";
const char MSG_SELFTEST_CHECK_Y_PL[] PROGMEM = "Kontrola Y axis  ";
const char * const MSG_SELFTEST_CHECK_Y_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_Y_EN,
	MSG_SELFTEST_CHECK_Y_CZ,
	MSG_SELFTEST_CHECK_Y_IT,
	MSG_SELFTEST_CHECK_Y_ES,
	MSG_SELFTEST_CHECK_Y_PL
};

const char MSG_SELFTEST_CHECK_Z_EN[] PROGMEM = "Checking Z axis  ";
const char MSG_SELFTEST_CHECK_Z_CZ[] PROGMEM = "Kontrola Z axis  ";
const char MSG_SELFTEST_CHECK_Z_IT[] PROGMEM = "Verifica asse Z";
const char MSG_SELFTEST_CHECK_Z_ES[] PROGMEM = "Control del eje Z";
const char MSG_SELFTEST_CHECK_Z_PL[] PROGMEM = "Kontrola Z axis  ";
const char * const MSG_SELFTEST_CHECK_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_Z_EN,
	MSG_SELFTEST_CHECK_Z_CZ,
	MSG_SELFTEST_CHECK_Z_IT,
	MSG_SELFTEST_CHECK_Z_ES,
	MSG_SELFTEST_CHECK_Z_PL
};

const char MSG_SELFTEST_ENDSTOP_EN[] PROGMEM = "Endstop";
const char MSG_SELFTEST_ENDSTOP_CZ[] PROGMEM = "Endstop";
const char MSG_SELFTEST_ENDSTOP_IT[] PROGMEM = "Limite corsa";
const char MSG_SELFTEST_ENDSTOP_ES[] PROGMEM = "Tope final";
const char MSG_SELFTEST_ENDSTOP_PL[] PROGMEM = "Endstop";
const char * const MSG_SELFTEST_ENDSTOP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_ENDSTOP_EN,
	MSG_SELFTEST_ENDSTOP_CZ,
	MSG_SELFTEST_ENDSTOP_IT,
	MSG_SELFTEST_ENDSTOP_ES,
	MSG_SELFTEST_ENDSTOP_PL
};

const char MSG_SELFTEST_ENDSTOPS_EN[] PROGMEM = "Endstops";
const char MSG_SELFTEST_ENDSTOPS_CZ[] PROGMEM = "Endstops";
const char MSG_SELFTEST_ENDSTOPS_IT[] PROGMEM = "Limiti corsa";
const char MSG_SELFTEST_ENDSTOPS_ES[] PROGMEM = "Topes final";
const char MSG_SELFTEST_ENDSTOPS_PL[] PROGMEM = "Endstops";
const char * const MSG_SELFTEST_ENDSTOPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_ENDSTOPS_EN,
	MSG_SELFTEST_ENDSTOPS_CZ,
	MSG_SELFTEST_ENDSTOPS_IT,
	MSG_SELFTEST_ENDSTOPS_ES,
	MSG_SELFTEST_ENDSTOPS_PL
};

const char MSG_SELFTEST_ENDSTOP_NOTHIT_EN[] PROGMEM = "Endstop not hit";
const char MSG_SELFTEST_ENDSTOP_NOTHIT_CZ[] PROGMEM = "Endstop not hit";
const char MSG_SELFTEST_ENDSTOP_NOTHIT_IT[] PROGMEM = "Lim. fuoriportata";
const char MSG_SELFTEST_ENDSTOP_NOTHIT_ES[] PROGMEM = "Tope fin. no toc.";
const char MSG_SELFTEST_ENDSTOP_NOTHIT_PL[] PROGMEM = "Endstop not hit";
const char * const MSG_SELFTEST_ENDSTOP_NOTHIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_ENDSTOP_NOTHIT_EN,
	MSG_SELFTEST_ENDSTOP_NOTHIT_CZ,
	MSG_SELFTEST_ENDSTOP_NOTHIT_IT,
	MSG_SELFTEST_ENDSTOP_NOTHIT_ES,
	MSG_SELFTEST_ENDSTOP_NOTHIT_PL
};

const char MSG_SELFTEST_ERROR_EN[] PROGMEM = "Selftest error !";
const char MSG_SELFTEST_ERROR_CZ[] PROGMEM = "Selftest error !";
const char MSG_SELFTEST_ERROR_IT[] PROGMEM = "Autotest negativo";
const char MSG_SELFTEST_ERROR_ES[] PROGMEM = "Autotest error!";
const char MSG_SELFTEST_ERROR_PL[] PROGMEM = "Selftest error !";
const char * const MSG_SELFTEST_ERROR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_ERROR_EN,
	MSG_SELFTEST_ERROR_CZ,
	MSG_SELFTEST_ERROR_IT,
	MSG_SELFTEST_ERROR_ES,
	MSG_SELFTEST_ERROR_PL
};

const char MSG_SELFTEST_FAILED_EN[] PROGMEM = "Selftest failed  ";
const char MSG_SELFTEST_FAILED_CZ[] PROGMEM = "Selftest selhal  ";
const char MSG_SELFTEST_FAILED_IT[] PROGMEM = "Autotest fallito";
const char MSG_SELFTEST_FAILED_ES[] PROGMEM = "Autotest fallado";
const char MSG_SELFTEST_FAILED_PL[] PROGMEM = "Selftest nieudany";
const char * const MSG_SELFTEST_FAILED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FAILED_EN,
	MSG_SELFTEST_FAILED_CZ,
	MSG_SELFTEST_FAILED_IT,
	MSG_SELFTEST_FAILED_ES,
	MSG_SELFTEST_FAILED_PL
};

const char MSG_SELFTEST_HEATERTHERMISTOR_EN[] PROGMEM = "Heater/Thermistor";
const char MSG_SELFTEST_HEATERTHERMISTOR_CZ[] PROGMEM = "Heater/Thermistor";
const char MSG_SELFTEST_HEATERTHERMISTOR_IT[] PROGMEM = "Riscald./Termistore";
const char MSG_SELFTEST_HEATERTHERMISTOR_ES[] PROGMEM = "Calent./Termistor";
const char MSG_SELFTEST_HEATERTHERMISTOR_PL[] PROGMEM = "Heater/Thermistor";
const char * const MSG_SELFTEST_HEATERTHERMISTOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_HEATERTHERMISTOR_EN,
	MSG_SELFTEST_HEATERTHERMISTOR_CZ,
	MSG_SELFTEST_HEATERTHERMISTOR_IT,
	MSG_SELFTEST_HEATERTHERMISTOR_ES,
	MSG_SELFTEST_HEATERTHERMISTOR_PL
};

const char MSG_SELFTEST_MOTOR_EN[] PROGMEM = "Motor";
const char MSG_SELFTEST_MOTOR_CZ[] PROGMEM = "Motor";
const char MSG_SELFTEST_MOTOR_IT[] PROGMEM = "Motore";
const char MSG_SELFTEST_MOTOR_ES[] PROGMEM = "Motor";
const char MSG_SELFTEST_MOTOR_PL[] PROGMEM = "Silnik";
const char * const MSG_SELFTEST_MOTOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_MOTOR_EN,
	MSG_SELFTEST_MOTOR_CZ,
	MSG_SELFTEST_MOTOR_IT,
	MSG_SELFTEST_MOTOR_ES,
	MSG_SELFTEST_MOTOR_PL
};

const char MSG_SELFTEST_NOTCONNECTED_EN[] PROGMEM = "Not connected";
const char MSG_SELFTEST_NOTCONNECTED_CZ[] PROGMEM = "Nezapojeno    ";
const char MSG_SELFTEST_NOTCONNECTED_IT[] PROGMEM = "Non connesso";
const char MSG_SELFTEST_NOTCONNECTED_ES[] PROGMEM = "No hay conexion  ";
const char MSG_SELFTEST_NOTCONNECTED_PL[] PROGMEM = "Nie podlaczono   ";
const char * const MSG_SELFTEST_NOTCONNECTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_NOTCONNECTED_EN,
	MSG_SELFTEST_NOTCONNECTED_CZ,
	MSG_SELFTEST_NOTCONNECTED_IT,
	MSG_SELFTEST_NOTCONNECTED_ES,
	MSG_SELFTEST_NOTCONNECTED_PL
};

const char MSG_SELFTEST_OK_EN[] PROGMEM = "Self test OK";
const char MSG_SELFTEST_OK_CZ[] PROGMEM = "Self test OK";
const char MSG_SELFTEST_OK_IT[] PROGMEM = "Autotest OK";
const char MSG_SELFTEST_OK_ES[] PROGMEM = "Self test OK";
const char MSG_SELFTEST_OK_PL[] PROGMEM = "Self test OK";
const char * const MSG_SELFTEST_OK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_OK_EN,
	MSG_SELFTEST_OK_CZ,
	MSG_SELFTEST_OK_IT,
	MSG_SELFTEST_OK_ES,
	MSG_SELFTEST_OK_PL
};

const char MSG_SELFTEST_PLEASECHECK_EN[] PROGMEM = "Please check :";
const char MSG_SELFTEST_PLEASECHECK_CZ[] PROGMEM = "Zkontrolujte :";
const char MSG_SELFTEST_PLEASECHECK_IT[] PROGMEM = "Verifica:";
const char MSG_SELFTEST_PLEASECHECK_ES[] PROGMEM = "Controla :";
const char MSG_SELFTEST_PLEASECHECK_PL[] PROGMEM = "Skontroluj :";
const char * const MSG_SELFTEST_PLEASECHECK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_PLEASECHECK_EN,
	MSG_SELFTEST_PLEASECHECK_CZ,
	MSG_SELFTEST_PLEASECHECK_IT,
	MSG_SELFTEST_PLEASECHECK_ES,
	MSG_SELFTEST_PLEASECHECK_PL
};

const char MSG_SELFTEST_START_EN[] PROGMEM = "Self test start  ";
const char MSG_SELFTEST_START_CZ[] PROGMEM = "Self test start  ";
const char MSG_SELFTEST_START_IT[] PROGMEM = "Inizia autotest";
const char MSG_SELFTEST_START_ES[] PROGMEM = "Autotest salida";
const char MSG_SELFTEST_START_PL[] PROGMEM = "Self test start  ";
const char * const MSG_SELFTEST_START_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_START_EN,
	MSG_SELFTEST_START_CZ,
	MSG_SELFTEST_START_IT,
	MSG_SELFTEST_START_ES,
	MSG_SELFTEST_START_PL
};

const char MSG_SELFTEST_WIRINGERROR_EN[] PROGMEM = "Wiring error";
const char MSG_SELFTEST_WIRINGERROR_CZ[] PROGMEM = "Chyba zapojeni";
const char MSG_SELFTEST_WIRINGERROR_IT[] PROGMEM = "Errore cablaggio";
const char MSG_SELFTEST_WIRINGERROR_ES[] PROGMEM = "Error de conexión";
const char MSG_SELFTEST_WIRINGERROR_PL[] PROGMEM = "Blad polaczenia";
const char * const MSG_SELFTEST_WIRINGERROR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_WIRINGERROR_EN,
	MSG_SELFTEST_WIRINGERROR_CZ,
	MSG_SELFTEST_WIRINGERROR_IT,
	MSG_SELFTEST_WIRINGERROR_ES,
	MSG_SELFTEST_WIRINGERROR_PL
};

const char MSG_SERIAL_ERROR_MENU_STRUCTURE_EN[] PROGMEM = "Error in menu structure";
const char MSG_SERIAL_ERROR_MENU_STRUCTURE_CZ[] PROGMEM = "Error in menu structure";
const char MSG_SERIAL_ERROR_MENU_STRUCTURE_IT[] PROGMEM = "Error in menu structure";
const char MSG_SERIAL_ERROR_MENU_STRUCTURE_ES[] PROGMEM = "Error in menu structure";
const char MSG_SERIAL_ERROR_MENU_STRUCTURE_PL[] PROGMEM = "Error in menu structure";
const char * const MSG_SERIAL_ERROR_MENU_STRUCTURE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SERIAL_ERROR_MENU_STRUCTURE_EN,
	MSG_SERIAL_ERROR_MENU_STRUCTURE_CZ,
	MSG_SERIAL_ERROR_MENU_STRUCTURE_IT,
	MSG_SERIAL_ERROR_MENU_STRUCTURE_ES,
	MSG_SERIAL_ERROR_MENU_STRUCTURE_PL
};

const char MSG_SETTINGS_EN[] PROGMEM = "Settings";
const char MSG_SETTINGS_CZ[] PROGMEM = "Nastaveni";
const char MSG_SETTINGS_IT[] PROGMEM = "Impostazioni";
const char MSG_SETTINGS_ES[] PROGMEM = "Ajuste";
const char MSG_SETTINGS_PL[] PROGMEM = "Ustawienia";
const char * const MSG_SETTINGS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SETTINGS_EN,
	MSG_SETTINGS_CZ,
	MSG_SETTINGS_IT,
	MSG_SETTINGS_ES,
	MSG_SETTINGS_PL
};

const char MSG_SET_HOME_OFFSETS_EN[] PROGMEM = "Set home offsets";
const char MSG_SET_HOME_OFFSETS_CZ[] PROGMEM = "Nastav pocatek home";
const char MSG_SET_HOME_OFFSETS_IT[] PROGMEM = "Set home offsets";
const char MSG_SET_HOME_OFFSETS_ES[] PROGMEM = "Set home offsets";
const char MSG_SET_HOME_OFFSETS_PL[] PROGMEM = "Nastav pocatek home";
const char * const MSG_SET_HOME_OFFSETS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SET_HOME_OFFSETS_EN,
	MSG_SET_HOME_OFFSETS_CZ,
	MSG_SET_HOME_OFFSETS_IT,
	MSG_SET_HOME_OFFSETS_ES,
	MSG_SET_HOME_OFFSETS_PL
};

const char MSG_SET_ORIGIN_EN[] PROGMEM = "Set origin";
const char MSG_SET_ORIGIN_CZ[] PROGMEM = "Nastav pocatek";
const char MSG_SET_ORIGIN_IT[] PROGMEM = "Set origin";
const char MSG_SET_ORIGIN_ES[] PROGMEM = "Set origin";
const char MSG_SET_ORIGIN_PL[] PROGMEM = "Nastav pocatek";
const char * const MSG_SET_ORIGIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SET_ORIGIN_EN,
	MSG_SET_ORIGIN_CZ,
	MSG_SET_ORIGIN_IT,
	MSG_SET_ORIGIN_ES,
	MSG_SET_ORIGIN_PL
};

const char MSG_SHOW_END_STOPS_EN[] PROGMEM = "Show end stops";
const char MSG_SHOW_END_STOPS_CZ[] PROGMEM = "Zobraz konc. spinace";
const char MSG_SHOW_END_STOPS_IT[] PROGMEM = "Show end stops";
const char MSG_SHOW_END_STOPS_ES[] PROGMEM = "Show end stops";
const char MSG_SHOW_END_STOPS_PL[] PROGMEM = "Show end stops";
const char * const MSG_SHOW_END_STOPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SHOW_END_STOPS_EN,
	MSG_SHOW_END_STOPS_CZ,
	MSG_SHOW_END_STOPS_IT,
	MSG_SHOW_END_STOPS_ES,
	MSG_SHOW_END_STOPS_PL
};

const char MSG_SILENT_MODE_OFF_EN[] PROGMEM = "Mode [high power]";
const char MSG_SILENT_MODE_OFF_CZ[] PROGMEM = "Mod  [vys. vykon]";
const char MSG_SILENT_MODE_OFF_IT[] PROGMEM = "Modo [piu forza]";
const char MSG_SILENT_MODE_OFF_ES[] PROGMEM = "Modo [mas fuerza]";
const char MSG_SILENT_MODE_OFF_PL[] PROGMEM = "Mod [w wydajnosc]";
const char * const MSG_SILENT_MODE_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SILENT_MODE_OFF_EN,
	MSG_SILENT_MODE_OFF_CZ,
	MSG_SILENT_MODE_OFF_IT,
	MSG_SILENT_MODE_OFF_ES,
	MSG_SILENT_MODE_OFF_PL
};

const char MSG_SILENT_MODE_ON_EN[] PROGMEM = "Mode     [silent]";
const char MSG_SILENT_MODE_ON_CZ[] PROGMEM = "Mod       [tichy]";
const char MSG_SILENT_MODE_ON_IT[] PROGMEM = "Modo     [silenzioso]";
const char MSG_SILENT_MODE_ON_ES[] PROGMEM = "Modo     [silencio]";
const char MSG_SILENT_MODE_ON_PL[] PROGMEM = "Mod       [cichy]";
const char * const MSG_SILENT_MODE_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SILENT_MODE_ON_EN,
	MSG_SILENT_MODE_ON_CZ,
	MSG_SILENT_MODE_ON_IT,
	MSG_SILENT_MODE_ON_ES,
	MSG_SILENT_MODE_ON_PL
};

const char MSG_SOFTWARE_RESET_EN[] PROGMEM = " Software Reset";
const char MSG_SOFTWARE_RESET_CZ[] PROGMEM = " Software Reset";
const char MSG_SOFTWARE_RESET_IT[] PROGMEM = " Software Reset";
const char MSG_SOFTWARE_RESET_ES[] PROGMEM = " Software Reset";
const char MSG_SOFTWARE_RESET_PL[] PROGMEM = " Software Reset";
const char * const MSG_SOFTWARE_RESET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SOFTWARE_RESET_EN,
	MSG_SOFTWARE_RESET_CZ,
	MSG_SOFTWARE_RESET_IT,
	MSG_SOFTWARE_RESET_ES,
	MSG_SOFTWARE_RESET_PL
};

const char MSG_SPEED_EN[] PROGMEM = "Speed";
const char MSG_SPEED_CZ[] PROGMEM = "Rychlost";
const char MSG_SPEED_IT[] PROGMEM = "Velcità";
const char MSG_SPEED_ES[] PROGMEM = "Velocidad";
const char MSG_SPEED_PL[] PROGMEM = "Predkosc";
const char * const MSG_SPEED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SPEED_EN,
	MSG_SPEED_CZ,
	MSG_SPEED_IT,
	MSG_SPEED_ES,
	MSG_SPEED_PL
};

const char MSG_STATISTICS_EN[] PROGMEM = "Statistics  ";
const char MSG_STATISTICS_CZ[] PROGMEM = "Statistika  ";
const char MSG_STATISTICS_IT[] PROGMEM = "Statistiche";
const char MSG_STATISTICS_ES[] PROGMEM = "Estadistica  ";
const char MSG_STATISTICS_PL[] PROGMEM = "Statystyka  ";
const char * const MSG_STATISTICS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATISTICS_EN,
	MSG_STATISTICS_CZ,
	MSG_STATISTICS_IT,
	MSG_STATISTICS_ES,
	MSG_STATISTICS_PL
};

const char MSG_STATS_FILAMENTUSED_EN[] PROGMEM = "Filament used:  ";
const char MSG_STATS_FILAMENTUSED_CZ[] PROGMEM = "Filament :  ";
const char MSG_STATS_FILAMENTUSED_IT[] PROGMEM = "Filamento:";
const char MSG_STATS_FILAMENTUSED_ES[] PROGMEM = "Filamento :  ";
const char MSG_STATS_FILAMENTUSED_PL[] PROGMEM = "Filament :  ";
const char * const MSG_STATS_FILAMENTUSED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_FILAMENTUSED_EN,
	MSG_STATS_FILAMENTUSED_CZ,
	MSG_STATS_FILAMENTUSED_IT,
	MSG_STATS_FILAMENTUSED_ES,
	MSG_STATS_FILAMENTUSED_PL
};

const char MSG_STATS_PRINTTIME_EN[] PROGMEM = "Print time:  ";
const char MSG_STATS_PRINTTIME_CZ[] PROGMEM = "Cas tisku :  ";
const char MSG_STATS_PRINTTIME_IT[] PROGMEM = "Tempo stampa:";
const char MSG_STATS_PRINTTIME_ES[] PROGMEM = "Tiempo de imp.:";
const char MSG_STATS_PRINTTIME_PL[] PROGMEM = "Czas druku :  ";
const char * const MSG_STATS_PRINTTIME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_PRINTTIME_EN,
	MSG_STATS_PRINTTIME_CZ,
	MSG_STATS_PRINTTIME_IT,
	MSG_STATS_PRINTTIME_ES,
	MSG_STATS_PRINTTIME_PL
};

const char MSG_STATS_TOTALFILAMENT_EN[] PROGMEM = "Total filament :";
const char MSG_STATS_TOTALFILAMENT_CZ[] PROGMEM = "Filament celkem :";
const char MSG_STATS_TOTALFILAMENT_IT[] PROGMEM = "Filamento tot:";
const char MSG_STATS_TOTALFILAMENT_ES[] PROGMEM = "Filamento total:";
const char MSG_STATS_TOTALFILAMENT_PL[] PROGMEM = "Filament lacznie :";
const char * const MSG_STATS_TOTALFILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_TOTALFILAMENT_EN,
	MSG_STATS_TOTALFILAMENT_CZ,
	MSG_STATS_TOTALFILAMENT_IT,
	MSG_STATS_TOTALFILAMENT_ES,
	MSG_STATS_TOTALFILAMENT_PL
};

const char MSG_STATS_TOTALPRINTTIME_EN[] PROGMEM = "Total print time :";
const char MSG_STATS_TOTALPRINTTIME_CZ[] PROGMEM = "Celkovy cas :";
const char MSG_STATS_TOTALPRINTTIME_IT[] PROGMEM = "Tempo stampa tot:";
const char MSG_STATS_TOTALPRINTTIME_ES[] PROGMEM = "Tiempo total :";
const char MSG_STATS_TOTALPRINTTIME_PL[] PROGMEM = "Czas calkowity :";
const char * const MSG_STATS_TOTALPRINTTIME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_TOTALPRINTTIME_EN,
	MSG_STATS_TOTALPRINTTIME_CZ,
	MSG_STATS_TOTALPRINTTIME_IT,
	MSG_STATS_TOTALPRINTTIME_ES,
	MSG_STATS_TOTALPRINTTIME_PL
};

const char MSG_STEPPER_TOO_HIGH_EN[] PROGMEM = "Steprate too high: ";
const char MSG_STEPPER_TOO_HIGH_CZ[] PROGMEM = "Steprate too high: ";
const char MSG_STEPPER_TOO_HIGH_IT[] PROGMEM = "Steprate too high: ";
const char MSG_STEPPER_TOO_HIGH_ES[] PROGMEM = "Steprate too high: ";
const char MSG_STEPPER_TOO_HIGH_PL[] PROGMEM = "Steprate too high: ";
const char * const MSG_STEPPER_TOO_HIGH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STEPPER_TOO_HIGH_EN,
	MSG_STEPPER_TOO_HIGH_CZ,
	MSG_STEPPER_TOO_HIGH_IT,
	MSG_STEPPER_TOO_HIGH_ES,
	MSG_STEPPER_TOO_HIGH_PL
};

const char MSG_STOPPED_EN[] PROGMEM = "STOPPED. ";
const char MSG_STOPPED_CZ[] PROGMEM = "STOPPED. ";
const char MSG_STOPPED_IT[] PROGMEM = "ARRESTATO ";
const char MSG_STOPPED_ES[] PROGMEM = "PARADA";
const char MSG_STOPPED_PL[] PROGMEM = "STOPPED. ";
const char * const MSG_STOPPED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STOPPED_EN,
	MSG_STOPPED_CZ,
	MSG_STOPPED_IT,
	MSG_STOPPED_ES,
	MSG_STOPPED_PL
};

const char MSG_STOP_PRINT_EN[] PROGMEM = "Stop print";
const char MSG_STOP_PRINT_CZ[] PROGMEM = "Zastavit tisk";
const char MSG_STOP_PRINT_IT[] PROGMEM = "Arresta stampa";
const char MSG_STOP_PRINT_ES[] PROGMEM = "Detener impresion";
const char MSG_STOP_PRINT_PL[] PROGMEM = "Zatrzymac druk";
const char * const MSG_STOP_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STOP_PRINT_EN,
	MSG_STOP_PRINT_CZ,
	MSG_STOP_PRINT_IT,
	MSG_STOP_PRINT_ES,
	MSG_STOP_PRINT_PL
};

const char MSG_STORE_EPROM_EN[] PROGMEM = "Store memory";
const char MSG_STORE_EPROM_CZ[] PROGMEM = "Store memory";
const char MSG_STORE_EPROM_IT[] PROGMEM = "Store memory";
const char MSG_STORE_EPROM_ES[] PROGMEM = "Store memory";
const char MSG_STORE_EPROM_PL[] PROGMEM = "Store memory";
const char * const MSG_STORE_EPROM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STORE_EPROM_EN,
	MSG_STORE_EPROM_CZ,
	MSG_STORE_EPROM_IT,
	MSG_STORE_EPROM_ES,
	MSG_STORE_EPROM_PL
};

const char MSG_SUPPORT_EN[] PROGMEM = "Support";
const char MSG_SUPPORT_CZ[] PROGMEM = "Podpora";
const char MSG_SUPPORT_IT[] PROGMEM = "Support";
const char MSG_SUPPORT_ES[] PROGMEM = "Support";
const char MSG_SUPPORT_PL[] PROGMEM = "Pomoc";
const char * const MSG_SUPPORT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SUPPORT_EN,
	MSG_SUPPORT_CZ,
	MSG_SUPPORT_IT,
	MSG_SUPPORT_ES,
	MSG_SUPPORT_PL
};

const char MSG_SWITCH_PS_OFF_EN[] PROGMEM = "Switch power off";
const char MSG_SWITCH_PS_OFF_CZ[] PROGMEM = "Zapnout zdroj";
const char MSG_SWITCH_PS_OFF_IT[] PROGMEM = "Switch power off";
const char MSG_SWITCH_PS_OFF_ES[] PROGMEM = "Switch power off";
const char MSG_SWITCH_PS_OFF_PL[] PROGMEM = "Zapnout zdroj";
const char * const MSG_SWITCH_PS_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SWITCH_PS_OFF_EN,
	MSG_SWITCH_PS_OFF_CZ,
	MSG_SWITCH_PS_OFF_IT,
	MSG_SWITCH_PS_OFF_ES,
	MSG_SWITCH_PS_OFF_PL
};

const char MSG_SWITCH_PS_ON_EN[] PROGMEM = "Switch power on";
const char MSG_SWITCH_PS_ON_CZ[] PROGMEM = "Vypnout zdroj";
const char MSG_SWITCH_PS_ON_IT[] PROGMEM = "Switch power on";
const char MSG_SWITCH_PS_ON_ES[] PROGMEM = "Switch power on";
const char MSG_SWITCH_PS_ON_PL[] PROGMEM = "Vypnout zdroj";
const char * const MSG_SWITCH_PS_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SWITCH_PS_ON_EN,
	MSG_SWITCH_PS_ON_CZ,
	MSG_SWITCH_PS_ON_IT,
	MSG_SWITCH_PS_ON_ES,
	MSG_SWITCH_PS_ON_PL
};

const char MSG_TAKE_EFFECT_EN[] PROGMEM = " for take effect";
const char MSG_TAKE_EFFECT_CZ[] PROGMEM = " pro projeveni zmen";
const char MSG_TAKE_EFFECT_IT[] PROGMEM = " per mostrare i camb.";
const char MSG_TAKE_EFFECT_ES[] PROGMEM = "para tomar efecto";
const char MSG_TAKE_EFFECT_PL[] PROGMEM = "wprow. zmian";
const char * const MSG_TAKE_EFFECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TAKE_EFFECT_EN,
	MSG_TAKE_EFFECT_CZ,
	MSG_TAKE_EFFECT_IT,
	MSG_TAKE_EFFECT_ES,
	MSG_TAKE_EFFECT_PL
};

const char MSG_TEMPERATURE_EN[] PROGMEM = "Temperature";
const char MSG_TEMPERATURE_CZ[] PROGMEM = "Teplota";
const char MSG_TEMPERATURE_IT[] PROGMEM = "Temperatura";
const char MSG_TEMPERATURE_ES[] PROGMEM = "Temperatura";
const char MSG_TEMPERATURE_PL[] PROGMEM = "Temperatura";
const char * const MSG_TEMPERATURE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMPERATURE_EN,
	MSG_TEMPERATURE_CZ,
	MSG_TEMPERATURE_IT,
	MSG_TEMPERATURE_ES,
	MSG_TEMPERATURE_PL
};

const char MSG_TUNE_EN[] PROGMEM = "Tune";
const char MSG_TUNE_CZ[] PROGMEM = "Ladit";
const char MSG_TUNE_IT[] PROGMEM = "Adatta";
const char MSG_TUNE_ES[] PROGMEM = "Ajustar";
const char MSG_TUNE_PL[] PROGMEM = "Nastroic";
const char * const MSG_TUNE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TUNE_EN,
	MSG_TUNE_CZ,
	MSG_TUNE_IT,
	MSG_TUNE_ES,
	MSG_TUNE_PL
};

const char MSG_UNKNOWN_COMMAND_EN[] PROGMEM = "Unknown command: \"";
const char MSG_UNKNOWN_COMMAND_CZ[] PROGMEM = "Unknown command: \"";
const char MSG_UNKNOWN_COMMAND_IT[] PROGMEM = "Unknown command: \"";
const char MSG_UNKNOWN_COMMAND_ES[] PROGMEM = "Unknown command: \"";
const char MSG_UNKNOWN_COMMAND_PL[] PROGMEM = "Unknown command: \"";
const char * const MSG_UNKNOWN_COMMAND_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNKNOWN_COMMAND_EN,
	MSG_UNKNOWN_COMMAND_CZ,
	MSG_UNKNOWN_COMMAND_IT,
	MSG_UNKNOWN_COMMAND_ES,
	MSG_UNKNOWN_COMMAND_PL
};

const char MSG_UNLOAD_FILAMENT_EN[] PROGMEM = "Unload filament";
const char MSG_UNLOAD_FILAMENT_CZ[] PROGMEM = "Vyjmout filament";
const char MSG_UNLOAD_FILAMENT_IT[] PROGMEM = "Scaricare fil.";
const char MSG_UNLOAD_FILAMENT_ES[] PROGMEM = "Sacar filamento";
const char MSG_UNLOAD_FILAMENT_PL[] PROGMEM = "Wyjac filament";
const char * const MSG_UNLOAD_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_EN,
	MSG_UNLOAD_FILAMENT_CZ,
	MSG_UNLOAD_FILAMENT_IT,
	MSG_UNLOAD_FILAMENT_ES,
	MSG_UNLOAD_FILAMENT_PL
};

const char MSG_USB_PRINTING_EN[] PROGMEM = "USB printing  ";
const char MSG_USB_PRINTING_CZ[] PROGMEM = "Tisk z USB  ";
const char MSG_USB_PRINTING_IT[] PROGMEM = "Stampa da USB";
const char MSG_USB_PRINTING_ES[] PROGMEM = "Impresion de USB ";
const char MSG_USB_PRINTING_PL[] PROGMEM = "Druk z USB  ";
const char * const MSG_USB_PRINTING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_USB_PRINTING_EN,
	MSG_USB_PRINTING_CZ,
	MSG_USB_PRINTING_IT,
	MSG_USB_PRINTING_ES,
	MSG_USB_PRINTING_PL
};

const char MSG_USERWAIT_EN[] PROGMEM = "Wait for user...";
const char MSG_USERWAIT_CZ[] PROGMEM = "Wait for user...";
const char MSG_USERWAIT_IT[] PROGMEM = "Attendi Utente...";
const char MSG_USERWAIT_ES[] PROGMEM = "Esperando ordenes";
const char MSG_USERWAIT_PL[] PROGMEM = "Wait for user...";
const char * const MSG_USERWAIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_USERWAIT_EN,
	MSG_USERWAIT_CZ,
	MSG_USERWAIT_IT,
	MSG_USERWAIT_ES,
	MSG_USERWAIT_PL
};

const char MSG_VE_JERK_EN[] PROGMEM = "Ve-jerk";
const char MSG_VE_JERK_CZ[] PROGMEM = "Ve-jerk";
const char MSG_VE_JERK_IT[] PROGMEM = "Ve-jerk";
const char MSG_VE_JERK_ES[] PROGMEM = "Ve-jerk";
const char MSG_VE_JERK_PL[] PROGMEM = "Ve-jerk";
const char * const MSG_VE_JERK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_VE_JERK_EN,
	MSG_VE_JERK_CZ,
	MSG_VE_JERK_IT,
	MSG_VE_JERK_ES,
	MSG_VE_JERK_PL
};

const char MSG_VMAX_EN[] PROGMEM = "Vmax ";
const char MSG_VMAX_CZ[] PROGMEM = "Vmax ";
const char MSG_VMAX_IT[] PROGMEM = "Vmax ";
const char MSG_VMAX_ES[] PROGMEM = "Vmax ";
const char MSG_VMAX_PL[] PROGMEM = "Vmax ";
const char * const MSG_VMAX_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_VMAX_EN,
	MSG_VMAX_CZ,
	MSG_VMAX_IT,
	MSG_VMAX_ES,
	MSG_VMAX_PL
};

const char MSG_VMIN_EN[] PROGMEM = "Vmin";
const char MSG_VMIN_CZ[] PROGMEM = "Vmin";
const char MSG_VMIN_IT[] PROGMEM = "Vmin";
const char MSG_VMIN_ES[] PROGMEM = "Vmin";
const char MSG_VMIN_PL[] PROGMEM = "Vmin";
const char * const MSG_VMIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_VMIN_EN,
	MSG_VMIN_CZ,
	MSG_VMIN_IT,
	MSG_VMIN_ES,
	MSG_VMIN_PL
};

const char MSG_VOLUMETRIC_EN[] PROGMEM = "Filament";
const char MSG_VOLUMETRIC_CZ[] PROGMEM = "Filament";
const char MSG_VOLUMETRIC_IT[] PROGMEM = "Filament";
const char MSG_VOLUMETRIC_ES[] PROGMEM = "Filament";
const char MSG_VOLUMETRIC_PL[] PROGMEM = "Filament";
const char * const MSG_VOLUMETRIC_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_VOLUMETRIC_EN,
	MSG_VOLUMETRIC_CZ,
	MSG_VOLUMETRIC_IT,
	MSG_VOLUMETRIC_ES,
	MSG_VOLUMETRIC_PL
};

const char MSG_VOLUMETRIC_ENABLED_EN[] PROGMEM = "E in mm3";
const char MSG_VOLUMETRIC_ENABLED_CZ[] PROGMEM = "E in mm3";
const char MSG_VOLUMETRIC_ENABLED_IT[] PROGMEM = "E in mm3";
const char MSG_VOLUMETRIC_ENABLED_ES[] PROGMEM = "E in mm3";
const char MSG_VOLUMETRIC_ENABLED_PL[] PROGMEM = "E in mm3";
const char * const MSG_VOLUMETRIC_ENABLED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_VOLUMETRIC_ENABLED_EN,
	MSG_VOLUMETRIC_ENABLED_CZ,
	MSG_VOLUMETRIC_ENABLED_IT,
	MSG_VOLUMETRIC_ENABLED_ES,
	MSG_VOLUMETRIC_ENABLED_PL
};

const char MSG_VTRAV_MIN_EN[] PROGMEM = "VTrav min";
const char MSG_VTRAV_MIN_CZ[] PROGMEM = "VTrav min";
const char MSG_VTRAV_MIN_IT[] PROGMEM = "VTrav min";
const char MSG_VTRAV_MIN_ES[] PROGMEM = "VTrav min";
const char MSG_VTRAV_MIN_PL[] PROGMEM = "VTrav min";
const char * const MSG_VTRAV_MIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_VTRAV_MIN_EN,
	MSG_VTRAV_MIN_CZ,
	MSG_VTRAV_MIN_IT,
	MSG_VTRAV_MIN_ES,
	MSG_VTRAV_MIN_PL
};

const char MSG_VXY_JERK_EN[] PROGMEM = "Vxy-jerk";
const char MSG_VXY_JERK_CZ[] PROGMEM = "Vxy-jerk";
const char MSG_VXY_JERK_IT[] PROGMEM = "Vxy-jerk";
const char MSG_VXY_JERK_ES[] PROGMEM = "Vxy-jerk";
const char MSG_VXY_JERK_PL[] PROGMEM = "Vxy-jerk";
const char * const MSG_VXY_JERK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_VXY_JERK_EN,
	MSG_VXY_JERK_CZ,
	MSG_VXY_JERK_IT,
	MSG_VXY_JERK_ES,
	MSG_VXY_JERK_PL
};

const char MSG_VZ_JERK_EN[] PROGMEM = "Vz-jerk";
const char MSG_VZ_JERK_CZ[] PROGMEM = "Vz-jerk";
const char MSG_VZ_JERK_IT[] PROGMEM = "Vz-jerk";
const char MSG_VZ_JERK_ES[] PROGMEM = "Vz-jerk";
const char MSG_VZ_JERK_PL[] PROGMEM = "Vz-jerk";
const char * const MSG_VZ_JERK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_VZ_JERK_EN,
	MSG_VZ_JERK_CZ,
	MSG_VZ_JERK_IT,
	MSG_VZ_JERK_ES,
	MSG_VZ_JERK_PL
};

const char MSG_WATCH_EN[] PROGMEM = "Info screen";
const char MSG_WATCH_CZ[] PROGMEM = "Informace";
const char MSG_WATCH_IT[] PROGMEM = "Guarda";
const char MSG_WATCH_ES[] PROGMEM = "Monitorizar";
const char MSG_WATCH_PL[] PROGMEM = "Informacje";
const char * const MSG_WATCH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WATCH_EN,
	MSG_WATCH_CZ,
	MSG_WATCH_IT,
	MSG_WATCH_ES,
	MSG_WATCH_PL
};

const char MSG_WATCHDOG_RESET_EN[] PROGMEM = " Watchdog Reset";
const char MSG_WATCHDOG_RESET_CZ[] PROGMEM = " Watchdog Reset";
const char MSG_WATCHDOG_RESET_IT[] PROGMEM = " Watchdog Reset";
const char MSG_WATCHDOG_RESET_ES[] PROGMEM = " Watchdog Reset";
const char MSG_WATCHDOG_RESET_PL[] PROGMEM = " Watchdog Reset";
const char * const MSG_WATCHDOG_RESET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WATCHDOG_RESET_EN,
	MSG_WATCHDOG_RESET_CZ,
	MSG_WATCHDOG_RESET_IT,
	MSG_WATCHDOG_RESET_ES,
	MSG_WATCHDOG_RESET_PL
};

const char MSG_X_EN[] PROGMEM = "x";
const char MSG_X_CZ[] PROGMEM = "x";
const char MSG_X_IT[] PROGMEM = "x";
const char MSG_X_ES[] PROGMEM = "x";
const char MSG_X_PL[] PROGMEM = "x";
const char * const MSG_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_X_EN,
	MSG_X_CZ,
	MSG_X_IT,
	MSG_X_ES,
	MSG_X_PL
};

const char MSG_XSTEPS_EN[] PROGMEM = "Xsteps/mm";
const char MSG_XSTEPS_CZ[] PROGMEM = "Xsteps/mm";
const char MSG_XSTEPS_IT[] PROGMEM = "Xsteps/mm";
const char MSG_XSTEPS_ES[] PROGMEM = "Xsteps/mm";
const char MSG_XSTEPS_PL[] PROGMEM = "Xsteps/mm";
const char * const MSG_XSTEPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_XSTEPS_EN,
	MSG_XSTEPS_CZ,
	MSG_XSTEPS_IT,
	MSG_XSTEPS_ES,
	MSG_XSTEPS_PL
};

const char MSG_X_MAX_EN[] PROGMEM = "x_max: ";
const char MSG_X_MAX_CZ[] PROGMEM = "x_max: ";
const char MSG_X_MAX_IT[] PROGMEM = "x_max: ";
const char MSG_X_MAX_ES[] PROGMEM = "x_max: ";
const char MSG_X_MAX_PL[] PROGMEM = "x_max: ";
const char * const MSG_X_MAX_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_X_MAX_EN,
	MSG_X_MAX_CZ,
	MSG_X_MAX_IT,
	MSG_X_MAX_ES,
	MSG_X_MAX_PL
};

const char MSG_X_MIN_EN[] PROGMEM = "x_min: ";
const char MSG_X_MIN_CZ[] PROGMEM = "x_min: ";
const char MSG_X_MIN_IT[] PROGMEM = "x_min: ";
const char MSG_X_MIN_ES[] PROGMEM = "x_min: ";
const char MSG_X_MIN_PL[] PROGMEM = "x_min: ";
const char * const MSG_X_MIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_X_MIN_EN,
	MSG_X_MIN_CZ,
	MSG_X_MIN_IT,
	MSG_X_MIN_ES,
	MSG_X_MIN_PL
};

const char MSG_Y_EN[] PROGMEM = "y";
const char MSG_Y_CZ[] PROGMEM = "y";
const char MSG_Y_IT[] PROGMEM = "y";
const char MSG_Y_ES[] PROGMEM = "y";
const char MSG_Y_PL[] PROGMEM = "y";
const char * const MSG_Y_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_Y_EN,
	MSG_Y_CZ,
	MSG_Y_IT,
	MSG_Y_ES,
	MSG_Y_PL
};

const char MSG_YES_EN[] PROGMEM = "Yes";
const char MSG_YES_CZ[] PROGMEM = "Ano";
const char MSG_YES_IT[] PROGMEM = "Si";
const char MSG_YES_ES[] PROGMEM = "Si";
const char MSG_YES_PL[] PROGMEM = "Tak";
const char * const MSG_YES_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_YES_EN,
	MSG_YES_CZ,
	MSG_YES_IT,
	MSG_YES_ES,
	MSG_YES_PL
};

const char MSG_YSTEPS_EN[] PROGMEM = "Ysteps/mm";
const char MSG_YSTEPS_CZ[] PROGMEM = "Ysteps/mm";
const char MSG_YSTEPS_IT[] PROGMEM = "Ysteps/mm";
const char MSG_YSTEPS_ES[] PROGMEM = "Ysteps/mm";
const char MSG_YSTEPS_PL[] PROGMEM = "Ysteps/mm";
const char * const MSG_YSTEPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_YSTEPS_EN,
	MSG_YSTEPS_CZ,
	MSG_YSTEPS_IT,
	MSG_YSTEPS_ES,
	MSG_YSTEPS_PL
};

const char MSG_Y_MAX_EN[] PROGMEM = "y_max: ";
const char MSG_Y_MAX_CZ[] PROGMEM = "y_max: ";
const char MSG_Y_MAX_IT[] PROGMEM = "y_max: ";
const char MSG_Y_MAX_ES[] PROGMEM = "y_max: ";
const char MSG_Y_MAX_PL[] PROGMEM = "y_max: ";
const char * const MSG_Y_MAX_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_Y_MAX_EN,
	MSG_Y_MAX_CZ,
	MSG_Y_MAX_IT,
	MSG_Y_MAX_ES,
	MSG_Y_MAX_PL
};

const char MSG_Y_MIN_EN[] PROGMEM = "y_min: ";
const char MSG_Y_MIN_CZ[] PROGMEM = "y_min: ";
const char MSG_Y_MIN_IT[] PROGMEM = "y_min: ";
const char MSG_Y_MIN_ES[] PROGMEM = "y_min: ";
const char MSG_Y_MIN_PL[] PROGMEM = "y_min: ";
const char * const MSG_Y_MIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_Y_MIN_EN,
	MSG_Y_MIN_CZ,
	MSG_Y_MIN_IT,
	MSG_Y_MIN_ES,
	MSG_Y_MIN_PL
};

const char MSG_Z_EN[] PROGMEM = "z";
const char MSG_Z_CZ[] PROGMEM = "z";
const char MSG_Z_IT[] PROGMEM = "z";
const char MSG_Z_ES[] PROGMEM = "z";
const char MSG_Z_PL[] PROGMEM = "z";
const char * const MSG_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_Z_EN,
	MSG_Z_CZ,
	MSG_Z_IT,
	MSG_Z_ES,
	MSG_Z_PL
};

const char MSG_ZPROBE_OUT_EN[] PROGMEM = "Z probe out. bed";
const char MSG_ZPROBE_OUT_CZ[] PROGMEM = "Z probe out. bed";
const char MSG_ZPROBE_OUT_IT[] PROGMEM = "Z probe out. bed";
const char MSG_ZPROBE_OUT_ES[] PROGMEM = "Z probe out. bed";
const char MSG_ZPROBE_OUT_PL[] PROGMEM = "Z probe out. bed";
const char * const MSG_ZPROBE_OUT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ZPROBE_OUT_EN,
	MSG_ZPROBE_OUT_CZ,
	MSG_ZPROBE_OUT_IT,
	MSG_ZPROBE_OUT_ES,
	MSG_ZPROBE_OUT_PL
};

const char MSG_ZPROBE_ZOFFSET_EN[] PROGMEM = "Z Offset";
const char MSG_ZPROBE_ZOFFSET_CZ[] PROGMEM = "Z Offset";
const char MSG_ZPROBE_ZOFFSET_IT[] PROGMEM = "Z Offset";
const char MSG_ZPROBE_ZOFFSET_ES[] PROGMEM = "Z Offset";
const char MSG_ZPROBE_ZOFFSET_PL[] PROGMEM = "Z Offset";
const char * const MSG_ZPROBE_ZOFFSET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ZPROBE_ZOFFSET_EN,
	MSG_ZPROBE_ZOFFSET_CZ,
	MSG_ZPROBE_ZOFFSET_IT,
	MSG_ZPROBE_ZOFFSET_ES,
	MSG_ZPROBE_ZOFFSET_PL
};

const char MSG_ZSTEPS_EN[] PROGMEM = "Zsteps/mm";
const char MSG_ZSTEPS_CZ[] PROGMEM = "Zsteps/mm";
const char MSG_ZSTEPS_IT[] PROGMEM = "Zsteps/mm";
const char MSG_ZSTEPS_ES[] PROGMEM = "Zsteps/mm";
const char MSG_ZSTEPS_PL[] PROGMEM = "Zsteps/mm";
const char * const MSG_ZSTEPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ZSTEPS_EN,
	MSG_ZSTEPS_CZ,
	MSG_ZSTEPS_IT,
	MSG_ZSTEPS_ES,
	MSG_ZSTEPS_PL
};

const char MSG_Z_MAX_EN[] PROGMEM = "z_max: ";
const char MSG_Z_MAX_CZ[] PROGMEM = "z_max: ";
const char MSG_Z_MAX_IT[] PROGMEM = "z_max: ";
const char MSG_Z_MAX_ES[] PROGMEM = "z_max: ";
const char MSG_Z_MAX_PL[] PROGMEM = "z_max: ";
const char * const MSG_Z_MAX_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_Z_MAX_EN,
	MSG_Z_MAX_CZ,
	MSG_Z_MAX_IT,
	MSG_Z_MAX_ES,
	MSG_Z_MAX_PL
};

const char MSG_Z_MIN_EN[] PROGMEM = "z_min: ";
const char MSG_Z_MIN_CZ[] PROGMEM = "z_min: ";
const char MSG_Z_MIN_IT[] PROGMEM = "z_min: ";
const char MSG_Z_MIN_ES[] PROGMEM = "z_min: ";
const char MSG_Z_MIN_PL[] PROGMEM = "z_min: ";
const char * const MSG_Z_MIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_Z_MIN_EN,
	MSG_Z_MIN_CZ,
	MSG_Z_MIN_IT,
	MSG_Z_MIN_ES,
	MSG_Z_MIN_PL
};

const char WELCOME_MSG_EN[] PROGMEM = CUSTOM_MENDEL_NAME " ready.";
const char WELCOME_MSG_CZ[] PROGMEM = CUSTOM_MENDEL_NAME " ok";
const char WELCOME_MSG_IT[] PROGMEM = CUSTOM_MENDEL_NAME " pronto.";
const char WELCOME_MSG_ES[] PROGMEM = CUSTOM_MENDEL_NAME " lista";
const char WELCOME_MSG_PL[] PROGMEM = CUSTOM_MENDEL_NAME " gotowa";
const char * const WELCOME_MSG_LANG_TABLE[LANG_NUM] PROGMEM = {
	WELCOME_MSG_EN,
	WELCOME_MSG_CZ,
	WELCOME_MSG_IT,
	WELCOME_MSG_ES,
	WELCOME_MSG_PL
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
