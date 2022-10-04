//swi2c.c
#include "swi2c.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "stdbool.h"
#include "Configuration_var.h"
#include "pins.h"
#include "fastio.h"

#ifdef SWI2C_SCL

#define SWI2C_RMSK   0x01 //read mask (bit0 = 1)
#define SWI2C_WMSK   0x00 //write mask (bit0 = 0)
#define SWI2C_ASHF   0x01 //address shift (<< 1)
#define SWI2C_DMSK   0x7f //device address mask

static void __delay(void);
static void swi2c_start(void);
static void swi2c_stop(void);
// static void swi2c_ack(void);
static void swi2c_nack(void);
static uint8_t swi2c_wait_ack();
static uint8_t swi2c_read(void);
static void swi2c_write(uint8_t data);


void swi2c_init(void)
{
	SET_INPUT(SWI2C_SDA);
	WRITE(SWI2C_SDA, 1); //SDA must be input with pullups while we are not sure if the slave is outputing or not

	WRITE(SWI2C_SCL, 0);
	SET_OUTPUT(SWI2C_SCL); //SCL can be an output at all times. The bus is not in a multi-master configuration.

	for (uint8_t i = 0; i < 100; i++) //wait. Not sure what for, but wait anyway.
		__delay();

	for (uint8_t i = 0; i < 10; i++) { //send nack 10 times. This makes sure that the slave gets a nack regardless of it's state when we init the bus.
		swi2c_nack();
	}
	swi2c_stop(); //"release" the bus by sending a stop condition.

	SET_OUTPUT(SWI2C_SDA); //finally make the SDA line an output since the bus is idle for sure.
}

void swi2c_disable(void)
{
	SET_INPUT(SWI2C_SDA);
	WRITE(SWI2C_SDA, 0);
	SET_INPUT(SWI2C_SCL);
	WRITE(SWI2C_SCL, 0);
}

static void __delay(void)
{
	_delay_us(1.5);
}

static void swi2c_start(void)
{
	WRITE(SWI2C_SDA, 0);
	__delay();
	WRITE(SWI2C_SCL, 0);
	__delay();
}

static void swi2c_stop(void)
{
	WRITE(SWI2C_SCL, 1);
	__delay();
	WRITE(SWI2C_SDA, 1);
	__delay();
}

/*
static void swi2c_ack(void)
{
	WRITE(SWI2C_SDA, 0);
	__delay();
	WRITE(SWI2C_SCL, 1);
	__delay();
	WRITE(SWI2C_SCL, 0);
	__delay();
}
*/

static void swi2c_nack(void)
{
	WRITE(SWI2C_SDA, 1);
	__delay();
	WRITE(SWI2C_SCL, 1);
	__delay();
	WRITE(SWI2C_SCL, 0);
	__delay();
}

static uint8_t swi2c_wait_ack()
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

static uint8_t swi2c_read(void)
{
	WRITE(SWI2C_SDA, 1);
	__delay();
	SET_INPUT(SWI2C_SDA);
	uint8_t data = 0;
	for (uint8_t bit = 8; bit-- > 0;)
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

static void swi2c_write(uint8_t data)
{
	for (uint8_t bit = 8; bit-- > 0;)
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
	if (!swi2c_wait_ack()) { swi2c_stop(); return 1; }
	swi2c_stop();
	return 0;
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

#endif //SWI2C_SCL
