#include "mmu2_fsensor.h"
#include "Filament_sensor.h"

namespace MMU2 {

FilamentState WhereIsFilament(){
    return fsensor.getFilamentPresent() ? FilamentState::AT_FSENSOR : FilamentState::NOT_PRESENT;
}

void ClearFilamentJamEvent() {
#if defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
    fsensor.clearEvent(Filament_sensor::Events::jam);
#endif
}

bool IsFilamentJammed() {
#if defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
    return fsensor.getEvent(Filament_sensor::Events::jam);
#else
    return false;
#endif
}

} // namespace MMU2
