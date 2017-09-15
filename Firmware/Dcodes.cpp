#include "Dcodes.h"
#include "Marlin.h"
#include "cmdqueue.h"

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
		asm volatile("jmp 0x1e000");
	else //reset
		asm volatile("jmp 0x00000");
/*
	cli(); //disable interrupts
	wdt_reset(); //reset watchdog
	WDTCSR = (1<<WDCE) | (1<<WDE); //enable watchdog
	WDTCSR = (1<<WDE) | (1<<WDP0); //30ms prescaler
	while(1); //wait for reset
*/
}

void dcode_1()
{
	MYSERIAL.println("D1 - Clear EEPROM");
	cli();
	for (int i = 0; i < 4096; i++)
		eeprom_write_byte((unsigned char*)i, (unsigned char)0);
	sei();
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
			MYSERIAL.println(" bytes written to RAM at addres ");
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
			MYSERIAL.println(" bytes written to EEPROM at addres ");
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


