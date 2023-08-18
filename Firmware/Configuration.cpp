#include "Configuration.h"
#include "Configuration_var.h"

const uint16_t _nPrinterType PROGMEM=PRINTER_TYPE;
const char _sPrinterName[] PROGMEM=PRINTER_NAME;
const uint16_t _nPrinterMmuType PROGMEM=PRINTER_MMU_TYPE;
const char _sPrinterMmuName[] PROGMEM=PRINTER_MMU_NAME;

static_assert(TEMP_HYSTERESIS > 0, "TEMP_HYSTERESIS must be greater than 0");
