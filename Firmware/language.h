/** @file */
//language.h
#ifndef LANGUAGE_H
#define LANGUAGE_H


#include "config.h"
#include "macros.h"
#include <inttypes.h>
#ifdef DEBUG_SEC_LANG
    #include <stdio.h>
#endif //DEBUG_SEC_LANG

#define PROTOCOL_VERSION "1.0"

#ifndef CUSTOM_MENDEL_NAME
    #define MACHINE_NAME "Mendel"
#endif

#ifndef MACHINE_UUID
   #define MACHINE_UUID "00000000-0000-0000-0000-000000000000"
#endif

#define MSG_FW_VERSION                   "Firmware"

#if (LANG_MODE == 0) //primary language only
#define PROGMEM_I2 __attribute__((section(".progmem0")))
#define PROGMEM_I1 __attribute__((section(".progmem1")))
#define PROGMEM_N1 __attribute__((section(".progmem2")))
#define _I(s) (__extension__({static const char __c[] PROGMEM_I1 = s; &__c[0];}))
#define ISTR(s) s
#define _i(s) _I(s)
#define _T(s) s
#else //(LANG_MODE == 0)
// section .loc_sec (originaly .progmem0) will be used for localized translated strings
#define PROGMEM_I2 __attribute__((section(".loc_sec")))
// section .loc_pri (originaly .progmem1) will be used for localized strings in english
#define PROGMEM_I1 __attribute__((section(".loc_pri")))
// section .noloc (originaly progmem2) will be used for not localized strings in english
#define PROGMEM_N1 __attribute__((section(".noloc")))
#define _I(s) (__extension__({static const char __c[] PROGMEM_I1 = "\xff\xff" s; &__c[0];}))
#define ISTR(s) "\xff\xff" s
#define _i(s) lang_get_translation(_I(s))
#define _T(s) lang_get_translation(s)
#endif //(LANG_MODE == 0)
#define _N(s) (__extension__({static const char __c[] PROGMEM_N1 = s; &__c[0];}))
#define _n(s) _N(s)

/** @brief lang_table_header_t structure - (size= 16byte) */
typedef struct
{
	uint32_t magic;      //+0 
	uint16_t size;       //+4
	uint16_t count;      //+6
	uint16_t checksum;   //+8
	uint16_t code;       //+10
	uint32_t signature;  //+12
} lang_table_header_t;

/** @brief lang_table_t structure - (size= 16byte + 2*count) */
typedef struct
{
	lang_table_header_t header;
	uint16_t table[];
} lang_table_t;

/** @name Language indices into their particular symbol tables.*/
///@{
#define LANG_ID_PRI 0
#define LANG_ID_SEC 1
///@}

/** @def LANG_ID_FORCE_SELECTION
 *  @brief Language is not defined and it shall be selected from the menu.*/
#define LANG_ID_FORCE_SELECTION 254

/** @def LANG_ID_UNDEFINED
 *  @brief Language is not defined on a virgin RAMBo board. */
#define LANG_ID_UNDEFINED 255

/** @def LANG_ID_DEFAULT
 *  @brief Default language ID, if no language is selected. */
#define LANG_ID_DEFAULT LANG_ID_PRI

/** @def LANG_MAGIC
 *  @brief Magic number at begin of lang table. */
#define LANG_MAGIC 0x4bb45aa5

/** @name Language codes (ISO639-1)*/
///@{
#define LANG_CODE_XX 0x3f3f //!<'??'
#define LANG_CODE_EN 0x656e //!<'en'
#define LANG_CODE_CZ 0x6373 //!<'cs'
#define LANG_CODE_DE 0x6465 //!<'de'
#define LANG_CODE_ES 0x6573 //!<'es'
#define LANG_CODE_FR 0x6672 //!<'fr'
#define LANG_CODE_IT 0x6974 //!<'it'
#define LANG_CODE_PL 0x706c //!<'pl'
#ifdef COMMUNITY_LANG_SUPPORT //Community language support
#ifdef COMMUNITY_LANG_NL
#define LANG_CODE_NL 0x6e6c //!<'nl'
#endif // COMMUNITY_LANG_NL
//Use the 3 lines below as a template and replace 'QR', '0X7172' and 'qr'
//#ifdef COMMUNITY_LANG_QR
//#define LANG_CODE_QR 0x7172 //!<'qr'
//#endif // COMMUNITY_LANG_QR
#endif // COMMUNITY_LANG_SUPPORT
///@}

#if defined(__cplusplus)
extern "C" {
#endif //defined(__cplusplus)

/** @brief Currectly active language selection.*/
extern uint8_t lang_selected;

#if (LANG_MODE != 0)
extern const char _SEC_LANG[LANG_SIZE_RESERVED];
extern const char* lang_get_translation(const char* s);
/** @def _SEC_LANG_TABLE
 *  @brief Align table to start of 256 byte page */
#define _SEC_LANG_TABLE ((((uint16_t)&_SEC_LANG) + 0x00ff) & 0xff00)
#endif //(LANG_MODE != 0)

/** @brief selects language, eeprom is updated in case of success */
extern uint8_t lang_select(uint8_t lang);
/** @brief performs checksum test of secondary language data */
extern uint8_t lang_check(uint16_t addr);
/** @return total number of languages (primary + all in xflash) */
extern uint8_t lang_get_count(void);
/** @brief reads lang table header and offset in xflash or progmem */
extern uint8_t lang_get_header(uint8_t lang, lang_table_header_t* header, uint32_t* offset);
/** @brief reads lang code from xflash or progmem */
extern uint16_t lang_get_code(uint8_t lang);
/** @return localized language name (text for menu item) */
extern const char* lang_get_name_by_code(uint16_t code);
/** @brief reset language to "LANG_ID_FORCE_SELECTION", epprom is updated */
extern void lang_reset(void);
/** @retval 1 language is selected */
extern uint8_t lang_is_selected(void);

#ifdef DEBUG_SEC_LANG
extern const char* lang_get_sec_lang_str_by_id(uint16_t id);
extern uint16_t lang_print_sec_lang(FILE* out);
#endif //DEBUG_SEC_LANG

extern void lang_boot_update_start(uint8_t lang);

#if defined(__cplusplus)
}
#endif //defined(__cplusplus)

#define CAT2(_s1, _s2) _s1
#define CAT4(_s1, _s2, _s3, _s4) _s1

#include "messages.h"


#endif //LANGUAGE_H

