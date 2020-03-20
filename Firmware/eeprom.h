
/**
 * @file
 */
 /** \ingroup eeprom_table */
 //! _This is a EEPROM table of currently implemented in Prusa firmware (dynamically generated from doxygen)._


#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

#define MAX_SHEETS 8
#define MAX_SHEET_NAME_LENGTH 7

typedef struct
{
    char name[MAX_SHEET_NAME_LENGTH]; //!< Can be null terminated, doesn't need to be null terminated
    int16_t z_offset; //!< Z_BABYSTEP_MIN .. Z_BABYSTEP_MAX = Z_BABYSTEP_MIN*2/1000 [mm] .. Z_BABYSTEP_MAX*2/1000 [mm]
    uint8_t bed_temp; //!< 0 .. 254 [°C]
    uint8_t pinda_temp; //!< 0 .. 254 [°C]
} Sheet;

typedef struct
{
    Sheet s[MAX_SHEETS];
    uint8_t active_sheet;
} Sheets;
// sizeof(Sheets). Do not change it unless EEPROM_Sheets_base is last item in EEPROM.
// Otherwise it would move following items.
#define EEPROM_SHEETS_SIZEOF 89

#ifdef __cplusplus
static_assert(sizeof(Sheets) == EEPROM_SHEETS_SIZEOF, "Sizeof(Sheets) is not EEPROM_SHEETS_SIZEOF.");
#endif
/** @defgroup eeprom_table EEPROM Table
 *  
 
  ---------------------------------------------------------------------------------
  EEPROM 8-bit Empty value = 0xFFh 255
  
  EEPROM 16-bit Empty value = 0xFFFFh 65535
  
  ---------------------------------------------------------------------------------
  How can you use the debug codes?
  - Serial terminal like Putty.
  - Octoprint does support D-codes
  - _Pronterface_ does <b>not</b> support D-codes


  ---------------------------------------------------------------------------------
  
  ## EEPROM Tabel
  
| Adress begin		| Bit/Type 	| Name 									| Valid values	| Default/FactoryReset	| Description 										| Gcode/Function| Debug code
| :--:				| :--: 		| :--: 									| :--:			| :--:					| :--:												| :--:			| :--:
| 0x0FFFh 4095		| uchar    	| EEPROM_SILENT 						| 00h 0			| ffh 255				| TMC Stealth mode off / miniRambo Power mode		| LCD menu		| D3 Ax0fff C1
| ^ 				| ^ 		| ^										| 01h 1			| ^						| TMC Stealth mode on / miniRambo Silent mode		| ^				| ^ 
| 0x0FFEh 4094		| uchar    	| EEPROM_LANG 							| 00h 0			| ffh 255				| English / LANG_ID_PRI								| LCD menu		| D3 Ax0ffe C1 
| ^ 				| ^ 		| ^										| 01h 1			| ^						| Other language LANG_ID_SEC						| ^ 			| ^
| 0x0FFCh 4092		| uint16	| EEPROM_BABYSTEP_X						| ???			| ffh 255				| Babystep for X axis _unsued_						| ??? 			| D3 Ax0ffc C2
| 0x0FFAh 4090		| uint16	| EEPROM_BABYSTEP_Y						| ???			| ffh 255				| Babystep for Y axis _unsued_						| ^ 			| D3 Ax0ffa C2
| 0x0FF8h 4088		| uint16	| EEPROM_BABYSTEP_Z						| ???			| ffh 255				| Babystep for Z axis _lagacy_						| ^ 			| D3 Ax0ff8 C2
| ^ 				| ^ 		| ^										| ^				| ^						| multiple values stored now in EEPROM_Sheets_base	| ^ 			| ^
| 0x0FF7h 4087		| uint8		| EEPROM_CALIBRATION_STATUS				| 00h 0			| ffh 255				| Unknown											| ??? 			| D3 Ax0ff7 C1
| ^ 				| ^ 		| ^										| 01h 1			| ^						| Calibrated										| ^ 			| ^
| ^ 				| ^ 		| ^										| E6h 230		| ^						| needs Live Z adjustment							| ^ 			| ^
| ^ 				| ^ 		| ^										| F0h 240		| ^						| needs Z calibration								| ^ 			| ^
| ^ 				| ^ 		| ^										| FAh 250		| ^						| needs XYZ calibration								| ^ 			| ^ 
| ^ 				| ^ 		| ^										| FFh 255		| ^						| Assbemled _default_								| ^ 			| ^
| 0x0FF5h 4085		| uint16	| EEPROM_BABYSTEP_Z0					| ???			| ???					| Babystep for Z ???								| ??? 			| D3 Ax0ff5 C2
| 0x0FF1h 4081		| uint32	| EEPROM_FILAMENTUSED					| ???			| 00h 0					| Filament used in meters							| ??? 			| D3 Ax0ff1 C4
| 0x0FEDh 4077		| uint32	| EEPROM_TOTALTIME						| ???			| 00h 0					| Total print time									| ??? 			| D3 Ax0fed C4
| 0x0FE5h 4069		| float		| EEPROM_BED_CALIBRATION_CENTER			| ???			| ???					| ???											 	| ??? 			| D3 Ax0fe5 C8
| ^					| ^			| ^										| ???			| ???					| ???												| ^ 			| ^
| 0x0FDDh 4061		| float		| EEPROM_BED_CALIBRATION_VEC_X			| ???			| ???					| ???											 	| ??? 			| D3 Ax0fdd C8
| ^					| ^			| ^										| ???			| ???					| ???												| ^ 			| ^	
| 0x0FD5h 4053		| float		| EEPROM_BED_CALIBRATION_VEC_Y			| ???			| ???					| ???											 	| ??? 			| D3 Ax0fd5 C8
| ^					| ^			| ^										| ???			| ???					| ???												| ^ 			| ^
| 0x0FC5h 4037		| int16		| EEPROM_BED_CALIBRATION_Z_JITTER		| ???			| ???					| ???											 	| ??? 			| D3 Ax0fc5 C16
| ^					| ^			| ^										| ???			| ???					| ???												| ^ 			| ^	
| ^					| ^			| ^										| ???			| ???					| ???												| ^ 			| ^
| ^					| ^			| ^										| ???			| ???					| ???												| ^ 			| ^
| ^					| ^			| ^										| ???			| ???					| ???												| ^ 			| ^
| ^					| ^			| ^										| ???			| ???					| ???												| ^ 			| ^
| ^					| ^			| ^										| ???			| ???					| ???												| ^ 			| ^
| ^					| ^			| ^										| ???			| ???					| ???												| ^ 			| ^
| 0x0FC4h 4036		| bool		| EEPROM_FARM_MODE						| 00h 0			| ffh 255				| Prusa farm mode off								| G99 			| D3 Ax0fc4 C1
| ^					| ^			| ^										| 01h 1			| ^						| Prusa farm mode on								| G98			| ^
| 0x0FC1h 4033		| int16		| EEPROM_FARM_NUMBER					| ???			| ff ff ffh				| Prusa farm number									| ??? 			| D3 Ax0fc1 C3
| 0x0FC0h 4032		| bool		| EEPROM_BED_CORRECTION_VALID			| 00h 0			| 00h 0					| Bed correction invalid							| ??? 			| D3 Ax0fc0 C1
| ^					| ^			| ^										| ffh 255		| 						| Bed correction valid	^							| ??? 			| ^
| 0x0FBFh 4031		| char		| EEPROM_BED_CORRECTION_LEFT			| 00h FFh		| 00h 0					| Bed manual correction left						| LCD menu 		| D3 Ax0fbf C1
| ^					| ^			| ^										| ???			| ???					| At this moment limited to +-100um					| ^ 			| ^
| 0x0FBEh 4030		| char		| EEPROM_BED_CORRECTION_RIGHT			| 00h FFh		| 00h 0					| Bed manual correction right						| LCD menu 		| D3 Ax0fbe C1
| ^					| ^			| ^										| ???			| ???					| At this moment limited to +-100um					| ^ 			| ^
| 0x0FBDh 4029		| char		| EEPROM_BED_CORRECTION_FRONT			| 00h FFh		| 00h 0					| Bed manual correction front						| LCD menu 		| D3 Ax0fbd C1
| ^					| ^			| ^										| ???			| ???					| At this moment limited to +-100um					| ^ 			| ^
| 0x0FBCh 4028		| char		| EEPROM_BED_CORRECTION_BACK			| 00h FFh		| 00h 0					| Bed manual correction back						| LCD menu 		| D3 Ax0fbc C1
| ^					| ^			| ^										| ???			| ???					| At this moment limited to +-100um					| ^ 			| ^
| 0x0FBBh 4027		| bool		| EEPROM_TOSHIBA_FLASH_AIR_COMPATIBLITY	| 00h 0			| 00h 0					| Toshiba Air off									| LCD menu 		| D3 Ax0fbb C1
| ^					| ^			| ^										| ??? 			| ffh 255				| Toshiba Air oon									| ^ 			| ^	
| 0x0FBAh 4026		| uchar		| EEPROM_PRINT_FLAG						| ???			| ???					| _unsued_											| ??? 			| D3 Ax0fba C1
| 0x0FB0h 4016		| int16		| EEPROM_PROBE_TEMP_SHIFT				| ???			| ???					| ???												| ??? 			| D3 Ax0fb0 C10
| ^					| ^			| ^										| ???			| ???					| ???												| ^ 			| ^
| ^					| ^			| ^										| ???			| ???					| ???												| ^ 			| ^
| ^					| ^			| ^										| ???			| ???					| ???												| ^ 			| ^
| ^					| ^			| ^										| ???			| ???					| ???												| ^ 			| ^
| 0x0FAFh 4015		| bool		| EEPROM_TEMP_CAL_ACTIVE				| 00h 0			| 00h 0					| PINDA Temp cal. inactive							| LCD menu		| D3 Ax0faf C1
| ^					| ^			| ^										| ffh 255		| ^						| PINDA Temp cal. active							| ^ 			| ^
| 0x0FA7h 4007		| uint32	| EEPROM_BOWDEN_LENGTH					| ???			| ff 00 ff ffh			| Bowden length										| ??? 			| D3 Ax0fae C8
| ^					| ^			| ^										| ???			| ff ff ff ffh			| ^													| ^ 			| ^
| 0x0FA6h 4006		| uint8		| EEPROM_CALIBRATION_STATUS_PINDA		| 00h 0			| ffh 255				| PINDA Temp not calibrated							| ??? 			| D3 Ax0fa6 C1
| ^					| ^			| ^										| 01h 1			| ^						| PINDA Temp calibrated								| ^ 			| ^
| 0x0FA5h 4005		| uint8		| EEPROM_UVLO							| 00h 0			| ffh 255				| Power Panic flag inactive 						| ??? 			| D3 Ax0fa5 C1
| ^					| ^			| ^										| 01h 1			| ^						| Power Panic flag active							| ^ 			| ^
| ^					| ^			| ^										| 02h 2			| ^						| Power Panic flag ???								| ^ 			| ^
| 0x0F9Dh 3997		| float		| EEPROM_UVLO_CURRENT_POSITION			| ???			| ffh 255				| Power Panic position 								| ??? 			| D3 Ax0f9d C8
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| 0x0F95h 3989		| char		| EEPROM_FILENAME						| ???			| ffh 255				| Power Panic Filename 								| ??? 			| D3 Ax0f95 C8
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| 0x0F91h 39851		| uint32	| EEPROM_FILE_POSITION					| ???			| ff ff ff ffh			| Power Panic File Postion 							| ??? 			| D3 Ax0f91 C4
| 0x0F8Dh 3981		| float		| EEPROM_UVLO_CURRENT_POSITION_Z		| ???			| ff ff ff ffh			| Power Panic Z Position	 						| ^ 			| D3 Ax0f8d C4
| 0x0F8Ch 3980		| ???		| EEPROM_UVLO_UNUSED_001				| ??? 			| ffh 255				| Power Panic UNUSED 								| ^ 			| D3 Ax0f8c C1
| 0x0F8Bh 3979		| uint8		| EEPROM_UVLO_TARGET_BED				| ???			| ffh 255				| Power Panic Bed temperature						| ^ 			| D3 Ax0f8b C1
| 0x0F89h 3977		| uint16	| EEPROM_UVLO_FEEDRATE					| ???			| ff ffh 65535			| Power Panic Feedrate								| ^ 			| D3 Ax0f89 C2
| 0x0F88h 3976		| uint8		| EEPROM_UVLO_FAN_SPEED					| ???			| ffh 255				| Power Panic Fan speed								| ^ 			| D3 Ax0f88 C1
| 0x0F87h 3975		| uint8		| EEPROM_FAN_CHECK_ENABLED				| 00h 0			| ???					| Fan Check disabled								| LCD menu		| D3 Ax0f87 C1
| ^					| ^			| ^										| 01h 1			| ffh 255				| Fan Check enabled (exception ffh=01h)				| ^ 			| ^
| 0x0F75h 3957		| uint16	| EEPROM_UVLO_MESH_BED_LEVELING			| ???			| ff ffh 65535			| Power Panic Mesh Bed Leveling						| ???			| D3 Ax0f75 C18 
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| ^					| ^			| ^										| ???			| ^						| ^													| ^ 			| ^
| 0x0F73h 3955		| uint16	| EEPROM_UVLO_Z_MICROSTEPS				| ???			| ff ffh 65535			| Power Panic Z microsteps							| ???			| D3 Ax0f73 C2 
| 0x0F72h 3954		| uint8		| EEPROM_UVLO_E_ABS						| ???			| ffh 255				| Power Panic ??? position							| ???			| D3 Ax0f72 C1
| 0x0F6Eh 3950		| foat		| EEPROM_UVLO_CURRENT_POSITION_E		| ???			| ff ff ff ffh			| Power Panic E position							| ???			| D3 Ax0f6e C4
| 0x0F69h 3945		| uint8		| EEPROM_CRASH_DET						| ffh 255		| ffh 255				| Crash detection enabled							| LCD menu		| D3 Ax0f69 C5
| ^					| ^			| ^										| 00h 0			| ^						| Crash detection disabled							| LCD menu		| ^
| ^					| ^			| ^										| ???			| ???					| ^													| ??? 			| ^
| ^					| ^			| ^										| ???			| ???					| ^													| ??? 			| ^
| ^					| ^			| ^										| ???			| ???					| ^													| ??? 			| ^
| 0x0F68h 3944		| uint8		| EEPROM_CRASH_COUNT_Y					| 00h-ffh 0-255	| ffh 255				| Crashes detected on y axis						| ???			| D3 Ax0f68 C1
| 0x0F67h 3943		| uint8		| EEPROM_FSENSOR						| 01h 1			| ffh 255				| Filament sensor enabled							| LCD menu		| D3 Ax0f67 C1
| 0x0F65h 3942		| uint8		| EEPROM_CRASH_COUNT_X					| 00h-ffh 0-255	| ffh 255				| Crashes detected on x axis						| ???			| D3 Ax0f66 C1
| 0x0F65h 3941		| uint8		| EEPROM_FERROR_COUNT					| 00h-ffh 0-255	| ffh 255				| Filament sensor error counter						| ???			| D3 Ax0f65 C1
| 0x0F64h 3940		| uint8		| EEPROM_POWER_COUNT					| 00h-ffh 0-255	| ffh 255				| Power failure counter								| ???			| D3 Ax0f64 C1
| 0x0F60h 3936		| float		| EEPROM_XYZ_CAL_SKEW					| ???			| ff ff ff ffh			| XYZ skew value									| ???			| D3 Ax0f60 C4
| 0x0F5Fh 3935		| uint8		| EEPROM_WIZARD_ACTIVE					| 00h 0			| ???					| Wizard active										| ???			| D3 Ax0f5f C1
| ^					| ^			| ^										| 01h 1			| ???					| ^													| ^ 			| ^
| 0x0F5Dh 3933		| uint16	| EEPROM_BELTSTATUS_X					| ???			| ff ffh				| X Beltstatus 										| ???			| D3 Ax0f5d C2
| 0x0F5Bh 3931		| uint16	| EEPROM_BELTSTATUS_Y					| ???			| ff ffh				| Y Beltstatus 										| ???			| D3 Ax0f5b C2
| 0x0F5Ah 3930		| uint8		| EEPROM_DIR_DEPTH						| 00h-ffh 0-255	| ffh 255				| Directory depth									| ???			| D3 Ax0f5a C1
| 0x0F0Ah 3850		| uint8		| EEPROM_DIRS							| ???			| ffh 255				| Directories ???									| ???			| D3 Ax0f0a C80
| 0x0F09h 3849		| uint8		| EEPROM_SD_SORT						| 00h 0			| ffh 255				| SD card sort by time								| LCD menu		| D3 Ax0f09 C1
| ^					| ^			| ^										| 01h 1			| ^						| SD card sort by alphabet							| LCD menu		| ^
| ^					| ^			| ^										| 02h 1			| ^						| SD card not sorted 								| LCD menu		| ^
| 0x0F08h 3848		| uint8		| EEPROM_SECOND_SERIAL_ACTIVE			| 00h 0			| ffh 255				| RPi Port disabled									| LCD menu		| D3 Ax0f08 C1
| ^					| ^			| ^										| 01h 1			| ^						| RPi Port enabled									| LCD menu		| ^
| 0x0F07h 3847		| uint8		| EEPROM_FSENS_AUTOLOAD_ENABLED			| 01h 1			| ffh 255				| Filament autoload enabled							| LCD menu		| D3 Ax0f07 C1
| ^					| ^			| ^										| 00h 0			| ^						| Filament autoload disabled						| LCD menu		| ^
*/
#define EEPROM_EMPTY_VALUE 0xFF
#define EEPROM_EMPTY_VALUE16 0xFFFF
// The total size of the EEPROM is
// 4096 for the Atmega2560
#define EEPROM_TOP 4096
#define EEPROM_SILENT 4095
#define EEPROM_LANG 4094
#define EEPROM_BABYSTEP_X 4092 //unused
#define EEPROM_BABYSTEP_Y 4090 //unused
#define EEPROM_BABYSTEP_Z 4088 //legacy, multiple values stored now in EEPROM_Sheets_base
#define EEPROM_CALIBRATION_STATUS 4087
#define EEPROM_BABYSTEP_Z0 4085
#define EEPROM_FILAMENTUSED 4081
// uint32_t
#define EEPROM_TOTALTIME 4077

