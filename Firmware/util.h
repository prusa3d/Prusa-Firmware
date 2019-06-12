#ifndef UTIL_H
#define UTIL_H

extern const char* FW_VERSION_STR_P();

// Definition of a firmware flavor numerical values.
enum FirmwareRevisionFlavorType
{
    FIRMWARE_REVISION_DEV = 0,
    FIRMWARE_REVISION_ALPHA = 1,
    FIRMWARE_REVISION_BETA = 2,
    FIRMWARE_REVISION_RC,
    FIRMWARE_REVISION_RC2,
    FIRMWARE_REVISION_RC3,
    FIRMWARE_REVISION_RC4,
    FIRMWARE_REVISION_RC5,
    FIRMWARE_REVISION_RELEASED = 127
};

extern bool show_upgrade_dialog_if_version_newer(const char *version_string);
extern bool force_selftest_if_fw_version();

extern void update_current_firmware_version_to_eeprom();



inline int8_t eeprom_read_int8(unsigned char* addr) {
	uint8_t v = eeprom_read_byte(addr);
	return *reinterpret_cast<int8_t*>(&v);
}

inline void eeprom_update_int8(unsigned char* addr, int8_t v) {
	eeprom_update_byte(addr, *reinterpret_cast<uint8_t*>(&v));
}


//-//
#define e_CHECK_MODE_NULL 0xFF
#define e_NOZZLE_DIAMETER_NULL 0xFF

typedef enum
{
    e_NOZZLE_DIAMETER_250,
    e_NOZZLE_DIAMETER_400,
    e_NOZZLE_DIAMETER_600
} eNOZZLE_DIAMETER;

typedef enum
{
    e_CHECK_MODE_none,
    e_CHECK_MODE_warn,
    e_CHECK_MODE_strict
} eCHECK_MODE;

extern eNOZZLE_DIAMETER eNozzleDiameter;
extern eCHECK_MODE eCheckMode;

void fCheckModeInit();
void nozzle_diameter_check(uint16_t nDiameter);

#endif /* UTIL_H */
