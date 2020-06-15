//pat9125.c
#include "pat9125.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "config.h"
#include <stdio.h>


//PAT9125 registers
#define PAT9125_PID1			0x00
#define PAT9125_PID2			0x01
#define PAT9125_MOTION			0x02
#define PAT9125_DELTA_XL		0x03
#define PAT9125_DELTA_YL		0x04
#define PAT9125_MODE			0x05
#define PAT9125_CONFIG			0x06
#define PAT9125_WP				0x09
#define PAT9125_SLEEP1			0x0a
#define PAT9125_SLEEP2			0x0b
#define PAT9125_RES_X			0x0d
#define PAT9125_RES_Y			0x0e
#define PAT9125_DELTA_XYH		0x12
#define PAT9125_SHUTTER			0x14
#define PAT9125_FRAME			0x17
#define PAT9125_ORIENTATION		0x19
#define PAT9125_BANK_SELECTION	0x7f


#ifdef PAT9125_SWSPI
#include "swspi.h"
#endif //PAT9125_SWSPI
#ifdef PAT9125_SWI2C
#include "swi2c.h"
#endif //PAT9125_SWI2C


uint8_t pat9125_PID1 = 0;
uint8_t pat9125_PID2 = 0;
int16_t pat9125_x = 0;
int16_t pat9125_y = 0;
uint8_t pat9125_b = 0;
uint8_t pat9125_s = 0;


// Init sequence, address & value.
const PROGMEM uint8_t pat9125_init_seq1[] = {
	// Disable write protect.
	PAT9125_WP, 0x5a,
	// Set the X resolution to zero to let the sensor know that it could safely ignore movement in the X axis.
    PAT9125_RES_X, PAT9125_XRES,
    // Set the Y resolution to a maximum (or nearly a maximum).
    PAT9125_RES_Y, PAT9125_YRES,
    // Set 12-bit X/Y data format.
    PAT9125_ORIENTATION, 0x04,
//	PAT9125_ORIENTATION, 0x04 | (xinv?0x08:0) | (yinv?0x10:0), //!? direction switching does not work
    // Now continues the magic sequence from the PAT912EL Application Note: Firmware Guides for Tracking Optimization.
    0x5e, 0x08,
    0x20, 0x64,
    0x2b, 0x6d,
    0x32, 0x2f,
    // stopper
    0x0ff
};

// Init sequence, address & value.
const PROGMEM uint8_t pat9125_init_seq2[] = {
	// Magic sequence to enforce full frame rate of the sensor.
	0x06, 0x028,
	0x33, 0x0d0,
	0x36, 0x0c2,
	0x3e, 0x001,
	0x3f, 0x015,
	0x41, 0x032,
	0x42, 0x03b,
	0x43, 0x0f2,
	0x44, 0x03b,
	0x45, 0x0f2,
	0x46, 0x022,
	0x47, 0x03b,
	0x48, 0x0f2,
	0x49, 0x03b,
	0x4a, 0x0f0,
	0x58, 0x098,
	0x59, 0x00c,
	0x5a, 0x008,
	0x5b, 0x00c,
	0x5c, 0x008,
	0x61, 0x010,
	0x67, 0x09b,
	0x6e, 0x022,
	0x71, 0x007,
	0x72, 0x008,
	// stopper
    0x0ff
};


uint8_t pat9125_rd_reg(uint8_t addr);
void pat9125_wr_reg(uint8_t addr, uint8_t data);
uint8_t pat9125_wr_reg_verify(uint8_t addr, uint8_t data);

extern FILE _uartout;
#define uartout (&_uartout)


uint8_t pat9125_init(void)
{
#ifdef PAT9125_SWSPI
	swspi_init();
#endif //PAT9125_SWSPI
#ifdef PAT9125_SWI2C
	swi2c_init();
#endif //PAT9125_SWI2C
	// Verify that the sensor responds with its correct product ID.
	pat9125_PID1 = pat9125_rd_reg(PAT9125_PID1);
	pat9125_PID2 = pat9125_rd_reg(PAT9125_PID2);
	if ((pat9125_PID1 != 0x31) || (pat9125_PID2 != 0x91))
	{
		pat9125_PID1 = pat9125_rd_reg(PAT9125_PID1);
		pat9125_PID2 = pat9125_rd_reg(PAT9125_PID2);
		if ((pat9125_PID1 != 0x31) || (pat9125_PID2 != 0x91))
			return 0;
	}

#ifdef PAT9125_NEW_INIT
	// Switch to bank0, not allowed to perform OTS_RegWriteRead.
	pat9125_wr_reg(PAT9125_BANK_SELECTION, 0);
	// Software reset (i.e. set bit7 to 1). It will reset to 0 automatically.
	// After the reset, OTS_RegWriteRead is not allowed.
	pat9125_wr_reg(PAT9125_CONFIG, 0x97);
	// Wait until the sensor reboots.
	// Delay 1ms.
	_delay_us(1000);
	{
		const uint8_t *ptr = pat9125_init_seq1;
		for (;;) {
			const uint8_t addr = pgm_read_byte_near(ptr ++);
			if (addr == 0x0ff)
				break;
			if (! pat9125_wr_reg_verify(addr, pgm_read_byte_near(ptr ++)))
				// Verification of the register write failed.
				return 0;
		}
	}
	// Delay 10ms.
	_delay_ms(10);
	// Switch to bank1, not allowed to perform OTS_RegWrite.
	pat9125_wr_reg(PAT9125_BANK_SELECTION, 0x01);
	{
		const uint8_t *ptr = pat9125_init_seq2;
		for (;;) {
			const uint8_t addr = pgm_read_byte_near(ptr ++);
			if (addr == 0x0ff)
				break;
			if (! pat9125_wr_reg_verify(addr, pgm_read_byte_near(ptr ++)))
				// Verification of the register write failed.
				return 0;
		}
	}
	// Switch to bank0, not allowed to perform OTS_RegWriteRead.
	pat9125_wr_reg(PAT9125_BANK_SELECTION, 0x00);
	// Enable write protect.
	pat9125_wr_reg(PAT9125_WP, 0x00);

	pat9125_PID1 = pat9125_rd_reg(PAT9125_PID1);
	pat9125_PID2 = pat9125_rd_reg(PAT9125_PID2);
#endif //PAT9125_NEW_INIT

	pat9125_wr_reg(PAT9125_RES_X, PAT9125_XRES);
	pat9125_wr_reg(PAT9125_RES_Y, PAT9125_YRES);
	fprintf_P(uartout, PSTR("PAT9125_RES_X=%hhu\n"), pat9125_rd_reg(PAT9125_RES_X));
	fprintf_P(uartout, PSTR("PAT9125_RES_Y=%hhu\n"), pat9125_rd_reg(PAT9125_RES_Y));
	return 1;
}

