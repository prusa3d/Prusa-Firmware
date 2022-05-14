#include "mmu2_reporting.h"
#include "mmu2_error_converter.h"
#include "mmu2/error_codes.h"
#include "mmu2/buttons.h"
#include "ultralcd.h"
#include "language.h"

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
    //! Show an error screen
    //! When an MMU error occurs, the LCD content will look like this:
    //! |01234567890123456789|
    //! |MMU FW update needed|     <- title/header of the error: max 20 characters
    //! |prusa3d.com/ERR04504|     <- URL 20 characters
    //! |                    |     <- empty line
    //! |>Retry  >Done >MoreW|     <- buttons
    const uint8_t ei = PrusaErrorCodeIndex(ec);
    uint8_t choice_selected = 0;
    bool two_choices = false;

    // Read and determine what operations should be shown on the menu
    // Note: uint16_t is used here to avoid compiler warning. uint8_t is only half the size of void*
    const uint8_t button_operation   = PrusaErrorButtons(ei);
    const uint8_t button_high_nibble = BUTTON_OP_HI_NIBBLE(button_operation);
    const uint8_t button_low_nibble  = BUTTON_OP_LO_NIBBLE(button_operation);

    // Check if the menu should have three or two choices
    if (button_high_nibble == (uint8_t)ButtonOperations::NoOperation)
    {
        // Two operations not specified, the error menu should only show two choices
        two_choices = true;
    }

back_to_choices:
    lcd_clear();
    lcd_update_enable(false);
     
    // Print title and header
    lcd_printf_P(PSTR("%.20S\nprusa3d.com/ERR04%hu"), _T(PrusaErrorTitle(ei)), PrusaErrorCode(ei) );

    // Render the choices and store selection in 'choice_selected'
    choice_selected = lcd_show_multiscreen_message_with_choices_and_wait_P(
        NULL, // NULL, since title screen is not in PROGMEM
        false,
        two_choices ? LEFT_BUTTON_CHOICE : MIDDLE_BUTTON_CHOICE,
        _T(PrusaErrorButtonTitle(button_low_nibble - 1)),
        _T(two_choices ? PrusaErrorButtonMore() : PrusaErrorButtonTitle(button_high_nibble - 1)),
        two_choices ? nullptr : _T(PrusaErrorButtonMore()),
        two_choices ? 
            10 // If two choices, allow the first choice to have more characters
            : 7
    );

    if ((two_choices && choice_selected == MIDDLE_BUTTON_CHOICE)      // Two choices and middle button selected
        || (!two_choices && choice_selected == RIGHT_BUTTON_CHOICE)) // Three choices and right most button selected
    {
        // 'More' show error description
        lcd_show_fullscreen_message_and_wait_P(_T(PrusaErrorDesc(ei)));

        // Return back to the choice menu
        goto back_to_choices;
    } else if(choice_selected == MIDDLE_BUTTON_CHOICE) {
        // TODO: User selected middle choice, not sure what to do.
        //       At the moment just return to the status screen
        switch (button_high_nibble)
        {
        case (uint8_t)ButtonOperations::Retry:
        case (uint8_t)ButtonOperations::Continue:
        case (uint8_t)ButtonOperations::RestartMMU:
        case (uint8_t)ButtonOperations::Unload:
        case (uint8_t)ButtonOperations::StopPrint:
        case (uint8_t)ButtonOperations::DisableMMU:
        default:
            lcd_update_enable(true);
            lcd_return_to_status();
            break;
        }
    } else {
        // TODO: User selected the left most choice, not sure what to do.
        //       At the moment just return to the status screen
        switch (button_low_nibble)
        {
        case (uint8_t)ButtonOperations::Retry:
        case (uint8_t)ButtonOperations::Continue:
        case (uint8_t)ButtonOperations::RestartMMU:
        case (uint8_t)ButtonOperations::Unload:
        case (uint8_t)ButtonOperations::StopPrint:
        case (uint8_t)ButtonOperations::DisableMMU:
        default:
            lcd_update_enable(true);
            lcd_return_to_status();
            break;
        }
    }
}

void ReportProgressHook(CommandInProgress cip, uint16_t ec) {
    custom_message_type = CustomMsg::MMUProgress;
    lcd_setstatuspgm( _T(ProgressCodeToText(ec)) );
}

Buttons ButtonPressed(uint16_t ec) { 
    // query the MMU error screen if a button has been pressed/selected
}

} // namespace MMU2
