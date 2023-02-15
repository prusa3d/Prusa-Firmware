
/**
 * @file
 * @author 3d-gussner
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
    uint8_t bed_temp; //!< 0 .. 254 [°C] NOTE: currently only written-to and never used
    uint8_t pinda_temp; //!< 0 .. 254 [°C] NOTE: currently only written-to and never used
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

  _Italic = unused or default_

  __Bold = Status__

  In Default/FactoryReset column the

   - __L__      Language
   - __S__      Statistics
   - __P__      Shipping prep
   - __M__      Service/Maintenance prep
   - __S/P__    Statistics and Shipping prep

  will overwrite existing values to 0 or default.
  A FactoryReset All Data will overwrite the whole EEPROM with ffh and some values will be initialized automatically,
  others need a reset / reboot.

  ---------------------------------------------------------------------------------
  How can you use the debug codes?
  - Serial terminal like Putty.
  - Octoprint does support D-codes
  - _Pronterface_ does __not__ support D-codes

  ### !!! D-codes are case sensitive so please don't use upper case A,C or X in the address you want to read !!!

  #### Useful tools/links:
  To convert hex to ascii       https://www.rapidtables.com/convert/number/hex-to-ascii.html

  To convert hex to dec         https://www.rapidtables.com/convert/number/hex-to-decimal.html

  Version: 1.0.2

  ---------------------------------------------------------------------------------


|Address begin|Bit/Type | Name                                  | Valid values | Default/FactoryReset  | Description                                       |Gcode/Function| Debug code
| :--         | :--     | :--                                   | :--:         | :--:                  | :--                                               | :--:         | :--:
| 0x0FFF 4095 | uchar   | EEPROM_SILENT                         | 00h 0        | ffh 255               | TMC Stealth mode: __off__ / miniRambo Power mode  | LCD menu     | D3 Ax0fff C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | TMC Stealth mode: __on__ / miniRambo Silent mode  | ^            | ^
| ^           | ^       | ^                                     | 02h 2        | ^                     | miniRambo Auto mode                               | ^            | ^
| 0x0FFE 4094 | uchar   | EEPROM_LANG                           | 00h 0        | ffh 255         __L__ | English / LANG_ID_PRI                             | LCD menu     | D3 Ax0ffe C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | Other language LANG_ID_SEC                        | ^            | ^
| 0x0FFC 4092 | uint16  | EEPROM_BABYSTEP_X                     | ???          | ff ffh 65535          | Babystep for X axis _unsued_                      | ???          | D3 Ax0ffc C2
| 0x0FFA 4090 | uint16  | EEPROM_BABYSTEP_Y                     | ???          | ff ffh 65535          | Babystep for Y axis _unsued_                      | ^            | D3 Ax0ffa C2
| 0x0FF8 4088 | uint16  | EEPROM_BABYSTEP_Z                     | ???          | ff ffh 65535          | Babystep for Z axis _lagacy_                      | ^            | D3 Ax0ff8 C2
| ^           | ^       | ^                                     | ^            | ^                     | multiple values stored now in EEPROM_Sheets_base  | ^            | ^
| 0x0FF7 4087 | uint8   | EEPROM_CALIBRATION_STATUS_V1          | ffh 255      | ffh 255               | Calibration status (<v3.12)                       | ???          | D3 Ax0ff7 C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | Calibrated                                        | ^            | ^
| ^           | ^       | ^                                     | e6h 230      | ^                     | needs Live Z adjustment                           | ^            | ^
| ^           | ^       | ^                                     | ebh 235      | ^                     | needs Temp Model calibration                      | ^            | ^
| ^           | ^       | ^                                     | f0h 240      | ^               __P__ | needs Z calibration                               | ^            | ^
| ^           | ^       | ^                                     | fah 250      | ^                     | needs XYZ calibration                             | ^            | ^
| ^           | ^       | ^                                     | 00h 0        | ^                     | Unknown (legacy)                                  | ^            | ^
| 0x0FF5 4085 | uint16  | EEPROM_BABYSTEP_Z0                    | ???          | ff ffh 65535          | Babystep for Z ???                                | ???          | D3 Ax0ff5 C2
| 0x0FF1 4081 | unint32 | EEPROM_FILAMENTUSED                   | ???          | 00 00 00 00h 0 __S/P__| Filament used in meters                           | ???          | D3 Ax0ff1 C4
| 0x0FED 4077 | unint32 | EEPROM_TOTALTIME                      | ???          | 00 00 00 00h 0 __S/P__| Total print time                                  | ???          | D3 Ax0fed C4
| 0x0FE5 4069 | float   | EEPROM_BED_CALIBRATION_CENTER         | ???          | ff ff ff ffh          | ???                                               | ???          | D3 Ax0fe5 C8
| 0x0FDD 4061 | float   | EEPROM_BED_CALIBRATION_VEC_X          | ???          | ff ff ff ffh          | ???                                               | ???          | D3 Ax0fdd C8
| 0x0FD5 4053 | float   | EEPROM_BED_CALIBRATION_VEC_Y          | ???          | ff ff ff ffh          | ???                                               | ???          | D3 Ax0fd5 C8
| 0x0FC5 4037 | int16   | EEPROM_BED_CALIBRATION_Z_JITTER       | ???          | ff ffh 65535          | ???                                               | ???          | D3 Ax0fc5 C16
| 0x0FC4 4036 | bool    | EEPROM_FARM_MODE                      | 00h 0        | ffh 255         __P__ | Prusa farm mode: __off__                          | G99          | D3 Ax0fc4 C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | Prusa farm mode: __on__                           | G98          | ^
| 0x0FC3 4035 | free    | _EEPROM_FREE_NR1_                     | ???          | ffh 255               | _Free EEPROM space_                               | _free space_ | D3 Ax0fc3 C1
| 0x0FC1 4033 | ???     | EEPROM_FARM_NUMBER                    | 000-999      | ff ffh / 000    __P__ | Prusa farm number _only 0-9 are allowed: 000-999_ | LCD menu     | D3 Ax0fc1 C2
| 0x0FC0 4032 | bool    | EEPROM_BED_CORRECTION_VALID           | 00h 0        | 00h 0                 | Bed correction invalid                            | ???          | D3 Ax0fc0 C1
| ^           | ^       | ^                                     | ffh 255      | ^                     | Bed correction valid                              | ???          | ^
| 0x0FBF 4031 | char    | EEPROM_BED_CORRECTION_LEFT            | 00h ffh      | 00h 0                 | Bed manual correction left                        | LCD menu     | D3 Ax0fbf C1
| ^           | ^       | ^                                     | ^            | ^                     | At this moment limited to +-100um                 | G80 Lxxx     | ^
| 0x0FBE 4030 | char    | EEPROM_BED_CORRECTION_RIGHT           | 00h ffh      | 00h 0                 | Bed manual correction right                       | LCD menu     | D3 Ax0fbe C1
| ^           | ^       | ^                                     | ^            | ^                     | At this moment limited to +-100um                 | G80 Rxxx     | ^
| 0x0FBD 4029 | char    | EEPROM_BED_CORRECTION_FRONT           | 00h ffh      | 00h 0                 | Bed manual correction front                       | LCD menu     | D3 Ax0fbd C1
| ^           | ^       | ^                                     | ^            | ^                     | At this moment limited to +-100um                 | G80 Fxxx     | ^
| 0x0FBC 4028 | char    | EEPROM_BED_CORRECTION_BACK            | 00h ffh      | 00h 0                 | Bed manual correction back                        | LCD menu     | D3 Ax0fbc C1
| ^           | ^       | ^                                     | ^            | ^                     | At this moment limited to +-100um                 | G80 Bxxx     | ^
| 0x0FBB 4027 | bool    | EEPROM_TOSHIBA_FLASH_AIR_COMPATIBLITY | 00h 0        | ffh 255               | Toshiba Air: __off__                              | LCD menu     | D3 Ax0fbb C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | Toshiba Air: __on__                               | ^            | ^
| 0x0FBA 4026 | uchar   | EEPROM_PRINT_FLAG                     | ???          | ???                   | _unsued_                                          | ???          | D3 Ax0fba C1
| 0x0FB0 4016 | int16   | EEPROM_PROBE_TEMP_SHIFT               | ???          | ???                   | ???                                               | ???          | D3 Ax0fb0 C10
| 0x0FAF 4015 | bool    | EEPROM_TEMP_CAL_ACTIVE                | 00h 0        | 00h 0                 | PINDA Temp cal.: __inactive__                     | LCD menu     | D3 Ax0faf C1
| ^           | ^       | ^                                     | ffh 255      | ^                     | PINDA Temp cal.: __active__                       | ^            | ^
| 0x0FA7 4007 | ???     | _EEPROM_FREE_NR6_                     | ???          | ffh 255               | _Free EEPROM space_                               | ???          | D3 Ax0fae C8
| ^           | ^       | ^                                     | ^            | 00 00 00 00h          | ^                                                 | ^            | ^
| 0x0FA6 4006 | uint8   | EEPROM_CALIBRATION_STATUS_PINDA       | 00h 0        | ffh 255               | PINDA Temp: __not calibrated__                    | ???          | D3 Ax0fa6 C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | PINDA Temp: __calibrated__                        | ^            | ^
| 0x0FA5 4005 | uint8   | EEPROM_UVLO                           | 00h 0        | ffh 255               | Power Panic flag: __inactive__                    | ???          | D3 Ax0fa5 C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | Power Panic flag: __active__                      | ^            | ^
| ^           | ^       | ^                                     | 02h 2        | ^                     | Power Panic flag: __???__                         | ^            | ^
| 0x0F9D 3997 | float   | EEPROM_UVLO_CURRENT_POSITION          | ???          | ffh 255               | Power Panic position                              | ???          | D3 Ax0f9d C8
| 0x0F95 3989 | char    | EEPROM_FILENAME                       | ???          | ffh 255               | Power Panic Filename                              | ???          | D3 Ax0f95 C8
| 0x0F91 3985 | unint32 | EEPROM_FILE_POSITION                  | ???          | ff ff ff ffh          | Power Panic File Position                         | ???          | D3 Ax0f91 C4
| 0x0F8D 3981 | float   | EEPROM_UVLO_CURRENT_POSITION_Z        | ???          | ff ff ff ffh          | Power Panic Z Position                            | ^            | D3 Ax0f8d C4
| 0x0F8C 3980 | ???     | EEPROM_UVLO_UNUSED_001                | ???          | ffh 255               | Power Panic _unused_                              | ^            | D3 Ax0f8c C1
| 0x0F8B 3979 | uint8   | EEPROM_UVLO_TARGET_BED                | ???          | ffh 255               | Power Panic Bed temperature                       | ^            | D3 Ax0f8b C1
| 0x0F89 3977 | uint16  | EEPROM_UVLO_FEEDRATE                  | ???          | ff ffh 65535          | Power Panic Feedrate                              | ^            | D3 Ax0f89 C2
| 0x0F88 3976 | uint8   | EEPROM_UVLO_FAN_SPEED                 | ???          | ffh 255               | Power Panic Fan speed                             | ^            | D3 Ax0f88 C1
| 0x0F87 3975 | uint8   | EEPROM_FAN_CHECK_ENABLED              | 00h 0        | ???                   | Fan Check __disabled__                            | LCD menu     | D3 Ax0f87 C1
| ^           | ^       | ^                                     | 01h 1        | ffh 255               | Fan Check __enabled__                             | ^            | ^
| 0x0F75 3957 | uint16  | EEPROM_UVLO_MESH_BED_LEVELING         | ???          | ff ffh 65535          | Power Panic Mesh Bed Leveling                     | ???          | D3 Ax0f75 C18
| 0x0F73 3955 | uint16  | EEPROM_UVLO_Z_MICROSTEPS              | ???          | ff ffh 65535          | Power Panic Z microsteps                          | ???          | D3 Ax0f73 C2
| 0x0F72 3954 | uint8   | EEPROM_UVLO_E_ABS                     | ???          | ffh 255               | Power Panic ??? position                          | ???          | D3 Ax0f72 C1
| 0x0F6E 3950 | float   | EEPROM_UVLO_CURRENT_POSITION_E        | ???          | ff ff ff ffh          | Power Panic E position                            | ???          | D3 Ax0f6e C4
| 0x0F6C 3948 | uint16  | EEPROM_UVLO_SAVED_SEGMENT_IDX         | all          | ff ffh 65535          | Power Panic index of multi-segment move           | ???          | D3 Ax0f6c C2
| 0x0F6B 3947 | ???     | _EEPROM_FREE_NR4_                     | ???          | ffh 255               | _Free EEPROM space_                               | _free space_ | D3 Ax0f6b C1
| 0x0F6A 3946 | ???     | _EEPROM_FREE_NR5_                     | ???          | ffh 255               | _Free EEPROM space_                               | _free space_ | D3 Ax0f6a C1
| 0x0F69 3945 | uint8   | EEPROM_CRASH_DET                      | ffh 255      | ffh 255               | Crash detection: __enabled__                      | LCD menu     | D3 Ax0f69 C1
| ^           | ^       | ^                                     | 00h 0        | ^                     | Crash detection: __disabled__                     | LCD menu     | ^
| 0x0F68 3944 | uint8   | EEPROM_CRASH_COUNT_Y                  | 00h-ffh 0-255| ffh 255       __S/P__ | Crashes detected on y axis                        | ???          | D3 Ax0f68 C1
| 0x0F67 3943 | uint8   | EEPROM_FSENSOR                        | 01h 1        | ffh 255         __P__ | Filament sensor: __enabled__                      | LCD menu     | D3 Ax0f67 C1
| ^           | ^       | ^                                     | 00h 0        | ^                     | Filament sensor: __disabled__                     | LCD menu     | ^
| 0x0F65 3942 | uint8   | EEPROM_CRASH_COUNT_X                  | 00h-ffh 0-255| ffh 255       __S/P__ | Crashes detected on x axis                        | ???          | D3 Ax0f66 C1
| 0x0F65 3941 | uint8   | EEPROM_FERROR_COUNT                   | 00h-ffh 0-255| ffh 255       __S/P__ | Filament sensor error counter                     | ???          | D3 Ax0f65 C1
| 0x0F64 3940 | uint8   | EEPROM_POWER_COUNT                    | 00h-ffh 0-255| ffh 255       __S/P__ | Power failure counter                             | ???          | D3 Ax0f64 C1
| 0x0F60 3936 | float   | EEPROM_XYZ_CAL_SKEW                   | ???          | ff ff ff ffh          | XYZ skew value                                    | ???          | D3 Ax0f60 C4
| 0x0F5F 3935 | uint8   | EEPROM_WIZARD_ACTIVE                  | 01h 1        | 01h 1           __P__ | Wizard __active__                                 | ???          | D3 Ax0f5f C1
| ^           | ^       | ^                                     | 00h 0        | ^                     | Wizard __inactive__                               | ^            | ^
| ^           | ^       | ^                                     | 02h 2        | 02h 2           __M__ | Wizard active - Z cal after shipping/service prep | ^            | ^
| 0x0F5D 3933 | uint16  | EEPROM_BELTSTATUS_X                   | ???          | ff ffh                | X Beltstatus                                      | ???          | D3 Ax0f5d C2
| 0x0F5B 3931 | uint16  | EEPROM_BELTSTATUS_Y                   | ???          | ff ffh                | Y Beltstatus                                      | ???          | D3 Ax0f5b C2
| 0x0F5A 3930 | uint8   | EEPROM_DIR_DEPTH                      | 00h-ffh 0-255| ffh 255               | Directory depth                                   | ???          | D3 Ax0f5a C1
| 0x0F0A 3850 | uint8   | EEPROM_DIRS                           | ???          | ffh 255               | Directories ???                                   | ???          | D3 Ax0f0a C80
| 0x0F09 3849 | uint8   | EEPROM_SD_SORT                        | 00h 0        | ffh 255               | SD card sort by:     __time__                     | LCD menu     | D3 Ax0f09 C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | SD card sort by:     __alphabet__                 | LCD menu     | ^
| ^           | ^       | ^                                     | 02h 2        | ^                     | SD card:            __not sorted__                | LCD menu     | ^
| 0x0F08 3848 | uint8   | EEPROM_SECOND_SERIAL_ACTIVE           | 00h 0        | ffh 255               | RPi Port: __disabled__                            | LCD menu     | D3 Ax0f08 C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | RPi Port: __enabled__                             | LCD menu     | ^
| 0x0F07 3847 | uint8   | EEPROM_FSENS_AUTOLOAD_ENABLED         | 01h 1        | ffh 255         __P__ | Filament autoload: __enabled__                    | LCD menu     | D3 Ax0f07 C1
| ^           | ^       | ^                                     | 00h 0        | ^                     | Filament autoload: __disabled__                   | LCD menu     | ^
| 0x0F05 3845 | uint16  | EEPROM_CRASH_COUNT_X_TOT              | 0000-fffe    | ff ffh        __S/P__ | Total crashes on x axis                           | ???          | D3 Ax0f05 C2
| 0x0F03 3843 | uint16  | EEPROM_CRASH_COUNT_Y_TOT              | 0000-fffe    | ff ffh        __S/P__ | Total crashes on y axis                           | ???          | D3 Ax0f03 C2
| 0x0F01 3841 | uint16  | EEPROM_FERROR_COUNT_TOT               | 0000-fffe    | ff ffh        __S/P__ | Total filament sensor errors                      | ???          | D3 Ax0f01 C2
| 0x0EFF 3839 | uint16  | EEPROM_POWER_COUNT_TOT                | 0000-fffe    | ff ffh        __S/P__ | Total power failures                              | ???          | D3 Ax0eff C2
| 0x0EFE 3838 | uint8   | EEPROM_TMC2130_HOME_X_ORIGIN          | ???          | ffh 255               | ???                                               | ???          | D3 Ax0efe C1
| 0x0EFD 3837 | uint8   | EEPROM_TMC2130_HOME_X_BSTEPS          | ???          | ffh 255               | ???                                               | ???          | D3 Ax0efd C1
| 0x0EFC 3836 | uint8   | EEPROM_TMC2130_HOME_X_FSTEPS          | ???          | ffh 255               | ???                                               | ???          | D3 Ax0efc C1
| 0x0EFB 3835 | uint8   | EEPROM_TMC2130_HOME_Y_ORIGIN          | ???          | ffh 255               | ???                                               | ???          | D3 Ax0efb C1
| 0x0EFA 3834 | uint8   | EEPROM_TMC2130_HOME_Y_BSTEPS          | ???          | ffh 255               | ???                                               | ???          | D3 Ax0efa C1
| 0x0EF9 3833 | uint8   | EEPROM_TMC2130_HOME_Y_FSTEPS          | ???          | ffh 255               | ???                                               | ???          | D3 Ax0ef9 C1
| 0x0EF8 3832 | uint8   | EEPROM_TMC2130_HOME_ENABLED           | ???          | ffh 255               | ???                                               | ???          | D3 Ax0ef8 C1
| 0x0EF7 3831 | uint8   | EEPROM_TMC2130_WAVE_X_FAC             | ???          | ffh 255               | ???                                               | ???          | D3 Ax0ef7 C1
| 0x0EF6 3830 | uint8   | EEPROM_TMC2130_WAVE_Y_FAC             | ???          | ffh 255               | ???                                               | ???          | D3 Ax0ef6 C1
| 0x0EF5 3829 | uint8   | EEPROM_TMC2130_WAVE_Z_FAC             | ???          | ffh 255               | ???                                               | ???          | D3 Ax0ef5 C1
| 0x0EF4 3828 | uint8   | EEPROM_TMC2130_WAVE_E_FAC             | ???          | ffh 255               | ???                                               | ???          | D3 Ax0ef4 C1
| 0x0EF3 3827 | uint8   | EEPROM_TMC2130_X_MRES                 | ???          | ffh 255               | ???                                               | ???          | D3 Ax0ef3 C1
| 0x0EF2 3826 | uint8   | EEPROM_TMC2130_Y_MRES                 | ???          | ffh 255               | ???                                               | ???          | D3 Ax0ef2 C1
| 0x0EF1 3825 | uint8   | EEPROM_TMC2130_Z_MRES                 | ???          | ffh 255               | ???                                               | ???          | D3 Ax0ef1 C1
| 0x0EF0 3824 | uint8   | EEPROM_TMC2130_E_MRES                 | ???          | ffh 255               | ???                                               | ???          | D3 Ax0ef0 C1
| 0x0EEE 3822 | uint16  | EEPROM_PRINTER_TYPE                   | ???          | ff ffh 65535          | Printer Type                                      | ???          | D3 Ax0eee C2
| ^           | ^       | ^                                     | 64 00h 100   | ^                     | PRINTER_MK1                                       | ???          | ^
| ^           | ^       | ^                                     | c8 00h 200   | ^                     | PRINTER_MK2                                       | ???          | ^
| ^           | ^       | ^                                     | c9 00h 201   | ^                     | PRINTER_MK2 with MMU1                             | ???          | ^
| ^           | ^       | ^                                     | ca 00h 202   | ^                     | PRINTER_MK2S                                      | ???          | ^
| ^           | ^       | ^                                     | cb 00h 203   | ^                     | PRINTER_MK2S with MMU1                            | ???          | ^
| ^           | ^       | ^                                     | fa 00h 250   | ^                     | PRINTER_MK2.5                                     | ???          | ^
| ^           | ^       | ^                                     | 1a 4fh 20250 | ^                     | PRINTER_MK2.5 with MMU2                           | ???          | ^
| ^           | ^       | ^                                     | fc 00h 252   | ^                     | PRINTER_MK2.5S                                    | ???          | ^
| ^           | ^       | ^                                     | 1c 4fh 20252 | ^                     | PRINTER_MK2.5S with MMU2S                         | ???          | ^
| ^           | ^       | ^                                     | 2c 01h 300   | ^                     | PRINTER_MK3                                       | ???          | ^
| ^           | ^       | ^                                     | 4c 4fh 20300 | ^                     | PRINTER_MK3 with MMU2                             | ???          | ^
| ^           | ^       | ^                                     | 2e 01h 302   | ^                     | PRINTER_MK3S                                      | ???          | ^
| ^           | ^       | ^                                     | 4e 4fh 20302 | ^                     | PRINTER_MK3S with MMU2S                           | ???          | ^
| 0x0EEC 3820 | uint16  | EEPROM_BOARD_TYPE                     | ???          | ff ffh 65535          | Board Type                                        | ???          | D3 Ax0eec C2
| ^           | ^       | ^                                     | c8 00h 200   | ^                     | BOARD_RAMBO_MINI_1_0                              | ???          | ^
| ^           | ^       | ^                                     | cb 00h 203   | ^                     | BOARD_RAMBO_MINI_1_3                              | ???          | ^
| ^           | ^       | ^                                     | 36 01h 310   | ^                     | BOARD_EINSY_1_0a                                  | ???          | ^
| 0x0EE8 3816 | float   | EEPROM_EXTRUDER_MULTIPLIER_0          | ???          | ff ff ff ffh          | Power panic Extruder 0 multiplier                 | ???          | D3 Ax0ee8 C4
| 0x0EE4 3812 | float   | EEPROM_EXTRUDER_MULTIPLIER_1          | ???          | ff ff ff ffh          | Power panic Extruder 1 multiplier                 | ???          | D3 Ax0ee4 C4
| 0x0EE0 3808 | float   | EEPROM_EXTRUDER_MULTIPLIER_2          | ???          | ff ff ff ffh          | Power panic Extruder 2 multiplier                 | ???          | D3 Ax0ee0 C4
| 0x0EDE 3806 | uint16  | EEPROM_EXTRUDEMULTIPLY                | ???          | ff ffh 65535          | Power panic Extruder multiplier                   | ???          | D3 Ax0ede C2
| 0x0EDA 3802 | float   | EEPROM_UVLO_TINY_CURRENT_POSITION_Z   | ???          | ff ff ff ffh          | Power panic Z position                            | ???          | D3 Ax0eda C4
| 0x0ED8 3800 | uint16  | EEPROM_UVLO_TARGET_HOTEND             | ???          | ff ffh 65535          | Power panic target Hotend temperature             | ???          | D3 Ax0ed8 C2
| 0x0ED7 3799 | uint8   | EEPROM_SOUND_MODE                     | 00h 0        | ffh 255               | Sound mode: __loud__                              | ???          | D3 Ax0ed7 C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | Sound mode: __once__                              | ^            | ^
| ^           | ^       | ^                                     | 02h 2        | ^                     | Sound mode: __silent__                            | ^            | ^
| ^           | ^       | ^                                     | 03h 3        | ^                     | Sound mode: __assist__                            | ^            | ^
| 0x0ED6 3798 | bool    | EEPROM_SPOOL_JOIN                     | 01h 1        | ffh 255               | MMU2/s autodeplete: __on__                        | ???          | D3 Ax0ed6 C1
| ^           | ^       | ^                                     | 00h 0        | ^                     | MMU2/s autodeplete: __off__                       | ^            | ^
| 0x0ED5 3797 | bool    | EEPROM_FSENS_RUNOUT_ENABLED           | 01h 1        | ffh 255         __P__ | Filament runout: __enabled__                      | LCD menu     | D3 Ax0ed5 C1
| ^           | ^       | ^                                     | 00h 0        | ^                     | Filament runout: __disabled__                     | LCD menu     | ^
| 0x0ED3 3795 | uint16  | EEPROM_MMU_FAIL_TOT                   | ???          | ff ffh 65535  __S/P__ | MMU2/s total failures                             | ???          | D3 Ax0ed3 C2
| 0x0ED2 3794 | uint8   | EEPROM_MMU_FAIL                       | ???          | ffh 255       __S/P__ | MMU2/s fails during print                         | ???          | D3 Ax0ed2 C1
| 0x0ED0 3792 | uint16  | EEPROM_MMU_LOAD_FAIL_TOT              | ???          | ff ffh 65535  __S/P__ | MMU2/s total load failures                        | ???          | D3 Ax0ed0 C2
| 0x0ECF 3791 | uint8   | EEPROM_MMU_LOAD_FAIL                  | ???          | ffh 255       __S/P__ | MMU2/s load failures during print                 | ???          | D3 Ax0ecf C1
| 0x0ECE 3790 | uint8   | EEPROM_MMU_CUTTER_ENABLED             | 00h 0        | ffh 255               | MMU2/s cutter: __disabled__                       | LCD menu     | D3 Ax0ece C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | MMU2/s cutter: __enabled__                        | ^            | ^
| ^           | ^       | ^                                     | 02h 2        | ^                     | MMU2/s cutter: __always__                         | ^            | ^
| 0x0DAE 3502 | uint16  | EEPROM_UVLO_MESH_BED_LEVELING_FULL    | ???          | ff ffh 65535          | Power panic Mesh bed leveling points              | ???          | D3 Ax0dae C288
| 0x0DAD 3501 | uint8   | EEPROM_MBL_TYPE                       | ???          | ffh 255               | Mesh bed leveling precision _unused atm_          | ???          | D3 Ax0dad C1
| 0x0DAC 3500 | bool    | EEPROM_MBL_MAGNET_ELIMINATION         | 01h 1        | ffh 255               | Mesh bed leveling does: __ignores__ magnets       | LCD menu     | D3 Ax0dac C1
| ^           | ^       | ^                                     | 00h 0        | ^                     | Mesh bed leveling does: __NOT ignores__ magnets   | ^            | ^
| 0x0DAB 3499 | uint8   | EEPROM_MBL_POINTS_NR                  | 03h 3        | ffh 255               | Mesh bed leveling points: __3x3__                 | LCD menu     | D3 Ax0dab C1
| ^           | ^       | ^                                     | 07h 7        | ^                     | Mesh bed leveling points: __7x7__                 | ^            | ^
| 0x0DAA 3498 | uint8   | EEPROM_MBL_PROBE_NR                   | 03h 3        | ffh 255               | MBL times measurements for each point: __3__      | LCD menu     | D3 Ax0daa C1
| ^           | ^       | ^                                     | 05h 5        | ^                     | MBL times measurements for each point: __5__      | ^            | ^
| ^           | ^       | ^                                     | 01h 1        | ^                     | MBL times measurements for each point: __1__      | ^            | ^
| 0x0DA9 3497 | uint8   | EEPROM_MMU_STEALTH                    | 01h 1        | ffh 255               | MMU2/s Silent mode: __on__                        | ???          | D3 Ax0da9 C1
| ^           | ^       | ^                                     | 00h 0        | ^                     | MMU2/s Silent mode: __off__                       | ^            | ^
| 0x0DA8 3496 | uint8   | EEPROM_CHECK_MODE                     | 01h 1        | ffh 255               | Check mode for nozzle is: __warn__                | LCD menu     | D3 Ax0da8 C1
| ^           | ^       | ^                                     | 02h 2        | ^                     | Check mode for nozzle is: __strict__              | ^            | ^
| ^           | ^       | ^                                     | 00h 0        | ^                     | Check mode for nozzle is: __none__                | ^            | ^
| 0x0DA7 3495 | uint8   | EEPROM_NOZZLE_DIAMETER                | 28h 40       | ffh 255               | Nozzle diameter is: __40 or 0.40mm__              | LCD menu     | D3 Ax0da7 C1
| ^           | ^       | ^                                     | 3ch 60       | ^                     | Nozzle diameter is: __60 or 0.60mm__              | ^            | ^
| ^           | ^       | ^                                     | 3ch 80       | ^                     | Nozzle diameter is: __80 or 0.80mm__              | ^            | ^
| ^           | ^       | ^                                     | 19h 25       | ^                     | Nozzle diameter is: __25 or 0.25mm__              | ^            | ^
| 0x0DA5 3493 | uint16  | EEPROM_NOZZLE_DIAMETER_uM             | 9001h        | ff ffh 65535          | Nozzle diameter is: __400um__                     | LCD menu     | D3 Ax0da5 C2
| ^           | ^       | ^                                     | 5802h        | ^                     | Nozzle diameter is: __600um__                     | ^            | ^
| ^           | ^       | ^                                     | 2003h        | ^                     | Nozzle diameter is: __800um__                     | ^            | ^
| ^           | ^       | ^                                     | fa00h        | ^                     | Nozzle diameter is: __250um__                     | ^            | ^
| 0x0DA4 3492 | uint8   | EEPROM_CHECK_MODEL                    | 01h 1        | ffh 255               | Check mode for printer model is: __warn__         | LCD menu     | D3 Ax0da4 C1
| ^           | ^       | ^                                     | 02h 2        | ^                     | Check mode for printer model is: __strict__       | ^            | ^
| ^           | ^       | ^                                     | 00h 0        | ^                     | Check mode for printer model is: __none__         | ^            | ^
| 0x0DA3 3491 | uint8   | EEPROM_CHECK_VERSION                  | 01h 1        | ffh 255               | Check mode for firmware is: __warn__              | LCD menu     | D3 Ax0da3 C1
| ^           | ^       | ^                                     | 02h 2        | ^                     | Check mode for firmware is: __strict__            | ^            | ^
| ^           | ^       | ^                                     | 00h 0        | ^                     | Check mode for firmware is: __none__              | ^            | ^
| 0x0DA2 3490 | uint8   | EEPROM_CHECK_GCODE                    | 01h 1        | ffh 255               | Check mode for gcode is: __warn__ _unused atm_    | LCD menu     | D3 Ax0da2 C1
| ^           | ^       | ^                                     | 02h 2        | ^                     | Check mode for gcode is: __strict__ _unused atm_  | ^            | ^
| ^           | ^       | ^                                     | 00h 0        | ^                     | Check mode for gcode is: __none__ _unused atm_    | ^            | ^
| 0x0D49 3401 | uint16  | EEPROM_SHEETS_BASE                    | ???          | ffh 255               | ???                                               | LCD menu     | D3 Ax0d49 C89
| 0x0D49 3401 | char    | _1st Sheet block_                     |536d6f6f746831| ffffffffffffff        | 1st sheet - Name: _Smooth1_                       | ^            | D3 Ax0d49 C7
| 0x0D50 3408 | uint16  | ^                                     | 00 00h 0     | ff ffh 65535          | 1st sheet - Z offset                              | ^            | D3 Ax0d50 C2
| 0x0D52 3410 | uint8   | ^                                     | 00h 0        | ffh 255               | 1st sheet - bed temp                              | ^            | D3 Ax0d52 C1
| 0x0D53 3411 | uint8   | ^                                     | 00h 0        | ffh 255               | 1st sheet - PINDA temp                            | ^            | D3 Ax0d53 C1
| 0x0D54 3412 | char    | _2nd Sheet block_                     |536d6f6f746832| ffffffffffffff        | 2nd sheet - Name: _Smooth2_                       | ^            | D3 Ax0d54 C7
| 0x0D5B 3419 | uint16  | ^                                     | 00 00h 0     | ff ffh 65535          | 2nd sheet - Z offset                              | ^            | D3 Ax0d5b C2
| 0x0D5D 3421 | uint8   | ^                                     | 00h 0        | ffh 255               | 2nd sheet - bed temp                              | ^            | D3 Ax0d5d C1
| 0x0D5E 3422 | uint8   | ^                                     | 00h 0        | ffh 255               | 2nd sheet - PINDA temp                            | ^            | D3 Ax0d5e C1
| 0x0D5F 3423 | char    | _3rd Sheet block_                     |54657874757231| ffffffffffffff        | 3rd sheet - Name: _Textur1_                       | ^            | D3 Ax0d5f C7
| 0x0D66 3430 | uint16  | ^                                     | 00 00h 0     | ff ffh 65535          | 3rd sheet - Z offset                              | ^            | D3 Ax0d66 C2
| 0x0D68 3432 | uint8   | ^                                     | 00h 0        | ffh 255               | 3rd sheet - bed temp                              | ^            | D3 Ax0d68 C1
| 0x0D69 3433 | uint8   | ^                                     | 00h 0        | ffh 255               | 3rd sheet - PINDA temp                            | ^            | D3 Ax0d69 C1
| 0x0D6A 3434 | char    | _4th Sheet block_                     |54657874757232| ffffffffffffff        | 4th sheet - Name: _Textur2_                       | ^            | D3 Ax0d6a C7
| 0x0D71 3441 | uint16  | ^                                     | 00 00h 0     | ff ffh 65535          | 4th sheet - Z offset                              | ^            | D3 Ax0d71 C2
| 0x0D73 3443 | uint8   | ^                                     | 00h 0        | ffh 255               | 4th sheet - bed temp                              | ^            | D3 Ax0d73 C1
| 0x0D74 3444 | uint8   | ^                                     | 00h 0        | ffh 255               | 4th sheet - PINDA temp                            | ^            | D3 Ax0d74 C1
| 0x0D75 3445 | char    | _5th Sheet block_                     |536174696e2020| ffffffffffffff        | 5th sheet - Name: _Satin  _                       | ^            | D3 Ax0d75 C7
| 0x0D7C 3452 | uint16  | ^                                     | 00 00h 0     | ff ffh 65535          | 5th sheet - Z offset                              | ^            | D3 Ax0d7c C2
| 0x0D7E 3454 | uint8   | ^                                     | 00h 0        | ffh 255               | 5th sheet - bed temp                              | ^            | D3 Ax0d7e C1
| 0x0D7F 3455 | uint8   | ^                                     | 00h 0        | ffh 255               | 5th sheet - PINDA temp                            | ^            | D3 Ax0d7f C1
| 0x0D80 3456 | char    | _6th Sheet block_                     |4e796c6f6e5041| ffffffffffffff        | 6th sheet - Name: _NylonPA_                       | ^            | D3 Ax0d80 C7
| 0x0D87 3463 | uint16  | ^                                     | 00 00h 0     | ff ffh 65535          | 6th sheet - Z offset                              | ^            | D3 Ax0d87 C2
| 0x0D89 3465 | uint8   | ^                                     | 00h 0        | ffh 255               | 6th sheet - bed temp                              | ^            | D3 Ax0d89 C1
| 0x0D8A 3466 | uint8   | ^                                     | 00h 0        | ffh 255               | 6th sheet - PINDA temp                            | ^            | D3 Ax0d8a C1
| 0x0D8B 3467 | char    | _7th Sheet block_                     |437573746f6d31| ffffffffffffff        | 7th sheet - Name: _Custom1_                       | ^            | D3 Ax0d8b C7
| 0x0D92 3474 | uint16  | ^                                     | 00 00h 0     | ff ffh 65535          | 7th sheet - Z offset                              | ^            | D3 Ax0d92 C2
| 0x0D94 3476 | uint8   | ^                                     | 00h 0        | ffh 255               | 7th sheet - bed temp                              | ^            | D3 Ax0d94 C1
| 0x0D95 3477 | uint8   | ^                                     | 00h 0        | ffh 255               | 7th sheet - PINDA temp                            | ^            | D3 Ax0d95 C1
| 0x0D96 3478 | char    | _8th Sheet block_                     |437573746f6d32| ffffffffffffff        | 8th sheet - Name: _Custom2_                       | ^            | D3 Ax0d96 C7
| 0x0D9D 3485 | uint16  | ^                                     | 00 00h 0     | ff ffh 65535          | 8th sheet - Z offset                              | ^            | D3 Ax0d9d C2
| 0x0D9F 3487 | uint8   | ^                                     | 00h 0        | ffh 255               | 8th sheet - bed temp                              | ^            | D3 Ax0d9f C1
| 0x0DA0 3488 | uint8   | ^                                     | 00h 0        | ffh 255               | 8th sheet - PINDA temp                            | ^            | D3 Ax0da0 C1
| 0x0DA1 3489 | uint8   | active_sheet                          | 00h 0        | ffh 255               | Active sheet index                                | ^            | D3 Ax0da1 C1
| 0x0D48 3400 | uint8   | EEPROM_FSENSOR_PCB                    | ffh 255      | ffh 255               | Filament Sensor type IR unknown                   | LCD Support  | D3 Ax0d48 C1
| ^           | ^       | ^                                     | 00h 0        | ^                     | Filament Sensor type IR 0.3 or older              | ^            | ^
| ^           | ^       | ^                                     | 01h 1        | ^                     | Filament Sensor type IR 0.4 or newer              | ^            | ^
| 0x0D47 3399 | uint8   | EEPROM_FSENSOR_ACTION_NA              | 00h 0        | ffh 255               | Filament Sensor action: __Continue__              | LCD menu     | D3 Ax0d47 C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | Filament Sensor action: __Pause__                 | ^            | ^
| 0x0D37 3383 | float   | EEPROM_UVLO_SAVED_START_POSITION      | ???          | ff ff ff ffh          | Power panic saved start position all-axis         | ???          | D3 Ax0d37 C16
| ^           | ^       | ^                                     | ???          | ^                     | Power panic saved start position e-axis           | ^            | D3 Ax0d43 C4
| ^           | ^       | ^                                     | ???          | ^                     | Power panic saved start position z-axis           | ^            | D3 Ax0d3f C4
| ^           | ^       | ^                                     | ???          | ^                     | Power panic saved start position y-axis           | ^            | D3 Ax0d3b C4
| ^           | ^       | ^                                     | ???          | ^                     | Power panic saved start position x-axis           | ^            | D3 Ax0d37 C4
| 0x0D35 3381 | uint16  | EEPROM_UVLO_FEEDMULTIPLY              | ???          | ff ffh 65355          | Power panic saved feed multiplier                 | ???          | D3 Ax0d35 C2
| 0x0D34 3380 | uint8   | EEPROM_BACKLIGHT_LEVEL_HIGH           | 00h - ffh    | 82h 130               | LCD backlight bright: __128__ Dim value to 255    | LCD menu     | D3 Ax0d34 C1
| 0x0D33 3379 | uint8   | EEPROM_BACKLIGHT_LEVEL_LOW            | 00h - ffh    | 32h 50                | LCD backlight dim: __50__ 0 to Bright value       | LCD menu     | D3 Ax0d33 C1
| 0x0D32 3378 | uint8   | EEPROM_BACKLIGHT_MODE                 | 02h 2        | ffh 255               | LCD backlight mode: __Auto__                      | LCD menu     | D3 Ax0d32 C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | LCD backlight mode: __Bright__                    | ^            | ^
| ^           | ^       | ^                                     | 00h 0        | ^                     | LCD backlight mode: __Dim__                       | ^            | ^
| 0x0D30 3376 | uint16  | EEPROM_BACKLIGHT_TIMEOUT              | 01 00 - ff ff| 0a 00h 65535          | LCD backlight timeout: __10__ seconds             | LCD menu     | D3 Ax0d30 C2
| 0x0D2C 3372 | float   | EEPROM_UVLO_LA_K                      | ???          | ff ff ff ffh          | Power panic saved Linear Advanced K value         | ???          | D3 Ax0d2c C4
| 0x0D2B 3371 | uint8   | EEPROM_ALTFAN_OVERRIDE                | ffh 255      | ffh 255               | ALTFAN override unknown state                     | LCD menu     | D3 Ax0d2b C1
| ^           | ^       | ^                                     | 00h 0        | ^                     | ALTFAN override deactivated                       | ^            | ^
| ^           | ^       | ^                                     | 01h 1        | ^                     | ALTFAN override activated                         | ^            | ^
| 0x0D2A 3370 | uint8   | EEPROM_EXPERIMENTAL_VISIBILITY        | ffh 255      | ffh 255               | Experimental menu visibility unknown state        | LCD menu     | D3 Ax0d2a C1
| ^           | ^       | ^                                     | 00h 0        | ^                     | Experimental menu visibility hidden               | ^            | ^
| ^           | ^       | ^                                     | 01h 1        | ^                     | Experimental menu visibility visible              | ^            | ^
| 0x0D29 3369 | uint8   | EEPROM_PINDA_TEMP_COMPENSATION        | ffh 255      | ffh 255               | PINDA temp compensation unknown state             | LCD menu     | D3 Ax0d29 C1
| ^           | ^       | ^                                     | 00h 0        | ^                     | PINDA has no temp compensation PINDA v1/2         | ^            | ^
| ^           | ^       | ^                                     | 01h 1        | ^                     | PINDA has temp compensation aka SuperPINDA        | ^            | ^
| 0x0D15 3349 | char[20]| EEPROM_PRUSA_SN                       | SN[19] == 0  | ffffffffffffffff...   | PRUSA Serial number string                        | PRUSA SN     | D3 Ax0d15 C20
| 0x0D11 3345 | float   | EEPROM_UVLO_ACCELL                    | ???          | ff ff ff ffh          | Power panic saved normal acceleration             | ???          | D3 Ax0d11 C4
| 0x0D0D 3341 | float   | EEPROM_UVLO_RETRACT_ACCELL            | ???          | ff ff ff ffh          | Power panic saved retract acceleration            | ???          | D3 Ax0d0d C4
| 0x0D09 3337 | float   | EEPROM_UVLO_TRAVEL_ACCELL             | ???          | ff ff ff ffh          | Power panic saved travel acceleration             | ???          | D3 Ax0d09 C4
| 0x0D05 3333 | unint32 | EEPROM_JOB_ID                         | ???          | 00 00 00 00h            | Job ID used by host software                    | D3 only      | D3 Ax0d05 C4
| 0x0D04 3332 | uint8   | EEPROM_ECOOL_ENABLE                   | ffh 255      | ^                     | Disable extruder motor scaling for non-farm print | LCD menu     | D3 Ax0d04 C1
| ^           | ^       | ^                                     | 2ah 42       | ^                     | Enable extruder motor scaling for non-farm print  | ^            | D3 Ax0d04 C1
| 0x0D03 3331 | uint8   | EEPROM_FW_CRASH_FLAG                  | ffh 255      | ffh 255               | Last FW crash reason (dump_crash_reason)          | D21/D22      | D3 Ax0d03 C1
| ^           | ^       | ^                                     | 00h 0        | ^                     | manual                                            | ^            | ^
| ^           | ^       | ^                                     | 01h 1        | ^                     | stack_error                                       | ^            | ^
| ^           | ^       | ^                                     | 02h 2        | ^                     | watchdog                                          | ^            | ^
| ^           | ^       | ^                                     | 03h 3        | ^                     | bad_isr                                           | ^            | ^
| ^           | ^       | ^                                     | 04h 4        | ^                     | bad_pullup_temp_isr                               | ^            | ^
| ^           | ^       | ^                                     | 05h 5        | ^                     | bad_pullup_step_isr                               | ^            | ^
| 0x0D02 3330 | uint8   | EEPROM_TEMP_MODEL_ENABLE              | 00h 0        | ff/00                 | Temp model deactivated                            | Temp model   | D3 Ax0d02 C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | Temp model activated                              | ^            | ^
| 0x0CFE 3326 | float   | EEPROM_TEMP_MODEL_P                   | ???          | ff ff ff ffh          | Temp model power (W)                              | Temp model   | D3 Ax0cfe C4
| 0x0CFA 3322 | float   | EEPROM_TEMP_MODEL_C                   | ???          | ff ff ff ffh          | Temp model capacitance (J/K)                      | Temp model   | D3 Ax0cfa C4
| 0x0CBA 3258 |float[16]| EEPROM_TEMP_MODEL_R                   | ???          | ff ff ff ffh          | Temp model resistance (K/W)                       | Temp model   | D3 Ax0cba C64
| 0x0CB6 3254 | float   | EEPROM_TEMP_MODEL_Ta_corr             | ???          | ff ff ff ffh          | Temp model ambient temperature correction (K)     | Temp model   | D3 Ax0cb6 C4
| 0x0CB2 3250 | float   | EEPROM_TEMP_MODEL_W                   | ???          | ff ff ff ffh          | Temp model warning threshold (K/s)                | Temp model   | D3 Ax0cb2 C4
| 0x0CAE 3246 | float   | EEPROM_TEMP_MODEL_E                   | ???          | ff ff ff ffh          | Temp model error threshold (K/s)                  | Temp model   | D3 Ax0cae C4
| 0x0CAD 3245 | uint8   | EEPROM_FSENSOR_JAM_DETECTION          | 01h 1        | ff/01                 | fsensor pat9125 jam detection feature             | LCD menu     | D3 Ax0cad C1
| 0x0CAC 3244 | uint8   | EEPROM_MMU_ENABLED                    | 00h 0        | ff/00                 | MMU enabled                                       | LCD menu     | D3 Ax0cac C1
| 0x0CA8 3240 | uint32  | EEPROM_MMU_MATERIAL_CHANGES           | ???          | ff ff ff ffh          | MMU toolchange counter over printers lifetime     | LCD statistic| D3 Ax0ca8 C4
| 0x0CA7 3239 | uint8   | EEPROM_HEAT_BED_ON_LOAD_FILAMENT      | ffh 255      | ffh 255               | Heat bed on load filament unknown state           | LCD menu     | D3 Ax0ca7 C1
| ^           | ^       | ^                                     | 00h 0        | ^                     | Do not heat bed on load filament                  | ^            | ^
| ^           | ^       | ^                                     | 01h 1        | ^                     | Heat bed on load filament                         | ^            | ^
| 0x0CA6 3238 | uint8   | EEPROM_CALIBRATION_STATUS_V2          | ffh 255      | ffh 255               | Calibration status (>=v3.12)                      | ???          | D3 Ax0ca6 C1
| ^           | ^       | ^                                     | 01h 1        | ^                     | Selftest passed                                   | ^            | ^
| ^           | ^       | ^                                     | 02h 2        | ^                     | XYZ cal passed                                    | ^            | ^
| ^           | ^       | ^                                     | 04h 4        | ^                     | Z cal passed                                      | ^            | ^
| ^           | ^       | ^                                     | 08h 8        | ^                     | Temp model cal passed                             | ^            | ^
| ^           | ^       | ^                                     | 10h 16       | ^                     | Live Adjust set                                   | ^            | ^
| ^           | ^       | ^                                     | 20h 32       | ^                     | Free bit                                          | ^            | ^
| ^           | ^       | ^                                     | 40h 64       | ^                     | Free bit                                          | ^            | ^
| ^           | ^       | ^                                     | 80h 128      | ^                     | Unknown                                           | ^            | ^
| 0x0CA5 3237 | float   | EEPROM_TEMP_MODEL_U                   | ???          | ff ff ff ffh          | Temp model linear temperature coefficient (W/K/W) | Temp model   | D3 Ax0ca5 C4
| 0x0CA1 3233 | float   | EEPROM_TEMP_MODEL_V                   | ???          | ff ff ff ffh          | Temp model linear temperature intercept (W/W)     | Temp model   | D3 Ax0ca1 C4
| 0x0C9D 3229 | float   | EEPROM_TEMP_MODEL_D                   | ???          | ff ff ff ffh          | Temp model sim. 1st order IIR filter factor       | Temp model   | D3 Ax0c9d C4
| 0x0C99 3225 | uint16  | EEPROM_TEMP_MODEL_L                   | 0-2160       | ff ffh                | Temp model sim. response lag (ms)                 | Temp model   | D3 Ax0c99 C2

|Address begin|Bit/Type | Name                                  | Valid values | Default/FactoryReset  | Description                                       |Gcode/Function| Debug code
| :--:        | :--:    | :--:                                  | :--:         | :--:                  | :--:                                              | :--:         | :--:
| 0x0012 18   | uint16  | EEPROM_FIRMWARE_VERSION_END           | ???          | ff ffh 65535          | ???                                               | ???          | D3 Ax0012 C2
| 0x0010 16   | uint16  | EEPROM_FIRMWARE_VERSION_FLAVOR        | ???          | ff ffh 65535          | ???                                               | ???          | D3 Ax0010 C2
| 0x000E 14   | uint16  | EEPROM_FIRMWARE_VERSION_REVISION      | ???          | ff ffh 65535          | Firmware version revision number DEV/ALPHA/BETA/RC| ???          | D3 Ax000e C2
| 0x000C 12   | uint16  | EEPROM_FIRMWARE_VERSION_MINOR         | ???          | ff ffh 65535          | Firmware version minor number                     | ???          | D3 Ax000c C2
| 0x000A 10   | uint16  | EEPROM_FIRMWARE_VERSION_MAJOR         | ???          | ff ffh 65535          | Firmware version major number                     | ???          | D3 Ax000a C2
| 0x0000 0    | char    | FW_PRUSA3D_MAGIC                      | ???          | ffffffffffffffffffff  | __`PRUSA3DFW`__                                   | ???          | D3 Ax0000 C10
*/

