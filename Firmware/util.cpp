#include "Configuration.h"

#include "ultralcd.h"
#include "sound.h"
#include "language.h"
#include "util.h"

// Allocate the version string in the program memory. Otherwise the string lands either on the stack or in the global RAM.
const char FW_VERSION_STR[] PROGMEM = FW_VERSION;

const char* FW_VERSION_STR_P()
{
    return FW_VERSION_STR;
}

const char FW_PRUSA3D_MAGIC_STR[] PROGMEM = FW_PRUSA3D_MAGIC;

const char* FW_PRUSA3D_MAGIC_STR_P()
{
    return FW_PRUSA3D_MAGIC_STR;
}

const char STR_REVISION_DEV  [] PROGMEM = "dev";
const char STR_REVISION_ALPHA[] PROGMEM = "alpha";
const char STR_REVISION_BETA [] PROGMEM = "beta";
const char STR_REVISION_RC   [] PROGMEM = "rc";

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

// Parse a major.minor.revision version number.
// Return true if valid.
inline bool parse_version(const char *str, uint16_t version[4])
{   
#if 0
    SERIAL_ECHOPGM("Parsing version string ");
    SERIAL_ECHO(str);
    SERIAL_ECHOLNPGM("");
#endif

    const char *major = str;
    const char *p = str;
    while (is_digit(*p)) ++ p;
    if (*p != '.')
        return false;
    const char *minor = ++ p;
    while (is_digit(*p)) ++ p;
    if (*p != '.')
        return false;
    const char *rev = ++ p;
    while (is_digit(*p)) ++ p;
    if (! is_whitespace_or_nl_or_eol(*p) && *p != '-')
        return false;

    char *endptr = NULL;
    version[0] = strtol(major, &endptr, 10);
    if (endptr != minor - 1)
        return false;
    version[1] = strtol(minor, &endptr, 10);
    if (endptr != rev - 1)
        return false;
    version[2] = strtol(rev, &endptr, 10);
    if (endptr != p)
        return false;

    version[3] = FIRMWARE_REVISION_RELEASED;
    if (*p ++ == '-') {
        const char *q = p;
        while (! is_whitespace_or_nl_or_eol(*q))
            ++ q;
        uint8_t n = q - p;
        if (n == strlen_P(STR_REVISION_DEV) && strncmp_P(p, STR_REVISION_DEV, n) == 0)
            version[3] = FIRMWARE_REVISION_DEV;
        else if (n == strlen_P(STR_REVISION_ALPHA) && strncmp_P(p, STR_REVISION_ALPHA, n) == 0)
            version[3] = FIRMWARE_REVISION_ALPHA;
        else if (n == strlen_P(STR_REVISION_BETA) && strncmp_P(p, STR_REVISION_BETA, n) == 0)
            version[3] = FIRMWARE_REVISION_BETA;
        else if ((n == 2 || n == 3) && (p[0] == 'r' || p[0] == 'R') && (p[1] == 'c' || p[1] == 'C')) {
            if (n == 2)
                version[3] = FIRMWARE_REVISION_RC;
            else {
                if (is_digit(p[2]))
                    version[3] = FIRMWARE_REVISION_RC + p[2] - '1';
                else
                    return false;
            }
        } else
            return false;
    }

#if 0
    SERIAL_ECHOPGM("Version parsed, major: ");
    SERIAL_ECHO(version[0]);
    SERIAL_ECHOPGM(", minor: ");
    SERIAL_ECHO(version[1]);
    SERIAL_ECHOPGM(", revision: ");
    SERIAL_ECHO(version[2]);
    SERIAL_ECHOPGM(", flavor: ");
    SERIAL_ECHO(version[3]);
    SERIAL_ECHOLNPGM("");
#endif
    return true;
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

// Parse a major.minor.revision version number.
// Return true if valid.
inline bool parse_version_P(const char *str, uint16_t version[4])
{    
#if 0
    SERIAL_ECHOPGM("Parsing version string ");
    SERIAL_ECHORPGM(str);
    SERIAL_ECHOLNPGM("");
#endif

    const char *major = str;
    const char *p = str;
    while (is_digit(char(pgm_read_byte(p)))) ++ p;
    if (pgm_read_byte(p) != '.')
        return false;
    const char *minor = ++ p;
    while (is_digit(char(pgm_read_byte(p)))) ++ p;
    if (pgm_read_byte(p) != '.')
        return false;
    const char *rev = ++ p;
    while (is_digit(char(pgm_read_byte(p)))) ++ p;
    if (! is_whitespace_or_nl_or_eol(char(pgm_read_byte(p))) && pgm_read_byte(p) != '-')
        return false;

    char buf[5];
    uint8_t n = minor - major - 1;
    if (n > 4)
        return false;
    memcpy_P(buf, major, n); buf[n] = 0;
    char *endptr = NULL;
    version[0] = strtol(buf, &endptr, 10);
    if (*endptr != 0)
        return false;
    n = rev - minor - 1;
    if (n > 4)
        return false;
    memcpy_P(buf, minor, n); buf[n] = 0;
    version[1] = strtol(buf, &endptr, 10);
    if (*endptr != 0)
        return false;
    n = p - rev;
    if (n > 4)
        return false;
    memcpy_P(buf, rev, n);
    buf[n] = 0;
    version[2] = strtol(buf, &endptr, 10);
    if (*endptr != 0)
        return false;

    version[3] = FIRMWARE_REVISION_RELEASED;
    if (pgm_read_byte(p ++) == '-') {
        const char *q = p;
        while (! is_whitespace_or_nl_or_eol(char(pgm_read_byte(q))))
            ++ q;
        n = q - p;
        if (n == strlen_P(STR_REVISION_DEV) && strncmp_PP(p, STR_REVISION_DEV, n) == 0)
            version[3] = FIRMWARE_REVISION_DEV;
        else if (n == strlen_P(STR_REVISION_ALPHA) && strncmp_PP(p, STR_REVISION_ALPHA, n) == 0)
            version[3] = FIRMWARE_REVISION_ALPHA;
        else if (n == strlen_P(STR_REVISION_BETA) && strncmp_PP(p, STR_REVISION_BETA, n) == 0)
            version[3] = FIRMWARE_REVISION_BETA;
        else if ((n == 2 || n == 3) && strncmp_PP(p, STR_REVISION_RC, 2) == 0) {
            if (n == 2)
                version[3] = FIRMWARE_REVISION_RC;
            else {
                p += 2;
                if (is_digit(pgm_read_byte(p)))
                    version[3] = FIRMWARE_REVISION_RC + pgm_read_byte(p) - '1';
                else
                    return false;
            }
        } else
            return false;
    }

#if 0
    SERIAL_ECHOPGM("Version parsed, major: ");
    SERIAL_ECHO(version[0]);
    SERIAL_ECHOPGM(", minor: ");
    SERIAL_ECHO(version[1]);
    SERIAL_ECHOPGM(", revision: ");
    SERIAL_ECHO(version[2]);
    SERIAL_ECHOPGM(", flavor: ");
    SERIAL_ECHO(version[3]);
    SERIAL_ECHOLNPGM("");
#endif
    return true;
}

// 1 - yes, 0 - false, -1 - error;
inline int8_t is_provided_version_newer(const char *version_string)
{
    uint16_t ver_gcode[4], ver_current[4];
    if (! parse_version(version_string, ver_gcode))
        return -1;
    if (! parse_version_P(FW_VERSION_STR, ver_current))
        return 0; // this shall not happen
    for (uint8_t i = 0; i < 3; ++ i)
        if (ver_gcode[i] > ver_current[i])
            return 1;
    return 0;
}

bool force_selftest_if_fw_version()
{
	//if fw version used before flashing new firmware (fw version currently stored in eeprom) is lower then 3.1.2-RC2, function returns true to force selftest

	uint16_t ver_eeprom[4];
	uint16_t ver_with_calibration[4] = {3, 1, 2, 4}; //hardcoded 3.1.2-RC2 version
	bool force_selftest = false;

	ver_eeprom[0] = eeprom_read_word((uint16_t*)EEPROM_FIRMWARE_VERSION_MAJOR);
	ver_eeprom[1] = eeprom_read_word((uint16_t*)EEPROM_FIRMWARE_VERSION_MINOR);
	ver_eeprom[2] = eeprom_read_word((uint16_t*)EEPROM_FIRMWARE_VERSION_REVISION);
	ver_eeprom[3] = eeprom_read_word((uint16_t*)EEPROM_FIRMWARE_VERSION_FLAVOR);

	for (uint8_t i = 0; i < 4; ++i) {
		if (ver_with_calibration[i] > ver_eeprom[i]) {
			force_selftest = true;
			break;
		}
		else if (ver_with_calibration[i] < ver_eeprom[i])
			break;
	}

	//force selftest also in case that version used before flashing new firmware was 3.2.0-RC1
	if ((ver_eeprom[0] == 3) && (ver_eeprom[1] == 2) && (ver_eeprom[2] == 0) && (ver_eeprom[3] == 3)) force_selftest = true;
	
	return force_selftest;
}

bool show_upgrade_dialog_if_version_newer(const char *version_string)
{
    uint16_t ver_gcode[4], ver_current[4];
    if (! parse_version(version_string, ver_gcode)) {
//        SERIAL_PROTOCOLLNPGM("parse_version failed");
        return false;
    }
    if (! parse_version_P(FW_VERSION_STR, ver_current)) {
//        SERIAL_PROTOCOLLNPGM("parse_version_P failed");
        return false; // this shall not happen
    }
//    SERIAL_PROTOCOLLNPGM("versions parsed");
    bool upgrade = false;
    for (uint8_t i = 0; i < 4; ++ i) {
        if (ver_gcode[i] > ver_current[i]) {
            upgrade = true;
            break;
        } else if (ver_gcode[i] < ver_current[i])
            break;
    }

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
    for (int8_t i = 0; i < FW_PRUSA3D_MAGIC_LEN; ++ i)
        eeprom_update_byte((uint8_t*)(EEPROM_FIRMWARE_PRUSA_MAGIC+i), pgm_read_byte(FW_PRUSA3D_MAGIC_STR+i));
    uint16_t ver_current[4];
    if (parse_version_P(FW_VERSION_STR, ver_current)) {
        eeprom_update_word((uint16_t*)EEPROM_FIRMWARE_VERSION_MAJOR,    ver_current[0]);
        eeprom_update_word((uint16_t*)EEPROM_FIRMWARE_VERSION_MINOR,    ver_current[1]);
        eeprom_update_word((uint16_t*)EEPROM_FIRMWARE_VERSION_REVISION, ver_current[2]);
        // See FirmwareRevisionFlavorType for the definition of firmware flavors.
        eeprom_update_word((uint16_t*)EEPROM_FIRMWARE_VERSION_FLAVOR,   ver_current[3]);
    }
}


//-//
#define MSG_PRINT_CHECKING_FAILED_TIMEOUT 30

ClNozzleDiameter oNozzleDiameter=ClNozzleDiameter::_Diameter_400;
ClCheckMode oCheckMode=ClCheckMode::_None;
ClCheckModel oCheckModel=ClCheckModel::_None;
ClCheckVersion oCheckVersion=ClCheckVersion::_None;
ClCheckGcode oCheckGcode=ClCheckGcode::_None;

void fCheckModeInit()
{
oCheckMode=(ClCheckMode)eeprom_read_byte((uint8_t*)EEPROM_CHECK_MODE);
if(oCheckMode==ClCheckMode::_Undef)
     {
     oCheckMode=ClCheckMode::_Warn;
     eeprom_update_byte((uint8_t*)EEPROM_CHECK_MODE,(uint8_t)oCheckMode);
     }
if(farm_mode)
     {
     oCheckMode=ClCheckMode::_Strict;
     if(eeprom_read_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM)==EEPROM_EMPTY_VALUE16)
          eeprom_update_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM,EEPROM_NOZZLE_DIAMETER_uM_DEFAULT);
     }
