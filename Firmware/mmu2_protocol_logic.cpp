#include "mmu2_protocol_logic.h"
#include "mmu2_log.h"
#include "mmu2_fsensor.h"
#include "system_timer.h"
#include <string.h>

namespace MMU2 {

StepStatus ProtocolLogicPartBase::ProcessFINDAReqSent(StepStatus finishedRV, State nextState){
    if (auto expmsg = logic->ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
        return expmsg;
    logic->findaPressed = logic->rsp.paramValue;
    state = nextState;
    return finishedRV;
}

void ProtocolLogicPartBase::CheckAndReportAsyncEvents(){
    // even when waiting for a query period, we need to report a change in filament sensor's state
    // - it is vital for a precise synchronization of moves of the printer and the MMU
    uint8_t fs = (uint8_t)WhereIsFilament();
    if( fs != logic->lastFSensor ){
        SendAndUpdateFilamentSensor();
    }
}

void ProtocolLogicPartBase::SendQuery(){
    logic->SendMsg(RequestMsg(RequestMsgCodes::Query, 0));
    state = State::QuerySent;
}

void ProtocolLogicPartBase::SendFINDAQuery(){
    logic->SendMsg(RequestMsg(RequestMsgCodes::Finda, 0 ) );
    state = State::FINDAReqSent;
}

void ProtocolLogicPartBase::SendAndUpdateFilamentSensor(){
    logic->SendMsg(RequestMsg(RequestMsgCodes::FilamentSensor, logic->lastFSensor = (uint8_t)WhereIsFilament() ) );
    state = State::FilamentSensorStateSent;
}

void ProtocolLogicPartBase::SendButton(uint8_t btn){
    logic->SendMsg(RequestMsg(RequestMsgCodes::Button, btn));
    state = State::ButtonSent;
}

void ProtocolLogicPartBase::SendVersion(uint8_t stage) {
    logic->SendMsg(RequestMsg(RequestMsgCodes::Version, stage));
    state = (State)((uint_fast8_t)State::S0Sent + stage);
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
            // otherwise [[fallthrough]]
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

void StartSeq::Restart() {
    SendVersion(0);
}

StepStatus StartSeq::Step() {
    if (auto expmsg = logic->ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
        return expmsg;

    // solve initial handshake
    switch (state) {
    case State::S0Sent: // received response to S0 - major
        if( logic->rsp.request.code != RequestMsgCodes::Version || logic->rsp.request.value != 0 ){
            // got a response to something else - protocol corruption probably, repeat the query
                SendVersion(0);
        } else {
            logic->mmuFwVersionMajor = logic->rsp.paramValue;
            if (logic->mmuFwVersionMajor != 2) {
                return VersionMismatch;
            }
            logic->dataTO.Reset(); // got meaningful response from the MMU, stop data layer timeout tracking
                SendVersion(1);
        }
        break;
    case State::S1Sent: // received response to S1 - minor
        if( logic->rsp.request.code != RequestMsgCodes::Version || logic->rsp.request.value != 1 ){
            // got a response to something else - protocol corruption probably, repeat the query OR restart the comm by issuing S0?
                SendVersion(1);
        } else {
            logic->mmuFwVersionMinor = logic->rsp.paramValue;
            if (logic->mmuFwVersionMinor != 0) {
                return VersionMismatch;
            }
                SendVersion(2);
        }
        break;
    case State::S2Sent: // received response to S2 - revision
        if( logic->rsp.request.code != RequestMsgCodes::Version || logic->rsp.request.value != 2 ){
            // got a response to something else - protocol corruption probably, repeat the query OR restart the comm by issuing S0?
                SendVersion(2);
        } else {
            logic->mmuFwVersionBuild = logic->rsp.paramValue;
            if (logic->mmuFwVersionBuild < 18) {
                return VersionMismatch;
            }
            // Start General Interrogation after line up.
            // For now we just send the state of the filament sensor, but we may request
            // data point states from the MMU as well. TBD in the future, especially with another protocol
            SendAndUpdateFilamentSensor();
        }
        break;
    case State::FilamentSensorStateSent:
        state = State::Ready;
        return Finished;
        break;
    case State::RecoveringProtocolError:
        // timer elapsed, clear the input buffer
            while (logic->uart->read() >= 0)
                ;
            SendVersion(0);
        break;
    default:
        return VersionMismatch;
    }
    return Processing;
}

void DelayedRestart::Restart() {
    state = State::RecoveringProtocolError;
}

StepStatus DelayedRestart::Step() {
    switch (state) {
    case State::RecoveringProtocolError:
        if (logic->Elapsed(heartBeatPeriod)) { // this basically means, that we are waiting until there is some traffic on
            while (logic->uart->read() != -1)
                ; // clear the input buffer
            // switch to StartSeq
            logic->Start();
        }
        return Processing;
        break;
    default:
        break;
    }
    return Finished;
}

void Command::Restart() {
    state = State::CommandSent;
    logic->SendMsg(logic->command.rq);
}

StepStatus Command::Step() {
    switch (state) {
    case State::CommandSent: {
        if (auto expmsg = logic->ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
            return expmsg;

        switch (logic->rsp.paramCode) { // the response should be either accepted or rejected
        case ResponseMsgParamCodes::Accepted:
            logic->progressCode = ProgressCode::OK;
            logic->errorCode = ErrorCode::RUNNING;
            state = State::Wait;
            break;
        case ResponseMsgParamCodes::Rejected:
            // rejected - should normally not happen, but report the error up
            logic->progressCode = ProgressCode::OK;
            logic->errorCode = ErrorCode::PROTOCOL_ERROR;
            return CommandRejected;
        default:
            return ProtocolError;
        }
        } break;
    case State::Wait:
        if (logic->Elapsed(heartBeatPeriod)) {
            SendQuery();
        } else { 
            // even when waiting for a query period, we need to report a change in filament sensor's state
            // - it is vital for a precise synchronization of moves of the printer and the MMU
            CheckAndReportAsyncEvents();
        }
        break;
    case State::QuerySent:
        if (auto expmsg = logic->ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
            return expmsg;
        [[fallthrough]];
    case State::ContinueFromIdle:
        switch (logic->rsp.paramCode) {
        case ResponseMsgParamCodes::Processing:
            logic->progressCode = static_cast<ProgressCode>(logic->rsp.paramValue);
            logic->errorCode = ErrorCode::OK;
            SendAndUpdateFilamentSensor(); // keep on reporting the state of fsensor regularly
            break;
        case ResponseMsgParamCodes::Error:
            // in case of an error the progress code remains as it has been before
            logic->errorCode = static_cast<ErrorCode>(logic->rsp.paramValue);
            // keep on reporting the state of fsensor regularly even in command error state
            // - the MMU checks FINDA and fsensor even while recovering from errors
            SendAndUpdateFilamentSensor();
            return CommandError;
        case ResponseMsgParamCodes::Button:
            // The user pushed a button on the MMU. Save it, do what we need to do 
            // to prepare, then pass it back to the MMU so it can work its magic.
            logic->buttonCode = static_cast<Buttons>(logic->rsp.paramValue);
            SendAndUpdateFilamentSensor();
            return ButtonPushed;
        case ResponseMsgParamCodes::Finished:
            logic->progressCode = ProgressCode::OK;
            state = State::Ready;
            return Finished;
        default:
            return ProtocolError;
        }
        break;
    case State::FilamentSensorStateSent:
        if (auto expmsg = logic->ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
            return expmsg;
        SendFINDAQuery();
        break;
    case State::FINDAReqSent:
        return ProcessFINDAReqSent(Processing, State::Wait);
    case State::ButtonSent:{
        // button is never confirmed ... may be it should be
        if (auto expmsg = logic->ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
            return expmsg;
        if (logic->rsp.paramCode == ResponseMsgParamCodes::Accepted) {
            // Button was accepted, decrement the retry.
            mmu2.DecrementRetryAttempts();
        }
        SendAndUpdateFilamentSensor();
        } break;
    default:
        return ProtocolError;
    }
    return Processing;
}

void Idle::Restart() {
    state = State::Ready;
}

StepStatus Idle::Step() {
    switch (state) {
    case State::Ready: // check timeout
        if (logic->Elapsed(heartBeatPeriod)) {
            SendQuery();
            return Processing;
        }
        break;
    case State::QuerySent: // check UART
        if (auto expmsg = logic->ExpectingMessage(linkLayerTimeout); expmsg != MessageReady)
            return expmsg;
        // If we are accidentally in Idle and we receive something like "T0 P1" - that means the communication dropped out while a command was in progress.
        // That causes no issues here, we just need to switch to Command processing and continue there from now on.
        // The usual response in this case should be some command and "F" - finished - that confirms we are in an Idle state even on the MMU side.
        switch( logic->rsp.request.code ){
        case RequestMsgCodes::Cut:
        case RequestMsgCodes::Eject:
        case RequestMsgCodes::Load:
        case RequestMsgCodes::Mode:
        case RequestMsgCodes::Tool:
        case RequestMsgCodes::Unload:
            if( logic->rsp.paramCode != ResponseMsgParamCodes::Finished ){
                logic->SwitchFromIdleToCommand();
                return Processing;
            }
            break;
        case RequestMsgCodes::Reset:
            // this one is kind of special
            // we do not transfer to any "running" command (i.e. we stay in Idle),
            // but in case there is an error reported we must make sure it gets propagated
            switch( logic->rsp.paramCode ){
            case ResponseMsgParamCodes::Processing:
                // @@TODO we may actually use this branch to report progress of manual operation on the MMU
                // The MMU sends e.g. X0 P27 after its restart when the user presses an MMU button to move the Selector
                // For now let's behave just like "finished"
            case ResponseMsgParamCodes::Finished:
                logic->errorCode = ErrorCode::OK;
                break;
            default:
                logic->errorCode = static_cast<ErrorCode>(logic->rsp.paramValue);
                SendFINDAQuery(); // continue Idle state without restarting the communication
                return CommandError;
            }
            break;
        default:
            break;
        }
        SendFINDAQuery();
        return Processing;
        break;
    case State::FINDAReqSent:
        return ProcessFINDAReqSent(Finished, State::Ready);
    default:
        return ProtocolError;
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
    : stopped(this)
    , startSeq(this)
    , delayedRestart(this)
    , idle(this)
    , command(this)
    , currentState(&stopped)
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
    , mmuFwVersionMajor(0)
    , mmuFwVersionMinor(0)
    , mmuFwVersionBuild(0)
{}

void ProtocolLogic::Start() {
    state = State::InitSequence;
    currentState = &startSeq;
    protocol.ResetResponseDecoder(); // important - finished delayed restart relies on this
    startSeq.Restart();
}

void ProtocolLogic::Stop() {
    state = State::Stopped;
    currentState = &stopped;
}

void ProtocolLogic::ToolChange(uint8_t slot) {
    PlanGenericRequest(RequestMsg(RequestMsgCodes::Tool, slot));
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
    if( ! currentState->ExpectsResponse() ){
        ActivatePlannedRequest();
    } // otherwise wait for an empty window to activate the request
}

bool ProtocolLogic::ActivatePlannedRequest(){
    if( plannedRq.code == RequestMsgCodes::Button ){
        // only issue the button to the MMU and do not restart the state machines
        // @@TODO - this is not completely correct, but it does the job.
        // In Idle mode the command part is not active, but we still need button handling in Idle mode (resolve MMU init errors)
        // -> command.SendButton is not correct, but it sends the message and everything works (for now)
        command.SendButton(plannedRq.value);
        plannedRq = RequestMsg(RequestMsgCodes::unknown, 0);
        return true;
    } else if( plannedRq.code != RequestMsgCodes::unknown ){
        currentState = &command;
        command.SetRequestMsg(plannedRq);
        plannedRq = RequestMsg(RequestMsgCodes::unknown, 0);
        command.Restart();
        return true;
    }
    return false;
}

void ProtocolLogic::SwitchFromIdleToCommand(){
    currentState = &command;
    command.SetRequestMsg(rsp.request);
    // we are recovering from a communication drop out, the command is already running
    // and we have just received a response to a Q0 message about a command progress
    command.ContinueFromIdle();
}

void ProtocolLogic::SwitchToIdle() {
    state = State::Running;
    currentState = &idle;
    idle.Restart();
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
    if( !strncmp(tmp, ">S0.\n", rqs) && !strncmp(lastMsg, tmp, rqs) ){
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

void ProtocolLogic::LogError(const char *reason){
    char lrb[lastReceivedBytes.size() * 3];
    FormatLastReceivedBytes(lrb);
    
    MMU2_ERROR_MSG(reason);
    SERIAL_ECHO(", last bytes: ");
    SERIAL_ECHOLN(lrb);
}

void ProtocolLogic::LogResponse(){
    char lrb[lastReceivedBytes.size()];
    FormatLastResponseMsgAndClearLRB(lrb);
    MMU2_ECHO_MSG(lrb);
    SERIAL_ECHOLN();
}

StepStatus ProtocolLogic::SuppressShortDropOuts(const char *msg, StepStatus ss) {
    if( dataTO.Record(ss) ){
        LogError(msg);
        return dataTO.InitialCause();
    } else {
        return Processing; // suppress short drop outs of communication
    }
}

StepStatus ProtocolLogic::HandleCommunicationTimeout() {
    uart->flush(); // clear the output buffer
    protocol.ResetResponseDecoder();
    Start();
    return SuppressShortDropOuts("Communication timeout", CommunicationTimeout);
}

StepStatus ProtocolLogic::HandleProtocolError() {
    uart->flush(); // clear the output buffer
    state = State::InitSequence;
    currentState = &delayedRestart;
    delayedRestart.Restart();
    return SuppressShortDropOuts("Protocol Error", ProtocolError);
}

StepStatus ProtocolLogic::Step() {
    if( ! currentState->ExpectsResponse() ){ // if not waiting for a response, activate a planned request immediately
        ActivatePlannedRequest();
    }
    auto currentStatus = currentState->Step();
    switch (currentStatus) {
    case Processing:
        // we are ok, the state machine continues correctly
        break;
    case Finished: {
        // We are ok, switching to Idle if there is no potential next request planned.
        // But the trouble is we must report a finished command if the previous command has just been finished
        // i.e. only try to find some planned command if we just finished the Idle cycle
        bool previousCommandFinished = currentState == &command; // @@TODO this is a nasty hack :( 
        if( ! ActivatePlannedRequest() ){ // if nothing is planned, switch to Idle
            SwitchToIdle();
        } else {
            // if the previous cycle was Idle and now we have planned a new command -> avoid returning Finished
            if( ! previousCommandFinished && currentState == &command){
                currentStatus = Processing;
            }
        }
    } break;
    case CommandRejected:
        // we have to repeat it - that's the only thing we can do
        // no change in state
        // @@TODO wait until Q0 returns command in progress finished, then we can send this one
        LogError("Command rejected");
        command.Restart();
        break;
    case CommandError:
        LogError("Command Error");
        // we shall probably transfer into the Idle state and await further instructions from the upper layer
        // Idle state may solve the problem of keeping up the heart beat running
        break;
    case VersionMismatch:
        LogError("Version mismatch");
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
    if( currentState != &command )
        return 0;
    return (uint8_t)command.ReqMsg().code;
}

bool DropOutFilter::Record(StepStatus ss){
    if( occurrences == maxOccurrences ){
        cause = ss;
    }
    --occurrences;
    return occurrences == 0;
}

} // namespace MMU2
