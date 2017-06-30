#ifndef TMC2130_H
#define TMC2130_H

static uint8_t sg_homing_axis = 0xFF;
static uint8_t sg_axis_stalled[2] = {0, 0};
static uint8_t sg_lastHomingStalled = false;

void tmc2130_check_overtemp();

void tmc2130_write(uint8_t chipselect, uint8_t address,uint8_t wval1,uint8_t wval2,uint8_t wval3,uint8_t wval4);
uint8_t tmc2130_read8(uint8_t chipselect, uint8_t address);
uint16_t tmc2130_readSG(uint8_t chipselect);
uint16_t tmc2130_readTStep(uint8_t chipselect);
void tmc2130_PWMconf(uint8_t cs, uint8_t PWMgrad, uint8_t PWMampl);

uint8_t st_didLastHomingStall();

void tmc2130_st_synchronize();

void tmc2130_st_home_enter(uint8_t axis);

void tmc2130_st_home_exit();

void tmc2130_init();

#endif TMC2130_H