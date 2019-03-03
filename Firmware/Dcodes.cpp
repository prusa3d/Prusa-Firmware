#include "Dcodes.h"
//#include "Marlin.h"
#include "language.h"
#include "cmdqueue.h"
#include <stdio.h>
#include <avr/pgmspace.h>


#define DBG(args...) printf_P(args)

inline void print_hex_nibble(uint8_t val)
{
	putchar((val > 9)?(val - 10 + 'a'):(val + '0'));
}

void print_hex_byte(uint8_t val)
{
	print_hex_nibble(val >> 4);
	print_hex_nibble(val & 15);
}

void print_hex_word(uint16_t val)
{
	print_hex_byte(val >> 8);
	print_hex_byte(val & 255);
}

void print_eeprom(uint16_t address, uint16_t count, uint8_t countperline = 16)
{
	while (count)
	{
		print_hex_word(address);
		putchar(' ');
		uint8_t count_line = countperline;
		while (count && count_line)
		{
			putchar(' ');
			print_hex_byte(eeprom_read_byte((uint8_t*)address++));
			count_line--;
			count--;
		}
		putchar('\n');
	}
}

int parse_hex(char* hex, uint8_t* data, int count)
{
	int parsed = 0;
	while (*hex)
	{
		if (count && (parsed >= count)) break;
		char c = *(hex++);
		if (c == ' ') continue;
		if (c == '\n') break;
		uint8_t val = 0x00;
		if ((c >= '0') && (c <= '9')) val |= ((c - '0') << 4);
		else if ((c >= 'a') && (c <= 'f')) val |= ((c - 'a' + 10) << 4);
		else return -parsed;
		c = *(hex++);
		if ((c >= '0') && (c <= '9')) val |= (c - '0');
		else if ((c >= 'a') && (c <= 'f')) val |= (c - 'a' + 10);
		else return -parsed;
		data[parsed] = val;
		parsed++;
	}
	return parsed;
}


void print_mem(uint32_t address, uint16_t count, uint8_t type, uint8_t countperline = 16)
{
	while (count)
	{
		if (type == 2)
			print_hex_nibble(address >> 16);
		print_hex_word(address);
		putchar(' ');
		uint8_t count_line = countperline;
		while (count && count_line)
		{
			uint8_t data = 0;
			switch (type)
			{
			case 0: data = *((uint8_t*)address++); break;
			case 1: data = eeprom_read_byte((uint8_t*)address++); break;
			case 2: data = pgm_read_byte_far((uint8_t*)address++); break;
			}
			putchar(' ');
			print_hex_byte(data);
			count_line--;
			count--;
		}
		putchar('\n');
	}
}

#ifdef DEBUG_DCODE3
#define EEPROM_SIZE 0x1000
void dcode_3()
{
	DBG(_N("D3 - Read/Write EEPROM\n"));
	uint16_t address = 0x0000; //default 0x0000
	uint16_t count = EEPROM_SIZE; //default 0x1000 (entire eeprom)
	if (code_seen('A')) // Address (0x0000-0x0fff)
		address = (strchr_pointer[1] == 'x')?strtol(strchr_pointer + 2, 0, 16):(int)code_value();
	if (code_seen('C')) // Count (0x0001-0x1000)
		count = (int)code_value();
	address &= 0x1fff;
	if (count > EEPROM_SIZE) count = EEPROM_SIZE;
	if ((address + count) > EEPROM_SIZE) count = EEPROM_SIZE - address;
	if (code_seen('X')) // Data
	{
		uint8_t data[16];
		count = parse_hex(strchr_pointer + 1, data, 16);
		if (count > 0)
		{
			for (uint16_t i = 0; i < count; i++)
				eeprom_write_byte((uint8_t*)(address + i), data[i]);
			printf_P(_N("%d bytes written to EEPROM at address 0x%04x"), count, address);
			putchar('\n');
		}
		else
			count = 0;
	}
	print_mem(address, count, 1);
/*	while (count)
	{
		print_hex_word(address);
		putchar(' ');
		uint8_t countperline = 16;
		while (count && countperline)
		{
			uint8_t data = eeprom_read_byte((uint8_t*)address++);
			putchar(' ');
			print_hex_byte(data);
			countperline--;
			count--;
		}
		putchar('\n');
	}*/
}
#endif //DEBUG_DCODE3


