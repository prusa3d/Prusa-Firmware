// XFLASH memory layout
#pragma once
#include <stdint.h>
#include "bootapp.h" // for RAMSIZE
#include "config.h"

#define XFLASH_SIZE 0x40000ul // size of XFLASH
#define LANG_OFFSET 0x0       // offset for language data

#ifndef XFLASH_DUMP
#define LANG_SIZE   XFLASH_SIZE
#else

#define DUMP_MAGIC  0x55525547ul

struct dump_header_t
{
    // start with a magic value to indicate the presence of a dump, so that clearing
    // a single page is sufficient for resetting the state
    uint32_t magic;

    uint8_t regs_present; // true when the lower segment containing registers is present
    uint8_t crash_reason; // uses values from dump_crash_source

    uint32_t pc;          // PC nearby the crash location
    uint16_t sp;          // SP nearby the crash location
};

struct dump_data_t
{
    // contiguous region containing all addressable ranges
    uint8_t regs[RAMSTART];
    uint8_t sram[RAMSIZE];
};

struct dump_t
{
    struct dump_header_t header;

    // data is page aligned (no real space waste, due to the larger
    // alignment required for the whole dump)
    struct dump_data_t __attribute__((aligned(256))) data;
};

// dump offset must be aligned to lower 4kb sector boundary
#define DUMP_OFFSET ((XFLASH_SIZE - sizeof(dump_t)) & ~0xFFFul)

#define DUMP_SIZE   (XFLASH_SIZE - DUMP_OFFSET) // effective dump size area
#define LANG_SIZE   DUMP_OFFSET                 // available language space

#endif
