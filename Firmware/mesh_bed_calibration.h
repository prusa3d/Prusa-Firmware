#ifndef MESH_BED_CALIBRATION_H
#define MESH_BED_CALIBRATION_H

extern void find_bed_induction_sensor_point_z();
extern bool find_bed_induction_sensor_point_xy();

extern bool find_bed_offset_and_skew();
extern bool improve_bed_offset_and_skew(int8_t method);
extern void reset_bed_offset_and_skew();

#endif /* MESH_BED_CALIBRATION_H */
