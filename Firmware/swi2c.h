#ifndef SWI2C_H
#define SWI2C_H

//initialize
extern void swi2c_init(unsigned char sda, unsigned char scl, unsigned char cfg);

//check device address acknowledge
extern int swi2c_check(unsigned char dev_addr);

//read write functions - 8bit address (most i2c chips)
#ifdef SWI2C_A8
extern int swi2c_readByte_A8(unsigned char dev_addr, unsigned char addr, unsigned char* pbyte);
extern int swi2c_writeByte_A8(unsigned char dev_addr, unsigned char addr, unsigned char* pbyte);
#endif //SWI2C_A8

//read write functions - 16bit address (e.g. serial eeprom AT24C256)
#ifdef SWI2C_A16
extern int swi2c_readByte_A16(unsigned char dev_addr, unsigned short addr, unsigned char* pbyte);
extern int swi2c_writeByte_A16(unsigned char dev_addr, unsigned short addr, unsigned char* pbyte);
#endif //SWI2C_A16

#endif //SWI2C_H
