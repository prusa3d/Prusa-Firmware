#include "mmu2_fsensor.h"

namespace MMU2 {

FilamentState WhereIsFilament(){
    // @@TODO
    return FilamentState::IN_NOZZLE;
}

// on AVR this does nothing
BlockRunoutRAII::BlockRunoutRAII() { }
BlockRunoutRAII::~BlockRunoutRAII() { }

} // namespace MMU2
