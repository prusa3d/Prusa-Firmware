#include "pat9125.h"
#include "swspi.h"


#ifdef SWSPI_RPI
//	#include <bcm2835.h>
	#define DELAY(delay) usleep(delay)
#endif //SWSPI_RPI

#ifdef SWSPI_AVR
	#include "Arduino.h"
	#define DELAY(delay) delayMicroseconds(delay)
#endif //SWSPI_AVR

unsigned char ucPID1 = 0;
unsigned char ucPID2 = 0;
int pat9125_x = 0;
int pat9125_y = 0;

int pat9125_init(unsigned char xres, unsigned char yres)
{
	swspi_init();
	ucPID1 = pat9125_rd_reg(PAT9125_PID1);
	ucPID2 = pat9125_rd_reg(PAT9125_PID2);
	if ((ucPID1 != 0x31) || (ucPID2 != 0x91))
	{
		return 0;
	}
    pat9125_wr_reg(PAT9125_RES_X, xres);
    pat9125_wr_reg(PAT9125_RES_Y, yres);
	return 1;
}

int pat9125_update()
{
	if ((ucPID1 == 0x31) && (ucPID2 == 0x91))
	{
		unsigned char ucMotion = pat9125_rd_reg(PAT9125_MOTION);
		if (ucMotion & 0x80)
		{
			int iDX = pat9125_rd_reg(PAT9125_DELTA_XL);
			int iDY = pat9125_rd_reg(PAT9125_DELTA_YL);
			if (iDX >= 0x80) iDX = iDX - 256;
			if (iDY >= 0x80) iDY = iDY - 256;
			pat9125_x += iDX;
			pat9125_y += iDY;
			return 1;
		}
	}
	return 0;
}

unsigned char pat9125_rd_reg(unsigned char addr)
{
	swspi_start();
	DELAY(100);
	swspi_tx(addr & 0x7f);
	DELAY(100);
	unsigned char data = swspi_rx();
	swspi_stop();
	DELAY(100);
	return data;
}

void pat9125_wr_reg(unsigned char addr, unsigned char data)
{
	swspi_start();
	DELAY(100);
	swspi_tx(addr | 0x80);
	DELAY(100);
	swspi_tx(data);
	swspi_stop();
	DELAY(100);
}
