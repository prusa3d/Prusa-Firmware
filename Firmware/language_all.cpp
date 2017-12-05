
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
const char MSG_ADJUSTZ_IT[] PROGMEM = "Autoregolare Z?";
const char MSG_ADJUSTZ_ES[] PROGMEM = "Ajustar Eje Z";
const char MSG_ADJUSTZ_PL[] PROGMEM = "Autodostroic Z?";
const char MSG_ADJUSTZ_DE[] PROGMEM = "Auto Z einstellen?";
const char * const MSG_ADJUSTZ_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ADJUSTZ_EN,
	MSG_ADJUSTZ_CZ,
	MSG_ADJUSTZ_IT,
	MSG_ADJUSTZ_ES,
	MSG_ADJUSTZ_PL,
	MSG_ADJUSTZ_DE
};

const char MSG_ALL_EN[] PROGMEM = "All";
const char MSG_ALL_CZ[] PROGMEM = "Vse";
const char MSG_ALL_IT[] PROGMEM = "Tutti";
const char MSG_ALL_ES[] PROGMEM = "Todos";
const char MSG_ALL_PL[] PROGMEM = "Wszystko";
const char MSG_ALL_DE[] PROGMEM = "Alle";
const char * const MSG_ALL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ALL_EN,
	MSG_ALL_CZ,
	MSG_ALL_IT,
	MSG_ALL_ES,
	MSG_ALL_PL,
	MSG_ALL_DE
};

const char MSG_AMAX_EN[] PROGMEM = "Amax ";
const char * const MSG_AMAX_LANG_TABLE[1] PROGMEM = {
	MSG_AMAX_EN
};

const char MSG_AUTHOR_EN[] PROGMEM = " | Author: ";
const char * const MSG_AUTHOR_LANG_TABLE[1] PROGMEM = {
	MSG_AUTHOR_EN
};

const char MSG_AUTO_HOME_EN[] PROGMEM = "Auto home";
const char MSG_AUTO_HOME_IT[] PROGMEM = "Trova origine";
const char MSG_AUTO_HOME_ES[] PROGMEM = "Llevar al origen";
const char MSG_AUTO_HOME_DE[] PROGMEM = "Startposition";
const char * const MSG_AUTO_HOME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AUTO_HOME_EN,
	MSG_AUTO_HOME_EN,
	MSG_AUTO_HOME_IT,
	MSG_AUTO_HOME_ES,
	MSG_AUTO_HOME_EN,
	MSG_AUTO_HOME_DE
};

const char MSG_AUTO_MODE_ON_EN[] PROGMEM = "Mode [auto power]";
const char MSG_AUTO_MODE_ON_CZ[] PROGMEM = "Mod [automaticky]";
const char MSG_AUTO_MODE_ON_IT[] PROGMEM = "Mode       [auto]";
const char MSG_AUTO_MODE_ON_ES[] PROGMEM = "Modo       [auto]";
const char MSG_AUTO_MODE_ON_PL[] PROGMEM = "Tryb       [auto]";
const char MSG_AUTO_MODE_ON_DE[] PROGMEM = "Mode [Auto]";
const char * const MSG_AUTO_MODE_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_AUTO_MODE_ON_EN,
	MSG_AUTO_MODE_ON_CZ,
	MSG_AUTO_MODE_ON_IT,
	MSG_AUTO_MODE_ON_ES,
	MSG_AUTO_MODE_ON_PL,
	MSG_AUTO_MODE_ON_DE
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
const char MSG_BABYSTEPPING_Z_CZ[] PROGMEM = "Dostavovani Z";
const char MSG_BABYSTEPPING_Z_IT[] PROGMEM = "Compensazione Z";
const char MSG_BABYSTEPPING_Z_ES[] PROGMEM = "Ajustar Z";
const char MSG_BABYSTEPPING_Z_PL[] PROGMEM = "Dostrojenie Z";
const char MSG_BABYSTEPPING_Z_DE[] PROGMEM = "Z wurde eingestellt";
const char * const MSG_BABYSTEPPING_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEPPING_Z_EN,
	MSG_BABYSTEPPING_Z_CZ,
	MSG_BABYSTEPPING_Z_IT,
	MSG_BABYSTEPPING_Z_ES,
	MSG_BABYSTEPPING_Z_PL,
	MSG_BABYSTEPPING_Z_DE
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
const char MSG_BABYSTEP_Z_IT[] PROGMEM = "Compensazione Z";
const char MSG_BABYSTEP_Z_ES[] PROGMEM = "Micropaso Eje Z";
const char MSG_BABYSTEP_Z_PL[] PROGMEM = "Dostrojenie osy Z";
const char MSG_BABYSTEP_Z_DE[] PROGMEM = "Z einstellen";
const char * const MSG_BABYSTEP_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEP_Z_EN,
	MSG_BABYSTEP_Z_CZ,
	MSG_BABYSTEP_Z_IT,
	MSG_BABYSTEP_Z_ES,
	MSG_BABYSTEP_Z_PL,
	MSG_BABYSTEP_Z_DE
};

const char MSG_BABYSTEP_Z_NOT_SET_EN[] PROGMEM = "Distance between tip of the nozzle and the bed surface has not been set yet. Please follow the manual, chapter First steps, section First layer calibration.";
const char MSG_BABYSTEP_Z_NOT_SET_CZ[] PROGMEM = "Neni zkalibrovana vzdalenost trysky od tiskove podlozky. Postupujte prosim podle manualu, kapitola Zaciname, odstavec Nastaveni prvni vrstvy.";
const char MSG_BABYSTEP_Z_NOT_SET_IT[] PROGMEM = "Distanza tra la punta dell'ugello e la superficie del letto non ancora imposta. Si prega di seguire il manuale, capitolo First steps, sezione First layer calibration.";
const char MSG_BABYSTEP_Z_NOT_SET_ES[] PROGMEM = "Distancia entre la punta del nozzle y la superficie de la heatbed aun no fijada. Por favor siga el manual, capitulo First steps, First layer calibration.";
const char MSG_BABYSTEP_Z_NOT_SET_PL[] PROGMEM = "Odleglosc dyszy od podkladki nie jest skalibrowana. Postepuj zgodnie z instrukcja rozdzial Zaczynamy, podrozdzial Kalibracja pierwszej warstwy.";
const char MSG_BABYSTEP_Z_NOT_SET_DE[] PROGMEM = "Der Abstand zwischen der Spitze der Duese und der Bett ist noch nicht eingestellt. Bitte folgen Sie dem Handbuch, First steps, section First layer calibration.";
const char * const MSG_BABYSTEP_Z_NOT_SET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BABYSTEP_Z_NOT_SET_EN,
	MSG_BABYSTEP_Z_NOT_SET_CZ,
	MSG_BABYSTEP_Z_NOT_SET_IT,
	MSG_BABYSTEP_Z_NOT_SET_ES,
	MSG_BABYSTEP_Z_NOT_SET_PL,
	MSG_BABYSTEP_Z_NOT_SET_DE
};

const char MSG_BED_EN[] PROGMEM = "Bed";
const char MSG_BED_IT[] PROGMEM = "Letto";
const char MSG_BED_ES[] PROGMEM = "Heatbed";
const char MSG_BED_PL[] PROGMEM = "Stolik";
const char MSG_BED_DE[] PROGMEM = "Bett";
const char * const MSG_BED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_EN,
	MSG_BED_EN,
	MSG_BED_IT,
	MSG_BED_ES,
	MSG_BED_PL,
	MSG_BED_DE
};

const char MSG_BED_CORRECTION_FRONT_EN[] PROGMEM = "Front side[um]";
const char MSG_BED_CORRECTION_FRONT_CZ[] PROGMEM = "Vpredu [um]";
const char MSG_BED_CORRECTION_FRONT_IT[] PROGMEM = "Fronte    [um]";
const char MSG_BED_CORRECTION_FRONT_ES[] PROGMEM = "Frontal   [um]";
const char MSG_BED_CORRECTION_FRONT_PL[] PROGMEM = "Do przodu [um]";
const char MSG_BED_CORRECTION_FRONT_DE[] PROGMEM = "Vorne     [um]";
const char * const MSG_BED_CORRECTION_FRONT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_FRONT_EN,
	MSG_BED_CORRECTION_FRONT_CZ,
	MSG_BED_CORRECTION_FRONT_IT,
	MSG_BED_CORRECTION_FRONT_ES,
	MSG_BED_CORRECTION_FRONT_PL,
	MSG_BED_CORRECTION_FRONT_DE
};

const char MSG_BED_CORRECTION_LEFT_EN[] PROGMEM = "Left side [um]";
const char MSG_BED_CORRECTION_LEFT_CZ[] PROGMEM = "Vlevo  [um]";
const char MSG_BED_CORRECTION_LEFT_IT[] PROGMEM = "Sinistra  [um]";
const char MSG_BED_CORRECTION_LEFT_ES[] PROGMEM = "Izquierda [um]";
const char MSG_BED_CORRECTION_LEFT_PL[] PROGMEM = "W lewo  [um]";
const char MSG_BED_CORRECTION_LEFT_DE[] PROGMEM = "Links     [um]";
const char * const MSG_BED_CORRECTION_LEFT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_LEFT_EN,
	MSG_BED_CORRECTION_LEFT_CZ,
	MSG_BED_CORRECTION_LEFT_IT,
	MSG_BED_CORRECTION_LEFT_ES,
	MSG_BED_CORRECTION_LEFT_PL,
	MSG_BED_CORRECTION_LEFT_DE
};

const char MSG_BED_CORRECTION_MENU_EN[] PROGMEM = "Bed level correct";
const char MSG_BED_CORRECTION_MENU_CZ[] PROGMEM = "Korekce podlozky";
const char MSG_BED_CORRECTION_MENU_IT[] PROGMEM = "Correz. liv.letto";
const char MSG_BED_CORRECTION_MENU_ES[] PROGMEM = "Corr. de la cama";
const char MSG_BED_CORRECTION_MENU_PL[] PROGMEM = "Korekta podkladki";
const char MSG_BED_CORRECTION_MENU_DE[] PROGMEM = "Bett level Korrekt";
const char * const MSG_BED_CORRECTION_MENU_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_MENU_EN,
	MSG_BED_CORRECTION_MENU_CZ,
	MSG_BED_CORRECTION_MENU_IT,
	MSG_BED_CORRECTION_MENU_ES,
	MSG_BED_CORRECTION_MENU_PL,
	MSG_BED_CORRECTION_MENU_DE
};

const char MSG_BED_CORRECTION_REAR_EN[] PROGMEM = "Rear side [um]";
const char MSG_BED_CORRECTION_REAR_CZ[] PROGMEM = "Vzadu  [um]";
const char MSG_BED_CORRECTION_REAR_IT[] PROGMEM = "Retro     [um]";
const char MSG_BED_CORRECTION_REAR_ES[] PROGMEM = "Trasera   [um]";
const char MSG_BED_CORRECTION_REAR_PL[] PROGMEM = "Do tylu  [um]";
const char MSG_BED_CORRECTION_REAR_DE[] PROGMEM = "Hinten    [um]";
const char * const MSG_BED_CORRECTION_REAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_REAR_EN,
	MSG_BED_CORRECTION_REAR_CZ,
	MSG_BED_CORRECTION_REAR_IT,
	MSG_BED_CORRECTION_REAR_ES,
	MSG_BED_CORRECTION_REAR_PL,
	MSG_BED_CORRECTION_REAR_DE
};

const char MSG_BED_CORRECTION_RESET_EN[] PROGMEM = "Reset";
const char MSG_BED_CORRECTION_RESET_DE[] PROGMEM = "Ruecksetzen";
const char * const MSG_BED_CORRECTION_RESET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_RESET_EN,
	MSG_BED_CORRECTION_RESET_EN,
	MSG_BED_CORRECTION_RESET_EN,
	MSG_BED_CORRECTION_RESET_EN,
	MSG_BED_CORRECTION_RESET_EN,
	MSG_BED_CORRECTION_RESET_DE
};

const char MSG_BED_CORRECTION_RIGHT_EN[] PROGMEM = "Right side[um]";
const char MSG_BED_CORRECTION_RIGHT_CZ[] PROGMEM = "Vpravo [um]";
const char MSG_BED_CORRECTION_RIGHT_IT[] PROGMEM = "Destra    [um]";
const char MSG_BED_CORRECTION_RIGHT_ES[] PROGMEM = "Derecha   [um]";
const char MSG_BED_CORRECTION_RIGHT_PL[] PROGMEM = "W prawo [um]";
const char MSG_BED_CORRECTION_RIGHT_DE[] PROGMEM = "Rechts    [um]";
const char * const MSG_BED_CORRECTION_RIGHT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_CORRECTION_RIGHT_EN,
	MSG_BED_CORRECTION_RIGHT_CZ,
	MSG_BED_CORRECTION_RIGHT_IT,
	MSG_BED_CORRECTION_RIGHT_ES,
	MSG_BED_CORRECTION_RIGHT_PL,
	MSG_BED_CORRECTION_RIGHT_DE
};

const char MSG_BED_DONE_EN[] PROGMEM = "Bed done";
const char MSG_BED_DONE_CZ[] PROGMEM = "Bed OK.";
const char MSG_BED_DONE_IT[] PROGMEM = "Piatto fatto.";
const char MSG_BED_DONE_ES[] PROGMEM = "Base preparada";
const char MSG_BED_DONE_PL[] PROGMEM = "Stolik OK.";
const char MSG_BED_DONE_DE[] PROGMEM = "Bett OK";
const char * const MSG_BED_DONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_DONE_EN,
	MSG_BED_DONE_CZ,
	MSG_BED_DONE_IT,
	MSG_BED_DONE_ES,
	MSG_BED_DONE_PL,
	MSG_BED_DONE_DE
};

const char MSG_BED_HEATING_EN[] PROGMEM = "Bed Heating";
const char MSG_BED_HEATING_CZ[] PROGMEM = "Zahrivani bed";
const char MSG_BED_HEATING_IT[] PROGMEM = "Riscald. letto";
const char MSG_BED_HEATING_ES[] PROGMEM = "Calentando Base";
const char MSG_BED_HEATING_PL[] PROGMEM = "Grzanie stolika..";
const char MSG_BED_HEATING_DE[] PROGMEM = "Bett aufwaermen";
const char * const MSG_BED_HEATING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_HEATING_EN,
	MSG_BED_HEATING_CZ,
	MSG_BED_HEATING_IT,
	MSG_BED_HEATING_ES,
	MSG_BED_HEATING_PL,
	MSG_BED_HEATING_DE
};

const char MSG_BED_LEVELING_FAILED_POINT_HIGH_EN[] PROGMEM = "Bed leveling failed. Sensor triggered too high. Waiting for reset.";
const char MSG_BED_LEVELING_FAILED_POINT_HIGH_CZ[] PROGMEM = "Kalibrace Z selhala. Sensor sepnul prilis vysoko. Cekam na reset.";
const char MSG_BED_LEVELING_FAILED_POINT_HIGH_IT[] PROGMEM = "Livellamento letto fallito.Risp sensore troppo prestoIn attesa di reset.";
const char MSG_BED_LEVELING_FAILED_POINT_HIGH_ES[] PROGMEM = "Nivelacion fallada. Sensor funciona demasiado temprano. Esperando reset.";
const char MSG_BED_LEVELING_FAILED_POINT_HIGH_PL[] PROGMEM = "Kalibracja Z nieudana. Sensor dotk. za wysoko. Czekam na reset.";
const char MSG_BED_LEVELING_FAILED_POINT_HIGH_DE[] PROGMEM = "Z-Kalibrierung fehlgeschlg. Sensor zu hoch ausgeloest. Warte auf Reset.";
const char * const MSG_BED_LEVELING_FAILED_POINT_HIGH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_LEVELING_FAILED_POINT_HIGH_EN,
	MSG_BED_LEVELING_FAILED_POINT_HIGH_CZ,
	MSG_BED_LEVELING_FAILED_POINT_HIGH_IT,
	MSG_BED_LEVELING_FAILED_POINT_HIGH_ES,
	MSG_BED_LEVELING_FAILED_POINT_HIGH_PL,
	MSG_BED_LEVELING_FAILED_POINT_HIGH_DE
};

const char MSG_BED_LEVELING_FAILED_POINT_LOW_EN[] PROGMEM = "Bed leveling failed. Sensor didnt trigger. Debris on nozzle? Waiting for reset.";
const char MSG_BED_LEVELING_FAILED_POINT_LOW_CZ[] PROGMEM = "Kalibrace Z selhala. Sensor nesepnul. Znecistena tryska? Cekam na reset.";
const char MSG_BED_LEVELING_FAILED_POINT_LOW_IT[] PROGMEM = "Livellamento letto fallito.NoRispSensor Residui su ugello? In attesa di reset.";
const char MSG_BED_LEVELING_FAILED_POINT_LOW_ES[] PROGMEM = "Nivelacion fallada. Sensor no funciona. Escombros en nozzle? Esperando reset.";
const char MSG_BED_LEVELING_FAILED_POINT_LOW_PL[] PROGMEM = "Kalibracja nieudana. Sensor nie dotknal. Zanieczysz. dysza? Czekam na reset.";
const char MSG_BED_LEVELING_FAILED_POINT_LOW_DE[] PROGMEM = "Z-Kal. fehlgeschlg. Sensor nicht ausgeloest. Schmutzige Duese? Warte auf Reset";
const char * const MSG_BED_LEVELING_FAILED_POINT_LOW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_LEVELING_FAILED_POINT_LOW_EN,
	MSG_BED_LEVELING_FAILED_POINT_LOW_CZ,
	MSG_BED_LEVELING_FAILED_POINT_LOW_IT,
	MSG_BED_LEVELING_FAILED_POINT_LOW_ES,
	MSG_BED_LEVELING_FAILED_POINT_LOW_PL,
	MSG_BED_LEVELING_FAILED_POINT_LOW_DE
};

const char MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_EN[] PROGMEM = "Bed leveling failed. Sensor disconnected or cable broken. Waiting for reset.";
const char MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_CZ[] PROGMEM = "Kalibrace Z selhala. Sensor je odpojeny nebo preruseny kabel. Cekam na reset.";
const char MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_IT[] PROGMEM = "Livellamento letto fallito. Sensore discon. o Cavo Dann. In attesa di reset.";
const char MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_ES[] PROGMEM = "Nivelacion fallada. Sensor desconectado o cables danados. Esperando reset.";
const char MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_PL[] PROGMEM = "Kalibracja nieudana. Sensor odlaczony lub uszkodz. kabel. Czekam na reset.";
const char MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_DE[] PROGMEM = "Z-Kalibrierung fehlgeschlg. Sensor nicht angeschlossen. Warte auf Reset.";
const char * const MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_EN,
	MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_CZ,
	MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_IT,
	MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_ES,
	MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_PL,
	MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED_DE
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_EN[] PROGMEM = "XYZ calibration failed. Front calibration points not reachable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_CZ[] PROGMEM = "Kalibrace XYZ selhala. Predni kalibracni body moc vpredu. Srovnejte tiskarnu.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_IT[] PROGMEM = "Calibrazione XYZ fallita. Punti anteriori non raggiungibili.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_ES[] PROGMEM = "Calibracion XYZ fallada. Puntos frontales no alcanzables.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_PL[] PROGMEM = "Kalibr. XYZ nieudana. Przed. punkty kalibr. zbyt do przodu. Wyrownac drukarke.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_DE[] PROGMEM = "XYZ-Kalibrierung fehlgeschlagen. Vordere Kalibrierpunkte sind zu weit vorne.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_PL,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR_DE
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_EN[] PROGMEM = "XYZ calibration failed. Left front calibration point not reachable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_CZ[] PROGMEM = "Kalibrace XYZ selhala. Levy predni bod moc vpredu. Srovnejte tiskarnu.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_IT[] PROGMEM = "Calibrazione XYZ fallita. Punto anteriore sinistro non raggiungibile.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_ES[] PROGMEM = "Calibracion XYZ fallada. Punto frontal izquierdo no alcanzable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_PL[] PROGMEM = "Kalibr. XYZ nieudana. Lewy przedni punkt zbyt do przodu. Wyrownac drukarke.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_DE[] PROGMEM = "XYZ-Kalibrierung fehlgeschlagen. Linker vorderer Kalibrierpunkt ist zu weit vorne.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_PL,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR_DE
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_EN[] PROGMEM = "XYZ calibration failed. Right front calibration point not reachable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_CZ[] PROGMEM = "Kalibrace XYZ selhala. Pravy predni bod moc vpredu. Srovnejte tiskarnu.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_IT[] PROGMEM = "Calibrazione XYZ fallita. Punto anteriore destro non raggiungibile.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_ES[] PROGMEM = "Calibracion XYZ fallad. Punto frontal derecho no alcanzable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_PL[] PROGMEM = "Kalibr. XYZ nieudana. Prawy przedni punkt zbyt do przodu. Wyrownac drukarke.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_DE[] PROGMEM = "XYZ-Kalibrierung fehlgeschlagen. Rechter vorderer Kalibrierpunkt ist zu weit vorne.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_PL,
	MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR_DE
};

const char MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_EN[] PROGMEM = "XYZ calibration failed. Please consult the manual.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_CZ[] PROGMEM = "Kalibrace XYZ selhala. Nahlednete do manualu.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_IT[] PROGMEM = "Calibrazione XYZ fallita. Si prega di consultare il manuale.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_ES[] PROGMEM = "Calibracion XYZ fallada. Consulta el manual por favor.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_PL[] PROGMEM = "Kalibracja XYZ niepowiedziona. Sprawdzic w instrukcji.";
const char MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_DE[] PROGMEM = "XYZ-Kalibrierung fehlgeschlagen. Bitte schauen Sie in das Handbuch.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_PL,
	MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED_DE
};

const char MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_EN[] PROGMEM = "XYZ calibration ok. X/Y axes are perpendicular. Congratulations!";
const char MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_CZ[] PROGMEM = "Kalibrace XYZ v poradku. X/Y osy jsou kolme. Gratuluji!";
const char MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_IT[] PROGMEM = "Calibrazione XYZ OK. Gli assi X/Y sono perpendicolari. Complimenti!";
const char MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_ES[] PROGMEM = "Calibracion XYZ ok. Ejes X/Y perpendiculares. Enhorabuena!";
const char MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_PL[] PROGMEM = "Kalibracja XYZ ok. Osie X/Y sa prostopadle. Gratulacje!";
const char MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_DE[] PROGMEM = "XYZ-Kalibrierung OK. X/Y-Achsen sind senkrecht zueinander. Glueckwunsch!";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_PL,
	MSG_BED_SKEW_OFFSET_DETECTION_PERFECT_DE
};

const char MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_EN[] PROGMEM = "XYZ calibration failed. Bed calibration point was not found.";
const char MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_CZ[] PROGMEM = "Kalibrace XYZ selhala. Kalibracni bod podlozky nenalezen.";
const char MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_IT[] PROGMEM = "Calibrazione XYZ fallita. Il punto di calibrazione sul letto non e' stato trovato.";
const char MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_ES[] PROGMEM = "Calibracion XYZ fallada. Puntos de calibracion en heatbed no encontrados.";
const char MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_PL[] PROGMEM = "Kalibr. XYZ nieudana. Kalibracyjny punkt podkladki nieznaleziony.";
const char MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_DE[] PROGMEM = "XYZ-Kalibrierung fehlgeschlagen. Bett-Kalibrierpunkt nicht gefunden.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_PL,
	MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND_DE
};

const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_EN[] PROGMEM = "XYZ calibration all right. Skew will be corrected automatically.";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_CZ[] PROGMEM = "Kalibrace XYZ v poradku. Zkoseni bude automaticky vyrovnano pri tisku.";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_IT[] PROGMEM = "Calibrazion XYZ corretta. La distorsione verra' automaticamente compensata.";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_ES[] PROGMEM = "Calibracion XYZ correcta. La inclinacion se corregira automaticamente.";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_PL[] PROGMEM = "Kalibracja XYZ prawidlowa. Skosy beda automatycznie wyrownane przy druku.";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_DE[] PROGMEM = "XYZ Kalibrierung in Ordnung. Schiefheit wird automatisch korrigiert.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_PL,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME_DE
};

const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_EN[] PROGMEM = "XYZ calibration all right. X/Y axes are slightly skewed. Good job!";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_CZ[] PROGMEM = "Kalibrace XYZ v poradku. X/Y osy mirne zkosene. Dobra prace!";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_IT[] PROGMEM = "Calibrazion XYZ corretta. Assi X/Y leggermente storti. Ben fatto!";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_ES[] PROGMEM = "Calibracion XYZ correcta. Los ejes X / Y estan ligeramente inclinados. Buen trabajo!";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_PL[] PROGMEM = "Kalibracja XYZ prawidlowa. Osie X/Y lekko skosne. Dobra robota!";
const char MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_DE[] PROGMEM = "XYZ Kalibrierung in Ordnung. X/Y Achsen sind etwas schief.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_PL,
	MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD_DE
};

const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_EN[] PROGMEM = "XYZ calibration compromised. Front calibration points not reachable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_CZ[] PROGMEM = "Kalibrace XYZ nepresna. Predni kalibracni body moc vpredu.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_IT[] PROGMEM = "Calibrazione XYZ compromessa. Punti anteriori non raggiungibili.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_ES[] PROGMEM = "Calibrazion XYZ comprometida. Puntos frontales no alcanzables.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_PL[] PROGMEM = "Kalibr. XYZ niedokladna. Przednie punkty kalibr. Zbyt wys. do przodu.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_DE[] PROGMEM = "XYZ-Kalibrierung ungenau. Vordere Kalibrierpunkte sind zu weit vorne.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_PL,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR_DE
};

const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_EN[] PROGMEM = "XYZ calibration compromised. Left front calibration point not reachable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_CZ[] PROGMEM = "Kalibrace XYZ nepresna. Levy predni bod moc vpredu.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_IT[] PROGMEM = "Calibrazione XYZ compromessa. Punto anteriore sinistro non raggiungibile.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_ES[] PROGMEM = "Calibrazion XYZ comprometida. Punto frontal izquierdo no alcanzable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_PL[] PROGMEM = "Kalibracja XYZ niedokladna. Lewy przedni punkt zbyt wysuniety do przodu.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_DE[] PROGMEM = "XYZ-Kalibrierung ungenau. Linker vorderer Kalibrierpunkt ist zu weit vorne.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_PL,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR_DE
};

const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_EN[] PROGMEM = "XYZ calibration compromised. Right front calibration point not reachable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_CZ[] PROGMEM = "Kalibrace XYZ nepresna. Pravy predni bod moc vpredu.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_IT[] PROGMEM = "Calibrazione XYZ compromessa. Punto anteriore destro non raggiungibile.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_ES[] PROGMEM = "Calibrazion XYZ comprometida. Punto frontal derecho no alcanzable.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_PL[] PROGMEM = "Kalibracja XYZ niedokladna. Prawy przedni punkt zbyt wysuniety do przodu.";
const char MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_DE[] PROGMEM = "XYZ-Kalibrierung ungenau. Rechter vorderer Kalibrierpunkt ist zu weit vorne.";
const char * const MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_EN,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_CZ,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_IT,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_ES,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_PL,
	MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR_DE
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
const char MSG_CALIBRATE_BED_IT[] PROGMEM = "Calibra XYZ";
const char MSG_CALIBRATE_BED_ES[] PROGMEM = "Calibra XYZ";
const char MSG_CALIBRATE_BED_PL[] PROGMEM = "Kalibracja XYZ";
const char MSG_CALIBRATE_BED_DE[] PROGMEM = "Kalibrierung XYZ";
const char * const MSG_CALIBRATE_BED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_BED_EN,
	MSG_CALIBRATE_BED_CZ,
	MSG_CALIBRATE_BED_IT,
	MSG_CALIBRATE_BED_ES,
	MSG_CALIBRATE_BED_PL,
	MSG_CALIBRATE_BED_DE
};

