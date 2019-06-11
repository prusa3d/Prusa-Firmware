/*
 * first_lay_cal.h
 *
 *  Created on: Jun 10, 2019
 *      Author: marek
 */

#ifndef FIRMWARE_FIRST_LAY_CAL_H_
#define FIRMWARE_FIRST_LAY_CAL_H_
#include <stdint.h>

void lay1cal_preheat();
void lay1cal_intro_line(char *cmd_buffer, uint8_t filament);
void lay1cal_before_meander();
void lay1cal_meander();



#endif /* FIRMWARE_FIRST_LAY_CAL_H_ */
