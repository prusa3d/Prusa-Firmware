
#include "Configuration_prusa.h"
#include "language_all.h"

#define LCD_WIDTH 20
extern unsigned char lang_selected;

const char MSG_ACTIVE_EXTRUDER_EN[] PROGMEM = "Active Extruder: ";
const char * const MSG_ACTIVE_EXTRUDER_LANG_TABLE[1] PROGMEM = {
	MSG_ACTIVE_EXTRUDER_EN
};

const char MSG_ADJUSTZ_EN[] PROGMEM = "Auto adjust Z?";
const char MSG_ADJUSTZ_CZ[] PROGMEM = "Auto doladit Z ?";
const char * const MSG_ADJUSTZ_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ADJUSTZ_EN,
	MSG_ADJUSTZ_CZ
};

const char MSG_ALL_EN[] PROGMEM = "All";
const char MSG_ALL_CZ[] PROGMEM = "Vse";
const char * const MSG_ALL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ALL_EN,
	MSG_ALL_CZ
};

const char MSG_AMAX_EN[] PROGMEM = "Amax ";
const char * const MSG_AMAX_LANG_TABLE[1] PROGMEM = {
	MSG_AMAX_EN
};

const char MSG_AUTHOR_EN[] PROGMEM = " | Author: ";
const char * const MSG_AUTHOR_LANG_TABLE[1] PROGMEM = {
	MSG_AUTHOR_EN
};

const char MSG_AUTOLOADING_ENABLED_EN[] PROGMEM = "Autoloading filament is active, just press the knob and insert filament...";
const char MSG_AUTOLOADING_ENABLED_CZ[] PROGMEM = "Automaticke zavadeni filamentu aktivni, stisknete tlacitko a vlozte filament...";
const char * const MSG_AUTOLOADING_ENABLED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AUTOLOADING_ENABLED_EN,
	MSG_AUTOLOADING_ENABLED_CZ
};

const char MSG_AUTOLOADING_ONLY_IF_FSENS_ON_EN[] PROGMEM = "Autoloading filament available only when filament sensor is turned on...";
const char MSG_AUTOLOADING_ONLY_IF_FSENS_ON_CZ[] PROGMEM = "Automaticke zavadeni filamentu dostupne pouze pri zapnutem filament senzoru...";
const char * const MSG_AUTOLOADING_ONLY_IF_FSENS_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AUTOLOADING_ONLY_IF_FSENS_ON_EN,
	MSG_AUTOLOADING_ONLY_IF_FSENS_ON_CZ
};

const char MSG_AUTOLOAD_FILAMENT_EN[] PROGMEM = "AutoLoad filament";
const char MSG_AUTOLOAD_FILAMENT_CZ[] PROGMEM = "AutoZavedeni fil.";
const char * const MSG_AUTOLOAD_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AUTOLOAD_FILAMENT_EN,
	MSG_AUTOLOAD_FILAMENT_CZ
};

const char MSG_AUTO_HOME_EN[] PROGMEM = "Auto home";
const char * const MSG_AUTO_HOME_LANG_TABLE[1] PROGMEM = {
	MSG_AUTO_HOME_EN
};

const char MSG_AUTO_MODE_ON_EN[] PROGMEM = "Mode [auto power]";
const char MSG_AUTO_MODE_ON_CZ[] PROGMEM = "Mod [automaticky]";
const char * const MSG_AUTO_MODE_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AUTO_MODE_ON_EN,
	MSG_AUTO_MODE_ON_CZ
};

const char MSG_A_RETRACT_EN[] PROGMEM = "A-retract";
const char * const MSG_A_RETRACT_LANG_TABLE[1] PROGMEM = {
	MSG_A_RETRACT_EN
};

const char MSG_BABYSTEPPING_X_EN[] PROGMEM = "Babystepping X";
const char * const MSG_BABYSTEPPING_X_LANG_TABLE[1] PROGMEM = {
	MSG_BABYSTEPPING_X_EN
};

const char MSG_BABYSTEPPING_Y_EN[] PROGMEM = "Babystepping Y";
const char * const MSG_BABYSTEPPING_Y_LANG_TABLE[1] PROGMEM = {
	MSG_BABYSTEPPING_Y_EN
};

const char MSG_BABYSTEPPING_Z_EN[] PROGMEM = "Adjusting Z";
const char * const MSG_BABYSTEPPING_Z_LANG_TABLE[1] PROGMEM = {
	MSG_BABYSTEPPING_Z_EN
};

const char MSG_BABYSTEP_X_EN[] PROGMEM = "Babystep X";
const char * const MSG_BABYSTEP_X_LANG_TABLE[1] PROGMEM = {
	MSG_BABYSTEP_X_EN
};

const char MSG_BABYSTEP_Y_EN[] PROGMEM = "Babystep Y";
const char * const MSG_BABYSTEP_Y_LANG_TABLE[1] PROGMEM = {
	MSG_BABYSTEP_Y_EN
};

const char MSG_BABYSTEP_Z_EN[] PROGMEM = "Live adjust Z";
const char MSG_BABYSTEP_Z_CZ[] PROGMEM = "Doladeni osy Z";
const char * const MSG_BABYSTEP_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEP_Z_EN,
	MSG_BABYSTEP_Z_CZ
};

const char MSG_BABYSTEP_Z_NOT_SET_EN[] PROGMEM = "Distance between tip of the nozzle and the bed surface has not been set yet. Please follow the manual, chapter First steps, section First layer calibration.";
const char MSG_BABYSTEP_Z_NOT_SET_CZ[] PROGMEM = "Neni zkalibrovana vzdalenost trysky od tiskove podlozky. Postupujte prosim podle manualu, kapitola Zaciname, odstavec Nastaveni prvni vrstvy.";
const char * const MSG_BABYSTEP_Z_NOT_SET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEP_Z_NOT_SET_EN,
	MSG_BABYSTEP_Z_NOT_SET_CZ
};

const char MSG_BED_EN[] PROGMEM = "Bed";
const char * const MSG_BED_LANG_TABLE[1] PROGMEM = {
	MSG_BED_EN
};

const char MSG_BED_CORRECTION_FRONT_EN[] PROGMEM = "Front side[um]";
const char MSG_BED_CORRECTION_FRONT_CZ[] PROGMEM = "Vpredu [um]";
const char * const MSG_BED_CORRECTION_FRONT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_FRONT_EN,
	MSG_BED_CORRECTION_FRONT_CZ
};

const char MSG_BED_CORRECTION_LEFT_EN[] PROGMEM = "Left side [um]";
const char MSG_BED_CORRECTION_LEFT_CZ[] PROGMEM = "Vlevo  [um]";
const char * const MSG_BED_CORRECTION_LEFT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_LEFT_EN,
	MSG_BED_CORRECTION_LEFT_CZ
};

const char MSG_BED_CORRECTION_MENU_EN[] PROGMEM = "Bed level correct";
const char MSG_BED_CORRECTION_MENU_CZ[] PROGMEM = "Korekce podlozky";
const char * const MSG_BED_CORRECTION_MENU_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_MENU_EN,
	MSG_BED_CORRECTION_MENU_CZ
};

const char MSG_BED_CORRECTION_REAR_EN[] PROGMEM = "Rear side [um]";
const char MSG_BED_CORRECTION_REAR_CZ[] PROGMEM = "Vzadu  [um]";
const char * const MSG_BED_CORRECTION_REAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_REAR_EN,
	MSG_BED_CORRECTION_REAR_CZ
};

const char MSG_BED_CORRECTION_RESET_EN[] PROGMEM = "Reset";
const char * const MSG_BED_CORRECTION_RESET_LANG_TABLE[1] PROGMEM = {
	MSG_BED_CORRECTION_RESET_EN
};

const char MSG_BED_CORRECTION_RIGHT_EN[] PROGMEM = "Right side[um]";
const char MSG_BED_CORRECTION_RIGHT_CZ[] PROGMEM = "Vpravo [um]";
const char * const MSG_BED_CORRECTION_RIGHT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_RIGHT_EN,
	MSG_BED_CORRECTION_RIGHT_CZ
};

const char MSG_BED_DONE_EN[] PROGMEM = "Bed done";
const char MSG_BED_DONE_CZ[] PROGMEM = "Bed OK.";
const char * const MSG_BED_DONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_DONE_EN,
	MSG_BED_DONE_CZ
};

const char MSG_BED_HEATING_EN[] PROGMEM = "Bed Heating";
const char MSG_BED_HEATING_CZ[] PROGMEM = "Zahrivani bed";
const char * const MSG_BED_HEATING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_HEATING_EN,
	MSG_BED_HEATING_CZ
};

const char MSG_BED_LEVELING_FAILED_POINT_HIGH_EN[] PROGMEM = "Bed leveling failed. Sensor triggered too high. Waiting for reset.";
const char MSG_BED_LEVELING_FAILED_POINT_HIGH_CZ[] PROGMEM = "Kalibrace Z selhala. Sensor sepnul prilis vysoko. Cekam na reset.";
const char * const MSG_BED_LEVELING_FAILED_POINT_HIGH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_LEVELING_FAILED_POINT_HIGH_EN,
	MSG_BED_LEVELING_FAILED_POINT_HIGH_CZ
};

const char MSG_BED_LEVELING_FAILED_POINT_LOW_EN[] PROGMEM = "Bed leveling failed. Sensor didnt trigger. Debris on nozzle? Waiting for reset.";
const char MSG_BED_LEVELING_FAILED_POINT_LOW_CZ[] PROGMEM = "Kalibrace Z selhala. Sensor nesepnul. Znecistena tryska? Cekam na reset.";
const char * const MSG_BED_LEVELING_FAILED_POINT_LOW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_LEVELING_FAILED_POINT_LOW_EN,
	MSG_BED_LEVELING_FAILED_POINT_LOW_CZ
};

const char MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_EN[] PROGMEM = "Bed leveling failed. Sensor disconnected or cable broken. Waiting for reset.";
const char MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_CZ[] PROGMEM = "Kalibrace Z selhala. Sensor je odpojeny nebo preruseny kabel. Cekam na reset.";
const char * const MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_EN,
	MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_CZ
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_EN[] PROGMEM = "XYZ calibration failed. Front calibration points not reachable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_CZ[] PROGMEM = "Kalibrace XYZ selhala. Predni kalibracni body moc vpredu. Srovnejte tiskarnu.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_CZ
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_EN[] PROGMEM = "XYZ calibration failed. Left front calibration point not reachable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_CZ[] PROGMEM = "Kalibrace XYZ selhala. Levy predni bod moc vpredu. Srovnejte tiskarnu.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_CZ
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_EN[] PROGMEM = "XYZ calibration failed. Right front calibration point not reachable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_CZ[] PROGMEM = "Kalibrace XYZ selhala. Pravy predni bod moc vpredu. Srovnejte tiskarnu.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_CZ
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_EN[] PROGMEM = "XYZ calibration failed. Please consult the manual.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_CZ[] PROGMEM = "Kalibrace XYZ selhala. Nahlednete do manualu.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_CZ
};

const char MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_EN[] PROGMEM = "XYZ calibration ok. X/Y axes are perpendicular. Congratulations!";
const char MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_CZ[] PROGMEM = "Kalibrace XYZ v poradku. X/Y osy jsou kolme. Gratuluji!";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_CZ
};

const char MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_EN[] PROGMEM = "XYZ calibration failed. Bed calibration point was not found.";
const char MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_CZ[] PROGMEM = "Kalibrace XYZ selhala. Kalibracni bod podlozky nenalezen.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_CZ
};

const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_EN[] PROGMEM = "XYZ calibration all right. Skew will be corrected automatically.";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_CZ[] PROGMEM = "Kalibrace XYZ v poradku. Zkoseni bude automaticky vyrovnano pri tisku.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_CZ
};

const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_EN[] PROGMEM = "XYZ calibration all right. X/Y axes are slightly skewed. Good job!";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_CZ[] PROGMEM = "Kalibrace XYZ v poradku. X/Y osy mirne zkosene. Dobra prace!";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_CZ
};

const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_EN[] PROGMEM = "XYZ calibration compromised. Front calibration points not reachable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_CZ[] PROGMEM = "Kalibrace XYZ nepresna. Predni kalibracni body moc vpredu.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_CZ
};

const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_EN[] PROGMEM = "XYZ calibration compromised. Left front calibration point not reachable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_CZ[] PROGMEM = "Kalibrace XYZ nepresna. Levy predni bod moc vpredu.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_CZ
};

const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_EN[] PROGMEM = "XYZ calibration compromised. Right front calibration point not reachable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_CZ[] PROGMEM = "Kalibrace XYZ nepresna. Pravy predni bod moc vpredu.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_CZ
};

const char MSG_BEGIN_FILE_LIST_EN[] PROGMEM = "Begin file list";
const char * const MSG_BEGIN_FILE_LIST_LANG_TABLE[1] PROGMEM = {
	MSG_BEGIN_FILE_LIST_EN
};

const char MSG_BROWNOUT_RESET_EN[] PROGMEM = " Brown out Reset";
const char * const MSG_BROWNOUT_RESET_LANG_TABLE[1] PROGMEM = {
	MSG_BROWNOUT_RESET_EN
};

const char MSG_CALIBRATE_BED_EN[] PROGMEM = "Calibrate XYZ";
const char MSG_CALIBRATE_BED_CZ[] PROGMEM = "Kalibrace XYZ";
const char * const MSG_CALIBRATE_BED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_BED_EN,
	MSG_CALIBRATE_BED_CZ
};

const char MSG_CALIBRATE_BED_RESET_EN[] PROGMEM = "Reset XYZ calibr.";
const char MSG_CALIBRATE_BED_RESET_CZ[] PROGMEM = "Reset XYZ kalibr.";
const char * const MSG_CALIBRATE_BED_RESET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_BED_RESET_EN,
	MSG_CALIBRATE_BED_RESET_CZ
};

const char MSG_CALIBRATE_E_EN[] PROGMEM = "Calibrate E";
const char MSG_CALIBRATE_E_CZ[] PROGMEM = "Kalibrovat E";
const char * const MSG_CALIBRATE_E_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_E_EN,
	MSG_CALIBRATE_E_CZ
};

const char MSG_CALIBRATE_PINDA_EN[] PROGMEM = "Calibrate";
const char MSG_CALIBRATE_PINDA_CZ[] PROGMEM = "Zkalibrovat";
const char * const MSG_CALIBRATE_PINDA_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_PINDA_EN,
	MSG_CALIBRATE_PINDA_CZ
};

const char MSG_CALIBRATE_Z_AUTO_EN[] PROGMEM = "Calibrating Z";
const char MSG_CALIBRATE_Z_AUTO_CZ[] PROGMEM = "Kalibruji Z";
const char * const MSG_CALIBRATE_Z_AUTO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_Z_AUTO_EN,
	MSG_CALIBRATE_Z_AUTO_CZ
};

const char MSG_CALIBRATION_PINDA_MENU_EN[] PROGMEM = "Temp. calibration";
const char MSG_CALIBRATION_PINDA_MENU_CZ[] PROGMEM = "Teplot. kalibrace";
const char * const MSG_CALIBRATION_PINDA_MENU_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATION_PINDA_MENU_EN,
	MSG_CALIBRATION_PINDA_MENU_CZ
};

const char MSG_CARD_MENU_EN[] PROGMEM = "Print from SD";
const char MSG_CARD_MENU_CZ[] PROGMEM = "Tisk z SD";
const char * const MSG_CARD_MENU_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CARD_MENU_EN,
	MSG_CARD_MENU_CZ
};

const char MSG_CHANGED_BOTH_EN[] PROGMEM = "Warning: both printer type and motherboard type changed.";
const char MSG_CHANGED_BOTH_CZ[] PROGMEM = "Varovani: doslo ke zmene typu tiskarny a motherboardu.";
const char * const MSG_CHANGED_BOTH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHANGED_BOTH_EN,
	MSG_CHANGED_BOTH_CZ
};

