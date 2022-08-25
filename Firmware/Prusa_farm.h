#pragma once

#include <inttypes.h>
#include "Configuration.h"

#ifdef PRUSA_FARM
extern uint8_t farm_mode;
#else
#define farm_mode 0
#endif

extern void prusa_statistics(uint8_t _message);
extern void prusa_statistics_update_from_status_screen();
extern void prusa_statistics_update_from_lcd_update();
extern void farm_mode_init();
extern bool farm_prusa_code_seen();
extern void farm_gcode_g98();
extern void farm_gcode_g99();
