#include "mmu2.h"
#include "mmu2_config.h"
#include "mmu2_error_converter.h"
#include "mmu2_fsensor.h"
#include "mmu2_log.h"
#include "mmu2_power.h"
#include "mmu2_progress_converter.h"
#include "mmu2_reporting.h"

#include "Marlin.h"
#include "language.h"
#include "messages.h"
#include "sound.h"
#include "stepper.h"
#include "strlen_cx.h"
#include "temperature.h"
#include "ultralcd.h"
#include "SpoolJoin.h"

// As of FW 3.12 we only support building the FW with only one extruder, all the multi-extruder infrastructure will be removed.
// Saves at least 800B of code size
static_assert(EXTRUDERS==1);

namespace MMU2 {

template<typename F>
void waitForHotendTargetTemp(uint16_t delay, F f){
    while (((degTargetHotend(active_extruder) - degHotend(active_extruder)) > 5)) {
        f();
        delay_keep_alive(delay);
    }
}

void WaitForHotendTargetTempBeep(){
    waitForHotendTargetTemp(3000, []{ Sound_MakeSound(e_SOUND_TYPE_StandardPrompt); } );
}

MMU2 mmu2;

MMU2::MMU2()
    : is_mmu_error_monitor_active(false)
    , logic(&mmu2Serial, MMU2_TOOL_CHANGE_LOAD_LENGTH)
    , extruder(MMU2_NO_TOOL)
    , tool_change_extruder(MMU2_NO_TOOL)
    , resume_position()
    , resume_hotend_temp(0)
    , logicStepLastStatus(StepStatus::Finished)
    , state(xState::Stopped)
    , mmu_print_saved(SavedState::None)
    , loadFilamentStarted(false)
    , unloadFilamentStarted(false)
    , loadingToNozzle(false)
    , inAutoRetry(false)
    , retryAttempts(MAX_RETRIES)
    , toolchange_counter(0)
    , tmcFailures(0)
{
}

void MMU2::Start() {
#ifdef MMU_HWRESET
    WRITE(MMU_RST_PIN, 1);
    SET_OUTPUT(MMU_RST_PIN); // setup reset pin
#endif //MMU_HWRESET

    mmu2Serial.begin(MMU_BAUD);

    PowerOn(); // I repurposed this to serve as our EEPROM disable toggle.
    Reset(ResetForm::ResetPin);

    mmu2Serial.flush(); // make sure the UART buffer is clear before starting communication

    extruder = MMU2_NO_TOOL;
    state = xState::Connecting;

    // start the communication
    logic.Start();

    ResetRetryAttempts();
}

void MMU2::Stop() {
    StopKeepPowered();
    PowerOff(); // This also disables the MMU in the EEPROM.
}

void MMU2::StopKeepPowered(){
    state = xState::Stopped;
    logic.Stop();
    mmu2Serial.close();
}

void MMU2::Reset(ResetForm level){
    switch (level) {
    case Software: ResetX0(); break;
    case ResetPin: TriggerResetPin(); break;
    case CutThePower: PowerCycle(); break;
    default: break;
    }
}

void MMU2::ResetX0() {
    logic.ResetMMU(); // Send soft reset
}

void MMU2::TriggerResetPin(){
    reset();
}

void MMU2::PowerCycle(){
    // cut the power to the MMU and after a while restore it
    // Sadly, MK3/S/+ cannot do this 
    // NOTE: the below will toggle the EEPROM var. Should we
    // assert this function is never called in the MK3 FW? Do we even care?
    PowerOff();
    delay_keep_alive(1000);
    PowerOn();
}

void MMU2::PowerOff(){
    power_off();
}

void MMU2::PowerOn(){
    power_on();
}

bool MMU2::ReadRegister(uint8_t address){
    if( ! WaitForMMUReady())
        return false;
    do {
        logic.ReadRegister(address); // we may signal the accepted/rejected status of the response as return value of this function
    } while( ! manage_response(false, false) );

    return true;
}

bool MMU2::WriteRegister(uint8_t address, uint16_t data){
    if( ! WaitForMMUReady())
        return false;

    // special case - intercept requests of extra loading distance and perform the change even on the printer's side
    if( address == 0x0b ){
        logic.PlanExtraLoadDistance(data);
    }

    do {
        logic.WriteRegister(address, data); // we may signal the accepted/rejected status of the response as return value of this function
    } while( ! manage_response(false, false) );

    return true;
}

void MMU2::mmu_loop() {
    // We only leave this method if the current command was successfully completed - that's the Marlin's way of blocking operation
    // Atomic compare_exchange would have been the most appropriate solution here, but this gets called only in Marlin's task,
    // so thread safety should be kept
    static bool avoidRecursion = false;
    if (avoidRecursion)
        return;
    avoidRecursion = true;

    mmu_loop_inner(true);

    avoidRecursion = false;
}

void __attribute__((noinline)) MMU2::mmu_loop_inner(bool reportErrors) {
    logicStepLastStatus = LogicStep(reportErrors); // it looks like the mmu_loop doesn't need to be a blocking call

    if (is_mmu_error_monitor_active) {
        // Call this every iteration to keep the knob rotation responsive
        // This includes when mmu_loop is called within manage_response
        ReportErrorHook((uint16_t)lastErrorCode);
    }
}

void MMU2::CheckFINDARunout() {
    // Check for FINDA filament runout
    if (!FindaDetectsFilament() && check_fsensor()) {
        SERIAL_ECHOLNPGM("FINDA filament runout!");
        stop_and_save_print_to_ram(0, 0);
        restore_print_from_ram_and_continue(0);
        if (SpoolJoin::spooljoin.isSpoolJoinEnabled() && get_current_tool() != (uint8_t)FILAMENT_UNKNOWN){ // Can't auto if F=?
            enquecommand_front_P(PSTR("M600 AUTO")); // save print and run M600 command
        } else {
            enquecommand_front_P(PSTR("M600")); // save print and run M600 command
        }
    }
}

struct ReportingRAII {
    CommandInProgress cip;
    inline ReportingRAII(CommandInProgress cip):cip(cip){
        BeginReport(cip, (uint16_t)ProgressCode::EngagingIdler);
    }
    inline ~ReportingRAII(){
        EndReport(cip, (uint16_t)ProgressCode::OK);
    }
};

bool MMU2::WaitForMMUReady(){
    switch(State()){
    case xState::Stopped:
        return false;
    case xState::Connecting:
        // shall we wait until the MMU reconnects?
        // fire-up a fsm_dlg and show "MMU not responding"?
    default:
        return true;
    }
}

bool MMU2::RetryIfPossible(uint16_t ec){
    if( retryAttempts ){
        SERIAL_ECHOPGM("retryAttempts=");SERIAL_ECHOLN((uint16_t)retryAttempts);
        SetButtonResponse(ButtonOperations::Retry);
        // check, that Retry is actually allowed on that operation
        if( ButtonAvailable(ec) != NoButton ){
            inAutoRetry = true;
            SERIAL_ECHOLNPGM("RetryButtonPressed");
            // We don't decrement until the button is acknowledged by the MMU.
            //--retryAttempts; // "used" one retry attempt
            return true;
        }
    }
    inAutoRetry = false;
    return false;
}

void MMU2::ResetRetryAttempts(){
    SERIAL_ECHOLNPGM("ResetRetryAttempts");
    retryAttempts = MAX_RETRIES;
}

void MMU2::DecrementRetryAttempts() {
    if (inAutoRetry && retryAttempts) {
        SERIAL_ECHOLNPGM("DecrementRetryAttempts");
        retryAttempts--;
    }
}

bool MMU2::VerifyFilamentEnteredPTFE()
{
    st_synchronize();

    if (!fsensor.getFilamentPresent()) return false;

    uint8_t fsensorState = 0;
    // MMU has finished its load, push the filament further by some defined constant length
    // If the filament sensor reads 0 at any moment, then report FAILURE
    current_position[E_AXIS] += MMU2_EXTRUDER_PTFE_LENGTH + MMU2_EXTRUDER_HEATBREAK_LENGTH - (logic.ExtraLoadDistance() - MMU2_FILAMENT_SENSOR_POSITION);
    plan_buffer_line_curposXYZE(MMU2_VERIFY_LOAD_TO_NOZZLE_FEED_RATE);
    current_position[E_AXIS] -= (MMU2_EXTRUDER_PTFE_LENGTH + MMU2_EXTRUDER_HEATBREAK_LENGTH - (logic.ExtraLoadDistance() - MMU2_FILAMENT_SENSOR_POSITION));
    plan_buffer_line_curposXYZE(MMU2_VERIFY_LOAD_TO_NOZZLE_FEED_RATE);

    while(blocks_queued())
    {
        // Wait for move to finish and monitor the fsensor the entire time
        // A single 0 reading will set the bit.
        fsensorState |= !fsensor.getFilamentPresent();
        manage_heater();
        manage_inactivity(true);
    }

    if (fsensorState)
    {
        IncrementLoadFails();
        return false;
    } else {
        // else, happy printing! :)
        return true;
    }
}

bool MMU2::ToolChangeCommonOnce(uint8_t slot){
    static_assert(MAX_RETRIES > 1); // need >1 retries to do the cut in the last attempt
    for(uint8_t retries = MAX_RETRIES; retries; --retries){
        for(;;) {
            tool_change_extruder = slot;
            logic.ToolChange(slot); // let the MMU pull the filament out and push a new one in
            if( manage_response(true, true) )
                break;
            // otherwise: failed to perform the command - unload first and then let it run again
            IncrementMMUFails();

            // just in case we stood in an error screen for too long and the hotend got cold
            ResumeHotendTemp();
            // if the extruder has been parked, it will get unparked once the ToolChange command finishes OK
            // - so no ResumeUnpark() at this spot

            unload();
            // if we run out of retries, we must do something ... may be raise an error screen and allow the user to do something
            // but honestly - if the MMU restarts during every toolchange,
            // something else is seriously broken and stopping a print is probably our best option.
        }
        // reset current position to whatever the planner thinks it is
        plan_set_e_position(current_position[E_AXIS]);
        if (VerifyFilamentEnteredPTFE()){
            return true; // success
        } else { // Prepare a retry attempt
            unload();
            if( retries == 1 && eeprom_read_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED) == EEPROM_MMU_CUTTER_ENABLED_enabled){
                cut_filament(slot); // try cutting filament tip at the last attempt
            }
        }
    }
    return false; // couldn't accomplish the task
}

