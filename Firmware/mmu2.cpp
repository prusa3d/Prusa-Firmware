#include "mmu2.h"
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

// Settings for filament load / unload from the LCD menu.
// This is for Prusa MK3-style extruders. Customize for your hardware.
#define MMU2_FILAMENTCHANGE_EJECT_FEED 80.0

#define NOZZLE_PARK_XY_FEEDRATE 50
#define NOZZLE_PARK_Z_FEEDRATE 15

// Nominal distance from the extruder gear to the nozzle tip is 87mm
// However, some slipping may occur and we need separate distances for
// LoadToNozzle and ToolChange.
// - +5mm seemed good for LoadToNozzle,
// - but too much (made blobs) for a ToolChange
static constexpr float MMU2_LOAD_TO_NOZZLE_LENGTH = 87.0F + 5.0F;

// As discussed with our PrusaSlicer profile specialist
// - ToolChange shall not try to push filament into the very tip of the nozzle
// to have some space for additional G-code to tune the extruded filament length
// in the profile
static constexpr float MMU2_TOOL_CHANGE_LOAD_LENGTH = 30.0F;

static constexpr float MMU2_LOAD_TO_NOZZLE_FEED_RATE = 20.0F;
static constexpr uint8_t MMU2_NO_TOOL = 99;
static constexpr uint32_t MMU_BAUD = 115200;

struct E_Step {
    float extrude;       ///< extrude distance in mm
    float feedRate; ///< feed rate in mm/s
};

static constexpr E_Step ramming_sequence[] PROGMEM = {
    { 1.0F,  1000.0F / 60.F},
    { 1.0F,  1500.0F / 60.F},
    { 2.0F,  2000.0F / 60.F},
    { 1.5F,  3000.0F / 60.F},
    { 2.5F,  4000.0F / 60.F},
    {-15.0F, 5000.0F / 60.F},
    {-14.0F, 1200.0F / 60.F},
    {-6.0F,   600.0F / 60.F},
    { 10.0F,  700.0F / 60.F},
    {-10.0F,  400.0F / 60.F},
    {-50.0F, 2000.0F / 60.F},
};

static constexpr E_Step load_to_nozzle_sequence[] PROGMEM = { 
    { 10.0F,  810.0F / 60.F}, // feed rate = 13.5mm/s - Load fast until filament reach end of nozzle
    { 25.0F,  198.0F / 60.F}, // feed rate = 3.3mm/s  - Load slower once filament is out of the nozzle
};

namespace MMU2 {

void execute_extruder_sequence(const E_Step *sequence, int steps);

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
    , logic(&mmu2Serial)
    , extruder(MMU2_NO_TOOL)
    , previous_extruder(MMU2_NO_TOOL)
    , tool_change_extruder(MMU2_NO_TOOL)
    , resume_position()
    , resume_hotend_temp(0)
    , logicStepLastStatus(StepStatus::Finished)
    , state(xState::Stopped)
    , mmu_print_saved(SavedState::None)
    , loadFilamentStarted(false)
    , loadingToNozzle(false)
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

void MMU2::mmu_loop() {
    // We only leave this method if the current command was successfully completed - that's the Marlin's way of blocking operation
    // Atomic compare_exchange would have been the most appropriate solution here, but this gets called only in Marlin's task,
    // so thread safety should be kept
    static bool avoidRecursion = false;
    if (avoidRecursion)
        return;
    avoidRecursion = true;

    logicStepLastStatus = LogicStep(); // it looks like the mmu_loop doesn't need to be a blocking call

    if (is_mmu_error_monitor_active){
        // Call this every iteration to keep the knob rotation responsive
        // This includes when mmu_loop is called within manage_response
        ReportErrorHook((uint16_t)lastErrorCode, mmu2.MMUCurrentErrorCode() == ErrorCode::OK ? ErrorSourcePrinter : ErrorSourceMMU);
    }

    avoidRecursion = false;
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

bool MMU2::tool_change(uint8_t index) {
    if( ! WaitForMMUReady())
        return false;

    if (index != extruder) {
        ReportingRAII rep(CommandInProgress::ToolChange);
        FSensorBlockRunout blockRunout;

        st_synchronize();

        tool_change_extruder = index;
        logic.ToolChange(index); // let the MMU pull the filament out and push a new one in
        manage_response(true, true);
        
        // reset current position to whatever the planner thinks it is
//        SERIAL_ECHOPGM("TC1:p=");
//        SERIAL_ECHO(position[E_AXIS]);
//        SERIAL_ECHOPGM("TC1:cp=");
//        SERIAL_ECHOLN(current_position[E_AXIS]);
        plan_set_e_position(current_position[E_AXIS]);
//        SERIAL_ECHOPGM("TC2:p=");
//        SERIAL_ECHO(position[E_AXIS]);
//        SERIAL_ECHOPGM("TC2:cp=");
//        SERIAL_ECHOLN(current_position[E_AXIS]);

        extruder = index; //filament change is finished
        previous_extruder = extruder;
        SetActiveExtruder(0);

        // @@TODO really report onto the serial? May be for the Octoprint? Not important now
        //        SERIAL_ECHO_START();
        //        SERIAL_ECHOLNPAIR(MSG_ACTIVE_EXTRUDER, int(extruder));
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
        tool_change_extruder = slot;
        logic.ToolChange(slot);
        manage_response(false, false);
        extruder = slot;
        previous_extruder = extruder;
        SetActiveExtruder(0);
        set_extrude_min_temp(EXTRUDE_MINTEMP);
    } break;

    case 'c': {
        waitForHotendTargetTemp(100, []{});
        execute_extruder_sequence((const E_Step *)load_to_nozzle_sequence, sizeof(load_to_nozzle_sequence) / sizeof (load_to_nozzle_sequence[0]));
    } break;
    }

    return true;
}

uint8_t MMU2::get_current_tool() const {
    return extruder == MMU2_NO_TOOL ? (uint8_t)FILAMENT_UNKNOWN : extruder;
}

uint8_t MMU2::get_tool_change_tool() const {
    return tool_change_extruder == MMU2_NO_TOOL ? (uint8_t)FILAMENT_UNKNOWN : tool_change_extruder;
}

bool MMU2::set_filament_type(uint8_t index, uint8_t type) {
    if( ! WaitForMMUReady())
        return false;
    
    // @@TODO - this is not supported in the new MMU yet
    // cmd_arg = filamentType;
    // command(MMU_CMD_F0 + index);

    manage_response(false, false); // true, true); -- Comment: how is it possible for a filament type set to fail?
    
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

        logic.UnloadFilament();
        manage_response(false, true);
        Sound_MakeSound(e_SOUND_TYPE_StandardConfirm);

        // no active tool
        extruder = MMU2_NO_TOOL;
        tool_change_extruder = MMU2_NO_TOOL;
    }
    return true;
}

