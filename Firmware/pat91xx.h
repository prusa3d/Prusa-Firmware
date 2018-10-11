//pat91xx.h
#ifndef PAT91xx_H
#define PAT91xx_H

#include <inttypes.h>
#include "Configuration_prusa.h"


#ifdef PAT9125
#define PAT91xx_XRES      0                       // 0 - 255
#define PAT91xx_YRES      240                     // 0 - 255

//PAT9125 registers
#define PAT9125_PID1			0x00
#define PAT9125_PID2			0x01
#define PAT9125_MOTION			0x02
#define PAT9125_DELTA_XL		     0x03
#define PAT9125_DELTA_YL		     0x04
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
#endif // PAT9125


#ifdef PAT9130
#define PAT91xx_XRES      0                       // 0 - 127
#define PAT91xx_YRES      120                     // 0 - 127

#define SWSPI_MISO	21                            // SCL (PD0)
#define SWSPI_MOSI	21                            // SCL (PD0)
#define SWSPI_SCK	20                            // SDA (PD1)
#define SWSPI_CS	62                            // A7 (PK0)

#define SWSPI_CONFIG 0x32                         // Mode (~ SPI Mode 3) & Frequency (~ 2x 4us)

#define PAT9130_CS_PIN SWSPI_CS
#define PAT9130_WRITE_MASK 0x80
#define PAT9130_READ_MASK (~(PAT9130_WRITE_MASK))

#define _PAT9130_PID1 0x31
#define _PAT9130_PID2 0x61
#define _PAT9130_WP 0x00                          // WriteProtect Enable
#define _PAT9130_WE 0x5A                          // WriteProtect Disable

//PAT9130 registers
#define PAT9130_PID1 0x00
#define PAT9130_PID2 0x01
#define PAT9130_WP 0x09
#define PAT9130_MOTION 0x02
#define PAT9130_DELTA_XL 0x03
#define PAT9130_DELTA_YL 0x04
#define PAT9130_DELTA_XH 0x11
#define PAT9130_DELTA_YH 0x12
#define PAT9130_RES_X 0x0D
#define PAT9130_RES_Y 0x0E
#define PAT9130_SHUTTER 0x15
#define PAT9130_FRAME 0x17
#endif // PAT9130


#define uartout (&_uartout)


#if defined(__cplusplus)
extern "C" {
#endif //defined(__cplusplus)


extern uint8_t pat91xx_PID1;
extern uint8_t pat91xx_PID2;

extern int16_t pat91xx_x;
extern int16_t pat91xx_y;
extern uint8_t pat91xx_b;
extern uint8_t pat91xx_s;

extern uint8_t pat91xx_init(void);
extern uint8_t pat91xx_update(void);
extern uint8_t pat91xx_update_y(void);
extern uint8_t pat91xx_update_y2(void);

uint8_t pat91xx_rd_reg(uint8_t addr);
void pat91xx_wr_reg(uint8_t addr, uint8_t data);
uint8_t pat91xx_wr_reg_verify(uint8_t addr, uint8_t data);


#if defined(__cplusplus)
}
#endif //defined(__cplusplus)

#endif //PAT91xx_H