#define EEPROM_BED_CALIBRATION_CENTER     (EEPROM_TOTALTIME-2*4)
#define EEPROM_BED_CALIBRATION_VEC_X      (EEPROM_BED_CALIBRATION_CENTER-2*4)
#define EEPROM_BED_CALIBRATION_VEC_Y      (EEPROM_BED_CALIBRATION_VEC_X-2*4)

// Offsets of the Z heiths of the calibration points from the first point.
// The offsets are saved as 16bit signed int, scaled to tenths of microns.
#define EEPROM_BED_CALIBRATION_Z_JITTER   (EEPROM_BED_CALIBRATION_VEC_Y-2*8)
#define EEPROM_FARM_MODE (EEPROM_BED_CALIBRATION_Z_JITTER-1)
#define EEPROM_FARM_NUMBER (EEPROM_FARM_MODE-3)

// Correction of the bed leveling, in micrometers.
// Maximum 50 micrometers allowed.
// Bed correction is valid if set to 1. If set to zero or 255, the successive 4 bytes are invalid.
#define EEPROM_BED_CORRECTION_VALID (EEPROM_FARM_NUMBER-1)
#define EEPROM_BED_CORRECTION_LEFT  (EEPROM_BED_CORRECTION_VALID-1)
#define EEPROM_BED_CORRECTION_RIGHT (EEPROM_BED_CORRECTION_LEFT-1)
#define EEPROM_BED_CORRECTION_FRONT (EEPROM_BED_CORRECTION_RIGHT-1)
#define EEPROM_BED_CORRECTION_REAR  (EEPROM_BED_CORRECTION_FRONT-1)
#define EEPROM_TOSHIBA_FLASH_AIR_COMPATIBLITY (EEPROM_BED_CORRECTION_REAR-1)
#define EEPROM_PRINT_FLAG (EEPROM_TOSHIBA_FLASH_AIR_COMPATIBLITY-1)
#define EEPROM_PROBE_TEMP_SHIFT (EEPROM_PRINT_FLAG - 2*5) //5 x int for storing pinda probe temp shift relative to 50 C; unit: motor steps 
#define EEPROM_TEMP_CAL_ACTIVE (EEPROM_PROBE_TEMP_SHIFT - 1)
#define EEPROM_BOWDEN_LENGTH (EEPROM_TEMP_CAL_ACTIVE - 2*4) //4 x int for bowden lengths for multimaterial
#define EEPROM_CALIBRATION_STATUS_PINDA (EEPROM_BOWDEN_LENGTH - 1) //0 - not calibrated; 1 - calibrated
#define EEPROM_UVLO						(EEPROM_CALIBRATION_STATUS_PINDA - 1) //1 - uvlo during print
#define EEPROM_UVLO_CURRENT_POSITION	(EEPROM_UVLO-2*4) // 2 x float for current_position in X and Y axes
#define EEPROM_FILENAME (EEPROM_UVLO_CURRENT_POSITION - 8) //8chars to store filename without extension
#define EEPROM_FILE_POSITION (EEPROM_FILENAME - 4) //32 bit for uint32_t file position 
#define EEPROM_UVLO_CURRENT_POSITION_Z	(EEPROM_FILE_POSITION - 4) //float for current position in Z
#define EEPROM_UVLO_UNUSED_001		(EEPROM_UVLO_CURRENT_POSITION_Z - 1) // uint8_t (unused)
#define EEPROM_UVLO_TARGET_BED			(EEPROM_UVLO_UNUSED_001 - 1)
#define EEPROM_UVLO_FEEDRATE			(EEPROM_UVLO_TARGET_BED - 2) //uint16_t
#define EEPROM_UVLO_FAN_SPEED			(EEPROM_UVLO_FEEDRATE - 1) 
#define EEPROM_FAN_CHECK_ENABLED		(EEPROM_UVLO_FAN_SPEED - 1)
#define EEPROM_UVLO_MESH_BED_LEVELING     (EEPROM_FAN_CHECK_ENABLED - 9*2)

