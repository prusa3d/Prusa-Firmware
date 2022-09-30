//swi2c.h
#ifndef SWI2C_H
#define SWI2C_H

#include <inttypes.h>
#include "config.h"


#if defined(__cplusplus)
extern "C" {
#endif //defined(__cplusplus)

//initialize
extern void swi2c_init(void);

//deinit pins
extern void swi2c_disable(void);

//check device address acknowledge
extern uint8_t swi2c_check(uint8_t dev_addr);

//read write functions - 8bit address (most i2c chips)
#ifdef SWI2C_A8
extern uint8_t swi2c_readByte_A8(uint8_t dev_addr, uint8_t addr, uint8_t* pbyte);
extern uint8_t swi2c_writeByte_A8(uint8_t dev_addr, uint8_t addr, uint8_t* pbyte);
#endif //SWI2C_A8

//read write functions - 16bit address (e.g. serial eeprom AT24C256)
#ifdef SWI2C_A16
extern uint8_t swi2c_readByte_A16(uint8_t dev_addr, uint16_t addr, uint8_t* pbyte);
extern uint8_t swi2c_writeByte_A16(uint8_t dev_addr, uint16_t addr, uint8_t* pbyte);
#endif //SWI2C_A16

#if defined(__cplusplus)
}
#endif //defined(__cplusplus)

#endif //SWI2C_H
