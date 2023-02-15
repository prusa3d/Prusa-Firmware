/*
  temperature.c - temperature control
  Part of Marlin
  
 Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 This firmware is a mashup between Sprinter and grbl.
  (https://github.com/kliment/Sprinter)
  (https://github.com/simen/grbl/tree)
 
 It has preliminary support for Matthew Roberts advance algorithm 
    http://reprap.org/pipermail/reprap-dev/2011-May/003323.html

 */

#include "temperature.h"
#include "stepper.h"
#include "ultralcd.h"
#include "menu.h"
#include "util.h"
#include "fancheck.h"
#include "messages.h"
#include "language.h"

#include "SdFatUtil.h"

#include <avr/wdt.h>
#include <util/atomic.h>
#include "adc.h"
#include "ConfigurationStore.h"
#include "Timer.h"
#include "Configuration_var.h"
#include "Prusa_farm.h"

#if (ADC_OVRSAMPL != OVERSAMPLENR)
#error "ADC_OVRSAMPL oversampling must match OVERSAMPLENR"
#endif

#ifdef SYSTEM_TIMER_2
#define ENABLE_SOFT_PWM_INTERRUPT()  TIMSK2 |= (1<<OCIE2B)
#define DISABLE_SOFT_PWM_INTERRUPT() TIMSK2 &= ~(1<<OCIE2B)
#else //SYSTEM_TIMER_2
#define ENABLE_SOFT_PWM_INTERRUPT()  TIMSK0 |= (1<<OCIE0B)
#define DISABLE_SOFT_PWM_INTERRUPT() TIMSK0 &= ~(1<<OCIE0B)
#endif //SYSTEM_TIMER_2

// temperature manager timer configuration
#define TEMP_MGR_INTV   0.27 // seconds, ~3.7Hz
#define TEMP_TIM_PRESCALE 256
#define TEMP_TIM_OCRA_OVF (uint16_t)(TEMP_MGR_INTV / ((long double)TEMP_TIM_PRESCALE / F_CPU))
#define TEMP_TIM_REGNAME(registerbase,number,suffix) _REGNAME(registerbase,number,suffix)
#undef B0 //Necessary hack because of "binary.h" included in "Arduino.h" included in "system_timer.h" included in this file...
#define TCCRxA TEMP_TIM_REGNAME(TCCR, TEMP_TIM, A)
#define TCCRxB TEMP_TIM_REGNAME(TCCR, TEMP_TIM, B)
#define TCCRxC TEMP_TIM_REGNAME(TCCR, TEMP_TIM, C)
#define TCNTx TEMP_TIM_REGNAME(TCNT, TEMP_TIM,)
#define OCRxA TEMP_TIM_REGNAME(OCR, TEMP_TIM, A)
#define TIMSKx TEMP_TIM_REGNAME(TIMSK, TEMP_TIM,)
#define TIFRx TEMP_TIM_REGNAME(TIFR, TEMP_TIM,)
#define TIMERx_COMPA_vect TEMP_TIM_REGNAME(TIMER, TEMP_TIM, _COMPA_vect)
#define CSx0 TEMP_TIM_REGNAME(CS, TEMP_TIM, 0)
#define CSx1 TEMP_TIM_REGNAME(CS, TEMP_TIM, 1)
#define CSx2 TEMP_TIM_REGNAME(CS, TEMP_TIM, 2)
#define WGMx0 TEMP_TIM_REGNAME(WGM, TEMP_TIM, 0)
#define WGMx1 TEMP_TIM_REGNAME(WGM, TEMP_TIM, 1)
#define WGMx2 TEMP_TIM_REGNAME(WGM, TEMP_TIM, 2)
#define WGMx3 TEMP_TIM_REGNAME(WGM, TEMP_TIM, 3)
#define COMxA0 TEMP_TIM_REGNAME(COM, TEMP_TIM, A0)
#define COMxB0 TEMP_TIM_REGNAME(COM, TEMP_TIM, B0)
#define COMxC0 TEMP_TIM_REGNAME(COM, TEMP_TIM, C0)
#define OCIExA TEMP_TIM_REGNAME(OCIE, TEMP_TIM, A)
#define OCFxA TEMP_TIM_REGNAME(OCF, TEMP_TIM, A)

#define TEMP_MGR_INT_FLAG_STATE()    (TIFRx & (1<<OCFxA))
#define TEMP_MGR_INT_FLAG_CLEAR()    TIFRx |= (1<<OCFxA)
#define TEMP_MGR_INTERRUPT_STATE()   (TIMSKx & (1<<OCIExA))
#define ENABLE_TEMP_MGR_INTERRUPT()  TIMSKx |=  (1<<OCIExA)
#define DISABLE_TEMP_MGR_INTERRUPT() TIMSKx &= ~(1<<OCIExA)

#ifdef TEMP_MODEL
// temperature model interface
#include "temp_model.h"
#endif

#include "Filament_sensor.h"

//===========================================================================
//=============================public variables============================
//===========================================================================
int target_temperature[EXTRUDERS] = { 0 };
int target_temperature_bed = 0;
int current_temperature_raw[EXTRUDERS] = { 0 };
float current_temperature[EXTRUDERS] = { 0.0 };

#ifdef PINDA_THERMISTOR
uint16_t current_temperature_raw_pinda = 0;
float current_temperature_pinda = 0.0;
#endif //PINDA_THERMISTOR

#ifdef AMBIENT_THERMISTOR
int current_temperature_raw_ambient = 0;
float current_temperature_ambient = 0.0;
#endif //AMBIENT_THERMISTOR

#ifdef VOLT_PWR_PIN
int current_voltage_raw_pwr = 0;
#endif

#ifdef VOLT_BED_PIN
int current_voltage_raw_bed = 0;
#endif

int current_temperature_bed_raw = 0;
float current_temperature_bed = 0.0;
  

#ifdef PIDTEMP
  float _Kp, _Ki, _Kd;
  int pid_cycle, pid_number_of_cycles;
  static bool pid_tuning_finished = true;

  bool pidTuningRunning() {
      return !pid_tuning_finished;
  }

  void preparePidTuning() {
      // ensure heaters are disabled before we switch off PID management!
      disable_heater();
      pid_tuning_finished = false;
  }
#endif //PIDTEMP
  
unsigned char soft_pwm_bed;

#ifdef BABYSTEPPING
  volatile int babystepsTodo[3]={0,0,0};
#endif

//===========================================================================
//=============================private variables============================
//===========================================================================
static volatile bool temp_meas_ready = false;

#ifdef PIDTEMP
  //static cannot be external:
  static float iState_sum[EXTRUDERS] = { 0 };
  static float dState_last[EXTRUDERS] = { 0 };
  static float pTerm[EXTRUDERS];
  static float iTerm[EXTRUDERS];
  static float dTerm[EXTRUDERS];
  static float pid_error[EXTRUDERS];
  static float iState_sum_min[EXTRUDERS];
  static float iState_sum_max[EXTRUDERS];
  static bool pid_reset[EXTRUDERS];
#endif //PIDTEMP
#ifdef PIDTEMPBED
  //static cannot be external:
  static float temp_iState_bed = { 0 };
  static float temp_dState_bed = { 0 };
  static float pTerm_bed;
  static float iTerm_bed;
  static float dTerm_bed;
  static float pid_error_bed;
  static float temp_iState_min_bed;
  static float temp_iState_max_bed;
#else //PIDTEMPBED
	static unsigned long  previous_millis_bed_heater;
#endif //PIDTEMPBED
  static unsigned char soft_pwm[EXTRUDERS];

#ifdef FAN_SOFT_PWM
  unsigned char fanSpeedSoftPwm;
  static unsigned char soft_pwm_fan;
#endif
uint8_t fanSpeedBckp = 255;

#if EXTRUDERS > 3
  # error Unsupported number of extruders
#elif EXTRUDERS > 2
  # define ARRAY_BY_EXTRUDERS(v1, v2, v3) { v1, v2, v3 }
#elif EXTRUDERS > 1
  # define ARRAY_BY_EXTRUDERS(v1, v2, v3) { v1, v2 }
#else
  # define ARRAY_BY_EXTRUDERS(v1, v2, v3) { v1 }
#endif

// Init min and max temp with extreme values to prevent false errors during startup
static int minttemp_raw[EXTRUDERS] = ARRAY_BY_EXTRUDERS( HEATER_0_RAW_LO_TEMP , HEATER_1_RAW_LO_TEMP , HEATER_2_RAW_LO_TEMP );
static int maxttemp_raw[EXTRUDERS] = ARRAY_BY_EXTRUDERS( HEATER_0_RAW_HI_TEMP , HEATER_1_RAW_HI_TEMP , HEATER_2_RAW_HI_TEMP );
static int minttemp[EXTRUDERS] = ARRAY_BY_EXTRUDERS( 0, 0, 0 );
static int maxttemp[EXTRUDERS] = ARRAY_BY_EXTRUDERS( 16383, 16383, 16383 );
#ifdef BED_MINTEMP
static int bed_minttemp_raw = HEATER_BED_RAW_LO_TEMP;
#endif
#ifdef BED_MAXTEMP
static int bed_maxttemp_raw = HEATER_BED_RAW_HI_TEMP;
#endif
#ifdef AMBIENT_MINTEMP
static int ambient_minttemp_raw = AMBIENT_RAW_LO_TEMP;
#endif
#ifdef AMBIENT_MAXTEMP
static int ambient_maxttemp_raw = AMBIENT_RAW_HI_TEMP;
#endif

static void *heater_ttbl_map[EXTRUDERS] = ARRAY_BY_EXTRUDERS( (void *)HEATER_0_TEMPTABLE, (void *)HEATER_1_TEMPTABLE, (void *)HEATER_2_TEMPTABLE );
static uint8_t heater_ttbllen_map[EXTRUDERS] = ARRAY_BY_EXTRUDERS( HEATER_0_TEMPTABLE_LEN, HEATER_1_TEMPTABLE_LEN, HEATER_2_TEMPTABLE_LEN );

static float analog2temp(int raw, uint8_t e);
static float analog2tempBed(int raw);
#ifdef AMBIENT_MAXTEMP
static float analog2tempAmbient(int raw);
#endif
static void updateTemperatures();

enum TempRunawayStates : uint8_t
{
	TempRunaway_INACTIVE = 0,
	TempRunaway_PREHEAT = 1,
	TempRunaway_ACTIVE = 2,
};

#ifndef SOFT_PWM_SCALE
#define SOFT_PWM_SCALE 0
#endif

//===========================================================================
//=============================   functions      ============================
//===========================================================================

#if (defined (TEMP_RUNAWAY_BED_HYSTERESIS) && TEMP_RUNAWAY_BED_TIMEOUT > 0) || (defined (TEMP_RUNAWAY_EXTRUDER_HYSTERESIS) && TEMP_RUNAWAY_EXTRUDER_TIMEOUT > 0)
static uint8_t temp_runaway_status[1 + EXTRUDERS];
static float temp_runaway_target[1 + EXTRUDERS];
static uint32_t temp_runaway_timer[1 + EXTRUDERS];
static uint16_t temp_runaway_error_counter[1 + EXTRUDERS];

static void temp_runaway_check(uint8_t _heater_id, float _target_temperature, float _current_temperature, float _output, bool _isbed);
static void temp_runaway_stop(bool isPreheat, bool isBed);
#endif

// return "false", if all extruder-heaters are 'off' (ie. "true", if any heater is 'on')
bool checkAllHotends(void)
{
    bool result=false;
    for(int i=0;i<EXTRUDERS;i++) result=(result||(target_temperature[i]!=0));
    return(result);
}

