/*
  HardwareSerial.h - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 28 September 2010 by Mark Sproul
*/

#ifndef MarlinSerial_h
#define MarlinSerial_h
#include "Marlin.h"

#if !defined(SERIAL_PORT) 
#define SERIAL_PORT 0
#endif

// The presence of the UBRRH register is used to detect a UART.
#define UART_PRESENT(port) ((port == 0 && (defined(UBRRH) || defined(UBRR0H))) || \
						(port == 1 && defined(UBRR1H)) || (port == 2 && defined(UBRR2H)) || \
						(port == 3 && defined(UBRR3H)))				
						
// These are macros to build serial port register names for the selected SERIAL_PORT (C preprocessor
// requires two levels of indirection to expand macro values properly)
#define SERIAL_REGNAME(registerbase,number,suffix) SERIAL_REGNAME_INTERNAL(registerbase,number,suffix)
#if SERIAL_PORT == 0 && (!defined(UBRR0H) || !defined(UDR0)) // use un-numbered registers if necessary
#define SERIAL_REGNAME_INTERNAL(registerbase,number,suffix) registerbase##suffix
#else
#define SERIAL_REGNAME_INTERNAL(registerbase,number,suffix) registerbase##number##suffix
#endif

// Registers used by MarlinSerial class (these are expanded 
// depending on selected serial port
#define M_UCSRxA SERIAL_REGNAME(UCSR,SERIAL_PORT,A) // defines M_UCSRxA to be UCSRnA where n is the serial port number
#define M_UCSRxB SERIAL_REGNAME(UCSR,SERIAL_PORT,B) 
#define M_RXENx SERIAL_REGNAME(RXEN,SERIAL_PORT,)    
#define M_TXENx SERIAL_REGNAME(TXEN,SERIAL_PORT,)    
#define M_RXCIEx SERIAL_REGNAME(RXCIE,SERIAL_PORT,)    
#define M_UDREx SERIAL_REGNAME(UDRE,SERIAL_PORT,)    
#define M_UDRx SERIAL_REGNAME(UDR,SERIAL_PORT,)  
#define M_UBRRxH SERIAL_REGNAME(UBRR,SERIAL_PORT,H)
#define M_UBRRxL SERIAL_REGNAME(UBRR,SERIAL_PORT,L)
#define M_RXCx SERIAL_REGNAME(RXC,SERIAL_PORT,)
#define M_FEx SERIAL_REGNAME(FE,SERIAL_PORT,)
#define M_USARTx_RX_vect SERIAL_REGNAME(USART,SERIAL_PORT,_RX_vect)
#define M_U2Xx SERIAL_REGNAME(U2X,SERIAL_PORT,)



#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define BYTE 0


#ifndef AT90USB
// Define constants and variables for buffering incoming serial data.  We're
// using a ring buffer (I think), in which rx_buffer_head is the index of the
// location to which to write the next incoming character and rx_buffer_tail
// is the index of the location from which to read.
#define RX_BUFFER_SIZE 128

extern uint8_t selectedSerialPort;

struct ring_buffer
{
  unsigned char buffer[RX_BUFFER_SIZE];
  int head;
  int tail;
};

#if UART_PRESENT(SERIAL_PORT)
  extern ring_buffer rx_buffer;
#endif

class MarlinSerial //: public Stream
{

  public:
    static void begin(long);
    static void end();
    static int peek(void);
    static int read(void);
    static void flush(void);
    
    static /*FORCE_INLINE*/ int available(void)
    {
      return (unsigned int)(RX_BUFFER_SIZE + rx_buffer.head - rx_buffer.tail) % RX_BUFFER_SIZE;
    }
    /*
    FORCE_INLINE void write(uint8_t c)
    {
      while (!((M_UCSRxA) & (1 << M_UDREx)))
        ;

      M_UDRx = c;
    }
    */
	static void write(uint8_t c)
	{
		if (selectedSerialPort == 0)
		{
			while (!((M_UCSRxA) & (1 << M_UDREx)));
			M_UDRx = c;
		}
		else if (selectedSerialPort == 1)
		{
			while (!((UCSR1A) & (1 << UDRE1)));
			UDR1 = c;
		}
	}
    
