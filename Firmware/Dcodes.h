#ifndef DCODES_H
#define DCODES_H

extern void dcode__1(); //D-1 - Endless loop (to simulate deadlock)
extern void dcode_0(); //D0 - Reset
extern void dcode_1(); //D1 - Clear EEPROM

#if defined DEBUG_DCODE2 || defined DEBUG_DCODES
extern void dcode_2(); //D2 - Read/Write RAM
#endif

#if defined DEBUG_DCODE3 || defined DEBUG_DCODES
extern void dcode_3(); //D3 - Read/Write EEPROM
#endif //DEBUG_DCODE3

extern void dcode_4(); //D4 - Read/Write PIN

#if defined DEBUG_DCODE5 || defined DEBUG_DCODES
extern void dcode_5(); //D5 - Read/Write FLASH
#endif //DEBUG_DCODE5

#if defined DEBUG_DCODE6 || defined DEBUG_DCODES
extern void dcode_6(); //D6 - Read/Write external FLASH
#endif

extern void dcode_7(); //D7 - Read/Write Bootloader
extern void dcode_8(); //D8 - Read/Write PINDA
extern void dcode_9(); //D9 - Read/Write ADC (Write=enable simulated, Read=disable simulated)
extern void dcode_10(); //D10 - XYZ calibration = OK
extern void dcode_12(); //D12 - Log time. Writes the current time in the log file.

#ifdef XFLASH_DUMP
extern void dcode_20(); //D20 - Generate an offline crash dump
extern void dcode_21(); //D21 - Print crash dump to serial
extern void dcode_22(); //D22 - Clear crash dump state
#endif

#ifdef EMERGENCY_SERIAL_DUMP
#include "xflash_dump.h"
extern void dcode_23(); //D23 - Request/generate an online serial crash dump
extern bool emergency_serial_dump; //emergency dump enabled flag
extern void serial_dump_and_reset(dump_crash_reason);
#endif

#ifdef HEATBED_ANALYSIS
extern void dcode_80(); //D80 - Bed check. This command will log data to SD card file "mesh.txt".
extern void dcode_81(); //D81 - Bed analysis. This command will log data to SD card file "wldsd.txt".
#endif //HEATBED_ANALYSIS

	extern void dcode_106(); //D106 - Print measured fan speed for different pwm values

#ifdef TMC2130
	extern void dcode_2130(); //D2130 - TMC2130
#endif //TMC2130

#if defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
	extern void dcode_9125(); //D9125 - PAT9125
#endif //defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)


#endif //DCODES_H