#define EEPROM_EMPTY_VALUE 0xFF
#define EEPROM_EMPTY_VALUE16 0xFFFF
#define EEPROM_EMPTY_VALUE32 0xFFFFFFFFl
// The total size of the EEPROM is
// 4096 for the Atmega2560
#define EEPROM_TOP 4096
#define EEPROM_SILENT 4095
#define EEPROM_LANG 4094
#define EEPROM_BABYSTEP_X 4092 //unused
#define EEPROM_BABYSTEP_Y 4090 //unused
#define EEPROM_BABYSTEP_Z 4088 //legacy, multiple values stored now in EEPROM_Sheets_base
#define EEPROM_CALIBRATION_STATUS_V1 4087 // legacy, used up to v3.11
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
#define EEPROM_FREE_NR1 (EEPROM_FARM_MODE-1)
#define EEPROM_FARM_NUMBER (EEPROM_FREE_NR1-2)

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
#define _EEPROM_FREE_NR6_ (EEPROM_TEMP_CAL_ACTIVE - 2*4) //4 x int (used to be for bowden lengths for SNMM)
#define EEPROM_CALIBRATION_STATUS_PINDA (_EEPROM_FREE_NR6_ - 1) //0 - not calibrated; 1 - calibrated
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
#define EEPROM_UVLO_SAVED_SEGMENT_IDX   (EEPROM_UVLO_CURRENT_POSITION_E - 2) //uint16_t

