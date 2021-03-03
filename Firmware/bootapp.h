//language.h
#ifndef BOOTAPP_H
#define BOOTAPP_H

#include "config.h"
#include <inttypes.h>

// This doesn't look correct as the end of RAM is at 0x21ff
// thus it looks like the important boot variables are actually located in the middle of stack
// -> may be the root cause of mysterious fails of language uploads and boot loops
// Moreover, it looks like we cannot fix it as the bootloader probably uses the same memory area,
// but at least we can copy the vars to a safe location right after the FW starts 
// before the stack gets filled with stuff and overwrites the vars
#define RAMSIZE        0x2000
#define ram_array ((uint8_t*)(0))
#define boot_src_addr  (*((uint32_t*)(RAMSIZE - 16)))
#define boot_dst_addr  (*((uint32_t*)(RAMSIZE - 12)))
#define boot_copy_size (*((uint16_t*)(RAMSIZE - 8)))
#define boot_reserved_in_the_middle_of_stack  (*((uint8_t*)(RAMSIZE - 6)))
#define boot_app_flags_in_the_middle_of_stack (*((uint8_t*)(RAMSIZE - 5)))
#define boot_app_magic_in_the_middle_of_stack (*((uint32_t*)(RAMSIZE - 4)))
#define BOOT_APP_FLG_ERASE 0x01
#define BOOT_APP_FLG_COPY  0x02
#define BOOT_APP_FLG_FLASH 0x04
#define BOOT_APP_FLG_RUN 0x08

#define BOOT_APP_FLG_USER0 0x80

#define BOOT_APP_MAGIC 0x55aa55aa


#if defined(__cplusplus)
extern "C" {
#endif //defined(__cplusplus)

extern uint8_t boot_reserved_copy_after_start;
extern uint8_t boot_app_flags_copy_after_start;
extern uint32_t boot_app_magic_copy_after_start;

extern void bootapp_print_vars(void);

extern void bootapp_ram2flash(uint16_t rptr, uint16_t fptr, uint16_t size);

extern void bootapp_reboot_user0(uint8_t reserved);

#if defined(__cplusplus)
}
#endif //defined(__cplusplus)

#endif //BOOTAPP_H
