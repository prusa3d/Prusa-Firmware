/// @file protocol.h
#pragma once
#include <stdint.h>

namespace modules {

/// @brief The MMU communication protocol implementation and related stuff.
///
/// See description of the new protocol in the MMU 2021 doc
/// @@TODO possibly add some checksum to verify the correctness of messages
namespace protocol {

/// Definition of request message codes
enum class RequestMsgCodes : uint8_t {
    unknown = 0,
    Query = 'Q',
    Tool = 'T',
    Load = 'L',
    Mode = 'M',
    Unload = 'U',
    Reset = 'X',
    Finda = 'P',
    Version = 'S',
    Button = 'B',
    Eject = 'E',
    Wait = 'W',
    Cut = 'K',
    FilamentType = 'F',
    FilamentSensor = 'f',
    Home = 'H'
};

/// Definition of response message parameter codes
enum class ResponseMsgParamCodes : uint8_t {
    unknown = 0,
    Processing = 'P',
    Error = 'E',
    Finished = 'F',
    Accepted = 'A',
    Rejected = 'R'
};

/// A request message - requests are being sent by the printer into the MMU.
struct RequestMsg {
    RequestMsgCodes code; ///< code of the request message
    uint8_t value; ///< value of the request message

    /// @param code of the request message
    /// @param value of the request message
    inline RequestMsg(RequestMsgCodes code, uint8_t value)
        : code(code)
        , value(value) {}
};

/// A response message - responses are being sent from the MMU into the printer as a response to a request message.
struct ResponseMsg {
    RequestMsg request; ///< response is always preceeded by the request message
    ResponseMsgParamCodes paramCode; ///< code of the parameter
    uint16_t paramValue; ///< value of the parameter

    /// @param request the source request message this response is a reply to
    /// @param paramCode code of the parameter
    /// @param paramValue value of the parameter
    inline ResponseMsg(RequestMsg request, ResponseMsgParamCodes paramCode, uint16_t paramValue)
        : request(request)
        , paramCode(paramCode)
        , paramValue(paramValue) {}
};

/// Message decoding return values
enum class DecodeStatus : uint_fast8_t {
    MessageCompleted, ///< message completed and successfully lexed
    NeedMoreData, ///< message incomplete yet, waiting for another byte to come
    Error, ///< input character broke message decoding
};

/// Protocol class is responsible for creating/decoding messages in Rx/Tx buffer
///
/// Beware - in the decoding more, it is meant to be a statefull instance which works through public methods
/// processing one input byte per call.
class Protocol {
public:
    inline Protocol()
        : rqState(RequestStates::Code)
        , requestMsg(RequestMsgCodes::unknown, 0)
        , rspState(ResponseStates::RequestCode)
        , responseMsg(RequestMsg(RequestMsgCodes::unknown, 0), ResponseMsgParamCodes::unknown, 0) {
    }

    /// Takes the input byte c and steps one step through the state machine
    /// @returns state of the message being decoded
    DecodeStatus DecodeRequest(uint8_t c);

    /// Decodes response message in rxbuff
    /// @returns decoded response message structure
    DecodeStatus DecodeResponse(uint8_t c);

    /// Encodes request message msg into txbuff memory
    /// It is expected the txbuff is large enough to fit the message
    /// @returns number of bytes written into txbuff
    static uint8_t EncodeRequest(const RequestMsg &msg, uint8_t *txbuff);

    /// @returns the maximum byte length necessary to encode a request message
    /// Beneficial in case of pre-allocating a buffer for enconding a RequestMsg.
    static constexpr uint8_t MaxRequestSize() { return 3; }

    /// Encode generic response Command Accepted or Rejected
    /// @param msg source request message for this response
    /// @param ar code of response parameter
    /// @param txbuff where to format the message
    /// @returns number of bytes written into txbuff
    static uint8_t EncodeResponseCmdAR(const RequestMsg &msg, ResponseMsgParamCodes ar, uint8_t *txbuff);

    /// Encode response to Read FINDA query
    /// @param msg source request message for this response
    /// @param findaValue 1/0 (on/off) status of FINDA
    /// @param txbuff where to format the message
    /// @returns number of bytes written into txbuff
    static uint8_t EncodeResponseReadFINDA(const RequestMsg &msg, uint8_t findaValue, uint8_t *txbuff);

    /// Encode response to Version query
    /// @param msg source request message for this response
    /// @param value version number (0-255)
    /// @param txbuff where to format the message
    /// @returns number of bytes written into txbuff
    static uint8_t EncodeResponseVersion(const RequestMsg &msg, uint8_t value, uint8_t *txbuff);

    /// Encode response to Query operation status
    /// @param msg source request message for this response
    /// @param code status of operation (Processing, Error, Finished)
    /// @param value related to status of operation(e.g. error code or progress)
    /// @param txbuff where to format the message
    /// @returns number of bytes written into txbuff
    static uint8_t EncodeResponseQueryOperation(const RequestMsg &msg, ResponseMsgParamCodes code, uint16_t value, uint8_t *txbuff);

    /// @returns the most recently lexed request message
    inline const RequestMsg GetRequestMsg() const { return requestMsg; }

    /// @returns the most recently lexed response message
    inline const ResponseMsg GetResponseMsg() const { return responseMsg; }

    /// resets the internal request decoding state (typically after an error)
    void ResetRequestDecoder() {
        rqState = RequestStates::Code;
    }

    /// resets the internal response decoding state (typically after an error)
    void ResetResponseDecoder() {
        rspState = ResponseStates::RequestCode;
    }

private:
    enum class RequestStates : uint8_t {
        Code, ///< starting state - expects message code
        Value, ///< expecting code value
        Error ///< automaton in error state
    };

    RequestStates rqState;
    RequestMsg requestMsg;

    enum class ResponseStates : uint8_t {
        RequestCode, ///< starting state - expects message code
        RequestValue, ///< expecting code value
        ParamCode, ///< expecting param code
        ParamValue, ///< expecting param value
        Error ///< automaton in error state
    };

    ResponseStates rspState;
    ResponseMsg responseMsg;

    static bool IsNewLine(uint8_t c) {
        return c == '\n' || c == '\r';
    }
    static bool IsDigit(uint8_t c) {
        return c >= '0' && c <= '9';
    }
};

} // namespace protocol
} // namespace modules

namespace mp = modules::protocol;
