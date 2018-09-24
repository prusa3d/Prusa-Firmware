#include "Marlin.h"
#include "planner.h"
#include "temperature.h"
#include "ultralcd.h"
#include "ConfigurationStore.h"
#include "Configuration_prusa.h"

#ifdef MESH_BED_LEVELING
#include "mesh_bed_leveling.h"
#endif

M500_conf cs;

#ifdef DEBUG_EEPROM_WRITE
#define EEPROM_WRITE_VAR(pos, value) _EEPROM_writeData(pos, (uint8_t*)&value, sizeof(value), #value)
#else //DEBUG_EEPROM_WRITE
#define EEPROM_WRITE_VAR(pos, value) _EEPROM_writeData(pos, (uint8_t*)&value, sizeof(value), 0)
#endif //DEBUG_EEPROM_WRITE
void _EEPROM_writeData(int &pos, uint8_t* value, uint8_t size, char* name)
{
#ifdef DEBUG_EEPROM_WRITE
	printf_P(PSTR("EEPROM_WRITE_VAR addr=0x%04x size=0x%02hhx name=%s\n"), pos, size, name);
#endif //DEBUG_EEPROM_WRITE
	while (size--) {
		uint8_t * const p = (uint8_t * const)pos;
		uint8_t v = *value;
		// EEPROM has only ~100,000 write cycles,
		// so only write bytes that have changed!
		if (v != eeprom_read_byte(p)) {
			eeprom_write_byte(p, v);
			if (eeprom_read_byte(p) != v) {
				SERIAL_ECHOLNPGM("EEPROM Error");
				return;
			}
		}
		pos++;
		value++;
	};

}

#ifdef DEBUG_EEPROM_READ
#define EEPROM_READ_VAR(pos, value) _EEPROM_readData(pos, (uint8_t*)&value, sizeof(value), #value)
#else //DEBUG_EEPROM_READ
#define EEPROM_READ_VAR(pos, value) _EEPROM_readData(pos, (uint8_t*)&value, sizeof(value), 0)
#endif //DEBUG_EEPROM_READ
void _EEPROM_readData(int &pos, uint8_t* value, uint8_t size, char* name)
{
#ifdef DEBUG_EEPROM_READ
	printf_P(PSTR("EEPROM_READ_VAR addr=0x%04x size=0x%02hhx name=%s\n"), pos, size, name);
#endif //DEBUG_EEPROM_READ
    do
    {
        *value = eeprom_read_byte((unsigned char*)pos);
        pos++;
        value++;
    }while(--size);
}

//======================================================================================
#define EEPROM_OFFSET 20
// IMPORTANT:  Whenever there are changes made to the variables stored in EEPROM
// in the functions below, also increment the version number. This makes sure that
// the default values are used whenever there is a change to the data, to prevent
// wrong data being written to the variables.
// ALSO:  always make sure the variables in the Store and retrieve sections are in the same order.

#define EEPROM_VERSION "V2"

#ifdef EEPROM_SETTINGS
void Config_StoreSettings(uint16_t offset) 
{
  int i = offset;
  strcpy(cs.version,"000"); //!< invalidate data first @TODO use erase to save one erase cycle
  
  _EEPROM_writeData(i,reinterpret_cast<uint8_t*>(&cs),sizeof(cs),0);
  strcpy(cs.version,EEPROM_VERSION); // // validate data
  i = offset;
  EEPROM_WRITE_VAR(i,cs.version); // validate data
  SERIAL_ECHO_START;
  SERIAL_ECHOLNPGM("Settings Stored");
}
#endif //EEPROM_SETTINGS


