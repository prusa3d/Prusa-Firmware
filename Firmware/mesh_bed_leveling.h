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
    
    mesh_bed_leveling() { reset(); }
    
    void reset();

    static float get_x(int i) { return float(MESH_MIN_X) + float(MESH_X_DIST) * float(i); }
    static float get_y(int i) { return float(MESH_MIN_Y) + float(MESH_Y_DIST) * float(i); }
    float get_z(float x, float y);
    void set_z(uint8_t ix, uint8_t iy, float z) { z_values[iy][ix] = z; }
    
    int select_x_index(float x);
    int select_y_index(float y);
    
#if MESH_NUM_X_POINTS>=5 && MESH_NUM_Y_POINTS>=5 && (MESH_NUM_X_POINTS&1)==1 && (MESH_NUM_Y_POINTS&1)==1
    void upsample_3x3();
#endif
};

extern mesh_bed_leveling mbl;

#endif  // MESH_BED_LEVELING
