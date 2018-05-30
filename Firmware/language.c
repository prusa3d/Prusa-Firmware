//language.c
#include "language.h"
#include <avr/pgmspace.h>
#include "bootapp.h"


// Currectly active language selection.
uint8_t lang_selected = 0;

#if (LANG_MODE == 0) //primary language only
#else //(LANG_MODE == 0)
//reserved xx kbytes for secondary language table
const char _SEC_LANG[LANG_SIZE_RESERVED] PROGMEM_I2 = "_SEC_LANG";
#endif //(LANG_MODE == 0)

//lang_table_t structure - 16byte header
typedef struct
{
	struct
	{
		uint32_t magic;      //+0
		uint16_t size;       //+4
		uint16_t count;      //+6
		uint16_t checksum;   //+8
		uint16_t reserved0;  //+10
		uint32_t reserved1;  //+12
	} header;
	uint16_t table[];
} lang_table_t;

//lang_table pointer
lang_table_t* lang_table = 0;


const char* lang_get_translation(const char* s)
{
	if (lang_selected == 0) return s + 2; //primary language selected
	if (lang_table == 0) return s + 2; //sec. lang table not found
	uint16_t ui = pgm_read_word(((uint16_t*)s)); //read string id
	if (ui == 0xffff) return s + 2; //translation not found
	ui = pgm_read_word(((uint16_t*)(((char*)lang_table + 16 + ui*2)))); //read relative offset
	if (pgm_read_byte(((uint8_t*)((char*)lang_table + ui))) == 0)
		return s + 2;//not translated string
	return (const char*)((char*)lang_table + ui); //return calculated pointer
}

const char* lang_get_sec_lang_str(const char* s)
{
	uint16_t ui = (uint16_t)&_SEC_LANG; //pointer to _SEC_LANG reserved space
	ui += 0x00ff; //add 1 page
	ui &= 0xff00; //align to page
	lang_table_t* _lang_table = ui; //table pointer
	ui = pgm_read_word(((uint16_t*)s)); //read string id
	if (ui == 0xffff) return s + 2; //translation not found
	ui = pgm_read_word(((uint16_t*)(((char*)_lang_table + 16 + ui*2)))); //read relative offset
	return (const char*)((char*)_lang_table + ui); //return calculated pointer
}

const char* lang_get_sec_lang_str_by_id(uint16_t id)
{
	uint16_t ui = ((((uint16_t)&_SEC_LANG) + 0x00ff) & 0xff00); //table pointer
	return ui + pgm_read_word(((uint16_t*)(ui + 16 + id * 2))); //read relative offset and return calculated pointer
}

const char* lang_select(uint8_t lang)
{
#if (LANG_MODE == 0) //primary language only
	return 0;
#else //(LANG_MODE == 0)
	if (lang == 0) //primary language
	{
		lang_table = 0;
		lang_selected = 0;
		return;
	}
	uint16_t ui = (uint16_t)&_SEC_LANG; //pointer to _SEC_LANG reserved space
	ui += 0x00ff; //add 1 page
	ui &= 0xff00; //align to page
	lang_table = ui; //set table pointer
	ui = pgm_read_word(((uint16_t*)(((char*)lang_table + 16)))); //read relative offset of first string (language name)
	return (const char*)((char*)lang_table + ui); //return calculated pointer
#endif //(LANG_MODE == 0)
}

uint8_t lang_get_count()
{
	uint16_t ui = (uint16_t)&_SEC_LANG; //pointer to _SEC_LANG reserved space
	ui += 0x00ff; //add 1 page
	ui &= 0xff00; //align to page
	lang_table_t* _lang_table = ui; //table pointer
	if (pgm_read_dword(((uint32_t*)(_lang_table + 0))) == 0x4bb45aa5) return 2;
	return 1;
}

const char* lang_get_name(uint8_t lang)
{
	if (lang == 0) return MSG_LANGUAGE_NAME + 2;
	return lang_get_sec_lang_str(MSG_LANGUAGE_NAME);
}

#ifdef DEBUG_SEC_LANG
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
	printf_P(_n(" _lt_magic        = 0x%08lx %S\n"), _lt_magic, (_lt_magic==0x4bb45aa5)?_n("OK"):_n("NA"));
	printf_P(_n(" _lt_size         = 0x%04x (%d)\n"), _lt_size, _lt_size);
	printf_P(_n(" _lt_count        = 0x%04x (%d)\n"), _lt_count, _lt_count);
	printf_P(_n(" _lt_chsum        = 0x%04x\n"), _lt_chsum);
	printf_P(_n(" _lt_resv0        = 0x%04x\n"), _lt_resv0);
	printf_P(_n(" _lt_resv1        = 0x%08lx\n"), _lt_resv1);
	if (_lt_magic != 0x4bb45aa5) return 0;
	puts_P(_n(" strings:\n"));
	uint16_t ui = ((((uint16_t)&_SEC_LANG) + 0x00ff) & 0xff00); //table pointer
	for (ui = 0; ui < _lt_count; ui++)
		fprintf_P(out, _n("  %3d %S\n"), ui, lang_get_sec_lang_str_by_id(ui));
	return _lt_count;
}
#endif //DEBUG_SEC_LANG


const char MSG_LANGUAGE_NAME[] PROGMEM_I1 = ISTR("English"); ////c=0 r=0

