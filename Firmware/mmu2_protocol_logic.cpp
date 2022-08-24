#include "mmu2_protocol_logic.h"
#include "mmu2_log.h"
#include "mmu2_fsensor.h"
#include "system_timer.h"
#include <string.h>

namespace MMU2 {

static constexpr uint8_t supportedMmuFWVersionMajor = 2;
static constexpr uint8_t supportedMmuFWVersionMinor = 1;
static constexpr uint8_t supportedMmuFWVersionBuild = 1;

void ProtocolLogic::CheckAndReportAsyncEvents(){
    // even when waiting for a query period, we need to report a change in filament sensor's state
    // - it is vital for a precise synchronization of moves of the printer and the MMU
    uint8_t fs = (uint8_t)WhereIsFilament();
    if( fs != lastFSensor ){
        SendAndUpdateFilamentSensor();
    }
}

void ProtocolLogic::SendQuery(){
    SendMsg(RequestMsg(RequestMsgCodes::Query, 0));
    scopeState = ScopeState::QuerySent;
}

void ProtocolLogic::SendFINDAQuery(){
    SendMsg(RequestMsg(RequestMsgCodes::Finda, 0 ) );
    scopeState = ScopeState::FINDAReqSent;
}

void ProtocolLogic::SendAndUpdateFilamentSensor(){
    SendMsg(RequestMsg(RequestMsgCodes::FilamentSensor, lastFSensor = (uint8_t)WhereIsFilament() ) );
    scopeState = ScopeState::FilamentSensorStateSent;
}

void ProtocolLogic::SendButton(uint8_t btn){
    SendMsg(RequestMsg(RequestMsgCodes::Button, btn));
    scopeState = ScopeState::ButtonSent;
}

void ProtocolLogic::SendVersion(uint8_t stage) {
    SendMsg(RequestMsg(RequestMsgCodes::Version, stage));
    scopeState = (ScopeState)((uint_fast8_t)ScopeState::S0Sent + stage);
}

void ProtocolLogic::SendReadRegister(uint8_t index, ScopeState nextState) {
    SendMsg(RequestMsg(RequestMsgCodes::Read, index));
    scopeState = nextState;
}


// searches for "ok\n" in the incoming serial data (that's the usual response of the old MMU FW)
struct OldMMUFWDetector {
    uint8_t ok;
    inline constexpr OldMMUFWDetector():ok(0) { }
    
    enum class State : uint8_t { MatchingPart, SomethingElse, Matched };
    
