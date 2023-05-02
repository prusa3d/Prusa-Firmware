#include "mesh_bed_leveling.h"
#include "mesh_bed_calibration.h"
#include "Configuration.h"

#ifdef MESH_BED_LEVELING

mesh_bed_leveling mbl;

void mesh_bed_leveling::reset() {
    active = 0;
    for (uint8_t row = 0; row < MESH_NUM_Y_POINTS; ++row) {
        for (uint8_t col = 0; col < MESH_NUM_X_POINTS; ++col) {
            mbl.z_values[row][col] = NAN;
        }
    }
}

float mesh_bed_leveling::get_z(float x, float y) {
    int   i, j;
    float s, t;
    
#if MESH_NUM_X_POINTS==3 && MESH_NUM_Y_POINTS==3
#define MESH_MID_X (0.5f*(MESH_MIN_X+MESH_MAX_X))
#define MESH_MID_Y (0.5f*(MESH_MIN_Y+MESH_MAX_Y))
    if (x < MESH_MID_X) {
        i = 0;
        s = (x - MESH_MIN_X) / MESH_X_DIST;
        if (s > 1.f)
            s = 1.f;
    } else {
        i = 1;
        s = (x - MESH_MID_X) / MESH_X_DIST;
        if (s < 0)
            s = 0;
    }
    if (y < MESH_MID_Y) {
        j = 0;
        t = (y - MESH_MIN_Y) / MESH_Y_DIST;
        if (t > 1.f)
            t = 1.f;
    } else {
        j = 1;
        t = (y - MESH_MID_Y) / MESH_Y_DIST;
        if (t < 0)
            t = 0;
    }
#else
    i = int(floor((x - MESH_MIN_X) / MESH_X_DIST));
    if (i < 0) {
        i = 0;
        s = (x - MESH_MIN_X) / MESH_X_DIST;
        if (s > 1.f)
            s = 1.f;
    }
    else if (i > MESH_NUM_X_POINTS - 2) {
        i = MESH_NUM_X_POINTS - 2;
        s = (x - get_x(i)) / MESH_X_DIST;
        if (s < 0)
            s = 0;
    } else {
        s = (x - get_x(i)) / MESH_X_DIST;
        if (s < 0)
            s = 0;
        else if (s > 1.f)
            s = 1.f;
    }
    j = int(floor((y - MESH_MIN_Y) / MESH_Y_DIST));
    if (j < 0) {
        j = 0;
        t = (y - MESH_MIN_Y) / MESH_Y_DIST;
        if (t > 1.f)
            t = 1.f;
    } else if (j > MESH_NUM_Y_POINTS - 2) {
        j = MESH_NUM_Y_POINTS - 2;
        t = (y - get_y(j)) / MESH_Y_DIST;
        if (t < 0)
            t = 0;
    } else {
        t = (y - get_y(j)) / MESH_Y_DIST;
        if (t < 0)
            t = 0;
        else if (t > 1.f)
            t = 1.f;
    }
#endif /* MESH_NUM_X_POINTS==3 && MESH_NUM_Y_POINTS==3 */
    
    float si = 1.f-s;
    float z0 = si * z_values[j  ][i] + s * z_values[j  ][i+1];
    float z1 = si * z_values[j+1][i] + s * z_values[j+1][i+1];
    return (1.f-t) * z0 + t * z1;
}

int mesh_bed_leveling::select_x_index(float x) {
    int i = 1;
    while (x > get_x(i) && i < MESH_NUM_X_POINTS - 1) i++;
    return i - 1;
}

int mesh_bed_leveling::select_y_index(float y) {
    int i = 1;
    while (y > get_y(i) && i < MESH_NUM_Y_POINTS - 1) i++;
    return i - 1;
}

#if MESH_NUM_X_POINTS>=5 && MESH_NUM_Y_POINTS>=5 && (MESH_NUM_X_POINTS&1)==1 && (MESH_NUM_Y_POINTS&1)==1
// Works for an odd number of MESH_NUM_X_POINTS and MESH_NUM_Y_POINTS

void mesh_bed_leveling::upsample_3x3()
{
    int idx0 = 0;
    int idx1 = MESH_NUM_X_POINTS / 2;
    int idx2 = MESH_NUM_X_POINTS - 1;
    {
        // First interpolate the points in X axis.
        static const float x0 = MESH_MIN_X;
        static const float x1 = 0.5f * float(MESH_MIN_X + MESH_MAX_X);
        static const float x2 = MESH_MAX_X;
        for (int j = 0; j < MESH_NUM_Y_POINTS; ++ j) {
            // Interpolate the remaining values by Largrangian polynomials.
            for (int i = 0; i < MESH_NUM_X_POINTS; ++ i) {
                if (!isnan(z_values[j][i]))
                    continue;
                float x = get_x(i);
                z_values[j][i] = 
                    z_values[j][idx0] * (x - x1) * (x - x2) / ((x0 - x1) * (x0 - x2)) +
                    z_values[j][idx1] * (x - x0) * (x - x2) / ((x1 - x0) * (x1 - x2)) +
                    z_values[j][idx2] * (x - x0) * (x - x1) / ((x2 - x0) * (x2 - x1));
            }
        }
    }
    {
        // Second interpolate the points in Y axis.
        static const float y0 = MESH_MIN_Y;
        static const float y1 = 0.5f * float(MESH_MIN_Y + MESH_MAX_Y);
        static const float y2 = MESH_MAX_Y;
        for (int i = 0; i < MESH_NUM_X_POINTS; ++ i) {
            // Interpolate the remaining values by Largrangian polynomials.
            for (int j = 1; j + 1 < MESH_NUM_Y_POINTS; ++ j) {
                if (!isnan(z_values[j][i]))
                    continue;
                float y = get_y(j);
                z_values[j][i] = 
                    z_values[idx0][i] * (y - y1) * (y - y2) / ((y0 - y1) * (y0 - y2)) +
                    z_values[idx1][i] * (y - y0) * (y - y2) / ((y1 - y0) * (y1 - y2)) +
                    z_values[idx2][i] * (y - y0) * (y - y1) / ((y2 - y0) * (y2 - y1));
            }
        }
    }
}
#endif // (MESH_NUM_X_POINTS>=5 && MESH_NUM_Y_POINTS>=5 && (MESH_NUM_X_POINTS&1)==1 && (MESH_NUM_Y_POINTS&1)==1)

#endif  // MESH_BED_LEVELING
