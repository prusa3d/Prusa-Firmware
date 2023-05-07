#include "Marlin.h"
#include "mesh_bed_calibration.h"

#ifdef MESH_BED_LEVELING

class mesh_bed_leveling {
public:
    uint8_t active;
    float z_values[MESH_NUM_Y_POINTS][MESH_NUM_X_POINTS];
    
    mesh_bed_leveling() { reset(); }
    
    void reset();

    static float get_x(int i) { return BED_X(i) + X_PROBE_OFFSET_FROM_EXTRUDER; }
    static float get_y(int i) { return BED_Y(i) + Y_PROBE_OFFSET_FROM_EXTRUDER; }
    float get_z(float x, float y);
    void set_z(uint8_t ix, uint8_t iy, float z) { z_values[iy][ix] = z; }
    void upsample_3x3();
    void print();
};

extern mesh_bed_leveling mbl;

#endif  // MESH_BED_LEVELING
