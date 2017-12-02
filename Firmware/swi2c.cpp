#include "uni_avr_rpi.h"

#ifdef SWI2C
#include "swi2c.h"

#ifdef __AVR
unsigned char swi2c_sda = 20; // SDA pin
unsigned char swi2c_scl = 21; // SCL pin
#endif //__AVR

#ifdef __RPI
unsigned char swi2c_sda = 2; // SDA pin
unsigned char swi2c_scl = 3; // SCL pin
#endif //__RPI

unsigned char swi2c_cfg = 0xb1; // config
//  bit0..3 = clock delay factor = 1 << 1 = 2 [us]
//  bit4..7 = ack timeout factor = 1 << 11 = 2048 [cycles]

#define SWI2C_SDA    swi2c_sda
#define SWI2C_SCL    swi2c_scl
#define SWI2C_RMSK   0x01 //read mask (bit0 = 1)
#define SWI2C_WMSK   0x00 //write mask (bit0 = 0)
#define SWI2C_ASHF   0x01 //address shift (<< 1)
#define SWI2C_DMSK   0x7f //device address mask


void swi2c_init(unsigned char sda, unsigned char scl, unsigned char cfg)
{
	swi2c_sda = sda;
	swi2c_scl = scl;
	swi2c_cfg = cfg;
	GPIO_OUT(SWI2C_SDA);
	GPIO_OUT(SWI2C_SCL);
	GPIO_SET(SWI2C_SDA);
	GPIO_SET(SWI2C_SCL);
	DELAY(1000);
}

void swi2c_start(int delay)
{
	GPIO_CLR(SWI2C_SDA);
	DELAY(delay);
	GPIO_CLR(SWI2C_SCL);
	DELAY(delay);
}

void swi2c_stop(int delay)
{
	GPIO_SET(SWI2C_SCL);
	DELAY(delay);
	GPIO_SET(SWI2C_SDA);
	DELAY(delay);
}

void swi2c_ack(int delay)
{
	GPIO_CLR(SWI2C_SDA);
	DELAY(delay);
	GPIO_SET(SWI2C_SCL);
	DELAY(delay);
	GPIO_CLR(SWI2C_SCL);
	DELAY(delay);
}

int swi2c_wait_ack(int delay, int ackto)
{
	GPIO_INP(SWI2C_SDA);
	DELAY(delay);
//	GPIO_SET(SWI2C_SDA);
	DELAY(delay);
	GPIO_SET(SWI2C_SCL);
//	DELAY(delay);
	int ack = 0;
	while (!(ack = !GPIO_GET(SWI2C_SDA)) && ackto--) DELAY(delay);
	GPIO_CLR(SWI2C_SCL);
	DELAY(delay);
	GPIO_OUT(SWI2C_SDA);
	DELAY(delay);
	GPIO_CLR(SWI2C_SDA);
	DELAY(delay);
	return ack;
}

unsigned char swi2c_read(int delay)
{
	GPIO_SET(SWI2C_SDA);
	DELAY(delay);
	GPIO_INP(SWI2C_SDA);
	unsigned char data = 0;
	int bit; for (bit = 7; bit >= 0; bit--)
	{
		GPIO_SET(SWI2C_SCL);
		DELAY(delay);
		data |= GPIO_GET(SWI2C_SDA) << bit;
		GPIO_CLR(SWI2C_SCL);
		DELAY(delay);
	}
	GPIO_OUT(SWI2C_SDA);
	return data;
}

void swi2c_write(int delay, unsigned char data)
{
	int bit; for (bit = 7; bit >= 0; bit--)
	{
		if (data & (1 << bit)) GPIO_SET(SWI2C_SDA);
		else GPIO_CLR(SWI2C_SDA);
		DELAY(delay);
		GPIO_SET(SWI2C_SCL);
		DELAY(delay);
		GPIO_CLR(SWI2C_SCL);
		DELAY(delay);
	}
}

int swi2c_check(unsigned char dev_addr)
{
	int delay = 1 << (swi2c_cfg & 0xf);
	int tmout = 1 << (swi2c_cfg >> 4);
	swi2c_start(delay);
	swi2c_write(delay, (dev_addr & SWI2C_DMSK) << SWI2C_ASHF);
	if (!swi2c_wait_ack(delay, tmout)) { swi2c_stop(delay); return 0; }
	swi2c_stop(delay);
	return 1;
}

