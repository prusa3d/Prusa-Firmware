With FW 3.12.0 we introduced the Thermal model see [here](https://blog.prusa3d.com/mk3s-3-12-beta-firmware-new-thermal-model-protection-and-blob-detection_71230).
This have been finetuned for the stock Prusa MK3/S and may cause issues with 3rd party hotends.

This README is a guide for the community and the 3rd party companies to prepare/compile/build their own firmware.

Thanks to E3D and the some REVO community members see [here](https://github.com/prusa3d/Prusa-Firmware/issues/4105) and [here](https://github.com/prusa3d/Prusa-Firmware/issues/3636) we have been able to provide the source code to build the E3D REVO firmware.

There are other 3rd party hotends which the community uses.

Steps to add a new 3rd party hotend:

1. Make a copy of the `/Firmware/variant/1_75mm_MK3S-EINSy10a-E3Dv6full.h` with a new name `1_75mm_MK3S-EINSy10a-<3rd party hotend:16>.h`
  - Example for the E3D REVO HF 60W: `/Firmware/variant/1_75mm_MK3S-EINSy10a-E3DREVO_HF_60W.h`
2. Open the new variant file.
3. Search for `#define NOZZLE_TYPE`
4. Change `"E3Dv6full"` to  `"<3rd party hotend:16>"`
  ```
  ...
  #define NOZZLE_TYPE "E3Dv6full"
  ...
  ```
  - Example
  ```
  ...
  #define NOZZLE_TYPE "E3DREVO_HF_60W"
  ...
  ```
5. Search `#define CUSTOM_MENDEL_NAME`
6. Change  `"Prusa i3 MK3S"` to `"Prusa i3 MK3S-<Short description:2>"`
  - `CUSTOM_MENDEL_NAME` string cannot exceed 16 chars in total!
  ```
  ...
  #define CUSTOM_MENDEL_NAME "Prusa i3 MK3S"
  ...
  ```
  - Example
  ```
  ...
#define CUSTOM_MENDEL_NAME "Prusa i3 MK3S-RH"
  ...
  ```
  ```
  ...
#define CUSTOM_MENDEL_NAME "Prusa MK3S-RHF60"
  ...
  ```
7. Add new PID values for the new Hotend type.
  - Search for `// Define PID constants for extruder`
  - Change `extruder` to new hotend name ` <3rd party hotend:16>` and update the new hotend PID default values
  ```
  // Define PID constants for extruder
  #define  DEFAULT_Kp 16.13
  #define  DEFAULT_Ki 1.1625
  #define  DEFAULT_Kd 56.23
  #endif
  ```
Example:
  ```
  ...
  // Define PID constants for E3D REVO HF 60W
  #define  DEFAULT_Kp 23.23
  #define  DEFAULT_Ki 1.1
  #define  DEFAULT_Kd 55.25
  #endif
  ...
  ```
8. Prepare for new 3rd party hotend TM value file
  - Search for `#include "temp_model/e3d_v6.h"`
  - Change the `e3d_v6.h` to `<3rd party hotend:16>.h`
  - Change below the `E3D_V6` to `<3rd party hotend:16>`
  ```
  ...
  #include "temp_model/e3d_v6.h"
  #define TEMP_MODEL_DEFAULT E3D_V6 // Default E3D v6 model parameters
  ...
  ```
  Example:

  ```
  ...
  #include "temp_model/e3d_REVO_HF_60W.h"
  #define TEMP_MODEL_DEFAULT E3D_REVO_HF_60W // Default E3D REVO HF 60W model parameters
  ...
  ```
9. Save and close the new variant file
10. Copy/paste `Firmware/temp_model/e3d_v6.h` as `Firmware/temp_model/<3rd party hotend:16>.h`
  - Example: `Firmware/temp_model/e3d_REVO_HF_60W.h`
11. Open the new file `Firmware/temp_model/<3rd party hotend:16>.h`
12. Search `E3D_V6` and replace it with what you have used in `#defined TEMP_MODEL_DEFAULT`
  - Example
  ```
  #pragma once

  #define TEMP_MODEL_E3D_REVO_HF_60W_VER 1      // model parameters version

  #define TEMP_MODEL_E3D_REVO_HF_60W_P 60.      // heater power (W)
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
