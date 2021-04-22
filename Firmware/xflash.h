//xflash.h
#ifndef _XFLASH_H
#define _XFLASH_H

#include <inttypes.h>
#include "config.h"
#include "spi.h"



#define XFLASH_STATUS_BUSY   0x01
#define XFLASH_STATUS_WEL    0x02
#define XFLASH_STATUS_BP0    0x04
#define XFLASH_STATUS_BP1    0x08
#define XFLASH_STATUS_TB     0x20
#define XFLASH_STATUS_SRP    0x80

#define XFLASH_SPI_RATE      0 // fosc/4 = 4MHz
#define XFLASH_SPCR          SPI_SPCR(XFLASH_SPI_RATE, 1, 1, 1, 0)
#define XFLASH_SPSR          SPI_SPSR(XFLASH_SPI_RATE)

#define XFLASH_SPI_ENTER() spi_setup(XFLASH_SPCR, XFLASH_SPSR)

#if defined(__cplusplus)
extern "C" {
#endif //defined(__cplusplus)


extern int8_t xflash_init(void);
extern void xflash_enable_wr(void);
extern void xflash_disable_wr(void);
extern uint8_t xflash_rd_status_reg(void);
#if 0
extern void w25x20cl_wr_status_reg(uint8_t val);
#endif
extern void xflash_rd_data(uint32_t addr, uint8_t* data, uint16_t cnt);
extern void xflash_page_program(uint32_t addr, uint8_t* data, uint16_t cnt);
extern void xflash_page_program_P(uint32_t addr, uint8_t* data, uint16_t cnt);
extern void xflash_sector_erase(uint32_t addr);
extern void xflash_block32_erase(uint32_t addr);
extern void xflash_block64_erase(uint32_t addr);
extern void xflash_chip_erase(void);
extern void xflash_page_program(uint32_t addr, uint8_t* data, uint16_t cnt);
extern void xflash_rd_uid(uint8_t* uid);
extern void xflash_wait_busy(void);

#if defined(__cplusplus)
}
#endif //defined(__cplusplus)
#endif //_XFLASH_H
