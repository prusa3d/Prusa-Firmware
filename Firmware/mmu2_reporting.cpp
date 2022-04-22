#include "mmu2_reporting.h"
#include "ultralcd.h"

namespace MMU2 {

const char * const ProgressCodeToText(uint16_t pc); // we may join progress convertor and reporter together

void BeginReport(CommandInProgress cip, uint16_t ec) {
    custom_message_type = CustomMsg::MMUProgress;
    lcd_setstatuspgm( ProgressCodeToText(ec) );
}

void EndReport(CommandInProgress cip, uint16_t ec) {
    // clear the status msg line - let the printed filename get visible again
    custom_message_type = CustomMsg::Status;
}

void ReportErrorHook(CommandInProgress cip, uint16_t ec) {
    // @@TODO - display an error screen - we still don't know how that will look like
    // The only thing we know is the fact, that the screen must not block the MMU automaton
}

void ReportProgressHook(CommandInProgress cip, uint16_t ec) {
    custom_message_type = CustomMsg::MMUProgress;
    lcd_setstatuspgm( ProgressCodeToText(ec) );
}

Buttons ButtonPressed(uint16_t ec) { 
    // query the MMU error screen if a button has been pressed/selected
}

} // namespace MMU2