// WARNING: the following function has been marked noinline to avoid a GCC 4.9.2 LTO
//          codegen bug causing a stack overwrite issue in process_commands()
void __attribute__((noinline)) PID_autotune(float temp, int extruder, int ncycles)
{
  preparePidTuning();

  pid_number_of_cycles = ncycles;
  float input = 0.0;
  pid_cycle=0;
  bool heating = true;

  unsigned long temp_millis = _millis();
  unsigned long t1=temp_millis;
  unsigned long t2=temp_millis;
  long t_high = 0;
  long t_low = 0;

  long bias, d;
  float Ku, Tu;
  float max = 0, min = 10000;
  uint8_t safety_check_cycles = 0;
  const uint8_t safety_check_cycles_count = (extruder < 0) ? 45 : 10; //10 cycles / 20s delay for extruder and 45 cycles / 90s for heatbed
  float temp_ambient;

#if (defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1)
  unsigned long extruder_autofan_last_check = _millis();
#endif

  if ((extruder >= EXTRUDERS)
  #if (TEMP_BED_PIN <= -1)
       ||(extruder < 0)
  #endif
       ){
          SERIAL_ECHOLNPGM("PID Autotune failed. Bad extruder number.");
		  pid_tuning_finished = true;
		  pid_cycle = 0;
          return;
        }
	
  SERIAL_ECHOLNPGM("PID Autotune start");

  if (extruder<0)
  {
     soft_pwm_bed = (MAX_BED_POWER)/2;
	 timer02_set_pwm0(soft_pwm_bed << 1);
     bias = d = (MAX_BED_POWER)/2;
     target_temperature_bed = (int)temp; // to display the requested target bed temperature properly on the main screen
   }
   else
   {
     soft_pwm[extruder] = (PID_MAX)/2;
     bias = d = (PID_MAX)/2;
     target_temperature[extruder] = (int)temp; // to display the requested target extruder temperature properly on the main screen
  }

  for(;;) {
#ifdef WATCHDOG
    wdt_reset();
#endif //WATCHDOG
    if(temp_meas_ready == true) { // temp sample ready
      updateTemperatures();

      input = (extruder<0)?current_temperature_bed:current_temperature[extruder];

      max=max(max,input);
      min=min(min,input);

      #if (defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1)
      if(_millis() - extruder_autofan_last_check > 2500) {
        checkExtruderAutoFans();
        extruder_autofan_last_check = _millis();
      }
      #endif

      if(heating == true && input > temp) {
        if(_millis() - t2 > 5000) { 
          heating=false;
          if (extruder<0)
		  {
            soft_pwm_bed = (bias - d) >> 1;
			timer02_set_pwm0(soft_pwm_bed << 1);
		  }
          else
            soft_pwm[extruder] = (bias - d) >> 1;
          t1=_millis();
          t_high=t1 - t2;
          max=temp;
        }
      }
      if(heating == false && input < temp) {
        if(_millis() - t1 > 5000) {
          heating=true;
          t2=_millis();
          t_low=t2 - t1;
          if(pid_cycle > 0) {
            bias += (d*(t_high - t_low))/(t_low + t_high);
            bias = constrain(bias, 20 ,(extruder<0?(MAX_BED_POWER):(PID_MAX))-20);
            if(bias > (extruder<0?(MAX_BED_POWER):(PID_MAX))/2) d = (extruder<0?(MAX_BED_POWER):(PID_MAX)) - 1 - bias;
            else d = bias;

            SERIAL_PROTOCOLPGM(" bias: "); SERIAL_PROTOCOL(bias);
            SERIAL_PROTOCOLPGM(" d: "); SERIAL_PROTOCOL(d);
            SERIAL_PROTOCOLPGM(" min: "); SERIAL_PROTOCOL(min);
            SERIAL_PROTOCOLPGM(" max: "); SERIAL_PROTOCOLLN(max);
            if(pid_cycle > 2) {
              Ku = (4.0*d)/(3.14159*(max-min)/2.0);
              Tu = ((float)(t_low + t_high)/1000.0);
              SERIAL_PROTOCOLPGM(" Ku: "); SERIAL_PROTOCOL(Ku);
              SERIAL_PROTOCOLPGM(" Tu: "); SERIAL_PROTOCOLLN(Tu);
              _Kp = 0.6*Ku;
              _Ki = 2*_Kp/Tu;
              _Kd = _Kp*Tu/8;
              SERIAL_PROTOCOLLNPGM(" Classic PID ");
              SERIAL_PROTOCOLPGM(" Kp: "); SERIAL_PROTOCOLLN(_Kp);
              SERIAL_PROTOCOLPGM(" Ki: "); SERIAL_PROTOCOLLN(_Ki);
              SERIAL_PROTOCOLPGM(" Kd: "); SERIAL_PROTOCOLLN(_Kd);
              /*
              _Kp = 0.33*Ku;
              _Ki = _Kp/Tu;
              _Kd = _Kp*Tu/3;
              SERIAL_PROTOCOLLNPGM(" Some overshoot ");
              SERIAL_PROTOCOLPGM(" Kp: "); SERIAL_PROTOCOLLN(_Kp);
              SERIAL_PROTOCOLPGM(" Ki: "); SERIAL_PROTOCOLLN(_Ki);
              SERIAL_PROTOCOLPGM(" Kd: "); SERIAL_PROTOCOLLN(_Kd);
              _Kp = 0.2*Ku;
              _Ki = 2*_Kp/Tu;
              _Kd = _Kp*Tu/3;
              SERIAL_PROTOCOLLNPGM(" No overshoot ");
              SERIAL_PROTOCOLPGM(" Kp: "); SERIAL_PROTOCOLLN(_Kp);
              SERIAL_PROTOCOLPGM(" Ki: "); SERIAL_PROTOCOLLN(_Ki);
              SERIAL_PROTOCOLPGM(" Kd: "); SERIAL_PROTOCOLLN(_Kd);
              */
            }
          }
          if (extruder<0)
		  {
            soft_pwm_bed = (bias + d) >> 1;
			timer02_set_pwm0(soft_pwm_bed << 1);
		  }
          else
            soft_pwm[extruder] = (bias + d) >> 1;
          pid_cycle++;
          min=temp;
        }
      } 
    }
    if(input > (temp + 20)) {
      SERIAL_PROTOCOLLNPGM("PID Autotune failed! Temperature too high");
	  pid_tuning_finished = true;
	  pid_cycle = 0;
      return;
    }
    if(_millis() - temp_millis > 2000) {
      int p;
      if (extruder<0){
        p=soft_pwm_bed;       
        SERIAL_PROTOCOLPGM("B:");
      }else{
        p=soft_pwm[extruder];       
        SERIAL_PROTOCOLPGM("T:");
      }
			
      SERIAL_PROTOCOL(input);   
      SERIAL_PROTOCOLPGM(" @:");
      SERIAL_PROTOCOLLN(p);       
		if (safety_check_cycles == 0) { //save ambient temp
			temp_ambient = input;
			//SERIAL_ECHOPGM("Ambient T: ");
			//MYSERIAL.println(temp_ambient);
			safety_check_cycles++;
		}
		else if (safety_check_cycles < safety_check_cycles_count) { //delay
			safety_check_cycles++;		
		}
		else if (safety_check_cycles == safety_check_cycles_count){ //check that temperature is rising
			safety_check_cycles++;
			//SERIAL_ECHOPGM("Time from beginning: ");
			//MYSERIAL.print(safety_check_cycles_count * 2);
			//SERIAL_ECHOPGM("s. Difference between current and ambient T: ");
			//MYSERIAL.println(input - temp_ambient);

			if (fabs(input - temp_ambient) < 5.0) { 
				temp_runaway_stop(false, (extruder<0));
				pid_tuning_finished = true;
				return;
			}
		}
      temp_millis = _millis();
    }
    if(((_millis() - t1) + (_millis() - t2)) > (10L*60L*1000L*2L)) {
      SERIAL_PROTOCOLLNPGM("PID Autotune failed! timeout");
	  pid_tuning_finished = true;
	  pid_cycle = 0;
      return;
    }
    if(pid_cycle > ncycles) {
      SERIAL_PROTOCOLLNPGM("PID Autotune finished! Put the last Kp, Ki and Kd constants from above into Configuration.h");
	  pid_tuning_finished = true;
	  pid_cycle = 0;
      return;
    }
    lcd_update(0);
  }
}

void updatePID()
{
  // TODO: iState_sum_max and PID values should be synchronized for temp_mgr_isr
#ifdef PIDTEMP
  for(uint_least8_t e = 0; e < EXTRUDERS; e++) {
     iState_sum_max[e] = PID_INTEGRAL_DRIVE_MAX / cs.Ki;  
  }
#endif
#ifdef PIDTEMPBED
  temp_iState_max_bed = PID_INTEGRAL_DRIVE_MAX / cs.bedKi;  
#endif
}
  
int getHeaterPower(int heater) {
	if (heater<0)
		return soft_pwm_bed;
  return soft_pwm[heater];
}

// reset PID state after changing target_temperature
void resetPID(uint8_t extruder _UNUSED) {}

enum class TempErrorSource : uint8_t
{
    hotend,
    bed,
#ifdef AMBIENT_THERMISTOR
    ambient,
#endif
};

// thermal error type (in order of decreasing priority!)
enum class TempErrorType : uint8_t
{
    max,
    min,
    preheat,
    runaway,
#ifdef TEMP_MODEL
    model,
#endif
};

// error state (updated via set_temp_error from isr context)
volatile static union
{
    uint8_t v;
    struct
    {
        uint8_t error: 1;  // error condition
        uint8_t assert: 1; // error is still asserted
        uint8_t source: 2; // source
        uint8_t index: 1;  // source index
        uint8_t type: 3;   // error type
    };
} temp_error_state;

// set the error type from within the temp_mgr isr to be handled in manager_heater
// - immediately disable all heaters and turn on all fans at full speed
// - prevent the user to set temperatures until all errors are cleared
void set_temp_error(TempErrorSource source, uint8_t index, TempErrorType type)
{
    // save the original target temperatures for recovery before disabling heaters
    if(!temp_error_state.error && !saved_printing) {
        saved_bed_temperature = target_temperature_bed;
        saved_extruder_temperature = target_temperature[index];
        saved_fan_speed = fanSpeed;
    }

    // keep disabling heaters and keep fans on as long as the condition is asserted
    disable_heater();
    hotendFanSetFullSpeed();

    // set the initial error source to the highest priority error
    if(!temp_error_state.error || (uint8_t)type < temp_error_state.type) {
        temp_error_state.source = (uint8_t)source;
        temp_error_state.index = index;
        temp_error_state.type = (uint8_t)type;
    }

    // always set the error state
    temp_error_state.error = true;
    temp_error_state.assert = true;
}

bool get_temp_error()
{
    return temp_error_state.v;
}

void handle_temp_error();

void manage_heater()
{
#ifdef WATCHDOG
    wdt_reset();
#endif //WATCHDOG

    // limit execution to the same rate as temp_mgr (low-level fault handling is already handled -
    // any remaining error handling is just user-facing and can wait one extra cycle)
    if(!temp_meas_ready)
        return;

    // syncronize temperatures with isr
    updateTemperatures();

#ifdef TEMP_MODEL
    // handle model warnings first, so not to override the error handler
    if(temp_model::warning_state.warning)
        temp_model::handle_warning();
#endif

    // handle temperature errors
    if(temp_error_state.v)
        handle_temp_error();

    // periodically check fans
    checkFans();

#ifdef TEMP_MODEL_DEBUG
    temp_model::log_usr();
#endif
}