const char MSG_CALIBRATE_BED_RESET_EN[] PROGMEM = "Reset XYZ calibr.";
const char MSG_CALIBRATE_BED_RESET_CZ[] PROGMEM = "Reset XYZ kalibr.";
const char MSG_CALIBRATE_BED_RESET_PL[] PROGMEM = "Reset kalibr. XYZ";
const char MSG_CALIBRATE_BED_RESET_DE[] PROGMEM = "Reset XYZ Kalibr.";
const char * const MSG_CALIBRATE_BED_RESET_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_BED_RESET_EN,
	MSG_CALIBRATE_BED_RESET_CZ,
	MSG_CALIBRATE_BED_RESET_EN,
	MSG_CALIBRATE_BED_RESET_EN,
	MSG_CALIBRATE_BED_RESET_PL,
	MSG_CALIBRATE_BED_RESET_DE
};

const char MSG_CALIBRATE_E_EN[] PROGMEM = "Calibrate E";
const char MSG_CALIBRATE_E_CZ[] PROGMEM = "Kalibrovat E";
const char MSG_CALIBRATE_E_IT[] PROGMEM = "Calibra E";
const char MSG_CALIBRATE_E_ES[] PROGMEM = "Calibrar E";
const char MSG_CALIBRATE_E_PL[] PROGMEM = "Kalibruj E";
const char MSG_CALIBRATE_E_DE[] PROGMEM = "Kalibriere E";
const char * const MSG_CALIBRATE_E_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_E_EN,
	MSG_CALIBRATE_E_CZ,
	MSG_CALIBRATE_E_IT,
	MSG_CALIBRATE_E_ES,
	MSG_CALIBRATE_E_PL,
	MSG_CALIBRATE_E_DE
};

const char MSG_CALIBRATE_PINDA_EN[] PROGMEM = "Calibrate";
const char MSG_CALIBRATE_PINDA_CZ[] PROGMEM = "Zkalibrovat";
const char MSG_CALIBRATE_PINDA_IT[] PROGMEM = "Calibrare";
const char MSG_CALIBRATE_PINDA_ES[] PROGMEM = "Calibrar";
const char MSG_CALIBRATE_PINDA_PL[] PROGMEM = "Skalibrowac";
const char MSG_CALIBRATE_PINDA_DE[] PROGMEM = "Kalibrieren";
const char * const MSG_CALIBRATE_PINDA_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATE_PINDA_EN,
	MSG_CALIBRATE_PINDA_CZ,
	MSG_CALIBRATE_PINDA_IT,
	MSG_CALIBRATE_PINDA_ES,
	MSG_CALIBRATE_PINDA_PL,
	MSG_CALIBRATE_PINDA_DE
};

const char MSG_CALIBRATION_PINDA_MENU_EN[] PROGMEM = "Temp. calibration";
const char MSG_CALIBRATION_PINDA_MENU_CZ[] PROGMEM = "Teplot. kalibrace";
const char MSG_CALIBRATION_PINDA_MENU_IT[] PROGMEM = "Taratura temp.";
const char MSG_CALIBRATION_PINDA_MENU_ES[] PROGMEM = "Calibracion temp.";
const char MSG_CALIBRATION_PINDA_MENU_PL[] PROGMEM = "Cieplna kalibr.";
const char MSG_CALIBRATION_PINDA_MENU_DE[] PROGMEM = "Temp. kalibrieren";
const char * const MSG_CALIBRATION_PINDA_MENU_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CALIBRATION_PINDA_MENU_EN,
	MSG_CALIBRATION_PINDA_MENU_CZ,
	MSG_CALIBRATION_PINDA_MENU_IT,
	MSG_CALIBRATION_PINDA_MENU_ES,
	MSG_CALIBRATION_PINDA_MENU_PL,
	MSG_CALIBRATION_PINDA_MENU_DE
};

const char MSG_CARD_MENU_EN[] PROGMEM = "Print from SD";
const char MSG_CARD_MENU_CZ[] PROGMEM = "Tisk z SD";
const char MSG_CARD_MENU_IT[] PROGMEM = "Stampa da SD";
const char MSG_CARD_MENU_ES[] PROGMEM = "Menu tarjeta SD";
const char MSG_CARD_MENU_PL[] PROGMEM = "Druk z SD";
const char MSG_CARD_MENU_DE[] PROGMEM = "Drucken von SD";
const char * const MSG_CARD_MENU_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CARD_MENU_EN,
	MSG_CARD_MENU_CZ,
	MSG_CARD_MENU_IT,
	MSG_CARD_MENU_ES,
	MSG_CARD_MENU_PL,
	MSG_CARD_MENU_DE
};

const char MSG_CENTER_EN[] PROGMEM = "Center:";
const char MSG_CENTER_CZ[] PROGMEM = "Stred:";
const char MSG_CENTER_IT[] PROGMEM = "Centro:";
const char MSG_CENTER_ES[] PROGMEM = "Centro:";
const char MSG_CENTER_PL[] PROGMEM = "Srodek:";
const char MSG_CENTER_DE[] PROGMEM = "Mitte:";
const char * const MSG_CENTER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CENTER_EN,
	MSG_CENTER_CZ,
	MSG_CENTER_IT,
	MSG_CENTER_ES,
	MSG_CENTER_PL,
	MSG_CENTER_DE
};

const char MSG_CHANGE_EXTR_EN[] PROGMEM = "Change extruder";
const char MSG_CHANGE_EXTR_CZ[] PROGMEM = "Zmenit extruder";
const char MSG_CHANGE_EXTR_IT[] PROGMEM = "Cambio estrusore.";
const char MSG_CHANGE_EXTR_ES[] PROGMEM = "Cambiar extrusor.";
const char MSG_CHANGE_EXTR_PL[] PROGMEM = "Zmienic ekstruder";
const char MSG_CHANGE_EXTR_DE[] PROGMEM = "Wechsel extruder";
const char * const MSG_CHANGE_EXTR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHANGE_EXTR_EN,
	MSG_CHANGE_EXTR_CZ,
	MSG_CHANGE_EXTR_IT,
	MSG_CHANGE_EXTR_ES,
	MSG_CHANGE_EXTR_PL,
	MSG_CHANGE_EXTR_DE
};

const char MSG_CHANGE_SUCCESS_EN[] PROGMEM = "Change success!";
const char MSG_CHANGE_SUCCESS_CZ[] PROGMEM = "Zmena uspesna!";
const char MSG_CHANGE_SUCCESS_IT[] PROGMEM = "Cambio riuscito!";
const char MSG_CHANGE_SUCCESS_ES[] PROGMEM = "Cambio correcto";
const char MSG_CHANGE_SUCCESS_PL[] PROGMEM = "Wymiana ok!";
const char MSG_CHANGE_SUCCESS_DE[] PROGMEM = "Wechsel erfolgr.!";
const char * const MSG_CHANGE_SUCCESS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHANGE_SUCCESS_EN,
	MSG_CHANGE_SUCCESS_CZ,
	MSG_CHANGE_SUCCESS_IT,
	MSG_CHANGE_SUCCESS_ES,
	MSG_CHANGE_SUCCESS_PL,
	MSG_CHANGE_SUCCESS_DE
};

const char MSG_CHANGING_FILAMENT_EN[] PROGMEM = "Changing filament!";
const char MSG_CHANGING_FILAMENT_CZ[] PROGMEM = "Vymena filamentu!";
const char MSG_CHANGING_FILAMENT_IT[] PROGMEM = "Cambiando filam.";
const char MSG_CHANGING_FILAMENT_ES[] PROGMEM = "Cambiando filamento";
const char MSG_CHANGING_FILAMENT_PL[] PROGMEM = "Wymiana filamentu";
const char MSG_CHANGING_FILAMENT_DE[] PROGMEM = "Filament-Wechsel!";
const char * const MSG_CHANGING_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHANGING_FILAMENT_EN,
	MSG_CHANGING_FILAMENT_CZ,
	MSG_CHANGING_FILAMENT_IT,
	MSG_CHANGING_FILAMENT_ES,
	MSG_CHANGING_FILAMENT_PL,
	MSG_CHANGING_FILAMENT_DE
};

const char MSG_CHOOSE_EXTRUDER_EN[] PROGMEM = "Choose extruder:";
const char MSG_CHOOSE_EXTRUDER_CZ[] PROGMEM = "Vyberte extruder:";
const char MSG_CHOOSE_EXTRUDER_IT[] PROGMEM = "Seleziona estrusore:";
const char MSG_CHOOSE_EXTRUDER_ES[] PROGMEM = "Elegir extrusor:";
const char MSG_CHOOSE_EXTRUDER_PL[] PROGMEM = "Wybierz ekstruder";
const char MSG_CHOOSE_EXTRUDER_DE[] PROGMEM = "Waehlen Sie Extruder";
const char * const MSG_CHOOSE_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CHOOSE_EXTRUDER_EN,
	MSG_CHOOSE_EXTRUDER_CZ,
	MSG_CHOOSE_EXTRUDER_IT,
	MSG_CHOOSE_EXTRUDER_ES,
	MSG_CHOOSE_EXTRUDER_PL,
	MSG_CHOOSE_EXTRUDER_DE
};

const char MSG_CLEAN_NOZZLE_E_EN[] PROGMEM = "E calibration finished. Please clean the nozzle. Click when done.";
const char MSG_CLEAN_NOZZLE_E_CZ[] PROGMEM = "E kalibrace ukoncena. Prosim ocistete trysku. Po te potvrdte tlacitkem.";
const char MSG_CLEAN_NOZZLE_E_IT[] PROGMEM = "Calibrazione E terminata. Si prega di pulire l'ugello. Click per continuare.";
const char MSG_CLEAN_NOZZLE_E_ES[] PROGMEM = "E calibrado. Limpia nozzle. Haz clic una vez terminado.";
const char MSG_CLEAN_NOZZLE_E_PL[] PROGMEM = "Kalibracja E skonczona. Prosze oczyscic dysze. Potem potwierdzic przyciskiem. ";
const char MSG_CLEAN_NOZZLE_E_DE[] PROGMEM = "E-Kalibrierung beendet. Bitte reinigen Sie die Duese. Klicken wenn fertig.";
const char * const MSG_CLEAN_NOZZLE_E_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CLEAN_NOZZLE_E_EN,
	MSG_CLEAN_NOZZLE_E_CZ,
	MSG_CLEAN_NOZZLE_E_IT,
	MSG_CLEAN_NOZZLE_E_ES,
	MSG_CLEAN_NOZZLE_E_PL,
	MSG_CLEAN_NOZZLE_E_DE
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
const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_IT[] PROGMEM = "I carrelli Z sin/des sono altezza max?";
const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_ES[] PROGMEM = "Carros Z izq./der. estan arriba maximo?";
const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_PL[] PROGMEM = "Oba wozki dojechaly do gornej ramy?";
const char MSG_CONFIRM_CARRIAGE_AT_THE_TOP_DE[] PROGMEM = "Ist der Schlitten ganz oben?";
const char * const MSG_CONFIRM_CARRIAGE_AT_THE_TOP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_EN,
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_CZ,
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_IT,
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_ES,
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_PL,
	MSG_CONFIRM_CARRIAGE_AT_THE_TOP_DE
};

const char MSG_CONFIRM_NOZZLE_CLEAN_EN[] PROGMEM = "Please clean the nozzle for calibration. Click when done.";
const char MSG_CONFIRM_NOZZLE_CLEAN_CZ[] PROGMEM = "Pro uspesnou kalibraci ocistete prosim tiskovou trysku. Potvrdte tlacitkem.";
const char MSG_CONFIRM_NOZZLE_CLEAN_IT[] PROGMEM = "Pulire l'ugello per la calibrazione, poi fare click.";
const char MSG_CONFIRM_NOZZLE_CLEAN_ES[] PROGMEM = "Limpia nozzle para calibracion. Click cuando acabes.";
const char MSG_CONFIRM_NOZZLE_CLEAN_PL[] PROGMEM = "Dla prawidl. kalibracji prosze oczyscic dysze. Potw. guzikiem.";
const char MSG_CONFIRM_NOZZLE_CLEAN_DE[] PROGMEM = "Bitte entfernen Sie ueberstehendes Filament von der Duese. Klicken wenn sauber.";
const char * const MSG_CONFIRM_NOZZLE_CLEAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONFIRM_NOZZLE_CLEAN_EN,
	MSG_CONFIRM_NOZZLE_CLEAN_CZ,
	MSG_CONFIRM_NOZZLE_CLEAN_IT,
	MSG_CONFIRM_NOZZLE_CLEAN_ES,
	MSG_CONFIRM_NOZZLE_CLEAN_PL,
	MSG_CONFIRM_NOZZLE_CLEAN_DE
};

const char MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_EN[] PROGMEM = "Filaments are now adjusted. Please clean the nozzle for calibration. Click when done.";
const char MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_CZ[] PROGMEM = "Filamenty jsou srovnany. Pro uspesnou kalibraci prosim ocistete trysku. Po te potvrdte tlacitkem.";
const char MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_IT[] PROGMEM = "I filamenti sono regolati. Si prega di pulire l'ugello per la calibrazione. Click per continuare.";
const char MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_ES[] PROGMEM = "Filamentos ajustados. Limpia nozzle para calibracion. Haz clic una vez terminado.";
const char MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_PL[] PROGMEM = "Dla prawidlowej kalibracji prosze oczyscic dysze. Potem potwierdzic przyciskiem.";
const char MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_DE[] PROGMEM = "Filamente sind jetzt eingestellt. Bitte reinigen Sie die Duese zur Kalibrierung. Klicken wenn fertig.";
const char * const MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_EN,
	MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_CZ,
	MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_IT,
	MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_ES,
	MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_PL,
	MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ_DE
};

const char MSG_CONTROL_EN[] PROGMEM = "Control";
const char * const MSG_CONTROL_LANG_TABLE[1] PROGMEM = {
	MSG_CONTROL_EN
};

const char MSG_COOLDOWN_EN[] PROGMEM = "Cooldown";
const char MSG_COOLDOWN_CZ[] PROGMEM = "Zchladit";
const char MSG_COOLDOWN_IT[] PROGMEM = "Raffredda";
const char MSG_COOLDOWN_ES[] PROGMEM = "Enfriar";
const char MSG_COOLDOWN_PL[] PROGMEM = "Wychlodzic";
const char MSG_COOLDOWN_DE[] PROGMEM = "Abkuehlen";
const char * const MSG_COOLDOWN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_COOLDOWN_EN,
	MSG_COOLDOWN_CZ,
	MSG_COOLDOWN_IT,
	MSG_COOLDOWN_ES,
	MSG_COOLDOWN_PL,
	MSG_COOLDOWN_DE
};

const char MSG_CORRECTLY_EN[] PROGMEM = "Changed correctly?";
const char MSG_CORRECTLY_CZ[] PROGMEM = "Vymena ok?";
const char MSG_CORRECTLY_IT[] PROGMEM = "Cambiato corr.?";
const char MSG_CORRECTLY_ES[] PROGMEM = "Cambiado correct.?";
const char MSG_CORRECTLY_PL[] PROGMEM = "Wymiana ok?";
const char MSG_CORRECTLY_DE[] PROGMEM = "Wechsel ok?";
const char * const MSG_CORRECTLY_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CORRECTLY_EN,
	MSG_CORRECTLY_CZ,
	MSG_CORRECTLY_IT,
	MSG_CORRECTLY_ES,
	MSG_CORRECTLY_PL,
	MSG_CORRECTLY_DE
};

const char MSG_COUNT_X_EN[] PROGMEM = " Count X: ";
const char * const MSG_COUNT_X_LANG_TABLE[1] PROGMEM = {
	MSG_COUNT_X_EN
};

const char MSG_CURRENT_EN[] PROGMEM = "Current";
const char MSG_CURRENT_CZ[] PROGMEM = "Pouze aktualni";
const char MSG_CURRENT_IT[] PROGMEM = "Attuale";
const char MSG_CURRENT_ES[] PROGMEM = "Actual";
const char MSG_CURRENT_PL[] PROGMEM = "Tylko aktualne";
const char MSG_CURRENT_DE[] PROGMEM = "Aktuelles";
const char * const MSG_CURRENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_CURRENT_EN,
	MSG_CURRENT_CZ,
	MSG_CURRENT_IT,
	MSG_CURRENT_ES,
	MSG_CURRENT_PL,
	MSG_CURRENT_DE
};

const char MSG_DATE_EN[] PROGMEM = "Date:";
const char MSG_DATE_CZ[] PROGMEM = "Datum:";
const char MSG_DATE_IT[] PROGMEM = "Data";
const char MSG_DATE_ES[] PROGMEM = "Fecha:";
const char MSG_DATE_PL[] PROGMEM = "Data:";
const char MSG_DATE_DE[] PROGMEM = "Datum";
const char * const MSG_DATE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_DATE_EN,
	MSG_DATE_CZ,
	MSG_DATE_IT,
	MSG_DATE_ES,
	MSG_DATE_PL,
	MSG_DATE_DE
};

const char MSG_DEFAULT_SETTINGS_LOADED_EN[] PROGMEM = "Default settings loaded";
const char MSG_DEFAULT_SETTINGS_LOADED_CZ[] PROGMEM = "Defaultni nastaveni nahrano";
const char MSG_DEFAULT_SETTINGS_LOADED_IT[] PROGMEM = "Settaggi predefiniti caricati";
const char MSG_DEFAULT_SETTINGS_LOADED_ES[] PROGMEM = "Ajustes por defecto cargados.";
const char MSG_DEFAULT_SETTINGS_LOADED_PL[] PROGMEM = "Ustawienia domy\x9Clne";
const char MSG_DEFAULT_SETTINGS_LOADED_DE[] PROGMEM = "Standardeinstellungen wurden zuruckgesetzt";
const char * const MSG_DEFAULT_SETTINGS_LOADED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_DEFAULT_SETTINGS_LOADED_EN,
	MSG_DEFAULT_SETTINGS_LOADED_CZ,
	MSG_DEFAULT_SETTINGS_LOADED_IT,
	MSG_DEFAULT_SETTINGS_LOADED_ES,
	MSG_DEFAULT_SETTINGS_LOADED_PL,
	MSG_DEFAULT_SETTINGS_LOADED_DE
};

const char MSG_DISABLE_STEPPERS_EN[] PROGMEM = "Disable steppers";
const char MSG_DISABLE_STEPPERS_CZ[] PROGMEM = "Vypnout motory";
const char MSG_DISABLE_STEPPERS_IT[] PROGMEM = "Disabilit motori";
const char MSG_DISABLE_STEPPERS_ES[] PROGMEM = "Apagar motores";
const char MSG_DISABLE_STEPPERS_PL[] PROGMEM = "Wylaczyc silniki";
const char MSG_DISABLE_STEPPERS_DE[] PROGMEM = "Motoren aus";
const char * const MSG_DISABLE_STEPPERS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_DISABLE_STEPPERS_EN,
	MSG_DISABLE_STEPPERS_CZ,
	MSG_DISABLE_STEPPERS_IT,
	MSG_DISABLE_STEPPERS_ES,
	MSG_DISABLE_STEPPERS_PL,
	MSG_DISABLE_STEPPERS_DE
};

const char MSG_DWELL_EN[] PROGMEM = "Sleep...";
const char MSG_DWELL_IT[] PROGMEM = "Sospensione...";
const char MSG_DWELL_ES[] PROGMEM = "En espera";
const char MSG_DWELL_DE[] PROGMEM = "Einen Moment bitte.";
const char * const MSG_DWELL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_DWELL_EN,
	MSG_DWELL_EN,
	MSG_DWELL_IT,
	MSG_DWELL_ES,
	MSG_DWELL_EN,
	MSG_DWELL_DE
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
const char MSG_ERROR_IT[] PROGMEM = "ERRORE:";
const char MSG_ERROR_PL[] PROGMEM = "BLAD:";
const char MSG_ERROR_DE[] PROGMEM = "FEHLER:";
const char * const MSG_ERROR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_ERROR_EN,
	MSG_ERROR_CZ,
	MSG_ERROR_IT,
	MSG_ERROR_EN,
	MSG_ERROR_PL,
	MSG_ERROR_DE
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
const char MSG_EXTRUDER_IT[] PROGMEM = "Estrusore";
const char MSG_EXTRUDER_ES[] PROGMEM = "Extrusor";
const char MSG_EXTRUDER_PL[] PROGMEM = "Ekstruder";
const char * const MSG_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_EXTRUDER_EN,
	MSG_EXTRUDER_EN,
	MSG_EXTRUDER_IT,
	MSG_EXTRUDER_ES,
	MSG_EXTRUDER_PL,
	MSG_EXTRUDER_EN
};

const char MSG_EXTRUDER_1_EN[] PROGMEM = "Extruder 1";
const char MSG_EXTRUDER_1_IT[] PROGMEM = "Estrusore 1";
const char MSG_EXTRUDER_1_ES[] PROGMEM = "Extrusor 1";
const char MSG_EXTRUDER_1_PL[] PROGMEM = "Ekstruder 1";
const char * const MSG_EXTRUDER_1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_EXTRUDER_1_EN,
	MSG_EXTRUDER_1_EN,
	MSG_EXTRUDER_1_IT,
	MSG_EXTRUDER_1_ES,
	MSG_EXTRUDER_1_PL,
	MSG_EXTRUDER_1_EN
};

const char MSG_EXTRUDER_2_EN[] PROGMEM = "Extruder 2";
const char MSG_EXTRUDER_2_IT[] PROGMEM = "Estrusore 2";
const char MSG_EXTRUDER_2_ES[] PROGMEM = "Extrusor 2";
const char MSG_EXTRUDER_2_PL[] PROGMEM = "Ekstruder 2";
const char * const MSG_EXTRUDER_2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_EXTRUDER_2_EN,
	MSG_EXTRUDER_2_EN,
	MSG_EXTRUDER_2_IT,
	MSG_EXTRUDER_2_ES,
	MSG_EXTRUDER_2_PL,
	MSG_EXTRUDER_2_EN
};

const char MSG_EXTRUDER_3_EN[] PROGMEM = "Extruder 3";
const char MSG_EXTRUDER_3_IT[] PROGMEM = "Estrusore 3";
const char MSG_EXTRUDER_3_ES[] PROGMEM = "Extrusor 3";
const char MSG_EXTRUDER_3_PL[] PROGMEM = "Ekstruder 3";
const char * const MSG_EXTRUDER_3_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_EXTRUDER_3_EN,
	MSG_EXTRUDER_3_EN,
	MSG_EXTRUDER_3_IT,
	MSG_EXTRUDER_3_ES,
	MSG_EXTRUDER_3_PL,
	MSG_EXTRUDER_3_EN
};

const char MSG_EXTRUDER_4_EN[] PROGMEM = "Extruder 4";
const char MSG_EXTRUDER_4_IT[] PROGMEM = "Estrusore 4";
const char MSG_EXTRUDER_4_ES[] PROGMEM = "Extrusor 4";
const char MSG_EXTRUDER_4_PL[] PROGMEM = "Ekstruder 4";
const char * const MSG_EXTRUDER_4_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_EXTRUDER_4_EN,
	MSG_EXTRUDER_4_EN,
	MSG_EXTRUDER_4_IT,
	MSG_EXTRUDER_4_ES,
	MSG_EXTRUDER_4_PL,
	MSG_EXTRUDER_4_EN
};

const char MSG_E_CAL_KNOB_EN[] PROGMEM = "Rotate knob until mark reaches extruder body. Click when done.";
const char MSG_E_CAL_KNOB_CZ[] PROGMEM = "Otacejte tlacitkem dokud znacka nedosahne tela extruderu. Potvrdte tlacitkem.";
const char MSG_E_CAL_KNOB_IT[] PROGMEM = "Girare la manopola affinche' il segno raggiunga il corpo dell'estrusore. Click per continuare.";
const char MSG_E_CAL_KNOB_ES[] PROGMEM = "Rotar el mando hasta que la marca llegue al cuerpo del extrusor. Haz clic una vez terminado.";
const char MSG_E_CAL_KNOB_PL[] PROGMEM = "Prosze otaczac przycisk poki znacznik nie dosiegnie ciala ekstrudera. Potwierdzic przyciskiem.";
const char MSG_E_CAL_KNOB_DE[] PROGMEM = "Knopf drehen bis die Filamentmarkierung erreicht ist. Klicken wenn fertig.";
const char * const MSG_E_CAL_KNOB_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_E_CAL_KNOB_EN,
	MSG_E_CAL_KNOB_CZ,
	MSG_E_CAL_KNOB_IT,
	MSG_E_CAL_KNOB_ES,
	MSG_E_CAL_KNOB_PL,
	MSG_E_CAL_KNOB_DE
};

const char MSG_Enqueing_EN[] PROGMEM = "enqueing \"";
const char * const MSG_Enqueing_LANG_TABLE[1] PROGMEM = {
	MSG_Enqueing_EN
};

const char MSG_FACTOR_EN[] PROGMEM = " \002 Fact";
const char * const MSG_FACTOR_LANG_TABLE[1] PROGMEM = {
	MSG_FACTOR_EN
};

const char MSG_FAN_SPEED_EN[] PROGMEM = "Fan speed";
const char MSG_FAN_SPEED_CZ[] PROGMEM = "Rychlost vent.";
const char MSG_FAN_SPEED_IT[] PROGMEM = "Velocita vent.";
const char MSG_FAN_SPEED_ES[] PROGMEM = "Velocidad Vent.";
const char MSG_FAN_SPEED_PL[] PROGMEM = "Predkosc went.";
const char MSG_FAN_SPEED_DE[] PROGMEM = "Luefter-Tempo";
const char * const MSG_FAN_SPEED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FAN_SPEED_EN,
	MSG_FAN_SPEED_CZ,
	MSG_FAN_SPEED_IT,
	MSG_FAN_SPEED_ES,
	MSG_FAN_SPEED_PL,
	MSG_FAN_SPEED_DE
};

const char MSG_FARM_CARD_MENU_EN[] PROGMEM = "Farm mode print";
const char * const MSG_FARM_CARD_MENU_LANG_TABLE[1] PROGMEM = {
	MSG_FARM_CARD_MENU_EN
};

const char MSG_FILAMENTCHANGE_EN[] PROGMEM = "Change filament";
const char MSG_FILAMENTCHANGE_CZ[] PROGMEM = "Vymenit filament";
const char MSG_FILAMENTCHANGE_IT[] PROGMEM = "Camb. filamento";
const char MSG_FILAMENTCHANGE_ES[] PROGMEM = "Cambiar filamento";
const char MSG_FILAMENTCHANGE_PL[] PROGMEM = "Wymienic filament";
const char MSG_FILAMENTCHANGE_DE[] PROGMEM = "Filament-Wechsel";
const char * const MSG_FILAMENTCHANGE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENTCHANGE_EN,
	MSG_FILAMENTCHANGE_CZ,
	MSG_FILAMENTCHANGE_IT,
	MSG_FILAMENTCHANGE_ES,
	MSG_FILAMENTCHANGE_PL,
	MSG_FILAMENTCHANGE_DE
};

