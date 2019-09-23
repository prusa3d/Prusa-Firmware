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
#define EEPROM_NOZZLE_DIAMETER_uM_DEFAULT 400

enum class ClPrintChecking:uint_least8_t
{
    _Nozzle=1,
    _Model=2,
    _Smodel=3,
    _Version=4,
    _Gcode=5
};

enum class ClNozzleDiameter:uint_least8_t
{
    _Diameter_250=25,
    _Diameter_400=40,
    _Diameter_600=60,
    _Diameter_Undef=EEPROM_EMPTY_VALUE
};

enum class ClCheckMode:uint_least8_t
{
    _None,
    _Warn,
    _Strict,
    _Undef=EEPROM_EMPTY_VALUE
};

enum class ClCheckModel:uint_least8_t
{
    _None,
    _Warn,
    _Strict,
    _Undef=EEPROM_EMPTY_VALUE
};

enum class ClCheckVersion:uint_least8_t
{
    _None,
    _Warn,
    _Strict,
    _Undef=EEPROM_EMPTY_VALUE
};

enum class ClCheckGcode:uint_least8_t
{
    _None,
    _Warn,
    _Strict,
    _Undef=EEPROM_EMPTY_VALUE
};

#define COMPARE_VALUE_EQUAL (((uint8_t)ClCompareValue::_Equal<<6)+((uint8_t)ClCompareValue::_Equal<<4)+((uint8_t)ClCompareValue::_Equal<<2)+((uint8_t)ClCompareValue::_Equal))
enum class ClCompareValue:uint_least8_t
{
    _Less=0,
    _Equal=1,
    _Greater=2
};

extern ClNozzleDiameter oNozzleDiameter;
extern ClCheckMode oCheckMode;
extern ClCheckModel oCheckModel;
extern ClCheckVersion oCheckVersion;
extern ClCheckGcode oCheckGcode;

void fCheckModeInit();
void nozzle_diameter_check(uint16_t nDiameter);
void printer_model_check(uint16_t nPrinterModel);
void printer_smodel_check(char* pStrPos);
void fw_version_check(const char *pVersion);
void gcode_level_check(uint16_t nGcodeLevel);

void fSetMmuMode(bool bMMu);

#endif /* UTIL_H */