#include "ConfigurationStore.h"
#include "cmdqueue.h"
#include "pat9125.h"
#include "adc.h"
#include "temperature.h"
#include <avr/wdt.h>
#include "bootapp.h"

/*
#define FLASHSIZE     0x40000

#define RAMSIZE        0x2000
#define boot_src_addr  (*((uint32_t*)(RAMSIZE - 16)))
#define boot_dst_addr  (*((uint32_t*)(RAMSIZE - 12)))
#define boot_copy_size (*((uint16_t*)(RAMSIZE - 8)))
#define boot_reserved  (*((uint8_t*)(RAMSIZE - 6)))
#define boot_app_flags (*((uint8_t*)(RAMSIZE - 5)))
#define boot_app_magic (*((uint32_t*)(RAMSIZE - 4)))
#define BOOT_APP_FLG_ERASE 0x01
#define BOOT_APP_FLG_COPY  0x02
#define BOOT_APP_FLG_FLASH 0x04

extern uint8_t fsensor_log;
extern float current_temperature_pinda;
extern float axis_steps_per_unit[NUM_AXIS];


//#define LOG(args...) printf(args)
#define LOG(args...)
*/
#ifdef DEBUG_DCODES

void dcode__1()
{
	printf("D-1 - Endless loop\n");
	cli();
	while (1);
}

void dcode_0()
{
	if (*(strchr_pointer + 1) == 0) return;
	LOG("D0 - Reset\n");
	if (code_seen('B')) //bootloader
	{
		cli();
		wdt_enable(WDTO_15MS);
		while(1);
	}
	else //reset
	{
#ifndef _NO_ASM
		asm volatile("jmp 0x00000");
#endif //_NO_ASM
	}
}

void dcode_1()
{
	LOG("D1 - Clear EEPROM and RESET\n");
	cli();
	for (int i = 0; i < 8192; i++)
		eeprom_write_byte((unsigned char*)i, (unsigned char)0xff);
	wdt_enable(WDTO_15MS);
	while(1);
}

void dcode_2()
{
	LOG("D2 - Read/Write RAM\n");
	uint16_t address = 0x0000; //default 0x0000
	uint16_t count = 0x2000; //default 0x2000 (entire ram)
	if (code_seen('A')) // Address (0x0000-0x1fff)
		address = (strchr_pointer[1] == 'x')?strtol(strchr_pointer + 2, 0, 16):(int)code_value();
	if (code_seen('C')) // Count (0x0001-0x2000)
		count = (int)code_value();
	address &= 0x1fff;
	if (count > 0x2000) count = 0x2000;
	if ((address + count) > 0x2000) count = 0x2000 - address;
	if (code_seen('X')) // Data
	{
		uint8_t data[16];
		count = parse_hex(strchr_pointer + 1, data, 16);
		if (count > 0)
		{
			for (int i = 0; i < count; i++)
				*((uint8_t*)(address + i)) =  data[i];
			LOG("%d bytes written to RAM at address %04x", count, address);
		}
		else
			count = 0;
	}
	print_mem(address, count, 0);
/*	while (count)
	{
		print_hex_word(address);
		putchar(' ');
		uint8_t countperline = 16;
		while (count && countperline)
		{
			uint8_t data = *((uint8_t*)address++);
			putchar(' ');
			print_hex_byte(data);
			countperline--;
			count--;
		}
		putchar('\n');
	}*/
}

