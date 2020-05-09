/**
 * @file
 * @author Marek Bel
 */

#include "catch.hpp"

#include "../Firmware/AutoDeplete.h"
uint8_t mmu_filament_count = 32;

TEST_CASE( "AutoDeplete test.", "[AutoDeplete]" )
{
    CHECK(ad_allDepleted() == false);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 1);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_getAlternative(15) == 15);
    CHECK(ad_getAlternative(31) == 31);

    ad_markDepleted(1);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 2);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_getAlternative(15) == 15);
    CHECK(ad_getAlternative(31) == 31);
    CHECK(ad_allDepleted() == false);

    ad_markDepleted(3);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 2);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 4);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_getAlternative(15) == 15);
    CHECK(ad_getAlternative(31) == 31);
    CHECK(ad_allDepleted() == false);

    ad_markDepleted(4);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 2);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 5);
    CHECK(ad_getAlternative(4) == 5);
    CHECK(ad_getAlternative(15) == 15);
    CHECK(ad_getAlternative(31) == 31);
    CHECK(ad_allDepleted() == false);

    ad_markDepleted(0);

    CHECK(ad_getAlternative(0) == 2);
    CHECK(ad_getAlternative(1) == 2);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 5);
    CHECK(ad_getAlternative(4) == 5);
    CHECK(ad_getAlternative(15) == 15);
    CHECK(ad_getAlternative(31) == 31);
    CHECK(ad_allDepleted() == false);

    ad_markDepleted(2);

    CHECK(ad_getAlternative(0) == 5);
    CHECK(ad_getAlternative(1) == 5);
    CHECK(ad_getAlternative(2) == 5);
    CHECK(ad_getAlternative(3) == 5);
    CHECK(ad_getAlternative(4) == 5);
    CHECK(ad_getAlternative(15) == 15);
    CHECK(ad_getAlternative(31) == 31);
    CHECK(ad_allDepleted() == false);

    for (uint8_t i=5; i<16; i++) {
	    ad_markDepleted(i);
	}

    CHECK(ad_getAlternative(0) == 16);
    CHECK(ad_getAlternative(1) == 16);
    CHECK(ad_getAlternative(2) == 16);
    CHECK(ad_getAlternative(3) == 16);
    CHECK(ad_getAlternative(4) == 16);
    CHECK(ad_getAlternative(15) == 16);
    CHECK(ad_getAlternative(31) == 31);
    CHECK(ad_allDepleted() == false);
    
    for (uint8_t i=16; i<31; i++) {
	    ad_markDepleted(i);
	}
    
    CHECK(ad_getAlternative(0) == 31);
    CHECK(ad_getAlternative(1) == 31);
    CHECK(ad_getAlternative(2) == 31);
    CHECK(ad_getAlternative(3) == 31);
    CHECK(ad_getAlternative(4) == 31);
    CHECK(ad_getAlternative(15) == 31);
    CHECK(ad_getAlternative(31) == 31);
    CHECK(ad_allDepleted() == false);
    
    ad_markDepleted(31);
    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 1);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_getAlternative(15) == 15);
    CHECK(ad_getAlternative(31) == 31);
    CHECK(ad_allDepleted() == true);
	
	
    ad_markLoaded(4);

    CHECK(ad_getAlternative(0) == 4);
    CHECK(ad_getAlternative(1) == 4);
    CHECK(ad_getAlternative(2) == 4);
    CHECK(ad_getAlternative(3) == 4);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_getAlternative(15) == 4);
    CHECK(ad_getAlternative(31) == 4);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(0);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 4);
    CHECK(ad_getAlternative(2) == 4);
    CHECK(ad_getAlternative(3) == 4);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_getAlternative(15) == 0);
    CHECK(ad_getAlternative(31) == 0);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(3);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 3);
    CHECK(ad_getAlternative(2) == 3);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_getAlternative(15) == 0);
    CHECK(ad_getAlternative(31) == 0);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(3);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 3);
    CHECK(ad_getAlternative(2) == 3);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_getAlternative(15) == 0);
    CHECK(ad_getAlternative(31) == 0);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(2);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 2);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_getAlternative(15) == 0);
    CHECK(ad_getAlternative(31) == 0);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(1);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 1);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_getAlternative(15) == 0);
    CHECK(ad_getAlternative(31) == 0);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(1);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 1);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_getAlternative(15) == 0);
    CHECK(ad_getAlternative(31) == 0);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(31);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 1);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_getAlternative(15) == 31);
    CHECK(ad_getAlternative(31) == 31);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(22);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 1);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_getAlternative(15) == 22);
    CHECK(ad_getAlternative(31) == 31);
    CHECK(ad_allDepleted() == false);

    ad_markLoaded(15);

    CHECK(ad_getAlternative(0) == 0);
    CHECK(ad_getAlternative(1) == 1);
    CHECK(ad_getAlternative(2) == 2);
    CHECK(ad_getAlternative(3) == 3);
    CHECK(ad_getAlternative(4) == 4);
    CHECK(ad_getAlternative(15) == 15);
    CHECK(ad_getAlternative(31) == 31);
    CHECK(ad_allDepleted() == false);
}
