//messages.c
#include "language.h"

//this is because we need include Configuration_prusa.h (CUSTOM_MENDEL_NAME)
#define bool char
#define true 1
#define false 0
#include "Configuration_prusa.h"

//internationalized messages
const char MSG_AUTO_HOME[] PROGMEM_I1 = ISTR("Auto home"); ////
const char MSG_BABYSTEP_Z[] PROGMEM_I1 = ISTR("Live adjust Z"); //// c=18
const char MSG_BABYSTEP_Z_NOT_SET[] PROGMEM_I1 = ISTR("Distance between tip of the nozzle and the bed surface has not been set yet. Please follow the manual, chapter First steps, section First layer calibration."); ////c=20 r=12
const char MSG_BED[] PROGMEM_I1 = ISTR("Bed"); ////
const char MSG_BED_DONE[] PROGMEM_I1 = ISTR("Bed done"); ////
const char MSG_BED_HEATING[] PROGMEM_I1 = ISTR("Bed Heating"); ////
const char MSG_BED_LEVELING_FAILED_POINT_LOW[] PROGMEM_I1 = ISTR("Bed leveling failed. Sensor didnt trigger. Debris on nozzle? Waiting for reset."); ////c=20 r=4
const char MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED[] PROGMEM_I1 = ISTR("XYZ calibration failed. Please consult the manual."); ////c=20 r=8
const char MSG_CALIBRATE_Z_AUTO[] PROGMEM_I1 = ISTR("Calibrating Z"); ////c=20 r=2
const char MSG_CARD_MENU[] PROGMEM_I1 = ISTR("Print from SD"); ////
const char MSG_CONFIRM_NOZZLE_CLEAN[] PROGMEM_I1 = ISTR("Please clean the nozzle for calibration. Click when done."); ////c=20 r=8
const char MSG_COOLDOWN[] PROGMEM_I1 = ISTR("Cooldown"); ////
const char MSG_CRASH_DETECTED[] PROGMEM_I1 = ISTR("Crash detected."); ////c=20 r=1
const char MSG_CRASHDETECT[] PROGMEM_I1 = ISTR("Crash det."); ////
const char MSG_ERROR[] PROGMEM_I1 = ISTR("ERROR:"); ////
const char MSG_EXTRUDER[] PROGMEM_I1 = ISTR("Extruder"); ////c=17 r=1
const char MSG_FILAMENT[] PROGMEM_I1 = ISTR("Filament"); ////c=17 r=1
const char MSG_FAN_SPEED[] PROGMEM_I1 = ISTR("Fan speed"); ////c=14
const char MSG_FILAMENT_CLEAN[] PROGMEM_I1 = ISTR("Filament extruding & with correct color?"); ////c=20 r=2
const char MSG_FILAMENT_LOADING_T0[] PROGMEM_I1 = ISTR("Insert filament into extruder 1. Click when done."); ////c=20 r=4
const char MSG_FILAMENT_LOADING_T1[] PROGMEM_I1 = ISTR("Insert filament into extruder 2. Click when done."); ////c=20 r=4
const char MSG_FILAMENT_LOADING_T2[] PROGMEM_I1 = ISTR("Insert filament into extruder 3. Click when done."); ////c=20 r=4
const char MSG_FILAMENT_LOADING_T3[] PROGMEM_I1 = ISTR("Insert filament into extruder 4. Click when done."); ////c=20 r=4
const char MSG_FILAMENTCHANGE[] PROGMEM_I1 = ISTR("Change filament"); ////
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1[] PROGMEM_I1 = ISTR("Searching bed calibration point"); ////c=60
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2[] PROGMEM_I1 = ISTR(" of 4"); ////c=14
const char MSG_FINISHING_MOVEMENTS[] PROGMEM_I1 = ISTR("Finishing movements"); ////c=20 r=1
const char MSG_FOLLOW_CALIBRATION_FLOW[] PROGMEM_I1 = ISTR("Printer has not been calibrated yet. Please follow the manual, chapter First steps, section Calibration flow."); ////c=20 r=8
const char MSG_FOLLOW_Z_CALIBRATION_FLOW[] PROGMEM_I1 = ISTR("There is still a need to make Z calibration. Please follow the manual, chapter First steps, section Calibration flow."); ////c=20 r=8
const char MSG_FSENSOR_AUTOLOAD[] PROGMEM_I1 = ISTR("F. autoload"); ////c=17 r=1
const char MSG_FSENSOR[] PROGMEM_I1 = ISTR("Fil. sensor"); ////
const char MSG_HEATING[] PROGMEM_I1 = ISTR("Heating"); ////
const char MSG_HEATING_COMPLETE[] PROGMEM_I1 = ISTR("Heating done."); ////c=20
const char MSG_HOMEYZ[] PROGMEM_I1 = ISTR("Calibrate Z"); ////
const char MSG_CHOOSE_EXTRUDER[] PROGMEM_I1 = ISTR("Choose extruder:"); ////c=20 r=1
const char MSG_CHOOSE_FILAMENT[] PROGMEM_I1 = ISTR("Choose filament:"); ////c=20 r=1
const char MSG_LOAD_FILAMENT[] PROGMEM_I1 = ISTR("Load filament"); //// Number 1 to 5 is added behind text e.g. "Load filament 1" c=16
const char MSG_LOADING_FILAMENT[] PROGMEM_I1 = ISTR("Loading filament"); ////c=20
const char MSG_EJECT_FILAMENT[] PROGMEM_I1 = ISTR("Eject filament"); //// Number 1 to 5 is added behind text e.g. "Eject filament 1" c=16
const char MSG_CUT_FILAMENT[] PROGMEM_I1 = ISTR("Cut filament"); //// Number 1 to 5 is added behind text e.g. "Cut filament 1" c=16
const char MSG_M117_V2_CALIBRATION[] PROGMEM_I1 = ISTR("M117 First layer cal."); ////c=25 r=1
const char MSG_MAIN[] PROGMEM_I1 = ISTR("Main"); ////
const char MSG_BACK[] PROGMEM_I1 = ISTR("Back"); ////
const char MSG_SHEET[] PROGMEM_I1 = ISTR("Sheet"); ////c=10
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1[] PROGMEM_I1 = ISTR("Measuring reference height of calibration point"); ////c=60
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2[] PROGMEM_I1 = ISTR(" of 9"); ////c=14
const char MSG_MENU_CALIBRATION[] PROGMEM_I1 = ISTR("Calibration"); ////
const char MSG_NO[] PROGMEM_I1 = ISTR("No"); ////
const char MSG_NOZZLE[] PROGMEM_I1 = ISTR("Nozzle"); ////
const char MSG_PAPER[] PROGMEM_I1 = ISTR("Place a sheet of paper under the nozzle during the calibration of first 4 points. If the nozzle catches the paper, power off the printer immediately."); ////c=20 r=8
const char MSG_PLACE_STEEL_SHEET[] PROGMEM_I1 = ISTR("Please place steel sheet on heatbed."); ////c=20 r=4
const char MSG_PLEASE_WAIT[] PROGMEM_I1 = ISTR("Please wait"); ////c=20
const char MSG_PREHEAT_NOZZLE[] PROGMEM_I1 = ISTR("Preheat the nozzle!"); ////c=20
const char MSG_PRESS_TO_UNLOAD[] PROGMEM_I1 = ISTR("Please press the knob to unload filament"); ////c=20 r=4
const char MSG_PRINT_ABORTED[] PROGMEM_I1 = ISTR("Print aborted"); ////c=20
const char MSG_PULL_OUT_FILAMENT[] PROGMEM_I1 = ISTR("Please pull out filament immediately"); ////c=20 r=4
const char MSG_RECOVER_PRINT[] PROGMEM_I1 = ISTR("Blackout occurred. Recover print?"); ////c=20 r=2
const char MSG_REFRESH[] PROGMEM_I1 = ISTR("\xF8" "Refresh"); ////
const char MSG_RESUMING_PRINT[] PROGMEM_I1 = ISTR("Resuming print"); ////
const char MSG_REMOVE_STEEL_SHEET[] PROGMEM_I1 = ISTR("Please remove steel sheet from heatbed."); ////c=20 r=4
const char MSG_SELFTEST_COOLING_FAN[] PROGMEM_I1 = ISTR("Front print fan?"); ////c=20
const char MSG_SELFTEST_EXTRUDER_FAN[] PROGMEM_I1 = ISTR("Left hotend fan?"); ////c=20
const char MSG_SELFTEST_FAILED[] PROGMEM_I1 = ISTR("Selftest failed  "); ////c=20
const char MSG_SELFTEST_FAN[] PROGMEM_I1 = ISTR("Fan test"); ////c=20
const char MSG_SELFTEST_FAN_NO[] PROGMEM_I1 = ISTR("Not spinning"); ////c=19
const char MSG_SELFTEST_FAN_YES[] PROGMEM_I1 = ISTR("Spinning"); ////c=19
const char MSG_SELFTEST_CHECK_BED[] PROGMEM_I1 = ISTR("Checking bed     "); ////c=20
const char MSG_SELFTEST_CHECK_FSENSOR[] PROGMEM_I1 = ISTR("Checking sensors "); ////c=20
const char MSG_SELFTEST_MOTOR[] PROGMEM_I1 = ISTR("Motor"); ////
const char MSG_SELFTEST_FILAMENT_SENSOR[] PROGMEM_I1 = ISTR("Filament sensor"); ////c=17
const char MSG_SELFTEST_WIRINGERROR[] PROGMEM_I1 = ISTR("Wiring error"); ////
const char MSG_SETTINGS[] PROGMEM_I1 = ISTR("Settings"); ////
const char MSG_HW_SETUP[] PROGMEM_I1 = ISTR("HW Setup"); ////
const char MSG_MODE[] PROGMEM_I1 = ISTR("Mode"); ////
const char MSG_HIGH_POWER[] PROGMEM_I1 = ISTR("High power"); ////
const char MSG_AUTO_POWER[] PROGMEM_I1 = ISTR("Auto power"); ////
const char MSG_SILENT[] PROGMEM_I1 = ISTR("Silent"); ////
const char MSG_NORMAL[] PROGMEM_I1 = ISTR("Normal"); ////
const char MSG_STEALTH[] PROGMEM_I1 = ISTR("Stealth"); ////
const char MSG_STEEL_SHEET_CHECK[] PROGMEM_I1 = ISTR("Is steel sheet on heatbed?"); ////c=20 r=2
const char MSG_STOP_PRINT[] PROGMEM_I1 = ISTR("Stop print"); ////
const char MSG_STOPPED[] PROGMEM_I1 = ISTR("STOPPED. "); ////
const char MSG_TEMP_CALIBRATION[] PROGMEM_I1 = ISTR("Temp. cal."); ////c=12 r=1
const char MSG_TEMP_CALIBRATION_DONE[] PROGMEM_I1 = ISTR("Temperature calibration is finished and active. Temp. calibration can be disabled in menu Settings->Temp. cal."); ////c=20 r=12
const char MSG_UNLOAD_FILAMENT[] PROGMEM_I1 = ISTR("Unload filament"); ////c=17
const char MSG_UNLOADING_FILAMENT[] PROGMEM_I1 = ISTR("Unloading filament"); ////c=20 r=1
const char MSG_WATCH[] PROGMEM_I1 = ISTR("Info screen"); ////
const char MSG_WIZARD_CALIBRATION_FAILED[] PROGMEM_I1 = ISTR("Please check our handbook and fix the problem. Then resume the Wizard by rebooting the printer."); ////c=20 r=8
const char MSG_WIZARD_DONE[] PROGMEM_I1 = ISTR("All is done. Happy printing!"); ////c=20 r=8
const char MSG_WIZARD_HEATING[] PROGMEM_I1 = ISTR("Preheating nozzle. Please wait."); ////c=20 r=3
const char MSG_WIZARD_QUIT[] PROGMEM_I1 = ISTR("You can always resume the Wizard from Calibration -> Wizard."); ////c=20 r=8
const char MSG_YES[] PROGMEM_I1 = ISTR("Yes"); ////
const char MSG_V2_CALIBRATION[] PROGMEM_I1 = ISTR("First layer cal."); ////c=17 r=1
const char WELCOME_MSG[] PROGMEM_I1 = ISTR(CUSTOM_MENDEL_NAME " OK."); ////c=20
const char MSG_OFF[] PROGMEM_I1 = ISTR("Off"); ////
const char MSG_ON[] PROGMEM_I1 = ISTR("On"); ////
const char MSG_NA[] PROGMEM_I1 = ISTR("N/A"); ////
const char MSG_AUTO_DEPLETE[] PROGMEM_I1 = ISTR("SpoolJoin"); ////
const char MSG_CUTTER[] PROGMEM_I1 = ISTR("Cutter"); ////
const char MSG_NONE[] PROGMEM_I1 = ISTR("None"); ////
const char MSG_WARN[] PROGMEM_I1 = ISTR("Warn"); ////
const char MSG_STRICT[] PROGMEM_I1 = ISTR("Strict"); ////
const char MSG_MODEL[] PROGMEM_I1 = ISTR("Model"); ////
const char MSG_FIRMWARE[] PROGMEM_I1 = ISTR("Firmware"); ////
const char MSG_GCODE[] PROGMEM_I1 = ISTR("Gcode"); ////
const char MSG_NOZZLE_DIAMETER[] PROGMEM_I1 = ISTR("Nozzle d."); ////
const char MSG_MMU_MODE[] PROGMEM_I1 = ISTR("MMU Mode"); ////
const char MSG_SD_CARD[] PROGMEM_I1 = ISTR("SD card"); ////
const char MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY[] PROGMEM_I1 = ISTR("FlashAir"); ////
const char MSG_SORT[] PROGMEM_I1 = ISTR("Sort"); ////
const char MSG_SORT_TIME[] PROGMEM_I1 = ISTR("Time"); ////
const char MSG_SORT_ALPHA[] PROGMEM_I1 = ISTR("Alphabet"); ////
const char MSG_RPI_PORT[] PROGMEM_I1 = ISTR("RPi port"); ////
const char MSG_SOUND[] PROGMEM_I1 = ISTR("Sound"); ////
const char MSG_SOUND_LOUD[] PROGMEM_I1 = ISTR("Loud"); ////
const char MSG_SOUND_ONCE[] PROGMEM_I1 = ISTR("Once"); ////
const char MSG_SOUND_BLIND[] PROGMEM_I1 = ISTR("Assist"); ////
const char MSG_MESH[] PROGMEM_I1 = ISTR("Mesh"); ////
const char MSG_Z_PROBE_NR[] PROGMEM_I1 = ISTR("Z-probe nr."); ////
const char MSG_MAGNETS_COMP[] PROGMEM_I1 = ISTR("Magnets comp."); ////
const char MSG_FS_ACTION[] PROGMEM_I1 = ISTR("FS Action"); ////
const char MSG_FS_CONTINUE[] PROGMEM_I1 = ISTR("Cont."); ////
const char MSG_FS_PAUSE[] PROGMEM_I1 = ISTR("Pause"); ////
const char MSG_BRIGHTNESS[] PROGMEM_I1 = ISTR("Brightness"); ////
const char MSG_BL_HIGH[] PROGMEM_I1 = ISTR("Level Bright"); ////
const char MSG_BL_LOW[] PROGMEM_I1 = ISTR("Level Dimmed"); ////
const char MSG_TIMEOUT[] PROGMEM_I1 = ISTR("Timeout"); ////
const char MSG_BRIGHT[] PROGMEM_I1 = ISTR("Bright"); ////
const char MSG_DIM[] PROGMEM_I1 = ISTR("Dim"); ////
const char MSG_AUTO[] PROGMEM_I1 = ISTR("Auto"); ////

