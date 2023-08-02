#pragma once
#include <stdint.h>

// Helper macros to parse the operations from Btns()
#define BUTTON_OP_RIGHT(X) ( ( X & 0xF0 ) >> 4 )
#define BUTTON_OP_MIDDLE(X) ( X & 0x0F )

namespace MMU2 {

/// Will be mapped onto dialog button responses in the FW
/// Those responses have their unique+translated texts as well
enum class ButtonOperations : uint8_t {
    NoOperation = 0,
    Retry       = 1,
    Continue    = 2,
    ResetMMU  = 3,
    Unload      = 4,
    StopPrint   = 5,
    DisableMMU  = 6,
    Tune = 7,
};

/// Button codes + extended actions performed on the printer's side
enum Buttons : uint8_t {
    Right = 0,
    Middle,
    Left,
    
    // performed on the printer's side
    ResetMMU,
    StopPrint,
    DisableMMU,
    TuneMMU, // Printer changes MMU register value
    
    NoButton = 0xff // shall be kept last
};


} // namespace MMU2
