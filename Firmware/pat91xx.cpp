//pat91xx.cpp
#include "pat91xx.h"


uint8_t pat91xx_PID1 = 0;
uint8_t pat91xx_PID2 = 0;
int16_t pat91xx_x = 0;
int16_t pat91xx_y = 0;
uint8_t pat91xx_b = 0;
uint8_t pat91xx_s = 0;


uint8_t pat91xx_wr_reg_verify(uint8_t addr, uint8_t data)
{
	pat91xx_wr_reg(addr, data);
	return pat91xx_rd_reg(addr) == data;
}
