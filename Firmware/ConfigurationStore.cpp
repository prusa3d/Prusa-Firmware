//! @file

#include "Marlin.h"
#include "planner.h"
#include "temperature.h"
#include "ultralcd.h"
#include "ConfigurationStore.h"
#include "Configuration_var.h"

#ifdef MESH_BED_LEVELING
#include "mesh_bed_leveling.h"
#endif

#ifdef TMC2130
#include "tmc2130.h"
#endif

M500_conf cs;

#define EEPROM_VERSION "V2"


#ifndef DISABLE_M503
void Config_PrintSettings(uint8_t level)
{  // Always have this function, even with EEPROM_SETTINGS disabled, the current values will be shown
#ifdef TMC2130
  printf_P(PSTR(
    "%SSteps per unit:\n%S  M92 X%.2f Y%.2f Z%.2f E%.2f\n"
        "%SUStep resolution: \n%S M350 X%d Y%d Z%d E%d\n"
		"%SMaximum feedrates - normal (mm/s):\n%S  M203 X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SMaximum feedrates - stealth (mm/s):\n%S  M203 X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SMaximum acceleration - normal (mm/s2):\n%S  M201 X%lu Y%lu Z%lu E%lu\n"
		"%SMaximum acceleration - stealth (mm/s2):\n%S  M201 X%lu Y%lu Z%lu E%lu\n"
		"%SAcceleration: P=print, R=retract, T=travel\n%S  M204 P%.2f R%.2f T%.2f\n"
		"%SAdvanced variables: S=Min feedrate (mm/s), T=Min travel feedrate (mm/s), B=minimum segment time (ms), X=maximum XY jerk (mm/s),  Z=maximum Z jerk (mm/s),  E=maximum E jerk (mm/s)\n%S  M205 S%.2f T%.2f B%.2f X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SHome offset (mm):\n%S  M206 X%.2f Y%.2f Z%.2f\n"
		),
		echomagic, echomagic, cs.axis_steps_per_unit[X_AXIS], cs.axis_steps_per_unit[Y_AXIS], cs.axis_steps_per_unit[Z_AXIS], cs.axis_steps_per_unit[E_AXIS],
		echomagic, echomagic, cs.axis_ustep_resolution[X_AXIS], cs.axis_ustep_resolution[Y_AXIS], cs.axis_ustep_resolution[Z_AXIS], cs.axis_ustep_resolution[E_AXIS],
		echomagic, echomagic, cs.max_feedrate_normal[X_AXIS], cs.max_feedrate_normal[Y_AXIS], cs.max_feedrate_normal[Z_AXIS], cs.max_feedrate_normal[E_AXIS],
		echomagic, echomagic, cs.max_feedrate_silent[X_AXIS], cs.max_feedrate_silent[Y_AXIS], cs.max_feedrate_silent[Z_AXIS], cs.max_feedrate_silent[E_AXIS],
		echomagic, echomagic, cs.max_acceleration_units_per_sq_second_normal[X_AXIS], cs.max_acceleration_units_per_sq_second_normal[Y_AXIS], cs.max_acceleration_units_per_sq_second_normal[Z_AXIS], cs.max_acceleration_units_per_sq_second_normal[E_AXIS],
		echomagic, echomagic, cs.max_acceleration_units_per_sq_second_silent[X_AXIS], cs.max_acceleration_units_per_sq_second_silent[Y_AXIS], cs.max_acceleration_units_per_sq_second_silent[Z_AXIS], cs.max_acceleration_units_per_sq_second_silent[E_AXIS],
		echomagic, echomagic, cs.acceleration, cs.retract_acceleration, cs.travel_acceleration,
		echomagic, echomagic, cs.minimumfeedrate, cs.mintravelfeedrate, cs.minsegmenttime, cs.max_jerk[X_AXIS], cs.max_jerk[Y_AXIS], cs.max_jerk[Z_AXIS], cs.max_jerk[E_AXIS],
		echomagic, echomagic, cs.add_homing[X_AXIS], cs.add_homing[Y_AXIS], cs.add_homing[Z_AXIS]
#else //TMC2130
	printf_P(PSTR(
		"%SSteps per unit:\n%S  M92 X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SMaximum feedrates (mm/s):\n%S  M203 X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SMaximum acceleration (mm/s2):\n%S  M201 X%lu Y%lu Z%lu E%lu\n"
		"%SAcceleration: P=print, R=retract, T=travel\n%S  M204 P%.2f R%.2f T%.2f\n"
		"%SAdvanced variables: S=Min feedrate (mm/s), T=Min travel feedrate (mm/s), B=minimum segment time (ms), X=maximum XY jerk (mm/s),  Z=maximum Z jerk (mm/s),  E=maximum E jerk (mm/s)\n%S  M205 S%.2f T%.2f B%.2f X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SHome offset (mm):\n%S  M206 X%.2f Y%.2f Z%.2f\n"
		),
		echomagic, echomagic, cs.axis_steps_per_unit[X_AXIS], cs.axis_steps_per_unit[Y_AXIS], cs.axis_steps_per_unit[Z_AXIS], cs.axis_steps_per_unit[E_AXIS],
		echomagic, echomagic, max_feedrate[X_AXIS], max_feedrate[Y_AXIS], max_feedrate[Z_AXIS], max_feedrate[E_AXIS],
		echomagic, echomagic, max_acceleration_units_per_sq_second[X_AXIS], max_acceleration_units_per_sq_second[Y_AXIS], max_acceleration_units_per_sq_second[Z_AXIS], max_acceleration_units_per_sq_second[E_AXIS],
		echomagic, echomagic, cs.acceleration, cs.retract_acceleration, cs.travel_acceleration,
		echomagic, echomagic, cs.minimumfeedrate, cs.mintravelfeedrate, cs.minsegmenttime, cs.max_jerk[X_AXIS], cs.max_jerk[Y_AXIS], cs.max_jerk[Z_AXIS], cs.max_jerk[E_AXIS],
		echomagic, echomagic, cs.add_homing[X_AXIS], cs.add_homing[Y_AXIS], cs.add_homing[Z_AXIS]
#endif //TMC2130
  );
#ifdef PIDTEMP
  printf_P(PSTR("%SPID settings:\n%S   M301 P%.2f I%.2f D%.2f\n"),
    echomagic, echomagic, cs.Kp, unscalePID_i(cs.Ki), unscalePID_d(cs.Kd));
#endif
#ifdef PIDTEMPBED
  printf_P(PSTR("%SPID heatbed settings:\n%S   M304 P%.2f I%.2f D%.2f\n"),
    echomagic, echomagic, cs.bedKp, unscalePID_i(cs.bedKi), unscalePID_d(cs.bedKd));
#endif
#ifdef FWRETRACT
  printf_P(PSTR(
    "%SRetract: S=Length (mm) F:Speed (mm/m) Z: ZLift (mm)\n%S   M207 S%.2f F%.2f Z%.2f\n"
    "%SRecover: S=Extra length (mm) F:Speed (mm/m)\n%S   M208 S%.2f F%.2f\n"
    "%SAuto-Retract: S=0 to disable, 1 to interpret extrude-only moves as retracts or recoveries\n%S   M209 S%d\n"
    ),
    echomagic, echomagic, cs.retract_length, cs.retract_feedrate*60, cs.retract_zlift,
    echomagic, echomagic, cs.retract_recover_length, cs.retract_recover_feedrate*60,
    echomagic, echomagic, (cs.autoretract_enabled ? 1 : 0)
  );
#if EXTRUDERS > 1
  printf_P(PSTR("%SMulti-extruder settings:\n%S   Swap retract length (mm):    %.2f\n%S   Swap rec. addl. length (mm): %.2f\n"),
    echomagic, echomagic, retract_length_swap, echomagic, retract_recover_length_swap);
#endif
  if (cs.volumetric_enabled) {
    printf_P(PSTR("%SFilament settings:\n%S   M200 D%.2f\n"),
      echomagic, echomagic, cs.filament_size[0]);
#if EXTRUDERS > 1
    printf_P(PSTR("%S   M200 T1 D%.2f\n"),
      echomagic, echomagic, cs.filament_size[1]);
#if EXTRUDERS > 2
    printf_P(PSTR("%S   M200 T1 D%.2f\n"),
      echomagic, echomagic, cs.filament_size[2]);
#endif
#endif
    } else {
        printf_P(PSTR("%SFilament settings: Disabled\n"), echomagic);
    }
#endif
  if (level >= 10) {
#ifdef LIN_ADVANCE
    printf_P(PSTR("%SLinear advance settings:%S   M900 K%.2f\n"),
                 echomagic, echomagic, extruder_advance_K);
#endif //LIN_ADVANCE
  }
    // Arc Interpolation Settings
    printf_P(PSTR(
        "%SArc Settings: P:Max length(mm) S:Min length (mm) N:Corrections R:Min segments F:Segments/sec.\n%S  M214 P%.2f S%.2f N%d R%d F%d\n"),
        echomagic, echomagic, cs.mm_per_arc_segment, cs.min_mm_per_arc_segment, cs.n_arc_correction, cs.min_arc_segments, cs.arc_segments_per_sec);
#ifdef TEMP_MODEL
    temp_model_report_settings();
#endif
}
#endif


#ifdef EEPROM_SETTINGS

static_assert (EXTRUDERS == 1, "ConfigurationStore M500_conf not implemented for more extruders, fix filament_size array size.");
static_assert (NUM_AXIS == 4, "ConfigurationStore M500_conf not implemented for more axis."
        "Fix axis_steps_per_unit max_feedrate_normal max_acceleration_units_per_sq_second_normal max_jerk max_feedrate_silent"
        " max_acceleration_units_per_sq_second_silent array size.");
#ifdef ENABLE_AUTO_BED_LEVELING
static_assert (false, "zprobe_zoffset was not initialized in printers in field to -(Z_PROBE_OFFSET_FROM_EXTRUDER), so it contains"
        "0.0, if this is not acceptable, increment EEPROM_VERSION to force use default_conf");
#endif

static_assert (sizeof(M500_conf) == 209, "sizeof(M500_conf) has changed, ensure that EEPROM_VERSION has been incremented, "
        "or if you added members in the end of struct, ensure that historically uninitialized values will be initialized."
        "If this is caused by change to more then 8bit processor, decide whether make this struct packed to save EEPROM,"
        "leave as it is to keep fast code, or reorder struct members to pack more tightly.");

static const M500_conf default_conf PROGMEM =
{
    EEPROM_VERSION,
    DEFAULT_AXIS_STEPS_PER_UNIT,
    DEFAULT_MAX_FEEDRATE,
    DEFAULT_MAX_ACCELERATION,
    DEFAULT_ACCELERATION,
    DEFAULT_RETRACT_ACCELERATION,
    DEFAULT_MINIMUMFEEDRATE,
    DEFAULT_MINTRAVELFEEDRATE,
    DEFAULT_MINSEGMENTTIME,
    {DEFAULT_XJERK, DEFAULT_YJERK, DEFAULT_ZJERK, DEFAULT_EJERK},
    {0,0,0},
    -(Z_PROBE_OFFSET_FROM_EXTRUDER),
    DEFAULT_Kp,
    DEFAULT_Ki*PID_dT,
    DEFAULT_Kd/PID_dT,
    DEFAULT_bedKp,
    DEFAULT_bedKi*PID_dT,
    DEFAULT_bedKd/PID_dT,
    0,
    false,
    RETRACT_LENGTH,
    RETRACT_FEEDRATE,
    RETRACT_ZLIFT,
    RETRACT_RECOVER_LENGTH,
    RETRACT_RECOVER_FEEDRATE,
    false,
    {DEFAULT_NOMINAL_FILAMENT_DIA,
#if EXTRUDERS > 1
    DEFAULT_NOMINAL_FILAMENT_DIA,
#if EXTRUDERS > 2
    DEFAULT_NOMINAL_FILAMENT_DIA,
#endif
#endif
    },
    DEFAULT_MAX_FEEDRATE_SILENT,
    DEFAULT_MAX_ACCELERATION_SILENT,
#ifdef TMC2130
    { TMC2130_USTEPS_XY, TMC2130_USTEPS_XY, TMC2130_USTEPS_Z, TMC2130_USTEPS_E },
#else // TMC2130
    {16,16,16,16},
#endif
    DEFAULT_TRAVEL_ACCELERATION,
    DEFAULT_MM_PER_ARC_SEGMENT,
    DEFAULT_MIN_MM_PER_ARC_SEGMENT,
    DEFAULT_N_ARC_CORRECTION,
    DEFAULT_MIN_ARC_SEGMENTS,
    DEFAULT_ARC_SEGMENTS_PER_SEC
};


void Config_StoreSettings()
{
  strcpy_P(cs.version, default_conf.version);
  eeprom_update_block(reinterpret_cast<uint8_t*>(&cs), reinterpret_cast<uint8_t*>(EEPROM_M500_base), sizeof(cs));
#ifdef TEMP_MODEL
  temp_model_save_settings();
#endif

  SERIAL_ECHO_START;
  SERIAL_ECHOLNPGM("Settings Stored");
}


//! @brief Read M500 configuration
//! @retval true Succeeded. Stored settings retrieved or default settings retrieved in case EEPROM cs was empty.
//! @retval false Failed. Default settings has been retrieved, because of version mismatch
bool Config_RetrieveSettings()
{
    eeprom_read_block(reinterpret_cast<uint8_t*>(cs.version), reinterpret_cast<uint8_t*>(EEPROM_M500_base->version), sizeof(cs.version));
    //  SERIAL_ECHOLN("Version: [" << ver << "] Stored version: [" << cs.version << "]");
    if (strncmp_P(cs.version, default_conf.version, sizeof(EEPROM_VERSION)) == 0)  // version number match
    {
        // Initialize arc interpolation settings in eeprom if they are not already
        eeprom_init_default_float(&EEPROM_M500_base->mm_per_arc_segment, pgm_read_float(&default_conf.mm_per_arc_segment));
        eeprom_init_default_float(&EEPROM_M500_base->min_mm_per_arc_segment, pgm_read_float(&default_conf.min_mm_per_arc_segment));
        eeprom_init_default_byte(&EEPROM_M500_base->n_arc_correction, pgm_read_byte(&default_conf.n_arc_correction));
        eeprom_init_default_word(&EEPROM_M500_base->min_arc_segments, pgm_read_word(&default_conf.min_arc_segments));
        eeprom_init_default_word(&EEPROM_M500_base->arc_segments_per_sec, pgm_read_word(&default_conf.arc_segments_per_sec));
        
        // Initialize the travel_acceleration in eeprom if not already
        eeprom_init_default_float(&EEPROM_M500_base->travel_acceleration, pgm_read_float(&default_conf.travel_acceleration));

        // Initialize the max_feedrate_silent and max_acceleration_units_per_sq_second_silent in eeprom if not already
        eeprom_init_default_block(&EEPROM_M500_base->max_feedrate_silent, sizeof(EEPROM_M500_base->max_feedrate_silent), default_conf.max_feedrate_silent);
        eeprom_init_default_block(&EEPROM_M500_base->max_acceleration_units_per_sq_second_silent, sizeof(EEPROM_M500_base->max_acceleration_units_per_sq_second_silent), default_conf.max_acceleration_units_per_sq_second_silent);

        // load the CS to RAM
        eeprom_read_block(reinterpret_cast<uint8_t*>(&cs), reinterpret_cast<uint8_t*>(EEPROM_M500_base), sizeof(cs));
        calculate_extruder_multipliers();

#ifdef TMC2130
    for (uint8_t j = X_AXIS; j <= Y_AXIS; j++)
    {
      if (cs.max_feedrate_normal[j] > NORMAL_MAX_FEEDRATE_XY)
        cs.max_feedrate_normal[j] = NORMAL_MAX_FEEDRATE_XY;
      if (cs.max_feedrate_silent[j] > SILENT_MAX_FEEDRATE_XY)
        cs.max_feedrate_silent[j] = SILENT_MAX_FEEDRATE_XY;
      if (cs.max_acceleration_units_per_sq_second_normal[j] > NORMAL_MAX_ACCEL_XY)
        cs.max_acceleration_units_per_sq_second_normal[j] = NORMAL_MAX_ACCEL_XY;
      if (cs.max_acceleration_units_per_sq_second_silent[j] > SILENT_MAX_ACCEL_XY)
        cs.max_acceleration_units_per_sq_second_silent[j] = SILENT_MAX_ACCEL_XY;
    }
        
    if(cs.axis_ustep_resolution[X_AXIS] == 0xff){ cs.axis_ustep_resolution[X_AXIS] = TMC2130_USTEPS_XY; }
    if(cs.axis_ustep_resolution[Y_AXIS] == 0xff){ cs.axis_ustep_resolution[Y_AXIS] = TMC2130_USTEPS_XY; }
    if(cs.axis_ustep_resolution[Z_AXIS] == 0xff){ cs.axis_ustep_resolution[Z_AXIS] = TMC2130_USTEPS_Z; }
    if(cs.axis_ustep_resolution[E_AXIS] == 0xff){ cs.axis_ustep_resolution[E_AXIS] = TMC2130_USTEPS_E; }

    tmc2130_set_res(X_AXIS, cs.axis_ustep_resolution[X_AXIS]);
    tmc2130_set_res(Y_AXIS, cs.axis_ustep_resolution[Y_AXIS]);
    tmc2130_set_res(Z_AXIS, cs.axis_ustep_resolution[Z_AXIS]);
    tmc2130_set_res(E_AXIS, cs.axis_ustep_resolution[E_AXIS]);
#endif //TMC2130

		reset_acceleration_rates();

    // Call updatePID (similar to when we have processed M301)
    updatePID();
#ifdef TEMP_MODEL
    temp_model_load_settings();
#endif

        SERIAL_ECHO_START;
        SERIAL_ECHOLNPGM("Stored settings retrieved");
    }
    else
    {
        Config_ResetDefault();
        //Return false to inform user that eeprom version was changed and firmware is using default hardcoded settings now.
        //In case that storing to eeprom was not used yet, do not inform user that hardcoded settings are used.
        if (eeprom_is_initialized_block(EEPROM_M500_base->version, sizeof(EEPROM_M500_base->version))) {
            return false;
        }
    }
    return true;
}
#endif

void Config_ResetDefault()
{
    memcpy_P(&cs,&default_conf, sizeof(cs));

  // steps per sq second need to be updated to agree with the units per sq second
    reset_acceleration_rates();
    
#ifdef PIDTEMP
    updatePID();
#endif//PIDTEMP
#ifdef TEMP_MODEL
    temp_model_reset_settings();
#endif

  calculate_extruder_multipliers();

SERIAL_ECHO_START;
SERIAL_ECHOLNPGM("Hardcoded Default Settings Loaded");

}

