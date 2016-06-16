#include "Marlin.h"
#include "Configuration.h"
#include "language_all.h"
#include "mesh_bed_calibration.h"
#include "mesh_bed_leveling.h"
#include "stepper.h"
#include "ultralcd.h"
// #include "qr_solve.h"

extern float home_retract_mm_ext(int axis);

static inline void go_xyz(float x, float y, float z, float fr)
{
    plan_buffer_line(x, y, z, current_position[E_AXIS], fr, active_extruder);
    st_synchronize();
}

static inline void go_xy(float x, float y, float fr)
{
    plan_buffer_line(x, y, current_position[Z_AXIS], current_position[E_AXIS], fr, active_extruder);
    st_synchronize();
}

static inline void go_to_current(float fr)
{
    plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], fr, active_extruder);
    st_synchronize();
}

static inline void update_current_position_xyz()
{
      current_position[X_AXIS] = st_get_position_mm(X_AXIS);
      current_position[Y_AXIS] = st_get_position_mm(Y_AXIS);
      current_position[Z_AXIS] = st_get_position_mm(Z_AXIS);
      plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
}

// At the current position, find the Z stop.
inline void find_bed_induction_sensor_point_z() 
{
    bool endstops_enabled  = enable_endstops(true);
    bool endstop_z_enabled = enable_z_endstop(false);

    // move down until you find the bed
    current_position[Z_AXIS] = -10;
    go_to_current(homing_feedrate[Z_AXIS]/60);
    // we have to let the planner know where we are right now as it is not where we said to go.
    update_current_position_xyz();

    // move up the retract distance
    current_position[Z_AXIS] += home_retract_mm_ext(Z_AXIS);
    go_to_current(homing_feedrate[Z_AXIS]/60);
    
    // move back down slowly to find bed
    current_position[Z_AXIS] -= home_retract_mm_ext(Z_AXIS) * 2;
    go_to_current(homing_feedrate[Z_AXIS]/(4*60));
    // we have to let the planner know where we are right now as it is not where we said to go.
    update_current_position_xyz();

    enable_endstops(endstops_enabled);
    enable_z_endstop(endstop_z_enabled);
}