void dcode_4()
{
	LOG("D4 - Read/Write PIN\n");
	if (code_seen('P')) // Pin (0-255)
	{
		int pin = (int)code_value();
		if ((pin >= 0) && (pin <= 255))
		{
			if (code_seen('F')) // Function in/out (0/1)
			{
				int fnc = (int)code_value();
				if (fnc == 0) pinMode(pin, INPUT);
				else if (fnc == 1) pinMode(pin, OUTPUT);
			}
			if (code_seen('V')) // Value (0/1)
			{
				int val = (int)code_value();
				if (val == 0) digitalWrite(pin, LOW);
				else if (val == 1) digitalWrite(pin, HIGH);
			}
			else
			{
				int val = (digitalRead(pin) != LOW)?1:0;
				printf("PIN%d=%d", pin, val);
			}
		}
	}
}
#endif //DEBUG_DCODES

#ifdef DEBUG_DCODE5

void dcode_5()
{
	printf_P(PSTR("D5 - Read/Write FLASH\n"));
	uint32_t address = 0x0000; //default 0x0000
	uint16_t count = 0x0400; //default 0x0400 (1kb block)
	if (code_seen('A')) // Address (0x00000-0x3ffff)
		address = (strchr_pointer[1] == 'x')?strtol(strchr_pointer + 2, 0, 16):(int)code_value();
	if (code_seen('C')) // Count (0x0001-0x2000)
		count = (int)code_value();
	address &= 0x3ffff;
	if (count > 0x2000) count = 0x2000;
	if ((address + count) > 0x40000) count = 0x40000 - address;
	bool bErase = false;
	bool bCopy = false;
	if (code_seen('E')) //Erase
		bErase = true;
	uint8_t data[16];
	if (code_seen('X')) // Data
	{
		count = parse_hex(strchr_pointer + 1, data, 16);
		if (count > 0) bCopy = true;
	}
	if (bErase || bCopy)
	{
		if (bErase)
		{
			printf_P(PSTR("%d bytes of FLASH at address %05x will be erased\n"), count, address);
		}
		if (bCopy)
		{
			printf_P(PSTR("%d  bytes will be written to FLASH at address %05x\n"), count, address);
		}
		cli();
		boot_app_magic = 0x55aa55aa;
		boot_app_flags = (bErase?(BOOT_APP_FLG_ERASE):0) | (bCopy?(BOOT_APP_FLG_COPY):0);
		boot_copy_size = (uint16_t)count;
		boot_dst_addr = (uint32_t)address;
		boot_src_addr = (uint32_t)(&data);
		bootapp_print_vars();
		wdt_enable(WDTO_15MS);
		while(1);
	}
	while (count)
	{
		print_hex_nibble(address >> 16);
		print_hex_word(address);
		putchar(' ');
		uint8_t countperline = 16;
		while (count && countperline)
		{
			uint8_t data = pgm_read_byte_far((uint8_t*)address++);
			putchar(' ');
			print_hex_byte(data);
			countperline--;
			count--;
		}
		putchar('\n');
	}
}
#endif //DEBUG_DCODE5

#ifdef DEBUG_DCODES

void dcode_6()
{
	LOG("D6 - Read/Write external FLASH\n");
}

void dcode_7()
{
	LOG("D7 - Read/Write Bootloader\n");
/*
	cli();
	boot_app_magic = 0x55aa55aa;
	boot_app_flags = BOOT_APP_FLG_ERASE | BOOT_APP_FLG_COPY | BOOT_APP_FLG_FLASH;
	boot_copy_size = (uint16_t)0xc00;
	boot_src_addr = (uint32_t)0x0003e400;
	boot_dst_addr = (uint32_t)0x0003f400;
	wdt_enable(WDTO_15MS);
	while(1);
*/
}

