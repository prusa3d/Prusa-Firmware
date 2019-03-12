#include "mesh_bed_leveling.h"
#include "mesh_bed_calibration.h"
#include "Configuration.h"

#ifdef MESH_BED_LEVELING

mesh_bed_leveling mbl;

mesh_bed_leveling::mesh_bed_leveling() { reset(); }

void mesh_bed_leveling::reset() {
    active = 0;
    for (int y = 0; y < MESH_NUM_Y_POINTS; y++)
        for (int x = 0; x < MESH_NUM_X_POINTS; x++)
            z_values[y][x] = 0;
}

static inline bool vec_undef(const float v[2])
{
    const uint32_t *vx = (const uint32_t*)v;
    return vx[0] == 0x0FFFFFFFF || vx[1] == 0x0FFFFFFFF;
}

#if MESH_NUM_X_POINTS>=5 && MESH_NUM_Y_POINTS>=5 && (MESH_NUM_X_POINTS&1)==1 && (MESH_NUM_Y_POINTS&1)==1
// Works for an odd number of MESH_NUM_X_POINTS and MESH_NUM_Y_POINTS

// #define MBL_BILINEAR
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
        for (int j = 0; j < 3; ++ j) {
            // 1) Copy the source points to their new destination.
            z_values[j][idx2] = z_values[j][2];
            z_values[j][idx1] = z_values[j][1];
            // 2) Interpolate the remaining values by Largrangian polynomials.
            for (int i = idx0 + 1; i < idx2; ++ i) {
                if (i == idx1)
                    continue;
                float x = get_x(i);
                #ifdef MBL_BILINEAR
                z_values[j][i] = (x < x1) ?
                    ((z_values[j][idx0] * (x - x0) + z_values[j][idx1] * (x1 - x)) / (x1 - x0)) :
                    ((z_values[j][idx1] * (x - x1) + z_values[j][idx2] * (x2 - x)) / (x2 - x1));
                #else
                z_values[j][i] = 
                    z_values[j][idx0] * (x - x1) * (x - x2) / ((x0 - x1) * (x0 - x2)) +
                    z_values[j][idx1] * (x - x0) * (x - x2) / ((x1 - x0) * (x1 - x2)) +
                    z_values[j][idx2] * (x - x0) * (x - x1) / ((x2 - x0) * (x2 - x1));
                #endif
            }
        }
    }
    {
        // Second interpolate the points in Y axis.
        static const float y0 = MESH_MIN_Y;
        static const float y1 = 0.5f * float(MESH_MIN_Y + MESH_MAX_Y);
        static const float y2 = MESH_MAX_Y;
        for (int i = 0; i < MESH_NUM_X_POINTS; ++ i) {
            // 1) Copy the intermediate points to their new destination.
            z_values[idx2][i] = z_values[2][i];
            z_values[idx1][i] = z_values[1][i];
            // 2) Interpolate the remaining values by Largrangian polynomials.
            for (int j = 1; j + 1 < MESH_NUM_Y_POINTS; ++ j) {
                if (j == idx1)
                    continue;
                float y = get_y(j);
                #ifdef MBL_BILINEAR
                z_values[j][i] = (y < y1) ? 
                    ((z_values[idx0][i] * (y - y0) + z_values[idx1][i] * (y1 - y)) / (y1 - y0)) :
                    ((z_values[idx1][i] * (y - y1) + z_values[idx2][i] * (y2 - y)) / (y2 - y1));
                #else
                z_values[j][i] = 
                    z_values[idx0][i] * (y - y1) * (y - y2) / ((y0 - y1) * (y0 - y2)) +
                    z_values[idx1][i] * (y - y0) * (y - y2) / ((y1 - y0) * (y1 - y2)) +
                    z_values[idx2][i] * (y - y0) * (y - y1) / ((y2 - y0) * (y2 - y1));
                #endif
            }
        }
    }

/*
    // Relax the non-measured points.
    const float weight = 0.2f;
    for (uint8_t iter = 0; iter < 20; ++ iter) {
        for (int8_t j = 1; j < 6; ++ j) {
            for (int8_t i = 1; i < 6; ++ i) {
                if (i == 3 || j == 3)
                    continue;
                if ((i % 3) == 0 && (j % 3) == 0)
                    continue;
                float avg = 0.25f * (z_values[j][i-1]+z_values[j][i+1]+z_values[j-1][i]+z_values[j+1][i]);
                z_values[j][i] = (1.f-weight)*z_values[j][i] + weight*avg;
            }
        }
    }
*/
}
#endif

#endif  // MESH_BED_LEVELING
