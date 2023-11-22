//! @file
//! @brief Printer States
//! @param NotReady
//! @param IsReady
//! @param Idle
//! @param SDPrintingFinished
//! @param HostPrintingFinished
//! @param IsSDPrinting
//! @param IsHostPrinting
//! @todo Pause/Resume states, Heating states and more

#pragma once
#include "macros.h"

// The order of the states is important as we check
// - LCD menu only shown when lower than "IsSDPrinting"
enum class PrinterState : uint8_t
{
    NotReady = 0,                 //Lowest state to simplify queries
    IsReady = 1,                  //
    Idle = 2,
    SDPrintingFinished = 3,
    HostPrintingFinished = 4,
    IsSDPrinting = 5,
    IsHostPrinting = 6,
};

PrinterState GetPrinterState();
PrinterState SetPrinterState(PrinterState status);