void dcode_8()
{
	printf_P(PSTR("D8 - Read/Write PINDA\n"));
	uint8_t cal_status = calibration_status_pinda();
	float temp_pinda = current_temperature_pinda;
	float offset_z = temp_compensation_pinda_thermistor_offset(temp_pinda);
	if ((strchr_pointer[1+1] == '?') || (strchr_pointer[1+1] == 0))
	{
		printf_P(PSTR("cal_status=%d\n"), cal_status?1:0);
		for (uint8_t i = 0; i < 6; i++)
		{
			uint16_t offs = 0;
			if (i > 0) offs = eeprom_read_word(((uint16_t*)EEPROM_PROBE_TEMP_SHIFT) + (i - 1));
			float foffs = ((float)offs) / axis_steps_per_unit[Z_AXIS];
			offs = 1000 * foffs;
			printf_P(PSTR("temp_pinda=%dC temp_shift=%dum\n"), 35 + i * 5, offs);
		}
	}
	else if (strchr_pointer[1+1] == '!')
	{
		cal_status = 1;
		eeprom_write_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, cal_status);
		eeprom_write_word(((uint16_t*)EEPROM_PROBE_TEMP_SHIFT) + 0,   8); //40C -  20um -   8usteps
		eeprom_write_word(((uint16_t*)EEPROM_PROBE_TEMP_SHIFT) + 1,  24); //45C -  60um -  24usteps
		eeprom_write_word(((uint16_t*)EEPROM_PROBE_TEMP_SHIFT) + 2,  48); //50C - 120um -  48usteps
		eeprom_write_word(((uint16_t*)EEPROM_PROBE_TEMP_SHIFT) + 3,  80); //55C - 200um -  80usteps
		eeprom_write_word(((uint16_t*)EEPROM_PROBE_TEMP_SHIFT) + 4, 120); //60C - 300um - 120usteps
	}
	else
	{
		if (code_seen('P')) // Pinda temperature [C]
			temp_pinda = code_value();
		offset_z = temp_compensation_pinda_thermistor_offset(temp_pinda);
		if (code_seen('Z')) // Z Offset [mm]
		{
			offset_z = code_value();
		}
	}
	printf_P(PSTR("temp_pinda=%d offset_z=%d.%03d\n"), (int)temp_pinda, (int)offset_z, ((int)(1000 * offset_z) % 1000));
}

const char* dcode_9_ADC_name(uint8_t i)
{
	switch (i)
	{
	case 0: return PSTR("TEMP_HEATER0");
	case 1: return PSTR("TEMP_HEATER1");
	case 2: return PSTR("TEMP_BED");
	case 3: return PSTR("TEMP_PINDA");
	case 4: return PSTR("VOLT_PWR");
	case 5: return PSTR("TEMP_AMBIENT");
	case 6: return PSTR("VOLT_BED");
	}
	return 0;
}

extern int current_temperature_raw[EXTRUDERS];
extern int current_temperature_bed_raw;
extern int current_temperature_raw_pinda;

#ifdef AMBIENT_THERMISTOR
extern int current_temperature_raw_ambient;
#endif //AMBIENT_THERMISTOR

#ifdef VOLT_PWR_PIN
extern int current_voltage_raw_pwr;
#endif //VOLT_PWR_PIN

#ifdef VOLT_BED_PIN
extern int current_voltage_raw_bed;
#endif //VOLT_BED_PIN

uint16_t dcode_9_ADC_val(uint8_t i)
{
	switch (i)
	{
	case 0: return current_temperature_raw[0];
	case 1: return 0;
	case 2: return current_temperature_bed_raw;
	case 3: return current_temperature_raw_pinda;
#ifdef VOLT_PWR_PIN
	case 4: return current_voltage_raw_pwr;
#endif //VOLT_PWR_PIN
#ifdef AMBIENT_THERMISTOR
	case 5: return current_temperature_raw_ambient;
#endif //AMBIENT_THERMISTOR
#ifdef VOLT_BED_PIN
	case 6: return current_voltage_raw_bed;
#endif //VOLT_BED_PIN
	}
	return 0;
}