oNozzleDiameter=(ClNozzleDiameter)eeprom_read_byte((uint8_t*)EEPROM_NOZZLE_DIAMETER);
if((oNozzleDiameter==ClNozzleDiameter::_Diameter_Undef)&& !farm_mode)
     {
     oNozzleDiameter=ClNozzleDiameter::_Diameter_400;
     eeprom_update_byte((uint8_t*)EEPROM_NOZZLE_DIAMETER,(uint8_t)oNozzleDiameter);
     eeprom_update_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM,EEPROM_NOZZLE_DIAMETER_uM_DEFAULT);
     }
oCheckModel=(ClCheckModel)eeprom_read_byte((uint8_t*)EEPROM_CHECK_MODEL);
if(oCheckModel==ClCheckModel::_Undef)
     {
     oCheckModel=ClCheckModel::_Warn;
     eeprom_update_byte((uint8_t*)EEPROM_CHECK_MODEL,(uint8_t)oCheckModel);
     }
oCheckVersion=(ClCheckVersion)eeprom_read_byte((uint8_t*)EEPROM_CHECK_VERSION);
if(oCheckVersion==ClCheckVersion::_Undef)
     {
     oCheckVersion=ClCheckVersion::_Warn;
     eeprom_update_byte((uint8_t*)EEPROM_CHECK_VERSION,(uint8_t)oCheckVersion);
     }
