#pragma once

#include <inttypes.h>
#include "Configuration.h"

#ifdef PRUSA_FARM
extern uint8_t farm_mode;
#else
#define farm_mode 0
#error prusa_farm_include_error
#endif
