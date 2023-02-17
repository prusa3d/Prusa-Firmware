#pragma once

#include <inttypes.h>
#include "config.h"

#define FARM_PREHEAT_HOTEND_TEMP 250
#define FARM_PREHEAT_HPB_TEMP 80

#define FARM_DEFAULT_SAFETYTIMER_TIME_ms (45*60*1000ul)
#define NC_TIME 10 //time in s for periodic important status messages sending which needs reponse from monitoring
#define NC_BUTTON_LONG_PRESS 15 //time in s

//#define FARM_CONNECT_MESSAGE

#ifdef PRUSA_FARM
extern uint8_t farm_mode;
#else
#define farm_mode 0
#endif

#ifdef PRUSA_M28
extern bool prusa_sd_card_upload;
extern void serial_read_stream();
#endif
extern void prusa_statistics(uint8_t _message);
extern void prusa_statistics_update_from_status_screen();
extern void prusa_statistics_update_from_lcd_update();
extern void farm_mode_init();
extern bool farm_prusa_code_seen();
extern void farm_gcode_g98();
extern void farm_gcode_g99();
extern void farm_disable();