uint8_t pat9125_update(void)
{
	if ((pat9125_PID1 == 0x31) && (pat9125_PID2 == 0x91))
	{
		uint8_t ucMotion = pat9125_rd_reg(PAT9125_MOTION);
		pat9125_b = pat9125_rd_reg(PAT9125_FRAME);
		pat9125_s = pat9125_rd_reg(PAT9125_SHUTTER);
		if (pat9125_PID1 == 0xff) return 0;
		if (ucMotion & 0x80)
		{
			uint16_t ucXL = pat9125_rd_reg(PAT9125_DELTA_XL);
			uint16_t ucYL = pat9125_rd_reg(PAT9125_DELTA_YL);
			uint16_t ucXYH = pat9125_rd_reg(PAT9125_DELTA_XYH);
			if (pat9125_PID1 == 0xff) return 0;
			int16_t iDX = ucXL | ((ucXYH << 4) & 0xf00);
			int16_t iDY = ucYL | ((ucXYH << 8) & 0xf00);
			if (iDX & 0x800) iDX -= 4096;
			if (iDY & 0x800) iDY -= 4096;
			pat9125_x += iDX;
			pat9125_y -= iDY; //negative number, because direction switching does not work
		}
		return 1;
	}
	return 0;
}

uint8_t pat9125_update_y(void)
{
	if ((pat9125_PID1 == 0x31) && (pat9125_PID2 == 0x91))
	{
		uint8_t ucMotion = pat9125_rd_reg(PAT9125_MOTION);
		if (pat9125_PID1 == 0xff) return 0;
		if (ucMotion & 0x80)
		{
			uint16_t ucYL = pat9125_rd_reg(PAT9125_DELTA_YL);
			uint16_t ucXYH = pat9125_rd_reg(PAT9125_DELTA_XYH);
			if (pat9125_PID1 == 0xff) return 0;
			int16_t iDY = ucYL | ((ucXYH << 8) & 0xf00);
			if (iDY & 0x800) iDY -= 4096;
			pat9125_y -= iDY; //negative number, because direction switching does not work
		}
		return 1;
	}
	return 0;
}

uint8_t pat9125_update_bs(void)
{
	if ((pat9125_PID1 == 0x31) && (pat9125_PID2 == 0x91))
	{
		pat9125_b = pat9125_rd_reg(PAT9125_FRAME);
		pat9125_s = pat9125_rd_reg(PAT9125_SHUTTER);
		if (pat9125_PID1 == 0xff) return 0;
		return 1;
	}
	return 0;
}

uint8_t pat9125_rd_reg(uint8_t addr)
{
	uint8_t data = 0;
#ifdef PAT9125_SWSPI
	swspi_start();
	swspi_tx(addr & 0x7f);
	data = swspi_rx();
	swspi_stop();
#endif //PAT9125_SWSPI
#ifdef PAT9125_SWI2C
	if (!swi2c_readByte_A8(PAT9125_I2C_ADDR, addr, &data)) //NO ACK error
	{
		pat9125_PID1 = 0xff;
		pat9125_PID2 = 0xff;
		return 0;
	}
#endif //PAT9125_SWI2C
	return data;
}

void pat9125_wr_reg(uint8_t addr, uint8_t data)
{
#ifdef PAT9125_SWSPI
	swspi_start();
	swspi_tx(addr | 0x80);
	swspi_tx(data);
	swspi_stop();
#endif //PAT9125_SWSPI
#ifdef PAT9125_SWI2C
	if (!swi2c_writeByte_A8(PAT9125_I2C_ADDR, addr, &data)) //NO ACK error
	{
		pat9125_PID1 = 0xff;
		pat9125_PID2 = 0xff;
		return;
	}
#endif //PAT9125_SWI2C
}

uint8_t pat9125_wr_reg_verify(uint8_t addr, uint8_t data)
{
	pat9125_wr_reg(addr, data);
	return pat9125_rd_reg(addr) == data;
}
