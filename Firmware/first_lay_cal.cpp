/*
 * first_lay_cal.cpp
 *
 *  Created on: Jun 10, 2019
 *      Author: marek
 */
#include "first_lay_cal.h"
#include "Configuration_prusa.h"
#include "language.h"

static const char cmd_0[] PROGMEM = "M107";
static const char cmd_1[] PROGMEM = "M104 S" STRINGIFY(PLA_PREHEAT_HOTEND_TEMP);
static const char cmd_2[] PROGMEM = "M140 S" STRINGIFY(PLA_PREHEAT_HPB_TEMP);
static const char cmd_3[] PROGMEM = "M190 S" STRINGIFY(PLA_PREHEAT_HPB_TEMP);
static const char cmd_4[] PROGMEM = "M109 S" STRINGIFY(PLA_PREHEAT_HOTEND_TEMP);
static const char cmd_6[] PROGMEM = "G28";
static const char cmd_7[] PROGMEM = "G92 E0.0";


const char * const layer1_cal[8] PROGMEM =
{
    cmd_0,
    cmd_1,
    cmd_2,
    cmd_3,
    cmd_4,
    MSG_M117_V2_CALIBRATION, //TODO missing internationalization
    cmd_6,
    cmd_7,
};
