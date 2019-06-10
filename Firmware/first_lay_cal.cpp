/*
 * first_lay_cal.cpp
 *
 *  Created on: Jun 10, 2019
 *      Author: marek
 */
#include "first_lay_cal.h"
#include "Configuration_prusa.h"
#include "language.h"
#include "Marlin.h"
#include <avr/pgmspace.h>


static const char cmd_0[] PROGMEM = "M107";
static const char cmd_1[] PROGMEM = "M104 S" STRINGIFY(PLA_PREHEAT_HOTEND_TEMP);
static const char cmd_2[] PROGMEM = "M140 S" STRINGIFY(PLA_PREHEAT_HPB_TEMP);
static const char cmd_3[] PROGMEM = "M190 S" STRINGIFY(PLA_PREHEAT_HPB_TEMP);
static const char cmd_4[] PROGMEM = "M109 S" STRINGIFY(PLA_PREHEAT_HOTEND_TEMP);
static const char cmd_5[] PROGMEM = "G28";
static const char cmd_6[] PROGMEM = "G92 E0.0";


static const char * const preheat_cmd[] PROGMEM =
{
    cmd_0,
    cmd_1,
    cmd_2,
    cmd_3,
    cmd_4,
    cmd_5, //call MSG_M117_V2_CALIBRATION before
    cmd_6,
};

void lay1cal_preheat()
{
    for (uint8_t i = 0; i < (sizeof(preheat_cmd)/sizeof(preheat_cmd[0])); ++i)
    {
        if (5 == i)  enquecommand_P(_T(MSG_M117_V2_CALIBRATION));
        enquecommand_P(static_cast<char*>(pgm_read_ptr(&preheat_cmd[i])));
    }

}
