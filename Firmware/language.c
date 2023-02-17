//language.c
#include "language.h"
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include "bootapp.h"

#include "Configuration.h"
#include "pins.h"

#ifdef XFLASH
#include "xflash.h"
#include "xflash_layout.h"
#endif //XFLASH

// Currently active language selection.
uint8_t lang_selected = 0;


#if (LANG_MODE == 0) //primary language only

uint8_t lang_select(_UNUSED uint8_t lang) { return 0; }
uint8_t lang_get_count() { return 1; }
uint16_t lang_get_code(_UNUSED uint8_t lang) { return LANG_CODE_EN; }
const char* lang_get_name_by_code(_UNUSED uint16_t code) { return _n("English"); }
void lang_reset(void) { }
uint8_t lang_is_selected(void) { return 1; }

#else //(LANG_MODE == 0) //secondary languages in progmem or xflash

//reserved xx kbytes for secondary language table
const char _SEC_LANG[LANG_SIZE_RESERVED] __attribute__((aligned(SPM_PAGESIZE))) PROGMEM_I2 = "_SEC_LANG";

//primary language signature
const uint32_t _PRI_LANG_SIGNATURE[1] __attribute__((section(".progmem0"))) = {0xffffffff};

//lang_table pointer
lang_table_t* lang_table = 0;

const char* lang_get_translation(const char* s)
{
	if (lang_selected == 0) return s + 2; //primary language selected, return orig. str.
	if (lang_table == 0) return s + 2; //sec. lang table not found, return orig. str.
	uint16_t ui = pgm_read_word(((uint16_t*)s)); //read string id
	if (ui == 0xffff) return s + 2; //id not assigned, return orig. str.
	ui = pgm_read_word(((uint16_t*)(((char*)lang_table + 16 + ui*2)))); //read relative offset
	if (pgm_read_byte(((uint8_t*)((char*)lang_table + ui))) == 0) //read first character
		return s + 2;//zero length string == not translated, return orig. str.
	return (const char*)((char*)lang_table + ui); //return calculated pointer
}

uint8_t lang_select(uint8_t lang)
{
	if (lang == LANG_ID_PRI) //primary language
	{
		lang_table = 0;
		lang_selected = lang;
	}
#ifdef XFLASH
	if (lang_get_code(lang) == lang_get_code(LANG_ID_SEC)) lang = LANG_ID_SEC;
	if (lang == LANG_ID_SEC) //current secondary language
	{
		if (pgm_read_dword(((uint32_t*)_SEC_LANG_TABLE)) == LANG_MAGIC) //magic valid
		{
			if (lang_check(_SEC_LANG_TABLE))
				if (pgm_read_dword(((uint32_t*)(_SEC_LANG_TABLE + 12))) == pgm_read_dword(((uint32_t*)(_PRI_LANG_SIGNATURE)))) //signature valid
				{
					lang_table = (lang_table_t*)(_SEC_LANG_TABLE); // set table pointer
					lang_selected = lang; // set language id
				}
		}
	}
#else //XFLASH
	if (lang == LANG_ID_SEC)
	{
		uint16_t table = _SEC_LANG_TABLE;
		if (pgm_read_dword(((uint32_t*)table)) == LANG_MAGIC) //magic valid
		{
			if (lang_check(table))
				if (pgm_read_dword(((uint32_t*)(table + 12))) == pgm_read_dword(((uint32_t*)(_PRI_LANG_SIGNATURE)))) //signature valid
				{
					lang_table = (lang_table_t*)table; // set table pointer
					lang_selected = lang; // set language id
				}
		}
	}
#endif //XFLASH
	if (lang_selected == lang)
	{
		eeprom_update_byte((unsigned char*)EEPROM_LANG, lang_selected);
		return 1;
	}
	return 0;
}

uint8_t lang_check(uint16_t addr)
{
	uint16_t sum = 0;
	uint16_t size = pgm_read_word((uint16_t*)(addr + 4));
	uint16_t lt_sum = pgm_read_word((uint16_t*)(addr + 8));
	uint16_t i; for (i = 0; i < size; i++)
		sum += (uint16_t)pgm_read_byte((uint8_t*)(addr + i)) << ((i & 1)?0:8);
	sum -= lt_sum; //subtract checksum
	sum = (sum >> 8) | ((sum & 0xff) << 8); //swap bytes
	return (sum == lt_sum);
}

uint8_t lang_get_count()
{
	if (pgm_read_dword(((uint32_t*)(_PRI_LANG_SIGNATURE))) == 0xffffffff)
		return 1; //signature not set - only primary language will be available
#ifdef XFLASH
	XFLASH_SPI_ENTER();
	uint8_t count = 2; //count = 1+n (primary + secondary + all in xflash)
	uint32_t addr = LANG_OFFSET;
	lang_table_header_t header; //table header structure
	while (1)
	{
		xflash_rd_data(addr, (uint8_t*)&header, sizeof(lang_table_header_t)); //read table header from xflash
		if (header.magic != LANG_MAGIC) break; //break if magic not valid
		addr += header.size; //calc address of next table
		count++; //inc counter
	}
#else //XFLASH
	uint16_t table = _SEC_LANG_TABLE;
	uint8_t count = 1; //count = 1 (primary)
	while (pgm_read_dword(((uint32_t*)table)) == LANG_MAGIC) //magic valid
	{
		table += pgm_read_word((uint16_t*)(table + 4));
		count++;
	}
#endif //XFLASH
	return count;
}