void MMU2::ToolChangeCommon(uint8_t slot){
    while( ! ToolChangeCommonOnce(slot) ){ // while not successfully fed into extruder's PTFE tube
        // failed autoretry, report an error by forcing a "printer" error into the MMU infrastructure - it is a hack to leverage existing code
        logic.SetPrinterError(ErrorCode::LOAD_TO_EXTRUDER_FAILED);
        SaveAndPark(true);
        SaveHotendTemp(true);
        // We only have to wait for the user to fix the issue and press "Retry".
        // Please see CheckUserInput() for details how we "leave" manage_response.
        // If manage_response returns false at this spot (MMU operation interrupted aka MMU reset)
        // we can safely continue because the MMU is not doing an operation now.
        manage_response(true, true);
    }

    extruder = slot; //filament change is finished
    SpoolJoin::spooljoin.setSlot(slot);

    // @@TODO really report onto the serial? May be for the Octoprint? Not important now
    //        SERIAL_ECHO_START();
    //        SERIAL_ECHOLNPAIR(MSG_ACTIVE_EXTRUDER, int(extruder));
    ++toolchange_counter;
}

bool MMU2::tool_change(uint8_t slot) {
    if( ! WaitForMMUReady())
        return false;

    if (slot != extruder) {
        if (FindaDetectsFilament()) {
            // If Tcodes are used manually through the serial
            // we need to unload manually as well -- but only if FINDA detects filament
            unload();
        }

        ReportingRAII rep(CommandInProgress::ToolChange);
        FSensorBlockRunout blockRunout;
        st_synchronize();
        ToolChangeCommon(slot);
    }
    return true;
}

