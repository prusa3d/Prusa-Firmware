//rbuf.c
#include "rbuf.h"
//#include <avr/interrupt.h>


void rbuf_ini(uint8_t* ptr, uint8_t l)
{
	ptr[0] = l;
	ptr[1] = 0;
	ptr[2] = 0;
}

//lock/unlock macros
//#define _lock() uint8_t _sreg = SREG; cli();
//#define _unlock() SREG = _sreg;
#define _lock()
#define _unlock()

//put single byte to buffer
int rbuf_put(uint8_t* ptr, uint8_t b)
{
//#ifdef _NO_ASM
	_lock();                         //lock
	uint8_t buf_w = ptr[1];          //get write index
	uint8_t buf_r = ptr[2];          //get read index
	_unlock();                       //unlock
	ptr[4 + buf_w] = b;              //store byte to buffer
	buf_w++;                         //incerment write index
	uint8_t buf_l = ptr[0];          //get length
	if (buf_w >= buf_l) buf_w = 0;   //rotate write index
	if (buf_w == buf_r) return -1;   //return -1 to signal buffer full
	ptr[1] = buf_w;                  //store write index
	return 0;                        //return 0 to signal success
//#else //_NO_ASM
// TODO - optimized assembler version
//	asm("movw r26, r24");
//	asm("ld r18, X+");
//	asm("cli");
//	asm("ld r19, X+");
//	asm("ld r20, X");
//	asm("cp r19, r18");
//	asm("brne .-6");*/
//#endif //_NO_ASM
}

//get single byte from buffer
int rbuf_get(uint8_t* ptr)
{
//#ifdef _NO_ASM
	_lock();                         //lock
	uint8_t buf_w = ptr[1];          //get write index
	uint8_t buf_r = ptr[2];          //get read index
	_unlock();                       //unlock
	if (buf_r == buf_w) return -1;   //return -1 to signal buffer empty
	int ret = ptr[4 + buf_r];        //get byte from buffer
	buf_r++;                         //increment read index
	uint8_t buf_l = ptr[0];          //get length
	if (buf_r >= buf_l) buf_r = 0;   //rotate read index
	ptr[2] = buf_r;                  //store read index
	return ret;                      //return byte (0-255)
//	return 0;                        //return 0 to signal success
//#else //_NO_ASM
// TODO - optimized assembler version
//#endif //_NO_ASM
}