uint8_t lang_get_header(uint8_t lang, lang_table_header_t* header, uint32_t* offset)
{
	if (lang == LANG_ID_PRI) return 0; //primary lang not supported for this function
#ifdef XFLASH
	if (lang == LANG_ID_SEC)
	{
		uint16_t ui = _SEC_LANG_TABLE; //table pointer
		memcpy_P(header, (lang_table_t*)(_SEC_LANG_TABLE), sizeof(lang_table_header_t)); //read table header from progmem
		if (offset) *offset = ui;
		return (header->magic == LANG_MAGIC)?1:0; //return 1 if magic valid
	}
	XFLASH_SPI_ENTER();
	uint32_t addr = LANG_OFFSET;
	lang--;
	while (1)
	{
		xflash_rd_data(addr, (uint8_t*)(header), sizeof(lang_table_header_t)); //read table header from xflash
		if (header->magic != LANG_MAGIC) break; //break if not valid
		if (offset) *offset = addr;
		if (--lang == 0) return 1;
		addr += header->size; //calc address of next table
	}
#else //XFLASH
	if (lang == LANG_ID_SEC)
	{
		uint16_t ui = _SEC_LANG_TABLE; //table pointer
		memcpy_P(header, (lang_table_header_t*)ui, sizeof(lang_table_header_t)); //read table header from progmem
		if (offset) *offset = ui;
		return (header->magic == LANG_MAGIC)?1:0; //return 1 if magic valid
	}
#endif //XFLASH
	return 0;
}

uint16_t lang_get_code(uint8_t lang)
{
	if (lang == LANG_ID_PRI) return LANG_CODE_EN; //primary lang = EN
#ifdef XFLASH
	if (lang == LANG_ID_SEC)
	{
		uint16_t ui = _SEC_LANG_TABLE; //table pointer
		if (pgm_read_dword(((uint32_t*)(ui + 0))) != LANG_MAGIC) return LANG_CODE_XX; //magic not valid
		return pgm_read_word(((uint32_t*)(ui + 10))); //return lang code from progmem
	}
	XFLASH_SPI_ENTER();
	uint32_t addr = LANG_OFFSET;
	lang_table_header_t header; //table header structure
	lang--;
	while (1)
	{
		xflash_rd_data(addr, (uint8_t*)&header, sizeof(lang_table_header_t)); //read table header from xflash
		if (header.magic != LANG_MAGIC) break; //break if not valid
		if (--lang == 0) return header.code;
		addr += header.size; //calc address of next table
	}
#else //XFLASH
	uint16_t table = _SEC_LANG_TABLE;
	uint8_t count = 1; //count = 1 (primary)
	while (pgm_read_dword((uint32_t*)table) == LANG_MAGIC) //magic valid
	{
		if (count == lang) return pgm_read_word(((uint16_t*)(table + 10))); //read language code
		table += pgm_read_word((uint16_t*)(table + 4));
		count++;
	}
#endif //XFLASH
	return LANG_CODE_XX;
}

const char* lang_get_name_by_code(uint16_t code)
{
	switch (code)
	{
	case LANG_CODE_EN: return _n("English");
	case LANG_CODE_CZ: return _n("Cestina");
	case LANG_CODE_DE: return _n("Deutsch");
	case LANG_CODE_ES: return _n("Espanol");
	case LANG_CODE_FR: return _n("Francais");
	case LANG_CODE_IT: return _n("Italiano");
	case LANG_CODE_PL: return _n("Polski");
#ifdef COMMUNITY_LANGUAGE_SUPPORT //Community language support
#ifdef COMMUNITY_LANG_GROUP1_NL
	case LANG_CODE_NL: return _n("Nederlands"); //community Dutch contribution
#endif // COMMUNITY_LANG_GROUP1_NL
#ifdef COMMUNITY_LANG_GROUP1_SV
	case LANG_CODE_SV: return _n("Svenska"); //community Swedish contribution
#endif // COMMUNITY_LANG_GROUP1_SV
#ifdef COMMUNITY_LANG_GROUP1_NO
	case LANG_CODE_NO: return _n("Norsk"); //community Swedish contribution
#endif // COMMUNITY_LANG_GROUP1_NO
#ifdef COMMUNITY_LANG_GROUP1_DA
	case LANG_CODE_DA: return _n("Dansk"); //community Danish contribution
#endif // COMMUNITY_LANG_GROUP1_DA
#ifdef COMMUNITY_LANG_GROUP1_SK
	case LANG_CODE_SK: return _n("Slovencina"); //community Slovak contribution
#endif // COMMUNITY_LANG_GROUP1_SK
#ifdef COMMUNITY_LANG_GROUP1_SL
	case LANG_CODE_SL: return _n("Slovenscina"); //community Slovanian contribution
#endif // COMMUNITY_LANG_GROUP1_SL
#ifdef COMMUNITY_LANG_GROUP1_HU
	case LANG_CODE_HU: return _n("Magyar"); //community Hungarian contribution
#endif // COMMUNITY_LANG_GROUP1_HU
#ifdef COMMUNITY_LANG_GROUP1_LB
	case LANG_CODE_LB: return _n("Letzebuergesch"); //community Luxembourgish contribution
#endif // COMMUNITY_LANG_GROUP1_LB
#ifdef COMMUNITY_LANG_GROUP1_HR
	case LANG_CODE_HR: return _n("Hrvatski"); //community Croatian contribution
#endif // COMMUNITY_LANG_GROUP1_HR
#ifdef COMMUNITY_LANG_GROUP2_LT
	case LANG_CODE_LT: return _n("Lietuviu"); //community Lithuanian contribution
#endif // COMMUNITY_LANG_GROUP2_LT
#ifdef COMMUNITY_LANG_GROUP1_RO
	case LANG_CODE_RO: return _n("Romana"); //community Romanian contribution
#endif // COMMUNITY_LANG_GROUP1_RO

//Use the 3 lines below as a template and replace 'QR' and 'New language'
//#ifdef COMMUNITY_LANG_GROUP1_QR 
//	case LANG_CODE_QR: return _n("New language"); //community contribution
//#endif // COMMUNITY_LANG_GROUP1_QR
#endif // COMMUNITY_LANGUAGE_SUPPORT
	}
	return _n("??");
}

