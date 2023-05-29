/// @file progress_codes.h
#pragma once
#include <stdint.h>

/// A complete set of progress codes which may be reported while running a high-level command/operation
/// This header file shall be included in the printer's firmware as well as a reference,
/// therefore the progress codes have been extracted to one place
enum class ProgressCode : uint_fast8_t {
    OK = 0, ///< finished ok

    EngagingIdler, // P1
    DisengagingIdler, // P2
    UnloadingToFinda, // P3
    UnloadingToPulley, //P4
    FeedingToFinda, // P5
    FeedingToBondtech, // P6
    FeedingToNozzle, // P7
    AvoidingGrind, // P8
    FinishingMoves, // P9

    ERRDisengagingIdler, // P10
    ERREngagingIdler, // P11 - unused: intended for SlowLoad which is removed now
    ERRWaitingForUser, // P12
    ERRInternal, // P13
    ERRHelpingFilament, // P14 - unused: intended for SlowLoad which is removed now
    ERRTMCFailed, // P15

    UnloadingFilament, // P16
    LoadingFilament, // P17
    SelectingFilamentSlot, // P18
    PreparingBlade, // P19
    PushingFilament, // P20
    PerformingCut, // P21
    ReturningSelector, // P22
    ParkingSelector, // P23
    EjectingFilament, // P24
    RetractingFromFinda, // P25
    Homing, // P26
    MovingSelector, // P27
    FeedingToFSensor, // P28
    UnloadingFilamentSlowly, // P29

    HWTestBegin, // P31
    HWTestIdler, // P31
    HWTestSelector, // P32
    HWTestPulley, // P33
    HWTestCleanup, // P34
    HWTestExec, // P35
    HWTestDisplay, // P36
    ErrHwTestFailed, // P37

    Empty = 0xff // dummy empty state
};
