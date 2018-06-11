/**
 * @file
 * @author Marek Bel
 */


#include "catch.hpp"
#include "../Firmware/Timer.h"
#include "../Firmware/TimerRemaining.h"

unsigned long millis()
{
    return 1;
}

TEST_CASE( "LongTimer tested.", "[timer]" )
{
    LongTimer timer;
    REQUIRE( timer.running() == false);

    timer.start();
    REQUIRE( timer.running() == true);

    timer.stop();
    REQUIRE( timer.running() == false);

    timer.start();
    REQUIRE( timer.expired(0) == true );
    REQUIRE( timer.expired(0) == false );
    REQUIRE( timer.running() == false);

    timer.start();
    REQUIRE( timer.expired(1) == false );
    REQUIRE( timer.running() == true);

    TimerRemaining otherTimer;
    otherTimer.start(100);
}
