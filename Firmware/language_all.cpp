#include <avr/pgmspace.h>
#define LCD_WIDTH 20
extern unsigned char lang_selected;
const char MSGEN0[] PROGMEM = { "Prusa i3 ready." }; //WELCOME_MSG
const char MSGCZ0[] PROGMEM = { "Prusa i3 pripravena" }; //WELCOME_MSG
const char MSGIT0[] PROGMEM = { "Prusa i3 pronto." }; //WELCOME_MSG
const char MSGES0[] PROGMEM = { "Prusa i3 lista" }; //WELCOME_MSG
const char MSGPL0[] PROGMEM = { "Prusa i3 gotowa" }; //WELCOME_MSG
const char MSGEN1[] PROGMEM = { "Card inserted" }; //MSG_SD_INSERTED
const char MSGCZ1[] PROGMEM = { "Karta vlozena" }; //MSG_SD_INSERTED
const char MSGIT1[] PROGMEM = { "SD Card inserita" }; //MSG_SD_INSERTED
const char MSGES1[] PROGMEM = { "Tarjeta colocada" }; //MSG_SD_INSERTED
const char MSGPL1[] PROGMEM = { "Karta wlozona" }; //MSG_SD_INSERTED
const char MSGEN2[] PROGMEM = { "Card removed" }; //MSG_SD_REMOVED
const char MSGCZ2[] PROGMEM = { "Karta vyjmuta" }; //MSG_SD_REMOVED
const char MSGIT2[] PROGMEM = { "SD Card rimossa" }; //MSG_SD_REMOVED
const char MSGES2[] PROGMEM = { "Tarjeta retirada" }; //MSG_SD_REMOVED
const char MSGPL2[] PROGMEM = { "Karta wyjeta" }; //MSG_SD_REMOVED
const char MSGEN3[] PROGMEM = { "Main" }; //MSG_MAIN
const char MSGCZ3[] PROGMEM = { "Hlavni nabidka" }; //MSG_MAIN
const char MSGIT3[] PROGMEM = { "Menu principale" }; //MSG_MAIN
const char MSGES3[] PROGMEM = { "Menu principal" }; //MSG_MAIN
const char MSGPL3[] PROGMEM = { "Menu glowne" }; //MSG_MAIN
const char MSGEN4[] PROGMEM = { "Autostart" }; //MSG_AUTOSTART
const char MSGCZ4[] PROGMEM = { "Autostart" }; //MSG_AUTOSTART
const char MSGIT4[] PROGMEM = { "Autostart" }; //MSG_AUTOSTART
const char MSGES4[] PROGMEM = { "Autostart" }; //MSG_AUTOSTART
const char MSGPL4[] PROGMEM = { "Autostart" }; //MSG_AUTOSTART
const char MSGEN5[] PROGMEM = { "Disable steppers" }; //MSG_DISABLE_STEPPERS
const char MSGCZ5[] PROGMEM = { "Vypnout motory" }; //MSG_DISABLE_STEPPERS
const char MSGIT5[] PROGMEM = { "Disabilita Motori" }; //MSG_DISABLE_STEPPERS
const char MSGES5[] PROGMEM = { "Apagar motores" }; //MSG_DISABLE_STEPPERS
const char MSGPL5[] PROGMEM = { "Wylaczyc silniki" }; //MSG_DISABLE_STEPPERS
const char MSGEN6[] PROGMEM = { "Auto home" }; //MSG_AUTO_HOME
const char MSGCZ6[] PROGMEM = { "Auto home" }; //MSG_AUTO_HOME
const char MSGIT6[] PROGMEM = { "Auto Home" }; //MSG_AUTO_HOME
const char MSGES6[] PROGMEM = { "Llevar al origen" }; //MSG_AUTO_HOME
const char MSGPL6[] PROGMEM = { "Auto home" }; //MSG_AUTO_HOME
const char MSGEN7[] PROGMEM = { "Set home offsets" }; //MSG_SET_HOME_OFFSETS
const char MSGCZ7[] PROGMEM = { "Nastav pocatek home" }; //MSG_SET_HOME_OFFSETS
const char MSGIT7[] PROGMEM = { "Set home offsets" }; //MSG_SET_HOME_OFFSETS
const char MSGES7[] PROGMEM = { "Set home offsets" }; //MSG_SET_HOME_OFFSETS
const char MSGPL7[] PROGMEM = { "Nastav pocatek home" }; //MSG_SET_HOME_OFFSETS
const char MSGEN8[] PROGMEM = { "Set origin" }; //MSG_SET_ORIGIN
const char MSGCZ8[] PROGMEM = { "Nastav pocatek" }; //MSG_SET_ORIGIN
const char MSGIT8[] PROGMEM = { "Set origin" }; //MSG_SET_ORIGIN
const char MSGES8[] PROGMEM = { "Set origin" }; //MSG_SET_ORIGIN
const char MSGPL8[] PROGMEM = { "Nastav pocatek" }; //MSG_SET_ORIGIN
const char MSGEN9[] PROGMEM = { "Preheat PLA" }; //MSG_PREHEAT_PLA
const char MSGCZ9[] PROGMEM = { "Predehrev PLA" }; //MSG_PREHEAT_PLA
const char MSGIT9[] PROGMEM = { "Preheat PLA" }; //MSG_PREHEAT_PLA
const char MSGES9[] PROGMEM = { "Preheat PLA" }; //MSG_PREHEAT_PLA
const char MSGPL9[] PROGMEM = { "Predehrev PLA" }; //MSG_PREHEAT_PLA
const char MSGEN10[] PROGMEM = { "Preheat PLA 1" }; //MSG_PREHEAT_PLA0
const char MSGCZ10[] PROGMEM = { "Predehrev PLA 1" }; //MSG_PREHEAT_PLA0
const char MSGIT10[] PROGMEM = { "Preheat PLA 1" }; //MSG_PREHEAT_PLA0
const char MSGES10[] PROGMEM = { "Preheat PLA 1" }; //MSG_PREHEAT_PLA0
const char MSGPL10[] PROGMEM = { "Predehrev PLA 1" }; //MSG_PREHEAT_PLA0
const char MSGEN11[] PROGMEM = { "Preheat PLA 2" }; //MSG_PREHEAT_PLA1
const char MSGCZ11[] PROGMEM = { "Predehrev PLA 2" }; //MSG_PREHEAT_PLA1
const char MSGIT11[] PROGMEM = { "Preheat PLA 2" }; //MSG_PREHEAT_PLA1
const char MSGES11[] PROGMEM = { "Preheat PLA 2" }; //MSG_PREHEAT_PLA1
const char MSGPL11[] PROGMEM = { "Predehrev PLA 2" }; //MSG_PREHEAT_PLA1
const char MSGEN12[] PROGMEM = { "Preheat PLA 3" }; //MSG_PREHEAT_PLA2
const char MSGCZ12[] PROGMEM = { "Predehrev PLA 3" }; //MSG_PREHEAT_PLA2
const char MSGIT12[] PROGMEM = { "Preheat PLA 3" }; //MSG_PREHEAT_PLA2
const char MSGES12[] PROGMEM = { "Preheat PLA 3" }; //MSG_PREHEAT_PLA2
const char MSGPL12[] PROGMEM = { "Predehrev PLA 3" }; //MSG_PREHEAT_PLA2
const char MSGEN13[] PROGMEM = { "Preheat PLA All" }; //MSG_PREHEAT_PLA012
const char MSGCZ13[] PROGMEM = { "Predehrev PLA All" }; //MSG_PREHEAT_PLA012
const char MSGIT13[] PROGMEM = { "Preheat PLA All" }; //MSG_PREHEAT_PLA012
const char MSGES13[] PROGMEM = { "Preheat PLA All" }; //MSG_PREHEAT_PLA012
const char MSGPL13[] PROGMEM = { "Predehrev PLA All" }; //MSG_PREHEAT_PLA012
const char MSGEN14[] PROGMEM = { "Preheat PLA Bed" }; //MSG_PREHEAT_PLA_BEDONLY
const char MSGCZ14[] PROGMEM = { "Predehrev PLA Bed" }; //MSG_PREHEAT_PLA_BEDONLY
const char MSGIT14[] PROGMEM = { "Preheat PLA Bed" }; //MSG_PREHEAT_PLA_BEDONLY
const char MSGES14[] PROGMEM = { "Preheat PLA Bed" }; //MSG_PREHEAT_PLA_BEDONLY
const char MSGPL14[] PROGMEM = { "Predehrev PLA Bed" }; //MSG_PREHEAT_PLA_BEDONLY
const char MSGEN15[] PROGMEM = { "Preheat PLA conf" }; //MSG_PREHEAT_PLA_SETTINGS
const char MSGCZ15[] PROGMEM = { "Predehrev PLA conf" }; //MSG_PREHEAT_PLA_SETTINGS
const char MSGIT15[] PROGMEM = { "Preheat PLA conf" }; //MSG_PREHEAT_PLA_SETTINGS
const char MSGES15[] PROGMEM = { "Preheat PLA conf" }; //MSG_PREHEAT_PLA_SETTINGS
const char MSGPL15[] PROGMEM = { "Predehrev PLA conf" }; //MSG_PREHEAT_PLA_SETTINGS
const char MSGEN16[] PROGMEM = { "Preheat ABS" }; //MSG_PREHEAT_ABS
const char MSGCZ16[] PROGMEM = { "Predehrev ABS" }; //MSG_PREHEAT_ABS
const char MSGIT16[] PROGMEM = { "Preheat ABS" }; //MSG_PREHEAT_ABS
const char MSGES16[] PROGMEM = { "Preheat ABS" }; //MSG_PREHEAT_ABS
const char MSGPL16[] PROGMEM = { "Predehrev ABS" }; //MSG_PREHEAT_ABS
const char MSGEN17[] PROGMEM = { "Preheat ABS 1" }; //MSG_PREHEAT_ABS0
const char MSGCZ17[] PROGMEM = { "Predehrev ABS 1" }; //MSG_PREHEAT_ABS0
const char MSGIT17[] PROGMEM = { "Preheat ABS 1" }; //MSG_PREHEAT_ABS0
const char MSGES17[] PROGMEM = { "Preheat ABS 1" }; //MSG_PREHEAT_ABS0
const char MSGPL17[] PROGMEM = { "Predehrev ABS 1" }; //MSG_PREHEAT_ABS0
const char MSGEN18[] PROGMEM = { "Preheat ABS 2" }; //MSG_PREHEAT_ABS1
const char MSGCZ18[] PROGMEM = { "Predehrev ABS 2" }; //MSG_PREHEAT_ABS1
const char MSGIT18[] PROGMEM = { "Preheat ABS 2" }; //MSG_PREHEAT_ABS1
const char MSGES18[] PROGMEM = { "Preheat ABS 2" }; //MSG_PREHEAT_ABS1
const char MSGPL18[] PROGMEM = { "Predehrev ABS 2" }; //MSG_PREHEAT_ABS1
const char MSGEN19[] PROGMEM = { "Preheat ABS 3" }; //MSG_PREHEAT_ABS2
const char MSGCZ19[] PROGMEM = { "Predehrev ABS 3" }; //MSG_PREHEAT_ABS2
const char MSGIT19[] PROGMEM = { "Preheat ABS 3" }; //MSG_PREHEAT_ABS2
const char MSGES19[] PROGMEM = { "Preheat ABS 3" }; //MSG_PREHEAT_ABS2
const char MSGPL19[] PROGMEM = { "Predehrev ABS 3" }; //MSG_PREHEAT_ABS2
const char MSGEN20[] PROGMEM = { "Preheat ABS All" }; //MSG_PREHEAT_ABS012
const char MSGCZ20[] PROGMEM = { "Predehrev ABS All" }; //MSG_PREHEAT_ABS012
const char MSGIT20[] PROGMEM = { "Preheat ABS All" }; //MSG_PREHEAT_ABS012
const char MSGES20[] PROGMEM = { "Preheat ABS All" }; //MSG_PREHEAT_ABS012
const char MSGPL20[] PROGMEM = { "Predehrev ABS All" }; //MSG_PREHEAT_ABS012
const char MSGEN21[] PROGMEM = { "Preheat ABS Bed" }; //MSG_PREHEAT_ABS_BEDONLY
const char MSGCZ21[] PROGMEM = { "Predehrev ABS Bed" }; //MSG_PREHEAT_ABS_BEDONLY
const char MSGIT21[] PROGMEM = { "Preheat ABS Bed" }; //MSG_PREHEAT_ABS_BEDONLY
const char MSGES21[] PROGMEM = { "Preheat ABS Bed" }; //MSG_PREHEAT_ABS_BEDONLY
const char MSGPL21[] PROGMEM = { "Predehrev ABS Bed" }; //MSG_PREHEAT_ABS_BEDONLY
const char MSGEN22[] PROGMEM = { "Preheat ABS conf" }; //MSG_PREHEAT_ABS_SETTINGS
const char MSGCZ22[] PROGMEM = { "Predehrev ABS conf" }; //MSG_PREHEAT_ABS_SETTINGS
const char MSGIT22[] PROGMEM = { "Preheat ABS conf" }; //MSG_PREHEAT_ABS_SETTINGS
const char MSGES22[] PROGMEM = { "Preheat ABS conf" }; //MSG_PREHEAT_ABS_SETTINGS
const char MSGPL22[] PROGMEM = { "Predehrev ABS conf" }; //MSG_PREHEAT_ABS_SETTINGS
const char MSGEN23[] PROGMEM = { "Cooldown" }; //MSG_COOLDOWN
const char MSGCZ23[] PROGMEM = { "Zchladit" }; //MSG_COOLDOWN
const char MSGIT23[] PROGMEM = { "Raffredda" }; //MSG_COOLDOWN
const char MSGES23[] PROGMEM = { "Enfriar" }; //MSG_COOLDOWN
const char MSGPL23[] PROGMEM = { "Wychlodzic" }; //MSG_COOLDOWN
const char MSGEN24[] PROGMEM = { "Switch power on" }; //MSG_SWITCH_PS_ON
const char MSGCZ24[] PROGMEM = { "Vypnout zdroj" }; //MSG_SWITCH_PS_ON
const char MSGIT24[] PROGMEM = { "Switch power on" }; //MSG_SWITCH_PS_ON
const char MSGES24[] PROGMEM = { "Switch power on" }; //MSG_SWITCH_PS_ON
const char MSGPL24[] PROGMEM = { "Vypnout zdroj" }; //MSG_SWITCH_PS_ON
const char MSGEN25[] PROGMEM = { "Switch power off" }; //MSG_SWITCH_PS_OFF
const char MSGCZ25[] PROGMEM = { "Zapnout zdroj" }; //MSG_SWITCH_PS_OFF
const char MSGIT25[] PROGMEM = { "Switch power off" }; //MSG_SWITCH_PS_OFF
const char MSGES25[] PROGMEM = { "Switch power off" }; //MSG_SWITCH_PS_OFF
const char MSGPL25[] PROGMEM = { "Zapnout zdroj" }; //MSG_SWITCH_PS_OFF
const char MSGEN26[] PROGMEM = { "Extrude" }; //MSG_EXTRUDE
const char MSGCZ26[] PROGMEM = { "Extrudovat" }; //MSG_EXTRUDE
const char MSGIT26[] PROGMEM = { "Extrude" }; //MSG_EXTRUDE
const char MSGES26[] PROGMEM = { "Extrude" }; //MSG_EXTRUDE
const char MSGPL26[] PROGMEM = { "Extrudovat" }; //MSG_EXTRUDE
const char MSGEN27[] PROGMEM = { "Retract" }; //MSG_RETRACT
const char MSGCZ27[] PROGMEM = { "Retract" }; //MSG_RETRACT
const char MSGIT27[] PROGMEM = { "Retract" }; //MSG_RETRACT
const char MSGES27[] PROGMEM = { "Retract" }; //MSG_RETRACT
const char MSGPL27[] PROGMEM = { "Retract" }; //MSG_RETRACT
const char MSGEN28[] PROGMEM = { "Move axis" }; //MSG_MOVE_AXIS
const char MSGCZ28[] PROGMEM = { "Posunout osu" }; //MSG_MOVE_AXIS
const char MSGIT28[] PROGMEM = { "Muovi Asse" }; //MSG_MOVE_AXIS
const char MSGES28[] PROGMEM = { "Mover ejes" }; //MSG_MOVE_AXIS
const char MSGPL28[] PROGMEM = { "Ruch osi" }; //MSG_MOVE_AXIS
const char MSGEN29[] PROGMEM = { "Move X" }; //MSG_MOVE_X
const char MSGCZ29[] PROGMEM = { "Posunout X" }; //MSG_MOVE_X
const char MSGIT29[] PROGMEM = { "Muovi X" }; //MSG_MOVE_X
const char MSGES29[] PROGMEM = { "Mover X" }; //MSG_MOVE_X
const char MSGPL29[] PROGMEM = { "Przesunac X" }; //MSG_MOVE_X
const char MSGEN30[] PROGMEM = { "Move Y" }; //MSG_MOVE_Y
const char MSGCZ30[] PROGMEM = { "Posunout Y" }; //MSG_MOVE_Y
const char MSGIT30[] PROGMEM = { "Muovi Y" }; //MSG_MOVE_Y
const char MSGES30[] PROGMEM = { "Mover Y" }; //MSG_MOVE_Y
const char MSGPL30[] PROGMEM = { "Przesunac Y" }; //MSG_MOVE_Y
const char MSGEN31[] PROGMEM = { "Move Z" }; //MSG_MOVE_Z
const char MSGCZ31[] PROGMEM = { "Posunout Z" }; //MSG_MOVE_Z
const char MSGIT31[] PROGMEM = { "Muovi Z" }; //MSG_MOVE_Z
const char MSGES31[] PROGMEM = { "Mover Z" }; //MSG_MOVE_Z
const char MSGPL31[] PROGMEM = { "Przesunac Z" }; //MSG_MOVE_Z
const char MSGEN32[] PROGMEM = { "Extruder" }; //MSG_MOVE_E
const char MSGCZ32[] PROGMEM = { "Extruder" }; //MSG_MOVE_E
const char MSGIT32[] PROGMEM = { "Estrusore" }; //MSG_MOVE_E
const char MSGES32[] PROGMEM = { "Extrusor" }; //MSG_MOVE_E
const char MSGPL32[] PROGMEM = { "Extruder" }; //MSG_MOVE_E
const char MSGEN33[] PROGMEM = { "Extruder2" }; //MSG_MOVE_E1
const char MSGCZ33[] PROGMEM = { "Extruder2" }; //MSG_MOVE_E1
const char MSGIT33[] PROGMEM = { "Extruder2" }; //MSG_MOVE_E1
const char MSGES33[] PROGMEM = { "Extruder2" }; //MSG_MOVE_E1
const char MSGPL33[] PROGMEM = { "Extruder2" }; //MSG_MOVE_E1
const char MSGEN34[] PROGMEM = { "Extruder3" }; //MSG_MOVE_E2
const char MSGCZ34[] PROGMEM = { "Extruder3" }; //MSG_MOVE_E2
const char MSGIT34[] PROGMEM = { "Extruder3" }; //MSG_MOVE_E2
const char MSGES34[] PROGMEM = { "Extruder3" }; //MSG_MOVE_E2
const char MSGPL34[] PROGMEM = { "Extruder3" }; //MSG_MOVE_E2
const char MSGEN35[] PROGMEM = { "Move 0.1mm" }; //MSG_MOVE_01MM
const char MSGCZ35[] PROGMEM = { "Posunout o 0.1mm" }; //MSG_MOVE_01MM
const char MSGIT35[] PROGMEM = { "Move 0.1mm" }; //MSG_MOVE_01MM
const char MSGES35[] PROGMEM = { "Move 0.1mm" }; //MSG_MOVE_01MM
const char MSGPL35[] PROGMEM = { "Posunout o 0.1mm" }; //MSG_MOVE_01MM
const char MSGEN36[] PROGMEM = { "Move 1mm" }; //MSG_MOVE_1MM
const char MSGCZ36[] PROGMEM = { "Posunout o 1mm" }; //MSG_MOVE_1MM
const char MSGIT36[] PROGMEM = { "Move 1mm" }; //MSG_MOVE_1MM
const char MSGES36[] PROGMEM = { "Move 1mm" }; //MSG_MOVE_1MM
const char MSGPL36[] PROGMEM = { "Posunout o 1mm" }; //MSG_MOVE_1MM
const char MSGEN37[] PROGMEM = { "Move 10mm" }; //MSG_MOVE_10MM
const char MSGCZ37[] PROGMEM = { "Posunout o 10mm" }; //MSG_MOVE_10MM
const char MSGIT37[] PROGMEM = { "Move 10mm" }; //MSG_MOVE_10MM
const char MSGES37[] PROGMEM = { "Move 10mm" }; //MSG_MOVE_10MM
const char MSGPL37[] PROGMEM = { "Posunout o 10mm" }; //MSG_MOVE_10MM
const char MSGEN38[] PROGMEM = { "Speed" }; //MSG_SPEED
const char MSGCZ38[] PROGMEM = { "Rychlost" }; //MSG_SPEED
const char MSGIT38[] PROGMEM = { "Velcità" }; //MSG_SPEED
const char MSGES38[] PROGMEM = { "Velocidad" }; //MSG_SPEED
const char MSGPL38[] PROGMEM = { "Predkosc" }; //MSG_SPEED
const char MSGEN39[] PROGMEM = { "Nozzle" }; //MSG_NOZZLE
const char MSGCZ39[] PROGMEM = { "Tryska" }; //MSG_NOZZLE
const char MSGIT39[] PROGMEM = { "Ugello" }; //MSG_NOZZLE
const char MSGES39[] PROGMEM = { "Fusor" }; //MSG_NOZZLE
const char MSGPL39[] PROGMEM = { "Dysza" }; //MSG_NOZZLE
const char MSGEN40[] PROGMEM = { "Nozzle2" }; //MSG_NOZZLE1
const char MSGCZ40[] PROGMEM = { "Tryska2" }; //MSG_NOZZLE1
const char MSGIT40[] PROGMEM = { "Nozzle2" }; //MSG_NOZZLE1
const char MSGES40[] PROGMEM = { "Nozzle2" }; //MSG_NOZZLE1
const char MSGPL40[] PROGMEM = { "Tryska2" }; //MSG_NOZZLE1
const char MSGEN41[] PROGMEM = { "Nozzle3" }; //MSG_NOZZLE2
const char MSGCZ41[] PROGMEM = { "Tryska3" }; //MSG_NOZZLE2
const char MSGIT41[] PROGMEM = { "Nozzle3" }; //MSG_NOZZLE2
const char MSGES41[] PROGMEM = { "Nozzle3" }; //MSG_NOZZLE2
const char MSGPL41[] PROGMEM = { "Tryska3" }; //MSG_NOZZLE2
const char MSGEN42[] PROGMEM = { "Bed" }; //MSG_BED
const char MSGCZ42[] PROGMEM = { "Bed" }; //MSG_BED
const char MSGIT42[] PROGMEM = { "Piatto" }; //MSG_BED
const char MSGES42[] PROGMEM = { "Base" }; //MSG_BED
const char MSGPL42[] PROGMEM = { "Stolik" }; //MSG_BED
const char MSGEN43[] PROGMEM = { "Fan speed" }; //MSG_FAN_SPEED
const char MSGCZ43[] PROGMEM = { "Rychlost vent." }; //MSG_FAN_SPEED
const char MSGIT43[] PROGMEM = { "Ventola" }; //MSG_FAN_SPEED
const char MSGES43[] PROGMEM = { "Ventilador" }; //MSG_FAN_SPEED
const char MSGPL43[] PROGMEM = { "Predkosc went." }; //MSG_FAN_SPEED
const char MSGEN44[] PROGMEM = { "Flow" }; //MSG_FLOW
const char MSGCZ44[] PROGMEM = { "Prutok" }; //MSG_FLOW
const char MSGIT44[] PROGMEM = { "Flusso" }; //MSG_FLOW
const char MSGES44[] PROGMEM = { "Flujo" }; //MSG_FLOW
const char MSGPL44[] PROGMEM = { "Przeplyw" }; //MSG_FLOW
const char MSGEN45[] PROGMEM = { "Flow 0" }; //MSG_FLOW0
const char MSGCZ45[] PROGMEM = { "Prutok 0" }; //MSG_FLOW0
const char MSGIT45[] PROGMEM = { "Flow 0" }; //MSG_FLOW0
const char MSGES45[] PROGMEM = { "Flow 0" }; //MSG_FLOW0
const char MSGPL45[] PROGMEM = { "Prutok 0" }; //MSG_FLOW0
const char MSGEN46[] PROGMEM = { "Flow 1" }; //MSG_FLOW1
const char MSGCZ46[] PROGMEM = { "Prutok 1" }; //MSG_FLOW1
const char MSGIT46[] PROGMEM = { "Flow 1" }; //MSG_FLOW1
const char MSGES46[] PROGMEM = { "Flow 1" }; //MSG_FLOW1
const char MSGPL46[] PROGMEM = { "Prutok 1" }; //MSG_FLOW1
const char MSGEN47[] PROGMEM = { "Flow 2" }; //MSG_FLOW2
const char MSGCZ47[] PROGMEM = { "Prutok 2" }; //MSG_FLOW2
const char MSGIT47[] PROGMEM = { "Flow 2" }; //MSG_FLOW2
const char MSGES47[] PROGMEM = { "Flow 2" }; //MSG_FLOW2
const char MSGPL47[] PROGMEM = { "Prutok 2" }; //MSG_FLOW2
const char MSGEN48[] PROGMEM = { "Control" }; //MSG_CONTROL
const char MSGCZ48[] PROGMEM = { "Kontrola" }; //MSG_CONTROL
const char MSGIT48[] PROGMEM = { "Control" }; //MSG_CONTROL
const char MSGES48[] PROGMEM = { "Control" }; //MSG_CONTROL
const char MSGPL48[] PROGMEM = { "Kontrola" }; //MSG_CONTROL
const char MSGEN49[] PROGMEM = { " \002 Min" }; //MSG_MIN
const char MSGCZ49[] PROGMEM = { " \002 Min" }; //MSG_MIN
const char MSGIT49[] PROGMEM = { " \002 Min" }; //MSG_MIN
const char MSGES49[] PROGMEM = { " \002 Min" }; //MSG_MIN
const char MSGPL49[] PROGMEM = { " \002 Min" }; //MSG_MIN
const char MSGEN50[] PROGMEM = { " \002 Max" }; //MSG_MAX
const char MSGCZ50[] PROGMEM = { " \002 Max" }; //MSG_MAX
const char MSGIT50[] PROGMEM = { " \002 Max" }; //MSG_MAX
const char MSGES50[] PROGMEM = { " \002 Max" }; //MSG_MAX
const char MSGPL50[] PROGMEM = { " \002 Max" }; //MSG_MAX
const char MSGEN51[] PROGMEM = { " \002 Fact" }; //MSG_FACTOR
const char MSGCZ51[] PROGMEM = { " \002 Fact" }; //MSG_FACTOR
const char MSGIT51[] PROGMEM = { " \002 Fact" }; //MSG_FACTOR
const char MSGES51[] PROGMEM = { " \002 Fact" }; //MSG_FACTOR
const char MSGPL51[] PROGMEM = { " \002 Fact" }; //MSG_FACTOR
const char MSGEN52[] PROGMEM = { "Autotemp" }; //MSG_AUTOTEMP
const char MSGCZ52[] PROGMEM = { "Autotemp" }; //MSG_AUTOTEMP
const char MSGIT52[] PROGMEM = { "Autotemp" }; //MSG_AUTOTEMP
const char MSGES52[] PROGMEM = { "Autotemp" }; //MSG_AUTOTEMP
const char MSGPL52[] PROGMEM = { "Autotemp" }; //MSG_AUTOTEMP
const char MSGEN53[] PROGMEM = { "On " }; //MSG_ON
const char MSGCZ53[] PROGMEM = { "On " }; //MSG_ON
const char MSGIT53[] PROGMEM = { "On " }; //MSG_ON
const char MSGES53[] PROGMEM = { "On " }; //MSG_ON
const char MSGPL53[] PROGMEM = { "On " }; //MSG_ON
const char MSGEN54[] PROGMEM = { "Off" }; //MSG_OFF
const char MSGCZ54[] PROGMEM = { "Off" }; //MSG_OFF
const char MSGIT54[] PROGMEM = { "Off" }; //MSG_OFF
const char MSGES54[] PROGMEM = { "Off" }; //MSG_OFF
const char MSGPL54[] PROGMEM = { "Off" }; //MSG_OFF
const char MSGEN55[] PROGMEM = { "PID-P" }; //MSG_PID_P
const char MSGCZ55[] PROGMEM = { "PID-P" }; //MSG_PID_P
const char MSGIT55[] PROGMEM = { "PID-P" }; //MSG_PID_P
const char MSGES55[] PROGMEM = { "PID-P" }; //MSG_PID_P
const char MSGPL55[] PROGMEM = { "PID-P" }; //MSG_PID_P
const char MSGEN56[] PROGMEM = { "PID-I" }; //MSG_PID_I
const char MSGCZ56[] PROGMEM = { "PID-I" }; //MSG_PID_I
const char MSGIT56[] PROGMEM = { "PID-I" }; //MSG_PID_I
const char MSGES56[] PROGMEM = { "PID-I" }; //MSG_PID_I
const char MSGPL56[] PROGMEM = { "PID-I" }; //MSG_PID_I
const char MSGEN57[] PROGMEM = { "PID-D" }; //MSG_PID_D
const char MSGCZ57[] PROGMEM = { "PID-D" }; //MSG_PID_D
const char MSGIT57[] PROGMEM = { "PID-D" }; //MSG_PID_D
const char MSGES57[] PROGMEM = { "PID-D" }; //MSG_PID_D
const char MSGPL57[] PROGMEM = { "PID-D" }; //MSG_PID_D
const char MSGEN58[] PROGMEM = { "PID-C" }; //MSG_PID_C
const char MSGCZ58[] PROGMEM = { "PID-C" }; //MSG_PID_C
const char MSGIT58[] PROGMEM = { "PID-C" }; //MSG_PID_C
const char MSGES58[] PROGMEM = { "PID-C" }; //MSG_PID_C
const char MSGPL58[] PROGMEM = { "PID-C" }; //MSG_PID_C
const char MSGEN59[] PROGMEM = { "Accel" }; //MSG_ACC
const char MSGCZ59[] PROGMEM = { "Accel" }; //MSG_ACC
const char MSGIT59[] PROGMEM = { "Accel" }; //MSG_ACC
const char MSGES59[] PROGMEM = { "Accel" }; //MSG_ACC
const char MSGPL59[] PROGMEM = { "Accel" }; //MSG_ACC
const char MSGEN60[] PROGMEM = { "Vxy-jerk" }; //MSG_VXY_JERK
const char MSGCZ60[] PROGMEM = { "Vxy-jerk" }; //MSG_VXY_JERK
const char MSGIT60[] PROGMEM = { "Vxy-jerk" }; //MSG_VXY_JERK
const char MSGES60[] PROGMEM = { "Vxy-jerk" }; //MSG_VXY_JERK
const char MSGPL60[] PROGMEM = { "Vxy-jerk" }; //MSG_VXY_JERK
const char MSGEN61[] PROGMEM = { "Vz-jerk" }; //MSG_VZ_JERK
const char MSGCZ61[] PROGMEM = { "Vz-jerk" }; //MSG_VZ_JERK
const char MSGIT61[] PROGMEM = { "Vz-jerk" }; //MSG_VZ_JERK
const char MSGES61[] PROGMEM = { "Vz-jerk" }; //MSG_VZ_JERK
const char MSGPL61[] PROGMEM = { "Vz-jerk" }; //MSG_VZ_JERK
const char MSGEN62[] PROGMEM = { "Ve-jerk" }; //MSG_VE_JERK
const char MSGCZ62[] PROGMEM = { "Ve-jerk" }; //MSG_VE_JERK
const char MSGIT62[] PROGMEM = { "Ve-jerk" }; //MSG_VE_JERK
const char MSGES62[] PROGMEM = { "Ve-jerk" }; //MSG_VE_JERK
const char MSGPL62[] PROGMEM = { "Ve-jerk" }; //MSG_VE_JERK
const char MSGEN63[] PROGMEM = { "Vmax " }; //MSG_VMAX
const char MSGCZ63[] PROGMEM = { "Vmax " }; //MSG_VMAX
const char MSGIT63[] PROGMEM = { "Vmax " }; //MSG_VMAX
const char MSGES63[] PROGMEM = { "Vmax " }; //MSG_VMAX
const char MSGPL63[] PROGMEM = { "Vmax " }; //MSG_VMAX
const char MSGEN64[] PROGMEM = { "x" }; //MSG_X
const char MSGCZ64[] PROGMEM = { "x" }; //MSG_X
const char MSGIT64[] PROGMEM = { "x" }; //MSG_X
const char MSGES64[] PROGMEM = { "x" }; //MSG_X
const char MSGPL64[] PROGMEM = { "x" }; //MSG_X
const char MSGEN65[] PROGMEM = { "y" }; //MSG_Y
const char MSGCZ65[] PROGMEM = { "y" }; //MSG_Y
const char MSGIT65[] PROGMEM = { "y" }; //MSG_Y
const char MSGES65[] PROGMEM = { "y" }; //MSG_Y
const char MSGPL65[] PROGMEM = { "y" }; //MSG_Y
const char MSGEN66[] PROGMEM = { "z" }; //MSG_Z
const char MSGCZ66[] PROGMEM = { "z" }; //MSG_Z
const char MSGIT66[] PROGMEM = { "z" }; //MSG_Z
const char MSGES66[] PROGMEM = { "z" }; //MSG_Z
const char MSGPL66[] PROGMEM = { "z" }; //MSG_Z
const char MSGEN67[] PROGMEM = { "e" }; //MSG_E
const char MSGCZ67[] PROGMEM = { "e" }; //MSG_E
const char MSGIT67[] PROGMEM = { "e" }; //MSG_E
const char MSGES67[] PROGMEM = { "e" }; //MSG_E
const char MSGPL67[] PROGMEM = { "e" }; //MSG_E
const char MSGEN68[] PROGMEM = { "Vmin" }; //MSG_VMIN
const char MSGCZ68[] PROGMEM = { "Vmin" }; //MSG_VMIN
const char MSGIT68[] PROGMEM = { "Vmin" }; //MSG_VMIN
const char MSGES68[] PROGMEM = { "Vmin" }; //MSG_VMIN
const char MSGPL68[] PROGMEM = { "Vmin" }; //MSG_VMIN
const char MSGEN69[] PROGMEM = { "VTrav min" }; //MSG_VTRAV_MIN
const char MSGCZ69[] PROGMEM = { "VTrav min" }; //MSG_VTRAV_MIN
const char MSGIT69[] PROGMEM = { "VTrav min" }; //MSG_VTRAV_MIN
const char MSGES69[] PROGMEM = { "VTrav min" }; //MSG_VTRAV_MIN
const char MSGPL69[] PROGMEM = { "VTrav min" }; //MSG_VTRAV_MIN
const char MSGEN70[] PROGMEM = { "Amax " }; //MSG_AMAX
const char MSGCZ70[] PROGMEM = { "Amax " }; //MSG_AMAX
const char MSGIT70[] PROGMEM = { "Amax " }; //MSG_AMAX
const char MSGES70[] PROGMEM = { "Amax " }; //MSG_AMAX
const char MSGPL70[] PROGMEM = { "Amax " }; //MSG_AMAX
const char MSGEN71[] PROGMEM = { "A-retract" }; //MSG_A_RETRACT
const char MSGCZ71[] PROGMEM = { "A-retract" }; //MSG_A_RETRACT
const char MSGIT71[] PROGMEM = { "A-retract" }; //MSG_A_RETRACT
const char MSGES71[] PROGMEM = { "A-retract" }; //MSG_A_RETRACT
const char MSGPL71[] PROGMEM = { "A-retract" }; //MSG_A_RETRACT
const char MSGEN72[] PROGMEM = { "Xsteps/mm" }; //MSG_XSTEPS
const char MSGCZ72[] PROGMEM = { "Xsteps/mm" }; //MSG_XSTEPS
const char MSGIT72[] PROGMEM = { "Xsteps/mm" }; //MSG_XSTEPS
const char MSGES72[] PROGMEM = { "Xsteps/mm" }; //MSG_XSTEPS
const char MSGPL72[] PROGMEM = { "Xsteps/mm" }; //MSG_XSTEPS
const char MSGEN73[] PROGMEM = { "Ysteps/mm" }; //MSG_YSTEPS
const char MSGCZ73[] PROGMEM = { "Ysteps/mm" }; //MSG_YSTEPS
const char MSGIT73[] PROGMEM = { "Ysteps/mm" }; //MSG_YSTEPS
const char MSGES73[] PROGMEM = { "Ysteps/mm" }; //MSG_YSTEPS
const char MSGPL73[] PROGMEM = { "Ysteps/mm" }; //MSG_YSTEPS
const char MSGEN74[] PROGMEM = { "Zsteps/mm" }; //MSG_ZSTEPS
const char MSGCZ74[] PROGMEM = { "Zsteps/mm" }; //MSG_ZSTEPS
const char MSGIT74[] PROGMEM = { "Zsteps/mm" }; //MSG_ZSTEPS
const char MSGES74[] PROGMEM = { "Zsteps/mm" }; //MSG_ZSTEPS
const char MSGPL74[] PROGMEM = { "Zsteps/mm" }; //MSG_ZSTEPS
const char MSGEN75[] PROGMEM = { "Esteps/mm" }; //MSG_ESTEPS
const char MSGCZ75[] PROGMEM = { "Esteps/mm" }; //MSG_ESTEPS
const char MSGIT75[] PROGMEM = { "Esteps/mm" }; //MSG_ESTEPS
const char MSGES75[] PROGMEM = { "Esteps/mm" }; //MSG_ESTEPS
const char MSGPL75[] PROGMEM = { "Esteps/mm" }; //MSG_ESTEPS
const char MSGEN76[] PROGMEM = { "Temperature" }; //MSG_TEMPERATURE
const char MSGCZ76[] PROGMEM = { "Teplota" }; //MSG_TEMPERATURE
const char MSGIT76[] PROGMEM = { "Temperatura" }; //MSG_TEMPERATURE
const char MSGES76[] PROGMEM = { "Temperatura" }; //MSG_TEMPERATURE
const char MSGPL76[] PROGMEM = { "Temperatura" }; //MSG_TEMPERATURE
const char MSGEN77[] PROGMEM = { "Motion" }; //MSG_MOTION
const char MSGCZ77[] PROGMEM = { "Pohyb" }; //MSG_MOTION
const char MSGIT77[] PROGMEM = { "Motion" }; //MSG_MOTION
const char MSGES77[] PROGMEM = { "Motion" }; //MSG_MOTION
const char MSGPL77[] PROGMEM = { "Pohyb" }; //MSG_MOTION
const char MSGEN78[] PROGMEM = { "Filament" }; //MSG_VOLUMETRIC
const char MSGCZ78[] PROGMEM = { "Filament" }; //MSG_VOLUMETRIC
const char MSGIT78[] PROGMEM = { "Filament" }; //MSG_VOLUMETRIC
const char MSGES78[] PROGMEM = { "Filament" }; //MSG_VOLUMETRIC
const char MSGPL78[] PROGMEM = { "Filament" }; //MSG_VOLUMETRIC
const char MSGEN79[] PROGMEM = { "E in mm3" }; //MSG_VOLUMETRIC_ENABLED
const char MSGCZ79[] PROGMEM = { "E in mm3" }; //MSG_VOLUMETRIC_ENABLED
const char MSGIT79[] PROGMEM = { "E in mm3" }; //MSG_VOLUMETRIC_ENABLED
const char MSGES79[] PROGMEM = { "E in mm3" }; //MSG_VOLUMETRIC_ENABLED
const char MSGPL79[] PROGMEM = { "E in mm3" }; //MSG_VOLUMETRIC_ENABLED
const char MSGEN80[] PROGMEM = { "Fil. Dia. 1" }; //MSG_FILAMENT_SIZE_EXTRUDER_0
const char MSGCZ80[] PROGMEM = { "Fil. Dia. 1" }; //MSG_FILAMENT_SIZE_EXTRUDER_0
const char MSGIT80[] PROGMEM = { "Fil. Dia. 1" }; //MSG_FILAMENT_SIZE_EXTRUDER_0
const char MSGES80[] PROGMEM = { "Fil. Dia. 1" }; //MSG_FILAMENT_SIZE_EXTRUDER_0
const char MSGPL80[] PROGMEM = { "Fil. Dia. 1" }; //MSG_FILAMENT_SIZE_EXTRUDER_0
const char MSGEN81[] PROGMEM = { "Fil. Dia. 2" }; //MSG_FILAMENT_SIZE_EXTRUDER_1
const char MSGCZ81[] PROGMEM = { "Fil. Dia. 2" }; //MSG_FILAMENT_SIZE_EXTRUDER_1
const char MSGIT81[] PROGMEM = { "Fil. Dia. 2" }; //MSG_FILAMENT_SIZE_EXTRUDER_1
const char MSGES81[] PROGMEM = { "Fil. Dia. 2" }; //MSG_FILAMENT_SIZE_EXTRUDER_1
const char MSGPL81[] PROGMEM = { "Fil. Dia. 2" }; //MSG_FILAMENT_SIZE_EXTRUDER_1
const char MSGEN82[] PROGMEM = { "Fil. Dia. 3" }; //MSG_FILAMENT_SIZE_EXTRUDER_2
const char MSGCZ82[] PROGMEM = { "Fil. Dia. 3" }; //MSG_FILAMENT_SIZE_EXTRUDER_2
const char MSGIT82[] PROGMEM = { "Fil. Dia. 3" }; //MSG_FILAMENT_SIZE_EXTRUDER_2
const char MSGES82[] PROGMEM = { "Fil. Dia. 3" }; //MSG_FILAMENT_SIZE_EXTRUDER_2
const char MSGPL82[] PROGMEM = { "Fil. Dia. 3" }; //MSG_FILAMENT_SIZE_EXTRUDER_2
const char MSGEN83[] PROGMEM = { "LCD contrast" }; //MSG_CONTRAST
const char MSGCZ83[] PROGMEM = { "LCD contrast" }; //MSG_CONTRAST
const char MSGIT83[] PROGMEM = { "LCD contrast" }; //MSG_CONTRAST
const char MSGES83[] PROGMEM = { "LCD contrast" }; //MSG_CONTRAST
const char MSGPL83[] PROGMEM = { "LCD contrast" }; //MSG_CONTRAST
const char MSGEN84[] PROGMEM = { "Store memory" }; //MSG_STORE_EPROM
const char MSGCZ84[] PROGMEM = { "Store memory" }; //MSG_STORE_EPROM
const char MSGIT84[] PROGMEM = { "Store memory" }; //MSG_STORE_EPROM
const char MSGES84[] PROGMEM = { "Store memory" }; //MSG_STORE_EPROM
const char MSGPL84[] PROGMEM = { "Store memory" }; //MSG_STORE_EPROM
const char MSGEN85[] PROGMEM = { "Load memory" }; //MSG_LOAD_EPROM
const char MSGCZ85[] PROGMEM = { "Ulozit pamet" }; //MSG_LOAD_EPROM
const char MSGIT85[] PROGMEM = { "Load memory" }; //MSG_LOAD_EPROM
const char MSGES85[] PROGMEM = { "Load memory" }; //MSG_LOAD_EPROM
const char MSGPL85[] PROGMEM = { "Ulozit pamet" }; //MSG_LOAD_EPROM
const char MSGEN86[] PROGMEM = { "Restore failsafe" }; //MSG_RESTORE_FAILSAFE
const char MSGCZ86[] PROGMEM = { "Obnovit vychozi" }; //MSG_RESTORE_FAILSAFE
const char MSGIT86[] PROGMEM = { "Restore failsafe" }; //MSG_RESTORE_FAILSAFE
const char MSGES86[] PROGMEM = { "Restore failsafe" }; //MSG_RESTORE_FAILSAFE
const char MSGPL86[] PROGMEM = { "Obnovit vychozi" }; //MSG_RESTORE_FAILSAFE
const char MSGEN87[] PROGMEM = { "Refresh" }; //MSG_REFRESH
const char MSGCZ87[] PROGMEM = { "Obnovit" }; //MSG_REFRESH
const char MSGIT87[] PROGMEM = { "Refresh" }; //MSG_REFRESH
const char MSGES87[] PROGMEM = { "Refresh" }; //MSG_REFRESH
const char MSGPL87[] PROGMEM = { "Obnovit" }; //MSG_REFRESH
const char MSGEN88[] PROGMEM = { "Info screen" }; //MSG_WATCH
const char MSGCZ88[] PROGMEM = { "Informace" }; //MSG_WATCH
const char MSGIT88[] PROGMEM = { "Guarda" }; //MSG_WATCH
const char MSGES88[] PROGMEM = { "Monitorizar" }; //MSG_WATCH
const char MSGPL88[] PROGMEM = { "Informacje" }; //MSG_WATCH
const char MSGEN89[] PROGMEM = { "Prepare" }; //MSG_PREPARE
const char MSGCZ89[] PROGMEM = { "Priprava" }; //MSG_PREPARE
const char MSGIT89[] PROGMEM = { "Prepare" }; //MSG_PREPARE
const char MSGES89[] PROGMEM = { "Prepare" }; //MSG_PREPARE
const char MSGPL89[] PROGMEM = { "Priprava" }; //MSG_PREPARE
const char MSGEN90[] PROGMEM = { "Tune" }; //MSG_TUNE
const char MSGCZ90[] PROGMEM = { "Ladit" }; //MSG_TUNE
const char MSGIT90[] PROGMEM = { "Adatta" }; //MSG_TUNE
const char MSGES90[] PROGMEM = { "Ajustar" }; //MSG_TUNE
const char MSGPL90[] PROGMEM = { "Nastroic" }; //MSG_TUNE
const char MSGEN91[] PROGMEM = { "Pause print" }; //MSG_PAUSE_PRINT
const char MSGCZ91[] PROGMEM = { "Pozastavit tisk" }; //MSG_PAUSE_PRINT
const char MSGIT91[] PROGMEM = { "Pausa" }; //MSG_PAUSE_PRINT
const char MSGES91[] PROGMEM = { "Pausar impresion" }; //MSG_PAUSE_PRINT
const char MSGPL91[] PROGMEM = { "Przerwac druk" }; //MSG_PAUSE_PRINT
const char MSGEN92[] PROGMEM = { "Resume print" }; //MSG_RESUME_PRINT
const char MSGCZ92[] PROGMEM = { "Pokracovat" }; //MSG_RESUME_PRINT
const char MSGIT92[] PROGMEM = { "Riprendi stampa" }; //MSG_RESUME_PRINT
const char MSGES92[] PROGMEM = { "Reanudar impres." }; //MSG_RESUME_PRINT
const char MSGPL92[] PROGMEM = { "Kontynuowac" }; //MSG_RESUME_PRINT
const char MSGEN93[] PROGMEM = { "Stop print" }; //MSG_STOP_PRINT
const char MSGCZ93[] PROGMEM = { "Zastavit tisk" }; //MSG_STOP_PRINT
const char MSGIT93[] PROGMEM = { "Arresta stampa" }; //MSG_STOP_PRINT
const char MSGES93[] PROGMEM = { "Detener impresion" }; //MSG_STOP_PRINT
const char MSGPL93[] PROGMEM = { "Zatrzymac druk" }; //MSG_STOP_PRINT
const char MSGEN94[] PROGMEM = { "Print from SD" }; //MSG_CARD_MENU
const char MSGCZ94[] PROGMEM = { "Tisk z SD" }; //MSG_CARD_MENU
const char MSGIT94[] PROGMEM = { "Menu SD Carta" }; //MSG_CARD_MENU
const char MSGES94[] PROGMEM = { "Menu de SD" }; //MSG_CARD_MENU
const char MSGPL94[] PROGMEM = { "Druk z SD" }; //MSG_CARD_MENU
const char MSGEN95[] PROGMEM = { "No SD card" }; //MSG_NO_CARD
const char MSGCZ95[] PROGMEM = { "Zadna SD karta" }; //MSG_NO_CARD
const char MSGIT95[] PROGMEM = { "No SD Carta" }; //MSG_NO_CARD
const char MSGES95[] PROGMEM = { "No hay tarjeta SD" }; //MSG_NO_CARD
const char MSGPL95[] PROGMEM = { "Brak karty SD" }; //MSG_NO_CARD
const char MSGEN96[] PROGMEM = { "Sleep..." }; //MSG_DWELL
const char MSGCZ96[] PROGMEM = { "Sleep..." }; //MSG_DWELL
const char MSGIT96[] PROGMEM = { "Sospensione..." }; //MSG_DWELL
const char MSGES96[] PROGMEM = { "Reposo..." }; //MSG_DWELL
const char MSGPL96[] PROGMEM = { "Sleep..." }; //MSG_DWELL
const char MSGEN97[] PROGMEM = { "Wait for user..." }; //MSG_USERWAIT
const char MSGCZ97[] PROGMEM = { "Wait for user..." }; //MSG_USERWAIT
const char MSGIT97[] PROGMEM = { "Attendi Utente..." }; //MSG_USERWAIT
const char MSGES97[] PROGMEM = { "Esperando ordenes" }; //MSG_USERWAIT
const char MSGPL97[] PROGMEM = { "Wait for user..." }; //MSG_USERWAIT
const char MSGEN98[] PROGMEM = { "Resuming print" }; //MSG_RESUMING
const char MSGCZ98[] PROGMEM = { "Obnoveni tisku" }; //MSG_RESUMING
const char MSGIT98[] PROGMEM = { "Riprendi Stampa" }; //MSG_RESUMING
const char MSGES98[] PROGMEM = { "Resumiendo impre." }; //MSG_RESUMING
const char MSGPL98[] PROGMEM = { "Wznowienie druku" }; //MSG_RESUMING
const char MSGEN99[] PROGMEM = { "Print aborted" }; //MSG_PRINT_ABORTED
const char MSGCZ99[] PROGMEM = { "Tisk prerusen" }; //MSG_PRINT_ABORTED
const char MSGIT99[] PROGMEM = { "Stampa abortita" }; //MSG_PRINT_ABORTED
const char MSGES99[] PROGMEM = { "Print aborted" }; //MSG_PRINT_ABORTED
const char MSGPL99[] PROGMEM = { "Druk przerwany" }; //MSG_PRINT_ABORTED
const char MSGEN100[] PROGMEM = { "No move." }; //MSG_NO_MOVE
const char MSGCZ100[] PROGMEM = { "No move." }; //MSG_NO_MOVE
const char MSGIT100[] PROGMEM = { "Nessun Movimento" }; //MSG_NO_MOVE
const char MSGES100[] PROGMEM = { "Sin movimiento" }; //MSG_NO_MOVE
const char MSGPL100[] PROGMEM = { "No move." }; //MSG_NO_MOVE
const char MSGEN101[] PROGMEM = { "KILLED. " }; //MSG_KILLED
const char MSGCZ101[] PROGMEM = { "KILLED. " }; //MSG_KILLED
const char MSGIT101[] PROGMEM = { "UCCISO " }; //MSG_KILLED
const char MSGES101[] PROGMEM = { "PARADA DE EMERG." }; //MSG_KILLED
const char MSGPL101[] PROGMEM = { "KILLED. " }; //MSG_KILLED
const char MSGEN102[] PROGMEM = { "STOPPED. " }; //MSG_STOPPED
const char MSGCZ102[] PROGMEM = { "STOPPED. " }; //MSG_STOPPED
const char MSGIT102[] PROGMEM = { "ARRESTATO " }; //MSG_STOPPED
const char MSGES102[] PROGMEM = { "PARADA" }; //MSG_STOPPED
const char MSGPL102[] PROGMEM = { "STOPPED. " }; //MSG_STOPPED
const char MSGEN103[] PROGMEM = { "Retract mm" }; //MSG_CONTROL_RETRACT
const char MSGCZ103[] PROGMEM = { "Retract mm" }; //MSG_CONTROL_RETRACT
const char MSGIT103[] PROGMEM = { "Retract mm" }; //MSG_CONTROL_RETRACT
const char MSGES103[] PROGMEM = { "Retract mm" }; //MSG_CONTROL_RETRACT
const char MSGPL103[] PROGMEM = { "Retract mm" }; //MSG_CONTROL_RETRACT
const char MSGEN104[] PROGMEM = { "Swap Re.mm" }; //MSG_CONTROL_RETRACT_SWAP
const char MSGCZ104[] PROGMEM = { "Swap Re.mm" }; //MSG_CONTROL_RETRACT_SWAP
const char MSGIT104[] PROGMEM = { "Swap Re.mm" }; //MSG_CONTROL_RETRACT_SWAP
const char MSGES104[] PROGMEM = { "Swap Re.mm" }; //MSG_CONTROL_RETRACT_SWAP
const char MSGPL104[] PROGMEM = { "Swap Re.mm" }; //MSG_CONTROL_RETRACT_SWAP
const char MSGEN105[] PROGMEM = { "Retract  V" }; //MSG_CONTROL_RETRACTF
const char MSGCZ105[] PROGMEM = { "Retract  V" }; //MSG_CONTROL_RETRACTF
const char MSGIT105[] PROGMEM = { "Retract  V" }; //MSG_CONTROL_RETRACTF
const char MSGES105[] PROGMEM = { "Retract  V" }; //MSG_CONTROL_RETRACTF
const char MSGPL105[] PROGMEM = { "Retract  V" }; //MSG_CONTROL_RETRACTF
const char MSGEN106[] PROGMEM = { "Hop mm" }; //MSG_CONTROL_RETRACT_ZLIFT
const char MSGCZ106[] PROGMEM = { "Hop mm" }; //MSG_CONTROL_RETRACT_ZLIFT
const char MSGIT106[] PROGMEM = { "Hop mm" }; //MSG_CONTROL_RETRACT_ZLIFT
const char MSGES106[] PROGMEM = { "Hop mm" }; //MSG_CONTROL_RETRACT_ZLIFT
const char MSGPL106[] PROGMEM = { "Hop mm" }; //MSG_CONTROL_RETRACT_ZLIFT
const char MSGEN107[] PROGMEM = { "UnRet +mm" }; //MSG_CONTROL_RETRACT_RECOVER
const char MSGCZ107[] PROGMEM = { "UnRet +mm" }; //MSG_CONTROL_RETRACT_RECOVER
const char MSGIT107[] PROGMEM = { "UnRet +mm" }; //MSG_CONTROL_RETRACT_RECOVER
const char MSGES107[] PROGMEM = { "UnRet +mm" }; //MSG_CONTROL_RETRACT_RECOVER
const char MSGPL107[] PROGMEM = { "UnRet +mm" }; //MSG_CONTROL_RETRACT_RECOVER
const char MSGEN108[] PROGMEM = { "S UnRet+mm" }; //MSG_CONTROL_RETRACT_RECOVER_SWAP
const char MSGCZ108[] PROGMEM = { "S UnRet+mm" }; //MSG_CONTROL_RETRACT_RECOVER_SWAP
const char MSGIT108[] PROGMEM = { "S UnRet+mm" }; //MSG_CONTROL_RETRACT_RECOVER_SWAP
const char MSGES108[] PROGMEM = { "S UnRet+mm" }; //MSG_CONTROL_RETRACT_RECOVER_SWAP
const char MSGPL108[] PROGMEM = { "S UnRet+mm" }; //MSG_CONTROL_RETRACT_RECOVER_SWAP
const char MSGEN109[] PROGMEM = { "UnRet  V" }; //MSG_CONTROL_RETRACT_RECOVERF
const char MSGCZ109[] PROGMEM = { "UnRet  V" }; //MSG_CONTROL_RETRACT_RECOVERF
const char MSGIT109[] PROGMEM = { "UnRet  V" }; //MSG_CONTROL_RETRACT_RECOVERF
const char MSGES109[] PROGMEM = { "UnRet  V" }; //MSG_CONTROL_RETRACT_RECOVERF
const char MSGPL109[] PROGMEM = { "UnRet  V" }; //MSG_CONTROL_RETRACT_RECOVERF
const char MSGEN110[] PROGMEM = { "AutoRetr." }; //MSG_AUTORETRACT
const char MSGCZ110[] PROGMEM = { "AutoRetr." }; //MSG_AUTORETRACT
const char MSGIT110[] PROGMEM = { "AutoRetr." }; //MSG_AUTORETRACT
const char MSGES110[] PROGMEM = { "AutoRetr." }; //MSG_AUTORETRACT
const char MSGPL110[] PROGMEM = { "AutoRetr." }; //MSG_AUTORETRACT
const char MSGEN111[] PROGMEM = { "Change filament" }; //MSG_FILAMENTCHANGE
const char MSGCZ111[] PROGMEM = { "Vymenit filament" }; //MSG_FILAMENTCHANGE
const char MSGIT111[] PROGMEM = { "Cambiare filamento" }; //MSG_FILAMENTCHANGE
const char MSGES111[] PROGMEM = { "Cambiar filamento" }; //MSG_FILAMENTCHANGE
const char MSGPL111[] PROGMEM = { "Wymienic filament" }; //MSG_FILAMENTCHANGE
const char MSGEN112[] PROGMEM = { "Init. SD card" }; //MSG_INIT_SDCARD
const char MSGCZ112[] PROGMEM = { "Inic. SD" }; //MSG_INIT_SDCARD
const char MSGIT112[] PROGMEM = { "Init. SD card" }; //MSG_INIT_SDCARD
const char MSGES112[] PROGMEM = { "Init. SD card" }; //MSG_INIT_SDCARD
const char MSGPL112[] PROGMEM = { "Inic. SD" }; //MSG_INIT_SDCARD
const char MSGEN113[] PROGMEM = { "Change SD card" }; //MSG_CNG_SDCARD
const char MSGCZ113[] PROGMEM = { "Vymenit SD" }; //MSG_CNG_SDCARD
const char MSGIT113[] PROGMEM = { "Change SD card" }; //MSG_CNG_SDCARD
const char MSGES113[] PROGMEM = { "Change SD card" }; //MSG_CNG_SDCARD
const char MSGPL113[] PROGMEM = { "Vymenit SD" }; //MSG_CNG_SDCARD
const char MSGEN114[] PROGMEM = { "Z probe out. bed" }; //MSG_ZPROBE_OUT
const char MSGCZ114[] PROGMEM = { "Z probe out. bed" }; //MSG_ZPROBE_OUT
const char MSGIT114[] PROGMEM = { "Z probe out. bed" }; //MSG_ZPROBE_OUT
const char MSGES114[] PROGMEM = { "Z probe out. bed" }; //MSG_ZPROBE_OUT
const char MSGPL114[] PROGMEM = { "Z probe out. bed" }; //MSG_ZPROBE_OUT
const char MSGEN115[] PROGMEM = { "Home X/Y before Z" }; //MSG_POSITION_UNKNOWN
const char MSGCZ115[] PROGMEM = { "Home X/Y before Z" }; //MSG_POSITION_UNKNOWN
const char MSGIT115[] PROGMEM = { "Home X/Y before Z" }; //MSG_POSITION_UNKNOWN
const char MSGES115[] PROGMEM = { "Home X/Y before Z" }; //MSG_POSITION_UNKNOWN
const char MSGPL115[] PROGMEM = { "Home X/Y before Z" }; //MSG_POSITION_UNKNOWN
const char MSGEN116[] PROGMEM = { "Z Offset" }; //MSG_ZPROBE_ZOFFSET
const char MSGCZ116[] PROGMEM = { "Z Offset" }; //MSG_ZPROBE_ZOFFSET
const char MSGIT116[] PROGMEM = { "Z Offset" }; //MSG_ZPROBE_ZOFFSET
const char MSGES116[] PROGMEM = { "Z Offset" }; //MSG_ZPROBE_ZOFFSET
const char MSGPL116[] PROGMEM = { "Z Offset" }; //MSG_ZPROBE_ZOFFSET
const char MSGEN117[] PROGMEM = { "Babystep X" }; //MSG_BABYSTEP_X
const char MSGCZ117[] PROGMEM = { "Babystep X" }; //MSG_BABYSTEP_X
const char MSGIT117[] PROGMEM = { "Babystep X" }; //MSG_BABYSTEP_X
const char MSGES117[] PROGMEM = { "Babystep X" }; //MSG_BABYSTEP_X
const char MSGPL117[] PROGMEM = { "Babystep X" }; //MSG_BABYSTEP_X
const char MSGEN118[] PROGMEM = { "Babystep Y" }; //MSG_BABYSTEP_Y
const char MSGCZ118[] PROGMEM = { "Babystep Y" }; //MSG_BABYSTEP_Y
const char MSGIT118[] PROGMEM = { "Babystep Y" }; //MSG_BABYSTEP_Y
const char MSGES118[] PROGMEM = { "Babystep Y" }; //MSG_BABYSTEP_Y
const char MSGPL118[] PROGMEM = { "Babystep Y" }; //MSG_BABYSTEP_Y
const char MSGEN119[] PROGMEM = { "Live adjust Z" }; //MSG_BABYSTEP_Z
const char MSGCZ119[] PROGMEM = { "Doladeni osy Z" }; //MSG_BABYSTEP_Z
const char MSGIT119[] PROGMEM = { "Babystep Z" }; //MSG_BABYSTEP_Z
const char MSGES119[] PROGMEM = { "Micropaso Z" }; //MSG_BABYSTEP_Z
const char MSGPL119[] PROGMEM = { "Dostrojenie osy Z" }; //MSG_BABYSTEP_Z
const char MSGEN120[] PROGMEM = { "Endstop abort" }; //MSG_ENDSTOP_ABORT
const char MSGCZ120[] PROGMEM = { "Endstop abort" }; //MSG_ENDSTOP_ABORT
const char MSGIT120[] PROGMEM = { "Endstop abort" }; //MSG_ENDSTOP_ABORT
const char MSGES120[] PROGMEM = { "Endstop abort" }; //MSG_ENDSTOP_ABORT
const char MSGPL120[] PROGMEM = { "Endstop abort" }; //MSG_ENDSTOP_ABORT
const char MSGEN121[] PROGMEM = { "Auto adjust Z ?" }; //MSG_ADJUSTZ
const char MSGCZ121[] PROGMEM = { "Auto doladit Z ?" }; //MSG_ADJUSTZ
const char MSGIT121[] PROGMEM = { "Auto regolare Z ?" }; //MSG_ADJUSTZ
const char MSGES121[] PROGMEM = { "Auto Micropaso Z?" }; //MSG_ADJUSTZ
const char MSGPL121[] PROGMEM = { "Autodostroic Z?" }; //MSG_ADJUSTZ
const char MSGEN122[] PROGMEM = { "Calibrate Z" }; //MSG_HOMEYZ
const char MSGCZ122[] PROGMEM = { "Kalibrovat Z" }; //MSG_HOMEYZ
const char MSGIT122[] PROGMEM = { "Calibrate Z" }; //MSG_HOMEYZ
const char MSGES122[] PROGMEM = { "Graduar Z" }; //MSG_HOMEYZ
const char MSGPL122[] PROGMEM = { "Kalibrowac Z" }; //MSG_HOMEYZ
const char MSGEN123[] PROGMEM = { "Settings" }; //MSG_SETTINGS
const char MSGCZ123[] PROGMEM = { "Nastaveni" }; //MSG_SETTINGS
const char MSGIT123[] PROGMEM = { "Impostazioni" }; //MSG_SETTINGS
const char MSGES123[] PROGMEM = { "Ajuste" }; //MSG_SETTINGS
const char MSGPL123[] PROGMEM = { "Ustawienia" }; //MSG_SETTINGS
const char MSGEN124[] PROGMEM = { "Preheat" }; //MSG_PREHEAT
const char MSGCZ124[] PROGMEM = { "Predehrev" }; //MSG_PREHEAT
const char MSGIT124[] PROGMEM = { "Preriscalda" }; //MSG_PREHEAT
const char MSGES124[] PROGMEM = { "Precalentar" }; //MSG_PREHEAT
const char MSGPL124[] PROGMEM = { "Grzanie" }; //MSG_PREHEAT
const char MSGEN125[] PROGMEM = { "Unload filament" }; //MSG_UNLOAD_FILAMENT
const char MSGCZ125[] PROGMEM = { "Vyjmout filament" }; //MSG_UNLOAD_FILAMENT
const char MSGIT125[] PROGMEM = { "Scaricare fil." }; //MSG_UNLOAD_FILAMENT
const char MSGES125[] PROGMEM = { "Sacar filamento" }; //MSG_UNLOAD_FILAMENT
const char MSGPL125[] PROGMEM = { "Wyjac filament" }; //MSG_UNLOAD_FILAMENT
const char MSGEN126[] PROGMEM = { "Load filament" }; //MSG_LOAD_FILAMENT
const char MSGCZ126[] PROGMEM = { "Zavest filament" }; //MSG_LOAD_FILAMENT
const char MSGIT126[] PROGMEM = { "Caricare filamento" }; //MSG_LOAD_FILAMENT
const char MSGES126[] PROGMEM = { "Introducir filamento" }; //MSG_LOAD_FILAMENT
const char MSGPL126[] PROGMEM = { "Wprowadz filament" }; //MSG_LOAD_FILAMENT
const char MSGEN127[] PROGMEM = { "Rectract" }; //MSG_RECTRACT
const char MSGCZ127[] PROGMEM = { "Rectract" }; //MSG_RECTRACT
const char MSGIT127[] PROGMEM = { "Rectract" }; //MSG_RECTRACT
const char MSGES127[] PROGMEM = { "Rectract" }; //MSG_RECTRACT
const char MSGPL127[] PROGMEM = { "Rectract" }; //MSG_RECTRACT
const char MSGEN128[] PROGMEM = { "ERROR:" }; //MSG_ERROR
const char MSGCZ128[] PROGMEM = { "CHYBA:" }; //MSG_ERROR
const char MSGIT128[] PROGMEM = { "ERROR:" }; //MSG_ERROR
const char MSGES128[] PROGMEM = { "ERROR:" }; //MSG_ERROR
const char MSGPL128[] PROGMEM = { "BLAD:" }; //MSG_ERROR
const char MSGEN129[] PROGMEM = { "Preheat the nozzle!" }; //MSG_PREHEAT_NOZZLE
const char MSGCZ129[] PROGMEM = { "Predehrejte trysku!" }; //MSG_PREHEAT_NOZZLE
const char MSGIT129[] PROGMEM = { "Preris. ugello!" }; //MSG_PREHEAT_NOZZLE
const char MSGES129[] PROGMEM = { "Precal. extrusor!" }; //MSG_PREHEAT_NOZZLE
const char MSGPL129[] PROGMEM = { "Nagrzej dysze!" }; //MSG_PREHEAT_NOZZLE
const char MSGEN130[] PROGMEM = { "Support" }; //MSG_SUPPORT
const char MSGCZ130[] PROGMEM = { "Podpora" }; //MSG_SUPPORT
const char MSGIT130[] PROGMEM = { "Support" }; //MSG_SUPPORT
const char MSGES130[] PROGMEM = { "Support" }; //MSG_SUPPORT
const char MSGPL130[] PROGMEM = { "Pomoc" }; //MSG_SUPPORT
const char MSGEN131[] PROGMEM = { "Changed correctly?" }; //MSG_CORRECTLY
const char MSGCZ131[] PROGMEM = { "Vymena ok?" }; //MSG_CORRECTLY
const char MSGIT131[] PROGMEM = { "Cambiato corr.?" }; //MSG_CORRECTLY
const char MSGES131[] PROGMEM = { "Cambiado correc.?" }; //MSG_CORRECTLY
const char MSGPL131[] PROGMEM = { "Wymiana ok?" }; //MSG_CORRECTLY
const char MSGEN132[] PROGMEM = { "Yes" }; //MSG_YES
const char MSGCZ132[] PROGMEM = { "Ano" }; //MSG_YES
const char MSGIT132[] PROGMEM = { "Si" }; //MSG_YES
const char MSGES132[] PROGMEM = { "Si" }; //MSG_YES
const char MSGPL132[] PROGMEM = { "Tak" }; //MSG_YES
const char MSGEN133[] PROGMEM = { "No" }; //MSG_NO
const char MSGCZ133[] PROGMEM = { "Ne" }; //MSG_NO
const char MSGIT133[] PROGMEM = { "No" }; //MSG_NO
const char MSGES133[] PROGMEM = { "No" }; //MSG_NO
const char MSGPL133[] PROGMEM = { "Nie" }; //MSG_NO
const char MSGEN134[] PROGMEM = { "Filament not loaded" }; //MSG_NOT_LOADED
const char MSGCZ134[] PROGMEM = { "Filament nezaveden" }; //MSG_NOT_LOADED
const char MSGIT134[] PROGMEM = { "Fil. no cargado" }; //MSG_NOT_LOADED
const char MSGES134[] PROGMEM = { "Fil. no cargado" }; //MSG_NOT_LOADED
const char MSGPL134[] PROGMEM = { "Brak filamentu" }; //MSG_NOT_LOADED
const char MSGEN135[] PROGMEM = { "Color not clear" }; //MSG_NOT_COLOR
const char MSGCZ135[] PROGMEM = { "Barva neni cista" }; //MSG_NOT_COLOR
const char MSGIT135[] PROGMEM = { "Color no claro" }; //MSG_NOT_COLOR
const char MSGES135[] PROGMEM = { "Color no claro" }; //MSG_NOT_COLOR
const char MSGPL135[] PROGMEM = { "Kolor zanieczysz." }; //MSG_NOT_COLOR
const char MSGEN136[] PROGMEM = { "Loading filament" }; //MSG_LOADING_FILAMENT
const char MSGCZ136[] PROGMEM = { "Zavadeni filamentu" }; //MSG_LOADING_FILAMENT
const char MSGIT136[] PROGMEM = { "Cargando fil." }; //MSG_LOADING_FILAMENT
const char MSGES136[] PROGMEM = { "Cargando fil." }; //MSG_LOADING_FILAMENT
const char MSGPL136[] PROGMEM = { "Wprow. filamentu" }; //MSG_LOADING_FILAMENT
const char MSGEN137[] PROGMEM = { "Please wait" }; //MSG_PLEASE_WAIT
const char MSGCZ137[] PROGMEM = { "Prosim cekejte" }; //MSG_PLEASE_WAIT
const char MSGIT137[] PROGMEM = { "Aspetta" }; //MSG_PLEASE_WAIT
const char MSGES137[] PROGMEM = { "Espera" }; //MSG_PLEASE_WAIT
const char MSGPL137[] PROGMEM = { "Prosze czekac" }; //MSG_PLEASE_WAIT
const char MSGEN138[] PROGMEM = { "Loading color" }; //MSG_LOADING_COLOR
const char MSGCZ138[] PROGMEM = { "Cisteni barvy" }; //MSG_LOADING_COLOR
const char MSGIT138[] PROGMEM = { "Cargando color" }; //MSG_LOADING_COLOR
const char MSGES138[] PROGMEM = { "Cargando color" }; //MSG_LOADING_COLOR
const char MSGPL138[] PROGMEM = { "Czyszcz. koloru" }; //MSG_LOADING_COLOR
const char MSGEN139[] PROGMEM = { "Change success!" }; //MSG_CHANGE_SUCCESS
const char MSGCZ139[] PROGMEM = { "Zmena uspesna!" }; //MSG_CHANGE_SUCCESS
const char MSGIT139[] PROGMEM = { "Cambia. riuscito!" }; //MSG_CHANGE_SUCCESS
const char MSGES139[] PROGMEM = { "Cambiar bien!" }; //MSG_CHANGE_SUCCESS
const char MSGPL139[] PROGMEM = { "Wymiana ok!" }; //MSG_CHANGE_SUCCESS
const char MSGEN140[] PROGMEM = { "And press the knob" }; //MSG_PRESS
const char MSGCZ140[] PROGMEM = { "A stisknete tlacitko" }; //MSG_PRESS
const char MSGIT140[] PROGMEM = { "Y pulse el mando" }; //MSG_PRESS
const char MSGES140[] PROGMEM = { "Y pulse el mando" }; //MSG_PRESS
const char MSGPL140[] PROGMEM = { "Nacisnij przycisk" }; //MSG_PRESS
const char MSGEN141[] PROGMEM = { "Insert filament" }; //MSG_INSERT_FILAMENT
const char MSGCZ141[] PROGMEM = { "Vlozte filament" }; //MSG_INSERT_FILAMENT
const char MSGIT141[] PROGMEM = { "Inserire filamento" }; //MSG_INSERT_FILAMENT
const char MSGES141[] PROGMEM = { "Inserta filamento" }; //MSG_INSERT_FILAMENT
const char MSGPL141[] PROGMEM = { "Wprowadz filament" }; //MSG_INSERT_FILAMENT
const char MSGEN142[] PROGMEM = { "Changing filament!" }; //MSG_CHANGING_FILAMENT
const char MSGCZ142[] PROGMEM = { "Vymena filamentu!" }; //MSG_CHANGING_FILAMENT
const char MSGIT142[] PROGMEM = { "Mutevole fil.!" }; //MSG_CHANGING_FILAMENT
const char MSGES142[] PROGMEM = { "Cambiando fil.!" }; //MSG_CHANGING_FILAMENT
const char MSGPL142[] PROGMEM = { "Wymiana filamentu" }; //MSG_CHANGING_FILAMENT
const char MSGEN143[] PROGMEM = { "Mode     [silent]" }; //MSG_SILENT_MODE_ON
const char MSGCZ143[] PROGMEM = { "Mod       [tichy]" }; //MSG_SILENT_MODE_ON
const char MSGIT143[] PROGMEM = { "Modo     [silenzioso]" }; //MSG_SILENT_MODE_ON
const char MSGES143[] PROGMEM = { "Modo     [silencio]" }; //MSG_SILENT_MODE_ON
const char MSGPL143[] PROGMEM = { "Mod       [cichy]" }; //MSG_SILENT_MODE_ON
const char MSGEN144[] PROGMEM = { "Mode [high power]" }; //MSG_SILENT_MODE_OFF
const char MSGCZ144[] PROGMEM = { "Mod  [vys. vykon]" }; //MSG_SILENT_MODE_OFF
const char MSGIT144[] PROGMEM = { "Modo [piu forza]" }; //MSG_SILENT_MODE_OFF
const char MSGES144[] PROGMEM = { "Modo [mas fuerza]" }; //MSG_SILENT_MODE_OFF
const char MSGPL144[] PROGMEM = { "Mod [w wydajnosc]" }; //MSG_SILENT_MODE_OFF
const char MSGEN145[] PROGMEM = { "Reboot the printer" }; //MSG_REBOOT
const char MSGCZ145[] PROGMEM = { "Restartujte tiskarnu" }; //MSG_REBOOT
const char MSGIT145[] PROGMEM = { "Riavvio la stamp." }; //MSG_REBOOT
const char MSGES145[] PROGMEM = { "Reiniciar la imp." }; //MSG_REBOOT
const char MSGPL145[] PROGMEM = { "Restart drukarki" }; //MSG_REBOOT
const char MSGEN146[] PROGMEM = { " for take effect" }; //MSG_TAKE_EFFECT
const char MSGCZ146[] PROGMEM = { " pro projeveni zmen" }; //MSG_TAKE_EFFECT
const char MSGIT146[] PROGMEM = { " per mostrare i camb." }; //MSG_TAKE_EFFECT
const char MSGES146[] PROGMEM = { "para tomar efecto" }; //MSG_TAKE_EFFECT
const char MSGPL146[] PROGMEM = { "wprow. zmian" }; //MSG_TAKE_EFFECT
const char MSGEN147[] PROGMEM = { "enqueing \"" }; //MSG_Enqueing
const char MSGCZ147[] PROGMEM = { "enqueing \"" }; //MSG_Enqueing
const char MSGIT147[] PROGMEM = { "enqueing \"" }; //MSG_Enqueing
const char MSGES147[] PROGMEM = { "enqueing \"" }; //MSG_Enqueing
const char MSGPL147[] PROGMEM = { "enqueing \"" }; //MSG_Enqueing
const char MSGEN148[] PROGMEM = { "PowerUp" }; //MSG_POWERUP
const char MSGCZ148[] PROGMEM = { "PowerUp" }; //MSG_POWERUP
const char MSGIT148[] PROGMEM = { "PowerUp" }; //MSG_POWERUP
const char MSGES148[] PROGMEM = { "PowerUp" }; //MSG_POWERUP
const char MSGPL148[] PROGMEM = { "PowerUp" }; //MSG_POWERUP
const char MSGEN149[] PROGMEM = { " External Reset" }; //MSG_EXTERNAL_RESET
const char MSGCZ149[] PROGMEM = { " External Reset" }; //MSG_EXTERNAL_RESET
const char MSGIT149[] PROGMEM = { " External Reset" }; //MSG_EXTERNAL_RESET
const char MSGES149[] PROGMEM = { " External Reset" }; //MSG_EXTERNAL_RESET
const char MSGPL149[] PROGMEM = { " External Reset" }; //MSG_EXTERNAL_RESET
const char MSGEN150[] PROGMEM = { " Brown out Reset" }; //MSG_BROWNOUT_RESET
const char MSGCZ150[] PROGMEM = { " Brown out Reset" }; //MSG_BROWNOUT_RESET
const char MSGIT150[] PROGMEM = { " Brown out Reset" }; //MSG_BROWNOUT_RESET
const char MSGES150[] PROGMEM = { " Brown out Reset" }; //MSG_BROWNOUT_RESET
const char MSGPL150[] PROGMEM = { " Brown out Reset" }; //MSG_BROWNOUT_RESET
const char MSGEN151[] PROGMEM = { " Watchdog Reset" }; //MSG_WATCHDOG_RESET
const char MSGCZ151[] PROGMEM = { " Watchdog Reset" }; //MSG_WATCHDOG_RESET
const char MSGIT151[] PROGMEM = { " Watchdog Reset" }; //MSG_WATCHDOG_RESET
const char MSGES151[] PROGMEM = { " Watchdog Reset" }; //MSG_WATCHDOG_RESET
const char MSGPL151[] PROGMEM = { " Watchdog Reset" }; //MSG_WATCHDOG_RESET
const char MSGEN152[] PROGMEM = { " Software Reset" }; //MSG_SOFTWARE_RESET
const char MSGCZ152[] PROGMEM = { " Software Reset" }; //MSG_SOFTWARE_RESET
const char MSGIT152[] PROGMEM = { " Software Reset" }; //MSG_SOFTWARE_RESET
const char MSGES152[] PROGMEM = { " Software Reset" }; //MSG_SOFTWARE_RESET
const char MSGPL152[] PROGMEM = { " Software Reset" }; //MSG_SOFTWARE_RESET
const char MSGEN153[] PROGMEM = { " | Author: " }; //MSG_AUTHOR
const char MSGCZ153[] PROGMEM = { " | Author: " }; //MSG_AUTHOR
const char MSGIT153[] PROGMEM = { " | Author: " }; //MSG_AUTHOR
const char MSGES153[] PROGMEM = { " | Author: " }; //MSG_AUTHOR
const char MSGPL153[] PROGMEM = { " | Author: " }; //MSG_AUTHOR
const char MSGEN154[] PROGMEM = { " Last Updated: " }; //MSG_CONFIGURATION_VER
const char MSGCZ154[] PROGMEM = { " Last Updated: " }; //MSG_CONFIGURATION_VER
const char MSGIT154[] PROGMEM = { " Last Updated: " }; //MSG_CONFIGURATION_VER
const char MSGES154[] PROGMEM = { " Last Updated: " }; //MSG_CONFIGURATION_VER
const char MSGPL154[] PROGMEM = { " Last Updated: " }; //MSG_CONFIGURATION_VER
const char MSGEN155[] PROGMEM = { " Free Memory: " }; //MSG_FREE_MEMORY
const char MSGCZ155[] PROGMEM = { " Free Memory: " }; //MSG_FREE_MEMORY
const char MSGIT155[] PROGMEM = { " Free Memory: " }; //MSG_FREE_MEMORY
const char MSGES155[] PROGMEM = { " Free Memory: " }; //MSG_FREE_MEMORY
const char MSGPL155[] PROGMEM = { " Free Memory: " }; //MSG_FREE_MEMORY
const char MSGEN156[] PROGMEM = { "  PlannerBufferBytes: " }; //MSG_PLANNER_BUFFER_BYTES
const char MSGCZ156[] PROGMEM = { "  PlannerBufferBytes: " }; //MSG_PLANNER_BUFFER_BYTES
const char MSGIT156[] PROGMEM = { "  PlannerBufferBytes: " }; //MSG_PLANNER_BUFFER_BYTES
const char MSGES156[] PROGMEM = { "  PlannerBufferBytes: " }; //MSG_PLANNER_BUFFER_BYTES
const char MSGPL156[] PROGMEM = { "  PlannerBufferBytes: " }; //MSG_PLANNER_BUFFER_BYTES
const char MSGEN157[] PROGMEM = { "ok" }; //MSG_OK
const char MSGCZ157[] PROGMEM = { "ok" }; //MSG_OK
const char MSGIT157[] PROGMEM = { "ok" }; //MSG_OK
const char MSGES157[] PROGMEM = { "ok" }; //MSG_OK
const char MSGPL157[] PROGMEM = { "ok" }; //MSG_OK
const char MSGEN158[] PROGMEM = { "Done saving file." }; //MSG_FILE_SAVED
const char MSGCZ158[] PROGMEM = { "Done saving file." }; //MSG_FILE_SAVED
const char MSGIT158[] PROGMEM = { "Done saving file." }; //MSG_FILE_SAVED
const char MSGES158[] PROGMEM = { "Done saving file." }; //MSG_FILE_SAVED
const char MSGPL158[] PROGMEM = { "Done saving file." }; //MSG_FILE_SAVED
const char MSGEN159[] PROGMEM = { "Line Number is not Last Line Number+1, Last Line: " }; //MSG_ERR_LINE_NO
const char MSGCZ159[] PROGMEM = { "Line Number is not Last Line Number+1, Last Line: " }; //MSG_ERR_LINE_NO
const char MSGIT159[] PROGMEM = { "Line Number is not Last Line Number+1, Last Line: " }; //MSG_ERR_LINE_NO
const char MSGES159[] PROGMEM = { "Line Number is not Last Line Number+1, Last Line: " }; //MSG_ERR_LINE_NO
const char MSGPL159[] PROGMEM = { "Line Number is not Last Line Number+1, Last Line: " }; //MSG_ERR_LINE_NO
const char MSGEN160[] PROGMEM = { "checksum mismatch, Last Line: " }; //MSG_ERR_CHECKSUM_MISMATCH
const char MSGCZ160[] PROGMEM = { "checksum mismatch, Last Line: " }; //MSG_ERR_CHECKSUM_MISMATCH
const char MSGIT160[] PROGMEM = { "checksum mismatch, Last Line: " }; //MSG_ERR_CHECKSUM_MISMATCH
const char MSGES160[] PROGMEM = { "checksum mismatch, Last Line: " }; //MSG_ERR_CHECKSUM_MISMATCH
const char MSGPL160[] PROGMEM = { "checksum mismatch, Last Line: " }; //MSG_ERR_CHECKSUM_MISMATCH
const char MSGEN161[] PROGMEM = { "No Checksum with line number, Last Line: " }; //MSG_ERR_NO_CHECKSUM
const char MSGCZ161[] PROGMEM = { "No Checksum with line number, Last Line: " }; //MSG_ERR_NO_CHECKSUM
const char MSGIT161[] PROGMEM = { "No Checksum with line number, Last Line: " }; //MSG_ERR_NO_CHECKSUM
const char MSGES161[] PROGMEM = { "No Checksum with line number, Last Line: " }; //MSG_ERR_NO_CHECKSUM
const char MSGPL161[] PROGMEM = { "No Checksum with line number, Last Line: " }; //MSG_ERR_NO_CHECKSUM
const char MSGEN162[] PROGMEM = { "No Line Number with checksum, Last Line: " }; //MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM
const char MSGCZ162[] PROGMEM = { "No Line Number with checksum, Last Line: " }; //MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM
const char MSGIT162[] PROGMEM = { "No Line Number with checksum, Last Line: " }; //MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM
const char MSGES162[] PROGMEM = { "No Line Number with checksum, Last Line: " }; //MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM
const char MSGPL162[] PROGMEM = { "No Line Number with checksum, Last Line: " }; //MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM
const char MSGEN163[] PROGMEM = { "Done printing file" }; //MSG_FILE_PRINTED
const char MSGCZ163[] PROGMEM = { "Done printing file" }; //MSG_FILE_PRINTED
const char MSGIT163[] PROGMEM = { "Done printing file" }; //MSG_FILE_PRINTED
const char MSGES163[] PROGMEM = { "Done printing file" }; //MSG_FILE_PRINTED
const char MSGPL163[] PROGMEM = { "Done printing file" }; //MSG_FILE_PRINTED
const char MSGEN164[] PROGMEM = { "Begin file list" }; //MSG_BEGIN_FILE_LIST
const char MSGCZ164[] PROGMEM = { "Begin file list" }; //MSG_BEGIN_FILE_LIST
const char MSGIT164[] PROGMEM = { "Begin file list" }; //MSG_BEGIN_FILE_LIST
const char MSGES164[] PROGMEM = { "Begin file list" }; //MSG_BEGIN_FILE_LIST
const char MSGPL164[] PROGMEM = { "Begin file list" }; //MSG_BEGIN_FILE_LIST
const char MSGEN165[] PROGMEM = { "End file list" }; //MSG_END_FILE_LIST
const char MSGCZ165[] PROGMEM = { "End file list" }; //MSG_END_FILE_LIST
const char MSGIT165[] PROGMEM = { "End file list" }; //MSG_END_FILE_LIST
const char MSGES165[] PROGMEM = { "End file list" }; //MSG_END_FILE_LIST
const char MSGPL165[] PROGMEM = { "End file list" }; //MSG_END_FILE_LIST
const char MSGEN166[] PROGMEM = { "M104 Invalid extruder " }; //MSG_M104_INVALID_EXTRUDER
const char MSGCZ166[] PROGMEM = { "M104 Invalid extruder " }; //MSG_M104_INVALID_EXTRUDER
const char MSGIT166[] PROGMEM = { "M104 Invalid extruder " }; //MSG_M104_INVALID_EXTRUDER
const char MSGES166[] PROGMEM = { "M104 Invalid extruder " }; //MSG_M104_INVALID_EXTRUDER
const char MSGPL166[] PROGMEM = { "M104 Invalid extruder " }; //MSG_M104_INVALID_EXTRUDER
const char MSGEN167[] PROGMEM = { "M105 Invalid extruder " }; //MSG_M105_INVALID_EXTRUDER
const char MSGCZ167[] PROGMEM = { "M105 Invalid extruder " }; //MSG_M105_INVALID_EXTRUDER
const char MSGIT167[] PROGMEM = { "M105 Invalid extruder " }; //MSG_M105_INVALID_EXTRUDER
const char MSGES167[] PROGMEM = { "M105 Invalid extruder " }; //MSG_M105_INVALID_EXTRUDER
const char MSGPL167[] PROGMEM = { "M105 Invalid extruder " }; //MSG_M105_INVALID_EXTRUDER
const char MSGEN168[] PROGMEM = { "M200 Invalid extruder " }; //MSG_M200_INVALID_EXTRUDER
const char MSGCZ168[] PROGMEM = { "M200 Invalid extruder " }; //MSG_M200_INVALID_EXTRUDER
const char MSGIT168[] PROGMEM = { "M200 Invalid extruder " }; //MSG_M200_INVALID_EXTRUDER
const char MSGES168[] PROGMEM = { "M200 Invalid extruder " }; //MSG_M200_INVALID_EXTRUDER
const char MSGPL168[] PROGMEM = { "M200 Invalid extruder " }; //MSG_M200_INVALID_EXTRUDER
const char MSGEN169[] PROGMEM = { "M218 Invalid extruder " }; //MSG_M218_INVALID_EXTRUDER
const char MSGCZ169[] PROGMEM = { "M218 Invalid extruder " }; //MSG_M218_INVALID_EXTRUDER
const char MSGIT169[] PROGMEM = { "M218 Invalid extruder " }; //MSG_M218_INVALID_EXTRUDER
const char MSGES169[] PROGMEM = { "M218 Invalid extruder " }; //MSG_M218_INVALID_EXTRUDER
const char MSGPL169[] PROGMEM = { "M218 Invalid extruder " }; //MSG_M218_INVALID_EXTRUDER
const char MSGEN170[] PROGMEM = { "M221 Invalid extruder " }; //MSG_M221_INVALID_EXTRUDER
const char MSGCZ170[] PROGMEM = { "M221 Invalid extruder " }; //MSG_M221_INVALID_EXTRUDER
const char MSGIT170[] PROGMEM = { "M221 Invalid extruder " }; //MSG_M221_INVALID_EXTRUDER
const char MSGES170[] PROGMEM = { "M221 Invalid extruder " }; //MSG_M221_INVALID_EXTRUDER
const char MSGPL170[] PROGMEM = { "M221 Invalid extruder " }; //MSG_M221_INVALID_EXTRUDER
const char MSGEN171[] PROGMEM = { "No thermistors - no temperature" }; //MSG_ERR_NO_THERMISTORS
const char MSGCZ171[] PROGMEM = { "No thermistors - no temperature" }; //MSG_ERR_NO_THERMISTORS
const char MSGIT171[] PROGMEM = { "No thermistors - no temperature" }; //MSG_ERR_NO_THERMISTORS
const char MSGES171[] PROGMEM = { "No thermistors - no temperature" }; //MSG_ERR_NO_THERMISTORS
const char MSGPL171[] PROGMEM = { "No thermistors - no temperature" }; //MSG_ERR_NO_THERMISTORS
const char MSGEN172[] PROGMEM = { "M109 Invalid extruder " }; //MSG_M109_INVALID_EXTRUDER
const char MSGCZ172[] PROGMEM = { "M109 Invalid extruder " }; //MSG_M109_INVALID_EXTRUDER
const char MSGIT172[] PROGMEM = { "M109 Invalid extruder " }; //MSG_M109_INVALID_EXTRUDER
const char MSGES172[] PROGMEM = { "M109 Invalid extruder " }; //MSG_M109_INVALID_EXTRUDER
const char MSGPL172[] PROGMEM = { "M109 Invalid extruder " }; //MSG_M109_INVALID_EXTRUDER
const char MSGEN173[] PROGMEM = { "Heating..." }; //MSG_HEATING
const char MSGCZ173[] PROGMEM = { "Zahrivani..." }; //MSG_HEATING
const char MSGIT173[] PROGMEM = { "Riscaldamento..." }; //MSG_HEATING
const char MSGES173[] PROGMEM = { "Calentando..." }; //MSG_HEATING
const char MSGPL173[] PROGMEM = { "Grzanie..." }; //MSG_HEATING
const char MSGEN174[] PROGMEM = { "Heating done." }; //MSG_HEATING_COMPLETE
const char MSGCZ174[] PROGMEM = { "Zahrivani OK." }; //MSG_HEATING_COMPLETE
const char MSGIT174[] PROGMEM = { "Riscaldamento fatto." }; //MSG_HEATING_COMPLETE
const char MSGES174[] PROGMEM = { "Calentando listo." }; //MSG_HEATING_COMPLETE
const char MSGPL174[] PROGMEM = { "Grzanie OK." }; //MSG_HEATING_COMPLETE
const char MSGEN175[] PROGMEM = { "Bed Heating." }; //MSG_BED_HEATING
const char MSGCZ175[] PROGMEM = { "Zahrivani bed..." }; //MSG_BED_HEATING
const char MSGIT175[] PROGMEM = { "Piatto riscaldam." }; //MSG_BED_HEATING
const char MSGES175[] PROGMEM = { "Base Calentando" }; //MSG_BED_HEATING
const char MSGPL175[] PROGMEM = { "Grzanie stolika.." }; //MSG_BED_HEATING
const char MSGEN176[] PROGMEM = { "Bed done." }; //MSG_BED_DONE
const char MSGCZ176[] PROGMEM = { "Bed OK." }; //MSG_BED_DONE
const char MSGIT176[] PROGMEM = { "Piatto fatto." }; //MSG_BED_DONE
const char MSGES176[] PROGMEM = { "Base listo." }; //MSG_BED_DONE
const char MSGPL176[] PROGMEM = { "Stolik OK." }; //MSG_BED_DONE
const char MSGEN177[] PROGMEM = { "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:https://github.com/prusa3d/Prusa-i3-Plus/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:" "Prusa i3 EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000\n" }; //MSG_M115_REPORT
const char MSGCZ177[] PROGMEM = { "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:https://github.com/prusa3d/Prusa-i3-Plus/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:" "Prusa i3 EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000\n" }; //MSG_M115_REPORT
const char MSGIT177[] PROGMEM = { "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:https://github.com/prusa3d/Prusa-i3-Plus/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:" "Prusa i3 EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000\n" }; //MSG_M115_REPORT
const char MSGES177[] PROGMEM = { "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:https://github.com/prusa3d/Prusa-i3-Plus/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:" "Prusa i3 EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000\n" }; //MSG_M115_REPORT
const char MSGPL177[] PROGMEM = { "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:https://github.com/prusa3d/Prusa-i3-Plus/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:" "Prusa i3 EXTRUDER_COUNT:1 UUID:00000000-0000-0000-0000-000000000000\n" }; //MSG_M115_REPORT
const char MSGEN178[] PROGMEM = { " Count X: " }; //MSG_COUNT_X
const char MSGCZ178[] PROGMEM = { " Count X: " }; //MSG_COUNT_X
const char MSGIT178[] PROGMEM = { " Count X: " }; //MSG_COUNT_X
const char MSGES178[] PROGMEM = { " Count X: " }; //MSG_COUNT_X
const char MSGPL178[] PROGMEM = { " Count X: " }; //MSG_COUNT_X
const char MSGEN179[] PROGMEM = { "Printer halted. kill() called!" }; //MSG_ERR_KILLED
const char MSGCZ179[] PROGMEM = { "Printer halted. kill() called!" }; //MSG_ERR_KILLED
const char MSGIT179[] PROGMEM = { "Printer halted. kill() called!" }; //MSG_ERR_KILLED
const char MSGES179[] PROGMEM = { "Printer halted. kill() called!" }; //MSG_ERR_KILLED
const char MSGPL179[] PROGMEM = { "Printer halted. kill() called!" }; //MSG_ERR_KILLED
const char MSGEN180[] PROGMEM = { "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)" }; //MSG_ERR_STOPPED
const char MSGCZ180[] PROGMEM = { "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)" }; //MSG_ERR_STOPPED
const char MSGIT180[] PROGMEM = { "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)" }; //MSG_ERR_STOPPED
const char MSGES180[] PROGMEM = { "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)" }; //MSG_ERR_STOPPED
const char MSGPL180[] PROGMEM = { "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)" }; //MSG_ERR_STOPPED
const char MSGEN181[] PROGMEM = { "Resend: " }; //MSG_RESEND
const char MSGCZ181[] PROGMEM = { "Resend: " }; //MSG_RESEND
const char MSGIT181[] PROGMEM = { "Resend: " }; //MSG_RESEND
const char MSGES181[] PROGMEM = { "Resend: " }; //MSG_RESEND
const char MSGPL181[] PROGMEM = { "Resend: " }; //MSG_RESEND
const char MSGEN182[] PROGMEM = { "Unknown command: \"" }; //MSG_UNKNOWN_COMMAND
const char MSGCZ182[] PROGMEM = { "Unknown command: \"" }; //MSG_UNKNOWN_COMMAND
const char MSGIT182[] PROGMEM = { "Unknown command: \"" }; //MSG_UNKNOWN_COMMAND
const char MSGES182[] PROGMEM = { "Unknown command: \"" }; //MSG_UNKNOWN_COMMAND
const char MSGPL182[] PROGMEM = { "Unknown command: \"" }; //MSG_UNKNOWN_COMMAND
const char MSGEN183[] PROGMEM = { "Active Extruder: " }; //MSG_ACTIVE_EXTRUDER
const char MSGCZ183[] PROGMEM = { "Active Extruder: " }; //MSG_ACTIVE_EXTRUDER
const char MSGIT183[] PROGMEM = { "Active Extruder: " }; //MSG_ACTIVE_EXTRUDER
const char MSGES183[] PROGMEM = { "Active Extruder: " }; //MSG_ACTIVE_EXTRUDER
const char MSGPL183[] PROGMEM = { "Active Extruder: " }; //MSG_ACTIVE_EXTRUDER
const char MSGEN184[] PROGMEM = { "Invalid extruder" }; //MSG_INVALID_EXTRUDER
const char MSGCZ184[] PROGMEM = { "Invalid extruder" }; //MSG_INVALID_EXTRUDER
const char MSGIT184[] PROGMEM = { "Invalid extruder" }; //MSG_INVALID_EXTRUDER
const char MSGES184[] PROGMEM = { "Invalid extruder" }; //MSG_INVALID_EXTRUDER
const char MSGPL184[] PROGMEM = { "Invalid extruder" }; //MSG_INVALID_EXTRUDER
const char MSGEN185[] PROGMEM = { "x_min: " }; //MSG_X_MIN
const char MSGCZ185[] PROGMEM = { "x_min: " }; //MSG_X_MIN
const char MSGIT185[] PROGMEM = { "x_min: " }; //MSG_X_MIN
const char MSGES185[] PROGMEM = { "x_min: " }; //MSG_X_MIN
const char MSGPL185[] PROGMEM = { "x_min: " }; //MSG_X_MIN
const char MSGEN186[] PROGMEM = { "x_max: " }; //MSG_X_MAX
const char MSGCZ186[] PROGMEM = { "x_max: " }; //MSG_X_MAX
const char MSGIT186[] PROGMEM = { "x_max: " }; //MSG_X_MAX
const char MSGES186[] PROGMEM = { "x_max: " }; //MSG_X_MAX
const char MSGPL186[] PROGMEM = { "x_max: " }; //MSG_X_MAX
const char MSGEN187[] PROGMEM = { "y_min: " }; //MSG_Y_MIN
const char MSGCZ187[] PROGMEM = { "y_min: " }; //MSG_Y_MIN
const char MSGIT187[] PROGMEM = { "y_min: " }; //MSG_Y_MIN
const char MSGES187[] PROGMEM = { "y_min: " }; //MSG_Y_MIN
const char MSGPL187[] PROGMEM = { "y_min: " }; //MSG_Y_MIN
const char MSGEN188[] PROGMEM = { "y_max: " }; //MSG_Y_MAX
const char MSGCZ188[] PROGMEM = { "y_max: " }; //MSG_Y_MAX
const char MSGIT188[] PROGMEM = { "y_max: " }; //MSG_Y_MAX
const char MSGES188[] PROGMEM = { "y_max: " }; //MSG_Y_MAX
const char MSGPL188[] PROGMEM = { "y_max: " }; //MSG_Y_MAX
const char MSGEN189[] PROGMEM = { "z_min: " }; //MSG_Z_MIN
const char MSGCZ189[] PROGMEM = { "z_min: " }; //MSG_Z_MIN
const char MSGIT189[] PROGMEM = { "z_min: " }; //MSG_Z_MIN
const char MSGES189[] PROGMEM = { "z_min: " }; //MSG_Z_MIN
const char MSGPL189[] PROGMEM = { "z_min: " }; //MSG_Z_MIN
const char MSGEN190[] PROGMEM = { "z_max: " }; //MSG_Z_MAX
const char MSGCZ190[] PROGMEM = { "z_max: " }; //MSG_Z_MAX
const char MSGIT190[] PROGMEM = { "z_max: " }; //MSG_Z_MAX
const char MSGES190[] PROGMEM = { "z_max: " }; //MSG_Z_MAX
const char MSGPL190[] PROGMEM = { "z_max: " }; //MSG_Z_MAX
const char MSGEN191[] PROGMEM = { "Reporting endstop status" }; //MSG_M119_REPORT
const char MSGCZ191[] PROGMEM = { "Reporting endstop status" }; //MSG_M119_REPORT
const char MSGIT191[] PROGMEM = { "Reporting endstop status" }; //MSG_M119_REPORT
const char MSGES191[] PROGMEM = { "Reporting endstop status" }; //MSG_M119_REPORT
const char MSGPL191[] PROGMEM = { "Reporting endstop status" }; //MSG_M119_REPORT
const char MSGEN192[] PROGMEM = { "TRIGGERED" }; //MSG_ENDSTOP_HIT
const char MSGCZ192[] PROGMEM = { "TRIGGERED" }; //MSG_ENDSTOP_HIT
const char MSGIT192[] PROGMEM = { "TRIGGERED" }; //MSG_ENDSTOP_HIT
const char MSGES192[] PROGMEM = { "TRIGGERED" }; //MSG_ENDSTOP_HIT
const char MSGPL192[] PROGMEM = { "TRIGGERED" }; //MSG_ENDSTOP_HIT
const char MSGEN193[] PROGMEM = { "open" }; //MSG_ENDSTOP_OPEN
const char MSGCZ193[] PROGMEM = { "open" }; //MSG_ENDSTOP_OPEN
const char MSGIT193[] PROGMEM = { "open" }; //MSG_ENDSTOP_OPEN
const char MSGES193[] PROGMEM = { "open" }; //MSG_ENDSTOP_OPEN
const char MSGPL193[] PROGMEM = { "open" }; //MSG_ENDSTOP_OPEN
const char MSGEN194[] PROGMEM = { "Hotend offsets:" }; //MSG_HOTEND_OFFSET
const char MSGCZ194[] PROGMEM = { "Hotend offsets:" }; //MSG_HOTEND_OFFSET
const char MSGIT194[] PROGMEM = { "Hotend offsets:" }; //MSG_HOTEND_OFFSET
const char MSGES194[] PROGMEM = { "Hotend offsets:" }; //MSG_HOTEND_OFFSET
const char MSGPL194[] PROGMEM = { "Hotend offsets:" }; //MSG_HOTEND_OFFSET
const char MSGEN195[] PROGMEM = { "Cannot open subdir" }; //MSG_SD_CANT_OPEN_SUBDIR
const char MSGCZ195[] PROGMEM = { "Cannot open subdir" }; //MSG_SD_CANT_OPEN_SUBDIR
const char MSGIT195[] PROGMEM = { "Cannot open subdir" }; //MSG_SD_CANT_OPEN_SUBDIR
const char MSGES195[] PROGMEM = { "Cannot open subdir" }; //MSG_SD_CANT_OPEN_SUBDIR
const char MSGPL195[] PROGMEM = { "Cannot open subdir" }; //MSG_SD_CANT_OPEN_SUBDIR
const char MSGEN196[] PROGMEM = { "SD init fail" }; //MSG_SD_INIT_FAIL
const char MSGCZ196[] PROGMEM = { "SD init fail" }; //MSG_SD_INIT_FAIL
const char MSGIT196[] PROGMEM = { "SD init fail" }; //MSG_SD_INIT_FAIL
const char MSGES196[] PROGMEM = { "SD init fail" }; //MSG_SD_INIT_FAIL
const char MSGPL196[] PROGMEM = { "SD init fail" }; //MSG_SD_INIT_FAIL
const char MSGEN197[] PROGMEM = { "volume.init failed" }; //MSG_SD_VOL_INIT_FAIL
const char MSGCZ197[] PROGMEM = { "volume.init failed" }; //MSG_SD_VOL_INIT_FAIL
const char MSGIT197[] PROGMEM = { "volume.init failed" }; //MSG_SD_VOL_INIT_FAIL
const char MSGES197[] PROGMEM = { "volume.init failed" }; //MSG_SD_VOL_INIT_FAIL
const char MSGPL197[] PROGMEM = { "volume.init failed" }; //MSG_SD_VOL_INIT_FAIL
const char MSGEN198[] PROGMEM = { "openRoot failed" }; //MSG_SD_OPENROOT_FAIL
const char MSGCZ198[] PROGMEM = { "openRoot failed" }; //MSG_SD_OPENROOT_FAIL
const char MSGIT198[] PROGMEM = { "openRoot failed" }; //MSG_SD_OPENROOT_FAIL
const char MSGES198[] PROGMEM = { "openRoot failed" }; //MSG_SD_OPENROOT_FAIL
const char MSGPL198[] PROGMEM = { "openRoot failed" }; //MSG_SD_OPENROOT_FAIL
const char MSGEN199[] PROGMEM = { "SD card ok" }; //MSG_SD_CARD_OK
const char MSGCZ199[] PROGMEM = { "SD card ok" }; //MSG_SD_CARD_OK
const char MSGIT199[] PROGMEM = { "SD card ok" }; //MSG_SD_CARD_OK
const char MSGES199[] PROGMEM = { "SD card ok" }; //MSG_SD_CARD_OK
const char MSGPL199[] PROGMEM = { "SD card ok" }; //MSG_SD_CARD_OK
const char MSGEN200[] PROGMEM = { "workDir open failed" }; //MSG_SD_WORKDIR_FAIL
const char MSGCZ200[] PROGMEM = { "workDir open failed" }; //MSG_SD_WORKDIR_FAIL
const char MSGIT200[] PROGMEM = { "workDir open failed" }; //MSG_SD_WORKDIR_FAIL
const char MSGES200[] PROGMEM = { "workDir open failed" }; //MSG_SD_WORKDIR_FAIL
const char MSGPL200[] PROGMEM = { "workDir open failed" }; //MSG_SD_WORKDIR_FAIL
const char MSGEN201[] PROGMEM = { "open failed, File: " }; //MSG_SD_OPEN_FILE_FAIL
const char MSGCZ201[] PROGMEM = { "open failed, File: " }; //MSG_SD_OPEN_FILE_FAIL
const char MSGIT201[] PROGMEM = { "open failed, File: " }; //MSG_SD_OPEN_FILE_FAIL
const char MSGES201[] PROGMEM = { "open failed, File: " }; //MSG_SD_OPEN_FILE_FAIL
const char MSGPL201[] PROGMEM = { "open failed, File: " }; //MSG_SD_OPEN_FILE_FAIL
const char MSGEN202[] PROGMEM = { "File opened: " }; //MSG_SD_FILE_OPENED
const char MSGCZ202[] PROGMEM = { "File opened: " }; //MSG_SD_FILE_OPENED
const char MSGIT202[] PROGMEM = { "File opened: " }; //MSG_SD_FILE_OPENED
const char MSGES202[] PROGMEM = { "File opened: " }; //MSG_SD_FILE_OPENED
const char MSGPL202[] PROGMEM = { "File opened: " }; //MSG_SD_FILE_OPENED
const char MSGEN203[] PROGMEM = { " Size: " }; //MSG_SD_SIZE
const char MSGCZ203[] PROGMEM = { " Size: " }; //MSG_SD_SIZE
const char MSGIT203[] PROGMEM = { " Size: " }; //MSG_SD_SIZE
const char MSGES203[] PROGMEM = { " Size: " }; //MSG_SD_SIZE
const char MSGPL203[] PROGMEM = { " Size: " }; //MSG_SD_SIZE
const char MSGEN204[] PROGMEM = { "File selected" }; //MSG_SD_FILE_SELECTED
const char MSGCZ204[] PROGMEM = { "File selected" }; //MSG_SD_FILE_SELECTED
const char MSGIT204[] PROGMEM = { "File selected" }; //MSG_SD_FILE_SELECTED
const char MSGES204[] PROGMEM = { "File selected" }; //MSG_SD_FILE_SELECTED
const char MSGPL204[] PROGMEM = { "File selected" }; //MSG_SD_FILE_SELECTED
const char MSGEN205[] PROGMEM = { "Writing to file: " }; //MSG_SD_WRITE_TO_FILE
const char MSGCZ205[] PROGMEM = { "Writing to file: " }; //MSG_SD_WRITE_TO_FILE
const char MSGIT205[] PROGMEM = { "Writing to file: " }; //MSG_SD_WRITE_TO_FILE
const char MSGES205[] PROGMEM = { "Writing to file: " }; //MSG_SD_WRITE_TO_FILE
const char MSGPL205[] PROGMEM = { "Writing to file: " }; //MSG_SD_WRITE_TO_FILE
const char MSGEN206[] PROGMEM = { "SD printing byte " }; //MSG_SD_PRINTING_BYTE
const char MSGCZ206[] PROGMEM = { "SD printing byte " }; //MSG_SD_PRINTING_BYTE
const char MSGIT206[] PROGMEM = { "SD printing byte " }; //MSG_SD_PRINTING_BYTE
const char MSGES206[] PROGMEM = { "SD printing byte " }; //MSG_SD_PRINTING_BYTE
const char MSGPL206[] PROGMEM = { "SD printing byte " }; //MSG_SD_PRINTING_BYTE
const char MSGEN207[] PROGMEM = { "Not SD printing" }; //MSG_SD_NOT_PRINTING
const char MSGCZ207[] PROGMEM = { "Not SD printing" }; //MSG_SD_NOT_PRINTING
const char MSGIT207[] PROGMEM = { "Not SD printing" }; //MSG_SD_NOT_PRINTING
const char MSGES207[] PROGMEM = { "Not SD printing" }; //MSG_SD_NOT_PRINTING
const char MSGPL207[] PROGMEM = { "Not SD printing" }; //MSG_SD_NOT_PRINTING
const char MSGEN208[] PROGMEM = { "error writing to file" }; //MSG_SD_ERR_WRITE_TO_FILE
const char MSGCZ208[] PROGMEM = { "error writing to file" }; //MSG_SD_ERR_WRITE_TO_FILE
const char MSGIT208[] PROGMEM = { "error writing to file" }; //MSG_SD_ERR_WRITE_TO_FILE
const char MSGES208[] PROGMEM = { "error writing to file" }; //MSG_SD_ERR_WRITE_TO_FILE
const char MSGPL208[] PROGMEM = { "error writing to file" }; //MSG_SD_ERR_WRITE_TO_FILE
const char MSGEN209[] PROGMEM = { "Cannot enter subdir: " }; //MSG_SD_CANT_ENTER_SUBDIR
const char MSGCZ209[] PROGMEM = { "Cannot enter subdir: " }; //MSG_SD_CANT_ENTER_SUBDIR
const char MSGIT209[] PROGMEM = { "Cannot enter subdir: " }; //MSG_SD_CANT_ENTER_SUBDIR
const char MSGES209[] PROGMEM = { "Cannot enter subdir: " }; //MSG_SD_CANT_ENTER_SUBDIR
const char MSGPL209[] PROGMEM = { "Cannot enter subdir: " }; //MSG_SD_CANT_ENTER_SUBDIR
const char MSGEN210[] PROGMEM = { "Steprate too high: " }; //MSG_STEPPER_TOO_HIGH
const char MSGCZ210[] PROGMEM = { "Steprate too high: " }; //MSG_STEPPER_TOO_HIGH
const char MSGIT210[] PROGMEM = { "Steprate too high: " }; //MSG_STEPPER_TOO_HIGH
const char MSGES210[] PROGMEM = { "Steprate too high: " }; //MSG_STEPPER_TOO_HIGH
const char MSGPL210[] PROGMEM = { "Steprate too high: " }; //MSG_STEPPER_TOO_HIGH
const char MSGEN211[] PROGMEM = { "endstops hit: " }; //MSG_ENDSTOPS_HIT
const char MSGCZ211[] PROGMEM = { "endstops hit: " }; //MSG_ENDSTOPS_HIT
const char MSGIT211[] PROGMEM = { "endstops hit: " }; //MSG_ENDSTOPS_HIT
const char MSGES211[] PROGMEM = { "endstops hit: " }; //MSG_ENDSTOPS_HIT
const char MSGPL211[] PROGMEM = { "endstops hit: " }; //MSG_ENDSTOPS_HIT
const char MSGEN212[] PROGMEM = { " cold extrusion prevented" }; //MSG_ERR_COLD_EXTRUDE_STOP
const char MSGCZ212[] PROGMEM = { " cold extrusion prevented" }; //MSG_ERR_COLD_EXTRUDE_STOP
const char MSGIT212[] PROGMEM = { " cold extrusion prevented" }; //MSG_ERR_COLD_EXTRUDE_STOP
const char MSGES212[] PROGMEM = { " cold extrusion prevented" }; //MSG_ERR_COLD_EXTRUDE_STOP
const char MSGPL212[] PROGMEM = { " cold extrusion prevented" }; //MSG_ERR_COLD_EXTRUDE_STOP
const char MSGEN213[] PROGMEM = { " too long extrusion prevented" }; //MSG_ERR_LONG_EXTRUDE_STOP
const char MSGCZ213[] PROGMEM = { " too long extrusion prevented" }; //MSG_ERR_LONG_EXTRUDE_STOP
const char MSGIT213[] PROGMEM = { " too long extrusion prevented" }; //MSG_ERR_LONG_EXTRUDE_STOP
const char MSGES213[] PROGMEM = { " too long extrusion prevented" }; //MSG_ERR_LONG_EXTRUDE_STOP
const char MSGPL213[] PROGMEM = { " too long extrusion prevented" }; //MSG_ERR_LONG_EXTRUDE_STOP
const char MSGEN214[] PROGMEM = { "Babystepping X" }; //MSG_BABYSTEPPING_X
const char MSGCZ214[] PROGMEM = { "Babystepping X" }; //MSG_BABYSTEPPING_X
const char MSGIT214[] PROGMEM = { "Babystepping X" }; //MSG_BABYSTEPPING_X
const char MSGES214[] PROGMEM = { "Babystepping X" }; //MSG_BABYSTEPPING_X
const char MSGPL214[] PROGMEM = { "Babystepping X" }; //MSG_BABYSTEPPING_X
const char MSGEN215[] PROGMEM = { "Babystepping Y" }; //MSG_BABYSTEPPING_Y
const char MSGCZ215[] PROGMEM = { "Babystepping Y" }; //MSG_BABYSTEPPING_Y
const char MSGIT215[] PROGMEM = { "Babystepping Y" }; //MSG_BABYSTEPPING_Y
const char MSGES215[] PROGMEM = { "Babystepping Y" }; //MSG_BABYSTEPPING_Y
const char MSGPL215[] PROGMEM = { "Babystepping Y" }; //MSG_BABYSTEPPING_Y
const char MSGEN216[] PROGMEM = { "Adjusting Z" }; //MSG_BABYSTEPPING_Z
const char MSGCZ216[] PROGMEM = { "Dostavovani Z" }; //MSG_BABYSTEPPING_Z
const char MSGIT216[] PROGMEM = { "Adjusting Z" }; //MSG_BABYSTEPPING_Z
const char MSGES216[] PROGMEM = { "Adjusting Z" }; //MSG_BABYSTEPPING_Z
const char MSGPL216[] PROGMEM = { "Dostavovani Z" }; //MSG_BABYSTEPPING_Z
const char MSGEN217[] PROGMEM = { "Error in menu structure" }; //MSG_SERIAL_ERROR_MENU_STRUCTURE
const char MSGCZ217[] PROGMEM = { "Error in menu structure" }; //MSG_SERIAL_ERROR_MENU_STRUCTURE
const char MSGIT217[] PROGMEM = { "Error in menu structure" }; //MSG_SERIAL_ERROR_MENU_STRUCTURE
const char MSGES217[] PROGMEM = { "Error in menu structure" }; //MSG_SERIAL_ERROR_MENU_STRUCTURE
const char MSGPL217[] PROGMEM = { "Error in menu structure" }; //MSG_SERIAL_ERROR_MENU_STRUCTURE
const char MSGEN218[] PROGMEM = { "English" }; //MSG_LANGUAGE_NAME
const char MSGCZ218[] PROGMEM = { "Cestina" }; //MSG_LANGUAGE_NAME
const char MSGIT218[] PROGMEM = { "Italiano" }; //MSG_LANGUAGE_NAME
const char MSGES218[] PROGMEM = { "Espanol" }; //MSG_LANGUAGE_NAME
const char MSGPL218[] PROGMEM = { "Polski" }; //MSG_LANGUAGE_NAME
const char MSGEN219[] PROGMEM = { "Select language     " }; //MSG_LANGUAGE_SELECT
const char MSGCZ219[] PROGMEM = { "Vyber jazyka        " }; //MSG_LANGUAGE_SELECT
const char MSGIT219[] PROGMEM = { "Selez. la lingua" }; //MSG_LANGUAGE_SELECT
const char MSGES219[] PROGMEM = { "Cambia la lengua " }; //MSG_LANGUAGE_SELECT
const char MSGPL219[] PROGMEM = { "Wybor jezyka        " }; //MSG_LANGUAGE_SELECT
const char MSGEN220[] PROGMEM = { "prusa3d.com" }; //MSG_PRUSA3D
const char MSGCZ220[] PROGMEM = { "prusa3d.cz" }; //MSG_PRUSA3D
const char MSGIT220[] PROGMEM = { "prusa3d.com" }; //MSG_PRUSA3D
const char MSGES220[] PROGMEM = { "prusa3d.com" }; //MSG_PRUSA3D
const char MSGPL220[] PROGMEM = { "prusa3d.cz" }; //MSG_PRUSA3D
const char MSGEN221[] PROGMEM = { "forum.prusa3d.com" }; //MSG_PRUSA3D_FORUM
const char MSGCZ221[] PROGMEM = { "forum.prusa3d.cz" }; //MSG_PRUSA3D_FORUM
const char MSGIT221[] PROGMEM = { "forum.prusa3d.com" }; //MSG_PRUSA3D_FORUM
const char MSGES221[] PROGMEM = { "forum.prusa3d.com" }; //MSG_PRUSA3D_FORUM
const char MSGPL221[] PROGMEM = { "forum.prusa3d.cz" }; //MSG_PRUSA3D_FORUM
const char MSGEN222[] PROGMEM = { "howto.prusa3d.com" }; //MSG_PRUSA3D_HOWTO
const char MSGCZ222[] PROGMEM = { "howto.prusa3d.cz" }; //MSG_PRUSA3D_HOWTO
const char MSGIT222[] PROGMEM = { "howto.prusa3d.com" }; //MSG_PRUSA3D_HOWTO
const char MSGES222[] PROGMEM = { "howto.prusa3d.com" }; //MSG_PRUSA3D_HOWTO
const char MSGPL222[] PROGMEM = { "howto.prusa3d.cz" }; //MSG_PRUSA3D_HOWTO
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
const char* MSGIT[]  = {MSGIT0, //WELCOME_MSG
MSGIT1, //MSG_SD_INSERTED
MSGIT2, //MSG_SD_REMOVED
MSGIT3, //MSG_MAIN
MSGIT4, //MSG_AUTOSTART
MSGIT5, //MSG_DISABLE_STEPPERS
MSGIT6, //MSG_AUTO_HOME
MSGIT7, //MSG_SET_HOME_OFFSETS
MSGIT8, //MSG_SET_ORIGIN
MSGIT9, //MSG_PREHEAT_PLA
MSGIT10, //MSG_PREHEAT_PLA0
MSGIT11, //MSG_PREHEAT_PLA1
MSGIT12, //MSG_PREHEAT_PLA2
MSGIT13, //MSG_PREHEAT_PLA012
MSGIT14, //MSG_PREHEAT_PLA_BEDONLY
MSGIT15, //MSG_PREHEAT_PLA_SETTINGS
MSGIT16, //MSG_PREHEAT_ABS
MSGIT17, //MSG_PREHEAT_ABS0
MSGIT18, //MSG_PREHEAT_ABS1
MSGIT19, //MSG_PREHEAT_ABS2
MSGIT20, //MSG_PREHEAT_ABS012
MSGIT21, //MSG_PREHEAT_ABS_BEDONLY
MSGIT22, //MSG_PREHEAT_ABS_SETTINGS
MSGIT23, //MSG_COOLDOWN
MSGIT24, //MSG_SWITCH_PS_ON
MSGIT25, //MSG_SWITCH_PS_OFF
MSGIT26, //MSG_EXTRUDE
MSGIT27, //MSG_RETRACT
MSGIT28, //MSG_MOVE_AXIS
MSGIT29, //MSG_MOVE_X
MSGIT30, //MSG_MOVE_Y
MSGIT31, //MSG_MOVE_Z
MSGIT32, //MSG_MOVE_E
MSGIT33, //MSG_MOVE_E1
MSGIT34, //MSG_MOVE_E2
MSGIT35, //MSG_MOVE_01MM
MSGIT36, //MSG_MOVE_1MM
MSGIT37, //MSG_MOVE_10MM
MSGIT38, //MSG_SPEED
MSGIT39, //MSG_NOZZLE
MSGIT40, //MSG_NOZZLE1
MSGIT41, //MSG_NOZZLE2
MSGIT42, //MSG_BED
MSGIT43, //MSG_FAN_SPEED
MSGIT44, //MSG_FLOW
MSGIT45, //MSG_FLOW0
MSGIT46, //MSG_FLOW1
MSGIT47, //MSG_FLOW2
MSGIT48, //MSG_CONTROL
MSGIT49, //MSG_MIN
MSGIT50, //MSG_MAX
MSGIT51, //MSG_FACTOR
MSGIT52, //MSG_AUTOTEMP
MSGIT53, //MSG_ON
MSGIT54, //MSG_OFF
MSGIT55, //MSG_PID_P
MSGIT56, //MSG_PID_I
MSGIT57, //MSG_PID_D
MSGIT58, //MSG_PID_C
MSGIT59, //MSG_ACC
MSGIT60, //MSG_VXY_JERK
MSGIT61, //MSG_VZ_JERK
MSGIT62, //MSG_VE_JERK
MSGIT63, //MSG_VMAX
MSGIT64, //MSG_X
MSGIT65, //MSG_Y
MSGIT66, //MSG_Z
MSGIT67, //MSG_E
MSGIT68, //MSG_VMIN
MSGIT69, //MSG_VTRAV_MIN
MSGIT70, //MSG_AMAX
MSGIT71, //MSG_A_RETRACT
MSGIT72, //MSG_XSTEPS
MSGIT73, //MSG_YSTEPS
MSGIT74, //MSG_ZSTEPS
MSGIT75, //MSG_ESTEPS
MSGIT76, //MSG_TEMPERATURE
MSGIT77, //MSG_MOTION
MSGIT78, //MSG_VOLUMETRIC
MSGIT79, //MSG_VOLUMETRIC_ENABLED
MSGIT80, //MSG_FILAMENT_SIZE_EXTRUDER_0
MSGIT81, //MSG_FILAMENT_SIZE_EXTRUDER_1
MSGIT82, //MSG_FILAMENT_SIZE_EXTRUDER_2
MSGIT83, //MSG_CONTRAST
MSGIT84, //MSG_STORE_EPROM
MSGIT85, //MSG_LOAD_EPROM
MSGIT86, //MSG_RESTORE_FAILSAFE
MSGIT87, //MSG_REFRESH
MSGIT88, //MSG_WATCH
MSGIT89, //MSG_PREPARE
MSGIT90, //MSG_TUNE
MSGIT91, //MSG_PAUSE_PRINT
MSGIT92, //MSG_RESUME_PRINT
MSGIT93, //MSG_STOP_PRINT
MSGIT94, //MSG_CARD_MENU
MSGIT95, //MSG_NO_CARD
MSGIT96, //MSG_DWELL
MSGIT97, //MSG_USERWAIT
MSGIT98, //MSG_RESUMING
MSGIT99, //MSG_PRINT_ABORTED
MSGIT100, //MSG_NO_MOVE
MSGIT101, //MSG_KILLED
MSGIT102, //MSG_STOPPED
MSGIT103, //MSG_CONTROL_RETRACT
MSGIT104, //MSG_CONTROL_RETRACT_SWAP
MSGIT105, //MSG_CONTROL_RETRACTF
MSGIT106, //MSG_CONTROL_RETRACT_ZLIFT
MSGIT107, //MSG_CONTROL_RETRACT_RECOVER
MSGIT108, //MSG_CONTROL_RETRACT_RECOVER_SWAP
MSGIT109, //MSG_CONTROL_RETRACT_RECOVERF
MSGIT110, //MSG_AUTORETRACT
MSGIT111, //MSG_FILAMENTCHANGE
MSGIT112, //MSG_INIT_SDCARD
MSGIT113, //MSG_CNG_SDCARD
MSGIT114, //MSG_ZPROBE_OUT
MSGIT115, //MSG_POSITION_UNKNOWN
MSGIT116, //MSG_ZPROBE_ZOFFSET
MSGIT117, //MSG_BABYSTEP_X
MSGIT118, //MSG_BABYSTEP_Y
MSGIT119, //MSG_BABYSTEP_Z
MSGIT120, //MSG_ENDSTOP_ABORT
MSGIT121, //MSG_ADJUSTZ
MSGIT122, //MSG_HOMEYZ
MSGIT123, //MSG_SETTINGS
MSGIT124, //MSG_PREHEAT
MSGIT125, //MSG_UNLOAD_FILAMENT
MSGIT126, //MSG_LOAD_FILAMENT
MSGIT127, //MSG_RECTRACT
MSGIT128, //MSG_ERROR
MSGIT129, //MSG_PREHEAT_NOZZLE
MSGIT130, //MSG_SUPPORT
MSGIT131, //MSG_CORRECTLY
MSGIT132, //MSG_YES
MSGIT133, //MSG_NO
MSGIT134, //MSG_NOT_LOADED
MSGIT135, //MSG_NOT_COLOR
MSGIT136, //MSG_LOADING_FILAMENT
MSGIT137, //MSG_PLEASE_WAIT
MSGIT138, //MSG_LOADING_COLOR
MSGIT139, //MSG_CHANGE_SUCCESS
MSGIT140, //MSG_PRESS
MSGIT141, //MSG_INSERT_FILAMENT
MSGIT142, //MSG_CHANGING_FILAMENT
MSGIT143, //MSG_SILENT_MODE_ON
MSGIT144, //MSG_SILENT_MODE_OFF
MSGIT145, //MSG_REBOOT
MSGIT146, //MSG_TAKE_EFFECT
MSGIT147, //MSG_Enqueing
MSGIT148, //MSG_POWERUP
MSGIT149, //MSG_EXTERNAL_RESET
MSGIT150, //MSG_BROWNOUT_RESET
MSGIT151, //MSG_WATCHDOG_RESET
MSGIT152, //MSG_SOFTWARE_RESET
MSGIT153, //MSG_AUTHOR
MSGIT154, //MSG_CONFIGURATION_VER
MSGIT155, //MSG_FREE_MEMORY
MSGIT156, //MSG_PLANNER_BUFFER_BYTES
MSGIT157, //MSG_OK
MSGIT158, //MSG_FILE_SAVED
MSGIT159, //MSG_ERR_LINE_NO
MSGIT160, //MSG_ERR_CHECKSUM_MISMATCH
MSGIT161, //MSG_ERR_NO_CHECKSUM
MSGIT162, //MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM
MSGIT163, //MSG_FILE_PRINTED
MSGIT164, //MSG_BEGIN_FILE_LIST
MSGIT165, //MSG_END_FILE_LIST
MSGIT166, //MSG_M104_INVALID_EXTRUDER
MSGIT167, //MSG_M105_INVALID_EXTRUDER
MSGIT168, //MSG_M200_INVALID_EXTRUDER
MSGIT169, //MSG_M218_INVALID_EXTRUDER
MSGIT170, //MSG_M221_INVALID_EXTRUDER
MSGIT171, //MSG_ERR_NO_THERMISTORS
MSGIT172, //MSG_M109_INVALID_EXTRUDER
MSGIT173, //MSG_HEATING
MSGIT174, //MSG_HEATING_COMPLETE
MSGIT175, //MSG_BED_HEATING
MSGIT176, //MSG_BED_DONE
MSGIT177, //MSG_M115_REPORT
MSGIT178, //MSG_COUNT_X
MSGIT179, //MSG_ERR_KILLED
MSGIT180, //MSG_ERR_STOPPED
MSGIT181, //MSG_RESEND
MSGIT182, //MSG_UNKNOWN_COMMAND
MSGIT183, //MSG_ACTIVE_EXTRUDER
MSGIT184, //MSG_INVALID_EXTRUDER
MSGIT185, //MSG_X_MIN
MSGIT186, //MSG_X_MAX
MSGIT187, //MSG_Y_MIN
MSGIT188, //MSG_Y_MAX
MSGIT189, //MSG_Z_MIN
MSGIT190, //MSG_Z_MAX
MSGIT191, //MSG_M119_REPORT
MSGIT192, //MSG_ENDSTOP_HIT
MSGIT193, //MSG_ENDSTOP_OPEN
MSGIT194, //MSG_HOTEND_OFFSET
MSGIT195, //MSG_SD_CANT_OPEN_SUBDIR
MSGIT196, //MSG_SD_INIT_FAIL
MSGIT197, //MSG_SD_VOL_INIT_FAIL
MSGIT198, //MSG_SD_OPENROOT_FAIL
MSGIT199, //MSG_SD_CARD_OK
MSGIT200, //MSG_SD_WORKDIR_FAIL
MSGIT201, //MSG_SD_OPEN_FILE_FAIL
MSGIT202, //MSG_SD_FILE_OPENED
MSGIT203, //MSG_SD_SIZE
MSGIT204, //MSG_SD_FILE_SELECTED
MSGIT205, //MSG_SD_WRITE_TO_FILE
MSGIT206, //MSG_SD_PRINTING_BYTE
MSGIT207, //MSG_SD_NOT_PRINTING
MSGIT208, //MSG_SD_ERR_WRITE_TO_FILE
MSGIT209, //MSG_SD_CANT_ENTER_SUBDIR
MSGIT210, //MSG_STEPPER_TOO_HIGH
MSGIT211, //MSG_ENDSTOPS_HIT
MSGIT212, //MSG_ERR_COLD_EXTRUDE_STOP
MSGIT213, //MSG_ERR_LONG_EXTRUDE_STOP
MSGIT214, //MSG_BABYSTEPPING_X
MSGIT215, //MSG_BABYSTEPPING_Y
MSGIT216, //MSG_BABYSTEPPING_Z
MSGIT217, //MSG_SERIAL_ERROR_MENU_STRUCTURE
MSGIT218, //MSG_LANGUAGE_NAME
MSGIT219, //MSG_LANGUAGE_SELECT
MSGIT220, //MSG_PRUSA3D
MSGIT221, //MSG_PRUSA3D_FORUM
MSGIT222, //MSG_PRUSA3D_HOWTO
};
const char* MSGES[]  = {MSGES0, //WELCOME_MSG
MSGES1, //MSG_SD_INSERTED
MSGES2, //MSG_SD_REMOVED
MSGES3, //MSG_MAIN
MSGES4, //MSG_AUTOSTART
MSGES5, //MSG_DISABLE_STEPPERS
MSGES6, //MSG_AUTO_HOME
MSGES7, //MSG_SET_HOME_OFFSETS
MSGES8, //MSG_SET_ORIGIN
MSGES9, //MSG_PREHEAT_PLA
MSGES10, //MSG_PREHEAT_PLA0
MSGES11, //MSG_PREHEAT_PLA1
MSGES12, //MSG_PREHEAT_PLA2
MSGES13, //MSG_PREHEAT_PLA012
MSGES14, //MSG_PREHEAT_PLA_BEDONLY
MSGES15, //MSG_PREHEAT_PLA_SETTINGS
MSGES16, //MSG_PREHEAT_ABS
MSGES17, //MSG_PREHEAT_ABS0
MSGES18, //MSG_PREHEAT_ABS1
MSGES19, //MSG_PREHEAT_ABS2
MSGES20, //MSG_PREHEAT_ABS012
MSGES21, //MSG_PREHEAT_ABS_BEDONLY
MSGES22, //MSG_PREHEAT_ABS_SETTINGS
MSGES23, //MSG_COOLDOWN
MSGES24, //MSG_SWITCH_PS_ON
MSGES25, //MSG_SWITCH_PS_OFF
MSGES26, //MSG_EXTRUDE
MSGES27, //MSG_RETRACT
MSGES28, //MSG_MOVE_AXIS
MSGES29, //MSG_MOVE_X
MSGES30, //MSG_MOVE_Y
MSGES31, //MSG_MOVE_Z
MSGES32, //MSG_MOVE_E
MSGES33, //MSG_MOVE_E1
MSGES34, //MSG_MOVE_E2
MSGES35, //MSG_MOVE_01MM
MSGES36, //MSG_MOVE_1MM
MSGES37, //MSG_MOVE_10MM
MSGES38, //MSG_SPEED
MSGES39, //MSG_NOZZLE
MSGES40, //MSG_NOZZLE1
MSGES41, //MSG_NOZZLE2
MSGES42, //MSG_BED
MSGES43, //MSG_FAN_SPEED
MSGES44, //MSG_FLOW
MSGES45, //MSG_FLOW0
MSGES46, //MSG_FLOW1
MSGES47, //MSG_FLOW2
MSGES48, //MSG_CONTROL
MSGES49, //MSG_MIN
MSGES50, //MSG_MAX
MSGES51, //MSG_FACTOR
MSGES52, //MSG_AUTOTEMP
MSGES53, //MSG_ON
MSGES54, //MSG_OFF
MSGES55, //MSG_PID_P
MSGES56, //MSG_PID_I
MSGES57, //MSG_PID_D
MSGES58, //MSG_PID_C
MSGES59, //MSG_ACC
MSGES60, //MSG_VXY_JERK
MSGES61, //MSG_VZ_JERK
MSGES62, //MSG_VE_JERK
MSGES63, //MSG_VMAX
MSGES64, //MSG_X
MSGES65, //MSG_Y
MSGES66, //MSG_Z
MSGES67, //MSG_E
MSGES68, //MSG_VMIN
MSGES69, //MSG_VTRAV_MIN
MSGES70, //MSG_AMAX
MSGES71, //MSG_A_RETRACT
MSGES72, //MSG_XSTEPS
MSGES73, //MSG_YSTEPS
MSGES74, //MSG_ZSTEPS
MSGES75, //MSG_ESTEPS
MSGES76, //MSG_TEMPERATURE
MSGES77, //MSG_MOTION
MSGES78, //MSG_VOLUMETRIC
MSGES79, //MSG_VOLUMETRIC_ENABLED
MSGES80, //MSG_FILAMENT_SIZE_EXTRUDER_0
MSGES81, //MSG_FILAMENT_SIZE_EXTRUDER_1
MSGES82, //MSG_FILAMENT_SIZE_EXTRUDER_2
MSGES83, //MSG_CONTRAST
MSGES84, //MSG_STORE_EPROM
MSGES85, //MSG_LOAD_EPROM
MSGES86, //MSG_RESTORE_FAILSAFE
MSGES87, //MSG_REFRESH
MSGES88, //MSG_WATCH
MSGES89, //MSG_PREPARE
MSGES90, //MSG_TUNE
MSGES91, //MSG_PAUSE_PRINT
MSGES92, //MSG_RESUME_PRINT
MSGES93, //MSG_STOP_PRINT
MSGES94, //MSG_CARD_MENU
MSGES95, //MSG_NO_CARD
MSGES96, //MSG_DWELL
MSGES97, //MSG_USERWAIT
MSGES98, //MSG_RESUMING
MSGES99, //MSG_PRINT_ABORTED
MSGES100, //MSG_NO_MOVE
MSGES101, //MSG_KILLED
MSGES102, //MSG_STOPPED
MSGES103, //MSG_CONTROL_RETRACT
MSGES104, //MSG_CONTROL_RETRACT_SWAP
MSGES105, //MSG_CONTROL_RETRACTF
MSGES106, //MSG_CONTROL_RETRACT_ZLIFT
MSGES107, //MSG_CONTROL_RETRACT_RECOVER
MSGES108, //MSG_CONTROL_RETRACT_RECOVER_SWAP
MSGES109, //MSG_CONTROL_RETRACT_RECOVERF
MSGES110, //MSG_AUTORETRACT
MSGES111, //MSG_FILAMENTCHANGE
MSGES112, //MSG_INIT_SDCARD
MSGES113, //MSG_CNG_SDCARD
MSGES114, //MSG_ZPROBE_OUT
MSGES115, //MSG_POSITION_UNKNOWN
MSGES116, //MSG_ZPROBE_ZOFFSET
MSGES117, //MSG_BABYSTEP_X
MSGES118, //MSG_BABYSTEP_Y
MSGES119, //MSG_BABYSTEP_Z
MSGES120, //MSG_ENDSTOP_ABORT
MSGES121, //MSG_ADJUSTZ
MSGES122, //MSG_HOMEYZ
MSGES123, //MSG_SETTINGS
MSGES124, //MSG_PREHEAT
MSGES125, //MSG_UNLOAD_FILAMENT
MSGES126, //MSG_LOAD_FILAMENT
MSGES127, //MSG_RECTRACT
MSGES128, //MSG_ERROR
MSGES129, //MSG_PREHEAT_NOZZLE
MSGES130, //MSG_SUPPORT
MSGES131, //MSG_CORRECTLY
MSGES132, //MSG_YES
MSGES133, //MSG_NO
MSGES134, //MSG_NOT_LOADED
MSGES135, //MSG_NOT_COLOR
MSGES136, //MSG_LOADING_FILAMENT
MSGES137, //MSG_PLEASE_WAIT
MSGES138, //MSG_LOADING_COLOR
MSGES139, //MSG_CHANGE_SUCCESS
MSGES140, //MSG_PRESS
MSGES141, //MSG_INSERT_FILAMENT
MSGES142, //MSG_CHANGING_FILAMENT
MSGES143, //MSG_SILENT_MODE_ON
MSGES144, //MSG_SILENT_MODE_OFF
MSGES145, //MSG_REBOOT
MSGES146, //MSG_TAKE_EFFECT
MSGES147, //MSG_Enqueing
MSGES148, //MSG_POWERUP
MSGES149, //MSG_EXTERNAL_RESET
MSGES150, //MSG_BROWNOUT_RESET
MSGES151, //MSG_WATCHDOG_RESET
MSGES152, //MSG_SOFTWARE_RESET
MSGES153, //MSG_AUTHOR
MSGES154, //MSG_CONFIGURATION_VER
MSGES155, //MSG_FREE_MEMORY
MSGES156, //MSG_PLANNER_BUFFER_BYTES
MSGES157, //MSG_OK
MSGES158, //MSG_FILE_SAVED
MSGES159, //MSG_ERR_LINE_NO
MSGES160, //MSG_ERR_CHECKSUM_MISMATCH
MSGES161, //MSG_ERR_NO_CHECKSUM
MSGES162, //MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM
MSGES163, //MSG_FILE_PRINTED
MSGES164, //MSG_BEGIN_FILE_LIST
MSGES165, //MSG_END_FILE_LIST
MSGES166, //MSG_M104_INVALID_EXTRUDER
MSGES167, //MSG_M105_INVALID_EXTRUDER
MSGES168, //MSG_M200_INVALID_EXTRUDER
MSGES169, //MSG_M218_INVALID_EXTRUDER
MSGES170, //MSG_M221_INVALID_EXTRUDER
MSGES171, //MSG_ERR_NO_THERMISTORS
MSGES172, //MSG_M109_INVALID_EXTRUDER
MSGES173, //MSG_HEATING
MSGES174, //MSG_HEATING_COMPLETE
MSGES175, //MSG_BED_HEATING
MSGES176, //MSG_BED_DONE
MSGES177, //MSG_M115_REPORT
MSGES178, //MSG_COUNT_X
MSGES179, //MSG_ERR_KILLED
MSGES180, //MSG_ERR_STOPPED
MSGES181, //MSG_RESEND
MSGES182, //MSG_UNKNOWN_COMMAND
MSGES183, //MSG_ACTIVE_EXTRUDER
MSGES184, //MSG_INVALID_EXTRUDER
MSGES185, //MSG_X_MIN
MSGES186, //MSG_X_MAX
MSGES187, //MSG_Y_MIN
MSGES188, //MSG_Y_MAX
MSGES189, //MSG_Z_MIN
MSGES190, //MSG_Z_MAX
MSGES191, //MSG_M119_REPORT
MSGES192, //MSG_ENDSTOP_HIT
MSGES193, //MSG_ENDSTOP_OPEN
MSGES194, //MSG_HOTEND_OFFSET
MSGES195, //MSG_SD_CANT_OPEN_SUBDIR
MSGES196, //MSG_SD_INIT_FAIL
MSGES197, //MSG_SD_VOL_INIT_FAIL
MSGES198, //MSG_SD_OPENROOT_FAIL
MSGES199, //MSG_SD_CARD_OK
MSGES200, //MSG_SD_WORKDIR_FAIL
MSGES201, //MSG_SD_OPEN_FILE_FAIL
MSGES202, //MSG_SD_FILE_OPENED
MSGES203, //MSG_SD_SIZE
MSGES204, //MSG_SD_FILE_SELECTED
MSGES205, //MSG_SD_WRITE_TO_FILE
MSGES206, //MSG_SD_PRINTING_BYTE
MSGES207, //MSG_SD_NOT_PRINTING
MSGES208, //MSG_SD_ERR_WRITE_TO_FILE
MSGES209, //MSG_SD_CANT_ENTER_SUBDIR
MSGES210, //MSG_STEPPER_TOO_HIGH
MSGES211, //MSG_ENDSTOPS_HIT
MSGES212, //MSG_ERR_COLD_EXTRUDE_STOP
MSGES213, //MSG_ERR_LONG_EXTRUDE_STOP
MSGES214, //MSG_BABYSTEPPING_X
MSGES215, //MSG_BABYSTEPPING_Y
MSGES216, //MSG_BABYSTEPPING_Z
MSGES217, //MSG_SERIAL_ERROR_MENU_STRUCTURE
MSGES218, //MSG_LANGUAGE_NAME
MSGES219, //MSG_LANGUAGE_SELECT
MSGES220, //MSG_PRUSA3D
MSGES221, //MSG_PRUSA3D_FORUM
MSGES222, //MSG_PRUSA3D_HOWTO
};
const char* MSGPL[]  = {MSGPL0, //WELCOME_MSG
MSGPL1, //MSG_SD_INSERTED
MSGPL2, //MSG_SD_REMOVED
MSGPL3, //MSG_MAIN
MSGPL4, //MSG_AUTOSTART
MSGPL5, //MSG_DISABLE_STEPPERS
MSGPL6, //MSG_AUTO_HOME
MSGPL7, //MSG_SET_HOME_OFFSETS
MSGPL8, //MSG_SET_ORIGIN
MSGPL9, //MSG_PREHEAT_PLA
MSGPL10, //MSG_PREHEAT_PLA0
MSGPL11, //MSG_PREHEAT_PLA1
MSGPL12, //MSG_PREHEAT_PLA2
MSGPL13, //MSG_PREHEAT_PLA012
MSGPL14, //MSG_PREHEAT_PLA_BEDONLY
MSGPL15, //MSG_PREHEAT_PLA_SETTINGS
MSGPL16, //MSG_PREHEAT_ABS
MSGPL17, //MSG_PREHEAT_ABS0
MSGPL18, //MSG_PREHEAT_ABS1
MSGPL19, //MSG_PREHEAT_ABS2
MSGPL20, //MSG_PREHEAT_ABS012
MSGPL21, //MSG_PREHEAT_ABS_BEDONLY
MSGPL22, //MSG_PREHEAT_ABS_SETTINGS
MSGPL23, //MSG_COOLDOWN
MSGPL24, //MSG_SWITCH_PS_ON
MSGPL25, //MSG_SWITCH_PS_OFF
MSGPL26, //MSG_EXTRUDE
MSGPL27, //MSG_RETRACT
MSGPL28, //MSG_MOVE_AXIS
MSGPL29, //MSG_MOVE_X
MSGPL30, //MSG_MOVE_Y
MSGPL31, //MSG_MOVE_Z
MSGPL32, //MSG_MOVE_E
MSGPL33, //MSG_MOVE_E1
MSGPL34, //MSG_MOVE_E2
MSGPL35, //MSG_MOVE_01MM
MSGPL36, //MSG_MOVE_1MM
MSGPL37, //MSG_MOVE_10MM
MSGPL38, //MSG_SPEED
MSGPL39, //MSG_NOZZLE
MSGPL40, //MSG_NOZZLE1
MSGPL41, //MSG_NOZZLE2
MSGPL42, //MSG_BED
MSGPL43, //MSG_FAN_SPEED
MSGPL44, //MSG_FLOW
MSGPL45, //MSG_FLOW0
MSGPL46, //MSG_FLOW1
MSGPL47, //MSG_FLOW2
MSGPL48, //MSG_CONTROL
MSGPL49, //MSG_MIN
MSGPL50, //MSG_MAX
MSGPL51, //MSG_FACTOR
MSGPL52, //MSG_AUTOTEMP
MSGPL53, //MSG_ON
MSGPL54, //MSG_OFF
MSGPL55, //MSG_PID_P
MSGPL56, //MSG_PID_I
MSGPL57, //MSG_PID_D
MSGPL58, //MSG_PID_C
MSGPL59, //MSG_ACC
MSGPL60, //MSG_VXY_JERK
MSGPL61, //MSG_VZ_JERK
MSGPL62, //MSG_VE_JERK
MSGPL63, //MSG_VMAX
MSGPL64, //MSG_X
MSGPL65, //MSG_Y
MSGPL66, //MSG_Z
MSGPL67, //MSG_E
MSGPL68, //MSG_VMIN
MSGPL69, //MSG_VTRAV_MIN
MSGPL70, //MSG_AMAX
MSGPL71, //MSG_A_RETRACT
MSGPL72, //MSG_XSTEPS
MSGPL73, //MSG_YSTEPS
MSGPL74, //MSG_ZSTEPS
MSGPL75, //MSG_ESTEPS
MSGPL76, //MSG_TEMPERATURE
MSGPL77, //MSG_MOTION
MSGPL78, //MSG_VOLUMETRIC
MSGPL79, //MSG_VOLUMETRIC_ENABLED
MSGPL80, //MSG_FILAMENT_SIZE_EXTRUDER_0
MSGPL81, //MSG_FILAMENT_SIZE_EXTRUDER_1
MSGPL82, //MSG_FILAMENT_SIZE_EXTRUDER_2
MSGPL83, //MSG_CONTRAST
MSGPL84, //MSG_STORE_EPROM
MSGPL85, //MSG_LOAD_EPROM
MSGPL86, //MSG_RESTORE_FAILSAFE
MSGPL87, //MSG_REFRESH
MSGPL88, //MSG_WATCH
MSGPL89, //MSG_PREPARE
MSGPL90, //MSG_TUNE
MSGPL91, //MSG_PAUSE_PRINT
MSGPL92, //MSG_RESUME_PRINT
MSGPL93, //MSG_STOP_PRINT
MSGPL94, //MSG_CARD_MENU
MSGPL95, //MSG_NO_CARD
MSGPL96, //MSG_DWELL
MSGPL97, //MSG_USERWAIT
MSGPL98, //MSG_RESUMING
MSGPL99, //MSG_PRINT_ABORTED
MSGPL100, //MSG_NO_MOVE
MSGPL101, //MSG_KILLED
MSGPL102, //MSG_STOPPED
MSGPL103, //MSG_CONTROL_RETRACT
MSGPL104, //MSG_CONTROL_RETRACT_SWAP
MSGPL105, //MSG_CONTROL_RETRACTF
MSGPL106, //MSG_CONTROL_RETRACT_ZLIFT
MSGPL107, //MSG_CONTROL_RETRACT_RECOVER
MSGPL108, //MSG_CONTROL_RETRACT_RECOVER_SWAP
MSGPL109, //MSG_CONTROL_RETRACT_RECOVERF
MSGPL110, //MSG_AUTORETRACT
MSGPL111, //MSG_FILAMENTCHANGE
MSGPL112, //MSG_INIT_SDCARD
MSGPL113, //MSG_CNG_SDCARD
MSGPL114, //MSG_ZPROBE_OUT
MSGPL115, //MSG_POSITION_UNKNOWN
MSGPL116, //MSG_ZPROBE_ZOFFSET
MSGPL117, //MSG_BABYSTEP_X
MSGPL118, //MSG_BABYSTEP_Y
MSGPL119, //MSG_BABYSTEP_Z
MSGPL120, //MSG_ENDSTOP_ABORT
MSGPL121, //MSG_ADJUSTZ
MSGPL122, //MSG_HOMEYZ
MSGPL123, //MSG_SETTINGS
MSGPL124, //MSG_PREHEAT
MSGPL125, //MSG_UNLOAD_FILAMENT
MSGPL126, //MSG_LOAD_FILAMENT
MSGPL127, //MSG_RECTRACT
MSGPL128, //MSG_ERROR
MSGPL129, //MSG_PREHEAT_NOZZLE
MSGPL130, //MSG_SUPPORT
MSGPL131, //MSG_CORRECTLY
MSGPL132, //MSG_YES
MSGPL133, //MSG_NO
MSGPL134, //MSG_NOT_LOADED
MSGPL135, //MSG_NOT_COLOR
MSGPL136, //MSG_LOADING_FILAMENT
MSGPL137, //MSG_PLEASE_WAIT
MSGPL138, //MSG_LOADING_COLOR
MSGPL139, //MSG_CHANGE_SUCCESS
MSGPL140, //MSG_PRESS
MSGPL141, //MSG_INSERT_FILAMENT
MSGPL142, //MSG_CHANGING_FILAMENT
MSGPL143, //MSG_SILENT_MODE_ON
MSGPL144, //MSG_SILENT_MODE_OFF
MSGPL145, //MSG_REBOOT
MSGPL146, //MSG_TAKE_EFFECT
MSGPL147, //MSG_Enqueing
MSGPL148, //MSG_POWERUP
MSGPL149, //MSG_EXTERNAL_RESET
MSGPL150, //MSG_BROWNOUT_RESET
MSGPL151, //MSG_WATCHDOG_RESET
MSGPL152, //MSG_SOFTWARE_RESET
MSGPL153, //MSG_AUTHOR
MSGPL154, //MSG_CONFIGURATION_VER
MSGPL155, //MSG_FREE_MEMORY
MSGPL156, //MSG_PLANNER_BUFFER_BYTES
MSGPL157, //MSG_OK
MSGPL158, //MSG_FILE_SAVED
MSGPL159, //MSG_ERR_LINE_NO
MSGPL160, //MSG_ERR_CHECKSUM_MISMATCH
MSGPL161, //MSG_ERR_NO_CHECKSUM
MSGPL162, //MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM
MSGPL163, //MSG_FILE_PRINTED
MSGPL164, //MSG_BEGIN_FILE_LIST
MSGPL165, //MSG_END_FILE_LIST
MSGPL166, //MSG_M104_INVALID_EXTRUDER
MSGPL167, //MSG_M105_INVALID_EXTRUDER
MSGPL168, //MSG_M200_INVALID_EXTRUDER
MSGPL169, //MSG_M218_INVALID_EXTRUDER
MSGPL170, //MSG_M221_INVALID_EXTRUDER
MSGPL171, //MSG_ERR_NO_THERMISTORS
MSGPL172, //MSG_M109_INVALID_EXTRUDER
MSGPL173, //MSG_HEATING
MSGPL174, //MSG_HEATING_COMPLETE
MSGPL175, //MSG_BED_HEATING
MSGPL176, //MSG_BED_DONE
MSGPL177, //MSG_M115_REPORT
MSGPL178, //MSG_COUNT_X
MSGPL179, //MSG_ERR_KILLED
MSGPL180, //MSG_ERR_STOPPED
MSGPL181, //MSG_RESEND
MSGPL182, //MSG_UNKNOWN_COMMAND
MSGPL183, //MSG_ACTIVE_EXTRUDER
MSGPL184, //MSG_INVALID_EXTRUDER
MSGPL185, //MSG_X_MIN
MSGPL186, //MSG_X_MAX
MSGPL187, //MSG_Y_MIN
MSGPL188, //MSG_Y_MAX
MSGPL189, //MSG_Z_MIN
MSGPL190, //MSG_Z_MAX
MSGPL191, //MSG_M119_REPORT
MSGPL192, //MSG_ENDSTOP_HIT
MSGPL193, //MSG_ENDSTOP_OPEN
MSGPL194, //MSG_HOTEND_OFFSET
MSGPL195, //MSG_SD_CANT_OPEN_SUBDIR
MSGPL196, //MSG_SD_INIT_FAIL
MSGPL197, //MSG_SD_VOL_INIT_FAIL
MSGPL198, //MSG_SD_OPENROOT_FAIL
MSGPL199, //MSG_SD_CARD_OK
MSGPL200, //MSG_SD_WORKDIR_FAIL
MSGPL201, //MSG_SD_OPEN_FILE_FAIL
MSGPL202, //MSG_SD_FILE_OPENED
MSGPL203, //MSG_SD_SIZE
MSGPL204, //MSG_SD_FILE_SELECTED
MSGPL205, //MSG_SD_WRITE_TO_FILE
MSGPL206, //MSG_SD_PRINTING_BYTE
MSGPL207, //MSG_SD_NOT_PRINTING
MSGPL208, //MSG_SD_ERR_WRITE_TO_FILE
MSGPL209, //MSG_SD_CANT_ENTER_SUBDIR
MSGPL210, //MSG_STEPPER_TOO_HIGH
MSGPL211, //MSG_ENDSTOPS_HIT
MSGPL212, //MSG_ERR_COLD_EXTRUDE_STOP
MSGPL213, //MSG_ERR_LONG_EXTRUDE_STOP
MSGPL214, //MSG_BABYSTEPPING_X
MSGPL215, //MSG_BABYSTEPPING_Y
MSGPL216, //MSG_BABYSTEPPING_Z
MSGPL217, //MSG_SERIAL_ERROR_MENU_STRUCTURE
MSGPL218, //MSG_LANGUAGE_NAME
MSGPL219, //MSG_LANGUAGE_SELECT
MSGPL220, //MSG_PRUSA3D
MSGPL221, //MSG_PRUSA3D_FORUM
MSGPL222, //MSG_PRUSA3D_HOWTO
};


const char** MSG_ALL[] = {MSGEN,MSGCZ,MSGIT,MSGES,MSGPL};
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