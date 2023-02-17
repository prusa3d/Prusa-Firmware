#include "Marlin.h"
#include "Dcodes.h"
#include "Configuration.h"
#include "language.h"
#include "cmdqueue.h"
#include <stdio.h>
#include <avr/pgmspace.h>

#define SHOW_TEMP_ADC_VALUES
#include "temperature.h"


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

// debug range address type (fits all SRAM/PROGMEM/XFLASH memory ranges)
#if defined(DEBUG_DCODE6) || defined(DEBUG_DCODES) || defined(XFLASH_DUMP)
#include "xflash.h"
#include "xflash_layout.h"

#define DADDR_SIZE 32
typedef uint32_t daddr_t; // XFLASH requires 24 bits
#else
#define DADDR_SIZE 16
typedef uint16_t daddr_t;
#endif

void print_hex_word(daddr_t val)
{
#if DADDR_SIZE > 16
    print_hex_byte((val >> 16) & 0xFF);
#endif
    print_hex_byte((val >> 8) & 0xFF);
    print_hex_byte(val & 0xFF);
}

int parse_hex(const char* hex, uint8_t* data, int count)
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


enum class dcode_mem_t:uint8_t { sram, eeprom, progmem, xflash };

void print_mem(daddr_t address, daddr_t count, dcode_mem_t type, uint8_t countperline = 16)
{
#if defined(DEBUG_DCODE6) || defined(DEBUG_DCODES) || defined(XFLASH_DUMP)
    if(type == dcode_mem_t::xflash)
        XFLASH_SPI_ENTER();
#endif
	while (count)
	{
		print_hex_word(address);
		putchar(' ');
		uint8_t count_line = countperline;
		while (count && count_line)
		{
			uint8_t data = 0;
			switch (type)
			{
			case dcode_mem_t::sram: data = *((uint8_t*)address); break;
			case dcode_mem_t::eeprom: data = eeprom_read_byte((uint8_t*)address); break;
			case dcode_mem_t::progmem: break;
#if defined(DEBUG_DCODE6) || defined(DEBUG_DCODES) || defined(XFLASH_DUMP)
            case dcode_mem_t::xflash: xflash_rd_data(address, &data, 1); break;
#else
            case dcode_mem_t::xflash: break;
#endif
			}
            ++address;
			putchar(' ');
			print_hex_byte(data);
			count_line--;
			count--;

            // sporadically call manage_heater, but only when interrupts are enabled (meaning
            // print_mem is called by D2). Don't do anything otherwise: we are inside a crash
            // handler where memory & stack needs to be preserved!
            if((SREG & (1 << SREG_I)) && !((uint16_t)count % 8192))
                manage_heater();
		}
		putchar('\n');
	}
}

// TODO: this only handles SRAM/EEPROM 16bit addresses
void write_mem(uint16_t address, uint16_t count, const uint8_t* data, const dcode_mem_t type)
{
    for (uint16_t i = 0; i < count; i++)
    {
        switch (type)
        {
        case dcode_mem_t::sram: *((uint8_t*)address) = data[i]; break;
        case dcode_mem_t::eeprom: eeprom_write_byte((uint8_t*)address, data[i]); break;
        case dcode_mem_t::progmem: break;
        case dcode_mem_t::xflash: break;
        }
        ++address;
    }
}

void dcode_core(daddr_t addr_start, const daddr_t addr_end, const dcode_mem_t type,
                uint8_t dcode, const char* type_desc)
{
    KEEPALIVE_STATE(NOT_BUSY);
    DBG(_N("D%d - Read/Write %S\n"), dcode, type_desc);
    daddr_t count = -1; // RW the entire space by default
    if (code_seen('A'))
        addr_start = (strchr_pointer[1] == 'x')?strtol(strchr_pointer + 2, 0, 16):(int)code_value();
    if (code_seen('C'))
        count = code_value_long();
    if (addr_start > addr_end)
        addr_start = addr_end;
    if ((addr_start + count) > addr_end || (addr_start + count) < addr_start)
        count = addr_end - addr_start;
    if (code_seen('X'))
    {
        uint8_t data[16];
        count = parse_hex(strchr_pointer + 1, data, 16);
        write_mem(addr_start, count, data, type);
#if DADDR_SIZE > 16
        DBG(_N("%lu bytes written to %S at address 0x%04lx\n"), count, type_desc, addr_start);
#else
        DBG(_N("%u bytes written to %S at address 0x%08x\n"), count, type_desc, addr_start);
#endif
    }
    print_mem(addr_start, count, type);
}

