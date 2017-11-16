#include "Dcodes.h"
#include "Marlin.h"

#ifdef DEBUG_DCODES

#include "ConfigurationStore.h"
#include "cmdqueue.h"
#include "pat9125.h"
#include <avr/wdt.h>


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


inline void serial_print_hex_nibble(uint8_t val)
{
	MYSERIAL.write((val > 9)?(val - 10 + 'a'):(val + '0'));
}

void serial_print_hex_byte(uint8_t val)
{
	serial_print_hex_nibble(val >> 4);
	serial_print_hex_nibble(val & 15);
}

void serial_print_hex_word(uint16_t val)
{
	serial_print_hex_byte(val >> 8);
	serial_print_hex_byte(val & 255);
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

void dcode_0()
{
	if (*(strchr_pointer + 1) == 0) return;
	MYSERIAL.println("D0 - Reset");
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
	MYSERIAL.println("D1 - Clear EEPROM and RESET");
	cli();
	for (int i = 0; i < 8192; i++)
		eeprom_write_byte((unsigned char*)i, (unsigned char)0xff);
	wdt_enable(WDTO_15MS);
	while(1);
}

void dcode_2()
{
	MYSERIAL.println("D2 - Read/Write RAM");
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
			MYSERIAL.print(count, DEC);
			MYSERIAL.println(" bytes written to RAM at address ");
			serial_print_hex_word(address);
			MYSERIAL.write('\n');
		}
		else
			count = 0;
	}
	while (count)
	{
		serial_print_hex_word(address);
		MYSERIAL.write(' ');
		uint8_t countperline = 16;
		while (count && countperline)
		{
			uint8_t data = *((uint8_t*)address++);
			MYSERIAL.write(' ');
			serial_print_hex_byte(data);
			countperline--;
			count--;
		}
		MYSERIAL.write('\n');
	}
}

void dcode_3()
{
	MYSERIAL.println("D3 - Read/Write EEPROM");
	uint16_t address = 0x0000; //default 0x0000
	uint16_t count = 0x2000; //default 0x2000 (entire eeprom)
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
				eeprom_write_byte((uint8_t*)(address + i), data[i]);
			MYSERIAL.print(count, DEC);
			MYSERIAL.println(" bytes written to EEPROM at address ");
			serial_print_hex_word(address);
			MYSERIAL.write('\n');
		}
		else
			count = 0;
	}
	while (count)
	{
		serial_print_hex_word(address);
		MYSERIAL.write(' ');
		uint8_t countperline = 16;
		while (count && countperline)
		{
			uint8_t data = eeprom_read_byte((uint8_t*)address++);
			MYSERIAL.write(' ');
			serial_print_hex_byte(data);
			countperline--;
			count--;
		}
		MYSERIAL.write('\n');
	}
}

void dcode_4()
{
	MYSERIAL.println("D4 - Read/Write PIN");
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
				MYSERIAL.print("PIN");
				MYSERIAL.print(pin);
				MYSERIAL.print("=");
				MYSERIAL.println(val);
			}
		}
	}
}

void dcode_5()
{
	MYSERIAL.println("D5 - Read/Write FLASH");
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
			MYSERIAL.print(count, DEC);
			MYSERIAL.println(" bytes of FLASH at address ");
			serial_print_hex_word(address);
			MYSERIAL.write(" will be erased\n");
		}
		if (bCopy)
		{
			MYSERIAL.print(count, DEC);
			MYSERIAL.println(" bytes will be written to FLASH at address ");
			serial_print_hex_word(address);
			MYSERIAL.write('\n');
		}
		cli();
		boot_app_magic = 0x55aa55aa;
		boot_app_flags = (bErase?(BOOT_APP_FLG_ERASE):0) | (bCopy?(BOOT_APP_FLG_COPY):0);
		boot_copy_size = (uint16_t)count;
		boot_dst_addr = (uint32_t)address;
		boot_src_addr = (uint32_t)(&data);
		wdt_enable(WDTO_15MS);
		while(1);
	}
	while (count)
	{
		serial_print_hex_nibble(address >> 16);
		serial_print_hex_word(address);
		MYSERIAL.write(' ');
		uint8_t countperline = 16;
		while (count && countperline)
		{
			uint8_t data = pgm_read_byte_far((uint8_t*)address++);
			MYSERIAL.write(' ');
			serial_print_hex_byte(data);
			countperline--;
			count--;
		}
		MYSERIAL.write('\n');
	}
}

void dcode_6()
{
	MYSERIAL.println("D6 - Read/Write external FLASH");
}

