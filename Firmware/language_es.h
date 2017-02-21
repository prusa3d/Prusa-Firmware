/**
 * Spanish
 *
 * LCD Menu Messages
 * Please note these are limited to 17 characters!
 *
 */

#define WELCOME_MSG                         CUSTOM_MENDEL_NAME " lista"
#define MSG_SD_INSERTED                     "Tarjeta colocada"
#define MSG_SD_REMOVED                      "Tarjeta retirada"
#define MSG_MAIN                            "Menu principal"
#define MSG_DISABLE_STEPPERS                "Apagar motores"
#define MSG_AUTO_HOME                       "Llevar al origen"
#define MSG_COOLDOWN                        "Enfriar"
#define MSG_MOVE_AXIS                       "Mover ejes"
#define MSG_MOVE_X                          "Mover X"
#define MSG_MOVE_Y                          "Mover Y"
#define MSG_MOVE_Z                          "Mover Z"
#define MSG_MOVE_E                          "Extrusor"
#define MSG_SPEED                           "Velocidad"
#define MSG_NOZZLE                          "Fusor"
#define MSG_BED                             "Base"
#define MSG_FAN_SPEED                       "Ventilador"
#define MSG_FLOW                            "Flujo"
#define MSG_TEMPERATURE                     "Temperatura"
#define MSG_WATCH                           "Monitorizar"
#define MSG_TUNE                            "Ajustar"
#define MSG_PAUSE_PRINT                     "Pausar impresion"
#define MSG_RESUME_PRINT                    "Reanudar impres."
#define MSG_STOP_PRINT                      "Detener impresion"
#define MSG_CARD_MENU                       "Menu de SD"
#define MSG_NO_CARD                         "No hay tarjeta SD"
#define MSG_DWELL                           "Reposo..."
#define MSG_USERWAIT                        "Esperando ordenes"
#define MSG_RESUMING                        "Resumiendo impre."
#define MSG_PRINT_ABORTED                   "Print aborted"
#define MSG_NO_MOVE                         "Sin movimiento"
#define MSG_KILLED                          "PARADA DE EMERG."
#define MSG_STOPPED                         "PARADA"
#define MSG_FILAMENTCHANGE                  "Cambiar filamento"
#define MSG_BABYSTEP_Z                      "Micropaso Z"
#define MSG_ADJUSTZ							"Auto Micropaso Z?"
#define MSG_PICK_Z							"Vyberte vytisk"

#define MSG_SETTINGS                        "Ajuste"
#define MSG_PREHEAT                         "Precalentar"
#define MSG_UNLOAD_FILAMENT                 "Sacar filamento"
#define MSG_LOAD_FILAMENT					"Poner filamento"
#define MSG_ERROR							"ERROR:"
#define MSG_PREHEAT_NOZZLE                  "Precal. extrusor!"
#define MSG_SUPPORT							"Support"
#define MSG_CORRECTLY						"Cambiado correc.?"
#define MSG_YES								"Si"
#define MSG_NO								"No"
#define MSG_NOT_LOADED 						"Fil. no cargado"
#define MSG_NOT_COLOR 						"Color no claro"
#define MSG_LOADING_FILAMENT				"Cargando fil."
#define MSG_PLEASE_WAIT						"Espera"
#define MSG_LOADING_COLOR					"Cargando color"
#define MSG_CHANGE_SUCCESS					"Cambiar bien!"
#define MSG_PRESS							"y pulse el mando"
#define MSG_INSERT_FILAMENT					"Inserta filamento"
#define MSG_CHANGING_FILAMENT				"Cambiando fil.!"
#define MSG_SILENT_MODE_ON					"Modo   [silencio]"
#define MSG_SILENT_MODE_OFF					"Modo [mas fuerza]" 
#define MSG_REBOOT							"Reiniciar la imp."
#define MSG_TAKE_EFFECT						"para tomar efecto"											
#define MSG_HEATING                         "Calentando..."
#define MSG_HEATING_COMPLETE                "Calentando listo."
#define MSG_BED_HEATING                     "Base Calentando"
#define MSG_BED_DONE                        "Base listo."
#define MSG_LANGUAGE_NAME					"Espanol"
#define MSG_LANGUAGE_SELECT					"Cambia la lengua "
#define MSG_PRUSA3D							"prusa3d.com"
#define MSG_PRUSA3D_FORUM					"forum.prusa3d.com"
#define MSG_PRUSA3D_HOWTO					"howto.prusa3d.com"


// Do not translate those!

