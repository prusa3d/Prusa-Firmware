//! @file
//! @date Jun 20, 2019
//! @author Marek Běl

#include "eeprom.h"
#include "Marlin.h"

#include <avr/eeprom.h>
#include <stdint.h>


#include "language.h"

void eeprom_init()
{
    eeprom_init_default_byte((uint8_t*)EEPROM_POWER_COUNT, 0);
    eeprom_init_default_byte((uint8_t*)EEPROM_CRASH_COUNT_X, 0);
    eeprom_init_default_byte((uint8_t*)EEPROM_CRASH_COUNT_Y, 0);
    eeprom_init_default_byte((uint8_t*)EEPROM_FERROR_COUNT, 0);
    eeprom_init_default_word((uint16_t*)EEPROM_POWER_COUNT_TOT, 0);
    eeprom_init_default_word((uint16_t*)EEPROM_CRASH_COUNT_X_TOT, 0);
    eeprom_init_default_word((uint16_t*)EEPROM_CRASH_COUNT_Y_TOT, 0);
    eeprom_init_default_word((uint16_t*)EEPROM_FERROR_COUNT_TOT, 0);

    eeprom_init_default_word((uint16_t*)EEPROM_MMU_FAIL_TOT, 0);
    eeprom_init_default_word((uint16_t*)EEPROM_MMU_LOAD_FAIL_TOT, 0);
    eeprom_init_default_byte((uint8_t*)EEPROM_MMU_FAIL, 0);
    eeprom_init_default_byte((uint8_t*)EEPROM_MMU_LOAD_FAIL, 0);
    eeprom_init_default_dword((uint32_t*)EEPROM_MMU_MATERIAL_CHANGES, 0);
    if (eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)) == EEPROM_EMPTY_VALUE)
    {
        eeprom_update_byte(&(EEPROM_Sheets_base->active_sheet), 0);
        // When upgrading from version older version (before multiple sheets were implemented in v3.8.0)
        // Sheet 1 uses the previous Live adjust Z (@EEPROM_BABYSTEP_Z)
        int last_babystep = eeprom_read_word((uint16_t *)EEPROM_BABYSTEP_Z);
        eeprom_update_word(reinterpret_cast<uint16_t *>(&(EEPROM_Sheets_base->s[0].z_offset)), last_babystep);
    }
    
    // initialize the sheet names in eeprom
    for (uint_least8_t i = 0; i < (sizeof(Sheets::s)/sizeof(Sheets::s[0])); i++) {
        SheetName sheetName;
        eeprom_default_sheet_name(i, sheetName);
        eeprom_init_default_block(EEPROM_Sheets_base->s[i].name, (sizeof(Sheet::name)/sizeof(Sheet::name[0])), sheetName.c);
    }

    if(!eeprom_is_sheet_initialized(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet))))
    {
        eeprom_switch_to_next_sheet();
    }
    check_babystep();

#ifdef PINDA_TEMP_COMP
    eeprom_init_default_byte((uint8_t*)EEPROM_PINDA_TEMP_COMPENSATION, 0);
#endif //PINDA_TEMP_COMP

    eeprom_init_default_dword((uint32_t*)EEPROM_JOB_ID, 0);
    eeprom_init_default_dword((uint32_t*)EEPROM_TOTALTIME, 0);
    eeprom_init_default_dword((uint32_t*)EEPROM_FILAMENTUSED, 0);
    eeprom_init_default_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED, 0);

    eeprom_init_default_byte((uint8_t*)EEPROM_HEAT_BED_ON_LOAD_FILAMENT, 1);

}

void eeprom_adjust_bed_reset() {
    eeprom_update_byte((uint8_t*)EEPROM_BED_CORRECTION_VALID, 1);
    eeprom_update_byte((uint8_t*)EEPROM_BED_CORRECTION_LEFT, 0);
    eeprom_update_byte((uint8_t*)EEPROM_BED_CORRECTION_RIGHT, 0);
    eeprom_update_byte((uint8_t*)EEPROM_BED_CORRECTION_FRONT, 0);
    eeprom_update_byte((uint8_t*)EEPROM_BED_CORRECTION_REAR, 0);
}

//! @brief Get default sheet name for index
//!
//! | index | sheetName |
//! | ----- | --------- |
//! | 0     | Smooth1   |
//! | 1     | Smooth2   |
//! | 2     | Textur1   |
//! | 3     | Textur2   |
//! | 4     | Satin     |
//! | 5     | NylonPA   |
//! | 6     | Custom1   |
//! | 7     | Custom2   |
//!
//! @param[in] index
//! @param[out] sheetName
void eeprom_default_sheet_name(uint8_t index, SheetName &sheetName)
{
    static_assert(8 == sizeof(SheetName),"Default sheet name needs to be adjusted.");

    if (index < 2)
    {
        strcpy_P(sheetName.c, PSTR("Smooth"));
    }
    else if (index < 4)
    {
        strcpy_P(sheetName.c, PSTR("Textur"));
    }
    else if (index < 5)
    {
        strcpy_P(sheetName.c, PSTR("Satin  "));
    }
    else if (index < 6)
    {
        strcpy_P(sheetName.c, PSTR("NylonPA"));
    }
    else
    {
        strcpy_P(sheetName.c, PSTR("Custom"));
    }
    if (index <4 || index >5)
    {
        sheetName.c[6] = '0' + ((index % 2)+1);
        sheetName.c[7] = '\0';
    }
}

