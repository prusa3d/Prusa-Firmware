#include "mmu2.h"
#include "mmu2_reporting.h"
#include "mmu2_error_converter.h"
#include "mmu2/error_codes.h"
#include "mmu2/buttons.h"
#include "ultralcd.h"
#include "Filament_sensor.h"
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

// Callback which is called while the printer is
// waiting for the user to click a button option
static void ReportErrorHook_cb(void)
{
    //TODO: MK3S needs to request an update for the FINDA value
    //      if we want it to be updated live on the menu screen
    lcd_set_cursor(3, 2);
    lcd_printf_P(PSTR("%d"), mmu2.FindaDetectsFilament());

    lcd_set_cursor(8, 2);
    lcd_printf_P(PSTR("%d"), fsensor.getFilamentPresent());

    lcd_set_cursor(11, 2);
    lcd_print("?>?"); // This is temporary until below TODO is resolved

    // TODO, see lcdui_print_extruder(void)
    //if (MMU2::mmu2.get_current_tool() == MMU2::FILAMENT_UNKNOWN)
    //    lcd_printf_P(_N(" ?>%u"), tmp_extruder + 1);
    //else
    //    lcd_printf_P(_N(" %u>%u"), MMU2::mmu2.get_current_tool() + 1, tmp_extruder + 1);

    // Print active extruder temperature
    lcd_set_cursor(16, 2);
    lcd_printf_P(PSTR("%d"), (int)(degHotend(0) + 0.5));
}

void ReportErrorHook(CommandInProgress cip, uint16_t ec) {
    //! Show an error screen
    //! When an MMU error occurs, the LCD content will look like this:
    //! |01234567890123456789|
    //! |MMU FW update needed|     <- title/header of the error: max 20 characters
    //! |prusa3d.com/ERR04504|     <- URL 20 characters
    //! |FI:1 FS:1  5>3 t201°|     <- status line, t is thermometer symbol
    //! |>Retry  >Done >MoreW|     <- buttons
    const uint8_t ei = PrusaErrorCodeIndex(ec);
    uint8_t choice_selected = 0;
    bool two_choices = false;

    // Read and determine what operations should be shown on the menu
    // Note: uint16_t is used here to avoid compiler warning. uint8_t is only half the size of void*
    const uint8_t button_operation   = PrusaErrorButtons(ei);
    const uint8_t button_op_right = BUTTON_OP_RIGHT(button_operation);
    const uint8_t button_op_middle  = BUTTON_OP_MIDDLE(button_operation);

    // Check if the menu should have three or two choices
    if (button_op_right == (uint8_t)ButtonOperations::NoOperation){
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
        two_choices ? LCD_LEFT_BUTTON_CHOICE : LCD_MIDDLE_BUTTON_CHOICE, // beware - LEFT button on the LCD matches the MIDDLE button on the MMU!
        _T(PrusaErrorButtonTitle(button_op_middle)),
        _T(two_choices ? PrusaErrorButtonMore() : PrusaErrorButtonTitle(button_op_right)),
        two_choices ? nullptr : _T(PrusaErrorButtonMore()),
        two_choices ? 10 : 7 // If two choices, allow the first choice to have more characters
    );

    if ((two_choices && choice_selected == LCD_MIDDLE_BUTTON_CHOICE)      // Two choices and middle button selected
        || (!two_choices && choice_selected == LCD_RIGHT_BUTTON_CHOICE)) // Three choices and right most button selected
    {
        // 'More' show error description
        lcd_show_fullscreen_message_and_wait_P(_T(PrusaErrorDesc(ei)));

        // Return back to the choice menu
        goto back_to_choices;
    } else if(choice_selected == LCD_MIDDLE_BUTTON_CHOICE) {
        SetButtonResponse((ButtonOperations)button_op_right);
    } else {
        SetButtonResponse((ButtonOperations)button_op_middle);
    }
    // if any button/command selected, close the screen
    lcd_update_enable(true);
    lcd_return_to_status();
}

void ReportProgressHook(CommandInProgress cip, uint16_t ec) {
    custom_message_type = CustomMsg::MMUProgress;
    lcd_setstatuspgm( _T(ProgressCodeToText(ec)) );
}

} // namespace MMU2
