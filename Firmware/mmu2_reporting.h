/// @file mmu2_reporting.h

#pragma once
#include <stdint.h>

namespace MMU2 {

enum CommandInProgress : uint8_t {
    NoCommand = 0,
    CutFilament = 'C',
    EjectFilament = 'E',
    Homing = 'H',
    LoadFilament = 'L',
    Reset = 'X',
    ToolChange = 'T',
    UnloadFilament = 'U',
};

/// Called at the begin of every MMU operation
void BeginReport(CommandInProgress cip, uint16_t ec);

/// Called at the end of every MMU operation
void EndReport(CommandInProgress cip, uint16_t ec);

/// Called when the MMU sends operation error (even repeatedly)
void ReportErrorHook(CommandInProgress cip, uint16_t ec);

/// Called when the MMU sends operation progress update
void ReportProgressHook(CommandInProgress cip, uint16_t ec);

/// @returns true if the MMU is communicating and available
/// can change at runtime
bool MMUAvailable();

/// Global Enable/Disable use MMU (to be stored in EEPROM)
bool UseMMU();

} // namespace
