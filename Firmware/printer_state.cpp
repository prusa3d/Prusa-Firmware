//! @file
//! @brief Printer State
//! @param GetPrinterState get current printer state
//! @param SetPrinterState set printer state

#include "printer_state.h"

static PrinterState printer_state;

PrinterState GetPrinterState() {
    return printer_state;
}

PrinterState SetPrinterState(PrinterState status) {
    return printer_state = status;
}
