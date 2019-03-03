/**
 * @file
 * @author Marek Bel
 */

#include "catch.hpp"

#include "../Firmware/AutoDeplete.h"

TEST_CASE( "AutoDeplete test.", "[AutoDeplete]" )
{
    CHECK(ad_allDepleted() == false);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 1);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);

    ad_markDepleted(1);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 2);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_allDepleted() == false);

    ad_markDepleted(3);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 2);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 4);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_allDepleted() == false);

    ad_markDepleted(4);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 2);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 0);
    CHECK(ad_getAlternative(4) == 0);
    CHECK(ad_allDepleted() == false);

    ad_markDepleted(4);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 2);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 0);
    CHECK(ad_getAlternative(4) == 0);
    CHECK(ad_allDepleted() == false);

    ad_markDepleted(0);

    CHECK(ad_getAlternative(0) == 2);
    CHECK(ad_getAlternative(1) == 2);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 2);
    CHECK(ad_getAlternative(4) == 2);
    CHECK(ad_allDepleted() == false);

    ad_markDepleted(2);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 1);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_allDepleted() == true);

    ad_markDepleted(2);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 1);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_allDepleted() == true);

    ad_markLoaded(4);

    CHECK(ad_getAlternative(0) == 4);
    CHECK(ad_getAlternative(1) == 4);
    CHECK(ad_getAlternative(2) == 4);
    CHECK(ad_getAlternative(3) == 4);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(0);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 4);
    CHECK(ad_getAlternative(2) == 4);
    CHECK(ad_getAlternative(3) == 4);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(3);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 3);
    CHECK(ad_getAlternative(2) == 3);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(3);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 3);
    CHECK(ad_getAlternative(2) == 3);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(2);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 2);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(1);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 1);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(1);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 1);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_allDepleted() == false);

}