const char MSG_FILAMENT_CLEAN_EN[] PROGMEM = "Is color clear?";
const char MSG_FILAMENT_CLEAN_CZ[] PROGMEM = "Je barva cista?";
const char MSG_FILAMENT_CLEAN_IT[] PROGMEM = "Il colore e' nitido?";
const char MSG_FILAMENT_CLEAN_ES[] PROGMEM = "Es el nuevo color nitido?";
const char MSG_FILAMENT_CLEAN_PL[] PROGMEM = "Czy kolor jest czysty?";
const char MSG_FILAMENT_CLEAN_DE[] PROGMEM = "Ist Farbe rein?";
const char * const MSG_FILAMENT_CLEAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_CLEAN_EN,
	MSG_FILAMENT_CLEAN_CZ,
	MSG_FILAMENT_CLEAN_IT,
	MSG_FILAMENT_CLEAN_ES,
	MSG_FILAMENT_CLEAN_PL,
	MSG_FILAMENT_CLEAN_DE
};

const char MSG_FILAMENT_LOADING_T0_EN[] PROGMEM = "Insert filament into extruder 1. Click when done.";
const char MSG_FILAMENT_LOADING_T0_CZ[] PROGMEM = "Vlozte filament do extruderu 1. Potvrdte tlacitkem.";
const char MSG_FILAMENT_LOADING_T0_IT[] PROGMEM = "Inserire filamento nell'estrusore 1. Click per continuare.";
const char MSG_FILAMENT_LOADING_T0_ES[] PROGMEM = "Insertar filamento en el extrusor 1. Haz clic una vez terminado.";
const char MSG_FILAMENT_LOADING_T0_PL[] PROGMEM = "Wloz filament do ekstrudera 1. Potwierdz przyciskiem.";
const char MSG_FILAMENT_LOADING_T0_DE[] PROGMEM = "Filament in extruder 1 einlegen. Klicken wenn fertig.";
const char * const MSG_FILAMENT_LOADING_T0_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_LOADING_T0_EN,
	MSG_FILAMENT_LOADING_T0_CZ,
	MSG_FILAMENT_LOADING_T0_IT,
	MSG_FILAMENT_LOADING_T0_ES,
	MSG_FILAMENT_LOADING_T0_PL,
	MSG_FILAMENT_LOADING_T0_DE
};

const char MSG_FILAMENT_LOADING_T1_EN[] PROGMEM = "Insert filament into extruder 2. Click when done.";
const char MSG_FILAMENT_LOADING_T1_CZ[] PROGMEM = "Vlozte filament do extruderu 2. Potvrdte tlacitkem.";
const char MSG_FILAMENT_LOADING_T1_IT[] PROGMEM = "Inserire filamento nell'estrusore 2. Click per continuare.";
const char MSG_FILAMENT_LOADING_T1_ES[] PROGMEM = "Insertar filamento en el extrusor 2. Haz clic una vez terminado.";
const char MSG_FILAMENT_LOADING_T1_PL[] PROGMEM = "Wloz filament do ekstrudera 2. Potwierdz przyciskiem.";
const char MSG_FILAMENT_LOADING_T1_DE[] PROGMEM = "Filament in extruder 2 einlegen. Klicken wenn fertig.";
const char * const MSG_FILAMENT_LOADING_T1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_LOADING_T1_EN,
	MSG_FILAMENT_LOADING_T1_CZ,
	MSG_FILAMENT_LOADING_T1_IT,
	MSG_FILAMENT_LOADING_T1_ES,
	MSG_FILAMENT_LOADING_T1_PL,
	MSG_FILAMENT_LOADING_T1_DE
};

const char MSG_FILAMENT_LOADING_T2_EN[] PROGMEM = "Insert filament into extruder 3. Click when done.";
const char MSG_FILAMENT_LOADING_T2_CZ[] PROGMEM = "Vlozte filament do extruderu 3. Potvrdte tlacitkem.";
const char MSG_FILAMENT_LOADING_T2_IT[] PROGMEM = "Inserire filamento nell'estrusore 3. Click per continuare.";
const char MSG_FILAMENT_LOADING_T2_ES[] PROGMEM = "Insertar filamento en el extrusor 3. Haz clic una vez terminado.";
const char MSG_FILAMENT_LOADING_T2_PL[] PROGMEM = "Wloz filament do ekstrudera 3. Potwierdz przyciskiem.";
const char MSG_FILAMENT_LOADING_T2_DE[] PROGMEM = "Filament in extruder 3 einlegen. Klicken wenn fertig.";
const char * const MSG_FILAMENT_LOADING_T2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_LOADING_T2_EN,
	MSG_FILAMENT_LOADING_T2_CZ,
	MSG_FILAMENT_LOADING_T2_IT,
	MSG_FILAMENT_LOADING_T2_ES,
	MSG_FILAMENT_LOADING_T2_PL,
	MSG_FILAMENT_LOADING_T2_DE
};

const char MSG_FILAMENT_LOADING_T3_EN[] PROGMEM = "Insert filament into extruder 4. Click when done.";
const char MSG_FILAMENT_LOADING_T3_CZ[] PROGMEM = "Vlozte filament do extruderu 4. Potvrdte tlacitkem.";
const char MSG_FILAMENT_LOADING_T3_IT[] PROGMEM = "Inserire filamento nell'estrusore 4. Click per continuare.";
const char MSG_FILAMENT_LOADING_T3_ES[] PROGMEM = "Insertar filamento en el extrusor 4. Haz clic una vez terminado.";
const char MSG_FILAMENT_LOADING_T3_PL[] PROGMEM = "Wloz filament do ekstrudera 4. Potwierdz przyciskiem.";
const char MSG_FILAMENT_LOADING_T3_DE[] PROGMEM = "Filament in extruder 4 einlegen. Klicken wenn fertig.";
const char * const MSG_FILAMENT_LOADING_T3_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILAMENT_LOADING_T3_EN,
	MSG_FILAMENT_LOADING_T3_CZ,
	MSG_FILAMENT_LOADING_T3_IT,
	MSG_FILAMENT_LOADING_T3_ES,
	MSG_FILAMENT_LOADING_T3_PL,
	MSG_FILAMENT_LOADING_T3_DE
};

const char MSG_FILE_CNT_EN[] PROGMEM = "Some files will not be sorted. Max. No. of files in 1 folder for sorting is 100.";
const char MSG_FILE_CNT_CZ[] PROGMEM = "Nektere soubory nebudou setrideny. Maximalni pocet souboru pro setrideni je 100.";
const char MSG_FILE_CNT_IT[] PROGMEM = "Alcuni dei file non potranno essere organizzati. 100 e il n. max. di file che possono essere organizzati.";
const char MSG_FILE_CNT_ES[] PROGMEM = "Algunos archivos no seran ordenados. El Max. num. de archivos para ordenar en 1 carpeta es 100.";
const char MSG_FILE_CNT_PL[] PROGMEM = "Niektore pliki nie beda sortowane. Max. ilosc plikow do sortu w 1 folderze to 100.";
const char MSG_FILE_CNT_DE[] PROGMEM = "Einige Dateien werden nicht sortiert. Max. Anzahl der Dateien in einem Ordner ist 100.";
const char * const MSG_FILE_CNT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILE_CNT_EN,
	MSG_FILE_CNT_CZ,
	MSG_FILE_CNT_IT,
	MSG_FILE_CNT_ES,
	MSG_FILE_CNT_PL,
	MSG_FILE_CNT_DE
};

const char MSG_FILE_INCOMPLETE_EN[] PROGMEM = "File incomplete. Continue anyway?";
const char MSG_FILE_INCOMPLETE_CZ[] PROGMEM = "Soubor nekompletni. Pokracovat?";
const char MSG_FILE_INCOMPLETE_IT[] PROGMEM = "File incompleto. Continuare comunque?";
const char MSG_FILE_INCOMPLETE_ES[] PROGMEM = "Archivo imcompleto. Deseas continuar?";
const char MSG_FILE_INCOMPLETE_PL[] PROGMEM = "Plik niekompletny. Kontynuowac?";
const char MSG_FILE_INCOMPLETE_DE[] PROGMEM = "Datei unvollstaend. Trotzdem fortfahren?";
const char * const MSG_FILE_INCOMPLETE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FILE_INCOMPLETE_EN,
	MSG_FILE_INCOMPLETE_CZ,
	MSG_FILE_INCOMPLETE_IT,
	MSG_FILE_INCOMPLETE_ES,
	MSG_FILE_INCOMPLETE_PL,
	MSG_FILE_INCOMPLETE_DE
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
const char MSG_FIL_ADJUSTING_IT[] PROGMEM = "Filamento in fase di regolazione. Attendere prego.";
const char MSG_FIL_ADJUSTING_ES[] PROGMEM = "Ajustando filamentos. Espera por favor.";
const char MSG_FIL_ADJUSTING_PL[] PROGMEM = "Przebiega wyrownanie filamentow. Prosze czekac.";
const char MSG_FIL_ADJUSTING_DE[] PROGMEM = "Filament positionieren. Bitte warten.";
const char * const MSG_FIL_ADJUSTING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIL_ADJUSTING_EN,
	MSG_FIL_ADJUSTING_CZ,
	MSG_FIL_ADJUSTING_IT,
	MSG_FIL_ADJUSTING_ES,
	MSG_FIL_ADJUSTING_PL,
	MSG_FIL_ADJUSTING_DE
};

const char MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_EN[] PROGMEM = "Iteration ";
const char MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_CZ[] PROGMEM = "Iterace ";
const char MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_IT[] PROGMEM = "Reiterazione ";
const char MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_ES[] PROGMEM = "Reiteracion ";
const char MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_PL[] PROGMEM = "Iteracja ";
const char * const MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_EN,
	MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_CZ,
	MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_IT,
	MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_ES,
	MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_PL,
	MSG_FIND_BED_OFFSET_AND_SKEW_ITERATION_EN
};

const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_EN[] PROGMEM = "Searching bed calibration point";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_CZ[] PROGMEM = "Hledam kalibracni bod podlozky";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_IT[] PROGMEM = "Ricerca del letto punto di calibraz.";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_ES[] PROGMEM = "Buscando punto de calibracion heatbed";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_PL[] PROGMEM = "Szukam punktu kalibracyjnego podkladki";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_DE[] PROGMEM = "Suchen Bett Kalibrierpunkt";
const char * const MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_EN,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_CZ,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_IT,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_ES,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_PL,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE1_DE
};

const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_EN[] PROGMEM = " of 4";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_CZ[] PROGMEM = " z 4";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_IT[] PROGMEM = " su 4";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_ES[] PROGMEM = " de 4";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_PL[] PROGMEM = " z 4";
const char MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_DE[] PROGMEM = " von 4";
const char * const MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_EN,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_CZ,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_IT,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_ES,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_PL,
	MSG_FIND_BED_OFFSET_AND_SKEW_LINE2_DE
};

const char MSG_FINISHING_MOVEMENTS_EN[] PROGMEM = "Finishing movements";
const char MSG_FINISHING_MOVEMENTS_CZ[] PROGMEM = "Dokoncovani pohybu";
const char MSG_FINISHING_MOVEMENTS_IT[] PROGMEM = "Arresto in corso";
const char MSG_FINISHING_MOVEMENTS_ES[] PROGMEM = "Term. movimientos";
const char MSG_FINISHING_MOVEMENTS_PL[] PROGMEM = "Konczenie druku";
const char MSG_FINISHING_MOVEMENTS_DE[] PROGMEM = "Bewegung beenden";
const char * const MSG_FINISHING_MOVEMENTS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FINISHING_MOVEMENTS_EN,
	MSG_FINISHING_MOVEMENTS_CZ,
	MSG_FINISHING_MOVEMENTS_IT,
	MSG_FINISHING_MOVEMENTS_ES,
	MSG_FINISHING_MOVEMENTS_PL,
	MSG_FINISHING_MOVEMENTS_DE
};

const char MSG_FLOW_EN[] PROGMEM = "Flow";
const char MSG_FLOW_CZ[] PROGMEM = "Prutok";
const char MSG_FLOW_IT[] PROGMEM = "Flusso";
const char MSG_FLOW_ES[] PROGMEM = "Flujo";
const char MSG_FLOW_PL[] PROGMEM = "Przeplyw";
const char MSG_FLOW_DE[] PROGMEM = "Durchfluss";
const char * const MSG_FLOW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FLOW_EN,
	MSG_FLOW_CZ,
	MSG_FLOW_IT,
	MSG_FLOW_ES,
	MSG_FLOW_PL,
	MSG_FLOW_DE
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
const char MSG_FOLLOW_CALIBRATION_FLOW_IT[] PROGMEM = "Stampante ancora non calibrata. Si prega di seguire il manuale, capitolo PRIMI PASSI, sezione della calibrazione.";
const char MSG_FOLLOW_CALIBRATION_FLOW_ES[] PROGMEM = "Impresora no esta calibrada todavia. Por favor usa el manual, capitulo First steps, Calibration flow.";
const char MSG_FOLLOW_CALIBRATION_FLOW_PL[] PROGMEM = "Drukarka nie zostala jeszcze skalibrowana. Prosze kierowac sie instrukcja, rozdzial Zaczynamy, podrozdzial Selftest.";
const char MSG_FOLLOW_CALIBRATION_FLOW_DE[] PROGMEM = "Der Drucker wurde noch nicht kalibriert. Bitte folgen Sie das Handbuch, Kapitel First steps, Abschnitt Calibration flow.";
const char * const MSG_FOLLOW_CALIBRATION_FLOW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_FOLLOW_CALIBRATION_FLOW_EN,
	MSG_FOLLOW_CALIBRATION_FLOW_CZ,
	MSG_FOLLOW_CALIBRATION_FLOW_IT,
	MSG_FOLLOW_CALIBRATION_FLOW_ES,
	MSG_FOLLOW_CALIBRATION_FLOW_PL,
	MSG_FOLLOW_CALIBRATION_FLOW_DE
};

const char MSG_FREE_MEMORY_EN[] PROGMEM = " Free Memory: ";
const char * const MSG_FREE_MEMORY_LANG_TABLE[1] PROGMEM = {
	MSG_FREE_MEMORY_EN
};

const char MSG_HEATING_EN[] PROGMEM = "Heating";
const char MSG_HEATING_CZ[] PROGMEM = "Zahrivani";
const char MSG_HEATING_IT[] PROGMEM = "Riscaldamento...";
const char MSG_HEATING_ES[] PROGMEM = "Calentando...";
const char MSG_HEATING_PL[] PROGMEM = "Grzanie...";
const char MSG_HEATING_DE[] PROGMEM = "Aufwaermen";
const char * const MSG_HEATING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HEATING_EN,
	MSG_HEATING_CZ,
	MSG_HEATING_IT,
	MSG_HEATING_ES,
	MSG_HEATING_PL,
	MSG_HEATING_DE
};

const char MSG_HEATING_COMPLETE_EN[] PROGMEM = "Heating done.";
const char MSG_HEATING_COMPLETE_CZ[] PROGMEM = "Zahrivani OK.";
const char MSG_HEATING_COMPLETE_IT[] PROGMEM = "Riscald. completo";
const char MSG_HEATING_COMPLETE_ES[] PROGMEM = "Calentamiento final.";
const char MSG_HEATING_COMPLETE_PL[] PROGMEM = "Grzanie OK.";
const char MSG_HEATING_COMPLETE_DE[] PROGMEM = "Aufwaermen OK";
const char * const MSG_HEATING_COMPLETE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HEATING_COMPLETE_EN,
	MSG_HEATING_COMPLETE_CZ,
	MSG_HEATING_COMPLETE_IT,
	MSG_HEATING_COMPLETE_ES,
	MSG_HEATING_COMPLETE_PL,
	MSG_HEATING_COMPLETE_DE
};

const char MSG_HOMEYZ_EN[] PROGMEM = "Calibrate Z";
const char MSG_HOMEYZ_CZ[] PROGMEM = "Kalibrovat Z";
const char MSG_HOMEYZ_IT[] PROGMEM = "Calibra Z";
const char MSG_HOMEYZ_ES[] PROGMEM = "Calibrar Z";
const char MSG_HOMEYZ_PL[] PROGMEM = "Kalibruj Z";
const char MSG_HOMEYZ_DE[] PROGMEM = "Kalibrieren Z";
const char * const MSG_HOMEYZ_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HOMEYZ_EN,
	MSG_HOMEYZ_CZ,
	MSG_HOMEYZ_IT,
	MSG_HOMEYZ_ES,
	MSG_HOMEYZ_PL,
	MSG_HOMEYZ_DE
};

const char MSG_HOMEYZ_DONE_EN[] PROGMEM = "Calibration done";
const char MSG_HOMEYZ_DONE_CZ[] PROGMEM = "Kalibrace OK";
const char MSG_HOMEYZ_DONE_IT[] PROGMEM = "Calibrazione OK";
const char MSG_HOMEYZ_DONE_ES[] PROGMEM = "Calibracion OK";
const char MSG_HOMEYZ_DONE_PL[] PROGMEM = "Kalibracja OK";
const char MSG_HOMEYZ_DONE_DE[] PROGMEM = "Kalibrierung OK";
const char * const MSG_HOMEYZ_DONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HOMEYZ_DONE_EN,
	MSG_HOMEYZ_DONE_CZ,
	MSG_HOMEYZ_DONE_IT,
	MSG_HOMEYZ_DONE_ES,
	MSG_HOMEYZ_DONE_PL,
	MSG_HOMEYZ_DONE_DE
};

const char MSG_HOMEYZ_PROGRESS_EN[] PROGMEM = "Calibrating Z";
const char MSG_HOMEYZ_PROGRESS_CZ[] PROGMEM = "Kalibruji Z";
const char MSG_HOMEYZ_PROGRESS_IT[] PROGMEM = "Calibrando Z";
const char MSG_HOMEYZ_PROGRESS_ES[] PROGMEM = "Calibrando Z";
const char MSG_HOMEYZ_PROGRESS_PL[] PROGMEM = "Kalibruje Z";
const char MSG_HOMEYZ_PROGRESS_DE[] PROGMEM = "Kalibriere Z";
const char * const MSG_HOMEYZ_PROGRESS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_HOMEYZ_PROGRESS_EN,
	MSG_HOMEYZ_PROGRESS_CZ,
	MSG_HOMEYZ_PROGRESS_IT,
	MSG_HOMEYZ_PROGRESS_ES,
	MSG_HOMEYZ_PROGRESS_PL,
	MSG_HOMEYZ_PROGRESS_DE
};

const char MSG_HOTEND_OFFSET_EN[] PROGMEM = "Hotend offsets:";
const char * const MSG_HOTEND_OFFSET_LANG_TABLE[1] PROGMEM = {
	MSG_HOTEND_OFFSET_EN
};

const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_EN[] PROGMEM = "Improving bed calibration point";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_CZ[] PROGMEM = "Zlepsuji presnost kalibracniho bodu";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_IT[] PROGMEM = "Perfezion. il letto punto di calibraz.";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_ES[] PROGMEM = "Mejorando punto de calibracion heatbed";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_PL[] PROGMEM = "Poprawiam precyzyjnosc punktu kalibracyjnego";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_DE[] PROGMEM = "Verbesserung Bett Kalibrierpunkt";
const char * const MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_EN,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_CZ,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_IT,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_ES,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_PL,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1_DE
};

const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_EN[] PROGMEM = " of 9";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_CZ[] PROGMEM = " z 9";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_IT[] PROGMEM = " su 9";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_ES[] PROGMEM = " de 9";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_PL[] PROGMEM = " z 9";
const char MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_DE[] PROGMEM = " von 9";
const char * const MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_EN,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_CZ,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_IT,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_ES,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_PL,
	MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2_DE
};

const char MSG_INIT_SDCARD_EN[] PROGMEM = "Init. SD card";
const char * const MSG_INIT_SDCARD_LANG_TABLE[1] PROGMEM = {
	MSG_INIT_SDCARD_EN
};

const char MSG_INSERT_FILAMENT_EN[] PROGMEM = "Insert filament";
const char MSG_INSERT_FILAMENT_CZ[] PROGMEM = "Vlozte filament";
const char MSG_INSERT_FILAMENT_IT[] PROGMEM = "Inserire filamento";
const char MSG_INSERT_FILAMENT_ES[] PROGMEM = "Introducir filamento";
const char MSG_INSERT_FILAMENT_PL[] PROGMEM = "Wprowadz filament";
const char MSG_INSERT_FILAMENT_DE[] PROGMEM = "Filament einlegen";
const char * const MSG_INSERT_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_INSERT_FILAMENT_EN,
	MSG_INSERT_FILAMENT_CZ,
	MSG_INSERT_FILAMENT_IT,
	MSG_INSERT_FILAMENT_ES,
	MSG_INSERT_FILAMENT_PL,
	MSG_INSERT_FILAMENT_DE
};

const char MSG_INVALID_EXTRUDER_EN[] PROGMEM = "Invalid extruder";
const char * const MSG_INVALID_EXTRUDER_LANG_TABLE[1] PROGMEM = {
	MSG_INVALID_EXTRUDER_EN
};

const char MSG_KILLED_EN[] PROGMEM = "KILLED. ";
const char MSG_KILLED_IT[] PROGMEM = "IN TILT.";
const char MSG_KILLED_ES[] PROGMEM = "PARADA DE EMERGENCIA";
const char MSG_KILLED_DE[] PROGMEM = "ABGEBROCHEN. ";
const char * const MSG_KILLED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_KILLED_EN,
	MSG_KILLED_EN,
	MSG_KILLED_IT,
	MSG_KILLED_ES,
	MSG_KILLED_EN,
	MSG_KILLED_DE
};

const char MSG_LANGUAGE_NAME_EN[] PROGMEM = "English";
const char MSG_LANGUAGE_NAME_CZ[] PROGMEM = "Cestina";
const char MSG_LANGUAGE_NAME_IT[] PROGMEM = "Italiano";
const char MSG_LANGUAGE_NAME_ES[] PROGMEM = "Espanol";
const char MSG_LANGUAGE_NAME_PL[] PROGMEM = "Polski";
const char MSG_LANGUAGE_NAME_DE[] PROGMEM = "Deutsch";
const char * const MSG_LANGUAGE_NAME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LANGUAGE_NAME_EN,
	MSG_LANGUAGE_NAME_CZ,
	MSG_LANGUAGE_NAME_IT,
	MSG_LANGUAGE_NAME_ES,
	MSG_LANGUAGE_NAME_PL,
	MSG_LANGUAGE_NAME_DE
};

const char MSG_LANGUAGE_SELECT_EN[] PROGMEM = "Select language";
const char MSG_LANGUAGE_SELECT_CZ[] PROGMEM = "Vyber jazyka";
const char MSG_LANGUAGE_SELECT_IT[] PROGMEM = "Seleziona lingua";
const char MSG_LANGUAGE_SELECT_ES[] PROGMEM = "Cambiae el idioma";
const char MSG_LANGUAGE_SELECT_PL[] PROGMEM = "Wybor jezyka";
const char MSG_LANGUAGE_SELECT_DE[] PROGMEM = "Waehle Sprache";
const char * const MSG_LANGUAGE_SELECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LANGUAGE_SELECT_EN,
	MSG_LANGUAGE_SELECT_CZ,
	MSG_LANGUAGE_SELECT_IT,
	MSG_LANGUAGE_SELECT_ES,
	MSG_LANGUAGE_SELECT_PL,
	MSG_LANGUAGE_SELECT_DE
};

const char MSG_LEFT_EN[] PROGMEM = "Left:";
const char MSG_LEFT_CZ[] PROGMEM = "Levy:";
const char MSG_LEFT_IT[] PROGMEM = "Sinistra:";
const char MSG_LEFT_ES[] PROGMEM = "Izquierda:";
const char MSG_LEFT_PL[] PROGMEM = "Lewy:";
const char MSG_LEFT_DE[] PROGMEM = "Links:";
const char * const MSG_LEFT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LEFT_EN,
	MSG_LEFT_CZ,
	MSG_LEFT_IT,
	MSG_LEFT_ES,
	MSG_LEFT_PL,
	MSG_LEFT_DE
};

const char MSG_LOADING_COLOR_EN[] PROGMEM = "Loading color";
const char MSG_LOADING_COLOR_CZ[] PROGMEM = "Cisteni barvy";
const char MSG_LOADING_COLOR_IT[] PROGMEM = "Caricando colore";
const char MSG_LOADING_COLOR_ES[] PROGMEM = "Cambiando color";
const char MSG_LOADING_COLOR_PL[] PROGMEM = "Czyszcz. koloru";
const char MSG_LOADING_COLOR_DE[] PROGMEM = "Lade Farbe";
const char * const MSG_LOADING_COLOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOADING_COLOR_EN,
	MSG_LOADING_COLOR_CZ,
	MSG_LOADING_COLOR_IT,
	MSG_LOADING_COLOR_ES,
	MSG_LOADING_COLOR_PL,
	MSG_LOADING_COLOR_DE
};

const char MSG_LOADING_FILAMENT_EN[] PROGMEM = "Loading filament";
const char MSG_LOADING_FILAMENT_CZ[] PROGMEM = "Zavadeni filamentu";
const char MSG_LOADING_FILAMENT_IT[] PROGMEM = "Caricando filam.";
const char MSG_LOADING_FILAMENT_ES[] PROGMEM = "Introduciendo filam.";
const char MSG_LOADING_FILAMENT_PL[] PROGMEM = "Wprow. filamentu";
const char MSG_LOADING_FILAMENT_DE[] PROGMEM = "Filament leadt";
const char * const MSG_LOADING_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOADING_FILAMENT_EN,
	MSG_LOADING_FILAMENT_CZ,
	MSG_LOADING_FILAMENT_IT,
	MSG_LOADING_FILAMENT_ES,
	MSG_LOADING_FILAMENT_PL,
	MSG_LOADING_FILAMENT_DE
};

const char MSG_LOAD_ALL_EN[] PROGMEM = "Load all";
const char MSG_LOAD_ALL_CZ[] PROGMEM = "Zavest vse";
const char MSG_LOAD_ALL_IT[] PROGMEM = "Caricare tutti";
const char MSG_LOAD_ALL_ES[] PROGMEM = "Intr. todos fil.";
const char MSG_LOAD_ALL_PL[] PROGMEM = "Zalad. wszystkie";
const char MSG_LOAD_ALL_DE[] PROGMEM = "Alle laden";
const char * const MSG_LOAD_ALL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_ALL_EN,
	MSG_LOAD_ALL_CZ,
	MSG_LOAD_ALL_IT,
	MSG_LOAD_ALL_ES,
	MSG_LOAD_ALL_PL,
	MSG_LOAD_ALL_DE
};

const char MSG_LOAD_EPROM_EN[] PROGMEM = "Load memory";
const char * const MSG_LOAD_EPROM_LANG_TABLE[1] PROGMEM = {
	MSG_LOAD_EPROM_EN
};

const char MSG_LOAD_FILAMENT_EN[] PROGMEM = "Load filament";
const char MSG_LOAD_FILAMENT_CZ[] PROGMEM = "Zavest filament";
const char MSG_LOAD_FILAMENT_IT[] PROGMEM = "Carica filamento";
const char MSG_LOAD_FILAMENT_ES[] PROGMEM = "Introducir filam.";
const char MSG_LOAD_FILAMENT_PL[] PROGMEM = "Wprowadz filament";
const char MSG_LOAD_FILAMENT_DE[] PROGMEM = "Filament laden";
const char * const MSG_LOAD_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_EN,
	MSG_LOAD_FILAMENT_CZ,
	MSG_LOAD_FILAMENT_IT,
	MSG_LOAD_FILAMENT_ES,
	MSG_LOAD_FILAMENT_PL,
	MSG_LOAD_FILAMENT_DE
};