#if defined DEBUG_DCODE3 || defined DEBUG_DCODES
#define EEPROM_SIZE 0x1000
    /*!
    ### D3 - Read/Write EEPROM <a href="https://reprap.org/wiki/G-code#D3:_Read.2FWrite_EEPROM">D3: Read/Write EEPROM</a>
    This command can be used without any additional parameters. It will read the entire eeprom.
    #### Usage
    
        D3 [ A | C | X ]
    
    #### Parameters
    - `A` - Address (x0000-x0fff)
    - `C` - Count (1-4096)
    - `X` - Data (hex)
	
	#### Notes
	- The hex address needs to be lowercase without the 0 before the x
	- Count is decimal 
	- The hex data needs to be lowercase
	
    */
void dcode_3()
{
    dcode_core(0, EEPROM_SIZE, dcode_mem_t::eeprom, 3, _N("EEPROM"));
}
#endif //DEBUG_DCODE3


#include "ConfigurationStore.h"
#include "cmdqueue.h"
#include "pat9125.h"
#include "adc.h"
#include "temperature.h"
#include <avr/wdt.h>
#include "bootapp.h"

#if 0
extern float current_temperature_pinda;
extern float axis_steps_per_unit[NUM_AXIS];


#define LOG(args...) printf(args)
#endif //0
#define LOG(args...)

    /*!
    *
    ### D-1 - Endless Loop <a href="https://reprap.org/wiki/G-code#G28:_Move_to_Origin_.28Home.29">D-1: Endless Loop</a>
      
          D-1
      
    *
    */
void dcode__1()
{
	DBG(_N("D-1 - Endless loop\n"));
//	cli();
	while (1);
}

#ifdef DEBUG_DCODES

    /*!
    ### D0 - Reset <a href="https://reprap.org/wiki/G-code#D0:_Reset">D0: Reset</a>
    #### Usage
    
        D0 [ B ]
    
    #### Parameters
    - `B` - Bootloader
    */
void dcode_0()
{
	if (*(strchr_pointer + 1) == 0) return;
	LOG("D0 - Reset\n");
	if (code_seen('B')) //bootloader
	{
		softReset();
	}
	else //reset
	{
#ifndef _NO_ASM
		asm volatile("jmp 0x00000");
#endif //_NO_ASM
	}
}

    /*!
    *
    ### D1 - Clear EEPROM and RESET <a href="https://reprap.org/wiki/G-code#D1:_Clear_EEPROM_and_RESET">D1: Clear EEPROM and RESET</a>
      
          D1
      
    *
    */
void dcode_1()
{
	LOG("D1 - Clear EEPROM and RESET\n");
	cli();
	for (int i = 0; i < 8192; i++)
		eeprom_write_byte((unsigned char*)i, (unsigned char)0xff);
	softReset();
}
#endif

#if defined DEBUG_DCODE2 || defined DEBUG_DCODES
    /*!
    ### D2 - Read/Write RAM <a href="https://reprap.org/wiki/G-code#D2:_Read.2FWrite_RAM">D3: Read/Write RAM</a>
    This command can be used without any additional parameters. It will read the entire RAM.
    #### Usage

        D2 [ A | C | X ]

    #### Parameters
    - `A` - Address (x0000-x21ff)
    - `C` - Count (1-8704)
    - `X` - Data

	#### Notes
	- The hex address needs to be lowercase without the 0 before the x
	- Count is decimal
	- The hex data needs to be lowercase

    */
void dcode_2()
{
    dcode_core(RAMSTART, RAMEND+1, dcode_mem_t::sram, 2, _N("SRAM"));
}
#endif

#ifdef DEBUG_DCODES
    /*!
    
    ### D4 - Read/Write PIN <a href="https://reprap.org/wiki/G-code#D4:_Read.2FWrite_PIN">D4: Read/Write PIN</a>
    To read the digital value of a pin you need only to define the pin number.
    #### Usage
    
        D4 [ P | F | V ]
    
    #### Parameters
    - `P` - Pin (0-255)
    - `F` - Function in/out (0/1)
    - `V` - Value (0/1)
    */
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

#if defined DEBUG_DCODE5 || defined DEBUG_DCODES

    /*!
    ### D5 - Read/Write FLASH <a href="https://reprap.org/wiki/G-code#D5:_Read.2FWrite_FLASH">D5: Read/Write Flash</a>
    This command can be used without any additional parameters. It will read the 1kb FLASH.
    #### Usage
    
        D5 [ A | C | X | E ]
    
    #### Parameters
    - `A` - Address (x00000-x3ffff)
    - `C` - Count (1-8192)
    - `X` - Data (hex)
    - `E` - Erase
 	
	#### Notes
	- The hex address needs to be lowercase without the 0 before the x
	- Count is decimal 
	- The hex data needs to be lowercase
	
   */
