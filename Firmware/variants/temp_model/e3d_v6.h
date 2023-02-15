#pragma once

#define TEMP_MODEL_E3D_V6_VER 1      // model parameters version

#define TEMP_MODEL_E3D_V6_P 38.      // heater power (W)
#define TEMP_MODEL_E3D_V6_U 0.       // linear temperature coefficient (W/K/power)
#define TEMP_MODEL_E3D_V6_V 1.       // linear temperature intercept (W/power)

#define TEMP_MODEL_E3D_V6_C 12.1     // initial guess for heatblock capacitance (J/K)
#define TEMP_MODEL_E3D_V6_R 20.5     // initial guess for heatblock resistance (K/W)

#define TEMP_MODEL_E3D_V6_fS 0.065   // sim. 1st order IIR filter factor (f=100/27)
#define TEMP_MODEL_E3D_V6_LAG 2100   // sim. response lag (ms, 0-2160)

#define TEMP_MODEL_E3D_V6_W 1.2      // Default warning threshold (K/s)
#define TEMP_MODEL_E3D_V6_E 1.74     // Default error threshold (K/s)

// fall-back resistance vector (R0-15)
#define TEMP_MODEL_E3D_V6_Rv {TEMP_MODEL_E3D_V6_R, 18.4, 16.7, 15.2, 14.1, 13.3, 12.7, 12.1, 11.7, 11.3, 11., 10.8, 10.6, 10.4, 10.2, 10.1}
