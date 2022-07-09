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

/**
 * @brief Called when the MMU or MK3S sends operation error (even repeatedly).
 * Render MMU error screen on the LCD. This must be non-blocking
 * and allow the MMU and printer to communicate with each other.
 * @param[in] ec error code
 * @param[in] res reporter error source, is either Printer (0) or MMU (1)
 */
void ReportErrorHook(uint16_t ec, uint8_t res);

/// Called when the MMU sends operation progress update
void ReportProgressHook(CommandInProgress cip, uint16_t ec);

/// Remders the sensor status line. Also used by the "resume temperature" screen.
void ReportErrorHookDynamicRender();

/// Renders the static part of the sensor state line. Also used by "resuming temperature screen"
void ReportErrorHookSensorLineRender();

/// @returns true if the MMU is communicating and available
/// can change at runtime
bool MMUAvailable();

/// Global Enable/Disable use MMU (to be stored in EEPROM)
bool UseMMU();

} // namespace
