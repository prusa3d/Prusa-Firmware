#pragma once
#include <stdint.h>

namespace MMU2 {

/// Possible states of filament from the perspective of presence in various parts of the printer
/// Beware, the numeric codes are important and sent into the MMU
enum class FilamentState : uint_fast8_t {
    NOT_PRESENT = 0, ///< filament sensor doesn't see the filament
    AT_FSENSOR = 1, ///< filament detected by the filament sensor, but the nozzle has not detected the filament yet
    IN_NOZZLE = 2 ///< filament detected by the filament sensor and also loaded in the nozzle
};

FilamentState WhereIsFilament();

/// Can be used to block printer's filament sensor handling - to avoid errorneous injecting of M600
/// while doing a toolchange with the MMU
class BlockRunoutRAII {
public:
    BlockRunoutRAII();
    ~BlockRunoutRAII();
};

} // namespace MMU2
