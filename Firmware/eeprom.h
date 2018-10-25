#ifndef EEPROM_H
#define EEPROM_H

// The total size of the EEPROM is
// 4096 for the Atmega2560
#define EEPROM_TOP 4096
#define EEPROM_SILENT 4095
#define EEPROM_LANG 4094
#define EEPROM_BABYSTEP_X 4092
#define EEPROM_BABYSTEP_Y 4090
#define EEPROM_BABYSTEP_Z 4088
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
#define EEPROM_UVLO_TARGET_HOTEND		(EEPROM_UVLO_CURRENT_POSITION_Z - 1)
#define EEPROM_UVLO_TARGET_BED			(EEPROM_UVLO_TARGET_HOTEND - 1)
#define EEPROM_UVLO_FEEDRATE			(EEPROM_UVLO_TARGET_BED - 2)
#define EEPROM_UVLO_FAN_SPEED			(EEPROM_UVLO_FEEDRATE - 1) 
#define EEPROM_FAN_CHECK_ENABLED		(EEPROM_UVLO_FAN_SPEED - 1)
#define EEPROM_UVLO_MESH_BED_LEVELING     (EEPROM_FAN_CHECK_ENABLED - 9*2)

#define EEPROM_UVLO_Z_MICROSTEPS     (EEPROM_UVLO_MESH_BED_LEVELING - 2)
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

//
#define EEPROM_UVLO_TINY_CURRENT_POSITION_Z (EEPROM_EXTRUDEMULTIPLY-4) // float
#define EEPROM_UVLO_TINY_Z_MICROSTEPS (EEPROM_UVLO_TINY_CURRENT_POSITION_Z-2) // uint16

// Sound Mode
//#define EEPROM_SOUND_MODE (EEPROM_EXTRUDEMULTIPLY-1) // uint8
#define EEPROM_SOUND_MODE (EEPROM_UVLO_TINY_Z_MICROSTEPS-1) // uint8
#define EEPROM_AUTO_DEPLETE (EEPROM_SOUND_MODE-1) //bool

#define EEPROM_FSENS_OQ_MEASS_ENABLED (EEPROM_AUTO_DEPLETE - 1) //bool

// !!!!!
// !!!!! this is end of EEPROM section ... all updates MUST BE inserted before this mark !!!!!
// !!!!!

//TMC2130 configuration
#define EEPROM_TMC_AXIS_SIZE  //axis configuration block size
#define EEPROM_TMC_X (EEPROM_TMC + 0 * EEPROM_TMC_AXIS_SIZE) //X axis configuration blok
#define EEPROM_TMC_Y (EEPROM_TMC + 1 * EEPROM_TMC_AXIS_SIZE) //Y axis
#define EEPROM_TMC_Z (EEPROM_TMC + 2 * EEPROM_TMC_AXIS_SIZE) //Z axis
#define EEPROM_TMC_E (EEPROM_TMC + 3 * EEPROM_TMC_AXIS_SIZE) //E axis
//TMC2130 - X axis
#define EEPROM_TMC_X_USTEPS_INTPOL   (EEPROM_TMC_X +  0) // 1byte, bit 0..4 USTEPS, bit 7 INTPOL
#define EEPROM_TMC_X_PWM_AMPL        (EEPROM_TMC_X +  1) // 1byte (0..255)
#define EEPROM_TMC_X_PWM_GRAD_FREQ   (EEPROM_TMC_X +  2) // 1byte, bit 0..3 GRAD, bit 4..5 FREQ
#define EEPROM_TMC_X_TCOOLTHRS       (EEPROM_TMC_X +  3) // 2bytes (0..)
#define EEPROM_TMC_X_SG_THRS         (EEPROM_TMC_X +  5) // 1byte, (-64..+63)
#define EEPROM_TMC_X_CURRENT_H       (EEPROM_TMC_X +  6) // 1byte, (0..63)
#define EEPROM_TMC_X_CURRENT_R       (EEPROM_TMC_X +  7) // 1byte, (0..63)
#define EEPROM_TMC_X_HOME_SG_THRS    (EEPROM_TMC_X +  8) // 1byte, (-64..+63)
#define EEPROM_TMC_X_HOME_CURRENT_R  (EEPROM_TMC_X +  9) // 1byte, (-64..+63)
#define EEPROM_TMC_X_HOME_DTCOOLTHRS (EEPROM_TMC_X + 10) // 1byte (-128..+127)
#define EEPROM_TMC_X_DTCOOLTHRS_LOW  (EEPROM_TMC_X + 11) // 1byte (-128..+127)
#define EEPROM_TMC_X_DTCOOLTHRS_HIGH (EEPROM_TMC_X + 12) // 1byte (-128..+127)
#define EEPROM_TMC_X_SG_THRS_LOW     (EEPROM_TMC_X + 13) // 1byte, (-64..+63)
#define EEPROM_TMC_X_SG_THRS_HIGH    (EEPROM_TMC_X + 14) // 1byte, (-64..+63)

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
static M500_conf * const EEPROM_M500_base = reinterpret_cast<M500_conf*>(20); //offset for storing settings using M500
#endif


#endif // EEPROM_H
