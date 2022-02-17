
/**
 * @file
 * @author leptun
 */
 /** \ingroup xflash_layout */
 
 //! This is the layout of the XFLASH (external SPI flash) in Prusa firmware (dynamically generated from doxygen).


/** @defgroup xflash_layout XFLASH Layout
 *  
 
  ---------------------------------------------------------------------------------
  The XFLASH has the following alignment requirements:
   - Block erase of 64KB. This is what the second bootloader uses. If anything even starts writing to a block, the entire block is erased by the bootloader. It will cause loss of crash dump on firmware upload. Nothing more than that.
   - Block erase of 32KB. Not used.
   - Sector erase of 4KB. Used by the xflash_dump. This is the minimum size for erasing and as such the dump is 4KB aligned as to not erase other stuff unintentionally.
   - Page write of 256B. Lower access can be used, but care must be used since the address wraps at the page boundary when writing.
   - Read has no alignment requirements.
  
  The following items are found in the xflash:
  
  ### 1. Languages (R)
    This is a variable size region that is built by the lang build scripts. More info can be found in those scripts.
    
    It is aligned at the beginning of xflash, offset 0.

  ### 2. MMU Firmware update files (64KB, R)
    This space is reserved for the two MMU firmware files:
     - The MMU firmware v2.0.0+.
     - The Bootloader self update file.
   
   It is aligned at the end of xflash, before xflash_dump

  ### 3. xflash_dump (12KB, RW)
    The crash dump structure is defined as dump_t.
    It composes of:
     - A header with some information such as crash reason and what info was dumped.
     - The dump itself. This is composed of the entire memory space from address 0 to the end of SRAM. So it includes also the registers (useless), the IO and extended IO (useful) and all RAM.
    
    Even though the dump needs around 9KB of storage, 12KB is used because of the sector erase size.
    
    It is aligned at the end of xflash.
*/


// XFLASH memory layout
#pragma once
#include <stdint.h>
#include "bootapp.h" // for RAMSIZE
#include "config.h"

#define XFLASH_SIZE 0x40000ul // size of XFLASH

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

#define DUMP_OFFSET ((XFLASH_SIZE - sizeof(dump_t)) & ~0xFFFul) // dump offset must be aligned to lower 4kb sector boundary
#define MMU_BOOTLOADER_UPDATE_OFFSET (DUMP_OFFSET - 32768) // 32KB of MMU bootloader self update.
#define MMU_FW_UPDATE_OFFSET (MMU_BOOTLOADER_UPDATE_OFFSET - 32768) // 32KB of MMU fw.
#define LANG_OFFSET 0x0 // offset for language data

#define LANG_SIZE (MMU_FW_UPDATE_OFFSET - LANG_OFFSET) // available language space
#define DUMP_SIZE (XFLASH_SIZE - DUMP_OFFSET) // effective dump size area
