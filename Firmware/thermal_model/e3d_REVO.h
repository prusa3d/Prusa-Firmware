#pragma once

#define THERMAL_MODEL_E3D_REVO_VER 1      // model parameters version

#define THERMAL_MODEL_E3D_REVO_P 40.      // heater power (W)
#define THERMAL_MODEL_E3D_REVO_U -0.0014  // linear temperature coefficient (W/K/power)
#define THERMAL_MODEL_E3D_REVO_V 1.05     // linear temperature intercept (W/power)

#define THERMAL_MODEL_E3D_REVO_C 8.77     // initial guess for heatblock capacitance (J/K)
#define THERMAL_MODEL_E3D_REVO_R 25.3     // initial guess for heatblock resistance (K/W)

#define THERMAL_MODEL_E3D_REVO_fS 0.15    // sim. 1st order IIR filter factor (f=100/27)
#define THERMAL_MODEL_E3D_REVO_LAG 270    // sim. response lag (ms, 0-2160)

#define THERMAL_MODEL_E3D_REVO_W 0.85     // Default warning threshold (K/s)
#define THERMAL_MODEL_E3D_REVO_E 1.23     // Default error threshold (K/s)

// fall-back resistance vector (R0-15)
#define THERMAL_MODEL_E3D_REVO_Rv {THERMAL_MODEL_E3D_REVO_R, 23.9, 22.5, 19.6, 19.0, 18.3, 17.7, 17.1, 16.8, 16.5, 16.3, 16.0, 15.9, 15.7, 15.6, 15.4}
