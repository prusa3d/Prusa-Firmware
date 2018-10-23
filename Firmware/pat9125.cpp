//pat9125.cpp
#include "Configuration_prusa.h"

#ifdef PAT9125
#include "pat91xx.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "config.h"
#include <stdio.h>

#ifdef PAT9125_SWSPI
#include "swspi.h"
#endif //PAT9125_SWSPI
#ifdef PAT9125_SWI2C
#include "swi2c.h"
#endif //PAT9125_SWI2C

extern FILE _uartout;


// Init sequence, address & value.
const PROGMEM uint8_t pat9125_init_seq1[] = {
	// Disable write protect.
	PAT9125_WP, 0x5a,
	// Set the X resolution to zero to let the sensor know that it could safely ignore movement in the X axis.
    PAT9125_RES_X, PAT91xx_XRES,
    // Set the Y resolution to a maximum (or nearly a maximum).
    PAT9125_RES_Y, PAT91xx_YRES,
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


uint8_t pat91xx_init(void)
{
#ifdef PAT9125_SWSPI
	swspi_init();
#endif //PAT9125_SWSPI
#ifdef PAT9125_SWI2C
	swi2c_init();
#endif //PAT9125_SWI2C
	// Verify that the sensor responds with its correct product ID.
	pat91xx_PID1 = pat91xx_rd_reg(PAT9125_PID1);
	pat91xx_PID2 = pat91xx_rd_reg(PAT9125_PID2);
	if ((pat91xx_PID1 != 0x31) || (pat91xx_PID2 != 0x91))
	{
		pat91xx_PID1 = pat91xx_rd_reg(PAT9125_PID1);
		pat91xx_PID2 = pat91xx_rd_reg(PAT9125_PID2);
		if ((pat91xx_PID1 != 0x31) || (pat91xx_PID2 != 0x91))
			return 0;
	}

#ifdef PAT9125_NEW_INIT
	// Switch to bank0, not allowed to perform OTS_RegWriteRead.
	pat91xx_wr_reg(PAT9125_BANK_SELECTION, 0);
	// Software reset (i.e. set bit7 to 1). It will reset to 0 automatically.
	// After the reset, OTS_RegWriteRead is not allowed.
	pat91xx_wr_reg(PAT9125_CONFIG, 0x97);
	// Wait until the sensor reboots.
	// Delay 1ms.
	_delay_us(1000);
	{
		const uint8_t *ptr = pat9125_init_seq1;
		for (;;) {
			const uint8_t addr = pgm_read_byte_near(ptr ++);
			if (addr == 0x0ff)
				break;
			if (! pat91xx_wr_reg_verify(addr, pgm_read_byte_near(ptr ++)))
				// Verification of the register write failed.
				return 0;
		}
	}
	// Delay 10ms.
	_delay_ms(10);
	// Switch to bank1, not allowed to perform OTS_RegWrite.
	pat91xx_wr_reg(PAT9125_BANK_SELECTION, 0x01);
	{
		const uint8_t *ptr = pat9125_init_seq2;
		for (;;) {
			const uint8_t addr = pgm_read_byte_near(ptr ++);
			if (addr == 0x0ff)
				break;
			if (! pat91xx_wr_reg_verify(addr, pgm_read_byte_near(ptr ++)))
				// Verification of the register write failed.
				return 0;
		}
	}
	// Switch to bank0, not allowed to perform OTS_RegWriteRead.
	pat91xx_wr_reg(PAT9125_BANK_SELECTION, 0x00);
	// Enable write protect.
	pat91xx_wr_reg(PAT9125_WP, 0x00);

	pat91xx_PID1 = pat91xx_rd_reg(PAT9125_PID1);
	pat91xx_PID2 = pat91xx_rd_reg(PAT9125_PID2);
#endif //PAT9125_NEW_INIT

	pat91xx_wr_reg(PAT9125_RES_X, PAT91xx_XRES);
	pat91xx_wr_reg(PAT9125_RES_Y, PAT91xx_YRES);
	fprintf_P(uartout, PSTR("PAT9125_RES_X=%hhu\n"), pat91xx_rd_reg(PAT9125_RES_X));
	fprintf_P(uartout, PSTR("PAT9125_RES_Y=%hhu\n"), pat91xx_rd_reg(PAT9125_RES_Y));
	return 1;
}

uint8_t pat91xx_update(void)
{
	if ((pat91xx_PID1 == 0x31) && (pat91xx_PID2 == 0x91))
	{
		uint8_t ucMotion = pat91xx_rd_reg(PAT9125_MOTION);
		pat91xx_b = pat91xx_rd_reg(PAT9125_FRAME);
		pat91xx_s = pat91xx_rd_reg(PAT9125_SHUTTER);
		if (pat91xx_PID1 == 0xff) return 0;
		if (ucMotion & 0x80)
		{
			uint8_t ucXL = pat91xx_rd_reg(PAT9125_DELTA_XL);
			uint8_t ucYL = pat91xx_rd_reg(PAT9125_DELTA_YL);
			uint8_t ucXYH = pat91xx_rd_reg(PAT9125_DELTA_XYH);
			if (pat91xx_PID1 == 0xff) return 0;
			int16_t iDX = ucXL | ((ucXYH << 4) & 0xf00);
			int16_t iDY = ucYL | ((ucXYH << 8) & 0xf00);
			if (iDX & 0x800) iDX -= 4096;
			if (iDY & 0x800) iDY -= 4096;
			pat91xx_x += iDX;
			pat91xx_y -= iDY; //negative number, because direction switching does not work
		}
		return 1;
	}
	return 0;
}

uint8_t pat91xx_update_y(void)
{
	if ((pat91xx_PID1 == 0x31) && (pat91xx_PID2 == 0x91))
	{
		uint8_t ucMotion = pat91xx_rd_reg(PAT9125_MOTION);
		if (pat91xx_PID1 == 0xff) return 0;
		if (ucMotion & 0x80)
		{
			uint8_t ucYL = pat91xx_rd_reg(PAT9125_DELTA_YL);
			uint8_t ucXYH = pat91xx_rd_reg(PAT9125_DELTA_XYH);
			if (pat91xx_PID1 == 0xff) return 0;
			int16_t iDY = ucYL | ((ucXYH << 8) & 0xf00);
			if (iDY & 0x800) iDY -= 4096;
			pat91xx_y -= iDY; //negative number, because direction switching does not work
		}
		return 1;
	}
	return 0;
}

uint8_t pat91xx_update_y2(void)
{
	if ((pat91xx_PID1 == 0x31) && (pat91xx_PID2 == 0x91))
	{
		uint8_t ucMotion = pat91xx_rd_reg(PAT9125_MOTION);
		if (pat91xx_PID1 == 0xff) return 0; //NOACK error
		if (ucMotion & 0x80)
		{
			int8_t dy = pat91xx_rd_reg(PAT9125_DELTA_YL);
			if (pat91xx_PID1 == 0xff) return 0; //NOACK error
			pat91xx_y -= dy; //negative number, because direction switching does not work
		}
		return 1;
	}
	return 0;
}

uint8_t pat91xx_rd_reg(uint8_t addr)
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
		pat91xx_PID1 = 0xff;
		pat91xx_PID2 = 0xff;
		return 0;
	}
#endif //PAT9125_SWI2C
	return data;
}

void pat91xx_wr_reg(uint8_t addr, uint8_t data)
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
		pat91xx_PID1 = 0xff;
		pat91xx_PID2 = 0xff;
		return;
	}
#endif //PAT9125_SWI2C
}

#endif // PAT9125
