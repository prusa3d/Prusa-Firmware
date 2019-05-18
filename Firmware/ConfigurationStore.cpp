//! @file

#include "Marlin.h"
#include "planner.h"
#include "temperature.h"
#include "ultralcd.h"
#include "ConfigurationStore.h"
#include "Configuration_prusa.h"

#ifdef MESH_BED_LEVELING
#include "mesh_bed_leveling.h"
#endif

#ifdef TMC2130
#include "tmc2130.h"
#endif


M500_conf cs;

//! @brief Write data to EEPROM
//! @param pos destination in EEPROM, 0 is start
//! @param value value to be written
//! @param size size of type pointed by value
//! @param name name of variable written, used only for debug input if DEBUG_EEPROM_WRITE defined
//! @retval true success
//! @retval false failed
#ifdef DEBUG_EEPROM_WRITE
static bool EEPROM_writeData(uint8_t* pos, uint8_t* value, uint8_t size, const char* name)
#else //DEBUG_EEPROM_WRITE
static bool EEPROM_writeData(uint8_t* pos, uint8_t* value, uint8_t size, const char*)
#endif //DEBUG_EEPROM_WRITE
{
#ifdef DEBUG_EEPROM_WRITE
	printf_P(PSTR("EEPROM_WRITE_VAR addr=0x%04x size=0x%02hhx name=%s\n"), pos, size, name);
#endif //DEBUG_EEPROM_WRITE
	while (size--)
	{

        eeprom_update_byte(pos, *value);
        if (eeprom_read_byte(pos) != *value) {
            SERIAL_ECHOLNPGM("EEPROM Error");
            return false;
        }

		pos++;
		value++;
	}
    return true;
}

#ifdef DEBUG_EEPROM_READ
static void EEPROM_readData(uint8_t* pos, uint8_t* value, uint8_t size, const char* name)
#else //DEBUG_EEPROM_READ
static void EEPROM_readData(uint8_t* pos, uint8_t* value, uint8_t size, const char*)
#endif //DEBUG_EEPROM_READ
{
#ifdef DEBUG_EEPROM_READ
	printf_P(PSTR("EEPROM_READ_VAR addr=0x%04x size=0x%02hhx name=%s\n"), pos, size, name);
#endif //DEBUG_EEPROM_READ
    while(size--)
    {
        *value = eeprom_read_byte(pos);
        pos++;
        value++;
    }
}

#define EEPROM_VERSION "V2"

#ifdef EEPROM_SETTINGS
void Config_StoreSettings()
{
  strcpy(cs.version,"000"); //!< invalidate data first @TODO use erase to save one erase cycle
  
  if (EEPROM_writeData(reinterpret_cast<uint8_t*>(EEPROM_M500_base),reinterpret_cast<uint8_t*>(&cs),sizeof(cs),0), "cs, invalid version")
  {
      strcpy(cs.version,EEPROM_VERSION); //!< validate data if write succeed
      EEPROM_writeData(reinterpret_cast<uint8_t*>(EEPROM_M500_base->version), reinterpret_cast<uint8_t*>(cs.version), sizeof(cs.version), "cs.version valid");
  }

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
        "%SUStep resolution: \n%S M350 X%d Y%d Z%d E%d\n"
		"%SMaximum feedrates - normal (mm/s):\n%S  M203 X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SMaximum feedrates - stealth (mm/s):\n%S  M203 X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SMaximum acceleration - normal (mm/s2):\n%S  M201 X%lu Y%lu Z%lu E%lu\n"
		"%SMaximum acceleration - stealth (mm/s2):\n%S  M201 X%lu Y%lu Z%lu E%lu\n"
		"%SAcceleration: S=acceleration, T=retract acceleration\n%S  M204 S%.2f T%.2f\n"
		"%SAdvanced variables: S=Min feedrate (mm/s), T=Min travel feedrate (mm/s), B=minimum segment time (ms), X=maximum XY jerk (mm/s),  Z=maximum Z jerk (mm/s),  E=maximum E jerk (mm/s)\n%S  M205 S%.2f T%.2f B%.2f X%.2f Y%.2f Z%.2f E%.2f\n"
		"%SHome offset (mm):\n%S  M206 X%.2f Y%.2f Z%.2f\n"
		),
		echomagic, echomagic, cs.axis_steps_per_unit[X_AXIS], cs.axis_steps_per_unit[Y_AXIS], cs.axis_steps_per_unit[Z_AXIS], cs.axis_steps_per_unit[E_AXIS],
		echomagic, echomagic, cs.axis_ustep_resolution[X_AXIS], cs.axis_ustep_resolution[Y_AXIS], cs.axis_ustep_resolution[Z_AXIS], cs.axis_ustep_resolution[E_AXIS],
		echomagic, echomagic, cs.max_feedrate_normal[X_AXIS], cs.max_feedrate_normal[Y_AXIS], cs.max_feedrate_normal[Z_AXIS], cs.max_feedrate_normal[E_AXIS],
		echomagic, echomagic, cs.max_feedrate_silent[X_AXIS], cs.max_feedrate_silent[Y_AXIS], cs.max_feedrate_silent[Z_AXIS], cs.max_feedrate_silent[E_AXIS],
		echomagic, echomagic, cs.max_acceleration_units_per_sq_second_normal[X_AXIS], cs.max_acceleration_units_per_sq_second_normal[Y_AXIS], cs.max_acceleration_units_per_sq_second_normal[Z_AXIS], cs.max_acceleration_units_per_sq_second_normal[E_AXIS],
		echomagic, echomagic, cs.max_acceleration_units_per_sq_second_silent[X_AXIS], cs.max_acceleration_units_per_sq_second_silent[Y_AXIS], cs.max_acceleration_units_per_sq_second_silent[Z_AXIS], cs.max_acceleration_units_per_sq_second_silent[E_AXIS],
		echomagic, echomagic, cs.acceleration, cs.retract_acceleration,
		echomagic, echomagic, cs.minimumfeedrate, cs.mintravelfeedrate, cs.minsegmenttime, cs.max_jerk[X_AXIS], cs.max_jerk[Y_AXIS], cs.max_jerk[Z_AXIS], cs.max_jerk[E_AXIS],
		echomagic, echomagic, cs.add_homing[X_AXIS], cs.add_homing[Y_AXIS], cs.add_homing[Z_AXIS]
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