void dcode_9()
{
	printf_P(PSTR("D9 - Read/Write ADC\n"));
	if ((strchr_pointer[1+1] == '?') || (strchr_pointer[1+1] == 0))
	{
		for (uint8_t i = 0; i < ADC_CHAN_CNT; i++)
			printf_P(PSTR("\tADC%d=%4d\t(%S)\n"), i, dcode_9_ADC_val(i) >> 4, dcode_9_ADC_name(i));
	}
	else
	{
		uint8_t index = 0xff;
		if (code_seen('I')) // index (index of used channel, not avr channel index)
			index = code_value();
		if (index < ADC_CHAN_CNT)
		{
			if (code_seen('V')) // value to be written as simulated
			{
				adc_sim_mask |= (1 << index);
				adc_values[index] = (((int)code_value()) << 4);
				printf_P(PSTR("ADC%d=%4d\n"), index, adc_values[index] >> 4);
			}
		}
	}
}

void dcode_10()
{//Tell the printer that XYZ calibration went OK
	LOG("D10 - XYZ calibration = OK\n");
	calibration_status_store(CALIBRATION_STATUS_LIVE_ADJUST); 
}

void dcode_12()
{//Time
	LOG("D12 - Time\n");

}


#ifdef TMC2130
#include "planner.h"
#include "tmc2130.h"
extern void st_synchronize();
/**
 * @brief D2130 Trinamic stepper controller
 * D2130<axis><command>[subcommand][value]
 *  * Axis
 *  * * 'X'
 *  * * 'Y'
 *  * * 'Z'
 *  * * 'E'
 *  * command
 *  * * '0' current off
 *  * * '1' current on
 *  * * '+' single step
 *  * * * value sereval steps
 *  * * '-' dtto oposite direction
 *  * * '?' read register
 *  * * * "mres"
 *  * * * "step"
 *  * * * "mscnt"
 *  * * * "mscuract"
 *  * * * "wave"
 *  * * '!' set register
 *  * * * "mres"
 *  * * * "step"
 *  * * * "wave"
 *  * * * *0, 180..250 meaning: off, 0.9..1.25, recommended value is 1.1
 *  * * '@' home calibrate axis
 *
 *  Example:
 *  D2130E?wave //print extruder microstep linearity compensation curve
 *  D2130E!wave0 //disable extruder linearity compensation curve, (sine curve is used)
 *  D2130E!wave220 // (sin(x))^1.1 extruder microstep compensation curve used
 */
