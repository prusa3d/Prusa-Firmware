#include "mmu2_error_converter.h"
#include "mmu2/error_codes.h"
#include "mmu2/errors_list.h"
#include "language.h"
#include <stdio.h>

namespace MMU2 {

static ButtonOperations buttonSelectedOperation = ButtonOperations::NoOperation;

// we don't have a constexpr find_if in C++17/STL yet
template <class InputIt, class UnaryPredicate>
constexpr InputIt find_if_cx(InputIt first, InputIt last, UnaryPredicate p) {
    for (; first != last; ++first) {
        if (p(*first)) {
            return first;
        }
    }
    return last;
}

// Making a constexpr FindError should instruct the compiler to optimize the PrusaErrorCodeIndex
// in such a way that no searching will ever be done at runtime.
// A call to FindError then compiles to a single instruction even on the AVR.
static constexpr uint8_t FindErrorIndex(uint16_t pec) {
    constexpr uint16_t errorCodesSize = sizeof(errorCodes) / sizeof(errorCodes[0]);
    constexpr const auto *errorCodesEnd = errorCodes + errorCodesSize;
    const auto *i = find_if_cx(errorCodes, errorCodesEnd, [pec](uint16_t ed){ return ed == pec; });
    return (i != errorCodesEnd) ? (i-errorCodes) : (errorCodesSize - 1);
}

// check that the searching algoritm works
static_assert( FindErrorIndex(ERR_MECHANICAL_FINDA_DIDNT_TRIGGER) == 0);
static_assert( FindErrorIndex(ERR_MECHANICAL_FINDA_DIDNT_GO_OFF) == 1);
static_assert( FindErrorIndex(ERR_MECHANICAL_FSENSOR_DIDNT_TRIGGER) == 2);
static_assert( FindErrorIndex(ERR_MECHANICAL_FSENSOR_DIDNT_GO_OFF) == 3);

uint8_t PrusaErrorCodeIndex(uint16_t ec) {
    switch (ec) {
    case (uint16_t)ErrorCode::FINDA_DIDNT_SWITCH_ON:
        return FindErrorIndex(ERR_MECHANICAL_FINDA_DIDNT_TRIGGER);
    case (uint16_t)ErrorCode::FINDA_DIDNT_SWITCH_OFF:
        return FindErrorIndex(ERR_MECHANICAL_FINDA_DIDNT_GO_OFF);
    case (uint16_t)ErrorCode::FSENSOR_DIDNT_SWITCH_ON:
        return FindErrorIndex(ERR_MECHANICAL_FSENSOR_DIDNT_TRIGGER);
    case (uint16_t)ErrorCode::FSENSOR_DIDNT_SWITCH_OFF:
        return FindErrorIndex(ERR_MECHANICAL_FSENSOR_DIDNT_GO_OFF);
    case (uint16_t)ErrorCode::FSENSOR_TOO_EARLY:
        return FindErrorIndex(ERR_MECHANICAL_FSENSOR_TOO_EARLY);
    case (uint16_t)ErrorCode::FINDA_FLICKERS:
        return FindErrorIndex(ERR_MECHANICAL_INSPECT_FINDA);
    case (uint16_t)ErrorCode::LOAD_TO_EXTRUDER_FAILED:
        return FindErrorIndex(ERR_MECHANICAL_LOAD_TO_EXTRUDER_FAILED);
    case (uint16_t)ErrorCode::FILAMENT_EJECTED:
        return FindErrorIndex(ERR_SYSTEM_FILAMENT_EJECTED);

    case (uint16_t)ErrorCode::STALLED_PULLEY:
    case (uint16_t)ErrorCode::MOVE_PULLEY_FAILED:
        return FindErrorIndex(ERR_MECHANICAL_PULLEY_CANNOT_MOVE);
        
    case (uint16_t)ErrorCode::HOMING_SELECTOR_FAILED:
        return FindErrorIndex(ERR_MECHANICAL_SELECTOR_CANNOT_HOME);
    case (uint16_t)ErrorCode::MOVE_SELECTOR_FAILED:
        return FindErrorIndex(ERR_MECHANICAL_SELECTOR_CANNOT_MOVE);
        
    case (uint16_t)ErrorCode::HOMING_IDLER_FAILED:
        return FindErrorIndex(ERR_MECHANICAL_IDLER_CANNOT_HOME);
    case (uint16_t)ErrorCode::MOVE_IDLER_FAILED:
        return FindErrorIndex(ERR_MECHANICAL_IDLER_CANNOT_MOVE);
        
    case (uint16_t)ErrorCode::MMU_NOT_RESPONDING:
        return FindErrorIndex(ERR_CONNECT_MMU_NOT_RESPONDING);
    case (uint16_t)ErrorCode::PROTOCOL_ERROR:
        return FindErrorIndex(ERR_CONNECT_COMMUNICATION_ERROR);
    case (uint16_t)ErrorCode::FILAMENT_ALREADY_LOADED:
        return FindErrorIndex(ERR_SYSTEM_FILAMENT_ALREADY_LOADED);
    case (uint16_t)ErrorCode::INVALID_TOOL:
        return FindErrorIndex(ERR_SYSTEM_INVALID_TOOL);
    case (uint16_t)ErrorCode::QUEUE_FULL:
        return FindErrorIndex(ERR_SYSTEM_QUEUE_FULL);
    case (uint16_t)ErrorCode::VERSION_MISMATCH:
        return FindErrorIndex(ERR_SYSTEM_FW_UPDATE_NEEDED);
    case (uint16_t)ErrorCode::INTERNAL:
        return FindErrorIndex(ERR_SYSTEM_FW_RUNTIME_ERROR);
    case (uint16_t)ErrorCode::FINDA_VS_EEPROM_DISREPANCY:
        return FindErrorIndex(ERR_SYSTEM_UNLOAD_MANUALLY);
    }
    
    // Electrical issues which can be detected somehow.
    // Need to be placed before TMC-related errors in order to process couples of error bits between single ones
    // and to keep the code size down.
    if (ec & (uint16_t)ErrorCode::TMC_PULLEY_BIT) {
        if ((ec & (uint16_t)ErrorCode::MMU_SOLDERING_NEEDS_ATTENTION) == (uint16_t)ErrorCode::MMU_SOLDERING_NEEDS_ATTENTION)
            return FindErrorIndex(ERR_ELECTRICAL_PULLEY_SELFTEST_FAILED);
    } else if (ec & (uint16_t)ErrorCode::TMC_SELECTOR_BIT) {
        if ((ec & (uint16_t)ErrorCode::MMU_SOLDERING_NEEDS_ATTENTION) == (uint16_t)ErrorCode::MMU_SOLDERING_NEEDS_ATTENTION)
            return FindErrorIndex(ERR_ELECTRICAL_SELECTOR_SELFTEST_FAILED);
    } else if (ec & (uint16_t)ErrorCode::TMC_IDLER_BIT) {
        if ((ec & (uint16_t)ErrorCode::MMU_SOLDERING_NEEDS_ATTENTION) == (uint16_t)ErrorCode::MMU_SOLDERING_NEEDS_ATTENTION)
            return FindErrorIndex(ERR_ELECTRICAL_IDLER_SELFTEST_FAILED);
    }

    // TMC-related errors - multiple of these can occur at once
    // - in such a case we report the first which gets found/converted into Prusa-Error-Codes (usually the fact, that one TMC has an issue is serious enough)
    // By carefully ordering the checks here we can prioritize the errors being reported to the user.
    if (ec & (uint16_t)ErrorCode::TMC_PULLEY_BIT) {
        if (ec & (uint16_t)ErrorCode::TMC_IOIN_MISMATCH)
            return FindErrorIndex(ERR_ELECTRICAL_PULLEY_TMC_DRIVER_ERROR);
        if (ec & (uint16_t)ErrorCode::TMC_RESET)
            return FindErrorIndex(ERR_ELECTRICAL_PULLEY_TMC_DRIVER_RESET);
        if (ec & (uint16_t)ErrorCode::TMC_UNDERVOLTAGE_ON_CHARGE_PUMP)
            return FindErrorIndex(ERR_ELECTRICAL_PULLEY_TMC_UNDERVOLTAGE_ERROR);
        if (ec & (uint16_t)ErrorCode::TMC_SHORT_TO_GROUND)
            return FindErrorIndex(ERR_ELECTRICAL_PULLEY_TMC_DRIVER_SHORTED);
        if (ec & (uint16_t)ErrorCode::TMC_OVER_TEMPERATURE_WARN)
            return FindErrorIndex(ERR_TEMPERATURE_PULLEY_WARNING_TMC_TOO_HOT);
        if (ec & (uint16_t)ErrorCode::TMC_OVER_TEMPERATURE_ERROR)
            return FindErrorIndex(ERR_TEMPERATURE_PULLEY_TMC_OVERHEAT_ERROR);
    } else if (ec & (uint16_t)ErrorCode::TMC_SELECTOR_BIT) {
        if (ec & (uint16_t)ErrorCode::TMC_IOIN_MISMATCH)
            return FindErrorIndex(ERR_ELECTRICAL_SELECTOR_TMC_DRIVER_ERROR);
        if (ec & (uint16_t)ErrorCode::TMC_RESET)
            return FindErrorIndex(ERR_ELECTRICAL_SELECTOR_TMC_DRIVER_RESET);
        if (ec & (uint16_t)ErrorCode::TMC_UNDERVOLTAGE_ON_CHARGE_PUMP)
            return FindErrorIndex(ERR_ELECTRICAL_SELECTOR_TMC_UNDERVOLTAGE_ERROR);
        if (ec & (uint16_t)ErrorCode::TMC_SHORT_TO_GROUND)
            return FindErrorIndex(ERR_ELECTRICAL_SELECTOR_TMC_DRIVER_SHORTED);
        if (ec & (uint16_t)ErrorCode::TMC_OVER_TEMPERATURE_WARN)
            return FindErrorIndex(ERR_TEMPERATURE_SELECTOR_WARNING_TMC_TOO_HOT);
        if (ec & (uint16_t)ErrorCode::TMC_OVER_TEMPERATURE_ERROR)
            return FindErrorIndex(ERR_TEMPERATURE_SELECTOR_TMC_OVERHEAT_ERROR);
    } else if (ec & (uint16_t)ErrorCode::TMC_IDLER_BIT) {
        if (ec & (uint16_t)ErrorCode::TMC_IOIN_MISMATCH)
            return FindErrorIndex(ERR_ELECTRICAL_IDLER_TMC_DRIVER_ERROR);
        if (ec & (uint16_t)ErrorCode::TMC_RESET)
            return FindErrorIndex(ERR_ELECTRICAL_IDLER_TMC_DRIVER_RESET);
        if (ec & (uint16_t)ErrorCode::TMC_UNDERVOLTAGE_ON_CHARGE_PUMP)
            return FindErrorIndex(ERR_ELECTRICAL_IDLER_TMC_UNDERVOLTAGE_ERROR);
        if (ec & (uint16_t)ErrorCode::TMC_SHORT_TO_GROUND)
            return FindErrorIndex(ERR_ELECTRICAL_IDLER_TMC_DRIVER_SHORTED);
        if (ec & (uint16_t)ErrorCode::TMC_OVER_TEMPERATURE_WARN)
            return FindErrorIndex(ERR_TEMPERATURE_IDLER_WARNING_TMC_TOO_HOT);
        if (ec & (uint16_t)ErrorCode::TMC_OVER_TEMPERATURE_ERROR)
            return FindErrorIndex(ERR_TEMPERATURE_IDLER_TMC_OVERHEAT_ERROR);
    }

    // if nothing got caught, return a generic runtime error
    return FindErrorIndex(ERR_SYSTEM_FW_RUNTIME_ERROR);
}

uint16_t PrusaErrorCode(uint8_t i){
    return pgm_read_word(errorCodes + i);
}

const char * PrusaErrorTitle(uint8_t i){
    return (const char *)pgm_read_ptr(errorTitles + i);
}

const char * PrusaErrorDesc(uint8_t i){
    return (const char *)pgm_read_ptr(errorDescs + i);
}

uint8_t PrusaErrorButtons(uint8_t i){
    return pgm_read_byte(errorButtons + i);
}

const char * PrusaErrorButtonTitle(uint8_t bi){
    // -1 represents the hidden NoOperation button which is not drawn in any way
    return (const char *)pgm_read_ptr(btnOperation + bi - 1);
}

const char * PrusaErrorButtonMore(){
    return _R(MSG_BTN_MORE);//@todo convert to PROGMEM_N1
}

struct ResetOnExit {
    ResetOnExit() = default;
    ~ResetOnExit(){
        buttonSelectedOperation = ButtonOperations::NoOperation;
    }
};

Buttons ButtonPressed(uint16_t ec) {
    if (buttonSelectedOperation == ButtonOperations::NoOperation) {
        return NoButton; // no button
    }
    
    ResetOnExit ros; // clear buttonSelectedOperation on exit from this call
    return ButtonAvailable(ec);
}

Buttons ButtonAvailable(uint16_t ec) {
    uint8_t ei = PrusaErrorCodeIndex(ec);
    
    // The list of responses which occur in mmu error dialogs
    // Return button index or perform some action on the MK3 by itself (like restart MMU)
    // Based on Prusa-Error-Codes errors_list.h
    // So far hardcoded, but shall be generated in the future
    switch ( PrusaErrorCode(ei) ) {
    case ERR_MECHANICAL_FINDA_DIDNT_TRIGGER:
    case ERR_MECHANICAL_FINDA_DIDNT_GO_OFF:
    case ERR_MECHANICAL_FSENSOR_DIDNT_TRIGGER:
    case ERR_MECHANICAL_FSENSOR_DIDNT_GO_OFF:
    case ERR_MECHANICAL_FSENSOR_TOO_EARLY:
    case ERR_MECHANICAL_INSPECT_FINDA:
    case ERR_MECHANICAL_SELECTOR_CANNOT_HOME:
    case ERR_MECHANICAL_SELECTOR_CANNOT_MOVE:
    case ERR_MECHANICAL_IDLER_CANNOT_HOME:
    case ERR_MECHANICAL_IDLER_CANNOT_MOVE:
    case ERR_MECHANICAL_PULLEY_CANNOT_MOVE:
    case ERR_SYSTEM_UNLOAD_MANUALLY:
        switch (buttonSelectedOperation) {
        // may be allow move selector right and left in the future
        case ButtonOperations::Retry: // "Repeat action"
            return Middle;
        default:
            break;
        }
        break;
    case ERR_MECHANICAL_LOAD_TO_EXTRUDER_FAILED:
    case ERR_SYSTEM_FILAMENT_EJECTED:
        switch (buttonSelectedOperation) {
        case ButtonOperations::Continue: // User solved the serious mechanical problem by hand - there is no other way around
            return Middle;
        default:
            break;
        }
        break;
    case ERR_TEMPERATURE_PULLEY_WARNING_TMC_TOO_HOT:
    case ERR_TEMPERATURE_SELECTOR_WARNING_TMC_TOO_HOT:
    case ERR_TEMPERATURE_IDLER_WARNING_TMC_TOO_HOT:
        switch (buttonSelectedOperation) {
        case ButtonOperations::Continue: // "Continue"
            return Left;
        case ButtonOperations::RestartMMU: // "Restart MMU"
            return RestartMMU;
        default:
            break;
        }
        break;
        
    case ERR_TEMPERATURE_PULLEY_TMC_OVERHEAT_ERROR:
    case ERR_TEMPERATURE_SELECTOR_TMC_OVERHEAT_ERROR:
    case ERR_TEMPERATURE_IDLER_TMC_OVERHEAT_ERROR:
        
    case ERR_ELECTRICAL_PULLEY_TMC_DRIVER_ERROR:
    case ERR_ELECTRICAL_SELECTOR_TMC_DRIVER_ERROR:
    case ERR_ELECTRICAL_IDLER_TMC_DRIVER_ERROR:
        
    case ERR_ELECTRICAL_PULLEY_TMC_DRIVER_RESET:
    case ERR_ELECTRICAL_SELECTOR_TMC_DRIVER_RESET:
    case ERR_ELECTRICAL_IDLER_TMC_DRIVER_RESET:
        
    case ERR_ELECTRICAL_PULLEY_TMC_UNDERVOLTAGE_ERROR:
    case ERR_ELECTRICAL_SELECTOR_TMC_UNDERVOLTAGE_ERROR:
    case ERR_ELECTRICAL_IDLER_TMC_UNDERVOLTAGE_ERROR:
        
    case ERR_ELECTRICAL_PULLEY_TMC_DRIVER_SHORTED:
    case ERR_ELECTRICAL_SELECTOR_TMC_DRIVER_SHORTED:
    case ERR_ELECTRICAL_IDLER_TMC_DRIVER_SHORTED:

    case ERR_ELECTRICAL_PULLEY_SELFTEST_FAILED:
    case ERR_ELECTRICAL_SELECTOR_SELFTEST_FAILED:
    case ERR_ELECTRICAL_IDLER_SELFTEST_FAILED:

    case ERR_SYSTEM_QUEUE_FULL:
    case ERR_SYSTEM_FW_RUNTIME_ERROR:
        switch (buttonSelectedOperation) {
        case ButtonOperations::RestartMMU: // "Restart MMU"
            return RestartMMU;
        default:
            break;
        }
        break;
    case ERR_CONNECT_MMU_NOT_RESPONDING:
    case ERR_CONNECT_COMMUNICATION_ERROR:
    case ERR_SYSTEM_FW_UPDATE_NEEDED:
        switch (buttonSelectedOperation) {
        case ButtonOperations::DisableMMU: // "Disable"
            return DisableMMU;
        case ButtonOperations::RestartMMU: // "RestartMMU"
            return RestartMMU;
        default:
            break;
        }
        break;
    case ERR_SYSTEM_FILAMENT_ALREADY_LOADED:
        switch (buttonSelectedOperation) {
        case ButtonOperations::Unload: // "Unload"
            return Left;
        case ButtonOperations::Continue: // "Proceed/Continue"
            return Right;
        default:
            break;
        }
        break;
        
    case ERR_SYSTEM_INVALID_TOOL:
        switch (buttonSelectedOperation) {
        case ButtonOperations::StopPrint: // "Stop print"
            return StopPrint;
        case ButtonOperations::RestartMMU: // "Restart MMU"
            return RestartMMU;
        default:
            break;
        }
        break;

    default:
        break;
    }
    
    return NoButton;
}

void SetButtonResponse(ButtonOperations rsp){
    buttonSelectedOperation = rsp;
}

} // namespace MMU2
