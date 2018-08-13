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

uint8_t uart2_ibuf[14] = {0, 0};

FILE _uart2io = {0};


int uart2_putchar(char c, FILE *stream __attribute__((unused)))
{
	while (!uart2_txready);
	UDR2 = c; // transmit byte
//	while (!uart2_txcomplete); // wait until byte sent
//	UCSR2A |= (1 << TXC2); // delete TXCflag
	return 0;
}

int uart2_getchar(FILE *stream __attribute__((unused)))
{
	if (rbuf_empty(uart2_ibuf)) return -1;
	return rbuf_get(uart2_ibuf);
}

//uart init (io + FILE stream)
void uart2_init(void)
{
	DDRH &=	~0x01;
	PORTH |= 0x01;
	rbuf_ini(uart2_ibuf, sizeof(uart2_ibuf) - 4);
	UCSR2A |= (1 << U2X2); // baudrate multiplier
	UBRR2L = UART_BAUD_SELECT(UART2_BAUD, F_CPU); // select baudrate
	UCSR2B = (1 << RXEN2) | (1 << TXEN2); // enable receiver and transmitter
	UCSR2B |= (1 << RXCIE2); // enable rx interrupt
	fdev_setup_stream(uart2io, uart2_putchar, uart2_getchar, _FDEV_SETUP_WRITE | _FDEV_SETUP_READ); //setup uart2 i/o stream
}

//returns 1 if chars in input buffer match to str
//returns -1 if chars does not match and 0 for empty buffer
int8_t uart2_rx_str_P(const char* str)
{
	uint8_t r = rbuf_r(uart2_ibuf);            //get read index
	uint8_t w = rbuf_w(uart2_ibuf);            //get write index
//	printf_P(PSTR("uart2_rx_str_P r=%d w=%d\n"), r, w);
	uint8_t e = rbuf_l(uart2_ibuf) - 1;        //get end index
	uint8_t len = strlen_P(str);                 //get string length
	str += len;                                //last char will be compared first
//	printf_P(PSTR(" len=%d\n"), len);
	while (len--)                              //loop over all chars
	{
		if (w == r) return 0;                  //empty buffer - return 0
		if ((--w) == 255) w = e;               //decrement index
		char c0 = pgm_read_byte(--str);        //read char from str
		char c1 = uart2_ibuf[4 + w];           //read char from input buffer
//		printf_P(PSTR(" uart2_rx_str_P w=%d l=%d c0=%02x c1=%02x\n"), w, len, c0, c1);
		if (c0 == c1) continue;                //if match, continue with next char
		if ((c0 == '\r') && (c1 == '\n'))      //match cr as lf
			continue;
		if ((c0 == '\n') && (c1 == '\r'))      //match lf as cr
			continue;
		return -1;                             //no match - return -1
	}
	return 1;                                  //all characters match - return 1
}

ISR(USART2_RX_vect)
{
	//printf_P(PSTR("USART2_RX_vect \n") );
	if (rbuf_put(uart2_ibuf, UDR2) < 0) // put received byte to buffer
	{ //rx buffer full
		//uart2_rx_clr(); //for sure, clear input buffer
		printf_P(PSTR("USART2 rx Full!!!\n"));
	}
}

