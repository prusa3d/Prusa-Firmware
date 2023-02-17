#pragma once
#include <inttypes.h>
#include <avr/pgmspace.h>

// Settings for filament load / unload from the LCD menu.
// This is for Prusa MK3-style extruders. Customize for your hardware.
#define MMU2_FILAMENTCHANGE_EJECT_FEED 80.0

#define NOZZLE_PARK_XY_FEEDRATE 50
#define NOZZLE_PARK_Z_FEEDRATE 15

// Nominal distance from the extruder gear to the nozzle tip is 87mm
// However, some slipping may occur and we need separate distances for
// LoadToNozzle and ToolChange.
// - +5mm seemed good for LoadToNozzle,
// - but too much (made blobs) for a ToolChange
static constexpr float MMU2_LOAD_TO_NOZZLE_LENGTH = 87.0F + 5.0F;

// As discussed with our PrusaSlicer profile specialist
// - ToolChange shall not try to push filament into the very tip of the nozzle
// to have some space for additional G-code to tune the extruded filament length
// in the profile
// Beware - this value is used to initialize the MMU logic layer - it will be sent to the MMU upon line up (written into its 8bit register 0x0b)
// However - in the G-code we can get a request to set the extra load distance at runtime to something else (M708 A0xb Xsomething).
// The printer intercepts such a call and sets its extra load distance to match the new value as well.
static constexpr float MMU2_FILAMENT_SENSOR_POSITION = 16; // mm
static constexpr float MMU2_LOAD_DISTANCE_PAST_GEARS = 5; // mm
static constexpr uint8_t MMU2_TOOL_CHANGE_LOAD_LENGTH = static_cast<uint8_t>(MMU2_FILAMENT_SENSOR_POSITION + MMU2_LOAD_DISTANCE_PAST_GEARS); // mm

static constexpr float MMU2_EXTRUDER_PTFE_LENGTH = 50.f; // mm
static constexpr float MMU2_EXTRUDER_HEATBREAK_LENGTH  = 17.7f; // mm

static constexpr float MMU2_LOAD_TO_NOZZLE_FEED_RATE = 20.0F; // mm/s
static constexpr float MMU2_UNLOAD_TO_FINDA_FEED_RATE = 120.0F; // mm/s

static constexpr float MMU2_VERIFY_LOAD_TO_NOZZLE_FEED_RATE = 50.0F; // mm/s
static constexpr float MMU2_VERIFY_LOAD_TO_NOZZLE_TWEAK = -5.F; // mm used to shorten/lenghten (negative number -> shorten) the distange of verify load to nozzle

// The first the MMU does is initialise its axis. Meanwhile the E-motor will unload 20mm of filament in approx. 1 second.
static constexpr float MMU2_RETRY_UNLOAD_TO_FINDA_LENGTH = 80.0f; // mm
static constexpr float MMU2_RETRY_UNLOAD_TO_FINDA_FEED_RATE = 80.0f; // mm/s

static constexpr uint8_t MMU2_NO_TOOL = 99;
static constexpr uint32_t MMU_BAUD = 115200;

struct E_Step {
    float extrude;       ///< extrude distance in mm
    float feedRate; ///< feed rate in mm/s
};

static constexpr E_Step ramming_sequence[] PROGMEM = {
    { 0.2816F,  1339.0F / 60.F},
    { 0.3051F,  1451.0F / 60.F},
    { 0.3453F,  1642.0F / 60.F},
    { 0.3990F,  1897.0F / 60.F},
    { 0.4761F,  2264.0F / 60.F},
    { 0.5767F,  2742.0F / 60.F},
    { 0.5691F,  3220.0F / 60.F},
    { 0.1081F,  3220.0F / 60.F},
    { 0.7644F,  3635.0F / 60.F},
    { 0.8248F,  3921.0F / 60.F},
    { 0.8483F,  4033.0F / 60.F},
    { -15.0F,   6000.0F / 60.F},
    { -24.5F,   1200.0F / 60.F},
    { -7.0F,    600.0F / 60.F},
    { -3.5F,    360.0F / 60.F},
    { 20.0F,    454.0F / 60.F},
    { -20.0F,   303.0F / 60.F},
    { -35.0F,   2000.0F / 60.F},
};

static constexpr E_Step load_to_nozzle_sequence[] PROGMEM = {
    { MMU2_EXTRUDER_PTFE_LENGTH,       810.0F / 60.F}, // feed rate = 13.5mm/s - Load fast while not at heatbreak
    { MMU2_EXTRUDER_HEATBREAK_LENGTH,  198.0F / 60.F}, // feed rate = 3.3mm/s  - Load slower once filament reaches heatbreak
};
