// Software SPI
#ifndef SWSPI_H
#define SWSPI_H

//initialize gpio
extern void swspi_init(unsigned char miso, unsigned char mosi, unsigned char sck, unsigned char cfg);
//transmit and receive (full duplex mode)
extern unsigned char swspi_txrx(unsigned char tx);
//transmit (half dublex mode, miso == mosi)
extern void swspi_tx(unsigned char tx);
//receive (half dublex mode, miso == mosi)
extern unsigned char swspi_rx();

#endif //SWSPI_H
