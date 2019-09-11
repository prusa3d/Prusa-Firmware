//! @file
//! @date Jun 10, 2019
//! @author Marek Bel
//! @brief First layer (Z offset) calibration

#include "first_lay_cal.h"
#include "Configuration_prusa.h"
#include "language.h"
#include "Marlin.h"
#include "mmu.h"
#include <avr/pgmspace.h>

//! @brief Wait for preheat
void lay1cal_wait_preheat()
{
    static const char cmd_preheat_0[] PROGMEM = "M107";
    static const char cmd_preheat_1[] PROGMEM = "M190";
    static const char cmd_preheat_2[] PROGMEM = "M109";
    static const char cmd_preheat_4[] PROGMEM = "G28";
    static const char cmd_preheat_5[] PROGMEM = "G92 E0.0";

    const char * const preheat_cmd[] =
    {
        cmd_preheat_0,
        cmd_preheat_1,
        cmd_preheat_2,
        _T(MSG_M117_V2_CALIBRATION),
        cmd_preheat_4,
        cmd_preheat_5,
    };

    for (uint8_t i = 0; i < (sizeof(preheat_cmd)/sizeof(preheat_cmd[0])); ++i)
    {
        enquecommand_P(preheat_cmd[i]);
    }

}

//! @brief Load filament
//! @param cmd_buffer character buffer needed to format gcodes
//! @param filament filament to use (applies for MMU only)
void lay1cal_load_filament(char *cmd_buffer, uint8_t filament)
{
    if (mmu_enabled)
    {
        enquecommand_P(PSTR("M83"));
        enquecommand_P(PSTR("G1 Y-3.0 F1000.0"));
        enquecommand_P(PSTR("G1 Z0.4 F1000.0"));
        sprintf_P(cmd_buffer, PSTR("T%d"), filament);
        enquecommand(cmd_buffer);
    }

}

//! @brief Print intro line
void lay1cal_intro_line()
{
    static const char cmd_intro_mmu_3[] PROGMEM = "G1 X55.0 E32.0 F1073.0";
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
        cmd_intro_mmu_3,
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

    if (mmu_enabled)
    {
        for (uint8_t i = 0; i < (sizeof(intro_mmu_cmd)/sizeof(intro_mmu_cmd[0])); ++i)
        {
            enquecommand_P(static_cast<char*>(pgm_read_ptr(&intro_mmu_cmd[i])));
        }
    }
    else
    {
        enquecommand_P(PSTR("G1 X60.0 E9.0 F1000.0"));
        enquecommand_P(PSTR("G1 X100.0 E12.5 F1000.0"));
    }
}

//! @brief Setup for printing meander
void lay1cal_before_meander()
{
    static const char cmd_pre_meander_0[] PROGMEM = "G92 E0.0";
    static const char cmd_pre_meander_1[] PROGMEM = "G21"; //set units to millimeters TODO unsupported command
    static const char cmd_pre_meander_2[] PROGMEM = "G90"; //use absolute coordinates
    static const char cmd_pre_meander_3[] PROGMEM = "M83"; //use relative distances for extrusion TODO: duplicate
    static const char cmd_pre_meander_4[] PROGMEM = "G1 E-1.50000 F2100.00000";
    static const char cmd_pre_meander_5[] PROGMEM = "G1 Z5 F7200.000";
    static const char cmd_pre_meander_6[] PROGMEM = "M204 S1000"; //set acceleration
    static const char cmd_pre_meander_7[] PROGMEM = "G1 F4000";

    static const char * const cmd_pre_meander[] PROGMEM =
    {
            cmd_pre_meander_0,
            cmd_pre_meander_1,
            cmd_pre_meander_2,
            cmd_pre_meander_3,
            cmd_pre_meander_4,
            cmd_pre_meander_5,
            cmd_pre_meander_6,
            cmd_pre_meander_7,
    };

    for (uint8_t i = 0; i < (sizeof(cmd_pre_meander)/sizeof(cmd_pre_meander[0])); ++i)
    {
        enquecommand_P(static_cast<char*>(pgm_read_ptr(&cmd_pre_meander[i])));
    }
}


