//pat9130.cpp
#include "Configuration_prusa.h"

#ifdef PAT9130
#include "pat91xx.h"
#include "Arduino.h"
#include "swspi.h"

extern FILE _uartout;


uint8_t pat91xx_init(void)
{
digitalWrite(PAT9130_CS_PIN,HIGH);                // PORTxn <- 1, after it DDxn <- 1
pinMode(PAT9130_CS_PIN,OUTPUT);

swspi_init(SWSPI_MISO,SWSPI_MOSI,SWSPI_SCK,SWSPI_CONFIG);

	// Verify that the sensor responds with its correct product ID.
	pat91xx_PID1 = pat91xx_rd_reg(PAT9130_PID1);
	pat91xx_PID2 = pat91xx_rd_reg(PAT9130_PID2);
	if ((pat91xx_PID1 != _PAT9130_PID1) || (pat91xx_PID2 != _PAT9130_PID2))
			return 0;

	pat91xx_wr_reg(PAT9130_WP, _PAT9130_WE);      // WriteProtect Disable
	pat91xx_wr_reg(PAT9130_RES_X, PAT91xx_XRES);
	pat91xx_wr_reg(PAT9130_RES_Y, PAT91xx_YRES);
	pat91xx_wr_reg(PAT9130_WP, _PAT9130_WP);      // WriteProtect Enable
	fprintf_P(uartout, PSTR("PAT9130_RES_X=%hhu\n"), pat91xx_rd_reg(PAT9130_RES_X));
	fprintf_P(uartout, PSTR("PAT9130_RES_Y=%hhu\n"), pat91xx_rd_reg(PAT9130_RES_Y));
	return 1;
}

uint8_t pat91xx_update(void)
{
	if ((pat91xx_PID1 == _PAT9130_PID1) && (pat91xx_PID2 == _PAT9130_PID2))
	{
		uint8_t ucMotion = pat91xx_rd_reg(PAT9130_MOTION);
		pat91xx_b = pat91xx_rd_reg(PAT9130_FRAME);
		pat91xx_s = pat91xx_rd_reg(PAT9130_SHUTTER);
//-//		if (pat91xx_PID1 == 0xff) return 0;
		if (ucMotion & 0x80)
		{
               int16_t iDY=pat91xx_rd_reg(PAT9130_DELTA_YH)<<8;
               iDY|=pat91xx_rd_reg(PAT9130_DELTA_YL);
               int16_t iDX=pat91xx_rd_reg(PAT9130_DELTA_XH)<<8;
               iDX|=pat91xx_rd_reg(PAT9130_DELTA_XL);
//-//			if (pat91xx_PID1 == 0xff) return 0;
			pat91xx_x += iDX;
			pat91xx_y -= iDY; //negative number, because direction switching does not work
		}
		return 1;
	}
	return 0;
}

uint8_t pat91xx_update_y(void)
{
	if ((pat91xx_PID1 == _PAT9130_PID1) && (pat91xx_PID2 == _PAT9130_PID2))
	{
		uint8_t ucMotion = pat91xx_rd_reg(PAT9130_MOTION);
//-//		if (pat91xx_PID1 == 0xff) return 0;
		if (ucMotion & 0x80)
		{
               int16_t iDY=pat91xx_rd_reg(PAT9130_DELTA_YH)<<8;
               iDY|=pat91xx_rd_reg(PAT9130_DELTA_YL);
//-//			if (pat91xx_PID1 == 0xff) return 0;
			pat91xx_y -= iDY; //negative number, because direction switching does not work
		}
		return 1;
	}
	return 0;
}

uint8_t pat91xx_update_y2(void)
{
return(pat91xx_update_y());
}


uint8_t pat91xx_rd_reg(uint8_t nRegister)
{
uint8_t nData;

digitalWrite(PAT9130_CS_PIN,LOW);                 // SS\ -> L
swspi_tx(nRegister&PAT9130_READ_MASK);            // MSB <- 0
nData=swspi_rx();
digitalWrite(PAT9130_CS_PIN,HIGH);                // SS\ -> H
return(nData);
}

void pat91xx_wr_reg(uint8_t nRegister,uint8_t nData)
{
digitalWrite(PAT9130_CS_PIN,LOW);                 // SS\ -> L
swspi_tx(nRegister|PAT9130_WRITE_MASK);           // MSB <- 1
swspi_tx(nData);
digitalWrite(PAT9130_CS_PIN,HIGH);                // SS\ -> H
}

#endif // PAT9130