const char MSG_CHANGED_MOTHERBOARD_EN[] PROGMEM = "Warning: motherboard type changed.";
const char MSG_CHANGED_MOTHERBOARD_CZ[] PROGMEM = "Varovani: doslo ke zmene typu motherboardu.";
const char * const MSG_CHANGED_MOTHERBOARD_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHANGED_MOTHERBOARD_EN,
	MSG_CHANGED_MOTHERBOARD_CZ
};

const char MSG_CHANGED_PRINTER_EN[] PROGMEM = "Warning: printer type changed.";
const char MSG_CHANGED_PRINTER_CZ[] PROGMEM = "Varovani: doslo ke zmene typu tiskarny.";
const char * const MSG_CHANGED_PRINTER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHANGED_PRINTER_EN,
	MSG_CHANGED_PRINTER_CZ
};

const char MSG_CHANGE_EXTR_EN[] PROGMEM = "Change extruder";
const char MSG_CHANGE_EXTR_CZ[] PROGMEM = "Zmenit extruder";
const char * const MSG_CHANGE_EXTR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHANGE_EXTR_EN,
	MSG_CHANGE_EXTR_CZ
};

const char MSG_CHANGE_SUCCESS_EN[] PROGMEM = "Change success!";
const char MSG_CHANGE_SUCCESS_CZ[] PROGMEM = "Zmena uspesna!";
const char * const MSG_CHANGE_SUCCESS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHANGE_SUCCESS_EN,
	MSG_CHANGE_SUCCESS_CZ
};

const char MSG_CHANGING_FILAMENT_EN[] PROGMEM = "Changing filament!";
const char MSG_CHANGING_FILAMENT_CZ[] PROGMEM = "Vymena filamentu!";
const char * const MSG_CHANGING_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHANGING_FILAMENT_EN,
	MSG_CHANGING_FILAMENT_CZ
};

const char MSG_CHECK_IDLER_EN[] PROGMEM = "Please open idler and remove filament manually.";
const char MSG_CHECK_IDLER_CZ[] PROGMEM = "Prosim otevrete idler a manualne odstrante filament.";
const char * const MSG_CHECK_IDLER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHECK_IDLER_EN,
	MSG_CHECK_IDLER_CZ
};

const char MSG_CHOOSE_EXTRUDER_EN[] PROGMEM = "Choose extruder:";
const char MSG_CHOOSE_EXTRUDER_CZ[] PROGMEM = "Vyberte extruder:";
const char * const MSG_CHOOSE_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHOOSE_EXTRUDER_EN,
	MSG_CHOOSE_EXTRUDER_CZ
};

const char MSG_CLEAN_NOZZLE_E_EN[] PROGMEM = "E calibration finished. Please clean the nozzle. Click when done.";
const char MSG_CLEAN_NOZZLE_E_CZ[] PROGMEM = "E kalibrace ukoncena. Prosim ocistete trysku. Po te potvrdte tlacitkem.";
const char * const MSG_CLEAN_NOZZLE_E_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CLEAN_NOZZLE_E_EN,
	MSG_CLEAN_NOZZLE_E_CZ
};

const char MSG_CNG_SDCARD_EN[] PROGMEM = "Change SD card";
const char * const MSG_CNG_SDCARD_LANG_TABLE[1] PROGMEM = {
	MSG_CNG_SDCARD_EN
};

const char MSG_CONFIGURATION_VER_EN[] PROGMEM = " Last Updated: ";
const char * const MSG_CONFIGURATION_VER_LANG_TABLE[1] PROGMEM = {
	MSG_CONFIGURATION_VER_EN
};

const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_EN[] PROGMEM = "Are left and right Z~carriages all up?";
const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_CZ[] PROGMEM = "Dojely oba Z voziky k~hornimu dorazu?";
const char * const MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_EN,
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_CZ
};

const char MSG_CONFIRM_NOZZLE_CLEAN_EN[] PROGMEM = "Please clean the nozzle for calibration. Click when done.";
const char MSG_CONFIRM_NOZZLE_CLEAN_CZ[] PROGMEM = "Pro uspesnou kalibraci ocistete prosim tiskovou trysku. Potvrdte tlacitkem.";
const char * const MSG_CONFIRM_NOZZLE_CLEAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONFIRM_NOZZLE_CLEAN_EN,
	MSG_CONFIRM_NOZZLE_CLEAN_CZ
};

const char MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_EN[] PROGMEM = "Filaments are now adjusted. Please clean the nozzle for calibration. Click when done.";
const char MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_CZ[] PROGMEM = "Filamenty jsou srovnany. Pro uspesnou kalibraci prosim ocistete trysku. Po te potvrdte tlacitkem.";
const char * const MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_EN,
	MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_CZ
};

const char MSG_CONTROL_EN[] PROGMEM = "Control";
const char * const MSG_CONTROL_LANG_TABLE[1] PROGMEM = {
	MSG_CONTROL_EN
};

const char MSG_COOLDOWN_EN[] PROGMEM = "Cooldown";
const char MSG_COOLDOWN_CZ[] PROGMEM = "Zchladit";
const char * const MSG_COOLDOWN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_COOLDOWN_EN,
	MSG_COOLDOWN_CZ
};

const char MSG_CORRECTLY_EN[] PROGMEM = "Changed correctly?";
const char MSG_CORRECTLY_CZ[] PROGMEM = "Vymena ok?";
const char * const MSG_CORRECTLY_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CORRECTLY_EN,
	MSG_CORRECTLY_CZ
};

const char MSG_COUNT_X_EN[] PROGMEM = " Count X: ";
const char * const MSG_COUNT_X_LANG_TABLE[1] PROGMEM = {
	MSG_COUNT_X_EN
};

const char MSG_CRASHDETECT_NA_EN[] PROGMEM = "Crash det.  [N/A]";
const char * const MSG_CRASHDETECT_NA_LANG_TABLE[1] PROGMEM = {
	MSG_CRASHDETECT_NA_EN
};

const char MSG_CRASHDETECT_OFF_EN[] PROGMEM = "Crash det.  [off]";
const char MSG_CRASHDETECT_OFF_CZ[] PROGMEM = "Crash det.  [vyp]";
const char * const MSG_CRASHDETECT_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CRASHDETECT_OFF_EN,
	MSG_CRASHDETECT_OFF_CZ
};

const char MSG_CRASHDETECT_ON_EN[] PROGMEM = "Crash det.   [on]";
const char MSG_CRASHDETECT_ON_CZ[] PROGMEM = "Crash det.  [zap]";
const char * const MSG_CRASHDETECT_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CRASHDETECT_ON_EN,
	MSG_CRASHDETECT_ON_CZ
};

const char MSG_CRASH_DETECTED_EN[] PROGMEM = "Crash detected.";
const char MSG_CRASH_DETECTED_CZ[] PROGMEM = "Detekovan naraz.";
const char * const MSG_CRASH_DETECTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CRASH_DETECTED_EN,
	MSG_CRASH_DETECTED_CZ
};

const char MSG_CRASH_DETECTED2_EN[] PROGMEM = "Crash detected. Continue printing?";
const char MSG_CRASH_DETECTED2_CZ[] PROGMEM = "Naraz detekovan, pokracovat v tisku?";
const char * const MSG_CRASH_DETECTED2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CRASH_DETECTED2_EN,
	MSG_CRASH_DETECTED2_CZ
};

const char MSG_CRASH_DET_ONLY_IN_NORMAL_EN[] PROGMEM = "\x1b[2JCrash detection can\x1b[1;0Hbe turned on only in\x1b[2;0HNormal mode";
const char MSG_CRASH_DET_ONLY_IN_NORMAL_CZ[] PROGMEM = "\x1b[2JCrash detekce muze\x1b[1;0Hbyt zapnuta pouze v\x1b[2;0HNormal modu";
const char * const MSG_CRASH_DET_ONLY_IN_NORMAL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CRASH_DET_ONLY_IN_NORMAL_EN,
	MSG_CRASH_DET_ONLY_IN_NORMAL_CZ
};

const char MSG_CRASH_DET_STEALTH_FORCE_OFF_EN[] PROGMEM = "\x1b[2JWARNING:\x1b[1;0HCrash detection\x1b[2;0Hdisabled in\x1b[3;0HStealth mode";
const char MSG_CRASH_DET_STEALTH_FORCE_OFF_CZ[] PROGMEM = "\x1b[2JPOZOR:\x1b[1;0HCrash detekce\x1b[2;0Hdeaktivovana ve\x1b[3;0HStealth modu";
const char * const MSG_CRASH_DET_STEALTH_FORCE_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CRASH_DET_STEALTH_FORCE_OFF_EN,
	MSG_CRASH_DET_STEALTH_FORCE_OFF_CZ
};

const char MSG_CURRENT_EN[] PROGMEM = "Current";
const char MSG_CURRENT_CZ[] PROGMEM = "Pouze aktualni";
const char * const MSG_CURRENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CURRENT_EN,
	MSG_CURRENT_CZ
};

const char MSG_DATE_EN[] PROGMEM = "Date:";
const char MSG_DATE_CZ[] PROGMEM = "Datum:";
const char * const MSG_DATE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_DATE_EN,
	MSG_DATE_CZ
};

const char MSG_DEFAULT_SETTINGS_LOADED_EN[] PROGMEM = "Old settings found. Default PID, Esteps etc. will be set.";
const char MSG_DEFAULT_SETTINGS_LOADED_CZ[] PROGMEM = "Neplatne hodnoty nastaveni. Bude pouzito vychozi PID, Esteps atd.";
const char * const MSG_DEFAULT_SETTINGS_LOADED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_DEFAULT_SETTINGS_LOADED_EN,
	MSG_DEFAULT_SETTINGS_LOADED_CZ
};

const char MSG_DISABLE_STEPPERS_EN[] PROGMEM = "Disable steppers";
const char MSG_DISABLE_STEPPERS_CZ[] PROGMEM = "Vypnout motory";
const char * const MSG_DISABLE_STEPPERS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_DISABLE_STEPPERS_EN,
	MSG_DISABLE_STEPPERS_CZ
};

const char MSG_DWELL_EN[] PROGMEM = "Sleep...";
const char * const MSG_DWELL_LANG_TABLE[1] PROGMEM = {
	MSG_DWELL_EN
};

const char MSG_ENDSTOPS_HIT_EN[] PROGMEM = "endstops hit: ";
const char * const MSG_ENDSTOPS_HIT_LANG_TABLE[1] PROGMEM = {
	MSG_ENDSTOPS_HIT_EN
};

const char MSG_ENDSTOP_HIT_EN[] PROGMEM = "TRIGGERED";
const char * const MSG_ENDSTOP_HIT_LANG_TABLE[1] PROGMEM = {
	MSG_ENDSTOP_HIT_EN
};

const char MSG_ENDSTOP_OPEN_EN[] PROGMEM = "open";
const char * const MSG_ENDSTOP_OPEN_LANG_TABLE[1] PROGMEM = {
	MSG_ENDSTOP_OPEN_EN
};

const char MSG_END_FILE_LIST_EN[] PROGMEM = "End file list";
const char * const MSG_END_FILE_LIST_LANG_TABLE[1] PROGMEM = {
	MSG_END_FILE_LIST_EN
};

const char MSG_ERROR_EN[] PROGMEM = "ERROR:";
const char MSG_ERROR_CZ[] PROGMEM = "CHYBA:";
const char * const MSG_ERROR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERROR_EN,
	MSG_ERROR_CZ
};

const char MSG_ERR_CHECKSUM_MISMATCH_EN[] PROGMEM = "checksum mismatch, Last Line: ";
const char * const MSG_ERR_CHECKSUM_MISMATCH_LANG_TABLE[1] PROGMEM = {
	MSG_ERR_CHECKSUM_MISMATCH_EN
};

const char MSG_ERR_COLD_EXTRUDE_STOP_EN[] PROGMEM = " cold extrusion prevented";
const char * const MSG_ERR_COLD_EXTRUDE_STOP_LANG_TABLE[1] PROGMEM = {
	MSG_ERR_COLD_EXTRUDE_STOP_EN
};

const char MSG_ERR_KILLED_EN[] PROGMEM = "Printer halted. kill() called!";
const char * const MSG_ERR_KILLED_LANG_TABLE[1] PROGMEM = {
	MSG_ERR_KILLED_EN
};

const char MSG_ERR_LINE_NO_EN[] PROGMEM = "Line Number is not Last Line Number+1, Last Line: ";
const char * const MSG_ERR_LINE_NO_LANG_TABLE[1] PROGMEM = {
	MSG_ERR_LINE_NO_EN
};

const char MSG_ERR_LONG_EXTRUDE_STOP_EN[] PROGMEM = " too long extrusion prevented";
const char * const MSG_ERR_LONG_EXTRUDE_STOP_LANG_TABLE[1] PROGMEM = {
	MSG_ERR_LONG_EXTRUDE_STOP_EN
};

const char MSG_ERR_NO_CHECKSUM_EN[] PROGMEM = "No Checksum with line number, Last Line: ";
const char * const MSG_ERR_NO_CHECKSUM_LANG_TABLE[1] PROGMEM = {
	MSG_ERR_NO_CHECKSUM_EN
};

const char MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_EN[] PROGMEM = "No Line Number with checksum, Last Line: ";
const char * const MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_LANG_TABLE[1] PROGMEM = {
	MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM_EN
};

const char MSG_ERR_NO_THERMISTORS_EN[] PROGMEM = "No thermistors - no temperature";
const char * const MSG_ERR_NO_THERMISTORS_LANG_TABLE[1] PROGMEM = {
	MSG_ERR_NO_THERMISTORS_EN
};

const char MSG_ERR_STOPPED_EN[] PROGMEM = "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)";
const char * const MSG_ERR_STOPPED_LANG_TABLE[1] PROGMEM = {
	MSG_ERR_STOPPED_EN
};

const char MSG_EXTERNAL_RESET_EN[] PROGMEM = " External Reset";
const char * const MSG_EXTERNAL_RESET_LANG_TABLE[1] PROGMEM = {
	MSG_EXTERNAL_RESET_EN
};

const char MSG_EXTRUDER_EN[] PROGMEM = "Extruder";
const char * const MSG_EXTRUDER_LANG_TABLE[1] PROGMEM = {
	MSG_EXTRUDER_EN
};

const char MSG_EXTRUDER_1_EN[] PROGMEM = "Extruder 1";
const char * const MSG_EXTRUDER_1_LANG_TABLE[1] PROGMEM = {
	MSG_EXTRUDER_1_EN
};

const char MSG_EXTRUDER_2_EN[] PROGMEM = "Extruder 2";
const char * const MSG_EXTRUDER_2_LANG_TABLE[1] PROGMEM = {
	MSG_EXTRUDER_2_EN
};

const char MSG_EXTRUDER_3_EN[] PROGMEM = "Extruder 3";
const char * const MSG_EXTRUDER_3_LANG_TABLE[1] PROGMEM = {
	MSG_EXTRUDER_3_EN
};

const char MSG_EXTRUDER_4_EN[] PROGMEM = "Extruder 4";
const char * const MSG_EXTRUDER_4_LANG_TABLE[1] PROGMEM = {
	MSG_EXTRUDER_4_EN
};

const char MSG_EXTRUDER_CORRECTION_EN[] PROGMEM = "E-correct";
const char * const MSG_EXTRUDER_CORRECTION_LANG_TABLE[1] PROGMEM = {
	MSG_EXTRUDER_CORRECTION_EN
};

const char MSG_EXTRUDER_CORRECTION_OFF_EN[] PROGMEM = "  [off";
const char * const MSG_EXTRUDER_CORRECTION_OFF_LANG_TABLE[1] PROGMEM = {
	MSG_EXTRUDER_CORRECTION_OFF_EN
};

const char MSG_E_CAL_KNOB_EN[] PROGMEM = "Rotate knob until mark reaches extruder body. Click when done.";
const char MSG_E_CAL_KNOB_CZ[] PROGMEM = "Otacejte tlacitkem dokud znacka nedosahne tela extruderu. Potvrdte tlacitkem.";
const char * const MSG_E_CAL_KNOB_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_E_CAL_KNOB_EN,
	MSG_E_CAL_KNOB_CZ
};