oCheckGcode=(ClCheckGcode)eeprom_read_byte((uint8_t*)EEPROM_CHECK_GCODE);
if(oCheckGcode==ClCheckGcode::_Undef)
     {
     oCheckGcode=ClCheckGcode::_Warn;
     eeprom_update_byte((uint8_t*)EEPROM_CHECK_GCODE,(uint8_t)oCheckGcode);
     }
}

void nozzle_diameter_check(uint16_t nDiameter)
{
uint16_t nDiameter_um;

if(oCheckMode==ClCheckMode::_None)
     return;
nDiameter_um=eeprom_read_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM);
if(nDiameter==nDiameter_um)
     return;
//SERIAL_ECHO_START;
//SERIAL_ECHOLNPGM("Printer nozzle diameter differs from the G-code ...");
//SERIAL_ECHOPGM("actual  : ");
//SERIAL_ECHOLN((float)(nDiameter_um/1000.0));
//SERIAL_ECHOPGM("expected: ");
//SERIAL_ECHOLN((float)(nDiameter/1000.0));
switch(oCheckMode)
     {
     case ClCheckMode::_Warn:
//          lcd_show_fullscreen_message_and_wait_P(_i("Printer nozzle diameter differs from the G-code. Continue?"));
lcd_display_message_fullscreen_P(_i("Printer nozzle diameter differs from the G-code. Continue?"));
lcd_wait_for_click_delay(MSG_PRINT_CHECKING_FAILED_TIMEOUT);
//???custom_message_type=CUSTOM_MSG_TYPE_STATUS; // display / status-line recovery
lcd_update_enable(true);           // display / status-line recovery
          break;
     case ClCheckMode::_Strict:
          lcd_show_fullscreen_message_and_wait_P(_i("Printer nozzle diameter differs from the G-code. Please check the value in settings. Print cancelled."));
          lcd_print_stop();
          break;
     case ClCheckMode::_None:
     case ClCheckMode::_Undef:
          break;
     }