#define EEPROM_UVLO_Z_MICROSTEPS     (EEPROM_UVLO_MESH_BED_LEVELING - 2) // uint16_t (could be removed)
#define EEPROM_UVLO_E_ABS            (EEPROM_UVLO_Z_MICROSTEPS - 1)
#define EEPROM_UVLO_CURRENT_POSITION_E	(EEPROM_UVLO_E_ABS - 4)                 //float for current position in E

// Crash detection mode EEPROM setting 
#define EEPROM_CRASH_DET         (EEPROM_UVLO_CURRENT_POSITION_E - 5)           // float (orig EEPROM_UVLO_MESH_BED_LEVELING-12) 
// Crash detection counter Y (last print)
#define EEPROM_CRASH_COUNT_Y       (EEPROM_CRASH_DET - 1)                       // uint8 (orig EEPROM_UVLO_MESH_BED_LEVELING-15)
// Filament sensor on/off EEPROM setting 
#define EEPROM_FSENSOR           (EEPROM_CRASH_COUNT_Y - 1)                     // uint8 (orig EEPROM_UVLO_MESH_BED_LEVELING-14) 
// Crash detection counter X (last print)
#define EEPROM_CRASH_COUNT_X       (EEPROM_FSENSOR - 1)                         // uint8 (orig EEPROM_UVLO_MESH_BED_LEVELING-15)
// Filament runout/error coutner (last print)
#define EEPROM_FERROR_COUNT      (EEPROM_CRASH_COUNT_X - 1)                     // uint8 (orig EEPROM_UVLO_MESH_BED_LEVELING-16)
// Power loss errors (last print)
#define EEPROM_POWER_COUNT       (EEPROM_FERROR_COUNT - 1)                      // uint8 (orig EEPROM_UVLO_MESH_BED_LEVELING-17)

