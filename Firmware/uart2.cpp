//uart2.c
#include "uart2.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "rbuf.h"
#include "macros.h"

#define UART_BAUD_SELECT(baudRate,xtalCpu) (((float)(xtalCpu))/(((float)(baudRate))*8.0)-1.0+0.5)
#define uart2_txready    (UCSR2A & (1 << UDRE2))

uint8_t uart2_ibuf[20] = {0, 0};
FILE _uart2io;

static int uart2_putchar(char c, _UNUSED FILE *stream)
{
	while (!uart2_txready);

	UDR2 = c; // transmit byte

	return 0;
}

static int uart2_getchar(_UNUSED FILE *stream)
{
	if (rbuf_empty(uart2_ibuf)) return -1;
	return rbuf_get(uart2_ibuf);
}

//uart init (io + FILE stream)
void uart2_init(uint32_t baudRate)
{
	DDRH &=	~0x01;
	PORTH |= 0x01;
	rbuf_ini(uart2_ibuf, sizeof(uart2_ibuf) - 4);
	UCSR2A |= (1 << U2X2); // baudrate multiplier
	UBRR2L = UART_BAUD_SELECT(baudRate, F_CPU); // select baudrate
	UCSR2B = (1 << RXEN2) | (1 << TXEN2); // enable receiver and transmitter
	UCSR2B |= (1 << RXCIE2); // enable rx interrupt
	fdev_setup_stream(uart2io, uart2_putchar, uart2_getchar, _FDEV_SETUP_WRITE | _FDEV_SETUP_READ); //setup uart2 i/o stream
}

ISR(USART2_RX_vect)
{
	if (rbuf_put(uart2_ibuf, UDR2) < 0) // put received byte to buffer
	{ //rx buffer full
		puts_P(PSTR("USART2 rx Full!!!"));
	}
}