const char MSG_Enqueing_EN[] PROGMEM = "enqueing \"";
const char * const MSG_Enqueing_LANG_TABLE[1] PROGMEM = {
	MSG_Enqueing_EN
};

const char MSG_FACTOR_EN[] PROGMEM = " \002 Fact";
const char * const MSG_FACTOR_LANG_TABLE[1] PROGMEM = {
	MSG_FACTOR_EN
};

const char MSG_FANS_CHECK_OFF_EN[] PROGMEM = "Fans check  [off]";
const char MSG_FANS_CHECK_OFF_CZ[] PROGMEM = "Kontr. vent.[vyp]";
const char * const MSG_FANS_CHECK_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FANS_CHECK_OFF_EN,
	MSG_FANS_CHECK_OFF_CZ
};

const char MSG_FANS_CHECK_ON_EN[] PROGMEM = "Fans check   [on]";
const char MSG_FANS_CHECK_ON_CZ[] PROGMEM = "Kontr. vent.[zap]";
const char * const MSG_FANS_CHECK_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FANS_CHECK_ON_EN,
	MSG_FANS_CHECK_ON_CZ
};

const char MSG_FAN_SPEED_EN[] PROGMEM = "Fan speed";
const char MSG_FAN_SPEED_CZ[] PROGMEM = "Rychlost vent.";
const char * const MSG_FAN_SPEED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FAN_SPEED_EN,
	MSG_FAN_SPEED_CZ
};

const char MSG_FARM_CARD_MENU_EN[] PROGMEM = "Farm mode print";
const char * const MSG_FARM_CARD_MENU_LANG_TABLE[1] PROGMEM = {
	MSG_FARM_CARD_MENU_EN
};

const char MSG_FILAMENTCHANGE_EN[] PROGMEM = "Change filament";
const char MSG_FILAMENTCHANGE_CZ[] PROGMEM = "Vymenit filament";
const char * const MSG_FILAMENTCHANGE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENTCHANGE_EN,
	MSG_FILAMENTCHANGE_CZ
};

const char MSG_FILAMENT_CLEAN_EN[] PROGMEM = "Filament extruding & with correct color?";
const char MSG_FILAMENT_CLEAN_CZ[] PROGMEM = "Filament vytlacen a spravne barvy?";
const char * const MSG_FILAMENT_CLEAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_CLEAN_EN,
	MSG_FILAMENT_CLEAN_CZ
};

const char MSG_FILAMENT_LOADING_T0_EN[] PROGMEM = "Insert filament into extruder 1. Click when done.";
const char MSG_FILAMENT_LOADING_T0_CZ[] PROGMEM = "Vlozte filament do extruderu 1. Potvrdte tlacitkem.";
const char * const MSG_FILAMENT_LOADING_T0_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_LOADING_T0_EN,
	MSG_FILAMENT_LOADING_T0_CZ
};

const char MSG_FILAMENT_LOADING_T1_EN[] PROGMEM = "Insert filament into extruder 2. Click when done.";
const char MSG_FILAMENT_LOADING_T1_CZ[] PROGMEM = "Vlozte filament do extruderu 2. Potvrdte tlacitkem.";
const char * const MSG_FILAMENT_LOADING_T1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_LOADING_T1_EN,
	MSG_FILAMENT_LOADING_T1_CZ
};

const char MSG_FILAMENT_LOADING_T2_EN[] PROGMEM = "Insert filament into extruder 3. Click when done.";
const char MSG_FILAMENT_LOADING_T2_CZ[] PROGMEM = "Vlozte filament do extruderu 3. Potvrdte tlacitkem.";
const char * const MSG_FILAMENT_LOADING_T2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_LOADING_T2_EN,
	MSG_FILAMENT_LOADING_T2_CZ
};

const char MSG_FILAMENT_LOADING_T3_EN[] PROGMEM = "Insert filament into extruder 4. Click when done.";
const char MSG_FILAMENT_LOADING_T3_CZ[] PROGMEM = "Vlozte filament do extruderu 4. Potvrdte tlacitkem.";
const char * const MSG_FILAMENT_LOADING_T3_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_LOADING_T3_EN,
	MSG_FILAMENT_LOADING_T3_CZ
};

const char MSG_FILAMENT_SENSOR_EN[] PROGMEM = "Filament sensor";
const char MSG_FILAMENT_SENSOR_CZ[] PROGMEM = "Senzor filamentu";
const char * const MSG_FILAMENT_SENSOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_SENSOR_EN,
	MSG_FILAMENT_SENSOR_CZ
};

const char MSG_FILE_CNT_EN[] PROGMEM = "Some files will not be sorted. Max. No. of files in 1 folder for sorting is 100.";
const char MSG_FILE_CNT_CZ[] PROGMEM = "Nektere soubory nebudou setrideny. Maximalni pocet souboru pro setrideni je 100.";
const char * const MSG_FILE_CNT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILE_CNT_EN,
	MSG_FILE_CNT_CZ
};

const char MSG_FILE_INCOMPLETE_EN[] PROGMEM = "File incomplete. Continue anyway?";
const char MSG_FILE_INCOMPLETE_CZ[] PROGMEM = "Soubor nekompletni. Pokracovat?";
const char * const MSG_FILE_INCOMPLETE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILE_INCOMPLETE_EN,
	MSG_FILE_INCOMPLETE_CZ
};

const char MSG_FILE_PRINTED_EN[] PROGMEM = "Done printing file";
const char * const MSG_FILE_PRINTED_LANG_TABLE[1] PROGMEM = {
	MSG_FILE_PRINTED_EN
};

const char MSG_FILE_SAVED_EN[] PROGMEM = "Done saving file.";
const char * const MSG_FILE_SAVED_LANG_TABLE[1] PROGMEM = {
	MSG_FILE_SAVED_EN
};

const char MSG_FIL_ADJUSTING_EN[] PROGMEM = "Adjusting filaments. Please wait.";
const char MSG_FIL_ADJUSTING_CZ[] PROGMEM = "Probiha srovnani filamentu. Prosim cekejte.";
const char * const MSG_FIL_ADJUSTING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIL_ADJUSTING_EN,
	MSG_FIL_ADJUSTING_CZ
};

const char MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_EN[] PROGMEM = "Iteration ";
const char MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_CZ[] PROGMEM = "Iterace ";
const char * const MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_EN,
	MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_CZ
};

const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_EN[] PROGMEM = "Searching bed calibration point";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_CZ[] PROGMEM = "Hledam kalibracni bod podlozky";
const char * const MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_EN,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_CZ
};

const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_EN[] PROGMEM = " of 4";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_CZ[] PROGMEM = " z 4";
const char * const MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_EN,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_CZ
};

const char MSG_FINISHING_MOVEMENTS_EN[] PROGMEM = "Finishing movements";
const char MSG_FINISHING_MOVEMENTS_CZ[] PROGMEM = "Dokoncovani pohybu";
const char * const MSG_FINISHING_MOVEMENTS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FINISHING_MOVEMENTS_EN,
	MSG_FINISHING_MOVEMENTS_CZ
};

const char MSG_FLOW_EN[] PROGMEM = "Flow";
const char MSG_FLOW_CZ[] PROGMEM = "Prutok";
const char * const MSG_FLOW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FLOW_EN,
	MSG_FLOW_CZ
};

const char MSG_FLOW0_EN[] PROGMEM = "Flow 0";
const char * const MSG_FLOW0_LANG_TABLE[1] PROGMEM = {
	MSG_FLOW0_EN
};

const char MSG_FLOW1_EN[] PROGMEM = "Flow 1";
const char * const MSG_FLOW1_LANG_TABLE[1] PROGMEM = {
	MSG_FLOW1_EN
};

const char MSG_FLOW2_EN[] PROGMEM = "Flow 2";
const char * const MSG_FLOW2_LANG_TABLE[1] PROGMEM = {
	MSG_FLOW2_EN
};

const char MSG_FOLLOW_CALIBRATION_FLOW_EN[] PROGMEM = "Printer has not been calibrated yet. Please follow the manual, chapter First steps, section Calibration flow.";
const char MSG_FOLLOW_CALIBRATION_FLOW_CZ[] PROGMEM = "Tiskarna nebyla jeste zkalibrovana. Postupujte prosim podle manualu, kapitola Zaciname, odstavec Postup kalibrace.";
const char * const MSG_FOLLOW_CALIBRATION_FLOW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FOLLOW_CALIBRATION_FLOW_EN,
	MSG_FOLLOW_CALIBRATION_FLOW_CZ
};

const char MSG_FORCE_SELFTEST_EN[] PROGMEM = "Selftest will be run to calibrate accurate sensorless rehoming.";
const char MSG_FORCE_SELFTEST_CZ[] PROGMEM = "Pro kalibraci presneho rehomovani bude nyni spusten selftest.";
const char * const MSG_FORCE_SELFTEST_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FORCE_SELFTEST_EN,
	MSG_FORCE_SELFTEST_CZ
};

const char MSG_FREE_MEMORY_EN[] PROGMEM = " Free Memory: ";
const char * const MSG_FREE_MEMORY_LANG_TABLE[1] PROGMEM = {
	MSG_FREE_MEMORY_EN
};

const char MSG_FSENSOR_NA_EN[] PROGMEM = "Fil. sensor [N/A]";
const char MSG_FSENSOR_NA_CZ[] PROGMEM = "Fil. senzor [N/A]";
const char * const MSG_FSENSOR_NA_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FSENSOR_NA_EN,
	MSG_FSENSOR_NA_CZ
};

const char MSG_FSENSOR_OFF_EN[] PROGMEM = "Fil. sensor [off]";
const char MSG_FSENSOR_OFF_CZ[] PROGMEM = "Fil. senzor [vyp]";
const char * const MSG_FSENSOR_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FSENSOR_OFF_EN,
	MSG_FSENSOR_OFF_CZ
};

const char MSG_FSENSOR_ON_EN[] PROGMEM = "Fil. sensor  [on]";
const char MSG_FSENSOR_ON_CZ[] PROGMEM = "Fil. senzor [zap]";
const char * const MSG_FSENSOR_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FSENSOR_ON_EN,
	MSG_FSENSOR_ON_CZ
};

const char MSG_FSENS_AUTOLOAD_NA_EN[] PROGMEM = "F. autoload [N/A]";
const char MSG_FSENS_AUTOLOAD_NA_CZ[] PROGMEM = "F. autozav. [N/A]";
const char * const MSG_FSENS_AUTOLOAD_NA_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FSENS_AUTOLOAD_NA_EN,
	MSG_FSENS_AUTOLOAD_NA_CZ
};

const char MSG_FSENS_AUTOLOAD_OFF_EN[] PROGMEM = "F. autoload [off]";
const char MSG_FSENS_AUTOLOAD_OFF_CZ[] PROGMEM = "F. autozav. [vyp]";
const char * const MSG_FSENS_AUTOLOAD_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FSENS_AUTOLOAD_OFF_EN,
	MSG_FSENS_AUTOLOAD_OFF_CZ
};

const char MSG_FSENS_AUTOLOAD_ON_EN[] PROGMEM = "F. autoload  [on]";
const char MSG_FSENS_AUTOLOAD_ON_CZ[] PROGMEM = "F. autozav. [zap]";
const char * const MSG_FSENS_AUTOLOAD_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FSENS_AUTOLOAD_ON_EN,
	MSG_FSENS_AUTOLOAD_ON_CZ
};

const char MSG_FSENS_NOT_RESPONDING_EN[] PROGMEM = "ERROR: Filament sensor is not responding, please check connection.";
const char MSG_FSENS_NOT_RESPONDING_CZ[] PROGMEM = "CHYBA: Filament senzor nereaguje, zkontrolujte zapojeni.";
const char * const MSG_FSENS_NOT_RESPONDING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FSENS_NOT_RESPONDING_EN,
	MSG_FSENS_NOT_RESPONDING_CZ
};

const char MSG_FW_VERSION_ALPHA_EN[] PROGMEM = "You are using firmware alpha version. This is development version. Using this version is not recommended and may cause printer damage.";
const char MSG_FW_VERSION_ALPHA_CZ[] PROGMEM = "Pouzivate alpha verzi firmwaru. Jedna se o vyvojovou verzi. Pouzivani teto verze firmware neni doporuceno a muze zpusobit poskozeni tiskarny.";
const char * const MSG_FW_VERSION_ALPHA_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FW_VERSION_ALPHA_EN,
	MSG_FW_VERSION_ALPHA_CZ
};

const char MSG_FW_VERSION_BETA_EN[] PROGMEM = "You are using firmware beta version. This is development version. Using this version is not recommended and may cause printer damage.";
const char MSG_FW_VERSION_BETA_CZ[] PROGMEM = "Pouzivate beta verzi firmwaru. Jedna se o vyvojovou verzi. Pouzivani teto verze firmware neni doporuceno a muze zpusobit poskozeni tiskarny.";
const char * const MSG_FW_VERSION_BETA_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FW_VERSION_BETA_EN,
	MSG_FW_VERSION_BETA_CZ
};

const char MSG_FW_VERSION_RC_EN[] PROGMEM = "This firmware version is release candidate. Some of the features may not work properly.";
const char MSG_FW_VERSION_RC_CZ[] PROGMEM = "Tato verze firmware je release candidate. Nektere z funkci nemusi pracovat spolehlive.";
const char * const MSG_FW_VERSION_RC_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FW_VERSION_RC_EN,
	MSG_FW_VERSION_RC_CZ
};

const char MSG_FW_VERSION_UNKNOWN_EN[] PROGMEM = "WARNING: This is an unofficial, unsupported build. Use at your own risk!";
const char MSG_FW_VERSION_UNKNOWN_CZ[] PROGMEM = "VAROVANI: Neznama, nepodporovana verze firmware. Pouziti na vlastni nebezpeci!";
const char * const MSG_FW_VERSION_UNKNOWN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FW_VERSION_UNKNOWN_EN,
	MSG_FW_VERSION_UNKNOWN_CZ
};

const char MSG_HEATING_EN[] PROGMEM = "Heating";
const char MSG_HEATING_CZ[] PROGMEM = "Zahrivani";
const char * const MSG_HEATING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HEATING_EN,
	MSG_HEATING_CZ
};

const char MSG_HEATING_COMPLETE_EN[] PROGMEM = "Heating done.";
const char MSG_HEATING_COMPLETE_CZ[] PROGMEM = "Zahrivani OK.";
const char * const MSG_HEATING_COMPLETE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HEATING_COMPLETE_EN,
	MSG_HEATING_COMPLETE_CZ
};

const char MSG_HOMEYZ_EN[] PROGMEM = "Calibrate Z";
const char MSG_HOMEYZ_CZ[] PROGMEM = "Kalibrovat Z";
const char * const MSG_HOMEYZ_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HOMEYZ_EN,
	MSG_HOMEYZ_CZ
};

const char MSG_HOMEYZ_DONE_EN[] PROGMEM = "Calibration done";
const char MSG_HOMEYZ_DONE_CZ[] PROGMEM = "Kalibrace OK";
const char * const MSG_HOMEYZ_DONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HOMEYZ_DONE_EN,
	MSG_HOMEYZ_DONE_CZ
};

const char MSG_HOMEYZ_PROGRESS_EN[] PROGMEM = "Calibrating Z";
const char MSG_HOMEYZ_PROGRESS_CZ[] PROGMEM = "Kalibruji Z";
const char * const MSG_HOMEYZ_PROGRESS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HOMEYZ_PROGRESS_EN,
	MSG_HOMEYZ_PROGRESS_CZ
};

const char MSG_HOTEND_OFFSET_EN[] PROGMEM = "Hotend offsets:";
const char * const MSG_HOTEND_OFFSET_LANG_TABLE[1] PROGMEM = {
	MSG_HOTEND_OFFSET_EN
};

const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_EN[] PROGMEM = "Improving bed calibration point";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_CZ[] PROGMEM = "Zlepsuji presnost kalibracniho bodu";
const char * const MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_EN,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_CZ
};

