/**
 * Swedish
 *
 * LCD Menu Messages
 * Please note these are limited to 17 characters!
 *
 */
#ifndef LANGUAGE_SV_H
#define LANGUAGE_SV_H

#define WELCOME_MSG                         MACHINE_NAME " klar.”
#define MSG_SD_INSERTED                     ”SD kort isatt"
#define MSG_SD_REMOVED                      ”SD kort borttaget"
#define MSG_MAIN                            ”Huvudmeny”
#define MSG_AUTOSTART                       "Autostart"
#define MSG_DISABLE_STEPPERS                ”Inaktivera motorer”
#define MSG_AUTO_HOME                       "Auto hem”
#define MSG_SET_HOME_OFFSETS                ”Sätt hem offset"
#define MSG_SET_ORIGIN                      ”Sätt utgångspunkt"
#define MSG_PREHEAT_PLA                     ”Förvärm PLA"
#define MSG_PREHEAT_PLA0                    "Förvärm PLA 1"
#define MSG_PREHEAT_PLA1                    "Förvärm PLA 2"
#define MSG_PREHEAT_PLA2                    "Förvärm PLA 3"
#define MSG_PREHEAT_PLA012                  "Förvärm PLA Alla”
#define MSG_PREHEAT_PLA_BEDONLY             "Förvärm PLA Bädd"
#define MSG_PREHEAT_PLA_SETTINGS            "Förvärm PLA konf"
#define MSG_PREHEAT_ABS                     "Förvärm ABS"
#define MSG_PREHEAT_ABS0                    "Förvärm ABS 1"
#define MSG_PREHEAT_ABS1                    "Förvärm ABS 2"
#define MSG_PREHEAT_ABS2                    "Förvärm ABS 3"
#define MSG_PREHEAT_ABS012                  "Förvärm ABS Alla”
#define MSG_PREHEAT_ABS_BEDONLY             "Förvärm ABS Bädd"
#define MSG_PREHEAT_ABS_SETTINGS            "Förvärm ABS konf"
#define MSG_COOLDOWN                        ”Svalnar”
#define MSG_SWITCH_PS_ON                    ”Slå på strömmen”
#define MSG_SWITCH_PS_OFF                   ”Slå av strömmen”
#define MSG_EXTRUDE                         "Extrudera”
#define MSG_RETRACT                         ”Dra tillbaka”
#define MSG_MOVE_AXIS                       ”Flytta axel”
#define MSG_MOVE_X                          ”Flytta X"
#define MSG_MOVE_Y                          ”Flytta Y"
#define MSG_MOVE_Z                          ”Flytta Z"
#define MSG_MOVE_E                          "Extruder"
#define MSG_MOVE_E1                         "Extruder2"
#define MSG_MOVE_E2                         "Extruder3"
#define MSG_MOVE_01MM                       ”Flytta 0.1mm"
#define MSG_MOVE_1MM                        ”Flytta 1mm"
#define MSG_MOVE_10MM                       ”Flytta 10mm"
#define MSG_SPEED                           ”Hastighet”
#define MSG_NOZZLE                          ”Munstycke”
#define MSG_NOZZLE1                         ”Munstycke2"
#define MSG_NOZZLE2                         ”Munstycke3"
#define MSG_BED                             "Bädd"
#define MSG_FAN_SPEED                       ”Fläkthastighet”
#define MSG_FLOW                            ”Flöde”
#define MSG_FLOW0                           "Flöde 0"
#define MSG_FLOW1                           "Flöde 1"
#define MSG_FLOW2                           "Flöde 2"
#define MSG_CONTROL                         ”Kontroll”
#define MSG_MIN                             " \002 Min"
#define MSG_MAX                             " \002 Max"
#define MSG_FACTOR                          " \002 Fakt”
#define MSG_AUTOTEMP                        "Autotemp"
#define MSG_ON                              ”På "
#define MSG_OFF                             ”Av”
#define MSG_PID_P                           "PID-P"
#define MSG_PID_I                           "PID-I"
#define MSG_PID_D                           "PID-D"
#define MSG_PID_C                           "PID-C"
#define MSG_ACC                             "Accel"
#define MSG_VXY_JERK                        "Vxy-ryck”
#define MSG_VZ_JERK                         "Vz-ryck”
#define MSG_VE_JERK                         "Ve-ryck”
#define MSG_VMAX                            "Vmax "
#define MSG_X                               "x"
#define MSG_Y                               "y"
#define MSG_Z                               "z"
#define MSG_E                               "e"
#define MSG_VMIN                            "Vmin"
#define MSG_VTRAV_MIN                       "VTrav min"
#define MSG_AMAX                            "Amax "
#define MSG_A_RETRACT                       ”A-dra tillbaka”
#define MSG_XSTEPS                          "Xsteg/mm"
#define MSG_YSTEPS                          "Ysteg/mm"
#define MSG_ZSTEPS                          "Zsteg/mm"
#define MSG_ESTEPS                          "Esteg/mm"
#define MSG_TEMPERATURE                     "Temperatur"
#define MSG_MOTION                          ”Rörelse”
#define MSG_VOLUMETRIC                      "Filament"
#define MSG_VOLUMETRIC_ENABLED		        "E per mm3"
#define MSG_FILAMENT_SIZE_EXTRUDER_0        "Fil. Dia. 1"
#define MSG_FILAMENT_SIZE_EXTRUDER_1        "Fil. Dia. 2"
#define MSG_FILAMENT_SIZE_EXTRUDER_2        "Fil. Dia. 3"
#define MSG_CONTRAST                        "LCD kontrast"
#define MSG_STORE_EPROM                     ”Lagra minne”
#define MSG_LOAD_EPROM                      ”Läs från minne”
#define MSG_RESTORE_FAILSAFE                ”Återställ felsäk”
#define MSG_REFRESH                         ”Uppdatera”
#define MSG_WATCH                           "Info skärm”
#define MSG_PREPARE                         ”Förbered”
#define MSG_TUNE                            ”Justera”
#define MSG_PAUSE_PRINT                     "Pausa print"
#define MSG_RESUME_PRINT                    ”Återuppta print"
#define MSG_STOP_PRINT                      "Stoppa print"
#define MSG_CARD_MENU                       "Printa från SD"
#define MSG_NO_CARD                         ”Inget SD kort”
#define MSG_DWELL                           ”Vila…”
#define MSG_USERWAIT                        ”Väntar på anv…”
#define MSG_RESUMING                        ”Återupptar print"
#define MSG_PRINT_ABORTED                   "Print avbruten”
#define MSG_NO_MOVE                         ”Ingen rörelse.”
#define MSG_KILLED                          ”DÖDAD. "
#define MSG_STOPPED                         ”STOPPAD. "
#define MSG_CONTROL_RETRACT                 ”Dra tillbaka mm"
#define MSG_CONTROL_RETRACT_SWAP            "Swap Re.mm"
#define MSG_CONTROL_RETRACTF                ”Dra tillbaka  V"
#define MSG_CONTROL_RETRACT_ZLIFT           "Hop mm"
#define MSG_CONTROL_RETRACT_RECOVER         "UnRet +mm"
#define MSG_CONTROL_RETRACT_RECOVER_SWAP    "S UnRet+mm"
#define MSG_CONTROL_RETRACT_RECOVERF        "UnRet  V"
#define MSG_AUTORETRACT                     "AutoDraTillb.”
#define MSG_FILAMENTCHANGE                  ”Byt filament"
#define MSG_INIT_SDCARD                     "Init. SD kort”
#define MSG_CNG_SDCARD                      ”Byt SD kort”
#define MSG_ZPROBE_OUT                      "Z probe utanf. bädd”
#define MSG_POSITION_UNKNOWN                ”Ta hem X/Y före Z"
#define MSG_ZPROBE_ZOFFSET                  "Z Offset"
#define MSG_BABYSTEP_X                      ”Myrsteg X"
#define MSG_BABYSTEP_Y                      "Myrsteg Y"
#define MSG_BABYSTEP_Z                      ”Realtidsjustera Z"
#define MSG_ENDSTOP_ABORT                   ”Ändstopp avbrott”
#define MSG_ADJUSTZ							"Auto just Z ?"

