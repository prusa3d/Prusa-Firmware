//language.c
#include "language.h"
#include <inttypes.h>
#include <avr/pgmspace.h>


// Currectly active language selection.
unsigned char lang_selected = 0;

#if (LANG_MODE == 0) //primary language only
#else //(LANG_MODE == 0)
//reserved xx kbytes for secondary language table
static const char _SEC_LANG[LANG_SIZE_RESERVED] PROGMEM_I2 = "_SEC_LANG";
#endif //(LANG_MODE == 0)

//lang_table_t structure - 16byte header
typedef struct
{
	struct
	{
		uint32_t magic;
		uint16_t size;
		uint16_t count;
		uint16_t checksum;
		uint16_t reserved0;
		uint32_t reserved1;
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
	return (const char*)((char*)lang_table + ui + 16); //return calculated pointer
}

const char* lang_select(unsigned char lang)
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
	ui += 0x0100; //add 1 page
	ui &= 0xff00; //align to page
	lang_table = ui; //set table pointer
	ui = pgm_read_word(((uint16_t*)(((char*)lang_table + 16)))); //read relative offset of first string (language name)
	return (const char*)((char*)lang_table + ui + 16); //return calculated pointer
#endif //(LANG_MODE == 0)
}