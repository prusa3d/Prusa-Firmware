#include "Configuration.h"

#include "ultralcd.h"
#include "menu.h"
#include "sound.h"
#include "language.h"
#include "util.h"
#include <avr/pgmspace.h>
#include "Prusa_farm.h"

// Allocate the version string in the program memory. Otherwise the string lands either on the stack or in the global RAM.
static const char FW_VERSION_STR[] PROGMEM = FW_VERSION;
const uint16_t FW_VERSION_NR[4] PROGMEM = {
    FW_MAJOR,
    FW_MINOR,
    FW_REVISION,
#ifndef FW_FLAVOR
    FW_COMMIT_NR
#else
#   if FW_DEV_VERSION == FW_VERSION_ALPHA
    FIRMWARE_REVISION_ALPHA + FW_FLAVERSION
#   elif FW_DEV_VERSION == FW_VERSION_BETA
    FIRMWARE_REVISION_BETA + FW_FLAVERSION
#   elif FW_DEV_VERSION == FW_VERSION_RC
    FIRMWARE_REVISION_RC + FW_FLAVERSION
#   elif FW_DEV_VERSION == FW_VERSION_GOLD
    0
#   endif
#endif
};

const char* FW_VERSION_STR_P()
{
    return FW_VERSION_STR;
}

const char FW_PRUSA3D_MAGIC_STR[] PROGMEM = FW_PRUSA3D_MAGIC;

const char* FW_PRUSA3D_MAGIC_STR_P()
{
    return FW_PRUSA3D_MAGIC_STR;
}

const char STR_REVISION_DEV  [] PROGMEM = "DEV";
const char STR_REVISION_ALPHA[] PROGMEM = "ALPHA";
const char STR_REVISION_BETA [] PROGMEM = "BETA";
const char STR_REVISION_RC   [] PROGMEM = "RC";

