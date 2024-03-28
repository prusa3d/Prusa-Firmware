#include <avr/pgmspace.h>
#include "eeprom.h"
#include "mmu2.h"
#include "mmu2_log.h"
#include "mmu2_reporting.h"
#include "mmu2_error_converter.h"
#include "mmu2_progress_converter.h"
#include "mmu2/error_codes.h"
#include "mmu2/buttons.h"
#include "menu.h"
#include "ultralcd.h"
#include "Filament_sensor.h"
#include "language.h"
#include "temperature.h"
#include "sound.h"

namespace MMU2 {

void BeginReport(CommandInProgress /*cip*/, ProgressCode ec) {
    custom_message_type = CustomMsg::MMUProgress;
    lcd_setstatuspgm( _T(ProgressCodeToText(ec)) );
}

void EndReport(CommandInProgress /*cip*/, ProgressCode /*ec*/) {
    // clear the status msg line - let the printed filename get visible again
    if (!printJobOngoing()) {
        lcd_setstatuspgm(MSG_WELCOME);
    }
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

    lcd_update_enable(false);
    lcd_clear();

    // Print title and header
    lcd_printf_P(PSTR("%.20S\nprusa.io/04%hu"), _T(PrusaErrorTitle(ei)), PrusaErrorCode(ei) );

    ReportErrorHookSensorLineRender();

    // Render the choices
    lcd_show_choices_prompt_P(two_choices ? LCD_LEFT_BUTTON_CHOICE : LCD_MIDDLE_BUTTON_CHOICE, _T(PrusaErrorButtonTitle(button_op_middle)), two_choices ? PrusaErrorButtonMore() : _T(PrusaErrorButtonTitle(button_op_right)), two_choices ? 18 : 9, two_choices ? nullptr : PrusaErrorButtonMore());
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
    static uint8_t reset_button_selection;

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
    if (lcd_encoder) {
        if (two_choices == false) { // third_choice is not nullptr, safe to dereference
            if (lcd_encoder < 0 && current_selection != LCD_LEFT_BUTTON_CHOICE) {
                // Rotating knob counter clockwise
                current_selection--;
            } else if (lcd_encoder > 0 && current_selection != LCD_RIGHT_BUTTON_CHOICE) {
                // Rotating knob clockwise
                current_selection++;
            }
        } else {
            if (lcd_encoder < 0 && current_selection != LCD_LEFT_BUTTON_CHOICE) {
                // Rotating knob counter clockwise
                current_selection = LCD_LEFT_BUTTON_CHOICE;
            } else if (lcd_encoder > 0 && current_selection != LCD_MIDDLE_BUTTON_CHOICE) {
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
        // Consume rotation event
        lcd_encoder = 0;
    }

    // Check if knob was clicked and consume the event
    if (lcd_clicked()) {
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

// Helper variable to monitor knob in MMU error screen in blocking functions e.g. manage_response
static bool is_mmu_error_monitor_active;

// Helper variable to stop rendering the error screen when the firmware is rendering complementary
// UI to resolve the error screen, for example tuning Idler Stallguard Threshold
// Set to false to allow the error screen to render again.
static bool putErrorScreenToSleep;

void CheckErrorScreenUserInput() {
    if (is_mmu_error_monitor_active) {
        // Call this every iteration to keep the knob rotation responsive
        // This includes when mmu_loop is called within manage_response
        ReportErrorHook((CommandInProgress)mmu2.GetCommandInProgress(), mmu2.GetLastErrorCode(), mmu2.MMULastErrorSource());
    }
}

bool TuneMenuEntered() {
    return putErrorScreenToSleep;
}

void ReportErrorHook(CommandInProgress /*cip*/, ErrorCode ec, uint8_t /*es*/) {
    if (putErrorScreenToSleep) return;
    
    if (mmu2.MMUCurrentErrorCode() == ErrorCode::OK && mmu2.MMULastErrorSource() == MMU2::ErrorSourceMMU) {
        // If the error code suddenly changes to OK, that means
        // a button was pushed on the MMU and the LCD should
        // dismiss the error screen until MMU raises a new error
        ReportErrorHookState = ReportErrorHookStates::DISMISS_ERROR_SCREEN;
    }

    const uint8_t ei = PrusaErrorCodeIndex((ErrorCode)ec);

    switch ((uint8_t)ReportErrorHookState) {
    case (uint8_t)ReportErrorHookStates::RENDER_ERROR_SCREEN:
        KEEPALIVE_STATE(PAUSED_FOR_USER);
        ReportErrorHookStaticRender(ei);
        ReportErrorHookState = ReportErrorHookStates::MONITOR_SELECTION;
        [[fallthrough]];
    case (uint8_t)ReportErrorHookStates::MONITOR_SELECTION:
        is_mmu_error_monitor_active = true;
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
                lcd_update_enable(true);
                lcd_return_to_status();
                sound_wait_for_user_reset();
                // Reset the state in case a new error is reported
                is_mmu_error_monitor_active = false;
                KEEPALIVE_STATE(IN_HANDLER);
                ReportErrorHookState = ReportErrorHookStates::RENDER_ERROR_SCREEN;
                break;
            default:
                break;
        }
        return; // Always return to loop() to let MMU trigger a call to ReportErrorHook again
        break;
    case (uint8_t)ReportErrorHookStates::DISMISS_ERROR_SCREEN:
        lcd_update_enable(true);
        lcd_return_to_status();
        sound_wait_for_user_reset();
        // Reset the state in case a new error is reported
        is_mmu_error_monitor_active = false;
        KEEPALIVE_STATE(IN_HANDLER);
        ReportErrorHookState = ReportErrorHookStates::RENDER_ERROR_SCREEN;
        break;
    default:
        break;
    }
}

void ReportProgressHook(CommandInProgress cip, ProgressCode ec) {
    if (cip != CommandInProgress::NoCommand) {
        custom_message_type = CustomMsg::MMUProgress;
        lcd_setstatuspgm( _T(ProgressCodeToText(ec)) );
    }
}

TryLoadUnloadReporter::TryLoadUnloadReporter(float delta_mm)
: dpixel0(0)
, dpixel1(0)
, lcd_cursor_col(0)
, pixel_per_mm(0.5F * float(LCD_WIDTH) / (delta_mm))
{
    lcd_clearstatus();
}

TryLoadUnloadReporter::~TryLoadUnloadReporter() {
    // Delay the next status message just so
    // the user can see the results clearly
    lcd_reset_status_message_timeout();
}

void TryLoadUnloadReporter::Render(uint8_t col, bool sensorState) {
    // Set the cursor position each time in case some other
    // part of the firmware changes the cursor position
    lcd_insert_char_into_status(col, sensorState ? LCD_STR_SOLID_BLOCK[0] : '-');
    if (!lcd_update_enabled) lcdui_print_status_line();
}

void TryLoadUnloadReporter::Progress(bool sensorState){
    // Always round up, you can only have 'whole' pixels. (floor is also an option)
    dpixel1 = ceil((stepper_get_machine_position_E_mm() - planner_get_current_position_E()) * pixel_per_mm);
    if (dpixel1 - dpixel0) {
        dpixel0 = dpixel1;
        if (lcd_cursor_col > (LCD_WIDTH - 1)) lcd_cursor_col = LCD_WIDTH - 1;
        Render(lcd_cursor_col++, sensorState);
    }
}

void TryLoadUnloadReporter::DumpToSerial(){
    char buf[LCD_WIDTH + 1];
    lcd_getstatus(buf);
    for (uint8_t i = 0; i < sizeof(buf); i++) {
        // 0xFF is -1 when converting from unsigned to signed char
        // If the number is negative, that means filament is present
        buf[i] = (buf[i] < 0) ? '1' : '0';
    }
    buf[LCD_WIDTH] = 0;
    MMU2_ECHO_MSGLN(buf);
}

/// Disables MMU in EEPROM
void DisableMMUInSettings() {
    eeprom_update_byte_notify((uint8_t *)EEPROM_MMU_ENABLED, false);
    mmu2.Status();
}

void IncrementLoadFails(){
    eeprom_increment_byte((uint8_t *)EEPROM_MMU_LOAD_FAIL);
    eeprom_increment_word((uint16_t *)EEPROM_MMU_LOAD_FAIL_TOT);
}

void IncrementMMUFails(){
    eeprom_increment_byte((uint8_t *)EEPROM_MMU_FAIL);
    eeprom_increment_word((uint16_t *)EEPROM_MMU_FAIL_TOT);
}

bool cutter_enabled(){
    return eeprom_read_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED) == EEPROM_MMU_CUTTER_ENABLED_enabled;
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
    lcd_display_message_fullscreen_P(_T(MSG_MMU_RESTORE_TEMP));
}

void ScreenUpdateEnable(){
    lcd_update_enable(true);
}

void ScreenClear(){
    lcd_clear();
}

struct TuneItem {
    uint8_t address;
    uint8_t minValue;
    uint8_t maxValue;
} __attribute__((packed));

static const TuneItem TuneItems[] PROGMEM = {
  { (uint8_t)Register::Selector_sg_thrs_R, 1, 4},
  { (uint8_t)Register::Idler_sg_thrs_R, 2, 10},
};

static_assert(sizeof(TuneItems)/sizeof(TuneItem) == 2);

struct _menu_tune_data_t
{
    menu_data_edit_t reserved; //13 bytes reserved for number editing functions
    int8_t status;             // 1 byte
    uint8_t currentValue;      // 1 byte
    TuneItem item;             // 3 bytes
};

static_assert(sizeof(_menu_tune_data_t) == 18);
static_assert(sizeof(menu_data)>= sizeof(_menu_tune_data_t),"_menu_tune_data_t doesn't fit into menu_data");

void tuneIdlerStallguardThresholdMenu() {
    static constexpr _menu_tune_data_t * const _md = (_menu_tune_data_t*)&(menu_data[0]);

    // Do not timeout the screen, otherwise there will be FW crash (menu recursion)
    lcd_timeoutToStatus.stop();
    if (_md->status == 0)
    {
        _md->status = 1; // Menu entered for the first time

        // Fetch the TuneItem from PROGMEM
        const uint8_t offset = (mmu2.MMUCurrentErrorCode() == ErrorCode::HOMING_IDLER_FAILED) ? 1 : 0;
        memcpy_P(&(_md->item), &TuneItems[offset], sizeof(TuneItem));

        // Fetch the value which is currently in MMU EEPROM
        mmu2.ReadRegister(_md->item.address);
        _md->currentValue = mmu2.GetLastReadRegisterValue();
    }

    MENU_BEGIN();
    ON_MENU_LEAVE(
        mmu2.WriteRegister(_md->item.address, (uint16_t)_md->currentValue);
        putErrorScreenToSleep = false;
        lcd_return_to_status();
        return;
    );
    MENU_ITEM_BACK_P(_T(MSG_DONE));
    MENU_ITEM_EDIT_int3_P(
        _T(MSG_MMU_SENSITIVITY),
        &_md->currentValue,
        _md->item.minValue,
        _md->item.maxValue
    );
    MENU_END();
}

void tuneIdlerStallguardThreshold() {
    putErrorScreenToSleep = true;
    menu_submenu(tuneIdlerStallguardThresholdMenu);
}

} // namespace MMU2
