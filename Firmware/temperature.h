/*
  temperature.h - temperature controller
  Part of Marlin

  Copyright (c) 2011 Erik van der Zalm

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef temperature_h
#define temperature_h 

#include "Marlin.h"
#include "config.h"

// public functions
void soft_pwm_init(); //initialize the soft pwm isr
void temp_mgr_init(); //initialize the temperature handler
void manage_heater(); //it is critical that this is called periodically.
bool get_temp_error(); //return true if any thermal error is set

extern bool checkAllHotends(void);

// low level conversion routines
// do not use these routines and variables outside of temperature.cpp
extern int target_temperature[EXTRUDERS];  
extern float current_temperature[EXTRUDERS];
#ifdef SHOW_TEMP_ADC_VALUES
  extern int current_temperature_raw[EXTRUDERS];
  extern int current_temperature_bed_raw;
#endif
extern int target_temperature_bed;
extern float current_temperature_bed;

#ifdef PINDA_THERMISTOR
extern uint16_t current_temperature_raw_pinda;
extern float current_temperature_pinda;
bool has_temperature_compensation();
#endif

#ifdef AMBIENT_THERMISTOR
extern int current_temperature_raw_ambient;
extern float current_temperature_ambient;
#endif

#ifdef VOLT_PWR_PIN
extern int current_voltage_raw_pwr;
#endif

#ifdef VOLT_BED_PIN
extern int current_voltage_raw_bed;
#endif

#ifdef IR_SENSOR_ANALOG
extern uint16_t current_voltage_raw_IR;
#endif //IR_SENSOR_ANALOG

extern bool bedPWMDisabled;

#ifdef PIDTEMP
  extern int pid_cycle, pid_number_of_cycles;
  extern float _Kp,_Ki,_Kd;
  float scalePID_i(float i);
  float scalePID_d(float d);
  float unscalePID_i(float i);
  float unscalePID_d(float d);

  bool pidTuningRunning(); // returns true if PID tuning is still running
  void preparePidTuning(); // non-blocking call to set "pidTuningRunning" to true immediately
#endif


#ifdef BABYSTEPPING
extern volatile int babystepsTodo[3];

inline void babystepsTodoZadd(int n)
{
    if (n != 0) {
        CRITICAL_SECTION_START
        babystepsTodo[Z_AXIS] += n;
        CRITICAL_SECTION_END
    }
}
#endif

void resetPID(uint8_t extruder);

//high level conversion routines, for use outside of temperature.cpp
//inline so that there is no performance decrease.
//deg=degreeCelsius

// Doesn't save FLASH when FORCE_INLINE removed.
FORCE_INLINE float degHotend(uint8_t extruder) {  
  return current_temperature[extruder];
};

#ifdef SHOW_TEMP_ADC_VALUES
  FORCE_INLINE float rawHotendTemp(uint8_t extruder) {  
    return current_temperature_raw[extruder];
  };

  FORCE_INLINE float rawBedTemp() {  
    return current_temperature_bed_raw;
  };
#endif

FORCE_INLINE float degBed() {
  return current_temperature_bed;
};

// Doesn't save FLASH when FORCE_INLINE removed.
FORCE_INLINE float degTargetHotend(uint8_t extruder) {  
  return target_temperature[extruder];
};

FORCE_INLINE float degTargetBed() {   
  return target_temperature_bed;
};

// Doesn't save FLASH when FORCE_INLINE removed.
FORCE_INLINE void setTargetHotend(const float &celsius) {  
  target_temperature[0] = celsius;
  resetPID(0);
};

FORCE_INLINE void setTargetBed(const float &celsius) {  
  target_temperature_bed = celsius;
};

FORCE_INLINE bool isHeatingHotend(uint8_t extruder){  
  return target_temperature[extruder] > current_temperature[extruder];
};

FORCE_INLINE bool isHeatingBed() {
  return target_temperature_bed > current_temperature_bed;
};

FORCE_INLINE bool isCoolingHotend(uint8_t extruder) {  
  return target_temperature[extruder] < current_temperature[extruder];
};

FORCE_INLINE bool isCoolingBed() {
  return target_temperature_bed < current_temperature_bed;
};

#define degHotend0() degHotend(0)
#define degTargetHotend0() degTargetHotend(0)
#define isHeatingHotend0() isHeatingHotend(0)
#define isCoolingHotend0() isCoolingHotend(0)

// return "false", if all heaters are 'off' (ie. "true", if any heater is 'on')
#define CHECK_ALL_HEATERS (checkAllHotends()||(target_temperature_bed!=0))

int getHeaterPower(int heater);
void disable_heater(); // Disable all heaters *instantaneously*
void updatePID();


FORCE_INLINE void autotempShutdown(){
 #ifdef AUTOTEMP
 if(autotemp_enabled)
 {
  autotemp_enabled=false;
  if(degTargetHotend(active_extruder)>autotemp_min)
    setTargetHotend(0);
 }
 #endif
}

void PID_autotune(float temp, int extruder, int ncycles);

#ifdef TEMP_MODEL
bool temp_model_enabled(); // return temperature model state
void temp_model_set_enabled(bool enabled);
void temp_model_set_warn_beep(bool enabled);
void temp_model_set_params(float P=NAN, float U=NAN, float V=NAN, float C=NAN, float D=NAN,
    int16_t L=-1, float Ta_corr=NAN, float warn=NAN, float err=NAN);
void temp_model_set_resistance(uint8_t index, float R);

void temp_model_report_settings();
void temp_model_reset_settings();
void temp_model_load_settings();
void temp_model_save_settings();

void temp_model_autotune(int16_t temp = 0, bool selftest = false);
bool temp_model_autotune_result(); // return true if the last autotune was complete and successful

#ifdef TEMP_MODEL_DEBUG
void temp_model_log_enable(bool enable);
#endif
#endif

#ifdef FAN_SOFT_PWM
extern unsigned char fanSpeedSoftPwm;
#endif
extern uint8_t fanSpeedBckp;

#endif
