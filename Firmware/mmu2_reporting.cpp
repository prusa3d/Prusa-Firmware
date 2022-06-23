#include "mmu2.h"
#include "mmu2_reporting.h"
#include "mmu2_error_converter.h"
#include "mmu2/error_codes.h"
#include "mmu2/buttons.h"
#include "ultralcd.h"
#include "Filament_sensor.h"
#include "language.h"
#include "temperature.h"
#include "sound.h"

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

/**
 * @brief Renders any characters that will be updated live on the MMU error screen.
 *Currently, this is FINDA and Filament Sensor status and Extruder temperature.
 */
extern void ReportErrorHookDynamicRender(void){
    // beware - this optimization abuses the fact, that FindaDetectsFilament returns 0 or 1 and '0' is followed by '1' in the ASCII table
    lcd_putc_at(3, 2, mmu2.FindaDetectsFilament() + '0');
    lcd_putc_at(8, 2, fsensor.getFilamentPresent() + '0');

    // print active/changing filament slot
    lcd_set_cursor(10, 2);
    lcdui_print_extruder();

    // Print active extruder temperature
    lcd_set_cursor(16, 2);
    lcd_printf_P(PSTR("%3d"), (int)(degHotend(0) + 0.5));
}

/**
 * @brief Renders any characters that are static on the MMU error screen i.e. they don't change.
 * @param[in] ei Error code index
 */
static void ReportErrorHookStaticRender(uint8_t ei) {
    //! Show an error screen
    //! When an MMU error occurs, the LCD content will look like this:
    //! |01234567890123456789|
    //! |MMU FW update needed|     <- title/header of the error: max 20 characters
    //! |prusa3d.com/ERR04504|     <- URL 20 characters
    //! |FI:1 FS:1  5>3 t201°|     <- status line, t is thermometer symbol
    //! |>Retry  >Done >MoreW|     <- buttons
    bool two_choices = false;

    // Read and determine what operations should be shown on the menu
    const uint8_t button_operation   = PrusaErrorButtons(ei);
    const uint8_t button_op_right = BUTTON_OP_RIGHT(button_operation);
    const uint8_t button_op_middle  = BUTTON_OP_MIDDLE(button_operation);

    // Check if the menu should have three or two choices
    if (button_op_right == (uint8_t)ButtonOperations::NoOperation){
        // Two operations not specified, the error menu should only show two choices
        two_choices = true;
    }

    lcd_set_custom_characters_nextpage();
    lcd_update_enable(false);
    lcd_clear();

    // Print title and header
    lcd_printf_P(PSTR("%.20S\nprusa3d.com/ERR04%hu"), _T(PrusaErrorTitle(ei)), PrusaErrorCode(ei) );

    ReportErrorHookSensorLineRender();
    
    // Render the choices
    lcd_show_choices_prompt_P(two_choices ? LCD_LEFT_BUTTON_CHOICE : LCD_MIDDLE_BUTTON_CHOICE, _T(PrusaErrorButtonTitle(button_op_middle)), _T(two_choices ? PrusaErrorButtonMore() : PrusaErrorButtonTitle(button_op_right)), two_choices ? 10 : 7, two_choices ? nullptr : _T(PrusaErrorButtonMore()));
}

extern void ReportErrorHookSensorLineRender()
{
    // Render static characters in third line
    lcd_set_cursor(0, 2);
    lcd_printf_P(PSTR("FI:  FS:    >  %c   %c"), LCD_STR_THERMOMETER[0], LCD_STR_DEGREE[0]);
}


/**
 * @brief Monitors the LCD button selection without blocking MMU communication
 * @param[in] ei Error code index
 * @return 0 if there is no knob click --
 * 1 if user clicked 'More' and firmware should render
 * the error screen when ReportErrorHook is called next --
 * 2 if the user selects an operation and we would like
 * to exit the error screen. The MMU will raise the menu
 * again if the error is not solved.
 */
