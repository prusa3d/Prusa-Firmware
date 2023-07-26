#pragma once

#define THERMAL_MODEL_E3D_REVO_HF_60W_VER 1      // model parameters version

#define THERMAL_MODEL_E3D_REVO_HF_60W_P 60.      // heater power (W)
#define THERMAL_MODEL_E3D_REVO_HF_60W_U -0.0014  // linear temperature coefficient (W/K/power)
#define THERMAL_MODEL_E3D_REVO_HF_60W_V 1.05     // linear temperature intercept (W/power)

#define THERMAL_MODEL_E3D_REVO_HF_60W_C 9.10     // initial guess for heatblock capacitance (J/K)
#define THERMAL_MODEL_E3D_REVO_HF_60W_R 30.6     // initial guess for heatblock resistance (K/W)

#define THERMAL_MODEL_E3D_REVO_HF_60W_fS 0.15    // sim. 1st order IIR filter factor (f=100/27)
#define THERMAL_MODEL_E3D_REVO_HF_60W_LAG 270    // sim. response lag (ms, 0-2160)

#define THERMAL_MODEL_E3D_REVO_HF_60W_W 0.85     // Default warning threshold (K/s)
#define THERMAL_MODEL_E3D_REVO_HF_60W_E 1.23     // Default error threshold (K/s)

// fall-back resistance vector (R0-15)
#define THERMAL_MODEL_E3D_REVO_HF_60W_Rv {THERMAL_MODEL_E3D_REVO_HF_60W_R, 29.0, 27.5, 24.5, 23.4, 22.3, 21.2, 20.2, 19.8, 19.4, 19.0, 18.6, 18.3, 18.1, 17.9, 17.7}
