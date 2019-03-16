#ifndef MESH_BED_CALIBRATION_H
#define MESH_BED_CALIBRATION_H

#define BED_ZERO_REF_X (- 22.f + X_PROBE_OFFSET_FROM_EXTRUDER) // -22 + 23 = 1
#define BED_ZERO_REF_Y (- 0.6f + Y_PROBE_OFFSET_FROM_EXTRUDER + 4.f) // -0.6 + 5 + 4 = 8.4

#ifdef HEATBED_V2

#define BED_X0 (2.f - BED_ZERO_REF_X) //1
#define BED_Y0 (9.4f - BED_ZERO_REF_Y) //1
#define BED_Xn (206.f - BED_ZERO_REF_X) //205
#define BED_Yn (213.4f - BED_ZERO_REF_Y) //205

#else

#define BED_X0 (13.f - BED_ZERO_REF_X)
#define BED_Y0 (8.4f - BED_ZERO_REF_Y)
#define BED_Xn (216.f - BED_ZERO_REF_X)
#define BED_Yn (202.4f - BED_ZERO_REF_Y)

#endif //not HEATBED_V2

#define BED_X(i, n) ((float)i * (BED_Xn - BED_X0) / (n - 1) + BED_X0)
#define BED_Y(i, n)  ((float)i * (BED_Yn - BED_Y0) / (n - 1) + BED_Y0)

// Exact positions of the print head above the bed reference points, in the world coordinates.
// The world coordinates match the machine coordinates only in case, when the machine
// is built properly, the end stops are at the correct positions and the axes are perpendicular.
extern const float bed_ref_points_4[] PROGMEM;

extern const float bed_skew_angle_mild;
extern const float bed_skew_angle_extreme;

// Is the world2machine correction activated?
enum World2MachineCorrectionMode
{
	WORLD2MACHINE_CORRECTION_NONE  = 0,
	WORLD2MACHINE_CORRECTION_SHIFT = 1,
	WORLD2MACHINE_CORRECTION_SKEW  = 2,
};
extern uint8_t world2machine_correction_mode;
// 2x2 transformation matrix from the world coordinates to the machine coordinates.
// Corrects for the rotation and skew of the machine axes.
// Used by the planner's plan_buffer_line() and plan_set_position().
extern float world2machine_rotation_and_skew[2][2];
extern float world2machine_rotation_and_skew_inv[2][2];
// Shift of the machine zero point, in the machine coordinates.
extern float world2machine_shift[2];

extern void world2machine_reset();
extern void world2machine_revert_to_uncorrected();
extern void world2machine_initialize();
extern void world2machine_read_valid(float vec_x[2], float vec_y[2], float cntr[2]);
extern void world2machine_update_current();

inline void world2machine(float &x, float &y)
{
	if (world2machine_correction_mode == WORLD2MACHINE_CORRECTION_NONE) {
		// No correction.
	} else {
		if (world2machine_correction_mode & WORLD2MACHINE_CORRECTION_SKEW) {
			// Firs the skew & rotation correction.
			float out_x = world2machine_rotation_and_skew[0][0] * x + world2machine_rotation_and_skew[0][1] * y;
			float out_y = world2machine_rotation_and_skew[1][0] * x + world2machine_rotation_and_skew[1][1] * y;
			x = out_x;
			y = out_y;
		}
		if (world2machine_correction_mode & WORLD2MACHINE_CORRECTION_SHIFT) {
			// Then add the offset.
			x += world2machine_shift[0];
			y += world2machine_shift[1];
		}
	}
}

inline void world2machine(const float &x, const float &y, float &out_x, float &out_y)
{
    out_x = x;
    out_y = y;
    world2machine(out_x, out_y);
}

inline void machine2world(float x, float y, float &out_x, float &out_y)
{
	if (world2machine_correction_mode == WORLD2MACHINE_CORRECTION_NONE) {
		// No correction.
		out_x = x;
		out_y = y;
	} else {
		if (world2machine_correction_mode & WORLD2MACHINE_CORRECTION_SHIFT) {
			// Then add the offset.
			x -= world2machine_shift[0];
			y -= world2machine_shift[1];
		}
		if (world2machine_correction_mode & WORLD2MACHINE_CORRECTION_SKEW) {
			// Firs the skew & rotation correction.
			out_x = world2machine_rotation_and_skew_inv[0][0] * x + world2machine_rotation_and_skew_inv[0][1] * y;
			out_y = world2machine_rotation_and_skew_inv[1][0] * x + world2machine_rotation_and_skew_inv[1][1] * y;
		}
	}
}

