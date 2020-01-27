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