#define EEPROM_FREE_NR4         (EEPROM_UVLO_SAVED_SEGMENT_IDX - 1)							// FREE EEPROM SPACE
#define EEPROM_FREE_NR5         (EEPROM_FREE_NR4 - 1)							// FREE EEPROM SPACE

// Crash detection mode EEPROM setting
#define EEPROM_CRASH_DET         (EEPROM_FREE_NR5 - 1)       				    // uint8 (orig EEPROM_UVLO_MESH_BED_LEVELING-12)
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
#define EEPROM_WIZARD_ACTIVE (EEPROM_XYZ_CAL_SKEW - 1)                          // 0: wizard not active, 1: wizard active, 2: wizard active without yes/no = forced calibrate Z after shipping/service prep.
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
#define EEPROM_SPOOL_JOIN (EEPROM_SOUND_MODE-1) //bool

#define EEPROM_FSENS_RUNOUT_ENABLED (EEPROM_SPOOL_JOIN - 1) //bool

#define EEPROM_MMU_FAIL_TOT (EEPROM_FSENS_RUNOUT_ENABLED - 2) //uint16_t
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

#define EEPROM_UVLO_SAVED_START_POSITION (EEPROM_FSENSOR_ACTION_NA - 4*4) // 4 x float for saved start position for all axes
#define EEPROM_UVLO_FEEDMULTIPLY (EEPROM_UVLO_SAVED_START_POSITION - 2) // uint16_t for feedmultiply

