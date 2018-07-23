//uart2.c
#include "uart2.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "rbuf.h"

#define UART2_BAUD 115200
#define UART_BAUD_SELECT(baudRate,xtalCpu) (((float)(xtalCpu))/(((float)(baudRate))*8.0)-1.0+0.5)
#define uart2_rxcomplete (UCSR2A & (1 << RXC2))
#define uart2_txcomplete (UCSR2A & (1 << TXC2))
#define uart2_txready    (UCSR2A & (1 << UDRE2))

uint8_t uart2_ibuf[10] = {0, 0};

FILE _uart2io = {0};


int uart2_putchar(char c, FILE *stream)
{
	while (!uart2_txready);
	UDR2 = c; // transmit byte
//	while (!uart2_txcomplete); // wait until byte sent
//	UCSR2A |= (1 << TXC2); // delete TXCflag
	return 0;
}

int uart2_getchar(FILE *stream)
{
	if (rbuf_empty(uart2_ibuf)) return -1;
	return rbuf_get(uart2_ibuf);
}

void uart2_init(void)
{
	rbuf_ini(uart2_ibuf, 6);
	UCSR2A |= (1 << U2X2); // baudrate multiplier
	UBRR2L = UART_BAUD_SELECT(UART2_BAUD, F_CPU); // select baudrate
	UCSR2B = (1 << RXEN2) | (1 << TXEN2); // enable receiver and transmitter
	UCSR2B |= (1 << RXCIE2); // enable rx interrupt
	fdev_setup_stream(uart2io, uart2_putchar, uart2_getchar, _FDEV_SETUP_WRITE | _FDEV_SETUP_READ); //setup uart2 i/o stream
}

void uart2_rx_clr(void)
{
	rbuf_w(uart2_ibuf) = 0;
	rbuf_r(uart2_ibuf) = 0;
}

uint8_t uart2_rx_ok(void)
{
	//printf_P(PSTR("uart2_rx_ok %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu\n"), uart2_ibuf[0], uart2_ibuf[1], uart2_ibuf[2], uart2_ibuf[3], uart2_ibuf[4], uart2_ibuf[5], uart2_ibuf[6], uart2_ibuf[7], uart2_ibuf[8], uart2_ibuf[9]);
//	return 0;
//	_lock();                                   //lock
	uint8_t i = rbuf_w(uart2_ibuf);            //get write index
//	_unlock();                                 //unlock
	uint8_t e = rbuf_l(uart2_ibuf) - 1;        //get end index
//	printf_P(PSTR("%d %d \n"), i, e);
//	return 0;
	if ((i--) == 255) i = e;                   //decrement index
	if ((uart2_ibuf[4 + i] != '\n') &&
		(uart2_ibuf[4 + i] != '\r')) return 0; //no match - exit
	if ((i--) == 255) i = e;                   //decrement index
	if (uart2_ibuf[4 + i] != 'k') return 0;    //no match - exit
	if ((i--) == 255) i = e;                   //decrement index
	if (uart2_ibuf[4 + i] != 'o') return 0;    //no match - exit
	uart2_ibuf[4 + i] = 0;                     //discard char
	return 1;                                  //match "ok\n"
}

ISR(USART2_RX_vect)
{
	//printf_P(PSTR("USART2_RX_vect \n") );
	if (rbuf_put(uart2_ibuf, UDR2) < 0) // put received byte to buffer
	{
		//rx buffer full
	}
}