//! @brief Get next initialized sheet
//!
//! If current sheet is the only sheet initialized, current sheet is returned.
//!
//! @param sheet Current sheet
//! @return next initialized sheet
//! @retval -1 no sheet is initialized
int8_t eeprom_next_initialized_sheet(int8_t sheet)
{
    for (int8_t i = 0; i < static_cast<int8_t>(sizeof(Sheets::s)/sizeof(Sheet)); ++i)
    {
        ++sheet;
        if (sheet >= static_cast<int8_t>(sizeof(Sheets::s)/sizeof(Sheet))) sheet = 0;
        if (eeprom_is_sheet_initialized(sheet)) return sheet;
    }
    return -1;
}

void eeprom_switch_to_next_sheet()
{
    int8_t sheet = eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet));

    sheet = eeprom_next_initialized_sheet(sheet);
    if (sheet >= 0) eeprom_update_byte(&(EEPROM_Sheets_base->active_sheet), sheet);
}

bool __attribute__((noinline)) eeprom_is_sheet_initialized(uint8_t sheet_num) {
    return (eeprom_read_word(reinterpret_cast<uint16_t*>(&(EEPROM_Sheets_base->s[sheet_num].z_offset))) != EEPROM_EMPTY_VALUE16);
}


bool __attribute__((noinline)) eeprom_is_initialized_block(const void *__p, size_t __n) {
    const uint8_t *p = (const uint8_t*)__p;
    while (__n--) {
        if (eeprom_read_byte(p++) != EEPROM_EMPTY_VALUE)
            return true;
    }
    return false;
}

void eeprom_update_block_P(const void *__src, void *__dst, size_t __n) {
    const uint8_t *src = (const uint8_t*)__src;
    uint8_t *dst = (uint8_t*)__dst;
    while (__n--) {
        eeprom_update_byte(dst++, pgm_read_byte(src++));
    }
}

void eeprom_toggle(uint8_t *__p) {
    eeprom_write_byte(__p, !eeprom_read_byte(__p));
}

void __attribute__((noinline)) eeprom_increment_byte(uint8_t *__p) {
    eeprom_write_byte(__p, eeprom_read_byte(__p) + 1);
}

void __attribute__((noinline)) eeprom_increment_word(uint16_t *__p) {
    eeprom_write_word(__p, eeprom_read_word(__p) + 1);
}

void __attribute__((noinline)) eeprom_increment_dword(uint32_t *__p) {
    eeprom_write_dword(__p, eeprom_read_dword(__p) + 1);
}


void __attribute__((noinline)) eeprom_add_byte(uint8_t *__p, uint8_t add) {
    eeprom_write_byte(__p, eeprom_read_byte(__p) + add);
}

void __attribute__((noinline)) eeprom_add_word(uint16_t *__p, uint16_t add) {
    eeprom_write_word(__p, eeprom_read_word(__p) + add);
}

void __attribute__((noinline)) eeprom_add_dword(uint32_t *__p, uint32_t add) {
    eeprom_write_dword(__p, eeprom_read_dword(__p) + add);
}


uint8_t __attribute__((noinline)) eeprom_init_default_byte(uint8_t *__p, uint8_t def) {
    uint8_t val = eeprom_read_byte(__p);
    if (val == EEPROM_EMPTY_VALUE) {
        eeprom_write_byte(__p, def);
        return def;
    }
    return val;
}

uint16_t __attribute__((noinline)) eeprom_init_default_word(uint16_t *__p, uint16_t def) {
    uint16_t val = eeprom_read_word(__p);
    if (val == EEPROM_EMPTY_VALUE16) {
        eeprom_write_word(__p, def);
        return def;
    }
    return val;
}

uint32_t __attribute__((noinline)) eeprom_init_default_dword(uint32_t *__p, uint32_t def) {
    uint32_t val = eeprom_read_dword(__p);
    if (val == EEPROM_EMPTY_VALUE32) {
        eeprom_write_dword(__p, def);
        return def;
    }
    return val;
}

void __attribute__((noinline)) eeprom_init_default_float(float *__p, float def) {
    if (eeprom_read_dword((uint32_t*)__p) == EEPROM_EMPTY_VALUE32)
        eeprom_write_float(__p, def);
}

void __attribute__((noinline)) eeprom_init_default_block(void *__p, size_t __n, const void *def) {
    if (!eeprom_is_initialized_block(__p, __n))
        eeprom_update_block(def, __p, __n);
}

void __attribute__((noinline)) eeprom_init_default_block_P(void *__p, size_t __n, const void *def) {
    if (!eeprom_is_initialized_block(__p, __n))
        eeprom_update_block_P(def, __p, __n);
}