/// Handle special T?/Tx/Tc commands
///
///- T? Gcode to extrude shouldn't have to follow, load to extruder wheels is done automatically
///- Tx Same as T?, except nozzle doesn't have to be preheated. Tc must be placed after extruder nozzle is preheated to finish filament load.
///- Tc Load to nozzle after filament was prepared by Tx and extruder nozzle is already heated.
bool MMU2::tool_change(char code, uint8_t slot) {
    if( ! WaitForMMUReady())
        return false;

    FSensorBlockRunout blockRunout;

    switch (code) {
    case '?': {
        waitForHotendTargetTemp(100, []{});
        load_filament_to_nozzle(slot);
    } break;

    case 'x': {
        set_extrude_min_temp(0); // Allow cold extrusion since Tx only loads to the gears not nozzle
        st_synchronize();
        ToolChangeCommon(slot); // the only difference was manage_response(false, false), but probably good enough
        set_extrude_min_temp(EXTRUDE_MINTEMP);
    } break;

    case 'c': {
        waitForHotendTargetTemp(100, []{});
        execute_load_to_nozzle_sequence();
    } break;
    }

    return true;
}

void MMU2::get_statistics() {
    logic.Statistics();
}

uint8_t MMU2::get_current_tool() const {
    return extruder == MMU2_NO_TOOL ? (uint8_t)FILAMENT_UNKNOWN : extruder;
}

uint8_t MMU2::get_tool_change_tool() const {
    return tool_change_extruder == MMU2_NO_TOOL ? (uint8_t)FILAMENT_UNKNOWN : tool_change_extruder;
}

bool MMU2::set_filament_type(uint8_t slot, uint8_t type) {
    if( ! WaitForMMUReady())
        return false;
    
    // @@TODO - this is not supported in the new MMU yet
    slot = slot; // @@TODO
    type = type; // @@TODO
    // cmd_arg = filamentType;
    // command(MMU_CMD_F0 + index);

    if( ! manage_response(false, false) ){
        // @@TODO failed to perform the command - retry
        ;
    } // true, true); -- Comment: how is it possible for a filament type set to fail?
    
    return true;
}

