// Extracted from Prusa-Error-Codes repo
// Subject to automation and optimization
// BEWARE - this file shall be included only into mmu2_error_converter.cpp, not anywhere else!
#pragma once
#include "inttypes.h"
#include "../language.h"
#include <avr/pgmspace.h>
#include "buttons.h"

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
    ERR_CONNECT_MMU_NOT_RESPONDING,
    ERR_CONNECT_COMMUNICATION_ERROR,
    ERR_SYSTEM_FILAMENT_ALREADY_LOADED, 
    ERR_SYSTEM_INVALID_TOOL, 
    ERR_SYSTEM_QUEUE_FULL, 
    ERR_SYSTEM_FW_UPDATE_NEEDED, 
    ERR_SYSTEM_FW_RUNTIME_ERROR,
    ERR_SYSTEM_UNLOAD_MANUALLY
};

// @@TODO some of the strings are duplicates, can be merged into one
static const char titleFINDA_DIDNT_TRIGGER[] PROGMEM_I1 = ISTR("FINDA DIDNT TRIGGER");
static const char titleFINDA_DIDNT_GO_OFF[] PROGMEM_I1 = ISTR("FINDA DIDNT GO OFF");
static const char titleFSENSOR_DIDNT_TRIGGER[] PROGMEM_I1 = ISTR("FSENSOR DIDNT TRIGGER");
static const char titleFSENSOR_DIDNT_GO_OFF[] PROGMEM_I1 = ISTR("FSENSOR DIDNT GO OFF");
static const char titlePULLEY_CANNOT_MOVE[] PROGMEM_I1 = ISTR("PULLEY CANNOT MOVE");
static const char titleFSENSOR_TOO_EARLY[] PROGMEM_I1 = ISTR("FSENSOR TOO EARLY");
static const char titleSELECTOR_CANNOT_MOVE[] PROGMEM_I1 = ISTR("SELECTOR CANNOT MOVE");
static const char titleSELECTOR_CANNOT_HOME[] PROGMEM_I1 = ISTR("SELECTOR CANNOT HOME");
static const char titleIDLER_CANNOT_MOVE[] PROGMEM_I1 = ISTR("IDLER CANNOT MOVE");
static const char titleIDLER_CANNOT_HOME[] PROGMEM_I1 = ISTR("IDLER CANNOT HOME");
static const char titlePULLEY_TMC_WARNING_TMC_TOO_HOT[] PROGMEM_I1 = ISTR("WARNING TMC TOO HOT");
static const char titleSELECTOR_TMC_WARNING_TMC_TOO_HOT[] PROGMEM_I1 = ISTR("WARNING TMC TOO HOT");
static const char titleIDLER_TMC_WARNING_TMC_TOO_HOT[] PROGMEM_I1 = ISTR("WARNING TMC TOO HOT");
static const char titlePULLEY_TMC_OVERHEAT_ERROR[] PROGMEM_I1 = ISTR("TMC OVERHEAT ERROR");
static const char titleSELECTOR_TMC_OVERHEAT_ERROR[] PROGMEM_I1 = ISTR("TMC OVERHEAT ERROR");
static const char titleIDLER_TMC_OVERHEAT_ERROR[] PROGMEM_I1 = ISTR("TMC OVERHEAT ERROR");
static const char titlePULLEY_TMC_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC DRIVER ERROR");
static const char titleSELECTOR_TMC_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC DRIVER ERROR");
static const char titleIDLER_TMC_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC DRIVER ERROR");
static const char titlePULLEY_TMC_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC DRIVER RESET");
static const char titleSELECTOR_TMC_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC DRIVER RESET");
static const char titleIDLER_TMC_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC DRIVER RESET");
static const char titlePULLEY_TMC_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("TMC UNDERVOLTAGE ERR");
static const char titleSELECTOR_TMC_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("TMC UNDERVOLTAGE ERR");
static const char titleIDLER_TMC_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("TMC UNDERVOLTAGE ERR");
static const char titlePULLEY_TMC_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("TMC DRIVER SHORTED");
static const char titleSELECTOR_TMC_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("TMC DRIVER SHORTED");
static const char titleIDLER_TMC_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("TMC DRIVER SHORTED");
static const char titleMMU_NOT_RESPONDING[] PROGMEM_I1 = ISTR("MMU NOT RESPONDING");
static const char titleCOMMUNICATION_ERROR[] PROGMEM_I1 = ISTR("COMMUNICATION ERROR");
static const char titleFILAMENT_ALREADY_LOADED[] PROGMEM_I1 = ISTR("FILAMENT ALREADY LOA"); 
static const char titleINVALID_TOOL[] PROGMEM_I1 = ISTR("INVALID TOOL"); 
static const char titleQUEUE_FULL[] PROGMEM_I1 = ISTR("QUEUE FULL"); 
static const char titleFW_UPDATE_NEEDED[] PROGMEM_I1 = ISTR("MMU FW UPDATE NEEDED"); 
static const char titleFW_RUNTIME_ERROR[] PROGMEM_I1 = ISTR("FW RUNTIME ERROR");
static const char titleUNLOAD_MANUALLY[] PROGMEM_I1 = ISTR("UNLOAD MANUALLY");