#define EEPROM_BACKLIGHT_LEVEL_HIGH (EEPROM_UVLO_FEEDMULTIPLY-1) // uint8
#define EEPROM_BACKLIGHT_LEVEL_LOW (EEPROM_BACKLIGHT_LEVEL_HIGH-1) // uint8
#define EEPROM_BACKLIGHT_MODE (EEPROM_BACKLIGHT_LEVEL_LOW-1) // uint8
#define EEPROM_BACKLIGHT_TIMEOUT (EEPROM_BACKLIGHT_MODE-2) // uint16

#define EEPROM_UVLO_LA_K (EEPROM_BACKLIGHT_TIMEOUT-4) // float

#define EEPROM_ALTFAN_OVERRIDE (EEPROM_UVLO_LA_K-1) //uint8
#define EEPROM_EXPERIMENTAL_VISIBILITY (EEPROM_ALTFAN_OVERRIDE-1) //uint8
#define EEPROM_PINDA_TEMP_COMPENSATION (EEPROM_EXPERIMENTAL_VISIBILITY-1) //uint8
#define EEPROM_PRUSA_SN (EEPROM_PINDA_TEMP_COMPENSATION-20) //char[20]

#define EEPROM_UVLO_ACCELL (EEPROM_PRUSA_SN-4) // float
#define EEPROM_UVLO_RETRACT_ACCELL (EEPROM_UVLO_ACCELL-4) // float
#define EEPROM_UVLO_TRAVEL_ACCELL (EEPROM_UVLO_RETRACT_ACCELL-4) // float