// Search around the current_position[X,Y],
// look for the induction sensor response.
// Adjust the  current_position[X,Y,Z] to the center of the target dot and its response Z coordinate.
#define FIND_BED_INDUCTION_SENSOR_POINT_X_RADIUS (8.f)
#define FIND_BED_INDUCTION_SENSOR_POINT_Y_RADIUS (6.f)
#define FIND_BED_INDUCTION_SENSOR_POINT_XY_STEP  (1.f)
#define FIND_BED_INDUCTION_SENSOR_POINT_Z_STEP   (0.5f)
inline bool find_bed_induction_sensor_point_xy()
{
    float feedrate = homing_feedrate[X_AXIS] / 60.f;
    bool found = false;

    {
        float x0 = current_position[X_AXIS] - FIND_BED_INDUCTION_SENSOR_POINT_X_RADIUS;
        float x1 = current_position[X_AXIS] + FIND_BED_INDUCTION_SENSOR_POINT_X_RADIUS;
        float y0 = current_position[Y_AXIS] - FIND_BED_INDUCTION_SENSOR_POINT_Y_RADIUS;
        float y1 = current_position[Y_AXIS] + FIND_BED_INDUCTION_SENSOR_POINT_Y_RADIUS;
        uint8_t nsteps_y;
        uint8_t i;
        if (x0 < X_MIN_POS)
            x0 = X_MIN_POS;
        if (x1 > X_MAX_POS)
            x1 = X_MAX_POS;
        if (y0 < Y_MIN_POS)
            y0 = Y_MIN_POS;
        if (y1 > Y_MAX_POS)
            y1 = Y_MAX_POS;
        nsteps_y = int(ceil((y1 - y0) / FIND_BED_INDUCTION_SENSOR_POINT_XY_STEP));

        enable_endstops(false);
        bool  dir_positive = true;

//        go_xyz(current_position[X_AXIS], current_position[Y_AXIS], MESH_HOME_Z_SEARCH, homing_feedrate[Z_AXIS]/60);
        go_xyz(x0, y0, current_position[Z_AXIS], feedrate);
        // Continously lower the Z axis.
        endstops_hit_on_purpose();
        enable_z_endstop(true);
        while (current_position[Z_AXIS] > -10.f) {
            // Do nsteps_y zig-zag movements.
            current_position[Y_AXIS] = y0;
            for (i = 0; i < nsteps_y; current_position[Y_AXIS] += (y1 - y0) / float(nsteps_y - 1), ++ i) {
                // Run with a slightly decreasing Z axis, zig-zag movement. Stop at the Z end-stop.
                current_position[Z_AXIS] -= FIND_BED_INDUCTION_SENSOR_POINT_Z_STEP / float(nsteps_y);
                go_xyz(dir_positive ? x1 : x0, current_position[Y_AXIS], current_position[Z_AXIS], feedrate);
                dir_positive = ! dir_positive;
                if (endstop_z_hit_on_purpose())
                    goto endloop;
            }
            for (i = 0; i < nsteps_y; current_position[Y_AXIS] -= (y1 - y0) / float(nsteps_y - 1), ++ i) {
                // Run with a slightly decreasing Z axis, zig-zag movement. Stop at the Z end-stop.
                current_position[Z_AXIS] -= FIND_BED_INDUCTION_SENSOR_POINT_Z_STEP / float(nsteps_y);
                go_xyz(dir_positive ? x1 : x0, current_position[Y_AXIS], current_position[Z_AXIS], feedrate);
                dir_positive = ! dir_positive;
                if (endstop_z_hit_on_purpose())
                    goto endloop;
            }
        }
        endloop:
//        SERIAL_ECHOLN("First hit");

        // we have to let the planner know where we are right now as it is not where we said to go.
        update_current_position_xyz();

        // Search in this plane for the first hit. Zig-zag first in X, then in Y axis.
        for (int8_t iter = 0; iter < 3; ++ iter) {
            if (iter > 0) {
                // Slightly lower the Z axis to get a reliable trigger.
                current_position[Z_AXIS] -= 0.02f;
                go_xyz(current_position[X_AXIS], current_position[Y_AXIS], MESH_HOME_Z_SEARCH, homing_feedrate[Z_AXIS]/60);
            }

            // Do nsteps_y zig-zag movements.
            float a, b;
            enable_endstops(false);
            enable_z_endstop(false);
            current_position[Y_AXIS] = y0;
            go_xy(x0, current_position[Y_AXIS], feedrate);
            enable_z_endstop(true);
            found = false;
            for (i = 0, dir_positive = true; i < nsteps_y; current_position[Y_AXIS] += (y1 - y0) / float(nsteps_y - 1), ++ i, dir_positive = ! dir_positive) {
                go_xy(dir_positive ? x1 : x0, current_position[Y_AXIS], feedrate);
                if (endstop_z_hit_on_purpose()) {
                    found = true;
                    break;
                }
            }
            update_current_position_xyz();
            if (! found) {
//                SERIAL_ECHOLN("Search in Y - not found");
                continue;
            }
//            SERIAL_ECHOLN("Search in Y - found");
            a = current_position[Y_AXIS];

            enable_z_endstop(false);
            current_position[Y_AXIS] = y1;
            go_xy(x0, current_position[Y_AXIS], feedrate);
            enable_z_endstop(true);
            found = false;
            for (i = 0, dir_positive = true; i < nsteps_y; current_position[Y_AXIS] -= (y1 - y0) / float(nsteps_y - 1), ++ i, dir_positive = ! dir_positive) {
                go_xy(dir_positive ? x1 : x0, current_position[Y_AXIS], feedrate);
                if (endstop_z_hit_on_purpose()) {
                    found = true;
                    break;
                }
            }
            update_current_position_xyz();
            if (! found) {
//                SERIAL_ECHOLN("Search in Y2 - not found");
                continue;
            }
//            SERIAL_ECHOLN("Search in Y2 - found");
            b = current_position[Y_AXIS];
            current_position[Y_AXIS] = 0.5f * (a + b);

            // Search in the X direction along a cross.
            found = false;
            enable_z_endstop(false);
            go_xy(x0, current_position[Y_AXIS], feedrate);
            enable_z_endstop(true);
            go_xy(x1, current_position[Y_AXIS], feedrate);
            update_current_position_xyz();
            if (! endstop_z_hit_on_purpose()) {
//                SERIAL_ECHOLN("Search X span 0 - not found");
                continue;
            }
//            SERIAL_ECHOLN("Search X span 0 - found");
            a = current_position[X_AXIS];
            enable_z_endstop(false);
            go_xy(x1, current_position[Y_AXIS], feedrate);
            enable_z_endstop(true);
            go_xy(x0, current_position[Y_AXIS], feedrate);
            update_current_position_xyz();
            if (! endstop_z_hit_on_purpose()) {
//                SERIAL_ECHOLN("Search X span 1 - not found");
                continue;
            }
//            SERIAL_ECHOLN("Search X span 1 - found");
            b = current_position[X_AXIS];
            // Go to the center.
            enable_z_endstop(false);
            current_position[X_AXIS] = 0.5f * (a + b);
            go_xy(current_position[X_AXIS], current_position[Y_AXIS], feedrate);
            found = true;

#if 1
            // Search in the Y direction along a cross.
            found = false;
            enable_z_endstop(false);
            go_xy(current_position[X_AXIS], y0, feedrate);
            enable_z_endstop(true);
            go_xy(current_position[X_AXIS], y1, feedrate);
            update_current_position_xyz();
            if (! endstop_z_hit_on_purpose()) {
//                SERIAL_ECHOLN("Search Y2 span 0 - not found");
                continue;
            }
//            SERIAL_ECHOLN("Search Y2 span 0 - found");
            a = current_position[Y_AXIS];
            enable_z_endstop(false);
            go_xy(current_position[X_AXIS], y1, feedrate);
            enable_z_endstop(true);
            go_xy(current_position[X_AXIS], y0, feedrate);
            update_current_position_xyz();
            if (! endstop_z_hit_on_purpose()) {
//                SERIAL_ECHOLN("Search Y2 span 1 - not found");
                continue;
            }
//            SERIAL_ECHOLN("Search Y2 span 1 - found");
            b = current_position[Y_AXIS];
            // Go to the center.
            enable_z_endstop(false);
            current_position[Y_AXIS] = 0.5f * (a + b);
            go_xy(current_position[X_AXIS], current_position[Y_AXIS], feedrate);
            found = true;
#endif
            break;
        }
    }

    enable_z_endstop(false);
    return found;
}