static const char * const errorTitles [] PROGMEM = {
    titleFINDA_DIDNT_TRIGGER,
    titleFINDA_DIDNT_GO_OFF,
    titleFSENSOR_DIDNT_TRIGGER,
    titleFSENSOR_DIDNT_GO_OFF,
    titlePULLEY_CANNOT_MOVE,
    titleFSENSOR_TOO_EARLY,
    titleSELECTOR_CANNOT_HOME,
    titleSELECTOR_CANNOT_MOVE,
    titleIDLER_CANNOT_HOME,
    titleIDLER_CANNOT_MOVE,
    titlePULLEY_TMC_WARNING_TMC_TOO_HOT,
    titleSELECTOR_TMC_WARNING_TMC_TOO_HOT,
    titleIDLER_TMC_WARNING_TMC_TOO_HOT,
    titlePULLEY_TMC_OVERHEAT_ERROR,
    titleSELECTOR_TMC_OVERHEAT_ERROR,
    titleIDLER_TMC_OVERHEAT_ERROR,
    titlePULLEY_TMC_DRIVER_ERROR,
    titleSELECTOR_TMC_DRIVER_ERROR,
    titleIDLER_TMC_DRIVER_ERROR,
    titlePULLEY_TMC_DRIVER_RESET,
    titleSELECTOR_TMC_DRIVER_RESET,
    titleIDLER_TMC_DRIVER_RESET,
    titlePULLEY_TMC_UNDERVOLTAGE_ERROR,
    titleSELECTOR_TMC_UNDERVOLTAGE_ERROR,
    titleIDLER_TMC_UNDERVOLTAGE_ERROR,
    titlePULLEY_TMC_DRIVER_SHORTED,
    titleSELECTOR_TMC_DRIVER_SHORTED,
    titleIDLER_TMC_DRIVER_SHORTED,
    titleMMU_NOT_RESPONDING,
    titleCOMMUNICATION_ERROR,
    titleFILAMENT_ALREADY_LOADED,
    titleINVALID_TOOL,
    titleQUEUE_FULL,
    titleFW_UPDATE_NEEDED,
    titleFW_RUNTIME_ERROR,
    titleUNLOAD_MANUALLY
};

