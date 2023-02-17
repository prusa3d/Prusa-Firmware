#include "mmu2_progress_converter.h"
#include "language.h"
#include "mmu2/progress_codes.h"
#include <avr/pgmspace.h>

namespace MMU2 {
                                                                   //01234567890123456789
static const char MSG_PROGRESS_OK[] PROGMEM_I1               = ISTR("OK"); ////MSG_PROGRESS_OK c=4
static const char MSG_PROGRESS_ENGAGE_IDLER[] PROGMEM_I1     = ISTR("Engaging idler"); ////MSG_PROGRESS_ENGAGE_IDLER c=20
static const char MSG_PROGRESS_DISENGAGE_IDLER[] PROGMEM_I1  = ISTR("Disengaging idler"); ////MSG_PROGRESS_DISENGAGE_IDLER c=20
static const char MSG_PROGRESS_UNLOAD_FINDA[] PROGMEM_I1     = ISTR("Unloading to FINDA"); ////MSG_PROGRESS_UNLOAD_FINDA c=20
static const char MSG_PROGRESS_UNLOAD_PULLEY[] PROGMEM_I1    = ISTR("Unloading to pulley"); ////MSG_PROGRESS_UNLOAD_PULLEY c=20
static const char MSG_PROGRESS_FEED_FINDA[] PROGMEM_I1       = ISTR("Feeding to FINDA"); ////MSG_PROGRESS_FEED_FINDA c=20
static const char MSG_PROGRESS_FEED_EXTRUDER[] PROGMEM_I1    = ISTR("Feeding to extruder"); ////MSG_PROGRESS_FEED_EXTRUDER c=20
static const char MSG_PROGRESS_FEED_NOZZLE[] PROGMEM_I1      = ISTR("Feeding to nozzle"); ////MSG_PROGRESS_FEED_NOZZLE c=20
static const char MSG_PROGRESS_AVOID_GRIND[] PROGMEM_I1      = ISTR("Avoiding grind"); ////MSG_PROGRESS_AVOID_GRIND c=20
static const char MSG_PROGRESS_WAIT_USER[] PROGMEM_I1        = ISTR("ERR Wait for User"); ////MSG_PROGRESS_WAIT_USER c=20
static const char MSG_PROGRESS_ERR_INTERNAL[] PROGMEM_I1     = ISTR("ERR Internal"); ////MSG_PROGRESS_ERR_INTERNAL c=20
static const char MSG_PROGRESS_ERR_HELP_FIL[] PROGMEM_I1     = ISTR("ERR Help filament"); ////MSG_PROGRESS_ERR_HELP_FIL c=20
static const char MSG_PROGRESS_ERR_TMC[] PROGMEM_I1          = ISTR("ERR TMC failed"); ////MSG_PROGRESS_ERR_TMC c=20
static const char MSG_PROGRESS_SELECT_SLOT[] PROGMEM_I1      = ISTR("Selecting fil. slot"); ////MSG_PROGRESS_SELECT_SLOT c=20
static const char MSG_PROGRESS_PREPARE_BLADE[] PROGMEM_I1    = ISTR("Preparing blade"); ////MSG_PROGRESS_PREPARE_BLADE c=20
static const char MSG_PROGRESS_PUSH_FILAMENT[] PROGMEM_I1    = ISTR("Pushing filament"); ////MSG_PROGRESS_PUSH_FILAMENT c=20
static const char MSG_PROGRESS_PERFORM_CUT[] PROGMEM_I1      = ISTR("Performing cut"); ////MSG_PROGRESS_PERFORM_CUT c=20
static const char MSG_PROGRESS_RETURN_SELECTOR[] PROGMEM_I1  = ISTR("Returning selector"); ////MSG_PROGRESS_RETURN_SELECTOR c=20
static const char MSG_PROGRESS_PARK_SELECTOR[] PROGMEM_I1    = ISTR("Parking selector"); ////MSG_PROGRESS_PARK_SELECTOR c=20
static const char MSG_PROGRESS_EJECT_FILAMENT[] PROGMEM_I1   = ISTR("Ejecting filament"); ////MSG_PROGRESS_EJECT_FILAMENT c=20 //@@todo duplicate
static const char MSG_PROGRESS_RETRACT_FINDA[] PROGMEM_I1    = ISTR("Retract from FINDA"); ////MSG_PROGRESS_RETRACT_FINDA c=20
static const char MSG_PROGRESS_HOMING[] PROGMEM_I1           = ISTR("Homing"); ////MSG_PROGRESS_HOMING c=20
static const char MSG_PROGRESS_MOVING_SELECTOR[] PROGMEM_I1  = ISTR("Moving selector"); ////MSG_PROGRESS_MOVING_SELECTOR c=20
static const char MSG_PROGRESS_FEED_FSENSOR[] PROGMEM_I1     = ISTR("Feeding to FSensor"); ////MSG_PROGRESS_FEED_FSENSOR c=20

static const char * const progressTexts[] PROGMEM = {
    _R(MSG_PROGRESS_OK),
    _R(MSG_PROGRESS_ENGAGE_IDLER),
    _R(MSG_PROGRESS_DISENGAGE_IDLER),
    _R(MSG_PROGRESS_UNLOAD_FINDA),
    _R(MSG_PROGRESS_UNLOAD_PULLEY),
    _R(MSG_PROGRESS_FEED_FINDA),
    _R(MSG_PROGRESS_FEED_EXTRUDER),
    _R(MSG_PROGRESS_FEED_NOZZLE),
    _R(MSG_PROGRESS_AVOID_GRIND),
    _R(MSG_FINISHING_MOVEMENTS), //reuse from messages.cpp
    _R(MSG_PROGRESS_DISENGAGE_IDLER), // err disengaging idler is the same text
    _R(MSG_PROGRESS_ENGAGE_IDLER), // engage dtto.
    _R(MSG_PROGRESS_WAIT_USER),
    _R(MSG_PROGRESS_ERR_INTERNAL),
    _R(MSG_PROGRESS_ERR_HELP_FIL),
    _R(MSG_PROGRESS_ERR_TMC),
    _R(MSG_UNLOADING_FILAMENT), //reuse from messages.cpp
    _R(MSG_LOADING_FILAMENT), //reuse from messages.cpp
    _R(MSG_PROGRESS_SELECT_SLOT),
    _R(MSG_PROGRESS_PREPARE_BLADE),
    _R(MSG_PROGRESS_PUSH_FILAMENT),
    _R(MSG_PROGRESS_PERFORM_CUT),
    _R(MSG_PROGRESS_RETURN_SELECTOR),
    _R(MSG_PROGRESS_PARK_SELECTOR),
    _R(MSG_PROGRESS_EJECT_FILAMENT),
    _R(MSG_PROGRESS_RETRACT_FINDA),
    _R(MSG_PROGRESS_HOMING),
    _R(MSG_PROGRESS_MOVING_SELECTOR),
    _R(MSG_PROGRESS_FEED_FSENSOR)
};

const char * ProgressCodeToText(uint16_t pc){
    // @@TODO ?? a better fallback option?
    return ( pc <= (sizeof(progressTexts) / sizeof(progressTexts[0])) )
       ? static_cast<const char *>(pgm_read_ptr(&progressTexts[pc]))
       : static_cast<const char *>(pgm_read_ptr(&progressTexts[0]));
}

} // namespace MMU2
