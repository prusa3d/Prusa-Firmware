#include "mmu2_log.h"

namespace MMU2 {

void LogErrorEvent_P(const char *msg){
    SERIAL_ECHO_START; //!@todo Decide MMU2 errors  on serial line
    SERIAL_MMU2();
    SERIAL_ECHOLNRPGM(msg);
}

void LogEchoEvent_P(const char *msg){
    SERIAL_ECHO_START;
    SERIAL_MMU2();
    SERIAL_ECHOLNRPGM(msg);
}

} // namespace MMU2
