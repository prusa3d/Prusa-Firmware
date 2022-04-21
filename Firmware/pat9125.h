#pragma once

#include <inttypes.h>

extern int16_t pat9125_x;
extern int16_t pat9125_y;
extern uint8_t pat9125_b;
extern uint8_t pat9125_s;

extern uint8_t pat9125_probe(void);     // Return non-zero if PAT9125 can be trivially detected
extern uint8_t pat9125_init(void);
extern uint8_t pat9125_update(void);    // update all sensor data
extern uint8_t pat9125_update_y(void);  // update _y only
extern uint8_t pat9125_update_bs(void); // update _b/_s only
