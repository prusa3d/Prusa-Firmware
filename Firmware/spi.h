//spi.h - hardware SPI
#ifndef SPI_H
#define SPI_H

#include <inttypes.h>
#include <avr/io.h>
#include "config.h"


#define SPI_SPCR(rat, pha, pol, mst, dor) ((rat & 3) | (pha?(1<<CPHA):0) | (pol?(1<<CPOL):0) | (mst?(1<<MSTR):0) | (dor?(1<<DORD):0) | (1<<SPE))
#define SPI_SPSR(rat) ((rat & 4)?(1<<SPI2X):0)

#define DD_SS   0
#define DD_SCK  1
#define DD_MOSI 2
#define DD_MISO 3

#if defined(__cplusplus)
extern "C" {
#endif //defined(__cplusplus)

static inline void spi_init()
{
	DDRB &= ~((1 << DD_SCK) | (1 << DD_MOSI) | (1 << DD_MISO));
	DDRB |= (1 << DD_SS) | (1 << DD_SCK) | (1 << DD_MOSI);
	PORTB &= ~((1 << DD_SCK) | (1 << DD_MOSI) | (1 << DD_MISO));
	PORTB |= (1 << DD_SS);
	SPCR = SPI_SPCR(0, 0, 0, 1, 0); //SPE=1, MSTR=1 (0x50)
	SPSR = 0x00;
}

static inline void spi_setup(uint8_t spcr, uint8_t spsr)
{
	SPCR = spcr;
	SPSR = spsr;
}

static inline uint8_t spi_txrx(uint8_t tx)
{
	SPDR = tx;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}

#if defined(__cplusplus)
}
#endif //defined(__cplusplus)

#endif //SPI_H
