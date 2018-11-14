// unification for AVR and RPI

#ifdef __AVR
	//#include "Arduino.h"
	#include "Marlin.h"
	#define GPIO_INP(gpio) pinMode(gpio, INPUT)
	#define GPIO_OUT(gpio) pinMode(gpio, OUTPUT)
	#define GPIO_SET(gpio) digitalWrite(gpio, HIGH)
	#define GPIO_CLR(gpio) digitalWrite(gpio, LOW)
	#define GPIO_GET(gpio) (digitalRead(gpio) != LOW)
	#define DELAY(delay) delayMicroseconds(delay)
	#define PRINT MYSERIAL.print
#endif //RC522_AVR

#ifdef __RPI
	#include <bcm2835.h>
	#define GPIO_INP(gpio) bcm2835_gpio_fsel(gpio, BCM2835_GPIO_FSEL_INPT)
	#define GPIO_OUT(gpio) bcm2835_gpio_fsel(gpio, BCM2835_GPIO_FSEL_OUTP)
	#define GPIO_SET(gpio) bcm2835_gpio_write(gpio, HIGH)
	#define GPIO_CLR(gpio) bcm2835_gpio_write(gpio, LOW)
	#define GPIO_GET(gpio) (bcm2835_gpio_lev(gpio) != LOW)
	#include <unistd.h>
	#define DELAY(delay) usleep(delay)
	#define PRINT(p) print(p)
	#define DEC 10
	#define HEX 16
	void print(const char* pc) { printf("%s", pc); }
	void print(int v) { printf("%d", v); }
	void print(float v) { printf("%f", v); }
#endif //RC522_RPI