const char MSG_LOAD_FILAMENT_1_EN[] PROGMEM = "Load filament 1";
const char MSG_LOAD_FILAMENT_1_CZ[] PROGMEM = "Zavest filament 1";
const char MSG_LOAD_FILAMENT_1_IT[] PROGMEM = "Caricare fil. 1";
const char MSG_LOAD_FILAMENT_1_ES[] PROGMEM = "Introducir fil. 1";
const char MSG_LOAD_FILAMENT_1_PL[] PROGMEM = "Zaladowac fil. 1";
const char MSG_LOAD_FILAMENT_1_DE[] PROGMEM = "Filament 1 laden";
const char * const MSG_LOAD_FILAMENT_1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_1_EN,
	MSG_LOAD_FILAMENT_1_CZ,
	MSG_LOAD_FILAMENT_1_IT,
	MSG_LOAD_FILAMENT_1_ES,
	MSG_LOAD_FILAMENT_1_PL,
	MSG_LOAD_FILAMENT_1_DE
};

const char MSG_LOAD_FILAMENT_2_EN[] PROGMEM = "Load filament 2";
const char MSG_LOAD_FILAMENT_2_CZ[] PROGMEM = "Zavest filament 2";
const char MSG_LOAD_FILAMENT_2_IT[] PROGMEM = "Caricare fil. 2";
const char MSG_LOAD_FILAMENT_2_ES[] PROGMEM = "Introducir fil. 2";
const char MSG_LOAD_FILAMENT_2_PL[] PROGMEM = "Zaladowac fil. 2";
const char MSG_LOAD_FILAMENT_2_DE[] PROGMEM = "Filament 2 laden";
const char * const MSG_LOAD_FILAMENT_2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_2_EN,
	MSG_LOAD_FILAMENT_2_CZ,
	MSG_LOAD_FILAMENT_2_IT,
	MSG_LOAD_FILAMENT_2_ES,
	MSG_LOAD_FILAMENT_2_PL,
	MSG_LOAD_FILAMENT_2_DE
};

const char MSG_LOAD_FILAMENT_3_EN[] PROGMEM = "Load filament 3";
const char MSG_LOAD_FILAMENT_3_CZ[] PROGMEM = "Zavest filament 3";
const char MSG_LOAD_FILAMENT_3_IT[] PROGMEM = "Caricare fil. 3";
const char MSG_LOAD_FILAMENT_3_ES[] PROGMEM = "Introducir fil. 3";
const char MSG_LOAD_FILAMENT_3_PL[] PROGMEM = "Zaladowac fil. 3";
const char MSG_LOAD_FILAMENT_3_DE[] PROGMEM = "Filament 3 laden";
const char * const MSG_LOAD_FILAMENT_3_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_3_EN,
	MSG_LOAD_FILAMENT_3_CZ,
	MSG_LOAD_FILAMENT_3_IT,
	MSG_LOAD_FILAMENT_3_ES,
	MSG_LOAD_FILAMENT_3_PL,
	MSG_LOAD_FILAMENT_3_DE
};

const char MSG_LOAD_FILAMENT_4_EN[] PROGMEM = "Load filament 4";
const char MSG_LOAD_FILAMENT_4_CZ[] PROGMEM = "Zavest filament 4";
const char MSG_LOAD_FILAMENT_4_IT[] PROGMEM = "Caricare fil. 4";
const char MSG_LOAD_FILAMENT_4_ES[] PROGMEM = "Introducir fil. 4";
const char MSG_LOAD_FILAMENT_4_PL[] PROGMEM = "Zaladowac fil. 4";
const char MSG_LOAD_FILAMENT_4_DE[] PROGMEM = "Filament 4 laden";
const char * const MSG_LOAD_FILAMENT_4_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOAD_FILAMENT_4_EN,
	MSG_LOAD_FILAMENT_4_CZ,
	MSG_LOAD_FILAMENT_4_IT,
	MSG_LOAD_FILAMENT_4_ES,
	MSG_LOAD_FILAMENT_4_PL,
	MSG_LOAD_FILAMENT_4_DE
};

const char MSG_LOOSE_PULLEY_EN[] PROGMEM = "Loose pulley";
const char MSG_LOOSE_PULLEY_CZ[] PROGMEM = "Uvolnena remenicka";
const char MSG_LOOSE_PULLEY_IT[] PROGMEM = "Puleggia lenta";
const char MSG_LOOSE_PULLEY_ES[] PROGMEM = "Polea suelta";
const char MSG_LOOSE_PULLEY_PL[] PROGMEM = "Kolo pasowe";
const char MSG_LOOSE_PULLEY_DE[] PROGMEM = "Lose Riemenscheibe";
const char * const MSG_LOOSE_PULLEY_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_LOOSE_PULLEY_EN,
	MSG_LOOSE_PULLEY_CZ,
	MSG_LOOSE_PULLEY_IT,
	MSG_LOOSE_PULLEY_ES,
	MSG_LOOSE_PULLEY_PL,
	MSG_LOOSE_PULLEY_DE
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
const char MSG_M117_V2_CALIBRATION_IT[] PROGMEM = "M117 Cal. primo layer.";
const char MSG_M117_V2_CALIBRATION_ES[] PROGMEM = "M117 Cal. primera cap.";
const char MSG_M117_V2_CALIBRATION_PL[] PROGMEM = "M117 Kal. 1. warstwy";
const char MSG_M117_V2_CALIBRATION_DE[] PROGMEM = "M117 Erste-Schicht Kal.";
const char * const MSG_M117_V2_CALIBRATION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_M117_V2_CALIBRATION_EN,
	MSG_M117_V2_CALIBRATION_CZ,
	MSG_M117_V2_CALIBRATION_IT,
	MSG_M117_V2_CALIBRATION_ES,
	MSG_M117_V2_CALIBRATION_PL,
	MSG_M117_V2_CALIBRATION_DE
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
const char MSG_MAIN_IT[] PROGMEM = "Menu principale";
const char MSG_MAIN_ES[] PROGMEM = "Menu principal";
const char MSG_MAIN_PL[] PROGMEM = "Menu glowne";
const char MSG_MAIN_DE[] PROGMEM = "Hauptmenue";
const char * const MSG_MAIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MAIN_EN,
	MSG_MAIN_CZ,
	MSG_MAIN_IT,
	MSG_MAIN_ES,
	MSG_MAIN_PL,
	MSG_MAIN_DE
};

const char MSG_MARK_FIL_EN[] PROGMEM = "Mark filament 100mm from extruder body. Click when done.";
const char MSG_MARK_FIL_CZ[] PROGMEM = "Oznacte filament 100 mm od tela extruderu a po te potvrdte tlacitkem.";
const char MSG_MARK_FIL_IT[] PROGMEM = "Segnare il filamento a 100 mm di distanza dal corpo dell'estrusore. Click per continuare.";
const char MSG_MARK_FIL_ES[] PROGMEM = "Marque el filamento 100 mm por encima del final del extrusor. Haz clic una vez terminado.";
const char MSG_MARK_FIL_PL[] PROGMEM = "Prosze oznaczyc filament 100 mm od ciala ekstrudera. Potwierdzic przyciskiem.";
const char MSG_MARK_FIL_DE[] PROGMEM = "Filament 100mm vom Extrudergehaeuse markieren. Klicken wenn Fertig.";
const char * const MSG_MARK_FIL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MARK_FIL_EN,
	MSG_MARK_FIL_CZ,
	MSG_MARK_FIL_IT,
	MSG_MARK_FIL_ES,
	MSG_MARK_FIL_PL,
	MSG_MARK_FIL_DE
};

const char MSG_MAX_EN[] PROGMEM = " \002 Max";
const char * const MSG_MAX_LANG_TABLE[1] PROGMEM = {
	MSG_MAX_EN
};

const char MSG_MEASURED_SKEW_EN[] PROGMEM = "Measured skew:";
const char MSG_MEASURED_SKEW_CZ[] PROGMEM = "Merene zkoseni:";
const char MSG_MEASURED_SKEW_IT[] PROGMEM = "Incl. misurata:";
const char MSG_MEASURED_SKEW_ES[] PROGMEM = "Inclin. medida:";
const char MSG_MEASURED_SKEW_PL[] PROGMEM = "Zmier. sciecie:";
const char MSG_MEASURED_SKEW_DE[] PROGMEM = "Schraeglauf:";
const char * const MSG_MEASURED_SKEW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MEASURED_SKEW_EN,
	MSG_MEASURED_SKEW_CZ,
	MSG_MEASURED_SKEW_IT,
	MSG_MEASURED_SKEW_ES,
	MSG_MEASURED_SKEW_PL,
	MSG_MEASURED_SKEW_DE
};

const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_EN[] PROGMEM = "Measuring reference height of calibration point";
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_CZ[] PROGMEM = "Merim referencni vysku kalibracniho bodu";
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_IT[] PROGMEM = "Misurare l'altezza di riferimento del punto di calibrazione";
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_ES[] PROGMEM = "Midiendo altura del punto de calibracion";
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_PL[] PROGMEM = "Okreslam wysokosc odniesienia punktu kalibracyjnego";
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_DE[] PROGMEM = "Messen der Referenzhoehe des Kalibrierpunktes";
const char * const MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_EN,
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_CZ,
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_IT,
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_ES,
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_PL,
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1_DE
};

const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_EN[] PROGMEM = " of 9";
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_CZ[] PROGMEM = " z 9";
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_IT[] PROGMEM = " su 9";
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_ES[] PROGMEM = " de 9";
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_PL[] PROGMEM = " z 9";
const char MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_DE[] PROGMEM = " von 9";
const char * const MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_EN,
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_CZ,
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_IT,
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_ES,
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_PL,
	MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2_DE
};

const char MSG_MENU_CALIBRATION_EN[] PROGMEM = "Calibration";
const char MSG_MENU_CALIBRATION_CZ[] PROGMEM = "Kalibrace";
const char MSG_MENU_CALIBRATION_IT[] PROGMEM = "Calibrazione";
const char MSG_MENU_CALIBRATION_ES[] PROGMEM = "Calibracion";
const char MSG_MENU_CALIBRATION_PL[] PROGMEM = "Kalibracja";
const char MSG_MENU_CALIBRATION_DE[] PROGMEM = "Kalibrierung";
const char * const MSG_MENU_CALIBRATION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MENU_CALIBRATION_EN,
	MSG_MENU_CALIBRATION_CZ,
	MSG_MENU_CALIBRATION_IT,
	MSG_MENU_CALIBRATION_ES,
	MSG_MENU_CALIBRATION_PL,
	MSG_MENU_CALIBRATION_DE
};

const char MSG_MESH_BED_LEVELING_EN[] PROGMEM = "Mesh Bed Leveling";
const char MSG_MESH_BED_LEVELING_IT[] PROGMEM = "Mesh livel. letto";
const char MSG_MESH_BED_LEVELING_DE[] PROGMEM = "Mesh Bett Leveling";
const char * const MSG_MESH_BED_LEVELING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MESH_BED_LEVELING_EN,
	MSG_MESH_BED_LEVELING_EN,
	MSG_MESH_BED_LEVELING_IT,
	MSG_MESH_BED_LEVELING_EN,
	MSG_MESH_BED_LEVELING_EN,
	MSG_MESH_BED_LEVELING_DE
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
const char MSG_MOVE_AXIS_IT[] PROGMEM = "Muovi asse";
const char MSG_MOVE_AXIS_ES[] PROGMEM = "Mover ejes";
const char MSG_MOVE_AXIS_PL[] PROGMEM = "Ruch osi";
const char MSG_MOVE_AXIS_DE[] PROGMEM = "Achsbewegung";
const char * const MSG_MOVE_AXIS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_AXIS_EN,
	MSG_MOVE_AXIS_CZ,
	MSG_MOVE_AXIS_IT,
	MSG_MOVE_AXIS_ES,
	MSG_MOVE_AXIS_PL,
	MSG_MOVE_AXIS_DE
};

const char MSG_MOVE_CARRIAGE_TO_THE_TOP_EN[] PROGMEM = "Calibrating XYZ. Rotate the knob to move the Z carriage up to the end stoppers. Click when done.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_CZ[] PROGMEM = "Kalibrace XYZ. Otacenim tlacitka posunte Z osu az k~hornimu dorazu. Potvrdte tlacitkem.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_IT[] PROGMEM = "Calibrazione XYZ. Ruotare la manopola per alzare il carrello Z fino all'altezza massima. Click per terminare.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_ES[] PROGMEM = "Calibrando XYZ. Gira el boton para subir el extrusor hasta tocar los topes superiores. Despues haz clic.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_PL[] PROGMEM = "Kalibracja XYZ. Przekrec galke, aby przesunac os Z do gornych krancowek. Nacisnij, by potwierdzic.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_DE[] PROGMEM = "Kalibrieren von XYZ. Drehen Sie den Knopf bis der obere Anschlag erreicht wird. Klicken Sie den Knopf wenn es ganz oben wird.";
const char * const MSG_MOVE_CARRIAGE_TO_THE_TOP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_CARRIAGE_TO_THE_TOP_EN,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_CZ,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_IT,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_ES,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_PL,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_DE
};

const char MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_EN[] PROGMEM = "Calibrating Z. Rotate the knob to move the Z carriage up to the end stoppers. Click when done.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_CZ[] PROGMEM = "Kalibrace Z. Otacenim tlacitka posunte Z osu az k~hornimu dorazu. Potvrdte tlacitkem.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_IT[] PROGMEM = "Calibrazione Z. Ruotare la manopola per alzare il carrello Z fino all'altezza massima. Click per terminare.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_ES[] PROGMEM = "Calibrando Z. Gira el boton para subir el extrusor hasta tocar los topes superiores. Despues haz clic.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_PL[] PROGMEM = "Kalibracja Z. Przekrec galke, aby przesunac os Z do gornych krancowek. Nacisnij, by potwierdzic.";
const char MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_DE[] PROGMEM = "Kalibrieren von Z. Drehen Sie den Knopf bis der obere Anschlag erreicht wird. Klicken Sie den Knopf wenn es ganz oben wird.";
const char * const MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_EN,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_CZ,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_IT,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_ES,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_PL,
	MSG_MOVE_CARRIAGE_TO_THE_TOP_Z_DE
};

const char MSG_MOVE_E_EN[] PROGMEM = "Extruder";
const char MSG_MOVE_E_IT[] PROGMEM = "Muovi Estrusore";
const char MSG_MOVE_E_ES[] PROGMEM = "Extruir";
const char * const MSG_MOVE_E_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_E_EN,
	MSG_MOVE_E_EN,
	MSG_MOVE_E_IT,
	MSG_MOVE_E_ES,
	MSG_MOVE_E_EN,
	MSG_MOVE_E_EN
};

const char MSG_MOVE_X_EN[] PROGMEM = "Move X";
const char MSG_MOVE_X_CZ[] PROGMEM = "Posunout X";
const char MSG_MOVE_X_IT[] PROGMEM = "Muovi X";
const char MSG_MOVE_X_ES[] PROGMEM = "Mover X";
const char MSG_MOVE_X_PL[] PROGMEM = "Przesunac X";
const char MSG_MOVE_X_DE[] PROGMEM = "Bewege X";
const char * const MSG_MOVE_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_X_EN,
	MSG_MOVE_X_CZ,
	MSG_MOVE_X_IT,
	MSG_MOVE_X_ES,
	MSG_MOVE_X_PL,
	MSG_MOVE_X_DE
};

const char MSG_MOVE_Y_EN[] PROGMEM = "Move Y";
const char MSG_MOVE_Y_CZ[] PROGMEM = "Posunout Y";
const char MSG_MOVE_Y_IT[] PROGMEM = "Muovi Y";
const char MSG_MOVE_Y_ES[] PROGMEM = "Mover Y";
const char MSG_MOVE_Y_PL[] PROGMEM = "Przesunac Y";
const char MSG_MOVE_Y_DE[] PROGMEM = "Bewege Y";
const char * const MSG_MOVE_Y_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_Y_EN,
	MSG_MOVE_Y_CZ,
	MSG_MOVE_Y_IT,
	MSG_MOVE_Y_ES,
	MSG_MOVE_Y_PL,
	MSG_MOVE_Y_DE
};

const char MSG_MOVE_Z_EN[] PROGMEM = "Move Z";
const char MSG_MOVE_Z_CZ[] PROGMEM = "Posunout Z";
const char MSG_MOVE_Z_IT[] PROGMEM = "Muovi Z";
const char MSG_MOVE_Z_ES[] PROGMEM = "Mover Z";
const char MSG_MOVE_Z_PL[] PROGMEM = "Przesunac Z";
const char MSG_MOVE_Z_DE[] PROGMEM = "Bewege Z";
const char * const MSG_MOVE_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_MOVE_Z_EN,
	MSG_MOVE_Z_CZ,
	MSG_MOVE_Z_IT,
	MSG_MOVE_Z_ES,
	MSG_MOVE_Z_PL,
	MSG_MOVE_Z_DE
};

const char MSG_NEW_FIRMWARE_AVAILABLE_EN[] PROGMEM = "New firmware version available:";
const char MSG_NEW_FIRMWARE_AVAILABLE_CZ[] PROGMEM = "Vysla nova verze firmware:";
const char MSG_NEW_FIRMWARE_AVAILABLE_IT[] PROGMEM = "Nuova versione del firmware disponibile";
const char MSG_NEW_FIRMWARE_AVAILABLE_ES[] PROGMEM = "Nuevo firmware disponible:";
const char MSG_NEW_FIRMWARE_AVAILABLE_PL[] PROGMEM = "Wyszla nowa wersja firmware:";
const char MSG_NEW_FIRMWARE_AVAILABLE_DE[] PROGMEM = "Neue Firmware Version verfuegbar:";
const char * const MSG_NEW_FIRMWARE_AVAILABLE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NEW_FIRMWARE_AVAILABLE_EN,
	MSG_NEW_FIRMWARE_AVAILABLE_CZ,
	MSG_NEW_FIRMWARE_AVAILABLE_IT,
	MSG_NEW_FIRMWARE_AVAILABLE_ES,
	MSG_NEW_FIRMWARE_AVAILABLE_PL,
	MSG_NEW_FIRMWARE_AVAILABLE_DE
};

const char MSG_NEW_FIRMWARE_PLEASE_UPGRADE_EN[] PROGMEM = "Please upgrade.";
const char MSG_NEW_FIRMWARE_PLEASE_UPGRADE_CZ[] PROGMEM = "Prosim aktualizujte.";
const char MSG_NEW_FIRMWARE_PLEASE_UPGRADE_IT[] PROGMEM = "Prega aggiorna.";
const char MSG_NEW_FIRMWARE_PLEASE_UPGRADE_ES[] PROGMEM = "Actualizar por favor";
const char MSG_NEW_FIRMWARE_PLEASE_UPGRADE_PL[] PROGMEM = "Prosze zaktualizowac";
const char MSG_NEW_FIRMWARE_PLEASE_UPGRADE_DE[] PROGMEM = "Bitte aktualisieren.";
const char * const MSG_NEW_FIRMWARE_PLEASE_UPGRADE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NEW_FIRMWARE_PLEASE_UPGRADE_EN,
	MSG_NEW_FIRMWARE_PLEASE_UPGRADE_CZ,
	MSG_NEW_FIRMWARE_PLEASE_UPGRADE_IT,
	MSG_NEW_FIRMWARE_PLEASE_UPGRADE_ES,
	MSG_NEW_FIRMWARE_PLEASE_UPGRADE_PL,
	MSG_NEW_FIRMWARE_PLEASE_UPGRADE_DE
};

const char MSG_NO_EN[] PROGMEM = "No";
const char MSG_NO_CZ[] PROGMEM = "Ne";
const char MSG_NO_PL[] PROGMEM = "Nie";
const char MSG_NO_DE[] PROGMEM = "Nein";
const char * const MSG_NO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NO_EN,
	MSG_NO_CZ,
	MSG_NO_EN,
	MSG_NO_EN,
	MSG_NO_PL,
	MSG_NO_DE
};

const char MSG_NOT_COLOR_EN[] PROGMEM = "Color not clear";
const char MSG_NOT_COLOR_CZ[] PROGMEM = "Barva neni cista";
const char MSG_NOT_COLOR_IT[] PROGMEM = "Colore non puro";
const char MSG_NOT_COLOR_ES[] PROGMEM = "Color no homogeneo";
const char MSG_NOT_COLOR_PL[] PROGMEM = "Kolor zanieczysz.";
const char MSG_NOT_COLOR_DE[] PROGMEM = "Farbe unklar";
const char * const MSG_NOT_COLOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOT_COLOR_EN,
	MSG_NOT_COLOR_CZ,
	MSG_NOT_COLOR_IT,
	MSG_NOT_COLOR_ES,
	MSG_NOT_COLOR_PL,
	MSG_NOT_COLOR_DE
};

const char MSG_NOT_LOADED_EN[] PROGMEM = "Filament not loaded";
const char MSG_NOT_LOADED_CZ[] PROGMEM = "Filament nezaveden";
const char MSG_NOT_LOADED_IT[] PROGMEM = "Fil. non caricato";
const char MSG_NOT_LOADED_ES[] PROGMEM = "Fil. no introducido";
const char MSG_NOT_LOADED_PL[] PROGMEM = "Brak filamentu";
const char MSG_NOT_LOADED_DE[] PROGMEM = "Fil. nicht geladen";
const char * const MSG_NOT_LOADED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOT_LOADED_EN,
	MSG_NOT_LOADED_CZ,
	MSG_NOT_LOADED_IT,
	MSG_NOT_LOADED_ES,
	MSG_NOT_LOADED_PL,
	MSG_NOT_LOADED_DE
};

const char MSG_NOZZLE_EN[] PROGMEM = "Nozzle";
const char MSG_NOZZLE_CZ[] PROGMEM = "Tryska";
const char MSG_NOZZLE_IT[] PROGMEM = "Ugello";
const char MSG_NOZZLE_PL[] PROGMEM = "Dysza";
const char MSG_NOZZLE_DE[] PROGMEM = "Duese";
const char * const MSG_NOZZLE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NOZZLE_EN,
	MSG_NOZZLE_CZ,
	MSG_NOZZLE_IT,
	MSG_NOZZLE_EN,
	MSG_NOZZLE_PL,
	MSG_NOZZLE_DE
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
const char MSG_NO_CARD_IT[] PROGMEM = "Nessuna SD";
const char MSG_NO_CARD_ES[] PROGMEM = "No hay tarjeta SD";
const char MSG_NO_CARD_PL[] PROGMEM = "Brak karty SD";
const char MSG_NO_CARD_DE[] PROGMEM = "Keine SD Karte";
const char * const MSG_NO_CARD_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NO_CARD_EN,
	MSG_NO_CARD_CZ,
	MSG_NO_CARD_IT,
	MSG_NO_CARD_ES,
	MSG_NO_CARD_PL,
	MSG_NO_CARD_DE
};

const char MSG_NO_MOVE_EN[] PROGMEM = "No move.";
const char MSG_NO_MOVE_IT[] PROGMEM = "Nessun movimento.";
const char MSG_NO_MOVE_ES[] PROGMEM = "Sin movimiento";
const char MSG_NO_MOVE_DE[] PROGMEM = "Keine Bewegung.";
const char * const MSG_NO_MOVE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_NO_MOVE_EN,
	MSG_NO_MOVE_EN,
	MSG_NO_MOVE_IT,
	MSG_NO_MOVE_ES,
	MSG_NO_MOVE_EN,
	MSG_NO_MOVE_DE
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
const char MSG_PAPER_IT[] PROGMEM = "Porre un foglio sotto l'ugello durante la calibrazione dei primi 4 punti. In caso l'ugello muova il foglio spegnere prontamente la stampante.";
const char MSG_PAPER_ES[] PROGMEM = "Colocar una hoja de papel sobre la superficie de impresion durante la calibracion de los primeros 4 puntos. Si la boquilla mueve el papel, apagar impresora inmediatamente.";
const char MSG_PAPER_PL[] PROGMEM = "Umiesc kartke papieru na podkladce i trzymaj pod dysza podczas pomiaru pierwszych 4 punktow. Jesli dysza zahaczy o papier, wylacz drukarke.";
const char MSG_PAPER_DE[] PROGMEM = "Legen ein Blatt Papier unter die Duese waehrend der Kalibrierung der ersten 4 Punkte. Wenn die Duese das Papier einklemmt, Drucker sofort ausschalten";
const char * const MSG_PAPER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PAPER_EN,
	MSG_PAPER_CZ,
	MSG_PAPER_IT,
	MSG_PAPER_ES,
	MSG_PAPER_PL,
	MSG_PAPER_DE
};

const char MSG_PAUSE_PRINT_EN[] PROGMEM = "Pause print";
const char MSG_PAUSE_PRINT_CZ[] PROGMEM = "Pozastavit tisk";
const char MSG_PAUSE_PRINT_IT[] PROGMEM = "Metti in pausa";
const char MSG_PAUSE_PRINT_ES[] PROGMEM = "Pausar impresion";
const char MSG_PAUSE_PRINT_PL[] PROGMEM = "Przerwac druk";
const char MSG_PAUSE_PRINT_DE[] PROGMEM = "Druck unterbrech.";
const char * const MSG_PAUSE_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PAUSE_PRINT_EN,
	MSG_PAUSE_PRINT_CZ,
	MSG_PAUSE_PRINT_IT,
	MSG_PAUSE_PRINT_ES,
	MSG_PAUSE_PRINT_PL,
	MSG_PAUSE_PRINT_DE
};

const char MSG_PICK_Z_EN[] PROGMEM = "Pick print";
const char MSG_PICK_Z_CZ[] PROGMEM = "Vyberte vytisk";
const char MSG_PICK_Z_ES[] PROGMEM = "Esc. Modelo Adecuado";
const char MSG_PICK_Z_PL[] PROGMEM = "Vyberte vytisk";
const char MSG_PICK_Z_DE[] PROGMEM = "Waehle Abdruck";
const char * const MSG_PICK_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PICK_Z_EN,
	MSG_PICK_Z_CZ,
	MSG_PICK_Z_EN,
	MSG_PICK_Z_ES,
	MSG_PICK_Z_PL,
	MSG_PICK_Z_DE
};

const char MSG_PID_EXTRUDER_EN[] PROGMEM = "PID calibration";
const char MSG_PID_EXTRUDER_CZ[] PROGMEM = "PID kalibrace";
const char MSG_PID_EXTRUDER_IT[] PROGMEM = "Calibrazione PID";
const char MSG_PID_EXTRUDER_ES[] PROGMEM = "Calibracion PID";
const char MSG_PID_EXTRUDER_PL[] PROGMEM = "Kalibracja PID";
const char MSG_PID_EXTRUDER_DE[] PROGMEM = "PID Kalibrierung";
const char * const MSG_PID_EXTRUDER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PID_EXTRUDER_EN,
	MSG_PID_EXTRUDER_CZ,
	MSG_PID_EXTRUDER_IT,
	MSG_PID_EXTRUDER_ES,
	MSG_PID_EXTRUDER_PL,
	MSG_PID_EXTRUDER_DE
};

