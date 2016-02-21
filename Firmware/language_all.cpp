#include <avr/pgmspace.h>
#define LCD_WIDTH 20
extern unsigned char lang_selected;
const char MSGEN0[] PROGMEM = { "Prusa i3 ready." }; //WELCOME_MSG
const char MSGCZ0[] PROGMEM = { "Prusa i3 pripravena" }; //WELCOME_MSG
const char MSGEN1[] PROGMEM = { "Card inserted" }; //MSG_SD_INSERTED
const char MSGCZ1[] PROGMEM = { "Karta vlozena" }; //MSG_SD_INSERTED
const char MSGEN2[] PROGMEM = { "Card removed" }; //MSG_SD_REMOVED
const char MSGCZ2[] PROGMEM = { "Karta vyjmuta" }; //MSG_SD_REMOVED
const char MSGEN3[] PROGMEM = { "Main" }; //MSG_MAIN
const char MSGCZ3[] PROGMEM = { "Hlavni nabidka" }; //MSG_MAIN
const char MSGEN4[] PROGMEM = { "Autostart" }; //MSG_AUTOSTART
const char MSGCZ4[] PROGMEM = { "Autostart" }; //MSG_AUTOSTART
const char MSGEN5[] PROGMEM = { "Disable steppers" }; //MSG_DISABLE_STEPPERS
const char MSGCZ5[] PROGMEM = { "Vypnout motory" }; //MSG_DISABLE_STEPPERS
const char MSGEN6[] PROGMEM = { "Auto home" }; //MSG_AUTO_HOME
const char MSGCZ6[] PROGMEM = { "Auto home" }; //MSG_AUTO_HOME
const char MSGEN7[] PROGMEM = { "Set home offsets" }; //MSG_SET_HOME_OFFSETS
const char MSGCZ7[] PROGMEM = { "Nastav pocatek home" }; //MSG_SET_HOME_OFFSETS
const char MSGEN8[] PROGMEM = { "Set origin" }; //MSG_SET_ORIGIN
const char MSGCZ8[] PROGMEM = { "Nastav pocatek" }; //MSG_SET_ORIGIN
const char MSGEN9[] PROGMEM = { "Preheat PLA" }; //MSG_PREHEAT_PLA
const char MSGCZ9[] PROGMEM = { "Predehrev PLA" }; //MSG_PREHEAT_PLA
const char MSGEN10[] PROGMEM = { "Preheat PLA 1" }; //MSG_PREHEAT_PLA0
const char MSGCZ10[] PROGMEM = { "Predehrev PLA 1" }; //MSG_PREHEAT_PLA0
const char MSGEN11[] PROGMEM = { "Preheat PLA 2" }; //MSG_PREHEAT_PLA1
const char MSGCZ11[] PROGMEM = { "Predehrev PLA 2" }; //MSG_PREHEAT_PLA1
const char MSGEN12[] PROGMEM = { "Preheat PLA 3" }; //MSG_PREHEAT_PLA2
const char MSGCZ12[] PROGMEM = { "Predehrev PLA 3" }; //MSG_PREHEAT_PLA2
const char MSGEN13[] PROGMEM = { "Preheat PLA All" }; //MSG_PREHEAT_PLA012
const char MSGCZ13[] PROGMEM = { "Predehrev PLA All" }; //MSG_PREHEAT_PLA012
const char MSGEN14[] PROGMEM = { "Preheat PLA Bed" }; //MSG_PREHEAT_PLA_BEDONLY
const char MSGCZ14[] PROGMEM = { "Predehrev PLA Bed" }; //MSG_PREHEAT_PLA_BEDONLY
const char MSGEN15[] PROGMEM = { "Preheat PLA conf" }; //MSG_PREHEAT_PLA_SETTINGS
const char MSGCZ15[] PROGMEM = { "Predehrev PLA conf" }; //MSG_PREHEAT_PLA_SETTINGS
const char MSGEN16[] PROGMEM = { "Preheat ABS" }; //MSG_PREHEAT_ABS
const char MSGCZ16[] PROGMEM = { "Predehrev ABS" }; //MSG_PREHEAT_ABS
const char MSGEN17[] PROGMEM = { "Preheat ABS 1" }; //MSG_PREHEAT_ABS0
const char MSGCZ17[] PROGMEM = { "Predehrev ABS 1" }; //MSG_PREHEAT_ABS0
const char MSGEN18[] PROGMEM = { "Preheat ABS 2" }; //MSG_PREHEAT_ABS1
const char MSGCZ18[] PROGMEM = { "Predehrev ABS 2" }; //MSG_PREHEAT_ABS1
const char MSGEN19[] PROGMEM = { "Preheat ABS 3" }; //MSG_PREHEAT_ABS2
const char MSGCZ19[] PROGMEM = { "Predehrev ABS 3" }; //MSG_PREHEAT_ABS2
const char MSGEN20[] PROGMEM = { "Preheat ABS All" }; //MSG_PREHEAT_ABS012
const char MSGCZ20[] PROGMEM = { "Predehrev ABS All" }; //MSG_PREHEAT_ABS012
const char MSGEN21[] PROGMEM = { "Preheat ABS Bed" }; //MSG_PREHEAT_ABS_BEDONLY
const char MSGCZ21[] PROGMEM = { "Predehrev ABS Bed" }; //MSG_PREHEAT_ABS_BEDONLY
const char MSGEN22[] PROGMEM = { "Preheat ABS conf" }; //MSG_PREHEAT_ABS_SETTINGS
const char MSGCZ22[] PROGMEM = { "Predehrev ABS conf" }; //MSG_PREHEAT_ABS_SETTINGS
const char MSGEN23[] PROGMEM = { "Cooldown" }; //MSG_COOLDOWN
const char MSGCZ23[] PROGMEM = { "Zchladit" }; //MSG_COOLDOWN
const char MSGEN24[] PROGMEM = { "Switch power on" }; //MSG_SWITCH_PS_ON
const char MSGCZ24[] PROGMEM = { "Vypnout zdroj" }; //MSG_SWITCH_PS_ON
const char MSGEN25[] PROGMEM = { "Switch power off" }; //MSG_SWITCH_PS_OFF
const char MSGCZ25[] PROGMEM = { "Zapnout zdroj" }; //MSG_SWITCH_PS_OFF
const char MSGEN26[] PROGMEM = { "Extrude" }; //MSG_EXTRUDE
const char MSGCZ26[] PROGMEM = { "Extrudovat" }; //MSG_EXTRUDE
const char MSGEN27[] PROGMEM = { "Retract" }; //MSG_RETRACT
const char MSGCZ27[] PROGMEM = { "Retract" }; //MSG_RETRACT
const char MSGEN28[] PROGMEM = { "Move axis" }; //MSG_MOVE_AXIS
const char MSGCZ28[] PROGMEM = { "Posunout osu" }; //MSG_MOVE_AXIS
const char MSGEN29[] PROGMEM = { "Move X" }; //MSG_MOVE_X
const char MSGCZ29[] PROGMEM = { "Posunout X" }; //MSG_MOVE_X
const char MSGEN30[] PROGMEM = { "Move Y" }; //MSG_MOVE_Y
const char MSGCZ30[] PROGMEM = { "Posunout Y" }; //MSG_MOVE_Y
const char MSGEN31[] PROGMEM = { "Move Z" }; //MSG_MOVE_Z
const char MSGCZ31[] PROGMEM = { "Posunout Z" }; //MSG_MOVE_Z
const char MSGEN32[] PROGMEM = { "Extruder" }; //MSG_MOVE_E
const char MSGCZ32[] PROGMEM = { "Extruder" }; //MSG_MOVE_E
const char MSGEN33[] PROGMEM = { "Extruder2" }; //MSG_MOVE_E1
const char MSGCZ33[] PROGMEM = { "Extruder2" }; //MSG_MOVE_E1
const char MSGEN34[] PROGMEM = { "Extruder3" }; //MSG_MOVE_E2
const char MSGCZ34[] PROGMEM = { "Extruder3" }; //MSG_MOVE_E2
const char MSGEN35[] PROGMEM = { "Move 0.1mm" }; //MSG_MOVE_01MM
const char MSGCZ35[] PROGMEM = { "Posunout o 0.1mm" }; //MSG_MOVE_01MM
const char MSGEN36[] PROGMEM = { "Move 1mm" }; //MSG_MOVE_1MM
const char MSGCZ36[] PROGMEM = { "Posunout o 1mm" }; //MSG_MOVE_1MM
const char MSGEN37[] PROGMEM = { "Move 10mm" }; //MSG_MOVE_10MM
const char MSGCZ37[] PROGMEM = { "Posunout o 10mm" }; //MSG_MOVE_10MM
const char MSGEN38[] PROGMEM = { "Speed" }; //MSG_SPEED
const char MSGCZ38[] PROGMEM = { "Rychlost" }; //MSG_SPEED
const char MSGEN39[] PROGMEM = { "Nozzle" }; //MSG_NOZZLE
const char MSGCZ39[] PROGMEM = { "Tryska" }; //MSG_NOZZLE
const char MSGEN40[] PROGMEM = { "Nozzle2" }; //MSG_NOZZLE1
const char MSGCZ40[] PROGMEM = { "Tryska2" }; //MSG_NOZZLE1
const char MSGEN41[] PROGMEM = { "Nozzle3" }; //MSG_NOZZLE2
const char MSGCZ41[] PROGMEM = { "Tryska3" }; //MSG_NOZZLE2
const char MSGEN42[] PROGMEM = { "Bed" }; //MSG_BED
const char MSGCZ42[] PROGMEM = { "Bed" }; //MSG_BED
const char MSGEN43[] PROGMEM = { "Fan speed" }; //MSG_FAN_SPEED
const char MSGCZ43[] PROGMEM = { "Rychlost vent." }; //MSG_FAN_SPEED
const char MSGEN44[] PROGMEM = { "Flow" }; //MSG_FLOW
const char MSGCZ44[] PROGMEM = { "Prutok" }; //MSG_FLOW
const char MSGEN45[] PROGMEM = { "Flow 0" }; //MSG_FLOW0
const char MSGCZ45[] PROGMEM = { "Prutok 0" }; //MSG_FLOW0
const char MSGEN46[] PROGMEM = { "Flow 1" }; //MSG_FLOW1
const char MSGCZ46[] PROGMEM = { "Prutok 1" }; //MSG_FLOW1
const char MSGEN47[] PROGMEM = { "Flow 2" }; //MSG_FLOW2
const char MSGCZ47[] PROGMEM = { "Prutok 2" }; //MSG_FLOW2
const char MSGEN48[] PROGMEM = { "Control" }; //MSG_CONTROL
const char MSGCZ48[] PROGMEM = { "Kontrola" }; //MSG_CONTROL
const char MSGEN49[] PROGMEM = { " \002 Min" }; //MSG_MIN
const char MSGCZ49[] PROGMEM = { " \002 Min" }; //MSG_MIN
const char MSGEN50[] PROGMEM = { " \002 Max" }; //MSG_MAX
const char MSGCZ50[] PROGMEM = { " \002 Max" }; //MSG_MAX
const char MSGEN51[] PROGMEM = { " \002 Fact" }; //MSG_FACTOR
const char MSGCZ51[] PROGMEM = { " \002 Fact" }; //MSG_FACTOR
const char MSGEN52[] PROGMEM = { "Autotemp" }; //MSG_AUTOTEMP
const char MSGCZ52[] PROGMEM = { "Autotemp" }; //MSG_AUTOTEMP
const char MSGEN53[] PROGMEM = { "On " }; //MSG_ON
const char MSGCZ53[] PROGMEM = { "On " }; //MSG_ON
const char MSGEN54[] PROGMEM = { "Off" }; //MSG_OFF
const char MSGCZ54[] PROGMEM = { "Off" }; //MSG_OFF
const char MSGEN55[] PROGMEM = { "PID-P" }; //MSG_PID_P
const char MSGCZ55[] PROGMEM = { "PID-P" }; //MSG_PID_P
const char MSGEN56[] PROGMEM = { "PID-I" }; //MSG_PID_I
const char MSGCZ56[] PROGMEM = { "PID-I" }; //MSG_PID_I
const char MSGEN57[] PROGMEM = { "PID-D" }; //MSG_PID_D
const char MSGCZ57[] PROGMEM = { "PID-D" }; //MSG_PID_D
const char MSGEN58[] PROGMEM = { "PID-C" }; //MSG_PID_C
const char MSGCZ58[] PROGMEM = { "PID-C" }; //MSG_PID_C
const char MSGEN59[] PROGMEM = { "Accel" }; //MSG_ACC
const char MSGCZ59[] PROGMEM = { "Accel" }; //MSG_ACC
const char MSGEN60[] PROGMEM = { "Vxy-jerk" }; //MSG_VXY_JERK
const char MSGCZ60[] PROGMEM = { "Vxy-jerk" }; //MSG_VXY_JERK
const char MSGEN61[] PROGMEM = { "Vz-jerk" }; //MSG_VZ_JERK
const char MSGCZ61[] PROGMEM = { "Vz-jerk" }; //MSG_VZ_JERK
const char MSGEN62[] PROGMEM = { "Ve-jerk" }; //MSG_VE_JERK
const char MSGCZ62[] PROGMEM = { "Ve-jerk" }; //MSG_VE_JERK
const char MSGEN63[] PROGMEM = { "Vmax " }; //MSG_VMAX
const char MSGCZ63[] PROGMEM = { "Vmax " }; //MSG_VMAX
const char MSGEN64[] PROGMEM = { "x" }; //MSG_X
const char MSGCZ64[] PROGMEM = { "x" }; //MSG_X
const char MSGEN65[] PROGMEM = { "y" }; //MSG_Y
const char MSGCZ65[] PROGMEM = { "y" }; //MSG_Y
const char MSGEN66[] PROGMEM = { "z" }; //MSG_Z
const char MSGCZ66[] PROGMEM = { "z" }; //MSG_Z
const char MSGEN67[] PROGMEM = { "e" }; //MSG_E
const char MSGCZ67[] PROGMEM = { "e" }; //MSG_E
const char MSGEN68[] PROGMEM = { "Vmin" }; //MSG_VMIN
const char MSGCZ68[] PROGMEM = { "Vmin" }; //MSG_VMIN
const char MSGEN69[] PROGMEM = { "VTrav min" }; //MSG_VTRAV_MIN
const char MSGCZ69[] PROGMEM = { "VTrav min" }; //MSG_VTRAV_MIN
const char MSGEN70[] PROGMEM = { "Amax " }; //MSG_AMAX
const char MSGCZ70[] PROGMEM = { "Amax " }; //MSG_AMAX
const char MSGEN71[] PROGMEM = { "A-retract" }; //MSG_A_RETRACT
const char MSGCZ71[] PROGMEM = { "A-retract" }; //MSG_A_RETRACT
const char MSGEN72[] PROGMEM = { "Xsteps/mm" }; //MSG_XSTEPS
const char MSGCZ72[] PROGMEM = { "Xsteps/mm" }; //MSG_XSTEPS
const char MSGEN73[] PROGMEM = { "Ysteps/mm" }; //MSG_YSTEPS
const char MSGCZ73[] PROGMEM = { "Ysteps/mm" }; //MSG_YSTEPS
const char MSGEN74[] PROGMEM = { "Zsteps/mm" }; //MSG_ZSTEPS
const char MSGCZ74[] PROGMEM = { "Zsteps/mm" }; //MSG_ZSTEPS
const char MSGEN75[] PROGMEM = { "Esteps/mm" }; //MSG_ESTEPS
const char MSGCZ75[] PROGMEM = { "Esteps/mm" }; //MSG_ESTEPS
const char MSGEN76[] PROGMEM = { "Temperature" }; //MSG_TEMPERATURE
const char MSGCZ76[] PROGMEM = { "Teplota" }; //MSG_TEMPERATURE
const char MSGEN77[] PROGMEM = { "Motion" }; //MSG_MOTION
const char MSGCZ77[] PROGMEM = { "Pohyb" }; //MSG_MOTION
const char MSGEN78[] PROGMEM = { "Filament" }; //MSG_VOLUMETRIC
const char MSGCZ78[] PROGMEM = { "Filament" }; //MSG_VOLUMETRIC
const char MSGEN79[] PROGMEM = { "E in mm3" }; //MSG_VOLUMETRIC_ENABLED
const char MSGCZ79[] PROGMEM = { "E in mm3" }; //MSG_VOLUMETRIC_ENABLED
const char MSGEN80[] PROGMEM = { "Fil. Dia. 1" }; //MSG_FILAMENT_SIZE_EXTRUDER_0
const char MSGCZ80[] PROGMEM = { "Fil. Dia. 1" }; //MSG_FILAMENT_SIZE_EXTRUDER_0
const char MSGEN81[] PROGMEM = { "Fil. Dia. 2" }; //MSG_FILAMENT_SIZE_EXTRUDER_1
const char MSGCZ81[] PROGMEM = { "Fil. Dia. 2" }; //MSG_FILAMENT_SIZE_EXTRUDER_1
const char MSGEN82[] PROGMEM = { "Fil. Dia. 3" }; //MSG_FILAMENT_SIZE_EXTRUDER_2
const char MSGCZ82[] PROGMEM = { "Fil. Dia. 3" }; //MSG_FILAMENT_SIZE_EXTRUDER_2
const char MSGEN83[] PROGMEM = { "LCD contrast" }; //MSG_CONTRAST
const char MSGCZ83[] PROGMEM = { "LCD contrast" }; //MSG_CONTRAST
const char MSGEN84[] PROGMEM = { "Store memory" }; //MSG_STORE_EPROM
const char MSGCZ84[] PROGMEM = { "Store memory" }; //MSG_STORE_EPROM
const char MSGEN85[] PROGMEM = { "Load memory" }; //MSG_LOAD_EPROM
const char MSGCZ85[] PROGMEM = { "Ulozit pamet" }; //MSG_LOAD_EPROM
const char MSGEN86[] PROGMEM = { "Restore failsafe" }; //MSG_RESTORE_FAILSAFE
const char MSGCZ86[] PROGMEM = { "Obnovit vychozi" }; //MSG_RESTORE_FAILSAFE
const char MSGEN87[] PROGMEM = { "Refresh" }; //MSG_REFRESH
const char MSGCZ87[] PROGMEM = { "Obnovit" }; //MSG_REFRESH
const char MSGEN88[] PROGMEM = { "Info screen" }; //MSG_WATCH
const char MSGCZ88[] PROGMEM = { "Informace" }; //MSG_WATCH
const char MSGEN89[] PROGMEM = { "Prepare" }; //MSG_PREPARE
const char MSGCZ89[] PROGMEM = { "Priprava" }; //MSG_PREPARE
const char MSGEN90[] PROGMEM = { "Tune" }; //MSG_TUNE
const char MSGCZ90[] PROGMEM = { "Ladit" }; //MSG_TUNE
const char MSGEN91[] PROGMEM = { "Pause print" }; //MSG_PAUSE_PRINT
const char MSGCZ91[] PROGMEM = { "Pozastavit tisk" }; //MSG_PAUSE_PRINT
const char MSGEN92[] PROGMEM = { "Resume print" }; //MSG_RESUME_PRINT
const char MSGCZ92[] PROGMEM = { "Pokracovat" }; //MSG_RESUME_PRINT
const char MSGEN93[] PROGMEM = { "Stop print" }; //MSG_STOP_PRINT
const char MSGCZ93[] PROGMEM = { "Zastavit tisk" }; //MSG_STOP_PRINT
const char MSGEN94[] PROGMEM = { "Print from SD" }; //MSG_CARD_MENU
const char MSGCZ94[] PROGMEM = { "Tisk z SD" }; //MSG_CARD_MENU
const char MSGEN95[] PROGMEM = { "No SD card" }; //MSG_NO_CARD
const char MSGCZ95[] PROGMEM = { "Zadna SD karta" }; //MSG_NO_CARD
const char MSGEN96[] PROGMEM = { "Sleep..." }; //MSG_DWELL
const char MSGCZ96[] PROGMEM = { "Sleep..." }; //MSG_DWELL
const char MSGEN97[] PROGMEM = { "Wait for user..." }; //MSG_USERWAIT
const char MSGCZ97[] PROGMEM = { "Wait for user..." }; //MSG_USERWAIT
const char MSGEN98[] PROGMEM = { "Resuming print" }; //MSG_RESUMING
const char MSGCZ98[] PROGMEM = { "Obnoveni tisku" }; //MSG_RESUMING
const char MSGEN99[] PROGMEM = { "Print aborted" }; //MSG_PRINT_ABORTED
const char MSGCZ99[] PROGMEM = { "Tisk prerusen" }; //MSG_PRINT_ABORTED
const char MSGEN100[] PROGMEM = { "No move." }; //MSG_NO_MOVE
const char MSGCZ100[] PROGMEM = { "No move." }; //MSG_NO_MOVE
const char MSGEN101[] PROGMEM = { "KILLED. " }; //MSG_KILLED
const char MSGCZ101[] PROGMEM = { "KILLED. " }; //MSG_KILLED
const char MSGEN102[] PROGMEM = { "STOPPED. " }; //MSG_STOPPED
const char MSGCZ102[] PROGMEM = { "STOPPED. " }; //MSG_STOPPED
const char MSGEN103[] PROGMEM = { "Retract mm" }; //MSG_CONTROL_RETRACT
const char MSGCZ103[] PROGMEM = { "Retract mm" }; //MSG_CONTROL_RETRACT
const char MSGEN104[] PROGMEM = { "Swap Re.mm" }; //MSG_CONTROL_RETRACT_SWAP
const char MSGCZ104[] PROGMEM = { "Swap Re.mm" }; //MSG_CONTROL_RETRACT_SWAP
const char MSGEN105[] PROGMEM = { "Retract  V" }; //MSG_CONTROL_RETRACTF
const char MSGCZ105[] PROGMEM = { "Retract  V" }; //MSG_CONTROL_RETRACTF
const char MSGEN106[] PROGMEM = { "Hop mm" }; //MSG_CONTROL_RETRACT_ZLIFT
const char MSGCZ106[] PROGMEM = { "Hop mm" }; //MSG_CONTROL_RETRACT_ZLIFT
const char MSGEN107[] PROGMEM = { "UnRet +mm" }; //MSG_CONTROL_RETRACT_RECOVER
const char MSGCZ107[] PROGMEM = { "UnRet +mm" }; //MSG_CONTROL_RETRACT_RECOVER
const char MSGEN108[] PROGMEM = { "S UnRet+mm" }; //MSG_CONTROL_RETRACT_RECOVER_SWAP
const char MSGCZ108[] PROGMEM = { "S UnRet+mm" }; //MSG_CONTROL_RETRACT_RECOVER_SWAP
const char MSGEN109[] PROGMEM = { "UnRet  V" }; //MSG_CONTROL_RETRACT_RECOVERF
const char MSGCZ109[] PROGMEM = { "UnRet  V" }; //MSG_CONTROL_RETRACT_RECOVERF
const char MSGEN110[] PROGMEM = { "AutoRetr." }; //MSG_AUTORETRACT
const char MSGCZ110[] PROGMEM = { "AutoRetr." }; //MSG_AUTORETRACT
const char MSGEN111[] PROGMEM = { "Change filament" }; //MSG_FILAMENTCHANGE
const char MSGCZ111[] PROGMEM = { "Vymenit filament" }; //MSG_FILAMENTCHANGE
const char MSGEN112[] PROGMEM = { "Init. SD card" }; //MSG_INIT_SDCARD
const char MSGCZ112[] PROGMEM = { "Inic. SD" }; //MSG_INIT_SDCARD
const char MSGEN113[] PROGMEM = { "Change SD card" }; //MSG_CNG_SDCARD
const char MSGCZ113[] PROGMEM = { "Vymenit SD" }; //MSG_CNG_SDCARD
const char MSGEN114[] PROGMEM = { "Z probe out. bed" }; //MSG_ZPROBE_OUT
const char MSGCZ114[] PROGMEM = { "Z probe out. bed" }; //MSG_ZPROBE_OUT
const char MSGEN115[] PROGMEM = { "Home X/Y before Z" }; //MSG_POSITION_UNKNOWN
const char MSGCZ115[] PROGMEM = { "Home X/Y before Z" }; //MSG_POSITION_UNKNOWN
const char MSGEN116[] PROGMEM = { "Z Offset" }; //MSG_ZPROBE_ZOFFSET
const char MSGCZ116[] PROGMEM = { "Z Offset" }; //MSG_ZPROBE_ZOFFSET
const char MSGEN117[] PROGMEM = { "Babystep X" }; //MSG_BABYSTEP_X
const char MSGCZ117[] PROGMEM = { "Babystep X" }; //MSG_BABYSTEP_X
const char MSGEN118[] PROGMEM = { "Babystep Y" }; //MSG_BABYSTEP_Y
const char MSGCZ118[] PROGMEM = { "Babystep Y" }; //MSG_BABYSTEP_Y
const char MSGEN119[] PROGMEM = { "Live adjust Z" }; //MSG_BABYSTEP_Z
const char MSGCZ119[] PROGMEM = { "Doladeni osy Z" }; //MSG_BABYSTEP_Z
const char MSGEN120[] PROGMEM = { "Endstop abort" }; //MSG_ENDSTOP_ABORT
const char MSGCZ120[] PROGMEM = { "Endstop abort" }; //MSG_ENDSTOP_ABORT
const char MSGEN121[] PROGMEM = { "Auto adjust Z ?" }; //MSG_ADJUSTZ
const char MSGCZ121[] PROGMEM = { "Auto doladit Z ?" }; //MSG_ADJUSTZ
const char MSGEN122[] PROGMEM = { "Calibrate Z" }; //MSG_HOMEYZ
const char MSGCZ122[] PROGMEM = { "Kalibrovat Z" }; //MSG_HOMEYZ
const char MSGEN123[] PROGMEM = { "Settings" }; //MSG_SETTINGS
const char MSGCZ123[] PROGMEM = { "Nastaveni" }; //MSG_SETTINGS
const char MSGEN124[] PROGMEM = { "Preheat" }; //MSG_PREHEAT
const char MSGCZ124[] PROGMEM = { "Predehrev" }; //MSG_PREHEAT
const char MSGEN125[] PROGMEM = { "Unload filament" }; //MSG_UNLOAD_FILAMENT
const char MSGCZ125[] PROGMEM = { "Vyjmout filament" }; //MSG_UNLOAD_FILAMENT
const char MSGEN126[] PROGMEM = { "Load filament" }; //MSG_LOAD_FILAMENT
const char MSGCZ126[] PROGMEM = { "Zavest filament" }; //MSG_LOAD_FILAMENT
const char MSGEN127[] PROGMEM = { "Rectract" }; //MSG_RECTRACT
const char MSGCZ127[] PROGMEM = { "Rectract" }; //MSG_RECTRACT
const char MSGEN128[] PROGMEM = { "ERROR:" }; //MSG_ERROR
const char MSGCZ128[] PROGMEM = { "CHYBA:" }; //MSG_ERROR
const char MSGEN129[] PROGMEM = { "Preheat the nozzle!" }; //MSG_PREHEAT_NOZZLE
const char MSGCZ129[] PROGMEM = { "Predehrejte trysku!" }; //MSG_PREHEAT_NOZZLE
const char MSGEN130[] PROGMEM = { "Support" }; //MSG_SUPPORT
const char MSGCZ130[] PROGMEM = { "Podpora" }; //MSG_SUPPORT
const char MSGEN131[] PROGMEM = { "Changed correctly?" }; //MSG_CORRECTLY
const char MSGCZ131[] PROGMEM = { "Vymena ok?" }; //MSG_CORRECTLY
const char MSGEN132[] PROGMEM = { "Yes" }; //MSG_YES
const char MSGCZ132[] PROGMEM = { "Ano" }; //MSG_YES
const char MSGEN133[] PROGMEM = { "No" }; //MSG_NO
const char MSGCZ133[] PROGMEM = { "Ne" }; //MSG_NO
const char MSGEN134[] PROGMEM = { "Filament not loaded" }; //MSG_NOT_LOADED
const char MSGCZ134[] PROGMEM = { "Filament nezaveden" }; //MSG_NOT_LOADED
const char MSGEN135[] PROGMEM = { "Color not clear" }; //MSG_NOT_COLOR
const char MSGCZ135[] PROGMEM = { "Barva neni cista" }; //MSG_NOT_COLOR
const char MSGEN136[] PROGMEM = { "Loading filament" }; //MSG_LOADING_FILAMENT
const char MSGCZ136[] PROGMEM = { "Zavadeni filamentu" }; //MSG_LOADING_FILAMENT
const char MSGEN137[] PROGMEM = { "Please wait" }; //MSG_PLEASE_WAIT
const char MSGCZ137[] PROGMEM = { "Prosim cekejte" }; //MSG_PLEASE_WAIT
const char MSGEN138[] PROGMEM = { "Loading color" }; //MSG_LOADING_COLOR
const char MSGCZ138[] PROGMEM = { "Cisteni barvy" }; //MSG_LOADING_COLOR
const char MSGEN139[] PROGMEM = { "Change success!" }; //MSG_CHANGE_SUCCESS
const char MSGCZ139[] PROGMEM = { "Zmena uspesna!" }; //MSG_CHANGE_SUCCESS
const char MSGEN140[] PROGMEM = { "And press the knob" }; //MSG_PRESS
const char MSGCZ140[] PROGMEM = { "A stisknete tlacitko" }; //MSG_PRESS
const char MSGEN141[] PROGMEM = { "Insert filament" }; //MSG_INSERT_FILAMENT
const char MSGCZ141[] PROGMEM = { "Vlozte filament" }; //MSG_INSERT_FILAMENT
const char MSGEN142[] PROGMEM = { "Changing filament!" }; //MSG_CHANGING_FILAMENT
const char MSGCZ142[] PROGMEM = { "Vymena filamentu!" }; //MSG_CHANGING_FILAMENT
const char MSGEN143[] PROGMEM = { "Mode     [silent]" }; //MSG_SILENT_MODE_ON
const char MSGCZ143[] PROGMEM = { "Mod       [tichy]" }; //MSG_SILENT_MODE_ON
const char MSGEN144[] PROGMEM = { "Mode [high power]" }; //MSG_SILENT_MODE_OFF
const char MSGCZ144[] PROGMEM = { "Mod  [vys. vykon]" }; //MSG_SILENT_MODE_OFF
const char MSGEN145[] PROGMEM = { "Reboot the printer" }; //MSG_REBOOT
const char MSGCZ145[] PROGMEM = { "Restartujte tiskarnu" }; //MSG_REBOOT
const char MSGEN146[] PROGMEM = { " for take effect" }; //MSG_TAKE_EFFECT
const char MSGCZ146[] PROGMEM = { " pro projeveni zmen" }; //MSG_TAKE_EFFECT
const char MSGEN147[] PROGMEM = { "enqueing \"" }; //MSG_Enqueing
const char MSGCZ147[] PROGMEM = { "enqueing \"" }; //MSG_Enqueing
const char MSGEN148[] PROGMEM = { "PowerUp" }; //MSG_POWERUP
const char MSGCZ148[] PROGMEM = { "PowerUp" }; //MSG_POWERUP
const char MSGEN149[] PROGMEM = { " External Reset" }; //MSG_EXTERNAL_RESET
const char MSGCZ149[] PROGMEM = { " External Reset" }; //MSG_EXTERNAL_RESET
const char MSGEN150[] PROGMEM = { " Brown out Reset" }; //MSG_BROWNOUT_RESET
const char MSGCZ150[] PROGMEM = { " Brown out Reset" }; //MSG_BROWNOUT_RESET
const char MSGEN151[] PROGMEM = { " Watchdog Reset" }; //MSG_WATCHDOG_RESET
const char MSGCZ151[] PROGMEM = { " Watchdog Reset" }; //MSG_WATCHDOG_RESET
const char MSGEN152[] PROGMEM = { " Software Reset" }; //MSG_SOFTWARE_RESET
const char MSGCZ152[] PROGMEM = { " Software Reset" }; //MSG_SOFTWARE_RESET
const char MSGEN153[] PROGMEM = { " | Author: " }; //MSG_AUTHOR
const char MSGCZ153[] PROGMEM = { " | Author: " }; //MSG_AUTHOR
const char MSGEN154[] PROGMEM = { " Last Updated: " }; //MSG_CONFIGURATION_VER
const char MSGCZ154[] PROGMEM = { " Last Updated: " }; //MSG_CONFIGURATION_VER
const char MSGEN155[] PROGMEM = { " Free Memory: " }; //MSG_FREE_MEMORY
const char MSGCZ155[] PROGMEM = { " Free Memory: " }; //MSG_FREE_MEMORY
const char MSGEN156[] PROGMEM = { "  PlannerBufferBytes: " }; //MSG_PLANNER_BUFFER_BYTES
const char MSGCZ156[] PROGMEM = { "  PlannerBufferBytes: " }; //MSG_PLANNER_BUFFER_BYTES
const char MSGEN157[] PROGMEM = { "ok" }; //MSG_OK
const char MSGCZ157[] PROGMEM = { "ok" }; //MSG_OK
const char MSGEN158[] PROGMEM = { "Done saving file." }; //MSG_FILE_SAVED
const char MSGCZ158[] PROGMEM = { "Done saving file." }; //MSG_FILE_SAVED
const char MSGEN159[] PROGMEM = { "Line Number is not Last Line Number+1, Last Line: " }; //MSG_ERR_LINE_NO
const char MSGCZ159[] PROGMEM = { "Line Number is not Last Line Number+1, Last Line: " }; //MSG_ERR_LINE_NO
const char MSGEN160[] PROGMEM = { "checksum mismatch, Last Line: " }; //MSG_ERR_CHECKSUM_MISMATCH
const char MSGCZ160[] PROGMEM = { "checksum mismatch, Last Line: " }; //MSG_ERR_CHECKSUM_MISMATCH
const char MSGEN161[] PROGMEM = { "No Checksum with line number, Last Line: " }; //MSG_ERR_NO_CHECKSUM
const char MSGCZ161[] PROGMEM = { "No Checksum with line number, Last Line: " }; //MSG_ERR_NO_CHECKSUM
const char MSGEN162[] PROGMEM = { "No Line Number with checksum, Last Line: " }; //MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM
const char MSGCZ162[] PROGMEM = { "No Line Number with checksum, Last Line: " }; //MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM
const char MSGEN163[] PROGMEM = { "Done printing file" }; //MSG_FILE_PRINTED
const char MSGCZ163[] PROGMEM = { "Done printing file" }; //MSG_FILE_PRINTED
const char MSGEN164[] PROGMEM = { "Begin file list" }; //MSG_BEGIN_FILE_LIST
const char MSGCZ164[] PROGMEM = { "Begin file list" }; //MSG_BEGIN_FILE_LIST
const char MSGEN165[] PROGMEM = { "End file list" }; //MSG_END_FILE_LIST
const char MSGCZ165[] PROGMEM = { "End file list" }; //MSG_END_FILE_LIST
const char MSGEN166[] PROGMEM = { "M104 Invalid extruder " }; //MSG_M104_INVALID_EXTRUDER
const char MSGCZ166[] PROGMEM = { "M104 Invalid extruder " }; //MSG_M104_INVALID_EXTRUDER
const char MSGEN167[] PROGMEM = { "M105 Invalid extruder " }; //MSG_M105_INVALID_EXTRUDER
const char MSGCZ167[] PROGMEM = { "M105 Invalid extruder " }; //MSG_M105_INVALID_EXTRUDER
const char MSGEN168[] PROGMEM = { "M200 Invalid extruder " }; //MSG_M200_INVALID_EXTRUDER
const char MSGCZ168[] PROGMEM = { "M200 Invalid extruder " }; //MSG_M200_INVALID_EXTRUDER
const char MSGEN169[] PROGMEM = { "M218 Invalid extruder " }; //MSG_M218_INVALID_EXTRUDER
const char MSGCZ169[] PROGMEM = { "M218 Invalid extruder " }; //MSG_M218_INVALID_EXTRUDER
const char MSGEN170[] PROGMEM = { "M221 Invalid extruder " }; //MSG_M221_INVALID_EXTRUDER
const char MSGCZ170[] PROGMEM = { "M221 Invalid extruder " }; //MSG_M221_INVALID_EXTRUDER
const char MSGEN171[] PROGMEM = { "No thermistors - no temperature" }; //MSG_ERR_NO_THERMISTORS
const char MSGCZ171[] PROGMEM = { "No thermistors - no temperature" }; //MSG_ERR_NO_THERMISTORS
const char MSGEN172[] PROGMEM = { "M109 Invalid extruder " }; //MSG_M109_INVALID_EXTRUDER
const char MSGCZ172[] PROGMEM = { "M109 Invalid extruder " }; //MSG_M109_INVALID_EXTRUDER
const char MSGEN173[] PROGMEM = { "Heating..." }; //MSG_HEATING
const char MSGCZ173[] PROGMEM = { "Zahrivani..." }; //MSG_HEATING
const char MSGEN174[] PROGMEM = { "Heating done." }; //MSG_HEATING_COMPLETE
const char MSGCZ174[] PROGMEM = { "Zahrivani OK." }; //MSG_HEATING_COMPLETE
const char MSGEN175[] PROGMEM = { "Bed Heating." }; //MSG_BED_HEATING
const char MSGCZ175[] PROGMEM = { "Zahrivani bed..." }; //MSG_BED_HEATING
const char MSGEN176[] PROGMEM = { "Bed done." }; //MSG_BED_DONE
const char MSGCZ176[] PROGMEM = { "Bed OK." }; //MSG_BED_DONE
const char MSGEN177[] PROGMEM = { "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:https://github.com/prusa3d/Prusa-i3-Plus/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:" "Prusa i3 EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000\n" }; //MSG_M115_REPORT
const char MSGCZ177[] PROGMEM = { "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:https://github.com/prusa3d/Prusa-i3-Plus/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:" "Prusa i3 EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000\n" }; //MSG_M115_REPORT
const char MSGEN178[] PROGMEM = { " Count X: " }; //MSG_COUNT_X
const char MSGCZ178[] PROGMEM = { " Count X: " }; //MSG_COUNT_X
const char MSGEN179[] PROGMEM = { "Printer halted. kill() called!" }; //MSG_ERR_KILLED
const char MSGCZ179[] PROGMEM = { "Printer halted. kill() called!" }; //MSG_ERR_KILLED
const char MSGEN180[] PROGMEM = { "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)" }; //MSG_ERR_STOPPED
const char MSGCZ180[] PROGMEM = { "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)" }; //MSG_ERR_STOPPED
const char MSGEN181[] PROGMEM = { "Resend: " }; //MSG_RESEND
const char MSGCZ181[] PROGMEM = { "Resend: " }; //MSG_RESEND
const char MSGEN182[] PROGMEM = { "Unknown command: \"" }; //MSG_UNKNOWN_COMMAND
const char MSGCZ182[] PROGMEM = { "Unknown command: \"" }; //MSG_UNKNOWN_COMMAND
const char MSGEN183[] PROGMEM = { "Active Extruder: " }; //MSG_ACTIVE_EXTRUDER
const char MSGCZ183[] PROGMEM = { "Active Extruder: " }; //MSG_ACTIVE_EXTRUDER
const char MSGEN184[] PROGMEM = { "Invalid extruder" }; //MSG_INVALID_EXTRUDER
const char MSGCZ184[] PROGMEM = { "Invalid extruder" }; //MSG_INVALID_EXTRUDER
const char MSGEN185[] PROGMEM = { "x_min: " }; //MSG_X_MIN
const char MSGCZ185[] PROGMEM = { "x_min: " }; //MSG_X_MIN
const char MSGEN186[] PROGMEM = { "x_max: " }; //MSG_X_MAX
const char MSGCZ186[] PROGMEM = { "x_max: " }; //MSG_X_MAX
const char MSGEN187[] PROGMEM = { "y_min: " }; //MSG_Y_MIN
const char MSGCZ187[] PROGMEM = { "y_min: " }; //MSG_Y_MIN
const char MSGEN188[] PROGMEM = { "y_max: " }; //MSG_Y_MAX
const char MSGCZ188[] PROGMEM = { "y_max: " }; //MSG_Y_MAX
const char MSGEN189[] PROGMEM = { "z_min: " }; //MSG_Z_MIN
const char MSGCZ189[] PROGMEM = { "z_min: " }; //MSG_Z_MIN
const char MSGEN190[] PROGMEM = { "z_max: " }; //MSG_Z_MAX
const char MSGCZ190[] PROGMEM = { "z_max: " }; //MSG_Z_MAX
const char MSGEN191[] PROGMEM = { "Reporting endstop status" }; //MSG_M119_REPORT
const char MSGCZ191[] PROGMEM = { "Reporting endstop status" }; //MSG_M119_REPORT
const char MSGEN192[] PROGMEM = { "TRIGGERED" }; //MSG_ENDSTOP_HIT
const char MSGCZ192[] PROGMEM = { "TRIGGERED" }; //MSG_ENDSTOP_HIT
const char MSGEN193[] PROGMEM = { "open" }; //MSG_ENDSTOP_OPEN
const char MSGCZ193[] PROGMEM = { "open" }; //MSG_ENDSTOP_OPEN
const char MSGEN194[] PROGMEM = { "Hotend offsets:" }; //MSG_HOTEND_OFFSET
const char MSGCZ194[] PROGMEM = { "Hotend offsets:" }; //MSG_HOTEND_OFFSET
const char MSGEN195[] PROGMEM = { "Cannot open subdir" }; //MSG_SD_CANT_OPEN_SUBDIR
const char MSGCZ195[] PROGMEM = { "Cannot open subdir" }; //MSG_SD_CANT_OPEN_SUBDIR
const char MSGEN196[] PROGMEM = { "SD init fail" }; //MSG_SD_INIT_FAIL
const char MSGCZ196[] PROGMEM = { "SD init fail" }; //MSG_SD_INIT_FAIL
const char MSGEN197[] PROGMEM = { "volume.init failed" }; //MSG_SD_VOL_INIT_FAIL
const char MSGCZ197[] PROGMEM = { "volume.init failed" }; //MSG_SD_VOL_INIT_FAIL
const char MSGEN198[] PROGMEM = { "openRoot failed" }; //MSG_SD_OPENROOT_FAIL
const char MSGCZ198[] PROGMEM = { "openRoot failed" }; //MSG_SD_OPENROOT_FAIL
const char MSGEN199[] PROGMEM = { "SD card ok" }; //MSG_SD_CARD_OK
const char MSGCZ199[] PROGMEM = { "SD card ok" }; //MSG_SD_CARD_OK
const char MSGEN200[] PROGMEM = { "workDir open failed" }; //MSG_SD_WORKDIR_FAIL
const char MSGCZ200[] PROGMEM = { "workDir open failed" }; //MSG_SD_WORKDIR_FAIL
const char MSGEN201[] PROGMEM = { "open failed, File: " }; //MSG_SD_OPEN_FILE_FAIL
const char MSGCZ201[] PROGMEM = { "open failed, File: " }; //MSG_SD_OPEN_FILE_FAIL
const char MSGEN202[] PROGMEM = { "File opened: " }; //MSG_SD_FILE_OPENED
const char MSGCZ202[] PROGMEM = { "File opened: " }; //MSG_SD_FILE_OPENED
const char MSGEN203[] PROGMEM = { " Size: " }; //MSG_SD_SIZE
const char MSGCZ203[] PROGMEM = { " Size: " }; //MSG_SD_SIZE
const char MSGEN204[] PROGMEM = { "File selected" }; //MSG_SD_FILE_SELECTED
const char MSGCZ204[] PROGMEM = { "File selected" }; //MSG_SD_FILE_SELECTED
const char MSGEN205[] PROGMEM = { "Writing to file: " }; //MSG_SD_WRITE_TO_FILE
const char MSGCZ205[] PROGMEM = { "Writing to file: " }; //MSG_SD_WRITE_TO_FILE
const char MSGEN206[] PROGMEM = { "SD printing byte " }; //MSG_SD_PRINTING_BYTE
const char MSGCZ206[] PROGMEM = { "SD printing byte " }; //MSG_SD_PRINTING_BYTE
const char MSGEN207[] PROGMEM = { "Not SD printing" }; //MSG_SD_NOT_PRINTING
const char MSGCZ207[] PROGMEM = { "Not SD printing" }; //MSG_SD_NOT_PRINTING
const char MSGEN208[] PROGMEM = { "error writing to file" }; //MSG_SD_ERR_WRITE_TO_FILE
const char MSGCZ208[] PROGMEM = { "error writing to file" }; //MSG_SD_ERR_WRITE_TO_FILE
const char MSGEN209[] PROGMEM = { "Cannot enter subdir: " }; //MSG_SD_CANT_ENTER_SUBDIR
const char MSGCZ209[] PROGMEM = { "Cannot enter subdir: " }; //MSG_SD_CANT_ENTER_SUBDIR
const char MSGEN210[] PROGMEM = { "Steprate too high: " }; //MSG_STEPPER_TOO_HIGH
const char MSGCZ210[] PROGMEM = { "Steprate too high: " }; //MSG_STEPPER_TOO_HIGH
const char MSGEN211[] PROGMEM = { "endstops hit: " }; //MSG_ENDSTOPS_HIT
const char MSGCZ211[] PROGMEM = { "endstops hit: " }; //MSG_ENDSTOPS_HIT
const char MSGEN212[] PROGMEM = { " cold extrusion prevented" }; //MSG_ERR_COLD_EXTRUDE_STOP
const char MSGCZ212[] PROGMEM = { " cold extrusion prevented" }; //MSG_ERR_COLD_EXTRUDE_STOP
const char MSGEN213[] PROGMEM = { " too long extrusion prevented" }; //MSG_ERR_LONG_EXTRUDE_STOP
const char MSGCZ213[] PROGMEM = { " too long extrusion prevented" }; //MSG_ERR_LONG_EXTRUDE_STOP
const char MSGEN214[] PROGMEM = { "Babystepping X" }; //MSG_BABYSTEPPING_X
const char MSGCZ214[] PROGMEM = { "Babystepping X" }; //MSG_BABYSTEPPING_X
const char MSGEN215[] PROGMEM = { "Babystepping Y" }; //MSG_BABYSTEPPING_Y
const char MSGCZ215[] PROGMEM = { "Babystepping Y" }; //MSG_BABYSTEPPING_Y
const char MSGEN216[] PROGMEM = { "Adjusting Z" }; //MSG_BABYSTEPPING_Z
const char MSGCZ216[] PROGMEM = { "Dostavovani Z" }; //MSG_BABYSTEPPING_Z
const char MSGEN217[] PROGMEM = { "Error in menu structure" }; //MSG_SERIAL_ERROR_MENU_STRUCTURE
const char MSGCZ217[] PROGMEM = { "Error in menu structure" }; //MSG_SERIAL_ERROR_MENU_STRUCTURE
const char MSGEN218[] PROGMEM = { "English" }; //MSG_LANGUAGE_NAME
const char MSGCZ218[] PROGMEM = { "Cestina" }; //MSG_LANGUAGE_NAME
const char MSGEN219[] PROGMEM = { "Select language     " }; //MSG_LANGUAGE_SELECT
const char MSGCZ219[] PROGMEM = { "Vyber jazyka        " }; //MSG_LANGUAGE_SELECT
const char MSGEN220[] PROGMEM = { "prusa3d.com" }; //MSG_PRUSA3D
const char MSGCZ220[] PROGMEM = { "prusa3d.cz" }; //MSG_PRUSA3D
const char MSGEN221[] PROGMEM = { "forum.prusa3d.com" }; //MSG_PRUSA3D_FORUM
const char MSGCZ221[] PROGMEM = { "forum.prusa3d.cz" }; //MSG_PRUSA3D_FORUM
const char MSGEN222[] PROGMEM = { "howto.prusa3d.com" }; //MSG_PRUSA3D_HOWTO
const char MSGCZ222[] PROGMEM = { "howto.prusa3d.cz" }; //MSG_PRUSA3D_HOWTO
const char* MSGEN[]  = {MSGEN0, //WELCOME_MSG
MSGEN1, //MSG_SD_INSERTED
MSGEN2, //MSG_SD_REMOVED
MSGEN3, //MSG_MAIN
MSGEN4, //MSG_AUTOSTART
MSGEN5, //MSG_DISABLE_STEPPERS
MSGEN6, //MSG_AUTO_HOME
MSGEN7, //MSG_SET_HOME_OFFSETS
MSGEN8, //MSG_SET_ORIGIN
MSGEN9, //MSG_PREHEAT_PLA
MSGEN10, //MSG_PREHEAT_PLA0
MSGEN11, //MSG_PREHEAT_PLA1
MSGEN12, //MSG_PREHEAT_PLA2
MSGEN13, //MSG_PREHEAT_PLA012
MSGEN14, //MSG_PREHEAT_PLA_BEDONLY
MSGEN15, //MSG_PREHEAT_PLA_SETTINGS
MSGEN16, //MSG_PREHEAT_ABS
MSGEN17, //MSG_PREHEAT_ABS0
MSGEN18, //MSG_PREHEAT_ABS1
MSGEN19, //MSG_PREHEAT_ABS2
MSGEN20, //MSG_PREHEAT_ABS012
MSGEN21, //MSG_PREHEAT_ABS_BEDONLY
MSGEN22, //MSG_PREHEAT_ABS_SETTINGS
MSGEN23, //MSG_COOLDOWN
MSGEN24, //MSG_SWITCH_PS_ON
MSGEN25, //MSG_SWITCH_PS_OFF
MSGEN26, //MSG_EXTRUDE
MSGEN27, //MSG_RETRACT
MSGEN28, //MSG_MOVE_AXIS
MSGEN29, //MSG_MOVE_X
MSGEN30, //MSG_MOVE_Y
MSGEN31, //MSG_MOVE_Z
MSGEN32, //MSG_MOVE_E
MSGEN33, //MSG_MOVE_E1
MSGEN34, //MSG_MOVE_E2
MSGEN35, //MSG_MOVE_01MM
MSGEN36, //MSG_MOVE_1MM
MSGEN37, //MSG_MOVE_10MM
MSGEN38, //MSG_SPEED
MSGEN39, //MSG_NOZZLE
MSGEN40, //MSG_NOZZLE1
MSGEN41, //MSG_NOZZLE2
MSGEN42, //MSG_BED
MSGEN43, //MSG_FAN_SPEED
MSGEN44, //MSG_FLOW
MSGEN45, //MSG_FLOW0
MSGEN46, //MSG_FLOW1
MSGEN47, //MSG_FLOW2
MSGEN48, //MSG_CONTROL
MSGEN49, //MSG_MIN
MSGEN50, //MSG_MAX
MSGEN51, //MSG_FACTOR
MSGEN52, //MSG_AUTOTEMP
MSGEN53, //MSG_ON
MSGEN54, //MSG_OFF
MSGEN55, //MSG_PID_P
MSGEN56, //MSG_PID_I
MSGEN57, //MSG_PID_D
MSGEN58, //MSG_PID_C
MSGEN59, //MSG_ACC
MSGEN60, //MSG_VXY_JERK
MSGEN61, //MSG_VZ_JERK
MSGEN62, //MSG_VE_JERK
MSGEN63, //MSG_VMAX
MSGEN64, //MSG_X
MSGEN65, //MSG_Y
MSGEN66, //MSG_Z
MSGEN67, //MSG_E
MSGEN68, //MSG_VMIN
MSGEN69, //MSG_VTRAV_MIN
MSGEN70, //MSG_AMAX
MSGEN71, //MSG_A_RETRACT
MSGEN72, //MSG_XSTEPS
MSGEN73, //MSG_YSTEPS
MSGEN74, //MSG_ZSTEPS
MSGEN75, //MSG_ESTEPS
MSGEN76, //MSG_TEMPERATURE
MSGEN77, //MSG_MOTION
MSGEN78, //MSG_VOLUMETRIC
MSGEN79, //MSG_VOLUMETRIC_ENABLED
MSGEN80, //MSG_FILAMENT_SIZE_EXTRUDER_0
MSGEN81, //MSG_FILAMENT_SIZE_EXTRUDER_1
MSGEN82, //MSG_FILAMENT_SIZE_EXTRUDER_2
MSGEN83, //MSG_CONTRAST
MSGEN84, //MSG_STORE_EPROM
MSGEN85, //MSG_LOAD_EPROM
MSGEN86, //MSG_RESTORE_FAILSAFE
MSGEN87, //MSG_REFRESH
MSGEN88, //MSG_WATCH
MSGEN89, //MSG_PREPARE
MSGEN90, //MSG_TUNE
MSGEN91, //MSG_PAUSE_PRINT
MSGEN92, //MSG_RESUME_PRINT
MSGEN93, //MSG_STOP_PRINT
MSGEN94, //MSG_CARD_MENU
MSGEN95, //MSG_NO_CARD
MSGEN96, //MSG_DWELL
MSGEN97, //MSG_USERWAIT
MSGEN98, //MSG_RESUMING
MSGEN99, //MSG_PRINT_ABORTED
MSGEN100, //MSG_NO_MOVE
MSGEN101, //MSG_KILLED
MSGEN102, //MSG_STOPPED
MSGEN103, //MSG_CONTROL_RETRACT
MSGEN104, //MSG_CONTROL_RETRACT_SWAP
MSGEN105, //MSG_CONTROL_RETRACTF
MSGEN106, //MSG_CONTROL_RETRACT_ZLIFT
MSGEN107, //MSG_CONTROL_RETRACT_RECOVER
MSGEN108, //MSG_CONTROL_RETRACT_RECOVER_SWAP
MSGEN109, //MSG_CONTROL_RETRACT_RECOVERF
MSGEN110, //MSG_AUTORETRACT
MSGEN111, //MSG_FILAMENTCHANGE
MSGEN112, //MSG_INIT_SDCARD
MSGEN113, //MSG_CNG_SDCARD
MSGEN114, //MSG_ZPROBE_OUT
MSGEN115, //MSG_POSITION_UNKNOWN
MSGEN116, //MSG_ZPROBE_ZOFFSET
MSGEN117, //MSG_BABYSTEP_X
MSGEN118, //MSG_BABYSTEP_Y
MSGEN119, //MSG_BABYSTEP_Z
MSGEN120, //MSG_ENDSTOP_ABORT
MSGEN121, //MSG_ADJUSTZ
MSGEN122, //MSG_HOMEYZ
MSGEN123, //MSG_SETTINGS
MSGEN124, //MSG_PREHEAT
MSGEN125, //MSG_UNLOAD_FILAMENT
MSGEN126, //MSG_LOAD_FILAMENT
MSGEN127, //MSG_RECTRACT
MSGEN128, //MSG_ERROR
MSGEN129, //MSG_PREHEAT_NOZZLE
MSGEN130, //MSG_SUPPORT
MSGEN131, //MSG_CORRECTLY
MSGEN132, //MSG_YES
MSGEN133, //MSG_NO
MSGEN134, //MSG_NOT_LOADED
MSGEN135, //MSG_NOT_COLOR
MSGEN136, //MSG_LOADING_FILAMENT
MSGEN137, //MSG_PLEASE_WAIT
MSGEN138, //MSG_LOADING_COLOR
MSGEN139, //MSG_CHANGE_SUCCESS
MSGEN140, //MSG_PRESS
MSGEN141, //MSG_INSERT_FILAMENT
MSGEN142, //MSG_CHANGING_FILAMENT
MSGEN143, //MSG_SILENT_MODE_ON
MSGEN144, //MSG_SILENT_MODE_OFF
MSGEN145, //MSG_REBOOT
MSGEN146, //MSG_TAKE_EFFECT
MSGEN147, //MSG_Enqueing
MSGEN148, //MSG_POWERUP
MSGEN149, //MSG_EXTERNAL_RESET
MSGEN150, //MSG_BROWNOUT_RESET
MSGEN151, //MSG_WATCHDOG_RESET
MSGEN152, //MSG_SOFTWARE_RESET
MSGEN153, //MSG_AUTHOR
MSGEN154, //MSG_CONFIGURATION_VER
MSGEN155, //MSG_FREE_MEMORY
MSGEN156, //MSG_PLANNER_BUFFER_BYTES
MSGEN157, //MSG_OK
MSGEN158, //MSG_FILE_SAVED
MSGEN159, //MSG_ERR_LINE_NO
MSGEN160, //MSG_ERR_CHECKSUM_MISMATCH
MSGEN161, //MSG_ERR_NO_CHECKSUM
MSGEN162, //MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM
MSGEN163, //MSG_FILE_PRINTED
MSGEN164, //MSG_BEGIN_FILE_LIST
MSGEN165, //MSG_END_FILE_LIST
MSGEN166, //MSG_M104_INVALID_EXTRUDER
MSGEN167, //MSG_M105_INVALID_EXTRUDER
MSGEN168, //MSG_M200_INVALID_EXTRUDER
MSGEN169, //MSG_M218_INVALID_EXTRUDER
MSGEN170, //MSG_M221_INVALID_EXTRUDER
MSGEN171, //MSG_ERR_NO_THERMISTORS
MSGEN172, //MSG_M109_INVALID_EXTRUDER
MSGEN173, //MSG_HEATING
MSGEN174, //MSG_HEATING_COMPLETE
MSGEN175, //MSG_BED_HEATING
MSGEN176, //MSG_BED_DONE
MSGEN177, //MSG_M115_REPORT
MSGEN178, //MSG_COUNT_X
MSGEN179, //MSG_ERR_KILLED
MSGEN180, //MSG_ERR_STOPPED
MSGEN181, //MSG_RESEND
MSGEN182, //MSG_UNKNOWN_COMMAND
MSGEN183, //MSG_ACTIVE_EXTRUDER
MSGEN184, //MSG_INVALID_EXTRUDER
MSGEN185, //MSG_X_MIN
MSGEN186, //MSG_X_MAX
MSGEN187, //MSG_Y_MIN
MSGEN188, //MSG_Y_MAX
MSGEN189, //MSG_Z_MIN
MSGEN190, //MSG_Z_MAX
MSGEN191, //MSG_M119_REPORT
MSGEN192, //MSG_ENDSTOP_HIT
MSGEN193, //MSG_ENDSTOP_OPEN
MSGEN194, //MSG_HOTEND_OFFSET
MSGEN195, //MSG_SD_CANT_OPEN_SUBDIR
MSGEN196, //MSG_SD_INIT_FAIL
MSGEN197, //MSG_SD_VOL_INIT_FAIL
MSGEN198, //MSG_SD_OPENROOT_FAIL
MSGEN199, //MSG_SD_CARD_OK
MSGEN200, //MSG_SD_WORKDIR_FAIL
MSGEN201, //MSG_SD_OPEN_FILE_FAIL
MSGEN202, //MSG_SD_FILE_OPENED
MSGEN203, //MSG_SD_SIZE
MSGEN204, //MSG_SD_FILE_SELECTED
MSGEN205, //MSG_SD_WRITE_TO_FILE
MSGEN206, //MSG_SD_PRINTING_BYTE
MSGEN207, //MSG_SD_NOT_PRINTING
MSGEN208, //MSG_SD_ERR_WRITE_TO_FILE
MSGEN209, //MSG_SD_CANT_ENTER_SUBDIR
MSGEN210, //MSG_STEPPER_TOO_HIGH
MSGEN211, //MSG_ENDSTOPS_HIT
MSGEN212, //MSG_ERR_COLD_EXTRUDE_STOP
MSGEN213, //MSG_ERR_LONG_EXTRUDE_STOP
MSGEN214, //MSG_BABYSTEPPING_X
MSGEN215, //MSG_BABYSTEPPING_Y
MSGEN216, //MSG_BABYSTEPPING_Z
MSGEN217, //MSG_SERIAL_ERROR_MENU_STRUCTURE
MSGEN218, //MSG_LANGUAGE_NAME
MSGEN219, //MSG_LANGUAGE_SELECT
MSGEN220, //MSG_PRUSA3D
MSGEN221, //MSG_PRUSA3D_FORUM
MSGEN222, //MSG_PRUSA3D_HOWTO
};
const char* MSGCZ[]  = {MSGCZ0, //WELCOME_MSG
MSGCZ1, //MSG_SD_INSERTED
MSGCZ2, //MSG_SD_REMOVED
MSGCZ3, //MSG_MAIN
MSGCZ4, //MSG_AUTOSTART
MSGCZ5, //MSG_DISABLE_STEPPERS
MSGCZ6, //MSG_AUTO_HOME
MSGCZ7, //MSG_SET_HOME_OFFSETS
MSGCZ8, //MSG_SET_ORIGIN
MSGCZ9, //MSG_PREHEAT_PLA
MSGCZ10, //MSG_PREHEAT_PLA0
MSGCZ11, //MSG_PREHEAT_PLA1
MSGCZ12, //MSG_PREHEAT_PLA2
MSGCZ13, //MSG_PREHEAT_PLA012
MSGCZ14, //MSG_PREHEAT_PLA_BEDONLY
MSGCZ15, //MSG_PREHEAT_PLA_SETTINGS
MSGCZ16, //MSG_PREHEAT_ABS
MSGCZ17, //MSG_PREHEAT_ABS0
MSGCZ18, //MSG_PREHEAT_ABS1
MSGCZ19, //MSG_PREHEAT_ABS2
MSGCZ20, //MSG_PREHEAT_ABS012
MSGCZ21, //MSG_PREHEAT_ABS_BEDONLY
MSGCZ22, //MSG_PREHEAT_ABS_SETTINGS
MSGCZ23, //MSG_COOLDOWN
MSGCZ24, //MSG_SWITCH_PS_ON
MSGCZ25, //MSG_SWITCH_PS_OFF
MSGCZ26, //MSG_EXTRUDE
MSGCZ27, //MSG_RETRACT
MSGCZ28, //MSG_MOVE_AXIS
MSGCZ29, //MSG_MOVE_X
MSGCZ30, //MSG_MOVE_Y
MSGCZ31, //MSG_MOVE_Z
MSGCZ32, //MSG_MOVE_E
MSGCZ33, //MSG_MOVE_E1
MSGCZ34, //MSG_MOVE_E2
MSGCZ35, //MSG_MOVE_01MM
MSGCZ36, //MSG_MOVE_1MM
MSGCZ37, //MSG_MOVE_10MM
MSGCZ38, //MSG_SPEED
MSGCZ39, //MSG_NOZZLE
MSGCZ40, //MSG_NOZZLE1
MSGCZ41, //MSG_NOZZLE2
MSGCZ42, //MSG_BED
MSGCZ43, //MSG_FAN_SPEED
MSGCZ44, //MSG_FLOW
MSGCZ45, //MSG_FLOW0
MSGCZ46, //MSG_FLOW1
MSGCZ47, //MSG_FLOW2
MSGCZ48, //MSG_CONTROL
MSGCZ49, //MSG_MIN
MSGCZ50, //MSG_MAX
MSGCZ51, //MSG_FACTOR
MSGCZ52, //MSG_AUTOTEMP
MSGCZ53, //MSG_ON
MSGCZ54, //MSG_OFF
MSGCZ55, //MSG_PID_P
MSGCZ56, //MSG_PID_I
MSGCZ57, //MSG_PID_D
MSGCZ58, //MSG_PID_C
MSGCZ59, //MSG_ACC
MSGCZ60, //MSG_VXY_JERK
MSGCZ61, //MSG_VZ_JERK
MSGCZ62, //MSG_VE_JERK
MSGCZ63, //MSG_VMAX
MSGCZ64, //MSG_X
MSGCZ65, //MSG_Y
MSGCZ66, //MSG_Z
MSGCZ67, //MSG_E
MSGCZ68, //MSG_VMIN
MSGCZ69, //MSG_VTRAV_MIN
MSGCZ70, //MSG_AMAX
MSGCZ71, //MSG_A_RETRACT
MSGCZ72, //MSG_XSTEPS
MSGCZ73, //MSG_YSTEPS
MSGCZ74, //MSG_ZSTEPS
MSGCZ75, //MSG_ESTEPS
MSGCZ76, //MSG_TEMPERATURE
MSGCZ77, //MSG_MOTION
MSGCZ78, //MSG_VOLUMETRIC
MSGCZ79, //MSG_VOLUMETRIC_ENABLED
MSGCZ80, //MSG_FILAMENT_SIZE_EXTRUDER_0
MSGCZ81, //MSG_FILAMENT_SIZE_EXTRUDER_1
MSGCZ82, //MSG_FILAMENT_SIZE_EXTRUDER_2
MSGCZ83, //MSG_CONTRAST
MSGCZ84, //MSG_STORE_EPROM
MSGCZ85, //MSG_LOAD_EPROM
MSGCZ86, //MSG_RESTORE_FAILSAFE
MSGCZ87, //MSG_REFRESH
MSGCZ88, //MSG_WATCH
MSGCZ89, //MSG_PREPARE
MSGCZ90, //MSG_TUNE
MSGCZ91, //MSG_PAUSE_PRINT
MSGCZ92, //MSG_RESUME_PRINT
MSGCZ93, //MSG_STOP_PRINT
MSGCZ94, //MSG_CARD_MENU
MSGCZ95, //MSG_NO_CARD
MSGCZ96, //MSG_DWELL
MSGCZ97, //MSG_USERWAIT
MSGCZ98, //MSG_RESUMING
MSGCZ99, //MSG_PRINT_ABORTED
MSGCZ100, //MSG_NO_MOVE
MSGCZ101, //MSG_KILLED
MSGCZ102, //MSG_STOPPED
MSGCZ103, //MSG_CONTROL_RETRACT
MSGCZ104, //MSG_CONTROL_RETRACT_SWAP
MSGCZ105, //MSG_CONTROL_RETRACTF
MSGCZ106, //MSG_CONTROL_RETRACT_ZLIFT
MSGCZ107, //MSG_CONTROL_RETRACT_RECOVER
MSGCZ108, //MSG_CONTROL_RETRACT_RECOVER_SWAP
MSGCZ109, //MSG_CONTROL_RETRACT_RECOVERF
MSGCZ110, //MSG_AUTORETRACT
MSGCZ111, //MSG_FILAMENTCHANGE
MSGCZ112, //MSG_INIT_SDCARD
MSGCZ113, //MSG_CNG_SDCARD
MSGCZ114, //MSG_ZPROBE_OUT
MSGCZ115, //MSG_POSITION_UNKNOWN
MSGCZ116, //MSG_ZPROBE_ZOFFSET
MSGCZ117, //MSG_BABYSTEP_X
MSGCZ118, //MSG_BABYSTEP_Y
MSGCZ119, //MSG_BABYSTEP_Z
MSGCZ120, //MSG_ENDSTOP_ABORT
MSGCZ121, //MSG_ADJUSTZ
MSGCZ122, //MSG_HOMEYZ
MSGCZ123, //MSG_SETTINGS
MSGCZ124, //MSG_PREHEAT
MSGCZ125, //MSG_UNLOAD_FILAMENT
MSGCZ126, //MSG_LOAD_FILAMENT
MSGCZ127, //MSG_RECTRACT
MSGCZ128, //MSG_ERROR
MSGCZ129, //MSG_PREHEAT_NOZZLE
MSGCZ130, //MSG_SUPPORT
MSGCZ131, //MSG_CORRECTLY
MSGCZ132, //MSG_YES
MSGCZ133, //MSG_NO
MSGCZ134, //MSG_NOT_LOADED
MSGCZ135, //MSG_NOT_COLOR
MSGCZ136, //MSG_LOADING_FILAMENT
MSGCZ137, //MSG_PLEASE_WAIT
MSGCZ138, //MSG_LOADING_COLOR
MSGCZ139, //MSG_CHANGE_SUCCESS
MSGCZ140, //MSG_PRESS
MSGCZ141, //MSG_INSERT_FILAMENT
MSGCZ142, //MSG_CHANGING_FILAMENT
MSGCZ143, //MSG_SILENT_MODE_ON
MSGCZ144, //MSG_SILENT_MODE_OFF
MSGCZ145, //MSG_REBOOT
MSGCZ146, //MSG_TAKE_EFFECT
MSGCZ147, //MSG_Enqueing
MSGCZ148, //MSG_POWERUP
MSGCZ149, //MSG_EXTERNAL_RESET
MSGCZ150, //MSG_BROWNOUT_RESET
MSGCZ151, //MSG_WATCHDOG_RESET
MSGCZ152, //MSG_SOFTWARE_RESET
MSGCZ153, //MSG_AUTHOR
MSGCZ154, //MSG_CONFIGURATION_VER
MSGCZ155, //MSG_FREE_MEMORY
MSGCZ156, //MSG_PLANNER_BUFFER_BYTES
MSGCZ157, //MSG_OK
MSGCZ158, //MSG_FILE_SAVED
MSGCZ159, //MSG_ERR_LINE_NO
MSGCZ160, //MSG_ERR_CHECKSUM_MISMATCH
MSGCZ161, //MSG_ERR_NO_CHECKSUM
MSGCZ162, //MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM
MSGCZ163, //MSG_FILE_PRINTED
MSGCZ164, //MSG_BEGIN_FILE_LIST
MSGCZ165, //MSG_END_FILE_LIST
MSGCZ166, //MSG_M104_INVALID_EXTRUDER
MSGCZ167, //MSG_M105_INVALID_EXTRUDER
MSGCZ168, //MSG_M200_INVALID_EXTRUDER
MSGCZ169, //MSG_M218_INVALID_EXTRUDER
MSGCZ170, //MSG_M221_INVALID_EXTRUDER
MSGCZ171, //MSG_ERR_NO_THERMISTORS
MSGCZ172, //MSG_M109_INVALID_EXTRUDER
MSGCZ173, //MSG_HEATING
MSGCZ174, //MSG_HEATING_COMPLETE
MSGCZ175, //MSG_BED_HEATING
MSGCZ176, //MSG_BED_DONE
MSGCZ177, //MSG_M115_REPORT
MSGCZ178, //MSG_COUNT_X
MSGCZ179, //MSG_ERR_KILLED
MSGCZ180, //MSG_ERR_STOPPED
MSGCZ181, //MSG_RESEND
MSGCZ182, //MSG_UNKNOWN_COMMAND
MSGCZ183, //MSG_ACTIVE_EXTRUDER
MSGCZ184, //MSG_INVALID_EXTRUDER
MSGCZ185, //MSG_X_MIN
MSGCZ186, //MSG_X_MAX
MSGCZ187, //MSG_Y_MIN
MSGCZ188, //MSG_Y_MAX
MSGCZ189, //MSG_Z_MIN
MSGCZ190, //MSG_Z_MAX
MSGCZ191, //MSG_M119_REPORT
MSGCZ192, //MSG_ENDSTOP_HIT
MSGCZ193, //MSG_ENDSTOP_OPEN
MSGCZ194, //MSG_HOTEND_OFFSET
MSGCZ195, //MSG_SD_CANT_OPEN_SUBDIR
MSGCZ196, //MSG_SD_INIT_FAIL
MSGCZ197, //MSG_SD_VOL_INIT_FAIL
MSGCZ198, //MSG_SD_OPENROOT_FAIL
MSGCZ199, //MSG_SD_CARD_OK
MSGCZ200, //MSG_SD_WORKDIR_FAIL
MSGCZ201, //MSG_SD_OPEN_FILE_FAIL
MSGCZ202, //MSG_SD_FILE_OPENED
MSGCZ203, //MSG_SD_SIZE
MSGCZ204, //MSG_SD_FILE_SELECTED
MSGCZ205, //MSG_SD_WRITE_TO_FILE
MSGCZ206, //MSG_SD_PRINTING_BYTE
MSGCZ207, //MSG_SD_NOT_PRINTING
MSGCZ208, //MSG_SD_ERR_WRITE_TO_FILE
MSGCZ209, //MSG_SD_CANT_ENTER_SUBDIR
MSGCZ210, //MSG_STEPPER_TOO_HIGH
MSGCZ211, //MSG_ENDSTOPS_HIT
MSGCZ212, //MSG_ERR_COLD_EXTRUDE_STOP
MSGCZ213, //MSG_ERR_LONG_EXTRUDE_STOP
MSGCZ214, //MSG_BABYSTEPPING_X
MSGCZ215, //MSG_BABYSTEPPING_Y
MSGCZ216, //MSG_BABYSTEPPING_Z
MSGCZ217, //MSG_SERIAL_ERROR_MENU_STRUCTURE
MSGCZ218, //MSG_LANGUAGE_NAME
MSGCZ219, //MSG_LANGUAGE_SELECT
MSGCZ220, //MSG_PRUSA3D
MSGCZ221, //MSG_PRUSA3D_FORUM
MSGCZ222, //MSG_PRUSA3D_HOWTO
};


const char** MSG_ALL[] = {MSGEN,MSGCZ};
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