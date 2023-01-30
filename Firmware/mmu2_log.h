#pragma once

#ifndef UNITTEST
#include "Marlin.h"

// Beware - before changing this prefix, think twice
// you'd need to change appmain.cpp app_marlin_serial_output_write_hook
// and MMU2::ReportError + MMU2::ReportProgress
static constexpr char mmu2Magic[] PROGMEM = "MMU2:";

namespace MMU2 {

/// Report the msg into the general logging subsystem (through Marlin's SERIAL_ECHO stuff)
/// @param msg pointer to a string in PROGMEM
/// On the AVR platform this variant reads the input string from PROGMEM.
/// On the ARM platform it calls LogErrorEvent directly (silently expecting the compiler to optimize it away)
void LogErrorEvent_P(const char *msg);

/// Report the msg into the general logging subsystem (through Marlin's SERIAL_ECHO stuff)
/// @param msg pointer to a string in PROGMEM
/// On the AVR platform this variant reads the input string from PROGMEM.
/// On the ARM platform it calls LogErrorEvent directly (silently expecting the compiler to optimize it away)
void LogEchoEvent_P(const char *msg);

} // namespace

    #define SERIAL_MMU2() \
        { serialprintPGM(mmu2Magic); }

    #define MMU2_ECHO_MSGLN(S)   \
        do {                     \
            SERIAL_ECHO_START;   \
            SERIAL_MMU2();       \
            SERIAL_ECHOLN(S);    \
        } while (0)
    #define MMU2_ERROR_MSGLN(S) MMU2_ECHO_MSGLN(S) //!@todo Decide MMU2 errors  on serial line
    #define MMU2_ECHO_MSGRPGM(S) \
        do {                     \
            SERIAL_ECHO_START;   \
            SERIAL_MMU2();       \
            SERIAL_ECHORPGM(S);  \
        } while (0)
    #define MMU2_ERROR_MSGRPGM(S) MMU2_ECHO_MSGRPGM(S) //!@todo Decide MMU2 errors  on serial line
    #define MMU2_ECHO_MSG(S)     \
        do {                     \
            SERIAL_ECHO_START;   \
            SERIAL_MMU2();       \
            SERIAL_ECHO(S);      \
        } while (0)
    #define MMU2_ERROR_MSG(S) MMU2_ECHO_MSG(S) //!@todo Decide MMU2 errors  on serial line

#else // #ifndef UNITTEST

    #define MMU2_ECHO_MSGLN(S)    /* */
    #define MMU2_ERROR_MSGLN(S)   /* */
    #define MMU2_ECHO_MSGRPGM(S)  /* */
    #define MMU2_ERROR_MSGRPGM(S) /* */

#endif // #ifndef UNITTEST