#define EEPROM_XYZ_CAL_SKEW (EEPROM_POWER_COUNT - 4)                            // float for skew backup
#define EEPROM_WIZARD_ACTIVE (EEPROM_XYZ_CAL_SKEW - 1)
#define EEPROM_BELTSTATUS_X (EEPROM_WIZARD_ACTIVE - 2)                          // uint16
#define EEPROM_BELTSTATUS_Y (EEPROM_BELTSTATUS_X - 2)                           // uint16

#define EEPROM_DIR_DEPTH        (EEPROM_BELTSTATUS_Y-1)
#define EEPROM_DIRS  (EEPROM_DIR_DEPTH-80) //8 chars for each dir name, max 10 levels
#define EEPROM_SD_SORT (EEPROM_DIRS - 1) //0 -time, 1-alpha, 2-none
#define EEPROM_SECOND_SERIAL_ACTIVE (EEPROM_SD_SORT - 1)

#define EEPROM_FSENS_AUTOLOAD_ENABLED (EEPROM_SECOND_SERIAL_ACTIVE - 1)

// Crash detection counter X (total)
#define EEPROM_CRASH_COUNT_X_TOT       (EEPROM_FSENS_AUTOLOAD_ENABLED - 2)     // uint16
// Crash detection counter Y (total)
#define EEPROM_CRASH_COUNT_Y_TOT       (EEPROM_CRASH_COUNT_X_TOT - 2)          // uint16
// Filament runout/error coutner (total)
#define EEPROM_FERROR_COUNT_TOT      (EEPROM_CRASH_COUNT_Y_TOT - 2)            // uint16
// Power loss errors (total)
#define EEPROM_POWER_COUNT_TOT       (EEPROM_FERROR_COUNT_TOT - 2)             // uint16

