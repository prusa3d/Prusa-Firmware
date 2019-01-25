//! @file
//! @author: Marek Bel
//! @brief Filament auto deplete engine for multi-material prints with MMUv2 (Now marketed as SpoolJoin)
//!
//! Interface for marking MMUv2 filaments as depleted and getting alternative filament for printing.

#ifndef AUTODEPLETE_H
#define AUTODEPLETE_H

#include <stdint.h>

void ad_markDepleted(uint8_t filament);
void ad_markLoaded(uint8_t filament);
uint8_t ad_getAlternative(uint8_t filament);
bool ad_allDepleted();

#endif /* AUTODEPLETE_H */
