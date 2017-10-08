#ifndef CONFIG_STORE_H
#define CONFIG_STORE_H
#define EEPROM_SETTINGS

#include "Configuration.h"

void Config_ResetDefault();

#ifndef DISABLE_M503
void Config_PrintSettings();
#else
FORCE_INLINE void Config_PrintSettings() {}
#endif

#ifdef EEPROM_SETTINGS
void Config_StoreSettings();
bool Config_RetrieveSettings();
#else
FORCE_INLINE void Config_StoreSettings() {}
FORCE_INLINE void Config_RetrieveSettings() { Config_ResetDefault(); Config_PrintSettings(); }
#endif

inline uint8_t calibration_status() { return eeprom_read_byte((uint8_t*)EEPROM_CALIBRATION_STATUS); }
inline void calibration_status_store(uint8_t status) { eeprom_update_byte((uint8_t*)EEPROM_CALIBRATION_STATUS, status); }
inline bool calibration_status_pinda() { return eeprom_read_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA); }

#endif//CONFIG_STORE_H