inline void machine2world(float &x, float &y)
{
	if (world2machine_correction_mode == WORLD2MACHINE_CORRECTION_NONE) {
		// No correction.
	} else {
		if (world2machine_correction_mode & WORLD2MACHINE_CORRECTION_SHIFT) {
			// Then add the offset.
			x -= world2machine_shift[0];
			y -= world2machine_shift[1];
		}
		if (world2machine_correction_mode & WORLD2MACHINE_CORRECTION_SKEW) {
			// Firs the skew & rotation correction.
			float out_x = world2machine_rotation_and_skew_inv[0][0] * x + world2machine_rotation_and_skew_inv[0][1] * y;
			float out_y = world2machine_rotation_and_skew_inv[1][0] * x + world2machine_rotation_and_skew_inv[1][1] * y;
			x = out_x;
			y = out_y;
		}
	}
}

inline bool world2machine_clamp(float &x, float &y)
{
	bool clamped = false;
	float tmpx, tmpy;
    world2machine(x, y, tmpx, tmpy);
    if (tmpx < X_MIN_POS) {
        tmpx = X_MIN_POS;
        clamped = true;
    }
    if (tmpy < Y_MIN_POS) {
        tmpy = Y_MIN_POS;
        clamped = true;
    }
    if (tmpx > X_MAX_POS) {
        tmpx = X_MAX_POS;
        clamped = true;
    }
    if (tmpy > Y_MAX_POS) {
        tmpy = Y_MAX_POS;
        clamped = true;
    }
    if (clamped)
        machine2world(tmpx, tmpy, x, y);
    return clamped;
}

extern bool find_bed_induction_sensor_point_z(float minimum_z = -10.f, uint8_t n_iter = 3, int verbosity_level = 0);
extern bool find_bed_induction_sensor_point_xy(int verbosity_level = 0);
extern void go_home_with_z_lift();

/**
 * @brief Bed skew and offest detection result
 *
 * Positive or zero: ok
 * Negative: failed
 */

enum BedSkewOffsetDetectionResultType {
	// Detection failed, some point was not found.
	BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND   = -1, //!< Point not found.
	BED_SKEW_OFFSET_DETECTION_FITTING_FAILED    = -2, //!< Fitting failed
	
	// Detection finished with success.
	BED_SKEW_OFFSET_DETECTION_PERFECT 			= 0,  //!< Perfect.
	BED_SKEW_OFFSET_DETECTION_SKEW_MILD			= 1,  //!< Mildly skewed.
	BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME		= 2   //!< Extremely skewed.
};

extern BedSkewOffsetDetectionResultType find_bed_offset_and_skew(int8_t verbosity_level, uint8_t &too_far_mask);
#ifndef NEW_XYZCAL
extern BedSkewOffsetDetectionResultType improve_bed_offset_and_skew(int8_t method, int8_t verbosity_level, uint8_t &too_far_mask);
#endif //NEW_XYZCAL

extern bool sample_mesh_and_store_reference();

extern void reset_bed_offset_and_skew();
extern bool is_bed_z_jitter_data_valid();

// Scan the mesh bed induction points one by one by a left-right zig-zag movement,
// write the trigger coordinates to the serial line.
// Useful for visualizing the behavior of the bed induction detector.
extern bool scan_bed_induction_points(int8_t verbosity_level);

// Load Z babystep value from the EEPROM into babystepLoadZ, 
// but don't apply it through the planner. This is useful on wake up
// after power panic, when it is expected, that the baby step has been already applied.
extern void babystep_load();

// Apply Z babystep value from the EEPROM through the planner.
extern void babystep_apply();

// Undo the current Z babystep value.
extern void babystep_undo();

// Reset the current babystep counter without moving the axes.
extern void babystep_reset();


extern void count_xyz_details(float (&distanceMin)[2]);
extern bool sample_z();
/*
typedef enum
{
	e_MBL_FAST, e_MBL_OPTIMAL, e_MBL_PREC
} e_MBL_TYPE;
*/
//extern e_MBL_TYPE e_mbl_type;
//extern void mbl_mode_set();
//extern void mbl_mode_init();
extern void mbl_settings_init();

extern bool mbl_point_measurement_valid(uint8_t ix, uint8_t iy, uint8_t meas_points, bool zigzag);
extern void mbl_interpolation(uint8_t meas_points);
#endif /* MESH_BED_CALIBRATION_H */
