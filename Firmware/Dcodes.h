#ifndef DCODES_H
#define DCODES_H

extern void dcode__1(); //D-1 - Endless loop (to simulate deadlock)
extern void dcode_0(); //D0 - Reset
extern void dcode_1(); //D1 - Clear EEPROM
extern void dcode_2(); //D2 - Read/Write RAM

#if defined DEBUG_DCODE3 || defined DEBUG_DCODES
extern void dcode_3(); //D3 - Read/Write EEPROM
#endif //DEBUG_DCODE3

extern void dcode_4(); //D4 - Read/Write PIN

#if defined DEBUG_DCODE5 || defined DEBUG_DCODES
extern void dcode_5(); //D5 - Read/Write FLASH
#endif //DEBUG_DCODE5

extern void dcode_6(); //D6 - Read/Write external FLASH
extern void dcode_7(); //D7 - Read/Write Bootloader
extern void dcode_8(); //D8 - Read/Write PINDA
extern void dcode_9(); //D9 - Read/Write ADC (Write=enable simulated, Read=disable simulated)
extern void dcode_10(); //D10 - XYZ calibration = OK
extern void dcode_12(); //D12 - Log time. Writes the current time in the log file.

#ifdef HEATBED_ANALYSIS
extern void dcode_80(); //D80 - Bed check. This command will log data to SD card file "mesh.txt".
extern void dcode_81(); //D81 - Bed analysis. This command will log data to SD card file "wldsd.txt".
#endif //HEATBED_ANALYSIS

	extern void dcode_106(); //D106 - Print measured fan speed for different pwm values

#ifdef TMC2130
	extern void dcode_2130(); //D2130 - TMC2130
#endif //TMC2130

#ifdef PAT9125
	extern void dcode_9125(); //D9125 - PAT9125
#endif //PAT9125


#endif //DCODES_H
