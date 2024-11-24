//uart2.h
#ifndef _UART2_H
#define _UART2_H

#include <inttypes.h>
#include <stdio.h>

extern FILE _uart2io;
#define uart2io (&_uart2io)

void uart2_init(uint32_t baudRate);

#endif //_UART2_H