void dcode_5()
{
	puts_P(PSTR("D5 - Read/Write FLASH"));
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
		softReset();
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

#if defined(XFLASH) && (defined DEBUG_DCODE6 || defined DEBUG_DCODES)
    /*!
    ### D6 - Read/Write external FLASH <a href="https://reprap.org/wiki/G-code#D6:_Read.2FWrite_external_FLASH">D6: Read/Write external Flash</a>
    This command can be used without any additional parameters. It will read the entire XFLASH.
    #### Usage

        D6 [ A | C | X ]

    #### Parameters
    - `A` - Address (x0000-x3ffff)
    - `C` - Count (1-262144)
    - `X` - Data

	#### Notes
	- The hex address needs to be lowercase without the 0 before the x
	- Count is decimal
	- The hex data needs to be lowercase
	- Writing is currently not implemented
    */
void dcode_6()
{
    dcode_core(0x0, XFLASH_SIZE, dcode_mem_t::xflash, 6, _N("XFLASH"));
}
#endif

#ifdef DEBUG_DCODES
    /*!
    ### D7 - Read/Write Bootloader <a href="https://reprap.org/wiki/G-code#D7:_Read.2FWrite_Bootloader">D7: Read/Write Bootloader</a>
    Reserved
    */
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
	softReset();
*/
}

    /*!
    ### D8 - Read/Write PINDA <a href="https://reprap.org/wiki/G-code#D8:_Read.2FWrite_PINDA">D8: Read/Write PINDA</a>
    #### Usage
    
        D8 [ ? | ! | P | Z ]
    
    #### Parameters
    - `?` - Read PINDA temperature shift values
    - `!` - Reset PINDA temperature shift values to default
    - `P` - Pinda temperature [C]
    - `Z` - Z Offset [mm]
    */
void dcode_8()
{
	puts_P(PSTR("D8 - Read/Write PINDA"));
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
			float foffs = ((float)offs) / cs.axis_steps_per_unit[Z_AXIS];
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

    /*!
    ### D9 - Read ADC <a href="https://reprap.org/wiki/G-code#D9:_Read.2FWrite_ADC">D9: Read ADC</a>
    #### Usage
    
        D9 [ I | V ]
    
    #### Parameters
    - `I` - ADC channel index 
        - `0` - Heater 0 temperature
        - `1` - Heater 1 temperature
        - `2` - Bed temperature
        - `3` - PINDA temperature
        - `4` - PWR voltage
        - `5` - Ambient temperature
        - `6` - BED voltage
    - `V` Value to be written as simulated
    */
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

uint16_t dcode_9_ADC_val(uint8_t i)
{
	switch (i)
	{
#ifdef SHOW_TEMP_ADC_VALUES
	case 0: return current_temperature_raw[0];
#endif //SHOW_TEMP_ADC_VALUES
	case 1: return 0;
#ifdef SHOW_TEMP_ADC_VALUES
	case 2: return current_temperature_bed_raw;
#endif //SHOW_TEMP_ADC_VALUES
#ifdef PINDA_THERMISTOR
	case 3: return current_temperature_raw_pinda;
#endif //PINDA_THERMISTOR
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
	puts_P(PSTR("D9 - Read/Write ADC"));
	if ((strchr_pointer[1+1] == '?') || (strchr_pointer[1+1] == 0))
	{
		for (uint8_t i = 0; i < ADC_CHAN_CNT; i++)
			printf_P(PSTR("\tADC%d=%4d\t(%S)\n"), i, dcode_9_ADC_val(i) >> 4, dcode_9_ADC_name(i));
	}
#if 0
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
				adc_values[index] = ((uint16_t)code_value_short() << 4);
				printf_P(PSTR("ADC%d=%4d\n"), index, adc_values[index] >> 4);
			}
		}
	}
#endif
}

    /*!
    ### D10 - Set XYZ calibration = OK <a href="https://reprap.org/wiki/G-code#D10:_Set_XYZ_calibration_.3D_OK">D10: Set XYZ calibration = OK</a>
    */
void dcode_10()
{//Tell the printer that XYZ calibration went OK
	LOG("D10 - XYZ calibration = OK\n");
	calibration_status_set(CALIBRATION_STATUS_XYZ);
}

    /*!
    ### D12 - Time <a href="https://reprap.org/wiki/G-code#D12:_Time">D12: Time</a>
    Writes the current time in the log file.
    */

void dcode_12()
{//Time
	LOG("D12 - Time\n");

}

