#include <stddef.h>

#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "xflash_dump.h"
#ifdef XFLASH_DUMP
#include "xflash.h"


bool xfdump_check_state()
{
    uint32_t magic;

    XFLASH_SPI_ENTER();
    xflash_rd_data(DUMP_OFFSET + offsetof(dump_t, header.magic),
                   (uint8_t*)&magic, sizeof(magic));

    return magic == DUMP_MAGIC;
}


bool xfdump_check_crash()
{
    // check_state will call SPI_ENTER for us
    if(!xfdump_check_state())
        return false;

    uint8_t crash;
    xflash_rd_data(DUMP_OFFSET + offsetof(dump_t, header.crash),
                   (uint8_t*)&crash, sizeof(crash));
    return crash;
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
    XFLASH_SPI_ENTER();
    for(uint32_t addr = DUMP_OFFSET;
        addr < DUMP_OFFSET + DUMP_SIZE;
        addr += 4096)
    {
        xflash_enable_wr();
        xflash_sector_erase(DUMP_OFFSET);
        xflash_wait_busy();
    }
}


static void xfdump_dump_core(dump_header_t& hdr, uint32_t addr, uint8_t* buf, uint16_t cnt)
{
    XFLASH_SPI_ENTER();

    // start by clearing all sectors (we need all of them in any case)
    xfdump_erase();

    // write header
    static_assert(sizeof(hdr) < 256, "header is larger than a single page write");
    xflash_enable_wr();
    xflash_page_program(DUMP_OFFSET, (uint8_t*)&hdr, sizeof(hdr));
    xflash_wait_busy();

    // write data
    xflash_multipage_program(addr, buf, cnt);
}


void xfdump_dump()
{
    dump_header_t buf;
    buf.magic = DUMP_MAGIC;
    buf.regs_present = false;
    buf.crash = false;

    // write sram only
    xfdump_dump_core(buf, DUMP_OFFSET + offsetof(dump_t, data.sram),
                     (uint8_t*)SRAM_START, SRAM_SIZE);
}


void xfdump_full_dump_and_reset(bool crash)
{
    dump_header_t buf;
    buf.magic = DUMP_MAGIC;
    buf.regs_present = true;
    buf.crash = crash;

    // disable interrupts for a cleaner register dump
    cli();

    // write all addressable ranges (this will trash bidirectional registers)
    xfdump_dump_core(buf, DUMP_OFFSET + offsetof(dump_t, data),
                     0, SRAM_START + SRAM_SIZE);

    // force a reset soon
    wdt_enable(0);
    while(true);
}
#endif