//not internationalized messages
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
const char MSG_ZPROBE_ZOFFSET[] PROGMEM_N1 = "Z Offset"; ////
const char MSG_TMC_OVERTEMP[] PROGMEM_N1 = "TMC DRIVER OVERTEMP"; ////
const char MSG_Enqueing[] PROGMEM_N1 = "enqueing \""; ////
const char MSG_ENDSTOPS_HIT[] PROGMEM_N1 = "endstops hit: "; ////
const char MSG_SD_ERR_WRITE_TO_FILE[] PROGMEM_N1 = "error writing to file"; ////
const char MSG_OK[] PROGMEM_N1 = "ok"; ////
const char MSG_SD_OPEN_FILE_FAIL[] PROGMEM_N1 = "open failed, File: "; ////
const char MSG_ENDSTOP_OPEN[] PROGMEM_N1 = "open"; ////
const char MSG_POWERUP[] PROGMEM_N1 = "PowerUp"; ////
const char MSG_ERR_STOPPED[] PROGMEM_N1 = "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)"; ////
const char MSG_ENDSTOP_HIT[] PROGMEM_N1 = "TRIGGERED"; ////
const char MSG_OCTOPRINT_PAUSED[] PROGMEM_N1 = "// action:paused"; ////
const char MSG_OCTOPRINT_RESUMED[] PROGMEM_N1 = "// action:resumed"; ////
const char MSG_OCTOPRINT_CANCEL[] PROGMEM_N1 = "// action:cancel"; ////
const char MSG_FANCHECK_EXTRUDER[] PROGMEM_N1 = "Err: EXTR. FAN ERROR"; ////c=20
const char MSG_FANCHECK_PRINT[] PROGMEM_N1 = "Err: PRINT FAN ERROR"; ////c=20
const char MSG_M112_KILL[] PROGMEM_N1 = "M112 called. Emergency Stop."; ////c=20
const char MSG_ADVANCE_K[] PROGMEM_N1 = "Advance K:"; ////c=13
const char MSG_POWERPANIC_DETECTED[] PROGMEM_N1 = "POWER PANIC DETECTED"; ////c=20
