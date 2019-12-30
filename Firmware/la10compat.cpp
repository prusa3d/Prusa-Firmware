#include "la10compat.h"
#include "Marlin.h"


static LA10C_MODE la10c_mode = LA10C_UNKNOWN;


void la10c_mode_change(LA10C_MODE mode)
{
    if(mode == la10c_mode) return;

    SERIAL_ECHOPGM("LA10C: Linear Advance mode: ");
    switch(mode)
    {
    case LA10C_UNKNOWN: SERIAL_ECHOLNPGM("UNKNOWN"); break;
    case LA10C_LA15:    SERIAL_ECHOLNPGM("1.5"); break;
    case LA10C_LA10:    SERIAL_ECHOLNPGM("1.0"); break;
    }
    la10c_mode = mode;
}


// Approximate a LA10 value to a LA15 equivalent.
static float la10c_convert(float k)
{
    float new_K = k * 0.004 - 0.06;
    return (new_K < 0? 0: new_K);
}


float la10c_value(float k)
{
    if(la10c_mode == LA10C_UNKNOWN)
    {
        // do not autodetect until a valid value is seen
        if(k == 0)
            return 0;
        else if(k < 0)
            return -1;

        la10c_mode_change(k < 10? LA10C_LA15: LA10C_LA10);
    }

    if(la10c_mode == LA10C_LA15)
        return (k >= 0 && k < 10? k: -1);
    else
        return (k >= 0? la10c_convert(k): -1);
}
