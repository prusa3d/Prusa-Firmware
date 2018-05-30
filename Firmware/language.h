//language.h
#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "config.h"

#define PROTOCOL_VERSION "1.0"

#ifdef CUSTOM_MENDEL_NAME
   // #define CUSTOM_MENDEL_NAME CUSTOM_MENDEL_NAME
#else
    #define MACHINE_NAME "Mendel"
#endif

#ifndef MACHINE_UUID
   #define MACHINE_UUID "00000000-0000-0000-0000-000000000000"
#endif

#define MSG_FW_VERSION                   "Firmware"

#define STRINGIFY_(n) #n
#define STRINGIFY(n) STRINGIFY_(n)

//section progmem0 will be used for localized translated strings
#define PROGMEM_I2 __attribute__((section(".progmem0")))
//section progmem1 will be used for localized strings in english
#define PROGMEM_I1 __attribute__((section(".progmem1")))
//section progmem2 will be used for not localized strings in english
#define PROGMEM_N1 __attribute__((section(".progmem2")))

#if (LANG_MODE == 0) //primary language only
#define _I(s) (__extension__({static const char __c[] PROGMEM_I1 = s; &__c[0];}))
#define ISTR(s) s
#define _i(s) _I(s)
#define _T(s) s
#else //(LANG_MODE == 0)
#define _I(s) (__extension__({static const char __c[] PROGMEM_I1 = "\xff\xff" s; &__c[0];}))
#define ISTR(s) "\xff\xff" s
#define _i(s) lang_get_translation(_I(s))
#define _T(s) lang_get_translation(s)
#endif //(LANG_MODE == 0)
#define _N(s) (__extension__({static const char __c[] PROGMEM_N1 = s; &__c[0];}))
#define _n(s) _N(s)


// Language indices into their particular symbol tables.
#define LANG_ID_PRI 0
#define LANG_ID_SEC 1

// Language is not defined and it shall be selected from the menu.
#define LANG_ID_FORCE_SELECTION 254
// Language is not defined on a virgin RAMBo board.
#define LANG_ID_UNDEFINED 255

// Default language ID, if no language is selected.
#define LANG_ID_DEFAULT LANG_ID_PRI

// Number of languages available in the language table.
#define LANG_NUM 2


#if defined(__cplusplus)
extern "C" {
#endif //defined(__cplusplus)

// Currectly active language selection.
extern unsigned char lang_selected;

#if (LANG_MODE != 0)
extern const char _SEC_LANG[LANG_SIZE_RESERVED];
#endif //(LANG_MODE == 0)

extern const char* lang_get_translation(const char* s);
extern const char* lang_get_sec_lang_str(const char* s);
extern const char* lang_select(unsigned char lang);
extern unsigned char lang_get_count();
extern const char* lang_get_name(unsigned char lang);


#if defined(__cplusplus)
}
#endif //defined(__cplusplus)

#define CAT2(_s1, _s2) _s1
#define CAT4(_s1, _s2, _s3, _s4) _s1

extern const char MSG_LANGUAGE_NAME[];

#include "messages.h"


#endif //LANGUAGE_H