void lang_reset(void)
{
	lang_selected = 0;
	eeprom_update_byte((unsigned char*)EEPROM_LANG, LANG_ID_FORCE_SELECTION);
}

uint8_t lang_is_selected(void)
{
	uint8_t lang_eeprom = eeprom_read_byte((unsigned char*)EEPROM_LANG);
	return (lang_eeprom != LANG_ID_FORCE_SELECTION) && (lang_eeprom == lang_selected);
}

#ifdef DEBUG_SEC_LANG
#include <stdio.h>
const char* lang_get_sec_lang_str_by_id(uint16_t id)
{
	uint16_t ui = _SEC_LANG_TABLE; //table pointer
	return ui + pgm_read_word(((uint16_t*)(ui + 16 + id * 2))); //read relative offset and return calculated pointer
}

uint16_t lang_print_sec_lang(FILE* out)
{
	printf_P(_n("&_SEC_LANG        = 0x%04x\n"), &_SEC_LANG);
	printf_P(_n("sizeof(_SEC_LANG) = 0x%04x\n"), sizeof(_SEC_LANG));
	uint16_t ptr_lang_table0 = ((uint16_t)(&_SEC_LANG) + 0xff) & 0xff00;
	printf_P(_n("&_lang_table0     = 0x%04x\n"), ptr_lang_table0);
	uint32_t _lt_magic = pgm_read_dword(((uint32_t*)(ptr_lang_table0 + 0)));
	uint16_t _lt_size = pgm_read_word(((uint16_t*)(ptr_lang_table0 + 4)));
	uint16_t _lt_count = pgm_read_word(((uint16_t*)(ptr_lang_table0 + 6)));
	uint16_t _lt_chsum = pgm_read_word(((uint16_t*)(ptr_lang_table0 + 8)));
	uint16_t _lt_resv0 = pgm_read_word(((uint16_t*)(ptr_lang_table0 + 10)));
	uint32_t _lt_resv1 = pgm_read_dword(((uint32_t*)(ptr_lang_table0 + 12)));
	printf_P(_n(" _lt_magic        = 0x%08lx %S\n"), _lt_magic, (_lt_magic==LANG_MAGIC)?_n("OK"):_n("NA"));
	printf_P(_n(" _lt_size         = 0x%04x (%d)\n"), _lt_size, _lt_size);
	printf_P(_n(" _lt_count        = 0x%04x (%d)\n"), _lt_count, _lt_count);
	printf_P(_n(" _lt_chsum        = 0x%04x\n"), _lt_chsum);
	printf_P(_n(" _lt_resv0        = 0x%04x\n"), _lt_resv0);
	printf_P(_n(" _lt_resv1        = 0x%08lx\n"), _lt_resv1);
	if (_lt_magic != LANG_MAGIC) return 0;
	puts_P(_n(" strings:\n"));
	uint16_t ui = _SEC_LANG_TABLE; //table pointer
	for (ui = 0; ui < _lt_count; ui++)
		fprintf_P(out, _n("  %3d %S\n"), ui, lang_get_sec_lang_str_by_id(ui));
	return _lt_count;
}
#endif //DEBUG_SEC_LANG

#endif //(LANG_MODE == 0)


void lang_boot_update_start(uint8_t lang)
{
	uint8_t cnt = lang_get_count();
	if ((lang < 2) || (lang > cnt)) return; //only languages from xflash can be selected
	bootapp_reboot_user0(lang << 3);
}
