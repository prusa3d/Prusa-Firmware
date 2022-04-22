#include "mmu2_error_converter.h"
#include "mmu2/error_codes.h"
#include "mmu2/errors_list.h"
#include "language.h"

namespace MMU2 {

// @@TODO ideally compute the numbers by using some constexpr function, but since
// the current avg-gcc doesn't support cycles in constexpr functions, it is hard to achieve.
// So for now this has been hand-crafted.
const uint16_t MMUErrorCodeIndex(uint16_t ec) {
    switch (ec) {
    case (uint16_t)ErrorCode::FINDA_DIDNT_SWITCH_ON:
        return 0;
    case (uint16_t)ErrorCode::FINDA_DIDNT_SWITCH_OFF:
        return 1;
    case (uint16_t)ErrorCode::FSENSOR_DIDNT_SWITCH_ON:
        return 2;
    case (uint16_t)ErrorCode::FSENSOR_DIDNT_SWITCH_OFF:
        return 3;
    case (uint16_t)ErrorCode::STALLED_PULLEY:
        return 4;
    case (uint16_t)ErrorCode::HOMING_SELECTOR_FAILED:
        return 5;
    case (uint16_t)ErrorCode::HOMING_IDLER_FAILED:
        return 6;
    case (uint16_t)ErrorCode::MMU_NOT_RESPONDING:
        return 25;
    case (uint16_t)ErrorCode::PROTOCOL_ERROR:
        return 26;
    case (uint16_t)ErrorCode::FILAMENT_ALREADY_LOADED:
        return 27;
    case (uint16_t)ErrorCode::INVALID_TOOL:
        return 28;
    case (uint16_t)ErrorCode::QUEUE_FULL:
        return 29;
    case (uint16_t)ErrorCode::VERSION_MISMATCH:
        return 30;
    case (uint16_t)ErrorCode::INTERNAL:
        return 31;
    }

//    // TMC-related errors - multiple of these can occur at once
//    // - in such a case we report the first which gets found/converted into Prusa-Error-Codes (usually the fact, that one TMC has an issue is serious enough)
//    // By carefully ordering the checks here we can prioritize the errors being reported to the user.
    if (ec & (uint16_t)ErrorCode::TMC_PULLEY_BIT) {
        if (ec & (uint16_t)ErrorCode::TMC_IOIN_MISMATCH)
            return 13;
        if (ec & (uint16_t)ErrorCode::TMC_RESET)
            return 16;
        if (ec & (uint16_t)ErrorCode::TMC_UNDERVOLTAGE_ON_CHARGE_PUMP)
            return 19;
        if (ec & (uint16_t)ErrorCode::TMC_SHORT_TO_GROUND)
            return 22;
        if (ec & (uint16_t)ErrorCode::TMC_OVER_TEMPERATURE_WARN)
            return 7;
        if (ec & (uint16_t)ErrorCode::TMC_OVER_TEMPERATURE_ERROR)
            return 10;
    } else if (ec & (uint16_t)ErrorCode::TMC_SELECTOR_BIT) {
        if (ec & (uint16_t)ErrorCode::TMC_IOIN_MISMATCH)
            return 14;
        if (ec & (uint16_t)ErrorCode::TMC_RESET)
            return 17;
        if (ec & (uint16_t)ErrorCode::TMC_UNDERVOLTAGE_ON_CHARGE_PUMP)
            return 20;
        if (ec & (uint16_t)ErrorCode::TMC_SHORT_TO_GROUND)
            return 23;
        if (ec & (uint16_t)ErrorCode::TMC_OVER_TEMPERATURE_WARN)
            return 8;
        if (ec & (uint16_t)ErrorCode::TMC_OVER_TEMPERATURE_ERROR)
            return 11;
    } else if (ec & (uint16_t)ErrorCode::TMC_IDLER_BIT) {
        if (ec & (uint16_t)ErrorCode::TMC_IOIN_MISMATCH)
            return 15;
        if (ec & (uint16_t)ErrorCode::TMC_RESET)
            return 18;
        if (ec & (uint16_t)ErrorCode::TMC_UNDERVOLTAGE_ON_CHARGE_PUMP)
            return 21;
        if (ec & (uint16_t)ErrorCode::TMC_SHORT_TO_GROUND)
            return 24;
        if (ec & (uint16_t)ErrorCode::TMC_OVER_TEMPERATURE_WARN)
            return 9;
        if (ec & (uint16_t)ErrorCode::TMC_OVER_TEMPERATURE_ERROR)
            return 12;
    }

//    // if nothing got caught, return a generic error
//    return FindError(ERR_OTHER);
}

void TranslateErr(uint16_t ec, char *dst, size_t dstSize) { 
    uint16_t ei = MMUErrorCodeIndex(ec);
    // just to prevent the compiler from stripping the data structures from the final binary for now
    *dst = errorButtons[ei];
    strncpy_P(dst + 1, errorTitles[ei], dstSize);
    strncat_P(dst, errorDescs[ei], dstSize);
    
}

} // namespace MMU2
