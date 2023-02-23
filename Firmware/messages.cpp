//messages.c
#include "language.h"

//this is because we need CUSTOM_MENDEL_NAME
#include "Configuration_var.h"

//internationalized messages
const char MSG_ALWAYS[] PROGMEM_I1 = ISTR("Always"); ////MSG_ALWAYS c=6
const char MSG_AUTO_HOME[] PROGMEM_I1 = ISTR("Auto home"); ////MSG_AUTO_HOME c=18
const char MSG_BABYSTEP_Z[] PROGMEM_I1 = ISTR("Live adjust Z"); ////MSG_BABYSTEP_Z c=18
const char MSG_BABYSTEP_Z_NOT_SET[] PROGMEM_I1 = ISTR("Distance between tip of the nozzle and the bed surface has not been set yet. Please follow the manual, chapter First steps, section First layer calibration."); ////MSG_BABYSTEP_Z_NOT_SET c=20 r=12
const char MSG_BED[] PROGMEM_I1 = ISTR("Bed"); ////MSG_BED c=13
const char MSG_BED_DONE[] PROGMEM_I1 = ISTR("Bed done"); ////MSG_BED_DONE c=20
const char MSG_BED_HEATING[] PROGMEM_I1 = ISTR("Bed Heating"); ////MSG_BED_HEATING c=20
const char MSG_BED_LEVELING_FAILED_POINT_LOW[] PROGMEM_I1 = ISTR("Bed leveling failed. Sensor didn't trigger. Debris on nozzle? Waiting for reset."); ////MSG_BED_LEVELING_FAILED_POINT_LOW c=20 r=6
const char MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED[] PROGMEM_I1 = ISTR("XYZ calibration failed. Please consult the manual."); ////MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED c=20 r=8
const char MSG_BELT_STATUS[] PROGMEM_I1 = ISTR("Belt status");////MSG_BELT_STATUS c=18
const char MSG_CANCEL[] PROGMEM_I1 = ISTR(">Cancel");////MSG_CANCEL c=10
const char MSG_CALIBRATE_Z_AUTO[] PROGMEM_I1 = ISTR("Calibrating Z"); ////MSG_CALIBRATE_Z_AUTO c=20 r=2
const char MSG_CARD_MENU[] PROGMEM_I1 = ISTR("Print from SD"); ////MSG_CARD_MENU c=18
const char MSG_CHECKING_X[] PROGMEM_I1 = ISTR("Checking X axis"); ////MSG_CHECKING_X c=20
const char MSG_CHECKING_Y[] PROGMEM_I1 = ISTR("Checking Y axis"); ////MSG_CHECKING_Y c=20
const char MSG_COMMUNITY_MADE[] PROGMEM_I1 = ISTR("Community made"); ////MSG_COMMUNITY_MADE c=18
const char MSG_CONFIRM_NOZZLE_CLEAN[] PROGMEM_I1 = ISTR("Please clean the nozzle for calibration. Click when done."); ////MSG_CONFIRM_NOZZLE_CLEAN c=20 r=8
const char MSG_COOLDOWN[] PROGMEM_I1 = ISTR("Cooldown"); ////MSG_COOLDOWN c=18
const char MSG_CRASH[] PROGMEM_I1 = ISTR("Crash"); ////MSG_CRASH c=7
const char MSG_CRASH_DETECTED[] PROGMEM_I1 = ISTR("Crash detected."); ////MSG_CRASH_DETECTED c=20
const char MSG_CRASHDETECT[] PROGMEM_I1 = ISTR("Crash det."); ////MSG_CRASHDETECT c=13
const char MSG_ERROR[] PROGMEM_I1 = ISTR("ERROR:"); ////MSG_ERROR c=10
const char MSG_EXTRUDER[] PROGMEM_I1 = ISTR("Extruder"); ////MSG_EXTRUDER c=17
const char MSG_FANS_CHECK[] PROGMEM_I1 = ISTR("Fans check"); ////MSG_FANS_CHECK c=13
const char MSG_FIL_RUNOUTS[] PROGMEM_I1 = ISTR("Fil. runouts"); ////MSG_FIL_RUNOUTS c=15
const char MSG_FILAMENT[] PROGMEM_I1 = ISTR("Filament"); ////MSG_FILAMENT c=17
const char MSG_FAN_SPEED[] PROGMEM_I1 = ISTR("Fan speed"); ////MSG_FAN_SPEED c=14
const char MSG_HOTEND_FAN_SPEED[] PROGMEM_I1 = ISTR("Hotend fan:");////MSG_HOTEND_FAN_SPEED c=15
const char MSG_PRINT_FAN_SPEED[] PROGMEM_I1 = ISTR("Print fan:"); ////MSG_PRINT_FAN_SPEED c=15
const char MSG_FILAMENT_CLEAN[] PROGMEM_I1 = ISTR("Filament extruding & with correct color?"); ////MSG_FILAMENT_CLEAN c=20 r=2
const char MSG_FILAMENT_LOADED[] PROGMEM_I1 = ISTR("Is filament loaded?"); ////MSG_FILAMENT_LOADED c=20 r=2
const char MSG_FILAMENTCHANGE[] PROGMEM_I1 = ISTR("Change filament"); ////MSG_FILAMENTCHANGE c=18
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1[] PROGMEM_I1 = ISTR("Searching bed calibration point"); ////MSG_FIND_BED_OFFSET_AND_SKEW_LINE1 c=20 r=3
const char MSG_FINISHING_MOVEMENTS[] PROGMEM_I1 = ISTR("Finishing movements"); ////MSG_FINISHING_MOVEMENTS c=20
const char MSG_FOLLOW_CALIBRATION_FLOW[] PROGMEM_I1 = ISTR("Printer has not been calibrated yet. Please follow the manual, chapter First steps, section Calibration flow."); ////MSG_FOLLOW_CALIBRATION_FLOW c=20 r=8
const char MSG_FOLLOW_Z_CALIBRATION_FLOW[] PROGMEM_I1 = ISTR("There is still a need to make Z calibration. Please follow the manual, chapter First steps, section Calibration flow."); ////MSG_FOLLOW_Z_CALIBRATION_FLOW c=20 r=9
const char MSG_FSENSOR_RUNOUT[] PROGMEM_I1 = ISTR("F. runout"); ////MSG_FSENSOR_RUNOUT c=13
const char MSG_FSENSOR_AUTOLOAD[] PROGMEM_I1 = ISTR("F. autoload"); ////MSG_FSENSOR_AUTOLOAD c=13
const char MSG_FSENSOR_JAM_DETECTION[] PROGMEM_I1 = ISTR("F. jam detect"); ////MSG_FSENSOR_JAM_DETECTION c=13
const char MSG_FSENSOR[] PROGMEM_I1 = ISTR("Fil. sensor"); ////MSG_FSENSOR c=12
const char MSG_HEATING[] PROGMEM_I1 = ISTR("Heating"); ////MSG_HEATING c=20
const char MSG_HEATING_COMPLETE[] PROGMEM_I1 = ISTR("Heating done."); ////MSG_HEATING_COMPLETE c=20
const char MSG_HOMEYZ[] PROGMEM_I1 = ISTR("Calibrate Z"); ////MSG_HOMEYZ c=18
const char MSG_ITERATION[] PROGMEM_I1 = ISTR("Iteration"); ////MSG_ITERATION c=12
const char MSG_SELECT_EXTRUDER[] PROGMEM_I1 = ISTR("Select extruder:"); ////MSG_SELECT_EXTRUDER c=20
const char MSG_SELECT_FILAMENT[] PROGMEM_I1 = ISTR("Select filament:"); ////MSG_SELECT_FILAMENT c=20
const char MSG_LAST_PRINT[] PROGMEM_I1 = ISTR("Last print"); ////MSG_LAST_PRINT c=18
const char MSG_LAST_PRINT_FAILURES[] PROGMEM_I1 = ISTR("Last print failures"); ////MSG_LAST_PRINT_FAILURES c=20
const char MSG_LOAD_FILAMENT[] PROGMEM_I1 = ISTR("Load filament"); ////MSG_LOAD_FILAMENT c=17
const char MSG_LOADING_TEST[] PROGMEM_I1 = ISTR("Loading Test"); ////MSG_LOADING_TEST c=18
const char MSG_LOADING_FILAMENT[] PROGMEM_I1 = ISTR("Loading filament"); ////MSG_LOADING_FILAMENT c=20
const char MSG_TESTING_FILAMENT[] PROGMEM_I1 = ISTR("Testing filament"); ////MSG_TESTING_FILAMENT c=20
const char MSG_EJECT_FROM_MMU[] PROGMEM_I1 = ISTR("Eject from MMU"); ////MSG_EJECT_FROM_MMU c=16
const char MSG_CUT_FILAMENT[] PROGMEM_I1 = ISTR("Cut filament"); ////MSG_CUT_FILAMENT c=17
const char MSG_MAIN[] PROGMEM_I1 = ISTR("Main"); ////MSG_MAIN c=18
const char MSG_BACK[] PROGMEM_I1 = ISTR("Back"); ////MSG_BACK c=18
const char MSG_SHEET[] PROGMEM_I1 = ISTR("Sheet"); ////MSG_SHEET c=10
const char MSG_STEEL_SHEETS[] PROGMEM_I1 = ISTR("Steel sheets"); ////MSG_STEEL_SHEETS c=18
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1[] PROGMEM_I1 = ISTR("Measuring reference height of calibration point"); ////MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1 c=20 r=3
const char MSG_CALIBRATION[] PROGMEM_I1 = ISTR("Calibration"); ////MSG_CALIBRATION c=18
const char MSG_MMU_FAILS[] PROGMEM_I1 = ISTR("MMU fails"); ////MSG_MMU_FAILS c=15
const char MSG_MMU_LOAD_FAILS[] PROGMEM_I1 = ISTR("MMU load fails"); ////MSG_MMU_LOAD_FAILS c=15
const char MSG_MMU_POWER_FAILS[] PROGMEM_I1 = ISTR("MMU power fails"); ////MSG_MMU_POWER_FAILS c=15
const char MSG_NO[] PROGMEM_I1 = ISTR("No"); ////MSG_NO c=4
const char MSG_NOZZLE[] PROGMEM_I1 = ISTR("Nozzle"); ////MSG_NOZZLE c=10
const char MSG_PAPER[] PROGMEM_I1 = ISTR("Place a sheet of paper under the nozzle during the calibration of first 4 points. If the nozzle catches the paper, power off the printer immediately."); ////MSG_PAPER c=20 r=10
const char MSG_PAUSE_PRINT[] PROGMEM_I1 = ISTR("Pause print");////MSG_PAUSE_PRINT c=18
const char MSG_PLACE_STEEL_SHEET[] PROGMEM_I1 = ISTR("Please place steel sheet on heatbed."); ////MSG_PLACE_STEEL_SHEET c=20 r=5
const char MSG_PLEASE_WAIT[] PROGMEM_I1 = ISTR("Please wait"); ////MSG_PLEASE_WAIT c=20
const char MSG_POWER_FAILURES[] PROGMEM_I1 = ISTR("Power failures"); ////MSG_POWER_FAILURES c=15
const char MSG_PREHEAT_NOZZLE[] PROGMEM_I1 = ISTR("Preheat the nozzle!"); ////MSG_PREHEAT_NOZZLE c=20
const char MSG_PRESS_TO_UNLOAD[] PROGMEM_I1 = ISTR("Please press the knob to unload filament"); ////MSG_PRESS_TO_UNLOAD c=20 r=4
const char MSG_PRINT_ABORTED[] PROGMEM_I1 = ISTR("Print aborted"); ////MSG_PRINT_ABORTED c=20
const char MSG_PULL_OUT_FILAMENT[] PROGMEM_I1 = ISTR("Please pull out filament immediately"); ////MSG_PULL_OUT_FILAMENT c=20 r=4
const char MSG_RECOVER_PRINT[] PROGMEM_I1 = ISTR("Blackout occurred. Recover print?"); ////MSG_RECOVER_PRINT c=20 r=2
const char MSG_REFRESH[] PROGMEM_I1 = ISTR("\x04Refresh"); ////MSG_REFRESH c=18
const char MSG_REMOVE_STEEL_SHEET[] PROGMEM_I1 = ISTR("Please remove steel sheet from heatbed."); ////MSG_REMOVE_STEEL_SHEET c=20 r=4
const char MSG_RESET[] PROGMEM_I1 = ISTR("Reset"); ////MSG_RESET c=14
const char MSG_RESUME_PRINT[] PROGMEM_I1 = ISTR("Resume print"); ////MSG_RESUME_PRINT c=18
const char MSG_RESUMING_PRINT[] PROGMEM_I1 = ISTR("Resuming print"); ////MSG_RESUMING_PRINT c=20
const char MSG_SELFTEST_PART_FAN[] PROGMEM_I1 = ISTR("Front print fan?"); ////MSG_SELFTEST_PART_FAN c=20
const char MSG_SELFTEST_HOTEND_FAN[] PROGMEM_I1 = ISTR("Left hotend fan?"); ////MSG_SELFTEST_HOTEND_FAN c=20
const char MSG_SELFTEST_FAILED[] PROGMEM_I1 = ISTR("Selftest failed"); ////MSG_SELFTEST_FAILED c=20
const char MSG_SELFTEST_FAN[] PROGMEM_I1 = ISTR("Fan test"); ////MSG_SELFTEST_FAN c=20
const char MSG_SELFTEST_FAN_NO[] PROGMEM_I1 = ISTR("Not spinning"); ////MSG_SELFTEST_FAN_NO c=19
const char MSG_SELFTEST_FAN_YES[] PROGMEM_I1 = ISTR("Spinning"); ////MSG_SELFTEST_FAN_YES c=19
const char MSG_SELFTEST_CHECK_BED[] PROGMEM_I1 = ISTR("Checking bed"); ////MSG_SELFTEST_CHECK_BED c=20
const char MSG_SELFTEST_CHECK_FSENSOR[] PROGMEM_I1 = ISTR("Checking sensors"); ////MSG_SELFTEST_CHECK_FSENSOR c=20
const char MSG_SELFTEST_MOTOR[] PROGMEM_I1 = ISTR("Motor"); ////MSG_SELFTEST_MOTOR c=18
const char MSG_SELFTEST_FILAMENT_SENSOR[] PROGMEM_I1 = ISTR("Filament sensor"); ////MSG_SELFTEST_FILAMENT_SENSOR c=17
const char MSG_SELFTEST_WIRINGERROR[] PROGMEM_I1 = ISTR("Wiring error"); ////MSG_SELFTEST_WIRINGERROR c=18
const char MSG_SETTINGS[] PROGMEM_I1 = ISTR("Settings"); ////MSG_SETTINGS c=18
const char MSG_SELECT_LANGUAGE[] PROGMEM_I1 = ISTR("Select language"); ////MSG_SELECT_LANGUAGE c=18
const char MSG_SORTING_FILES[] PROGMEM_I1 = ISTR("Sorting files"); ////MSG_SORTING_FILES c=20
const char MSG_TOTAL[] PROGMEM_I1 = ISTR("Total"); ////MSG_TOTAL c=6
const char MSG_MATERIAL_CHANGES[] PROGMEM_I1 = ISTR("Material changes"); ////MSG_MATERIAL_CHANGES c=18
const char MSG_TOTAL_FAILURES[] PROGMEM_I1 = ISTR("Total failures"); ////MSG_TOTAL_FAILURES c=20
const char MSG_HW_SETUP[] PROGMEM_I1 = ISTR("HW Setup"); ////MSG_HW_SETUP c=18
const char MSG_MODE[] PROGMEM_I1 = ISTR("Mode"); ////MSG_MODE c=6
const char MSG_HIGH_POWER[] PROGMEM_I1 = ISTR("High power"); ////MSG_HIGH_POWER c=10
const char MSG_AUTO_POWER[] PROGMEM_I1 = ISTR("Auto power"); ////MSG_AUTO_POWER c=10
const char MSG_SILENT[] PROGMEM_I1 = ISTR("Silent"); ////MSG_SILENT c=7
const char MSG_NORMAL[] PROGMEM_I1 = ISTR("Normal"); ////MSG_NORMAL c=7
const char MSG_STEALTH[] PROGMEM_I1 = ISTR("Stealth"); ////MSG_STEALTH c=7
const char MSG_STEEL_SHEET_CHECK[] PROGMEM_I1 = ISTR("Is steel sheet on heatbed?"); ////MSG_STEEL_SHEET_CHECK c=20 r=2
const char MSG_STOP_PRINT[] PROGMEM_I1 = ISTR("Stop print"); ////MSG_STOP_PRINT c=18
const char MSG_STOPPED[] PROGMEM_I1 = ISTR("STOPPED."); ////MSG_STOPPED c=20
const char MSG_PINDA_CALIBRATION[] PROGMEM_I1 = ISTR("PINDA cal."); ////MSG_PINDA_CALIBRATION c=13
const char MSG_PINDA_CALIBRATION_DONE[] PROGMEM_I1 = ISTR("PINDA calibration is finished and active. It can be disabled in menu Settings->PINDA cal."); ////MSG_PINDA_CALIBRATION_DONE c=20 r=8
const char MSG_UNLOAD_FILAMENT[] PROGMEM_I1 = ISTR("Unload filament"); ////MSG_UNLOAD_FILAMENT c=16
const char MSG_UNLOADING_FILAMENT[] PROGMEM_I1 = ISTR("Unloading filament"); ////MSG_UNLOADING_FILAMENT c=20
const char MSG_INFO_SCREEN[] PROGMEM_I1 = ISTR("Info screen"); ////MSG_INFO_SCREEN c=18
const char MSG_WIZARD_CALIBRATION_FAILED[] PROGMEM_I1 = ISTR("Please check our handbook and fix the problem. Then resume the Wizard by rebooting the printer."); ////MSG_WIZARD_CALIBRATION_FAILED c=20 r=8
const char MSG_WIZARD_DONE[] PROGMEM_I1 = ISTR("All is done. Happy printing!"); ////MSG_WIZARD_DONE c=20 r=3
const char MSG_WIZARD_HEATING[] PROGMEM_I1 = ISTR("Preheating nozzle. Please wait."); ////MSG_WIZARD_HEATING c=20 r=3
const char MSG_WIZARD_QUIT[] PROGMEM_I1 = ISTR("You can always resume the Wizard from Calibration -> Wizard."); ////MSG_WIZARD_QUIT c=20 r=8
const char MSG_WIZARD_WELCOME[] PROGMEM_I1 = ISTR("Hi, I am your Original Prusa i3 printer. Would you like me to guide you through the setup process?"); ////MSG_WIZARD_WELCOME c=20 r=7
const char MSG_WIZARD_WELCOME_SHIPPING[] PROGMEM_I1 = ISTR("Hi, I am your Original Prusa i3 printer. I will guide you through a short setup process, in which the Z-axis will be calibrated. Then, you will be ready to print."); ////MSG_WIZARD_WELCOME_SHIPPING c=20 r=12
const char MSG_YES[] PROGMEM_I1 = ISTR("Yes"); ////MSG_YES c=4
const char MSG_V2_CALIBRATION[] PROGMEM_I1 = ISTR("First layer cal."); ////MSG_V2_CALIBRATION c=18
const char MSG_OFF[] PROGMEM_I1 = ISTR("Off"); ////MSG_OFF c=3
const char MSG_ON[] PROGMEM_I1 = ISTR("On"); ////MSG_ON c=3
const char MSG_NA[] PROGMEM_I1 = ISTR("N/A"); ////MSG_NA c=3
const char MSG_CUTTER[] PROGMEM_I1 = ISTR("Cutter"); ////MSG_CUTTER c=9
const char MSG_NONE[] PROGMEM_I1 = ISTR("None"); ////MSG_NONE c=8
const char MSG_WARN[] PROGMEM_I1 = ISTR("Warn"); ////MSG_WARN c=8
const char MSG_STRICT[] PROGMEM_I1 = ISTR("Strict"); ////MSG_STRICT c=8
const char MSG_MODEL[] PROGMEM_I1 = ISTR("Model"); ////MSG_MODEL c=8
const char MSG_GCODE_DIFF_PRINTER_CONTINUE[] PROGMEM_I1 = ISTR("G-code sliced for a different printer type. Continue?"); ////MSG_GCODE_DIFF_PRINTER_CONTINUE c=20 r=3
const char MSG_GCODE_DIFF_PRINTER_CANCELLED[] PROGMEM_I1 =ISTR("G-code sliced for a different printer type. Please re-slice the model again. Print cancelled."); ////MSG_GCODE_DIFF_PRINTER_CANCELLED c=20 r=8
const char MSG_GCODE_NEWER_FIRMWARE_CONTINUE[] PROGMEM_I1 = ISTR("G-code sliced for a newer firmware. Continue?"); ////MSG_GCODE_NEWER_FIRMWARE_CONTINUE c=20 r=3
const char MSG_GCODE_NEWER_FIRMWARE_CANCELLED[] PROGMEM_I1 = ISTR("G-code sliced for a newer firmware. Please update the firmware. Print cancelled."); ////MSG_GCODE_NEWER_FIRMWARE_CANCELLED c=20 r=8
const char MSG_GCODE_DIFF_CONTINUE[] PROGMEM_I1 = ISTR("G-code sliced for a different level. Continue?"); ////MSG_GCODE_DIFF_CONTINUE c=20 r=3
const char MSG_GCODE_DIFF_CANCELLED[] PROGMEM_I1 = ISTR("G-code sliced for a different level. Please re-slice the model again. Print cancelled."); ////MSG_GCODE_DIFF_CANCELLED c=20 r=8
const char MSG_NOZZLE_DIFFERS_CONTINUE[] PROGMEM_I1 = ISTR("Nozzle diameter differs from the G-code. Continue?"); ////MSG_NOZZLE_DIFFERS_CONTINUE c=20 r=3
const char MSG_NOZZLE_DIFFERS_CANCELLED[] PROGMEM_I1 = ISTR("Nozzle diameter differs from the G-code. Please check the value in settings. Print cancelled."); ////MSG_NOZZLE_DIFFERS_CANCELLED c=20 r=9
const char MSG_NOZZLE_DIAMETER[] PROGMEM_I1 = ISTR("Nozzle d."); ////MSG_NOZZLE_DIAMETER c=10
const char MSG_MMU_MODE[] PROGMEM_I1 = ISTR("MMU Mode"); ////MSG_MMU_MODE c=8
const char MSG_SD_CARD[] PROGMEM_I1 = ISTR("SD card"); ////MSG_SD_CARD c=8
const char MSG_SORT[] PROGMEM_I1 = ISTR("Sort"); ////MSG_SORT c=7
const char MSG_SORT_TIME[] PROGMEM_I1 = ISTR("Time"); ////MSG_SORT_TIME c=8
const char MSG_SORT_ALPHA[] PROGMEM_I1 = ISTR("Alphabet"); ////MSG_SORT_ALPHA c=8
const char MSG_RPI_PORT[] PROGMEM_I1 = ISTR("RPi port"); ////MSG_RPI_PORT c=13
const char MSG_SOUND[] PROGMEM_I1 = ISTR("Sound"); ////MSG_SOUND c=9
const char MSG_SOUND_LOUD[] PROGMEM_I1 = ISTR("Loud"); ////MSG_SOUND_LOUD c=7
const char MSG_SOUND_ONCE[] PROGMEM_I1 = ISTR("Once"); ////MSG_SOUND_ONCE c=7
const char MSG_SOUND_BLIND[] PROGMEM_I1 = ISTR("Assist"); ////MSG_SOUND_BLIND c=7
const char MSG_MESH[] PROGMEM_I1 = ISTR("Mesh"); ////MSG_MESH c=12
const char MSG_MESH_BED_LEVELING[] PROGMEM_I1 = ISTR("Mesh Bed Leveling"); ////MSG_MESH_BED_LEVELING c=18
const char MSG_Z_PROBE_NR[] PROGMEM_I1 = ISTR("Z-probe nr."); ////MSG_Z_PROBE_NR c=14
const char MSG_MAGNETS_COMP[] PROGMEM_I1 = ISTR("Magnets comp."); ////MSG_MAGNETS_COMP c=13
const char MSG_FS_ACTION[] PROGMEM_I1 = ISTR("FS Action"); ////MSG_FS_ACTION c=10
const char MSG_CONTINUE_SHORT[] PROGMEM_I1 = ISTR("Cont."); ////MSG_CONTINUE_SHORT c=5
const char MSG_PAUSE[] PROGMEM_I1 = ISTR("Pause"); ////MSG_PAUSE c=5
const char MSG_BRIGHTNESS[] PROGMEM_I1 = ISTR("Brightness"); ////MSG_BRIGHTNESS c=18
const char MSG_BL_HIGH[] PROGMEM_I1 = ISTR("Level Bright"); ////MSG_BL_HIGH c=12
const char MSG_BL_LOW[] PROGMEM_I1 = ISTR("Level Dimmed"); ////MSG_BL_LOW c=12
const char MSG_TIMEOUT[] PROGMEM_I1 = ISTR("Timeout"); ////MSG_TIMEOUT c=12
const char MSG_BRIGHT[] PROGMEM_I1 = ISTR("Bright"); ////MSG_BRIGHT c=6
const char MSG_DIM[] PROGMEM_I1 = ISTR("Dim"); ////MSG_DIM c=6
const char MSG_AUTO[] PROGMEM_I1 = ISTR("Auto"); ////MSG_AUTO c=6
#if (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
// Beware - the space at the beginning is necessary since it is reused in LCD menu items which are to be with a space
const char MSG_IR_04_OR_NEWER[] PROGMEM_I1 = ISTR(" 0.4 or newer");////MSG_IR_04_OR_NEWER c=18
const char MSG_IR_03_OR_OLDER[] PROGMEM_I1 = ISTR(" 0.3 or older");////MSG_IR_03_OR_OLDER c=18
const char MSG_IR_UNKNOWN[] PROGMEM_I1 = ISTR("unknown state");////MSG_IR_UNKNOWN c=18
#endif
extern const char MSG_PAUSED_THERMAL_ERROR[] PROGMEM_I1 = ISTR("PAUSED THERMAL ERROR");////MSG_PAUSED_THERMAL_ERROR c=20
#ifdef TEMP_MODEL
extern const char MSG_THERMAL_ANOMALY[] PROGMEM_I1 = ISTR("THERMAL ANOMALY");////MSG_THERMAL_ANOMALY c=20
extern const char MSG_TM_NOT_CAL[] PROGMEM_I1 = ISTR("Thermal model not calibrated yet.");////MSG_TM_NOT_CAL c=20 r=4
extern const char MSG_TM_ACK_ERROR[] PROGMEM_I1 = ISTR("Clear TM error");////MSG_TM_ACK_ERROR c=18
#endif
extern const char MSG_LOAD_ALL[] PROGMEM_I1 = ISTR("Load All"); ////MSG_LOAD_ALL c=18
extern const char MSG_NOZZLE_CNG_MENU [] PROGMEM_I1 = ISTR("Nozzle change");////MSG_NOZZLE_CNG_MENU c=18
extern const char MSG_NOZZLE_CNG_READ_HELP [] PROGMEM_I1 = ISTR("For a Nozzle change please read\nprusa.io/nozzle-mk3s");////MSG_NOZZLE_CNG_READ_HELP c=20 r=4
extern const char MSG_NOZZLE_CNG_CHANGED [] PROGMEM_I1 = ISTR("Hotend at 280C! Nozzle changed and tightened to specs?");////MSG_NOZZLE_CNG_CHANGED c=20 r=6

