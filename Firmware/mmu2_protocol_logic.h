#pragma once
#include <stdint.h>
// #include <array> //@@TODO Don't we have STL for AVR somewhere?
template<typename T, uint8_t N>
class array {
    T data[N];
public:
    array() = default;
    inline constexpr T* begin()const { return data; }
    inline constexpr T* end()const { return data + N; }
    constexpr uint8_t size()const { return N; }
    inline T &operator[](uint8_t i){
        return data[i];
    }
};

#include "mmu2/error_codes.h"
#include "mmu2/progress_codes.h"
#include "mmu2/buttons.h"
#include "mmu2_protocol.h"

#include "mmu2_serial.h"

/// New MMU2 protocol logic
namespace MMU2 {

using namespace modules::protocol;

class ProtocolLogic;

/// ProtocolLogic stepping statuses
enum StepStatus : uint_fast8_t {
    Processing = 0,
    MessageReady, ///< a message has been successfully decoded from the received bytes
    Finished,
    CommunicationTimeout, ///< the MMU failed to respond to a request within a specified time frame
    ProtocolError, ///< bytes read from the MMU didn't form a valid response
    CommandRejected, ///< the MMU rejected the command due to some other command in progress, may be the user is operating the MMU locally (button commands)
    CommandError, ///< the command in progress stopped due to unrecoverable error, user interaction required
    VersionMismatch, ///< the MMU reports its firmware version incompatible with our implementation
    CommunicationRecovered,
    ButtonPushed, ///< The MMU reported the user pushed one of its three buttons. 
};


static constexpr uint32_t linkLayerTimeout = 2000; ///< default link layer communication timeout
static constexpr uint32_t dataLayerTimeout = linkLayerTimeout * 3; ///< data layer communication timeout
static constexpr uint32_t heartBeatPeriod = linkLayerTimeout / 2; ///< period of heart beat messages (Q0)

static_assert( heartBeatPeriod < linkLayerTimeout && linkLayerTimeout < dataLayerTimeout, "Incorrect ordering of timeouts");

/// Base class for sub-automata of the ProtocolLogic class.
/// Their operation should never block (wait inside).
class ProtocolLogicPartBase {
public:
    inline ProtocolLogicPartBase(ProtocolLogic *logic)
        : logic(logic)
        , state(State::Ready) {}

    /// Restarts the sub-automaton
    virtual void Restart() = 0;

    /// Makes one step in the sub-automaton
    /// @returns StepStatus
    virtual StepStatus Step() = 0;
    
    /// @returns true if the state machine is waiting for a response from the MMU
    bool ExpectsResponse()const { return state != State::Ready && state != State::Wait; }

protected:
    ProtocolLogic *logic; ///< pointer to parent ProtocolLogic layer
    friend class ProtocolLogic;
    
    /// Common internal states of the derived sub-automata
    /// General rule of thumb: *Sent states are waiting for a response from the MMU
    enum class State : uint_fast8_t { 
        Ready,
        Wait,

        S0Sent,
        S1Sent,
        S2Sent,
        QuerySent,
        CommandSent,
        FilamentSensorStateSent,
        FINDAReqSent,
        ButtonSent,

        ContinueFromIdle
    };

    State state; ///< internal state of the sub-automaton

    /// @returns the status of processing of the FINDA query response
    /// @param finishedRV returned value in case the message was successfully received and processed
    /// @param nextState is a state where the state machine should transfer to after the message was successfully received and processed
    StepStatus ProcessFINDAReqSent(StepStatus finishedRV, State nextState);
    
    /// Called repeatedly while waiting for a query (Q0) period.
    /// All event checks to report immediately from the printer to the MMU shall be done in this method.
    /// So far, the only such a case is the filament sensor, but there can be more like this in the future.
    void CheckAndReportAsyncEvents();
    
    void SendQuery();
    
    void SendFINDAQuery();
    
    void SendAndUpdateFilamentSensor();