#ifdef HEATBED_ANALYSIS
    /*!
    ### D80 - Bed check <a href="https://reprap.org/wiki/G-code#D80:_Bed_check">D80: Bed check</a>
    This command will log data to SD card file "mesh.txt".
    #### Usage
    
        D80 [ E | F | G | H | I | J ]
    
    #### Parameters
    - `E` - Dimension X (default 40)
    - `F` - Dimention Y (default 40)
    - `G` - Points X (default 40)
    - `H` - Points Y (default 40)
    - `I` - Offset X (default 74)
    - `J` - Offset Y (default 34)
  */
void dcode_80()
{
	float dimension_x = 40;
	float dimension_y = 40;
	int points_x = 40;
	int points_y = 40;
	float offset_x = 74;
	float offset_y = 33;

	if (code_seen('E')) dimension_x = code_value();
	if (code_seen('F')) dimension_y = code_value();
	if (code_seen('G')) {points_x = code_value(); }
	if (code_seen('H')) {points_y = code_value(); }
	if (code_seen('I')) {offset_x = code_value(); }
	if (code_seen('J')) {offset_y = code_value(); }
	printf_P(PSTR("DIM X: %f\n"), dimension_x);
	printf_P(PSTR("DIM Y: %f\n"), dimension_y);
	printf_P(PSTR("POINTS X: %d\n"), points_x);
	printf_P(PSTR("POINTS Y: %d\n"), points_y);
	printf_P(PSTR("OFFSET X: %f\n"), offset_x);
	printf_P(PSTR("OFFSET Y: %f\n"), offset_y);
		bed_check(dimension_x,dimension_y,points_x,points_y,offset_x,offset_y);
}


    /*!
    ### D81 - Bed analysis <a href="https://reprap.org/wiki/G-code#D81:_Bed_analysis">D80: Bed analysis</a>
    This command will log data to SD card file "wldsd.txt".
    #### Usage
    
        D81 [ E | F | G | H | I | J ]
    
    #### Parameters
    - `E` - Dimension X (default 40)
    - `F` - Dimention Y (default 40)
    - `G` - Points X (default 40)
    - `H` - Points Y (default 40)
    - `I` - Offset X (default 74)
    - `J` - Offset Y (default 34)
  */
void dcode_81()
{
	float dimension_x = 40;
	float dimension_y = 40;
	int points_x = 40;
	int points_y = 40;
	float offset_x = 74;
	float offset_y = 33;

	if (code_seen('E')) dimension_x = code_value();
	if (code_seen('F')) dimension_y = code_value();
	if (code_seen("G")) { strchr_pointer+=1; points_x = code_value(); }
	if (code_seen("H")) { strchr_pointer+=1; points_y = code_value(); }
	if (code_seen("I")) { strchr_pointer+=1; offset_x = code_value(); }
	if (code_seen("J")) { strchr_pointer+=1; offset_y = code_value(); }
	
	bed_analysis(dimension_x,dimension_y,points_x,points_y,offset_x,offset_y);
	
}

#endif //HEATBED_ANALYSIS

    /*!
    ### D106 - Print measured fan speed for different pwm values <a href="https://reprap.org/wiki/G-code#D106:_Print_measured_fan_speed_for_different_pwm_values">D106: Print measured fan speed for different pwm values</a>
    */
void dcode_106()
{
	for (int i = 255; i > 0; i = i - 5) {
		fanSpeed = i;
		//delay_keep_alive(2000);
		for (int j = 0; j < 100; j++) {
			delay_keep_alive(100);
			}
			printf_P(_N("%d: %d\n"), i, fan_speed[1]);
	}
}

#ifdef TMC2130
#include "planner.h"
#include "tmc2130.h"
extern void st_synchronize();
    /*!
    ### D2130 - Trinamic stepper controller <a href="https://reprap.org/wiki/G-code#D2130:_Trinamic_stepper_controller">D2130: Trinamic stepper controller</a>
    @todo Please review by owner of the code. RepRap Wiki Gcode needs to be updated after review of owner as well.
    
    #### Usage
    
        D2130 [ Axis | Command | Subcommand | Value ]
    
    #### Parameters
    - Axis
      - `X` - X stepper driver
      - `Y` - Y stepper driver
      - `Z` - Z stepper driver
      - `E` - Extruder stepper driver
    - Commands
      - `0`   - Current off
      - `1`   - Current on
      - `+`   - Single step
      - `-`   - Single step oposite direction
      - `NNN` - Value sereval steps
      - `?`   - Read register
      - Subcommands for read register
        - `mres`     - Micro step resolution. More information in datasheet '5.5.2 CHOPCONF – Chopper Configuration'
        - `step`     - Step
        - `mscnt`    - Microstep counter. More information in datasheet '5.5 Motor Driver Registers'
        - `mscuract` - Actual microstep current for motor. More information in datasheet '5.5 Motor Driver Registers'
        - `wave`     - Microstep linearity compensation curve
      - `!`   - Set register
      - Subcommands for set register
        - `mres`     - Micro step resolution
        - `step`     - Step
        - `wave`     - Microstep linearity compensation curve
        - Values for set register
          - `0, 180 --> 250` - Off
          - `0.9 --> 1.25`   - Valid values (recommended is 1.1)
      - `@`   - Home calibrate axis
    
    Examples:
      
          D2130E?wave
      
      Print extruder microstep linearity compensation curve
      
          D2130E!wave0
      
      Disable extruder linearity compensation curve, (sine curve is used)
      
          D2130E!wave220
      
      (sin(x))^1.1 extruder microstep compensation curve used
    
    Notes:
      For more information see https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC2130_datasheet.pdf
    *
	*/