#define PGM_RD_W(x)   (short)pgm_read_word(&x)
// Derived from RepRap FiveD extruder::getTemperature()
// For hot end temperature measurement.
static float analog2temp(int raw, uint8_t e) {
  if(e >= EXTRUDERS)
  {
      SERIAL_ERROR_START;
      SERIAL_ERROR((int)e);
      SERIAL_ERRORLNPGM(" - Invalid extruder number !");
      kill(NULL, 6);
      return 0.0;
  } 
  #ifdef HEATER_0_USES_MAX6675
    if (e == 0)
    {
      return 0.25 * raw;
    }
  #endif

  if(heater_ttbl_map[e] != NULL)
  {
    float celsius = 0;
    uint8_t i;
    short (*tt)[][2] = (short (*)[][2])(heater_ttbl_map[e]);

    for (i=1; i<heater_ttbllen_map[e]; i++)
    {
      if (PGM_RD_W((*tt)[i][0]) > raw)
      {
        celsius = PGM_RD_W((*tt)[i-1][1]) + 
          (raw - PGM_RD_W((*tt)[i-1][0])) * 
          (float)(PGM_RD_W((*tt)[i][1]) - PGM_RD_W((*tt)[i-1][1])) /
          (float)(PGM_RD_W((*tt)[i][0]) - PGM_RD_W((*tt)[i-1][0]));
        break;
      }
    }

    // Overflow: Set to last value in the table
    if (i == heater_ttbllen_map[e]) celsius = PGM_RD_W((*tt)[i-1][1]);

    return celsius;
  }
  return ((raw * ((5.0 * 100.0) / 1024.0) / OVERSAMPLENR) * TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET;
}

// Derived from RepRap FiveD extruder::getTemperature()
// For bed temperature measurement.
static float analog2tempBed(int raw) {
  #ifdef BED_USES_THERMISTOR
    float celsius = 0;
    byte i;

    for (i=1; i<BEDTEMPTABLE_LEN; i++)
    {
      if (PGM_RD_W(BEDTEMPTABLE[i][0]) > raw)
      {
        celsius  = PGM_RD_W(BEDTEMPTABLE[i-1][1]) + 
          (raw - PGM_RD_W(BEDTEMPTABLE[i-1][0])) * 
          (float)(PGM_RD_W(BEDTEMPTABLE[i][1]) - PGM_RD_W(BEDTEMPTABLE[i-1][1])) /
          (float)(PGM_RD_W(BEDTEMPTABLE[i][0]) - PGM_RD_W(BEDTEMPTABLE[i-1][0]));
        break;
      }
    }

    // Overflow: Set to last value in the table
    if (i == BEDTEMPTABLE_LEN) celsius = PGM_RD_W(BEDTEMPTABLE[i-1][1]);


	// temperature offset adjustment
#ifdef BED_OFFSET
	float _offset = BED_OFFSET;
	float _offset_center = BED_OFFSET_CENTER;
	float _offset_start = BED_OFFSET_START;
	float _first_koef = (_offset / 2) / (_offset_center - _offset_start);
	float _second_koef = (_offset / 2) / (100 - _offset_center);


	if (celsius >= _offset_start && celsius <= _offset_center)
	{
		celsius = celsius + (_first_koef * (celsius - _offset_start));
	}
	else if (celsius > _offset_center && celsius <= 100)
	{
		celsius = celsius + (_first_koef * (_offset_center - _offset_start)) + ( _second_koef * ( celsius - ( 100 - _offset_center ) )) ;
	}
	else if (celsius > 100)
	{
		celsius = celsius + _offset;
	}
#endif


    return celsius;
  #elif defined BED_USES_AD595
    return ((raw * ((5.0 * 100.0) / 1024.0) / OVERSAMPLENR) * TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET;
  #else
    return 0;
  #endif
}

#ifdef AMBIENT_THERMISTOR
static float analog2tempAmbient(int raw)
{
    float celsius = 0;
    byte i;

    for (i=1; i<AMBIENTTEMPTABLE_LEN; i++)
    {
      if (PGM_RD_W(AMBIENTTEMPTABLE[i][0]) > raw)
      {
        celsius  = PGM_RD_W(AMBIENTTEMPTABLE[i-1][1]) + 
          (raw - PGM_RD_W(AMBIENTTEMPTABLE[i-1][0])) * 
          (float)(PGM_RD_W(AMBIENTTEMPTABLE[i][1]) - PGM_RD_W(AMBIENTTEMPTABLE[i-1][1])) /
          (float)(PGM_RD_W(AMBIENTTEMPTABLE[i][0]) - PGM_RD_W(AMBIENTTEMPTABLE[i-1][0]));
        break;
      }
    }
    // Overflow: Set to last value in the table
    if (i == AMBIENTTEMPTABLE_LEN) celsius = PGM_RD_W(AMBIENTTEMPTABLE[i-1][1]);
    return celsius;
}
#endif //AMBIENT_THERMISTOR

void soft_pwm_init()
{
#if MB(RUMBA) && ((TEMP_SENSOR_0==-1)||(TEMP_SENSOR_1==-1)||(TEMP_SENSOR_2==-1)||(TEMP_SENSOR_BED==-1))
  //disable RUMBA JTAG in case the thermocouple extension is plugged on top of JTAG connector
  MCUCR=(1<<JTD); 
  MCUCR=(1<<JTD);
#endif
  
  // Finish init of mult extruder arrays 
  for(int e = 0; e < EXTRUDERS; e++) {
    // populate with the first value 
    maxttemp[e] = maxttemp[0];
#ifdef PIDTEMP
    iState_sum_min[e] = 0.0;
    iState_sum_max[e] = PID_INTEGRAL_DRIVE_MAX / cs.Ki;
#endif //PIDTEMP
#ifdef PIDTEMPBED
    temp_iState_min_bed = 0.0;
    temp_iState_max_bed = PID_INTEGRAL_DRIVE_MAX / cs.bedKi;
#endif //PIDTEMPBED
  }

  #if defined(HEATER_0_PIN) && (HEATER_0_PIN > -1) 
    SET_OUTPUT(HEATER_0_PIN);
  #endif  
  #if defined(HEATER_1_PIN) && (HEATER_1_PIN > -1) 
    SET_OUTPUT(HEATER_1_PIN);
  #endif  
  #if defined(HEATER_2_PIN) && (HEATER_2_PIN > -1) 
    SET_OUTPUT(HEATER_2_PIN);
  #endif  
  #if defined(HEATER_BED_PIN) && (HEATER_BED_PIN > -1) 
    SET_OUTPUT(HEATER_BED_PIN);
  #endif  
  #if defined(FAN_PIN) && (FAN_PIN > -1) 
    SET_OUTPUT(FAN_PIN);
    #ifdef FAST_PWM_FAN
    setPwmFrequency(FAN_PIN, 1); // No prescaling. Pwm frequency = F_CPU/256/8
    #endif
    #ifdef FAN_SOFT_PWM
    soft_pwm_fan = fanSpeedSoftPwm / (1 << (8 - FAN_SOFT_PWM_BITS));
    #endif
  #endif

  #ifdef HEATER_0_USES_MAX6675
    #ifndef SDSUPPORT
      SET_OUTPUT(SCK_PIN);
      WRITE(SCK_PIN,0);
    
      SET_OUTPUT(MOSI_PIN);
      WRITE(MOSI_PIN,1);
    
      SET_INPUT(MISO_PIN);
      WRITE(MISO_PIN,1);
    #endif
    /* Using pinMode and digitalWrite, as that was the only way I could get it to compile */
    
    //Have to toggle SD card CS pin to low first, to enable firmware to talk with SD card
	pinMode(SS_PIN, OUTPUT);
	digitalWrite(SS_PIN,0);  
	pinMode(MAX6675_SS, OUTPUT);
	digitalWrite(MAX6675_SS,1);
  #endif

#ifdef HEATER_0_MINTEMP
  minttemp[0] = HEATER_0_MINTEMP;
  while(analog2temp(minttemp_raw[0], 0) < HEATER_0_MINTEMP) {
#if HEATER_0_RAW_LO_TEMP < HEATER_0_RAW_HI_TEMP
    minttemp_raw[0] += OVERSAMPLENR;
#else
    minttemp_raw[0] -= OVERSAMPLENR;
#endif
  }
#endif //MINTEMP
#ifdef HEATER_0_MAXTEMP
  maxttemp[0] = HEATER_0_MAXTEMP;
  while(analog2temp(maxttemp_raw[0], 0) > HEATER_0_MAXTEMP) {
#if HEATER_0_RAW_LO_TEMP < HEATER_0_RAW_HI_TEMP
    maxttemp_raw[0] -= OVERSAMPLENR;
#else
    maxttemp_raw[0] += OVERSAMPLENR;
#endif
  }
#endif //MAXTEMP

#if (EXTRUDERS > 1) && defined(HEATER_1_MINTEMP)
  minttemp[1] = HEATER_1_MINTEMP;
  while(analog2temp(minttemp_raw[1], 1) < HEATER_1_MINTEMP) {
#if HEATER_1_RAW_LO_TEMP < HEATER_1_RAW_HI_TEMP
    minttemp_raw[1] += OVERSAMPLENR;
#else
    minttemp_raw[1] -= OVERSAMPLENR;
#endif
  }
#endif // MINTEMP 1
#if (EXTRUDERS > 1) && defined(HEATER_1_MAXTEMP)
  maxttemp[1] = HEATER_1_MAXTEMP;
  while(analog2temp(maxttemp_raw[1], 1) > HEATER_1_MAXTEMP) {
#if HEATER_1_RAW_LO_TEMP < HEATER_1_RAW_HI_TEMP
    maxttemp_raw[1] -= OVERSAMPLENR;
#else
    maxttemp_raw[1] += OVERSAMPLENR;
#endif
  }
#endif //MAXTEMP 1

#if (EXTRUDERS > 2) && defined(HEATER_2_MINTEMP)
  minttemp[2] = HEATER_2_MINTEMP;
  while(analog2temp(minttemp_raw[2], 2) < HEATER_2_MINTEMP) {
#if HEATER_2_RAW_LO_TEMP < HEATER_2_RAW_HI_TEMP
    minttemp_raw[2] += OVERSAMPLENR;
#else
    minttemp_raw[2] -= OVERSAMPLENR;
#endif
  }
#endif //MINTEMP 2
#if (EXTRUDERS > 2) && defined(HEATER_2_MAXTEMP)
  maxttemp[2] = HEATER_2_MAXTEMP;
  while(analog2temp(maxttemp_raw[2], 2) > HEATER_2_MAXTEMP) {
#if HEATER_2_RAW_LO_TEMP < HEATER_2_RAW_HI_TEMP
    maxttemp_raw[2] -= OVERSAMPLENR;
#else
    maxttemp_raw[2] += OVERSAMPLENR;
#endif
  }
#endif //MAXTEMP 2

#ifdef BED_MINTEMP
  while(analog2tempBed(bed_minttemp_raw) < BED_MINTEMP) {
#if HEATER_BED_RAW_LO_TEMP < HEATER_BED_RAW_HI_TEMP
    bed_minttemp_raw += OVERSAMPLENR;
#else
    bed_minttemp_raw -= OVERSAMPLENR;
#endif
  }
#endif //BED_MINTEMP
#ifdef BED_MAXTEMP
  while(analog2tempBed(bed_maxttemp_raw) > BED_MAXTEMP) {
#if HEATER_BED_RAW_LO_TEMP < HEATER_BED_RAW_HI_TEMP
    bed_maxttemp_raw -= OVERSAMPLENR;
#else
    bed_maxttemp_raw += OVERSAMPLENR;
#endif
  }
#endif //BED_MAXTEMP

#ifdef AMBIENT_MINTEMP
  while(analog2tempAmbient(ambient_minttemp_raw) < AMBIENT_MINTEMP) {
#if AMBIENT_RAW_LO_TEMP < AMBIENT_RAW_HI_TEMP
    ambient_minttemp_raw += OVERSAMPLENR;
#else
    ambient_minttemp_raw -= OVERSAMPLENR;
#endif
  }
#endif //AMBIENT_MINTEMP
#ifdef AMBIENT_MAXTEMP
  while(analog2tempAmbient(ambient_maxttemp_raw) > AMBIENT_MAXTEMP) {
#if AMBIENT_RAW_LO_TEMP < AMBIENT_RAW_HI_TEMP
    ambient_maxttemp_raw -= OVERSAMPLENR;
#else
    ambient_maxttemp_raw += OVERSAMPLENR;
#endif
  }
#endif //AMBIENT_MAXTEMP

  timer0_init(); //enables the heatbed timer.

  // timer2 already enabled earlier in the code
  // now enable the COMPB temperature interrupt
  OCR2B = 128;
  ENABLE_SOFT_PWM_INTERRUPT();

  timer4_init(); //for tone and Hotend fan PWM
}

#if (defined (TEMP_RUNAWAY_BED_HYSTERESIS) && TEMP_RUNAWAY_BED_TIMEOUT > 0) || (defined (TEMP_RUNAWAY_EXTRUDER_HYSTERESIS) && TEMP_RUNAWAY_EXTRUDER_TIMEOUT > 0)
static void temp_runaway_check(uint8_t _heater_id, float _target_temperature, float _current_temperature, float _output, bool _isbed)
{
	float __delta;
	float __hysteresis = 0;
	uint16_t __timeout = 0;
	bool temp_runaway_check_active = false;
	static float __preheat_start[2] = { 0,0}; //currently just bed and one extruder
	static uint8_t __preheat_counter[2] = { 0,0};
	static uint8_t __preheat_errors[2] = { 0,0};

	if (_millis() - temp_runaway_timer[_heater_id] > 2000)
	{

#ifdef 	TEMP_RUNAWAY_BED_TIMEOUT
          if (_isbed)
          {
               __hysteresis = TEMP_RUNAWAY_BED_HYSTERESIS;
               __timeout = TEMP_RUNAWAY_BED_TIMEOUT;
          }
#endif
#ifdef 	TEMP_RUNAWAY_EXTRUDER_TIMEOUT
          if (!_isbed)
          {
               __hysteresis = TEMP_RUNAWAY_EXTRUDER_HYSTERESIS;
               __timeout = TEMP_RUNAWAY_EXTRUDER_TIMEOUT;
          }
#endif

		temp_runaway_timer[_heater_id] = _millis();
		if (_output == 0)
		{
			temp_runaway_check_active = false;
			temp_runaway_error_counter[_heater_id] = 0;
		}

		if (temp_runaway_target[_heater_id] != _target_temperature)
		{
			if (_target_temperature > 0)
			{
				temp_runaway_status[_heater_id] = TempRunaway_PREHEAT;
				temp_runaway_target[_heater_id] = _target_temperature;
				__preheat_start[_heater_id] = _current_temperature;
				__preheat_counter[_heater_id] = 0;
			}
			else
			{
				temp_runaway_status[_heater_id] = TempRunaway_INACTIVE;
				temp_runaway_target[_heater_id] = _target_temperature;
			}
		}

		if ((_current_temperature < _target_temperature)  && (temp_runaway_status[_heater_id] == TempRunaway_PREHEAT))
		{
			__preheat_counter[_heater_id]++;
			if (__preheat_counter[_heater_id] > ((_isbed) ? 16 : 8)) // periodicaly check if current temperature changes
			{
				/*SERIAL_ECHOPGM("Heater:");
				MYSERIAL.print(_heater_id);
				SERIAL_ECHOPGM(" T:");
				MYSERIAL.print(_current_temperature);
				SERIAL_ECHOPGM(" Tstart:");
				MYSERIAL.print(__preheat_start[_heater_id]);
				SERIAL_ECHOPGM(" delta:");
				MYSERIAL.print(_current_temperature-__preheat_start[_heater_id]);*/
				
//-//				if (_current_temperature - __preheat_start[_heater_id] < 2) {
//-//				if (_current_temperature - __preheat_start[_heater_id] < ((_isbed && (_current_temperature>105.0))?0.6:2.0)) {
                    __delta=2.0;
                    if(_isbed)
                         {
                         __delta=3.0;
                         if(_current_temperature>90.0) __delta=2.0;
                         if(_current_temperature>105.0) __delta=0.6;
                         }
				if (_current_temperature - __preheat_start[_heater_id] < __delta) {
					__preheat_errors[_heater_id]++;
					/*SERIAL_ECHOPGM(" Preheat errors:");
					MYSERIAL.println(__preheat_errors[_heater_id]);*/
				}
				else {
					//SERIAL_ECHOLNPGM("");
					__preheat_errors[_heater_id] = 0;
				}

				if (__preheat_errors[_heater_id] > ((_isbed) ? 3 : 5)) 
                    set_temp_error((_isbed?TempErrorSource::bed:TempErrorSource::hotend), _heater_id, TempErrorType::preheat);

				__preheat_start[_heater_id] = _current_temperature;
				__preheat_counter[_heater_id] = 0;
			}
		}

//-//		if (_current_temperature >= _target_temperature  && temp_runaway_status[_heater_id] == TempRunaway_PREHEAT)
		if ((_current_temperature > (_target_temperature - __hysteresis))  && temp_runaway_status[_heater_id] == TempRunaway_PREHEAT)
		{
			/*SERIAL_ECHOPGM("Heater:");
			MYSERIAL.print(_heater_id);
			MYSERIAL.println(" ->tempRunaway");*/
			temp_runaway_status[_heater_id] = TempRunaway_ACTIVE;
			temp_runaway_check_active = false;
			temp_runaway_error_counter[_heater_id] = 0;
		}

		if (_output > 0)
		{
			temp_runaway_check_active = true;
		}


		if (temp_runaway_check_active)
		{			
			//	we are in range
			if ((_current_temperature > (_target_temperature - __hysteresis)) && (_current_temperature < (_target_temperature + __hysteresis)))
			{
				temp_runaway_check_active = false;
				temp_runaway_error_counter[_heater_id] = 0;
			}
			else
			{
				if (temp_runaway_status[_heater_id] > TempRunaway_PREHEAT)
				{
					temp_runaway_error_counter[_heater_id]++;
					if (temp_runaway_error_counter[_heater_id] * 2 > __timeout)
                        set_temp_error((_isbed?TempErrorSource::bed:TempErrorSource::hotend), _heater_id, TempErrorType::runaway);
				}
			}
		}

	}
}

static void temp_runaway_stop(bool isPreheat, bool isBed)
{
    if(IsStopped() == false) {
        if (isPreheat) {
            lcd_setalertstatuspgm(isBed? PSTR("BED PREHEAT ERROR") : PSTR("PREHEAT ERROR"), LCD_STATUS_CRITICAL);
            SERIAL_ERROR_START;
            if (isBed) {
                SERIAL_ERRORLNPGM(" THERMAL RUNAWAY (PREHEAT HEATBED)");
            } else {
                SERIAL_ERRORLNPGM(" THERMAL RUNAWAY (PREHEAT HOTEND)");
            }
        } else {
            lcd_setalertstatuspgm(isBed? PSTR("BED THERMAL RUNAWAY") : PSTR("THERMAL RUNAWAY"), LCD_STATUS_CRITICAL);
            SERIAL_ERROR_START;
            if (isBed) {
                SERIAL_ERRORLNPGM(" HEATBED THERMAL RUNAWAY");
            } else {
                SERIAL_ERRORLNPGM(" HOTEND THERMAL RUNAWAY");
            }
        }
        prusa_statistics(0);
        prusa_statistics(isPreheat? 91 : 90);
    }
    ThermalStop();
}
#endif

//! signal a temperature error on both the lcd and serial
//! @param type short error abbreviation (PROGMEM)
//! @param e optional extruder index for hotend errors
static void temp_error_messagepgm(const char* PROGMEM type, uint8_t e = EXTRUDERS)
{
    char msg[LCD_WIDTH];
    strcpy_P(msg, PSTR("Err: "));
    strcat_P(msg, type);
    lcd_setalertstatus(msg, LCD_STATUS_CRITICAL);

    SERIAL_ERROR_START;

    if(e != EXTRUDERS) {
        SERIAL_ERROR((int)e);
        SERIAL_ERRORPGM(": ");
    }

    SERIAL_ERRORPGM("Heaters switched off. ");
    SERIAL_ERRORRPGM(type);
    SERIAL_ERRORLNPGM(" triggered!");
}


static void max_temp_error(uint8_t e) {
    if(IsStopped() == false) {
        temp_error_messagepgm(PSTR("MAXTEMP"), e);
        prusa_statistics(93);
    }
#ifndef BOGUS_TEMPERATURE_FAILSAFE_OVERRIDE
    ThermalStop();
#endif
}

static void min_temp_error(uint8_t e) {
    static const char err[] PROGMEM = "MINTEMP";
    if(IsStopped() == false) {
        temp_error_messagepgm(err, e);
        prusa_statistics(92);
    }
    ThermalStop();
}

static void bed_max_temp_error(void) {
    if(IsStopped() == false) {
        temp_error_messagepgm(PSTR("MAXTEMP BED"));
    }
    ThermalStop();
}

static void bed_min_temp_error(void) {
    static const char err[] PROGMEM = "MINTEMP BED";
    if(IsStopped() == false) {
        temp_error_messagepgm(err);
	}
    ThermalStop();
}


#ifdef AMBIENT_THERMISTOR
static void ambient_max_temp_error(void) {
    if(IsStopped() == false) {
        temp_error_messagepgm(PSTR("MAXTEMP AMB"));
    }
    ThermalStop();
}

static void ambient_min_temp_error(void) {
    if(IsStopped() == false) {
        temp_error_messagepgm(PSTR("MINTEMP AMB"));
    }
    ThermalStop();
}
#endif


#ifdef HEATER_0_USES_MAX6675
#define MAX6675_HEAT_INTERVAL 250
long max6675_previous_millis = MAX6675_HEAT_INTERVAL;
int max6675_temp = 2000;

int read_max6675()
{
  if (_millis() - max6675_previous_millis < MAX6675_HEAT_INTERVAL) 
    return max6675_temp;
  
  max6675_previous_millis = _millis();
  max6675_temp = 0;
    
  #ifdef	PRR
    PRR &= ~(1<<PRSPI);
  #elif defined PRR0
    PRR0 &= ~(1<<PRSPI);
  #endif
  
  SPCR = (1<<MSTR) | (1<<SPE) | (1<<SPR0);
  
  // enable TT_MAX6675
  WRITE(MAX6675_SS, 0);
  
  // ensure 100ns delay - a bit extra is fine
  asm("nop");//50ns on 20Mhz, 62.5ns on 16Mhz
  asm("nop");//50ns on 20Mhz, 62.5ns on 16Mhz
  
  // read MSB
  SPDR = 0;
  for (;(SPSR & (1<<SPIF)) == 0;);
  max6675_temp = SPDR;
  max6675_temp <<= 8;
  
  // read LSB
  SPDR = 0;
  for (;(SPSR & (1<<SPIF)) == 0;);
  max6675_temp |= SPDR;
  
  // disable TT_MAX6675
  WRITE(MAX6675_SS, 1);

  if (max6675_temp & 4) 
  {
    // thermocouple open
    max6675_temp = 2000;
  }
  else 
  {
    max6675_temp = max6675_temp >> 3;
  }

  return max6675_temp;
}
#endif

#ifdef BABYSTEPPING
FORCE_INLINE static void applyBabysteps() {
  for(uint8_t axis=0;axis<3;axis++)
  {
    int curTodo=babystepsTodo[axis]; //get rid of volatile for performance

    if(curTodo>0)
{
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        babystep(axis,/*fwd*/true);
        babystepsTodo[axis]--; //less to do next time
      }
    }
    else
    if(curTodo<0)
    {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        babystep(axis,/*fwd*/false);
        babystepsTodo[axis]++; //less to do next time
      }
    }
  }
}
#endif //BABYSTEPPING