void dcode_2130()
{
	printf_P(PSTR("D2130 - TMC2130\n"));
	uint8_t axis = 0xff;
	switch (strchr_pointer[1+4])
	{
	case 'X': axis = X_AXIS; break;
	case 'Y': axis = Y_AXIS; break;
	case 'Z': axis = Z_AXIS; break;
	case 'E': axis = E_AXIS; break;
	}
	if (axis != 0xff)
	{
		char ch_axis = strchr_pointer[1+4];
		if (strchr_pointer[1+5] == '0') { tmc2130_set_pwr(axis, 0); }
		else if (strchr_pointer[1+5] == '1') { tmc2130_set_pwr(axis, 1); }
		else if (strchr_pointer[1+5] == '+')
		{
			if (strchr_pointer[1+6] == 0)
			{
				tmc2130_set_dir(axis, 0);
				tmc2130_do_step(axis);
			}
			else
			{
				uint8_t steps = atoi(strchr_pointer + 1 + 6);
				tmc2130_do_steps(axis, steps, 0, 1000);
			}
		}
		else if (strchr_pointer[1+5] == '-')
		{
			if (strchr_pointer[1+6] == 0)
			{
				tmc2130_set_dir(axis, 1);
				tmc2130_do_step(axis);
			}
			else
			{
				uint8_t steps = atoi(strchr_pointer + 1 + 6);
				tmc2130_do_steps(axis, steps, 1, 1000);
			}
		}
		else if (strchr_pointer[1+5] == '?')
		{
			if (strcmp(strchr_pointer + 7, "mres") == 0) printf_P(PSTR("%c mres=%d\n"), ch_axis, tmc2130_mres[axis]);
			else if (strcmp(strchr_pointer + 7, "step") == 0) printf_P(PSTR("%c step=%d\n"), ch_axis, tmc2130_rd_MSCNT(axis) >> tmc2130_mres[axis]);
			else if (strcmp(strchr_pointer + 7, "mscnt") == 0) printf_P(PSTR("%c MSCNT=%d\n"), ch_axis, tmc2130_rd_MSCNT(axis));
			else if (strcmp(strchr_pointer + 7, "mscuract") == 0)
			{
				uint32_t val = tmc2130_rd_MSCURACT(axis);
				int curA = (val & 0xff);
				int curB = ((val >> 16) & 0xff);
				if ((val << 7) & 0x8000) curA -= 256;
				if ((val >> 9) & 0x8000) curB -= 256;
				printf_P(PSTR("%c MSCURACT=0x%08lx A=%d B=%d\n"), ch_axis, val, curA, curB);
			}
			else if (strcmp(strchr_pointer + 7, "wave") == 0)
			{
				tmc2130_get_wave(axis, 0, stdout);
			}
		}
		else if (strchr_pointer[1+5] == '!')
		{
			if (strncmp(strchr_pointer + 7, "step", 4) == 0)
			{
				uint8_t step = atoi(strchr_pointer + 11);
				uint16_t res = tmc2130_get_res(axis);
				tmc2130_goto_step(axis, step & (4*res - 1), 2, 1000, res);
			}
			else if (strncmp(strchr_pointer + 7, "mres", 4) == 0)
			{
				uint8_t mres = strchr_pointer[11] - '0';
				if ((mres >= 0) && (mres <= 8))
				{
					st_synchronize();
					uint16_t res = tmc2130_get_res(axis);
					uint16_t res_new = tmc2130_mres2usteps(mres);
					tmc2130_set_res(axis, res_new);
					if (res_new > res)
						axis_steps_per_unit[axis] *= (res_new / res);
					else
						axis_steps_per_unit[axis] /= (res / res_new);
				}
			}
			else if (strncmp(strchr_pointer + 7, "wave", 4) == 0)
			{
				uint8_t fac1000 = atoi(strchr_pointer + 11) & 0xffff;
				if (fac1000 < TMC2130_WAVE_FAC1000_MIN) fac1000 = 0;
				if (fac1000 > TMC2130_WAVE_FAC1000_MAX) fac1000 = TMC2130_WAVE_FAC1000_MAX;
				tmc2130_set_wave(axis, 247, fac1000);
				tmc2130_wave_fac[axis] = fac1000;
			}
		}
		else if (strchr_pointer[1+5] == '@')
		{
			tmc2130_home_calibrate(axis);
		}
	}
}
#endif //TMC2130

#ifdef PAT9125
void dcode_9125()
{
	LOG("D9125 - PAT9125\n");
	if ((strchr_pointer[1+4] == '?') || (strchr_pointer[1+4] == 0))
	{
//		printf("res_x=%d res_y=%d x=%d y=%d b=%d s=%d\n", pat9125_xres, pat9125_yres, pat9125_x, pat9125_y, pat9125_b, pat9125_s);
		printf("x=%d y=%d b=%d s=%d\n", pat9125_x, pat9125_y, pat9125_b, pat9125_s);
		return;
	}
	if (strchr_pointer[1+4] == '!')
	{
		pat9125_update();
		printf("x=%d y=%d b=%d s=%d\n", pat9125_x, pat9125_y, pat9125_b, pat9125_s);
		return;
	}
/*
	if (code_seen('R'))
	{
		unsigned char res = (int)code_value();
		LOG("pat9125_init(xres=yres=%d)=%d\n", res, pat9125_init(res, res));
	}
*/
	if (code_seen('X'))
	{
		pat9125_x = (int)code_value();
		LOG("pat9125_x=%d\n", pat9125_x);
	}
	if (code_seen('Y'))
	{
		pat9125_y = (int)code_value();
		LOG("pat9125_y=%d\n", pat9125_y);
	}
	if (code_seen('L'))
	{
		fsensor_log = (int)code_value();
		LOG("fsensor_log=%d\n", fsensor_log);
	}
}
#endif //PAT9125


#endif //DEBUG_DCODES
