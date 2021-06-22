#include <stddef.h>

#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "xflash_dump.h"
#ifdef XFLASH_DUMP
#include "asm.h"
#include "xflash.h"
#include "Marlin.h" // for softReset

bool xfdump_check_state(dump_crash_reason* reason)
{
    uint32_t magic;

    XFLASH_SPI_ENTER();
    xflash_rd_data(DUMP_OFFSET + offsetof(dump_t, header.magic),
                   (uint8_t*)&magic, sizeof(magic));
    if (magic != DUMP_MAGIC)
        return false;

    if (reason)
    {
        xflash_rd_data(DUMP_OFFSET + offsetof(dump_t, header.crash_reason),
                       (uint8_t*)reason, sizeof(*reason));
    }
    return true;
}


void xfdump_reset()
{
    XFLASH_SPI_ENTER();
    xflash_enable_wr();
    xflash_sector_erase(DUMP_OFFSET + offsetof(dump_t, header.magic));
    xflash_wait_busy();
}


static void xfdump_erase()
{
    for(uint32_t addr = DUMP_OFFSET;
        addr < DUMP_OFFSET + DUMP_SIZE;
        addr += 4096)
    {
        xflash_enable_wr();
        xflash_sector_erase(addr);
        xflash_wait_busy();
    }
}


static void __attribute__((noinline)) xfdump_dump_core(dump_header_t& hdr, uint32_t addr, uint8_t* buf, uint16_t cnt)
{
    XFLASH_SPI_ENTER();

    // start by clearing all sectors (we need all of them in any case)
    xfdump_erase();

    // sample SP/PC
    hdr.sp = SP;
    GETPC(&hdr.pc);

    // write header
    static_assert(sizeof(hdr) <= 256, "header is larger than a single page write");
    xflash_enable_wr();
    xflash_page_program(DUMP_OFFSET, (uint8_t*)&hdr, sizeof(hdr));
    xflash_wait_busy();

    // write data
    static_assert(sizeof(dump_t::data) <= RAMEND+1, "dump area size insufficient");
    xflash_multipage_program(addr, buf, cnt);
}


void xfdump_dump()
{
    dump_header_t buf;
    buf.magic = DUMP_MAGIC;
    buf.regs_present = false;
    buf.crash_reason = (uint8_t)dump_crash_reason::manual;

    // write sram only
    xfdump_dump_core(buf, DUMP_OFFSET + offsetof(dump_t, data.sram),
                     (uint8_t*)RAMSTART, RAMSIZE);
}


void xfdump_full_dump_and_reset(dump_crash_reason reason)
{
    dump_header_t buf;
    buf.magic = DUMP_MAGIC;
    buf.regs_present = true;
    buf.crash_reason = (uint8_t)reason;

    // disable interrupts for a cleaner register dump
    cli();

    // ensure there's always enough time (with some margin) to dump
    // dump time on w25x20cl: ~150ms
    wdt_enable(WDTO_500MS);

    // write all addressable ranges (this will trash bidirectional registers)
    xfdump_dump_core(buf, DUMP_OFFSET + offsetof(dump_t, data), 0, RAMEND+1);

    // force a reset even sooner
    softReset();
}
#endif
