
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

<table>
<caption id="multi_row">EEPROM Table</caption>
<tr>            <th>Adress begin     		<th>Bit/Type		<th>Name										<th>Valid values		<th>Default/FactoryReset<th>Description												<th>Debug code
<tr><td rowspan="3">0x0FFFh 4095<td rowspan="3">uchar<td rowspan="3">EEPROM_SILENT								<td>00h 0	<td rowspan="3">???					<td>TMC Stealth mode off / miniRambo Power mode	<td rowspan="3">D3 Ax0fff C1
<tr>																											<td>01h 1										<td>TMC Stealth mode on / miniRambo Silent mode	
<tr>																											<td>02h 2										<td>Auto mode		
<tr><td rowspan="2">0x0FFEh 4094<td rowspan="2">uchar<td rowspan="2">EEPROM_LANG								<td>00h 0	<td rowspan="2">00h 0				<td>English / LANG_ID_PRI						<td rowspan="2">D3 Ax0ffe C1
<tr>																											<td>01h 1										<td>Other language LANG_ID_SEC
<tr><td rowspan="1">0x0FFCh 4092			<td>uint16			<td>EEPROM_BABYSTEP_X							<td>???					<td>ffh 255				<td>Babystep for X axis _unsued_							<td>D3 Ax0ffc C2
<tr><td rowspan="1">0x0FFAh 4090			<td>uint16			<td>EEPROM_BABYSTEP_Y							<td>???					<td>ffh 222				<td>Babystep for Y axis _unsued_							<td>D3 Ax0ffa C2
<tr><td rowspan="2">0x0FF8h 4088<td rowspan="2">uint16<td rowspan="2">EEPROM_BABYSTEP_Z				<td rowspan="2">???		<td rowspan="2">ffh 255				<td>Babystep for Z axis _lagacy_				<td rowspan="2">D3 Ax0ff8 C2
<tr>																																							<td>multiple values stored now in EEPROM_Sheets_base
<tr><td rowspan="6">0x0FF7h 4087<td rowspan="6">uint8<td rowspan="6">EEPROM_CALIBRATION_STATUS					<td>00h 0	<td rowspan="6">ffh 255				<td>Unknown										<td rowspan="6">D3 Ax0ff7 C1
<tr>																											<td>01h 1										<td>Calibrated
<tr>																											<td>E6h 230										<td>needs Live Z adjustment
<tr>																											<td>F0h 240										<td>needs Z calibration
<tr>																											<td>FAh 250										<td>needs XYZ calibration 
<tr>																											<td>FFh 255										<td>Assbemled _default_
<tr><td rowspan="1">0x0FF5h 4085			<td>uint16			<td>EEPROM_BABYSTEP_Z0							<td>???					<td>???					<td>Babystep for Z ???										<td>D3 Ax0ff5 C2
<tr><td rowspan="1">0x0FF1h 4081			<td>uint32			<td>EEPROM_FILAMENTUSED							<td>???					<td>00h 0				<td>Filament used in meters									<td>D3 Ax0ff1 C4
<tr><td rowspan="1">0x0FEDh 4077			<td>uint32			<td>EEPROM_TOTALTIME							<td>???					<td>00h 0				<td>Total print time										<td>D3 Ax0fed C4
<tr><td rowspan="2">0x0FE5h 4069<td rowspan="2">float<td rowspan="2">EEPROM_BED_CALIBRATION_CENTER	<td rowspan="2">???		<td rowspan="2">???					<td>???											 <td rowspan="2">D3 Ax0fe5 C8
<tr>																																							<td>???
<tr><td rowspan="2">0x0FDDh 4061<td rowspan="2">float<td rowspan="2">EEPROM_BED_CALIBRATION_VEC_X	<td rowspan="2">???		<td rowspan="2">???					<td>???											 <td rowspan="2">D3 Ax0fdd C8
<tr>																																							<td>???
<tr><td rowspan="2">0x0FD5h 4053<td rowspan="2">float<td rowspan="2">EEPROM_BED_CALIBRATION_VEC_Y	<td rowspan="2">???		<td rowspan="2">???					<td>???											 <td rowspan="2">D3 Ax0fd5 C8
<tr>																																							<td>???
<tr><td rowspan="8">0x0FC5h 4037<td rowspan="8">int16<td rowspan="8">EEPROM_BED_CALIBRATION_Z_JITTER<td rowspan="8">???		<td rowspan="8">???					<td>???											 <td rowspan="8">D3 Ax0fc5 C16
<tr>																																							<td>???
<tr>																																							<td>???
<tr>																																							<td>???
<tr>																																							<td>???
<tr>																																							<td>???
<tr>																																							<td>???
<tr>																																							<td>???
<tr><td rowspan="2">0x0FC4h 4036<td rowspan="2">bool<td rowspan="2">EEPROM_FARM_MODE							<td>00h 0	<td rowspan="2">00h 0				<td>Prusa farm mode off							<td rowspan="2">D3 Ax0fc4 C1
<tr>																											<td>ffh 255										<td>Prusa farm mode on
<tr><td rowspan="1">0x0FC1h 4033			<td>int16			<td>EEPROM_FARM_NUMBER							<td>???					<td>ff ff ffh			<td>Prusa farm number										<td>D3 Ax0fc1 C3
<tr><td rowspan="2">0x0FC0h 4032<td rowspan="2">bool<td rowspan="2">EEPROM_BED_CORRECTION_VALID					<td>00h 0	<td rowspan="2">00h 0				<td>Bed correction invalid						<td rowspan="2">D3 Ax0fc0 C1
<tr>																											<td>ffh 255										<td>Bed correction valid
<tr><td rowspan="2">0x0FBFh 4031<td rowspan="2">char<td rowspan="2">EEPROM_BED_CORRECTION_LEFT		<td rowspan="2">00h FFh	<td rowspan="2">00h 0				<td>Bed manual correction left					<td rowspan="2">D3 Ax0fbf C1
<tr>																																							<td>At this moment limited to +-100um
<tr><td rowspan="2">0x0FBEh 4030<td rowspan="2">char<td rowspan="2">EEPROM_BED_CORRECTION_RIGHT		<td rowspan="2">00h FFh	<td rowspan="2">00h 0				<td>Bed manual correction right					<td rowspan="2">D3 Ax0fbe C1
<tr>																																							<td>At this moment limited to +-100um
<tr><td rowspan="2">0x0FBDh 4029<td rowspan="2">char<td rowspan="2">EEPROM_BED_CORRECTION_FRONT		<td rowspan="2">00h FFh	<td rowspan="2">00h 0				<td>Bed manual correction front					<td rowspan="2">D3 Ax0fbd C1
<tr>																																							<td>At this moment limited to +-100um
<tr><td rowspan="2">0x0FBCh 4028<td rowspan="2">char<td rowspan="2">EEPROM_BED_CORRECTION_BACK		<td rowspan="2">00h FFh	<td rowspan="2">00h 0				<td>Bed manual correction back					<td rowspan="2">D3 Ax0fbc C1
<tr>																																							<td>At this moment limited to +-100um
<tr><td rowspan="2">0x0FBBh 4027<td rowspan="2">bool<td rowspan="2">EEPROM_TOSHIBA_FLASH_AIR_COMPATIBLITY		<td>00h 0		<td rowspan="2">00h 0			<td>Toshiba Air off								<td rowspan="2">D3 Ax0fbb C1
<tr>																											<td>ffh 255										<td>Toshiba Air oon
<tr><td rowspan="1">0x0FBAh 4026			<td>uchar			<td>EEPROM_PRINT_FLAG							<td>???					<td>???					<td>_unsued_												<td>D3 Ax0fba C1
<tr><td rowspan="5">0x0FB0h 4016<td rowspan="5">int16<td rowspan="5">EEPROM_PROBE_TEMP_SHIFT		<td rowspan="5">???		<td rowspan="5">???					<td>???											<td rowspan="5">D3 Ax0fb0 C10
<tr>																																							<td>???
<tr>																																							<td>???
<tr>																																							<td>???
<tr>																																							<td>???
<tr><td rowspan="2">0x0FAFh 4015<td rowspan="2">bool<td rowspan="2">EEPROM_TEMP_CAL_ACTIVE						<td>00h 0	<td rowspan="2">00h 0				<td>PINDA Temp cal. inactive					<td rowspan="2">D3 Ax0faf C1
<tr>																											<td>FFh 255										<td>PINDA Temp cal. active
<tr><td rowspan="2">0x0FA7h 4007<td rowspan="2">uint32<td rowspan="2">EEPROM_BOWDEN_LENGTH			<td rowspan="2">???		<td>ff 00 ff ffh					<td rowspan="2">Bowden length					<td rowspan="2">D3 Ax0fae C8
<tr>																														<td>ff ff ff ffh
<tr><td rowspan="2">0x0FA6h 4006<td rowspan="2">uint8<td rowspan="2">EEPROM_CALIBRATION_STATUS_PINDA			<td>00h 0	<td rowspan="2">ffh 255				<td>PINDA Temp not calibrated					<td rowspan="2">D3 Ax0fa6 C1
<tr>																											<td>01h 1										<td>PINDA Temp calibrated
<tr><td rowspan="3">0x0FA5h 4005<td rowspan="3">uint8<td rowspan="3">EEPROM_UVLO								<td>00h 0	<td rowspan="3">ffh 255				<td>Power Panic flag inactive 					<td rowspan="3">D3 Ax0fa5 C1
<tr>																											<td>01h 1										<td>Power Panic flag active
<tr>																											<td>02h 2										<td>Power Panic flag ???
<tr><td rowspan="2">0x0F9Dh 3997<td rowspan="2">float<td rowspan="2">EEPROM_UVLO_CURRENT_POSITION				<td>???		<td rowspan="2">ffh 255	<td rowspan="2">Power Panic position 						<td rowspan="2">D3 Ax0f9d C8
<tr>																											<td>???
<tr><td rowspan="8">0x0F95h 3989<td rowspan="8">char<td rowspan="8">EEPROM_FILENAME								<td>???		<td rowspan="8">ffh 255	<td rowspan="8">Power Panic Filename 						<td rowspan="8">D3 Ax0f95 C8
<tr>																											<td>???
<tr>																											<td>???
<tr>																											<td>???
<tr>																											<td>???
<tr>																											<td>???
<tr>																											<td>???
<tr>																											<td>???
<tr>			<td>0x0F91h 39851			<td>unit32			<td>EEPROM_FILE_POSITION						<td>???		<td>ff ff ff ffh					<td>Power Panic File Postion 								<td>D3 Ax0f91 C4
<tr>			<td>0x0F8Dh 3981			<td>float			<td>EEPROM_UVLO_CURRENT_POSITION_Z				<td>???		<td>ff ff ff ffh					<td>Power Panic Z Position	 								<td>D3 Ax0f8d C4
<tr><td rowspan="1">0x0F8Ch 3980<td rowspan="1">???<td rowspan="1">EEPROM_UVLO_UNUSED_001						<td>??? 	<td rowspan="1">ffh 255	<td rowspan="1">Power Panic UNUSED 							<td rowspan="1">D3 Ax0f8c C1
<tr><td rowspan="1">0x0F8Bh 3979<td rowspan="1">uint8<td rowspan="1">EEPROM_UVLO_TARGET_BED						<td>???		<td rowspan="1">ffh 255				<td>Power Panic Bed temperature					<td rowspan="1">D3 Ax0f8b C1
<tr><td rowspan="1">0x0F89h 3977<td rowspan="1">uint16<td rowspan="1">EEPROM_UVLO_FEEDRATE						<td>???		<td rowspan="1">ff ffh 65535		<td>Power Panic Feedrate						<td rowspan="1">D3 Ax0f89 C2
<tr><td rowspan="1">0x0F88h 3976<td rowspan="1">uint8<td rowspan="1">EEPROM_UVLO_FAN_SPEED						<td>???		<td rowspan="1">ffh 255				<td>Power Panic Fan speed						<td rowspan="1">D3 Ax0f88 C1
<tr><td rowspan="2">0x0F87h 3975<td rowspan="2">uint8<td rowspan="2">EEPROM_FAN_CHECK_ENABLED					<td>00h 0				<td>					<td>Fan Check disabled							<td rowspan="2">D3 Ax0f87 C1
<tr>																											<td>01h 1				<td>ffh 255				<td>Fan Check enabled (exception ffh=01h)


</table>
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