//not internationalized messages
#if 0
const char MSG_FW_VERSION_BETA[] PROGMEM_N1 = "You are using a BETA firmware version! It is in a development state! Use this version with CAUTION as it may DAMAGE the printer!"; ////MSG_FW_VERSION_BETA c=20 r=8
#endif
const char MSG_SPOOL_JOIN[] PROGMEM_N1 = "SpoolJoin"; ////MSG_SPOOL_JOIN c=13
const char MSG_FIRMWARE[] PROGMEM_N1 = "Firmware"; ////MSG_FIRMWARE c=8
const char MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY[] PROGMEM_N1 = "FlashAir"; ////MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY c=8
const char MSG_PINDA[] PROGMEM_N1 = "PINDA"; ////MSG_PINDA c=5
const char MSG_WELCOME[] PROGMEM_N1 = WELCOME_MSG;
const char MSG_SD_WORKDIR_FAIL[] PROGMEM_N1 = "workDir open failed"; ////
const char MSG_BROWNOUT_RESET[] PROGMEM_N1 = " Brown out Reset"; ////
const char MSG_EXTERNAL_RESET[] PROGMEM_N1 = " External Reset"; ////
const char MSG_FILE_SAVED[] PROGMEM_N1 = "Done saving file."; ////
const char MSG_POSITION_UNKNOWN[] PROGMEM_N1 = "Home X/Y before Z"; ////
const char MSG_SOFTWARE_RESET[] PROGMEM_N1 = " Software Reset"; ////
const char MSG_UNKNOWN_COMMAND[] PROGMEM_N1 = "Unknown command: \""; ////
const char MSG_WATCHDOG_RESET[] PROGMEM_N1 = " Watchdog Reset"; ////
const char MSG_Z_MAX[] PROGMEM_N1 = "z_max: "; ////
const char MSG_Z_MIN[] PROGMEM_N1 = "z_min: "; ////
const char MSG_ZPROBE_OUT[] PROGMEM_N1 = "Z probe out. bed"; ////
#ifdef ENABLE_AUTO_BED_LEVELING
const char MSG_ZPROBE_ZOFFSET[] PROGMEM_N1 = "Z Offset"; ////
#endif
const char MSG_TMC_OVERTEMP[] PROGMEM_N1 = "TMC DRIVER OVERTEMP"; ////
const char MSG_Enqueing[] PROGMEM_N1 = "enqueing \""; ////
const char MSG_ENDSTOPS_HIT[] PROGMEM_N1 = "endstops hit: "; ////
const char MSG_SD_ERR_WRITE_TO_FILE[] PROGMEM_N1 = "error writing to file"; ////
const char MSG_OK[] PROGMEM_N1 = "ok"; ////
const char MSG_OK_CAPS[] PROGMEM_N1 = "OK"; ////
const char MSG_SD_OPEN_FILE_FAIL[] PROGMEM_N1 = "open failed, File: "; ////
const char MSG_ENDSTOP_OPEN[] PROGMEM_N1 = "open"; ////
const char MSG_POWERUP[] PROGMEM_N1 = "PowerUp"; ////
const char MSG_ERR_STOPPED[] PROGMEM_N1 = "Printer stopped due to errors. Supervision required."; ////
const char MSG_ENDSTOP_HIT[] PROGMEM_N1 = "TRIGGERED"; ////
const char MSG_OCTOPRINT_ASK_PAUSE[] PROGMEM_N1 = "// action:pause"; ////
const char MSG_OCTOPRINT_PAUSED[] PROGMEM_N1 = "// action:paused"; ////
const char MSG_OCTOPRINT_ASK_RESUME[] PROGMEM_N1 = "// action:resume"; ////
const char MSG_OCTOPRINT_RESUMED[] PROGMEM_N1 = "// action:resumed"; ////
const char MSG_OCTOPRINT_CANCEL[] PROGMEM_N1 = "// action:cancel"; ////
const char MSG_FANCHECK_HOTEND[] PROGMEM_N1 = "Err:HOTEND FAN ERROR"; ////c=20
const char MSG_FANCHECK_PRINT[] PROGMEM_N1 = "Err:PRINT FAN ERROR"; ////c=20
const char MSG_M112_KILL[] PROGMEM_N1 = "M112 called. Emergency Stop."; ////c=20
const char MSG_ADVANCE_K[] PROGMEM_N1 = "Advance K:"; ////c=13
const char MSG_POWERPANIC_DETECTED[] PROGMEM_N1 = "POWER PANIC DETECTED"; ////c=20
const char MSG_LCD_STATUS_CHANGED[] PROGMEM_N1 = "LCD status changed";
const char MSG_UNKNOWN_CODE[] PROGMEM_N1 = "Unknown %c code: %s\n";