bool MMU2::unload() {
    if( ! WaitForMMUReady())
        return false;

    WaitForHotendTargetTempBeep();

    {
        FSensorBlockRunout blockRunout;
        ReportingRAII rep(CommandInProgress::UnloadFilament);
        filament_ramming();

        // we assume the printer managed to relieve filament tip from the gears,
        // so repeating that part in case of an MMU restart is not necessary
        for(;;) {
            logic.UnloadFilament();
            if( manage_response(false, true) )
                break;
            IncrementMMUFails();
        }

        Sound_MakeSound(e_SOUND_TYPE_StandardConfirm);

        // no active tool
        extruder = MMU2_NO_TOOL;
        tool_change_extruder = MMU2_NO_TOOL;
    }
    return true;
}

bool MMU2::cut_filament(uint8_t slot){
    if( ! WaitForMMUReady())
        return false;

    ReportingRAII rep(CommandInProgress::CutFilament);

    for(;;){
        logic.CutFilament(slot);
        if( manage_response(false, true) )
            break;
        IncrementMMUFails();
    }

    return true;
}

void FullScreenMsg(const char *pgmS, uint8_t slot){
    lcd_update_enable(false);
    lcd_clear();
    lcd_puts_at_P(0, 1, pgmS);
    lcd_print(' ');
    lcd_print(slot + 1);
}

bool MMU2::loading_test(uint8_t slot){
    FullScreenMsg(_T(MSG_TESTING_FILAMENT), slot);
    tool_change(slot);
    st_synchronize();
    unload();
    lcd_update_enable(true);
    return true;
}

bool MMU2::load_filament(uint8_t slot) {
    if( ! WaitForMMUReady())
        return false;

    FullScreenMsg(_T(MSG_LOADING_FILAMENT), slot);

    ReportingRAII rep(CommandInProgress::LoadFilament);
    for(;;) {
        logic.LoadFilament(slot);
        if( manage_response(false, false) )
            break;
        IncrementMMUFails();
    }

    Sound_MakeSound(e_SOUND_TYPE_StandardConfirm);

    lcd_update_enable(true);

    return true;
}

struct LoadingToNozzleRAII {
    MMU2 &mmu2;
    explicit inline LoadingToNozzleRAII(MMU2 &mmu2):mmu2(mmu2){
        mmu2.loadingToNozzle = true;
    }
    inline ~LoadingToNozzleRAII(){
        mmu2.loadingToNozzle = false;
    }
};

bool MMU2::load_filament_to_nozzle(uint8_t slot) {
    if( ! WaitForMMUReady())
        return false;

    LoadingToNozzleRAII ln(*this);

    WaitForHotendTargetTempBeep();

    FullScreenMsg(_T(MSG_LOADING_FILAMENT), slot);
    {
        // used for MMU-menu operation "Load to Nozzle"
        ReportingRAII rep(CommandInProgress::ToolChange);
        FSensorBlockRunout blockRunout;

        if( extruder != MMU2_NO_TOOL ){ // we already have some filament loaded - free it + shape its tip properly
            filament_ramming();
        }

        ToolChangeCommon(slot);

        // Finish loading to the nozzle with finely tuned steps.
        execute_load_to_nozzle_sequence();
        Sound_MakeSound(e_SOUND_TYPE_StandardConfirm);
    }
    lcd_update_enable(true);
    return true;
}

bool MMU2::eject_filament(uint8_t slot, bool recover) {
    if( ! WaitForMMUReady())
        return false;

    ReportingRAII rep(CommandInProgress::EjectFilament);
    current_position[E_AXIS] -= MMU2_FILAMENTCHANGE_EJECT_FEED;
    plan_buffer_line_curposXYZE(2500.F / 60.F);
    st_synchronize();
    logic.EjectFilament(slot);
    if( ! manage_response(false, false) ){
        // @@TODO failed to perform the command - retry
        ;
    }

    if (recover) {
        //        LCD_MESSAGEPGM(MSG_MMU2_EJECT_RECOVER);
        Sound_MakeSound(e_SOUND_TYPE_StandardPrompt);
//@@TODO        wait_for_user = true;
        
        //#if ENABLED(HOST_PROMPT_SUPPORT)
        //        host_prompt_do(PROMPT_USER_CONTINUE, PSTR("MMU2 Eject Recover"), PSTR("Continue"));
        //#endif
        //#if ENABLED(EXTENSIBLE_UI)
        //        ExtUI::onUserConfirmRequired_P(PSTR("MMU2 Eject Recover"));
        //#endif
        
//@@TODO        while (wait_for_user) idle(true);
        
        Sound_MakeSound(e_SOUND_TYPE_StandardConfirm);
        // logic.Command(); //@@TODO command(MMU_CMD_R0);
        if( ! manage_response(false, false) ){
            // @@TODO failed to perform the command - retry
            ;
        }
    }

    // no active tool
    extruder = MMU2_NO_TOOL;
    tool_change_extruder = MMU2_NO_TOOL;
    Sound_MakeSound(e_SOUND_TYPE_StandardConfirm);
//    disable_E0();

    return true;
}

