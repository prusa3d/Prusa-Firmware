With FW 3.12.0 we introduced the Thermal model see [here](https://blog.prusa3d.com/mk3s-3-12-beta-firmware-new-thermal-model-protection-and-blob-detection_71230).
This have been finetuned for the stock Prusa MK3/S and may cause issues with 3rd party hotends.

This README is a guide for the community and the 3rd party companies to prepare/compile/build their own firmware.

Thanks to E3D and the some REVO community members see [here](https://github.com/prusa3d/Prusa-Firmware/issues/4105) and [here](https://github.com/prusa3d/Prusa-Firmware/issues/3636) we have been able to provide the source code to build the E3D REVO firmware.

There are other 3rd party hotends which the community uses.

Steps to add a new 3rd party hotend:

1. Make a copy of the `/Firmware/variant/1_75mm_MK3S-EINSy10a-E3Dv6full.h` with a new name `1_75mm_MK3S-EINSy10a-<3rd party hotend:16>.h`
  a. Example for the E3D REVO HF 60W: `/Firmware/variant/1_75mm_MK3S-EINSy10a-E3DREVO_HF_60W.h`
2. Open the new variant file.
3. Add the 3rd party hotend with new number and the description in the comment part `#define HOTEND 1 //1= E3Dv6 2= E3D REVO`
  a. Example: `#define HOTEND 3 //1= E3Dv6 2= E3D REVO 3= E3DREVO_HF_60W`
4. Add a new `#elif HOTEND == <new number>` and `#define NOZZLE_TYPE "<3rd party hotend:16>"`
  a.Example
  ```
  ...
  #elif HOTEND ==2
  #define NOZZLE_TYPE "E3DREVO"
  #elif HOTEND ==3
  #define NOZZLE_TYPE "E3DREVO_HF_60W"
  #endif
  ...
  ```
5. Change the `#define CUSTOM_MENDEL_NAME "Prusa i3 MK3S-<Short description:2>`
  a. `CUSTOM_MENDEL_NAME` string cannot exceed 17 chars in total!
6. Add new PID values for the new Hotend type.
  a. Search for `#else // E3D v6  PID values`
  b. Copy, paste (above `#else // E3D v6  PID values`) and update the new hotend PID default values
  ```
  #elif HOTEND == 2 //E3D REVO PID values
  #define  DEFAULT_Kp 16.13
  #define  DEFAULT_Ki 1.1625
  #define  DEFAULT_Kd 56.23
  ```
Example:
  ```
  ...
  #elif HOTEND == 2 //E3D REVO PID values
  #define  DEFAULT_Kp 16.13
  #define  DEFAULT_Ki 1.1625
  #define  DEFAULT_Kd 56.23
  #elif HOTEND == 3 //E3D REVO HF 60W PID values
  #define  DEFAULT_Kp 23.23
  #define  DEFAULT_Ki 1.1
  #define  DEFAULT_Kd 55.25
  #else // E3D v6  PID values
  ...
  ```
7. Prepare for new 3rd party hotend TM value file
  a. Search for `#define TEMP_MODEL_DEFAULT E3D_REVO // Default E3D REVO model parameters` or the last entry
  b. Copy the lines starting with `#elif HOTEND == 2` till the search, paste (above next `#endif`) and update this
  ```
  ...
  #if HOTEND == 1
  #include "temp_model/e3d_v6.h"
  #define TEMP_MODEL_DEFAULT E3D_V6 // Default E3D v6 model parameters
  #elif HOTEND == 2
  #include "temp_model/e3d_REVO.h"
  #define TEMP_MODEL_DEFAULT E3D_REVO // Default E3D REVO model parameters
  #endif
  ...
  ```
  Example:

  ```
  ...
  #if HOTEND == 1
  #include "temp_model/e3d_v6.h"
  #define TEMP_MODEL_DEFAULT E3D_V6 // Default E3D v6 model parameters
  #elif HOTEND == 2
  #include "temp_model/e3d_REVO.h"
  #define TEMP_MODEL_DEFAULT E3D_REVO // Default E3D REVO model parameters
  #elif HOTEND == 3
  #include "temp_model/e3d_REVO_HF_60W.h"
  #define TEMP_MODEL_DEFAULT E3D_REVO_HF_60W // Default E3D REVO HF 60W model parameters
  #endif
  ...
  ```
8. Save and close the new variant file
9. Copy/paste `Firmware/temp_model/e3d_v6.h` as `Firmware/temp_model/<3rd party hotend:16>.h`
  a. Example: `Firmware/temp_model/e3d_REVO_HF_60W.h`
10. Open the new file `Firmware/temp_model/<3rd party hotend:16>.h`
11. Search `E3D_V6` and replace it with what you have used in `#defined TEMP_MODEL_DEFAULT`
  a. Example
  ```
  #pragma once

  #define TEMP_MODEL_E3D_REVO_HF_60W_VER 1      // model parameters version

  #define TEMP_MODEL_E3D_REVO_HF_60W_P 40.      // heater power (W)
  #define TEMP_MODEL_E3D_REVO_HF_60W_U -0.0014  // linear temperature coefficient (W/K/power)
  #define TEMP_MODEL_E3D_REVO_HF_60W_V 1.05     // linear temperature intercept (W/power)

  #define TEMP_MODEL_E3D_REVO_HF_60W_C 8.77     // initial guess for heatblock capacitance (J/K)
  #define TEMP_MODEL_E3D_REVO_HF_60W_R 25.3     // initial guess for heatblock resistance (K/W)

  #define TEMP_MODEL_E3D_REVO_HF_60W_fS 0.15    // sim. 1st order IIR filter factor (f=100/27)
  #define TEMP_MODEL_E3D_REVO_HF_60W_LAG 270    // sim. response lag (ms, 0-2160)

  #define TEMP_MODEL_E3D_REVO_HF_60W_W 1.2      // Default warning threshold (K/s)
  #define TEMP_MODEL_E3D_REVO_HF_60W_E 1.74     // Default error threshold (K/s)

  // fall-back resistance vector (R0-15)
  #define TEMP_MODEL_E3D_REVO_HF_60W_Rv {TEMP_MODEL_E3D_REVO_HF_60W_R, 23.9, 22.5, 19.6, 19.0, 18.3, 17.7, 17.1, 16.8, 16.5, 16.3, 16.0, 15.9, 15.7, 15.6, 15.4}

```
