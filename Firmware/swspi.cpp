#include "uni_avr_rpi.h"

#ifdef __SWSPI
#include "swspi.h"

#ifdef __RPI
//#define swspi_miso	9
#define swspi_miso	10
#define swspi_mosi	10
#define swspi_sck	11
#define SWSPI_CS	7
#endif //__RPI


#define SWSPI_DEL	0x0f //delay mask (0-3. bit, delay = 1 << DEL [us])
#define SWSPI_POL	0x10 //polarity mask (4. bit, 1=inverted)
#define SWSPI_PHA	0x20 //phase mask (5. bit)
#define SWSPI_DOR	0x40 //data order mask (6. bit, 0=MSB first, 1=LSB first)

#define SWSPI_SCK_UP if (swspi_cfg & SWSPI_POL) GPIO_CLR(swspi_sck); else GPIO_SET(swspi_sck);
#define SWSPI_SCK_DN if (swspi_cfg & SWSPI_POL) GPIO_SET(swspi_sck); else GPIO_CLR(swspi_sck);

unsigned char swspi_miso = 0;
unsigned char swspi_mosi = 0;
unsigned char swspi_sck = 0;
unsigned char swspi_cfg = 0;

void swspi_init(unsigned char miso, unsigned char mosi, unsigned char sck, unsigned char cfg)
{
	swspi_miso = miso;
	swspi_mosi = mosi;
	swspi_sck = sck;
	swspi_cfg = cfg;
	GPIO_INP(swspi_miso);
	GPIO_OUT(swspi_mosi);
	GPIO_OUT(swspi_sck);
	GPIO_CLR(swspi_mosi);
	SWSPI_SCK_DN;
}

void swspi_tx(unsigned char tx)
{
	int delay = 1 << (swspi_cfg & SWSPI_DEL));
	if (swspi_miso == swspi_mosi) GPIO_OUT(swspi_mosi);
	unsigned char i = 0; for (; i < 8; i++)
	{
		if (tx & 0x80) GPIO_SET(swspi_mosi);
		else GPIO_CLR(swspi_mosi);
	    DELAY(delay);
		SWSPI_SCK_UP;
	    DELAY(delay);
		SWSPI_SCK_DN;
		tx <<= 1;
	}
}

unsigned char swspi_rx()
{
	int delay = 1 << (swspi_cfg & SWSPI_DEL));
	if (swspi_miso == swspi_mosi) GPIO_OUT(swspi_mosi);
	unsigned char rx = 0;
	unsigned char i = 0; for (; i < 8; i++)
	{
		rx <<= 1;
	    DELAY(delay);
		SWSPI_SCK_UP;
	    DELAY(delay);
		rx |= GPIO_GET(swspi_miso)?1:0;
		SWSPI_SCK_DN;
	}
	return rx;
}

unsigned char swspi_txrx(unsigned char tx)
{
	int delay = 1 << (swspi_cfg & SWSPI_DEL));
	unsigned char rx = 0;
	unsigned char i = 0; for (; i < 8; i++)
	{
		rx <<= 1;
		if (tx & 0x80) GPIO_SET(swspi_mosi);
		else GPIO_CLR(swspi_mosi);
	    DELAY(delay);
		SWSPI_SCK_UP;
	    DELAY(delay);
		rx |= GPIO_GET(swspi_miso)?1:0;
		SWSPI_SCK_DN;
		tx <<= 1;
	}
	return rx;
}

#endif //__SWSPI
