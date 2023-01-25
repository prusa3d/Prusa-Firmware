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

/// @brief Called when the MMU or MK3S sends operation error (even repeatedly).
/// Render MMU error screen on the LCD. This must be non-blocking
/// and allow the MMU and printer to communicate with each other.
/// @param[in] ec error code
/// @param[in] es error source
void ReportErrorHook(CommandInProgress cip, uint16_t ec, uint8_t es);

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

/// Increments EEPROM cell - number of failed loads into the nozzle
/// Note: technically, this is not an MMU error but an error of the printer.
void IncrementLoadFails();

/// Increments EEPROM cell - number of MMU errors
void IncrementMMUFails();

// Beware: enum values intentionally chosen to match the 8bit FW to save code size
enum SoundType {
    Prompt = 2,
    Confirm = 3
};

void MakeSound(SoundType s);

void FullScreenMsgCut(uint8_t slot);
void FullScreenMsgEject(uint8_t slot);
void FullScreenMsgTest(uint8_t slot);
void FullScreenMsgLoad(uint8_t slot);
void FullScreenMsgRestoringTemperature();

void ScreenUpdateEnable();
void ScreenClear();

} // namespace
