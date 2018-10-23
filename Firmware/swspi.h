// Software SPI
#ifndef SWSPI_H
#define SWSPI_H

// ~ uni_avr_rpi.h
#include "Arduino.h"
#define GPIO_INP(gpio) pinMode(gpio, INPUT)
#define GPIO_OUT(gpio) pinMode(gpio, OUTPUT)
#define GPIO_SET(gpio) digitalWrite(gpio, HIGH)
#define GPIO_CLR(gpio) digitalWrite(gpio, LOW)
#define GPIO_GET(gpio) (digitalRead(gpio) != LOW)
#define DELAY(delay) delayMicroseconds(delay)

//initialize gpio
extern void swspi_init(unsigned char miso, unsigned char mosi, unsigned char sck, unsigned char cfg);
//transmit and receive (full duplex mode)
extern unsigned char swspi_txrx(unsigned char tx);
//transmit (half duplex mode, miso == mosi)
extern void swspi_tx(unsigned char tx);
//receive (half duplex mode, miso == mosi)
extern unsigned char swspi_rx();

#endif //SWSPI_H
