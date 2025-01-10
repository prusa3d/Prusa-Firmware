#pragma once
#include <stdint.h>
#include <string.h>

extern const uint16_t FW_VERSION_NR[4];
const char* FW_VERSION_STR_P();

extern const char FW_VERSION_HASH[];
const char* FW_VERSION_HASH_P();

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
bool eeprom_fw_version_older_than_p(const uint16_t (&req_ver)[4]);
void update_current_firmware_version_to_eeprom();

#define EEPROM_NOZZLE_DIAMETER_uM_DEFAULT 400

enum class ClPrintChecking:uint_least8_t
{
    _Nozzle=1,
    _Model=2,
    _Smodel=3,
    _Version=4,
    _Gcode=5,
    _Features=6,
    _SheetType=7
};

enum class ClNozzleDiameter:uint_least8_t
{
    _Diameter_250=25,
    _Diameter_400=40,
    _Diameter_600=60,
    _Diameter_800=80,
    _Diameter_Undef=EEPROM_EMPTY_VALUE
};

#ifdef STEEL_SHEET_TYPES
enum class ClCheckSheetType:uint_least8_t
{
    _Smooth     =0b00000001,
    _Textured   =0b00000010,
    _Satin      =0b00000100,
    _NylonPA    =0b00001000,
    _PP         =0b00010000,
    _Custom     =0b00100000,
    _Undef      =0b00000000,
};
#endif //STEEL_SHEET_TYPES

enum class ClCheckMode:uint_least8_t
{
    _None,
    _Warn,
    _Strict,
#ifdef STEEL_SHEET_TYPES
    _Always,
#endif //STEEL_SHEET_TYPES
    _Undef=EEPROM_EMPTY_VALUE
};

#define COMPARE_VALUE_EQUAL (((uint8_t)ClCompareValue::_Equal<<6)+((uint8_t)ClCompareValue::_Equal<<4)+((uint8_t)ClCompareValue::_Equal<<2)+((uint8_t)ClCompareValue::_Equal))
enum class ClCompareValue:uint_least8_t
{
    _Less=0,
    _Equal=1,
    _Greater=2
};

struct unquoted_string {
public:
    /// @brief Given a pointer to a quoted string, filter out the quotes
    /// @param pStr A constant pointer to a constant string to be searched/filtered. Modifying the pointer is strictly forbidden.
    /// NOTE: Forcing inline saves ~36 bytes of flash
    inline __attribute__((always_inline)) unquoted_string(const char * const pStr)
    : len(0)
    , found(false)
    {
        const char * pStrEnd = NULL;

        // Start of the string
        this->ptr = strchr(pStr, '"');
        if (!this->ptr) {
            // First quote not found
            return;
        }

        // Skip the leading quote
        this->ptr++;

        // End of the string
        pStrEnd = strchr(this->ptr, '"');
        if(!pStrEnd) {
            // Second quote not found
            return;
        }
        this->len = pStrEnd - this->ptr;
        this->found = true;
    }

    bool WasFound() { return found; }
    uint8_t GetLength() { return len; }
    const char * GetUnquotedString() { return ptr; }
private:
    const char * ptr = NULL;
    uint8_t len;
    bool found;
};

extern ClNozzleDiameter oNozzleDiameter;
extern ClCheckMode oCheckMode;
extern ClCheckMode oCheckModel;
extern ClCheckMode oCheckVersion;
extern ClCheckMode oCheckGcode;
extern ClCheckMode oCheckFilament;
#ifdef STEEL_SHEET_TYPES
extern ClCheckMode oCheckSheets;
extern ClCheckSheetType oCheckSheetType;
#endif //STEEL_SHEET_TYPES

void fCheckModeInit();
void nozzle_diameter_check(uint16_t nDiameter);
void printer_model_check(uint16_t nPrinterModel, uint16_t actualPrinterModel);
void printer_smodel_check(const char *pStrPos, const char *actualPrinterSModel);
void fw_version_check(const char *pVersion);
void gcode_level_check(uint8_t nGcodeLevel);
#ifdef STEEL_SHEET_TYPES
void sheet_type_check(uint8_t nSheetType, uint8_t wSheetType);
#endif //STEEL_SHEET_TYPES

/// Check if the filament is present before starting a print job.
/// Depending on the check level set in the menus the printer will:
///   - None: not issue any warning about missing filament
///   - Warning (default): The user is warned about missing filament
///     and is prompted to continue with Yes/No. If No is selected,
///     the print is aborted. If no user input is given (e.g. from
///     host printing) then the warning will expire in 30 seconds and
///     the printer assumes the Yes option was selected.
///   - Strict: If the filament is not detected when a print is started,
///     it is immediately canceled with a message saying the filament is
///     missing.
/// @returns false if the print is canceled, true otherwise
bool filament_presence_check();

uint16_t nPrinterType(bool bMMu);
const char *sPrinterType(bool bMMu);

#define IP4_STR_SIZE 16
extern void ip4_to_str(char* dest, uint8_t* IP);

// Calibration status of the machine
// (unsigned char*)EEPROM_CALIBRATION_STATUS_V2
typedef uint8_t CalibrationStatus;
const CalibrationStatus CALIBRATION_STATUS_SELFTEST      = 0b00000001; // Selftest
const CalibrationStatus CALIBRATION_STATUS_XYZ           = 0b00000010; // XYZ calibration
const CalibrationStatus CALIBRATION_STATUS_Z             = 0b00000100; // Z calibration
#ifdef THERMAL_MODEL
const CalibrationStatus CALIBRATION_STATUS_THERMAL_MODEL = 0b00001000; // Thermal model calibration
#endif
const CalibrationStatus CALIBRATION_STATUS_LIVE_ADJUST   = 0b00010000; // 1st layer calibration
const CalibrationStatus CALIBRATION_STATUS_UNKNOWN       = 0b10000000; // Freshly assembled or unknown status

// Calibration steps performed by the wizard
const CalibrationStatus CALIBRATION_WIZARD_STEPS =
    CALIBRATION_STATUS_SELFTEST |
    CALIBRATION_STATUS_XYZ |
    CALIBRATION_STATUS_Z |
#ifdef THERMAL_MODEL
    CALIBRATION_STATUS_THERMAL_MODEL |
#endif
    CALIBRATION_STATUS_LIVE_ADJUST;

// Calibration steps enforced after service prep
const CalibrationStatus CALIBRATION_FORCE_PREP = CALIBRATION_STATUS_Z;

bool calibration_status_get(CalibrationStatus components);
void calibration_status_set(CalibrationStatus components);
void calibration_status_clear(CalibrationStatus components);

// PINDA has an independent calibration flag
inline bool calibration_status_pinda() { return eeprom_read_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA); }
