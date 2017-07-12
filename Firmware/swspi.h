#ifndef SWSPI_H
#define SWSPI_H

//#define SWSPI_RPI
#define SWSPI_AVR

#ifdef SWSPI_RPI
//#define SWSPI_MISO	9
#define SWSPI_MISO	10
#define SWSPI_MOSI	10
#define SWSPI_SCK	11
#define SWSPI_CS	7
#endif //SWSPI_RPI

#ifdef SWSPI_AVR
//#if MOTHERBOARD == BOARD_EINY_0_3a
#define SWSPI_MISO	16 //RX2
#define SWSPI_MOSI	16 //RX2
#define SWSPI_SCK	17 //TX2
#define SWSPI_CS	20 //SDA
//#endif //(MOTHERBOARD == 299)
/*#if MOTHERBOARD == BOARD_EINY_0_4a
#define SWSPI_MISO	62 //PK0
#define SWSPI_MOSI	62 //PK0
#define SWSPI_SCK	21 //SCL
#endif //(MOTHERBOARD == BOARD_EINY_0_3a)
#if (MOTHERBOARD == BOARD_EINY_0_4a)
#define SWSPI_MISO	21 //PK0
#define SWSPI_MOSI	21 //PK0
#define SWSPI_SCK	62 //SCL
#define SWSPI_CS	20 //SDA
#endif //(MOTHERBOARD == BOARD_EINY_0_4a)*/
#endif //SWSPI_AVR

#define SWSPI_POL	1 //polarity
#define SWSPI_PHA	0 //phase
#define SWSPI_DOR	0 //data order
#define SWSPI_DEL	2 //delay


void swspi_init();

#if (SWSPI_MOSI == SWSPI_MISO)

void swspi_tx(unsigned char tx);
unsigned char swspi_rx();

#else //(SWSPI_MOSI == SWSPI_MISO)

#define swspi_tx swspi_txrx
#define swspi_rx swspi_txrx
unsigned char swspi_txrx(unsigned char tx);

#endif //(SWSPI_MOSI == SWSPI_MISO)

void swspi_start();
void swspi_stop();


#endif //SWSPI_H
