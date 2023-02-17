/**
 * @file
 * @author Marek Bel
 */


#include "catch2/catch_test_macros.hpp"
#include "../Firmware/Timer.h"
#include "../Firmware/TimerRemaining.h"

static unsigned long now = 0;

unsigned long millis()
{
    return now;
}

unsigned long millis2()
{
    return now;
}

static void basicTimer()
{
    LongTimer timer;
    CHECK( timer.running() == false);

    timer.start();
    CHECK( timer.running() == true);

    timer.stop();
    CHECK( timer.running() == false);

    timer.start();
    CHECK( timer.expired(0) == true);
    CHECK( timer.expired(0) == false);
    CHECK( timer.running() == false);

    timer.start();
    CHECK( timer.expired(1) == false);
    CHECK( timer.running() == true);
    ++now;
    CHECK( timer.expired(1) == true);
    CHECK( timer.running() == false);
    --now;

    timer.start();
    CHECK( timer.expired(ULONG_MAX - 1) == false);
    now+= ULONG_MAX - 2;
    CHECK( timer.expired(ULONG_MAX - 1) == false);
    now++;
    CHECK( timer.expired(ULONG_MAX - 1) == true);
    CHECK( timer.running() == false);
    now-= ULONG_MAX - 1;

    timer.start();
    CHECK( timer.expired(ULONG_MAX) == false);
    now+= ULONG_MAX - 1;
    CHECK( timer.expired(ULONG_MAX) == false);
    now++;
    CHECK( timer.expired(ULONG_MAX) == true);
    CHECK( timer.running() == false);
    now-= ULONG_MAX;

    timer.start();
    CHECK( timer.running() == true);
    CHECK( timer.expired(12*3600000ul + 38*60000ul + 15000) == false);
    now+= 12*3600000ul + 38*60000ul + 14999;
    CHECK( timer.expired(12*3600000ul + 38*60000ul + 15000) == false);
    ++now;
    CHECK( timer.expired(12*3600000ul + 38*60000ul + 15000) == true);
    CHECK( timer.running() == false);
    now-= 12*3600000ul + 38*60000ul + 15000;

}

TEST_CASE( "LongTimer test.", "[timer]" )
{
    now = 0;
    basicTimer();
    now = 1;
    basicTimer();
    now = ULONG_MAX;
    basicTimer();
    now = ULONG_MAX - 1;
    basicTimer();
    now = 12*3600000ul + 38*60000ul + 15000;
    basicTimer();
}

static void basicRemaining()
{
    TimerRemaining timer;
    CHECK( timer.running() == false);
    CHECK( timer.remaining() == 0);

    timer.start(100);
    CHECK( timer.running() == true);
    CHECK( timer.remaining() == 100);
    now += 99;
    CHECK( timer.running() == true);
    CHECK( timer.remaining() == 1);

    ++now;
    CHECK( timer.running() == true);
    CHECK( timer.remaining() == 0);
    CHECK( timer.running() == false);
    now -=100;

    timer.start(1);
    timer.stop();
    CHECK( timer.remaining() == 0);
    CHECK( timer.running() == false);

    timer.start(0);
    CHECK( timer.remaining() == 0);
    CHECK( timer.running() == false);

    timer.start(1);
    CHECK( timer.remaining() == 1);
    CHECK( timer.running() == true);

    ++now;
    CHECK( timer.running() == true);
    CHECK( timer.remaining() == 0);
    CHECK( timer.running() == false);
    --now;

    timer.start(ULONG_MAX - 1);
    CHECK( timer.running() == true);
    CHECK( timer.remaining() == ULONG_MAX - 1);
    now+= ULONG_MAX - 2;
    CHECK( timer.running() == true);
    CHECK( timer.remaining() == 1);
    CHECK( timer.running() == true);
    ++now;
    CHECK( timer.remaining() == 0);
    CHECK( timer.running() == false);
    now-= ULONG_MAX - 1;

    timer.start(ULONG_MAX);
    CHECK( timer.running() == true);
    CHECK( timer.remaining() == ULONG_MAX);
    now+= ULONG_MAX - 1;
    CHECK( timer.remaining() == 1);
    CHECK( timer.running() == true);
    ++now;
    CHECK( timer.remaining() == 0);
    CHECK( timer.running() == false);

    timer.start(12*3600000ul + 38*60000ul + 15000);
    CHECK( timer.running() == true);
    CHECK( timer.remaining() == 12*3600000ul + 38*60000ul + 15000);
    now+= 12*3600000ul + 38*60000ul + 14999;
    CHECK( timer.remaining() == 1);
    ++now;
    CHECK( timer.remaining() == 0);
    CHECK( timer.running() == false);
    now-= 12*3600000ul + 38*60000ul + 15000;
}

TEST_CASE( "TimerRemaining test.", "[timer]" )
{
    now = 0;
    basicRemaining();
    now = 1;
    basicRemaining();
    now = ULONG_MAX;
    basicRemaining();
    now = ULONG_MAX - 1;
    basicRemaining();

    now = 12*3600000ul + 38*60000ul + 15000;
    basicRemaining();

}