#define EEPROM_JOB_ID (EEPROM_UVLO_TRAVEL_ACCELL-4) //uint32_t

#define EEPROM_ECOOL_ENABLE (EEPROM_JOB_ID-1) // uint8_t
#define EEPROM_FW_CRASH_FLAG (EEPROM_ECOOL_ENABLE-1) // uint8_t

#define EEPROM_TEMP_MODEL_ENABLE (EEPROM_FW_CRASH_FLAG-1) // uint8_t
#define EEPROM_TEMP_MODEL_P (EEPROM_TEMP_MODEL_ENABLE-4) // float
#define EEPROM_TEMP_MODEL_C (EEPROM_TEMP_MODEL_P-4) // float
#define EEPROM_TEMP_MODEL_R (EEPROM_TEMP_MODEL_C-4*16) // float[16]
#define EEPROM_TEMP_MODEL_Ta_corr (EEPROM_TEMP_MODEL_R-4) // float
#define EEPROM_TEMP_MODEL_W (EEPROM_TEMP_MODEL_Ta_corr-4) // float
#define EEPROM_TEMP_MODEL_E (EEPROM_TEMP_MODEL_W-4) // float

#define EEPROM_FSENSOR_JAM_DETECTION (EEPROM_TEMP_MODEL_E-1) // uint8_t
#define EEPROM_MMU_ENABLED (EEPROM_FSENSOR_JAM_DETECTION-1) // uint8_t
#define EEPROM_MMU_MATERIAL_CHANGES (EEPROM_MMU_ENABLED-4) // uint32_t
#define EEPROM_HEAT_BED_ON_LOAD_FILAMENT (EEPROM_MMU_MATERIAL_CHANGES-1) //uint8
#define EEPROM_CALIBRATION_STATUS_V2 (EEPROM_HEAT_BED_ON_LOAD_FILAMENT-1) //uint8