void MMU2::Button(uint8_t index){
    LogEchoEvent_P(PSTR("Button"));
    logic.Button(index);
}

void MMU2::Home(uint8_t mode){
    logic.Home(mode);
}

void MMU2::SaveHotendTemp(bool turn_off_nozzle) {
    if (mmu_print_saved & SavedState::Cooldown) return;

    if (turn_off_nozzle && !(mmu_print_saved & SavedState::CooldownPending)){
        resume_hotend_temp = degTargetHotend(active_extruder);
        mmu_print_saved |= SavedState::CooldownPending;
        LogEchoEvent_P(PSTR("Heater cooldown pending"));
    }
}

void MMU2::SaveAndPark(bool move_axes) {
    if (mmu_print_saved == SavedState::None) { // First occurrence. Save current position, park print head, disable nozzle heater.
        LogEchoEvent_P(PSTR("Saving and parking"));
        st_synchronize();

        if (move_axes){
            mmu_print_saved |= SavedState::ParkExtruder;
            // save current pos
            for(uint8_t i = 0; i < 3; ++i){
                resume_position.xyz[i] = current_position[i];
            }

            // lift Z
            raise_z(MMU_ERR_Z_PAUSE_LIFT);

            // move XY aside
            if (axis_known_position[X_AXIS] && axis_known_position[Y_AXIS])
            {
                current_position[X_AXIS] = MMU_ERR_X_PAUSE_POS;
                current_position[Y_AXIS] = MMU_ERR_Y_PAUSE_POS;
                plan_buffer_line_curposXYZE(NOZZLE_PARK_XY_FEEDRATE);
                st_synchronize();
            }
        }
    }
    // keep the motors powered forever (until some other strategy is chosen)
    // @@TODO do we need that in 8bit?
    // gcode.reset_stepper_timeout();
}

void MMU2::ResumeHotendTemp() {
    if ((mmu_print_saved & SavedState::CooldownPending))
    {
        // Clear the "pending" flag if we haven't cooled yet.
        mmu_print_saved &= ~(SavedState::CooldownPending);
        LogEchoEvent_P(PSTR("Cooldown flag cleared"));
    }
    if ((mmu_print_saved & SavedState::Cooldown) && resume_hotend_temp) {
        LogEchoEvent_P(PSTR("Resuming Temp"));
        MMU2_ECHO_MSGRPGM(PSTR("Restoring hotend temperature "));
        SERIAL_ECHOLN(resume_hotend_temp);
        mmu_print_saved &= ~(SavedState::Cooldown);
        setTargetHotend(resume_hotend_temp, active_extruder);
        lcd_display_message_fullscreen_P(_i("MMU Retry: Restoring temperature...")); ////MSG_MMU_RESTORE_TEMP c=20 r=4
        //@todo better report the event and let the GUI do its work somewhere else
        ReportErrorHookSensorLineRender();
        waitForHotendTargetTemp(100, []{
            manage_inactivity(true);
            mmu2.mmu_loop_inner(false);
            ReportErrorHookDynamicRender();
        });
        lcd_update_enable(true); // temporary hack to stop this locking the printer...
        LogEchoEvent_P(PSTR("Hotend temperature reached"));
        lcd_clear();
    }
}

void MMU2::ResumeUnpark(){
    if (mmu_print_saved & SavedState::ParkExtruder) {
        LogEchoEvent_P(PSTR("Resuming XYZ"));

        current_position[X_AXIS] = resume_position.xyz[X_AXIS];
        current_position[Y_AXIS] = resume_position.xyz[Y_AXIS];
        plan_buffer_line_curposXYZE(NOZZLE_PARK_XY_FEEDRATE);
        st_synchronize();
        
        current_position[Z_AXIS] = resume_position.xyz[Z_AXIS];
        plan_buffer_line_curposXYZE(NOZZLE_PARK_Z_FEEDRATE);
        st_synchronize();
        mmu_print_saved &= ~(SavedState::ParkExtruder);
    }
}