const char MSG_PID_FINISHED_EN[] PROGMEM = "PID cal. finished";
const char MSG_PID_FINISHED_CZ[] PROGMEM = "PID kal. ukoncena";
const char MSG_PID_FINISHED_IT[] PROGMEM = "Cal. PID completa";
const char MSG_PID_FINISHED_ES[] PROGMEM = "Cal. PID terminada";
const char MSG_PID_FINISHED_PL[] PROGMEM = "Kal. PID zakonczona";
const char MSG_PID_FINISHED_DE[] PROGMEM = "PID Kalib. fertig";
const char * const MSG_PID_FINISHED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PID_FINISHED_EN,
	MSG_PID_FINISHED_CZ,
	MSG_PID_FINISHED_IT,
	MSG_PID_FINISHED_ES,
	MSG_PID_FINISHED_PL,
	MSG_PID_FINISHED_DE
};

const char MSG_PID_RUNNING_EN[] PROGMEM = "PID cal.           ";
const char MSG_PID_RUNNING_CZ[] PROGMEM = "PID kal.           ";
const char MSG_PID_RUNNING_IT[] PROGMEM = "Cal. PID";
const char MSG_PID_RUNNING_ES[] PROGMEM = "Cal. PID           ";
const char MSG_PID_RUNNING_PL[] PROGMEM = "Kal. PID";
const char MSG_PID_RUNNING_DE[] PROGMEM = "PID Kalib.";
const char * const MSG_PID_RUNNING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PID_RUNNING_EN,
	MSG_PID_RUNNING_CZ,
	MSG_PID_RUNNING_IT,
	MSG_PID_RUNNING_ES,
	MSG_PID_RUNNING_PL,
	MSG_PID_RUNNING_DE
};

const char MSG_PINDA_NOT_CALIBRATED_EN[] PROGMEM = "Temperature calibration has not been run yet";
const char MSG_PINDA_NOT_CALIBRATED_CZ[] PROGMEM = "Tiskarna nebyla teplotne zkalibrovana";
const char MSG_PINDA_NOT_CALIBRATED_IT[] PROGMEM = "Taratura della temperatura non ancora eseguita";
const char MSG_PINDA_NOT_CALIBRATED_ES[] PROGMEM = "La temperatura de calibracion no ha sido ajustada";
const char MSG_PINDA_NOT_CALIBRATED_PL[] PROGMEM = "Cieplna kalibracja nie byla przeprowadzona";
const char MSG_PINDA_NOT_CALIBRATED_DE[] PROGMEM = "Temperatur wurde nicht kalibriert";
const char * const MSG_PINDA_NOT_CALIBRATED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PINDA_NOT_CALIBRATED_EN,
	MSG_PINDA_NOT_CALIBRATED_CZ,
	MSG_PINDA_NOT_CALIBRATED_IT,
	MSG_PINDA_NOT_CALIBRATED_ES,
	MSG_PINDA_NOT_CALIBRATED_PL,
	MSG_PINDA_NOT_CALIBRATED_DE
};

const char MSG_PINDA_PREHEAT_EN[] PROGMEM = "PINDA Heating";
const char MSG_PINDA_PREHEAT_CZ[] PROGMEM = "Nahrivani PINDA";
const char MSG_PINDA_PREHEAT_IT[] PROGMEM = "Riscald. PINDA";
const char MSG_PINDA_PREHEAT_ES[] PROGMEM = "Calentando PINDA";
const char MSG_PINDA_PREHEAT_PL[] PROGMEM = "Grzanie PINDA";
const char MSG_PINDA_PREHEAT_DE[] PROGMEM = "PINDA erwaermen";
const char * const MSG_PINDA_PREHEAT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PINDA_PREHEAT_EN,
	MSG_PINDA_PREHEAT_CZ,
	MSG_PINDA_PREHEAT_IT,
	MSG_PINDA_PREHEAT_ES,
	MSG_PINDA_PREHEAT_PL,
	MSG_PINDA_PREHEAT_DE
};

const char MSG_PLANNER_BUFFER_BYTES_EN[] PROGMEM = "  PlannerBufferBytes: ";
const char * const MSG_PLANNER_BUFFER_BYTES_LANG_TABLE[1] PROGMEM = {
	MSG_PLANNER_BUFFER_BYTES_EN
};

const char MSG_PLA_FILAMENT_LOADED_EN[] PROGMEM = "Is PLA filament loaded?";
const char MSG_PLA_FILAMENT_LOADED_CZ[] PROGMEM = "Je PLA filament zaveden?";
const char MSG_PLA_FILAMENT_LOADED_IT[] PROGMEM = "Il PLA e stato caricato?";
const char MSG_PLA_FILAMENT_LOADED_ES[] PROGMEM = "Esta el filamento PLA cargado?";
const char MSG_PLA_FILAMENT_LOADED_PL[] PROGMEM = "Fialment PLA jest zaladowany?";
const char MSG_PLA_FILAMENT_LOADED_DE[] PROGMEM = "Ist PLA Filament geladen?";
const char * const MSG_PLA_FILAMENT_LOADED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PLA_FILAMENT_LOADED_EN,
	MSG_PLA_FILAMENT_LOADED_CZ,
	MSG_PLA_FILAMENT_LOADED_IT,
	MSG_PLA_FILAMENT_LOADED_ES,
	MSG_PLA_FILAMENT_LOADED_PL,
	MSG_PLA_FILAMENT_LOADED_DE
};

const char MSG_PLEASE_LOAD_PLA_EN[] PROGMEM = "Please load PLA filament first.";
const char MSG_PLEASE_LOAD_PLA_CZ[] PROGMEM = "Nejdrive zavedte PLA filament prosim.";
const char MSG_PLEASE_LOAD_PLA_IT[] PROGMEM = "Per favore prima caricare filamento di PLA.";
const char MSG_PLEASE_LOAD_PLA_ES[] PROGMEM = "Carga el filamento PLA primero por favor.";
const char MSG_PLEASE_LOAD_PLA_PL[] PROGMEM = "Prosze, najpierw zaladuj filament PLA.";
const char MSG_PLEASE_LOAD_PLA_DE[] PROGMEM = "Bitte laden Sie zuerst PLA Filament.";
const char * const MSG_PLEASE_LOAD_PLA_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PLEASE_LOAD_PLA_EN,
	MSG_PLEASE_LOAD_PLA_CZ,
	MSG_PLEASE_LOAD_PLA_IT,
	MSG_PLEASE_LOAD_PLA_ES,
	MSG_PLEASE_LOAD_PLA_PL,
	MSG_PLEASE_LOAD_PLA_DE
};

const char MSG_PLEASE_WAIT_EN[] PROGMEM = "Please wait";
const char MSG_PLEASE_WAIT_CZ[] PROGMEM = "Prosim cekejte";
const char MSG_PLEASE_WAIT_IT[] PROGMEM = "Aspetta";
const char MSG_PLEASE_WAIT_ES[] PROGMEM = "Por Favor Esperar";
const char MSG_PLEASE_WAIT_PL[] PROGMEM = "Prosze czekac";
const char MSG_PLEASE_WAIT_DE[] PROGMEM = "Bitte warten";
const char * const MSG_PLEASE_WAIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PLEASE_WAIT_EN,
	MSG_PLEASE_WAIT_CZ,
	MSG_PLEASE_WAIT_IT,
	MSG_PLEASE_WAIT_ES,
	MSG_PLEASE_WAIT_PL,
	MSG_PLEASE_WAIT_DE
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
const char MSG_PREHEAT_IT[] PROGMEM = "Preriscalda";
const char MSG_PREHEAT_ES[] PROGMEM = "Precalentar";
const char MSG_PREHEAT_PL[] PROGMEM = "Grzanie";
const char MSG_PREHEAT_DE[] PROGMEM = "Vorwaermen";
const char * const MSG_PREHEAT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_EN,
	MSG_PREHEAT_CZ,
	MSG_PREHEAT_IT,
	MSG_PREHEAT_ES,
	MSG_PREHEAT_PL,
	MSG_PREHEAT_DE
};

const char MSG_PREHEAT_NOZZLE_EN[] PROGMEM = "Preheat the nozzle!";
const char MSG_PREHEAT_NOZZLE_CZ[] PROGMEM = "Predehrejte trysku!";
const char MSG_PREHEAT_NOZZLE_IT[] PROGMEM = "Preris. ugello!";
const char MSG_PREHEAT_NOZZLE_ES[] PROGMEM = "Precalentar extrusor";
const char MSG_PREHEAT_NOZZLE_PL[] PROGMEM = "Nagrzej dysze!";
const char MSG_PREHEAT_NOZZLE_DE[] PROGMEM = "Duese Vorwaermen";
const char * const MSG_PREHEAT_NOZZLE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREHEAT_NOZZLE_EN,
	MSG_PREHEAT_NOZZLE_CZ,
	MSG_PREHEAT_NOZZLE_IT,
	MSG_PREHEAT_NOZZLE_ES,
	MSG_PREHEAT_NOZZLE_PL,
	MSG_PREHEAT_NOZZLE_DE
};

const char MSG_PREPARE_FILAMENT_EN[] PROGMEM = "Prepare new filament";
const char MSG_PREPARE_FILAMENT_CZ[] PROGMEM = "Pripravte filament";
const char MSG_PREPARE_FILAMENT_IT[] PROGMEM = "Preparare filamento";
const char MSG_PREPARE_FILAMENT_ES[] PROGMEM = "Preparar filamento";
const char MSG_PREPARE_FILAMENT_PL[] PROGMEM = "Przygotuj filament";
const char MSG_PREPARE_FILAMENT_DE[] PROGMEM = "Filam. bereithalten";
const char * const MSG_PREPARE_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PREPARE_FILAMENT_EN,
	MSG_PREPARE_FILAMENT_CZ,
	MSG_PREPARE_FILAMENT_IT,
	MSG_PREPARE_FILAMENT_ES,
	MSG_PREPARE_FILAMENT_PL,
	MSG_PREPARE_FILAMENT_DE
};

const char MSG_PRESS_EN[] PROGMEM = "and press the knob";
const char MSG_PRESS_CZ[] PROGMEM = "a stisknete tlacitko";
const char MSG_PRESS_IT[] PROGMEM = "e cliccare manopola";
const char MSG_PRESS_ES[] PROGMEM = "Haz clic";
const char MSG_PRESS_PL[] PROGMEM = "Nacisnij przycisk";
const char MSG_PRESS_DE[] PROGMEM = "und Knopf druecken";
const char * const MSG_PRESS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRESS_EN,
	MSG_PRESS_CZ,
	MSG_PRESS_IT,
	MSG_PRESS_ES,
	MSG_PRESS_PL,
	MSG_PRESS_DE
};

const char MSG_PRINTER_DISCONNECTED_EN[] PROGMEM = "Printer disconnected";
const char * const MSG_PRINTER_DISCONNECTED_LANG_TABLE[1] PROGMEM = {
	MSG_PRINTER_DISCONNECTED_EN
};

const char MSG_PRINT_ABORTED_EN[] PROGMEM = "Print aborted";
const char MSG_PRINT_ABORTED_CZ[] PROGMEM = "Tisk prerusen";
const char MSG_PRINT_ABORTED_IT[] PROGMEM = "Stampa abortita";
const char MSG_PRINT_ABORTED_ES[] PROGMEM = "Impresion cancelada";
const char MSG_PRINT_ABORTED_PL[] PROGMEM = "Druk przerwany";
const char MSG_PRINT_ABORTED_DE[] PROGMEM = "Druck abgebrochen";
const char * const MSG_PRINT_ABORTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRINT_ABORTED_EN,
	MSG_PRINT_ABORTED_CZ,
	MSG_PRINT_ABORTED_IT,
	MSG_PRINT_ABORTED_ES,
	MSG_PRINT_ABORTED_PL,
	MSG_PRINT_ABORTED_DE
};

const char MSG_PRINT_PAUSED_EN[] PROGMEM = "Print paused";
const char MSG_PRINT_PAUSED_CZ[] PROGMEM = "Tisk pozastaven";
const char MSG_PRINT_PAUSED_IT[] PROGMEM = "Stampa in pausa";
const char MSG_PRINT_PAUSED_ES[] PROGMEM = "Impresion en pausa";
const char MSG_PRINT_PAUSED_PL[] PROGMEM = "Druk zatrzymany";
const char MSG_PRINT_PAUSED_DE[] PROGMEM = "Druck pausiert";
const char * const MSG_PRINT_PAUSED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRINT_PAUSED_EN,
	MSG_PRINT_PAUSED_CZ,
	MSG_PRINT_PAUSED_IT,
	MSG_PRINT_PAUSED_ES,
	MSG_PRINT_PAUSED_PL,
	MSG_PRINT_PAUSED_DE
};

const char MSG_PRUSA3D_EN[] PROGMEM = "prusa3d.com";
const char MSG_PRUSA3D_CZ[] PROGMEM = "prusa3d.cz";
const char MSG_PRUSA3D_PL[] PROGMEM = "prusa3d.cz";
const char * const MSG_PRUSA3D_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRUSA3D_EN,
	MSG_PRUSA3D_CZ,
	MSG_PRUSA3D_EN,
	MSG_PRUSA3D_EN,
	MSG_PRUSA3D_PL,
	MSG_PRUSA3D_EN
};

const char MSG_PRUSA3D_FORUM_EN[] PROGMEM = "forum.prusa3d.com";
const char MSG_PRUSA3D_FORUM_CZ[] PROGMEM = "forum.prusa3d.cz";
const char MSG_PRUSA3D_FORUM_PL[] PROGMEM = "forum.prusa3d.cz";
const char * const MSG_PRUSA3D_FORUM_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRUSA3D_FORUM_EN,
	MSG_PRUSA3D_FORUM_CZ,
	MSG_PRUSA3D_FORUM_EN,
	MSG_PRUSA3D_FORUM_EN,
	MSG_PRUSA3D_FORUM_PL,
	MSG_PRUSA3D_FORUM_EN
};

const char MSG_PRUSA3D_HOWTO_EN[] PROGMEM = "howto.prusa3d.com";
const char MSG_PRUSA3D_HOWTO_CZ[] PROGMEM = "howto.prusa3d.cz";
const char MSG_PRUSA3D_HOWTO_PL[] PROGMEM = "howto.prusa3d.cz";
const char * const MSG_PRUSA3D_HOWTO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_PRUSA3D_HOWTO_EN,
	MSG_PRUSA3D_HOWTO_CZ,
	MSG_PRUSA3D_HOWTO_EN,
	MSG_PRUSA3D_HOWTO_EN,
	MSG_PRUSA3D_HOWTO_PL,
	MSG_PRUSA3D_HOWTO_EN
};

const char MSG_REBOOT_EN[] PROGMEM = "Reboot the printer";
const char MSG_REBOOT_CZ[] PROGMEM = "Restartujte tiskarnu";
const char MSG_REBOOT_IT[] PROGMEM = "Riavvia stampante";
const char MSG_REBOOT_ES[] PROGMEM = "Reiniciar impresora";
const char MSG_REBOOT_PL[] PROGMEM = "Restart drukarki";
const char MSG_REBOOT_DE[] PROGMEM = "Zum Uebernehmen ";
const char * const MSG_REBOOT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_REBOOT_EN,
	MSG_REBOOT_CZ,
	MSG_REBOOT_IT,
	MSG_REBOOT_ES,
	MSG_REBOOT_PL,
	MSG_REBOOT_DE
};

const char MSG_RECTRACT_EN[] PROGMEM = "Rectract";
const char * const MSG_RECTRACT_LANG_TABLE[1] PROGMEM = {
	MSG_RECTRACT_EN
};

const char MSG_REFRESH_EN[] PROGMEM = "\xF8" "Refresh";
const char * const MSG_REFRESH_LANG_TABLE[1] PROGMEM = {
	MSG_REFRESH_EN
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
const char MSG_RESUME_PRINT_IT[] PROGMEM = "Riprendi stampa";
const char MSG_RESUME_PRINT_ES[] PROGMEM = "Reanudar impres.";
const char MSG_RESUME_PRINT_PL[] PROGMEM = "Kontynuowac";
const char MSG_RESUME_PRINT_DE[] PROGMEM = "Fortsetzen";
const char * const MSG_RESUME_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESUME_PRINT_EN,
	MSG_RESUME_PRINT_CZ,
	MSG_RESUME_PRINT_IT,
	MSG_RESUME_PRINT_ES,
	MSG_RESUME_PRINT_PL,
	MSG_RESUME_PRINT_DE
};

const char MSG_RESUMING_EN[] PROGMEM = "Resuming print";
const char MSG_RESUMING_CZ[] PROGMEM = "Obnoveni tisku";
const char MSG_RESUMING_IT[] PROGMEM = "Riprendi stampa";
const char MSG_RESUMING_ES[] PROGMEM = "Resumiendo impresion";
const char MSG_RESUMING_PL[] PROGMEM = "Wznowienie druku";
const char MSG_RESUMING_DE[] PROGMEM = "Druck fortgesetzt";
const char * const MSG_RESUMING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESUMING_EN,
	MSG_RESUMING_CZ,
	MSG_RESUMING_IT,
	MSG_RESUMING_ES,
	MSG_RESUMING_PL,
	MSG_RESUMING_DE
};

const char MSG_RESUMING_PRINT_EN[] PROGMEM = "Resuming print";
const char MSG_RESUMING_PRINT_CZ[] PROGMEM = "Obnovovani tisku";
const char MSG_RESUMING_PRINT_IT[] PROGMEM = "Stampa in ripresa";
const char MSG_RESUMING_PRINT_ES[] PROGMEM = "Reanudar impresion";
const char MSG_RESUMING_PRINT_PL[] PROGMEM = "Wznawianie druku";
const char MSG_RESUMING_PRINT_DE[] PROGMEM = "Druck weitergehen";
const char * const MSG_RESUMING_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RESUMING_PRINT_EN,
	MSG_RESUMING_PRINT_CZ,
	MSG_RESUMING_PRINT_IT,
	MSG_RESUMING_PRINT_ES,
	MSG_RESUMING_PRINT_PL,
	MSG_RESUMING_PRINT_DE
};

const char MSG_RIGHT_EN[] PROGMEM = "Right:";
const char MSG_RIGHT_CZ[] PROGMEM = "Pravy:";
const char MSG_RIGHT_IT[] PROGMEM = "Destra:";
const char MSG_RIGHT_ES[] PROGMEM = "Derecha:";
const char MSG_RIGHT_PL[] PROGMEM = "Prawy:";
const char MSG_RIGHT_DE[] PROGMEM = "Rechts:";
const char * const MSG_RIGHT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_RIGHT_EN,
	MSG_RIGHT_CZ,
	MSG_RIGHT_IT,
	MSG_RIGHT_ES,
	MSG_RIGHT_PL,
	MSG_RIGHT_DE
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
const char MSG_SD_INSERTED_IT[] PROGMEM = "SD inserita";
const char MSG_SD_INSERTED_ES[] PROGMEM = "Tarjeta insertada";
const char MSG_SD_INSERTED_PL[] PROGMEM = "Karta wlozona";
const char MSG_SD_INSERTED_DE[] PROGMEM = "SD eingesetzt";
const char * const MSG_SD_INSERTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_INSERTED_EN,
	MSG_SD_INSERTED_CZ,
	MSG_SD_INSERTED_IT,
	MSG_SD_INSERTED_ES,
	MSG_SD_INSERTED_PL,
	MSG_SD_INSERTED_DE
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
const char MSG_SD_REMOVED_IT[] PROGMEM = "SD rimossa";
const char MSG_SD_REMOVED_ES[] PROGMEM = "Tarjeta retirada";
const char MSG_SD_REMOVED_PL[] PROGMEM = "Karta wyjeta";
const char MSG_SD_REMOVED_DE[] PROGMEM = "SD entfernt ";
const char * const MSG_SD_REMOVED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SD_REMOVED_EN,
	MSG_SD_REMOVED_CZ,
	MSG_SD_REMOVED_IT,
	MSG_SD_REMOVED_ES,
	MSG_SD_REMOVED_PL,
	MSG_SD_REMOVED_DE
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

const char MSG_SELFTEST_EN[] PROGMEM = "Selftest         ";
const char MSG_SELFTEST_IT[] PROGMEM = "Autotest";
const char MSG_SELFTEST_ES[] PROGMEM = "Selftest";
const char MSG_SELFTEST_DE[] PROGMEM = "Selbsttest       ";
const char * const MSG_SELFTEST_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_EN,
	MSG_SELFTEST_EN,
	MSG_SELFTEST_IT,
	MSG_SELFTEST_ES,
	MSG_SELFTEST_EN,
	MSG_SELFTEST_DE
};

const char MSG_SELFTEST_BEDHEATER_EN[] PROGMEM = "Bed / Heater";
const char MSG_SELFTEST_BEDHEATER_IT[] PROGMEM = "Letto/Riscald.";
const char MSG_SELFTEST_BEDHEATER_ES[] PROGMEM = "Heatbed";
const char MSG_SELFTEST_BEDHEATER_DE[] PROGMEM = "Bett / Heater";
const char * const MSG_SELFTEST_BEDHEATER_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_BEDHEATER_EN,
	MSG_SELFTEST_BEDHEATER_EN,
	MSG_SELFTEST_BEDHEATER_IT,
	MSG_SELFTEST_BEDHEATER_ES,
	MSG_SELFTEST_BEDHEATER_EN,
	MSG_SELFTEST_BEDHEATER_DE
};

const char MSG_SELFTEST_CHECK_ALLCORRECT_EN[] PROGMEM = "All correct      ";
const char MSG_SELFTEST_CHECK_ALLCORRECT_CZ[] PROGMEM = "Vse OK           ";
const char MSG_SELFTEST_CHECK_ALLCORRECT_IT[] PROGMEM = "Nessun errore";
const char MSG_SELFTEST_CHECK_ALLCORRECT_ES[] PROGMEM = "Todo bien";
const char MSG_SELFTEST_CHECK_ALLCORRECT_PL[] PROGMEM = "Wszystko OK      ";
const char MSG_SELFTEST_CHECK_ALLCORRECT_DE[] PROGMEM = "Alles richtig    ";
const char * const MSG_SELFTEST_CHECK_ALLCORRECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_ALLCORRECT_EN,
	MSG_SELFTEST_CHECK_ALLCORRECT_CZ,
	MSG_SELFTEST_CHECK_ALLCORRECT_IT,
	MSG_SELFTEST_CHECK_ALLCORRECT_ES,
	MSG_SELFTEST_CHECK_ALLCORRECT_PL,
	MSG_SELFTEST_CHECK_ALLCORRECT_DE
};

const char MSG_SELFTEST_CHECK_BED_EN[] PROGMEM = "Checking bed     ";
const char MSG_SELFTEST_CHECK_BED_CZ[] PROGMEM = "Kontrola bed     ";
const char MSG_SELFTEST_CHECK_BED_IT[] PROGMEM = "Verifica letto";
const char MSG_SELFTEST_CHECK_BED_ES[] PROGMEM = "Control heatbed";
const char MSG_SELFTEST_CHECK_BED_PL[] PROGMEM = "Kontrola bed     ";
const char MSG_SELFTEST_CHECK_BED_DE[] PROGMEM = "Pruefe Bett        ";
const char * const MSG_SELFTEST_CHECK_BED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_BED_EN,
	MSG_SELFTEST_CHECK_BED_CZ,
	MSG_SELFTEST_CHECK_BED_IT,
	MSG_SELFTEST_CHECK_BED_ES,
	MSG_SELFTEST_CHECK_BED_PL,
	MSG_SELFTEST_CHECK_BED_DE
};

const char MSG_SELFTEST_CHECK_ENDSTOPS_EN[] PROGMEM = "Checking endstops";
const char MSG_SELFTEST_CHECK_ENDSTOPS_CZ[] PROGMEM = "Kontrola endstops";
const char MSG_SELFTEST_CHECK_ENDSTOPS_IT[] PROGMEM = "Verifica finecorsa";
const char MSG_SELFTEST_CHECK_ENDSTOPS_ES[] PROGMEM = "Control topes";
const char MSG_SELFTEST_CHECK_ENDSTOPS_PL[] PROGMEM = "Kontrola endstops";
const char MSG_SELFTEST_CHECK_ENDSTOPS_DE[] PROGMEM = "Pruefe Endschalter  ";
const char * const MSG_SELFTEST_CHECK_ENDSTOPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_ENDSTOPS_EN,
	MSG_SELFTEST_CHECK_ENDSTOPS_CZ,
	MSG_SELFTEST_CHECK_ENDSTOPS_IT,
	MSG_SELFTEST_CHECK_ENDSTOPS_ES,
	MSG_SELFTEST_CHECK_ENDSTOPS_PL,
	MSG_SELFTEST_CHECK_ENDSTOPS_DE
};

const char MSG_SELFTEST_CHECK_HOTEND_EN[] PROGMEM = "Checking hotend  ";
const char MSG_SELFTEST_CHECK_HOTEND_CZ[] PROGMEM = "Kontrola hotend  ";
const char MSG_SELFTEST_CHECK_HOTEND_IT[] PROGMEM = "Verifica ugello";
const char MSG_SELFTEST_CHECK_HOTEND_ES[] PROGMEM = "Control hotend ";
const char MSG_SELFTEST_CHECK_HOTEND_PL[] PROGMEM = "Kontrola hotend  ";
const char MSG_SELFTEST_CHECK_HOTEND_DE[] PROGMEM = "Pruefe Duese";
const char * const MSG_SELFTEST_CHECK_HOTEND_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_HOTEND_EN,
	MSG_SELFTEST_CHECK_HOTEND_CZ,
	MSG_SELFTEST_CHECK_HOTEND_IT,
	MSG_SELFTEST_CHECK_HOTEND_ES,
	MSG_SELFTEST_CHECK_HOTEND_PL,
	MSG_SELFTEST_CHECK_HOTEND_DE
};

const char MSG_SELFTEST_CHECK_X_EN[] PROGMEM = "Checking X axis  ";
const char MSG_SELFTEST_CHECK_X_CZ[] PROGMEM = "Kontrola X axis  ";
const char MSG_SELFTEST_CHECK_X_IT[] PROGMEM = "Verifica asse X";
const char MSG_SELFTEST_CHECK_X_ES[] PROGMEM = "Control tope X";
const char MSG_SELFTEST_CHECK_X_PL[] PROGMEM = "Kontrola X axis  ";
const char MSG_SELFTEST_CHECK_X_DE[] PROGMEM = "Pruefe X Achse    ";
const char * const MSG_SELFTEST_CHECK_X_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_X_EN,
	MSG_SELFTEST_CHECK_X_CZ,
	MSG_SELFTEST_CHECK_X_IT,
	MSG_SELFTEST_CHECK_X_ES,
	MSG_SELFTEST_CHECK_X_PL,
	MSG_SELFTEST_CHECK_X_DE
};

const char MSG_SELFTEST_CHECK_Y_EN[] PROGMEM = "Checking Y axis  ";
const char MSG_SELFTEST_CHECK_Y_CZ[] PROGMEM = "Kontrola Y axis  ";
const char MSG_SELFTEST_CHECK_Y_IT[] PROGMEM = "Verifica asse Y";
const char MSG_SELFTEST_CHECK_Y_ES[] PROGMEM = "Control tope Y";
const char MSG_SELFTEST_CHECK_Y_PL[] PROGMEM = "Kontrola Y axis  ";
const char MSG_SELFTEST_CHECK_Y_DE[] PROGMEM = "Pruefe Y Achse    ";
const char * const MSG_SELFTEST_CHECK_Y_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_Y_EN,
	MSG_SELFTEST_CHECK_Y_CZ,
	MSG_SELFTEST_CHECK_Y_IT,
	MSG_SELFTEST_CHECK_Y_ES,
	MSG_SELFTEST_CHECK_Y_PL,
	MSG_SELFTEST_CHECK_Y_DE
};