#define MSG_Enqueing                        "enqueing \""
#define MSG_POWERUP                         "PowerUp"
#define MSG_CONFIGURATION_VER               " Last Updated: "
#define MSG_FREE_MEMORY                     " Free Memory: "
#define MSG_PLANNER_BUFFER_BYTES            "  PlannerBufferBytes: "
#define MSG_OK                              "ok"
#define MSG_ERR_CHECKSUM_MISMATCH           "checksum mismatch, Last Line: "
#define MSG_ERR_NO_CHECKSUM                 "No Checksum with line number, Last Line: "
#define MSG_BEGIN_FILE_LIST                 "Begin file list"
#define MSG_END_FILE_LIST                   "End file list"
#define MSG_M104_INVALID_EXTRUDER           "M104 Invalid extruder "
#define MSG_M105_INVALID_EXTRUDER           "M105 Invalid extruder "
#define MSG_M200_INVALID_EXTRUDER           "M200 Invalid extruder "
#define MSG_M218_INVALID_EXTRUDER           "M218 Invalid extruder "
#define MSG_M221_INVALID_EXTRUDER           "M221 Invalid extruder "
#define MSG_ERR_NO_THERMISTORS              "No thermistors - no temperature"
#define MSG_M109_INVALID_EXTRUDER           "M109 Invalid extruder "
#define MSG_M115_REPORT                     "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:" FIRMWARE_URL " PROTOCOL_VERSION:" PROTOCOL_VERSION " MACHINE_TYPE:" CUSTOM_MENDEL_NAME " EXTRUDER_COUNT:" STRINGIFY(EXTRUDERS) " UUID:" MACHINE_UUID "\n"
#define MSG_ERR_KILLED                      "Printer halted. kill() called!"
#define MSG_ERR_STOPPED                     "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)"
#define MSG_RESEND                          "Resend: "
#define MSG_M119_REPORT                     "Reporting endstop status"
#define MSG_ENDSTOP_HIT                     "TRIGGERED"
#define MSG_ENDSTOP_OPEN                    "open"
#define MSG_SD_CANT_OPEN_SUBDIR             "Cannot open subdir"
#define MSG_SD_INIT_FAIL                    "SD init fail"
#define MSG_SD_VOL_INIT_FAIL                "volume.init failed"
#define MSG_SD_OPENROOT_FAIL                "openRoot failed"
#define MSG_SD_CARD_OK                      "SD card ok"
#define MSG_SD_WORKDIR_FAIL                 "workDir open failed"
#define MSG_SD_OPEN_FILE_FAIL               "open failed, File: "
#define MSG_SD_FILE_OPENED                  "File opened: "
#define MSG_SD_FILE_SELECTED                "File selected"
#define MSG_SD_WRITE_TO_FILE                "Writing to file: "
#define MSG_SD_PRINTING_BYTE                "SD printing byte "
#define MSG_SD_NOT_PRINTING                 "Not SD printing"
#define MSG_SD_ERR_WRITE_TO_FILE            "error writing to file"
#define MSG_SD_CANT_ENTER_SUBDIR            "Cannot enter subdir: "
#define MSG_STEPPER_TOO_HIGH                "Steprate too high: "
#define MSG_ENDSTOPS_HIT                    "endstops hit: "
#define MSG_ERR_COLD_EXTRUDE_STOP           " cold extrusion prevented"
#define MSG_BABYSTEPPING_X                  "Babystepping X"
#define MSG_BABYSTEPPING_Y                  "Babystepping Y"
#define MSG_BABYSTEPPING_Z                  "Adjusting Z"
#define MSG_SERIAL_ERROR_MENU_STRUCTURE     "Error in menu structure"
#define MSG_SET_HOME_OFFSETS                "Set home offsets"
#define MSG_SET_ORIGIN                      "Set origin"
#define MSG_SWITCH_PS_ON                    "Switch power on"
#define MSG_SWITCH_PS_OFF                   "Switch power off"
#define MSG_NOZZLE1                         "Nozzle2"
#define MSG_NOZZLE2                         "Nozzle3"
#define MSG_FLOW0                           "Flow 0"
#define MSG_FLOW1                           "Flow 1"
#define MSG_FLOW2                           "Flow 2"
#define MSG_CONTROL                         "Control"
#define MSG_MIN                             " \002 Min"
#define MSG_MAX                             " \002 Max"
#define MSG_FACTOR                          " \002 Fact"
#define MSG_MOTION                          "Motion"
#define MSG_VOLUMETRIC                      "Filament"
#define MSG_VOLUMETRIC_ENABLED		        "E in mm3"
#define MSG_STORE_EPROM                     "Store memory"
#define MSG_LOAD_EPROM                      "Load memory"
#define MSG_RESTORE_FAILSAFE                "Restore failsafe"
#define MSG_REFRESH                         "\xF8" "Refresh"
#define MSG_INIT_SDCARD                     "Init. SD card"
#define MSG_CNG_SDCARD                      "Change SD card"
#define MSG_BABYSTEP_X                      "Babystep X"
#define MSG_BABYSTEP_Y                      "Babystep Y"
#define MSG_RECTRACT                        "Rectract"

