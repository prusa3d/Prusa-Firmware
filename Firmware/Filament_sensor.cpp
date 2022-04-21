#include "Filament_sensor.h"

#ifdef FILAMENT_SENSOR
#if FILAMENT_SENSOR_TYPE == FSENSOR_IR
IR_sensor fsensor;
#elif FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG
IR_sensor_analog fsensor;
#elif FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125
PAT9125_sensor fsensor;
#endif
#endif //FILAMENT_SENSOR
