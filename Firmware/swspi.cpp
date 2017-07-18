#include "swspi.h"


#ifdef SWSPI_RPI
	#include <bcm2835.h>
	#define GPIO_INP(gpio) bcm2835_gpio_fsel(gpio, BCM2835_GPIO_FSEL_INPT)
	#define GPIO_OUT(gpio) bcm2835_gpio_fsel(gpio, BCM2835_GPIO_FSEL_OUTP)
	#define GPIO_SET(gpio) bcm2835_gpio_write(gpio, HIGH)
	#define GPIO_CLR(gpio) bcm2835_gpio_write(gpio, LOW)
	#define GPIO_GET(gpio) (bcm2835_gpio_lev(gpio) != LOW)
	#define DELAY(delay) usleep(delay)
#endif //SWSPI_RPI

#ifdef SWSPI_AVR
	//#include "Arduino.h"
	#include "Marlin.h"
	#define GPIO_INP(gpio) pinMode(gpio, INPUT)
	#define GPIO_OUT(gpio) pinMode(gpio, OUTPUT)
	#define GPIO_SET(gpio) digitalWrite(gpio, HIGH)
	#define GPIO_CLR(gpio) digitalWrite(gpio, LOW)
	#define GPIO_GET(gpio) (digitalRead(gpio) != LOW)
	#define DELAY(delay) delayMicroseconds(delay)
#endif //SWSPI_AVR

#if (SWSPI_POL != 0)
	#define SWSPI_SCK_UP GPIO_CLR(SWSPI_SCK)
	#define SWSPI_SCK_DN GPIO_SET(SWSPI_SCK)
#else
	#define SWSPI_SCK_UP GPIO_SET(SWSPI_SCK)
	#define SWSPI_SCK_DN GPIO_CLR(SWSPI_SCK)
#endif


void swspi_init()
{
	GPIO_INP(SWSPI_MISO);
	GPIO_OUT(SWSPI_MOSI);
	GPIO_OUT(SWSPI_SCK);
	GPIO_OUT(SWSPI_CS);
	GPIO_CLR(SWSPI_MOSI);
	SWSPI_SCK_DN;
	GPIO_SET(SWSPI_CS);
}

#if (SWSPI_MOSI == SWSPI_MISO)

void swspi_tx(unsigned char tx)
{
	GPIO_OUT(SWSPI_MOSI);
	unsigned char i = 0; for (; i < 8; i++)
	{
		if (tx & 0x80) GPIO_SET(SWSPI_MOSI);
		else GPIO_CLR(SWSPI_MOSI);
	    DELAY(SWSPI_DEL);
		SWSPI_SCK_UP;
	    DELAY(SWSPI_DEL);
		SWSPI_SCK_DN;
		tx <<= 1;
	}
}

unsigned char swspi_rx()
{
	GPIO_INP(SWSPI_MISO);
	unsigned char rx = 0;
	unsigned char i = 0; for (; i < 8; i++)
	{
		rx <<= 1;
	    DELAY(SWSPI_DEL);
		SWSPI_SCK_UP;
	    DELAY(SWSPI_DEL);
		rx |= GPIO_GET(SWSPI_MISO)?1:0;
		SWSPI_SCK_DN;
	}
	return rx;
}

#else //(SWSPI_MOSI == SWSPI_MISO)

unsigned char swspi_txrx(unsigned char tx)
{
	unsigned char rx = 0;
	unsigned char i = 0; for (; i < 8; i++)
	{
		rx <<= 1;
		if (tx & 0x80) GPIO_SET(SWSPI_MOSI);
		else GPIO_CLR(SWSPI_MOSI);
	    DELAY(SWSPI_DEL);
		SWSPI_SCK_UP;
	    DELAY(SWSPI_DEL);
		rx |= GPIO_GET(SWSPI_MISO)?1:0;
		SWSPI_SCK_DN;
		tx <<= 1;
	}
	return rx;
}

#endif //(SWSPI_MOSI == SWSPI_MISO)

void swspi_start()
{
	GPIO_CLR(SWSPI_CS);
}

void swspi_stop()
{
	GPIO_SET(SWSPI_CS);
}