#define MSG_HOMEYZ                          ”Kalibrera Z"
#define MSG_SETTINGS                         ”Inställningar”
#define MSG_PREHEAT                         "Förvärm"
#define MSG_UNLOAD_FILAMENT                 ”Ladda ur filament"
#define MSG_LOAD_FILAMENT                 ”Ladda filament"

#define MSG_RECTRACT                        ”Dra tillbaka”
#define MSG_ERROR                        ”FEL:”
#define MSG_PREHEAT_NOZZLE                       "Förvärm munstycket!”
#define MSG_SUPPORT "Support"
#define MSG_CORRECTLY			”Rätt ändrad?”
#define MSG_YES					”Ja”
#define MSG_NO					Nej”
#define MSG_NOT_LOADED 			"Filament ej laddat”
#define MSG_NOT_COLOR 			”Färg ej klar”
#define MSG_LOADING_FILAMENT	”Laddar filament”
#define MSG_PLEASE_WAIT			”Vänta”
#define MSG_LOADING_COLOR		”Laddar färg”
#define MSG_CHANGE_SUCCESS		”Bytet lyckades!”
#define MSG_PRESS				”Och tryck på reglaget”
#define MSG_INSERT_FILAMENT		”Sätt i filament”
#define MSG_CHANGING_FILAMENT	”Byter filament!"


