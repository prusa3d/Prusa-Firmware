// Extracted from Prusa-Error-Codes repo
// Subject to automation and optimization
// BEWARE - this file shall be included only into mmu2_error_converter.cpp, not anywhere else!
#pragma once
#include "inttypes.h"
#include "../language.h"
#include <avr/pgmspace.h>
#include "buttons.h"
#include "../strlen_cx.h"
#include "../mmu2_supported_version.h"

namespace MMU2 {

static constexpr uint8_t ERR_MMU_CODE = 4;

typedef enum : uint16_t {
    ERR_UNDEF = 0,

    ERR_MECHANICAL = 100,
    ERR_MECHANICAL_FINDA_DIDNT_TRIGGER = 101,
    ERR_MECHANICAL_FINDA_DIDNT_GO_OFF = 102,
    ERR_MECHANICAL_FSENSOR_DIDNT_TRIGGER = 103,
    ERR_MECHANICAL_FSENSOR_DIDNT_GO_OFF = 104,

    ERR_MECHANICAL_PULLEY_CANNOT_MOVE = 105,
    ERR_MECHANICAL_FSENSOR_TOO_EARLY = 106,
    ERR_MECHANICAL_INSPECT_FINDA = 107,
    ERR_MECHANICAL_LOAD_TO_EXTRUDER_FAILED = 108,
    ERR_MECHANICAL_SELECTOR_CANNOT_HOME = 115,
    ERR_MECHANICAL_SELECTOR_CANNOT_MOVE = 116,
    ERR_MECHANICAL_IDLER_CANNOT_HOME = 125,
    ERR_MECHANICAL_IDLER_CANNOT_MOVE = 126,

    ERR_TEMPERATURE = 200,
    ERR_TEMPERATURE_PULLEY_WARNING_TMC_TOO_HOT = 201,
    ERR_TEMPERATURE_SELECTOR_WARNING_TMC_TOO_HOT = 211,
    ERR_TEMPERATURE_IDLER_WARNING_TMC_TOO_HOT = 221,

    ERR_TEMPERATURE_PULLEY_TMC_OVERHEAT_ERROR = 202,
    ERR_TEMPERATURE_SELECTOR_TMC_OVERHEAT_ERROR = 212,
    ERR_TEMPERATURE_IDLER_TMC_OVERHEAT_ERROR = 222,


    ERR_ELECTRICAL = 300,
    ERR_ELECTRICAL_PULLEY_TMC_DRIVER_ERROR = 301,
    ERR_ELECTRICAL_SELECTOR_TMC_DRIVER_ERROR = 311,
    ERR_ELECTRICAL_IDLER_TMC_DRIVER_ERROR = 321,

    ERR_ELECTRICAL_PULLEY_TMC_DRIVER_RESET = 302,
    ERR_ELECTRICAL_SELECTOR_TMC_DRIVER_RESET = 312,
    ERR_ELECTRICAL_IDLER_TMC_DRIVER_RESET = 322,

    ERR_ELECTRICAL_PULLEY_TMC_UNDERVOLTAGE_ERROR = 303,
    ERR_ELECTRICAL_SELECTOR_TMC_UNDERVOLTAGE_ERROR = 313,
    ERR_ELECTRICAL_IDLER_TMC_UNDERVOLTAGE_ERROR = 323,

    ERR_ELECTRICAL_PULLEY_TMC_DRIVER_SHORTED = 304,
    ERR_ELECTRICAL_SELECTOR_TMC_DRIVER_SHORTED = 314,
    ERR_ELECTRICAL_IDLER_TMC_DRIVER_SHORTED = 324,

    ERR_ELECTRICAL_PULLEY_SELFTEST_FAILED = 305,
    ERR_ELECTRICAL_SELECTOR_SELFTEST_FAILED = 315,
    ERR_ELECTRICAL_IDLER_SELFTEST_FAILED = 325,

    ERR_CONNECT = 400,
    ERR_CONNECT_MMU_NOT_RESPONDING = 401,
    ERR_CONNECT_COMMUNICATION_ERROR = 402,


    ERR_SYSTEM = 500,
    ERR_SYSTEM_FILAMENT_ALREADY_LOADED = 501,
    ERR_SYSTEM_INVALID_TOOL = 502,
    ERR_SYSTEM_QUEUE_FULL = 503,
    ERR_SYSTEM_FW_UPDATE_NEEDED = 504,
    ERR_SYSTEM_FW_RUNTIME_ERROR = 505,
    ERR_SYSTEM_UNLOAD_MANUALLY = 506,
    ERR_SYSTEM_FILAMENT_EJECTED = 507,

    ERR_OTHER = 900
} err_num_t;

// Avr gcc has serious trouble understanding static data structures in PROGMEM
// and inadvertedly falls back to copying the whole structure into RAM (which is obviously unwanted).
// But since this file ought to be generated in the future from yaml prescription,
// it really makes no difference if there are "nice" data structures or plain arrays.
static const constexpr uint16_t errorCodes[] PROGMEM = {
    ERR_MECHANICAL_FINDA_DIDNT_TRIGGER,
    ERR_MECHANICAL_FINDA_DIDNT_GO_OFF,
    ERR_MECHANICAL_FSENSOR_DIDNT_TRIGGER,
    ERR_MECHANICAL_FSENSOR_DIDNT_GO_OFF,
    ERR_MECHANICAL_PULLEY_CANNOT_MOVE,
    ERR_MECHANICAL_FSENSOR_TOO_EARLY,
    ERR_MECHANICAL_INSPECT_FINDA,
    ERR_MECHANICAL_LOAD_TO_EXTRUDER_FAILED,
    ERR_MECHANICAL_SELECTOR_CANNOT_HOME,
    ERR_MECHANICAL_SELECTOR_CANNOT_MOVE,
    ERR_MECHANICAL_IDLER_CANNOT_HOME,
    ERR_MECHANICAL_IDLER_CANNOT_MOVE,
    ERR_TEMPERATURE_PULLEY_WARNING_TMC_TOO_HOT,
    ERR_TEMPERATURE_SELECTOR_WARNING_TMC_TOO_HOT,
    ERR_TEMPERATURE_IDLER_WARNING_TMC_TOO_HOT,
    ERR_TEMPERATURE_PULLEY_TMC_OVERHEAT_ERROR,
    ERR_TEMPERATURE_SELECTOR_TMC_OVERHEAT_ERROR,
    ERR_TEMPERATURE_IDLER_TMC_OVERHEAT_ERROR,
    ERR_ELECTRICAL_PULLEY_TMC_DRIVER_ERROR,
    ERR_ELECTRICAL_SELECTOR_TMC_DRIVER_ERROR,
    ERR_ELECTRICAL_IDLER_TMC_DRIVER_ERROR,
    ERR_ELECTRICAL_PULLEY_TMC_DRIVER_RESET,
    ERR_ELECTRICAL_SELECTOR_TMC_DRIVER_RESET,
    ERR_ELECTRICAL_IDLER_TMC_DRIVER_RESET,
    ERR_ELECTRICAL_PULLEY_TMC_UNDERVOLTAGE_ERROR,
    ERR_ELECTRICAL_SELECTOR_TMC_UNDERVOLTAGE_ERROR,
    ERR_ELECTRICAL_IDLER_TMC_UNDERVOLTAGE_ERROR,
    ERR_ELECTRICAL_PULLEY_TMC_DRIVER_SHORTED,
    ERR_ELECTRICAL_SELECTOR_TMC_DRIVER_SHORTED,
    ERR_ELECTRICAL_IDLER_TMC_DRIVER_SHORTED,
    ERR_ELECTRICAL_PULLEY_SELFTEST_FAILED,
    ERR_ELECTRICAL_SELECTOR_SELFTEST_FAILED,
    ERR_ELECTRICAL_IDLER_SELFTEST_FAILED,
    ERR_CONNECT_MMU_NOT_RESPONDING,
    ERR_CONNECT_COMMUNICATION_ERROR,
    ERR_SYSTEM_FILAMENT_ALREADY_LOADED, 
    ERR_SYSTEM_INVALID_TOOL, 
    ERR_SYSTEM_QUEUE_FULL, 
    ERR_SYSTEM_FW_UPDATE_NEEDED, 
    ERR_SYSTEM_FW_RUNTIME_ERROR,
    ERR_SYSTEM_UNLOAD_MANUALLY,
    ERR_SYSTEM_FILAMENT_EJECTED
};

// @@TODO some of the strings are duplicates, can be merged into one     01234567890123456789
static const char MSG_TITLE_FINDA_DIDNT_TRIGGER[] PROGMEM_I1     = ISTR("FINDA DIDNT TRIGGER"); ////MSG_TITLE_FINDA_DIDNT_TRIGGER c=20
static const char MSG_TITLE_FINDA_DIDNT_GO_OFF[] PROGMEM_I1      = ISTR("FINDA: FILAM. STUCK"); ////MSG_TITLE_FINDA_DIDNT_GO_OFF c=20
static const char MSG_TITLE_FSENSOR_DIDNT_TRIGGER[] PROGMEM_I1   = ISTR("FSENSOR DIDNT TRIGG."); ////MSG_TITLE_FSENSOR_DIDNT_TRIGGER c=20
static const char MSG_TITLE_FSENSOR_DIDNT_GO_OFF[] PROGMEM_I1    = ISTR("FSENSOR: FIL. STUCK"); ////MSG_TITLE_FSENSOR_DIDNT_GO_OFF c=20
static const char MSG_TITLE_PULLEY_CANNOT_MOVE[] PROGMEM_I1      = ISTR("PULLEY CANNOT MOVE"); ////MSG_TITLE_PULLEY_CANNOT_MOVE c=20
static const char MSG_TITLE_FSENSOR_TOO_EARLY[] PROGMEM_I1       = ISTR("FSENSOR TOO EARLY"); ////MSG_TITLE_FSENSOR_TOO_EARLY c=20
static const char MSG_TITLE_INSPECT_FINDA[] PROGMEM_I1           = ISTR("INSPECT FINDA"); ////MSG_TITLE_INSPECT_FINDA c=20
static const char MSG_TITLE_LOAD_TO_EXTRUDER_FAILED[] PROGMEM_I1 = ISTR("LOAD TO EXTR. FAILED"); ////MSG_TITLE_LOAD_TO_EXTRUDER_FAILED c=20
static const char MSG_TITLE_SELECTOR_CANNOT_MOVE[] PROGMEM_I1    = ISTR("SELECTOR CANNOT MOVE"); ////MSG_TITLE_SELECTOR_CANNOT_MOVE c=20
static const char MSG_TITLE_SELECTOR_CANNOT_HOME[] PROGMEM_I1    = ISTR("SELECTOR CANNOT HOME"); ////MSG_TITLE_SELECTOR_CANNOT_HOME c=20
static const char MSG_TITLE_IDLER_CANNOT_MOVE[] PROGMEM_I1       = ISTR("IDLER CANNOT MOVE"); ////MSG_TITLE_IDLER_CANNOT_MOVE c=20
static const char MSG_TITLE_IDLER_CANNOT_HOME[] PROGMEM_I1       = ISTR("IDLER CANNOT HOME"); ////MSG_TITLE_IDLER_CANNOT_HOME c=20
static const char MSG_TITLE_TMC_WARNING_TMC_TOO_HOT[] PROGMEM_I1 = ISTR("WARNING TMC TOO HOT"); ////MSG_TITLE_TMC_WARNING_TMC_TOO_HOT c=20
//static const char MSG_TITLE_TMC_WARNING_TMC_TOO_HOT[] PROGMEM_I1 = ISTR("WARNING TMC TOO HOT"); ////MSG_TITLE_TMC_WARNING_TMC_TOO_HOT c=20
//static const char MSG_TITLE_TMC_WARNING_TMC_TOO_HOT[] PROGMEM_I1 = ISTR("WARNING TMC TOO HOT");
static const char MSG_TITLE_TMC_OVERHEAT_ERROR[] PROGMEM_I1      = ISTR("TMC OVERHEAT ERROR"); ////MSG_TITLE_TMC_OVERHEAT_ERROR c=20
//static const char MSG_TITLE_TMC_OVERHEAT_ERROR[] PROGMEM_I1 = ISTR("TMC OVERHEAT ERROR");
//static const char MSG_TITLE_TMC_OVERHEAT_ERROR[] PROGMEM_I1 = ISTR("TMC OVERHEAT ERROR");
static const char MSG_TITLE_TMC_DRIVER_ERROR[] PROGMEM_I1        = ISTR("TMC DRIVER ERROR"); ////MSG_TITLE_TMC_DRIVER_ERROR c=20
//static const char MSG_TITLE_TMC_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC DRIVER ERROR");
//static const char MSG_TITLE_TMC_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC DRIVER ERROR");
static const char MSG_TITLE_TMC_DRIVER_RESET[] PROGMEM_I1        = ISTR("TMC DRIVER RESET"); ////MSG_TITLE_TMC_DRIVER_RESET c=20
//static const char MSG_TITLE_TMC_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC DRIVER RESET");
//static const char MSG_TITLE_TMC_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC DRIVER RESET");
static const char MSG_TITLE_TMC_UNDERVOLTAGE_ERROR[] PROGMEM_I1  = ISTR("TMC UNDERVOLTAGE ERR"); ////MSG_TITLE_TMC_UNDERVOLTAGE_ERROR c=20
//static const char MSG_TITLE_TMC_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("TMC UNDERVOLTAGE ERR");
//static const char MSG_TITLE_TMC_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("TMC UNDERVOLTAGE ERR");
static const char MSG_TITLE_TMC_DRIVER_SHORTED[] PROGMEM_I1      = ISTR("TMC DRIVER SHORTED"); ////MSG_TITLE_TMC_DRIVER_SHORTED c=20
//static const char MSG_TITLE_TMC_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("TMC DRIVER SHORTED");
//static const char MSG_TITLE_TMC_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("TMC DRIVER SHORTED");
static const char MSG_TITLE_SELFTEST_FAILED[] PROGMEM_I1      = ISTR("MMU SELFTEST FAILED"); ////MSG_TITLE_SELFTEST_FAILED c=20
static const char MSG_TITLE_MMU_NOT_RESPONDING[] PROGMEM_I1      = ISTR("MMU NOT RESPONDING"); ////MSG_TITLE_MMU_NOT_RESPONDING c=20
static const char MSG_TITLE_COMMUNICATION_ERROR[] PROGMEM_I1     = ISTR("COMMUNICATION ERROR"); ////MSG_TITLE_COMMUNICATION_ERROR c=20
static const char MSG_TITLE_FIL_ALREADY_LOADED[] PROGMEM_I1      = ISTR("FILAMENT ALREADY LOA"); ////MSG_TITLE_FIL_ALREADY_LOADED c=20
static const char MSG_TITLE_INVALID_TOOL[] PROGMEM_I1            = ISTR("INVALID TOOL"); ////MSG_TITLE_INVALID_TOOL c=20
static const char MSG_TITLE_QUEUE_FULL[] PROGMEM_I1              = ISTR("QUEUE FULL"); ////MSG_TITLE_QUEUE_FULL c=20
static const char MSG_TITLE_FW_UPDATE_NEEDED[] PROGMEM_I1        = ISTR("MMU FW UPDATE NEEDED"); ////MSG_TITLE_FW_UPDATE_NEEDED c=20
static const char MSG_TITLE_FW_RUNTIME_ERROR[] PROGMEM_I1        = ISTR("FW RUNTIME ERROR"); ////MSG_TITLE_FW_RUNTIME_ERROR c=20
static const char MSG_TITLE_UNLOAD_MANUALLY[] PROGMEM_I1         = ISTR("UNLOAD MANUALLY"); ////MSG_TITLE_UNLOAD_MANUALLY c=20
static const char MSG_TITLE_FILAMENT_EJECTED[] PROGMEM_I1        = ISTR("FILAMENT EJECTED"); ////MSG_TITLE_FILAMENT_EJECTED c=20

static const char * const errorTitles [] PROGMEM = {
    _R(MSG_TITLE_FINDA_DIDNT_TRIGGER),
    _R(MSG_TITLE_FINDA_DIDNT_GO_OFF),
    _R(MSG_TITLE_FSENSOR_DIDNT_TRIGGER),
    _R(MSG_TITLE_FSENSOR_DIDNT_GO_OFF),
    _R(MSG_TITLE_PULLEY_CANNOT_MOVE),
    _R(MSG_TITLE_FSENSOR_TOO_EARLY),
    _R(MSG_TITLE_INSPECT_FINDA),
    _R(MSG_TITLE_LOAD_TO_EXTRUDER_FAILED),
    _R(MSG_TITLE_SELECTOR_CANNOT_HOME),
    _R(MSG_TITLE_SELECTOR_CANNOT_MOVE),
    _R(MSG_TITLE_IDLER_CANNOT_HOME),
    _R(MSG_TITLE_IDLER_CANNOT_MOVE),
    _R(MSG_TITLE_TMC_WARNING_TMC_TOO_HOT),
    _R(MSG_TITLE_TMC_WARNING_TMC_TOO_HOT),
    _R(MSG_TITLE_TMC_WARNING_TMC_TOO_HOT),
    _R(MSG_TITLE_TMC_OVERHEAT_ERROR),
    _R(MSG_TITLE_TMC_OVERHEAT_ERROR),
    _R(MSG_TITLE_TMC_OVERHEAT_ERROR),
    _R(MSG_TITLE_TMC_DRIVER_ERROR),
    _R(MSG_TITLE_TMC_DRIVER_ERROR),
    _R(MSG_TITLE_TMC_DRIVER_ERROR),
    _R(MSG_TITLE_TMC_DRIVER_RESET),
    _R(MSG_TITLE_TMC_DRIVER_RESET),
    _R(MSG_TITLE_TMC_DRIVER_RESET),
    _R(MSG_TITLE_TMC_UNDERVOLTAGE_ERROR),
    _R(MSG_TITLE_TMC_UNDERVOLTAGE_ERROR),
    _R(MSG_TITLE_TMC_UNDERVOLTAGE_ERROR),
    _R(MSG_TITLE_TMC_DRIVER_SHORTED),
    _R(MSG_TITLE_TMC_DRIVER_SHORTED),
    _R(MSG_TITLE_TMC_DRIVER_SHORTED),
    _R(MSG_TITLE_SELFTEST_FAILED),
    _R(MSG_TITLE_SELFTEST_FAILED),
    _R(MSG_TITLE_SELFTEST_FAILED),
    _R(MSG_TITLE_MMU_NOT_RESPONDING),
    _R(MSG_TITLE_COMMUNICATION_ERROR),
    _R(MSG_TITLE_FIL_ALREADY_LOADED),
    _R(MSG_TITLE_INVALID_TOOL),
    _R(MSG_TITLE_QUEUE_FULL),
    _R(MSG_TITLE_FW_UPDATE_NEEDED),
    _R(MSG_TITLE_FW_RUNTIME_ERROR),
    _R(MSG_TITLE_UNLOAD_MANUALLY),
    _R(MSG_TITLE_FILAMENT_EJECTED)
};

// @@TODO looking at the texts, they can be composed of several parts and/or parametrized (could save a lot of space ;) )
// Moreover, some of them have been disabled in favour of saving some more code size.
static const char MSG_DESC_FINDA_DIDNT_TRIGGER[] PROGMEM_I1 = ISTR("FINDA didn't trigger while loading the filament. Ensure the filament can move and FINDA works."); ////MSG_DESC_FINDA_DIDNT_TRIGGER c=20 r=8
static const char MSG_DESC_FINDA_DIDNT_GO_OFF[] PROGMEM_I1 = ISTR("FINDA didn't switch off while unloading filament. Try unloading manually. Ensure filament can move and FINDA works."); ////MSG_DESC_FINDA_DIDNT_GO_OFF c=20 r=8
static const char MSG_DESC_FSENSOR_DIDNT_TRIGGER[] PROGMEM_I1 = ISTR("Filament sensor didn't trigger while loading the filament. Ensure the filament reached the fsensor and the sensor works."); ////MSG_DESC_FSENSOR_DIDNT_TRIGGER c=20 r=8
static const char MSG_DESC_FSENSOR_DIDNT_GO_OFF[] PROGMEM_I1 = ISTR("Filament sensor didn't switch off while unloading filament. Ensure filament can move and the sensor works."); ////MSG_DESC_FSENSOR_DIDNT_GO_OFF c=20 r=8
static const char MSG_DESC_PULLEY_STALLED[] PROGMEM_I1 = ISTR("Pulley motor stalled. Ensure the pulley can move and check the wiring."); ////MSG_DESC_PULLEY_STALLED c=20 r=8
static const char MSG_DESC_FSENSOR_TOO_EARLY[] PROGMEM_I1 = ISTR("Filament sensor triggered too early while loading to extruder. Check there isn't anything stuck in PTFE tube. Check that sensor reads properly."); ////MSG_DESC_FSENSOR_TOO_EARLY c=20 r=8
static const char MSG_DESC_INSPECT_FINDA[] PROGMEM_I1 = ISTR("Selector can't move due to FINDA detecting a filament. Make sure no filament is in selector and FINDA works properly."); ////MSG_DESC_INSPECT_FINDA c=20 r=8
static const char MSG_DESC_LOAD_TO_EXTRUDER_FAILED[] PROGMEM_I1 = ISTR("Loading to extruder failed. Inspect the filament tip shape. Refine the sensor calibration, if needed."); ////MSG_DESC_LOAD_TO_EXTRUDER_FAILED c=20 r=8
static const char MSG_DESC_SELECTOR_CANNOT_HOME[] PROGMEM_I1 = ISTR("The Selector cannot home properly. Check for anything blocking its movement."); ////MSG_DESC_SELECTOR_CANNOT_HOME c=20 r=8
static const char MSG_DESC_CANNOT_MOVE[] PROGMEM_I1 = ISTR("Can't move Selector or Idler."); /////MSG_DESC_CANNOT_MOVE c=20 r=4
//static const char MSG_DESC_SELECTOR_CANNOT_MOVE[] PROGMEM_I1 = ISTR("The Selector cannot move. Check for anything blocking its movement. Check the wiring is correct.");
static const char MSG_DESC_IDLER_CANNOT_HOME[] PROGMEM_I1 = ISTR("The Idler cannot home properly. Check for anything blocking its movement."); ////MSG_DESC_IDLER_CANNOT_HOME c=20 r=8
//static const char MSG_DESC_IDLER_CANNOT_MOVE[] PROGMEM_I1 = ISTR("The Idler cannot move properly. Check for anything blocking its movement. Check the wiring is correct.");
static const char MSG_DESC_TMC[] PROGMEM_I1 = ISTR("More details online."); ////MSG_DESC_TMC c=20 r=8
//static const char MSG_DESC_PULLEY_WARNING_TMC_TOO_HOT[] PROGMEM_I1 = ISTR("TMC driver for the Pulley motor is almost overheating. Make sure there is sufficient airflow near the MMU board.");
//static const char MSG_DESC_SELECTOR_WARNING_TMC_TOO_HOT[] PROGMEM_I1 = ISTR("TMC driver for the Selector motor is almost overheating. Make sure there is sufficient airflow near the MMU board.");
//static const char MSG_DESC_IDLER_WARNING_TMC_TOO_HOT[] PROGMEM_I1 = ISTR("TMC driver for the Idler motor is almost overheating. Make sure there is sufficient airflow near the MMU board.");
//static const char MSG_DESC_PULLEY_TMC_OVERHEAT_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Pulley motor is overheated. Cool down the MMU board and reset MMU.");
//static const char MSG_DESC_SELECTOR_TMC_OVERHEAT_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Selector motor is overheated. Cool down the MMU board and reset MMU.");
//static const char MSG_DESC_IDLER_TMC_OVERHEAT_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Idler motor is overheated. Cool down the MMU board and reset MMU.");
//static const char MSG_DESC_PULLEY_TMC_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Pulley motor is not responding. Try resetting the MMU. If the issue persists contact support.");
//static const char MSG_DESC_SELECTOR_TMC_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Selector motor is not responding. Try resetting the MMU. If the issue persists contact support.");
//static const char MSG_DESC_IDLER_TMC_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Idler motor is not responding. Try resetting the MMU. If the issue persists contact support.");
//static const char MSG_DESC_PULLEY_TMC_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC driver for the Pulley motor was restarted. There is probably an issue with the electronics. Check the wiring and connectors.");
//static const char MSG_DESC_SELECTOR_TMC_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC driver for the Selector motor was restarted. There is probably an issue with the electronics. Check the wiring and connectors.");
//static const char MSG_DESC_IDLER_TMC_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC driver for the Idler motor was restarted. There is probably an issue with the electronics. Check the wiring and connectors.");
//static const char MSG_DESC_PULLEY_TMC_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("Not enough current for the Pulley TMC driver. There is probably an issue with the electronics. Check the wiring and connectors.");
//static const char MSG_DESC_SELECTOR_TMC_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("Not enough current for the Selector TMC driver. There is probably an issue with the electronics. Check the wiring and connectors.");
//static const char MSG_DESC_IDLER_TMC_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("Not enough current for the Idler TMC driver. There is probably an issue with the electronics. Check the wiring and connectors.");
//static const char MSG_DESC_PULLEY_TMC_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("Short circuit on the Pulley TMC driver. Check the wiring and connectors. If the issue persists contact support.");
//static const char MSG_DESC_SELECTOR_TMC_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("Short circuit on the Selector TMC driver. Check the wiring and connectors. If the issue persists contact support.");
//static const char MSG_DESC_IDLER_TMC_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("Short circuit on the Idler TMC driver. Check the wiring and connectors. If the issue persists contact support.");
static const char MSG_DESC_MMU_NOT_RESPONDING[] PROGMEM_I1 = ISTR("MMU unit not responding. Check the wiring and connectors. If the issue persists, contact support."); ////MSG_DESC_MMU_NOT_RESPONDING c=20 r=8
static const char MSG_DESC_COMMUNICATION_ERROR[] PROGMEM_I1 = ISTR("MMU unit not responding correctly. Check the wiring and connectors. If the issue persists, contact support."); ////MSG_DESC_COMMUNICATION_ERROR c=20 r=9
static const char MSG_DESC_FILAMENT_ALREADY_LOADED[] PROGMEM_I1 = ISTR("Cannot perform the action, filament is already loaded. Unload it first."); ////MSG_DESC_FILAMENT_ALREADY_LOADED c=20 r=8
static const char MSG_DESC_INVALID_TOOL[] PROGMEM_I1 = ISTR("Requested filament tool is not available on this hardware. Check the G-code for tool index out of range (T0-T4)."); ////MSG_DESC_INVALID_TOOL c=20 r=8
static const char MSG_DESC_QUEUE_FULL[] PROGMEM_I1 = ISTR("MMU Firmware internal error, please reset the MMU."); ////MSG_DESC_QUEUE_FULL c=20 r=8
static const char MSG_DESC_FW_RUNTIME_ERROR[] PROGMEM_I1 = ISTR("Internal runtime error. Try resetting the MMU unit or updating the firmware. If the issue persists, contact support."); ////MSG_DESC_FW_RUNTIME_ERROR c=20 r=11
static const char MSG_DESC_UNLOAD_MANUALLY[] PROGMEM_I1 = ISTR("Filament detected unexpectedly. Ensure no filament is loaded. Check the sensors and wiring."); ////MSG_DESC_UNLOAD_MANUALLY c=20 r=8
static const char MSG_DESC_FILAMENT_EJECTED[] PROGMEM_I1 = ISTR("Remove the ejected filament from the front of the MMU unit."); ////MSG_DESC_FILAMENT_EJECTED c=20 r=8

// Read explanation in mmu2_protocol_logic.cpp -> supportedMmuFWVersion
static constexpr char MSG_DESC_FW_UPDATE_NEEDED[] PROGMEM_I1 = ISTR("The MMU unit firmware version incompatible with the printer's FW. Update to version 2.1.8."); ////MSG_DESC_FW_UPDATE_NEEDED c=20 r=9
static constexpr uint8_t szFWUN = sizeof(MSG_DESC_FW_UPDATE_NEEDED);
// at least check the individual version characters in MSG_DESC_FW_UPDATE_NEEDED
static_assert(MSG_DESC_FW_UPDATE_NEEDED[szFWUN - 7] == ('0' + mmuVersionMajor));
static_assert(MSG_DESC_FW_UPDATE_NEEDED[szFWUN - 5] == ('0' + mmuVersionMinor));
static_assert(MSG_DESC_FW_UPDATE_NEEDED[szFWUN - 3] == ('0' + mmuVersionPatch));

static const char * const errorDescs[] PROGMEM = {
    _R(MSG_DESC_FINDA_DIDNT_TRIGGER),
    _R(MSG_DESC_FINDA_DIDNT_GO_OFF),
    _R(MSG_DESC_FSENSOR_DIDNT_TRIGGER),
    _R(MSG_DESC_FSENSOR_DIDNT_GO_OFF),
    _R(MSG_DESC_PULLEY_STALLED),
    _R(MSG_DESC_FSENSOR_TOO_EARLY),
    _R(MSG_DESC_INSPECT_FINDA),
    _R(MSG_DESC_LOAD_TO_EXTRUDER_FAILED),
    _R(MSG_DESC_SELECTOR_CANNOT_HOME),
    _R(MSG_DESC_CANNOT_MOVE),
    _R(MSG_DESC_IDLER_CANNOT_HOME),
    _R(MSG_DESC_CANNOT_MOVE),
    _R(MSG_DESC_TMC), // descPULLEY_WARNING_TMC_TOO_HOT
    _R(MSG_DESC_TMC), // descSELECTOR_WARNING_TMC_TOO_HOT
    _R(MSG_DESC_TMC), // descIDLER_WARNING_TMC_TOO_HOT
    _R(MSG_DESC_TMC), // descPULLEY_TMC_OVERHEAT_ERROR
    _R(MSG_DESC_TMC), // descSELECTOR_TMC_OVERHEAT_ERROR
    _R(MSG_DESC_TMC), // descIDLER_TMC_OVERHEAT_ERROR
    _R(MSG_DESC_TMC), // descPULLEY_TMC_DRIVER_ERROR
    _R(MSG_DESC_TMC), // descSELECTOR_TMC_DRIVER_ERROR
    _R(MSG_DESC_TMC), // descIDLER_TMC_DRIVER_ERROR
    _R(MSG_DESC_TMC), // descPULLEY_TMC_DRIVER_RESET
    _R(MSG_DESC_TMC), // descSELECTOR_TMC_DRIVER_RESET
    _R(MSG_DESC_TMC), // descIDLER_TMC_DRIVER_RESET
    _R(MSG_DESC_TMC), // descPULLEY_TMC_UNDERVOLTAGE_ERROR
    _R(MSG_DESC_TMC), // descSELECTOR_TMC_UNDERVOLTAGE_ERROR
    _R(MSG_DESC_TMC), // descIDLER_TMC_UNDERVOLTAGE_ERROR
    _R(MSG_DESC_TMC), // descPULLEY_TMC_DRIVER_SHORTED
    _R(MSG_DESC_TMC), // descSELECTOR_TMC_DRIVER_SHORTED
    _R(MSG_DESC_TMC), // descIDLER_TMC_DRIVER_SHORTED
    _R(MSG_DESC_TMC), // descPULLEY_SELFTEST_FAILED
    _R(MSG_DESC_TMC), // descSELECTOR_SELFTEST_FAILED
    _R(MSG_DESC_TMC), // descIDLER_SELFTEST_FAILED
    _R(MSG_DESC_MMU_NOT_RESPONDING),
    _R(MSG_DESC_COMMUNICATION_ERROR),
    _R(MSG_DESC_FILAMENT_ALREADY_LOADED),
    _R(MSG_DESC_INVALID_TOOL),
    _R(MSG_DESC_QUEUE_FULL),
    _R(MSG_DESC_FW_UPDATE_NEEDED),
    _R(MSG_DESC_FW_RUNTIME_ERROR),
    _R(MSG_DESC_UNLOAD_MANUALLY),
    _R(MSG_DESC_FILAMENT_EJECTED)
};

// we have max 3 buttons/operations to select from
// one of them is "More" to show the explanation text normally hidden in the next screens.
// It is displayed with W (Double down arrow, special character from CGRAM)
// 01234567890123456789
// >bttxt   >bttxt   >W
// Therefore at least some of the buttons, which can occur on the screen together, can only be 8-chars long max @@TODO.
// Beware - we only have space for 2 buttons on the LCD while the MMU has 3 buttons
// -> the left button on the MMU is not used/rendered on the LCD (it is also almost unused on the MMU side)
static const char MSG_BTN_RETRY[] PROGMEM_I1 = ISTR("Retry"); ////MSG_BTN_RETRY c=8
static const char MSG_BTN_CONTINUE[] PROGMEM_I1 = ISTR("Done"); ////MSG_BTN_CONTINUE c=8
static const char MSG_BTN_RESTART_MMU[] PROGMEM_I1 = ISTR("RstMMU"); ////MSG_BTN_RESTART_MMU c=8
static const char MSG_BTN_UNLOAD[] PROGMEM_I1 = ISTR("Unload"); ////MSG_BTN_UNLOAD c=8
static const char MSG_BTN_STOP[] PROGMEM_I1 = ISTR("Stop"); ////MSG_BTN_STOP c=8
static const char MSG_BTN_DISABLE_MMU[] PROGMEM_I1 = ISTR("Disable"); ////MSG_BTN_DISABLE_MMU c=8
static const char MSG_BTN_MORE[] PROGMEM_I1 = ISTR("\x06"); ////MSG_BTN_MORE c=8 //@todo convert to PROGMEM_N1

// Used to parse the buttons from Btns().
static const char * const btnOperation[] PROGMEM = {
    _R(MSG_BTN_RETRY),
    _R(MSG_BTN_CONTINUE),
    _R(MSG_BTN_RESTART_MMU),
    _R(MSG_BTN_UNLOAD),
    _R(MSG_BTN_STOP),
    _R(MSG_BTN_DISABLE_MMU),
};

// We have 8 different operations/buttons at this time, so we need at least 4 bits to encode each.
// Since one of the buttons is always "More", we can skip that one.
// Therefore we need just 1 byte to describe the necessary buttons for each screen.
uint8_t constexpr Btns(ButtonOperations bMiddle, ButtonOperations bRight){
    return ((uint8_t)bRight) << 4 | ((uint8_t)bMiddle);
}

static const uint8_t errorButtons[] PROGMEM = {
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//FINDA_DIDNT_TRIGGER
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//FINDA_DIDNT_GO_OFF
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//FSENSOR_DIDNT_TRIGGER
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//FSENSOR_DIDNT_GO_OFF

    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//PULLEY_STALLED
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//FSENSOR_TOO_EARLY
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//INSPECT_FINDA
    Btns(ButtonOperations::Continue, ButtonOperations::NoOperation),//LOAD_TO_EXTRUDER_FAILED
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//SELECTOR_CANNOT_HOME
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//SELECTOR_CANNOT_MOVE
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//IDLER_CANNOT_HOME
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//IDLER_CANNOT_MOVE

    Btns(ButtonOperations::Continue, ButtonOperations::RestartMMU),//PULLEY_WARNING_TMC_TOO_HOT
    Btns(ButtonOperations::Continue, ButtonOperations::RestartMMU),//SELECTOR_WARNING_TMC_TOO_HOT
    Btns(ButtonOperations::Continue, ButtonOperations::RestartMMU),//IDLER_WARNING_TMC_TOO_HOT

    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//PULLEY_TMC_OVERHEAT_ERROR
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//SELECTOR_TMC_OVERHEAT_ERROR
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//IDLER_TMC_OVERHEAT_ERROR
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//PULLEY_TMC_DRIVER_ERROR
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//SELECTOR_TMC_DRIVER_ERROR
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//IDLER_TMC_DRIVER_ERROR
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//PULLEY_TMC_DRIVER_RESET
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//SELECTOR_TMC_DRIVER_RESET
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//IDLER_TMC_DRIVER_RESET
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//PULLEY_TMC_UNDERVOLTAGE_ERROR
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//SELECTOR_TMC_UNDERVOLTAGE_ERROR
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//IDLER_TMC_UNDERVOLTAGE_ERROR
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//PULLEY_TMC_DRIVER_SHORTED
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//SELECTOR_TMC_DRIVER_SHORTED
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//IDLER_TMC_DRIVER_SHORTED
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//PULLEY_SELFTEST_FAILED
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//SELECTOR_SELFTEST_FAILED
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//IDLER_SELFTEST_FAILED
    Btns(ButtonOperations::RestartMMU, ButtonOperations::DisableMMU),//MMU_NOT_RESPONDING
    Btns(ButtonOperations::RestartMMU, ButtonOperations::DisableMMU),//COMMUNICATION_ERROR

    Btns(ButtonOperations::Unload, ButtonOperations::Continue),//FILAMENT_ALREADY_LOADED
    Btns(ButtonOperations::StopPrint, ButtonOperations::RestartMMU),//INVALID_TOOL
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//QUEUE_FULL
    Btns(ButtonOperations::RestartMMU, ButtonOperations::DisableMMU),//FW_UPDATE_NEEDED
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//FW_RUNTIME_ERROR
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//UNLOAD_MANUALLY
    Btns(ButtonOperations::Continue, ButtonOperations::NoOperation),//FILAMENT_EJECTED
};

static_assert( sizeof(errorCodes) / sizeof(errorCodes[0]) == sizeof(errorDescs) / sizeof (errorDescs[0]));
static_assert( sizeof(errorCodes) / sizeof(errorCodes[0]) == sizeof(errorTitles) / sizeof (errorTitles[0]));
static_assert( sizeof(errorCodes) / sizeof(errorCodes[0]) == sizeof(errorButtons) / sizeof (errorButtons[0]));

} // namespace MMU2
