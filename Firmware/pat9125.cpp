#include "uni_avr_rpi.h"

#ifdef PAT9125

#include "pat9125.h"

#ifdef PAT9125_SWSPI
#include "swspi.h"
#endif //PAT9125_SWSPI
#ifdef PAT9125_SWI2C
#include "swi2c.h"
#endif //PAT9125_SWI2C
#ifdef PAT9125_HWI2C
#include <Wire.h>
#endif //PAT9125_HWI2C


unsigned char pat9125_PID1 = 0;
unsigned char pat9125_PID2 = 0;
int pat9125_x = 0;
int pat9125_y = 0;
unsigned char pat9125_b = 0;
unsigned char pat9125_s = 0;

// Init sequence, address & value.
const PROGMEM unsigned char pat9125_init_seq1[] = {
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
const PROGMEM unsigned char pat9125_init_seq2[] = {
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

int pat9125_init()
{
#ifdef PAT9125_SWSPI
	swspi_init();
#endif //PAT9125_SWSPI
#ifdef PAT9125_SWI2C
	swi2c_init(PAT9125_SWI2C_SDA, PAT9125_SWI2C_SCL, PAT9125_SWI2C_CFG);
#endif //PAT9125_SWI2C
#ifdef PAT9125_HWI2C
	Wire.begin();
#endif //PAT9125_HWI2C
	// Verify that the sensor responds with its correct product ID.
	pat9125_PID1 = pat9125_rd_reg(PAT9125_PID1);
	pat9125_PID2 = pat9125_rd_reg(PAT9125_PID2);
//	pat9125_PID1 = 0x31;
//	pat9125_PID2 = 0x91;
	if ((pat9125_PID1 != 0x31) || (pat9125_PID2 != 0x91))
	{
		pat9125_PID1 = pat9125_rd_reg(PAT9125_PID1);
		pat9125_PID2 = pat9125_rd_reg(PAT9125_PID2);
		if ((pat9125_PID1 != 0x31) || (pat9125_PID2 != 0x91))
			return 0;
	}
	// Switch to bank0, not allowed to perform OTS_RegWriteRead.
	pat9125_wr_reg(PAT9125_BANK_SELECTION, 0);
	// Software reset (i.e. set bit7 to 1). It will reset to 0 automatically.
	// After the reset, OTS_RegWriteRead is not allowed.
	pat9125_wr_reg(PAT9125_CONFIG, 0x97);
	// Wait until the sensor reboots.
	// Delay 1ms.
	delayMicroseconds(1000);
	{
		const unsigned char *ptr = pat9125_init_seq1;
		for (;;) {
			const unsigned char addr = pgm_read_byte_near(ptr ++);
			if (addr == 0x0ff)
				break;
			if (! pat9125_wr_reg_verify(addr, pgm_read_byte_near(ptr ++)))
				// Verification of the register write failed.
				return 0;
		}
	}
	// Delay 10ms.
	delayMicroseconds(10000);
	// Switch to bank1, not allowed to perform OTS_RegWrite.
	pat9125_wr_reg(PAT9125_BANK_SELECTION, 0x01);
	{
		const unsigned char *ptr = pat9125_init_seq2;
		for (;;) {
			const unsigned char addr = pgm_read_byte_near(ptr ++);
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
	return 1;
}

int pat9125_update()
{
	if ((pat9125_PID1 == 0x31) && (pat9125_PID2 == 0x91))
	{
		unsigned char ucMotion = pat9125_rd_reg(PAT9125_MOTION);
		pat9125_b = pat9125_rd_reg(PAT9125_FRAME);
		pat9125_s = pat9125_rd_reg(PAT9125_SHUTTER);
		if (pat9125_PID1 == 0xff) return 0;
		if (ucMotion & 0x80)
		{
			unsigned char ucXL = pat9125_rd_reg(PAT9125_DELTA_XL);
			unsigned char ucYL = pat9125_rd_reg(PAT9125_DELTA_YL);
			unsigned char ucXYH = pat9125_rd_reg(PAT9125_DELTA_XYH);
			if (pat9125_PID1 == 0xff) return 0;
			int iDX = ucXL | ((ucXYH << 4) & 0xf00);
			int iDY = ucYL | ((ucXYH << 8) & 0xf00);
			if (iDX & 0x800) iDX -= 4096;
			if (iDY & 0x800) iDY -= 4096;
			pat9125_x += iDX;
			pat9125_y -= iDY; //negative number, because direction switching does not work
		}
		return 1;
	}
	return 0;
}

int pat9125_update_y()
{
	if ((pat9125_PID1 == 0x31) && (pat9125_PID2 == 0x91))
	{
		unsigned char ucMotion = pat9125_rd_reg(PAT9125_MOTION);
		if (pat9125_PID1 == 0xff) return 0;
		if (ucMotion & 0x80)
		{
			unsigned char ucYL = pat9125_rd_reg(PAT9125_DELTA_YL);
			unsigned char ucXYH = pat9125_rd_reg(PAT9125_DELTA_XYH);
			if (pat9125_PID1 == 0xff) return 0;
			int iDY = ucYL | ((ucXYH << 8) & 0xf00);
			if (iDY & 0x800) iDY -= 4096;
			pat9125_y -= iDY; //negative number, because direction switching does not work
		}
		return 1;
	}
	return 0;
}

unsigned char pat9125_rd_reg(unsigned char addr)
{
//    printf_P(PSTR("pat9125_rd_reg 0x%hhx "), addr);
	unsigned char data = 0;
#ifdef PAT9125_SWSPI
	swspi_start();
	swspi_tx(addr & 0x7f);
	data = swspi_rx();
	swspi_stop();
#endif //PAT9125_SWSPI
#ifdef PAT9125_SWI2C
	int iret = swi2c_readByte_A8(PAT9125_I2C_ADDR, addr, &data);
	if (!iret) //NO ACK error
	{
		pat9125_PID1 = 0xff;
		pat9125_PID2 = 0xff;
//	    printf_P(PSTR("ERR\n"));
		return 0;
	}
//    printf_P(PSTR("0x%hhx OK\n"), data);
#endif //PAT9125_SWI2C
#ifdef PAT9125_HWI2C
	Wire.beginTransmission(PAT9125_I2C_ADDR);
	Wire.write(addr);
	Wire.endTransmission();
	if (Wire.requestFrom(PAT9125_I2C_ADDR, 1) == 1)
//	if (Wire.available())
		data = Wire.read();
#endif //PAT9125_HWI2C
	return data;
}

void pat9125_wr_reg(unsigned char addr, unsigned char data)
{
//    printf_P(PSTR("pat9125_wr_reg 0x%hhx 0x%hhx  "), addr, data);
#ifdef PAT9125_SWSPI
	swspi_start();
	swspi_tx(addr | 0x80);
	swspi_tx(data);
	swspi_stop();
#endif //PAT9125_SWSPI
#ifdef PAT9125_SWI2C
	int iret = swi2c_writeByte_A8(PAT9125_I2C_ADDR, addr, &data);
	if (!iret) //NO ACK error
	{
		pat9125_PID1 = 0xff;
		pat9125_PID2 = 0xff;
//	    printf_P(PSTR("ERR\n"));
		return;
	}
//    printf_P(PSTR("OK\n"));

#endif //PAT9125_SWI2C
#ifdef PAT9125_HWI2C
	Wire.beginTransmission(PAT9125_I2C_ADDR);
	Wire.write(addr);
	Wire.write(data);
	Wire.endTransmission();
#endif //PAT9125_HWI2C
}

bool pat9125_wr_reg_verify(unsigned char addr, unsigned char data)
{
	pat9125_wr_reg(addr, data);
	return pat9125_rd_reg(addr) == data;
}

#endif //PAT9125
