#include "mmu2_reporting.h"

// @@TODO implement the interface for MK3

namespace MMU2 {

void BeginReport(CommandInProgress cip, uint16_t ec) { }

void EndReport(CommandInProgress cip, uint16_t ec) { }

void ReportErrorHook(CommandInProgress cip, uint16_t ec) { }

void ReportProgressHook(CommandInProgress cip, uint16_t ec) { }

Buttons ButtonPressed(uint16_t ec) { }

bool MMUAvailable() { }

bool UseMMU() { }

} // namespace MMU2