#define MSG_SILENT_MODE_ON					”Läge     [tyst]”
#define MSG_SILENT_MODE_OFF					”Läge [mer kraft]” 
#define MSG_REBOOT							”Starta om printern”
#define MSG_TAKE_EFFECT						" för att ladda ändringar”											

#define MSG_Enqueing                        ”ställer i kö \""
#define MSG_POWERUP                         ”Startar”
#define MSG_EXTERNAL_RESET                  " Extern återställning”
#define MSG_BROWNOUT_RESET                  " Strömavbr nollst”
#define MSG_WATCHDOG_RESET                  " Vakthund nollst”
#define MSG_SOFTWARE_RESET                  " Mjukvara nollst"
#define MSG_AUTHOR                          " | Författare: "
#define MSG_CONFIGURATION_VER               " Senast uppdaterad: "
#define MSG_FREE_MEMORY                     " Ledigt minne: "
#define MSG_PLANNER_BUFFER_BYTES            "  PlannerBufferBytes: "
#define MSG_OK                              "ok"
#define MSG_FILE_SAVED                      ”Sparat klart fil.”
#define MSG_ERR_LINE_NO                     "Line Number is not Last Line Number+1, Last Line: "
#define MSG_ERR_CHECKSUM_MISMATCH           "checksum mismatch, Last Line: "
#define MSG_ERR_NO_CHECKSUM                 "No Checksum with line number, Last Line: "
#define MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM "No Line Number with checksum, Last Line: "
#define MSG_FILE_PRINTED                    ”Printat klart filen”
#define MSG_BEGIN_FILE_LIST                 "Begin file list"
#define MSG_END_FILE_LIST                   "End file list"
#define MSG_M104_INVALID_EXTRUDER           "M104 Ogiltig extruder "
#define MSG_M105_INVALID_EXTRUDER           "M105 Ogiltig extruder "
#define MSG_M200_INVALID_EXTRUDER           "M200 Ogiltig extruder "
#define MSG_M218_INVALID_EXTRUDER           "M218 Ogiltig extruder "
#define MSG_M221_INVALID_EXTRUDER           "M221 Ogiltig extruder "
#define MSG_ERR_NO_THERMISTORS              ”Inga termistorer - ingen temperatur”
#define MSG_M109_INVALID_EXTRUDER           "M109 Ogiltig extruder "
#define MSG_HEATING                         ”Värmer upp…”
#define MSG_HEATING_COMPLETE                ”Uppvärmning klar.”
#define MSG_BED_HEATING                     ”Värmer upp bädden.”
#define MSG_BED_DONE                        ”Bädden klar.”
#define MSG_M115_REPORT                     "FIRMWARE_NAME:Marlin V1.0.2; Sprinter/grbl mashup for gen6 FIRMWARE_URL:" FIRMWARE_URL " PROTOCOL_VERSION:" PROTOCOL_VERSION " MACHINE_TYPE:" MACHINE_NAME " EXTRUDER_COUNT:" STRINGIFY(EXTRUDERS) " UUID:" MACHINE_UUID "\n"
#define MSG_COUNT_X                         " Count X: "
#define MSG_ERR_KILLED                      ”Printer stoppad. kill() called!"
#define MSG_ERR_STOPPED                     "Printer stoppad på grund av fel. Åtgärda felet och använd M999 för omstart. (Temperatur nollställd. Ställ in efter omstart)”
#define MSG_RESEND                          ”Skickar igen: "
#define MSG_UNKNOWN_COMMAND                 ”Okänt kommando: \""
#define MSG_ACTIVE_EXTRUDER                 "Aktiv Extruder: "
#define MSG_INVALID_EXTRUDER                "Ogiltig extruder"
#define MSG_X_MIN                           "x_min: "
#define MSG_X_MAX                           "x_max: "
#define MSG_Y_MIN                           "y_min: "
#define MSG_Y_MAX                           "y_max: "
#define MSG_Z_MIN                           "z_min: "
#define MSG_Z_MAX                           "z_max: "
#define MSG_M119_REPORT                     ”Rapporterar status för ändstopp”
#define MSG_ENDSTOP_HIT                     ”TRIGGAD”
#define MSG_ENDSTOP_OPEN                    ”öppen”
#define MSG_HOTEND_OFFSET                   "Hotend offsets:"

