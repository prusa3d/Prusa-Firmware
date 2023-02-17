// XFLASH dumper
#pragma once
#include "xflash_layout.h"

enum class dump_crash_reason : uint8_t
{
    manual = 0,
    stack_error,
    watchdog,
    bad_isr,
    bad_pullup_temp_isr,
    bad_pullup_step_isr,
};

#ifdef XFLASH_DUMP
void xfdump_reset();    // reset XFLASH dump state
void xfdump_dump();     // create a new SRAM memory dump

// return true if a dump is present, save type in "reason" if provided
bool xfdump_check_state(dump_crash_reason* reason = NULL);

// create a new dump containing registers and SRAM, then reset
void xfdump_full_dump_and_reset(dump_crash_reason crash = dump_crash_reason::manual);
#endif
