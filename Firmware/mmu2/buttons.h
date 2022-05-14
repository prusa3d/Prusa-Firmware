#pragma once
#include <stdint.h>

// Helper macros to parse the operations from Btns()
#define BUTTON_OP_HI_NIBBLE(X) ( ( X & 0xF0 ) >> 4 )
#define BUTTON_OP_LO_NIBBLE(X) ( X & 0x0F )

namespace MMU2 {

/// Will be mapped onto dialog button responses in the FW
/// Those responses have their unique+translated texts as well
enum class ButtonOperations : uint8_t {
    NoOperation = 0,
    Retry       = 1,
    Continue    = 2,
    RestartMMU  = 3,
    Unload      = 4,
    StopPrint   = 5,
    DisableMMU  = 6,
};

} // namespace MMU2