#define MSG_HOMEYZ                          "Calibrar Z"
#define MSG_HOMEYZ_PROGRESS                 "Calibrando Z"
#define MSG_HOMEYZ_DONE                     "Calibracion OK"

#define MSG_SELFTEST_ERROR                  "Autotest error!"
#define MSG_SELFTEST_PLEASECHECK            "Controla :"   
#define MSG_SELFTEST_NOTCONNECTED           "No hay conexion  "
#define MSG_SELFTEST_HEATERTHERMISTOR       "Calent./Termistor"
#define MSG_SELFTEST_BEDHEATER              "Cama/Calentador"
#define MSG_SELFTEST_WIRINGERROR            "Error de conexion"
#define MSG_SELFTEST_ENDSTOPS               "Topes final"
#define MSG_SELFTEST_MOTOR                  "Motor"
#define MSG_SELFTEST_ENDSTOP                "Tope final"
#define MSG_SELFTEST_ENDSTOP_NOTHIT         "Tope fin. no toc."
#define MSG_SELFTEST_OK                     "Self test OK"

#define(length=20) MSG_SELFTEST_FAN					"Test del ventilador";
#define(length=20) MSG_SELFTEST_COOLING_FAN			"Vent. al frente?";
#define(length=20) MSG_SELFTEST_EXTRUDER_FAN			"Vent. en la izg?";
#define MSG_SELFTEST_FAN_YES				"Ventilador gira";
#define MSG_SELFTEST_FAN_NO					"Ventilador no gira";

#define MSG_STATS_TOTALFILAMENT             "Filamento total:"
#define MSG_STATS_TOTALPRINTTIME            "Tiempo total :"
#define MSG_STATS_FILAMENTUSED              "Filamento :  "
#define MSG_STATS_PRINTTIME                 "Tiempo de imp.:"

#define MSG_SELFTEST_START                  "Autotest salida"
#define MSG_SELFTEST_CHECK_ENDSTOPS         "Cont. topes final"
#define MSG_SELFTEST_CHECK_HOTEND           "Control hotend " 
#define MSG_SELFTEST_CHECK_X                "Control del eje X"
#define MSG_SELFTEST_CHECK_Y                "Control del eje Y"
#define MSG_SELFTEST_CHECK_Z                "Control del eje Z"
#define MSG_SELFTEST_CHECK_BED              "Control de cama"
#define MSG_SELFTEST_CHECK_ALLCORRECT       "Todo bie "
#define MSG_SELFTEST                        "Autotest"
#define MSG_SELFTEST_FAILED                 "Autotest fallado"

#define MSG_STATISTICS                      "Estadistica  "
#define MSG_USB_PRINTING                    "Impresion de USB "

#define MSG_SHOW_END_STOPS                  "Ensena tope final"
#define MSG_CALIBRATE_BED                   "Calibra XYZ"
#define MSG_CALIBRATE_BED_RESET             "Reset XYZ calibr."
#define MSG_MOVE_CARRIAGE_TO_THE_TOP        "Calibrando XYZ. Gira el boton para subir el carro Z hasta golpe piezas superioras. Despues haz clic."
#define MSG_MOVE_CARRIAGE_TO_THE_TOP_Z       "Calibrando Z. Gira el boton para subir el carro Z hasta golpe piezas superioras. Despues haz clic."

#define MSG_CONFIRM_NOZZLE_CLEAN            "Limpiar boquilla para calibracion. Click cuando acabes."
#define MSG_CONFIRM_CARRIAGE_AT_THE_TOP     "Carros Z izq./der. estan arriba maximo?"
#define MSG_FIND_BED_OFFSET_AND_SKEW_LINE1  "Buscando cama punto de calibracion"
#define MSG_FIND_BED_OFFSET_AND_SKEW_LINE2  " de 4"
#define MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1   "Mejorando cama punto de calibracion"
#define MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2   " de 9"
#define MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1	"Medir la altura del punto de la calibracion"
#define MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2	" de 9"