    static void checkRx(void)
    {
        if (selectedSerialPort == 0) {
            if((M_UCSRxA & (1<<M_RXCx)) != 0) {
                // Test for a framing error.
                if (M_UCSRxA & (1<<M_FEx)) {
                    // Characters received with the framing errors will be ignored.
                    // The temporary variable "c" was made volatile, so the compiler does not optimize this out.
                    (void)(*(char *)M_UDRx);
                } else {
                    unsigned char c  =  M_UDRx;
                    int i = (unsigned int)(rx_buffer.head + 1) % RX_BUFFER_SIZE;
                    // if we should be storing the received character into the location
                    // just before the tail (meaning that the head would advance to the
                    // current location of the tail), we're about to overflow the buffer
                    // and so we don't write the character or advance the head.
                    if (i != rx_buffer.tail) {
                        rx_buffer.buffer[rx_buffer.head] = c;
                        rx_buffer.head = i;
                    }
                    //selectedSerialPort = 0;
#ifdef DEBUG_DUMP_TO_2ND_SERIAL
					UDR1 = c;
#endif //DEBUG_DUMP_TO_2ND_SERIAL
                }
            }
        } else { // if(selectedSerialPort == 1) {
            if((UCSR1A & (1<<RXC1)) != 0) {
                // Test for a framing error.
                if (UCSR1A & (1<<FE1)) {
                    // Characters received with the framing errors will be ignored.
                    // The temporary variable "c" was made volatile, so the compiler does not optimize this out.
                    (void)(*(char *)UDR1);
                } else {
                    unsigned char c  =  UDR1;
                    int i = (unsigned int)(rx_buffer.head + 1) % RX_BUFFER_SIZE;
                    // if we should be storing the received character into the location
                    // just before the tail (meaning that the head would advance to the
                    // current location of the tail), we're about to overflow the buffer
                    // and so we don't write the character or advance the head.
                    if (i != rx_buffer.tail) {
                        rx_buffer.buffer[rx_buffer.head] = c;
                        rx_buffer.head = i;
                    }
                    //selectedSerialPort = 1;
#ifdef DEBUG_DUMP_TO_2ND_SERIAL
					M_UDRx = c;
#endif //DEBUG_DUMP_TO_2ND_SERIAL
                }
            }
        }
    }
    
    
    private:
    static void printNumber(unsigned long, uint8_t);
    static void printFloat(double, uint8_t);
    
    
  public:
    
    static /*FORCE_INLINE*/ void write(const char *str)
    {
      while (*str)
        write(*str++);
    }


    static /*FORCE_INLINE*/ void write(const uint8_t *buffer, size_t size)
    {
      while (size--)
        write(*buffer++);
    }

/*    static FORCE_INLINE void print(const String &s)
    {
      for (int i = 0; i < (int)s.length(); i++) {
        write(s[i]);
      }
    }*/
    
    static FORCE_INLINE void print(const char *str)
    {
      write(str);
    }
    static void print(char, int = BYTE);
    static void print(unsigned char, int = BYTE);
    static void print(int, int = DEC);
    static void print(unsigned int, int = DEC);
    static void print(long, int = DEC);
    static void print(unsigned long, int = DEC);
    static void print(double, int = 2);

//    static void println(const String &s);
    static void println(const char[]);
    static void println(char, int = BYTE);
    static void println(unsigned char, int = BYTE);
    static void println(int, int = DEC);
    static void println(unsigned int, int = DEC);
    static void println(long, int = DEC);
    static void println(unsigned long, int = DEC);
    static void println(double, int = 2);
    static void println(void);
};

extern MarlinSerial MSerial;
#endif // !AT90USB

// Use the UART for BT in AT90USB configurations
#if defined(AT90USB) && defined (BTENABLED)
   extern HardwareSerial bt;
#endif

#endif