////////////////////////////////////////
// TMC2130 Accurate sensorless homing 

// X-axis home origin (stepper phase in microsteps, 0..63 for 16ustep resolution)
#define EEPROM_TMC2130_HOME_X_ORIGIN           (EEPROM_POWER_COUNT_TOT - 1)                    // uint8
// X-axis home bsteps (number of microsteps backward)
#define EEPROM_TMC2130_HOME_X_BSTEPS           (EEPROM_TMC2130_HOME_X_ORIGIN - 1)              // uint8
// X-axis home fsteps (number of microsteps forward)
#define EEPROM_TMC2130_HOME_X_FSTEPS           (EEPROM_TMC2130_HOME_X_BSTEPS - 1)              // uint8
// Y-axis home origin (stepper phase in microsteps, 0..63 for 16ustep resolution)
#define EEPROM_TMC2130_HOME_Y_ORIGIN           (EEPROM_TMC2130_HOME_X_FSTEPS - 1)              // uint8
// X-axis home bsteps (number of microsteps backward)
#define EEPROM_TMC2130_HOME_Y_BSTEPS           (EEPROM_TMC2130_HOME_Y_ORIGIN - 1)              // uint8
// X-axis home fsteps (number of microsteps forward)
#define EEPROM_TMC2130_HOME_Y_FSTEPS           (EEPROM_TMC2130_HOME_Y_BSTEPS - 1)              // uint8
// Accurate homing enabled
#define EEPROM_TMC2130_HOME_ENABLED            (EEPROM_TMC2130_HOME_Y_FSTEPS - 1)              // uint8