FORCE_INLINE static void soft_pwm_core()
{
  static uint8_t pwm_count = (1 << SOFT_PWM_SCALE);
  static uint8_t soft_pwm_0;
#ifdef SLOW_PWM_HEATERS
  static unsigned char slow_pwm_count = 0;
  static unsigned char state_heater_0 = 0;
  static unsigned char state_timer_heater_0 = 0;
#endif
#if (EXTRUDERS > 1) || defined(HEATERS_PARALLEL)
  static unsigned char soft_pwm_1;
#ifdef SLOW_PWM_HEATERS
  static unsigned char state_heater_1 = 0;
  static unsigned char state_timer_heater_1 = 0;
#endif
#endif
#if EXTRUDERS > 2
  static unsigned char soft_pwm_2;
#ifdef SLOW_PWM_HEATERS
  static unsigned char state_heater_2 = 0;
  static unsigned char state_timer_heater_2 = 0;
#endif
#endif
#if HEATER_BED_PIN > -1
  // @@DR static unsigned char soft_pwm_b;
#ifdef SLOW_PWM_HEATERS
  static unsigned char state_heater_b = 0;
  static unsigned char state_timer_heater_b = 0;
#endif
#endif

#ifndef SLOW_PWM_HEATERS
  /*
   * standard PWM modulation
   */
  if (pwm_count == 0)
  {
    soft_pwm_0 = soft_pwm[0];
    if(soft_pwm_0 > 0)
	{ 
      WRITE(HEATER_0_PIN,1);
#ifdef HEATERS_PARALLEL
      WRITE(HEATER_1_PIN,1);
#endif
    } else WRITE(HEATER_0_PIN,0);
#if EXTRUDERS > 1
    soft_pwm_1 = soft_pwm[1];
    if(soft_pwm_1 > 0) WRITE(HEATER_1_PIN,1); else WRITE(HEATER_1_PIN,0);
#endif
#if EXTRUDERS > 2
    soft_pwm_2 = soft_pwm[2];
    if(soft_pwm_2 > 0) WRITE(HEATER_2_PIN,1); else WRITE(HEATER_2_PIN,0);
#endif
  }
#if defined(HEATER_BED_PIN) && HEATER_BED_PIN > -1
  
#if 0  // @@DR vypnuto pro hw pwm bedu
  // tuhle prasarnu bude potreba poustet ve stanovenych intervalech, jinak nemam moc sanci zareagovat
  // teoreticky by se tato cast uz vubec nemusela poustet
  if ((pwm_count & ((1 << HEATER_BED_SOFT_PWM_BITS) - 1)) == 0)
  {
    soft_pwm_b = soft_pwm_bed >> (7 - HEATER_BED_SOFT_PWM_BITS);
#  ifndef SYSTEM_TIMER_2
	// tady budu krokovat pomalou frekvenci na automatu - tohle je rizeni spinani a rozepinani
	// jako ridici frekvenci mam 2khz, jako vystupni frekvenci mam 30hz
	// 2kHz jsou ovsem ve slysitelnem pasmu, mozna bude potreba jit s frekvenci nahoru (a tomu taky prizpusobit ostatni veci)
	// Teoreticky bych mohl stahnout OCR0B citac na 6, cimz bych se dostal nekam ke 40khz a tady potom honit PWM rychleji nebo i pomaleji
	// to nicemu nevadi. Soft PWM scale by se 20x zvetsilo (no dobre, 16x), cimz by se to posunulo k puvodnimu 30Hz PWM
	//if(soft_pwm_b > 0) WRITE(HEATER_BED_PIN,1); else WRITE(HEATER_BED_PIN,0);
#  endif //SYSTEM_TIMER_2
  }
#endif
#endif
  
#ifdef FAN_SOFT_PWM
  if ((pwm_count & ((1 << FAN_SOFT_PWM_BITS) - 1)) == 0)
  {
    soft_pwm_fan = fanSpeedSoftPwm / (1 << (8 - FAN_SOFT_PWM_BITS));
    if(soft_pwm_fan > 0) WRITE(FAN_PIN,1); else WRITE(FAN_PIN,0);
  }
#endif
  if(soft_pwm_0 < pwm_count)
  { 
    WRITE(HEATER_0_PIN,0);
#ifdef HEATERS_PARALLEL
    WRITE(HEATER_1_PIN,0);
#endif
  }

#if EXTRUDERS > 1
  if(soft_pwm_1 < pwm_count) WRITE(HEATER_1_PIN,0);
#endif
#if EXTRUDERS > 2
  if(soft_pwm_2 < pwm_count) WRITE(HEATER_2_PIN,0);
#endif

#if 0 // @@DR  
#if defined(HEATER_BED_PIN) && HEATER_BED_PIN > -1
  if (soft_pwm_b < (pwm_count & ((1 << HEATER_BED_SOFT_PWM_BITS) - 1))){
	  //WRITE(HEATER_BED_PIN,0);
  }
  //WRITE(HEATER_BED_PIN, pwm_count & 1 );
#endif
#endif
#ifdef FAN_SOFT_PWM
  if (soft_pwm_fan < (pwm_count & ((1 << FAN_SOFT_PWM_BITS) - 1))) WRITE(FAN_PIN,0);
#endif
  
  pwm_count += (1 << SOFT_PWM_SCALE);
  pwm_count &= 0x7f;

#else //ifndef SLOW_PWM_HEATERS
  /*
   * SLOW PWM HEATERS
   *
   * for heaters drived by relay
   */
#ifndef MIN_STATE_TIME
#define MIN_STATE_TIME 16 // MIN_STATE_TIME * 65.5 = time in milliseconds
#endif
  if (slow_pwm_count == 0) {
    // EXTRUDER 0 
    soft_pwm_0 = soft_pwm[0];
    if (soft_pwm_0 > 0) {
      // turn ON heather only if the minimum time is up 
      if (state_timer_heater_0 == 0) { 
	// if change state set timer 
	if (state_heater_0 == 0) {
	  state_timer_heater_0 = MIN_STATE_TIME;
	}
	state_heater_0 = 1;
	WRITE(HEATER_0_PIN, 1);
#ifdef HEATERS_PARALLEL
	WRITE(HEATER_1_PIN, 1);
#endif
      }
    } else {
      // turn OFF heather only if the minimum time is up 
      if (state_timer_heater_0 == 0) {
	// if change state set timer 
	if (state_heater_0 == 1) {
	  state_timer_heater_0 = MIN_STATE_TIME;
	}
	state_heater_0 = 0;
	WRITE(HEATER_0_PIN, 0);
#ifdef HEATERS_PARALLEL
	WRITE(HEATER_1_PIN, 0);
#endif
      }
    }
    
#if EXTRUDERS > 1
    // EXTRUDER 1
    soft_pwm_1 = soft_pwm[1];
    if (soft_pwm_1 > 0) {
      // turn ON heather only if the minimum time is up 
      if (state_timer_heater_1 == 0) { 
	// if change state set timer 
	if (state_heater_1 == 0) {
	  state_timer_heater_1 = MIN_STATE_TIME;
	}
	state_heater_1 = 1;
	WRITE(HEATER_1_PIN, 1);
      }
    } else {
      // turn OFF heather only if the minimum time is up 
      if (state_timer_heater_1 == 0) {
	// if change state set timer 
	if (state_heater_1 == 1) {
	  state_timer_heater_1 = MIN_STATE_TIME;
	}
	state_heater_1 = 0;
	WRITE(HEATER_1_PIN, 0);
      }
    }
#endif
    
#if EXTRUDERS > 2
    // EXTRUDER 2
    soft_pwm_2 = soft_pwm[2];
    if (soft_pwm_2 > 0) {
      // turn ON heather only if the minimum time is up 
      if (state_timer_heater_2 == 0) { 
	// if change state set timer 
	if (state_heater_2 == 0) {
	  state_timer_heater_2 = MIN_STATE_TIME;
	}
	state_heater_2 = 1;
	WRITE(HEATER_2_PIN, 1);
      }
    } else {
      // turn OFF heather only if the minimum time is up 
      if (state_timer_heater_2 == 0) {
	// if change state set timer 
	if (state_heater_2 == 1) {
	  state_timer_heater_2 = MIN_STATE_TIME;
	}
	state_heater_2 = 0;
	WRITE(HEATER_2_PIN, 0);
      }
    }
#endif
    
#if defined(HEATER_BED_PIN) && HEATER_BED_PIN > -1
    // BED
    soft_pwm_b = soft_pwm_bed;
    if (soft_pwm_b > 0) {
      // turn ON heather only if the minimum time is up 
      if (state_timer_heater_b == 0) { 
	// if change state set timer 
	if (state_heater_b == 0) {
	  state_timer_heater_b = MIN_STATE_TIME;
	}
	state_heater_b = 1;
	//WRITE(HEATER_BED_PIN, 1);
      }
    } else {
      // turn OFF heather only if the minimum time is up 
      if (state_timer_heater_b == 0) {
	// if change state set timer 
	if (state_heater_b == 1) {
	  state_timer_heater_b = MIN_STATE_TIME;
	}
	state_heater_b = 0;
	WRITE(HEATER_BED_PIN, 0);
      }
    }
#endif
  } // if (slow_pwm_count == 0)
  
  // EXTRUDER 0 
  if (soft_pwm_0 < slow_pwm_count) {
    // turn OFF heather only if the minimum time is up 
    if (state_timer_heater_0 == 0) { 
      // if change state set timer 
      if (state_heater_0 == 1) {
	state_timer_heater_0 = MIN_STATE_TIME;
      }
      state_heater_0 = 0;
      WRITE(HEATER_0_PIN, 0);
#ifdef HEATERS_PARALLEL
      WRITE(HEATER_1_PIN, 0);
#endif
    }
  }
    
#if EXTRUDERS > 1
  // EXTRUDER 1 
  if (soft_pwm_1 < slow_pwm_count) {
    // turn OFF heather only if the minimum time is up 
    if (state_timer_heater_1 == 0) { 
      // if change state set timer 
      if (state_heater_1 == 1) {
	state_timer_heater_1 = MIN_STATE_TIME;
      }
      state_heater_1 = 0;
      WRITE(HEATER_1_PIN, 0);
    }
  }
#endif
  
#if EXTRUDERS > 2
  // EXTRUDER 2
  if (soft_pwm_2 < slow_pwm_count) {
    // turn OFF heather only if the minimum time is up 
    if (state_timer_heater_2 == 0) { 
      // if change state set timer 
      if (state_heater_2 == 1) {
	state_timer_heater_2 = MIN_STATE_TIME;
      }
      state_heater_2 = 0;
      WRITE(HEATER_2_PIN, 0);
    }
  }
#endif
  
#if defined(HEATER_BED_PIN) && HEATER_BED_PIN > -1
  // BED
  if (soft_pwm_b < slow_pwm_count) {
    // turn OFF heather only if the minimum time is up 
    if (state_timer_heater_b == 0) { 
      // if change state set timer 
      if (state_heater_b == 1) {
	state_timer_heater_b = MIN_STATE_TIME;
      }
      state_heater_b = 0;
      WRITE(HEATER_BED_PIN, 0);
    }
  }
#endif
  
#ifdef FAN_SOFT_PWM
  if ((pwm_count & ((1 << FAN_SOFT_PWM_BITS) - 1)) == 0)
    soft_pwm_fan = fanSpeedSoftPwm / (1 << (8 - FAN_SOFT_PWM_BITS));
    if (soft_pwm_fan > 0) WRITE(FAN_PIN,1); else WRITE(FAN_PIN,0);
  }
  if (soft_pwm_fan < pwm_count) WRITE(FAN_PIN,0);
