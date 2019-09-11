//! @file
//! @date Jun 10, 2019
//! @author Marek Bel

#ifndef FIRMWARE_FIRST_LAY_CAL_H_
#define FIRMWARE_FIRST_LAY_CAL_H_
#include <stdint.h>

void lay1cal_wait_preheat();
void lay1cal_load_filament(char *cmd_buffer, uint8_t filament);
void lay1cal_intro_line();
void lay1cal_before_meander();
void lay1cal_meander(char *cmd_buffer);
void lay1cal_square(char *cmd_buffer, uint8_t i);

#endif /* FIRMWARE_FIRST_LAY_CAL_H_ */
