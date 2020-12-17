//xyzcal.h - xyz calibration with image processing
#ifndef _XYZCAL_H
#define _XYZCAL_H

#include <inttypes.h>


extern void xyzcal_meassure_enter(void);

extern void xyzcal_meassure_leave(void);

extern bool xyzcal_lineXYZ_to(int16_t x, int16_t y, int16_t z, uint16_t delay_us, int8_t check_pinda);

extern bool xyzcal_spiral2(int16_t cx, int16_t cy, int16_t z0, int16_t dz, int16_t radius, int16_t rotation, uint16_t delay_us, int8_t check_pinda, uint16_t* pad);

extern bool xyzcal_spiral8(int16_t cx, int16_t cy, int16_t z0, int16_t dz, int16_t radius, uint16_t delay_us, int8_t check_pinda, uint16_t* pad);

//extern int8_t xyzcal_meassure_pinda_hysterezis(int16_t min_z, int16_t max_z, uint16_t delay_us, uint8_t samples);

extern bool xyzcal_searchZ(void);

extern bool xyzcal_scan_and_process(void);

extern bool xyzcal_find_bed_induction_sensor_point_xy(void);


#endif //_XYZCAL_H