#endif

  pwm_count += (1 << SOFT_PWM_SCALE);
  pwm_count &= 0x7f;
  
  // increment slow_pwm_count only every 64 pwm_count circa 65.5ms
  if ((pwm_count % 64) == 0) {
    slow_pwm_count++;
    slow_pwm_count &= 0x7f;
    
    // Extruder 0
    if (state_timer_heater_0 > 0) {
      state_timer_heater_0--;
    } 
  
#if EXTRUDERS > 1
    // Extruder 1
    if (state_timer_heater_1 > 0) 
      state_timer_heater_1--;
#endif
    
#if EXTRUDERS > 2
    // Extruder 2
    if (state_timer_heater_2 > 0) 
      state_timer_heater_2--;
#endif
    
#if defined(HEATER_BED_PIN) && HEATER_BED_PIN > -1
    // Bed   
    if (state_timer_heater_b > 0) 
      state_timer_heater_b--;
#endif
  } //if ((pwm_count % 64) == 0) {
  
#endif //ifndef SLOW_PWM_HEATERS
}

FORCE_INLINE static void soft_pwm_isr()
{
  lcd_buttons_update();
  soft_pwm_core();

#ifdef BABYSTEPPING
  applyBabysteps();
#endif //BABYSTEPPING

  // Check if a stack overflow happened
  if (!SdFatUtil::test_stack_integrity()) stack_error();

#if (defined(FANCHECK) && defined(TACH_0) && (TACH_0 > -1))
  readFanTach();
#endif //(defined(TACH_0))
}

// Timer2 (originaly timer0) is shared with millies
#ifdef SYSTEM_TIMER_2
ISR(TIMER2_COMPB_vect)
#else //SYSTEM_TIMER_2
ISR(TIMER0_COMPB_vect)
#endif //SYSTEM_TIMER_2
{
    DISABLE_SOFT_PWM_INTERRUPT();
    NONATOMIC_BLOCK(NONATOMIC_FORCEOFF) {
        soft_pwm_isr();
    }
    ENABLE_SOFT_PWM_INTERRUPT();
}