////////////////////////////////////////
// TMC2130 uStep linearity correction

// Linearity correction factor (XYZE)
#define EEPROM_TMC2130_WAVE_X_FAC              (EEPROM_TMC2130_HOME_ENABLED - 1)               // uint8
#define EEPROM_TMC2130_WAVE_Y_FAC              (EEPROM_TMC2130_WAVE_X_FAC - 1)                 // uint8
#define EEPROM_TMC2130_WAVE_Z_FAC              (EEPROM_TMC2130_WAVE_Y_FAC - 1)                 // uint8
#define EEPROM_TMC2130_WAVE_E_FAC              (EEPROM_TMC2130_WAVE_Z_FAC - 1)                 // uint8


////////////////////////////////////////
// TMC2130 uStep resolution

// microstep resolution (XYZE): usteps = (256 >> mres)
#define EEPROM_TMC2130_X_MRES              (EEPROM_TMC2130_WAVE_E_FAC - 1)                     // uint8
#define EEPROM_TMC2130_Y_MRES              (EEPROM_TMC2130_X_MRES - 1)                         // uint8
#define EEPROM_TMC2130_Z_MRES              (EEPROM_TMC2130_Y_MRES - 1)                         // uint8
#define EEPROM_TMC2130_E_MRES              (EEPROM_TMC2130_Z_MRES - 1)                         // uint8