bool MMU2::cut_filament(uint8_t index){
    if( ! WaitForMMUReady())
        return false;

    ReportingRAII rep(CommandInProgress::CutFilament);
    logic.CutFilament(index);
    manage_response(false, true);
    
    return true;
}

void FullScreenMsg(const char *pgmS, uint8_t slot){
    lcd_update_enable(false);
    lcd_clear();
    lcd_puts_at_P(0, 1, pgmS);
    lcd_print(' ');
    lcd_print(slot + 1);
}

bool MMU2::load_to_bondtech(uint8_t index){
    FullScreenMsg(_T(MSG_TESTING_FILAMENT), index);
    tool_change(index);
    st_synchronize();
    unload();
    lcd_update_enable(true);
    return true;
}

bool MMU2::load_filament(uint8_t index) {
    if( ! WaitForMMUReady())
        return false;

    FullScreenMsg(_T(MSG_LOADING_FILAMENT), index);

    ReportingRAII rep(CommandInProgress::LoadFilament);
    logic.LoadFilament(index);
    manage_response(false, false);
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

bool MMU2::load_filament_to_nozzle(uint8_t index) {
    if( ! WaitForMMUReady())
        return false;

    LoadingToNozzleRAII ln(*this);

    WaitForHotendTargetTempBeep();

    FullScreenMsg(_T(MSG_LOADING_FILAMENT), index);
    {
        // used for MMU-menu operation "Load to Nozzle"
        ReportingRAII rep(CommandInProgress::ToolChange);
        FSensorBlockRunout blockRunout;

        if( extruder != MMU2_NO_TOOL ){ // we already have some filament loaded - free it + shape its tip properly
            filament_ramming();
        }

        tool_change_extruder = index;
        logic.ToolChange(index);
        manage_response(true, true);

        // The MMU's idler is disengaged at this point
        // That means the MK3/S now has fully control

        // reset current position to whatever the planner thinks it is
        st_synchronize();
//        SERIAL_ECHOPGM("LFTN1:p=");
//        SERIAL_ECHO(position[E_AXIS]);
//        SERIAL_ECHOPGM("LFTN1:cp=");
//        SERIAL_ECHOLN(current_position[E_AXIS]);
        plan_set_e_position(current_position[E_AXIS]);
//        SERIAL_ECHOPGM("LFTN2:p=");
//        SERIAL_ECHO(position[E_AXIS]);
//        SERIAL_ECHOPGM("LFTN2:cp=");
//        SERIAL_ECHOLN(current_position[E_AXIS]);

        // Finish loading to the nozzle with finely tuned steps.
        execute_extruder_sequence((const E_Step *)load_to_nozzle_sequence, sizeof(load_to_nozzle_sequence) / sizeof (load_to_nozzle_sequence[0]));

        extruder = index;
        previous_extruder = extruder;
        SetActiveExtruder(0);

        Sound_MakeSound(e_SOUND_TYPE_StandardConfirm);
    }
    lcd_update_enable(true);
    return true;
}

bool MMU2::eject_filament(uint8_t index, bool recover) {
    if( ! WaitForMMUReady())
        return false;

    ReportingRAII rep(CommandInProgress::EjectFilament);
    current_position[E_AXIS] -= MMU2_FILAMENTCHANGE_EJECT_FEED;
    plan_buffer_line_curposXYZE(2500.F / 60.F);
    st_synchronize();
    logic.EjectFilament(index);
    manage_response(false, false);

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
        manage_response(false, false);
    }

    // no active tool
    extruder = MMU2_NO_TOOL;
    tool_change_extruder = MMU2_NO_TOOL;
    Sound_MakeSound(e_SOUND_TYPE_StandardConfirm);
//    disable_E0();

    return true;
}