void MMU2::CheckUserInput(){
    auto btn = ButtonPressed((uint16_t)lastErrorCode);

    // Was a button pressed on the MMU itself instead of the LCD?
    if (btn == Buttons::NoButton && lastButton != Buttons::NoButton){
        btn = lastButton;
        lastButton = Buttons::NoButton; // Clear it. 
    }

    switch (btn) {
    case Left:
    case Middle:
    case Right:
        SERIAL_ECHOPGM("CheckUserInput-btnLMR ");
        SERIAL_ECHOLN(btn);
        ResumeHotendTemp(); // Recover the hotend temp before we attempt to do anything else...

        // In case of LOAD_TO_EXTRUDER_FAILED sending a button into the MMU has an interesting side effect
        // - it triggers the standalone LoadFilament function on the current active slot.
        // Considering the fact, that we are recovering from a failed load to extruder, this side effect is actually quite beneficial
        // - it checks if the filament is correctly loaded in the MMU (we assume the user was playing with the filament to recover from the failed load)
        Button(btn);

        // A quick hack: for specific error codes move the E-motor every time.
        // Not sure if we can rely on the fsensor.
        // Just plan the move, let the MMU take over when it is ready
        switch(lastErrorCode){
        case ErrorCode::FSENSOR_DIDNT_SWITCH_OFF:
        case ErrorCode::FSENSOR_TOO_EARLY:
            HelpUnloadToFinda();
            break;
        case ErrorCode::LOAD_TO_EXTRUDER_FAILED:
            // A horrible hack - clear the explicit printer error allowing manage_response to recover on MMU's Finished state
            // Moreover - if the MMU is currently doing something (like the LoadFilament - see comment above)
            // we'll actually wait for it automagically in manage_response and after it finishes correctly,
            // we'll issue another command (like toolchange)
            logic.ClearPrinterError();
            break;
        default:
            break;
        }
        break;
    case RestartMMU:
        Reset(ResetPin); // we cannot do power cycle on the MK3
        // ... but mmu2_power.cpp knows this and triggers a soft-reset instead.
        break;
    case DisableMMU:
        Stop(); // Poweroff handles updating the EEPROM shutoff.
        break;
    case StopPrint:
        // @@TODO not sure if we shall handle this high level operation at this spot
        break;
    default:
        break;
    }
}

/// Originally, this was used to wait for response and deal with timeout if necessary.
/// The new protocol implementation enables much nicer and intense reporting, so this method will boil down
/// just to verify the result of an issued command (which was basically the original idea)
///
/// It is closely related to mmu_loop() (which corresponds to our ProtocolLogic::Step()), which does NOT perform any blocking wait for a command to finish.
/// But - in case of an error, the command is not yet finished, but we must react accordingly - move the printhead elsewhere, stop heating, eat a cat or so.
/// That's what's being done here...
bool MMU2::manage_response(const bool move_axes, const bool turn_off_nozzle) {
    mmu_print_saved = SavedState::None;

    KEEPALIVE_STATE(IN_PROCESS);

    LongTimer nozzleTimeout;

    for (;;) {
        // in our new implementation, we know the exact state of the MMU at any moment, we do not have to wait for a timeout
        // So in this case we shall decide if the operation is:
        // - still running -> wait normally in idle()
        // - failed -> then do the safety moves on the printer like before
        // - finished ok -> proceed with reading other commands
        manage_heater();
        manage_inactivity(true); // calls LogicStep() and remembers its return status
        lcd_update(0);

        if (mmu_print_saved & SavedState::CooldownPending){
            if (!nozzleTimeout.running()){
                nozzleTimeout.start();
                LogEchoEvent_P(PSTR("Cooling Timeout started"));
            } else if (nozzleTimeout.expired(DEFAULT_SAFETYTIMER_TIME_MINS*60*1000ul)){ // mins->msec.
                mmu_print_saved &= ~(SavedState::CooldownPending);
                mmu_print_saved |= SavedState::Cooldown;
                setAllTargetHotends(0);
                LogEchoEvent_P(PSTR("Heater cooldown"));
            }
        } else if (nozzleTimeout.running()) {
            nozzleTimeout.stop();
            LogEchoEvent_P(PSTR("Cooling timer stopped"));
        }

        switch (logicStepLastStatus) {
        case Finished: 
            // command/operation completed, let Marlin continue its work
            // the E may have some more moves to finish - wait for them
            ResumeHotendTemp();
            ResumeUnpark(); // We can now travel back to the tower or wherever we were when we saved.
            ResetRetryAttempts(); // Reset the retry counter.
            st_synchronize(); 
            return true;
        case Interrupted:
            // now what :D ... big bad ... ramming, unload, retry the whole command originally issued
            return false;
        case VersionMismatch: // this basically means the MMU will be disabled until reconnected
            CheckUserInput();
            return true;
        case PrinterError:
            CheckUserInput();
            // if button pressed "Done", return true, otherwise stay within manage_response
            // Please see CheckUserInput() for details how we "leave" manage_response
            break;
        case CommandError:
        case CommunicationTimeout:
        case ProtocolError:
        case ButtonPushed:
            if (!inAutoRetry){
                // Don't proceed to the park/save if we are doing an autoretry.
                SaveAndPark(move_axes);
                SaveHotendTemp(turn_off_nozzle);
                CheckUserInput();
            }
            break;
        case CommunicationRecovered: // @@TODO communication recovered and may be an error recovered as well
            // may be the logic layer can detect the change of state a respond with one "Recovered" to be handled here
            ResumeHotendTemp();
            ResumeUnpark();
            break;
        case Processing: // wait for the MMU to respond
        default:
            break;
        }
    }
}

