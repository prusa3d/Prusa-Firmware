// la10compat: LA10->LA15 conversion
//
// When the current mode is UNKNOWN autodetection is active and any K<10
// will set the mode to LA15, LA10 is set otherwise. When LA10
// compatbility mode is active the K factor is converted to a LA15
// equivalent (that is, the return value is always a LA15 value).
//
// E-jerk<2 is also bumped in LA10 mode to restore the the printing speed
// to values comparable to existing settings.
//
// Once the interpretation mode has been set it is kept until the mode
// is explicitly reset. This is done to handle transparent fallback for
// old firmware revisions in combination with the following gcode
// sequence:
//
//   M900 K0.01 ; set LA15 value (interpreted by any firmware)
//   M900 K10   ; set LA10 value (ignored by LA15 firmware)
//
// A LA15 firmware without this module will only parse the first
// correctly, rejecting the second. A LA10 FW will parse both, but keep
// the last value. Since the LA15 value, if present, corresponds to the
// truth value, the compatibility stub needs to "lock" onto the first
// seen value for the current print.
//
// The mode needs to be carefully reset for each print in order for
// diffent versions of M900 to be interpreted independently.

#pragma once

enum __attribute__((packed)) LA10C_MODE
{
    LA10C_UNKNOWN = 0,
    LA10C_LA15    = 1,
    LA10C_LA10    = 2
};

// Explicitly set/get/reset the interpretation mode for la10c_value()
void la10c_mode_change(LA10C_MODE mode);
LA10C_MODE la10c_mode_get();
static inline void la10c_reset() { la10c_mode_change(LA10C_UNKNOWN); }

// Return a LA15 K value according to the supplied value and mode
float la10c_value(float k);

// Return an updated LA15 E-jerk value according to the current mode
float la10c_jerk(float j);