if(!farm_mode)
     {
     bSettings=false;                             // flag ('fake parameter') for 'lcd_hw_setup_menu()' function
     menu_submenu(lcd_hw_setup_menu);
     }
}

void printer_model_check(uint16_t nPrinterModel)
{
if(oCheckModel==ClCheckModel::_None)
     return;
if(nPrinterModel==nPrinterType)
     return;
//SERIAL_ECHO_START;
//SERIAL_ECHOLNPGM("Printer model differs from the G-code ...");
//SERIAL_ECHOPGM("actual  : ");
//SERIAL_ECHOLN(nPrinterType);
//SERIAL_ECHOPGM("expected: ");
//SERIAL_ECHOLN(nPrinterModel);
switch(oCheckModel)
     {
     case ClCheckModel::_Warn:
//          lcd_show_fullscreen_message_and_wait_P(_i("Printer model differs from the G-code. Continue?"));
lcd_display_message_fullscreen_P(_i("G-code sliced for a different printer type. Continue?"));
lcd_wait_for_click_delay(MSG_PRINT_CHECKING_FAILED_TIMEOUT);
//???custom_message_type=CUSTOM_MSG_TYPE_STATUS; // display / status-line recovery
lcd_update_enable(true);           // display / status-line recovery
          break;
     case ClCheckModel::_Strict:
          lcd_show_fullscreen_message_and_wait_P(_i("G-code sliced for a different printer type. Please re-slice the model again. Print cancelled."));
          lcd_print_stop();
          break;
     case ClCheckModel::_None:
     case ClCheckModel::_Undef:
          break;
     }
}