    void SendButton(uint8_t btn);
};

/// Starting sequence of the communication with the MMU.
/// The printer shall ask for MMU's version numbers.
/// If everything goes well and the MMU's version is good enough,
/// the ProtocolLogic layer may continue talking to the MMU
class StartSeq : public ProtocolLogicPartBase {
public:
    inline StartSeq(ProtocolLogic *logic)
        : ProtocolLogicPartBase(logic) {}
    void Restart() override;
    StepStatus Step() override;
};

/// A command and its lifecycle.
/// CommandSent:
/// - the command was placed into the UART TX buffer, awaiting response from the MMU
/// - if the MMU confirms the command, we'll wait for it to finish
/// - if the MMU refuses the command, we report an error (should normally not happen unless someone is hacking the communication without waiting for the previous command to finish)
/// Wait:
/// - waiting for the MMU to process the command - may take several seconds, for example Tool change operation
/// - meawhile, every 300ms we send a Q0 query to obtain the current state of the command being processed
/// - as soon as we receive a response to Q0 from the MMU, we process it in the next state
/// QuerySent - check the reply from the MMU - can be any of the following:
/// - Processing: the MMU is still working
/// - Error: the command failed on the MMU, we'll have the exact error report in the response message
/// - Finished: the MMU finished the command successfully, another command may be issued now
class Command : public ProtocolLogicPartBase {
public:
    inline Command(ProtocolLogic *logic)
        : ProtocolLogicPartBase(logic)
        , rq(RequestMsgCodes::unknown, 0) {}
    void Restart() override;
    StepStatus Step() override;
    inline void SetRequestMsg(RequestMsg msg) {
        rq = msg;
    }
    void ContinueFromIdle(){
        state = State::ContinueFromIdle;
    }
    inline const RequestMsg &ReqMsg()const { return rq; }

private:
    RequestMsg rq;
};

/// Idle state - we have no command for the MMU, so we are only regularly querying its state with Q0 messages.
/// The idle state can be interrupted any time to issue a command into the MMU
class Idle : public ProtocolLogicPartBase {
public:
    inline Idle(ProtocolLogic *logic)
        : ProtocolLogicPartBase(logic) {}
    void Restart() override;
    StepStatus Step() override;
};

/// The communication with the MMU is stopped/disabled (for whatever reason).
/// Nothing is being put onto the UART.
class Stopped : public ProtocolLogicPartBase {
public:
    inline Stopped(ProtocolLogic *logic)
        : ProtocolLogicPartBase(logic) {}
    void Restart() override {}
    StepStatus Step() override { return Processing; }
};

///< Filter of short consecutive drop outs which are recovered instantly
class DropOutFilter {
    StepStatus cause;
    uint8_t occurrences;
public:
    static constexpr uint8_t maxOccurrences = 10; // ideally set this to >8 seconds -> 12x heartBeatPeriod
    static_assert (maxOccurrences > 1, "we should really silently ignore at least 1 comm drop out if recovered immediately afterwards");
    DropOutFilter() = default;
    
    /// @returns true if the error should be reported to higher levels (max. number of consecutive occurrences reached)
    bool Record(StepStatus ss);
    
    /// @returns the initial cause which started this drop out event
    inline StepStatus InitialCause()const { return cause; }
    
    /// Rearms the object for further processing - basically call this once the MMU responds with something meaningful (e.g. S0 A2)
    inline void Reset(){ occurrences = maxOccurrences; }
};

/// Logic layer of the MMU vs. printer communication protocol
class ProtocolLogic {
public:
    ProtocolLogic(MMU2Serial *uart);

    /// Start/Enable communication with the MMU
    void Start();

    /// Stop/Disable communication with the MMU
    void Stop();

    // Issue commands to the MMU
    void ToolChange(uint8_t slot);
    void UnloadFilament();
    void LoadFilament(uint8_t slot);
    void EjectFilament(uint8_t slot);
    void CutFilament(uint8_t slot);
    void ResetMMU();
    void Button(uint8_t index);
    void Home(uint8_t mode);

    /// Step the state machine
    StepStatus Step();

    /// @returns the current/latest error code as reported by the MMU
    ErrorCode Error() const { return errorCode; }

