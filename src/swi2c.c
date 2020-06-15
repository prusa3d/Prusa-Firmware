//swi2c.c
#include "swi2c.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "Configuration_prusa.h"
#include "pins.h"
#include "io_atmega2560.h"


#define SWI2C_RMSK   0x01 //read mask (bit0 = 1)
#define SWI2C_WMSK   0x00 //write mask (bit0 = 0)
#define SWI2C_ASHF   0x01 //address shift (<< 1)
#define SWI2C_DMSK   0x7f //device address mask


void __delay(void)
{
	_delay_us(1.5);
}

void swi2c_init(void)
{
	PIN_OUT(SWI2C_SDA);
	PIN_OUT(SWI2C_SCL);
	PIN_SET(SWI2C_SDA);
	PIN_SET(SWI2C_SCL);
	uint8_t i; for (i = 0; i < 100; i++)
		__delay();
}

void swi2c_start(void)
{
	PIN_CLR(SWI2C_SDA);
	__delay();
	PIN_CLR(SWI2C_SCL);
	__delay();
}

void swi2c_stop(void)
{
	PIN_SET(SWI2C_SCL);
	__delay();
	PIN_SET(SWI2C_SDA);
	__delay();
}

void swi2c_ack(void)
{
	PIN_CLR(SWI2C_SDA);
	__delay();
	PIN_SET(SWI2C_SCL);
	__delay();
	PIN_CLR(SWI2C_SCL);
	__delay();
}

uint8_t swi2c_wait_ack()
{
	PIN_INP(SWI2C_SDA);
	__delay();
//	PIN_SET(SWI2C_SDA);
	__delay();
	PIN_SET(SWI2C_SCL);
//	__delay();
	uint8_t ack = 0;
	uint16_t ackto = SWI2C_TMO;
	while (!(ack = (PIN_GET(SWI2C_SDA)?0:1)) && ackto--) __delay();
	PIN_CLR(SWI2C_SCL);
	__delay();
	PIN_OUT(SWI2C_SDA);
	__delay();
	PIN_CLR(SWI2C_SDA);
	__delay();
	return ack;
}

uint8_t swi2c_read(void)
{
	PIN_SET(SWI2C_SDA);
	__delay();
	PIN_INP(SWI2C_SDA);
	uint8_t data = 0;
	int8_t bit; for (bit = 7; bit >= 0; bit--)
	{
		PIN_SET(SWI2C_SCL);
		__delay();
		data |= (PIN_GET(SWI2C_SDA)?1:0) << bit;
		PIN_CLR(SWI2C_SCL);
		__delay();
	}
	PIN_OUT(SWI2C_SDA);
	return data;
}

void swi2c_write(uint8_t data)
{
	int8_t bit; for (bit = 7; bit >= 0; bit--)
	{
		if (data & (1 << bit)) PIN_SET(SWI2C_SDA);
		else PIN_CLR(SWI2C_SDA);
		__delay();
		PIN_SET(SWI2C_SCL);
		__delay();
		PIN_CLR(SWI2C_SCL);
		__delay();
	}
}

uint8_t swi2c_check(uint8_t dev_addr)
{
	swi2c_start();
	swi2c_write((dev_addr & SWI2C_DMSK) << SWI2C_ASHF);
	if (!swi2c_wait_ack()) { swi2c_stop(); return 0; }
	swi2c_stop();
	return 1;
}

#ifdef SWI2C_A8 //8bit address

uint8_t swi2c_readByte_A8(uint8_t dev_addr, uint8_t addr, uint8_t* pbyte)
{
	swi2c_start();
	swi2c_write(SWI2C_WMSK | ((dev_addr & SWI2C_DMSK) << SWI2C_ASHF));
	if (!swi2c_wait_ack()) { swi2c_stop(); return 0; }
	swi2c_write(addr & 0xff);
	if (!swi2c_wait_ack()) return 0;
	swi2c_stop();
	swi2c_start();
	swi2c_write(SWI2C_RMSK | ((dev_addr & SWI2C_DMSK) << SWI2C_ASHF));
	if (!swi2c_wait_ack()) return 0;
	uint8_t byte = swi2c_read();
	swi2c_stop();
	if (pbyte) *pbyte = byte;
	return 1;
}

uint8_t swi2c_writeByte_A8(uint8_t dev_addr, uint8_t addr, uint8_t* pbyte)
{
	swi2c_start();
	swi2c_write(SWI2C_WMSK | ((dev_addr & SWI2C_DMSK) << SWI2C_ASHF));
	if (!swi2c_wait_ack()) { swi2c_stop(); return 0; }
	swi2c_write(addr & 0xff);
	if (!swi2c_wait_ack()) return 0;
	swi2c_write(*pbyte);
	if (!swi2c_wait_ack()) return 0;
	swi2c_stop();
	return 1;
}

#endif //SWI2C_A8

#ifdef SWI2C_A16 //16bit address

uint8_t swi2c_readByte_A16(uint8_t dev_addr, unsigned short addr, uint8_t* pbyte)
{
	swi2c_start();
	swi2c_write(SWI2C_WMSK | ((dev_addr & SWI2C_DMSK) << SWI2C_ASHF));
	if (!swi2c_wait_ack()) { swi2c_stop(); return 0; }
	swi2c_write(addr >> 8);
	if (!swi2c_wait_ack()) return 0;
	swi2c_write(addr & 0xff);
	if (!swi2c_wait_ack()) return 0;
	swi2c_stop();
	swi2c_start();
	swi2c_write(SWI2C_RMSK | ((dev_addr & SWI2C_DMSK) << SWI2C_ASHF));
	if (!swi2c_wait_ack()) return 0;
	uint8_t byte = swi2c_read();
	swi2c_stop();
	if (pbyte) *pbyte = byte;
	return 1;
}

uint8_t swi2c_writeByte_A16(uint8_t dev_addr, unsigned short addr, uint8_t* pbyte)
{
	swi2c_start();
	swi2c_write(SWI2C_WMSK | ((dev_addr & SWI2C_DMSK) << SWI2C_ASHF));
	if (!swi2c_wait_ack()) { swi2c_stop(); return 0; }
	swi2c_write(addr >> 8);
	if (!swi2c_wait_ack()) return 0;
	swi2c_write(addr & 0xff);
	if (!swi2c_wait_ack()) return 0;
	swi2c_write(*pbyte);
	if (!swi2c_wait_ack()) return 0;
	swi2c_stop();
	return 1;
}

#endif //SWI2C_A16
