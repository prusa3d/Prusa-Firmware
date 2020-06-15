//! @file
//! @date Jun 20, 2019
//! @author Marek Běl

#include "eeprom.h"
#include "Marlin.h"

#include <avr/eeprom.h>
#include <stdint.h>


#include "language.h"

#if 0
template <typename T>
static T eeprom_read(T *address);

template<>
char eeprom_read<char>(char *address)
{
    return eeprom_read_byte(reinterpret_cast<uint8_t*>(address));
}
#endif

template <typename T>
static void eeprom_write(T *address, T value);

template<>
void eeprom_write<char>(char *addres, char value)
{
    eeprom_write_byte(reinterpret_cast<uint8_t*>(addres), static_cast<uint8_t>(value));
}


template <typename T>
static bool eeprom_is_uninitialized(T *address);

template <>
bool eeprom_is_uninitialized<char>(char *address)
{
    return (0xff == eeprom_read_byte(reinterpret_cast<uint8_t*>(address)));
}

bool eeprom_is_sheet_initialized(uint8_t sheet_num)
{
  return (0xffff != eeprom_read_word(reinterpret_cast<uint16_t*>(&(EEPROM_Sheets_base->
  s[sheet_num].z_offset))));
}

void eeprom_init()
{
    if (eeprom_read_byte((uint8_t*)EEPROM_POWER_COUNT) == 0xff) eeprom_write_byte((uint8_t*)EEPROM_POWER_COUNT, 0);
    if (eeprom_read_byte((uint8_t*)EEPROM_CRASH_COUNT_X) == 0xff) eeprom_write_byte((uint8_t*)EEPROM_CRASH_COUNT_X, 0);
    if (eeprom_read_byte((uint8_t*)EEPROM_CRASH_COUNT_Y) == 0xff) eeprom_write_byte((uint8_t*)EEPROM_CRASH_COUNT_Y, 0);
    if (eeprom_read_byte((uint8_t*)EEPROM_FERROR_COUNT) == 0xff) eeprom_write_byte((uint8_t*)EEPROM_FERROR_COUNT, 0);
    if (eeprom_read_word((uint16_t*)EEPROM_POWER_COUNT_TOT) == 0xffff) eeprom_write_word((uint16_t*)EEPROM_POWER_COUNT_TOT, 0);
    if (eeprom_read_word((uint16_t*)EEPROM_CRASH_COUNT_X_TOT) == 0xffff) eeprom_write_word((uint16_t*)EEPROM_CRASH_COUNT_X_TOT, 0);
    if (eeprom_read_word((uint16_t*)EEPROM_CRASH_COUNT_Y_TOT) == 0xffff) eeprom_write_word((uint16_t*)EEPROM_CRASH_COUNT_Y_TOT, 0);
    if (eeprom_read_word((uint16_t*)EEPROM_FERROR_COUNT_TOT) == 0xffff) eeprom_write_word((uint16_t*)EEPROM_FERROR_COUNT_TOT, 0);

    if (eeprom_read_word((uint16_t*)EEPROM_MMU_FAIL_TOT) == 0xffff) eeprom_update_word((uint16_t *)EEPROM_MMU_FAIL_TOT, 0);
    if (eeprom_read_word((uint16_t*)EEPROM_MMU_LOAD_FAIL_TOT) == 0xffff) eeprom_update_word((uint16_t *)EEPROM_MMU_LOAD_FAIL_TOT, 0);
    if (eeprom_read_byte((uint8_t*)EEPROM_MMU_FAIL) == 0xff) eeprom_update_byte((uint8_t *)EEPROM_MMU_FAIL, 0);
    if (eeprom_read_byte((uint8_t*)EEPROM_MMU_LOAD_FAIL) == 0xff) eeprom_update_byte((uint8_t *)EEPROM_MMU_LOAD_FAIL, 0);
    if (eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)) == EEPROM_EMPTY_VALUE)
    {
        eeprom_update_byte(&(EEPROM_Sheets_base->active_sheet), 0);
        // When upgrading from version older version (before multiple sheets were implemented in v3.8.0)
        // Sheet 1 uses the previous Live adjust Z (@EEPROM_BABYSTEP_Z)
        int last_babystep = eeprom_read_word((uint16_t *)EEPROM_BABYSTEP_Z);
        eeprom_update_word(reinterpret_cast<uint16_t *>(&(EEPROM_Sheets_base->s[0].z_offset)), last_babystep);
    }
    
    for (uint_least8_t i = 0; i < (sizeof(Sheets::s)/sizeof(Sheets::s[0])); ++i)
    {
        bool is_uninitialized = true;
        for (uint_least8_t j = 0; j < (sizeof(Sheet::name)/sizeof(Sheet::name[0])); ++j)
        {
            if (!eeprom_is_uninitialized(&(EEPROM_Sheets_base->s[i].name[j]))) is_uninitialized = false;
        }
        if(is_uninitialized)
        {
            SheetName sheetName;
            eeprom_default_sheet_name(i,sheetName);

            for (uint_least8_t a = 0; a < sizeof(Sheet::name); ++a){
                eeprom_write(&(EEPROM_Sheets_base->s[i].name[a]), sheetName.c[a]);
            }
        }
    }
    if(!eeprom_is_sheet_initialized(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet))))
    {
        eeprom_switch_to_next_sheet();
    }
    check_babystep();
}

//! @brief Get default sheet name for index
//!
//! | index | sheetName |
//! | ----- | --------- |
//! | 0     | Smooth1   |
//! | 1     | Smooth2   |
//! | 2     | Textur1   |
//! | 3     | Textur2   |
//! | 4     | Custom1   |
//! | 5     | Custom2   |
//! | 6     | Custom3   |
//! | 7     | Custom4   |
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
    else
    {
        strcpy_P(sheetName.c, PSTR("Custom"));
    }

    switch (index)
    {
    case 0:
        sheetName.c[6] = '1';
        break;
    case 1:
        sheetName.c[6] = '2';
        break;
    case 2:
        sheetName.c[6] = '1';
        break;
    case 3:
        sheetName.c[6] = '2';
        break;
    case 4:
        sheetName.c[6] = '1';
        break;
    case 5:
        sheetName.c[6] = '2';
        break;
    case 6:
        sheetName.c[6] = '3';
        break;
    case 7:
        sheetName.c[6] = '4';
        break;
    default:
        break;
    }

    sheetName.c[7] = '\0';
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