// HW
#define EEPROM_PRINTER_TYPE          (EEPROM_TMC2130_E_MRES - 2)                               // uint16
#define EEPROM_BOARD_TYPE            (EEPROM_PRINTER_TYPE - 2)                                 // uint16

// Extruder multiplier for power panic
#define EEPROM_EXTRUDER_MULTIPLIER_0 (EEPROM_BOARD_TYPE - 4)                                   //float
#define EEPROM_EXTRUDER_MULTIPLIER_1 (EEPROM_EXTRUDER_MULTIPLIER_0 - 4)                        //float
#define EEPROM_EXTRUDER_MULTIPLIER_2 (EEPROM_EXTRUDER_MULTIPLIER_1 - 4)                        //float
#define EEPROM_EXTRUDEMULTIPLY (EEPROM_EXTRUDER_MULTIPLIER_2 - 2)                              // uint16

#define EEPROM_UVLO_TINY_CURRENT_POSITION_Z (EEPROM_EXTRUDEMULTIPLY-4) // float
#define EEPROM_UVLO_TARGET_HOTEND (EEPROM_UVLO_TINY_CURRENT_POSITION_Z-2) // uint16

// Sound Mode
#define EEPROM_SOUND_MODE (EEPROM_UVLO_TARGET_HOTEND-1) // uint8
#define EEPROM_AUTO_DEPLETE (EEPROM_SOUND_MODE-1) //bool

#define EEPROM_FSENS_OQ_MEASS_ENABLED (EEPROM_AUTO_DEPLETE - 1) //bool

#define EEPROM_MMU_FAIL_TOT (EEPROM_FSENS_OQ_MEASS_ENABLED - 2) //uint16_t
#define EEPROM_MMU_FAIL (EEPROM_MMU_FAIL_TOT - 1) //uint8_t

#define EEPROM_MMU_LOAD_FAIL_TOT (EEPROM_MMU_FAIL - 2) //uint16_t
#define EEPROM_MMU_LOAD_FAIL (EEPROM_MMU_LOAD_FAIL_TOT - 1) //uint8_t
#define EEPROM_MMU_CUTTER_ENABLED (EEPROM_MMU_LOAD_FAIL - 1)
#define EEPROM_UVLO_MESH_BED_LEVELING_FULL     (EEPROM_MMU_CUTTER_ENABLED - 12*12*2) //allow 12 calibration points for future expansion

#define EEPROM_MBL_TYPE	(EEPROM_UVLO_MESH_BED_LEVELING_FULL-1) //uint8_t for mesh bed leveling precision
#define EEPROM_MBL_MAGNET_ELIMINATION (EEPROM_MBL_TYPE -1)  
#define EEPROM_MBL_POINTS_NR (EEPROM_MBL_MAGNET_ELIMINATION -1) //uint8_t number of points in one exis for mesh bed leveling
#define EEPROM_MBL_PROBE_NR (EEPROM_MBL_POINTS_NR-1) //number of measurements for each point

