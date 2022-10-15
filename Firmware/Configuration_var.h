// Include the printer's variant configuration header
#pragma once

// This is set by the cmake build to be able to take control of
// the variant header without breaking existing build mechanisms.
#ifndef CMAKE_CONTROL
#include "Configuration_prusa.h"
#else
#include FW_VARIANT
#endif
