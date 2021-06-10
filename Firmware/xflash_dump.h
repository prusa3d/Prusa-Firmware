// XFLASH dumper
#pragma once
#include "xflash_layout.h"
#ifdef XFLASH_DUMP

void xfdump_reset();        // reset XFLASH dump state
bool xfdump_check_state();  // return true if a dump is present
bool xfdump_check_crash();  // return true if a dump is present and is a crash dump
void xfdump_dump();         // create a new SRAM memory dump

enum class dump_crash_reason : uint8_t
{
    manual = 0,
    stack_error,
    watchdog,
};

// create a new dump containing registers and SRAM, then reset
void xfdump_full_dump_and_reset(dump_crash_reason crash = dump_crash_reason::manual);

#endif