//! @brief Count extrude length
//!
//! @param layer_height layer height in mm
//! @param extrusion_width extrusion width in mm
//! @param extrusion_length extrusion length in mm
//! @return filament length in mm which needs to be extruded to form line
static constexpr float count_e(float layer_height, float extrusion_width, float extrusion_length)
{
    return (extrusion_length * layer_height * extrusion_width / (M_PI * pow(1.75, 2) / 4));
}

static const float width = 0.4; //!< line width
static const float length = 20 - width; //!< line length
static const float height = 0.2; //!< layer height TODO This is wrong, as current Z height is 0.15 mm
static const float extr = count_e(height, width, length); //!< E axis movement needed to print line

//! @brief Print meander
//! @param cmd_buffer character buffer needed to format gcodes
void lay1cal_meander(char *cmd_buffer)
{
    static const char cmd_meander_0[] PROGMEM = "G1 X50 Y155";
    static const char cmd_meander_1[] PROGMEM = "G1 Z0.150 F7200.000";
    static const char cmd_meander_2[] PROGMEM = "G1 F1080";
    static const char cmd_meander_3[] PROGMEM = "G1 X75 Y155 E2.5";
    static const char cmd_meander_4[] PROGMEM = "G1 X100 Y155 E2";
    static const char cmd_meander_5[] PROGMEM = "G1 X200 Y155 E2.62773";
    static const char cmd_meander_6[] PROGMEM = "G1 X200 Y135 E0.66174";
    static const char cmd_meander_7[] PROGMEM = "G1 X50 Y135 E3.62773";
    static const char cmd_meander_8[] PROGMEM = "G1 X50 Y115 E0.49386";
    static const char cmd_meander_9[] PROGMEM = "G1 X200 Y115 E3.62773";
    static const char cmd_meander_10[] PROGMEM = "G1 X200 Y95 E0.49386";
    static const char cmd_meander_11[] PROGMEM = "G1 X50 Y95 E3.62773";
    static const char cmd_meander_12[] PROGMEM = "G1 X50 Y75 E0.49386";
    static const char cmd_meander_13[] PROGMEM = "G1 X200 Y75 E3.62773";
    static const char cmd_meander_14[] PROGMEM = "G1 X200 Y55 E0.49386";
    static const char cmd_meander_15[] PROGMEM = "G1 X50 Y55 E3.62773";

    static const char * const cmd_meander[] PROGMEM =
    {
        cmd_meander_0,
        cmd_meander_1,
        cmd_meander_2,
        cmd_meander_3,
        cmd_meander_4,
        cmd_meander_5,
        cmd_meander_6,
        cmd_meander_7,
        cmd_meander_8,
        cmd_meander_9,
        cmd_meander_10,
        cmd_meander_11,
        cmd_meander_12,
        cmd_meander_13,
        cmd_meander_14,
        cmd_meander_15,
    };

    for (uint8_t i = 0; i < (sizeof(cmd_meander)/sizeof(cmd_meander[0])); ++i)
    {
        enquecommand_P(static_cast<char*>(pgm_read_ptr(&cmd_meander[i])));
    }
    sprintf_P(cmd_buffer, PSTR("G1 X50 Y35 E%-.3f"), extr);
    enquecommand(cmd_buffer);
}

//! @brief Print square
//!
//! This function needs to be called 16 times for i from 0 to 15.
//!
//! @param cmd_buffer character buffer needed to format gcodes
//! @param i iteration
void lay1cal_square(char *cmd_buffer, uint8_t i)
{
    const float extr_short_segment = count_e(height, width, width);

    static const char fmt1[] PROGMEM = "G1 X%d Y%-.2f E%-.3f";
    static const char fmt2[] PROGMEM = "G1 Y%-.2f E%-.3f";
    sprintf_P(cmd_buffer, fmt1, 70, (35 - i*width * 2), extr);
    enquecommand(cmd_buffer);
    sprintf_P(cmd_buffer, fmt2, (35 - (2 * i + 1)*width), extr_short_segment);
    enquecommand(cmd_buffer);
    sprintf_P(cmd_buffer, fmt1, 50, (35 - (2 * i + 1)*width), extr);
    enquecommand(cmd_buffer);
    sprintf_P(cmd_buffer, fmt2, (35 - (i + 1)*width * 2), extr_short_segment);
    enquecommand(cmd_buffer);
}
