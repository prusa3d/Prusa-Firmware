#pragma once

#define THERMAL_MODEL_E3D_REVO_HT_40W_VER 1      // model parameters version

#define THERMAL_MODEL_E3D_REVO_HT_40W_P 38.      // heater power (W)
#define THERMAL_MODEL_E3D_REVO_HT_40W_U 0.  // linear temperature coefficient (W/K/power)
#define THERMAL_MODEL_E3D_REVO_HT_40W_V 1.     // linear temperature intercept (W/power)

#define THERMAL_MODEL_E3D_REVO_HT_40W_C 12.10     // initial guess for heatblock capacitance (J/K)
#define THERMAL_MODEL_E3D_REVO_HT_40W_R 20.5     // initial guess for heatblock resistance (K/W)

#define THERMAL_MODEL_E3D_REVO_HT_40W_fS 0.065    // sim. 1st order IIR filter factor (f=100/27)
#define THERMAL_MODEL_E3D_REVO_HT_40W_LAG 2160    // sim. response lag (ms, 0-2160)

#define THERMAL_MODEL_E3D_REVO_HT_40W_W 1.20     // Default warning threshold (K/s)
#define THERMAL_MODEL_E3D_REVO_HT_40W_E 1.74     // Default error threshold (K/s)

// fall-back resistance vector (R0-15)
#define THERMAL_MODEL_E3D_REVO_HT_40W_Rv {THERMAL_MODEL_E3D_REVO_HT_40W_R, 19.3, 17.7, 14.5, 13.6, 12.8, 11.9, 11., 10.8, 10.5, 10.1, 9.8, 9.6, 9.4, 9.2, 9.}
