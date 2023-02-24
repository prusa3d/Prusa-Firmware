#include "mmu2.h"
#include "mmu2_config.h"
#include "mmu2_error_converter.h"
#include "mmu2_fsensor.h"
#include "mmu2_log.h"
#include "mmu2_marlin.h"
#include "mmu2_marlin_macros.h"
#include "mmu2_power.h"
#include "mmu2_progress_converter.h"
#include "mmu2_reporting.h"

#include "strlen_cx.h"
#include "SpoolJoin.h"

#ifdef __AVR__
// As of FW 3.12 we only support building the FW with only one extruder, all the multi-extruder infrastructure will be removed.
// Saves at least 800B of code size
static_assert(EXTRUDERS == 1);

constexpr float MMM_TO_MMS(float MM_M) { return MM_M / 60.0f; }
#endif

namespace MMU2 {

template <typename F>
void waitForHotendTargetTemp(uint16_t delay, F f) {
    while (((thermal_degTargetHotend() - thermal_degHotend()) > 5)) {
        f();
        safe_delay_keep_alive(delay);
    }
}

void WaitForHotendTargetTempBeep() {
    waitForHotendTargetTemp(3000, []{ });
    MakeSound(Prompt);
}

MMU2 mmu2;

MMU2::MMU2()
    : is_mmu_error_monitor_active(false)
    , logic(&mmu2Serial, MMU2_TOOL_CHANGE_LOAD_LENGTH, MMU2_LOAD_TO_NOZZLE_FEED_RATE)
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
    , toolchange_counter(0)
    , tmcFailures(0) {
}

void MMU2::Start() {
    mmu2Serial.begin(MMU_BAUD);

    PowerOn();          // I repurposed this to serve as our EEPROM disable toggle.
    mmu2Serial.flush(); // make sure the UART buffer is clear before starting communication

    extruder = MMU2_NO_TOOL;
    state = xState::Connecting;

    // start the communication
    logic.Start();

    logic.ResetRetryAttempts();
}

void MMU2::Stop() {
    StopKeepPowered();
    PowerOff(); // This also disables the MMU in the EEPROM.
}

void MMU2::StopKeepPowered() {
    state = xState::Stopped;
    logic.Stop();
    mmu2Serial.close();
}

void MMU2::Reset(ResetForm level) {
    switch (level) {
    case Software:
        ResetX0();
        break;
    case ResetPin:
        TriggerResetPin();
        break;
    case CutThePower:
        PowerCycle();
        break;
    case EraseEEPROM:
        ResetX42();
        break;
    default:
        break;
    }
}

void MMU2::ResetX0() {
    logic.ResetMMU(); // Send soft reset
}

void MMU2::ResetX42(){
    logic.ResetMMU(42);
}

void MMU2::TriggerResetPin() {
    reset();
}

void MMU2::PowerCycle() {
    // cut the power to the MMU and after a while restore it
    // Sadly, MK3/S/+ cannot do this
    // NOTE: the below will toggle the EEPROM var. Should we
    // assert this function is never called in the MK3 FW? Do we even care?
    PowerOff();
    safe_delay_keep_alive(1000);
    PowerOn();
}

void MMU2::PowerOff() {
    power_off();
}

void MMU2::PowerOn() {
    power_on();
}

bool MMU2::ReadRegister(uint8_t address) {
    if (!WaitForMMUReady())
        return false;
    do {
        logic.ReadRegister(address); // we may signal the accepted/rejected status of the response as return value of this function
    } while (!manage_response(false, false));

    return true;
}

bool MMU2::WriteRegister(uint8_t address, uint16_t data) {
    if (!WaitForMMUReady())
        return false;

    // special case - intercept requests of extra loading distance and perform the change even on the printer's side
    if (address == 0x0b) {
        logic.PlanExtraLoadDistance(data);
    }

    do {
        logic.WriteRegister(address, data); // we may signal the accepted/rejected status of the response as return value of this function
    } while (!manage_response(false, false));

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
        ReportErrorHook((CommandInProgress)logic.CommandInProgress(), (uint16_t)lastErrorCode, uint8_t(lastErrorSource));
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
    explicit inline ReportingRAII(CommandInProgress cip)
        : cip(cip) {
        BeginReport(cip, (uint16_t)ProgressCode::EngagingIdler);
    }
    inline ~ReportingRAII() {
        EndReport(cip, (uint16_t)ProgressCode::OK);
    }
};

bool MMU2::WaitForMMUReady() {
    switch (State()) {
    case xState::Stopped:
        return false;
    case xState::Connecting:
        // shall we wait until the MMU reconnects?
        // fire-up a fsm_dlg and show "MMU not responding"?
    default:
        return true;
    }
}

bool MMU2::RetryIfPossible(uint16_t ec) {
    if (logic.RetryAttempts()) {
        SetButtonResponse(ButtonOperations::Retry);
        // check, that Retry is actually allowed on that operation
        if (ButtonAvailable(ec) != NoButton) {
            logic.SetInAutoRetry(true);
            SERIAL_ECHOLNPGM("RetryButtonPressed");
            // We don't decrement until the button is acknowledged by the MMU.
            //--retryAttempts; // "used" one retry attempt
            return true;
        }
    }
    logic.SetInAutoRetry(false);
    return false;
}

bool MMU2::VerifyFilamentEnteredPTFE() {
    planner_synchronize();

    if (WhereIsFilament() == FilamentState::NOT_PRESENT)
        return false;

    uint8_t fsensorState = 0;
    // MMU has finished its load, push the filament further by some defined constant length
    // If the filament sensor reads 0 at any moment, then report FAILURE
    MoveE(MMU2_EXTRUDER_PTFE_LENGTH + MMU2_EXTRUDER_HEATBREAK_LENGTH + MMU2_VERIFY_LOAD_TO_NOZZLE_TWEAK - (logic.ExtraLoadDistance() - MMU2_FILAMENT_SENSOR_POSITION), MMU2_VERIFY_LOAD_TO_NOZZLE_FEED_RATE);
    MoveE(-(MMU2_EXTRUDER_PTFE_LENGTH + MMU2_EXTRUDER_HEATBREAK_LENGTH + MMU2_VERIFY_LOAD_TO_NOZZLE_TWEAK - (logic.ExtraLoadDistance() - MMU2_FILAMENT_SENSOR_POSITION)), MMU2_VERIFY_LOAD_TO_NOZZLE_FEED_RATE);

    while (planner_any_moves()) {
        // Wait for move to finish and monitor the fsensor the entire time
        // A single 0 reading will set the bit.
        fsensorState |= (WhereIsFilament() == FilamentState::NOT_PRESENT);
        safe_delay_keep_alive(0);
    }

    if (fsensorState) {
        IncrementLoadFails();
        return false;
    } else {
        // else, happy printing! :)
        return true;
    }
}

bool MMU2::ToolChangeCommonOnce(uint8_t slot) {
    static_assert(MAX_RETRIES > 1); // need >1 retries to do the cut in the last attempt
    for (uint8_t retries = MAX_RETRIES; retries; --retries) {
        for (;;) {
            Disable_E0(); // it may seem counterintuitive to disable the E-motor, but it gets enabled in the planner whenever the E-motor is to move
            tool_change_extruder = slot;
            logic.ToolChange(slot); // let the MMU pull the filament out and push a new one in
            if (manage_response(true, true))
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
        planner_set_current_position_E(planner_get_current_position_E());
        if (VerifyFilamentEnteredPTFE()) {
            return true; // success
        } else {         // Prepare a retry attempt
            unload();
            if (retries == 2 && cutter_enabled()) {
                cut_filament(slot, false); // try cutting filament tip at the last attempt
            }
        }
    }
    return false; // couldn't accomplish the task
}

void MMU2::ToolChangeCommon(uint8_t slot) {
    while (!ToolChangeCommonOnce(slot)) { // while not successfully fed into extruder's PTFE tube
        // failed autoretry, report an error by forcing a "printer" error into the MMU infrastructure - it is a hack to leverage existing code
        // @@TODO theoretically logic layer may not need to be spoiled with the printer error - may be just the manage_response needs it...
        logic.SetPrinterError(ErrorCode::LOAD_TO_EXTRUDER_FAILED);
        // We only have to wait for the user to fix the issue and press "Retry".
        // Please see CheckUserInput() for details how we "leave" manage_response.
        // If manage_response returns false at this spot (MMU operation interrupted aka MMU reset)
        // we can safely continue because the MMU is not doing an operation now.
        static_cast<void>(manage_response(true, true)); // yes, I'd like to silence [[nodiscard]] warning at this spot by casting to void
    }

    extruder = slot; //filament change is finished
    SpoolJoin::spooljoin.setSlot(slot);

    ++toolchange_counter;
}

bool MMU2::tool_change(uint8_t slot) {
    if (!WaitForMMUReady())
        return false;

    if (slot != extruder) {
        if (/*FindaDetectsFilament()*/
            /*!IS_SD_PRINTING && !usb_timer.running()*/
            !marlin_printingIsActive()) {
            // If Tcodes are used manually through the serial
            // we need to unload manually as well -- but only if FINDA detects filament
            unload();
        }

        ReportingRAII rep(CommandInProgress::ToolChange);
        FSensorBlockRunout blockRunout;
        planner_synchronize();
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
    if (!WaitForMMUReady())
        return false;

    FSensorBlockRunout blockRunout;

    switch (code) {
    case '?': {
        waitForHotendTargetTemp(100, [] {});
        load_filament_to_nozzle(slot);
    } break;

    case 'x': {
        thermal_setExtrudeMintemp(0); // Allow cold extrusion since Tx only loads to the gears not nozzle
        planner_synchronize();
        ToolChangeCommon(slot); // the only difference was manage_response(false, false), but probably good enough
        thermal_setExtrudeMintemp(EXTRUDE_MINTEMP);
    } break;

    case 'c': {
        waitForHotendTargetTemp(100, [] {});
        execute_load_to_nozzle_sequence();
    } break;
    }

    return true;
}

void MMU2::get_statistics() {
    logic.Statistics();
}

uint8_t __attribute__((noinline)) MMU2::get_current_tool() const {
    return extruder == MMU2_NO_TOOL ? (uint8_t)FILAMENT_UNKNOWN : extruder;
}

uint8_t MMU2::get_tool_change_tool() const {
    return tool_change_extruder == MMU2_NO_TOOL ? (uint8_t)FILAMENT_UNKNOWN : tool_change_extruder;
}

bool MMU2::set_filament_type(uint8_t /*slot*/, uint8_t /*type*/) {
    if (!WaitForMMUReady())
        return false;
    
    // @@TODO - this is not supported in the new MMU yet
    //    slot = slot; // @@TODO
    //    type = type; // @@TODO
    // cmd_arg = filamentType;
    // command(MMU_CMD_F0 + index);

    if (!manage_response(false, false)) {
        // @@TODO failed to perform the command - retry
        ;
    } // true, true); -- Comment: how is it possible for a filament type set to fail?

    return true;
}

bool MMU2::unload() {
    if (!WaitForMMUReady())
        return false;

    WaitForHotendTargetTempBeep();

    {
        FSensorBlockRunout blockRunout;
        ReportingRAII rep(CommandInProgress::UnloadFilament);
        filament_ramming();

        // we assume the printer managed to relieve filament tip from the gears,
        // so repeating that part in case of an MMU restart is not necessary
        for (;;) {
            Disable_E0();
            logic.UnloadFilament();
            if (manage_response(false, true))
                break;
            IncrementMMUFails();
        }
        MakeSound(Confirm);

        // no active tool
        extruder = MMU2_NO_TOOL;
        tool_change_extruder = MMU2_NO_TOOL;
    }
    return true;
}

bool MMU2::cut_filament(uint8_t slot, bool enableFullScreenMsg /*= true*/) {
    if (!WaitForMMUReady())
        return false;

    if (enableFullScreenMsg) {
        FullScreenMsgCut(slot);
    }
    {
        if (FindaDetectsFilament()) {
            unload();
        }

        ReportingRAII rep(CommandInProgress::CutFilament);
        for (;;) {
            Disable_E0();
            logic.CutFilament(slot);
            if (manage_response(false, true))
                break;
            IncrementMMUFails();
        }
    }
    extruder = MMU2_NO_TOOL;
    tool_change_extruder = MMU2_NO_TOOL;
    MakeSound(SoundType::Confirm);
    return true;
}

bool MMU2::loading_test(uint8_t slot) {
    FullScreenMsgTest(slot);
    tool_change(slot);
    planner_synchronize();
    unload();
    ScreenUpdateEnable();
    return true;
}

bool MMU2::load_filament(uint8_t slot) {
    if (!WaitForMMUReady())
        return false;

    FullScreenMsgLoad(slot);

    ReportingRAII rep(CommandInProgress::LoadFilament);
    for (;;) {
        Disable_E0();
        logic.LoadFilament(slot);
        if (manage_response(false, false))
            break;
        IncrementMMUFails();
    }

    MakeSound(SoundType::Confirm);

    ScreenUpdateEnable();

    return true;
}

struct LoadingToNozzleRAII {
    MMU2 &mmu2;
    explicit inline LoadingToNozzleRAII(MMU2 &mmu2)
        : mmu2(mmu2) {
        mmu2.loadingToNozzle = true;
    }
    inline ~LoadingToNozzleRAII() {
        mmu2.loadingToNozzle = false;
    }
};

bool MMU2::load_filament_to_nozzle(uint8_t slot) {
    if (!WaitForMMUReady())
        return false;

    LoadingToNozzleRAII ln(*this);

    WaitForHotendTargetTempBeep();

    FullScreenMsgLoad(slot);
    {
        // used for MMU-menu operation "Load to Nozzle"
        ReportingRAII rep(CommandInProgress::ToolChange);
        FSensorBlockRunout blockRunout;

        if (extruder != MMU2_NO_TOOL) { // we already have some filament loaded - free it + shape its tip properly
            filament_ramming();
        }

        ToolChangeCommon(slot);

        // Finish loading to the nozzle with finely tuned steps.
        execute_load_to_nozzle_sequence();
        MakeSound(Confirm);
    }
    ScreenUpdateEnable();
    return true;
}

bool MMU2::eject_filament(uint8_t slot, bool enableFullScreenMsg /* = true */) {
    if (!WaitForMMUReady())
        return false;

    if (enableFullScreenMsg) {
        FullScreenMsgEject(slot);
    }
    {
        if (FindaDetectsFilament()) {
            unload();
        }

        ReportingRAII rep(CommandInProgress::EjectFilament);
        for (;;) {
            Disable_E0();
            logic.EjectFilament(slot);
            if (manage_response(false, true))
                break;
            IncrementMMUFails();
        }
    }
    extruder = MMU2_NO_TOOL;
    tool_change_extruder = MMU2_NO_TOOL;
    MakeSound(Confirm);
    return true;
}

void MMU2::Button(uint8_t index) {
    LogEchoEvent_P(PSTR("Button"));
    logic.Button(index);
}

void MMU2::Home(uint8_t mode) {
    logic.Home(mode);
}

void MMU2::SaveHotendTemp(bool turn_off_nozzle) {
    if (mmu_print_saved & SavedState::Cooldown)
        return;

    if (turn_off_nozzle && !(mmu_print_saved & SavedState::CooldownPending)) {
        Disable_E0();
        resume_hotend_temp = thermal_degTargetHotend();
        mmu_print_saved |= SavedState::CooldownPending;
        LogEchoEvent_P(PSTR("Heater cooldown pending"));
    }
}

void MMU2::SaveAndPark(bool move_axes) {
    if (mmu_print_saved == SavedState::None) { // First occurrence. Save current position, park print head, disable nozzle heater.
        LogEchoEvent_P(PSTR("Saving and parking"));
        Disable_E0();
        planner_synchronize();

        if (move_axes) {
            mmu_print_saved |= SavedState::ParkExtruder;
            resume_position = planner_current_position(); // save current pos

            // lift Z
            MoveRaiseZ(MMU_ERR_Z_PAUSE_LIFT);

            // move XY aside
            if (all_axes_homed()) {
                nozzle_park();
            }
        }
    }
    // keep the motors powered forever (until some other strategy is chosen)
    // @@TODO do we need that in 8bit?
    gcode_reset_stepper_timeout();
}

void MMU2::ResumeHotendTemp() {
    if ((mmu_print_saved & SavedState::CooldownPending)) {
        // Clear the "pending" flag if we haven't cooled yet.
        mmu_print_saved &= ~(SavedState::CooldownPending);
        LogEchoEvent_P(PSTR("Cooldown flag cleared"));
    }
    if ((mmu_print_saved & SavedState::Cooldown) && resume_hotend_temp) {
        LogEchoEvent_P(PSTR("Resuming Temp"));
        // @@TODO MMU2_ECHO_MSGRPGM(PSTR("Restoring hotend temperature "));
        SERIAL_ECHOLN(resume_hotend_temp);
        mmu_print_saved &= ~(SavedState::Cooldown);
        thermal_setTargetHotend(resume_hotend_temp);
        FullScreenMsgRestoringTemperature();
        //@todo better report the event and let the GUI do its work somewhere else
        ReportErrorHookSensorLineRender();
        waitForHotendTargetTemp(100, [] {
            marlin_manage_inactivity(true);
            mmu2.mmu_loop_inner(false);
            ReportErrorHookDynamicRender();
        });
        ScreenUpdateEnable(); // temporary hack to stop this locking the printer...
        LogEchoEvent_P(PSTR("Hotend temperature reached"));
        ScreenClear();
    }
}

void MMU2::ResumeUnpark() {
    if (mmu_print_saved & SavedState::ParkExtruder) {
        LogEchoEvent_P(PSTR("Resuming XYZ"));

        // Move XY to starting position, then Z
        motion_do_blocking_move_to_xy(resume_position.xyz[0], resume_position.xyz[1], feedRate_t(NOZZLE_PARK_XY_FEEDRATE));

        // Move Z_AXIS to saved position
        motion_do_blocking_move_to_z(resume_position.xyz[2], feedRate_t(NOZZLE_PARK_Z_FEEDRATE));

        mmu_print_saved &= ~(SavedState::ParkExtruder);
    }
}

void MMU2::CheckUserInput() {
    auto btn = ButtonPressed((uint16_t)lastErrorCode);

    // Was a button pressed on the MMU itself instead of the LCD?
    if (btn == Buttons::NoButton && lastButton != Buttons::NoButton) {
        btn = lastButton;
        lastButton = Buttons::NoButton; // Clear it.
    }

    switch (btn) {
    case Left:
    case Middle:
    case Right:
        SERIAL_ECHOPGM("CheckUserInput-btnLMR ");
        SERIAL_ECHOLN(btn);

        // clear the explicit printer error as soon as possible so that the MMU error screens + reporting doesn't get too confused
        if (lastErrorCode == ErrorCode::LOAD_TO_EXTRUDER_FAILED) {
            // A horrible hack - clear the explicit printer error allowing manage_response to recover on MMU's Finished state
            // Moreover - if the MMU is currently doing something (like the LoadFilament - see comment above)
            // we'll actually wait for it automagically in manage_response and after it finishes correctly,
            // we'll issue another command (like toolchange)
            logic.ClearPrinterError();
            lastErrorCode = ErrorCode::OK;
            lastErrorSource = ErrorSourceNone; // this seems to help clearing the error screen
        }

        ResumeHotendTemp(); // Recover the hotend temp before we attempt to do anything else...

        if (mmu2.MMULastErrorSource() == MMU2::ErrorSourceMMU) {
            // Do not send a button to the MMU unless the MMU is in error state
            Button(btn);
        }

        // A quick hack: for specific error codes move the E-motor every time.
        // Not sure if we can rely on the fsensor.
        // Just plan the move, let the MMU take over when it is ready
        switch (lastErrorCode) {
        case ErrorCode::FSENSOR_DIDNT_SWITCH_OFF:
        case ErrorCode::FSENSOR_TOO_EARLY:
            HelpUnloadToFinda();
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

    MARLIN_KEEPALIVE_STATE_IN_PROCESS;

    LongTimer nozzleTimeout;

    for (;;) {
        // in our new implementation, we know the exact state of the MMU at any moment, we do not have to wait for a timeout
        // So in this case we shall decide if the operation is:
        // - still running -> wait normally in idle()
        // - failed -> then do the safety moves on the printer like before
        // - finished ok -> proceed with reading other commands
        safe_delay_keep_alive(0); // calls LogicStep() and remembers its return status

        if (mmu_print_saved & SavedState::CooldownPending) {
            if (!nozzleTimeout.running()) {
                nozzleTimeout.start();
                LogEchoEvent_P(PSTR("Cooling Timeout started"));
            } else if (nozzleTimeout.expired(DEFAULT_SAFETYTIMER_TIME_MINS * 60 * 1000ul)) { // mins->msec.
                mmu_print_saved &= ~(SavedState::CooldownPending);
                mmu_print_saved |= SavedState::Cooldown;
                thermal_setTargetHotend(0);
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
            ResumeUnpark();             // We can now travel back to the tower or wherever we were when we saved.
            logic.ResetRetryAttempts(); // Reset the retry counter.
            planner_synchronize();
            return true;
        case Interrupted:
            // now what :D ... big bad ... ramming, unload, retry the whole command originally issued
            return false;
        case VersionMismatch: // this basically means the MMU will be disabled until reconnected
            CheckUserInput();
            return true;
        case PrinterError:
            SaveAndPark(move_axes);
            SaveHotendTemp(turn_off_nozzle);
            CheckUserInput();
            // if button pressed "Done", return true, otherwise stay within manage_response
            // Please see CheckUserInput() for details how we "leave" manage_response
            break;
        case CommandError:
        case CommunicationTimeout:
        case ProtocolError:
        case ButtonPushed:
            if (!logic.InAutoRetry()) {
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
        if (reportErrors) {
            switch (ss) {
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

    if (logic.Running()) {
        state = xState::Active;
    }
    return ss;
}

void MMU2::filament_ramming() {
    execute_extruder_sequence(ramming_sequence, sizeof(ramming_sequence) / sizeof(E_Step));
}

void MMU2::execute_extruder_sequence(const E_Step *sequence, uint8_t steps) {
    planner_synchronize();
    Enable_E0();

    const E_Step *step = sequence;
    for (uint8_t i = steps; i ; --i) {
        MoveE(pgm_read_float(&(step->extrude)), pgm_read_float(&(step->feedRate)));
        step++;
    }
    planner_synchronize(); // it looks like it's better to sync the moves at the end - smoother move (if the sequence is not too long).

    Disable_E0();
}

void MMU2::execute_load_to_nozzle_sequence() {
    planner_synchronize();
    // Compensate for configurable Extra Loading Distance
    planner_set_current_position_E(planner_get_current_position_E() - (logic.ExtraLoadDistance() - MMU2_FILAMENT_SENSOR_POSITION));
    execute_extruder_sequence(load_to_nozzle_sequence, sizeof(load_to_nozzle_sequence) / sizeof(load_to_nozzle_sequence[0]));
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
    switch (logic.Progress()) {
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

    if (ec != lastErrorCode) { // deduplicate: only report changes in error codes into the log
        lastErrorCode = ec;
        lastErrorSource = res;
        LogErrorEvent_P(_O(PrusaErrorTitle(PrusaErrorCodeIndex((uint16_t)ec))));

        if (ec != ErrorCode::OK) {
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

    if (!mmu2.RetryIfPossible((uint16_t)ec)) {
        // If retry attempts are all used up
        // or if 'Retry' operation is not available
        // raise the MMU error sceen and wait for user input
        ReportErrorHook((CommandInProgress)logic.CommandInProgress(), (uint16_t)ec, uint8_t(lastErrorSource));
    }

    static_assert(mmu2Magic[0] == 'M'
            && mmu2Magic[1] == 'M'
            && mmu2Magic[2] == 'U'
            && mmu2Magic[3] == '2'
            && mmu2Magic[4] == ':'
            && strlen_constexpr(mmu2Magic) == 5,
        "MMU2 logging prefix mismatch, must be updated at various spots");
}

void MMU2::ReportProgress(ProgressCode pc) {
    ReportProgressHook((CommandInProgress)logic.CommandInProgress(), (uint16_t)pc);
    LogEchoEvent_P(_O(ProgressCodeToText((uint16_t)pc)));
}

void MMU2::OnMMUProgressMsg(ProgressCode pc) {
    if (pc != lastProgressCode) {
        OnMMUProgressMsgChanged(pc);
    } else {
        OnMMUProgressMsgSame(pc);
    }
}

void MMU2::OnMMUProgressMsgChanged(ProgressCode pc) {
    ReportProgress(pc);
    lastProgressCode = pc;
    switch (pc) {
    case ProgressCode::UnloadingToFinda:
        if ((CommandInProgress)logic.CommandInProgress() == CommandInProgress::UnloadFilament
        || ((CommandInProgress)logic.CommandInProgress() == CommandInProgress::ToolChange)) {
            // If MK3S sent U0 command, ramming sequence takes care of releasing the filament.
            // If Toolchange is done while printing, PrusaSlicer takes care of releasing the filament
            // If printing is not in progress, ToolChange will issue a U0 command.
            break;
        } else {
            // We're likely recovering from an MMU error
            planner_synchronize();
            unloadFilamentStarted = true;
            HelpUnloadToFinda();
        }
        break;
    case ProgressCode::FeedingToFSensor:
        // prepare for the movement of the E-motor
        planner_synchronize();
        loadFilamentStarted = true;
        break;
    default:
        // do nothing yet
        break;
    }
}

void __attribute__((noinline)) MMU2::HelpUnloadToFinda() {
    MoveE(-MMU2_RETRY_UNLOAD_TO_FINDA_LENGTH, MMU2_RETRY_UNLOAD_TO_FINDA_FEED_RATE);
}

void MMU2::OnMMUProgressMsgSame(ProgressCode pc) {
    switch (pc) {
    case ProgressCode::UnloadingToFinda:
        if (unloadFilamentStarted && !planner_any_moves()) { // Only plan a move if there is no move ongoing
            switch (WhereIsFilament()) {
            case FilamentState::AT_FSENSOR:
            case FilamentState::IN_NOZZLE:
            case FilamentState::UNAVAILABLE: // actually Unavailable makes sense as well to start the E-move to release the filament from the gears
                HelpUnloadToFinda();
                break;
            default:
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
                MoveE(logic.ExtraLoadDistance() + 2, MMU2_LOAD_TO_NOZZLE_FEED_RATE);
                break;
            case FilamentState::NOT_PRESENT:
                // fsensor not triggered, continue moving extruder
                if (!planner_any_moves()) { // Only plan a move if there is no move ongoing
                    MoveE(2.0f, MMU2_LOAD_TO_NOZZLE_FEED_RATE);
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