StepStatus MMU2::LogicStep(bool reportErrors) {
    CheckUserInput(); // Process any buttons before proceeding with another MMU Query
    StepStatus ss = logic.Step();
    switch (ss) {
    case Finished:
        // At this point it is safe to trigger a runout and not interrupt the MMU protocol
        CheckFINDARunout();
        break;
    case Processing:
        OnMMUProgressMsg(logic.Progress());
        break;
    case ButtonPushed:
        lastButton = logic.Button();
        LogEchoEvent_P(PSTR("MMU Button pushed"));
        CheckUserInput(); // Process the button immediately
        break;
    case Interrupted:
        // can be silently handed over to a higher layer, no processing necessary at this spot
        break;
    default:
        if(reportErrors) {
            switch (ss)
            {
            case CommandError:
                ReportError(logic.Error(), ErrorSourceMMU);
                break;
            case CommunicationTimeout:
                state = xState::Connecting;
                ReportError(ErrorCode::MMU_NOT_RESPONDING, ErrorSourcePrinter);
                break;
            case ProtocolError:
                state = xState::Connecting;
                ReportError(ErrorCode::PROTOCOL_ERROR, ErrorSourcePrinter);
                break;
            case VersionMismatch:
                StopKeepPowered();
                ReportError(ErrorCode::VERSION_MISMATCH, ErrorSourcePrinter);
                break;
            case PrinterError:
                ReportError(logic.PrinterError(), ErrorSourcePrinter);
                break;
            default:
                break;
            }
        }
    }

    if( logic.Running() ){
        state = xState::Active;
    }
    return ss;
}

void MMU2::filament_ramming() {
    execute_extruder_sequence((const E_Step *)ramming_sequence, sizeof(ramming_sequence) / sizeof(E_Step));
}

void MMU2::execute_extruder_sequence(const E_Step *sequence, uint8_t steps) {
    st_synchronize();
    const E_Step *step = sequence;
    for (uint8_t i = 0; i < steps; i++) {
        current_position[E_AXIS] += pgm_read_float(&(step->extrude));
        plan_buffer_line_curposXYZE(pgm_read_float(&(step->feedRate)));
        st_synchronize();
        step++;
    }
}

void MMU2::execute_load_to_nozzle_sequence() {
    st_synchronize();
    // Compensate for configurable Extra Loading Distance
    current_position[E_AXIS] -= (logic.ExtraLoadDistance() - MMU2_FILAMENT_SENSOR_POSITION);
    execute_extruder_sequence((const E_Step *)load_to_nozzle_sequence, sizeof(load_to_nozzle_sequence) / sizeof (load_to_nozzle_sequence[0]));
}

void MMU2::ReportError(ErrorCode ec, ErrorSource res) {
    // Due to a potential lossy error reporting layers linked to this hook
    // we'd better report everything to make sure especially the error states
    // do not get lost. 
    // - The good news here is the fact, that the MMU reports the errors repeatedly until resolved.
    // - The bad news is, that MMU not responding may repeatedly occur on printers not having the MMU at all.
    // 
    // Not sure how to properly handle this situation, options:
    // - skip reporting "MMU not responding" (at least for now)
    // - report only changes of states (we can miss an error message)
    // - may be some combination of MMUAvailable + UseMMU flags and decide based on their state
    // Right now the filtering of MMU_NOT_RESPONDING is done in ReportErrorHook() as it is not a problem if mmu2.cpp

    // Depending on the Progress code, we may want to do some action when an error occurs
    switch (logic.Progress()){
    case ProgressCode::UnloadingToFinda:
        unloadFilamentStarted = false;
        break;
    case ProgressCode::FeedingToFSensor:
        // FSENSOR error during load. Make sure E-motor stops moving.
        loadFilamentStarted = false;
        break;
    default:
        break;
    }

    if( ec != lastErrorCode ){ // deduplicate: only report changes in error codes into the log
        lastErrorCode = ec;
        lastErrorSource = res;
        LogErrorEvent_P( _O(PrusaErrorTitle(PrusaErrorCodeIndex((uint16_t)ec))) );

        if( ec != ErrorCode::OK ){
            IncrementMMUFails();

            // check if it is a "power" failure - we consider TMC-related errors as power failures
            static constexpr uint16_t tmcMask =
                ( (uint16_t)ErrorCode::TMC_IOIN_MISMATCH
                | (uint16_t)ErrorCode::TMC_RESET
                | (uint16_t)ErrorCode::TMC_UNDERVOLTAGE_ON_CHARGE_PUMP
                | (uint16_t)ErrorCode::TMC_SHORT_TO_GROUND
                | (uint16_t)ErrorCode::TMC_OVER_TEMPERATURE_WARN
                | (uint16_t)ErrorCode::TMC_OVER_TEMPERATURE_ERROR
                | (uint16_t)ErrorCode::MMU_SOLDERING_NEEDS_ATTENTION ) & 0x7fffU; // skip the top bit
            static_assert(tmcMask == 0x7e00); // just make sure we fail compilation if any of the TMC error codes change

            if ((uint16_t)ec & tmcMask) { // @@TODO can be optimized to uint8_t operation
                // TMC-related errors are from 0x8200 higher
                IncrementTMCFailures();
            }
        }
    }

    if( !mmu2.RetryIfPossible((uint16_t)ec) ) {
        // If retry attempts are all used up
        // or if 'Retry' operation is not available
        // raise the MMU error sceen and wait for user input
        ReportErrorHook((uint16_t)ec);
    }

    static_assert(mmu2Magic[0] == 'M' 
        && mmu2Magic[1] == 'M' 
        && mmu2Magic[2] == 'U' 
        && mmu2Magic[3] == '2' 
        && mmu2Magic[4] == ':' 
        && strlen_constexpr(mmu2Magic) == 5, 
        "MMU2 logging prefix mismatch, must be updated at various spots"
    );
}

