#ifndef CONFIG_STORE_H
#define CONFIG_STORE_H
#define EEPROM_SETTINGS

#include "Configuration.h"

typedef struct
{
    char version[4];
    float axis_steps_per_unit[4];
    float max_feedrate_normal[4];
    unsigned long max_acceleration_units_per_sq_second_normal[4];
    float acceleration;
    float retract_acceleration;
    float minimumfeedrate;
    float mintravelfeedrate;
    unsigned long minsegmenttime;
    float max_jerk[4];
    float add_homing[3];
    float zprobe_zoffset;
    float Kp;
    float Ki;
    float Kd;
    float bedKp;
    float bedKi;
    float bedKd;
    int lcd_contrast; //!< unused
    bool autoretract_enabled;
    float retract_length;
    float retract_feedrate;
    float retract_zlift;
    float retract_recover_length;
    float retract_recover_feedrate;
    bool volumetric_enabled;
    float filament_size[1];
    float max_feedrate_silent[4];
    unsigned long max_acceleration_units_per_sq_second_silent[4];
} __attribute__ ((packed)) M500_conf;

//extern M500_conf cs;

void Config_ResetDefault();

#ifndef DISABLE_M503
void Config_PrintSettings(uint8_t level = 0);
#else
FORCE_INLINE void Config_PrintSettings() {}
#endif

#ifdef EEPROM_SETTINGS
void Config_StoreSettings(uint16_t offset);
bool Config_RetrieveSettings(uint16_t offset);
#else
FORCE_INLINE void Config_StoreSettings() {}
FORCE_INLINE void Config_RetrieveSettings() { Config_ResetDefault(); Config_PrintSettings(); }
#endif

inline uint8_t calibration_status() { return eeprom_read_byte((uint8_t*)EEPROM_CALIBRATION_STATUS); }
inline void calibration_status_store(uint8_t status) { eeprom_update_byte((uint8_t*)EEPROM_CALIBRATION_STATUS, status); }
inline bool calibration_status_pinda() { return eeprom_read_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA); }

#endif//CONFIG_STORE_H
