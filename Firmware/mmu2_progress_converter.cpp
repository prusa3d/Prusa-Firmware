#include "mmu2_progress_converter.h"
#include "language.h"
#include "mmu2/progress_codes.h"
#include <avr/pgmspace.h>

namespace MMU2 {
                                                             // 12345678901234567890
static const char progressOk[] PROGMEM_I1               = ISTR("OK");
static const char progressEngageIdler[] PROGMEM_I1      = ISTR("Engaging idler");
static const char progressDisengeIdler[] PROGMEM_I1     = ISTR("Disengaging idler");
static const char progressUnloadFinda[] PROGMEM_I1      = ISTR("Unloading to FINDA");
static const char progressUnloadPulley[] PROGMEM_I1     = ISTR("Unloading to pulley");
static const char progressFeedFinda[] PROGMEM_I1        = ISTR("Feeding to FINDA");
static const char progressFeedBondtech[] PROGMEM_I1     = ISTR("Feeding to extruder");
static const char progressFeedNozzle[] PROGMEM_I1       = ISTR("Feeding to nozzle");
static const char progressAvoidGrind[] PROGMEM_I1       = ISTR("Avoiding grind");
static const char progressFinishMoves[] PROGMEM_I1      = ISTR("Finishing moves");
static const char progressWaitForUser[] PROGMEM_I1      = ISTR("ERR Wait for User");
static const char progressErrInternal[] PROGMEM_I1      = ISTR("ERR Internal");
static const char progressErrHelpFil[] PROGMEM_I1       = ISTR("ERR Help filament");
static const char progressErrTmc[] PROGMEM_I1           = ISTR("ERR TMC failed");
static const char progressUnloadFilament[] PROGMEM_I1   = ISTR("Unloading filament");
static const char progressLoadFilament[] PROGMEM_I1     = ISTR("Loading filament");
static const char progressSelectSlot[] PROGMEM_I1       = ISTR("Selecting fil. slot");
static const char progressPrepareBlade[] PROGMEM_I1     = ISTR("Preparing blade");
static const char progressPushFilament[] PROGMEM_I1     = ISTR("Pushing filament");
static const char progressPerformCut[] PROGMEM_I1       = ISTR("Performing cut");
static const char progressReturnSelector[] PROGMEM_I1   = ISTR("Returning selector");
static const char progressParkSelector[] PROGMEM_I1     = ISTR("Parking selector");
static const char progressEjectFilament[] PROGMEM_I1    = ISTR("Ejecting filament");
static const char progressRetractFinda[] PROGMEM_I1     = ISTR("Retract from FINDA");
static const char progressHoming[] PROGMEM_I1           = ISTR("Homing");
static const char progressMovingSelector[] PROGMEM_I1   = ISTR("Moving selector");
static const char progressFeedingToFSensor[] PROGMEM_I1 = ISTR("Feeding to FSensor");

static const char * const progressTexts[] PROGMEM = {
    progressOk,
    progressEngageIdler,
    progressDisengeIdler,
    progressUnloadFinda,
    progressUnloadPulley,
    progressFeedFinda,
    progressFeedBondtech,
    progressFeedNozzle,
    progressAvoidGrind,
    progressFinishMoves,
    progressDisengeIdler, // err disengaging idler is the same text
    progressEngageIdler, // engage dtto.
    progressWaitForUser,
    progressErrInternal,
    progressErrHelpFil,
    progressErrTmc,
    progressUnloadFilament,
    progressLoadFilament,
    progressSelectSlot,
    progressPrepareBlade,
    progressPushFilament,
    progressPerformCut,
    progressReturnSelector,
    progressParkSelector,
    progressEjectFilament,
    progressRetractFinda,
    progressHoming,
    progressMovingSelector,
    progressFeedingToFSensor
};

const char * const ProgressCodeToText(uint16_t pc){
    // @@TODO ?? a better fallback option?
    return ( pc <= (sizeof(progressTexts) / sizeof(progressTexts[0])) )
       ? static_cast<const char * const>(pgm_read_ptr(&progressTexts[pc]))
       : static_cast<const char * const>(pgm_read_ptr(&progressTexts[0]));
}

} // namespace MMU2
