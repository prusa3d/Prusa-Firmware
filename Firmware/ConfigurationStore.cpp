#include "Marlin.h"
#include "planner.h"
#include "temperature.h"
#include "ultralcd.h"
#include "ConfigurationStore.h"
#include "Configuration_prusa.h"

#ifdef MESH_BED_LEVELING
#include "mesh_bed_leveling.h"
#endif

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
// IMPORTANT:  Whenever there are changes made to the variables stored in EEPROM
// in the functions below, also increment the version number and update EEPROM_M500_SIZE. This makes sure that
// the default values are used whenever there is a change to the data, to prevent
// wrong data being written to the variables.
// ALSO:  always make sure the variables in the Store and retrieve sections are in the same order.

#define EEPROM_VERSION "V2"

#ifdef EEPROM_SETTINGS
void Config_StoreSettings(uint16_t offset) 
{
  char ver[4]= "000";
  int i = offset;
  EEPROM_WRITE_VAR(i,ver); // invalidate data first 
  EEPROM_WRITE_VAR(i,axis_steps_per_unit);
  EEPROM_WRITE_VAR(i,max_feedrate_normal);
  EEPROM_WRITE_VAR(i,max_acceleration_units_per_sq_second_normal);
  EEPROM_WRITE_VAR(i,acceleration);
  EEPROM_WRITE_VAR(i,retract_acceleration);
  EEPROM_WRITE_VAR(i,minimumfeedrate);
  EEPROM_WRITE_VAR(i,mintravelfeedrate);
  EEPROM_WRITE_VAR(i,minsegmenttime);
  EEPROM_WRITE_VAR(i,max_jerk[X_AXIS]);
  EEPROM_WRITE_VAR(i,max_jerk[Y_AXIS]);
  EEPROM_WRITE_VAR(i,max_jerk[Z_AXIS]);
  EEPROM_WRITE_VAR(i,max_jerk[E_AXIS]);
  EEPROM_WRITE_VAR(i,add_homing);
/*  EEPROM_WRITE_VAR(i,plaPreheatHotendTemp);
  EEPROM_WRITE_VAR(i,plaPreheatHPBTemp);
  EEPROM_WRITE_VAR(i,plaPreheatFanSpeed);
  EEPROM_WRITE_VAR(i,absPreheatHotendTemp);
  EEPROM_WRITE_VAR(i,absPreheatHPBTemp);
  EEPROM_WRITE_VAR(i,absPreheatFanSpeed);
*/
  
  EEPROM_WRITE_VAR(i,zprobe_zoffset);
  #ifdef PIDTEMP
    EEPROM_WRITE_VAR(i,Kp);
    EEPROM_WRITE_VAR(i,Ki);
    EEPROM_WRITE_VAR(i,Kd);
  #else
		float dummy = 3000.0f;
    EEPROM_WRITE_VAR(i,dummy);
		dummy = 0.0f;
    EEPROM_WRITE_VAR(i,dummy);
    EEPROM_WRITE_VAR(i,dummy);
  #endif
  #ifdef PIDTEMPBED
	EEPROM_WRITE_VAR(i, bedKp);
	EEPROM_WRITE_VAR(i, bedKi);
	EEPROM_WRITE_VAR(i, bedKd);
  #endif

  int lcd_contrast = 0;
  EEPROM_WRITE_VAR(i,lcd_contrast);

  #ifdef FWRETRACT
  EEPROM_WRITE_VAR(i,autoretract_enabled);
  EEPROM_WRITE_VAR(i,retract_length);
  #if EXTRUDERS > 1
  EEPROM_WRITE_VAR(i,retract_length_swap);
  #endif
  EEPROM_WRITE_VAR(i,retract_feedrate);
  EEPROM_WRITE_VAR(i,retract_zlift);
  EEPROM_WRITE_VAR(i,retract_recover_length);
  #if EXTRUDERS > 1
  EEPROM_WRITE_VAR(i,retract_recover_length_swap);
  #endif
  EEPROM_WRITE_VAR(i,retract_recover_feedrate);
  #endif

  // Save filament sizes
  EEPROM_WRITE_VAR(i, volumetric_enabled);
  EEPROM_WRITE_VAR(i, filament_size[0]);
  #if EXTRUDERS > 1
  EEPROM_WRITE_VAR(i, filament_size[1]);
  #if EXTRUDERS > 2
  EEPROM_WRITE_VAR(i, filament_size[2]);
  #endif
  #endif



  EEPROM_WRITE_VAR(i,max_feedrate_silent);
  EEPROM_WRITE_VAR(i,max_acceleration_units_per_sq_second_silent);
  if (EEPROM_M500_SIZE + EEPROM_OFFSET == i) {
	  char ver2[4] = EEPROM_VERSION;
	  i = offset;
	  EEPROM_WRITE_VAR(i, ver2); // validate data
	  SERIAL_ECHO_START;
	  SERIAL_ECHOLNPGM("Settings Stored");
  }
  else { //size of eeprom M500 section probably changed by mistake and data are not valid; do not validate data by storing eeprom version
	  //M500 EEPROM section will be erased on next printer reboot and default vaules will be used
	  puts_P(PSTR("Data stored to EEPROM not valid."));
  }
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
		echomagic, echomagic, axis_steps_per_unit[X_AXIS], axis_steps_per_unit[Y_AXIS], axis_steps_per_unit[Z_AXIS], axis_steps_per_unit[E_AXIS],
		echomagic, echomagic, max_feedrate_normal[X_AXIS], max_feedrate_normal[Y_AXIS], max_feedrate_normal[Z_AXIS], max_feedrate_normal[E_AXIS],
		echomagic, echomagic, max_feedrate_silent[X_AXIS], max_feedrate_silent[Y_AXIS], max_feedrate_silent[Z_AXIS], max_feedrate_silent[E_AXIS],
		echomagic, echomagic, max_acceleration_units_per_sq_second_normal[X_AXIS], max_acceleration_units_per_sq_second_normal[Y_AXIS], max_acceleration_units_per_sq_second_normal[Z_AXIS], max_acceleration_units_per_sq_second_normal[E_AXIS],
		echomagic, echomagic, max_acceleration_units_per_sq_second_silent[X_AXIS], max_acceleration_units_per_sq_second_silent[Y_AXIS], max_acceleration_units_per_sq_second_silent[Z_AXIS], max_acceleration_units_per_sq_second_silent[E_AXIS],
		echomagic, echomagic, acceleration, retract_acceleration,
		echomagic, echomagic, minimumfeedrate, mintravelfeedrate, minsegmenttime, max_jerk[X_AXIS], max_jerk[Y_AXIS], max_jerk[Z_AXIS], max_jerk[E_AXIS],
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
		echomagic, echomagic, axis_steps_per_unit[X_AXIS], axis_steps_per_unit[Y_AXIS], axis_steps_per_unit[Z_AXIS], axis_steps_per_unit[E_AXIS],
		echomagic, echomagic, max_feedrate[X_AXIS], max_feedrate[Y_AXIS], max_feedrate[Z_AXIS], max_feedrate[E_AXIS],
		echomagic, echomagic, max_acceleration_units_per_sq_second[X_AXIS], max_acceleration_units_per_sq_second[Y_AXIS], max_acceleration_units_per_sq_second[Z_AXIS], max_acceleration_units_per_sq_second[E_AXIS],
		echomagic, echomagic, acceleration, retract_acceleration,
		echomagic, echomagic, minimumfeedrate, mintravelfeedrate, minsegmenttime, max_jerk[X_AXIS], max_jerk[Y_AXIS], max_jerk[Z_AXIS], max_jerk[E_AXIS],
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
bool Config_RetrieveSettings(uint16_t offset)
{
    int i=offset;
	bool previous_settings_retrieved = true;
    char stored_ver[4];
    char ver[4]=EEPROM_VERSION;
    EEPROM_READ_VAR(i,stored_ver); //read stored version
    //  SERIAL_ECHOLN("Version: [" << ver << "] Stored version: [" << stored_ver << "]");
    if (strncmp(ver,stored_ver,3) == 0)
    {
        // version number match
        EEPROM_READ_VAR(i,axis_steps_per_unit);
        EEPROM_READ_VAR(i,max_feedrate_normal);
        EEPROM_READ_VAR(i,max_acceleration_units_per_sq_second_normal);
        
        // steps per sq second need to be updated to agree with the units per sq second (as they are what is used in the planner)
        
        EEPROM_READ_VAR(i,acceleration);
        EEPROM_READ_VAR(i,retract_acceleration);
        EEPROM_READ_VAR(i,minimumfeedrate);
        EEPROM_READ_VAR(i,mintravelfeedrate);
        EEPROM_READ_VAR(i,minsegmenttime);
        EEPROM_READ_VAR(i,max_jerk[X_AXIS]);
        EEPROM_READ_VAR(i,max_jerk[Y_AXIS]);
		EEPROM_READ_VAR(i,max_jerk[Z_AXIS]);
		EEPROM_READ_VAR(i,max_jerk[E_AXIS]);
		if (max_jerk[X_AXIS] > DEFAULT_XJERK) max_jerk[X_AXIS] = DEFAULT_XJERK;
		if (max_jerk[Y_AXIS] > DEFAULT_YJERK) max_jerk[Y_AXIS] = DEFAULT_YJERK;
        EEPROM_READ_VAR(i,add_homing);
	/*
        EEPROM_READ_VAR(i,plaPreheatHotendTemp);
        EEPROM_READ_VAR(i,plaPreheatHPBTemp);
        EEPROM_READ_VAR(i,plaPreheatFanSpeed);
        EEPROM_READ_VAR(i,absPreheatHotendTemp);
        EEPROM_READ_VAR(i,absPreheatHPBTemp);
        EEPROM_READ_VAR(i,absPreheatFanSpeed);
        */

        
        EEPROM_READ_VAR(i,zprobe_zoffset);
        #ifndef PIDTEMP
        float Kp,Ki,Kd;
        #endif
        // do not need to scale PID values as the values in EEPROM are already scaled		
        EEPROM_READ_VAR(i,Kp);
        EEPROM_READ_VAR(i,Ki);
        EEPROM_READ_VAR(i,Kd);
		#ifdef PIDTEMPBED
		EEPROM_READ_VAR(i, bedKp);
		EEPROM_READ_VAR(i, bedKi);
		EEPROM_READ_VAR(i, bedKd);
		#endif

		int lcd_contrast;
		EEPROM_READ_VAR(i,lcd_contrast);

		#ifdef FWRETRACT
		EEPROM_READ_VAR(i,autoretract_enabled);
		EEPROM_READ_VAR(i,retract_length);
		#if EXTRUDERS > 1
		EEPROM_READ_VAR(i,retract_length_swap);
		#endif
		EEPROM_READ_VAR(i,retract_feedrate);
		EEPROM_READ_VAR(i,retract_zlift);
		EEPROM_READ_VAR(i,retract_recover_length);
		#if EXTRUDERS > 1
		EEPROM_READ_VAR(i,retract_recover_length_swap);
		#endif
		EEPROM_READ_VAR(i,retract_recover_feedrate);
		#endif

		EEPROM_READ_VAR(i, volumetric_enabled);
		EEPROM_READ_VAR(i, filament_size[0]);
#if EXTRUDERS > 1
		EEPROM_READ_VAR(i, filament_size[1]);
#if EXTRUDERS > 2
		EEPROM_READ_VAR(i, filament_size[2]);
#endif
#endif

		calculate_extruder_multipliers();
		int max_feedrate_silent_address = i;
        EEPROM_READ_VAR(i,max_feedrate_silent);  
        EEPROM_READ_VAR(i,max_acceleration_units_per_sq_second_silent);

		//if max_feedrate_silent and max_acceleration_units_per_sq_second_silent were never stored to eeprom, use default values:
	    float tmp_feedrate[]=DEFAULT_MAX_FEEDRATE_SILENT;
		unsigned long tmp_acceleration[]=DEFAULT_MAX_ACCELERATION_SILENT;
		for (uint8_t axis = X_AXIS; axis <= E_AXIS; axis++) {
			if (eeprom_read_dword((uint32_t*)(max_feedrate_silent_address + axis * 4)) == 0xffffffff) max_feedrate_silent[axis] = tmp_feedrate[axis];
			if (max_acceleration_units_per_sq_second_silent[axis] == 0xffffffff) max_acceleration_units_per_sq_second_silent[axis] = tmp_acceleration[axis];
		}

#ifdef TMC2130
		for (uint8_t j = X_AXIS; j <= Y_AXIS; j++)
		{
			if (max_feedrate_normal[j] > NORMAL_MAX_FEEDRATE_XY)
				max_feedrate_normal[j] = NORMAL_MAX_FEEDRATE_XY;
			if (max_feedrate_silent[j] > SILENT_MAX_FEEDRATE_XY)
				max_feedrate_silent[j] = SILENT_MAX_FEEDRATE_XY;
			if (max_acceleration_units_per_sq_second_normal[j] > NORMAL_MAX_ACCEL_XY)
				max_acceleration_units_per_sq_second_normal[j] = NORMAL_MAX_ACCEL_XY;
			if (max_acceleration_units_per_sq_second_silent[j] > SILENT_MAX_ACCEL_XY)
				max_acceleration_units_per_sq_second_silent[j] = SILENT_MAX_ACCEL_XY;
		}
#endif //TMC2130

		reset_acceleration_rates();

		// Call updatePID (similar to when we have processed M301)
		updatePID();

		if (EEPROM_M500_SIZE + EEPROM_OFFSET == i) {
			SERIAL_ECHO_START;
			SERIAL_ECHOLNPGM("Stored settings retrieved");

		}
		else { //size of eeprom M500 section probably changed by mistake and data are not valid; default values will be used
			puts_P(PSTR("Data read from EEPROM not valid."));
			Config_ResetDefault();
			previous_settings_retrieved = false;
		}

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
    float tmp1[]=DEFAULT_AXIS_STEPS_PER_UNIT;
    float tmp2[]=DEFAULT_MAX_FEEDRATE;
    long tmp3[]=DEFAULT_MAX_ACCELERATION;
    float tmp4[]=DEFAULT_MAX_FEEDRATE_SILENT;
    long tmp5[]=DEFAULT_MAX_ACCELERATION_SILENT;
    for (short i=0;i<4;i++) 
    {
        axis_steps_per_unit[i]=tmp1[i];  
        max_feedrate_normal[i]=tmp2[i];  
        max_acceleration_units_per_sq_second_normal[i]=tmp3[i];
        max_feedrate_silent[i]=tmp4[i];  
        max_acceleration_units_per_sq_second_silent[i]=tmp5[i];
    }

	// steps per sq second need to be updated to agree with the units per sq second
    reset_acceleration_rates();
    
    acceleration=DEFAULT_ACCELERATION;
    retract_acceleration=DEFAULT_RETRACT_ACCELERATION;
    minimumfeedrate=DEFAULT_MINIMUMFEEDRATE;
    minsegmenttime=DEFAULT_MINSEGMENTTIME;       
    mintravelfeedrate=DEFAULT_MINTRAVELFEEDRATE;
    max_jerk[X_AXIS] = DEFAULT_XJERK;
    max_jerk[Y_AXIS] = DEFAULT_YJERK;
    max_jerk[Z_AXIS] = DEFAULT_ZJERK;
    max_jerk[E_AXIS] = DEFAULT_EJERK;
    add_homing[X_AXIS] = add_homing[Y_AXIS] = add_homing[Z_AXIS] = 0;

#ifdef ENABLE_AUTO_BED_LEVELING
    zprobe_zoffset = -Z_PROBE_OFFSET_FROM_EXTRUDER;
#endif
#ifdef PIDTEMP
    Kp = DEFAULT_Kp;
    Ki = scalePID_i(DEFAULT_Ki);
    Kd = scalePID_d(DEFAULT_Kd);
    
    // call updatePID (similar to when we have processed M301)
    updatePID();
    
#ifdef PID_ADD_EXTRUSION_RATE
    Kc = DEFAULT_Kc;
#endif//PID_ADD_EXTRUSION_RATE
#endif//PIDTEMP

#ifdef FWRETRACT
	autoretract_enabled = false;
	retract_length = RETRACT_LENGTH;
#if EXTRUDERS > 1
	retract_length_swap = RETRACT_LENGTH_SWAP;
#endif
	retract_feedrate = RETRACT_FEEDRATE;
	retract_zlift = RETRACT_ZLIFT;
	retract_recover_length = RETRACT_RECOVER_LENGTH;
#if EXTRUDERS > 1
	retract_recover_length_swap = RETRACT_RECOVER_LENGTH_SWAP;
#endif
	retract_recover_feedrate = RETRACT_RECOVER_FEEDRATE;
#endif

	volumetric_enabled = false;
	filament_size[0] = DEFAULT_NOMINAL_FILAMENT_DIA;
#if EXTRUDERS > 1
	filament_size[1] = DEFAULT_NOMINAL_FILAMENT_DIA;
#if EXTRUDERS > 2
	filament_size[2] = DEFAULT_NOMINAL_FILAMENT_DIA;
#endif
#endif
	calculate_extruder_multipliers();

SERIAL_ECHO_START;
SERIAL_ECHOLNPGM("Hardcoded Default Settings Loaded");

}