#define MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND           "Calibracion XYZ fallada. Puntos de calibracion en la cama no encontrados."
#define MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED            "Calibracion XYZ fallada. Consultar el manual por favor."
#define MSG_BED_SKEW_OFFSET_DETECTION_PERFECT               "Calibracion XYZ ok. Ejes X/Y perpendiculares. Felicitaciones!"
#define MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD             "Calibracion XYZ correcta. Los ejes X / Y estan ligeramente inclinados. Buen trabajo!"
#define MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME          "Calibracion XYZ correcta. La inclinacion se corregira automaticamente."
#define MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR     "Calibracion XYZ fallad. Punto delantero izquierdo no alcanzable."
#define MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR        "Calibracion XYZ fallad. Punto delantero derecho no alcanzable."
#define MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR     "Calibracion XYZ fallad. Punto delanteros no alcanzables."
#define MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR        "Calibrazion XYZ comprometida. Punto delantero izquierdo no alcanzable."
#define MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR       "Calibrazion XYZ comprometida. Punto delantero derecho no alcanzable."
#define MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR        "Calibrazion XYZ comprometida. Punto delanteros no alcanzables."
#define MSG_BED_LEVELING_FAILED_POINT_LOW           				"Nivelacion fallada. Sensor no funciona. Escombros en Boqui.? Esperando reset."
#define MSG_BED_LEVELING_FAILED_POINT_HIGH          				"Nivelacion fallada. Sensor funciona demasiado temprano. Esperando reset."
#define MSG_BED_LEVELING_FAILED_PROBE_DISCONNECTED      			"Nivelacion fallada. Sensor desconectado o cables danados. Esperando reset."
#define MSG_NEW_FIRMWARE_AVAILABLE                  				"Nuevo firmware disponible:"
#define MSG_NEW_FIRMWARE_PLEASE_UPGRADE                 			"Actualizar por favor"
#define MSG_FOLLOW_CALIBRATION_FLOW                    			"Impresora no esta calibrada todavia. Por favor usar el manual, el capitulo First steps, seleccion Calibration flow."
#define MSG_BABYSTEP_Z_NOT_SET                      			"Distancia entre la punta de la boquilla y la superficie de la cama no fijada aun. Por favor siga el manual, capitulo First steps, seccion First layer calibration."
#define MSG_BED_CORRECTION_MENU                                 "Corr. de la cama"
#define MSG_BED_CORRECTION_LEFT                                 "Izquierda [um]"
#define MSG_BED_CORRECTION_RIGHT                                "Derecha   [um]"
#define MSG_BED_CORRECTION_FRONT                                "Adelante  [um]"
#define MSG_BED_CORRECTION_REAR                                 "Atras     [um]"
#define MSG_BED_CORRECTION_RESET                                "Reset"

#define MSG_MESH_BED_LEVELING									"Mesh Bed Leveling"
#define MSG_MENU_CALIBRATION									"Calibracion"
#define MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF					"SD card [normal]"
#define MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON					"SD card [FlshAir]"

#define MSG_LOOSE_PULLEY								"Polea suelta"
#define MSG_FILAMENT_LOADING_T0							"Insertar filamento en el extrusor 1. Haga clic una vez terminado."
#define MSG_FILAMENT_LOADING_T1							"Insertar filamento en el extrusor 2. Haga clic una vez terminado."
#define MSG_FILAMENT_LOADING_T2							"Insertar filamento en el extrusor 3. Haga clic una vez terminado."
#define MSG_FILAMENT_LOADING_T3							"Insertar filamento en el extrusor 4. Haga clic una vez terminado."
#define MSG_CHANGE_EXTR									"Cambiar extrusor."
#define MSG_FIL_LOADED_CHECK							"Esta cargado el filamento?"
#define MSG_FIL_TUNING									"Rotar el mando para ajustar el filamento."
#define MSG_FIL_ADJUSTING								"Ajustando filamentos. Esperar por favor."
#define MSG_CONFIRM_NOZZLE_CLEAN_FIL_ADJ				"Filamentos ajustados. Limpie la boquilla para calibracion. Haga clic una vez terminado."
#define MSG_CALIBRATE_E									"Calibrar E"
#define MSG_E_CAL_KNOB									"Rotar el mando hasta que la marca llegue al cuerpo del extrusor. Haga clic una vez terminado."
#define MSG_MARK_FIL									"Marque el filamento 100 mm por encima del final del extrusor. Hacer clic una vez terminado."
#define MSG_CLEAN_NOZZLE_E								"E calibrado. Limpiar la boquilla. Haga clic una vez terminado."
#define MSG_WAITING_TEMP								"Esperando enfriamiento de la cama y del extrusor."
#define MSG_FILAMENT_CLEAN								"Es el nuevo color nitido?"
#define MSG_UNLOADING_FILAMENT							"Soltando filamento"

