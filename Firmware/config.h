#ifndef _CONFIG_H
#define _CONFIG_H


//ADC configuration
#define ADC_CHAN_MSK      0b0000001001011111 //used AD channels bit mask (0,1,2,3,4,6,9)
#define ADC_CHAN_CNT      7         //number of used channels)
#define ADC_OVRSAMPL      16        //oversampling multiplier
#define ADC_CALLBACK      adc_ready //callback function ()

//SM4 configuration
#define SM4_DEFDELAY      500       //default step delay [us]

//TMC2130 - Trinamic stepper driver
//pinout - hardcoded
//spi:
#define TMC2130_SPI_RATE       0 // fosc/4 = 4MHz
#define TMC2130_SPCR           SPI_SPCR(TMC2130_SPI_RATE, 1, 1, 1, 0)
#define TMC2130_SPSR           SPI_SPSR(TMC2130_SPI_RATE)

//W25X20CL configuration
//pinout:
#define W25X20CL_PIN_CS        32
//spi:
#define W25X20CL_SPI_RATE      0 // fosc/4 = 4MHz
#define W25X20CL_SPCR          SPI_SPCR(W25X20CL_SPI_RATE, 1, 1, 1, 0)
#define W25X20CL_SPSR          SPI_SPSR(W25X20CL_SPI_RATE)

//LANG - Multi-language support
//#define LANG_MODE               0 // primary language only
#define LANG_MODE               1 // sec. language support
#define LANG_SIZE_RESERVED 0x2700 // reserved space for secondary language (~10kb)
//#define LANG_SIZE_RESERVED 0x1ef8 // reserved space for secondary language (~10kb)


#endif //_CONFIG_H