void dcode_2130()
{
	puts_P(PSTR("D2130 - TMC2130"));
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
				if (mres <= 8)
				{
					st_synchronize();
					uint16_t res = tmc2130_get_res(axis);
					uint16_t res_new = tmc2130_mres2usteps(mres);
					tmc2130_set_res(axis, res_new);
					if (res_new > res)
						cs.axis_steps_per_unit[axis] *= (res_new / res);
					else
						cs.axis_steps_per_unit[axis] /= (res / res_new);
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

#if defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
    /*!
    ### D9125 - PAT9125 filament sensor <a href="https://reprap.org/wiki/G-code#D9:_Read.2FWrite_ADC">D9125: PAT9125 filament sensor</a>
    #### Usage
    
        D9125 [ ? | ! | R | X | Y | L ]
    
    #### Parameters
    - `?` - Print values
    - `!` - Print values
    - `R` - Resolution. Not active in code
    - `X` - X values
    - `Y` - Y values
    */
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
}
#endif //defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)

#endif //DEBUG_DCODES

#ifdef XFLASH_DUMP
#include "xflash_dump.h"

void dcode_20()
{
    if(code_seen('E'))
        xfdump_full_dump_and_reset();
    else
    {
        unsigned long ts = _millis();
        xfdump_dump();
        ts = _millis() - ts;
        DBG(_N("dump completed in %lums\n"), ts);
    }
}

void dcode_21()
{
    if(!xfdump_check_state())
        DBG(_N("no dump available\n"));
    else
    {
        KEEPALIVE_STATE(NOT_BUSY);
        DBG(_N("D21 - read crash dump\n"));
        print_mem(DUMP_OFFSET, sizeof(dump_t), dcode_mem_t::xflash);
    }
}

void dcode_22()
{
    if(!xfdump_check_state())
        DBG(_N("no dump available\n"));
    else
    {
        xfdump_reset();
        DBG(_N("dump cleared\n"));
    }
}
#endif

#ifdef EMERGENCY_SERIAL_DUMP
#include "asm.h"
#include "xflash_dump.h"

bool emergency_serial_dump = false;

void dcode_23()
{
    if(code_seen('E'))
        serial_dump_and_reset(dump_crash_reason::manual);
    else
    {
        emergency_serial_dump = !code_seen('R');
        SERIAL_ECHOPGM("serial dump ");
        SERIAL_ECHOLNRPGM(emergency_serial_dump? _N("enabled"): _N("disabled"));
    }
}

void __attribute__((noinline)) serial_dump_and_reset(dump_crash_reason reason)
{
    uint16_t sp;
    uint32_t pc;

    // we're being called from a live state, so shut off interrupts ...
    cli();

    // sample SP/PC
    sp = SP;
    pc = GETPC();

    // extend WDT long enough to allow writing the entire stream
    wdt_enable(WDTO_8S);

    // ... and heaters
    WRITE(FAN_PIN, HIGH);
    disable_heater();

    // this function can also be called from within a corrupted state, so not use
    // printf family of functions that use the heap or grow the stack.
    SERIAL_ECHOLNPGM("D23 - emergency serial dump");
    SERIAL_ECHOPGM("error: ");
    MYSERIAL.print((uint8_t)reason, DEC);
    SERIAL_ECHOPGM(" 0x");
    MYSERIAL.print(pc, HEX);
    SERIAL_ECHOPGM(" 0x");
    MYSERIAL.println(sp, HEX);

    print_mem(0, RAMEND+1, dcode_mem_t::sram);
    SERIAL_ECHOLNRPGM(MSG_OK);

    // reset soon
    softReset();
}
#endif