void check_max_temp_raw()
{
    //heater
#if HEATER_0_RAW_LO_TEMP > HEATER_0_RAW_HI_TEMP
    if (current_temperature_raw[0] <= maxttemp_raw[0]) {
#else
    if (current_temperature_raw[0] >= maxttemp_raw[0]) {
#endif
        set_temp_error(TempErrorSource::hotend, 0, TempErrorType::max);
    }
    //bed
#if defined(BED_MAXTEMP) && (TEMP_SENSOR_BED != 0)
#if HEATER_BED_RAW_LO_TEMP > HEATER_BED_RAW_HI_TEMP
    if (current_temperature_bed_raw <= bed_maxttemp_raw) {
#else
    if (current_temperature_bed_raw >= bed_maxttemp_raw) {
#endif
        set_temp_error(TempErrorSource::bed, 0, TempErrorType::max);
    }
#endif
    //ambient
#if defined(AMBIENT_MAXTEMP) && (TEMP_SENSOR_AMBIENT != 0)
#if AMBIENT_RAW_LO_TEMP > AMBIENT_RAW_HI_TEMP
    if (current_temperature_raw_ambient <= ambient_maxttemp_raw) {
#else
    if (current_temperature_raw_ambient >= ambient_maxttemp_raw) {
#endif
        set_temp_error(TempErrorSource::ambient, 0, TempErrorType::max);
    }
#endif
}

//! number of repeating the same state with consecutive step() calls
//! used to slow down text switching
struct alert_automaton_mintemp {
	const char *m2;
	alert_automaton_mintemp(const char *m2):m2(m2){}
private:
	enum { ALERT_AUTOMATON_SPEED_DIV = 5 };
	enum class States : uint8_t { Init = 0, TempAboveMintemp, ShowPleaseRestart, ShowMintemp };
	States state = States::Init;
	uint8_t repeat = ALERT_AUTOMATON_SPEED_DIV;

	void substep(const char* next_msg, States next_state){
		if( repeat == 0 ){
			state = next_state; // advance to the next state
			lcd_setalertstatuspgm(next_msg, LCD_STATUS_CRITICAL);
			repeat = ALERT_AUTOMATON_SPEED_DIV; // and prepare repeating for it too
		} else {
			--repeat;
		}
	}
public:
	//! brief state automaton step routine
	//! @param current_temp current hotend/bed temperature (for computing simple hysteresis)
	//! @param mintemp minimal temperature including hysteresis to check current_temp against
	void step(float current_temp, float mintemp){
		static const char m1[] PROGMEM = "Please restart";
		switch(state){
		case States::Init: // initial state - check hysteresis
			if( current_temp > mintemp ){
				lcd_setalertstatuspgm(m2, LCD_STATUS_CRITICAL);
				state = States::TempAboveMintemp;
			}
			// otherwise keep the Err MINTEMP alert message on the display,
			// i.e. do not transfer to state 1
			break;
		case States::TempAboveMintemp: // the temperature has risen above the hysteresis check
		case States::ShowMintemp: // displaying "MINTEMP fixed"
			substep(m1, States::ShowPleaseRestart);
			break;
		case States::ShowPleaseRestart: // displaying "Please restart"
			substep(m2, States::ShowMintemp);
			break;
		}
	}
};
static const char m2hotend[] PROGMEM = "MINTEMP HOTEND fixed";
static const char m2bed[] PROGMEM = "MINTEMP BED fixed";
static alert_automaton_mintemp alert_automaton_hotend(m2hotend), alert_automaton_bed(m2bed);

void check_min_temp_heater0()
{
#if HEATER_0_RAW_LO_TEMP > HEATER_0_RAW_HI_TEMP
	if (current_temperature_raw[0] >= minttemp_raw[0]) {
#else
	if (current_temperature_raw[0] <= minttemp_raw[0]) {
#endif
        set_temp_error(TempErrorSource::hotend, 0, TempErrorType::min);
	}
}

void check_min_temp_bed()
{
#if HEATER_BED_RAW_LO_TEMP > HEATER_BED_RAW_HI_TEMP
	if (current_temperature_bed_raw >= bed_minttemp_raw) {
#else
	if (current_temperature_bed_raw <= bed_minttemp_raw) {
#endif
        set_temp_error(TempErrorSource::bed, 0, TempErrorType::min);
	}
}

#ifdef AMBIENT_MINTEMP
void check_min_temp_ambient()
{
#if AMBIENT_RAW_LO_TEMP > AMBIENT_RAW_HI_TEMP
	if (current_temperature_raw_ambient >= ambient_minttemp_raw) {
#else
	if (current_temperature_raw_ambient <= ambient_minttemp_raw) {
#endif
        set_temp_error(TempErrorSource::ambient, 0, TempErrorType::min);
	}
}
#endif

void handle_temp_error()
{
    // relay to the original handler
    switch((TempErrorType)temp_error_state.type) {
    case TempErrorType::min:
        switch((TempErrorSource)temp_error_state.source) {
        case TempErrorSource::hotend:
            if(temp_error_state.assert) {
                min_temp_error(temp_error_state.index);
            } else {
                // no recovery, just force the user to restart the printer
                // which is a safer variant than just continuing printing
                // The automaton also checks for hysteresis - the temperature must have reached a few degrees above the MINTEMP, before
                // we shall signalize, that MINTEMP has been fixed
                // Code notice: normally the alert_automaton instance would have been placed here
                // as static alert_automaton_mintemp alert_automaton_hotend, but
                alert_automaton_hotend.step(current_temperature[0], minttemp[0] + TEMP_HYSTERESIS);
            }
            break;
        case TempErrorSource::bed:
            if(temp_error_state.assert) {
                bed_min_temp_error();
            } else {
                // no recovery, just force the user to restart the printer
                // which is a safer variant than just continuing printing
                alert_automaton_bed.step(current_temperature_bed, BED_MINTEMP + TEMP_HYSTERESIS);
            }
            break;
#ifdef AMBIENT_THERMISTOR
        case TempErrorSource::ambient:
            ambient_min_temp_error();
            break;
#endif
        }
        break;
    case TempErrorType::max:
        switch((TempErrorSource)temp_error_state.source) {
        case TempErrorSource::hotend:
            max_temp_error(temp_error_state.index);
            break;
        case TempErrorSource::bed:
            bed_max_temp_error();
            break;
#ifdef AMBIENT_THERMISTOR
        case TempErrorSource::ambient:
            ambient_max_temp_error();
            break;
#endif
        }
        break;
    case TempErrorType::preheat:
    case TempErrorType::runaway:
        switch((TempErrorSource)temp_error_state.source) {
        case TempErrorSource::hotend:
        case TempErrorSource::bed:
            temp_runaway_stop(
                ((TempErrorType)temp_error_state.type == TempErrorType::preheat),
                ((TempErrorSource)temp_error_state.source == TempErrorSource::bed));
            break;
#ifdef AMBIENT_THERMISTOR
        case TempErrorSource::ambient:
            // not needed
            break;
#endif
        }
        break;
#ifdef TEMP_MODEL
    case TempErrorType::model:
        if(temp_error_state.assert) {
            if(IsStopped() == false) {
                SERIAL_ECHOLNPGM("TM: error triggered!");
            }
            ThermalStop(true);
            WRITE(BEEPER, HIGH);
        } else {
            temp_error_state.v = 0;
            WRITE(BEEPER, LOW);

            // hotend error was transitory and disappeared, re-enable bed
            if (!target_temperature_bed)
                target_temperature_bed = saved_bed_temperature;

            SERIAL_ECHOLNPGM("TM: error cleared");
        }
        break;
#endif
    }
}

#ifdef PIDTEMP
// Apply the scale factors to the PID values

float scalePID_i(float i)
{
	return i*PID_dT;
}

float unscalePID_i(float i)
{
	return i/PID_dT;
}

float scalePID_d(float d)
{
    return d/PID_dT;
}

float unscalePID_d(float d)
{
	return d*PID_dT;
}

#endif //PIDTEMP

#ifdef PINDA_THERMISTOR
//! @brief PINDA thermistor detected
//!
//! @retval true firmware should do temperature compensation and allow calibration
//! @retval false PINDA thermistor is not detected, disable temperature compensation and calibration
//! @retval true/false when forced via LCD menu Settings->HW Setup->SuperPINDA
//!
bool has_temperature_compensation()
{
#ifdef SUPERPINDA_SUPPORT
#ifdef PINDA_TEMP_COMP
   	uint8_t pinda_temp_compensation = eeprom_read_byte((uint8_t*)EEPROM_PINDA_TEMP_COMPENSATION);
    if (pinda_temp_compensation == EEPROM_EMPTY_VALUE) //Unkown PINDA temp compenstation, so check it.
      {
#endif //PINDA_TEMP_COMP
        return (current_temperature_pinda >= PINDA_MINTEMP) ? true : false;
#ifdef PINDA_TEMP_COMP
      }
    else if (pinda_temp_compensation == 0) return true; //Overwritten via LCD menu SuperPINDA [No]
    else return false; //Overwritten via LCD menu SuperPINDA [YES]
#endif //PINDA_TEMP_COMP
#else
    return true;
#endif
}
#endif //PINDA_THERMISTOR


// RAII helper class to run a code block with temp_mgr_isr disabled
class TempMgrGuard
{
    bool temp_mgr_state;

public:
    TempMgrGuard() {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            temp_mgr_state = TEMP_MGR_INTERRUPT_STATE();
            DISABLE_TEMP_MGR_INTERRUPT();
        }
    }

    ~TempMgrGuard() throw() {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            if(temp_mgr_state) ENABLE_TEMP_MGR_INTERRUPT();
        }
    }
};

void temp_mgr_init()
{
    // initialize the ADC and start a conversion
    adc_init();
    adc_start_cycle();

    // initialize temperature timer
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

        // CTC
        TCCRxB &= ~(1<<WGMx3);
        TCCRxB |=  (1<<WGMx2);
        TCCRxA &= ~(1<<WGMx1);
        TCCRxA &= ~(1<<WGMx0);

        // output mode = 00 (disconnected)
        TCCRxA &= ~(3<<COMxA0);
        TCCRxA &= ~(3<<COMxB0);

        // x/256 prescaler
        TCCRxB |=  (1<<CSx2);
        TCCRxB &= ~(1<<CSx1);
        TCCRxB &= ~(1<<CSx0);

        // reset counter
        TCNTx = 0;
        OCRxA = TEMP_TIM_OCRA_OVF;

        // clear pending interrupts, enable COMPA
        TEMP_MGR_INT_FLAG_CLEAR();
        ENABLE_TEMP_MGR_INTERRUPT();

    }
}

static void pid_heater(uint8_t e, const float current, const int target)
{
    float pid_input;
    float pid_output;

#ifdef PIDTEMP
    pid_input = current;

#ifndef PID_OPENLOOP
    if(target == 0) {
        pid_output = 0;
        pid_reset[e] = true;
    } else {
        pid_error[e] = target - pid_input;
        if(pid_reset[e]) {
            iState_sum[e] = 0.0;
            dTerm[e] = 0.0;                       // 'dState_last[e]' initial setting is not necessary (see end of if-statement)
            pid_reset[e] = false;
        }
#ifndef PonM
        pTerm[e] = cs.Kp * pid_error[e];
        iState_sum[e] += pid_error[e];
        iState_sum[e] = constrain(iState_sum[e], iState_sum_min[e], iState_sum_max[e]);
        iTerm[e] = cs.Ki * iState_sum[e];
        // PID_K1 defined in Configuration.h in the PID settings
#define K2 (1.0-PID_K1)
        dTerm[e] = (cs.Kd * (pid_input - dState_last[e]))*K2 + (PID_K1 * dTerm[e]); // e.g. digital filtration of derivative term changes
        pid_output = pTerm[e] + iTerm[e] - dTerm[e]; // subtraction due to "Derivative on Measurement" method (i.e. derivative of input instead derivative of error is used)
        if (pid_output > PID_MAX) {
            if (pid_error[e] > 0 ) iState_sum[e] -= pid_error[e]; // conditional un-integration
            pid_output=PID_MAX;
        } else if (pid_output < 0) {
            if (pid_error[e] < 0 ) iState_sum[e] -= pid_error[e]; // conditional un-integration
            pid_output=0;
        }
#else // PonM ("Proportional on Measurement" method)
        iState_sum[e] += cs.Ki * pid_error[e];
        iState_sum[e] -= cs.Kp * (pid_input - dState_last[e]);
        iState_sum[e] = constrain(iState_sum[e], 0, PID_INTEGRAL_DRIVE_MAX);
        dTerm[e] = cs.Kd * (pid_input - dState_last[e]);
        pid_output = iState_sum[e] - dTerm[e];  // subtraction due to "Derivative on Measurement" method (i.e. derivative of input instead derivative of error is used)
        pid_output = constrain(pid_output, 0, PID_MAX);
#endif // PonM
    }
    dState_last[e] = pid_input;
#else //PID_OPENLOOP
    pid_output = constrain(target[e], 0, PID_MAX);
#endif //PID_OPENLOOP

#ifdef PID_DEBUG
    SERIAL_ECHO_START;
    SERIAL_ECHO(" PID_DEBUG ");
    SERIAL_ECHO(e);
    SERIAL_ECHO(": Input ");
    SERIAL_ECHO(pid_input);
    SERIAL_ECHO(" Output ");
    SERIAL_ECHO(pid_output);
    SERIAL_ECHO(" pTerm ");
    SERIAL_ECHO(pTerm[e]);
    SERIAL_ECHO(" iTerm ");
    SERIAL_ECHO(iTerm[e]);
    SERIAL_ECHO(" dTerm ");
    SERIAL_ECHOLN(-dTerm[e]);
#endif //PID_DEBUG

#else /* PID off */
    pid_output = 0;
    if(current[e] < target[e]) {
        pid_output = PID_MAX;
    }
#endif

    // Check if temperature is within the correct range
    if((current < maxttemp[e]) && (target != 0))
        soft_pwm[e] = (int)pid_output >> 1;
    else
        soft_pwm[e] = 0;
}

static void pid_bed(const float current, const int target)
{
    float pid_input;
    float pid_output;

#ifndef PIDTEMPBED
    if(_millis() - previous_millis_bed_heater < BED_CHECK_INTERVAL)
        return;
    previous_millis_bed_heater = _millis();
#endif

#if TEMP_SENSOR_BED != 0

#ifdef PIDTEMPBED
    pid_input = current;

#ifndef PID_OPENLOOP
    pid_error_bed = target - pid_input;
    pTerm_bed = cs.bedKp * pid_error_bed;
    temp_iState_bed += pid_error_bed;
    temp_iState_bed = constrain(temp_iState_bed, temp_iState_min_bed, temp_iState_max_bed);
    iTerm_bed = cs.bedKi * temp_iState_bed;

    //PID_K1 defined in Configuration.h in the PID settings
#define K2 (1.0-PID_K1)
    dTerm_bed= (cs.bedKd * (pid_input - temp_dState_bed))*K2 + (PID_K1 * dTerm_bed);
    temp_dState_bed = pid_input;

    pid_output = pTerm_bed + iTerm_bed - dTerm_bed;
    if (pid_output > MAX_BED_POWER) {
        if (pid_error_bed > 0 )  temp_iState_bed -= pid_error_bed; // conditional un-integration
        pid_output=MAX_BED_POWER;
    } else if (pid_output < 0){
        if (pid_error_bed < 0 )  temp_iState_bed -= pid_error_bed; // conditional un-integration
        pid_output=0;
    }

#else
    pid_output = constrain(target, 0, MAX_BED_POWER);
#endif //PID_OPENLOOP

    if(current < BED_MAXTEMP)
    {
        soft_pwm_bed = (int)pid_output >> 1;
        timer02_set_pwm0(soft_pwm_bed << 1);
    }
    else
    {
        soft_pwm_bed = 0;
        timer02_set_pwm0(soft_pwm_bed << 1);
    }

#elif !defined(BED_LIMIT_SWITCHING)
    // Check if temperature is within the correct range
    if(current < BED_MAXTEMP)
    {
        if(current >= target)
        {
            soft_pwm_bed = 0;
            timer02_set_pwm0(soft_pwm_bed << 1);
        }
        else
        {
            soft_pwm_bed = MAX_BED_POWER>>1;
            timer02_set_pwm0(soft_pwm_bed << 1);
        }
    }
    else
    {
        soft_pwm_bed = 0;
        timer02_set_pwm0(soft_pwm_bed << 1);
        WRITE(HEATER_BED_PIN,LOW);
    }
#else //#ifdef BED_LIMIT_SWITCHING
    // Check if temperature is within the correct band
    if(current < BED_MAXTEMP)
    {
        if(current > target + BED_HYSTERESIS)
        {
            soft_pwm_bed = 0;
            timer02_set_pwm0(soft_pwm_bed << 1);
        }
        else if(current <= target - BED_HYSTERESIS)
        {
            soft_pwm_bed = MAX_BED_POWER>>1;
            timer02_set_pwm0(soft_pwm_bed << 1);
        }
    }
    else
    {
        soft_pwm_bed = 0;
        timer02_set_pwm0(soft_pwm_bed << 1);
        WRITE(HEATER_BED_PIN,LOW);
    }
#endif //BED_LIMIT_SWITCHING

    if(target==0)
    {
        soft_pwm_bed = 0;
        timer02_set_pwm0(soft_pwm_bed << 1);
    }
#endif //TEMP_SENSOR_BED
}

// ISR-safe temperatures
static volatile bool adc_values_ready = false;
float current_temperature_isr[EXTRUDERS];
int target_temperature_isr[EXTRUDERS];
float current_temperature_bed_isr;
int target_temperature_bed_isr;
#ifdef PINDA_THERMISTOR
float current_temperature_pinda_isr;
#endif
#ifdef AMBIENT_THERMISTOR
float current_temperature_ambient_isr;
#endif

// ISR callback from adc when sampling finished
void adc_callback()
{
    current_temperature_raw[0] = adc_values[ADC_PIN_IDX(TEMP_0_PIN)]; //heater
    current_temperature_bed_raw = adc_values[ADC_PIN_IDX(TEMP_BED_PIN)];
#ifdef PINDA_THERMISTOR
    current_temperature_raw_pinda = adc_values[ADC_PIN_IDX(TEMP_PINDA_PIN)];
#endif //PINDA_THERMISTOR
#ifdef AMBIENT_THERMISTOR
    current_temperature_raw_ambient = adc_values[ADC_PIN_IDX(TEMP_AMBIENT_PIN)]; // 5->6
#endif //AMBIENT_THERMISTOR
#ifdef VOLT_PWR_PIN
    current_voltage_raw_pwr = adc_values[ADC_PIN_IDX(VOLT_PWR_PIN)];
#endif
#ifdef VOLT_BED_PIN
    current_voltage_raw_bed = adc_values[ADC_PIN_IDX(VOLT_BED_PIN)]; // 6->9
#endif
#ifdef IR_SENSOR_ANALOG
    current_voltage_raw_IR = adc_values[ADC_PIN_IDX(VOLT_IR_PIN)];
#endif //IR_SENSOR_ANALOG
    adc_values_ready = true;
}

static void setCurrentTemperaturesFromIsr()
{
    for(uint8_t e=0;e<EXTRUDERS;e++)
        current_temperature[e] = current_temperature_isr[e];
    current_temperature_bed = current_temperature_bed_isr;
#ifdef PINDA_THERMISTOR
    current_temperature_pinda = current_temperature_pinda_isr;
#endif
#ifdef AMBIENT_THERMISTOR
    current_temperature_ambient = current_temperature_ambient_isr;
#endif
}

static void setIsrTargetTemperatures()
{
    for(uint8_t e=0;e<EXTRUDERS;e++)
        target_temperature_isr[e] = target_temperature[e];
    target_temperature_bed_isr = target_temperature_bed;
}

/* Synchronize temperatures:
   - fetch updated values from temp_mgr_isr to current values
   - update target temperatures for temp_mgr_isr regulation *if* no temperature error is set
   This function is blocking: check temp_meas_ready before calling! */
static void updateTemperatures()
{
    TempMgrGuard temp_mgr_guard;
    setCurrentTemperaturesFromIsr();
    if(!temp_error_state.v) {
        // refuse to update target temperatures in any error condition!
        setIsrTargetTemperatures();
    }
    temp_meas_ready = false;
}

/* Convert raw values into actual temperatures for temp_mgr. The raw values are created in the ADC
   interrupt context, while this function runs from temp_mgr_isr which *is* preemptible as
   analog2temp is relatively slow */
static void setIsrTemperaturesFromRawValues()
{
    for(uint8_t e=0;e<EXTRUDERS;e++)
        current_temperature_isr[e] = analog2temp(current_temperature_raw[e], e);
    current_temperature_bed_isr = analog2tempBed(current_temperature_bed_raw);
#ifdef PINDA_THERMISTOR
    current_temperature_pinda_isr = analog2tempBed(current_temperature_raw_pinda);
#endif
#ifdef AMBIENT_THERMISTOR
    current_temperature_ambient_isr = analog2tempAmbient(current_temperature_raw_ambient); //thermistor for ambient is NTCG104LH104JT1 (2000)
#endif
    temp_meas_ready = true;
}

static void temp_mgr_pid()
{
    for(uint8_t e = 0; e < EXTRUDERS; e++)
        pid_heater(e, current_temperature_isr[e], target_temperature_isr[e]);
    pid_bed(current_temperature_bed_isr, target_temperature_bed_isr);
}

static void check_temp_runaway()
{
#ifdef TEMP_RUNAWAY_EXTRUDER_HYSTERESIS
    for(uint8_t e = 0; e < EXTRUDERS; e++)
        temp_runaway_check(e+1, target_temperature_isr[e], current_temperature_isr[e], soft_pwm[e], false);
#endif
#ifdef TEMP_RUNAWAY_BED_HYSTERESIS
    temp_runaway_check(0, target_temperature_bed_isr, current_temperature_bed_isr, soft_pwm_bed, true);
#endif
}

static void check_temp_raw();

static void temp_mgr_isr()
{
    // update *_isr temperatures from raw values for PID regulation
    setIsrTemperaturesFromRawValues();

    // clear the error assertion flag before checking again
    temp_error_state.assert = false;
    check_temp_raw(); // check min/max temp using raw values
    check_temp_runaway(); // classic temperature hysteresis check
#ifdef TEMP_MODEL
    temp_model::check(); // model-based heater check
#ifdef TEMP_MODEL_DEBUG
    temp_model::log_isr();
#endif
#endif

    // PID regulation
    if (pid_tuning_finished)
        temp_mgr_pid();
}

ISR(TIMERx_COMPA_vect)
{
    // immediately schedule a new conversion
    if(adc_values_ready != true) return;
    adc_values_ready = false;
    adc_start_cycle();

    // run temperature management with interrupts enabled to reduce latency
    DISABLE_TEMP_MGR_INTERRUPT();
    NONATOMIC_BLOCK(NONATOMIC_FORCEOFF) {
        temp_mgr_isr();
    }
    ENABLE_TEMP_MGR_INTERRUPT();
}

void disable_heater()
{
  setTargetHotend(0);
  setTargetBed(0);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      // propagate all values down the chain
      setIsrTargetTemperatures();
      temp_mgr_pid();

      // we can't call soft_pwm_core directly to toggle the pins as it would require removing the inline
      // attribute, so disable each pin individually
#if defined(HEATER_0_PIN) && HEATER_0_PIN > -1 && EXTRUDERS > 0
      WRITE(HEATER_0_PIN,LOW);
#endif
#if defined(HEATER_1_PIN) && HEATER_1_PIN > -1 && EXTRUDERS > 1
      WRITE(HEATER_1_PIN,LOW);
#endif
#if defined(HEATER_2_PIN) && HEATER_2_PIN > -1 && EXTRUDERS > 2
      WRITE(HEATER_2_PIN,LOW);
#endif
#if defined(HEATER_BED_PIN) && HEATER_BED_PIN > -1
      // TODO: this doesn't take immediate effect!
      timer02_set_pwm0(0);
      bedPWMDisabled = 0;
#endif
  }
}

static void check_min_temp_raw()
{
    static bool bCheckingOnHeater = false; // state variable, which allows to short no-checking delay (is set, when temperature is (first time) over heaterMintemp)
    static bool bCheckingOnBed    = false; // state variable, which allows to short no-checking delay (is set, when temperature is (first time) over bedMintemp)
    static ShortTimer oTimer4minTempHeater;
    static ShortTimer oTimer4minTempBed;

#ifdef AMBIENT_THERMISTOR
#ifdef AMBIENT_MINTEMP
    // we need to check ambient temperature
    check_min_temp_ambient();
#endif
#if AMBIENT_RAW_LO_TEMP > AMBIENT_RAW_HI_TEMP
    if(current_temperature_raw_ambient>(OVERSAMPLENR*MINTEMP_MINAMBIENT_RAW)) // thermistor is NTC type
#else
    if(current_temperature_raw_ambient=<(OVERSAMPLENR*MINTEMP_MINAMBIENT_RAW))
#endif
    {
        // ambient temperature is low
#endif //AMBIENT_THERMISTOR
        // *** 'common' part of code for MK2.5 & MK3
        // * nozzle checking
        if(target_temperature_isr[active_extruder]>minttemp[active_extruder]) {
            // ~ nozzle heating is on
            bCheckingOnHeater=bCheckingOnHeater||(current_temperature_isr[active_extruder]>(minttemp[active_extruder]+TEMP_HYSTERESIS)); // for eventually delay cutting
            if(oTimer4minTempHeater.expired(HEATER_MINTEMP_DELAY)||(!oTimer4minTempHeater.running())||bCheckingOnHeater) {
                bCheckingOnHeater=true;   // not necessary
                check_min_temp_heater0(); // delay is elapsed or temperature is/was over minTemp => periodical checking is active
            }
        }
        else {
            // ~ nozzle heating is off
            oTimer4minTempHeater.start();
            bCheckingOnHeater=false;
        }
        // * bed checking
        if(target_temperature_bed_isr>BED_MINTEMP) {
            // ~ bed heating is on
            bCheckingOnBed=bCheckingOnBed||(current_temperature_bed_isr>(BED_MINTEMP+TEMP_HYSTERESIS)); // for eventually delay cutting
            if(oTimer4minTempBed.expired(BED_MINTEMP_DELAY)||(!oTimer4minTempBed.running())||bCheckingOnBed) {
                bCheckingOnBed=true;  // not necessary
                check_min_temp_bed(); // delay is elapsed or temperature is/was over minTemp => periodical checking is active
            }
        }
        else {
            // ~ bed heating is off
            oTimer4minTempBed.start();
            bCheckingOnBed=false;
        }
        // *** end of 'common' part
#ifdef AMBIENT_THERMISTOR
    }
    else {
        // ambient temperature is standard
        check_min_temp_heater0();
        check_min_temp_bed();
    }
#endif //AMBIENT_THERMISTOR
}

static void check_temp_raw()
{
    // order is relevant: check_min_temp_raw requires max to be reliable due to
    // ambient temperature being used for low handling temperatures
    check_max_temp_raw();
    check_min_temp_raw();
}

#ifdef TEMP_MODEL
namespace temp_model {

void model_data::reset(uint8_t heater_pwm _UNUSED, uint8_t fan_pwm _UNUSED,
    float heater_temp _UNUSED, float ambient_temp _UNUSED)
{
    // pre-compute invariant values
    C_i = (TEMP_MGR_INTV / C);
    warn_s = warn * TEMP_MGR_INTV;
    err_s = err * TEMP_MGR_INTV;
    dT_lag_size = L / (uint16_t)(TEMP_MGR_INTV * 1000);

    // initial values
    for(uint8_t i = 0; i != TEMP_MODEL_MAX_LAG_SIZE; ++i)
        dT_lag_buf[i] = NAN;
    dT_lag_idx = 0;
    dT_err_prev = 0;
    T_prev = NAN;

    // clear the initialization flag
    flag_bits.uninitialized = false;
}

static constexpr float iir_mul(const float a, const float b, const float f, const float nanv)
{
    const float a_ = !isnan(a) ? a : nanv;
    return (a_ * (1.f - f)) + (b * f);
}

void model_data::step(uint8_t heater_pwm, uint8_t fan_pwm, float heater_temp, float ambient_temp)
{
    constexpr float soft_pwm_inv = 1. / ((1 << 7) - 1);

    // input values
    const float heater_scale = soft_pwm_inv * heater_pwm;
    const float cur_heater_temp = heater_temp;
    const float cur_ambient_temp = ambient_temp + Ta_corr;
    const float cur_R = R[fan_pwm]; // resistance at current fan power (K/W)

    float dP = P * heater_scale; // current power [W]
    dP *= (cur_heater_temp * U) + V; // linear temp. correction
    float dPl = (cur_heater_temp - cur_ambient_temp) / cur_R; // [W] leakage power
    float dT = (dP - dPl) * C_i; // expected temperature difference (K)

    // filter and lag dT
    uint8_t dT_next_idx = (dT_lag_idx == (dT_lag_size - 1) ? 0: dT_lag_idx + 1);
    float dT_lag = dT_lag_buf[dT_next_idx];
    float dT_lag_prev = dT_lag_buf[dT_lag_idx];
    float dT_f = iir_mul(dT_lag_prev, dT, fS, dT);
    dT_lag_buf[dT_next_idx] = dT_f;
    dT_lag_idx = dT_next_idx;

    // calculate and filter dT_err
    float dT_err = (cur_heater_temp - T_prev) - dT_lag;
    float dT_err_f = iir_mul(dT_err_prev, dT_err, TEMP_MODEL_fE, 0.);
    T_prev = cur_heater_temp;
    dT_err_prev = dT_err_f;

    // check and trigger errors
    flag_bits.error = (fabsf(dT_err_f) > err_s);
    flag_bits.warning = (fabsf(dT_err_f) > warn_s);
}

// clear error flags and mark as uninitialized
static void reinitialize()
{
    data.flags = 1; // shorcut to reset all error flags
    warning_state.assert = false; // explicitly clear assertions
}

// verify calibration status and trigger a model reset if valid
static void setup()
{
    if(!calibrated()) enabled = false;
    reinitialize();
}

static bool calibrated()
{
    if(!(data.P > 0)) return false;
    if(isnan(data.U)) return false;
    if(isnan(data.V)) return false;
    if(!(data.C > 0)) return false;
    if(isnan(data.fS)) return false;
    if(!(data.L > 0)) return false;
    if(!(data.Ta_corr != NAN)) return false;
    for(uint8_t i = 0; i != TEMP_MODEL_R_SIZE; ++i) {
        if(!(temp_model::data.R[i] >= 0))
            return false;
    }
    if(!(data.warn != NAN)) return false;
    if(!(data.err != NAN)) return false;
    return true;
}

static void check()
{
    if(!enabled) return;

    uint8_t heater_pwm = soft_pwm[0];
    uint8_t fan_pwm = soft_pwm_fan;
    float heater_temp = current_temperature_isr[0];
    float ambient_temp = current_temperature_ambient_isr;

    // check if a reset is required to seed the model: this needs to be done with valid
    // ADC values, so we can't do that directly in init()
    if(data.flag_bits.uninitialized)
        data.reset(heater_pwm, fan_pwm, heater_temp, ambient_temp);

    // step the model
    data.step(heater_pwm, fan_pwm, heater_temp, ambient_temp);

    // handle errors
    if(data.flag_bits.error)
        set_temp_error(TempErrorSource::hotend, 0, TempErrorType::model);

    // handle warning conditions as lower-priority but with greater feedback
    warning_state.assert = data.flag_bits.warning;
    if(warning_state.assert) {
        warning_state.warning = true;
        warning_state.dT_err = temp_model::data.dT_err_prev;
    }
}

static void handle_warning()
{
    // update values
    float warn = data.warn;
    float dT_err;
    {
        TempMgrGuard temp_mgr_guard;
        dT_err = warning_state.dT_err;
    }
    dT_err /= TEMP_MGR_INTV; // per-sample => K/s

    printf_P(PSTR("TM: error |%f|>%f\n"), (double)dT_err, (double)warn);

    static bool first = true;
    if(warning_state.assert) {
        if (first) {
            if(warn_beep) {
                lcd_setalertstatuspgm(_T(MSG_THERMAL_ANOMALY), LCD_STATUS_INFO);
                WRITE(BEEPER, HIGH);
            }
            first = false;
        } else {
            if(warn_beep) TOGGLE(BEEPER);
        }
    } else {
        // warning cleared, reset state
        warning_state.warning = false;
        if(warn_beep) WRITE(BEEPER, LOW);
        first = true;
    }
}

#ifdef TEMP_MODEL_DEBUG
static void log_usr()
{
    if(!log_buf.enabled) return;

    uint8_t counter = log_buf.entry.counter;
    if (counter == log_buf.serial) return;

    int8_t delta_ms;
    uint8_t cur_pwm;

    // avoid strict-aliasing warnings
    union { float cur_temp; uint32_t cur_temp_b; };
    union { float cur_amb; uint32_t cur_amb_b; };

    {
        TempMgrGuard temp_mgr_guard;
        delta_ms = log_buf.entry.delta_ms;
        counter = log_buf.entry.counter;
        cur_pwm = log_buf.entry.cur_pwm;
        cur_temp = log_buf.entry.cur_temp;
        cur_amb = log_buf.entry.cur_amb;
    }

    uint8_t d = counter - log_buf.serial;
    log_buf.serial = counter;

    printf_P(PSTR("TML %d %d %x %lx %lx\n"), (unsigned)d - 1, (int)delta_ms + 1,
        (int)cur_pwm, (unsigned long)cur_temp_b, (unsigned long)cur_amb_b);
}

static void log_isr()
{
    if(!log_buf.enabled) return;

    uint32_t stamp = _millis();
    uint8_t delta_ms = stamp - log_buf.entry.stamp - (uint32_t)(TEMP_MGR_INTV * 1000);
    log_buf.entry.stamp = stamp;

    ++log_buf.entry.counter;
    log_buf.entry.delta_ms = delta_ms;
    log_buf.entry.cur_pwm = soft_pwm[0];
    log_buf.entry.cur_temp = current_temperature_isr[0];
    log_buf.entry.cur_amb = current_temperature_ambient_isr;
}
#endif

} // namespace temp_model

static void temp_model_reset_enabled(bool enabled)
{
    TempMgrGuard temp_mgr_guard;
    temp_model::enabled = enabled;
    temp_model::reinitialize();
}

bool temp_model_enabled()
{
    return temp_model::enabled;
}

void temp_model_set_enabled(bool enabled)
{
    // set the enabled flag
    {
        TempMgrGuard temp_mgr_guard;
        temp_model::enabled = enabled;
        temp_model::setup();
    }

    // verify that the model has been enabled
    if(enabled && !temp_model::enabled)
        SERIAL_ECHOLNPGM("TM: invalid parameters, cannot enable");
}

void temp_model_set_warn_beep(bool enabled)
{
    temp_model::warn_beep = enabled;
}

// set the model lag rounding to the effective sample resolution, ensuring the reported/stored lag
// matches the current model constraints (future-proofing for model changes)
static void temp_model_set_lag(uint16_t ms)
{
    static const uint16_t intv_ms = (uint16_t)(TEMP_MGR_INTV * 1000);
    uint16_t samples = ((ms + intv_ms/2) / intv_ms);

    // ensure we do not exceed the maximum lag buffer and have at least one lag sample for filtering
    if(samples < 1)
        samples = 1;
    else if(samples > TEMP_MODEL_MAX_LAG_SIZE)
        samples = TEMP_MODEL_MAX_LAG_SIZE;

    // round back to ms
    temp_model::data.L = samples * intv_ms;
}

void temp_model_set_params(float P, float U, float V, float C, float D, int16_t L, float Ta_corr, float warn, float err)
{
    TempMgrGuard temp_mgr_guard;

    if(!isnan(P) && P > 0) temp_model::data.P = P;
    if(!isnan(U)) temp_model::data.U = U;
    if(!isnan(V)) temp_model::data.V = V;
    if(!isnan(C) && C > 0) temp_model::data.C = C;
    if(!isnan(D)) temp_model::data.fS = D;
    if(L >= 0) temp_model_set_lag(L);
    if(!isnan(Ta_corr)) temp_model::data.Ta_corr = Ta_corr;
    if(!isnan(warn) && warn > 0) temp_model::data.warn = warn;
    if(!isnan(err) && err > 0) temp_model::data.err = err;

    // ensure warn <= err
    if (temp_model::data.warn > temp_model::data.err)
        temp_model::data.warn = temp_model::data.err;

    temp_model::setup();
}

void temp_model_set_resistance(uint8_t index, float R)
{
    if(index >= TEMP_MODEL_R_SIZE || R <= 0)
        return;

    TempMgrGuard temp_mgr_guard;
    temp_model::data.R[index] = R;
    temp_model::setup();
}

void temp_model_report_settings()
{
    SERIAL_ECHO_START;
    SERIAL_ECHOLNPGM("Temperature Model settings:");
    for(uint8_t i = 0; i != TEMP_MODEL_R_SIZE; ++i)
        printf_P(PSTR("%S  M310 I%u R%.2f\n"), echomagic, (unsigned)i, (double)temp_model::data.R[i]);
    printf_P(PSTR("%S  M310 P%.2f U%.4f V%.2f C%.2f D%.4f L%u S%u B%u E%.2f W%.2f T%.2f\n"),
        echomagic, (double)temp_model::data.P, (double)temp_model::data.U, (double)temp_model::data.V,
        (double)temp_model::data.C, (double)temp_model::data.fS, (unsigned)temp_model::data.L,
        (unsigned)temp_model::enabled, (unsigned)temp_model::warn_beep,
        (double)temp_model::data.err, (double)temp_model::data.warn,
        (double)temp_model::data.Ta_corr);
}

void temp_model_reset_settings()
{
    TempMgrGuard temp_mgr_guard;

    temp_model::data.P = TEMP_MODEL_DEF(P);
    temp_model::data.U = TEMP_MODEL_DEF(U);
    temp_model::data.V = TEMP_MODEL_DEF(V);
    temp_model::data.C = TEMP_MODEL_DEF(C);
    temp_model::data.fS = TEMP_MODEL_DEF(fS);
    temp_model::data.L = (uint16_t)(TEMP_MODEL_DEF(LAG) / (TEMP_MGR_INTV * 1000) + 0.5) * (uint16_t)(TEMP_MGR_INTV * 1000);
    for(uint8_t i = 0; i != TEMP_MODEL_R_SIZE; ++i)
        temp_model::data.R[i] = pgm_read_float(TEMP_MODEL_R_DEFAULT + i);
    temp_model::data.Ta_corr = TEMP_MODEL_Ta_corr;
    temp_model::data.warn = TEMP_MODEL_DEF(W);
    temp_model::data.err = TEMP_MODEL_DEF(E);
    temp_model::warn_beep = true;
    temp_model::enabled = true;
    temp_model::reinitialize();
}

void temp_model_load_settings()
{
    static_assert(TEMP_MODEL_R_SIZE == 16); // ensure we don't desync with the eeprom table
    TempMgrGuard temp_mgr_guard;

    // handle upgrade from a model without UVDL (FW<3.13, TM VER<1): model is retro-compatible,
    // reset UV to an identity without doing any special handling
    eeprom_init_default_float((float*)EEPROM_TEMP_MODEL_U, TEMP_MODEL_DEF(U));
    eeprom_init_default_float((float*)EEPROM_TEMP_MODEL_V, TEMP_MODEL_DEF(V));
    eeprom_init_default_float((float*)EEPROM_TEMP_MODEL_D, TEMP_MODEL_DEF(fS));
    eeprom_init_default_word((uint16_t*)EEPROM_TEMP_MODEL_L, TEMP_MODEL_DEF(LAG));
    eeprom_init_default_byte((uint8_t*)EEPROM_TEMP_MODEL_VER, TEMP_MODEL_DEF(VER));

    temp_model::enabled = eeprom_read_byte((uint8_t*)EEPROM_TEMP_MODEL_ENABLE);
    temp_model::data.P = eeprom_read_float((float*)EEPROM_TEMP_MODEL_P);
    temp_model::data.U = eeprom_read_float((float*)EEPROM_TEMP_MODEL_U);
    temp_model::data.V = eeprom_read_float((float*)EEPROM_TEMP_MODEL_V);
    temp_model::data.C = eeprom_read_float((float*)EEPROM_TEMP_MODEL_C);
    temp_model::data.fS = eeprom_read_float((float*)EEPROM_TEMP_MODEL_D);
    temp_model_set_lag(eeprom_read_word((uint16_t*)EEPROM_TEMP_MODEL_L));
    for(uint8_t i = 0; i != TEMP_MODEL_R_SIZE; ++i)
        temp_model::data.R[i] = eeprom_read_float((float*)EEPROM_TEMP_MODEL_R + i);
    temp_model::data.Ta_corr = eeprom_read_float((float*)EEPROM_TEMP_MODEL_Ta_corr);
    temp_model::data.warn = eeprom_read_float((float*)EEPROM_TEMP_MODEL_W);
    temp_model::data.err = eeprom_read_float((float*)EEPROM_TEMP_MODEL_E);

    if(!temp_model::calibrated()) {
        SERIAL_ECHOLNPGM("TM: stored calibration invalid, resetting");
        temp_model_reset_settings();
    }
    temp_model::setup();
}

void temp_model_save_settings()
{
    eeprom_update_byte((uint8_t*)EEPROM_TEMP_MODEL_ENABLE, temp_model::enabled);
    eeprom_update_float((float*)EEPROM_TEMP_MODEL_P, temp_model::data.P);
    eeprom_update_float((float*)EEPROM_TEMP_MODEL_U, temp_model::data.U);
    eeprom_update_float((float*)EEPROM_TEMP_MODEL_V, temp_model::data.V);
    eeprom_update_float((float*)EEPROM_TEMP_MODEL_C, temp_model::data.C);
    eeprom_update_float((float*)EEPROM_TEMP_MODEL_D, temp_model::data.fS);
    eeprom_update_word((uint16_t*)EEPROM_TEMP_MODEL_L, temp_model::data.L);
    for(uint8_t i = 0; i != TEMP_MODEL_R_SIZE; ++i)
        eeprom_update_float((float*)EEPROM_TEMP_MODEL_R + i, temp_model::data.R[i]);
    eeprom_update_float((float*)EEPROM_TEMP_MODEL_Ta_corr, temp_model::data.Ta_corr);
    eeprom_update_float((float*)EEPROM_TEMP_MODEL_W, temp_model::data.warn);
    eeprom_update_float((float*)EEPROM_TEMP_MODEL_E, temp_model::data.err);
}

namespace temp_model_cal {

// set current fan speed for both front/backend
static __attribute__((noinline)) void set_fan_speed(uint8_t fan_speed)
{
    // reset the fan measuring state due to missing hysteresis handling on the checking side
    resetFanCheck();

    fanSpeed = fan_speed;
#ifdef FAN_SOFT_PWM
    fanSpeedSoftPwm = fan_speed;
#endif
}

static void waiting_handler()
{
    manage_heater();
    host_keepalive();
    host_autoreport();
    checkFans();
    lcd_update(0);
}

static void wait(unsigned ms)
{
    unsigned long mark = _millis() + ms;
    while(_millis() < mark) {
        if(temp_error_state.v) break;
        waiting_handler();
    }
}

static void __attribute__((noinline)) wait_temp()
{
    while(current_temperature[0] < (target_temperature[0] - TEMP_HYSTERESIS)) {
        if(temp_error_state.v) break;
        waiting_handler();
    }
}

static void cooldown(float temp)
{
    uint8_t old_speed = fanSpeed;
    set_fan_speed(255);
    while(current_temperature[0] >= temp) {
        if(temp_error_state.v) break;
        float ambient = current_temperature_ambient + temp_model::data.Ta_corr;
        if(current_temperature[0] < (ambient + TEMP_HYSTERESIS)) {
            // do not get stuck waiting very close to ambient temperature
            break;
        }
        waiting_handler();
    }
    set_fan_speed(old_speed);
}

static uint16_t record(uint16_t samples = REC_BUFFER_SIZE) {
    TempMgrGuard temp_mgr_guard;

    uint16_t pos = 0;
    while(pos < samples) {
        if(!TEMP_MGR_INT_FLAG_STATE()) {
            // temperatures not ready yet, just manage heaters while waiting to reduce jitter
            manage_heater();
            continue;
        }
        TEMP_MGR_INT_FLAG_CLEAR();

        // manually repeat what the regular isr would do
        if(adc_values_ready != true) continue;
        adc_values_ready = false;
        adc_start_cycle();
        temp_mgr_isr();

        // stop recording for an hard error condition
        if(temp_error_state.v)
            return 0;

        // record a new entry
        rec_entry& entry = rec_buffer[pos];
        entry.temp = current_temperature_isr[0];
        entry.pwm = soft_pwm[0];
        ++pos;

        // it's now safer to give regular serial/lcd updates a shot
        waiting_handler();
    }

    return pos;
}

static float cost_fn(uint16_t samples, float* const var, float v, uint8_t fan_pwm, float ambient)
{
    *var = v;
    temp_model::data.reset(rec_buffer[0].pwm, fan_pwm, rec_buffer[0].temp, ambient);
    float err = 0;
    uint16_t cnt = 0;
    for(uint16_t i = 1; i < samples; ++i) {
        temp_model::data.step(rec_buffer[i].pwm, fan_pwm, rec_buffer[i].temp, ambient);
        float err_v = temp_model::data.dT_err_prev;
        if(!isnan(err_v)) {
            err += err_v * err_v;
            ++cnt;
        }
    }
    return cnt ? (err / cnt) : NAN;
}

constexpr float GOLDEN_RATIO = 0.6180339887498949;

static void update_section(float points[2], const float bounds[2])
{
    float d = GOLDEN_RATIO * (bounds[1] - bounds[0]);
    points[0] = bounds[0] + d;
    points[1] = bounds[1] - d;
}

static float estimate(uint16_t samples,
    float* const var, float min, float max,
    float thr, uint16_t max_itr,
    uint8_t fan_pwm, float ambient)
{
    // during estimation we alter the model values without an extra copy to conserve memory
    // so we cannot keep the main checker active until a value has been found
    bool was_enabled = temp_model::enabled;
    temp_model_reset_enabled(false);

    float orig = *var;
    float e = NAN;
    float points[2];
    float bounds[2] = {min, max};
    update_section(points, bounds);

    for(uint8_t it = 0; it != max_itr; ++it) {
        float c1 = cost_fn(samples, var, points[0], fan_pwm, ambient);
        float c2 = cost_fn(samples, var, points[1], fan_pwm, ambient);
        bool dir = (c2 < c1);
        bounds[dir] = points[!dir];
        update_section(points, bounds);
        float x = points[!dir];
        e = (1-GOLDEN_RATIO) * fabsf((bounds[0]-bounds[1]) / x);

        printf_P(PSTR("TM iter:%u v:%.2f e:%.3f\n"), it, x, e);
        if(e < thr) {
            if(x == min || x == max) {
                // real value likely outside of the search boundaries
                break;
            }

            *var = x;
            temp_model_reset_enabled(was_enabled);
            return e;
        }
    }

    SERIAL_ECHOLNPGM("TM estimation did not converge");
    *var = orig;
    temp_model_reset_enabled(was_enabled);
    return NAN;
}

static bool autotune(int16_t cal_temp)
{
    uint16_t samples;
    float e;
    char tm_message[LCD_WIDTH+1];

    // bootstrap C/R values without fan
    set_fan_speed(0);

    for(uint8_t i = 0; i != 2; ++i) {
        const char* PROGMEM verb = (i == 0? PSTR("initial"): PSTR("refine"));
        target_temperature[0] = 0;
        if(current_temperature[0] >= TEMP_MODEL_CAL_T_low) {
            sprintf_P(tm_message, PSTR("TM: cool down <%dC"), TEMP_MODEL_CAL_T_low);
            lcd_setstatus_serial(tm_message);
            cooldown(TEMP_MODEL_CAL_T_low);
            wait(10000);
        }

        sprintf_P(tm_message, PSTR("TM: %S C est."), verb);
        lcd_setstatus_serial(tm_message);
        target_temperature[0] = cal_temp;
        samples = record();
        if(temp_error_state.v || !samples)
            return true;

        // we need a high R value for the initial C guess
        if(isnan(temp_model::data.R[0]))
            temp_model::data.R[0] = TEMP_MODEL_CAL_R_high;

        e = estimate(samples, &temp_model::data.C,
            TEMP_MODEL_CAL_C_low, TEMP_MODEL_CAL_C_high,
            TEMP_MODEL_CAL_C_thr, TEMP_MODEL_CAL_C_itr,
            0, current_temperature_ambient);
        if(isnan(e))
            return true;

        wait_temp();
        if(i) break; // we don't need to refine R
        wait(30000); // settle PID regulation

        sprintf_P(tm_message, PSTR("TM: %S R %dC"), verb, cal_temp);
        lcd_setstatus_serial(tm_message);
        samples = record();
        if(temp_error_state.v || !samples)
            return true;

        e = estimate(samples, &temp_model::data.R[0],
            TEMP_MODEL_CAL_R_low, TEMP_MODEL_CAL_R_high,
            TEMP_MODEL_CAL_R_thr, TEMP_MODEL_CAL_R_itr,
            0, current_temperature_ambient);
        if(isnan(e))
            return true;
    }

    // Estimate fan losses at regular intervals, starting from full speed to avoid low-speed
    // kickstart issues, although this requires us to wait more for the PID stabilization.
    // Normally exhibits logarithmic behavior with the stock fan+shroud, so the shorter interval
    // at lower speeds is helpful to increase the resolution of the interpolation.
    set_fan_speed(255);
    wait(30000);

    for(int8_t i = TEMP_MODEL_R_SIZE - 1; i > 0; i -= TEMP_MODEL_CAL_R_STEP) {
        // always disable the checker while estimating fan resistance as the difference
        // (esp with 3rd-party blowers) can be massive
        temp_model::data.R[i] = NAN;

        uint8_t speed = 256 / TEMP_MODEL_R_SIZE * (i + 1) - 1;
        set_fan_speed(speed);
        wait(10000);

        sprintf_P(tm_message, PSTR("TM: R[%u] estimat."), (unsigned)i);
        lcd_setstatus_serial(tm_message);
        samples = record();
        if(temp_error_state.v || !samples)
            return true;

        // a fixed fan pwm (the norminal value) is used here, as soft_pwm_fan will be modified
        // during fan measurements and we'd like to include that skew during normal operation.
        e = estimate(samples, &temp_model::data.R[i],
            TEMP_MODEL_CAL_R_low, temp_model::data.R[0], TEMP_MODEL_CAL_R_thr, TEMP_MODEL_CAL_R_itr,
            i, current_temperature_ambient);
        if(isnan(e))
            return true;
    }

    // interpolate remaining steps to speed-up calibration
    // TODO: verify that the sampled values are monotically increasing?
    int8_t next = TEMP_MODEL_R_SIZE - 1;
    for(uint8_t i = TEMP_MODEL_R_SIZE - 2; i != 0; --i) {
        if(!((TEMP_MODEL_R_SIZE - i - 1) % TEMP_MODEL_CAL_R_STEP)) {
            next = i;
            continue;
        }
        int8_t prev = next - TEMP_MODEL_CAL_R_STEP;
        if(prev < 0) prev = 0;
        float f = (float)(i - prev) / TEMP_MODEL_CAL_R_STEP;
        float d = (temp_model::data.R[next] - temp_model::data.R[prev]);
        temp_model::data.R[i] = temp_model::data.R[prev] + d * f;
    }

    return false;
}

} // namespace temp_model_cal

static bool temp_model_autotune_err = true;

void temp_model_autotune(int16_t temp, bool selftest)
{
    float orig_C, orig_R[TEMP_MODEL_R_SIZE];
    bool orig_enabled;
    static_assert(sizeof(orig_R) == sizeof(temp_model::data.R));

    // fail-safe error state
    temp_model_autotune_err = true;

    char tm_message[LCD_WIDTH+1];
    if(moves_planned() || printer_active()) {
        sprintf_P(tm_message, PSTR("TM: Cal. NOT IDLE"));
        lcd_setstatus_serial(tm_message);
        return;
    }

    // lockout the printer during calibration
    KEEPALIVE_STATE(IN_PROCESS);
    menu_set_block(MENU_BLOCK_TEMP_MODEL_AUTOTUNE);
    lcd_return_to_status();

    // save the original model data and set the model checking state during self-calibration
    orig_C = temp_model::data.C;
    memcpy(orig_R, temp_model::data.R, sizeof(temp_model::data.R));
    orig_enabled = temp_model::enabled;
    temp_model_reset_enabled(selftest);

    // autotune
    SERIAL_ECHOLNPGM("TM: calibration start");
    temp_model_autotune_err = temp_model_cal::autotune(temp > 0 ? temp : TEMP_MODEL_CAL_T_high);

    // always reset temperature
    disable_heater();

    if(temp_model_autotune_err) {
        sprintf_P(tm_message, PSTR("TM: calibr. failed!"));
        lcd_setstatus_serial(tm_message);
        if(temp_error_state.v)
            temp_model_cal::set_fan_speed(255);

        // show calibrated values before overwriting them
        temp_model_report_settings();

        // restore original state
        temp_model::data.C = orig_C;
        memcpy(temp_model::data.R, orig_R, sizeof(temp_model::data.R));
        temp_model_set_enabled(orig_enabled);
    } else {
        calibration_status_set(CALIBRATION_STATUS_TEMP_MODEL);
        lcd_setstatuspgm(MSG_WELCOME);
        temp_model_cal::set_fan_speed(0);
        temp_model_set_enabled(orig_enabled);
        temp_model_report_settings();
    }

    lcd_consume_click();
    menu_unset_block(MENU_BLOCK_TEMP_MODEL_AUTOTUNE);
}

bool temp_model_autotune_result()
{
    return !temp_model_autotune_err;
}

#ifdef TEMP_MODEL_DEBUG
void temp_model_log_enable(bool enable)
{
    if(enable) {
        TempMgrGuard temp_mgr_guard;
        temp_model::log_buf.entry.stamp = _millis();
    }
    temp_model::log_buf.enabled = enable;
}
#endif
#endif