#define EEPROM_TEMP_MODEL_U (EEPROM_CALIBRATION_STATUS_V2-4) //float
#define EEPROM_TEMP_MODEL_V (EEPROM_TEMP_MODEL_U-4) //float
#define EEPROM_TEMP_MODEL_D (EEPROM_TEMP_MODEL_V-4) //float
#define EEPROM_TEMP_MODEL_L (EEPROM_TEMP_MODEL_D-2) //uint16_t
#define EEPROM_TEMP_MODEL_VER (EEPROM_TEMP_MODEL_L-1) //uint8_t

//This is supposed to point to last item to allow EEPROM overrun check. Please update when adding new items.
#define EEPROM_LAST_ITEM EEPROM_TEMP_MODEL_VER
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
#define EEPROM_ECOOL_MAGIC_NUMBER 42

#ifdef __cplusplus
#include "ConfigurationStore.h"
static_assert(EEPROM_FIRMWARE_VERSION_END < 20, "Firmware version EEPROM address conflicts with EEPROM_M500_base");
static M500_conf * const EEPROM_M500_base = reinterpret_cast<M500_conf*>(20); //offset for storing settings using M500
static_assert(((sizeof(M500_conf) + 20) < EEPROM_LAST_ITEM), "M500_conf address space conflicts with previous items.");
#endif