static uint8_t ReportErrorHookMonitor(uint8_t ei) {
    uint8_t ret = 0;
    bool two_choices = false;
    static int8_t enc_dif = lcd_encoder_diff;

    if (lcd_encoder_diff == 0)
    {
         // lcd_update_enable(true) was called outside ReportErrorHookMonitor
         // It will set lcd_encoder_diff to 0, sync enc_dif
        enc_dif = 0;
    }

    // Read and determine what operations should be shown on the menu
    const uint8_t button_operation   = PrusaErrorButtons(ei);
    const uint8_t button_op_right = BUTTON_OP_RIGHT(button_operation);
    const uint8_t button_op_middle  = BUTTON_OP_MIDDLE(button_operation);

    // Check if the menu should have three or two choices
    if (button_op_right == (uint8_t)ButtonOperations::NoOperation){
        // Two operations not specified, the error menu should only show two choices
        two_choices = true;
    }

    static int8_t current_selection = two_choices ? LCD_LEFT_BUTTON_CHOICE : LCD_MIDDLE_BUTTON_CHOICE;
    static int8_t choice_selected = -1;

    // Check if knob was rotated
    if (abs(enc_dif - lcd_encoder_diff) >= ENCODER_PULSES_PER_STEP) {
        if (two_choices == false) { // third_choice is not nullptr, safe to dereference
            if (enc_dif > lcd_encoder_diff && current_selection != LCD_LEFT_BUTTON_CHOICE) {
                // Rotating knob counter clockwise
                current_selection--;
            } else if (enc_dif < lcd_encoder_diff && current_selection != LCD_RIGHT_BUTTON_CHOICE) {
                // Rotating knob clockwise
                current_selection++;
            }
        } else {
            if (enc_dif > lcd_encoder_diff && current_selection != LCD_LEFT_BUTTON_CHOICE) {
                // Rotating knob counter clockwise
                current_selection = LCD_LEFT_BUTTON_CHOICE;
            } else if (enc_dif < lcd_encoder_diff && current_selection != LCD_MIDDLE_BUTTON_CHOICE) {
                // Rotating knob clockwise
                current_selection = LCD_MIDDLE_BUTTON_CHOICE;
            }
        }

        // Update '>' render only
        lcd_set_cursor(0, 3);
        lcd_print(current_selection == LCD_LEFT_BUTTON_CHOICE ? '>': ' ');
        if (two_choices == false)
        {
            lcd_set_cursor(7, 3);
            lcd_print(current_selection == LCD_MIDDLE_BUTTON_CHOICE ? '>': ' ');
            lcd_set_cursor(13, 3);
            lcd_print(current_selection == LCD_RIGHT_BUTTON_CHOICE ? '>': ' ');
        } else {
            lcd_set_cursor(10, 3);
            lcd_print(current_selection == LCD_MIDDLE_BUTTON_CHOICE ? '>': ' ');
        }
        // Consume rotation event and make feedback sound
        enc_dif = lcd_encoder_diff;
        Sound_MakeSound(e_SOUND_TYPE_EncoderMove);
    }

    // Check if knob was clicked and consume the event
    if (lcd_clicked()) {
        Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
        choice_selected = current_selection;
    } else {
        // continue monitoring
        return ret;
    }

    if ((two_choices && choice_selected == LCD_MIDDLE_BUTTON_CHOICE)      // Two choices and middle button selected
        || (!two_choices && choice_selected == LCD_RIGHT_BUTTON_CHOICE)) // Three choices and right most button selected
    {
        // 'More' show error description
        lcd_show_fullscreen_message_and_wait_P(_T(PrusaErrorDesc(ei)));
        ret = 1;
    } else if(choice_selected == LCD_MIDDLE_BUTTON_CHOICE) {
        SetButtonResponse((ButtonOperations)button_op_right);
        ret = 2;
    } else {
        SetButtonResponse((ButtonOperations)button_op_middle);
        ret = 2;
    }

    // Reset static variables to their default value
    current_selection = two_choices ? LCD_LEFT_BUTTON_CHOICE : LCD_MIDDLE_BUTTON_CHOICE;
    choice_selected = -1;
    return ret;
}

enum class ReportErrorHookStates : uint8_t {
    RENDER_ERROR_SCREEN  = 0,
    MONITOR_SELECTION    = 1,
    DISMISS_ERROR_SCREEN = 2,
};

enum ReportErrorHookStates ReportErrorHookState = ReportErrorHookStates::RENDER_ERROR_SCREEN;

/**
 * @brief Render MMU error screen on the LCD. This must be non-blocking
 * and allow the MMU and printer to communicate with each other.
 * @param[in] ec Error code
 */
void ReportErrorHook(uint16_t ec) {

//    SERIAL_ECHOPGM("ReportErrorHookState=");
//    SERIAL_ECHOLN((int)ReportErrorHookState);

    if (mmu2.MMUCurrentErrorCode() == ErrorCode::OK) {
        // If the error code suddenly changes to OK, that means
        // a button was pushed on the MMU and the LCD should
        // dismiss the error screen until MMU raises a new error
        ReportErrorHookState = ReportErrorHookStates::DISMISS_ERROR_SCREEN;
        mmu2.ResetRetryAttempts();
    } else {
        // attempt an automatic Retry button
        if( ReportErrorHookState == ReportErrorHookStates::MONITOR_SELECTION ){
            if( mmu2.RetryIfPossible(ec) ){
                ReportErrorHookState = ReportErrorHookStates::DISMISS_ERROR_SCREEN;
            }
        }
    }

    const uint8_t ei = PrusaErrorCodeIndex(ec);

    switch ((uint8_t)ReportErrorHookState)
    {
    case (uint8_t)ReportErrorHookStates::RENDER_ERROR_SCREEN:
        ReportErrorHookStaticRender(ei);
        ReportErrorHookState = ReportErrorHookStates::MONITOR_SELECTION;
        // Fall through
    case (uint8_t)ReportErrorHookStates::MONITOR_SELECTION:
        mmu2.is_mmu_error_monitor_active = true;
        ReportErrorHookDynamicRender(); // Render dynamic characters
        switch (ReportErrorHookMonitor(ei))
        {
            case 0:
                // No choice selected, return to loop()
                break;
            case 1:
                // More button selected, change state
                ReportErrorHookState = ReportErrorHookStates::RENDER_ERROR_SCREEN;
                break;
            case 2:
                // Exit error screen and enable lcd updates
                lcd_set_custom_characters();
                lcd_update_enable(true);
                lcd_return_to_status();
                // Reset the state in case a new error is reported
                mmu2.is_mmu_error_monitor_active = false;
                ReportErrorHookState = ReportErrorHookStates::RENDER_ERROR_SCREEN;
                break;
            default:
                break;
        }
        return; // Always return to loop() to let MMU trigger a call to ReportErrorHook again
        break;
    case (uint8_t)ReportErrorHookStates::DISMISS_ERROR_SCREEN:
        lcd_set_custom_characters();
        lcd_update_enable(true);
        lcd_return_to_status();
        // Reset the state in case a new error is reported
        mmu2.is_mmu_error_monitor_active = false;
        ReportErrorHookState = ReportErrorHookStates::RENDER_ERROR_SCREEN;
        break;
    default:
        break;
    }
}

void ReportProgressHook(CommandInProgress cip, uint16_t ec) {
    custom_message_type = CustomMsg::MMUProgress;
    lcd_setstatuspgm( _T(ProgressCodeToText(ec)) );
}

} // namespace MMU2
