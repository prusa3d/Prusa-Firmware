#include "la10compat.h"
#include "Marlin.h"


static LA10C_MODE la10c_mode = LA10C_UNKNOWN; // Current LA compatibility mode
static float la10c_orig_jerk = 0;             // Unadjusted/saved e-jerk


LA10C_MODE la10c_mode_get()
{
    return la10c_mode;
}


void la10c_mode_change(LA10C_MODE mode)
{
    if(mode == la10c_mode) return;

    // always restore to the last unadjusted E-jerk value
    if(la10c_orig_jerk)
        cs.max_jerk[E_AXIS] = la10c_orig_jerk;

    SERIAL_ECHOPGM("LA10C: Linear Advance mode: ");
    switch(mode)
    {
    case LA10C_UNKNOWN: SERIAL_ECHOLNPGM("UNKNOWN"); break;
    case LA10C_LA15:    SERIAL_ECHOLNPGM("1.5"); break;
    case LA10C_LA10:    SERIAL_ECHOLNPGM("1.0"); break;
    }
    la10c_mode = mode;

    // adjust the E-jerk if needed
    cs.max_jerk[E_AXIS] = la10c_jerk(cs.max_jerk[E_AXIS]);
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


float la10c_jerk(float j)
{
    la10c_orig_jerk = j;

    if(la10c_mode != LA10C_LA10)
        return j;

    // check for a compatible range of values prior to convert (be sure that
    // a higher E-jerk would still be compatible wrt the E accell range)
    if(j < 4.5 && cs.max_acceleration_units_per_sq_second_normal[E_AXIS] < 2000)
        return j;

    // bring low E-jerk values into equivalent LA 1.5 values by
    // flattening the response in the (1-4.5) range using a piecewise
    // function. Is it truly worth to preserve the difference between
    // 1.5/2.5 E-jerk for LA1.0? Probably not, but we try nonetheless.
    j = j < 1.0? j * 3.625:
        j < 4.5? j * 0.25 + 3.375:
        j;

    SERIAL_ECHOPGM("LA10C: Adjusted E-Jerk: ");
    SERIAL_ECHOLN(j);
    return j;
}