// Search around the current_position[X,Y,Z].
// It is expected, that the induction sensor is switched on at the current position.
// Look around this center point by painting a star around the point.
inline bool improve_bed_induction_sensor_point()
{
    static const float search_radius = 8.f;

    bool  endstops_enabled  = enable_endstops(false);
    bool  endstop_z_enabled = enable_z_endstop(false);
    bool  found = false;
    float feedrate = homing_feedrate[X_AXIS] / 60.f;
    float center_old_x = current_position[X_AXIS];
    float center_old_y = current_position[Y_AXIS];
    float center_x = 0.f;
    float center_y = 0.f;

    for (uint8_t iter = 0; iter < 4; ++ iter) {
        switch (iter) {
        case 0:
            destination[X_AXIS] = center_old_x - search_radius * 0.707;
            destination[Y_AXIS] = center_old_y - search_radius * 0.707;
            break;
        case 1:
            destination[X_AXIS] = center_old_x + search_radius * 0.707;
            destination[Y_AXIS] = center_old_y + search_radius * 0.707;
            break;
        case 2:
            destination[X_AXIS] = center_old_x + search_radius * 0.707;
            destination[Y_AXIS] = center_old_y - search_radius * 0.707;
            break;
        case 3:
        default:
            destination[X_AXIS] = center_old_x - search_radius * 0.707;
            destination[Y_AXIS] = center_old_y + search_radius * 0.707;
            break;
        }

        // Trim the vector from center_old_[x,y] to destination[x,y] by the bed dimensions.
        float vx = destination[X_AXIS] - center_old_x;
        float vy = destination[Y_AXIS] - center_old_y;
        float l  = sqrt(vx*vx+vy*vy);
        float t;
        if (destination[X_AXIS] < X_MIN_POS) {
            // Exiting the bed at xmin.
            t = (center_x - X_MIN_POS) / l;
            destination[X_AXIS] = X_MIN_POS;
            destination[Y_AXIS] = center_old_y + t * vy;
        } else if (destination[X_AXIS] > X_MAX_POS) {
            // Exiting the bed at xmax.
            t = (X_MAX_POS - center_x) / l;
            destination[X_AXIS] = X_MAX_POS;
            destination[Y_AXIS] = center_old_y + t * vy;
        }
        if (destination[Y_AXIS] < Y_MIN_POS) {
            // Exiting the bed at ymin.
            t = (center_y - Y_MIN_POS) / l;
            destination[X_AXIS] = center_old_x + t * vx;
            destination[Y_AXIS] = Y_MIN_POS;
        } else if (destination[Y_AXIS] > Y_MAX_POS) {
            // Exiting the bed at xmax.
            t = (Y_MAX_POS - center_y) / l;
            destination[X_AXIS] = center_old_x + t * vx;
            destination[Y_AXIS] = Y_MAX_POS;
        }

        // Move away from the measurement point.
        enable_endstops(false);
        go_xy(destination[X_AXIS], destination[Y_AXIS], feedrate);
        // Move towards the measurement point, until the induction sensor triggers.
        enable_endstops(true);
        go_xy(center_old_x, center_old_y, feedrate);
        update_current_position_xyz();
        center_x += current_position[X_AXIS];
        center_y += current_position[Y_AXIS];
    }

    // Calculate the new center, move to the new center.
    center_x /= 4.f;
    center_y /= 4.f;
    current_position[X_AXIS] = center_x;
    current_position[Y_AXIS] = center_y;
    enable_endstops(false);
    go_xy(current_position[X_AXIS], current_position[Y_AXIS], feedrate);

    enable_endstops(endstops_enabled);
    enable_z_endstop(endstop_z_enabled);
    return found;
}