    /// @returns the current/latest process code as reported by the MMU
    ProgressCode Progress() const { return progressCode; }
    
    /// @returns the current/latest button code as reported by the MMU
    Buttons Button() const { return buttonCode; }

    uint8_t CommandInProgress()const;

    inline bool Running()const {
        return state == State::Running;
    }

    inline bool FindaPressed() const {
        return findaPressed;
    }

    inline uint8_t MmuFwVersionMajor() const {
        return mmuFwVersionMajor;
    }

    inline uint8_t MmuFwVersionMinor() const {
        return mmuFwVersionMinor;
    }

    inline uint8_t MmuFwVersionBuild() const {
        return mmuFwVersionBuild;
    }
#ifndef UNITTEST
private:
#endif
    
    StepStatus ProcessUARTByte(uint8_t c);
    StepStatus ExpectingMessage(uint32_t timeout);
    void SendMsg(RequestMsg rq);
    void SwitchToIdle();
    void HandleCommunicationTimeout();
    StepStatus HandleCommError(const char *msg, StepStatus ss);
    bool Elapsed(uint32_t timeout) const;
    void RecordUARTActivity();
    void RecordReceivedByte(uint8_t c);
    void FormatLastReceivedBytes(char *dst);
    void FormatLastResponseMsgAndClearLRB(char *dst);
    void LogRequestMsg(const uint8_t *txbuff, uint8_t size);
    void LogError(const char *reason);
    void LogResponse();
    void SwitchFromIdleToCommand();
    
    enum class State : uint_fast8_t {
        Stopped,      ///< stopped for whatever reason
        InitSequence, ///< initial sequence running
        Running       ///< normal operation - Idle + Command processing
    };

    // individual sub-state machines - may be they can be combined into a union since only one is active at once
    Stopped stopped;
    StartSeq startSeq;
    Idle idle;
    Command command;
    ProtocolLogicPartBase *currentState; ///< command currently being processed
    
    /// Records the next planned state, "unknown" msg code if no command is planned.
    /// This is not intended to be a queue of commands to process, protocol_logic must not queue commands.
    /// It exists solely to prevent breaking the Request-Response protocol handshake -
    /// - during tests it turned out, that the commands from Marlin are coming in such an asynchronnous way, that
    /// we could accidentally send T2 immediately after Q0 without waiting for reception of response to Q0.
    /// 
    /// Beware, if Marlin manages to call PlanGenericCommand multiple times before a response comes,
    /// these variables will get overwritten by the last call.
    /// However, that should not happen under normal circumstances as Marlin should wait for the Command to finish, 
    /// which includes all responses (and error recovery if any).
    RequestMsg plannedRq;

    /// Plan a command to be processed once the immediate response to a sent request arrives
    void PlanGenericRequest(RequestMsg rq);
    /// Activate the planned state once the immediate response to a sent request arrived
    bool ActivatePlannedRequest();

    uint32_t lastUARTActivityMs; ///< timestamp - last ms when something occurred on the UART
    DropOutFilter dataTO; ///< Filter of short consecutive drop outs which are recovered instantly

    ResponseMsg rsp; ///< decoded response message from the MMU protocol

    State state; ///< internal state of ProtocolLogic

    Protocol protocol; ///< protocol codec
    
    array<uint8_t, 16> lastReceivedBytes; ///< remembers the last few bytes of incoming communication for diagnostic purposes
    uint8_t lrb;

    MMU2Serial *uart; ///< UART interface

    ErrorCode errorCode;       ///< last received error code from the MMU
    ProgressCode progressCode; ///< last received progress code from the MMU
    Buttons buttonCode;        ///< Last received button from the MMU.

    uint8_t lastFSensor; ///< last state of filament sensor

    bool findaPressed;

    uint8_t mmuFwVersionMajor, mmuFwVersionMinor;
    uint8_t mmuFwVersionBuild;

    friend class ProtocolLogicPartBase;
    friend class Stopped;
    friend class Command;
    friend class Idle;
    friend class StartSeq;

    friend class MMU2;
};

} // namespace MMU2
