#include "mmu2_fsensor.h"
#include "Filament_sensor.h"

namespace MMU2 {

FilamentState WhereIsFilament(){
    return fsensor.getFilamentPresent() ? FilamentState::IN_NOZZLE : FilamentState::NOT_PRESENT;
}

// on AVR this does nothing
BlockRunoutRAII::BlockRunoutRAII() { }
BlockRunoutRAII::~BlockRunoutRAII() { }

} // namespace MMU2