// Search around the current_position[X,Y,Z].
// It is expected, that the induction sensor is switched on at the current position.
// Look around this center point by painting a star around the point.
#define IMPROVE_BED_INDUCTION_SENSOR_SEARCH_RADIUS (8.f)
inline bool improve_bed_induction_sensor_point2(bool lift_z_on_min_y)
{
    float center_old_x = current_position[X_AXIS];
    float center_old_y = current_position[Y_AXIS];
    float a, b;

    enable_endstops(false);

    {
        float x0 = center_old_x - IMPROVE_BED_INDUCTION_SENSOR_SEARCH_RADIUS;
        float x1 = center_old_x + IMPROVE_BED_INDUCTION_SENSOR_SEARCH_RADIUS;
        if (x0 < X_MIN_POS)
            x0 = X_MIN_POS;
        if (x1 > X_MAX_POS)
            x1 = X_MAX_POS;

        // Search in the X direction along a cross.
        enable_z_endstop(false);
        go_xy(x0, current_position[Y_AXIS], homing_feedrate[X_AXIS] / 60.f);
        enable_z_endstop(true);
        go_xy(x1, current_position[Y_AXIS], homing_feedrate[X_AXIS] / 60.f);
        update_current_position_xyz();
        if (! endstop_z_hit_on_purpose())
            return false;
        a = current_position[X_AXIS];
        enable_z_endstop(false);
        go_xy(x1, current_position[Y_AXIS], homing_feedrate[X_AXIS] / 60.f);
        enable_z_endstop(true);
        go_xy(x0, current_position[Y_AXIS], homing_feedrate[X_AXIS] / 60.f);
        update_current_position_xyz();
        if (! endstop_z_hit_on_purpose())
            return false;
        b = current_position[X_AXIS];

        // Go to the center.
        enable_z_endstop(false);
        current_position[X_AXIS] = 0.5f * (a + b);
        go_xy(current_position[X_AXIS], current_position[Y_AXIS], homing_feedrate[X_AXIS] / 60.f);
    }

    {
        float y0 = center_old_y - IMPROVE_BED_INDUCTION_SENSOR_SEARCH_RADIUS;
        float y1 = center_old_y + IMPROVE_BED_INDUCTION_SENSOR_SEARCH_RADIUS;
        if (y0 < Y_MIN_POS)
            y0 = Y_MIN_POS;
        if (y1 > Y_MAX_POS)
            y1 = Y_MAX_POS;

        // Search in the Y direction along a cross.
        enable_z_endstop(false);
        go_xy(current_position[X_AXIS], y0, homing_feedrate[X_AXIS] / 60.f);
        if (lift_z_on_min_y) {
            // The first row of points are very close to the end stop.
            // Lift the sensor to disengage the trigger. This is necessary because of the sensor hysteresis.
            go_xyz(current_position[X_AXIS], y0, current_position[Z_AXIS]+5.f, homing_feedrate[Z_AXIS] / 60.f);
            // and go back.
            go_xyz(current_position[X_AXIS], y0, current_position[Z_AXIS], homing_feedrate[Z_AXIS] / 60.f);
        }
        if (lift_z_on_min_y && (READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING) == 1) {
            // Already triggering before we started the move.
            // Shift the trigger point slightly outwards.
            a = current_position[Y_AXIS] - 1.5f;
        } else {
            enable_z_endstop(true);
            go_xy(current_position[X_AXIS], y1, homing_feedrate[X_AXIS] / 60.f);
            update_current_position_xyz();
            if (! endstop_z_hit_on_purpose())
                return false;
            a = current_position[Y_AXIS];
        }
        enable_z_endstop(false);
        go_xy(current_position[X_AXIS], y1, homing_feedrate[X_AXIS] / 60.f);
        enable_z_endstop(true);
        go_xy(current_position[X_AXIS], y0, homing_feedrate[X_AXIS] / 60.f);
        update_current_position_xyz();
        if (! endstop_z_hit_on_purpose())
            return false;
        b = current_position[Y_AXIS];

        // Go to the center.
        enable_z_endstop(false);
        current_position[Y_AXIS] = 0.5f * (a + b);
        go_xy(current_position[X_AXIS], current_position[Y_AXIS], homing_feedrate[X_AXIS] / 60.f);
    }

    return true;
}