static_assert (sizeof(M500_conf) == 192, "sizeof(M500_conf) has changed, ensure that EEPROM_VERSION has been incremented, "
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
};

//! @brief Read M500 configuration
//! @retval true Succeeded. Stored settings retrieved or default settings retrieved in case EEPROM has been erased.
//! @retval false Failed. Default settings has been retrieved, because of older version or corrupted data.
bool Config_RetrieveSettings()
{
	bool previous_settings_retrieved = true;
    char ver[4]=EEPROM_VERSION;
    EEPROM_readData(reinterpret_cast<uint8_t*>(EEPROM_M500_base->version), reinterpret_cast<uint8_t*>(cs.version), sizeof(cs.version), "cs.version"); //read stored version
    //  SERIAL_ECHOLN("Version: [" << ver << "] Stored version: [" << cs.version << "]");
    if (strncmp(ver,cs.version,3) == 0)  // version number match
    {

        EEPROM_readData(reinterpret_cast<uint8_t*>(EEPROM_M500_base), reinterpret_cast<uint8_t*>(&cs), sizeof(cs), "cs");

        
		if (cs.max_jerk[X_AXIS] > DEFAULT_XJERK) cs.max_jerk[X_AXIS] = DEFAULT_XJERK;
		if (cs.max_jerk[Y_AXIS] > DEFAULT_YJERK) cs.max_jerk[Y_AXIS] = DEFAULT_YJERK;
        calculate_extruder_multipliers();

		//if max_feedrate_silent and max_acceleration_units_per_sq_second_silent were never stored to eeprom, use default values:
        for (uint8_t i = 0; i < (sizeof(cs.max_feedrate_silent)/sizeof(cs.max_feedrate_silent[0])); ++i)
        {
            const uint32_t erased = 0xffffffff;
            bool initialized = false;

            for(uint8_t j = 0; j < sizeof(float); ++j)
            {
                if(0xff != reinterpret_cast<uint8_t*>(&(cs.max_feedrate_silent[i]))[j]) initialized = true;
            }
            if (!initialized) memcpy_P(&cs.max_feedrate_silent[i],&default_conf.max_feedrate_silent[i], sizeof(cs.max_feedrate_silent[i]));
            if (erased == cs.max_acceleration_units_per_sq_second_silent[i]) {
                memcpy_P(&cs.max_acceleration_units_per_sq_second_silent[i],&default_conf.max_acceleration_units_per_sq_second_silent[i],sizeof(cs.max_acceleration_units_per_sq_second_silent[i]));
            }
        }

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
        SERIAL_ECHO_START;
        SERIAL_ECHOLNPGM("Stored settings retrieved");
    }
    else
    {
        Config_ResetDefault();
		//Return false to inform user that eeprom version was changed and firmware is using default hardcoded settings now.
		//In case that storing to eeprom was not used yet, do not inform user that hardcoded settings are used.
		if (eeprom_read_byte(reinterpret_cast<uint8_t*>(&(EEPROM_M500_base->version[0]))) != 0xFF ||
			eeprom_read_byte(reinterpret_cast<uint8_t*>(&(EEPROM_M500_base->version[1]))) != 0xFF ||
			eeprom_read_byte(reinterpret_cast<uint8_t*>(&(EEPROM_M500_base->version[2]))) != 0xFF)
		{
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