uint8_t mCompareValue(uint16_t nX,uint16_t nY)
{
if(nX>nY)
     return((uint8_t)ClCompareValue::_Greater);
if(nX<nY)
     return((uint8_t)ClCompareValue::_Less);
return((uint8_t)ClCompareValue::_Equal);
}

void fw_version_check(const char *pVersion)
{
uint16_t aVersion[4];
uint8_t nCompareValueResult;

if(oCheckVersion==ClCheckVersion::_None)
     return;
parse_version(pVersion,aVersion);
nCompareValueResult=mCompareValue(aVersion[0],eeprom_read_word((uint16_t*)EEPROM_FIRMWARE_VERSION_MAJOR))<<6;
nCompareValueResult+=mCompareValue(aVersion[1],eeprom_read_word((uint16_t*)EEPROM_FIRMWARE_VERSION_MINOR))<<4;
nCompareValueResult+=mCompareValue(aVersion[2],eeprom_read_word((uint16_t*)EEPROM_FIRMWARE_VERSION_REVISION))<<2;
nCompareValueResult+=mCompareValue(aVersion[3],eeprom_read_word((uint16_t*)EEPROM_FIRMWARE_VERSION_FLAVOR));
if(nCompareValueResult==COMPARE_VALUE_EQUAL)
     return;
if((nCompareValueResult<COMPARE_VALUE_EQUAL)&&oCheckVersion==ClCheckVersion::_Warn)
     return;
//SERIAL_ECHO_START;
//SERIAL_ECHOLNPGM("Printer FW version differs from the G-code ...");
//SERIAL_ECHOPGM("actual  : ");
//SERIAL_ECHOLN(FW_VERSION);
//SERIAL_ECHOPGM("expected: ");
//SERIAL_ECHOLN(pVersion);
switch(oCheckVersion)
     {
     case ClCheckVersion::_Warn:
//          lcd_show_fullscreen_message_and_wait_P(_i("Printer FW version differs from the G-code. Continue?"));
lcd_display_message_fullscreen_P(_i("G-code sliced for a newer firmware. Continue?"));
lcd_wait_for_click_delay(MSG_PRINT_CHECKING_FAILED_TIMEOUT);
//???custom_message_type=CUSTOM_MSG_TYPE_STATUS; // display / status-line recovery
lcd_update_enable(true);           // display / status-line recovery
          break;
     case ClCheckVersion::_Strict:
          lcd_show_fullscreen_message_and_wait_P(_i("G-code sliced for a newer firmware. Please update the firmware. Print cancelled."));
          lcd_print_stop();
          break;
     case ClCheckVersion::_None:
     case ClCheckVersion::_Undef:
          break;
     }
}