#define MESH_BED_CALIBRATION_SHOW_LCD

bool find_bed_offset_and_skew()
{
    // Reusing the z_values memory for the measurement cache.
    // 7x7=49 floats, good for 16 (x,y,z) vectors.
    float *pts = &mbl.z_values[0][0];
    float *vec_x = pts + 3 * 4;
    float *vec_y = vec_x + 3;
    float *cntr  = vec_y + 3;
    memset(pts, 0, sizeof(float) * 7 * 7);

#ifdef MESH_BED_CALIBRATION_SHOW_LCD
    lcd_implementation_clear();
    lcd_print_at_PGM(0, 0, MSG_FIND_BED_OFFSET_AND_SKEW_LINE1);
#endif /* MESH_BED_CALIBRATION_SHOW_LCD */

    // Collect the rear 2x3 points.
    current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
    for (int k = 0; k < 4; ++ k) {
#ifdef MESH_BED_CALIBRATION_SHOW_LCD
        lcd_print_at_PGM(0, 1, MSG_FIND_BED_OFFSET_AND_SKEW_LINE2);
        lcd_implementation_print_at(0, 2, k+1);
        lcd_printPGM(MSG_FIND_BED_OFFSET_AND_SKEW_LINE3);
#endif /* MESH_BED_CALIBRATION_SHOW_LCD */

        int i, j;
        switch (k) {
            case 0: i = 1; j = 0; break;
            case 1: i = 2; j = 1; break;
            case 2: i = 1; j = 2; break;
            case 3: i = 0; j = 1; break;
        }
        float *pt = pts + k * 3;
        // Go up to z_initial.
        go_to_current(homing_feedrate[Z_AXIS] / 60.f);
        // Go to the measurement point position.
        mbl.get_meas_xy(i, j, current_position[X_AXIS], current_position[Y_AXIS], true); // use default, uncorrected coordinates
        go_to_current(homing_feedrate[X_AXIS] / 60.f);
        if (! find_bed_induction_sensor_point_xy())
            return false;
        find_bed_induction_sensor_point_z();
        pt[0] = current_position[X_AXIS];
        pt[1] = current_position[Y_AXIS];
        pt[2] = current_position[Z_AXIS];
        // Start searching for the other points at 3mm above the last point.
        current_position[Z_AXIS] += 3.f;
        cntr[0] += pt[0];
        cntr[1] += pt[1];
        cntr[2] += pt[2];
    }

    // Average the X and Y vectors. They may not be perpendicular, if the printer is built incorrectly.
    {
        float  len;
        // Average the center point.
        cntr[0] *= 1.f/4.f;
        cntr[1] *= 1.f/4.f;
        cntr[2] *= 1.f/4.f;
        // Average the X vector.
        vec_x[0] = (pts[3 * 1 + 0] - pts[3 * 3 + 0]) / 2.f;
        vec_x[1] = (pts[3 * 1 + 1] - pts[3 * 3 + 1]) / 2.f;
        len = sqrt(vec_x[0]*vec_x[0] + vec_x[1]*vec_x[1]);
        if (0) {
        // if (len < MEAS_NUM_X_DIST) {
            // Scale the vector up to MEAS_NUM_X_DIST lenght.
            float factor = MEAS_NUM_X_DIST / len;
            vec_x[0] *= factor;
            vec_x[0] *= factor;
        } else {
            // The vector is longer than MEAS_NUM_X_DIST. The X/Y axes are skewed.
            // Verify the maximum skew?
        }
        // Average the Y vector.
        vec_y[0] = (pts[3 * 2 + 0] - pts[3 * 0 + 0]) / 2.f;
        vec_y[1] = (pts[3 * 2 + 1] - pts[3 * 0 + 1]) / 2.f;
        len = sqrt(vec_y[0]*vec_y[0] + vec_y[1]*vec_y[1]);
        if (0) {
        // if (len < MEAS_NUM_Y_DIST) {
            // Scale the vector up to MEAS_NUM_X_DIST lenght.
            float factor = MEAS_NUM_Y_DIST / len;
            vec_y[1] *= factor;
            vec_y[1] *= factor;
        } else {
            // The vector is longer than MEAS_NUM_X_DIST. The X/Y axes are skewed.
            // Verify the maximum skew?
        }

        // Fearlessly store the calibration values into the eeprom.
        eeprom_update_float((float*)(EEPROM_BED_CALIBRATION_CENTER+0), cntr [0]);
        eeprom_update_float((float*)(EEPROM_BED_CALIBRATION_CENTER+4), cntr [1]);
        eeprom_update_float((float*)(EEPROM_BED_CALIBRATION_VEC_X +0), vec_x[0]);
        eeprom_update_float((float*)(EEPROM_BED_CALIBRATION_VEC_X +4), vec_x[1]);
        eeprom_update_float((float*)(EEPROM_BED_CALIBRATION_VEC_Y +0), vec_y[0]);
        eeprom_update_float((float*)(EEPROM_BED_CALIBRATION_VEC_Y +4), vec_y[1]);

#if 0
        SERIAL_ECHOLN("Calibration done.");
        SERIAL_ECHO("Center: ");
        SERIAL_ECHO(cntr[0]);
        SERIAL_ECHO(",");
        SERIAL_ECHO(cntr[1]);
        SERIAL_ECHO(", x: ");
        SERIAL_ECHO(vec_x[0]);
        SERIAL_ECHO(",");
        SERIAL_ECHO(vec_x[1]);
        SERIAL_ECHO(", y: ");
        SERIAL_ECHO(vec_y[0]);
        SERIAL_ECHO(",");
        SERIAL_ECHO(vec_y[1]);
        SERIAL_ECHOLN("");
#endif
    }
    return true;
}