const char MSG_SELFTEST_CHECK_Z_EN[] PROGMEM = "Checking Z axis  ";
const char MSG_SELFTEST_CHECK_Z_CZ[] PROGMEM = "Kontrola Z axis  ";
const char MSG_SELFTEST_CHECK_Z_IT[] PROGMEM = "Verifica asse Z";
const char MSG_SELFTEST_CHECK_Z_ES[] PROGMEM = "Control tope Z";
const char MSG_SELFTEST_CHECK_Z_PL[] PROGMEM = "Kontrola Z axis  ";
const char MSG_SELFTEST_CHECK_Z_DE[] PROGMEM = "Pruefe Z Achse    ";
const char * const MSG_SELFTEST_CHECK_Z_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_CHECK_Z_EN,
	MSG_SELFTEST_CHECK_Z_CZ,
	MSG_SELFTEST_CHECK_Z_IT,
	MSG_SELFTEST_CHECK_Z_ES,
	MSG_SELFTEST_CHECK_Z_PL,
	MSG_SELFTEST_CHECK_Z_DE
};

const char MSG_SELFTEST_COOLING_FAN_EN[] PROGMEM = "Front print fan?";
const char MSG_SELFTEST_COOLING_FAN_CZ[] PROGMEM = "Predni tiskovy vent?";
const char MSG_SELFTEST_COOLING_FAN_IT[] PROGMEM = "Vent di stampa ant.?";
const char MSG_SELFTEST_COOLING_FAN_ES[] PROGMEM = "Vent. frontal?";
const char MSG_SELFTEST_COOLING_FAN_PL[] PROGMEM = "Przedni went. druku?";
const char MSG_SELFTEST_COOLING_FAN_DE[] PROGMEM = "Vorderer Luefter?";
const char * const MSG_SELFTEST_COOLING_FAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_COOLING_FAN_EN,
	MSG_SELFTEST_COOLING_FAN_CZ,
	MSG_SELFTEST_COOLING_FAN_IT,
	MSG_SELFTEST_COOLING_FAN_ES,
	MSG_SELFTEST_COOLING_FAN_PL,
	MSG_SELFTEST_COOLING_FAN_DE
};

const char MSG_SELFTEST_ENDSTOP_EN[] PROGMEM = "Endstop";
const char MSG_SELFTEST_ENDSTOP_IT[] PROGMEM = "Finecorsa";
const char MSG_SELFTEST_ENDSTOP_ES[] PROGMEM = "Tope final";
const char * const MSG_SELFTEST_ENDSTOP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_ENDSTOP_EN,
	MSG_SELFTEST_ENDSTOP_EN,
	MSG_SELFTEST_ENDSTOP_IT,
	MSG_SELFTEST_ENDSTOP_ES,
	MSG_SELFTEST_ENDSTOP_EN,
	MSG_SELFTEST_ENDSTOP_EN
};

const char MSG_SELFTEST_ENDSTOPS_EN[] PROGMEM = "Endstops";
const char MSG_SELFTEST_ENDSTOPS_IT[] PROGMEM = "Finecorsa (2)";
const char MSG_SELFTEST_ENDSTOPS_ES[] PROGMEM = "Topes finales";
const char MSG_SELFTEST_ENDSTOPS_DE[] PROGMEM = "Endschalter";
const char * const MSG_SELFTEST_ENDSTOPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_ENDSTOPS_EN,
	MSG_SELFTEST_ENDSTOPS_EN,
	MSG_SELFTEST_ENDSTOPS_IT,
	MSG_SELFTEST_ENDSTOPS_ES,
	MSG_SELFTEST_ENDSTOPS_EN,
	MSG_SELFTEST_ENDSTOPS_DE
};

const char MSG_SELFTEST_ENDSTOP_NOTHIT_EN[] PROGMEM = "Endstop not hit";
const char MSG_SELFTEST_ENDSTOP_NOTHIT_IT[] PROGMEM = "Finec. fuori por.";
const char MSG_SELFTEST_ENDSTOP_NOTHIT_ES[] PROGMEM = "Tope no alcanzado";
const char MSG_SELFTEST_ENDSTOP_NOTHIT_DE[] PROGMEM = "Ende nicht getrof.";
const char * const MSG_SELFTEST_ENDSTOP_NOTHIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_ENDSTOP_NOTHIT_EN,
	MSG_SELFTEST_ENDSTOP_NOTHIT_EN,
	MSG_SELFTEST_ENDSTOP_NOTHIT_IT,
	MSG_SELFTEST_ENDSTOP_NOTHIT_ES,
	MSG_SELFTEST_ENDSTOP_NOTHIT_EN,
	MSG_SELFTEST_ENDSTOP_NOTHIT_DE
};

const char MSG_SELFTEST_ERROR_EN[] PROGMEM = "Selftest error !";
const char MSG_SELFTEST_ERROR_IT[] PROGMEM = "Autotest negativo";
const char MSG_SELFTEST_ERROR_ES[] PROGMEM = "Autotest error!";
const char MSG_SELFTEST_ERROR_DE[] PROGMEM = "Selbtest Fehler!";
const char * const MSG_SELFTEST_ERROR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_ERROR_EN,
	MSG_SELFTEST_ERROR_EN,
	MSG_SELFTEST_ERROR_IT,
	MSG_SELFTEST_ERROR_ES,
	MSG_SELFTEST_ERROR_EN,
	MSG_SELFTEST_ERROR_DE
};

const char MSG_SELFTEST_EXTRUDER_FAN_EN[] PROGMEM = "Left hotend fan?";
const char MSG_SELFTEST_EXTRUDER_FAN_CZ[] PROGMEM = "Levy vent na trysce?";
const char MSG_SELFTEST_EXTRUDER_FAN_IT[] PROGMEM = "Vent SX sull'ugello?";
const char MSG_SELFTEST_EXTRUDER_FAN_ES[] PROGMEM = "Vent. izquierdo?";
const char MSG_SELFTEST_EXTRUDER_FAN_PL[] PROGMEM = "Lewy went na dysze?";
const char MSG_SELFTEST_EXTRUDER_FAN_DE[] PROGMEM = "Linker Luefter?";
const char * const MSG_SELFTEST_EXTRUDER_FAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_EXTRUDER_FAN_EN,
	MSG_SELFTEST_EXTRUDER_FAN_CZ,
	MSG_SELFTEST_EXTRUDER_FAN_IT,
	MSG_SELFTEST_EXTRUDER_FAN_ES,
	MSG_SELFTEST_EXTRUDER_FAN_PL,
	MSG_SELFTEST_EXTRUDER_FAN_DE
};

const char MSG_SELFTEST_FAILED_EN[] PROGMEM = "Selftest failed  ";
const char MSG_SELFTEST_FAILED_CZ[] PROGMEM = "Selftest selhal  ";
const char MSG_SELFTEST_FAILED_IT[] PROGMEM = "Autotest fallito";
const char MSG_SELFTEST_FAILED_ES[] PROGMEM = "Fallo Selftest";
const char MSG_SELFTEST_FAILED_PL[] PROGMEM = "Selftest nieudany";
const char MSG_SELFTEST_FAILED_DE[] PROGMEM = "Selbsttest misslung.";
const char * const MSG_SELFTEST_FAILED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FAILED_EN,
	MSG_SELFTEST_FAILED_CZ,
	MSG_SELFTEST_FAILED_IT,
	MSG_SELFTEST_FAILED_ES,
	MSG_SELFTEST_FAILED_PL,
	MSG_SELFTEST_FAILED_DE
};

const char MSG_SELFTEST_FAN_EN[] PROGMEM = "Fan test";
const char MSG_SELFTEST_FAN_CZ[] PROGMEM = "Test ventilatoru";
const char MSG_SELFTEST_FAN_IT[] PROGMEM = "Prova del ventilator";
const char MSG_SELFTEST_FAN_ES[] PROGMEM = "Test ventiladores";
const char MSG_SELFTEST_FAN_PL[] PROGMEM = "Test wentylatora";
const char MSG_SELFTEST_FAN_DE[] PROGMEM = "Lueftertest";
const char * const MSG_SELFTEST_FAN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FAN_EN,
	MSG_SELFTEST_FAN_CZ,
	MSG_SELFTEST_FAN_IT,
	MSG_SELFTEST_FAN_ES,
	MSG_SELFTEST_FAN_PL,
	MSG_SELFTEST_FAN_DE
};

const char MSG_SELFTEST_FAN_NO_EN[] PROGMEM = "Not spinning";
const char MSG_SELFTEST_FAN_NO_CZ[] PROGMEM = "Netoci se";
const char MSG_SELFTEST_FAN_NO_IT[] PROGMEM = "Non si gira";
const char MSG_SELFTEST_FAN_NO_ES[] PROGMEM = "Ventilador no gira";
const char MSG_SELFTEST_FAN_NO_PL[] PROGMEM = "Nie kreci sie";
const char MSG_SELFTEST_FAN_NO_DE[] PROGMEM = "Dreht nicht";
const char * const MSG_SELFTEST_FAN_NO_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FAN_NO_EN,
	MSG_SELFTEST_FAN_NO_CZ,
	MSG_SELFTEST_FAN_NO_IT,
	MSG_SELFTEST_FAN_NO_ES,
	MSG_SELFTEST_FAN_NO_PL,
	MSG_SELFTEST_FAN_NO_DE
};

const char MSG_SELFTEST_FAN_YES_EN[] PROGMEM = "Spinning";
const char MSG_SELFTEST_FAN_YES_CZ[] PROGMEM = "Toci se";
const char MSG_SELFTEST_FAN_YES_IT[] PROGMEM = "Gira";
const char MSG_SELFTEST_FAN_YES_ES[] PROGMEM = "Ventilador gira";
const char MSG_SELFTEST_FAN_YES_PL[] PROGMEM = "Kreci sie";
const char MSG_SELFTEST_FAN_YES_DE[] PROGMEM = "Dreht";
const char * const MSG_SELFTEST_FAN_YES_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_FAN_YES_EN,
	MSG_SELFTEST_FAN_YES_CZ,
	MSG_SELFTEST_FAN_YES_IT,
	MSG_SELFTEST_FAN_YES_ES,
	MSG_SELFTEST_FAN_YES_PL,
	MSG_SELFTEST_FAN_YES_DE
};

const char MSG_SELFTEST_HEATERTHERMISTOR_EN[] PROGMEM = "Heater/Thermistor";
const char MSG_SELFTEST_HEATERTHERMISTOR_IT[] PROGMEM = "Riscald./Termist.";
const char MSG_SELFTEST_HEATERTHERMISTOR_ES[] PROGMEM = "Hotend";
const char * const MSG_SELFTEST_HEATERTHERMISTOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_HEATERTHERMISTOR_EN,
	MSG_SELFTEST_HEATERTHERMISTOR_EN,
	MSG_SELFTEST_HEATERTHERMISTOR_IT,
	MSG_SELFTEST_HEATERTHERMISTOR_ES,
	MSG_SELFTEST_HEATERTHERMISTOR_EN,
	MSG_SELFTEST_HEATERTHERMISTOR_EN
};

const char MSG_SELFTEST_MOTOR_EN[] PROGMEM = "Motor";
const char MSG_SELFTEST_MOTOR_IT[] PROGMEM = "Motore";
const char MSG_SELFTEST_MOTOR_PL[] PROGMEM = "Silnik";
const char * const MSG_SELFTEST_MOTOR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_MOTOR_EN,
	MSG_SELFTEST_MOTOR_EN,
	MSG_SELFTEST_MOTOR_IT,
	MSG_SELFTEST_MOTOR_EN,
	MSG_SELFTEST_MOTOR_PL,
	MSG_SELFTEST_MOTOR_EN
};

const char MSG_SELFTEST_NOTCONNECTED_EN[] PROGMEM = "Not connected";
const char MSG_SELFTEST_NOTCONNECTED_CZ[] PROGMEM = "Nezapojeno    ";
const char MSG_SELFTEST_NOTCONNECTED_IT[] PROGMEM = "Non connesso";
const char MSG_SELFTEST_NOTCONNECTED_ES[] PROGMEM = "No hay conexion  ";
const char MSG_SELFTEST_NOTCONNECTED_PL[] PROGMEM = "Nie podlaczono   ";
const char MSG_SELFTEST_NOTCONNECTED_DE[] PROGMEM = "Nicht angeschlossen";
const char * const MSG_SELFTEST_NOTCONNECTED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_NOTCONNECTED_EN,
	MSG_SELFTEST_NOTCONNECTED_CZ,
	MSG_SELFTEST_NOTCONNECTED_IT,
	MSG_SELFTEST_NOTCONNECTED_ES,
	MSG_SELFTEST_NOTCONNECTED_PL,
	MSG_SELFTEST_NOTCONNECTED_DE
};

const char MSG_SELFTEST_OK_EN[] PROGMEM = "Self test OK";
const char MSG_SELFTEST_OK_IT[] PROGMEM = "Autotest OK";
const char MSG_SELFTEST_OK_DE[] PROGMEM = "Selbsttest OK";
const char * const MSG_SELFTEST_OK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_OK_EN,
	MSG_SELFTEST_OK_EN,
	MSG_SELFTEST_OK_IT,
	MSG_SELFTEST_OK_EN,
	MSG_SELFTEST_OK_EN,
	MSG_SELFTEST_OK_DE
};

const char MSG_SELFTEST_PLEASECHECK_EN[] PROGMEM = "Please check :";
const char MSG_SELFTEST_PLEASECHECK_CZ[] PROGMEM = "Zkontrolujte :";
const char MSG_SELFTEST_PLEASECHECK_IT[] PROGMEM = "Verificare:";
const char MSG_SELFTEST_PLEASECHECK_ES[] PROGMEM = "Controla :";
const char MSG_SELFTEST_PLEASECHECK_PL[] PROGMEM = "Skontroluj :";
const char MSG_SELFTEST_PLEASECHECK_DE[] PROGMEM = "Bitte pruefe:";
const char * const MSG_SELFTEST_PLEASECHECK_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_PLEASECHECK_EN,
	MSG_SELFTEST_PLEASECHECK_CZ,
	MSG_SELFTEST_PLEASECHECK_IT,
	MSG_SELFTEST_PLEASECHECK_ES,
	MSG_SELFTEST_PLEASECHECK_PL,
	MSG_SELFTEST_PLEASECHECK_DE
};

const char MSG_SELFTEST_START_EN[] PROGMEM = "Self test start  ";
const char MSG_SELFTEST_START_IT[] PROGMEM = "Avvia autotest";
const char MSG_SELFTEST_START_ES[] PROGMEM = "Inicio Selftest";
const char MSG_SELFTEST_START_DE[] PROGMEM = "Selbsttest start ";
const char * const MSG_SELFTEST_START_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_START_EN,
	MSG_SELFTEST_START_EN,
	MSG_SELFTEST_START_IT,
	MSG_SELFTEST_START_ES,
	MSG_SELFTEST_START_EN,
	MSG_SELFTEST_START_DE
};

const char MSG_SELFTEST_WIRINGERROR_EN[] PROGMEM = "Wiring error";
const char MSG_SELFTEST_WIRINGERROR_CZ[] PROGMEM = "Chyba zapojeni";
const char MSG_SELFTEST_WIRINGERROR_IT[] PROGMEM = "Errore cablaggio";
const char MSG_SELFTEST_WIRINGERROR_ES[] PROGMEM = "Error de conexion";
const char MSG_SELFTEST_WIRINGERROR_PL[] PROGMEM = "Blad polaczenia";
const char MSG_SELFTEST_WIRINGERROR_DE[] PROGMEM = "Verdrahtungfehler";
const char * const MSG_SELFTEST_WIRINGERROR_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SELFTEST_WIRINGERROR_EN,
	MSG_SELFTEST_WIRINGERROR_CZ,
	MSG_SELFTEST_WIRINGERROR_IT,
	MSG_SELFTEST_WIRINGERROR_ES,
	MSG_SELFTEST_WIRINGERROR_PL,
	MSG_SELFTEST_WIRINGERROR_DE
};

const char MSG_SERIAL_ERROR_MENU_STRUCTURE_EN[] PROGMEM = "Error in menu structure";
const char * const MSG_SERIAL_ERROR_MENU_STRUCTURE_LANG_TABLE[1] PROGMEM = {
	MSG_SERIAL_ERROR_MENU_STRUCTURE_EN
};

const char MSG_SETTINGS_EN[] PROGMEM = "Settings";
const char MSG_SETTINGS_CZ[] PROGMEM = "Nastaveni";
const char MSG_SETTINGS_IT[] PROGMEM = "Impostazioni";
const char MSG_SETTINGS_ES[] PROGMEM = "Configuracion";
const char MSG_SETTINGS_PL[] PROGMEM = "Ustawienia";
const char MSG_SETTINGS_DE[] PROGMEM = "Einstellungen";
const char * const MSG_SETTINGS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SETTINGS_EN,
	MSG_SETTINGS_CZ,
	MSG_SETTINGS_IT,
	MSG_SETTINGS_ES,
	MSG_SETTINGS_PL,
	MSG_SETTINGS_DE
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
const char MSG_SET_TEMPERATURE_IT[] PROGMEM = "Imposta temperatura";
const char MSG_SET_TEMPERATURE_ES[] PROGMEM = "Establecer temp.:";
const char MSG_SET_TEMPERATURE_PL[] PROGMEM = "Ustawic temperature";
const char MSG_SET_TEMPERATURE_DE[] PROGMEM = "Temp. einsetzen";
const char * const MSG_SET_TEMPERATURE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SET_TEMPERATURE_EN,
	MSG_SET_TEMPERATURE_CZ,
	MSG_SET_TEMPERATURE_IT,
	MSG_SET_TEMPERATURE_ES,
	MSG_SET_TEMPERATURE_PL,
	MSG_SET_TEMPERATURE_DE
};

const char MSG_SEVERE_SKEW_EN[] PROGMEM = "Severe skew:";
const char MSG_SEVERE_SKEW_CZ[] PROGMEM = "Tezke zkoseni:";
const char MSG_SEVERE_SKEW_IT[] PROGMEM = "Inc. rilevante:";
const char MSG_SEVERE_SKEW_ES[] PROGMEM = "Inclin. severa:";
const char MSG_SEVERE_SKEW_PL[] PROGMEM = "Ostre sciecie:";
const char MSG_SEVERE_SKEW_DE[] PROGMEM = "Schwerer Schr.:";
const char * const MSG_SEVERE_SKEW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SEVERE_SKEW_EN,
	MSG_SEVERE_SKEW_CZ,
	MSG_SEVERE_SKEW_IT,
	MSG_SEVERE_SKEW_ES,
	MSG_SEVERE_SKEW_PL,
	MSG_SEVERE_SKEW_DE
};

const char MSG_SHOW_END_STOPS_EN[] PROGMEM = "Show end stops";
const char MSG_SHOW_END_STOPS_CZ[] PROGMEM = "Stav konc. spin.";
const char MSG_SHOW_END_STOPS_IT[] PROGMEM = "Stato finecorsa";
const char MSG_SHOW_END_STOPS_ES[] PROGMEM = "Ensena tope final";
const char MSG_SHOW_END_STOPS_PL[] PROGMEM = "Pokaz krancowki";
const char MSG_SHOW_END_STOPS_DE[] PROGMEM = "Endschalter Stat.";
const char * const MSG_SHOW_END_STOPS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SHOW_END_STOPS_EN,
	MSG_SHOW_END_STOPS_CZ,
	MSG_SHOW_END_STOPS_IT,
	MSG_SHOW_END_STOPS_ES,
	MSG_SHOW_END_STOPS_PL,
	MSG_SHOW_END_STOPS_DE
};

const char MSG_SILENT_MODE_OFF_EN[] PROGMEM = "Mode [high power]";
const char MSG_SILENT_MODE_OFF_CZ[] PROGMEM = "Mod  [vys. vykon]";
const char MSG_SILENT_MODE_OFF_IT[] PROGMEM = "Mode      [forte]";
const char MSG_SILENT_MODE_OFF_ES[] PROGMEM = "Modo [rend.pleno]";
const char MSG_SILENT_MODE_OFF_PL[] PROGMEM = "Tryb[w wydajnosc]";
const char MSG_SILENT_MODE_OFF_DE[] PROGMEM = "Mode [Hohe Leist]";
const char * const MSG_SILENT_MODE_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SILENT_MODE_OFF_EN,
	MSG_SILENT_MODE_OFF_CZ,
	MSG_SILENT_MODE_OFF_IT,
	MSG_SILENT_MODE_OFF_ES,
	MSG_SILENT_MODE_OFF_PL,
	MSG_SILENT_MODE_OFF_DE
};

const char MSG_SILENT_MODE_ON_EN[] PROGMEM = "Mode     [silent]";
const char MSG_SILENT_MODE_ON_CZ[] PROGMEM = "Mod       [tichy]";
const char MSG_SILENT_MODE_ON_IT[] PROGMEM = "Modo [silenzioso]";
const char MSG_SILENT_MODE_ON_ES[] PROGMEM = "Modo   [silencio]";
const char MSG_SILENT_MODE_ON_PL[] PROGMEM = "Tryb      [cichy]";
const char MSG_SILENT_MODE_ON_DE[] PROGMEM = "Mode     [leise]";
const char * const MSG_SILENT_MODE_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SILENT_MODE_ON_EN,
	MSG_SILENT_MODE_ON_CZ,
	MSG_SILENT_MODE_ON_IT,
	MSG_SILENT_MODE_ON_ES,
	MSG_SILENT_MODE_ON_PL,
	MSG_SILENT_MODE_ON_DE
};

const char MSG_SLIGHT_SKEW_EN[] PROGMEM = "Slight skew:";
const char MSG_SLIGHT_SKEW_CZ[] PROGMEM = "Lehke zkoseni:";
const char MSG_SLIGHT_SKEW_IT[] PROGMEM = "Incl. leggera:";
const char MSG_SLIGHT_SKEW_ES[] PROGMEM = "Inclin. ligera:";
const char MSG_SLIGHT_SKEW_PL[] PROGMEM = "Lekkie sciecie:";
const char MSG_SLIGHT_SKEW_DE[] PROGMEM = "Leichter Schr.:";
const char * const MSG_SLIGHT_SKEW_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SLIGHT_SKEW_EN,
	MSG_SLIGHT_SKEW_CZ,
	MSG_SLIGHT_SKEW_IT,
	MSG_SLIGHT_SKEW_ES,
	MSG_SLIGHT_SKEW_PL,
	MSG_SLIGHT_SKEW_DE
};

const char MSG_SOFTWARE_RESET_EN[] PROGMEM = " Software Reset";
const char * const MSG_SOFTWARE_RESET_LANG_TABLE[1] PROGMEM = {
	MSG_SOFTWARE_RESET_EN
};

const char MSG_SORTING_EN[] PROGMEM = "Sorting files";
const char MSG_SORTING_CZ[] PROGMEM = "Trideni souboru";
const char MSG_SORTING_IT[] PROGMEM = "Ordine dei file";
const char MSG_SORTING_ES[] PROGMEM = "Ordenando archivos";
const char MSG_SORTING_PL[] PROGMEM = "Sortowanie plikow";
const char MSG_SORTING_DE[] PROGMEM = "Dateien sortieren";
const char * const MSG_SORTING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SORTING_EN,
	MSG_SORTING_CZ,
	MSG_SORTING_IT,
	MSG_SORTING_ES,
	MSG_SORTING_PL,
	MSG_SORTING_DE
};

const char MSG_SORT_ALPHA_EN[] PROGMEM = "Sort: [Alphabet]";
const char MSG_SORT_ALPHA_CZ[] PROGMEM = "Trideni [Abeceda]";
const char MSG_SORT_ALPHA_IT[] PROGMEM = "Ordine:[Alfabeto]";
const char MSG_SORT_ALPHA_ES[] PROGMEM = "Ordenar:[Alfabet]";
const char MSG_SORT_ALPHA_PL[] PROGMEM = "Sortuj: [Alfabet]";
const char MSG_SORT_ALPHA_DE[] PROGMEM = "Sortieren:[Alpha]";
const char * const MSG_SORT_ALPHA_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SORT_ALPHA_EN,
	MSG_SORT_ALPHA_CZ,
	MSG_SORT_ALPHA_IT,
	MSG_SORT_ALPHA_ES,
	MSG_SORT_ALPHA_PL,
	MSG_SORT_ALPHA_DE
};

const char MSG_SORT_NONE_EN[] PROGMEM = "Sort: [None]";
const char MSG_SORT_NONE_CZ[] PROGMEM = "Trideni [Zadne]";
const char MSG_SORT_NONE_IT[] PROGMEM = "Ordine: [Nessuno]";
const char MSG_SORT_NONE_ES[] PROGMEM = "Ordenar:[Ninguno]";
const char MSG_SORT_NONE_PL[] PROGMEM = "Sortuj: [Zaden]";
const char MSG_SORT_NONE_DE[] PROGMEM = "Sortieren:[Keine]";
const char * const MSG_SORT_NONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SORT_NONE_EN,
	MSG_SORT_NONE_CZ,
	MSG_SORT_NONE_IT,
	MSG_SORT_NONE_ES,
	MSG_SORT_NONE_PL,
	MSG_SORT_NONE_DE
};

const char MSG_SORT_TIME_EN[] PROGMEM = "Sort: [Time]";
const char MSG_SORT_TIME_CZ[] PROGMEM = "Trideni [Cas]";
const char MSG_SORT_TIME_IT[] PROGMEM = "Ordine: [Tempo]";
const char MSG_SORT_TIME_ES[] PROGMEM = "Ordenar: [Tiempo]";
const char MSG_SORT_TIME_PL[] PROGMEM = "Sortuj: [Czas]";
const char MSG_SORT_TIME_DE[] PROGMEM = "Sortieren: [Zeit]";
const char * const MSG_SORT_TIME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SORT_TIME_EN,
	MSG_SORT_TIME_CZ,
	MSG_SORT_TIME_IT,
	MSG_SORT_TIME_ES,
	MSG_SORT_TIME_PL,
	MSG_SORT_TIME_DE
};

const char MSG_SPEED_EN[] PROGMEM = "Speed";
const char MSG_SPEED_CZ[] PROGMEM = "Rychlost";
const char MSG_SPEED_IT[] PROGMEM = "Velocita";
const char MSG_SPEED_ES[] PROGMEM = "Velocidad";
const char MSG_SPEED_PL[] PROGMEM = "Predkosc";
const char MSG_SPEED_DE[] PROGMEM = "Geschwindigkeit";
const char * const MSG_SPEED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SPEED_EN,
	MSG_SPEED_CZ,
	MSG_SPEED_IT,
	MSG_SPEED_ES,
	MSG_SPEED_PL,
	MSG_SPEED_DE
};

const char MSG_STACK_ERROR_EN[] PROGMEM = "Error - static memory has been overwritten";
const char * const MSG_STACK_ERROR_LANG_TABLE[1] PROGMEM = {
	MSG_STACK_ERROR_EN
};

const char MSG_STATISTICS_EN[] PROGMEM = "Statistics  ";
const char MSG_STATISTICS_CZ[] PROGMEM = "Statistika  ";
const char MSG_STATISTICS_IT[] PROGMEM = "Statistiche";
const char MSG_STATISTICS_ES[] PROGMEM = "Estadisticas  ";
const char MSG_STATISTICS_PL[] PROGMEM = "Statystyka  ";
const char MSG_STATISTICS_DE[] PROGMEM = "Statistiken ";
const char * const MSG_STATISTICS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATISTICS_EN,
	MSG_STATISTICS_CZ,
	MSG_STATISTICS_IT,
	MSG_STATISTICS_ES,
	MSG_STATISTICS_PL,
	MSG_STATISTICS_DE
};