void MMU2::ReportProgress(ProgressCode pc) {
    ReportProgressHook((CommandInProgress)logic.CommandInProgress(), (uint16_t)pc);
    LogEchoEvent_P( _O(ProgressCodeToText((uint16_t)pc)) );
}

void MMU2::OnMMUProgressMsg(ProgressCode pc){
    if (pc != lastProgressCode) {
        OnMMUProgressMsgChanged(pc);
    } else {
        OnMMUProgressMsgSame(pc);
    }
}

void MMU2::OnMMUProgressMsgChanged(ProgressCode pc){
    ReportProgress(pc);
    lastProgressCode = pc;
    switch (pc) {
    case ProgressCode::UnloadingToFinda:
        if ((CommandInProgress)logic.CommandInProgress() == CommandInProgress::UnloadFilament
        || ((CommandInProgress)logic.CommandInProgress() == CommandInProgress::ToolChange))
        {
            // If MK3S sent U0 command, ramming sequence takes care of releasing the filament.
            // If Toolchange is done while printing, PrusaSlicer takes care of releasing the filament
            // If printing is not in progress, ToolChange will issue a U0 command.
            break;
        } else {
            // We're likely recovering from an MMU error
            st_synchronize();
            unloadFilamentStarted = true;
            HelpUnloadToFinda();
        }
        break;
    case ProgressCode::FeedingToFSensor:
        // prepare for the movement of the E-motor
        st_synchronize();
        loadFilamentStarted = true;
        break;
    default:
        // do nothing yet
        break;
    }
}

void __attribute__((noinline)) MMU2::HelpUnloadToFinda(){
    current_position[E_AXIS] -= MMU2_RETRY_UNLOAD_TO_FINDA_LENGTH;
    plan_buffer_line_curposXYZE(MMU2_RETRY_UNLOAD_TO_FINDA_FEED_RATE);
}

void MMU2::OnMMUProgressMsgSame(ProgressCode pc){
    switch (pc) {
    case ProgressCode::UnloadingToFinda:
        if (unloadFilamentStarted && !blocks_queued()) { // Only plan a move if there is no move ongoing
            if (fsensor.getFilamentPresent()) {
                HelpUnloadToFinda();
            } else {
                unloadFilamentStarted = false;
            }
        }
        break;
    case ProgressCode::FeedingToFSensor:
        if (loadFilamentStarted) {
            switch (WhereIsFilament()) {
            case FilamentState::AT_FSENSOR:
                // fsensor triggered, finish FeedingToExtruder state
                loadFilamentStarted = false;
                // After the MMU knows the FSENSOR is triggered it will:
                // 1. Push the filament by additional 30mm (see fsensorToNozzle)
                // 2. Disengage the idler and push another 2mm.
                current_position[E_AXIS] += logic.ExtraLoadDistance() + 2;
                plan_buffer_line_curposXYZE(MMU2_LOAD_TO_NOZZLE_FEED_RATE);
                break;
            case FilamentState::NOT_PRESENT:
                // fsensor not triggered, continue moving extruder
                if (!blocks_queued()) { // Only plan a move if there is no move ongoing
                    current_position[E_AXIS] += 2.0f;
                    plan_buffer_line_curposXYZE(MMU2_LOAD_TO_NOZZLE_FEED_RATE);
                }
                break;
            default:
                // Abort here?
                break;
            }
        }
        break;
    default:
        // do nothing yet
        break;
    }
}

} // namespace MMU2