#ifndef DISABLE_M503
void Config_PrintSettings(uint8_t level)
{  // Always have this function, even with EEPROM_SETTINGS disabled, the current values will be shown
#ifdef TMC2130
	printf_P(PSTR(
		"%SSteps per unit:\n%S  M92 X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SMaximum feedrates - normal (mm/s):\n%S  M203 X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SMaximum feedrates - stealth (mm/s):\n%S  M203 X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SMaximum acceleration - normal (mm/s2):\n%S  M201 X%lu Y%lu Z%lu E%lu\n"
		"%SMaximum acceleration - stealth (mm/s2):\n%S  M201 X%lu Y%lu Z%lu E%lu\n"
		"%SAcceleration: S=acceleration, T=retract acceleration\n%S  M204 S%.2f T%.2f\n"
		"%SAdvanced variables: S=Min feedrate (mm/s), T=Min travel feedrate (mm/s), B=minimum segment time (ms), X=maximum XY jerk (mm/s),  Z=maximum Z jerk (mm/s),  E=maximum E jerk (mm/s)\n%S  M205 S%.2f T%.2f B%.2f X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SHome offset (mm):\n%S  M206 X%.2f Y%.2f Z%.2f\n"
		),
		echomagic, echomagic, cs.axis_steps_per_unit[X_AXIS], cs.axis_steps_per_unit[Y_AXIS], cs.axis_steps_per_unit[Z_AXIS], cs.axis_steps_per_unit[E_AXIS],
		echomagic, echomagic, cs.max_feedrate_normal[X_AXIS], cs.max_feedrate_normal[Y_AXIS], cs.max_feedrate_normal[Z_AXIS], cs.max_feedrate_normal[E_AXIS],
		echomagic, echomagic, max_feedrate_silent[X_AXIS], max_feedrate_silent[Y_AXIS], max_feedrate_silent[Z_AXIS], max_feedrate_silent[E_AXIS],
		echomagic, echomagic, cs.max_acceleration_units_per_sq_second_normal[X_AXIS], cs.max_acceleration_units_per_sq_second_normal[Y_AXIS], cs.max_acceleration_units_per_sq_second_normal[Z_AXIS], cs.max_acceleration_units_per_sq_second_normal[E_AXIS],
		echomagic, echomagic, max_acceleration_units_per_sq_second_silent[X_AXIS], max_acceleration_units_per_sq_second_silent[Y_AXIS], max_acceleration_units_per_sq_second_silent[Z_AXIS], max_acceleration_units_per_sq_second_silent[E_AXIS],
		echomagic, echomagic, cs.acceleration, cs.retract_acceleration,
		echomagic, echomagic, cs.minimumfeedrate, cs.mintravelfeedrate, minsegmenttime, max_jerk[X_AXIS], max_jerk[Y_AXIS], max_jerk[Z_AXIS], max_jerk[E_AXIS],
		echomagic, echomagic, add_homing[X_AXIS], add_homing[Y_AXIS], add_homing[Z_AXIS]
#else //TMC2130
	printf_P(PSTR(
		"%SSteps per unit:\n%S  M92 X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SMaximum feedrates (mm/s):\n%S  M203 X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SMaximum acceleration (mm/s2):\n%S  M201 X%lu Y%lu Z%lu E%lu\n"
		"%SAcceleration: S=acceleration, T=retract acceleration\n%S  M204 S%.2f T%.2f\n"
		"%SAdvanced variables: S=Min feedrate (mm/s), T=Min travel feedrate (mm/s), B=minimum segment time (ms), X=maximum XY jerk (mm/s),  Z=maximum Z jerk (mm/s),  E=maximum E jerk (mm/s)\n%S  M205 S%.2f T%.2f B%.2f X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SHome offset (mm):\n%S  M206 X%.2f Y%.2f Z%.2f\n"
		),
		echomagic, echomagic, cs.axis_steps_per_unit[X_AXIS], cs.axis_steps_per_unit[Y_AXIS], cs.axis_steps_per_unit[Z_AXIS], cs.axis_steps_per_unit[E_AXIS],
		echomagic, echomagic, max_feedrate[X_AXIS], max_feedrate[Y_AXIS], max_feedrate[Z_AXIS], max_feedrate[E_AXIS],
		echomagic, echomagic, max_acceleration_units_per_sq_second[X_AXIS], max_acceleration_units_per_sq_second[Y_AXIS], max_acceleration_units_per_sq_second[Z_AXIS], max_acceleration_units_per_sq_second[E_AXIS],
		echomagic, echomagic, cs.acceleration, cs.retract_acceleration,
		echomagic, echomagic, cs.minimumfeedrate, cs.mintravelfeedrate, minsegmenttime, max_jerk[X_AXIS], max_jerk[Y_AXIS], max_jerk[Z_AXIS], max_jerk[E_AXIS],
		echomagic, echomagic, add_homing[X_AXIS], add_homing[Y_AXIS], add_homing[Z_AXIS]
#endif //TMC2130
	);
#ifdef PIDTEMP
	printf_P(PSTR("%SPID settings:\n%S   M301 P%.2f I%.2f D%.2f\n"),
		echomagic, echomagic, Kp, unscalePID_i(Ki), unscalePID_d(Kd));
#endif
#ifdef PIDTEMPBED
	printf_P(PSTR("%SPID heatbed settings:\n%S   M304 P%.2f I%.2f D%.2f\n"),
		echomagic, echomagic, bedKp, unscalePID_i(bedKi), unscalePID_d(bedKd));
#endif
#ifdef FWRETRACT
	printf_P(PSTR(
		"%SRetract: S=Length (mm) F:Speed (mm/m) Z: ZLift (mm)\n%S   M207 S%.2f F%.2f Z%.2f\n"
		"%SRecover: S=Extra length (mm) F:Speed (mm/m)\n%S   M208 S%.2f F%.2f\n"
		"%SAuto-Retract: S=0 to disable, 1 to interpret extrude-only moves as retracts or recoveries\n%S   M209 S%d\n"
		),
		echomagic, echomagic, retract_length, retract_feedrate*60, retract_zlift,
		echomagic, echomagic, retract_recover_length, retract_recover_feedrate*60,
		echomagic, echomagic, (autoretract_enabled ? 1 : 0)
	);
#if EXTRUDERS > 1
	printf_P(PSTR("%SMulti-extruder settings:\n%S   Swap retract length (mm):    %.2f\n%S   Swap rec. addl. length (mm): %.2f\n"),
		echomagic, echomagic, retract_length_swap, echomagic, retract_recover_length_swap);
#endif
	if (volumetric_enabled) {
		printf_P(PSTR("%SFilament settings:\n%S   M200 D%.2f\n"),
			echomagic, echomagic, filament_size[0]);
#if EXTRUDERS > 1
		printf_P(PSTR("%S   M200 T1 D%.2f\n"),
			echomagic, echomagic, filament_size[1]);
#if EXTRUDERS > 2
		printf_P(PSTR("%S   M200 T1 D%.2f\n"),
			echomagic, echomagic, filament_size[2]);
#endif
#endif
    } else {
        printf_P(PSTR("%SFilament settings: Disabled\n"), echomagic);
    }
#endif
	if (level >= 10) {
#ifdef LIN_ADVANCE
		printf_P(PSTR("%SLinear advance settings:\n   M900 K%.2f   E/D = %.2f\n"),
			echomagic, extruder_advance_k, advance_ed_ratio);
#endif //LIN_ADVANCE
	}
}
#endif


#ifdef EEPROM_SETTINGS

static_assert (EXTRUDERS == 1, "ConfigurationStore M500_conf not implemented for more extruders.");
static_assert (NUM_AXIS == 4, "ConfigurationStore M500_conf not implemented for more axis.");
#ifdef ENABLE_AUTO_BED_LEVELING
static_assert (false, "zprobe_zoffset was not initialized in printers in field to -(Z_PROBE_OFFSET_FROM_EXTRUDER), so it contains"
        "0.0, if this is not acceptable, increment EEPROM_VERSION to force use default_conf");
#endif


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
    {DEFAULT_NOMINAL_FILAMENT_DIA},
    DEFAULT_MAX_FEEDRATE_SILENT,
    DEFAULT_MAX_ACCELERATION_SILENT,
};

