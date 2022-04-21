#include "mmu2_fsensor.h"
#include "Filament_sensor.h"

namespace MMU2 {

FilamentState WhereIsFilament(){
    return fsensor.getFilamentPresent() ? FilamentState::IN_NOZZLE : FilamentState::NOT_PRESENT;
}


BlockRunoutRAII::BlockRunoutRAII() {
#ifdef FILAMENT_SENSOR
    fsensor.setRunoutEnabled(false); //suppress filament runouts while loading filament.
    fsensor.setAutoLoadEnabled(false); //suppress filament autoloads while loading filament.
#if (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
    fsensor.setJamDetectionEnabled(false); //suppress filament jam detection while loading filament.
#endif //(FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
#endif
}

BlockRunoutRAII::~BlockRunoutRAII() {
#ifdef FILAMENT_SENSOR
    fsensor.settings_init(); // restore filament runout state.
#endif
}

} // namespace MMU2
