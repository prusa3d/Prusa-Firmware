//xyzcal.h - xyz calibration with image processing
#pragma once

#include <inttypes.h>

#include "mesh_bed_calibration.h"

extern void xyzcal_measure_enter(void);
extern void xyzcal_measure_leave(void);

extern bool xyzcal_lineXYZ_to(int16_t x, int16_t y, int16_t z, uint16_t delay_us, int8_t check_pinda);

extern bool xyzcal_spiral2(int16_t cx, int16_t cy, int16_t z0, int16_t dz, int16_t radius, int16_t rotation, uint16_t delay_us, int8_t check_pinda, uint16_t* pad);

extern bool xyzcal_spiral8(int16_t cx, int16_t cy, int16_t z0, int16_t dz, int16_t radius, uint16_t delay_us, int8_t check_pinda, uint16_t* pad);

//extern int8_t xyzcal_measure_pinda_hysteresis(int16_t min_z, int16_t max_z, uint16_t delay_us, uint8_t samples);

extern BedSkewOffsetDetectionResultType xyzcal_find_bed_induction_sensor_point_xy();