const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_EN[] PROGMEM = " of 4";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_CZ[] PROGMEM = " z 4";
const char * const MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_EN,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_CZ
};

const char MSG_INFO_EXTRUDER_EN[] PROGMEM = "Extruder info";
const char * const MSG_INFO_EXTRUDER_LANG_TABLE[1] PROGMEM = {
	MSG_INFO_EXTRUDER_EN
};

const char MSG_INFO_FILAMENT_XDIFF_EN[] PROGMEM = "Fil. Xd:";
const char * const MSG_INFO_FILAMENT_XDIFF_LANG_TABLE[1] PROGMEM = {
	MSG_INFO_FILAMENT_XDIFF_EN
};

const char MSG_INFO_FILAMENT_YDIFF_EN[] PROGMEM = "Fil. Ydiff:";
const char * const MSG_INFO_FILAMENT_YDIFF_LANG_TABLE[1] PROGMEM = {
	MSG_INFO_FILAMENT_YDIFF_EN
};

const char MSG_INFO_NOZZLE_FAN_EN[] PROGMEM = "Nozzle FAN:";
const char MSG_INFO_NOZZLE_FAN_CZ[] PROGMEM = "Trysk. vent:";
const char * const MSG_INFO_NOZZLE_FAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INFO_NOZZLE_FAN_EN,
	MSG_INFO_NOZZLE_FAN_CZ
};

const char MSG_INFO_PRINT_FAN_EN[] PROGMEM = "Print FAN: ";
const char MSG_INFO_PRINT_FAN_CZ[] PROGMEM = "Tisk. vent:";
const char * const MSG_INFO_PRINT_FAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INFO_PRINT_FAN_EN,
	MSG_INFO_PRINT_FAN_CZ
};

const char MSG_INIT_SDCARD_EN[] PROGMEM = "Init. SD card";
const char * const MSG_INIT_SDCARD_LANG_TABLE[1] PROGMEM = {
	MSG_INIT_SDCARD_EN
};

const char MSG_INSERT_FILAMENT_EN[] PROGMEM = "Insert filament";
const char MSG_INSERT_FILAMENT_CZ[] PROGMEM = "Vlozte filament";
const char * const MSG_INSERT_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INSERT_FILAMENT_EN,
	MSG_INSERT_FILAMENT_CZ
};

const char MSG_INVALID_EXTRUDER_EN[] PROGMEM = "Invalid extruder";
const char * const MSG_INVALID_EXTRUDER_LANG_TABLE[1] PROGMEM = {
	MSG_INVALID_EXTRUDER_EN
};

const char MSG_KILLED_EN[] PROGMEM = "KILLED. ";
const char * const MSG_KILLED_LANG_TABLE[1] PROGMEM = {
	MSG_KILLED_EN
};

const char MSG_LANGUAGE_NAME_EN[] PROGMEM = "English";
const char MSG_LANGUAGE_NAME_CZ[] PROGMEM = "Cestina";
const char * const MSG_LANGUAGE_NAME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LANGUAGE_NAME_EN,
	MSG_LANGUAGE_NAME_CZ
};

const char MSG_LANGUAGE_SELECT_EN[] PROGMEM = "Select language";
const char MSG_LANGUAGE_SELECT_CZ[] PROGMEM = "Vyber jazyka";
const char * const MSG_LANGUAGE_SELECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LANGUAGE_SELECT_EN,
	MSG_LANGUAGE_SELECT_CZ
};

const char MSG_LEFT_EN[] PROGMEM = "Left:";
const char MSG_LEFT_CZ[] PROGMEM = "Levy:";
const char * const MSG_LEFT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LEFT_EN,
	MSG_LEFT_CZ
};

const char MSG_LOADING_COLOR_EN[] PROGMEM = "Loading color";
const char MSG_LOADING_COLOR_CZ[] PROGMEM = "Cisteni barvy";
const char * const MSG_LOADING_COLOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOADING_COLOR_EN,
	MSG_LOADING_COLOR_CZ
};

const char MSG_LOADING_FILAMENT_EN[] PROGMEM = "Loading filament";
const char MSG_LOADING_FILAMENT_CZ[] PROGMEM = "Zavadeni filamentu";
const char * const MSG_LOADING_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOADING_FILAMENT_EN,
	MSG_LOADING_FILAMENT_CZ
};

const char MSG_LOAD_ALL_EN[] PROGMEM = "Load all";
const char MSG_LOAD_ALL_CZ[] PROGMEM = "Zavest vse";
const char * const MSG_LOAD_ALL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_ALL_EN,
	MSG_LOAD_ALL_CZ
};

const char MSG_LOAD_EPROM_EN[] PROGMEM = "Load memory";
const char * const MSG_LOAD_EPROM_LANG_TABLE[1] PROGMEM = {
	MSG_LOAD_EPROM_EN
};

const char MSG_LOAD_FILAMENT_EN[] PROGMEM = "Load filament";
const char MSG_LOAD_FILAMENT_CZ[] PROGMEM = "Zavest filament";
const char * const MSG_LOAD_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_EN,
	MSG_LOAD_FILAMENT_CZ
};

const char MSG_LOAD_FILAMENT_1_EN[] PROGMEM = "Load filament 1";
const char MSG_LOAD_FILAMENT_1_CZ[] PROGMEM = "Zavest filament 1";
const char * const MSG_LOAD_FILAMENT_1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_1_EN,
	MSG_LOAD_FILAMENT_1_CZ
};

const char MSG_LOAD_FILAMENT_2_EN[] PROGMEM = "Load filament 2";
const char MSG_LOAD_FILAMENT_2_CZ[] PROGMEM = "Zavest filament 2";
const char * const MSG_LOAD_FILAMENT_2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_2_EN,
	MSG_LOAD_FILAMENT_2_CZ
};

const char MSG_LOAD_FILAMENT_3_EN[] PROGMEM = "Load filament 3";
const char MSG_LOAD_FILAMENT_3_CZ[] PROGMEM = "Zavest filament 3";
const char * const MSG_LOAD_FILAMENT_3_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_3_EN,
	MSG_LOAD_FILAMENT_3_CZ
};

const char MSG_LOAD_FILAMENT_4_EN[] PROGMEM = "Load filament 4";
const char MSG_LOAD_FILAMENT_4_CZ[] PROGMEM = "Zavest filament 4";
const char * const MSG_LOAD_FILAMENT_4_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_4_EN,
	MSG_LOAD_FILAMENT_4_CZ
};

const char MSG_LOOSE_PULLEY_EN[] PROGMEM = "Loose pulley";
const char MSG_LOOSE_PULLEY_CZ[] PROGMEM = "Uvolnena remenicka";
const char * const MSG_LOOSE_PULLEY_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOOSE_PULLEY_EN,
	MSG_LOOSE_PULLEY_CZ
};

const char MSG_M104_INVALID_EXTRUDER_EN[] PROGMEM = "M104 Invalid extruder ";
const char * const MSG_M104_INVALID_EXTRUDER_LANG_TABLE[1] PROGMEM = {
	MSG_M104_INVALID_EXTRUDER_EN
};

const char MSG_M105_INVALID_EXTRUDER_EN[] PROGMEM = "M105 Invalid extruder ";
const char * const MSG_M105_INVALID_EXTRUDER_LANG_TABLE[1] PROGMEM = {
	MSG_M105_INVALID_EXTRUDER_EN
};

const char MSG_M109_INVALID_EXTRUDER_EN[] PROGMEM = "M109 Invalid extruder ";
const char * const MSG_M109_INVALID_EXTRUDER_LANG_TABLE[1] PROGMEM = {
	MSG_M109_INVALID_EXTRUDER_EN
};

const char MSG_M115_REPORT_EN[] PROGMEM = "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:https://github.com/prusa3d/Prusa-i3-Plus/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:" CUSTOM_MENDEL_NAME " EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000\n";
const char * const MSG_M115_REPORT_LANG_TABLE[1] PROGMEM = {
	MSG_M115_REPORT_EN
};

const char MSG_M117_V2_CALIBRATION_EN[] PROGMEM = "M117 First layer cal.";
const char MSG_M117_V2_CALIBRATION_CZ[] PROGMEM = "M117 Kal. prvni vrstvy";
const char * const MSG_M117_V2_CALIBRATION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M117_V2_CALIBRATION_EN,
	MSG_M117_V2_CALIBRATION_CZ
};

const char MSG_M119_REPORT_EN[] PROGMEM = "Reporting endstop status";
const char * const MSG_M119_REPORT_LANG_TABLE[1] PROGMEM = {
	MSG_M119_REPORT_EN
};

const char MSG_M200_INVALID_EXTRUDER_EN[] PROGMEM = "M200 Invalid extruder ";
const char * const MSG_M200_INVALID_EXTRUDER_LANG_TABLE[1] PROGMEM = {
	MSG_M200_INVALID_EXTRUDER_EN
};

const char MSG_M218_INVALID_EXTRUDER_EN[] PROGMEM = "M218 Invalid extruder ";
const char * const MSG_M218_INVALID_EXTRUDER_LANG_TABLE[1] PROGMEM = {
	MSG_M218_INVALID_EXTRUDER_EN
};

const char MSG_M221_INVALID_EXTRUDER_EN[] PROGMEM = "M221 Invalid extruder ";
const char * const MSG_M221_INVALID_EXTRUDER_LANG_TABLE[1] PROGMEM = {
	MSG_M221_INVALID_EXTRUDER_EN
};

const char MSG_MAIN_EN[] PROGMEM = "Main";
const char MSG_MAIN_CZ[] PROGMEM = "Hlavni nabidka";
const char * const MSG_MAIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MAIN_EN,
	MSG_MAIN_CZ
};

const char MSG_MARK_FIL_EN[] PROGMEM = "Mark filament 100mm from extruder body. Click when done.";
const char MSG_MARK_FIL_CZ[] PROGMEM = "Oznacte filament 100 mm od tela extruderu a po te potvrdte tlacitkem.";
const char * const MSG_MARK_FIL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MARK_FIL_EN,
	MSG_MARK_FIL_CZ
};

const char MSG_MAX_EN[] PROGMEM = " \002 Max";
const char * const MSG_MAX_LANG_TABLE[1] PROGMEM = {
	MSG_MAX_EN
};

const char MSG_MEASURED_SKEW_EN[] PROGMEM = "Measured skew:";
const char MSG_MEASURED_SKEW_CZ[] PROGMEM = "Merene zkoseni:";
const char * const MSG_MEASURED_SKEW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MEASURED_SKEW_EN,
	MSG_MEASURED_SKEW_CZ
};

const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_EN[] PROGMEM = "Measuring reference height of calibration point";
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_CZ[] PROGMEM = "Merim referencni vysku kalibracniho bodu";
const char * const MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_EN,
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_CZ
};

const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_EN[] PROGMEM = " of 9";
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_CZ[] PROGMEM = " z 9";
const char * const MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_EN,
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_CZ
};

const char MSG_MENU_BELT_STATUS_EN[] PROGMEM = "Belt status";
const char MSG_MENU_BELT_STATUS_CZ[] PROGMEM = "Stav remenu";
const char * const MSG_MENU_BELT_STATUS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MENU_BELT_STATUS_EN,
	MSG_MENU_BELT_STATUS_CZ
};

const char MSG_MENU_CALIBRATION_EN[] PROGMEM = "Calibration";
const char MSG_MENU_CALIBRATION_CZ[] PROGMEM = "Kalibrace";
const char * const MSG_MENU_CALIBRATION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MENU_CALIBRATION_EN,
	MSG_MENU_CALIBRATION_CZ
};

const char MSG_MENU_TEMPERATURES_EN[] PROGMEM = "Temperatures";
const char MSG_MENU_TEMPERATURES_CZ[] PROGMEM = "Teploty";
const char * const MSG_MENU_TEMPERATURES_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MENU_TEMPERATURES_EN,
	MSG_MENU_TEMPERATURES_CZ
};

const char MSG_MENU_VOLTAGES_EN[] PROGMEM = "Voltages";
const char MSG_MENU_VOLTAGES_CZ[] PROGMEM = "Napeti";
const char * const MSG_MENU_VOLTAGES_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MENU_VOLTAGES_EN,
	MSG_MENU_VOLTAGES_CZ
};

const char MSG_MESH_BED_LEVELING_EN[] PROGMEM = "Mesh Bed Leveling";
const char * const MSG_MESH_BED_LEVELING_LANG_TABLE[1] PROGMEM = {
	MSG_MESH_BED_LEVELING_EN
};

const char MSG_MIN_EN[] PROGMEM = " \002 Min";
const char * const MSG_MIN_LANG_TABLE[1] PROGMEM = {
	MSG_MIN_EN
};

const char MSG_MOTION_EN[] PROGMEM = "Motion";
const char * const MSG_MOTION_LANG_TABLE[1] PROGMEM = {
	MSG_MOTION_EN
};

const char MSG_MOVE_01MM_EN[] PROGMEM = "Move 0.1mm";
const char * const MSG_MOVE_01MM_LANG_TABLE[1] PROGMEM = {
	MSG_MOVE_01MM_EN
};

const char MSG_MOVE_10MM_EN[] PROGMEM = "Move 10mm";
const char * const MSG_MOVE_10MM_LANG_TABLE[1] PROGMEM = {
	MSG_MOVE_10MM_EN
};

const char MSG_MOVE_1MM_EN[] PROGMEM = "Move 1mm";
const char * const MSG_MOVE_1MM_LANG_TABLE[1] PROGMEM = {
	MSG_MOVE_1MM_EN
};

const char MSG_MOVE_AXIS_EN[] PROGMEM = "Move axis";
const char MSG_MOVE_AXIS_CZ[] PROGMEM = "Posunout osu";
const char * const MSG_MOVE_AXIS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_AXIS_EN,
	MSG_MOVE_AXIS_CZ
};

const char MSG_MOVE_CARRIAGE_TO_THE_TOP_EN[] PROGMEM = "Calibrating XYZ. Rotate the knob to move the Z carriage up to the end stoppers. Click when done.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_CZ[] PROGMEM = "Kalibrace XYZ. Otacenim tlacitka posunte Z osu az k~hornimu dorazu. Potvrdte tlacitkem.";
const char * const MSG_MOVE_CARRIAGE_TO_THE_TOP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_CARRIAGE_TO_THE_TOP_EN,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_CZ
};

const char MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_EN[] PROGMEM = "Calibrating Z. Rotate the knob to move the Z carriage up to the end stoppers. Click when done.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_CZ[] PROGMEM = "Kalibrace Z. Otacenim tlacitka posunte Z osu az k~hornimu dorazu. Potvrdte tlacitkem.";
const char * const MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_EN,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_CZ
};

const char MSG_MOVE_E_EN[] PROGMEM = "Extruder";
const char * const MSG_MOVE_E_LANG_TABLE[1] PROGMEM = {
	MSG_MOVE_E_EN
};

const char MSG_MOVE_X_EN[] PROGMEM = "Move X";
const char MSG_MOVE_X_CZ[] PROGMEM = "Posunout X";
const char * const MSG_MOVE_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_X_EN,
	MSG_MOVE_X_CZ
};

const char MSG_MOVE_Y_EN[] PROGMEM = "Move Y";
const char MSG_MOVE_Y_CZ[] PROGMEM = "Posunout Y";
const char * const MSG_MOVE_Y_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_Y_EN,
	MSG_MOVE_Y_CZ
};

const char MSG_MOVE_Z_EN[] PROGMEM = "Move Z";
const char MSG_MOVE_Z_CZ[] PROGMEM = "Posunout Z";
const char * const MSG_MOVE_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_Z_EN,
	MSG_MOVE_Z_CZ
};

const char MSG_NEW_FIRMWARE_AVAILABLE_EN[] PROGMEM = "New firmware version available:";
const char MSG_NEW_FIRMWARE_AVAILABLE_CZ[] PROGMEM = "Vysla nova verze firmware:";
const char * const MSG_NEW_FIRMWARE_AVAILABLE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NEW_FIRMWARE_AVAILABLE_EN,
	MSG_NEW_FIRMWARE_AVAILABLE_CZ
};

