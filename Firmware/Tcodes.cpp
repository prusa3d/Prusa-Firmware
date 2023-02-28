#include "Tcodes.h"
#include "SpoolJoin.h"
#include "Marlin.h"
#include "language.h"
#include "messages.h"
#include "mmu2.h"
#include "stepper.h"
#include "ultralcd.h"
#include <avr/pgmspace.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

static const char duplicate_Tcode_ignored[] PROGMEM = "Duplicate T-code ignored.";

inline bool IsInvalidTCode(char *const s, uint8_t i) { 
    return ((s[i] < '0' || s[i] > '4') && s[i] != '?' && s[i] != 'x' && s[i] != 'c'); 
}

inline void TCodeInvalid() { 
    SERIAL_ECHOLNPGM("Invalid T code."); 
}

struct SChooseFromMenu {
    uint8_t slot:7;
    uint8_t loadToNozzle:1;
    inline constexpr SChooseFromMenu(uint8_t slot, bool loadToNozzle):slot(slot), loadToNozzle(loadToNozzle){}
    inline constexpr SChooseFromMenu():slot(0), loadToNozzle(false) { }
};

SChooseFromMenu TCodeChooseFromMenu() {
    return SChooseFromMenu( choose_menu_P(_T(MSG_SELECT_FILAMENT), _T(MSG_FILAMENT)), MMU2::mmu2.Enabled() );
}

void TCodes(char *const strchr_pointer, uint8_t codeValue) {
    uint8_t index = 1;
    for ( /*nothing*/ ; strchr_pointer[index] == ' ' || strchr_pointer[index] == '\t'; index++)
        ;

    strchr_pointer[index] = tolower(strchr_pointer[index]);

    if (IsInvalidTCode(strchr_pointer, index)){
        TCodeInvalid();
    } else if (strchr_pointer[index] == 'x'){
        // load to extruder gears; if mmu is not present do nothing
        if (MMU2::mmu2.Enabled()) {
            MMU2::mmu2.tool_change(strchr_pointer[index], choose_menu_P(_T(MSG_SELECT_FILAMENT), _T(MSG_FILAMENT)));
        }
    } else if (strchr_pointer[index] == 'c'){
        // load from extruder gears to nozzle (nozzle should be preheated)
        if (MMU2::mmu2.Enabled()) {
            MMU2::mmu2.tool_change(strchr_pointer[index], MMU2::mmu2.get_current_tool());
        }
    } else {
        SChooseFromMenu selectedSlot;
        if (strchr_pointer[index] == '?') {
            selectedSlot = TCodeChooseFromMenu();
        /*} else if (MMU2::mmu2.Enabled() && SpoolJoin::spooljoin.isSpoolJoinEnabled()) {
            // TODO: What if the next slot has no filament?
            selectedSlot.slot = SpoolJoin::spooljoin.nextSlot();*/
        } else {
            selectedSlot.slot = codeValue;
        }
        st_synchronize();

        if (MMU2::mmu2.Enabled()) {
            if (selectedSlot.slot == MMU2::mmu2.get_current_tool()){ 
                // don't execute the same T-code twice in a row
                puts_P(duplicate_Tcode_ignored);
            } else {
#if defined(MMU_HAS_CUTTER) && defined(MMU_ALWAYS_CUT)
                if (EEPROM_MMU_CUTTER_ENABLED_always == eeprom_read_byte((uint8_t *)EEPROM_MMU_CUTTER_ENABLED)) {
                    MMU2::mmu2.cut_filament(selectedSlot.slot);
                }
#endif // defined(MMU_HAS_CUTTER) && defined(MMU_ALWAYS_CUT)
                if (selectedSlot.loadToNozzle){ // for single material usage with mmu
                    MMU2::mmu2.load_filament_to_nozzle(selectedSlot.slot);
                } else {
                    MMU2::mmu2.tool_change(selectedSlot.slot);
                }
            }
        } else {
            if (selectedSlot.slot >= EXTRUDERS) {
                SERIAL_ECHO_START;
                SERIAL_ECHO('T');
                SERIAL_ECHOLN(selectedSlot.slot + '0');
                SERIAL_ECHOLNRPGM(_n("Invalid extruder")); ////MSG_INVALID_EXTRUDER
            } else {
// @@TODO               if (code_seen('F')) {
//                    next_feedrate = code_value();
//                    if (next_feedrate > 0.0) {
//                        feedrate = next_feedrate;
//                    }
//                }
                SERIAL_ECHO_START;
                SERIAL_ECHORPGM(_n("Active Extruder: ")); ////MSG_ACTIVE_EXTRUDER
                SERIAL_ECHOLN(active_extruder + '0'); // this is not changed in our FW at all, can be optimized away
            }
        }
    }
}