// @@TODO looking at the texts, they can be composed of several parts and/or parametrized (could save a lot of space ;) )
static const char descFINDA_DIDNT_TRIGGER[] PROGMEM_I1 = ISTR("FINDA didn't trigger while loading filament. Ensure filament can move and FINDA works.");
static const char descFINDA_DIDNT_GO_OFF[] PROGMEM_I1 = ISTR("FINDA didn't switch off while unloading filament. Try unloading manually. Ensure filament can move and FINDA works.");
static const char descFSENSOR_DIDNT_TRIGGER[] PROGMEM_I1 = ISTR("Filament sensor didn't trigger while loading filament. Ensure filament reached the fsensor and the sensor works.");
static const char descFSENSOR_DIDNT_GO_OFF[] PROGMEM_I1 = ISTR("Filament sensor didn't switch off while unloading filament. Ensure filament can move and the sensor works.");
static const char descPULLEY_STALLED[] PROGMEM_I1 = ISTR("The Pulley motor stalled - Ensure the pulley can move and check the wiring.");
static const char descFSENSOR_TOO_EARLY[] PROGMEM_I1 = ISTR("Filament sensor triggered too early while loading to extruder. Check there isn't anything stuck in PTFE tube. Check that sensor reads properly.");
static const char descSELECTOR_CANNOT_HOME[] PROGMEM_I1 = ISTR("The Selector cannot home properly - check for anything blocking its movement.");
static const char descSELECTOR_CANNOT_MOVE[] PROGMEM_I1 = ISTR("The Selector cannot move - check for anything blocking its movement. Check the wiring is correct.");
static const char descIDLER_CANNOT_MOVE[] PROGMEM_I1 = ISTR("The Idler cannot home properly - check for anything blocking its movement.");
static const char descIDLER_CANNOT_HOME[] PROGMEM_I1 = ISTR("The Idler cannot move properly - check for anything blocking its movement. Check the wiring is correct.");
static const char descPULLEY_WARNING_TMC_TOO_HOT[] PROGMEM_I1 = ISTR("TMC driver for the Pulley motor is almost overheating. Make sure there is sufficient airflow near the MMU board.");
static const char descSELECTOR_WARNING_TMC_TOO_HOT[] PROGMEM_I1 = ISTR("TMC driver for the Selector motor is almost overheating. Make sure there is sufficient airflow near the MMU board.");
static const char descIDLER_WARNING_TMC_TOO_HOT[] PROGMEM_I1 = ISTR("TMC driver for the Idler motor is almost overheating. Make sure there is sufficient airflow near the MMU board.");
static const char descPULLEY_TMC_OVERHEAT_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Pulley motor is overheated. Cool down the MMU board and reset MMU.");
static const char descSELECTOR_TMC_OVERHEAT_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Selector motor is overheated. Cool down the MMU board and reset MMU.");
static const char descIDLER_TMC_OVERHEAT_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Idler motor is overheated. Cool down the MMU board and reset MMU.");
static const char descPULLEY_TMC_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Pulley motor is not responding. Try resetting the MMU. If the issue persists contact support.");
static const char descSELECTOR_TMC_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Selector motor is not responding. Try resetting the MMU. If the issue persists contact support.");
static const char descIDLER_TMC_DRIVER_ERROR[] PROGMEM_I1 = ISTR("TMC driver for the Idler motor is not responding. Try resetting the MMU. If the issue persists contact support.");
static const char descPULLEY_TMC_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC driver for the Pulley motor was restarted. There is probably an issue with the electronics. Check the wiring and connectors.");
static const char descSELECTOR_TMC_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC driver for the Selector motor was restarted. There is probably an issue with the electronics. Check the wiring and connectors.");
static const char descIDLER_TMC_DRIVER_RESET[] PROGMEM_I1 = ISTR("TMC driver for the Idler motor was restarted. There is probably an issue with the electronics. Check the wiring and connectors.");
static const char descPULLEY_TMC_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("Not enough current for the Pulley TMC driver. There is probably an issue with the electronics. Check the wiring and connectors.");
static const char descSELECTOR_TMC_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("Not enough current for the Selector TMC driver. There is probably an issue with the electronics. Check the wiring and connectors.");
static const char descIDLER_TMC_UNDERVOLTAGE_ERROR[] PROGMEM_I1 = ISTR("Not enough current for the Idler TMC driver. There is probably an issue with the electronics. Check the wiring and connectors.");
static const char descPULLEY_TMC_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("Short circuit on the Pulley TMC driver. Check the wiring and connectors. If the issue persists contact support.");
static const char descSELECTOR_TMC_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("Short circuit on the Selector TMC driver. Check the wiring and connectors. If the issue persists contact support.");
static const char descIDLER_TMC_DRIVER_SHORTED[] PROGMEM_I1 = ISTR("Short circuit on the Idler TMC driver. Check the wiring and connectors. If the issue persists contact support.");
static const char descMMU_NOT_RESPONDING[] PROGMEM_I1 = ISTR("MMU unit not responding. Check the wiring and connectors. If the issue persists contact support.");
static const char descCOMMUNICATION_ERROR[] PROGMEM_I1 = ISTR("MMU unit not responding correctly. Check the wiring and connectors. If the issue persists contact support.");
static const char descFILAMENT_ALREADY_LOADED[] PROGMEM_I1 = ISTR("Cannot perform the action, filament is already loaded. Unload it first."); 
static const char descINVALID_TOOL[] PROGMEM_I1 = ISTR("Requested filament tool is not available on this hardware. Check the G-code for tool index out of range (T0-T4)."); 
static const char descQUEUE_FULL[] PROGMEM_I1 = ISTR("MMU Firmware internal error, please reset the MMU."); 
static const char descFW_UPDATE_NEEDED[] PROGMEM_I1 = ISTR("The MMU unit reports its FW version incompatible with the printer's firmware. Make sure the MMU firmware is up to date."); 
static const char descFW_RUNTIME_ERROR[] PROGMEM_I1 = ISTR("Internal runtime error. Try resetting the MMU unit or updating the firmware. If the issue persists contact support.");
static const char descUNLOAD_MANUALLY[] PROGMEM_I1 = ISTR("Unexpected FINDA reading. Ensure no filament is under FINDA and the selector is free. Check FINDA connection.");

