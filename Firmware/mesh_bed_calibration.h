#ifndef MESH_BED_CALIBRATION_H
#define MESH_BED_CALIBRATION_H

// Exact positions of the print head above the bed reference points, in the world coordinates.
// The world coordinates match the machine coordinates only in case, when the machine
// is built properly, the end stops are at the correct positions and the axes are perpendicular.
extern const float bed_ref_points[] PROGMEM;

// 2x2 transformation matrix from the world coordinates to the machine coordinates.
// Corrects for the rotation and skew of the machine axes.
// Used by the planner's plan_buffer_line() and plan_set_position().
extern float world2machine_rotation_and_skew[2][2];
// Shift of the machine zero point, in the machine coordinates.
extern float world2machine_shift[2];

// Resets the transformation to identity.
extern void world2machine_reset();
// Loads the transformation from the EEPROM, if available.
extern void world2machine_initialize();

// When switching from absolute to corrected coordinates,
// this will apply an inverse world2machine transformation
// to current_position[x,y].
extern void world2machine_update_current();


extern void find_bed_induction_sensor_point_z();
extern bool find_bed_induction_sensor_point_xy();

extern bool find_bed_offset_and_skew();
extern bool improve_bed_offset_and_skew(int8_t method);
extern void reset_bed_offset_and_skew();

#endif /* MESH_BED_CALIBRATION_H */