void dcode_7()
{
	MYSERIAL.println("D7 - Read/Write Bootloader");
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
	MYSERIAL.println("D8 - Read/Write PINDA");
	uint8_t cal_status = calibration_status_pinda();
	float temp_pinda = current_temperature_pinda;
	float offset_z = temp_compensation_pinda_thermistor_offset(temp_pinda);
	if ((strchr_pointer[1+1] == '?') || (strchr_pointer[1+1] == 0))
	{
		MYSERIAL.print("cal_status=");
		MYSERIAL.println(cal_status?"1":"0");
		for (uint8_t i = 0; i < 6; i++)
		{
			MYSERIAL.print("temp_pinda=");
			MYSERIAL.print(35 + i * 5, DEC);
			MYSERIAL.print("C, temp_shift=");
			uint16_t offs = 0;
			if (i > 0) offs = eeprom_read_word(((uint16_t*)EEPROM_PROBE_TEMP_SHIFT) + (i - 1));
			MYSERIAL.print(((float)offs) / axis_steps_per_unit[Z_AXIS], 3);
			MYSERIAL.println("mm");
		}
	}
	else if (strchr_pointer[1+1] == '!')
	{
		cal_status = 1;
		eeprom_write_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, cal_status);
		eeprom_write_word(((uint16_t*)EEPROM_PROBE_TEMP_SHIFT) + 0, 50); //40C - 
		eeprom_write_word(((uint16_t*)EEPROM_PROBE_TEMP_SHIFT) + 1, 100); //45C - 
		eeprom_write_word(((uint16_t*)EEPROM_PROBE_TEMP_SHIFT) + 2, 150); //50C - 
		eeprom_write_word(((uint16_t*)EEPROM_PROBE_TEMP_SHIFT) + 3, 200); //55C - 
		eeprom_write_word(((uint16_t*)EEPROM_PROBE_TEMP_SHIFT) + 4, 250); //60C - 
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
	MYSERIAL.print("temp_pinda=");
	MYSERIAL.println(temp_pinda);
	MYSERIAL.print("offset_z=");
	MYSERIAL.println(offset_z, 3);
}

void dcode_10()
{//Tell the printer that XYZ calibration went OK
	MYSERIAL.println("D10 - XYZ calibration = OK");
	calibration_status_store(CALIBRATION_STATUS_LIVE_ADJUST); 
}

void dcode_12()
{//Reset Filament error, Power loss and crash counter ( Do it before every print and you can get stats for the print )
	MYSERIAL.println("D12 - Reset failstat counters");
    eeprom_update_byte((uint8_t*)EEPROM_CRASH_COUNT, 0x00);
    eeprom_update_byte((uint8_t*)EEPROM_FERROR_COUNT, 0x00);
    eeprom_update_byte((uint8_t*)EEPROM_POWER_COUNT, 0x00);
}

void dcode_2130()
{
//	printf("test");
}

void dcode_9125()
{
	MYSERIAL.println("D9125 - PAT9125");
	if ((strchr_pointer[1+4] == '?') || (strchr_pointer[1+4] == 0))
	{
		MYSERIAL.print("res_x=");
		MYSERIAL.print(pat9125_xres, DEC);
		MYSERIAL.print(" res_y=");
		MYSERIAL.print(pat9125_yres, DEC);
		MYSERIAL.print(" x=");
		MYSERIAL.print(pat9125_x, DEC);
		MYSERIAL.print(" y=");
		MYSERIAL.print(pat9125_y, DEC);
		MYSERIAL.print(" b=");
		MYSERIAL.print(pat9125_b, DEC);
		MYSERIAL.print(" s=");
		MYSERIAL.println(pat9125_s, DEC);
		return;
	}
	if (strchr_pointer[1+4] == '!')
	{
		pat9125_update();
		MYSERIAL.print("x=");
		MYSERIAL.print(pat9125_x, DEC);
		MYSERIAL.print(" y=");
		MYSERIAL.print(pat9125_y, DEC);
		MYSERIAL.print(" b=");
		MYSERIAL.print(pat9125_b, DEC);
		MYSERIAL.print(" s=");
		MYSERIAL.println(pat9125_s, DEC);
		return;
	}
	if (code_seen('R'))
	{
		unsigned char res = (int)code_value();
		MYSERIAL.print("pat9125_init(xres=yres=");
		MYSERIAL.print(res, DEC);
		MYSERIAL.print(")=");
		MYSERIAL.println(pat9125_init(res, res), DEC);
	}
	if (code_seen('X'))
	{
		pat9125_x = (int)code_value();
		MYSERIAL.print("pat9125_x=");
		MYSERIAL.print(pat9125_x, DEC);
	}
	if (code_seen('Y'))
	{
		pat9125_y = (int)code_value();
		MYSERIAL.print("pat9125_y=");
		MYSERIAL.print(pat9125_y, DEC);
	}
	if (code_seen('L'))
	{
		fsensor_log = (int)code_value();
	}
}

#endif //DEBUG_DCODES
