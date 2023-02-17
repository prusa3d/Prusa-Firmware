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

const char * ProgressCodeToText(uint16_t pc); // we may join progress convertor and reporter together

void BeginReport(CommandInProgress /*cip*/, uint16_t ec) {
    custom_message_type = CustomMsg::MMUProgress;
    lcd_setstatuspgm( _T(ProgressCodeToText(ec)) );
}

void EndReport(CommandInProgress /*cip*/, uint16_t /*ec*/) {
    // clear the status msg line - let the printed filename get visible again
    lcd_setstatuspgm(MSG_WELCOME); // should be seen only when the printer is not printing a file
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
    //! |prusa.io/04504      |     <- URL max 20 characters
    //! |FI:1 FS:1  5>3 t201°|     <- status line, t is thermometer symbol
    //! |>Retry   >Done    >W|     <- buttons
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
    lcd_printf_P(PSTR("%.20S\nprusa.io/04%hu"), _T(PrusaErrorTitle(ei)), PrusaErrorCode(ei) );

    ReportErrorHookSensorLineRender();

    // Render the choices
    //@todo convert MSG_BTN_MORE to PROGMEM_N1
    lcd_show_choices_prompt_P(two_choices ? LCD_LEFT_BUTTON_CHOICE : LCD_MIDDLE_BUTTON_CHOICE, _T(PrusaErrorButtonTitle(button_op_middle)), _T(two_choices ? PrusaErrorButtonMore() : PrusaErrorButtonTitle(button_op_right)), two_choices ? 18 : 9, two_choices ? nullptr : _T(PrusaErrorButtonMore()));
}

void ReportErrorHookSensorLineRender(){
    // Render static characters in third line
    lcd_puts_at_P(0, 2, PSTR("FI:  FS:    >  " LCD_STR_THERMOMETER "   " LCD_STR_DEGREE));
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
    static uint8_t reset_button_selection;

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

    if (reset_button_selection) {
        // If a new error screen is shown, we must reset the button selection
        // Default selection is different depending on how many buttons are present
        current_selection = two_choices ? LCD_LEFT_BUTTON_CHOICE : LCD_MIDDLE_BUTTON_CHOICE;
        choice_selected = -1;
        reset_button_selection = 0;
    }

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
        //! @brief Button menu
        //!
        //! @code{.unparsed}
        //! |01234567890123456789|
        //! |                    |
        //! |                    |
        //! |                    |
        //! |>(left)             |
        //! ----------------------
        //! Three choices 
        //! |>(left)>(mid)>(righ)|
        //! ----------------------
        //! Two choices
        //! ----------------------
        //! |>(left)   >(mid)    |
        //! ----------------------
        //! @endcode
        //
        lcd_putc_at(0, 3, current_selection == LCD_LEFT_BUTTON_CHOICE ? '>': ' ');
        if (two_choices == false)
        {
            lcd_putc_at(9, 3, current_selection == LCD_MIDDLE_BUTTON_CHOICE ? '>': ' ');
            lcd_putc_at(18, 3, current_selection == LCD_RIGHT_BUTTON_CHOICE ? '>': ' ');
        } else {
            // More button for two button screen
            lcd_putc_at(18, 3, current_selection == LCD_MIDDLE_BUTTON_CHOICE ? '>': ' ');
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

    // Next MMU error screen should reset the choice selection
    reset_button_selection = 1;
    return ret;
}

enum class ReportErrorHookStates : uint8_t {
    RENDER_ERROR_SCREEN  = 0,
    MONITOR_SELECTION    = 1,
    DISMISS_ERROR_SCREEN = 2,
};

enum ReportErrorHookStates ReportErrorHookState = ReportErrorHookStates::RENDER_ERROR_SCREEN;

void ReportErrorHook(CommandInProgress /*cip*/, uint16_t ec, uint8_t /*es*/) {
    if (mmu2.MMUCurrentErrorCode() == ErrorCode::OK && mmu2.MMULastErrorSource() == MMU2::ErrorSourceMMU) {
        // If the error code suddenly changes to OK, that means
        // a button was pushed on the MMU and the LCD should
        // dismiss the error screen until MMU raises a new error
        ReportErrorHookState = ReportErrorHookStates::DISMISS_ERROR_SCREEN;
    }

    const uint8_t ei = PrusaErrorCodeIndex(ec);

    switch ((uint8_t)ReportErrorHookState) {
    case (uint8_t)ReportErrorHookStates::RENDER_ERROR_SCREEN:
        ReportErrorHookStaticRender(ei);
        ReportErrorHookState = ReportErrorHookStates::MONITOR_SELECTION;
        [[fallthrough]];
    case (uint8_t)ReportErrorHookStates::MONITOR_SELECTION:
        mmu2.is_mmu_error_monitor_active = true;
        ReportErrorHookDynamicRender(); // Render dynamic characters
        sound_wait_for_user();
        switch (ReportErrorHookMonitor(ei)) {
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
                sound_wait_for_user_reset();
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
        sound_wait_for_user_reset();
        // Reset the state in case a new error is reported
        mmu2.is_mmu_error_monitor_active = false;
        ReportErrorHookState = ReportErrorHookStates::RENDER_ERROR_SCREEN;
        break;
    default:
        break;
    }
}

void ReportProgressHook(CommandInProgress cip, uint16_t ec) {
    if (cip != CommandInProgress::NoCommand) {
        custom_message_type = CustomMsg::MMUProgress;
        lcd_setstatuspgm( _T(ProgressCodeToText(ec)) );
    } else {
        // If there is no command in progress we can display other
        // useful information such as the name of the SD file 
        // being printed
        custom_message_type = CustomMsg::Status;
    }
}

void IncrementLoadFails(){
    eeprom_increment_byte((uint8_t *)EEPROM_MMU_LOAD_FAIL);
    eeprom_increment_word((uint16_t *)EEPROM_MMU_LOAD_FAIL_TOT);
}

void IncrementMMUFails(){
    eeprom_increment_byte((uint8_t *)EEPROM_MMU_FAIL);
    eeprom_increment_word((uint16_t *)EEPROM_MMU_FAIL_TOT);
}

void MakeSound(SoundType s){
    Sound_MakeSound( (eSOUND_TYPE)s);
}

static void FullScreenMsg(const char *pgmS, uint8_t slot){
    lcd_update_enable(false);
    lcd_clear();
    lcd_puts_at_P(0, 1, pgmS);
    lcd_print(' ');
    lcd_print(slot + 1);
}

void FullScreenMsgCut(uint8_t slot){
    FullScreenMsg(_T(MSG_CUT_FILAMENT), slot);
}

void FullScreenMsgEject(uint8_t slot){
    FullScreenMsg(_T(MSG_EJECT_FROM_MMU), slot);
}

void FullScreenMsgTest(uint8_t slot){
    FullScreenMsg(_T(MSG_TESTING_FILAMENT), slot);
}

void FullScreenMsgLoad(uint8_t slot){
    FullScreenMsg(_T(MSG_LOADING_FILAMENT), slot);
}

void FullScreenMsgRestoringTemperature(){
    lcd_display_message_fullscreen_P(_i("MMU Retry: Restoring temperature...")); ////MSG_MMU_RESTORE_TEMP c=20 r=4
}

void ScreenUpdateEnable(){
    lcd_update_enable(true);
}

void ScreenClear(){
    lcd_clear();
}

} // namespace MMU2
