#pragma once
#include <stdint.h>

extern const char* FW_VERSION_STR_P();

// Definition of a firmware flavor numerical values.
// To keep it short as possible
// DEVs/ALPHAs/BETAs limited to max 8 flavor versions
// RCs limited to 32 flavor versions
// Final Release always 64 as highest
enum FirmwareRevisionFlavorType : uint16_t {
    FIRMWARE_REVISION_RELEASED = 0x0040,
    FIRMWARE_REVISION_DEV = 0x0000,
    FIRMWARE_REVISION_ALPHA = 0x008,
    FIRMWARE_REVISION_BETA = 0x0010,
    FIRMWARE_REVISION_RC = 0x0020
};

bool show_upgrade_dialog_if_version_newer(const char *version_string);
bool eeprom_fw_version_older_than(const uint16_t (&req_ver)[4]);
void update_current_firmware_version_to_eeprom();


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
    _Diameter_800=80,
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
void printer_model_check(uint16_t nPrinterModel, uint16_t actualPrinterModel);
void printer_smodel_check(const char *pStrPos, const char *actualPrinterSModel);
void fw_version_check(const char *pVersion);
void gcode_level_check(uint16_t nGcodeLevel);

uint16_t nPrinterType(bool bMMu);
const char *sPrinterType(bool bMMu);

#define IP4_STR_SIZE 16
extern void ip4_to_str(char* dest, uint8_t* IP);