inline bool is_whitespace_or_nl(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

inline bool is_whitespace_or_nl_or_eol(char c)
{
    return c == 0 || c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

inline bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

char const * __attribute__((noinline)) Number(char const *str, uint16_t *v){
    *v = 0;
    while(is_digit(*str)){
        *v *= 10;
        *v += *str - '0';
        ++str;
    }
    return str;
}

bool __attribute__((noinline)) Tag(const char *str, const char *tag_P, uint8_t tagSize, uint16_t tagMask, uint16_t *v){
    if( ! strncmp_P(str, tag_P, tagSize) ){
        Number(str + tagSize, v);
        *v |= tagMask;
        return true;
    }
    return false;
}

// Parse a major.minor.revision version number.
// Return true if valid.
bool parse_version(const char *str, uint16_t version[4]) {
    for(uint8_t i = 0; i < 2; ++i){
        str = Number(str, version + i);
        if (*str != '.')
            return false;
        ++str;
    }
    str = Number(str, version + 2);

    version[3] = FIRMWARE_REVISION_RELEASED;

    // skip everything else until eol or '-'
    for(;;){
        if(is_whitespace_or_nl_or_eol(*str)){
            // speculatively reached the end of line, silently ignoring anything which is not a '-'
            return true;
        }
        if( *str == '-'){
            break; // tag expected
        }
        ++str;
    }

//    SERIAL_ECHOPGM("parse_version: ");
//    SERIAL_ECHO(version[0]);
//    SERIAL_ECHO('.');
//    SERIAL_ECHO(version[1]);
//    SERIAL_ECHO('.');
//    SERIAL_ECHO(version[2]);
//    SERIAL_ECHO('.');
//    SERIAL_ECHOLN(version[3]);
    if (*str++ == '-') {
        switch(*str){
        case 'A': // expect ALPHA
            static_assert(sizeof(STR_REVISION_ALPHA) == 6);
            return Tag( str, STR_REVISION_ALPHA, sizeof(STR_REVISION_ALPHA) - 1, FIRMWARE_REVISION_ALPHA, version + 3);
        case 'B': // expect BETA
            static_assert(sizeof(STR_REVISION_BETA) == 5);
            return Tag( str, STR_REVISION_BETA, sizeof(STR_REVISION_BETA) - 1, FIRMWARE_REVISION_BETA, version + 3);
        case 'D': // expect DEV
            static_assert(sizeof(STR_REVISION_DEV) == 4);
            return Tag( str, STR_REVISION_DEV, sizeof(STR_REVISION_DEV) - 1, FIRMWARE_REVISION_DEV, version + 3);
        case 'R': // expect RC
            static_assert(sizeof(STR_REVISION_RC) == 3);
            return Tag( str, STR_REVISION_RC, sizeof(STR_REVISION_RC) - 1, FIRMWARE_REVISION_RC, version + 3);
        default: return false; // fail everything else
            }
    }
//    SERIAL_ECHOPGM("parse_version with tag: ");
//    SERIAL_ECHO(version[0]);
//    SERIAL_ECHO('.');
//    SERIAL_ECHO(version[1]);
//    SERIAL_ECHO('.');
//    SERIAL_ECHO(version[2]);
//    SERIAL_ECHO('.');
//    SERIAL_ECHOLN(version[3]);
    return false;
}

inline bool strncmp_PP(const char *p1, const char *p2, uint8_t n)
{
    for (; n > 0; -- n, ++ p1, ++ p2) {
		if (pgm_read_byte(p1) >= 65 && pgm_read_byte(p1) <= 92) //p1 is upper case (p2 is always lowercase)
		{
			if ((pgm_read_byte(p1)+32) < pgm_read_byte(p2))
				return -1;
			if ((pgm_read_byte(p1)+32) > pgm_read_byte(p2))
				return 1;
		}
		else if (pgm_read_byte(p1) == 0) {
			return 0;
		}
		else { //p1 is lowercase
			if (pgm_read_byte(p1) < pgm_read_byte(p2))
				return -1;
			if (pgm_read_byte(p1) > pgm_read_byte(p2))
				return 1;
		}            
    }
    return 0;
}

// 1 - yes, 0 - false, -1 - error;
inline int8_t is_provided_version_newer(const char *version_string)
{
    uint16_t ver_gcode[4];
    if (! parse_version(version_string, ver_gcode))
        return -1;
    for (uint8_t i = 0; i < 4; ++ i)
    {
        uint16_t v = (uint16_t)pgm_read_word(&FW_VERSION_NR[i]);
        if (ver_gcode[i] > v)
            return 1;
        else if (ver_gcode[i] < v)
            return 0;
    }
    return 0;
}

bool eeprom_fw_version_older_than_p(const uint16_t (&ver_req)[4])
{
    uint16_t ver_eeprom[4];
    ver_eeprom[0] = eeprom_read_word((uint16_t*)EEPROM_FIRMWARE_VERSION_MAJOR);
    ver_eeprom[1] = eeprom_read_word((uint16_t*)EEPROM_FIRMWARE_VERSION_MINOR);
    ver_eeprom[2] = eeprom_read_word((uint16_t*)EEPROM_FIRMWARE_VERSION_REVISION);
    ver_eeprom[3] = eeprom_read_word((uint16_t*)EEPROM_FIRMWARE_VERSION_FLAVOR);

    for (uint8_t i = 0; i < 4; ++i) {
        uint16_t v = pgm_read_word(&ver_req[i]);
        if (v > ver_eeprom[i])
            return true;
        else if (v < ver_eeprom[i])
            break;
    }

    return false;
}

bool show_upgrade_dialog_if_version_newer(const char *version_string)
{
    if(oCheckVersion == ClCheckVersion::_None)
        return false;

    int8_t upgrade = is_provided_version_newer(version_string);
    if (upgrade < 0)
        return false;

    if (upgrade) {
        lcd_display_message_fullscreen_P(_i("New firmware version available:"));////MSG_NEW_FIRMWARE_AVAILABLE c=20 r=2
        lcd_puts_at_P(0, 2, PSTR(""));
        for (const char *c = version_string; ! is_whitespace_or_nl_or_eol(*c); ++ c)
            lcd_putc(*c);
        lcd_puts_at_P(0, 3, _i("Please upgrade."));////MSG_NEW_FIRMWARE_PLEASE_UPGRADE c=20
        Sound_MakeCustom(50,1000,false);
        delay_keep_alive(500);
        Sound_MakeCustom(50,1000,false);
        lcd_wait_for_click_delay(30);
        lcd_update_enable(true);
        lcd_clear();
        lcd_update(0);
    }

    // Succeeded.
    return true;
}

void update_current_firmware_version_to_eeprom()
{
    for (int8_t i = 0; i < FW_PRUSA3D_MAGIC_LEN; ++ i){
        eeprom_update_byte((uint8_t*)(EEPROM_FIRMWARE_PRUSA_MAGIC+i), pgm_read_byte(FW_PRUSA3D_MAGIC_STR+i));
    }
    eeprom_update_word((uint16_t*)EEPROM_FIRMWARE_VERSION_MAJOR,    (uint16_t)pgm_read_word(&FW_VERSION_NR[0]));
    eeprom_update_word((uint16_t*)EEPROM_FIRMWARE_VERSION_MINOR,    (uint16_t)pgm_read_word(&FW_VERSION_NR[1]));
    eeprom_update_word((uint16_t*)EEPROM_FIRMWARE_VERSION_REVISION, (uint16_t)pgm_read_word(&FW_VERSION_NR[2]));
    // See FirmwareRevisionFlavorType for the definition of firmware flavors.
    eeprom_update_word((uint16_t*)EEPROM_FIRMWARE_VERSION_FLAVOR,   (uint16_t)pgm_read_word(&FW_VERSION_NR[3]));
}

ClNozzleDiameter oNozzleDiameter=ClNozzleDiameter::_Diameter_400;
ClCheckMode oCheckMode=ClCheckMode::_None;
ClCheckModel oCheckModel=ClCheckModel::_None;
ClCheckVersion oCheckVersion=ClCheckVersion::_None;
ClCheckGcode oCheckGcode=ClCheckGcode::_None;

void fCheckModeInit() {
    oCheckMode = (ClCheckMode)eeprom_read_byte((uint8_t *)EEPROM_CHECK_MODE);
    if (oCheckMode == ClCheckMode::_Undef) {
        oCheckMode = ClCheckMode::_Warn;
        eeprom_update_byte((uint8_t *)EEPROM_CHECK_MODE, (uint8_t)oCheckMode);
    }
    if (farm_mode) {
        oCheckMode = ClCheckMode::_Strict;
        eeprom_init_default_word((uint16_t *)EEPROM_NOZZLE_DIAMETER_uM, EEPROM_NOZZLE_DIAMETER_uM_DEFAULT);
    }
    oNozzleDiameter = (ClNozzleDiameter)eeprom_read_byte((uint8_t *)EEPROM_NOZZLE_DIAMETER);
    if ((oNozzleDiameter == ClNozzleDiameter::_Diameter_Undef) && !farm_mode) {
        oNozzleDiameter = ClNozzleDiameter::_Diameter_400;
        eeprom_update_byte((uint8_t *)EEPROM_NOZZLE_DIAMETER, (uint8_t)oNozzleDiameter);
        eeprom_update_word((uint16_t *)EEPROM_NOZZLE_DIAMETER_uM, EEPROM_NOZZLE_DIAMETER_uM_DEFAULT);
    }
    oCheckModel = (ClCheckModel)eeprom_read_byte((uint8_t *)EEPROM_CHECK_MODEL);
    if (oCheckModel == ClCheckModel::_Undef) {
        oCheckModel = ClCheckModel::_Warn;
        eeprom_update_byte((uint8_t *)EEPROM_CHECK_MODEL, (uint8_t)oCheckModel);
    }
    oCheckVersion = (ClCheckVersion)eeprom_read_byte((uint8_t *)EEPROM_CHECK_VERSION);
    if (oCheckVersion == ClCheckVersion::_Undef) {
        oCheckVersion = ClCheckVersion::_Warn;
        eeprom_update_byte((uint8_t *)EEPROM_CHECK_VERSION, (uint8_t)oCheckVersion);
    }
    oCheckGcode = (ClCheckGcode)eeprom_read_byte((uint8_t *)EEPROM_CHECK_GCODE);
    if (oCheckGcode == ClCheckGcode::_Undef) {
        oCheckGcode = ClCheckGcode::_Warn;
        eeprom_update_byte((uint8_t *)EEPROM_CHECK_GCODE, (uint8_t)oCheckGcode);
    }
}

static void render_M862_warnings(const char* warning, const char* strict, uint8_t check)
{
    if (check == 1) { // Warning, stop print if user selects 'No'
        if (lcd_show_fullscreen_message_yes_no_and_wait_P(warning, true, LCD_LEFT_BUTTON_CHOICE) == LCD_MIDDLE_BUTTON_CHOICE) {
            lcd_print_stop();
        }
    } else if (check == 2) { // Strict, always stop print
        lcd_show_fullscreen_message_and_wait_P(strict);
        lcd_print_stop();
    }
}

void nozzle_diameter_check(uint16_t nDiameter) {
    uint16_t nDiameter_um;

    if (oCheckMode == ClCheckMode::_None)
        return;
    nDiameter_um = eeprom_read_word((uint16_t *)EEPROM_NOZZLE_DIAMETER_uM);
    if (nDiameter == nDiameter_um)
        return;
    // SERIAL_ECHO_START;
    // SERIAL_ECHOLNPGM("Printer nozzle diameter differs from the G-code ...");
    // SERIAL_ECHOPGM("actual  : ");
    // SERIAL_ECHOLN((float)(nDiameter_um/1000.0));
    // SERIAL_ECHOPGM("expected: ");
    // SERIAL_ECHOLN((float)(nDiameter/1000.0));

    render_M862_warnings(
        _T(MSG_NOZZLE_DIFFERS_CONTINUE)
        ,_T(MSG_NOZZLE_DIFFERS_CANCELLED)
        ,(uint8_t)oCheckMode
    );

    if (!farm_mode) {
        bSettings = false; // flag ('fake parameter') for 'lcd_hw_setup_menu()' function
        menu_submenu(lcd_hw_setup_menu);
    }
}

void printer_model_check(uint16_t nPrinterModel, uint16_t actualPrinterModel) {
    if (oCheckModel == ClCheckModel::_None)
        return;
    if (nPrinterModel == actualPrinterModel)
        return;
    // SERIAL_ECHO_START;
    // SERIAL_ECHOLNPGM("Printer model differs from the G-code ...");
    // SERIAL_ECHOPGM("actual  : ");
    // SERIAL_ECHOLN(actualPrinterModel);
    // SERIAL_ECHOPGM("expected: ");
    // SERIAL_ECHOLN(nPrinterModel);
    render_M862_warnings(
        _T(MSG_GCODE_DIFF_PRINTER_CONTINUE)
        ,_T(MSG_GCODE_DIFF_PRINTER_CANCELLED)
        ,(uint8_t)oCheckModel
    );
}

uint8_t mCompareValue(uint16_t nX, uint16_t nY) {
    if (nX > nY)
        return ((uint8_t)ClCompareValue::_Greater);
    if (nX < nY)
        return ((uint8_t)ClCompareValue::_Less);
    return ((uint8_t)ClCompareValue::_Equal);
}

void fw_version_check(const char *pVersion) {
    if (oCheckVersion == ClCheckVersion::_None)
        return;

    uint16_t aVersion[4];
    uint8_t nCompareValueResult;
    parse_version(pVersion, aVersion);
    nCompareValueResult = mCompareValue(aVersion[0], eeprom_read_word((uint16_t *)EEPROM_FIRMWARE_VERSION_MAJOR)) << 6;
    nCompareValueResult += mCompareValue(aVersion[1], eeprom_read_word((uint16_t *)EEPROM_FIRMWARE_VERSION_MINOR)) << 4;
    nCompareValueResult += mCompareValue(aVersion[2], eeprom_read_word((uint16_t *)EEPROM_FIRMWARE_VERSION_REVISION)) << 2;
    nCompareValueResult += mCompareValue(aVersion[3], eeprom_read_word((uint16_t *)EEPROM_FIRMWARE_VERSION_FLAVOR));
    if (nCompareValueResult <= COMPARE_VALUE_EQUAL)
        return;

/*
    SERIAL_ECHO_START;
    SERIAL_ECHOLNPGM("Printer FW version differs from the G-code ...");
    SERIAL_ECHOPGM("actual  : ");
    SERIAL_ECHO(eeprom_read_word((uint16_t *)EEPROM_FIRMWARE_VERSION_MAJOR));
    SERIAL_ECHO('.');
    SERIAL_ECHO(eeprom_read_word((uint16_t *)EEPROM_FIRMWARE_VERSION_MINOR));
    SERIAL_ECHO('.');
    SERIAL_ECHO(eeprom_read_word((uint16_t *)EEPROM_FIRMWARE_VERSION_REVISION));
    SERIAL_ECHO('.');
    SERIAL_ECHO(eeprom_read_word((uint16_t *)EEPROM_FIRMWARE_VERSION_FLAVOR));
    SERIAL_ECHOPGM("\nexpected: ");
    SERIAL_ECHO(aVersion[0]);
    SERIAL_ECHO('.');
    SERIAL_ECHO(aVersion[1]);
    SERIAL_ECHO('.');
    SERIAL_ECHO(aVersion[2]);
    SERIAL_ECHO('.');
    SERIAL_ECHOLN(aVersion[3]);
*/

    render_M862_warnings(
        _T(MSG_GCODE_NEWER_FIRMWARE_CONTINUE)
        ,_T(MSG_GCODE_NEWER_FIRMWARE_CANCELLED)
        ,(uint8_t)oCheckVersion
    );
}

void gcode_level_check(uint16_t nGcodeLevel) {
    if (oCheckGcode == ClCheckGcode::_None)
        return;
    if (nGcodeLevel <= (uint16_t)GCODE_LEVEL)
        return;

    // SERIAL_ECHO_START;
    // SERIAL_ECHOLNPGM("Printer G-code level differs from the G-code ...");
    // SERIAL_ECHOPGM("actual  : ");
    // SERIAL_ECHOLN(GCODE_LEVEL);
    // SERIAL_ECHOPGM("expected: ");
    // SERIAL_ECHOLN(nGcodeLevel);

    render_M862_warnings(
        _T(MSG_GCODE_DIFF_CONTINUE)
        ,_T(MSG_GCODE_DIFF_CANCELLED)
        ,(uint8_t)oCheckGcode
    );
}

#define GCODE_DELIMITER '"'

char *code_string(const char *pStr, size_t *nLength) {
char* pStrBegin;
char* pStrEnd;

pStrBegin=strchr(pStr,GCODE_DELIMITER);
if(!pStrBegin)
     return(NULL);
pStrBegin++;
pStrEnd=strchr(pStrBegin,GCODE_DELIMITER);
if(!pStrEnd)
     return(NULL);
*nLength=pStrEnd-pStrBegin;
return pStrBegin;
}

void printer_smodel_check(const char *pStrPos, const char *actualPrinterSModel) {
char* pResult;
size_t nLength,nPrinterNameLength;

nPrinterNameLength = strlen_P(actualPrinterSModel);
pResult=code_string(pStrPos,&nLength);

if(pResult != NULL && nLength == nPrinterNameLength) {
     // Only compare them if the lengths match
     if (strncmp_P(pResult, actualPrinterSModel, nLength) == 0) return;
}

    render_M862_warnings(
        _T(MSG_GCODE_DIFF_PRINTER_CONTINUE)
        ,_T(MSG_GCODE_DIFF_PRINTER_CANCELLED)
        ,(uint8_t)oCheckModel
    );
}

uint16_t nPrinterType(bool bMMu) {
    if (bMMu) {
        return pgm_read_word(&_nPrinterMmuType);
    }
    else {
        return pgm_read_word(&_nPrinterType);
    }
}

const char *sPrinterType(bool bMMu) {
    if (bMMu) {
        return _sPrinterMmuName;
    }
    else {
        return _sPrinterName;
    }
}

void ip4_to_str(char* dest, uint8_t* IP)
{
    sprintf_P(dest, PSTR("%u.%u.%u.%u"), IP[0], IP[1], IP[2], IP[3]);
}


bool calibration_status_get(CalibrationStatus components)
{
    CalibrationStatus status = eeprom_read_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_V2);
    return ((status & components) == components);
}

void calibration_status_set(CalibrationStatus components)
{
    CalibrationStatus status = eeprom_read_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_V2);
    status |= components;
    eeprom_update_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_V2, status);
}

void calibration_status_clear(CalibrationStatus components)
{
    CalibrationStatus status = eeprom_read_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_V2);
    status &= ~components;
    eeprom_update_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_V2, status);
}