const char MSG_STATS_FILAMENTUSED_EN[] PROGMEM = "Filament used:  ";
const char MSG_STATS_FILAMENTUSED_CZ[] PROGMEM = "Filament :  ";
const char MSG_STATS_FILAMENTUSED_IT[] PROGMEM = "Filamento usato:";
const char MSG_STATS_FILAMENTUSED_ES[] PROGMEM = "Filamento usado:  ";
const char MSG_STATS_FILAMENTUSED_PL[] PROGMEM = "Filament :  ";
const char MSG_STATS_FILAMENTUSED_DE[] PROGMEM = "Filamentverbrauch:";
const char * const MSG_STATS_FILAMENTUSED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_FILAMENTUSED_EN,
	MSG_STATS_FILAMENTUSED_CZ,
	MSG_STATS_FILAMENTUSED_IT,
	MSG_STATS_FILAMENTUSED_ES,
	MSG_STATS_FILAMENTUSED_PL,
	MSG_STATS_FILAMENTUSED_DE
};

const char MSG_STATS_PRINTTIME_EN[] PROGMEM = "Print time:  ";
const char MSG_STATS_PRINTTIME_CZ[] PROGMEM = "Cas tisku :  ";
const char MSG_STATS_PRINTTIME_IT[] PROGMEM = "Tempo di stampa:";
const char MSG_STATS_PRINTTIME_ES[] PROGMEM = "Tiempo de imp.:";
const char MSG_STATS_PRINTTIME_PL[] PROGMEM = "Czas druku :  ";
const char MSG_STATS_PRINTTIME_DE[] PROGMEM = "Druckzeit:  ";
const char * const MSG_STATS_PRINTTIME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_PRINTTIME_EN,
	MSG_STATS_PRINTTIME_CZ,
	MSG_STATS_PRINTTIME_IT,
	MSG_STATS_PRINTTIME_ES,
	MSG_STATS_PRINTTIME_PL,
	MSG_STATS_PRINTTIME_DE
};

const char MSG_STATS_TOTALFILAMENT_EN[] PROGMEM = "Total filament :";
const char MSG_STATS_TOTALFILAMENT_CZ[] PROGMEM = "Filament celkem :";
const char MSG_STATS_TOTALFILAMENT_IT[] PROGMEM = "Filamento tot:";
const char MSG_STATS_TOTALFILAMENT_ES[] PROGMEM = "Filamento total:";
const char MSG_STATS_TOTALFILAMENT_PL[] PROGMEM = "Filament lacznie :";
const char MSG_STATS_TOTALFILAMENT_DE[] PROGMEM = "Gesamtfilament:";
const char * const MSG_STATS_TOTALFILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_TOTALFILAMENT_EN,
	MSG_STATS_TOTALFILAMENT_CZ,
	MSG_STATS_TOTALFILAMENT_IT,
	MSG_STATS_TOTALFILAMENT_ES,
	MSG_STATS_TOTALFILAMENT_PL,
	MSG_STATS_TOTALFILAMENT_DE
};

const char MSG_STATS_TOTALPRINTTIME_EN[] PROGMEM = "Total print time :";
const char MSG_STATS_TOTALPRINTTIME_CZ[] PROGMEM = "Celkovy cas :";
const char MSG_STATS_TOTALPRINTTIME_IT[] PROGMEM = "Tempo stampa tot:";
const char MSG_STATS_TOTALPRINTTIME_ES[] PROGMEM = "Tiempo total :";
const char MSG_STATS_TOTALPRINTTIME_PL[] PROGMEM = "Czas calkowity :";
const char MSG_STATS_TOTALPRINTTIME_DE[] PROGMEM = "Totale Druckzeit:";
const char * const MSG_STATS_TOTALPRINTTIME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STATS_TOTALPRINTTIME_EN,
	MSG_STATS_TOTALPRINTTIME_CZ,
	MSG_STATS_TOTALPRINTTIME_IT,
	MSG_STATS_TOTALPRINTTIME_ES,
	MSG_STATS_TOTALPRINTTIME_PL,
	MSG_STATS_TOTALPRINTTIME_DE
};

const char MSG_STEPPER_TOO_HIGH_EN[] PROGMEM = "Steprate too high: ";
const char * const MSG_STEPPER_TOO_HIGH_LANG_TABLE[1] PROGMEM = {
	MSG_STEPPER_TOO_HIGH_EN
};

const char MSG_STOPPED_EN[] PROGMEM = "STOPPED. ";
const char MSG_STOPPED_IT[] PROGMEM = "ARRESTATO.";
const char MSG_STOPPED_ES[] PROGMEM = "PARADA";
const char MSG_STOPPED_DE[] PROGMEM = "GESTOPPT. ";
const char * const MSG_STOPPED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STOPPED_EN,
	MSG_STOPPED_EN,
	MSG_STOPPED_IT,
	MSG_STOPPED_ES,
	MSG_STOPPED_EN,
	MSG_STOPPED_DE
};

const char MSG_STOP_PRINT_EN[] PROGMEM = "Stop print";
const char MSG_STOP_PRINT_CZ[] PROGMEM = "Zastavit tisk";
const char MSG_STOP_PRINT_IT[] PROGMEM = "Arresta stampa";
const char MSG_STOP_PRINT_ES[] PROGMEM = "Detener impresion";
const char MSG_STOP_PRINT_PL[] PROGMEM = "Zatrzymac druk";
const char MSG_STOP_PRINT_DE[] PROGMEM = "Druck abbrechen";
const char * const MSG_STOP_PRINT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_STOP_PRINT_EN,
	MSG_STOP_PRINT_CZ,
	MSG_STOP_PRINT_IT,
	MSG_STOP_PRINT_ES,
	MSG_STOP_PRINT_PL,
	MSG_STOP_PRINT_DE
};

const char MSG_STORE_EPROM_EN[] PROGMEM = "Store memory";
const char * const MSG_STORE_EPROM_LANG_TABLE[1] PROGMEM = {
	MSG_STORE_EPROM_EN
};

const char MSG_SUPPORT_EN[] PROGMEM = "Support";
const char MSG_SUPPORT_CZ[] PROGMEM = "Podpora";
const char MSG_SUPPORT_ES[] PROGMEM = "Soporte";
const char MSG_SUPPORT_PL[] PROGMEM = "Pomoc";
const char * const MSG_SUPPORT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SUPPORT_EN,
	MSG_SUPPORT_CZ,
	MSG_SUPPORT_EN,
	MSG_SUPPORT_ES,
	MSG_SUPPORT_PL,
	MSG_SUPPORT_EN
};

const char MSG_SWITCH_PS_OFF_EN[] PROGMEM = "Switch power off";
const char MSG_SWITCH_PS_OFF_CZ[] PROGMEM = "Zapnout zdroj";
const char MSG_SWITCH_PS_OFF_PL[] PROGMEM = "Zapnout zdroj";
const char MSG_SWITCH_PS_OFF_DE[] PROGMEM = "Netzteil AUS";
const char * const MSG_SWITCH_PS_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SWITCH_PS_OFF_EN,
	MSG_SWITCH_PS_OFF_CZ,
	MSG_SWITCH_PS_OFF_EN,
	MSG_SWITCH_PS_OFF_EN,
	MSG_SWITCH_PS_OFF_PL,
	MSG_SWITCH_PS_OFF_DE
};

const char MSG_SWITCH_PS_ON_EN[] PROGMEM = "Switch power on";
const char MSG_SWITCH_PS_ON_CZ[] PROGMEM = "Vypnout zdroj";
const char MSG_SWITCH_PS_ON_PL[] PROGMEM = "Vypnout zdroj";
const char MSG_SWITCH_PS_ON_DE[] PROGMEM = "Netzteil EIN";
const char * const MSG_SWITCH_PS_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_SWITCH_PS_ON_EN,
	MSG_SWITCH_PS_ON_CZ,
	MSG_SWITCH_PS_ON_EN,
	MSG_SWITCH_PS_ON_EN,
	MSG_SWITCH_PS_ON_PL,
	MSG_SWITCH_PS_ON_DE
};

const char MSG_TAKE_EFFECT_EN[] PROGMEM = " for take effect";
const char MSG_TAKE_EFFECT_CZ[] PROGMEM = " pro projeveni zmen";
const char MSG_TAKE_EFFECT_IT[] PROGMEM = " per attualizzare";
const char MSG_TAKE_EFFECT_ES[] PROGMEM = " para aplicar cambios";
const char MSG_TAKE_EFFECT_PL[] PROGMEM = " wprow. zmian";
const char MSG_TAKE_EFFECT_DE[] PROGMEM = "Drucker neu starten";
const char * const MSG_TAKE_EFFECT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TAKE_EFFECT_EN,
	MSG_TAKE_EFFECT_CZ,
	MSG_TAKE_EFFECT_IT,
	MSG_TAKE_EFFECT_ES,
	MSG_TAKE_EFFECT_PL,
	MSG_TAKE_EFFECT_DE
};

const char MSG_TEMPERATURE_EN[] PROGMEM = "Temperature";
const char MSG_TEMPERATURE_CZ[] PROGMEM = "Teplota";
const char MSG_TEMPERATURE_IT[] PROGMEM = "Temperatura";
const char MSG_TEMPERATURE_ES[] PROGMEM = "Temperatura";
const char MSG_TEMPERATURE_PL[] PROGMEM = "Temperatura";
const char MSG_TEMPERATURE_DE[] PROGMEM = "Temperatur";
const char * const MSG_TEMPERATURE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMPERATURE_EN,
	MSG_TEMPERATURE_CZ,
	MSG_TEMPERATURE_IT,
	MSG_TEMPERATURE_ES,
	MSG_TEMPERATURE_PL,
	MSG_TEMPERATURE_DE
};

const char MSG_TEMP_CALIBRATION_EN[] PROGMEM = "Temp. cal.          ";
const char MSG_TEMP_CALIBRATION_CZ[] PROGMEM = "Tepl. kal.          ";
const char MSG_TEMP_CALIBRATION_IT[] PROGMEM = "Cal. temp.          ";
const char MSG_TEMP_CALIBRATION_ES[] PROGMEM = "Cal. temp.          ";
const char MSG_TEMP_CALIBRATION_PL[] PROGMEM = "Ciepl. kal.         ";
const char MSG_TEMP_CALIBRATION_DE[] PROGMEM = "Temp Kalib.         ";
const char * const MSG_TEMP_CALIBRATION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMP_CALIBRATION_EN,
	MSG_TEMP_CALIBRATION_CZ,
	MSG_TEMP_CALIBRATION_IT,
	MSG_TEMP_CALIBRATION_ES,
	MSG_TEMP_CALIBRATION_PL,
	MSG_TEMP_CALIBRATION_DE
};

const char MSG_TEMP_CALIBRATION_DONE_EN[] PROGMEM = "Temperature calibration is finished. Click to continue.";
const char MSG_TEMP_CALIBRATION_DONE_CZ[] PROGMEM = "Teplotni kalibrace dokoncena. Pokracujte stiskem tlacitka.";
const char MSG_TEMP_CALIBRATION_DONE_IT[] PROGMEM = "Taratura temperatura terminata. Fare click per continuare.";
const char MSG_TEMP_CALIBRATION_DONE_ES[] PROGMEM = "Calibracon temperatura terminada. Haz clic para continuar.";
const char MSG_TEMP_CALIBRATION_DONE_PL[] PROGMEM = "Cieplna kalibracja zakonczona. Kontynuuj przyciskiem";
const char MSG_TEMP_CALIBRATION_DONE_DE[] PROGMEM = "Temp. Kalibrierung fertig. Klicken um weiter zu gehen.";
const char * const MSG_TEMP_CALIBRATION_DONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMP_CALIBRATION_DONE_EN,
	MSG_TEMP_CALIBRATION_DONE_CZ,
	MSG_TEMP_CALIBRATION_DONE_IT,
	MSG_TEMP_CALIBRATION_DONE_ES,
	MSG_TEMP_CALIBRATION_DONE_PL,
	MSG_TEMP_CALIBRATION_DONE_DE
};

const char MSG_TEMP_CALIBRATION_OFF_EN[] PROGMEM = "Temp. cal. [OFF]";
const char MSG_TEMP_CALIBRATION_OFF_CZ[] PROGMEM = "Tepl. kal. [OFF]";
const char MSG_TEMP_CALIBRATION_OFF_IT[] PROGMEM = "Cal. temp. [OFF]";
const char MSG_TEMP_CALIBRATION_OFF_ES[] PROGMEM = "Cal. temp. [OFF]";
const char MSG_TEMP_CALIBRATION_OFF_PL[] PROGMEM = "Ciepl. kal. [OFF]";
const char MSG_TEMP_CALIBRATION_OFF_DE[] PROGMEM = "Temp. Kal. [OFF]";
const char * const MSG_TEMP_CALIBRATION_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMP_CALIBRATION_OFF_EN,
	MSG_TEMP_CALIBRATION_OFF_CZ,
	MSG_TEMP_CALIBRATION_OFF_IT,
	MSG_TEMP_CALIBRATION_OFF_ES,
	MSG_TEMP_CALIBRATION_OFF_PL,
	MSG_TEMP_CALIBRATION_OFF_DE
};

const char MSG_TEMP_CALIBRATION_ON_EN[] PROGMEM = "Temp. cal. [ON]";
const char MSG_TEMP_CALIBRATION_ON_CZ[] PROGMEM = "Tepl. kal. [ON]";
const char MSG_TEMP_CALIBRATION_ON_IT[] PROGMEM = "Cal. temp. [ON]";
const char MSG_TEMP_CALIBRATION_ON_ES[] PROGMEM = "Cal. temp. [ON]";
const char MSG_TEMP_CALIBRATION_ON_PL[] PROGMEM = "Ciepl. kal. [ON]";
const char MSG_TEMP_CALIBRATION_ON_DE[] PROGMEM = "Temp. Kal. [ON]";
const char * const MSG_TEMP_CALIBRATION_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TEMP_CALIBRATION_ON_EN,
	MSG_TEMP_CALIBRATION_ON_CZ,
	MSG_TEMP_CALIBRATION_ON_IT,
	MSG_TEMP_CALIBRATION_ON_ES,
	MSG_TEMP_CALIBRATION_ON_PL,
	MSG_TEMP_CALIBRATION_ON_DE
};

const char MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_EN[] PROGMEM = "SD card [normal]";
const char MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_PL[] PROGMEM = "karta SD [normal]";
const char MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_DE[] PROGMEM = "SD Karte [normal]";
const char * const MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_EN,
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_EN,
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_EN,
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_EN,
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_PL,
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF_DE
};

const char MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_EN[] PROGMEM = "SD card [FlshAir]";
const char MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_PL[] PROGMEM = "karta SD[FlshAir]";
const char MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_DE[] PROGMEM = "SD Karte [FlashAir]";
const char * const MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_EN,
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_EN,
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_EN,
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_EN,
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_PL,
	MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON_DE
};

const char MSG_TUNE_EN[] PROGMEM = "Tune";
const char MSG_TUNE_CZ[] PROGMEM = "Ladit";
const char MSG_TUNE_IT[] PROGMEM = "Regola";
const char MSG_TUNE_ES[] PROGMEM = "Ajustar";
const char MSG_TUNE_PL[] PROGMEM = "Nastroic";
const char MSG_TUNE_DE[] PROGMEM = "Feineinstellung";
const char * const MSG_TUNE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_TUNE_EN,
	MSG_TUNE_CZ,
	MSG_TUNE_IT,
	MSG_TUNE_ES,
	MSG_TUNE_PL,
	MSG_TUNE_DE
};

const char MSG_UNKNOWN_COMMAND_EN[] PROGMEM = "Unknown command: \"";
const char * const MSG_UNKNOWN_COMMAND_LANG_TABLE[1] PROGMEM = {
	MSG_UNKNOWN_COMMAND_EN
};

const char MSG_UNLOADING_FILAMENT_EN[] PROGMEM = "Unloading filament";
const char MSG_UNLOADING_FILAMENT_CZ[] PROGMEM = "Vysouvam filament";
const char MSG_UNLOADING_FILAMENT_IT[] PROGMEM = "Rilasc. filamento";
const char MSG_UNLOADING_FILAMENT_ES[] PROGMEM = "Soltando filamento";
const char MSG_UNLOADING_FILAMENT_PL[] PROGMEM = "Wysuwam filament";
const char MSG_UNLOADING_FILAMENT_DE[] PROGMEM = "Filament auswerfen";
const char * const MSG_UNLOADING_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOADING_FILAMENT_EN,
	MSG_UNLOADING_FILAMENT_CZ,
	MSG_UNLOADING_FILAMENT_IT,
	MSG_UNLOADING_FILAMENT_ES,
	MSG_UNLOADING_FILAMENT_PL,
	MSG_UNLOADING_FILAMENT_DE
};

const char MSG_UNLOAD_ALL_EN[] PROGMEM = "Unload all";
const char MSG_UNLOAD_ALL_CZ[] PROGMEM = "Vyjmout vse";
const char MSG_UNLOAD_ALL_IT[] PROGMEM = "Rilasciare tutti";
const char MSG_UNLOAD_ALL_ES[] PROGMEM = "Soltar todos fil.";
const char MSG_UNLOAD_ALL_PL[] PROGMEM = "Wyjac wszystkie";
const char MSG_UNLOAD_ALL_DE[] PROGMEM = "Alles entladen";
const char * const MSG_UNLOAD_ALL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_ALL_EN,
	MSG_UNLOAD_ALL_CZ,
	MSG_UNLOAD_ALL_IT,
	MSG_UNLOAD_ALL_ES,
	MSG_UNLOAD_ALL_PL,
	MSG_UNLOAD_ALL_DE
};

const char MSG_UNLOAD_FILAMENT_EN[] PROGMEM = "Unload filament";
const char MSG_UNLOAD_FILAMENT_CZ[] PROGMEM = "Vyjmout filament";
const char MSG_UNLOAD_FILAMENT_IT[] PROGMEM = "Scarica filamento";
const char MSG_UNLOAD_FILAMENT_ES[] PROGMEM = "Soltar filamento";
const char MSG_UNLOAD_FILAMENT_PL[] PROGMEM = "Wyjac filament";
const char MSG_UNLOAD_FILAMENT_DE[] PROGMEM = "Filament entladen";
const char * const MSG_UNLOAD_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_EN,
	MSG_UNLOAD_FILAMENT_CZ,
	MSG_UNLOAD_FILAMENT_IT,
	MSG_UNLOAD_FILAMENT_ES,
	MSG_UNLOAD_FILAMENT_PL,
	MSG_UNLOAD_FILAMENT_DE
};

const char MSG_UNLOAD_FILAMENT_1_EN[] PROGMEM = "Unload filament 1";
const char MSG_UNLOAD_FILAMENT_1_CZ[] PROGMEM = "Vyjmout filam. 1";
const char MSG_UNLOAD_FILAMENT_1_IT[] PROGMEM = "Rilasciare fil. 1";
const char MSG_UNLOAD_FILAMENT_1_ES[] PROGMEM = "Soltar fil. 1";
const char MSG_UNLOAD_FILAMENT_1_PL[] PROGMEM = "Wyjac filament 1";
const char MSG_UNLOAD_FILAMENT_1_DE[] PROGMEM = "Filam. 1 entladen";
const char * const MSG_UNLOAD_FILAMENT_1_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_1_EN,
	MSG_UNLOAD_FILAMENT_1_CZ,
	MSG_UNLOAD_FILAMENT_1_IT,
	MSG_UNLOAD_FILAMENT_1_ES,
	MSG_UNLOAD_FILAMENT_1_PL,
	MSG_UNLOAD_FILAMENT_1_DE
};

const char MSG_UNLOAD_FILAMENT_2_EN[] PROGMEM = "Unload filament 2";
const char MSG_UNLOAD_FILAMENT_2_CZ[] PROGMEM = "Vyjmout filam. 2";
const char MSG_UNLOAD_FILAMENT_2_IT[] PROGMEM = "Rilasciare fil. 1";
const char MSG_UNLOAD_FILAMENT_2_ES[] PROGMEM = "Soltar fil. 2";
const char MSG_UNLOAD_FILAMENT_2_PL[] PROGMEM = "Wyjac filament 2";
const char MSG_UNLOAD_FILAMENT_2_DE[] PROGMEM = "Filam. 2 entladen";
const char * const MSG_UNLOAD_FILAMENT_2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_2_EN,
	MSG_UNLOAD_FILAMENT_2_CZ,
	MSG_UNLOAD_FILAMENT_2_IT,
	MSG_UNLOAD_FILAMENT_2_ES,
	MSG_UNLOAD_FILAMENT_2_PL,
	MSG_UNLOAD_FILAMENT_2_DE
};

const char MSG_UNLOAD_FILAMENT_3_EN[] PROGMEM = "Unload filament 3";
const char MSG_UNLOAD_FILAMENT_3_CZ[] PROGMEM = "Vyjmout filam. 3";
const char MSG_UNLOAD_FILAMENT_3_IT[] PROGMEM = "Rilasciare fil. 1";
const char MSG_UNLOAD_FILAMENT_3_ES[] PROGMEM = "Soltar fil. 3";
const char MSG_UNLOAD_FILAMENT_3_PL[] PROGMEM = "Wyjac filament 3";
const char MSG_UNLOAD_FILAMENT_3_DE[] PROGMEM = "Filam. 3 entladen";
const char * const MSG_UNLOAD_FILAMENT_3_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_3_EN,
	MSG_UNLOAD_FILAMENT_3_CZ,
	MSG_UNLOAD_FILAMENT_3_IT,
	MSG_UNLOAD_FILAMENT_3_ES,
	MSG_UNLOAD_FILAMENT_3_PL,
	MSG_UNLOAD_FILAMENT_3_DE
};

const char MSG_UNLOAD_FILAMENT_4_EN[] PROGMEM = "Unload filament 4";
const char MSG_UNLOAD_FILAMENT_4_CZ[] PROGMEM = "Vyjmout filam. 4";
const char MSG_UNLOAD_FILAMENT_4_IT[] PROGMEM = "Rilasciare fil. 1";
const char MSG_UNLOAD_FILAMENT_4_ES[] PROGMEM = "Soltar fil. 4";
const char MSG_UNLOAD_FILAMENT_4_PL[] PROGMEM = "Wyjac filament 4";
const char MSG_UNLOAD_FILAMENT_4_DE[] PROGMEM = "Filam. 4 entladen";
const char * const MSG_UNLOAD_FILAMENT_4_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_UNLOAD_FILAMENT_4_EN,
	MSG_UNLOAD_FILAMENT_4_CZ,
	MSG_UNLOAD_FILAMENT_4_IT,
	MSG_UNLOAD_FILAMENT_4_ES,
	MSG_UNLOAD_FILAMENT_4_PL,
	MSG_UNLOAD_FILAMENT_4_DE
};

const char MSG_USB_PRINTING_EN[] PROGMEM = "USB printing  ";
const char MSG_USB_PRINTING_CZ[] PROGMEM = "Tisk z USB  ";
const char MSG_USB_PRINTING_IT[] PROGMEM = "Stampa da USB";
const char MSG_USB_PRINTING_ES[] PROGMEM = "Impresion con USB ";
const char MSG_USB_PRINTING_PL[] PROGMEM = "Druk z USB  ";
const char MSG_USB_PRINTING_DE[] PROGMEM = "Drucken ueber USB";
const char * const MSG_USB_PRINTING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_USB_PRINTING_EN,
	MSG_USB_PRINTING_CZ,
	MSG_USB_PRINTING_IT,
	MSG_USB_PRINTING_ES,
	MSG_USB_PRINTING_PL,
	MSG_USB_PRINTING_DE
};

const char MSG_USED_EN[] PROGMEM = "Used during print";
const char MSG_USED_CZ[] PROGMEM = "Pouzite behem tisku";
const char MSG_USED_IT[] PROGMEM = "Usati nella stampa";
const char MSG_USED_ES[] PROGMEM = "Usado en impresion";
const char MSG_USED_PL[] PROGMEM = "Uzyte przy druku";
const char MSG_USED_DE[] PROGMEM = "Beim Druck benutzte";
const char * const MSG_USED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_USED_EN,
	MSG_USED_CZ,
	MSG_USED_IT,
	MSG_USED_ES,
	MSG_USED_PL,
	MSG_USED_DE
};

const char MSG_USERWAIT_EN[] PROGMEM = "Wait for user...";
const char MSG_USERWAIT_IT[] PROGMEM = "Attendendo utente";
const char MSG_USERWAIT_ES[] PROGMEM = "Esperando ordenes";
const char MSG_USERWAIT_DE[] PROGMEM = "Warte auf user...";
const char * const MSG_USERWAIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_USERWAIT_EN,
	MSG_USERWAIT_EN,
	MSG_USERWAIT_IT,
	MSG_USERWAIT_ES,
	MSG_USERWAIT_EN,
	MSG_USERWAIT_DE
};

const char MSG_V2_CALIBRATION_EN[] PROGMEM = "First layer cal.";
const char MSG_V2_CALIBRATION_CZ[] PROGMEM = "Kal. prvni vrstvy";
const char MSG_V2_CALIBRATION_IT[] PROGMEM = "Cal. primo layer.";
const char MSG_V2_CALIBRATION_ES[] PROGMEM = "Cal. primera cap.";
const char MSG_V2_CALIBRATION_PL[] PROGMEM = "Kal. 1. warstwy";
const char MSG_V2_CALIBRATION_DE[] PROGMEM = "Erste-Schicht Kal";
const char * const MSG_V2_CALIBRATION_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_V2_CALIBRATION_EN,
	MSG_V2_CALIBRATION_CZ,
	MSG_V2_CALIBRATION_IT,
	MSG_V2_CALIBRATION_ES,
	MSG_V2_CALIBRATION_PL,
	MSG_V2_CALIBRATION_DE
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
const char MSG_WAITING_TEMP_IT[] PROGMEM = "In attesa del raffreddamento della testina e del piatto";
const char MSG_WAITING_TEMP_ES[] PROGMEM = "Esperando enfriamiento de heatbed y extrusor.";
const char MSG_WAITING_TEMP_PL[] PROGMEM = "Oczekiwanie na wychlodzenie dyszy i podkladki.";
const char MSG_WAITING_TEMP_DE[] PROGMEM = "Warten auf Abkuehlung von Heater und Bett.";
const char * const MSG_WAITING_TEMP_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WAITING_TEMP_EN,
	MSG_WAITING_TEMP_CZ,
	MSG_WAITING_TEMP_IT,
	MSG_WAITING_TEMP_ES,
	MSG_WAITING_TEMP_PL,
	MSG_WAITING_TEMP_DE
};

