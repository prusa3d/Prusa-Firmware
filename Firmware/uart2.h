//uart2.h
#ifndef _UART2_H
#define _UART2_H

#include <inttypes.h>
#include <stdio.h>


#if defined(__cplusplus)
extern "C" {
#endif //defined(__cplusplus)


extern FILE _uart2io;
#define uart2io (&_uart2io)

//extern uint8_t uart2_ibuf[10];

extern void uart2_init(void);

extern void uart2_rx_clr(void);

extern uint8_t uart2_rx_ok(void);


#if defined(__cplusplus)
}
#endif //defined(__cplusplus)
#endif //_UART2_H
