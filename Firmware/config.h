#ifndef _CONFIG_H
#define _CONFIG_H


#include "Configuration_prusa.h"
#include "pins.h"

#if (defined(VOLT_IR_PIN) && defined(IR_SENSOR))
# define IR_SENSOR_ANALOG
#endif

//ADC configuration
#ifndef IR_SENSOR_ANALOG
#define ADC_CHAN_MSK      0b0000001001011111 //used AD channels bit mask (0,1,2,3,4,6,9)
#define ADC_DIDR_MSK      0b0000001001011111 //AD channels DIDR mask (1 ~ disabled digital input)
#define ADC_CHAN_CNT      7         //number of used channels)
#else //!IR_SENSOR_ANALOG
#define ADC_CHAN_MSK      0b0000001101011111 //used AD channels bit mask (0,1,2,3,4,6,8,9)
#define ADC_DIDR_MSK      0b0000001001011111 //AD channels DIDR mask (1 ~ disabled digital input)
#define ADC_CHAN_CNT      8         //number of used channels)
#endif //!IR_SENSOR_ANALOG
#define ADC_OVRSAMPL      16        //oversampling multiplier
#define ADC_CALLBACK      adc_ready //callback function ()

//SWI2C configuration
//#define SWI2C_SDA         20 //SDA on P3
//#define SWI2C_SCL         21 //SCL on P3
#define SWI2C_A8
#define SWI2C_DEL         20 //2us clock delay
#define SWI2C_TMO         2048 //2048 cycles timeout

//PAT9125 configuration
//#define PAT9125_SWSPI // software SPI mode (incomplete)
#ifdef SWI2C_SCL
#define PAT9125_SWI2C   // software I2C mode
#else
#define PAT9125_I2C     // hardware I2C mode
#endif

#define PAT9125_I2C_ADDR  0x75  //ID=LO
//#define PAT9125_I2C_ADDR  0x79  //ID=HI
//#define PAT9125_I2C_ADDR  0x73  //ID=NC
#define PAT9125_XRES      0
#define PAT9125_YRES      240                   // maximum resolution (5*X cpi)
#define PAT9124_YRES_MM   (5*PAT9125_YRES/25.4) // counts per mm

//SM4 configuration
#define SM4_DEFDELAY      500       //default step delay [us]

//TMC2130 - Trinamic stepper driver
//pinout - hardcoded
//spi:
#define TMC2130_SPI_RATE       0 // fosc/4 = 4MHz
#define TMC2130_SPCR           SPI_SPCR(TMC2130_SPI_RATE, 1, 1, 1, 0)
#define TMC2130_SPSR           SPI_SPSR(TMC2130_SPI_RATE)

//LANG - Multi-language support
//#define LANG_MODE              0 // primary language only
#define LANG_MODE              1 // sec. language support

#define LANG_SIZE_RESERVED     0x3000 // reserved space for secondary language (12288 bytes). Maximum 32768 bytes

#if (LANG_SIZE_RESERVED % 256)
  #error "LANG_SIZE_RESERVED should be a multiple of a page size"
#endif

//Community language support
#define COMMUNITY_LANG_GROUP 1

#if (COMMUNITY_LANG_GROUP == 1)
#define COMMUNITY_LANG_GROUP1_NL // Community Dutch language
#define COMMUNITY_LANG_GROUP1_RO // Community Romanian language
#define COMMUNITY_LANG_GROUP1_HU // Community Hungarian language
#define COMMUNITY_LANG_GROUP1_HR // Community Croatian language
#define COMMUNITY_LANG_GROUP1_SK // Community Slovak language
#define COMMUNITY_LANG_GROUP1_SV // Community Swedish language
#define COMMUNITY_LANG_GROUP1_NO // Community Norwegian language
//#define COMMUNITY_LANG_GROUP1_DA // Community Danish language
//#define COMMUNITY_LANG_GROUP1_SL // Community Slovanian language
//#define COMMUNITY_LANG_GROUP1_LB // Community Luxembourgish language
//#define COMMUNITY_LANG_GROUP1_LT // Community Lithuanian language
//#define COMMUNITY_LANG_GROUP1_QR // Community new language //..use this as a template and replace 'QR'
#endif

#if (COMMUNITY_LANG_GROUP >=1 )
#define COMMUNITY_LANGUAGE_SUPPORT
#endif

// Sanity checks for correct configuration of XFLASH_DUMP options
#if defined(XFLASH_DUMP) && !defined(XFLASH)
#error "XFLASH_DUMP requires XFLASH support"
#endif
#if (defined(MENU_DUMP) || defined(EMERGENCY_DUMP)) && !defined(XFLASH_DUMP)
#error "MENU_DUMP and EMERGENCY_DUMP require XFLASH_DUMP"
#endif

// Support for serial dumps is mutually exclusive with XFLASH_DUMP features
#if defined(EMERGENCY_DUMP) && defined(EMERGENCY_SERIAL_DUMP)
#error "EMERGENCY_DUMP and EMERGENCY_SERIAL_DUMP are mutually exclusive"
#endif
#if defined(MENU_DUMP) && defined(MENU_SERIAL_DUMP)
#error "MENU_DUMP and MENU_SERIAL_DUMP are mutually exclusive"
#endif

// Reduce internal duplication
#if defined(EMERGENCY_DUMP) || defined(EMERGENCY_SERIAL_DUMP)
#define EMERGENCY_HANDLERS
#endif

#endif //_CONFIG_H