#define MSG_SD_CANT_OPEN_SUBDIR             ”Kan ej öppn underkat”
#define MSG_SD_INIT_FAIL                    "SD init missl”
#define MSG_SD_VOL_INIT_FAIL                ”vol.init missl"
#define MSG_SD_OPENROOT_FAIL                "openRoot missl"
#define MSG_SD_CARD_OK                      "SD kort ok"
#define MSG_SD_WORKDIR_FAIL                 "workDir öppning missl”
#define MSG_SD_OPEN_FILE_FAIL               ”öppning missl, Fil: "
#define MSG_SD_FILE_OPENED                  ”Fil öppnad: "
#define MSG_SD_SIZE                         " Storlek: "
#define MSG_SD_FILE_SELECTED                "Fil vald”
#define MSG_SD_WRITE_TO_FILE                ”Skriver till fil: "
#define MSG_SD_PRINTING_BYTE                "SD printar byte "
#define MSG_SD_NOT_PRINTING                 "SD printar ej”
#define MSG_SD_ERR_WRITE_TO_FILE            ”fel vid skrivn till fil”
#define MSG_SD_CANT_ENTER_SUBDIR            ”Kan ej gå in i underkat: "

#define MSG_STEPPER_TOO_HIGH                "Steprate too high: "
#define MSG_ENDSTOPS_HIT                    "endstops hit: "
#define MSG_ERR_COLD_EXTRUDE_STOP           " kallextrudering förhindr”
#define MSG_ERR_LONG_EXTRUDE_STOP           " för lång extrudering förhindr”
#define MSG_BABYSTEPPING_X                  ”Myrstegar X"
#define MSG_BABYSTEPPING_Y                  ”Myrstegar Y"
#define MSG_BABYSTEPPING_Z                  ”Justerar Z"
#define MSG_SERIAL_ERROR_MENU_STRUCTURE     ”Fel i menystrukturen”

#define MSG_LANGUAGE_NAME					”Svenska”
#define MSG_LANGUAGE_SELECT					”Välj språk     "
 #define MSG_PRUSA3D					"prusa3d.com"
 #define MSG_PRUSA3D_FORUM					"forum.prusa3d.com"
 #define MSG_PRUSA3D_HOWTO					"howto.prusa3d.com"
 

#endif // LANGUAGE_SV_H
