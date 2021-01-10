//swi2c.c
#include "swi2c.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "stdbool.h"
#include "Configuration_prusa.h"
#include "pins.h"
#include "fastio.h"


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
	WRITE(SWI2C_SDA, 1);
	WRITE(SWI2C_SCL, 1);
	SET_OUTPUT(SWI2C_SDA);
	SET_OUTPUT(SWI2C_SCL);
	uint8_t i; for (i = 0; i < 100; i++)
		__delay();
}

void swi2c_start(void)
{
	WRITE(SWI2C_SDA, 0);
	__delay();
	WRITE(SWI2C_SCL, 0);
	__delay();
}

void swi2c_stop(void)
{
	WRITE(SWI2C_SCL, 1);
	__delay();
	WRITE(SWI2C_SDA, 1);
	__delay();
}

void swi2c_ack(void)
{
	WRITE(SWI2C_SDA, 0);
	__delay();
	WRITE(SWI2C_SCL, 1);
	__delay();
	WRITE(SWI2C_SCL, 0);
	__delay();
}

uint8_t swi2c_wait_ack()
{
	SET_INPUT(SWI2C_SDA);
	__delay();
//	WRITE(SWI2C_SDA, 1);
	__delay();
	WRITE(SWI2C_SCL, 1);
//	__delay();
	uint8_t ack = 0;
	uint16_t ackto = SWI2C_TMO;
	while (!(ack = (!READ(SWI2C_SDA))) && ackto--) __delay();
	WRITE(SWI2C_SCL, 0);
	__delay();
	SET_OUTPUT(SWI2C_SDA);
	__delay();
	WRITE(SWI2C_SDA, 0);
	__delay();
	return ack;
}

uint8_t swi2c_read(void)
{
	WRITE(SWI2C_SDA, 1);
	__delay();
	SET_INPUT(SWI2C_SDA);
	uint8_t data = 0;
	int8_t bit; for (bit = 7; bit >= 0; bit--)
	{
		WRITE(SWI2C_SCL, 1);
		__delay();
		data |= (READ(SWI2C_SDA)) << bit;
		WRITE(SWI2C_SCL, 0);
		__delay();
	}
	SET_OUTPUT(SWI2C_SDA);
	return data;
}

void swi2c_write(uint8_t data)
{
	int8_t bit; for (bit = 7; bit >= 0; bit--)
	{
		WRITE(SWI2C_SDA, data & _BV(bit));
		__delay();
		WRITE(SWI2C_SCL, 1);
		__delay();
		WRITE(SWI2C_SCL, 0);
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
