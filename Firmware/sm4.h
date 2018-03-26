//sm4.h - simple 4-axis stepper control
#ifndef _SM4_H
#define _SM4_H

#include <inttypes.h>
#include "config.h"


#if defined(__cplusplus)
extern "C" {
#endif //defined(__cplusplus)


// callback prototype for stop condition (return 0 - continue, return 1 - stop)
typedef uint8_t (*sm4_stop_cb)();

// callback prototype for updating position counters
typedef void (*sm4_update_pos_cb)(uint16_t dx, uint16_t dy, uint16_t dz, uint16_t de);

// callback pointer
extern sm4_update_pos_cb sm4_update_pos;

// returns direction for single axis
extern uint8_t sm4_get_dir(uint8_t axis);

// set direction for single axis (0 - positive, 1 - negative)
extern void sm4_set_dir(uint8_t axis, uint8_t dir);

// returns direction of all axes as bitmask
extern uint8_t sm4_get_dir_bits(void);

// set direction for all axes as bitmask (0 - positive, 1 - negative)
extern void sm4_set_dir_bits(uint8_t msk);

// step axes by bitmask
extern void sm4_do_step(uint8_t axes_mask);

// xyz linear-interpolated relative move
uint8_t sm4_line_xyz_ui(uint16_t dx, uint16_t dy, uint16_t dz, uint16_t delay_us, sm4_stop_cb stop);


#if defined(__cplusplus)
}
#endif //defined(__cplusplus)
#endif //_SM4_H