void MMU2::Button(uint8_t index){
    logic.Button(index);
}

void MMU2::Home(uint8_t mode){
    logic.Home(mode);
}

void MMU2::SaveAndPark(bool move_axes, bool turn_off_nozzle) {
    if (mmu_print_saved == SavedState::None) { // First occurrence. Save current position, park print head, disable nozzle heater.
        LogEchoEvent("Saving and parking");
        st_synchronize();
      
        resume_hotend_temp = degTargetHotend(active_extruder);

        if (move_axes){
            mmu_print_saved |= SavedState::ParkExtruder;
            // save current pos
            for(uint8_t i = 0; i < 3; ++i){
                resume_position.xyz[i] = current_position[i];
            }

            // lift Z
            current_position[Z_AXIS] += Z_PAUSE_LIFT;
            if (current_position[Z_AXIS] > Z_MAX_POS) 
                current_position[Z_AXIS] = Z_MAX_POS;
            plan_buffer_line_curposXYZE(NOZZLE_PARK_Z_FEEDRATE);
            st_synchronize();

            // move XY aside
            current_position[X_AXIS] = X_PAUSE_POS;
            current_position[Y_AXIS] = Y_PAUSE_POS;
            plan_buffer_line_curposXYZE(NOZZLE_PARK_XY_FEEDRATE);
            st_synchronize();
        }

        if (turn_off_nozzle){
            mmu_print_saved |= SavedState::CooldownPending;
            LogEchoEvent("Heater cooldown pending");
            // This just sets the flag that we should timeout and shut off the nozzle in 30 minutes...
            //setAllTargetHotends(0);
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
        LogEchoEvent("Cooldown flag cleared");
    }
    if ((mmu_print_saved & SavedState::Cooldown) && resume_hotend_temp) {
        LogEchoEvent("Resuming Temp");
        MMU2_ECHO_MSG("Restoring hotend temperature ");
        SERIAL_ECHOLN(resume_hotend_temp);
        mmu_print_saved &= ~(SavedState::Cooldown);
        setTargetHotend(resume_hotend_temp, active_extruder);
        lcd_display_message_fullscreen_P(_i("MMU Retry: Restoring temperature...")); // better report the event and let the GUI do its work somewhere else
        ReportErrorHookSensorLineRender();
        waitForHotendTargetTemp(1000, []{
            ReportErrorHookDynamicRender();
            manage_inactivity(true);
        });
        lcd_update_enable(true); // temporary hack to stop this locking the printer...
        LogEchoEvent("Hotend temperature reached");
        lcd_clear();
    }
}

void MMU2::ResumeUnpark()
{
    if (mmu_print_saved & SavedState::ParkExtruder) {
        LogEchoEvent("Resuming XYZ");

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
    if (btn == Buttons::NoButton && lastButton != Buttons::NoButton)
    {
        btn = lastButton;
        lastButton = Buttons::NoButton; // Clear it. 
    }
    switch (btn) {
    case Left:
    case Middle:
    case Right:
        ResumeHotendTemp(); // Recover the hotend temp before we attempt to do anything else...
        Button(btn);
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
void MMU2::manage_response(const bool move_axes, const bool turn_off_nozzle) {
    mmu_print_saved = SavedState::None;

    KEEPALIVE_STATE(PAUSED_FOR_USER);

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

        if (mmu_print_saved & SavedState::CooldownPending)
        {
            if (!nozzleTimeout.running())
            {
                nozzleTimeout.start();
                LogEchoEvent(" Cooling Timeout started");
            } 
            else if (nozzleTimeout.expired(DEFAULT_SAFETYTIMER_TIME_MINS*60*1000ul)) // mins->msec. TODO: do we use the global or have our own independent timeout
            {
                mmu_print_saved &= ~(SavedState::CooldownPending);
                mmu_print_saved |= SavedState::Cooldown;
                setAllTargetHotends(0);
                LogEchoEvent("Heater cooldown");
            }
        }
        else if (nozzleTimeout.running())
        {
            nozzleTimeout.stop();
            LogEchoEvent("Cooling timer stopped");
        }

        switch (logicStepLastStatus) {
        case Finished: 
            // command/operation completed, let Marlin continue its work
            // the E may have some more moves to finish - wait for them
            ResumeUnpark(); // We can now travel back to the tower or wherever we were when we saved.
            st_synchronize(); 
            return;
        case VersionMismatch: // this basically means the MMU will be disabled until reconnected
            CheckUserInput();
            return;
        case CommunicationTimeout:
        case CommandError:
        case ProtocolError:
            SaveAndPark(move_axes, turn_off_nozzle); // and wait for the user to resolve the problem
            CheckUserInput();
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

StepStatus MMU2::LogicStep() {
    StepStatus ss = logic.Step();
    switch (ss) {
    case Finished:
    case Processing:
        OnMMUProgressMsg(logic.Progress());
        break;
    case CommandError:
        ReportError(logic.Error(), ErrorSourceMMU);
        CheckUserInput();
        break;
    case CommunicationTimeout:
        state = xState::Connecting;
        ReportError(ErrorCode::MMU_NOT_RESPONDING, ErrorSourcePrinter);
        CheckUserInput();
        break;
    case ProtocolError:
        state = xState::Connecting;
        ReportError(ErrorCode::PROTOCOL_ERROR, ErrorSourcePrinter);
        CheckUserInput();
        break;
    case VersionMismatch:
        StopKeepPowered();
        ReportError(ErrorCode::VERSION_MISMATCH, ErrorSourcePrinter);
        CheckUserInput();
        break;
    case ButtonPushed:
        lastButton = logic.Button();
        LogEchoEvent("MMU Button pushed");
        CheckUserInput();
        break;
    default:
        break;
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
//        SERIAL_ECHOPGM("EES:");
//        SERIAL_ECHOLN(position[E_AXIS]);
        step++;
    }
}

void MMU2::SetActiveExtruder(uint8_t ex){ 
    active_extruder = ex; 
}

void MMU2::ReportError(ErrorCode ec, uint8_t res) {
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
    ReportErrorHook((uint16_t)ec, res);

    if( ec != lastErrorCode ){ // deduplicate: only report changes in error codes into the log
        lastErrorCode = ec;
        SERIAL_ECHO_START;
        SERIAL_ECHOLNRPGM( PrusaErrorTitle(PrusaErrorCodeIndex((uint16_t)ec)) );
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

    SERIAL_ECHO_START;
    SERIAL_ECHOLNRPGM( ProgressCodeToText((uint16_t)pc) );
}

void MMU2::OnMMUProgressMsg(ProgressCode pc){
    if (pc != lastProgressCode) {
        ReportProgress(pc);
        lastProgressCode = pc;

        // Act accordingly - one-time handling
        switch (pc) {
        case ProgressCode::FeedingToBondtech:
            // prepare for the movement of the E-motor
            st_synchronize();
            loadFilamentStarted = true;
            break;
        default:
            // do nothing yet
            break;
        }
    } else {
        // Act accordingly - every status change (even the same state)
        switch (pc) {
        case ProgressCode::FeedingToBondtech:
        case ProgressCode::FeedingToFSensor:
            if (loadFilamentStarted) {
                switch (WhereIsFilament()) {
                case FilamentState::AT_FSENSOR:
                    // fsensor triggered, finish FeedingToBondtech state
                    loadFilamentStarted = false;
                    // After the MMU knows the FSENSOR is triggered it will:
                    // 1. Push the filament by additional 30mm (see fsensorToNozzle)
                    // 2. Disengage the idler and push another 5mm.
//                    SERIAL_ECHOPGM("ATF1=");
//                    SERIAL_ECHO(current_position[E_AXIS]);
                    current_position[E_AXIS] += 30.0f + 2.0f;
                    plan_buffer_line_curposXYZE(MMU2_LOAD_TO_NOZZLE_FEED_RATE);
//                    SERIAL_ECHOPGM("ATF2=");
//                    SERIAL_ECHOLN(current_position[E_AXIS]);
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
}

void MMU2::LogErrorEvent(const char *msg){
    MMU2_ERROR_MSG(msg);
    SERIAL_ECHOLN();
}

void MMU2::LogEchoEvent(const char *msg){
    MMU2_ECHO_MSG(msg);
    SERIAL_ECHOLN();
}

} // namespace MMU2