bool improve_bed_offset_and_skew(int8_t method)
{
    // Reusing the z_values memory for the measurement cache.
    // 7x7=49 floats, good for 16 (x,y,z) vectors.
    float *pts = &mbl.z_values[0][0];
    float *vec_x = pts + 2 * 9;
    float *vec_y = vec_x + 2;
    float *cntr  = vec_y + 2;
    memset(pts, 0, sizeof(float) * 7 * 7);

    bool endstops_enabled  = enable_endstops(false);
    bool endstop_z_enabled = enable_z_endstop(false);

#ifdef MESH_BED_CALIBRATION_SHOW_LCD
    lcd_implementation_clear();
    lcd_print_at_PGM(0, 0, MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE1);
#endif /* MESH_BED_CALIBRATION_SHOW_LCD */

    // Collect a matrix of 9x9 points.
    for (int8_t mesh_point = 0; mesh_point < 9; ++ mesh_point) {
        int ix = mesh_point % MESH_MEAS_NUM_X_POINTS;
        int iy = mesh_point / MESH_MEAS_NUM_X_POINTS;
        if (iy & 1) ix = (MESH_MEAS_NUM_X_POINTS - 1) - ix; // Zig zag
        // Print the decrasing ID of the measurement point.
#ifdef MESH_BED_CALIBRATION_SHOW_LCD
        lcd_print_at_PGM(0, 1, MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE2);
        lcd_implementation_print_at(0, 2, mesh_point+1);
        lcd_printPGM(MSG_IMPROVE_BED_OFFSET_AND_SKEW_LINE3);
#endif /* MESH_BED_CALIBRATION_SHOW_LCD */

        // Move up.
        current_position[Z_AXIS] = MESH_HOME_Z_SEARCH;
        enable_endstops(false);
        enable_z_endstop(false);
        go_to_current(homing_feedrate[Z_AXIS]/60);
        // Go to the measurement point.
        // Use the coorrected coordinate, which is a result of find_bed_offset_and_skew().
        mbl.get_meas_xy(ix, iy, current_position[X_AXIS], current_position[Y_AXIS], false);
        go_to_current(homing_feedrate[X_AXIS]/60);
        // Find its Z position by running the normal vertical search.
//        delay_keep_alive(3000);
        find_bed_induction_sensor_point_z();
//        delay_keep_alive(3000);
        // Improve the point position by searching its center in a current plane.
        int8_t n_errors = 3;
        for (int8_t iter = 0; iter < 4; ++ iter) {
            bool found = false;
            switch (method) {
                case 0: found = improve_bed_induction_sensor_point(); break;
                case 1: found = improve_bed_induction_sensor_point2(iy == 0); break;
                default: break;
            }
            if (! found) {
                if (n_errors -- == 0) {
                    // Give up.
                    goto canceled;
                } else {
                    // Try to move the Z axis down a bit to increase a chance of the sensor to trigger.
                    current_position[Z_AXIS] -= 0.025f;
                    enable_endstops(false);
                    enable_z_endstop(false);
                    go_to_current(homing_feedrate[Z_AXIS]);
                }
            }
        }
//        delay_keep_alive(3000);
        float *pt = pts + 2 * (ix + iy * 3);
        pt[0] = current_position[X_AXIS];
        pt[1] = current_position[Y_AXIS];
        cntr[0] += pt[0];
        cntr[1] += pt[1];
    }

    // Average the X and Y vectors. They may not be perpendicular, if the printer is built incorrectly.
    // Average the center point.
    cntr[0] *= 1.f/9.f;
    cntr[1] *= 1.f/9.f;
    // Average the X vector.
    vec_x[0] = (pts[2 * 2 + 0] - pts[2 * 0 + 0] + pts[2 * 5 + 0] - pts[2 * 3 + 0] + pts[2 * 8 + 0] - pts[2 * 6 + 0]) / 6.f;
    vec_x[1] = (pts[2 * 2 + 1] - pts[2 * 0 + 1] + pts[2 * 5 + 1] - pts[2 * 3 + 1] + pts[2 * 8 + 1] - pts[2 * 6 + 1]) / 6.f;
    // Average the Y vector.
    vec_y[0] = (pts[2 * 6 + 0] - pts[2 * 0 + 0] + pts[2 * 7 + 0] - pts[2 * 1 + 0] + pts[2 * 8 + 0] - pts[2 * 2 + 0]) / 6.f;
    vec_y[1] = (pts[2 * 6 + 1] - pts[2 * 0 + 1] + pts[2 * 7 + 1] - pts[2 * 1 + 1] + pts[2 * 8 + 1] - pts[2 * 2 + 1]) / 6.f;

#if 1
    // Fearlessly store the calibration values into the eeprom.
    eeprom_update_float((float*)(EEPROM_BED_CALIBRATION_CENTER+0), cntr [0]);
    eeprom_update_float((float*)(EEPROM_BED_CALIBRATION_CENTER+4), cntr [1]);
    eeprom_update_float((float*)(EEPROM_BED_CALIBRATION_VEC_X +0), vec_x[0]);
    eeprom_update_float((float*)(EEPROM_BED_CALIBRATION_VEC_X +4), vec_x[1]);
    eeprom_update_float((float*)(EEPROM_BED_CALIBRATION_VEC_Y +0), vec_y[0]);
    eeprom_update_float((float*)(EEPROM_BED_CALIBRATION_VEC_Y +4), vec_y[1]);
#endif

#if 0
    // and let us know the result.
    SERIAL_ECHOLN("Calibration done.");
    SERIAL_ECHO("Center: ");
    SERIAL_ECHO(cntr[0]);
    SERIAL_ECHO(",");
    SERIAL_ECHO(cntr[1]);
    SERIAL_ECHO(", x: ");
    SERIAL_ECHO(vec_x[0]);
    SERIAL_ECHO(",");
    SERIAL_ECHO(vec_x[1]);
    SERIAL_ECHO(", y: ");
    SERIAL_ECHO(vec_y[0]);
    SERIAL_ECHO(",");
    SERIAL_ECHO(vec_y[1]);
    SERIAL_ECHOLN("");
#endif

    enable_endstops(endstops_enabled);
    enable_z_endstop(endstop_z_enabled);
    return true;

canceled:
    enable_endstops(endstops_enabled);
    enable_z_endstop(endstop_z_enabled);
    return false;
}

