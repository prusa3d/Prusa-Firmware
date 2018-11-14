#include "Marlin.h"

#ifdef MESH_BED_LEVELING

#define MEAS_NUM_X_DIST (float(MESH_MAX_X - MESH_MIN_X)/float(MESH_MEAS_NUM_X_POINTS - 1))
#define MEAS_NUM_Y_DIST (float(MESH_MAX_Y - MESH_MIN_Y)/float(MESH_MEAS_NUM_Y_POINTS - 1))

#define MESH_X_DIST (float(MESH_MAX_X - MESH_MIN_X)/float(MESH_NUM_X_POINTS - 1))
#define MESH_Y_DIST (float(MESH_MAX_Y - MESH_MIN_Y)/float(MESH_NUM_Y_POINTS - 1))

class mesh_bed_leveling {
public:
    uint8_t active;
    float z_values[MESH_NUM_Y_POINTS][MESH_NUM_X_POINTS];
    
    mesh_bed_leveling();
    
    void reset();
    
#if MESH_NUM_X_POINTS>=5 && MESH_NUM_Y_POINTS>=5 && (MESH_NUM_X_POINTS&1)==1 && (MESH_NUM_Y_POINTS&1)==1
    void upsample_3x3();
#endif
    
    static float get_x(int i) { return float(MESH_MIN_X) + float(MESH_X_DIST) * float(i); }
    static float get_y(int i) { return float(MESH_MIN_Y) + float(MESH_Y_DIST) * float(i); }
    
    // Measurement point for the Z probe.
    // If use_default=true, then the default positions for a correctly built printer are used.
    // Otherwise a correction matrix is pulled from the EEPROM if available.
    static void get_meas_xy(int ix, int iy, float &x, float &y, bool use_default);
    
    void set_z(int ix, int iy, float z) { z_values[iy][ix] = z; }
    
    int select_x_index(float x) {
        int i = 1;
        while (x > get_x(i) && i < MESH_NUM_X_POINTS - 1) i++;
        return i - 1;
    }
    
    int select_y_index(float y) {
        int i = 1;
        while (y > get_y(i) && i < MESH_NUM_Y_POINTS - 1) i++;
        return i - 1;
    }
    
    float get_z(float x, float y) {
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
    
};

extern mesh_bed_leveling mbl;

#endif  // MESH_BED_LEVELING