    /// @returns true when "ok\n" gets detected
    State Detect(uint8_t c){
        // consume old MMU FW's data if any -> avoid confusion of protocol decoder
        if(ok == 0 && c == 'o'){
            ++ok;
            return State::MatchingPart;
        } else if(ok == 1 && c == 'k'){
            ++ok;
            return State::MatchingPart;
        } else if(ok == 2 && c == '\n'){
            return State::Matched;
        }
        return State::SomethingElse;
    }
};

StepStatus ProtocolLogic::ExpectingMessage(uint32_t timeout) {
    int bytesConsumed = 0;
    int c = -1;
    
    OldMMUFWDetector oldMMUh4x0r; // old MMU FW hacker ;)
        
    // try to consume as many rx bytes as possible (until a message has been completed)
    while((c = uart->read()) >= 0){
        ++bytesConsumed;
        RecordReceivedByte(c);
        switch (protocol.DecodeResponse(c)) {
        case DecodeStatus::MessageCompleted:
            rsp = protocol.GetResponseMsg();
            LogResponse();
            RecordUARTActivity(); // something has happened on the UART, update the timeout record
            return MessageReady;
        case DecodeStatus::NeedMoreData:
            break;
        case DecodeStatus::Error:{
            // consume old MMU FW's data if any -> avoid confusion of protocol decoder
            auto old = oldMMUh4x0r.Detect(c);
            if( old == OldMMUFWDetector::State::Matched ){
                // hack bad FW version - BEWARE - we silently assume that the first query is an "S0"
                // The old MMU FW responds with "ok\n" and we fake the response to a bad FW version at this spot
                rsp = ResponseMsg(RequestMsg(RequestMsgCodes::Version, 0), ResponseMsgParamCodes::Accepted, 0);
                return MessageReady;
            } else if( old == OldMMUFWDetector::State::MatchingPart ){
                break;
            }
            }
            [[fallthrough]]; // otherwise
        default:
            RecordUARTActivity(); // something has happened on the UART, update the timeout record
            return ProtocolError;
        }
    }
    if( bytesConsumed != 0 ){
        RecordUARTActivity(); // something has happened on the UART, update the timeout record
        return Processing; // consumed some bytes, but message still not ready
    } else if (Elapsed(timeout)) {
        return CommunicationTimeout;
    }
    return Processing;
}

void ProtocolLogic::SendMsg(RequestMsg rq) {
    uint8_t txbuff[Protocol::MaxRequestSize()];
    uint8_t len = Protocol::EncodeRequest(rq, txbuff);
    uart->write(txbuff, len);
    LogRequestMsg(txbuff, len);
    RecordUARTActivity();
}

void ProtocolLogic::StartSeqRestart() {
    retries = maxRetries;
    SendVersion(0);
}

void ProtocolLogic::DelayedRestartRestart() {
    scopeState = ScopeState::RecoveringProtocolError;
}

void ProtocolLogic::CommandRestart() {
    scopeState = ScopeState::CommandSent;
    SendMsg(rq);
}

void ProtocolLogic::IdleRestart() {
    scopeState = ScopeState::Ready;
}

StepStatus ProtocolLogic::StartSeqStep(){
    if (auto expmsg = ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
        return expmsg;
    
    // solve initial handshake
    switch (scopeState) {
    case ScopeState::S0Sent: // received response to S0 - major
        if( rsp.request.code != RequestMsgCodes::Version || rsp.request.value != 0 ){
            // got a response to something else - protocol corruption probably, repeat the query
            SendVersion(0);
        } else {
            mmuFwVersionMajor = rsp.paramValue;
            if (mmuFwVersionMajor != supportedMmuFWVersionMajor) {
                if( --retries == 0){
                    // if (--retries == 0) has a specific meaning - since we are losing bytes on the UART for no obvious reason
                    // it can happen, that the reported version number is not complete - i.e. "1" instead of "19"
                    // Therefore we drop the MMU only if we run out of retries for this very reason.
                    // There is a limited amount of retries per the whole start seq.
                    // We also must be able to actually detect an unsupported MMU FW version, so the amount of retries shall be kept small.
                    return VersionMismatch;
                } else {
                    SendVersion(0);
                }
            } else {
                dataTO.Reset(); // got meaningful response from the MMU, stop data layer timeout tracking
                SendVersion(1);
            }
        }
        break;
    case ScopeState::S1Sent: // received response to S1 - minor
        if( rsp.request.code != RequestMsgCodes::Version || rsp.request.value != 1 ){
            // got a response to something else - protocol corruption probably, repeat the query OR restart the comm by issuing S0?
            SendVersion(1);
        } else {
            mmuFwVersionMinor = rsp.paramValue;
            if (mmuFwVersionMinor != supportedMmuFWVersionMinor){
                if( --retries == 0) {
                    return VersionMismatch;
                } else {
                    SendVersion(1);
                }
            } else {
                SendVersion(2);
            }
        }
        break;
    case ScopeState::S2Sent: // received response to S2 - revision
        if( rsp.request.code != RequestMsgCodes::Version || rsp.request.value != 2 ){
            // got a response to something else - protocol corruption probably, repeat the query OR restart the comm by issuing S0?
            SendVersion(2);
        } else {
            mmuFwVersionBuild = rsp.paramValue;
            if (mmuFwVersionBuild < supportedMmuFWVersionBuild){
                if( --retries == 0 ) {
                    return VersionMismatch;
                } else {
                    SendVersion(2);
                }
            } else {
                // Start General Interrogation after line up.
                // For now we just send the state of the filament sensor, but we may request
                // data point states from the MMU as well. TBD in the future, especially with another protocol
                SendAndUpdateFilamentSensor();
            }
        }
        break;
    case ScopeState::FilamentSensorStateSent:
        scopeState = ScopeState::Ready;
        SwitchFromStartToIdle();
        return Processing; // Returning Finished is not a good idea in case of a fast error recovery
        // - it tells the printer, that the command which experienced a protocol error and recovered successfully actually terminated.
        // In such a case we must return "Processing" in order to keep the MMU state machine running and prevent the printer from executing next G-codes.
        break;
    case ScopeState::RecoveringProtocolError:
        // timer elapsed, clear the input buffer
        while (uart->read() >= 0)
            ;
        SendVersion(0);
        break;
    default:
        return VersionMismatch;
    }
    return Finished;
}

StepStatus ProtocolLogic::DelayedRestartStep() {
    switch (scopeState) {
    case ScopeState::RecoveringProtocolError:
        if (Elapsed(heartBeatPeriod)) { // this basically means, that we are waiting until there is some traffic on
            while (uart->read() != -1)
                ; // clear the input buffer
            // switch to StartSeq
            Start();
        }
        return Processing;
        break;
    default:
        break;
    }
    return Finished;
}

StepStatus ProtocolLogic::CommandStep() {
    switch (scopeState) {
    case ScopeState::Wait:
        if (Elapsed(heartBeatPeriod)) {
            SendQuery();
        } else { 
            // even when waiting for a query period, we need to report a change in filament sensor's state
            // - it is vital for a precise synchronization of moves of the printer and the MMU
            CheckAndReportAsyncEvents();
        }
        break;
    case ScopeState::CommandSent: {
        if (auto expmsg = ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
            return expmsg;

        switch (rsp.paramCode) { // the response should be either accepted or rejected
        case ResponseMsgParamCodes::Accepted:
            progressCode = ProgressCode::OK;
            errorCode = ErrorCode::RUNNING;
            scopeState = ScopeState::Wait;
            break;
        case ResponseMsgParamCodes::Rejected:
            // rejected - should normally not happen, but report the error up
            progressCode = ProgressCode::OK;
            errorCode = ErrorCode::PROTOCOL_ERROR;
            return CommandRejected;
        default:
            return ProtocolError;
        }
        } break;
    case ScopeState::QuerySent:
        if (auto expmsg = ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
            return expmsg;
        [[fallthrough]];
    case ScopeState::ContinueFromIdle:
        switch (rsp.paramCode) {
        case ResponseMsgParamCodes::Processing:
            progressCode = static_cast<ProgressCode>(rsp.paramValue);
            errorCode = ErrorCode::OK;
            SendAndUpdateFilamentSensor(); // keep on reporting the state of fsensor regularly
            break;
        case ResponseMsgParamCodes::Error:
            // in case of an error the progress code remains as it has been before
            errorCode = static_cast<ErrorCode>(rsp.paramValue);
            // keep on reporting the state of fsensor regularly even in command error state
            // - the MMU checks FINDA and fsensor even while recovering from errors
            SendAndUpdateFilamentSensor();
            return CommandError;
        case ResponseMsgParamCodes::Button:
            // The user pushed a button on the MMU. Save it, do what we need to do 
            // to prepare, then pass it back to the MMU so it can work its magic.
            buttonCode = static_cast<Buttons>(rsp.paramValue);
            SendAndUpdateFilamentSensor();
            return ButtonPushed;
        case ResponseMsgParamCodes::Finished:
            progressCode = ProgressCode::OK;
            scopeState = ScopeState::Ready;
            return Finished;
        default:
            return ProtocolError;
        }
        break;
    case ScopeState::FilamentSensorStateSent:
        if (auto expmsg = ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
            return expmsg;
        SendFINDAQuery();
        scopeState = ScopeState::FINDAReqSent;
        return Processing;
    case ScopeState::FINDAReqSent:
        if (auto expmsg = ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
            return expmsg;
        SendReadRegister(3, ScopeState::StatisticsSent);
        scopeState = ScopeState::StatisticsSent;
        return Processing;
    case ScopeState::StatisticsSent:
        if (auto expmsg = ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
            return expmsg;
        scopeState = ScopeState::Wait;
        return Processing;
    case ScopeState::ButtonSent:
        if (auto expmsg = ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
            return expmsg;
        if (rsp.paramCode == ResponseMsgParamCodes::Accepted) {
            // Button was accepted, decrement the retry.
            mmu2.DecrementRetryAttempts();
        }
        SendAndUpdateFilamentSensor();
        break;
    default:
        return ProtocolError;
    }
    return Processing;
}

StepStatus ProtocolLogic::IdleStep() {
    if(scopeState == ScopeState::Ready){ // check timeout
        if (Elapsed(heartBeatPeriod)) {
            SendQuery();
            return Processing;
        }
    } else {
        if (auto expmsg = ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
            return expmsg;
        
        switch (scopeState) {
        case ScopeState::QuerySent: // check UART
            // If we are accidentally in Idle and we receive something like "T0 P1" - that means the communication dropped out while a command was in progress.
            // That causes no issues here, we just need to switch to Command processing and continue there from now on.
            // The usual response in this case should be some command and "F" - finished - that confirms we are in an Idle state even on the MMU side.
            switch( rsp.request.code ){
            case RequestMsgCodes::Cut:
            case RequestMsgCodes::Eject:
            case RequestMsgCodes::Load:
            case RequestMsgCodes::Mode:
            case RequestMsgCodes::Tool:
            case RequestMsgCodes::Unload:
                if( rsp.paramCode != ResponseMsgParamCodes::Finished ){
                    SwitchFromIdleToCommand();
                    return Processing;
                }
                break;
            case RequestMsgCodes::Reset:
                // this one is kind of special
                // we do not transfer to any "running" command (i.e. we stay in Idle),
                // but in case there is an error reported we must make sure it gets propagated
                switch( rsp.paramCode ){
                case ResponseMsgParamCodes::Button:
                    // The user pushed a button on the MMU. Save it, do what we need to do 
                    // to prepare, then pass it back to the MMU so it can work its magic.
                    buttonCode = static_cast<Buttons>(rsp.paramValue);
                    SendFINDAQuery();
                    return ButtonPushed;
                case ResponseMsgParamCodes::Processing:
                    // @@TODO we may actually use this branch to report progress of manual operation on the MMU
                    // The MMU sends e.g. X0 P27 after its restart when the user presses an MMU button to move the Selector
                    // For now let's behave just like "finished"
                case ResponseMsgParamCodes::Finished:
                    errorCode = ErrorCode::OK;
                    break;
                default:
                    errorCode = static_cast<ErrorCode>(rsp.paramValue);
                    SendFINDAQuery(); // continue Idle state without restarting the communication
                    return CommandError;
                }
                break;
            default:
                return ProtocolError;
            }
            SendFINDAQuery();
            return Processing;
            break;
        case ScopeState::FINDAReqSent:
            SendReadRegister(3, ScopeState::StatisticsSent);
            scopeState = ScopeState::StatisticsSent;
            return Processing;
        case ScopeState::StatisticsSent:
            failStatistics = rsp.paramValue;
            scopeState = ScopeState::Ready;
            return Processing;
        case ScopeState::ButtonSent:
            if (rsp.paramCode == ResponseMsgParamCodes::Accepted) {
                // Button was accepted, decrement the retry.
                mmu2.DecrementRetryAttempts();
            }
            SendFINDAQuery();
            break;
        default:
            return ProtocolError;
        }
    }
    
    // The "return Finished" in this state machine requires a bit of explanation:
    // The Idle state either did nothing (still waiting for the heartbeat timeout)
    // or just successfully received the answer to Q0, whatever that was.
    // In both cases, it is ready to hand over work to a command or something else,
    // therefore we are returning Finished (also to exit mmu_loop() and unblock Marlin's loop!).
    // If there is no work, we'll end up in the Idle state again
    // and we'll send the heartbeat message after the specified timeout.
    return Finished;
}

ProtocolLogic::ProtocolLogic(MMU2Serial *uart)
    : currentScope(Scope::Stopped)
    , scopeState(ScopeState::Ready)
    , plannedRq(RequestMsgCodes::unknown, 0)
    , lastUARTActivityMs(0)
    , dataTO()
    , rsp(RequestMsg(RequestMsgCodes::unknown, 0), ResponseMsgParamCodes::unknown, 0)
    , state(State::Stopped)
    , lrb(0)
    , uart(uart)
    , errorCode(ErrorCode::OK)
    , progressCode(ProgressCode::OK)
    , buttonCode(NoButton)
    , lastFSensor((uint8_t)WhereIsFilament())
    , findaPressed(false)
    , failStatistics(0)
    , mmuFwVersionMajor(0)
    , mmuFwVersionMinor(0)
    , mmuFwVersionBuild(0)
{}

void ProtocolLogic::Start() {
    state = State::InitSequence;
    currentScope = Scope::StartSeq;
    protocol.ResetResponseDecoder(); // important - finished delayed restart relies on this
    StartSeqRestart();
}

void ProtocolLogic::Stop() {
    state = State::Stopped;
    currentScope = Scope::Stopped;
}

void ProtocolLogic::ToolChange(uint8_t slot) {
    PlanGenericRequest(RequestMsg(RequestMsgCodes::Tool, slot));
}

void ProtocolLogic::Statistics() {
    PlanGenericRequest(RequestMsg(RequestMsgCodes::Version, 3));
}

void ProtocolLogic::UnloadFilament() {
    PlanGenericRequest(RequestMsg(RequestMsgCodes::Unload, 0));
}

void ProtocolLogic::LoadFilament(uint8_t slot) {
    PlanGenericRequest(RequestMsg(RequestMsgCodes::Load, slot));
}

void ProtocolLogic::EjectFilament(uint8_t slot) {
    PlanGenericRequest(RequestMsg(RequestMsgCodes::Eject, slot));
}

void ProtocolLogic::CutFilament(uint8_t slot){
    PlanGenericRequest(RequestMsg(RequestMsgCodes::Cut, slot));
}

void ProtocolLogic::ResetMMU() {
    PlanGenericRequest(RequestMsg(RequestMsgCodes::Reset, 0));
}

void ProtocolLogic::Button(uint8_t index){
    PlanGenericRequest(RequestMsg(RequestMsgCodes::Button, index));
}

void ProtocolLogic::Home(uint8_t mode){
    PlanGenericRequest(RequestMsg(RequestMsgCodes::Home, mode));
}

void ProtocolLogic::PlanGenericRequest(RequestMsg rq) {
    plannedRq = rq;
    if( ! ExpectsResponse() ){
        ActivatePlannedRequest();
    } // otherwise wait for an empty window to activate the request
}

bool ProtocolLogic::ActivatePlannedRequest(){
    if( plannedRq.code == RequestMsgCodes::Button ){
        // only issue the button to the MMU and do not restart the state machines
        SendButton(plannedRq.value);
        plannedRq = RequestMsg(RequestMsgCodes::unknown, 0);
        return true;
    } else if( plannedRq.code != RequestMsgCodes::unknown ){
        currentScope = Scope::Command;
        SetRequestMsg(plannedRq);
        plannedRq = RequestMsg(RequestMsgCodes::unknown, 0);
        CommandRestart();
        return true;
    }
    return false;
}

void ProtocolLogic::SwitchFromIdleToCommand(){
    currentScope = Scope::Command;
    SetRequestMsg(rsp.request);
    // we are recovering from a communication drop out, the command is already running
    // and we have just received a response to a Q0 message about a command progress
    CommandContinueFromIdle();
}

void ProtocolLogic::SwitchToIdle() {
    state = State::Running;
    currentScope = Scope::Idle;
    IdleRestart();
}

void ProtocolLogic::SwitchFromStartToIdle(){
    state = State::Running;
    currentScope = Scope::Idle;
    IdleRestart();
    SendQuery(); // force sending Q0 immediately
    scopeState = ScopeState::QuerySent;
}

StepStatus ProtocolLogic::ScopeStep(){
    switch(currentScope){
    case Scope::StartSeq:
        return StartSeqStep();
    case Scope::DelayedRestart:
        return DelayedRestartStep();
    case Scope::Idle:
        return IdleStep();
    case Scope::Command:
        return CommandStep();
    case Scope::Stopped:
        return StoppedStep();
    default:
        break;
    }
    return Finished;
}

bool ProtocolLogic::Elapsed(uint32_t timeout) const {
    return _millis() >= (lastUARTActivityMs + timeout);
}

void ProtocolLogic::RecordUARTActivity() {
    lastUARTActivityMs = _millis();
}

void ProtocolLogic::RecordReceivedByte(uint8_t c){
    lastReceivedBytes[lrb] = c;
    lrb = (lrb+1) % lastReceivedBytes.size();
}

constexpr char NibbleToChar(uint8_t c){
    switch (c) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        return c + '0';
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        return (c - 10) + 'a';
    default:
        return 0;
    }
}

void ProtocolLogic::FormatLastReceivedBytes(char *dst){
    for(uint8_t i = 0; i < lastReceivedBytes.size(); ++i){
        uint8_t b = lastReceivedBytes[ (lrb-i-1) % lastReceivedBytes.size() ];
        dst[i*3] = NibbleToChar(b >> 4);
        dst[i*3+1] = NibbleToChar(b & 0xf);
        dst[i*3+2] = ' ';
    }
    dst[ (lastReceivedBytes.size() - 1) * 3 + 2] = 0; // terminate properly
}

void ProtocolLogic::FormatLastResponseMsgAndClearLRB(char *dst){
    *dst++ = '<';
    for(uint8_t i = 0; i < lrb; ++i){
        uint8_t b = lastReceivedBytes[ i ];
        if( b < 32 )b = '.';
        if( b > 127 )b = '.';
        *dst++ = b;
    }
    *dst = 0; // terminate properly
    lrb = 0; // reset the input buffer index in case of a clean message
}

void ProtocolLogic::LogRequestMsg(const uint8_t *txbuff, uint8_t size){
    constexpr uint_fast8_t rqs = modules::protocol::Protocol::MaxRequestSize() + 2;
    char tmp[rqs] = ">";
    static char lastMsg[rqs] = "";
    for(uint8_t i = 0; i < size; ++i){
        uint8_t b = txbuff[i];
        if( b < 32 )b = '.';
        if( b > 127 )b = '.';
        tmp[i+1] = b;
    }
    tmp[size+1] = '\n';
    tmp[size+2] = 0;
    if( !strncmp_P(tmp, PSTR(">S0*99.\n"), rqs) && !strncmp(lastMsg, tmp, rqs) ){
        // @@TODO we skip the repeated request msgs for now 
        // to avoid spoiling the whole log just with ">S0" messages
        // especially when the MMU is not connected.
        // We'll lose the ability to see if the printer is actually
        // trying to find the MMU, but since it has been reliable in the past
        // we can live without it for now.
    } else {
        MMU2_ECHO_MSG(tmp);
    }
    memcpy(lastMsg, tmp, rqs);
}

void ProtocolLogic::LogError(const char *reason_P){
    char lrb[lastReceivedBytes.size() * 3];
    FormatLastReceivedBytes(lrb);
    
    MMU2_ERROR_MSGRPGM(reason_P);
    SERIAL_ECHOPGM(", last bytes: ");
    SERIAL_ECHOLN(lrb);
}

void ProtocolLogic::LogResponse(){
    char lrb[lastReceivedBytes.size()];
    FormatLastResponseMsgAndClearLRB(lrb);
    MMU2_ECHO_MSG(lrb);
    SERIAL_ECHOLN();
}

StepStatus ProtocolLogic::SuppressShortDropOuts(const char *msg_P, StepStatus ss) {
    if( dataTO.Record(ss) ){
        LogError(msg_P);
        return dataTO.InitialCause();
    } else {
        return Processing; // suppress short drop outs of communication
    }
}

StepStatus ProtocolLogic::HandleCommunicationTimeout() {
    uart->flush(); // clear the output buffer
    protocol.ResetResponseDecoder();
    Start();
    return SuppressShortDropOuts(PSTR("Communication timeout"), CommunicationTimeout);
}

StepStatus ProtocolLogic::HandleProtocolError() {
    uart->flush(); // clear the output buffer
    state = State::InitSequence;
    currentScope = Scope::DelayedRestart;
    DelayedRestartRestart();
    return SuppressShortDropOuts(PSTR("Protocol Error"), ProtocolError);
}

StepStatus ProtocolLogic::Step() {
    if( ! ExpectsResponse() ){ // if not waiting for a response, activate a planned request immediately
        ActivatePlannedRequest();
    }
    auto currentStatus = ScopeStep();
    switch (currentStatus) {
    case Processing:
        // we are ok, the state machine continues correctly
        break;
    case Finished: {
        // We are ok, switching to Idle if there is no potential next request planned.
        // But the trouble is we must report a finished command if the previous command has just been finished
        // i.e. only try to find some planned command if we just finished the Idle cycle
        bool previousCommandFinished = currentScope == Scope::Command; // @@TODO this is a nasty hack :( 
        if( ! ActivatePlannedRequest() ){ // if nothing is planned, switch to Idle
            SwitchToIdle();
        } else {
            // if the previous cycle was Idle and now we have planned a new command -> avoid returning Finished
            if( ! previousCommandFinished && currentScope == Scope::Command){
                currentStatus = Processing;
            }
        }
    } break;
    case CommandRejected:
        // we have to repeat it - that's the only thing we can do
        // no change in state
        // @@TODO wait until Q0 returns command in progress finished, then we can send this one
        LogError(PSTR("Command rejected"));
        CommandRestart();
        break;
    case CommandError:
        LogError(PSTR("Command Error"));
        // we shall probably transfer into the Idle state and await further instructions from the upper layer
        // Idle state may solve the problem of keeping up the heart beat running
        break;
    case VersionMismatch:
        LogError(PSTR("Version mismatch"));
        Stop(); // cannot continue
        break;
    case ProtocolError:
        currentStatus = HandleProtocolError();
        break;
    case CommunicationTimeout:
        currentStatus = HandleCommunicationTimeout();
        break;
    default:
        break;
    }
    return currentStatus;
}

uint8_t ProtocolLogic::CommandInProgress() const {
    if( currentScope != Scope::Command )
        return 0;
    return (uint8_t)ReqMsg().code;
}

bool DropOutFilter::Record(StepStatus ss){
    if( occurrences == maxOccurrences ){
        cause = ss;
    }
    --occurrences;
    return occurrences == 0;
}

} // namespace MMU2