const char MSG_NEW_FIRMWARE_PLEASE_UPGRADE_EN[] PROGMEM = "Please upgrade.";
const char MSG_NEW_FIRMWARE_PLEASE_UPGRADE_CZ[] PROGMEM = "Prosim aktualizujte.";
const char * const MSG_NEW_FIRMWARE_PLEASE_UPGRADE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NEW_FIRMWARE_PLEASE_UPGRADE_EN,
	MSG_NEW_FIRMWARE_PLEASE_UPGRADE_CZ
};

const char MSG_NO_EN[] PROGMEM = "No";
const char MSG_NO_CZ[] PROGMEM = "Ne";
const char * const MSG_NO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NO_EN,
	MSG_NO_CZ
};

const char MSG_NOT_COLOR_EN[] PROGMEM = "Color not correct";
const char MSG_NOT_COLOR_CZ[] PROGMEM = "Barva neni cista";
const char * const MSG_NOT_COLOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOT_COLOR_EN,
	MSG_NOT_COLOR_CZ
};

const char MSG_NOT_LOADED_EN[] PROGMEM = "Filament not loaded";
const char MSG_NOT_LOADED_CZ[] PROGMEM = "Filament nezaveden";
const char * const MSG_NOT_LOADED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOT_LOADED_EN,
	MSG_NOT_LOADED_CZ
};

const char MSG_NOZZLE_EN[] PROGMEM = "Nozzle";
const char MSG_NOZZLE_CZ[] PROGMEM = "Tryska";
const char * const MSG_NOZZLE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOZZLE_EN,
	MSG_NOZZLE_CZ
};

const char MSG_NOZZLE1_EN[] PROGMEM = "Nozzle2";
const char * const MSG_NOZZLE1_LANG_TABLE[1] PROGMEM = {
	MSG_NOZZLE1_EN
};

const char MSG_NOZZLE2_EN[] PROGMEM = "Nozzle3";
const char * const MSG_NOZZLE2_LANG_TABLE[1] PROGMEM = {
	MSG_NOZZLE2_EN
};

const char MSG_NO_CARD_EN[] PROGMEM = "No SD card";
const char MSG_NO_CARD_CZ[] PROGMEM = "Zadna SD karta";
const char * const MSG_NO_CARD_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NO_CARD_EN,
	MSG_NO_CARD_CZ
};

const char MSG_NO_MOVE_EN[] PROGMEM = "No move.";
const char * const MSG_NO_MOVE_LANG_TABLE[1] PROGMEM = {
	MSG_NO_MOVE_EN
};

const char MSG_OFF_EN[] PROGMEM = "Off";
const char * const MSG_OFF_LANG_TABLE[1] PROGMEM = {
	MSG_OFF_EN
};

const char MSG_OK_EN[] PROGMEM = "ok";
const char * const MSG_OK_LANG_TABLE[1] PROGMEM = {
	MSG_OK_EN
};

const char MSG_ON_EN[] PROGMEM = "On ";
const char * const MSG_ON_LANG_TABLE[1] PROGMEM = {
	MSG_ON_EN
};

const char MSG_PAPER_EN[] PROGMEM = "Place a sheet of paper under the nozzle during the calibration of first 4 points. If the nozzle catches the paper, power off the printer immediately.";
const char MSG_PAPER_CZ[] PROGMEM = "Umistete list papiru na podlozku a udrzujte jej pod tryskou behem mereni prvnich 4 bodu. Pokud tryska zachyti papir, vypnete tiskarnu.";
const char * const MSG_PAPER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PAPER_EN,
	MSG_PAPER_CZ
};

const char MSG_PAUSE_PRINT_EN[] PROGMEM = "Pause print";
const char MSG_PAUSE_PRINT_CZ[] PROGMEM = "Pozastavit tisk";
const char * const MSG_PAUSE_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PAUSE_PRINT_EN,
	MSG_PAUSE_PRINT_CZ
};

const char MSG_PICK_Z_EN[] PROGMEM = "Pick print";
const char MSG_PICK_Z_CZ[] PROGMEM = "Vyberte vytisk";
const char * const MSG_PICK_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PICK_Z_EN,
	MSG_PICK_Z_CZ
};

const char MSG_PID_EXTRUDER_EN[] PROGMEM = "PID calibration";
const char MSG_PID_EXTRUDER_CZ[] PROGMEM = "PID kalibrace";
const char * const MSG_PID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PID_EXTRUDER_EN,
	MSG_PID_EXTRUDER_CZ
};

const char MSG_PID_FINISHED_EN[] PROGMEM = "PID cal. finished";
const char MSG_PID_FINISHED_CZ[] PROGMEM = "PID kal. ukoncena";
const char * const MSG_PID_FINISHED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PID_FINISHED_EN,
	MSG_PID_FINISHED_CZ
};

const char MSG_PID_RUNNING_EN[] PROGMEM = "PID cal.           ";
const char MSG_PID_RUNNING_CZ[] PROGMEM = "PID kal.           ";
const char * const MSG_PID_RUNNING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PID_RUNNING_EN,
	MSG_PID_RUNNING_CZ
};

const char MSG_PINDA_NOT_CALIBRATED_EN[] PROGMEM = "Temperature calibration has not been run yet";
const char MSG_PINDA_NOT_CALIBRATED_CZ[] PROGMEM = "Tiskarna nebyla teplotne zkalibrovana";
const char * const MSG_PINDA_NOT_CALIBRATED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PINDA_NOT_CALIBRATED_EN,
	MSG_PINDA_NOT_CALIBRATED_CZ
};

const char MSG_PINDA_PREHEAT_EN[] PROGMEM = "PINDA Heating";
const char MSG_PINDA_PREHEAT_CZ[] PROGMEM = "Nahrivani PINDA";
const char * const MSG_PINDA_PREHEAT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PINDA_PREHEAT_EN,
	MSG_PINDA_PREHEAT_CZ
};

const char MSG_PLACE_STEEL_SHEET_EN[] PROGMEM = "Please place steel sheet on heatbed.";
const char MSG_PLACE_STEEL_SHEET_CZ[] PROGMEM = "Umistete prosim tiskovy plat na heatbed";
const char * const MSG_PLACE_STEEL_SHEET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PLACE_STEEL_SHEET_EN,
	MSG_PLACE_STEEL_SHEET_CZ
};

const char MSG_PLANNER_BUFFER_BYTES_EN[] PROGMEM = "  PlannerBufferBytes: ";
const char * const MSG_PLANNER_BUFFER_BYTES_LANG_TABLE[1] PROGMEM = {
	MSG_PLANNER_BUFFER_BYTES_EN
};

const char MSG_PLA_FILAMENT_LOADED_EN[] PROGMEM = "Is PLA filament loaded?";
const char MSG_PLA_FILAMENT_LOADED_CZ[] PROGMEM = "Je PLA filament zaveden?";
const char * const MSG_PLA_FILAMENT_LOADED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PLA_FILAMENT_LOADED_EN,
	MSG_PLA_FILAMENT_LOADED_CZ
};

const char MSG_PLEASE_LOAD_PLA_EN[] PROGMEM = "Please load PLA filament first.";
const char MSG_PLEASE_LOAD_PLA_CZ[] PROGMEM = "Nejdrive zavedte PLA filament prosim.";
const char * const MSG_PLEASE_LOAD_PLA_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PLEASE_LOAD_PLA_EN,
	MSG_PLEASE_LOAD_PLA_CZ
};

const char MSG_PLEASE_WAIT_EN[] PROGMEM = "Please wait";
const char MSG_PLEASE_WAIT_CZ[] PROGMEM = "Prosim cekejte";
const char * const MSG_PLEASE_WAIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PLEASE_WAIT_EN,
	MSG_PLEASE_WAIT_CZ
};

const char MSG_POSITION_UNKNOWN_EN[] PROGMEM = "Home X/Y before Z";
const char * const MSG_POSITION_UNKNOWN_LANG_TABLE[1] PROGMEM = {
	MSG_POSITION_UNKNOWN_EN
};

const char MSG_POWERUP_EN[] PROGMEM = "PowerUp";
const char * const MSG_POWERUP_LANG_TABLE[1] PROGMEM = {
	MSG_POWERUP_EN
};

const char MSG_PREHEAT_EN[] PROGMEM = "Preheat";
const char MSG_PREHEAT_CZ[] PROGMEM = "Predehrev";
const char * const MSG_PREHEAT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_EN,
	MSG_PREHEAT_CZ
};

const char MSG_PREHEAT_NOZZLE_EN[] PROGMEM = "Preheat the nozzle!";
const char MSG_PREHEAT_NOZZLE_CZ[] PROGMEM = "Predehrejte trysku!";
const char * const MSG_PREHEAT_NOZZLE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_NOZZLE_EN,
	MSG_PREHEAT_NOZZLE_CZ
};

const char MSG_PREPARE_FILAMENT_EN[] PROGMEM = "Prepare new filament";
const char MSG_PREPARE_FILAMENT_CZ[] PROGMEM = "Pripravte filament";
const char * const MSG_PREPARE_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREPARE_FILAMENT_EN,
	MSG_PREPARE_FILAMENT_CZ
};

const char MSG_PRESS_EN[] PROGMEM = "and press the knob";
const char MSG_PRESS_CZ[] PROGMEM = "a stisknete tlacitko";
const char * const MSG_PRESS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRESS_EN,
	MSG_PRESS_CZ
};

const char MSG_PRESS_TO_PREHEAT_EN[] PROGMEM = "Press knob to preheat nozzle and continue.";
const char MSG_PRESS_TO_PREHEAT_CZ[] PROGMEM = "Pro nahrati trysky a pokracovani stisknete tlacitko.";
const char * const MSG_PRESS_TO_PREHEAT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRESS_TO_PREHEAT_EN,
	MSG_PRESS_TO_PREHEAT_CZ
};

const char MSG_PRESS_TO_UNLOAD_EN[] PROGMEM = "Please press the knob to unload filament";
const char MSG_PRESS_TO_UNLOAD_CZ[] PROGMEM = "Pro vysunuti filamentu stisknete prosim tlacitko";
const char * const MSG_PRESS_TO_UNLOAD_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRESS_TO_UNLOAD_EN,
	MSG_PRESS_TO_UNLOAD_CZ
};

const char MSG_PRINTER_DISCONNECTED_EN[] PROGMEM = "Printer disconnected";
const char * const MSG_PRINTER_DISCONNECTED_LANG_TABLE[1] PROGMEM = {
	MSG_PRINTER_DISCONNECTED_EN
};

const char MSG_PRINT_ABORTED_EN[] PROGMEM = "Print aborted";
const char MSG_PRINT_ABORTED_CZ[] PROGMEM = "Tisk prerusen";
const char * const MSG_PRINT_ABORTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRINT_ABORTED_EN,
	MSG_PRINT_ABORTED_CZ
};

const char MSG_PRINT_PAUSED_EN[] PROGMEM = "Print paused";
const char MSG_PRINT_PAUSED_CZ[] PROGMEM = "Tisk pozastaven";
const char * const MSG_PRINT_PAUSED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRINT_PAUSED_EN,
	MSG_PRINT_PAUSED_CZ
};

const char MSG_PRUSA3D_EN[] PROGMEM = "prusa3d.com";
const char MSG_PRUSA3D_CZ[] PROGMEM = "prusa3d.cz";
const char * const MSG_PRUSA3D_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRUSA3D_EN,
	MSG_PRUSA3D_CZ
};

const char MSG_PRUSA3D_FORUM_EN[] PROGMEM = "forum.prusa3d.com";
const char MSG_PRUSA3D_FORUM_CZ[] PROGMEM = "forum.prusa3d.cz";
const char * const MSG_PRUSA3D_FORUM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRUSA3D_FORUM_EN,
	MSG_PRUSA3D_FORUM_CZ
};

const char MSG_PRUSA3D_HOWTO_EN[] PROGMEM = "howto.prusa3d.com";
const char MSG_PRUSA3D_HOWTO_CZ[] PROGMEM = "howto.prusa3d.cz";
const char * const MSG_PRUSA3D_HOWTO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRUSA3D_HOWTO_EN,
	MSG_PRUSA3D_HOWTO_CZ
};

const char MSG_PULL_OUT_FILAMENT_EN[] PROGMEM = "Please pull out filament immediately";
const char MSG_PULL_OUT_FILAMENT_CZ[] PROGMEM = "Prosim vyjmete urychlene filament";
const char * const MSG_PULL_OUT_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PULL_OUT_FILAMENT_EN,
	MSG_PULL_OUT_FILAMENT_CZ
};

const char MSG_REBOOT_EN[] PROGMEM = "Reboot the printer";
const char MSG_REBOOT_CZ[] PROGMEM = "Restartujte tiskarnu";
const char * const MSG_REBOOT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_REBOOT_EN,
	MSG_REBOOT_CZ
};

const char MSG_RECOVERING_PRINT_EN[] PROGMEM = "Recovering print    ";
const char MSG_RECOVERING_PRINT_CZ[] PROGMEM = "Obnovovani tisku    ";
const char * const MSG_RECOVERING_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RECOVERING_PRINT_EN,
	MSG_RECOVERING_PRINT_CZ
};

const char MSG_RECOVER_PRINT_EN[] PROGMEM = "Blackout occurred. Recover print?";
const char MSG_RECOVER_PRINT_CZ[] PROGMEM = "Detekovan vypadek proudu.Obnovit tisk?";
const char * const MSG_RECOVER_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RECOVER_PRINT_EN,
	MSG_RECOVER_PRINT_CZ
};

const char MSG_RECTRACT_EN[] PROGMEM = "Rectract";
const char * const MSG_RECTRACT_LANG_TABLE[1] PROGMEM = {
	MSG_RECTRACT_EN
};

const char MSG_REFRESH_EN[] PROGMEM = "\xF8" "Refresh";
const char * const MSG_REFRESH_LANG_TABLE[1] PROGMEM = {
	MSG_REFRESH_EN
};

const char MSG_REMOVE_STEEL_SHEET_EN[] PROGMEM = "Please remove steel sheet from heatbed.";
const char MSG_REMOVE_STEEL_SHEET_CZ[] PROGMEM = "Odstrante tiskovy plat z heatbed prosim.";
const char * const MSG_REMOVE_STEEL_SHEET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_REMOVE_STEEL_SHEET_EN,
	MSG_REMOVE_STEEL_SHEET_CZ
};

const char MSG_RESEND_EN[] PROGMEM = "Resend: ";
const char * const MSG_RESEND_LANG_TABLE[1] PROGMEM = {
	MSG_RESEND_EN
};

const char MSG_RESET_CALIBRATE_E_EN[] PROGMEM = "Reset E Cal.";
const char * const MSG_RESET_CALIBRATE_E_LANG_TABLE[1] PROGMEM = {
	MSG_RESET_CALIBRATE_E_EN
};

const char MSG_RESTORE_FAILSAFE_EN[] PROGMEM = "Restore failsafe";
const char * const MSG_RESTORE_FAILSAFE_LANG_TABLE[1] PROGMEM = {
	MSG_RESTORE_FAILSAFE_EN
};

const char MSG_RESUME_PRINT_EN[] PROGMEM = "Resume print";
const char MSG_RESUME_PRINT_CZ[] PROGMEM = "Pokracovat";
const char * const MSG_RESUME_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESUME_PRINT_EN,
	MSG_RESUME_PRINT_CZ
};

const char MSG_RESUMING_EN[] PROGMEM = "Resuming print";
const char MSG_RESUMING_CZ[] PROGMEM = "Obnoveni tisku";
const char * const MSG_RESUMING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESUMING_EN,
	MSG_RESUMING_CZ
};

const char MSG_RESUMING_PRINT_EN[] PROGMEM = "Resuming print";
const char MSG_RESUMING_PRINT_CZ[] PROGMEM = "Obnovovani tisku";
const char * const MSG_RESUMING_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESUMING_PRINT_EN,
	MSG_RESUMING_PRINT_CZ
};