const char MSG_WATCH_EN[] PROGMEM = "Info screen";
const char MSG_WATCH_CZ[] PROGMEM = "Informace";
const char MSG_WATCH_IT[] PROGMEM = "Schermata info";
const char MSG_WATCH_ES[] PROGMEM = "Monitorizar";
const char MSG_WATCH_PL[] PROGMEM = "Informacje";
const char MSG_WATCH_DE[] PROGMEM = "Information";
const char * const MSG_WATCH_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WATCH_EN,
	MSG_WATCH_CZ,
	MSG_WATCH_IT,
	MSG_WATCH_ES,
	MSG_WATCH_PL,
	MSG_WATCH_DE
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
const char MSG_WIZARD_CALIBRATION_FAILED_IT[] PROGMEM = "Per favore consulta il nostro manuale per risolvere il problema. Poi riprendi il Wizard dopo aver riavviato la stampante.";
const char MSG_WIZARD_CALIBRATION_FAILED_ES[] PROGMEM = "Lee el manual y resuelve el problema. Despues, reinicia la impresora y continua con el Wizard";
const char MSG_WIZARD_CALIBRATION_FAILED_PL[] PROGMEM = "Prosze sprawdz nasz poradnik i napraw problem. Potem przywroc Wizard restartujac drukarke.";
const char MSG_WIZARD_CALIBRATION_FAILED_DE[] PROGMEM = "Bitte ueberpruefen Sie unser Handbuch und beheben Sie das Problem. Fahren Sie dann mit dem Assistenten fort, indem Sie den Drucker neu starten.";
const char * const MSG_WIZARD_CALIBRATION_FAILED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_CALIBRATION_FAILED_EN,
	MSG_WIZARD_CALIBRATION_FAILED_CZ,
	MSG_WIZARD_CALIBRATION_FAILED_IT,
	MSG_WIZARD_CALIBRATION_FAILED_ES,
	MSG_WIZARD_CALIBRATION_FAILED_PL,
	MSG_WIZARD_CALIBRATION_FAILED_DE
};

const char MSG_WIZARD_CLEAN_HEATBED_EN[] PROGMEM = "Please clean heatbed and then press the knob.";
const char MSG_WIZARD_CLEAN_HEATBED_CZ[] PROGMEM = "Prosim ocistete heatbed a stisknete tlacitko.";
const char MSG_WIZARD_CLEAN_HEATBED_IT[] PROGMEM = "Per favore pulisci il piatto, poi premi la manopola.";
const char MSG_WIZARD_CLEAN_HEATBED_ES[] PROGMEM = "Limpia la superficie de la heatbed, por favor, y haz clic";
const char MSG_WIZARD_CLEAN_HEATBED_PL[] PROGMEM = "Prosze oczysc stolik i nacisnij guzik.";
const char MSG_WIZARD_CLEAN_HEATBED_DE[] PROGMEM = "Bitte reinigen Sie das Heizbett und druecken Sie dann den Knopf.";
const char * const MSG_WIZARD_CLEAN_HEATBED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_CLEAN_HEATBED_EN,
	MSG_WIZARD_CLEAN_HEATBED_CZ,
	MSG_WIZARD_CLEAN_HEATBED_IT,
	MSG_WIZARD_CLEAN_HEATBED_ES,
	MSG_WIZARD_CLEAN_HEATBED_PL,
	MSG_WIZARD_CLEAN_HEATBED_DE
};

const char MSG_WIZARD_DONE_EN[] PROGMEM = "All is done. Happy printing!";
const char MSG_WIZARD_DONE_CZ[] PROGMEM = "Vse je hotovo.";
const char MSG_WIZARD_DONE_IT[] PROGMEM = "Ben fatto. Buona stampa!";
const char MSG_WIZARD_DONE_ES[] PROGMEM = "Terminado! Feliz impresion!";
const char MSG_WIZARD_DONE_PL[] PROGMEM = "Gotowe. Udanego druku!";
const char MSG_WIZARD_DONE_DE[] PROGMEM = "Alles wurde getan. Viel Spass beim Drucken!";
const char * const MSG_WIZARD_DONE_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_DONE_EN,
	MSG_WIZARD_DONE_CZ,
	MSG_WIZARD_DONE_IT,
	MSG_WIZARD_DONE_ES,
	MSG_WIZARD_DONE_PL,
	MSG_WIZARD_DONE_DE
};

const char MSG_WIZARD_FILAMENT_LOADED_EN[] PROGMEM = "Is filament loaded?";
const char MSG_WIZARD_FILAMENT_LOADED_CZ[] PROGMEM = "Je filament zaveden?";
const char MSG_WIZARD_FILAMENT_LOADED_IT[] PROGMEM = "Il filamento e stato caricato?";
const char MSG_WIZARD_FILAMENT_LOADED_ES[] PROGMEM = "Esta el filamento cargado?";
const char MSG_WIZARD_FILAMENT_LOADED_PL[] PROGMEM = "Filament jest zaladowany?";
const char MSG_WIZARD_FILAMENT_LOADED_DE[] PROGMEM = "Ist das Filament geladen?";
const char * const MSG_WIZARD_FILAMENT_LOADED_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_FILAMENT_LOADED_EN,
	MSG_WIZARD_FILAMENT_LOADED_CZ,
	MSG_WIZARD_FILAMENT_LOADED_IT,
	MSG_WIZARD_FILAMENT_LOADED_ES,
	MSG_WIZARD_FILAMENT_LOADED_PL,
	MSG_WIZARD_FILAMENT_LOADED_DE
};

const char MSG_WIZARD_HEATING_EN[] PROGMEM = "Preheating nozzle. Please wait.";
const char MSG_WIZARD_HEATING_CZ[] PROGMEM = "Predehrivam trysku. Prosim cekejte.";
const char MSG_WIZARD_HEATING_IT[] PROGMEM = "Sto preriscaldando l'ugello. Per favore attendi.";
const char MSG_WIZARD_HEATING_ES[] PROGMEM = "Precalentando nozzle. Espera por favor.";
const char MSG_WIZARD_HEATING_PL[] PROGMEM = "Nagrzewanie dyszy. Prosze czekac.";
const char MSG_WIZARD_HEATING_DE[] PROGMEM = "Vorheizen der Duese. Bitte warten.";
const char * const MSG_WIZARD_HEATING_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_HEATING_EN,
	MSG_WIZARD_HEATING_CZ,
	MSG_WIZARD_HEATING_IT,
	MSG_WIZARD_HEATING_ES,
	MSG_WIZARD_HEATING_PL,
	MSG_WIZARD_HEATING_DE
};

const char MSG_WIZARD_INSERT_CORRECT_FILAMENT_EN[] PROGMEM = "Please load PLA filament and then resume Wizard by rebooting the printer.";
const char MSG_WIZARD_INSERT_CORRECT_FILAMENT_CZ[] PROGMEM = "Prosim zavedte PLA filament a po te obnovte Wizarda stisknutim reset tlacitka.";
const char MSG_WIZARD_INSERT_CORRECT_FILAMENT_IT[] PROGMEM = "Per favore carica filamento di PLA e riprendi il Wizard dopo aver riavviato la stampante.";
const char MSG_WIZARD_INSERT_CORRECT_FILAMENT_ES[] PROGMEM = "Carga filamento PLA, por favor, y reinicia la impresora para continuar con el Wizard";
const char MSG_WIZARD_INSERT_CORRECT_FILAMENT_PL[] PROGMEM = "Prosze zaladuj filament PLA i przywroc Wizard przez restart drukarki.";
const char MSG_WIZARD_INSERT_CORRECT_FILAMENT_DE[] PROGMEM = "Bitte laden Sie PLA-Filament und fahren Sie mit dem Assistenten fort, indem Sie den Drucker neu starten.";
const char * const MSG_WIZARD_INSERT_CORRECT_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_INSERT_CORRECT_FILAMENT_EN,
	MSG_WIZARD_INSERT_CORRECT_FILAMENT_CZ,
	MSG_WIZARD_INSERT_CORRECT_FILAMENT_IT,
	MSG_WIZARD_INSERT_CORRECT_FILAMENT_ES,
	MSG_WIZARD_INSERT_CORRECT_FILAMENT_PL,
	MSG_WIZARD_INSERT_CORRECT_FILAMENT_DE
};

const char MSG_WIZARD_LOAD_FILAMENT_EN[] PROGMEM = "Please insert PLA filament to the extruder, then press knob to load it.";
const char MSG_WIZARD_LOAD_FILAMENT_CZ[] PROGMEM = "Prosim vlozte PLA filament do extruderu, po te stisknete tlacitko pro zavedeni filamentu.";
const char MSG_WIZARD_LOAD_FILAMENT_IT[] PROGMEM = "Per favore inserisci il filamento di PLA nell'estrusore, poi premi la manopola per caricare.";
const char MSG_WIZARD_LOAD_FILAMENT_ES[] PROGMEM = "Inserta, por favor, filamento PLA en el extrusor. Despues haz clic para cargarlo.";
const char MSG_WIZARD_LOAD_FILAMENT_PL[] PROGMEM = "Prosze umiesc filament PLA w ekstruderze i nacisnij przycisk by zaladowac.";
const char MSG_WIZARD_LOAD_FILAMENT_DE[] PROGMEM = "Fuehren Sie bitte PLA Filament in den Extruder und bestaetigen Sie den Knopf um es zu laden.";
const char * const MSG_WIZARD_LOAD_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_LOAD_FILAMENT_EN,
	MSG_WIZARD_LOAD_FILAMENT_CZ,
	MSG_WIZARD_LOAD_FILAMENT_IT,
	MSG_WIZARD_LOAD_FILAMENT_ES,
	MSG_WIZARD_LOAD_FILAMENT_PL,
	MSG_WIZARD_LOAD_FILAMENT_DE
};

const char MSG_WIZARD_PLA_FILAMENT_EN[] PROGMEM = "Is it PLA filament?";
const char MSG_WIZARD_PLA_FILAMENT_CZ[] PROGMEM = "Je to PLA filament?";
const char MSG_WIZARD_PLA_FILAMENT_IT[] PROGMEM = "E questo un filamento di PLA?";
const char MSG_WIZARD_PLA_FILAMENT_ES[] PROGMEM = "Es el filamento PLA?";
const char MSG_WIZARD_PLA_FILAMENT_PL[] PROGMEM = "Czy to filament PLA?";
const char MSG_WIZARD_PLA_FILAMENT_DE[] PROGMEM = "Ist es wirklich PLA Filament?";
const char * const MSG_WIZARD_PLA_FILAMENT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_PLA_FILAMENT_EN,
	MSG_WIZARD_PLA_FILAMENT_CZ,
	MSG_WIZARD_PLA_FILAMENT_IT,
	MSG_WIZARD_PLA_FILAMENT_ES,
	MSG_WIZARD_PLA_FILAMENT_PL,
	MSG_WIZARD_PLA_FILAMENT_DE
};

const char MSG_WIZARD_QUIT_EN[] PROGMEM = "You can always resume the Wizard from Calibration -> Wizard.";
const char MSG_WIZARD_QUIT_CZ[] PROGMEM = "Wizarda muzete kdykoliv znovu spustit z menu Calibration -> Wizard";
const char MSG_WIZARD_QUIT_IT[] PROGMEM = "E possibile proseguire la guide Wizard in qualsiasi momento attraverso Calibrazione -> Wizard.";
const char MSG_WIZARD_QUIT_ES[] PROGMEM = "Siempre puedes acceder al asistente desde Calibracion -> Wizard";
const char MSG_WIZARD_QUIT_PL[] PROGMEM = "Zawsze mozesz przywrocic Wizard przez Kalibracja -> Wizard.";
const char MSG_WIZARD_QUIT_DE[] PROGMEM = "Sie koennen immer den Asistenten im Menu neu aufrufen: Kalibrierung -> Assistant";
const char * const MSG_WIZARD_QUIT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_QUIT_EN,
	MSG_WIZARD_QUIT_CZ,
	MSG_WIZARD_QUIT_IT,
	MSG_WIZARD_QUIT_ES,
	MSG_WIZARD_QUIT_PL,
	MSG_WIZARD_QUIT_DE
};

const char MSG_WIZARD_REPEAT_V2_CAL_EN[] PROGMEM = "Do you want to repeat last step to readjust distance between nozzle and heatbed?";
const char MSG_WIZARD_REPEAT_V2_CAL_CZ[] PROGMEM = "Chcete opakovat posledni krok a pozmenit vzdalenost mezi tryskou a heatbed?";
const char MSG_WIZARD_REPEAT_V2_CAL_IT[] PROGMEM = "Desideri ripetere l'ultimo passaggio per migliorare la distanza fra ugello e piatto?";
const char MSG_WIZARD_REPEAT_V2_CAL_ES[] PROGMEM = "Quieres repetir el ultimo paso para reajustar la distancia nozzle-heatbed?";
const char MSG_WIZARD_REPEAT_V2_CAL_PL[] PROGMEM = "Chcesz powtorzyc ostatni krok i przestawic odleglosc miedzy dysza a stolikiem?";
const char MSG_WIZARD_REPEAT_V2_CAL_DE[] PROGMEM = "Moechten Sie den letzten Schritt wiederholen um den Abstand zwischen Duese und Druckbett neu einzustellen?";
const char * const MSG_WIZARD_REPEAT_V2_CAL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_REPEAT_V2_CAL_EN,
	MSG_WIZARD_REPEAT_V2_CAL_CZ,
	MSG_WIZARD_REPEAT_V2_CAL_IT,
	MSG_WIZARD_REPEAT_V2_CAL_ES,
	MSG_WIZARD_REPEAT_V2_CAL_PL,
	MSG_WIZARD_REPEAT_V2_CAL_DE
};

const char MSG_WIZARD_RERUN_EN[] PROGMEM = "Running Wizard will delete current calibration results and start from the beginning. Continue?";
const char MSG_WIZARD_RERUN_CZ[] PROGMEM = "Spusteni Wizarda vymaze ulozene vysledky vsech kalibraci a spusti kalibracni proces od zacatku. Pokracovat?";
const char MSG_WIZARD_RERUN_IT[] PROGMEM = "Se avvi il Wizard perderai la calibrazione preesistente e dovrai ricominciare dall'inizio. Continuare?";
const char MSG_WIZARD_RERUN_ES[] PROGMEM = "Ejecutar el Wizard borrara los valores de calibracion actuales y comenzara de nuevo. Continuar?";
const char MSG_WIZARD_RERUN_PL[] PROGMEM = "Wlaczenie Wizard usunie obecne dane kalibracyjne i zacznie od nowa. Kontynuowac?";
const char MSG_WIZARD_RERUN_DE[] PROGMEM = "Der laufende Assistent loescht die aktuelle Kalibrierergebnisse und wird von Anfang an beginnen. Fortsetzen?";
const char * const MSG_WIZARD_RERUN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_RERUN_EN,
	MSG_WIZARD_RERUN_CZ,
	MSG_WIZARD_RERUN_IT,
	MSG_WIZARD_RERUN_ES,
	MSG_WIZARD_RERUN_PL,
	MSG_WIZARD_RERUN_DE
};

const char MSG_WIZARD_SELFTEST_EN[] PROGMEM = "First, I will run the selftest to check most common assembly problems.";
const char MSG_WIZARD_SELFTEST_CZ[] PROGMEM = "Nejdriv pomoci selftestu zkontoluji nejcastejsi chyby vznikajici pri sestaveni tiskarny.";
const char MSG_WIZARD_SELFTEST_IT[] PROGMEM = "Anzitutto avviero il Self Test per controllare gli errori di assemblaggio piu comuni.";
const char MSG_WIZARD_SELFTEST_ES[] PROGMEM = "Primero, hare el Selftest para comprobar los problemas de montaje mas comunes.";
const char MSG_WIZARD_SELFTEST_PL[] PROGMEM = "Najpierw wlacze autotest w celu kontrolli najczestszych problemow z montazem.";
const char MSG_WIZARD_SELFTEST_DE[] PROGMEM = "Zunaechst fuehre ich den Selbsttest durch um die haeufigsten Probleme bei der Aufbau zu ueberpruefen.";
const char * const MSG_WIZARD_SELFTEST_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_SELFTEST_EN,
	MSG_WIZARD_SELFTEST_CZ,
	MSG_WIZARD_SELFTEST_IT,
	MSG_WIZARD_SELFTEST_ES,
	MSG_WIZARD_SELFTEST_PL,
	MSG_WIZARD_SELFTEST_DE
};

const char MSG_WIZARD_V2_CAL_EN[] PROGMEM = "Now I will calibrate distance between tip of the nozzle and heatbed surface.";
const char MSG_WIZARD_V2_CAL_CZ[] PROGMEM = "Nyni zkalibruji vzdalenost mezi koncem trysky a povrchem heatbedu.";
const char MSG_WIZARD_V2_CAL_IT[] PROGMEM = "Adesso tarero lo stacco fra ugello e superfice del piatto.";
const char MSG_WIZARD_V2_CAL_ES[] PROGMEM = "Voy a calibrar la distancia entre la punta del nozzle y la superficie de la heatbed.";
const char MSG_WIZARD_V2_CAL_PL[] PROGMEM = "Kalibruje odleglosc miedzy koncowka dyszy a stolikiem.";
const char MSG_WIZARD_V2_CAL_DE[] PROGMEM = "Jetzt werde ich den Abstand zwischen Duesenspitze und Druckbett kalibrieren.";
const char * const MSG_WIZARD_V2_CAL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_V2_CAL_EN,
	MSG_WIZARD_V2_CAL_CZ,
	MSG_WIZARD_V2_CAL_IT,
	MSG_WIZARD_V2_CAL_ES,
	MSG_WIZARD_V2_CAL_PL,
	MSG_WIZARD_V2_CAL_DE
};

const char MSG_WIZARD_V2_CAL_2_EN[] PROGMEM = "I will start to print line and you will gradually lower the nozzle by rotating the knob, until you reach optimal height. Check the pictures in our handbook in chapter Calibration.";
const char MSG_WIZARD_V2_CAL_2_CZ[] PROGMEM = "Zacnu tisknout linku a Vy budete postupne snizovat trysku otacenim tlacitka dokud nedosahnete optimalni vysky. Prohlednete si obrazky v nasi prirucce v kapitole Kalibrace";
const char MSG_WIZARD_V2_CAL_2_IT[] PROGMEM = "Adesso iniziero a stampare una linea e tu dovrai abbassare l'ugello poco per volta ruotando la manopola sino a raggiungere una altezza ottimale. Per favore dai uno sguardo all'immagine del nostro manuale, cap.Calibrazione.";
const char MSG_WIZARD_V2_CAL_2_ES[] PROGMEM = "Voy a comenzar a imprimir la linea y tu bajaras el nozzle gradualmente al rotar el mando, hasta que llegues a la altura optima. Mira las imagenes del capitulo Calibracion en el manual.";
const char MSG_WIZARD_V2_CAL_2_PL[] PROGMEM = "Zaczne drukowac linie. Stopniowo opuszczaj dysze przekrecajac guzik, poki nie uzyskasz optymalnej wysokosci. Sprawdz obrazki w naszym poradniku w rozdz. Kalibracja";
const char MSG_WIZARD_V2_CAL_2_DE[] PROGMEM = "Ich werde jetzt erste Linie drucken. Waehrend des Druckes koennen Sie die Duese allmaehlich senken indem Sie den Knopf drehen, bis Sie die optimale Hoehe erreichen. Ueberpruefen Sie die Bilder in unserem Handbuch im Kapitel Kalibrierung";
const char * const MSG_WIZARD_V2_CAL_2_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_V2_CAL_2_EN,
	MSG_WIZARD_V2_CAL_2_CZ,
	MSG_WIZARD_V2_CAL_2_IT,
	MSG_WIZARD_V2_CAL_2_ES,
	MSG_WIZARD_V2_CAL_2_PL,
	MSG_WIZARD_V2_CAL_2_DE
};

const char MSG_WIZARD_WELCOME_EN[] PROGMEM = "Hi, I am your Original Prusa i3 printer. Would you like me to guide you through the setup process?";
const char MSG_WIZARD_WELCOME_CZ[] PROGMEM = "Dobry den, jsem vase tiskarna Original Prusa i3. Chcete abych Vas provedla kalibracnim procesem?";
const char MSG_WIZARD_WELCOME_IT[] PROGMEM = "Ciao, sono la tua stampante Original Prusa i3. Gradiresti aiuto attraverso il processo di configurazione?";
const char MSG_WIZARD_WELCOME_ES[] PROGMEM = "Hola, soy tu impresora Original Prusa i3. Quieres que te guie a traves de la configuracion?";
const char MSG_WIZARD_WELCOME_PL[] PROGMEM = "Czesc, jestem Twoja drukarka Original Prusa i3. Czy potrzebujesz pomocy z instalacja?";
const char MSG_WIZARD_WELCOME_DE[] PROGMEM = "Hallo, ich bin dein Original Prusa i3 Drucker. Moechten Sie meine Hilfe durch den Setup-Prozess?";
const char * const MSG_WIZARD_WELCOME_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_WELCOME_EN,
	MSG_WIZARD_WELCOME_CZ,
	MSG_WIZARD_WELCOME_IT,
	MSG_WIZARD_WELCOME_ES,
	MSG_WIZARD_WELCOME_PL,
	MSG_WIZARD_WELCOME_DE
};

const char MSG_WIZARD_WILL_PREHEAT_EN[] PROGMEM = "Now I will preheat nozzle for PLA.";
const char MSG_WIZARD_WILL_PREHEAT_CZ[] PROGMEM = "Nyni predehreji trysku pro PLA.";
const char MSG_WIZARD_WILL_PREHEAT_IT[] PROGMEM = "Adesso preriscaldero l'ugello per PLA.";
const char MSG_WIZARD_WILL_PREHEAT_ES[] PROGMEM = "Voy a precalentar el nozzle para PLA ahora.";
const char MSG_WIZARD_WILL_PREHEAT_PL[] PROGMEM = "Nagrzewam dysze dla PLA.";
const char MSG_WIZARD_WILL_PREHEAT_DE[] PROGMEM = "Jetzt werde ich die Duese fuer PLA vorheizen. ";
const char * const MSG_WIZARD_WILL_PREHEAT_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_WILL_PREHEAT_EN,
	MSG_WIZARD_WILL_PREHEAT_CZ,
	MSG_WIZARD_WILL_PREHEAT_IT,
	MSG_WIZARD_WILL_PREHEAT_ES,
	MSG_WIZARD_WILL_PREHEAT_PL,
	MSG_WIZARD_WILL_PREHEAT_DE
};

const char MSG_WIZARD_XYZ_CAL_EN[] PROGMEM = "I will run xyz calibration now. It will take approx. 12 mins.";
const char MSG_WIZARD_XYZ_CAL_CZ[] PROGMEM = "Nyni provedu xyz kalibraci. Zabere to priblizne 12 min.";
const char MSG_WIZARD_XYZ_CAL_IT[] PROGMEM = "Adesso avviero una Calibrazione XYZ. Puo durare circa 12 min.";
const char MSG_WIZARD_XYZ_CAL_ES[] PROGMEM = "Hare la calibracion XYZ. Tardara 12 min. aproximadamente.";
const char MSG_WIZARD_XYZ_CAL_PL[] PROGMEM = "Wlaczam kalibracje xyz. Zajmie to ok. 12 min.";
const char MSG_WIZARD_XYZ_CAL_DE[] PROGMEM = "Ich werde jetzt die XYZ-Kalibrierung durchfuehren. Es wird ca. 12 Minuten dauern";
const char * const MSG_WIZARD_XYZ_CAL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_XYZ_CAL_EN,
	MSG_WIZARD_XYZ_CAL_CZ,
	MSG_WIZARD_XYZ_CAL_IT,
	MSG_WIZARD_XYZ_CAL_ES,
	MSG_WIZARD_XYZ_CAL_PL,
	MSG_WIZARD_XYZ_CAL_DE
};

const char MSG_WIZARD_Z_CAL_EN[] PROGMEM = "I will run z calibration now.";
const char MSG_WIZARD_Z_CAL_CZ[] PROGMEM = "Nyni provedu z kalibraci.";
const char MSG_WIZARD_Z_CAL_IT[] PROGMEM = "Adesso avviero una Calibrazione Z.";
const char MSG_WIZARD_Z_CAL_ES[] PROGMEM = "Voy a hacer Calibracion Z ahora.";
const char MSG_WIZARD_Z_CAL_PL[] PROGMEM = "Wlaczam kalibracje z.";
const char MSG_WIZARD_Z_CAL_DE[] PROGMEM = "Ich werde jetzt die Z Kalibrierung durchfuehren.";
const char * const MSG_WIZARD_Z_CAL_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_WIZARD_Z_CAL_EN,
	MSG_WIZARD_Z_CAL_CZ,
	MSG_WIZARD_Z_CAL_IT,
	MSG_WIZARD_Z_CAL_ES,
	MSG_WIZARD_Z_CAL_PL,
	MSG_WIZARD_Z_CAL_DE
};

const char MSG_XYZ_DETAILS_EN[] PROGMEM = "XYZ cal. details";
const char MSG_XYZ_DETAILS_CZ[] PROGMEM = "Detaily XYZ kal.";
const char MSG_XYZ_DETAILS_IT[] PROGMEM = "XYZ Cal. dettagli";
const char MSG_XYZ_DETAILS_ES[] PROGMEM = "Inform. XYZ cal.";
const char MSG_XYZ_DETAILS_PL[] PROGMEM = "Szczegoly kal.XYZ";
const char MSG_XYZ_DETAILS_DE[] PROGMEM = "XYZ Kal. Details";
const char * const MSG_XYZ_DETAILS_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_XYZ_DETAILS_EN,
	MSG_XYZ_DETAILS_CZ,
	MSG_XYZ_DETAILS_IT,
	MSG_XYZ_DETAILS_ES,
	MSG_XYZ_DETAILS_PL,
	MSG_XYZ_DETAILS_DE
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
const char MSG_YES_IT[] PROGMEM = "Si";
const char MSG_YES_ES[] PROGMEM = "Si";
const char MSG_YES_PL[] PROGMEM = "Tak";
const char MSG_YES_DE[] PROGMEM = "Ja";
const char * const MSG_YES_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_YES_EN,
	MSG_YES_CZ,
	MSG_YES_IT,
	MSG_YES_ES,
	MSG_YES_PL,
	MSG_YES_DE
};

const char MSG_Y_DISTANCE_FROM_MIN_EN[] PROGMEM = "Y distance from min:";
const char MSG_Y_DISTANCE_FROM_MIN_CZ[] PROGMEM = "Y vzdalenost od min:";
const char MSG_Y_DISTANCE_FROM_MIN_IT[] PROGMEM = "Distanza Y da min:";
const char MSG_Y_DISTANCE_FROM_MIN_ES[] PROGMEM = "Dist. Y desde min:";
const char MSG_Y_DISTANCE_FROM_MIN_PL[] PROGMEM = "Odleglosc Y od min.:";
const char MSG_Y_DISTANCE_FROM_MIN_DE[] PROGMEM = "Y Entfernung vom min";
const char * const MSG_Y_DISTANCE_FROM_MIN_LANG_TABLE[LANG_NUM] PROGMEM = {
	MSG_Y_DISTANCE_FROM_MIN_EN,
	MSG_Y_DISTANCE_FROM_MIN_CZ,
	MSG_Y_DISTANCE_FROM_MIN_IT,
	MSG_Y_DISTANCE_FROM_MIN_ES,
	MSG_Y_DISTANCE_FROM_MIN_PL,
	MSG_Y_DISTANCE_FROM_MIN_DE
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
const char WELCOME_MSG_IT[] PROGMEM = CUSTOM_MENDEL_NAME " pronta.";
const char WELCOME_MSG_ES[] PROGMEM = CUSTOM_MENDEL_NAME " prep.";
const char WELCOME_MSG_PL[] PROGMEM = CUSTOM_MENDEL_NAME " gotowa";
const char WELCOME_MSG_DE[] PROGMEM = CUSTOM_MENDEL_NAME " bereit.";
const char * const WELCOME_MSG_LANG_TABLE[LANG_NUM] PROGMEM = {
	WELCOME_MSG_EN,
	WELCOME_MSG_CZ,
	WELCOME_MSG_IT,
	WELCOME_MSG_ES,
	WELCOME_MSG_PL,
	WELCOME_MSG_DE
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