void gcode_level_check(uint16_t nGcodeLevel)
{
if(oCheckGcode==ClCheckGcode::_None)
     return;
if(nGcodeLevel==(uint16_t)GCODE_LEVEL)
     return;
if((nGcodeLevel<(uint16_t)GCODE_LEVEL)&&(oCheckGcode==ClCheckGcode::_Warn))
     return;
//SERIAL_ECHO_START;
//SERIAL_ECHOLNPGM("Printer G-code level differs from the G-code ...");
//SERIAL_ECHOPGM("actual  : ");
//SERIAL_ECHOLN(GCODE_LEVEL);
//SERIAL_ECHOPGM("expected: ");
//SERIAL_ECHOLN(nGcodeLevel);
switch(oCheckGcode)
     {
     case ClCheckGcode::_Warn:
//          lcd_show_fullscreen_message_and_wait_P(_i("Printer G-code level differs from the G-code. Continue?"));
lcd_display_message_fullscreen_P(_i("G-code sliced for a different level. Continue?"));
lcd_wait_for_click_delay(MSG_PRINT_CHECKING_FAILED_TIMEOUT);
//???custom_message_type=CUSTOM_MSG_TYPE_STATUS; // display / status-line recovery
lcd_update_enable(true);           // display / status-line recovery
          break;
     case ClCheckGcode::_Strict:
          lcd_show_fullscreen_message_and_wait_P(_i("G-code sliced for a different level. Please re-slice the model again. Print cancelled."));
          lcd_print_stop();
          break;
     case ClCheckGcode::_None:
     case ClCheckGcode::_Undef:
          break;
     }
}

//-// -> cmdqueue ???
#define PRINTER_NAME_LENGTH ((sizeof(PRINTER_MMU_NAME)>sizeof(PRINTER_NAME))?(sizeof(PRINTER_MMU_NAME)-1):(sizeof(PRINTER_NAME)-1))
#define GCODE_DELIMITER '"'
#define ELLIPSIS "..."

char* code_string(char* pStr,size_t* nLength)
{
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
return(pStrBegin);
}

void printer_smodel_check(char* pStrPos)
{
char* pResult;
size_t nLength,nPrinterNameLength;
bool bCheckOK;
char sPrinterName[PRINTER_NAME_LENGTH+sizeof(ELLIPSIS)-1+1]="";

nPrinterNameLength=strlen_P(::sPrinterName);
pResult=code_string(pStrPos,&nLength);
if(pResult!=NULL)
     {
     strlcpy(sPrinterName,pResult,min(nPrinterNameLength,nLength)+1);
     if(nLength>nPrinterNameLength)
          strcat(sPrinterName,ELLIPSIS);
     bCheckOK=(nLength==nPrinterNameLength);
     if(bCheckOK&&(!strncasecmp_P(pResult,::sPrinterName,nLength))) // i.e. string compare execute only if lengths are same
          return;
     }
//SERIAL_ECHO_START;
//SERIAL_ECHOLNPGM("Printer model differs from the G-code ...");
//SERIAL_ECHOPGM("actual  : \"");
//serialprintPGM(::sPrinterName);
//SERIAL_ECHOLNPGM("\"");
//SERIAL_ECHOPGM("expected: \"");
////SERIAL_ECHO(sPrinterName);
//SERIAL_ECHOLNPGM("\"");
switch(oCheckModel)
     {
     case ClCheckModel::_Warn:
//          lcd_show_fullscreen_message_and_wait_P(_i("Printer model differs from the G-code. Continue?"));
lcd_display_message_fullscreen_P(_i("G-code sliced for a different printer type. Continue?"));
lcd_wait_for_click_delay(MSG_PRINT_CHECKING_FAILED_TIMEOUT);
//???custom_message_type=CUSTOM_MSG_TYPE_STATUS; // display / status-line recovery
lcd_update_enable(true);           // display / status-line recovery
          break;
     case ClCheckModel::_Strict:
          lcd_show_fullscreen_message_and_wait_P(_i("G-code sliced for a different printer type. Please re-slice the model again. Print cancelled."));
          lcd_print_stop();
          break;
     case ClCheckModel::_None:
     case ClCheckModel::_Undef:
          break;
     }
}

void fSetMmuMode(bool bMMu)
{
if(bMMu)
     {
     nPrinterType=pgm_read_word(&_nPrinterMmuType);
     sPrinterName=_sPrinterMmuName;
     }
else {
     nPrinterType=pgm_read_word(&_nPrinterType);
     sPrinterName=_sPrinterName;
     }
}