static const char * const errorDescs[] PROGMEM = {
    descFINDA_DIDNT_TRIGGER,
    descFINDA_DIDNT_GO_OFF,
    descFSENSOR_DIDNT_TRIGGER,
    descFSENSOR_DIDNT_GO_OFF,
    descPULLEY_STALLED,
    descFSENSOR_TOO_EARLY,
    descSELECTOR_CANNOT_HOME,
    descSELECTOR_CANNOT_MOVE,
    descIDLER_CANNOT_HOME,
    descIDLER_CANNOT_MOVE,
    descPULLEY_WARNING_TMC_TOO_HOT,
    descSELECTOR_WARNING_TMC_TOO_HOT,
    descIDLER_WARNING_TMC_TOO_HOT,
    descPULLEY_TMC_OVERHEAT_ERROR,
    descSELECTOR_TMC_OVERHEAT_ERROR,
    descIDLER_TMC_OVERHEAT_ERROR,
    descPULLEY_TMC_DRIVER_ERROR,
    descSELECTOR_TMC_DRIVER_ERROR,
    descIDLER_TMC_DRIVER_ERROR,
    descPULLEY_TMC_DRIVER_RESET,
    descSELECTOR_TMC_DRIVER_RESET,
    descIDLER_TMC_DRIVER_RESET,
    descPULLEY_TMC_UNDERVOLTAGE_ERROR,
    descSELECTOR_TMC_UNDERVOLTAGE_ERROR,
    descIDLER_TMC_UNDERVOLTAGE_ERROR,
    descPULLEY_TMC_DRIVER_SHORTED,
    descSELECTOR_TMC_DRIVER_SHORTED,
    descIDLER_TMC_DRIVER_SHORTED,
    descMMU_NOT_RESPONDING,
    descCOMMUNICATION_ERROR,
    descFILAMENT_ALREADY_LOADED,
    descINVALID_TOOL,
    descQUEUE_FULL,
    descFW_UPDATE_NEEDED,
    descFW_RUNTIME_ERROR,
    descUNLOAD_MANUALLY
};

// we have max 3 buttons/operations to select from
// one of them is "More" to show the explanation text normally hidden in the next screens.
// 01234567890123456789
// >bttxt >bttxt >MoreW
// Therefore at least some of the buttons, which can occur on the screen together, need to be 5-chars long max @@TODO.
// Beware - we only have space for 2 buttons on the LCD while the MMU has 3 buttons
// -> the left button on the MMU is not used/rendered on the LCD (it is also almost unused on the MMU side)
static const char btnRetry[] PROGMEM_I1 = ISTR("Retry");
static const char btnContinue[] PROGMEM_I1 = ISTR("Done");
static const char btnRestartMMU[] PROGMEM_I1 = ISTR("RstMMU");
static const char btnUnload[] PROGMEM_I1 = ISTR("Unload");
static const char btnStop[] PROGMEM_I1 = ISTR("Stop");
static const char btnDisableMMU[] PROGMEM_I1 = ISTR("Disable");
static const char btnMore[] PROGMEM_I1 = ISTR("More\x06");

// Used to parse the buttons from Btns().
static const char * const btnOperation[] PROGMEM = {
    btnRetry,
    btnContinue,
    btnRestartMMU,
    btnUnload,
    btnStop,
    btnDisableMMU
};

// We have 8 different operations/buttons at this time, so we need at least 4 bits to encode each.
// Since one of the buttons is always "More", we can skip that one.
// Therefore we need just 1 byte to describe the necessary buttons for each screen.
uint8_t constexpr Btns(ButtonOperations bMiddle, ButtonOperations bRight){
    return ((uint8_t)bRight) << 4 | ((uint8_t)bMiddle);
}

static const uint8_t errorButtons[] PROGMEM = {
    Btns(ButtonOperations::Retry, ButtonOperations::Continue),//FINDA_DIDNT_TRIGGER
    Btns(ButtonOperations::Retry, ButtonOperations::Continue),//FINDA_DIDNT_GO_OFF
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//FSENSOR_DIDNT_TRIGGER
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//FSENSOR_DIDNT_GO_OFF

    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//PULLEY_STALLED
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//FSENSOR_TOO_EARLY
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
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//MMU_NOT_RESPONDING
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//COMMUNICATION_ERROR

    Btns(ButtonOperations::Unload, ButtonOperations::Continue),//FILAMENT_ALREADY_LOADED
    Btns(ButtonOperations::StopPrint, ButtonOperations::RestartMMU),//INVALID_TOOL
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//QUEUE_FULL
    Btns(ButtonOperations::DisableMMU, ButtonOperations::NoOperation),//FW_UPDATE_NEEDED
    Btns(ButtonOperations::RestartMMU, ButtonOperations::NoOperation),//FW_RUNTIME_ERROR
    Btns(ButtonOperations::Retry, ButtonOperations::NoOperation),//UNLOAD_MANUALLY
};

static_assert( sizeof(errorCodes) / sizeof(errorCodes[0]) == sizeof(errorDescs) / sizeof (errorDescs[0]));
static_assert( sizeof(errorCodes) / sizeof(errorCodes[0]) == sizeof(errorTitles) / sizeof (errorTitles[0]));
static_assert( sizeof(errorCodes) / sizeof(errorCodes[0]) == sizeof(errorButtons) / sizeof (errorButtons[0]));

} // namespace MMU2