enum
{
    EEPROM_MMU_CUTTER_ENABLED_enabled = 1,
    EEPROM_MMU_CUTTER_ENABLED_always = 2,
};

#ifdef __cplusplus
void eeprom_init();
void eeprom_adjust_bed_reset();
struct SheetName
{
    char c[sizeof(Sheet::name) + 1];
};
void eeprom_default_sheet_name(uint8_t index, SheetName &sheetName);
int8_t eeprom_next_initialized_sheet(int8_t sheet);
void eeprom_switch_to_next_sheet();
bool eeprom_is_sheet_initialized(uint8_t sheet_num);

bool eeprom_is_initialized_block(const void *__p, size_t __n);
void eeprom_update_block_P(const void *__src, void *__dst, size_t __n);
void eeprom_toggle(uint8_t *__p);

void eeprom_increment_byte(uint8_t *__p);
void eeprom_increment_word(uint16_t *__p);
void eeprom_increment_dword(uint32_t *__p);

void eeprom_add_byte(uint8_t *__p, uint8_t add);
void eeprom_add_word(uint16_t *__p, uint16_t add);
void eeprom_add_dword(uint32_t *__p, uint32_t add);

uint8_t eeprom_init_default_byte(uint8_t *__p, uint8_t def);
uint16_t eeprom_init_default_word(uint16_t *__p, uint16_t def);
uint32_t eeprom_init_default_dword(uint32_t *__p, uint32_t def);
void eeprom_init_default_float(float *__p, float def);
void eeprom_init_default_block(void *__p, size_t __n, const void *def);
void eeprom_init_default_block_P(void *__p, size_t __n, const void *def);
#endif

#endif // EEPROM_H
