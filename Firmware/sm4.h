//sm4.h - simple 4-axis stepper control
#ifndef _SM4_H
#define _SM4_H

#include <inttypes.h>
#include "config.h"


#if defined(__cplusplus)
extern "C" {
#endif //defined(__cplusplus)


// callback prototype for stop condition (return 0 - continue, return 1 - stop)
typedef uint8_t (*sm4_stop_cb_t)();

// callback prototype for updating position counters
typedef void (*sm4_update_pos_cb_t)(uint16_t dx, uint16_t dy, uint16_t dz, uint16_t de);

// callback prototype for calculating delay
typedef uint16_t (*sm4_calc_delay_cb_t)(uint16_t nd, uint16_t dd);


// callback pointer - stop
extern sm4_stop_cb_t sm4_stop_cb;

// callback pointer - update_pos
extern sm4_update_pos_cb_t sm4_update_pos_cb;

// callback pointer - calc_delay
extern sm4_calc_delay_cb_t sm4_calc_delay_cb;


// returns direction for single axis (0 - positive, 1 - negative)
extern uint8_t sm4_get_dir(uint8_t axis);

// set direction for single axis (0 - positive, 1 - negative)
extern void sm4_set_dir(uint8_t axis, uint8_t dir);

// returns direction of all axes as bitmask (0 - positive, 1 - negative)
extern uint8_t sm4_get_dir_bits(void);

// set direction for all axes as bitmask (0 - positive, 1 - negative)
extern void sm4_set_dir_bits(uint8_t dir_bits);

// step axes by bitmask
extern void sm4_do_step(uint8_t axes_mask);

// xyze linear-interpolated relative move, returns remaining diagonal steps (>0 means stoped)
extern uint16_t sm4_line_xyze_ui(uint16_t dx, uint16_t dy, uint16_t dz, uint16_t de);


#if defined(__cplusplus)
}
#endif //defined(__cplusplus)
#endif //_SM4_H