#ifdef SWI2C_A8 //8bit address

int swi2c_readByte_A8(unsigned char dev_addr, unsigned char addr, unsigned char* pbyte)
{
	int delay = 1 << (swi2c_cfg & 0xf);
	int tmout = 1 << (swi2c_cfg >> 4);
	swi2c_start(delay);
	swi2c_write(delay, SWI2C_WMSK | ((dev_addr & SWI2C_DMSK) << SWI2C_ASHF));
	if (!swi2c_wait_ack(delay, tmout)) { swi2c_stop(delay); return 0; }
	swi2c_write(delay, addr & 0xff);
	if (!swi2c_wait_ack(delay, tmout)) return 0;
	swi2c_stop(delay);
	swi2c_start(delay);
	swi2c_write(delay, SWI2C_RMSK | ((dev_addr & SWI2C_DMSK) << SWI2C_ASHF));
	if (!swi2c_wait_ack(delay, tmout)) return 0;
	unsigned char byte = swi2c_read(delay);
	swi2c_stop(delay);
	if (pbyte) *pbyte = byte;
	return 1;
}

int swi2c_writeByte_A8(unsigned char dev_addr, unsigned char addr, unsigned char* pbyte)
{
	int delay = 1 << (swi2c_cfg & 0xf);
	int tmout = 1 << (swi2c_cfg >> 4);
	swi2c_start(delay);
	swi2c_write(delay, SWI2C_WMSK | ((dev_addr & SWI2C_DMSK) << SWI2C_ASHF));
	if (!swi2c_wait_ack(delay, tmout)) { swi2c_stop(delay); return 0; }
	swi2c_write(delay, addr & 0xff);
	if (!swi2c_wait_ack(delay, tmout)) return 0;
	swi2c_write(delay, *pbyte);
	if (!swi2c_wait_ack(delay, tmout)) return 0;
	swi2c_stop(delay);
	return 1;
}

#endif //SWI2C_A8

#ifdef SWI2C_A16 //16bit address

int swi2c_readByte_A16(unsigned char dev_addr, unsigned short addr, unsigned char* pbyte)
{
	int delay = 1 << (swi2c_cfg & 0xf);
	int tmout = 1 << (swi2c_cfg >> 4);
	swi2c_start(delay);
	swi2c_write(delay, SWI2C_WMSK | ((dev_addr & SWI2C_DMSK) << SWI2C_ASHF));
	if (!swi2c_wait_ack(delay, tmout)) { swi2c_stop(delay); return 0; }
	swi2c_write(delay, addr >> 8);
	if (!swi2c_wait_ack(delay, tmout)) return 0;
	swi2c_write(delay, addr & 0xff);
	if (!swi2c_wait_ack(delay, tmout)) return 0;
	swi2c_stop(delay);
	swi2c_start(delay);
	swi2c_write(delay, SWI2C_RMSK | ((dev_addr & SWI2C_DMSK) << SWI2C_ASHF));
	if (!swi2c_wait_ack(delay, tmout)) return 0;
	unsigned char byte = swi2c_read(delay);
	swi2c_stop(delay);
	if (pbyte) *pbyte = byte;
	return 1;
}

int swi2c_writeByte_A16(unsigned char dev_addr, unsigned short addr, unsigned char* pbyte)
{
	int delay = 1 << (swi2c_cfg & 0xf);
	int tmout = 1 << (swi2c_cfg >> 4);
	swi2c_start(delay);
	swi2c_write(delay, SWI2C_WMSK | ((dev_addr & SWI2C_DMSK) << SWI2C_ASHF));
	if (!swi2c_wait_ack(delay, tmout)) { swi2c_stop(delay); return 0; }
	swi2c_write(delay, addr >> 8);
	if (!swi2c_wait_ack(delay, tmout)) return 0;
	swi2c_write(delay, addr & 0xff);
	if (!swi2c_wait_ack(delay, tmout)) return 0;
	swi2c_write(delay, *pbyte);
	if (!swi2c_wait_ack(delay, tmout)) return 0;
	swi2c_stop(delay);
	return 1;
}

#endif //SWI2C_A16


#endif //SWI2C