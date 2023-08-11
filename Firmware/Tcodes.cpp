#include "Tcodes.h"
#include "SpoolJoin.h"
#include "Marlin.h"
#include "language.h"
#include "messages.h"
#include "mmu2.h"
#include "ultralcd.h"
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>

static const char duplicate_Tcode_ignored[] PROGMEM = "Duplicate T-code ignored.";

inline bool IsInvalidTCode(char *const s, uint8_t i) { 
    return ((s[i] < '0' || s[i] > '4') && s[i] != '?' && s[i] != 'x' && s[i] != 'c'); 
}

inline void TCodeInvalid() { 
    SERIAL_ECHOLNPGM("Invalid T code."); 
}

void TCodes(char *const strchr_pointer, const uint8_t codeValue) {
    uint8_t index = 1;
    for ( /*nothing*/ ; strchr_pointer[index] == ' ' || strchr_pointer[index] == '\t'; index++)
        ;

    strchr_pointer[index] = tolower(strchr_pointer[index]);

    if (IsInvalidTCode(strchr_pointer, index)){
        TCodeInvalid();
    } else if (strchr_pointer[index] == 'x' || strchr_pointer[index] == '?'){
        // load to extruder gears; if mmu is not present do nothing
        if (MMU2::mmu2.Enabled()) {
            MMU2::mmu2.tool_change(strchr_pointer[index], choose_menu_P(_T(MSG_SELECT_FILAMENT), _T(MSG_FILAMENT)));
        }
    } else if (strchr_pointer[index] == 'c'){
        // load from extruder gears to nozzle (nozzle should be preheated)
        if (MMU2::mmu2.Enabled()) {
            MMU2::mmu2.tool_change(strchr_pointer[index], MMU2::mmu2.get_current_tool());
        }
    } else { // Process T0 ... T4
        if (MMU2::mmu2.Enabled()) {
            if (codeValue == MMU2::mmu2.get_current_tool()){ 
                // don't execute the same T-code twice in a row
                puts_P(duplicate_Tcode_ignored);
            } else {
#if defined(MMU_HAS_CUTTER) && defined(MMU_ALWAYS_CUT)
                if (EEPROM_MMU_CUTTER_ENABLED_always == eeprom_read_byte((uint8_t *)EEPROM_MMU_CUTTER_ENABLED)) {
                    MMU2::mmu2.cut_filament(codeValue);
                }
#endif // defined(MMU_HAS_CUTTER) && defined(MMU_ALWAYS_CUT)
                MMU2::mmu2.tool_change(codeValue);
            }
        } else {
            SERIAL_ECHO_START;
            if (codeValue >= EXTRUDERS) {
                SERIAL_ECHO('T');
                SERIAL_ECHOLN(codeValue + '0');
                SERIAL_ECHOLNRPGM(_n("Invalid extruder")); ////MSG_INVALID_EXTRUDER
            } else {
// @@TODO               if (code_seen('F')) {
//                    next_feedrate = code_value();
//                    if (next_feedrate > 0.0) {
//                        feedrate = next_feedrate;
//                    }
//                }
                SERIAL_ECHORPGM(_n("Active Extruder: 0")); ////MSG_ACTIVE_EXTRUDER
            }
        }
    }
}
