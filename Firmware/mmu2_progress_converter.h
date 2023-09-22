#pragma once
#include <stdint.h>
#include <stddef.h>
#ifdef __AVR__
    #include "mmu2/progress_codes.h"
#else
    #include "../../../../../../Prusa-Firmware-MMU/src/logic/progress_codes.h"
#endif

namespace MMU2 {

const char *ProgressCodeToText(ProgressCode pc);

}
