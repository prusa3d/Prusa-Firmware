//! @file
//! @date Jun 10, 2019
//! @author Marek Bel

#ifndef FIRMWARE_FIRST_LAY_CAL_H_
#define FIRMWARE_FIRST_LAY_CAL_H_
#include <stdint.h>

void lay1cal_wait_preheat();
[[nodiscard]] bool lay1cal_load_filament(char *cmd_buffer, uint8_t filament);
void lay1cal_intro_line(bool skipExtraPurge, float layer_height, float extrusion_width);
void lay1cal_before_meander();
void lay1cal_meander_start(float layer_height, float extrusion_width);
void lay1cal_meander(float layer_height, float extrusion_width);
void lay1cal_square(uint8_t step, float layer_height, float extrusion_width);
void lay1cal_finish(bool mmu_enabled);

#endif /* FIRMWARE_FIRST_LAY_CAL_H_ */
