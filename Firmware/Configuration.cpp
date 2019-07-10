#include "Configuration.h"
#include "Configuration_prusa.h"

const uint16_t _nPrinterType PROGMEM=PRINTER_TYPE;
const char _sPrinterName[] PROGMEM=PRINTER_NAME;
const uint16_t _nPrinterMmuType PROGMEM=PRINTER_MMU_TYPE;
const char _sPrinterMmuName[] PROGMEM=PRINTER_MMU_NAME;

uint16_t nPrinterType;
PGM_P sPrinterName;