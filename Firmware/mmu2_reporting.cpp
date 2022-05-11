#include "mmu2_reporting.h"
#include "mmu2_error_converter.h"
#include "mmu2/error_codes.h"
#include "mmu2/errors_list.h"
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
    //! Show an error screen
    //! When an MMU error occurs, the LCD content will look like this:
    //! |01234567890123456789|
    //! |MMU FW update needed|     <- title/header of the error: max 20 characters
    //! |prusa3d.com/ERR04504|     <- URL 20 characters
    //! |                    |     <- empty line
    //! |>Retry  >Done >MoreW|     <- buttons
    const uint16_t ei = MMUErrorCodeIndex((uint16_t)ec);
    uint8_t choice_selected = 0;
    bool two_choices = false;

    // Read and determine what operations should be shown on the menu
    // Note: uint16_t is used here to avoid compiler warning. uint8_t is only half the size of void*
    const uint8_t button_operation   = reinterpret_cast<uint16_t>(const_cast<void*>(pgm_read_ptr(&errorButtons[ei])));
    const uint8_t button_high_nibble = BUTTON_OP_HI_NIBBLE(button_operation);
    const uint8_t button_low_nibble  = BUTTON_OP_LO_NIBBLE(button_operation);

    // Check if the menu should have three or two choices
    if (button_low_nibble == (uint8_t)ButtonOperations::NoOperation)
    {
        // Two operations not specified, the error menu should only show two choices
        two_choices = true;
    }

back_to_choices:
    lcd_clear();
    lcd_update_enable(false);

    // Print title and header
    lcd_printf_P(PSTR("%S\nprusa3d.com/ERR04%hu"),
        static_cast<const char * const>(pgm_read_ptr(&errorTitles[ei])),
        reinterpret_cast<uint16_t>(const_cast<void*>(pgm_read_ptr(&errorCodes[ei])))
    );

    // Render the choices and store selection in 'choice_selected'
    choice_selected = lcd_show_multiscreen_message_with_choices_and_wait_P(
        NULL, // NULL, since title screen is not in PROGMEM
        false,
        false,
        two_choices ?
            static_cast<const char * const>(pgm_read_ptr(&btnOperation[button_high_nibble - 1]))
            : static_cast<const char * const>(pgm_read_ptr(&btnOperation[button_low_nibble - 1])),
        two_choices ?
            btnMore
            : static_cast<const char * const>(pgm_read_ptr(&btnOperation[button_high_nibble - 1])),
        two_choices ? nullptr : btnMore,
        two_choices ? 
            10 // If two choices, allow the first choice to have more characters
            : 7,
        13
    );

    if ((two_choices && choice_selected == 1)      // Two choices and middle button selected
        || (!two_choices && choice_selected == 2)) // Three choices and right most button selected
    {
        // 'More' show error description
        lcd_show_fullscreen_message_and_wait_P(
            static_cast<const char * const>(pgm_read_ptr(&errorDescs[ei]))
        );

        // Return back to the choice menu
        goto back_to_choices;
    } else if(choice_selected == 1) {
        // TODO: User selected middle choice, not sure what to do.
        //       At the moment just return to the status screen
        switch (button_high_nibble)
        {
        case (uint8_t)ButtonOperations::Retry:
        case (uint8_t)ButtonOperations::SlowLoad:
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
        switch ( two_choices ?
            button_high_nibble
            : button_low_nibble
        )
        {
        case (uint8_t)ButtonOperations::Retry:
        case (uint8_t)ButtonOperations::SlowLoad:
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
    lcd_setstatuspgm( ProgressCodeToText(ec) );
}

Buttons ButtonPressed(uint16_t ec) { 
    // query the MMU error screen if a button has been pressed/selected
}

} // namespace MMU2