const char MSG_RIGHT_EN[] PROGMEM = "Right:";
const char MSG_RIGHT_CZ[] PROGMEM = "Pravy:";
const char * const MSG_RIGHT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RIGHT_EN,
	MSG_RIGHT_CZ
};

const char MSG_SD_CANT_ENTER_SUBDIR_EN[] PROGMEM = "Cannot enter subdir: ";
const char * const MSG_SD_CANT_ENTER_SUBDIR_LANG_TABLE[1] PROGMEM = {
	MSG_SD_CANT_ENTER_SUBDIR_EN
};

const char MSG_SD_CANT_OPEN_SUBDIR_EN[] PROGMEM = "Cannot open subdir";
const char * const MSG_SD_CANT_OPEN_SUBDIR_LANG_TABLE[1] PROGMEM = {
	MSG_SD_CANT_OPEN_SUBDIR_EN
};

const char MSG_SD_CARD_OK_EN[] PROGMEM = "SD card ok";
const char * const MSG_SD_CARD_OK_LANG_TABLE[1] PROGMEM = {
	MSG_SD_CARD_OK_EN
};

const char MSG_SD_ERR_WRITE_TO_FILE_EN[] PROGMEM = "error writing to file";
const char * const MSG_SD_ERR_WRITE_TO_FILE_LANG_TABLE[1] PROGMEM = {
	MSG_SD_ERR_WRITE_TO_FILE_EN
};

const char MSG_SD_FILE_OPENED_EN[] PROGMEM = "File opened: ";
const char * const MSG_SD_FILE_OPENED_LANG_TABLE[1] PROGMEM = {
	MSG_SD_FILE_OPENED_EN
};

const char MSG_SD_FILE_SELECTED_EN[] PROGMEM = "File selected";
const char * const MSG_SD_FILE_SELECTED_LANG_TABLE[1] PROGMEM = {
	MSG_SD_FILE_SELECTED_EN
};

const char MSG_SD_INIT_FAIL_EN[] PROGMEM = "SD init fail";
const char * const MSG_SD_INIT_FAIL_LANG_TABLE[1] PROGMEM = {
	MSG_SD_INIT_FAIL_EN
};

const char MSG_SD_INSERTED_EN[] PROGMEM = "Card inserted";
const char MSG_SD_INSERTED_CZ[] PROGMEM = "Karta vlozena";
const char * const MSG_SD_INSERTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_INSERTED_EN,
	MSG_SD_INSERTED_CZ
};

const char MSG_SD_NOT_PRINTING_EN[] PROGMEM = "Not SD printing";
const char * const MSG_SD_NOT_PRINTING_LANG_TABLE[1] PROGMEM = {
	MSG_SD_NOT_PRINTING_EN
};

const char MSG_SD_OPENROOT_FAIL_EN[] PROGMEM = "openRoot failed";
const char * const MSG_SD_OPENROOT_FAIL_LANG_TABLE[1] PROGMEM = {
	MSG_SD_OPENROOT_FAIL_EN
};

const char MSG_SD_OPEN_FILE_FAIL_EN[] PROGMEM = "open failed, File: ";
const char * const MSG_SD_OPEN_FILE_FAIL_LANG_TABLE[1] PROGMEM = {
	MSG_SD_OPEN_FILE_FAIL_EN
};

const char MSG_SD_PRINTING_BYTE_EN[] PROGMEM = "SD printing byte ";
const char * const MSG_SD_PRINTING_BYTE_LANG_TABLE[1] PROGMEM = {
	MSG_SD_PRINTING_BYTE_EN
};

const char MSG_SD_REMOVED_EN[] PROGMEM = "Card removed";
const char MSG_SD_REMOVED_CZ[] PROGMEM = "Karta vyjmuta";
const char * const MSG_SD_REMOVED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_REMOVED_EN,
	MSG_SD_REMOVED_CZ
};

const char MSG_SD_SIZE_EN[] PROGMEM = " Size: ";
const char * const MSG_SD_SIZE_LANG_TABLE[1] PROGMEM = {
	MSG_SD_SIZE_EN
};

const char MSG_SD_VOL_INIT_FAIL_EN[] PROGMEM = "volume.init failed";
const char * const MSG_SD_VOL_INIT_FAIL_LANG_TABLE[1] PROGMEM = {
	MSG_SD_VOL_INIT_FAIL_EN
};

const char MSG_SD_WORKDIR_FAIL_EN[] PROGMEM = "workDir open failed";
const char * const MSG_SD_WORKDIR_FAIL_LANG_TABLE[1] PROGMEM = {
	MSG_SD_WORKDIR_FAIL_EN
};

const char MSG_SD_WRITE_TO_FILE_EN[] PROGMEM = "Writing to file: ";
const char * const MSG_SD_WRITE_TO_FILE_LANG_TABLE[1] PROGMEM = {
	MSG_SD_WRITE_TO_FILE_EN
};

const char MSG_SECOND_SERIAL_OFF_EN[] PROGMEM = "RPi port    [off]";
const char MSG_SECOND_SERIAL_OFF_CZ[] PROGMEM = "RPi port    [vyp]";
const char * const MSG_SECOND_SERIAL_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SECOND_SERIAL_OFF_EN,
	MSG_SECOND_SERIAL_OFF_CZ
};

const char MSG_SECOND_SERIAL_ON_EN[] PROGMEM = "RPi port     [on]";
const char MSG_SECOND_SERIAL_ON_CZ[] PROGMEM = "RPi port    [zap]";
const char * const MSG_SECOND_SERIAL_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SECOND_SERIAL_ON_EN,
	MSG_SECOND_SERIAL_ON_CZ
};

const char MSG_SELFTEST_EN[] PROGMEM = "Selftest         ";
const char * const MSG_SELFTEST_LANG_TABLE[1] PROGMEM = {
	MSG_SELFTEST_EN
};

const char MSG_SELFTEST_AXIS_EN[] PROGMEM = "Axis";
const char MSG_SELFTEST_AXIS_CZ[] PROGMEM = "Osa";
const char * const MSG_SELFTEST_AXIS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_AXIS_EN,
	MSG_SELFTEST_AXIS_CZ
};

const char MSG_SELFTEST_AXIS_LENGTH_EN[] PROGMEM = "Axis length";
const char MSG_SELFTEST_AXIS_LENGTH_CZ[] PROGMEM = "Delka osy";
const char * const MSG_SELFTEST_AXIS_LENGTH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_AXIS_LENGTH_EN,
	MSG_SELFTEST_AXIS_LENGTH_CZ
};

const char MSG_SELFTEST_BEDHEATER_EN[] PROGMEM = "Bed / Heater";
const char * const MSG_SELFTEST_BEDHEATER_LANG_TABLE[1] PROGMEM = {
	MSG_SELFTEST_BEDHEATER_EN
};

const char MSG_SELFTEST_CHECK_ALLCORRECT_EN[] PROGMEM = "All correct      ";
const char MSG_SELFTEST_CHECK_ALLCORRECT_CZ[] PROGMEM = "Vse OK           ";
const char * const MSG_SELFTEST_CHECK_ALLCORRECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_ALLCORRECT_EN,
	MSG_SELFTEST_CHECK_ALLCORRECT_CZ
};

const char MSG_SELFTEST_CHECK_BED_EN[] PROGMEM = "Checking bed     ";
const char MSG_SELFTEST_CHECK_BED_CZ[] PROGMEM = "Kontrola bed     ";
const char * const MSG_SELFTEST_CHECK_BED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_BED_EN,
	MSG_SELFTEST_CHECK_BED_CZ
};

const char MSG_SELFTEST_CHECK_ENDSTOPS_EN[] PROGMEM = "Checking endstops";
const char MSG_SELFTEST_CHECK_ENDSTOPS_CZ[] PROGMEM = "Kontrola endstops";
const char * const MSG_SELFTEST_CHECK_ENDSTOPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_ENDSTOPS_EN,
	MSG_SELFTEST_CHECK_ENDSTOPS_CZ
};

const char MSG_SELFTEST_CHECK_FSENSOR_EN[] PROGMEM = "Checking sensors ";
const char MSG_SELFTEST_CHECK_FSENSOR_CZ[] PROGMEM = "Kontrola senzoru";
const char * const MSG_SELFTEST_CHECK_FSENSOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_FSENSOR_EN,
	MSG_SELFTEST_CHECK_FSENSOR_CZ
};

const char MSG_SELFTEST_CHECK_HOTEND_EN[] PROGMEM = "Checking hotend  ";
const char MSG_SELFTEST_CHECK_HOTEND_CZ[] PROGMEM = "Kontrola hotend  ";
const char * const MSG_SELFTEST_CHECK_HOTEND_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_HOTEND_EN,
	MSG_SELFTEST_CHECK_HOTEND_CZ
};

const char MSG_SELFTEST_CHECK_X_EN[] PROGMEM = "Checking X axis  ";
const char MSG_SELFTEST_CHECK_X_CZ[] PROGMEM = "Kontrola X axis  ";
const char * const MSG_SELFTEST_CHECK_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_X_EN,
	MSG_SELFTEST_CHECK_X_CZ
};

const char MSG_SELFTEST_CHECK_Y_EN[] PROGMEM = "Checking Y axis  ";
const char MSG_SELFTEST_CHECK_Y_CZ[] PROGMEM = "Kontrola Y axis  ";
const char * const MSG_SELFTEST_CHECK_Y_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_Y_EN,
	MSG_SELFTEST_CHECK_Y_CZ
};

const char MSG_SELFTEST_CHECK_Z_EN[] PROGMEM = "Checking Z axis  ";
const char MSG_SELFTEST_CHECK_Z_CZ[] PROGMEM = "Kontrola Z axis  ";
const char * const MSG_SELFTEST_CHECK_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_Z_EN,
	MSG_SELFTEST_CHECK_Z_CZ
};

const char MSG_SELFTEST_COOLING_FAN_EN[] PROGMEM = "Front print fan?";
const char MSG_SELFTEST_COOLING_FAN_CZ[] PROGMEM = "Predni tiskovy vent?";
const char * const MSG_SELFTEST_COOLING_FAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_COOLING_FAN_EN,
	MSG_SELFTEST_COOLING_FAN_CZ
};

const char MSG_SELFTEST_ENDSTOP_EN[] PROGMEM = "Endstop";
const char * const MSG_SELFTEST_ENDSTOP_LANG_TABLE[1] PROGMEM = {
	MSG_SELFTEST_ENDSTOP_EN
};

const char MSG_SELFTEST_ENDSTOPS_EN[] PROGMEM = "Endstops";
const char * const MSG_SELFTEST_ENDSTOPS_LANG_TABLE[1] PROGMEM = {
	MSG_SELFTEST_ENDSTOPS_EN
};

const char MSG_SELFTEST_ENDSTOP_NOTHIT_EN[] PROGMEM = "Endstop not hit";
const char * const MSG_SELFTEST_ENDSTOP_NOTHIT_LANG_TABLE[1] PROGMEM = {
	MSG_SELFTEST_ENDSTOP_NOTHIT_EN
};

const char MSG_SELFTEST_ERROR_EN[] PROGMEM = "Selftest error !";
const char * const MSG_SELFTEST_ERROR_LANG_TABLE[1] PROGMEM = {
	MSG_SELFTEST_ERROR_EN
};

const char MSG_SELFTEST_EXTRUDER_FAN_EN[] PROGMEM = "Left hotend fan?";
const char MSG_SELFTEST_EXTRUDER_FAN_CZ[] PROGMEM = "Levy vent na trysce?";
const char * const MSG_SELFTEST_EXTRUDER_FAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_EXTRUDER_FAN_EN,
	MSG_SELFTEST_EXTRUDER_FAN_CZ
};

const char MSG_SELFTEST_EXTRUDER_FAN_SPEED_EN[] PROGMEM = "Extruder fan:";
const char MSG_SELFTEST_EXTRUDER_FAN_SPEED_CZ[] PROGMEM = "Levy vent.:";
const char * const MSG_SELFTEST_EXTRUDER_FAN_SPEED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_EXTRUDER_FAN_SPEED_EN,
	MSG_SELFTEST_EXTRUDER_FAN_SPEED_CZ
};

const char MSG_SELFTEST_FAILED_EN[] PROGMEM = "Selftest failed  ";
const char MSG_SELFTEST_FAILED_CZ[] PROGMEM = "Selftest selhal  ";
const char * const MSG_SELFTEST_FAILED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FAILED_EN,
	MSG_SELFTEST_FAILED_CZ
};

const char MSG_SELFTEST_FAN_EN[] PROGMEM = "Fan test";
const char MSG_SELFTEST_FAN_CZ[] PROGMEM = "Test ventilatoru";
const char * const MSG_SELFTEST_FAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FAN_EN,
	MSG_SELFTEST_FAN_CZ
};

const char MSG_SELFTEST_FANS_EN[] PROGMEM = "Front/left fans";
const char MSG_SELFTEST_FANS_CZ[] PROGMEM = "Predni/levy vent.";
const char * const MSG_SELFTEST_FANS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FANS_EN,
	MSG_SELFTEST_FANS_CZ
};

const char MSG_SELFTEST_FAN_NO_EN[] PROGMEM = "Not spinning";
const char MSG_SELFTEST_FAN_NO_CZ[] PROGMEM = "Netoci se";
const char * const MSG_SELFTEST_FAN_NO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FAN_NO_EN,
	MSG_SELFTEST_FAN_NO_CZ
};

const char MSG_SELFTEST_FAN_YES_EN[] PROGMEM = "Spinning";
const char MSG_SELFTEST_FAN_YES_CZ[] PROGMEM = "Toci se";
const char * const MSG_SELFTEST_FAN_YES_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FAN_YES_EN,
	MSG_SELFTEST_FAN_YES_CZ
};

const char MSG_SELFTEST_FILAMENT_SENSOR_EN[] PROGMEM = "Filament sensor:";
const char MSG_SELFTEST_FILAMENT_SENSOR_CZ[] PROGMEM = "Senzor filamentu:";
const char * const MSG_SELFTEST_FILAMENT_SENSOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FILAMENT_SENSOR_EN,
	MSG_SELFTEST_FILAMENT_SENSOR_CZ
};

const char MSG_SELFTEST_HEATERTHERMISTOR_EN[] PROGMEM = "Heater/Thermistor";
const char * const MSG_SELFTEST_HEATERTHERMISTOR_LANG_TABLE[1] PROGMEM = {
	MSG_SELFTEST_HEATERTHERMISTOR_EN
};

const char MSG_SELFTEST_MOTOR_EN[] PROGMEM = "Motor";
const char * const MSG_SELFTEST_MOTOR_LANG_TABLE[1] PROGMEM = {
	MSG_SELFTEST_MOTOR_EN
};

const char MSG_SELFTEST_NOTCONNECTED_EN[] PROGMEM = "Not connected";
const char MSG_SELFTEST_NOTCONNECTED_CZ[] PROGMEM = "Nezapojeno    ";
const char * const MSG_SELFTEST_NOTCONNECTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_NOTCONNECTED_EN,
	MSG_SELFTEST_NOTCONNECTED_CZ
};

const char MSG_SELFTEST_OK_EN[] PROGMEM = "Self test OK";
const char * const MSG_SELFTEST_OK_LANG_TABLE[1] PROGMEM = {
	MSG_SELFTEST_OK_EN
};

const char MSG_SELFTEST_PLEASECHECK_EN[] PROGMEM = "Please check :";
const char MSG_SELFTEST_PLEASECHECK_CZ[] PROGMEM = "Zkontrolujte :";
const char * const MSG_SELFTEST_PLEASECHECK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_PLEASECHECK_EN,
	MSG_SELFTEST_PLEASECHECK_CZ
};

const char MSG_SELFTEST_PRINT_FAN_SPEED_EN[] PROGMEM = "Print fan:";
const char MSG_SELFTEST_PRINT_FAN_SPEED_CZ[] PROGMEM = "Tiskovy vent.:";
const char * const MSG_SELFTEST_PRINT_FAN_SPEED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_PRINT_FAN_SPEED_EN,
	MSG_SELFTEST_PRINT_FAN_SPEED_CZ
};