void reset_bed_offset_and_skew()
{
    eeprom_update_dword((uint32_t*)(EEPROM_BED_CALIBRATION_CENTER+0), 0x0FFFFFFFF);
    eeprom_update_dword((uint32_t*)(EEPROM_BED_CALIBRATION_CENTER+4), 0x0FFFFFFFF);
    eeprom_update_dword((uint32_t*)(EEPROM_BED_CALIBRATION_VEC_X +0), 0x0FFFFFFFF);
    eeprom_update_dword((uint32_t*)(EEPROM_BED_CALIBRATION_VEC_X +4), 0x0FFFFFFFF);
    eeprom_update_dword((uint32_t*)(EEPROM_BED_CALIBRATION_VEC_Y +0), 0x0FFFFFFFF);
    eeprom_update_dword((uint32_t*)(EEPROM_BED_CALIBRATION_VEC_Y +4), 0x0FFFFFFFF);
}

#if 0
static const float[9][2] PROGMEM bed_points = {
};

bool calculate_machine_skew_and_offset_LS(
    // Matrix of 9 2D points (18 floats)
    float *pts,
    // Resulting correction matrix.
    float *vec_x,
    float *vec_y,
    float *cntr,
    // Temporary values, 49-18-(2*3)=25 floats
    float *temp
{
    {
        // Create covariance matrix for A, collect the right hand side b.
        float A[3][3] = { 0.f };
        float b[3] = { 0.f };
        float acc;
        for (uint8_t r = 0; r < 3; ++ r) {
            for (uint8_t c = 0; c < 3; ++ c) {
                acc = 0;
                for (uint8_t i = 0; i < 9; ++ i) {
                    float a = (r == 2) ? 1.f : pts[2 * i + r];
                    float b = (c == 2) ? 1.f : pts[2 * i + c];
                    acc += a * b;
                }
                A[r][c] = acc;
            }
            acc = 0.f;
            for (uint8_t i = 0; i < 9; ++ i) {
                float a = (r == 2) ? 1.f : pts[2 * i + r];
                float b = pgm_read_float(&coeff2[i][0]);
                acc += a * b;
            }
            b[r] = acc;
        }
        // Solve the linear equation for ax, bx, cx.
        float x[3] = { 0.f };
        for (uint8_t iter = 0; iter < 100; ++ iter) {
            x[0] = (b[0] - A[1] * x[1] - A[2] * x[2]) / A[0];
            x[1] = (b[1] - A[0] * x[0] - A[2] * x[2]) / A[1];
            x[2] = (b[2] - A[0] * x[0] - A[1] * x[1]) / A[2];
        }
        // Store the result to the output variables.
        vec_x[0] = x[0];
        vec_y[0] = x[1];
        cntr[0] = x[2];

        // Recalculate b for the y values.
        for (uint8_t r = 0; r < 3; ++ r) {
            acc = 0.f;
            for (uint8_t i = 0; i < 9; ++ i) {
                float a = (r == 2) ? 1.f : pts[2 * i + r];
                float b = pgm_read_float(&coeff2[i][1]);
                acc += a * b;
            }
            b[r] = acc;
        }
        // Solve the linear equation for ay, by, cy.
        x[0] = 0.f, x[1] = 0.f; x[2] = 0.f;
        for (uint8_t iter = 0; iter < 100; ++ iter) {
            x[0] = (b[0] - A[1] * x[1] - A[2] * x[2]) / A[0];
            x[1] = (b[1] - A[0] * x[0] - A[2] * x[2]) / A[1];
            x[2] = (b[2] - A[0] * x[0] - A[1] * x[1]) / A[2];
        }
        // Store the result to the output variables.
        vec_x[1] = x[0];
        vec_y[1] = x[1];
        cntr[1] = x[2];
    }

    // Normalize the vectors. We expect, that the machine axes may be skewed a bit, but the distances are correct.
    // l shall be very close to 1 already.
    float l = sqrt(vec_x[0]*vec_x[0] + vec_x[1] * vec_x[1]);
    vec_x[0] /= l;
    vec_x[1] /= l;
    l = sqrt(vec_y[0]*vec_y[0] + vec_y[1] * vec_y[1]);
    vec_y[0] /= l;
    vec_y[1] /= l;



    // Invert the transformation matrix made of vec_x, vec_y and cntr.

}
#endif