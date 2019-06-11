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
#include "mmu.h"
#include <avr/pgmspace.h>


static const char cmd_preheat_0[] PROGMEM = "M107";
static const char cmd_preheat_1[] PROGMEM = "M104 S" STRINGIFY(PLA_PREHEAT_HOTEND_TEMP);
static const char cmd_preheat_2[] PROGMEM = "M140 S" STRINGIFY(PLA_PREHEAT_HPB_TEMP);
static const char cmd_preheat_3[] PROGMEM = "M190 S" STRINGIFY(PLA_PREHEAT_HPB_TEMP);
static const char cmd_preheat_4[] PROGMEM = "M109 S" STRINGIFY(PLA_PREHEAT_HOTEND_TEMP);
static const char cmd_preheat_5[] PROGMEM = "G28";
static const char cmd_preheat_6[] PROGMEM = "G92 E0.0";


static const char * const preheat_cmd[] PROGMEM =
{
    cmd_preheat_0,
    cmd_preheat_1,
    cmd_preheat_2,
    cmd_preheat_3,
    cmd_preheat_4,
    cmd_preheat_5, //call MSG_M117_V2_CALIBRATION before
    cmd_preheat_6,
};

void lay1cal_preheat()
{
    for (uint8_t i = 0; i < (sizeof(preheat_cmd)/sizeof(preheat_cmd[0])); ++i)
    {
        if (5 == i)  enquecommand_P(_T(MSG_M117_V2_CALIBRATION));
        enquecommand_P(static_cast<char*>(pgm_read_ptr(&preheat_cmd[i])));
    }

}

static const char cmd_intro_mmu_0[] PROGMEM = "M83";
static const char cmd_intro_mmu_1[] PROGMEM = "G1 Y-3.0 F1000.0";
static const char cmd_intro_mmu_2[] PROGMEM = "G1 Z0.4 F1000.0";
static const char cmd_intro_mmu_3[] PROGMEM = "G1 X55.0 E32.0 F1073.0"; // call T code before
static const char cmd_intro_mmu_4[] PROGMEM = "G1 X5.0 E32.0 F1800.0";
static const char cmd_intro_mmu_5[] PROGMEM = "G1 X55.0 E8.0 F2000.0";
static const char cmd_intro_mmu_6[] PROGMEM = "G1 Z0.3 F1000.0";
static const char cmd_intro_mmu_7[] PROGMEM = "G92 E0.0";
static const char cmd_intro_mmu_8[] PROGMEM = "G1 X240.0 E25.0  F2200.0";
static const char cmd_intro_mmu_9[] PROGMEM = "G1 Y-2.0 F1000.0";
static const char cmd_intro_mmu_10[] PROGMEM = "G1 X55.0 E25 F1400.0";
static const char cmd_intro_mmu_11[] PROGMEM = "G1 Z0.20 F1000.0";
static const char cmd_intro_mmu_12[] PROGMEM = "G1 X5.0 E4.0 F1000.0";

static const char * const intro_mmu_cmd[] PROGMEM =
{
    cmd_intro_mmu_0,
    cmd_intro_mmu_1,
    cmd_intro_mmu_2,
    cmd_intro_mmu_3, // call T code before
    cmd_intro_mmu_4,
    cmd_intro_mmu_5,
    cmd_intro_mmu_6,
    cmd_intro_mmu_7,
    cmd_intro_mmu_8,
    cmd_intro_mmu_9,
    cmd_intro_mmu_10,
    cmd_intro_mmu_11,
    cmd_intro_mmu_12,
};

void lay1cal_intro_line(char *cmd_buffer, uint8_t filament)
{
    if (mmu_enabled)
    {
        for (uint8_t i = 0; i < (sizeof(intro_mmu_cmd)/sizeof(intro_mmu_cmd[0])); ++i)
        {
            if (3 == i)
                {
                    sprintf_P(cmd_buffer, PSTR("T%d"), filament);
                    enquecommand(cmd_buffer);
                }
            enquecommand_P(static_cast<char*>(pgm_read_ptr(&intro_mmu_cmd[i])));
        }
    }
    else
    {
        enquecommand_P(PSTR("G1 X60.0 E9.0 F1000.0"));
        enquecommand_P(PSTR("G1 X100.0 E12.5 F1000.0"));
    }
}
