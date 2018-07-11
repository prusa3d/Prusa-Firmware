//bootapp.c
#include "bootapp.h"
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>


/*
void bootapp_print_vars()
{
	printf_P(_n("boot_src_addr  =0x%08lx\n"), boot_src_addr);
	printf_P(_n("boot_dst_addr  =0x%08lx\n"), boot_dst_addr);
	printf_P(_n("boot_copy_size =0x%04x\n"), boot_copy_size);
	printf_P(_n("boot_reserved  =0x%02x\n"), boot_reserved);
	printf_P(_n("boot_app_flags =0x%02x\n"), boot_app_flags);
	printf_P(_n("boot_app_magic =0x%08lx\n"), boot_app_magic);
}
*/

void bootapp_ram2flash(uint16_t rptr, uint16_t fptr, uint16_t size)
{
	cli();
	boot_app_magic = BOOT_APP_MAGIC;
	boot_app_flags |= BOOT_APP_FLG_COPY;
	boot_app_flags |= BOOT_APP_FLG_ERASE;
/*	uint16_t ui; for (ui = 0; ui < size; ui++)
	{
		uint8_t uc = ram_array[ui+rptr];
		if (pgm_read_byte(ui+fptr) & uc != uc)
		{
			boot_app_flags |= BOOT_APP_FLG_ERASE;
			break;
		}
	}*/
	boot_copy_size = (uint16_t)size;
	boot_src_addr = (uint32_t)rptr;
	boot_dst_addr = (uint32_t)fptr;
	wdt_enable(WDTO_15MS);
	while(1);
}

void bootapp_reboot_user0(uint8_t reserved)
{
	cli();
	boot_app_magic = BOOT_APP_MAGIC;
	boot_app_flags = BOOT_APP_FLG_USER0;
	boot_reserved = reserved;
	wdt_enable(WDTO_15MS);
	while(1);
}
