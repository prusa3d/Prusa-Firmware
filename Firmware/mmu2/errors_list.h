// Extracted from Prusa-Error-Codes repo
// Subject to automation and optimization
#pragma once
#include "inttypes.h"
#include "../language.h"
#include <avr/pgmspace.h>

namespace MMU2 {

static constexpr uint8_t ERR_MMU_CODE = 4;

typedef enum : uint16_t {
    ERR_UNDEF = 0,

    ERR_MECHANICAL = 100,
    ERR_MECHANICAL_FINDA_DIDNT_TRIGGER,
    ERR_MECHANICAL_FINDA_DIDNT_SWITCH_OFF,
    ERR_MECHANICAL_FSENSOR_DIDNT_TRIGGER,
    ERR_MECHANICAL_FSENSOR_DIDNT_SWITCH_OFF,

    ERR_MECHANICAL_PULLEY_STALLED = 105,
    ERR_MECHANICAL_SELECTOR_CANNOT_HOME = 115,
    ERR_MECHANICAL_IDLER_CANNOT_HOME = 125,


    ERR_TEMPERATURE = 200,
    ERR_TEMPERATURE_TMC_PULLEY_OVER_TEMPERATURE_WARN = 201,
    ERR_TEMPERATURE_TMC_SELECTOR_OVER_TEMPERATURE_WARN = 211,
    ERR_TEMPERATURE_TMC_IDLER_OVER_TEMPERATURE_WARN = 221,

    ERR_TEMPERATURE_TMC_PULLEY_OVER_TEMPERATURE_ERROR = 202,
    ERR_TEMPERATURE_TMC_SELECTOR_OVER_TEMPERATURE_ERROR = 212,
    ERR_TEMPERATURE_TMC_IDLER_OVER_TEMPERATURE_ERROR = 222,


    ERR_ELECTRICAL = 300,
    ERR_ELECTRICAL_TMC_PULLEY_DRIVER_ERROR = 301,
    ERR_ELECTRICAL_TMC_SELECTOR_DRIVER_ERROR = 311,
    ERR_ELECTRICAL_TMC_IDLER_DRIVER_ERROR = 321,

    ERR_ELECTRICAL_TMC_PULLEY_DRIVER_RESET = 302,
    ERR_ELECTRICAL_TMC_SELECTOR_DRIVER_RESET = 312,
    ERR_ELECTRICAL_TMC_IDLER_DRIVER_RESET = 322,

    ERR_ELECTRICAL_TMC_PULLEY_UNDERVOLTAGE_ERROR = 303,
    ERR_ELECTRICAL_TMC_SELECTOR_UNDERVOLTAGE_ERROR = 313,
    ERR_ELECTRICAL_TMC_IDLER_UNDERVOLTAGE_ERROR = 323,

    ERR_ELECTRICAL_TMC_PULLEY_DRIVER_SHORTED = 304,
    ERR_ELECTRICAL_TMC_SELECTOR_DRIVER_SHORTED = 314,
    ERR_ELECTRICAL_TMC_IDLER_DRIVER_SHORTED = 324,


    ERR_CONNECT = 400,
    ERR_CONNECT_MMU_NOT_RESPONDING = 401,
    ERR_CONNECT_COMMUNICATION_ERROR = 402,


    ERR_SYSTEM = 500,
    ERR_SYSTEM_FILAMENT_ALREADY_LOADED = 501,
    ERR_SYSTEM_INVALID_TOOL = 502,
    ERR_SYSTEM_QUEUE_FULL = 503,
    ERR_SYSTEM_VERSION_MISMATCH = 504,
    ERR_SYSTEM_RUNTIME_ERROR = 505,

    ERR_OTHER = 900
} err_num_t;

// Avr gcc has serious trouble understanding static data structures in PROGMEM
// and inadvertedly falls back to copying the whole structure into RAM (which is obviously unwanted).
// But since this file ought to be generated in the future from yaml prescription,
// it really makes no difference if there are "nice" data structures or plain arrays.
static const uint16_t errorCodes[] PROGMEM = {
    ERR_MECHANICAL_FINDA_DIDNT_TRIGGER,
    ERR_MECHANICAL_FINDA_DIDNT_SWITCH_OFF,
    ERR_MECHANICAL_FSENSOR_DIDNT_TRIGGER,
    ERR_MECHANICAL_FSENSOR_DIDNT_SWITCH_OFF,
    ERR_MECHANICAL_PULLEY_STALLED,
    ERR_MECHANICAL_SELECTOR_CANNOT_HOME,
    ERR_MECHANICAL_IDLER_CANNOT_HOME,
    ERR_TEMPERATURE_TMC_PULLEY_OVER_TEMPERATURE_WARN,
    ERR_TEMPERATURE_TMC_SELECTOR_OVER_TEMPERATURE_WARN,
    ERR_TEMPERATURE_TMC_IDLER_OVER_TEMPERATURE_WARN,
    ERR_TEMPERATURE_TMC_PULLEY_OVER_TEMPERATURE_ERROR,
    ERR_TEMPERATURE_TMC_SELECTOR_OVER_TEMPERATURE_ERROR,
    ERR_TEMPERATURE_TMC_IDLER_OVER_TEMPERATURE_ERROR,
    ERR_ELECTRICAL_TMC_PULLEY_DRIVER_ERROR,
    ERR_ELECTRICAL_TMC_SELECTOR_DRIVER_ERROR,
    ERR_ELECTRICAL_TMC_IDLER_DRIVER_ERROR,
    ERR_ELECTRICAL_TMC_PULLEY_DRIVER_RESET,
    ERR_ELECTRICAL_TMC_SELECTOR_DRIVER_RESET,
    ERR_ELECTRICAL_TMC_IDLER_DRIVER_RESET,
    ERR_ELECTRICAL_TMC_PULLEY_UNDERVOLTAGE_ERROR,
    ERR_ELECTRICAL_TMC_SELECTOR_UNDERVOLTAGE_ERROR,
    ERR_ELECTRICAL_TMC_IDLER_UNDERVOLTAGE_ERROR,
    ERR_ELECTRICAL_TMC_PULLEY_DRIVER_SHORTED,
    ERR_ELECTRICAL_TMC_SELECTOR_DRIVER_SHORTED,
    ERR_ELECTRICAL_TMC_IDLER_DRIVER_SHORTED,
    ERR_CONNECT_MMU_NOT_RESPONDING,
    ERR_CONNECT_COMMUNICATION_ERROR,
    ERR_SYSTEM_FILAMENT_ALREADY_LOADED, 
    ERR_SYSTEM_INVALID_TOOL, 
    ERR_SYSTEM_QUEUE_FULL, 
    ERR_SYSTEM_VERSION_MISMATCH, 
    ERR_SYSTEM_RUNTIME_ERROR,
};

// @@TODO some of the strings are duplicates, can be merged into one
static const char titleFINDA_DIDNT_TRIGGER[] PROGMEM_I1 = ISTR("FINDA NOT ON");
static const char titleFINDA_DIDNT_SWITCH_OFF[] PROGMEM_I1 = ISTR("FINDA NOT OFF");
static const char titleFSENSOR_DIDNT_TRIGGER[] PROGMEM_I1 = ISTR("FSENSOR NOT ON");
static const char titleFSENSOR_DIDNT_SWITCH_OFF[] PROGMEM_I1 = ISTR("FSENSOR NOT OFF");
static const char titlePULLEY_STALLED[] PROGMEM_I1 = ISTR("PULLEY STALLED");
static const char titleSELECTOR_CANNOT_HOME[] PROGMEM_I1 = ISTR("SELECTOR CAN'T HOME");
static const char titleIDLER_CANNOT_HOME[] PROGMEM_I1 = ISTR("IDLER CANNOT HOME");
static const char titleTMC_PULLEY_OVER_TEMPERATURE_WARN[] PROGMEM_I1 = ISTR("TMC TOO HOT");
static const char titleTMC_SELECTOR_OVER_TEMPERATURE_WARN[] PROGMEM_I1 = ISTR("TMC TOO HOT");
static const char titleTMC_IDLER_OVER_TEMPERATURE_WARN[] PROGMEM_I1 = ISTR("TMC TOO HOT");
static const char titleTMC_PULLEY_OVER_TEMPERATURE_ERROR[] PROGMEM_I1 = ISTR("TMC OVERHEAT ERROR");
static const char titleTMC_SELECTOR_OVER_TEMPERATURE_ERROR[] PROGMEM_I1 = ISTR("TMC OVERHEAT ERROR");
static const char titleTMC_IDLER_OVER_TEMPERATURE_ERROR[] PROGMEM_I1 = ISTR("TMC OVERHEAT ERROR");
static const char titleTMC_PULLEY_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC DRIVER ERROR");
static const char titleTMC_SELECTOR_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC DRIVER ERROR");
static const char titleTMC_IDLER_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC DRIVER ERROR");
static const char titleTMC_PULLEY_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC DRIVER RESET");
static const char titleTMC_SELECTOR_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC DRIVER RESET");
static const char titleTMC_IDLER_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC DRIVER RESET");
static const char titleTMC_PULLEY_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("TMC UNDERVOLT ERROR");
static const char titleTMC_SELECTOR_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("TMC UNDERVOLT ERROR");
static const char titleTMC_IDLER_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("TMC UNDERVOLT ERROR");
static const char titleTMC_PULLEY_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("TMC DRIVER SHORTED");
static const char titleTMC_SELECTOR_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("TMC DRIVER SHORTED");
static const char titleTMC_IDLER_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("TMC DRIVER SHORTED");
static const char titleMMU_NOT_RESPONDING[] PROGMEM_I1 = ISTR("MMU NOT RESPONDING");
static const char titleCOMMUNICATION_ERROR[] PROGMEM_I1 = ISTR("COMMUNICATION ERROR");
static const char titleFILAMENT_ALREADY_LOADED[] PROGMEM_I1 = ISTR("ALREADY LOADED"); 
static const char titleINVALID_TOOL[] PROGMEM_I1 = ISTR("INVALID TOOL"); 
static const char titleQUEUE_FULL[] PROGMEM_I1 = ISTR("QUEUE FULL"); 
static const char titleVERSION_MISMATCH[] PROGMEM_I1 = ISTR("FW VERSION MISMATCH"); 
static const char titleRUNTIME_ERROR[] PROGMEM_I1 = ISTR("FW RUNTIME ERROR");

static const char * const errorTitles [] PROGMEM = {
    titleFINDA_DIDNT_TRIGGER,
    titleFINDA_DIDNT_SWITCH_OFF,
    titleFSENSOR_DIDNT_TRIGGER,
    titleFSENSOR_DIDNT_SWITCH_OFF,
    titlePULLEY_STALLED,
    titleSELECTOR_CANNOT_HOME,
    titleIDLER_CANNOT_HOME,
    titleTMC_PULLEY_OVER_TEMPERATURE_WARN,
    titleTMC_SELECTOR_OVER_TEMPERATURE_WARN,
    titleTMC_IDLER_OVER_TEMPERATURE_WARN,
    titleTMC_PULLEY_OVER_TEMPERATURE_ERROR,
    titleTMC_SELECTOR_OVER_TEMPERATURE_ERROR,
    titleTMC_IDLER_OVER_TEMPERATURE_ERROR,
    titleTMC_PULLEY_DRIVER_ERROR,
    titleTMC_SELECTOR_DRIVER_ERROR,
    titleTMC_IDLER_DRIVER_ERROR,
    titleTMC_PULLEY_DRIVER_RESET,
    titleTMC_SELECTOR_DRIVER_RESET,
    titleTMC_IDLER_DRIVER_RESET,
    titleTMC_PULLEY_UNDERVOLTAGE_ERROR,
    titleTMC_SELECTOR_UNDERVOLTAGE_ERROR,
    titleTMC_IDLER_UNDERVOLTAGE_ERROR,
    titleTMC_PULLEY_DRIVER_SHORTED,
    titleTMC_SELECTOR_DRIVER_SHORTED,
    titleTMC_IDLER_DRIVER_SHORTED,
    titleMMU_NOT_RESPONDING,
    titleCOMMUNICATION_ERROR,
    titleFILAMENT_ALREADY_LOADED,
    titleINVALID_TOOL,
    titleQUEUE_FULL,
    titleVERSION_MISMATCH,
    titleRUNTIME_ERROR,
};

// @@TODO looking at the texts, they can be composed of several parts and/or parametrized (could save a lot of space ;) )
static const char descFINDA_DIDNT_TRIGGER[] PROGMEM_I1 = ISTR("FINDA didn't trigger while loading filament. Ensure that the steel FINDA ball can move freely and check the wiring.");
static const char descFINDA_DIDNT_SWITCH_OFF[] PROGMEM_I1 = ISTR("FINDA didn't switch off while unloading filament. Try unloading the filament manually and ensure steel FINDA ball can move freely.");
static const char descFSENSOR_DIDNT_TRIGGER[] PROGMEM_I1 = ISTR("Filament sensor didn't trigger while loading the filament. Check that the filament reached the fsensor and check the wiring.");
static const char descFSENSOR_DIDNT_SWITCH_OFF[] PROGMEM_I1 = ISTR("Filament sensor didn't switch off while unloading the filament. The filament is probably stuck near the sensor or the sensor is malfunctioning.");
static const char descPULLEY_STALLED[] PROGMEM_I1 = ISTR("The Pulley stalled - check for anything blocking the filament from being pushed/pulled to/from the extruder.");
static const char descSELECTOR_CANNOT_HOME[] PROGMEM_I1 = ISTR("The Selector cannot home properly - check for anything blocking its movement.");
static const char descIDLER_CANNOT_HOME[] PROGMEM_I1 = ISTR("The Idler cannot home properly - check for anything blocking its movement.");
static const char descTMC_PULLEY_OVER_TEMPERATURE_WARN[] PROGMEM_I1 = ISTR("TMC driver for the Pulley motor is almost overheated. Make sure there is sufficient airflow near the MMU board.");
static const char descTMC_SELECTOR_OVER_TEMPERATURE_WARN[] PROGMEM_I1 = ISTR("TMC driver for the Selector motor is almost overheated. Make sure there is sufficient airflow near the MMU board.");
static const char descTMC_IDLER_OVER_TEMPERATURE_WARN[] PROGMEM_I1 = ISTR("TMC driver for the Idler motor is almost overheated. Make sure there is sufficient airflow near the MMU board.");
static const char descTMC_PULLEY_OVER_TEMPERATURE_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Pulley motor is overheated. Cool down the MMU board and restart MMU.");
static const char descTMC_SELECTOR_OVER_TEMPERATURE_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Selector motor is overheated. Cool down the MMU board and restart MMU.");
static const char descTMC_IDLER_OVER_TEMPERATURE_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Idler motor is overheated. Cool down the MMU board and restart MMU.");
static const char descTMC_PULLEY_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Pulley motor is not responding. Try restarting the MMU. If the issue persist contact the support.");
static const char descTMC_SELECTOR_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Selector motor is not responding. Try restarting the MMU. If the issue persist contact the support.");
static const char descTMC_IDLER_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Idler motor is not responding. Try restarting the MMU. If the issue persist contact the support.");
static const char descTMC_PULLEY_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC driver for the Pulley motor was restarted. There is probably an issue with the electronics. Check the wiring and connectors.");
static const char descTMC_SELECTOR_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC driver for the Selector motor was restarted. There is probably an issue with the electronics. Check the wiring and connectors.");
static const char descTMC_IDLER_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC driver for the Idler motor was restarted. There is probably an issue with the electronics. Check the wiring and connectors.");
static const char descTMC_PULLEY_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("Not enough current for the Pulley TMC driver. There is probably an issue with the electronics. Check the wiring and connectors.");
static const char descTMC_SELECTOR_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("Not enough current for the Selector TMC driver. There is probably an issue with the electronics. Check the wiring and connectors.");
static const char descTMC_IDLER_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("Not enough current for the Idler TMC driver. There is probably an issue with the electronics. Check the wiring and connectors.");
static const char descTMC_PULLEY_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("Short circuit on the Pulley TMC driver. Check the wiring and connectors. If the issue persist contact the support.");
static const char descTMC_SELECTOR_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("Short circuit on the Selector TMC driver. Check the wiring and connectors. If the issue persist contact the support.");
static const char descTMC_IDLER_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("Short circuit on the Idler TMC driver. Check the wiring and connectors. If the issue persist contact the support.");
static const char descMMU_NOT_RESPONDING[] PROGMEM_I1 = ISTR("MMU unit not responding. Check the wiring and connectors. If the issue persist contact the support.");
static const char descCOMMUNICATION_ERROR[] PROGMEM_I1 = ISTR("MMU unit not responding correctly. Check the wiring and connectors. If the issue persist contact the support.");
static const char descFILAMENT_ALREADY_LOADED[] PROGMEM_I1 = ISTR("Cannot perform the action, filament is already loaded. Unload it first."); 
static const char descINVALID_TOOL[] PROGMEM_I1 = ISTR("Requested filament tool is not available on this hardware. Check the G-code file for possible issue."); 
static const char descQUEUE_FULL[] PROGMEM_I1 = ISTR("Internal runtime error of the firmware, please restart the MMU."); 
static const char descVERSION_MISMATCH[] PROGMEM_I1 = ISTR("The MMU unit reports its FW version incompatible with the printer's firmware. Make sure the MMU firmware is up to date."); 
static const char descRUNTIME_ERROR[] PROGMEM_I1 = ISTR("Internal runtime error. Try restarting the MMU unit or updating the firmware. If the issue persist contact the support.");

static const char * const errorDescs[] PROGMEM = {
    descFINDA_DIDNT_TRIGGER,
    descFINDA_DIDNT_SWITCH_OFF,
    descFSENSOR_DIDNT_TRIGGER,
    descFSENSOR_DIDNT_SWITCH_OFF,
    descPULLEY_STALLED,
    descSELECTOR_CANNOT_HOME,
    descIDLER_CANNOT_HOME,
    descTMC_PULLEY_OVER_TEMPERATURE_WARN,
    descTMC_SELECTOR_OVER_TEMPERATURE_WARN,
    descTMC_IDLER_OVER_TEMPERATURE_WARN,
    descTMC_PULLEY_OVER_TEMPERATURE_ERROR,
    descTMC_SELECTOR_OVER_TEMPERATURE_ERROR,
    descTMC_IDLER_OVER_TEMPERATURE_ERROR,
    descTMC_PULLEY_DRIVER_ERROR,
    descTMC_SELECTOR_DRIVER_ERROR,
    descTMC_IDLER_DRIVER_ERROR,
    descTMC_PULLEY_DRIVER_RESET,
    descTMC_SELECTOR_DRIVER_RESET,
    descTMC_IDLER_DRIVER_RESET,
    descTMC_PULLEY_UNDERVOLTAGE_ERROR,
    descTMC_SELECTOR_UNDERVOLTAGE_ERROR,
    descTMC_IDLER_UNDERVOLTAGE_ERROR,
    descTMC_PULLEY_DRIVER_SHORTED,
    descTMC_SELECTOR_DRIVER_SHORTED,
    descTMC_IDLER_DRIVER_SHORTED,
    descMMU_NOT_RESPONDING,
    descCOMMUNICATION_ERROR,
    descFILAMENT_ALREADY_LOADED,
    descINVALID_TOOL,
    descQUEUE_FULL,
    descVERSION_MISMATCH,
    descRUNTIME_ERROR,
};

/// Will be mapped onto dialog button responses in the FW
/// Those responses have their unique+translated texts as well
enum class ButtonOperations : uint8_t {
    NoOperation,
    Retry,
    SlowLoad,
    Continue,
    RestartMMU,
    Unload,
    StopPrint,
    DisableMMU,
};

// we have max 3 buttons/operations to select from
// one of them is "More" to show the explanation text normally hidden in the next screens.
// 01234567890123456789
// >bttxt >bttxt >MoreW
// Therefore at least some of the buttons, which can occur on the screen together, need to be 5-chars long max @@TODO.
// @@TODO beware - this doesn't correspond to the HW MMU buttons - needs to be discussed
static const char btnRetry[] PROGMEM_I1 = ISTR("Retry");
static const char btnSlowLoad[] PROGMEM_I1 = ISTR("Slow");
static const char btnContinue[] PROGMEM_I1 = ISTR("Done");
static const char btnRestartMMU[] PROGMEM_I1 = ISTR("RstMMU");
static const char btnUnload[] PROGMEM_I1 = ISTR("Unload");
static const char btnStop[] PROGMEM_I1 = ISTR("Stop");
static const char btnDisableMMU[] PROGMEM_I1 = ISTR("Disable");
static const char btnMore[] PROGMEM_I1 = ISTR("More\x01");

// We have 8 different operations/buttons at this time, so we need at least 4 bits to encode each.
// Since one of the buttons is always "More", we can skip that one.
// Therefore we need just 1 byte to describe the necessary buttons for each screen.
uint8_t constexpr Btns(ButtonOperations b0, ButtonOperations b1){
    return ((uint8_t)b1) << 4 | ((uint8_t)b0);
}

static const uint8_t errorButtons[] PROGMEM = {
    Btns(ButtonOperations::Retry, ButtonOperations::Continue),
    Btns(ButtonOperations::Retry, ButtonOperations::Continue),
    Btns(ButtonOperations::Retry, ButtonOperations::Continue),
    Btns(ButtonOperations::Retry, ButtonOperations::Continue),
    
    Btns(ButtonOperations::NoOperation, ButtonOperations::Continue),
    Btns(ButtonOperations::NoOperation, ButtonOperations::Continue),
    Btns(ButtonOperations::NoOperation, ButtonOperations::Continue),

    Btns(ButtonOperations::Continue, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::Continue, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::Continue, ButtonOperations::RestartMMU),
    
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),

    Btns(ButtonOperations::Unload, ButtonOperations::Continue),
    Btns(ButtonOperations::StopPrint, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::DisableMMU),
    Btns(ButtonOperations::NoOperation, ButtonOperations::RestartMMU),
};

} // namespace MMU2
