//bootapp.c
#include "bootapp.h"
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>


#include <stdio.h>

extern void softReset();

void bootapp_print_vars(void)
{
	printf_P(PSTR("boot_src_addr  =0x%08lx\n"), boot_src_addr);
	printf_P(PSTR("boot_dst_addr  =0x%08lx\n"), boot_dst_addr);
	printf_P(PSTR("boot_copy_size =0x%04x\n"), boot_copy_size);
	printf_P(PSTR("boot_reserved  =0x%02x\n"), boot_reserved);
	printf_P(PSTR("boot_app_flags =0x%02x\n"), boot_app_flags);
	printf_P(PSTR("boot_app_magic =0x%08lx\n"), boot_app_magic);
}


void bootapp_ram2flash(uint16_t rptr, uint16_t fptr, uint16_t size)
{
	cli();
	boot_app_magic = BOOT_APP_MAGIC;
	boot_app_flags |= BOOT_APP_FLG_COPY | BOOT_APP_FLG_ERASE;
	boot_copy_size = (uint16_t)size;
	boot_src_addr = (uint32_t)rptr;
	boot_dst_addr = (uint32_t)fptr;
	// bootapp_print_vars();
	softReset();
}

void bootapp_reboot_user0(uint8_t reserved)
{
	cli();
	boot_app_magic = BOOT_APP_MAGIC;
	boot_app_flags = BOOT_APP_FLG_USER0;
	boot_copy_size = 0;
	boot_reserved = reserved;
	// bootapp_print_vars();
	softReset();
}