const char MSG_SELFTEST_START_EN[] PROGMEM = "Self test start  ";
const char * const MSG_SELFTEST_START_LANG_TABLE[1] PROGMEM = {
	MSG_SELFTEST_START_EN
};

const char MSG_SELFTEST_SWAPPED_EN[] PROGMEM = "Swapped";
const char MSG_SELFTEST_SWAPPED_CZ[] PROGMEM = "Prohozene";
const char * const MSG_SELFTEST_SWAPPED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_SWAPPED_EN,
	MSG_SELFTEST_SWAPPED_CZ
};

const char MSG_SELFTEST_WIRINGERROR_EN[] PROGMEM = "Wiring error";
const char MSG_SELFTEST_WIRINGERROR_CZ[] PROGMEM = "Chyba zapojeni";
const char * const MSG_SELFTEST_WIRINGERROR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_WIRINGERROR_EN,
	MSG_SELFTEST_WIRINGERROR_CZ
};

const char MSG_SERIAL_ERROR_MENU_STRUCTURE_EN[] PROGMEM = "Error in menu structure";
const char * const MSG_SERIAL_ERROR_MENU_STRUCTURE_LANG_TABLE[1] PROGMEM = {
	MSG_SERIAL_ERROR_MENU_STRUCTURE_EN
};

const char MSG_SETTINGS_EN[] PROGMEM = "Settings";
const char MSG_SETTINGS_CZ[] PROGMEM = "Nastaveni";
const char * const MSG_SETTINGS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SETTINGS_EN,
	MSG_SETTINGS_CZ
};

const char MSG_SET_HOME_OFFSETS_EN[] PROGMEM = "Set home offsets";
const char * const MSG_SET_HOME_OFFSETS_LANG_TABLE[1] PROGMEM = {
	MSG_SET_HOME_OFFSETS_EN
};

const char MSG_SET_ORIGIN_EN[] PROGMEM = "Set origin";
const char * const MSG_SET_ORIGIN_LANG_TABLE[1] PROGMEM = {
	MSG_SET_ORIGIN_EN
};

const char MSG_SET_TEMPERATURE_EN[] PROGMEM = "Set temperature:";
const char MSG_SET_TEMPERATURE_CZ[] PROGMEM = "Nastavte teplotu:";
const char * const MSG_SET_TEMPERATURE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SET_TEMPERATURE_EN,
	MSG_SET_TEMPERATURE_CZ
};

const char MSG_SEVERE_SKEW_EN[] PROGMEM = "Severe skew:";
const char MSG_SEVERE_SKEW_CZ[] PROGMEM = "Tezke zkoseni:";
const char * const MSG_SEVERE_SKEW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SEVERE_SKEW_EN,
	MSG_SEVERE_SKEW_CZ
};

const char MSG_SHOW_END_STOPS_EN[] PROGMEM = "Show end stops";
const char MSG_SHOW_END_STOPS_CZ[] PROGMEM = "Stav konc. spin.";
const char * const MSG_SHOW_END_STOPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SHOW_END_STOPS_EN,
	MSG_SHOW_END_STOPS_CZ
};

const char MSG_SILENT_MODE_OFF_EN[] PROGMEM = "Mode [high power]";
const char MSG_SILENT_MODE_OFF_CZ[] PROGMEM = "Mod  [vys. vykon]";
const char * const MSG_SILENT_MODE_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SILENT_MODE_OFF_EN,
	MSG_SILENT_MODE_OFF_CZ
};

const char MSG_SILENT_MODE_ON_EN[] PROGMEM = "Mode     [silent]";
const char MSG_SILENT_MODE_ON_CZ[] PROGMEM = "Mod       [tichy]";
const char * const MSG_SILENT_MODE_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SILENT_MODE_ON_EN,
	MSG_SILENT_MODE_ON_CZ
};

const char MSG_SLIGHT_SKEW_EN[] PROGMEM = "Slight skew:";
const char MSG_SLIGHT_SKEW_CZ[] PROGMEM = "Lehke zkoseni:";
const char * const MSG_SLIGHT_SKEW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SLIGHT_SKEW_EN,
	MSG_SLIGHT_SKEW_CZ
};

const char MSG_SOFTWARE_RESET_EN[] PROGMEM = " Software Reset";
const char * const MSG_SOFTWARE_RESET_LANG_TABLE[1] PROGMEM = {
	MSG_SOFTWARE_RESET_EN
};

const char MSG_SORTING_EN[] PROGMEM = "Sorting files";
const char MSG_SORTING_CZ[] PROGMEM = "Trideni souboru";
const char * const MSG_SORTING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SORTING_EN,
	MSG_SORTING_CZ
};

const char MSG_SORT_ALPHA_EN[] PROGMEM = "Sort:  [Alphabet]";
const char MSG_SORT_ALPHA_CZ[] PROGMEM = "Trideni [Abeceda]";
const char * const MSG_SORT_ALPHA_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SORT_ALPHA_EN,
	MSG_SORT_ALPHA_CZ
};

const char MSG_SORT_NONE_EN[] PROGMEM = "Sort:      [None]";
const char MSG_SORT_NONE_CZ[] PROGMEM = "Trideni   [Zadne]";
const char * const MSG_SORT_NONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SORT_NONE_EN,
	MSG_SORT_NONE_CZ
};

const char MSG_SORT_TIME_EN[] PROGMEM = "Sort:      [Time]";
const char MSG_SORT_TIME_CZ[] PROGMEM = "Trideni     [Cas]";
const char * const MSG_SORT_TIME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SORT_TIME_EN,
	MSG_SORT_TIME_CZ
};

const char MSG_SPEED_EN[] PROGMEM = "Speed";
const char MSG_SPEED_CZ[] PROGMEM = "Rychlost";
const char * const MSG_SPEED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SPEED_EN,
	MSG_SPEED_CZ
};

const char MSG_STACK_ERROR_EN[] PROGMEM = "Error - static memory has been overwritten";
const char MSG_STACK_ERROR_CZ[] PROGMEM = "Chyba - Doslo k prepisu staticke pameti!";
const char * const MSG_STACK_ERROR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STACK_ERROR_EN,
	MSG_STACK_ERROR_CZ
};

const char MSG_STATISTICS_EN[] PROGMEM = "Statistics  ";
const char MSG_STATISTICS_CZ[] PROGMEM = "Statistika  ";
const char * const MSG_STATISTICS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATISTICS_EN,
	MSG_STATISTICS_CZ
};

const char MSG_STATS_FILAMENTUSED_EN[] PROGMEM = "Filament used:  ";
const char MSG_STATS_FILAMENTUSED_CZ[] PROGMEM = "Filament :  ";
const char * const MSG_STATS_FILAMENTUSED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_FILAMENTUSED_EN,
	MSG_STATS_FILAMENTUSED_CZ
};

const char MSG_STATS_PRINTTIME_EN[] PROGMEM = "Print time:  ";
const char MSG_STATS_PRINTTIME_CZ[] PROGMEM = "Cas tisku :  ";
const char * const MSG_STATS_PRINTTIME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_PRINTTIME_EN,
	MSG_STATS_PRINTTIME_CZ
};

const char MSG_STATS_TOTALFILAMENT_EN[] PROGMEM = "Total filament :";
const char MSG_STATS_TOTALFILAMENT_CZ[] PROGMEM = "Filament celkem :";
const char * const MSG_STATS_TOTALFILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_TOTALFILAMENT_EN,
	MSG_STATS_TOTALFILAMENT_CZ
};

const char MSG_STATS_TOTALPRINTTIME_EN[] PROGMEM = "Total print time :";
const char MSG_STATS_TOTALPRINTTIME_CZ[] PROGMEM = "Celkovy cas :";
const char * const MSG_STATS_TOTALPRINTTIME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_TOTALPRINTTIME_EN,
	MSG_STATS_TOTALPRINTTIME_CZ
};

const char MSG_STEALTH_MODE_OFF_EN[] PROGMEM = "Mode     [Normal]";
const char MSG_STEALTH_MODE_OFF_CZ[] PROGMEM = "Mod      [Normal]";
const char * const MSG_STEALTH_MODE_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STEALTH_MODE_OFF_EN,
	MSG_STEALTH_MODE_OFF_CZ
};

const char MSG_STEALTH_MODE_ON_EN[] PROGMEM = "Mode    [Stealth]";
const char MSG_STEALTH_MODE_ON_CZ[] PROGMEM = "Mod     [Stealth]";
const char * const MSG_STEALTH_MODE_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STEALTH_MODE_ON_EN,
	MSG_STEALTH_MODE_ON_CZ
};

const char MSG_STEEL_SHEET_CHECK_EN[] PROGMEM = "Is steel sheet on heatbed?";
const char MSG_STEEL_SHEET_CHECK_CZ[] PROGMEM = "Je tiskovy plat na heatbed?";
const char * const MSG_STEEL_SHEET_CHECK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STEEL_SHEET_CHECK_EN,
	MSG_STEEL_SHEET_CHECK_CZ
};

const char MSG_STEPPER_TIMER_OVERFLOW_ERROR_EN[] PROGMEM = "Error - stepper timer overflow";
const char * const MSG_STEPPER_TIMER_OVERFLOW_ERROR_LANG_TABLE[1] PROGMEM = {
	MSG_STEPPER_TIMER_OVERFLOW_ERROR_EN
};

const char MSG_STEPPER_TOO_HIGH_EN[] PROGMEM = "Steprate too high: ";
const char * const MSG_STEPPER_TOO_HIGH_LANG_TABLE[1] PROGMEM = {
	MSG_STEPPER_TOO_HIGH_EN
};

const char MSG_STOPPED_EN[] PROGMEM = "STOPPED. ";
const char * const MSG_STOPPED_LANG_TABLE[1] PROGMEM = {
	MSG_STOPPED_EN
};

const char MSG_STOP_PRINT_EN[] PROGMEM = "Stop print";
const char MSG_STOP_PRINT_CZ[] PROGMEM = "Zastavit tisk";
const char * const MSG_STOP_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STOP_PRINT_EN,
	MSG_STOP_PRINT_CZ
};

const char MSG_STORE_EPROM_EN[] PROGMEM = "Store memory";
const char * const MSG_STORE_EPROM_LANG_TABLE[1] PROGMEM = {
	MSG_STORE_EPROM_EN
};

const char MSG_SUPPORT_EN[] PROGMEM = "Support";
const char MSG_SUPPORT_CZ[] PROGMEM = "Podpora";
const char * const MSG_SUPPORT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SUPPORT_EN,
	MSG_SUPPORT_CZ
};

const char MSG_SWITCH_PS_OFF_EN[] PROGMEM = "Switch power off";
const char MSG_SWITCH_PS_OFF_CZ[] PROGMEM = "Zapnout zdroj";
const char * const MSG_SWITCH_PS_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SWITCH_PS_OFF_EN,
	MSG_SWITCH_PS_OFF_CZ
};

const char MSG_SWITCH_PS_ON_EN[] PROGMEM = "Switch power on";
const char MSG_SWITCH_PS_ON_CZ[] PROGMEM = "Vypnout zdroj";
const char * const MSG_SWITCH_PS_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SWITCH_PS_ON_EN,
	MSG_SWITCH_PS_ON_CZ
};

const char MSG_TAKE_EFFECT_EN[] PROGMEM = " for take effect";
const char MSG_TAKE_EFFECT_CZ[] PROGMEM = " pro projeveni zmen";
const char * const MSG_TAKE_EFFECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TAKE_EFFECT_EN,
	MSG_TAKE_EFFECT_CZ
};

const char MSG_TEMPERATURE_EN[] PROGMEM = "Temperature";
const char MSG_TEMPERATURE_CZ[] PROGMEM = "Teplota";
const char * const MSG_TEMPERATURE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMPERATURE_EN,
	MSG_TEMPERATURE_CZ
};

const char MSG_TEMP_CALIBRATION_EN[] PROGMEM = "Temp. cal.          ";
const char MSG_TEMP_CALIBRATION_CZ[] PROGMEM = "Tepl. kal.          ";
const char * const MSG_TEMP_CALIBRATION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMP_CALIBRATION_EN,
	MSG_TEMP_CALIBRATION_CZ
};

const char MSG_TEMP_CALIBRATION_DONE_EN[] PROGMEM = "Temperature calibration is finished. Click to continue.";
const char MSG_TEMP_CALIBRATION_DONE_CZ[] PROGMEM = "Teplotni kalibrace dokoncena. Pokracujte stiskem tlacitka.";
const char * const MSG_TEMP_CALIBRATION_DONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMP_CALIBRATION_DONE_EN,
	MSG_TEMP_CALIBRATION_DONE_CZ
};

const char MSG_TEMP_CALIBRATION_OFF_EN[] PROGMEM = "Temp. cal.  [off]";
const char MSG_TEMP_CALIBRATION_OFF_CZ[] PROGMEM = "Tepl. kal.  [vyp]";
const char * const MSG_TEMP_CALIBRATION_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMP_CALIBRATION_OFF_EN,
	MSG_TEMP_CALIBRATION_OFF_CZ
};

const char MSG_TEMP_CALIBRATION_ON_EN[] PROGMEM = "Temp. cal.   [on]";
const char MSG_TEMP_CALIBRATION_ON_CZ[] PROGMEM = "Tepl. kal.  [zap]";
const char * const MSG_TEMP_CALIBRATION_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMP_CALIBRATION_ON_EN,
	MSG_TEMP_CALIBRATION_ON_CZ
};

const char MSG_TEMP_CAL_WARNING_EN[] PROGMEM = "Stable ambient temperature 21-26C is needed a rigid stand is required.";
const char * const MSG_TEMP_CAL_WARNING_LANG_TABLE[1] PROGMEM = {
	MSG_TEMP_CAL_WARNING_EN
};

const char MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_EN[] PROGMEM = "SD card  [normal]";
const char * const MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_LANG_TABLE[1] PROGMEM = {
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_EN
};

const char MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_EN[] PROGMEM = "SD card [FlshAir]";
const char * const MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_LANG_TABLE[1] PROGMEM = {
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_EN
};

const char MSG_TUNE_EN[] PROGMEM = "Tune";
const char MSG_TUNE_CZ[] PROGMEM = "Ladit";
const char * const MSG_TUNE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TUNE_EN,
	MSG_TUNE_CZ
};

const char MSG_UNKNOWN_COMMAND_EN[] PROGMEM = "Unknown command: \"";
const char * const MSG_UNKNOWN_COMMAND_LANG_TABLE[1] PROGMEM = {
	MSG_UNKNOWN_COMMAND_EN
};

const char MSG_UNLOADING_FILAMENT_EN[] PROGMEM = "Unloading filament";
const char MSG_UNLOADING_FILAMENT_CZ[] PROGMEM = "Vysouvam filament";
const char * const MSG_UNLOADING_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOADING_FILAMENT_EN,
	MSG_UNLOADING_FILAMENT_CZ
};

const char MSG_UNLOAD_ALL_EN[] PROGMEM = "Unload all";
const char MSG_UNLOAD_ALL_CZ[] PROGMEM = "Vyjmout vse";
const char * const MSG_UNLOAD_ALL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_ALL_EN,
	MSG_UNLOAD_ALL_CZ
};

const char MSG_UNLOAD_FILAMENT_EN[] PROGMEM = "Unload filament";
const char MSG_UNLOAD_FILAMENT_CZ[] PROGMEM = "Vyjmout filament";
const char * const MSG_UNLOAD_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_EN,
	MSG_UNLOAD_FILAMENT_CZ
};

const char MSG_UNLOAD_FILAMENT_1_EN[] PROGMEM = "Unload filament 1";
const char MSG_UNLOAD_FILAMENT_1_CZ[] PROGMEM = "Vyjmout filam. 1";
const char * const MSG_UNLOAD_FILAMENT_1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_1_EN,
	MSG_UNLOAD_FILAMENT_1_CZ
};

