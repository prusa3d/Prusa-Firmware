//pat9125.c
#include "pat9125.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "config.h"
#include <stdio.h>
#include "Configuration_var.h"

#if defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)

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

#if defined(PAT9125_SWI2C)
#include "swi2c.h"
#elif defined(PAT9125_I2C)
#include "twi.h"
#else
#error unknown PAT9125 communication method
#endif


uint8_t pat9125_PID1 = 0;
uint8_t pat9125_PID2 = 0;
int16_t pat9125_x = 0;
int16_t pat9125_y = 0;
uint8_t pat9125_b = 0;
uint8_t pat9125_s = 0;


// Init sequence, address & value.
const PROGMEM uint8_t pat9125_init_bank0[] = {
	// Disable write protect.
	PAT9125_WP, 0x5a,
	// Set the X resolution to zero to let the sensor know that it could safely ignore movement in the X axis.
	PAT9125_RES_X, PAT9125_XRES,
	// Set the Y resolution to a maximum (or nearly a maximum).
	PAT9125_RES_Y, PAT9125_YRES,
	// Set data format and sensor orientation.
	PAT9125_ORIENTATION, ((PAT9125_12B_RES?0x04:0) | (PAT9125_INVERT_X?0x08:0) | (PAT9125_INVERT_Y?0x10:0) | (PAT9125_SWAP_XY?0x20:0)),
	
	// Now continues the magic sequence from the PAT912EL Application Note: Firmware Guides for Tracking Optimization.
	0x5e, 0x08,
	0x20, 0x64,
	0x2b, 0x6d,
	0x32, 0x2f,
	0xff //end of sequence
};

// Init sequence, address & value.
const PROGMEM uint8_t pat9125_init_bank1[] = {
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
	0xff //end of sequence
};


static uint8_t pat9125_rd_reg(uint8_t addr);
static void pat9125_wr_reg(uint8_t addr, uint8_t data);
static uint8_t pat9125_wr_reg_verify(uint8_t addr, uint8_t data);
static uint8_t pat9125_wr_seq(const uint8_t* seq);

extern FILE _uartout;
#define uartout (&_uartout)


uint8_t pat9125_probe()
{
#if defined(PAT9125_SWI2C)
    swi2c_init();
    return swi2c_check(PAT9125_I2C_ADDR) == 0;
#elif defined(PAT9125_I2C)
    twi_init();
    return twi_check(PAT9125_I2C_ADDR) == 0;
#endif
}

uint8_t pat9125_init(void)
{
	if (!pat9125_probe())
		return 0;

// Switch to bank0, not allowed to perform pat9125_wr_reg_verify on this register.
	pat9125_wr_reg(PAT9125_BANK_SELECTION, 0);

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

#if PAT9125_NEW_INIT
	// Software reset (i.e. set bit7 to 1). It will reset to 0 automatically.
	// pat9125_wr_reg_verify is not allowed because the register contents will change as soon as they are written. No point in verifying those.
	pat9125_wr_reg(PAT9125_CONFIG, 0x97);
	// Wait until the sensor reboots.
	_delay_ms(1);
	
	//Write init sequence in bank0. MUST ALREADY BE IN bank0.
	if (!pat9125_wr_seq(pat9125_init_bank0))
		return 0;
	
	_delay_ms(10); // not sure why this is here. But I'll allow it.
	
	// Switch to bank1, not allowed to perform pat9125_wr_reg_verify on this register.
	pat9125_wr_reg(PAT9125_BANK_SELECTION, 0x01);
	//Write init sequence in bank1. MUST ALREADY BE IN bank1.
	if (!pat9125_wr_seq(pat9125_init_bank1))
		return 0;
	
	// Switch to bank0, not allowed to perform pat9125_wr_reg_verify on this register.
	pat9125_wr_reg(PAT9125_BANK_SELECTION, 0x00);
	
	// Enable write protect.
	pat9125_wr_reg(PAT9125_WP, 0x00); //prevents writing to registers over 0x09

	pat9125_PID1 = pat9125_rd_reg(PAT9125_PID1);
	pat9125_PID2 = pat9125_rd_reg(PAT9125_PID2);

#else //PAT9125_NEW_INIT
	// Disable write protect.
	pat9125_wr_reg(PAT9125_WP, 0x5a); //allows writing to all registers
	
	pat9125_wr_reg(PAT9125_RES_X, PAT9125_XRES);
	pat9125_wr_reg(PAT9125_RES_Y, PAT9125_YRES);
	printf_P(PSTR("PAT9125_RES_X=%u\n"), pat9125_rd_reg(PAT9125_RES_X));
	printf_P(PSTR("PAT9125_RES_Y=%u\n"), pat9125_rd_reg(PAT9125_RES_Y));
	
	pat9125_wr_reg(PAT9125_ORIENTATION, ((PAT9125_12B_RES?0x04:0) | (PAT9125_INVERT_X?0x08:0) | (PAT9125_INVERT_Y?0x10:0) | (PAT9125_SWAP_XY?0x20:0)));
	
	// Enable write protect.
	pat9125_wr_reg(PAT9125_WP, 0x00); //prevents writing to registers over 0x09
#endif //PAT9125_NEW_INIT

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
			pat9125_y += iDY;
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
			pat9125_y += iDY;
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

static uint8_t pat9125_rd_reg(uint8_t addr)
{
	uint8_t data = 0;
#if defined(PAT9125_SWI2C)
	if (!swi2c_readByte_A8(PAT9125_I2C_ADDR, addr, &data)) //NO ACK error
        goto error;
#elif defined(PAT9125_I2C)
	if (twi_r8(PAT9125_I2C_ADDR,addr,&data))
        goto error;
#endif
	return data;

 error:
    pat9125_PID1 = 0xff;
    pat9125_PID2 = 0xff;
    return 0;
}

static void pat9125_wr_reg(uint8_t addr, uint8_t data)
{
#if defined(PAT9125_SWI2C)
	if (!swi2c_writeByte_A8(PAT9125_I2C_ADDR, addr, &data)) //NO ACK error
        goto error;
#elif defined(PAT9125_I2C)
	if (twi_w8(PAT9125_I2C_ADDR,addr,data))
        goto error;
#endif
    return;

 error:
    pat9125_PID1 = 0xff;
    pat9125_PID2 = 0xff;
    return;
}

static uint8_t pat9125_wr_reg_verify(uint8_t addr, uint8_t data)
{
	pat9125_wr_reg(addr, data);
	return pat9125_rd_reg(addr) == data;
}

static uint8_t pat9125_wr_seq(const uint8_t* seq)
{
	for (;;) {
		const uint8_t addr = pgm_read_byte(seq++);
		if (addr == 0xff)
			break;
		if (!pat9125_wr_reg_verify(addr, pgm_read_byte(seq++)))
			// Verification of the register write failed.
			return 0;
	}
	return 1;
}

#endif