static_assert (sizeof(M500_conf) == 188, "sizeof(M500_conf) has changed, ensure that version has been incremented, "
        "or if you added members in the end of struct, ensure that historically uninitialized values will be initialized");

//!
//! @retval true Stored or default settings retrieved
//! @retval false default settings retrieved, eeprom was erased.
bool Config_RetrieveSettings(uint16_t offset)
{
    int i=offset;
	bool previous_settings_retrieved = true;
    char ver[4]=EEPROM_VERSION;
    EEPROM_READ_VAR(i,cs.version); //read stored version
    //  SERIAL_ECHOLN("Version: [" << ver << "] Stored version: [" << cs.version << "]");
    if (strncmp(ver,cs.version,3) == 0)  // version number match
    {
        i=offset;

        EEPROM_READ_VAR(i,cs);

        
		if (max_jerk[X_AXIS] > DEFAULT_XJERK) max_jerk[X_AXIS] = DEFAULT_XJERK;
		if (max_jerk[Y_AXIS] > DEFAULT_YJERK) max_jerk[Y_AXIS] = DEFAULT_YJERK;
        calculate_extruder_multipliers();


#ifdef TMC2130
		for (uint8_t j = X_AXIS; j <= Y_AXIS; j++)
		{
			if (cs.max_feedrate_normal[j] > NORMAL_MAX_FEEDRATE_XY)
				cs.max_feedrate_normal[j] = NORMAL_MAX_FEEDRATE_XY;
			if (max_feedrate_silent[j] > SILENT_MAX_FEEDRATE_XY)
				max_feedrate_silent[j] = SILENT_MAX_FEEDRATE_XY;
			if (cs.max_acceleration_units_per_sq_second_normal[j] > NORMAL_MAX_ACCEL_XY)
				cs.max_acceleration_units_per_sq_second_normal[j] = NORMAL_MAX_ACCEL_XY;
			if (max_acceleration_units_per_sq_second_silent[j] > SILENT_MAX_ACCEL_XY)
				max_acceleration_units_per_sq_second_silent[j] = SILENT_MAX_ACCEL_XY;
		}
#endif //TMC2130

		reset_acceleration_rates();

		// Call updatePID (similar to when we have processed M301)
		updatePID();
        SERIAL_ECHO_START;
        SERIAL_ECHOLNPGM("Stored settings retrieved");
    }
    else
    {
        Config_ResetDefault();
		//Return false to inform user that eeprom version was changed and firmware is using default hardcoded settings now.
		//In case that storing to eeprom was not used yet, do not inform user that hardcoded settings are used.
		if (eeprom_read_byte((uint8_t *)offset) != 0xFF ||
			eeprom_read_byte((uint8_t *)offset + 1) != 0xFF ||
			eeprom_read_byte((uint8_t *)offset + 2) != 0xFF) {
			previous_settings_retrieved = false;
		}
    }
    #ifdef EEPROM_CHITCHAT
      Config_PrintSettings();
    #endif
	return previous_settings_retrieved;
}
#endif

void Config_ResetDefault()
{
    memcpy_P(&cs,&default_conf, sizeof(cs));

	// steps per sq second need to be updated to agree with the units per sq second
    reset_acceleration_rates();
    
#ifdef PIDTEMP
    updatePID();
#ifdef PID_ADD_EXTRUSION_RATE
    Kc = DEFAULT_Kc; //this is not stored by Config_StoreSettings
#endif//PID_ADD_EXTRUSION_RATE
#endif//PIDTEMP

	calculate_extruder_multipliers();

SERIAL_ECHO_START;
SERIAL_ECHOLNPGM("Hardcoded Default Settings Loaded");

}