const char MSG_UNLOAD_FILAMENT_2_EN[] PROGMEM = "Unload filament 2";
const char MSG_UNLOAD_FILAMENT_2_CZ[] PROGMEM = "Vyjmout filam. 2";
const char * const MSG_UNLOAD_FILAMENT_2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_2_EN,
	MSG_UNLOAD_FILAMENT_2_CZ
};

const char MSG_UNLOAD_FILAMENT_3_EN[] PROGMEM = "Unload filament 3";
const char MSG_UNLOAD_FILAMENT_3_CZ[] PROGMEM = "Vyjmout filam. 3";
const char * const MSG_UNLOAD_FILAMENT_3_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_3_EN,
	MSG_UNLOAD_FILAMENT_3_CZ
};

const char MSG_UNLOAD_FILAMENT_4_EN[] PROGMEM = "Unload filament 4";
const char MSG_UNLOAD_FILAMENT_4_CZ[] PROGMEM = "Vyjmout filam. 4";
const char * const MSG_UNLOAD_FILAMENT_4_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_4_EN,
	MSG_UNLOAD_FILAMENT_4_CZ
};

const char MSG_UNLOAD_SUCCESSFUL_EN[] PROGMEM = "Was filament unload successful?";
const char MSG_UNLOAD_SUCCESSFUL_CZ[] PROGMEM = "Bylo vysunuti filamentu uspesne?";
const char * const MSG_UNLOAD_SUCCESSFUL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_SUCCESSFUL_EN,
	MSG_UNLOAD_SUCCESSFUL_CZ
};

const char MSG_USB_PRINTING_EN[] PROGMEM = "USB printing  ";
const char MSG_USB_PRINTING_CZ[] PROGMEM = "Tisk z USB  ";
const char * const MSG_USB_PRINTING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_USB_PRINTING_EN,
	MSG_USB_PRINTING_CZ
};

const char MSG_USED_EN[] PROGMEM = "Used during print";
const char MSG_USED_CZ[] PROGMEM = "Pouzite behem tisku";
const char * const MSG_USED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_USED_EN,
	MSG_USED_CZ
};

const char MSG_USERWAIT_EN[] PROGMEM = "Wait for user...";
const char * const MSG_USERWAIT_LANG_TABLE[1] PROGMEM = {
	MSG_USERWAIT_EN
};

const char MSG_V2_CALIBRATION_EN[] PROGMEM = "First layer cal.";
const char MSG_V2_CALIBRATION_CZ[] PROGMEM = "Kal. prvni vrstvy";
const char * const MSG_V2_CALIBRATION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_V2_CALIBRATION_EN,
	MSG_V2_CALIBRATION_CZ
};

const char MSG_VMIN_EN[] PROGMEM = "Vmin";
const char * const MSG_VMIN_LANG_TABLE[1] PROGMEM = {
	MSG_VMIN_EN
};

const char MSG_VOLUMETRIC_EN[] PROGMEM = "Filament";
const char * const MSG_VOLUMETRIC_LANG_TABLE[1] PROGMEM = {
	MSG_VOLUMETRIC_EN
};

const char MSG_VOLUMETRIC_ENABLED_EN[] PROGMEM = "E in mm3";
const char * const MSG_VOLUMETRIC_ENABLED_LANG_TABLE[1] PROGMEM = {
	MSG_VOLUMETRIC_ENABLED_EN
};

const char MSG_VTRAV_MIN_EN[] PROGMEM = "VTrav min";
const char * const MSG_VTRAV_MIN_LANG_TABLE[1] PROGMEM = {
	MSG_VTRAV_MIN_EN
};

const char MSG_WAITING_TEMP_EN[] PROGMEM = "Waiting for nozzle and bed cooling";
const char MSG_WAITING_TEMP_CZ[] PROGMEM = "Cekani na zchladnuti trysky a podlozky.";
const char * const MSG_WAITING_TEMP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WAITING_TEMP_EN,
	MSG_WAITING_TEMP_CZ
};

const char MSG_WATCH_EN[] PROGMEM = "Info screen";
const char MSG_WATCH_CZ[] PROGMEM = "Informace";
const char * const MSG_WATCH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WATCH_EN,
	MSG_WATCH_CZ
};

const char MSG_WATCHDOG_RESET_EN[] PROGMEM = " Watchdog Reset";
const char * const MSG_WATCHDOG_RESET_LANG_TABLE[1] PROGMEM = {
	MSG_WATCHDOG_RESET_EN
};

const char MSG_WIZARD_EN[] PROGMEM = "Wizard";
const char * const MSG_WIZARD_LANG_TABLE[1] PROGMEM = {
	MSG_WIZARD_EN
};

const char MSG_WIZARD_CALIBRATION_FAILED_EN[] PROGMEM = "Please check our handbook and fix the problem. Then resume the Wizard by rebooting the printer.";
const char MSG_WIZARD_CALIBRATION_FAILED_CZ[] PROGMEM = "Prosim nahlednete do manualu a opravte problem. Po te obnovte Wizarda rebootovanim tiskarny.";
const char * const MSG_WIZARD_CALIBRATION_FAILED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_CALIBRATION_FAILED_EN,
	MSG_WIZARD_CALIBRATION_FAILED_CZ
};

const char MSG_WIZARD_CLEAN_HEATBED_EN[] PROGMEM = "Please clean heatbed and then press the knob.";
const char MSG_WIZARD_CLEAN_HEATBED_CZ[] PROGMEM = "Prosim ocistete heatbed a stisknete tlacitko.";
const char * const MSG_WIZARD_CLEAN_HEATBED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_CLEAN_HEATBED_EN,
	MSG_WIZARD_CLEAN_HEATBED_CZ
};

const char MSG_WIZARD_DONE_EN[] PROGMEM = "All is done. Happy printing!";
const char MSG_WIZARD_DONE_CZ[] PROGMEM = "Vse je hotovo.";
const char * const MSG_WIZARD_DONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_DONE_EN,
	MSG_WIZARD_DONE_CZ
};

const char MSG_WIZARD_FILAMENT_LOADED_EN[] PROGMEM = "Is filament loaded?";
const char MSG_WIZARD_FILAMENT_LOADED_CZ[] PROGMEM = "Je filament zaveden?";
const char * const MSG_WIZARD_FILAMENT_LOADED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_FILAMENT_LOADED_EN,
	MSG_WIZARD_FILAMENT_LOADED_CZ
};

const char MSG_WIZARD_HEATING_EN[] PROGMEM = "Preheating nozzle. Please wait.";
const char MSG_WIZARD_HEATING_CZ[] PROGMEM = "Predehrivam trysku. Prosim cekejte.";
const char * const MSG_WIZARD_HEATING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_HEATING_EN,
	MSG_WIZARD_HEATING_CZ
};

const char MSG_WIZARD_INSERT_CORRECT_FILAMENT_EN[] PROGMEM = "Please load PLA filament and then resume Wizard by rebooting the printer.";
const char MSG_WIZARD_INSERT_CORRECT_FILAMENT_CZ[] PROGMEM = "Prosim zavedte PLA filament a po te obnovte Wizarda stisknutim reset tlacitka.";
const char * const MSG_WIZARD_INSERT_CORRECT_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_INSERT_CORRECT_FILAMENT_EN,
	MSG_WIZARD_INSERT_CORRECT_FILAMENT_CZ
};

const char MSG_WIZARD_LOAD_FILAMENT_EN[] PROGMEM = "Please insert PLA filament to the extruder, then press knob to load it.";
const char MSG_WIZARD_LOAD_FILAMENT_CZ[] PROGMEM = "Prosim vlozte PLA filament do extruderu, po te stisknete tlacitko pro zavedeni filamentu.";
const char * const MSG_WIZARD_LOAD_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_LOAD_FILAMENT_EN,
	MSG_WIZARD_LOAD_FILAMENT_CZ
};

const char MSG_WIZARD_PLA_FILAMENT_EN[] PROGMEM = "Is it PLA filament?";
const char MSG_WIZARD_PLA_FILAMENT_CZ[] PROGMEM = "Je to PLA filament?";
const char * const MSG_WIZARD_PLA_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_PLA_FILAMENT_EN,
	MSG_WIZARD_PLA_FILAMENT_CZ
};

const char MSG_WIZARD_QUIT_EN[] PROGMEM = "You can always resume the Wizard from Calibration -> Wizard.";
const char MSG_WIZARD_QUIT_CZ[] PROGMEM = "Wizarda muzete kdykoliv znovu spustit z menu Calibration -> Wizard";
const char * const MSG_WIZARD_QUIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_QUIT_EN,
	MSG_WIZARD_QUIT_CZ
};

const char MSG_WIZARD_REPEAT_V2_CAL_EN[] PROGMEM = "Do you want to repeat last step to readjust distance between nozzle and heatbed?";
const char MSG_WIZARD_REPEAT_V2_CAL_CZ[] PROGMEM = "Chcete opakovat posledni krok a pozmenit vzdalenost mezi tryskou a heatbed?";
const char * const MSG_WIZARD_REPEAT_V2_CAL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_REPEAT_V2_CAL_EN,
	MSG_WIZARD_REPEAT_V2_CAL_CZ
};

const char MSG_WIZARD_RERUN_EN[] PROGMEM = "Running Wizard will delete current calibration results and start from the beginning. Continue?";
const char MSG_WIZARD_RERUN_CZ[] PROGMEM = "Spusteni Wizarda vymaze ulozene vysledky vsech kalibraci a spusti kalibracni proces od zacatku. Pokracovat?";
const char * const MSG_WIZARD_RERUN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_RERUN_EN,
	MSG_WIZARD_RERUN_CZ
};

const char MSG_WIZARD_SELFTEST_EN[] PROGMEM = "First, I will run the selftest to check most common assembly problems.";
const char MSG_WIZARD_SELFTEST_CZ[] PROGMEM = "Nejdriv pomoci selftestu zkontoluji nejcastejsi chyby vznikajici pri sestaveni tiskarny.";
const char * const MSG_WIZARD_SELFTEST_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_SELFTEST_EN,
	MSG_WIZARD_SELFTEST_CZ
};

const char MSG_WIZARD_V2_CAL_EN[] PROGMEM = "Now I will calibrate distance between tip of the nozzle and heatbed surface.";
const char MSG_WIZARD_V2_CAL_CZ[] PROGMEM = "Nyni zkalibruji vzdalenost mezi koncem trysky a povrchem heatbedu.";
const char * const MSG_WIZARD_V2_CAL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_V2_CAL_EN,
	MSG_WIZARD_V2_CAL_CZ
};

const char MSG_WIZARD_V2_CAL_2_EN[] PROGMEM = "I will start to print line and you will gradually lower the nozzle by rotating the knob, until you reach optimal height. Check the pictures in our handbook in chapter Calibration.";
const char MSG_WIZARD_V2_CAL_2_CZ[] PROGMEM = "Zacnu tisknout linku a Vy budete postupne snizovat trysku otacenim tlacitka dokud nedosahnete optimalni vysky. Prohlednete si obrazky v nasi prirucce v kapitole Kalibrace";
const char * const MSG_WIZARD_V2_CAL_2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_V2_CAL_2_EN,
	MSG_WIZARD_V2_CAL_2_CZ
};

const char MSG_WIZARD_WELCOME_EN[] PROGMEM = "Hi, I am your Original Prusa i3 printer. Would you like me to guide you through the setup process?";
const char MSG_WIZARD_WELCOME_CZ[] PROGMEM = "Dobry den, jsem vase tiskarna Original Prusa i3. Chcete abych Vas provedla kalibracnim procesem?";
const char * const MSG_WIZARD_WELCOME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_WELCOME_EN,
	MSG_WIZARD_WELCOME_CZ
};

const char MSG_WIZARD_WILL_PREHEAT_EN[] PROGMEM = "Now I will preheat nozzle for PLA.";
const char MSG_WIZARD_WILL_PREHEAT_CZ[] PROGMEM = "Nyni predehreji trysku pro PLA.";
const char * const MSG_WIZARD_WILL_PREHEAT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_WILL_PREHEAT_EN,
	MSG_WIZARD_WILL_PREHEAT_CZ
};

const char MSG_WIZARD_XYZ_CAL_EN[] PROGMEM = "I will run xyz calibration now. It will take approx. 12 mins.";
const char MSG_WIZARD_XYZ_CAL_CZ[] PROGMEM = "Nyni provedu xyz kalibraci. Zabere to priblizne 12 min.";
const char * const MSG_WIZARD_XYZ_CAL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_XYZ_CAL_EN,
	MSG_WIZARD_XYZ_CAL_CZ
};

const char MSG_WIZARD_Z_CAL_EN[] PROGMEM = "I will run z calibration now.";
const char MSG_WIZARD_Z_CAL_CZ[] PROGMEM = "Nyni provedu z kalibraci.";
const char * const MSG_WIZARD_Z_CAL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_Z_CAL_EN,
	MSG_WIZARD_Z_CAL_CZ
};

const char MSG_XYZ_DETAILS_EN[] PROGMEM = "XYZ cal. details";
const char MSG_XYZ_DETAILS_CZ[] PROGMEM = "Detaily XYZ kal.";
const char * const MSG_XYZ_DETAILS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_XYZ_DETAILS_EN,
	MSG_XYZ_DETAILS_CZ
};

const char MSG_X_MAX_EN[] PROGMEM = "x_max: ";
const char * const MSG_X_MAX_LANG_TABLE[1] PROGMEM = {
	MSG_X_MAX_EN
};

const char MSG_X_MIN_EN[] PROGMEM = "x_min: ";
const char * const MSG_X_MIN_LANG_TABLE[1] PROGMEM = {
	MSG_X_MIN_EN
};

const char MSG_YES_EN[] PROGMEM = "Yes";
const char MSG_YES_CZ[] PROGMEM = "Ano";
const char * const MSG_YES_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_YES_EN,
	MSG_YES_CZ
};

const char MSG_Y_DISTANCE_FROM_MIN_EN[] PROGMEM = "Y distance from min:";
const char MSG_Y_DISTANCE_FROM_MIN_CZ[] PROGMEM = "Y vzdalenost od min:";
const char * const MSG_Y_DISTANCE_FROM_MIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_Y_DISTANCE_FROM_MIN_EN,
	MSG_Y_DISTANCE_FROM_MIN_CZ
};

const char MSG_Y_MAX_EN[] PROGMEM = "y_max: ";
const char * const MSG_Y_MAX_LANG_TABLE[1] PROGMEM = {
	MSG_Y_MAX_EN
};

const char MSG_Y_MIN_EN[] PROGMEM = "y_min: ";
const char * const MSG_Y_MIN_LANG_TABLE[1] PROGMEM = {
	MSG_Y_MIN_EN
};

const char MSG_ZPROBE_OUT_EN[] PROGMEM = "Z probe out. bed";
const char * const MSG_ZPROBE_OUT_LANG_TABLE[1] PROGMEM = {
	MSG_ZPROBE_OUT_EN
};

const char MSG_ZPROBE_ZOFFSET_EN[] PROGMEM = "Z Offset";
const char * const MSG_ZPROBE_ZOFFSET_LANG_TABLE[1] PROGMEM = {
	MSG_ZPROBE_ZOFFSET_EN
};

const char MSG_Z_MAX_EN[] PROGMEM = "z_max: ";
const char * const MSG_Z_MAX_LANG_TABLE[1] PROGMEM = {
	MSG_Z_MAX_EN
};

const char MSG_Z_MIN_EN[] PROGMEM = "z_min: ";
const char * const MSG_Z_MIN_LANG_TABLE[1] PROGMEM = {
	MSG_Z_MIN_EN
};

const char WELCOME_MSG_EN[] PROGMEM = CUSTOM_MENDEL_NAME " ready.";
const char WELCOME_MSG_CZ[] PROGMEM = CUSTOM_MENDEL_NAME " ok";
const char * const WELCOME_MSG_LANG_TABLE[LANG_NUM] PROGMEM = {
	WELCOME_MSG_EN,
	WELCOME_MSG_CZ
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