#define EEPROM_MMU_STEALTH (EEPROM_MBL_PROBE_NR-1)

#define EEPROM_CHECK_MODE (EEPROM_MMU_STEALTH-1) // uint8
#define EEPROM_NOZZLE_DIAMETER (EEPROM_CHECK_MODE-1) // uint8
#define EEPROM_NOZZLE_DIAMETER_uM (EEPROM_NOZZLE_DIAMETER-2) // uint16
#define EEPROM_CHECK_MODEL (EEPROM_NOZZLE_DIAMETER_uM-1) // uint8
#define EEPROM_CHECK_VERSION (EEPROM_CHECK_MODEL-1) // uint8
#define EEPROM_CHECK_GCODE (EEPROM_CHECK_VERSION-1) // uint8

#define EEPROM_SHEETS_BASE (EEPROM_CHECK_GCODE - EEPROM_SHEETS_SIZEOF) // Sheets
static Sheets * const EEPROM_Sheets_base = (Sheets*)(EEPROM_SHEETS_BASE);

#define EEPROM_FSENSOR_PCB (EEPROM_SHEETS_BASE-1) // uint8
#define EEPROM_FSENSOR_ACTION_NA (EEPROM_FSENSOR_PCB-1) // uint8

#define EEPROM_UVLO_SAVED_TARGET (EEPROM_FSENSOR_ACTION_NA - 4*4) // 4 x float for saved target for all axes
#define EEPROM_UVLO_FEEDMULTIPLY (EEPROM_UVLO_SAVED_TARGET - 2) // uint16_t for feedmultiply

#define EEPROM_BACKLIGHT_LEVEL_HIGH (EEPROM_UVLO_FEEDMULTIPLY-1) // uint8
#define EEPROM_BACKLIGHT_LEVEL_LOW (EEPROM_BACKLIGHT_LEVEL_HIGH-1) // uint8
#define EEPROM_BACKLIGHT_MODE (EEPROM_BACKLIGHT_LEVEL_LOW-1) // uint8
#define EEPROM_BACKLIGHT_TIMEOUT (EEPROM_BACKLIGHT_MODE-2) // uint16

#define EEPROM_UVLO_LA_K (EEPROM_BACKLIGHT_TIMEOUT-4) // float

//This is supposed to point to last item to allow EEPROM overrun check. Please update when adding new items.
#define EEPROM_LAST_ITEM EEPROM_UVLO_LA_K
// !!!!!
// !!!!! this is end of EEPROM section ... all updates MUST BE inserted before this mark !!!!!
// !!!!!



// Currently running firmware, each digit stored as uint16_t.
// The flavor differentiates a dev, alpha, beta, release candidate or a release version.
#define EEPROM_FIRMWARE_VERSION_END       (FW_PRUSA3D_MAGIC_LEN+8)
#define EEPROM_FIRMWARE_VERSION_FLAVOR    (FW_PRUSA3D_MAGIC_LEN+6)
#define EEPROM_FIRMWARE_VERSION_REVISION  (FW_PRUSA3D_MAGIC_LEN+4)
#define EEPROM_FIRMWARE_VERSION_MINOR     (FW_PRUSA3D_MAGIC_LEN+2)
#define EEPROM_FIRMWARE_VERSION_MAJOR     FW_PRUSA3D_MAGIC_LEN
// Magic string, indicating that the current or the previous firmware running was the Prusa3D firmware.
#define EEPROM_FIRMWARE_PRUSA_MAGIC 0

#ifdef __cplusplus
#include "ConfigurationStore.h"
static_assert(EEPROM_FIRMWARE_VERSION_END < 20, "Firmware version EEPROM address conflicts with EEPROM_M500_base");
static constexpr M500_conf * const EEPROM_M500_base = reinterpret_cast<M500_conf*>(20); //offset for storing settings using M500
static_assert(((sizeof(M500_conf) + 20) < EEPROM_LAST_ITEM), "M500_conf address space conflicts with previous items.");
#endif

enum
{
    EEPROM_MMU_CUTTER_ENABLED_enabled = 1,
    EEPROM_MMU_CUTTER_ENABLED_always = 2,
};

#ifdef __cplusplus
void eeprom_init();
bool eeprom_is_sheet_initialized(uint8_t sheet_num);
struct SheetName
{
    char c[sizeof(Sheet::name) + 1];
};
void eeprom_default_sheet_name(uint8_t index, SheetName &sheetName);
int8_t eeprom_next_initialized_sheet(int8_t sheet);
void eeprom_switch_to_next_sheet();
#endif

#endif // EEPROM_H
