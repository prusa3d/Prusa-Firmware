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


#include "Marlin.h"
#include "ultralcd.h"
#include "sound.h"
#include "temperature.h"
#include "cardreader.h"

#include "Sd2PinMap.h"

#include <avr/wdt.h>
#include "adc.h"
#include "ConfigurationStore.h"


//===========================================================================
//=============================public variables============================
//===========================================================================
int target_temperature[EXTRUDERS] = { 0 };
int target_temperature_bed = 0;
int current_temperature_raw[EXTRUDERS] = { 0 };
float current_temperature[EXTRUDERS] = { 0.0 };

#ifdef PINDA_THERMISTOR
int current_temperature_raw_pinda =  0 ;
float current_temperature_pinda = 0.0;
#endif //PINDA_THERMISTOR

#ifdef AMBIENT_THERMISTOR
int current_temperature_raw_ambient =  0 ;
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
#ifdef TEMP_SENSOR_1_AS_REDUNDANT
  int redundant_temperature_raw = 0;
  float redundant_temperature = 0.0;
#endif
  

#ifdef PIDTEMP
  float _Kp, _Ki, _Kd;
  int pid_cycle, pid_number_of_cycles;
  bool pid_tuning_finished = false;
  #ifdef PID_ADD_EXTRUSION_RATE
    float Kc=DEFAULT_Kc;
  #endif
#endif //PIDTEMP
  
#ifdef FAN_SOFT_PWM
  unsigned char fanSpeedSoftPwm;
#endif


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
  static float temp_iState[EXTRUDERS] = { 0 };
  static float temp_dState[EXTRUDERS] = { 0 };
  static float pTerm[EXTRUDERS];
  static float iTerm[EXTRUDERS];
  static float dTerm[EXTRUDERS];
  //int output;
  static float pid_error[EXTRUDERS];
  static float temp_iState_min[EXTRUDERS];
  static float temp_iState_max[EXTRUDERS];
  // static float pid_input[EXTRUDERS];
  // static float pid_output[EXTRUDERS];
  static bool pid_reset[EXTRUDERS];
#endif //PIDTEMP
#ifdef PIDTEMPBED
  //static cannot be external:
  static float temp_iState_bed = { 0 };
  static float temp_dState_bed = { 0 };
  static float pTerm_bed;
  static float iTerm_bed;
  static float dTerm_bed;
  //int output;
  static float pid_error_bed;
  static float temp_iState_min_bed;
  static float temp_iState_max_bed;
#else //PIDTEMPBED
	static unsigned long  previous_millis_bed_heater;
#endif //PIDTEMPBED
  static unsigned char soft_pwm[EXTRUDERS];

#ifdef FAN_SOFT_PWM
  static unsigned char soft_pwm_fan;
#endif
#if (defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1) || \
    (defined(EXTRUDER_1_AUTO_FAN_PIN) && EXTRUDER_1_AUTO_FAN_PIN > -1) || \
    (defined(EXTRUDER_2_AUTO_FAN_PIN) && EXTRUDER_2_AUTO_FAN_PIN > -1)
  static unsigned long extruder_autofan_last_check;
#endif  


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

#ifdef TEMP_SENSOR_1_AS_REDUNDANT
  static void *heater_ttbl_map[2] = {(void *)HEATER_0_TEMPTABLE, (void *)HEATER_1_TEMPTABLE };
  static uint8_t heater_ttbllen_map[2] = { HEATER_0_TEMPTABLE_LEN, HEATER_1_TEMPTABLE_LEN };
#else
  static void *heater_ttbl_map[EXTRUDERS] = ARRAY_BY_EXTRUDERS( (void *)HEATER_0_TEMPTABLE, (void *)HEATER_1_TEMPTABLE, (void *)HEATER_2_TEMPTABLE );
  static uint8_t heater_ttbllen_map[EXTRUDERS] = ARRAY_BY_EXTRUDERS( HEATER_0_TEMPTABLE_LEN, HEATER_1_TEMPTABLE_LEN, HEATER_2_TEMPTABLE_LEN );
#endif

static float analog2temp(int raw, uint8_t e);
static float analog2tempBed(int raw);
static float analog2tempAmbient(int raw);
static void updateTemperaturesFromRawValues();

enum TempRunawayStates
{
	TempRunaway_INACTIVE = 0,
	TempRunaway_PREHEAT = 1,
	TempRunaway_ACTIVE = 2,
};

#ifdef WATCH_TEMP_PERIOD
int watch_start_temp[EXTRUDERS] = ARRAY_BY_EXTRUDERS(0,0,0);
unsigned long watchmillis[EXTRUDERS] = ARRAY_BY_EXTRUDERS(0,0,0);
#endif //WATCH_TEMP_PERIOD

#ifndef SOFT_PWM_SCALE
#define SOFT_PWM_SCALE 0
#endif

//===========================================================================
//=============================   functions      ============================
//===========================================================================

#if (defined (TEMP_RUNAWAY_BED_HYSTERESIS) && TEMP_RUNAWAY_BED_TIMEOUT > 0) || (defined (TEMP_RUNAWAY_EXTRUDER_HYSTERESIS) && TEMP_RUNAWAY_EXTRUDER_TIMEOUT > 0)
static float temp_runaway_status[4];
static float temp_runaway_target[4];
static float temp_runaway_timer[4];
static int temp_runaway_error_counter[4];

static void temp_runaway_check(int _heater_id, float _target_temperature, float _current_temperature, float _output, bool _isbed);
static void temp_runaway_stop(bool isPreheat, bool isBed);
#endif

  void PID_autotune(float temp, int extruder, int ncycles)
  {
  pid_number_of_cycles = ncycles;
  pid_tuning_finished = false;
  float input = 0.0;
  pid_cycle=0;
  bool heating = true;

  unsigned long temp_millis = millis();
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

#if (defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1) || \
    (defined(EXTRUDER_1_AUTO_FAN_PIN) && EXTRUDER_1_AUTO_FAN_PIN > -1) || \
    (defined(EXTRUDER_2_AUTO_FAN_PIN) && EXTRUDER_2_AUTO_FAN_PIN > -1)
  unsigned long extruder_autofan_last_check = millis();
#endif

  if ((extruder >= EXTRUDERS)
  #if (TEMP_BED_PIN <= -1)
       ||(extruder < 0)
  #endif
       ){
          SERIAL_ECHOLN("PID Autotune failed. Bad extruder number.");
		  pid_tuning_finished = true;
		  pid_cycle = 0;
          return;
        }
	
  SERIAL_ECHOLN("PID Autotune start");
  
  disable_heater(); // switch off all heaters.

  if (extruder<0)
  {
     soft_pwm_bed = (MAX_BED_POWER)/2;
     bias = d = (MAX_BED_POWER)/2;
   }
   else
   {
     soft_pwm[extruder] = (PID_MAX)/2;
     bias = d = (PID_MAX)/2;
  }




 for(;;) {
#ifdef WATCHDOG
    wdt_reset();
#endif //WATCHDOG
    if(temp_meas_ready == true) { // temp sample ready
      updateTemperaturesFromRawValues();

      input = (extruder<0)?current_temperature_bed:current_temperature[extruder];

      max=max(max,input);
      min=min(min,input);

      #if (defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1) || \
          (defined(EXTRUDER_1_AUTO_FAN_PIN) && EXTRUDER_1_AUTO_FAN_PIN > -1) || \
          (defined(EXTRUDER_2_AUTO_FAN_PIN) && EXTRUDER_2_AUTO_FAN_PIN > -1)
      if(millis() - extruder_autofan_last_check > 2500) {
        checkExtruderAutoFans();
        extruder_autofan_last_check = millis();
      }
      #endif

      if(heating == true && input > temp) {
        if(millis() - t2 > 5000) { 
          heating=false;
          if (extruder<0)
            soft_pwm_bed = (bias - d) >> 1;
          else
            soft_pwm[extruder] = (bias - d) >> 1;
          t1=millis();
          t_high=t1 - t2;
          max=temp;
        }
      }
      if(heating == false && input < temp) {
        if(millis() - t1 > 5000) {
          heating=true;
          t2=millis();
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
            soft_pwm_bed = (bias + d) >> 1;
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
    if(millis() - temp_millis > 2000) {
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

			if (abs(input - temp_ambient) < 5.0) { 
				temp_runaway_stop(false, (extruder<0));
				pid_tuning_finished = true;
				return;
			}
		}
      temp_millis = millis();
    }
    if(((millis() - t1) + (millis() - t2)) > (10L*60L*1000L*2L)) {
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
#ifdef PIDTEMP
  for(int e = 0; e < EXTRUDERS; e++) { 
     temp_iState_max[e] = PID_INTEGRAL_DRIVE_MAX / cs.Ki;  
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

#if (defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1) || \
    (defined(EXTRUDER_1_AUTO_FAN_PIN) && EXTRUDER_1_AUTO_FAN_PIN > -1) || \
    (defined(EXTRUDER_2_AUTO_FAN_PIN) && EXTRUDER_2_AUTO_FAN_PIN > -1)

  #if defined(FAN_PIN) && FAN_PIN > -1
    #if EXTRUDER_0_AUTO_FAN_PIN == FAN_PIN 
       #error "You cannot set EXTRUDER_0_AUTO_FAN_PIN equal to FAN_PIN"
    #endif
    #if EXTRUDER_1_AUTO_FAN_PIN == FAN_PIN 
       #error "You cannot set EXTRUDER_1_AUTO_FAN_PIN equal to FAN_PIN"
    #endif
    #if EXTRUDER_2_AUTO_FAN_PIN == FAN_PIN 
       #error "You cannot set EXTRUDER_2_AUTO_FAN_PIN equal to FAN_PIN"
    #endif
  #endif 

void setExtruderAutoFanState(int pin, bool state)
{
  unsigned char newFanSpeed = (state != 0) ? EXTRUDER_AUTO_FAN_SPEED : 0;
  // this idiom allows both digital and PWM fan outputs (see M42 handling).
  pinMode(pin, OUTPUT);
  digitalWrite(pin, newFanSpeed);
  analogWrite(pin, newFanSpeed);
}

#if (defined(FANCHECK) && (((defined(TACH_0) && (TACH_0 >-1)) || (defined(TACH_1) && (TACH_1 > -1)))))

void countFanSpeed()
{
	//SERIAL_ECHOPGM("edge counter 1:"); MYSERIAL.println(fan_edge_counter[1]);
	fan_speed[0] = (fan_edge_counter[0] * (float(250) / (millis() - extruder_autofan_last_check)));
	fan_speed[1] = (fan_edge_counter[1] * (float(250) / (millis() - extruder_autofan_last_check)));
	/*SERIAL_ECHOPGM("time interval: "); MYSERIAL.println(millis() - extruder_autofan_last_check);
	SERIAL_ECHOPGM("extruder fan speed:"); MYSERIAL.print(fan_speed[0]); SERIAL_ECHOPGM("; edge counter:"); MYSERIAL.println(fan_edge_counter[0]);
	SERIAL_ECHOPGM("print fan speed:"); MYSERIAL.print(fan_speed[1]); SERIAL_ECHOPGM("; edge counter:"); MYSERIAL.println(fan_edge_counter[1]);
	SERIAL_ECHOLNPGM(" ");*/
	fan_edge_counter[0] = 0;
	fan_edge_counter[1] = 0;
}

extern bool fans_check_enabled;

void checkFanSpeed()
{
	fans_check_enabled = (eeprom_read_byte((uint8_t*)EEPROM_FAN_CHECK_ENABLED) > 0);
	static unsigned char fan_speed_errors[2] = { 0,0 };
#if (defined(FANCHECK) && defined(TACH_0) && (TACH_0 >-1))
	if ((fan_speed[0] == 0) && (current_temperature[0] > EXTRUDER_AUTO_FAN_TEMPERATURE)) fan_speed_errors[0]++;
	else fan_speed_errors[0] = 0;
#endif
#if (defined(FANCHECK) && defined(TACH_1) && (TACH_1 >-1))
	if ((fan_speed[1] == 0) && ((blocks_queued() ? block_buffer[block_buffer_tail].fan_speed : fanSpeed) > MIN_PRINT_FAN_SPEED)) fan_speed_errors[1]++;
	else fan_speed_errors[1] = 0;
#endif

	if ((fan_speed_errors[0] > 5) && fans_check_enabled) {
		fan_speed_errors[0] = 0;
		fanSpeedError(0); //extruder fan
	}
	if ((fan_speed_errors[1] > 15) && fans_check_enabled) {
		fan_speed_errors[1] = 0;
		fanSpeedError(1); //print fan
	}
}

void fanSpeedError(unsigned char _fan) {
	if (get_message_level() != 0 && isPrintPaused) return; 
	//to ensure that target temp. is not set to zero in case taht we are resuming print 
	if (card.sdprinting) {
		if (heating_status != 0) {
			lcd_print_stop();
		}
		else {
			lcd_pause_print();
		}
	}
	else {
			setTargetHotend0(0);
			SERIAL_ECHOLNPGM("// action:pause"); //for octoprint
	}
	switch (_fan) {
	case 0:
			SERIAL_ECHOLNPGM("Extruder fan speed is lower then expected");
			if (get_message_level() == 0) {
if((eSoundMode==e_SOUND_MODE_LOUD)||(eSoundMode==e_SOUND_MODE_ONCE)||(eSoundMode==e_SOUND_MODE_SILENT))
				WRITE(BEEPER, HIGH);
				delayMicroseconds(200);
				WRITE(BEEPER, LOW);
				delayMicroseconds(100);
				LCD_ALERTMESSAGEPGM("Err: EXTR. FAN ERROR");
			}
		break;
	case 1:
			SERIAL_ECHOLNPGM("Print fan speed is lower then expected");
			if (get_message_level() == 0) {
if((eSoundMode==e_SOUND_MODE_LOUD)||(eSoundMode==e_SOUND_MODE_ONCE)||(eSoundMode==e_SOUND_MODE_SILENT))
				WRITE(BEEPER, HIGH);
				delayMicroseconds(200);
				WRITE(BEEPER, LOW);
				delayMicroseconds(100);
				LCD_ALERTMESSAGEPGM("Err: PRINT FAN ERROR");
			}
		break;
	}
}
#endif //(defined(TACH_0) && TACH_0 >-1) || (defined(TACH_1) && TACH_1 > -1)


void checkExtruderAutoFans()
{
  uint8_t fanState = 0;

  // which fan pins need to be turned on?      
  #if defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1
  if (current_temperature[0] > EXTRUDER_AUTO_FAN_TEMPERATURE)
	  fanState |= 1;
  #endif
  #if defined(EXTRUDER_1_AUTO_FAN_PIN) && EXTRUDER_1_AUTO_FAN_PIN > -1
    if (current_temperature[1] > EXTRUDER_AUTO_FAN_TEMPERATURE) 
    {
      if (EXTRUDER_1_AUTO_FAN_PIN == EXTRUDER_0_AUTO_FAN_PIN) 
        fanState |= 1;
      else
        fanState |= 2;
    }
  #endif
  #if defined(EXTRUDER_2_AUTO_FAN_PIN) && EXTRUDER_2_AUTO_FAN_PIN > -1
    if (current_temperature[2] > EXTRUDER_AUTO_FAN_TEMPERATURE) 
    {
      if (EXTRUDER_2_AUTO_FAN_PIN == EXTRUDER_0_AUTO_FAN_PIN) 
        fanState |= 1;
      else if (EXTRUDER_2_AUTO_FAN_PIN == EXTRUDER_1_AUTO_FAN_PIN) 
        fanState |= 2;
      else
        fanState |= 4;
    }
  #endif
  
  // update extruder auto fan states
  #if defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1
    setExtruderAutoFanState(EXTRUDER_0_AUTO_FAN_PIN, (fanState & 1) != 0);
  #endif 
  #if defined(EXTRUDER_1_AUTO_FAN_PIN) && EXTRUDER_1_AUTO_FAN_PIN > -1
    if (EXTRUDER_1_AUTO_FAN_PIN != EXTRUDER_0_AUTO_FAN_PIN) 
      setExtruderAutoFanState(EXTRUDER_1_AUTO_FAN_PIN, (fanState & 2) != 0);
  #endif 
  #if defined(EXTRUDER_2_AUTO_FAN_PIN) && EXTRUDER_2_AUTO_FAN_PIN > -1
    if (EXTRUDER_2_AUTO_FAN_PIN != EXTRUDER_0_AUTO_FAN_PIN 
        && EXTRUDER_2_AUTO_FAN_PIN != EXTRUDER_1_AUTO_FAN_PIN)
      setExtruderAutoFanState(EXTRUDER_2_AUTO_FAN_PIN, (fanState & 4) != 0);
  #endif 
}

#endif // any extruder auto fan pins set

void manage_heater()
{
#ifdef WATCHDOG
    wdt_reset();
#endif //WATCHDOG

  float pid_input;
  float pid_output;

  if(temp_meas_ready != true)   //better readability
    return; 

  updateTemperaturesFromRawValues();

#ifdef TEMP_RUNAWAY_BED_HYSTERESIS
  temp_runaway_check(0, target_temperature_bed, current_temperature_bed, (int)soft_pwm_bed, true);
#endif

  for(int e = 0; e < EXTRUDERS; e++) 
  {

#ifdef TEMP_RUNAWAY_EXTRUDER_HYSTERESIS
	  temp_runaway_check(e+1, target_temperature[e], current_temperature[e], (int)soft_pwm[e], false);
#endif

  #ifdef PIDTEMP
    pid_input = current_temperature[e];

    #ifndef PID_OPENLOOP
        pid_error[e] = target_temperature[e] - pid_input;
        if(pid_error[e] > PID_FUNCTIONAL_RANGE) {
          pid_output = BANG_MAX;
          pid_reset[e] = true;
        }
        else if(pid_error[e] < -PID_FUNCTIONAL_RANGE || target_temperature[e] == 0) {
          pid_output = 0;
          pid_reset[e] = true;
        }
        else {
          if(pid_reset[e] == true) {
            temp_iState[e] = 0.0;
            pid_reset[e] = false;
          }
          pTerm[e] = cs.Kp * pid_error[e];
          temp_iState[e] += pid_error[e];
          temp_iState[e] = constrain(temp_iState[e], temp_iState_min[e], temp_iState_max[e]);
          iTerm[e] = cs.Ki * temp_iState[e];

          //K1 defined in Configuration.h in the PID settings
          #define K2 (1.0-K1)
          dTerm[e] = (cs.Kd * (pid_input - temp_dState[e]))*K2 + (K1 * dTerm[e]);
          pid_output = pTerm[e] + iTerm[e] - dTerm[e];
          if (pid_output > PID_MAX) {
            if (pid_error[e] > 0 )  temp_iState[e] -= pid_error[e]; // conditional un-integration
            pid_output=PID_MAX;
          } else if (pid_output < 0){
            if (pid_error[e] < 0 )  temp_iState[e] -= pid_error[e]; // conditional un-integration
            pid_output=0;
          }
        }
        temp_dState[e] = pid_input;
    #else 
          pid_output = constrain(target_temperature[e], 0, PID_MAX);
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
    SERIAL_ECHOLN(dTerm[e]);
    #endif //PID_DEBUG
  #else /* PID off */
    pid_output = 0;
    if(current_temperature[e] < target_temperature[e]) {
      pid_output = PID_MAX;
    }
  #endif

    // Check if temperature is within the correct range
#ifdef AMBIENT_THERMISTOR
    if(((current_temperature_ambient < MINTEMP_MINAMBIENT) || (current_temperature[e] > minttemp[e])) && (current_temperature[e] < maxttemp[e])) 
#else //AMBIENT_THERMISTOR
    if((current_temperature[e] > minttemp[e]) && (current_temperature[e] < maxttemp[e])) 
#endif //AMBIENT_THERMISTOR
    {
      soft_pwm[e] = (int)pid_output >> 1;
    }
    else
	{
      soft_pwm[e] = 0;
    }

    #ifdef WATCH_TEMP_PERIOD
    if(watchmillis[e] && millis() - watchmillis[e] > WATCH_TEMP_PERIOD)
    {
        if(degHotend(e) < watch_start_temp[e] + WATCH_TEMP_INCREASE)
        {
            setTargetHotend(0, e);
            LCD_MESSAGEPGM("Heating failed");
            SERIAL_ECHO_START;
            SERIAL_ECHOLN("Heating failed");
        }else{
            watchmillis[e] = 0;
        }
    }
    #endif
    #ifdef TEMP_SENSOR_1_AS_REDUNDANT
      if(fabs(current_temperature[0] - redundant_temperature) > MAX_REDUNDANT_TEMP_SENSOR_DIFF) {
        disable_heater();
        if(IsStopped() == false) {
          SERIAL_ERROR_START;
          SERIAL_ERRORLNPGM("Extruder switched off. Temperature difference between temp sensors is too high !");
          LCD_ALERTMESSAGEPGM("Err: REDUNDANT TEMP ERROR");
        }
        #ifndef BOGUS_TEMPERATURE_FAILSAFE_OVERRIDE
          Stop();
        #endif
      }
    #endif
  } // End extruder for loop

#ifndef DEBUG_DISABLE_FANCHECK
  #if (defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1) || \
      (defined(EXTRUDER_1_AUTO_FAN_PIN) && EXTRUDER_1_AUTO_FAN_PIN > -1) || \
      (defined(EXTRUDER_2_AUTO_FAN_PIN) && EXTRUDER_2_AUTO_FAN_PIN > -1)
  if(millis() - extruder_autofan_last_check > 1000)  // only need to check fan state very infrequently
  {
#if (defined(FANCHECK) && ((defined(TACH_0) && (TACH_0 >-1)) || (defined(TACH_1) && (TACH_1 > -1))))
	countFanSpeed();
	checkFanSpeed();
#endif //(defined(TACH_0) && TACH_0 >-1) || (defined(TACH_1) && TACH_1 > -1)
    checkExtruderAutoFans();
    extruder_autofan_last_check = millis();
  }  
  #endif       
#endif //DEBUG_DISABLE_FANCHECK
  
  #ifndef PIDTEMPBED
  if(millis() - previous_millis_bed_heater < BED_CHECK_INTERVAL)
    return;
  previous_millis_bed_heater = millis();
  #endif

  #if TEMP_SENSOR_BED != 0

  #ifdef PIDTEMPBED
    pid_input = current_temperature_bed;

    #ifndef PID_OPENLOOP
		  pid_error_bed = target_temperature_bed - pid_input;
		  pTerm_bed = cs.bedKp * pid_error_bed;
		  temp_iState_bed += pid_error_bed;
		  temp_iState_bed = constrain(temp_iState_bed, temp_iState_min_bed, temp_iState_max_bed);
		  iTerm_bed = cs.bedKi * temp_iState_bed;

		  //K1 defined in Configuration.h in the PID settings
		  #define K2 (1.0-K1)
		  dTerm_bed= (cs.bedKd * (pid_input - temp_dState_bed))*K2 + (K1 * dTerm_bed);
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
      pid_output = constrain(target_temperature_bed, 0, MAX_BED_POWER);
    #endif //PID_OPENLOOP

#ifdef AMBIENT_THERMISTOR
	  if(((current_temperature_bed > BED_MINTEMP) || (current_temperature_ambient < MINTEMP_MINAMBIENT)) && (current_temperature_bed < BED_MAXTEMP)) 
#else //AMBIENT_THERMISTOR
	  if((current_temperature_bed > BED_MINTEMP) && (current_temperature_bed < BED_MAXTEMP)) 
#endif //AMBIENT_THERMISTOR
	  {
	    soft_pwm_bed = (int)pid_output >> 1;
	  }
	  else {
	    soft_pwm_bed = 0;
	  }

    #elif !defined(BED_LIMIT_SWITCHING)
      // Check if temperature is within the correct range
      if((current_temperature_bed > BED_MINTEMP) && (current_temperature_bed < BED_MAXTEMP))
      {
        if(current_temperature_bed >= target_temperature_bed)
        {
          soft_pwm_bed = 0;
        }
        else 
        {
          soft_pwm_bed = MAX_BED_POWER>>1;
        }
      }
      else
      {
        soft_pwm_bed = 0;
        WRITE(HEATER_BED_PIN,LOW);
      }
    #else //#ifdef BED_LIMIT_SWITCHING
      // Check if temperature is within the correct band
      if((current_temperature_bed > BED_MINTEMP) && (current_temperature_bed < BED_MAXTEMP))
      {
        if(current_temperature_bed > target_temperature_bed + BED_HYSTERESIS)
        {
          soft_pwm_bed = 0;
        }
        else if(current_temperature_bed <= target_temperature_bed - BED_HYSTERESIS)
        {
          soft_pwm_bed = MAX_BED_POWER>>1;
        }
      }
      else
      {
        soft_pwm_bed = 0;
        WRITE(HEATER_BED_PIN,LOW);
      }
    #endif
  #endif
  
#ifdef HOST_KEEPALIVE_FEATURE
  host_keepalive();
#endif
}

#define PGM_RD_W(x)   (short)pgm_read_word(&x)
// Derived from RepRap FiveD extruder::getTemperature()
// For hot end temperature measurement.
static float analog2temp(int raw, uint8_t e) {
#ifdef TEMP_SENSOR_1_AS_REDUNDANT
  if(e > EXTRUDERS)
#else
  if(e >= EXTRUDERS)
#endif
  {
      SERIAL_ERROR_START;
      SERIAL_ERROR((int)e);
      SERIAL_ERRORLNPGM(" - Invalid extruder number !");
      kill(PSTR(""), 6);
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

/* Called to get the raw values into the the actual temperatures. The raw values are created in interrupt context,
    and this function is called from normal context as it is too slow to run in interrupts and will block the stepper routine otherwise */
static void updateTemperaturesFromRawValues()
{
    for(uint8_t e=0;e<EXTRUDERS;e++)
    {
        current_temperature[e] = analog2temp(current_temperature_raw[e], e);
    }

#ifdef PINDA_THERMISTOR
	current_temperature_pinda = analog2tempBed(current_temperature_raw_pinda);
#endif

#ifdef AMBIENT_THERMISTOR
	current_temperature_ambient = analog2tempAmbient(current_temperature_raw_ambient); //thermistor for ambient is NTCG104LH104JT1 (2000)
#endif
   
#ifdef DEBUG_HEATER_BED_SIM
	current_temperature_bed = target_temperature_bed;
#else //DEBUG_HEATER_BED_SIM
	current_temperature_bed = analog2tempBed(current_temperature_bed_raw);
#endif //DEBUG_HEATER_BED_SIM

    #ifdef TEMP_SENSOR_1_AS_REDUNDANT
      redundant_temperature = analog2temp(redundant_temperature_raw, 1);
    #endif

    //Reset the watchdog after we know we have a temperature measurement.
#ifdef WATCHDOG
    wdt_reset();
#endif //WATCHDOG

    CRITICAL_SECTION_START;
    temp_meas_ready = false;
    CRITICAL_SECTION_END;
}


void tp_init()
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
    temp_iState_min[e] = 0.0;
    temp_iState_max[e] = PID_INTEGRAL_DRIVE_MAX / cs.Ki;
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
    soft_pwm_fan = fanSpeedSoftPwm / 2;
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

  adc_init();

  // Use timer0 for temperature measurement
  // Interleave temperature interrupt with millies interrupt
  OCR0B = 128;
  TIMSK0 |= (1<<OCIE0B);  
  
  // Wait for temperature measurement to settle
  delay(250);

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
  /* No bed MINTEMP error implemented?!? */
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
}

void setWatch() 
{  
#ifdef WATCH_TEMP_PERIOD
  for (int e = 0; e < EXTRUDERS; e++)
  {
    if(degHotend(e) < degTargetHotend(e) - (WATCH_TEMP_INCREASE * 2))
    {
      watch_start_temp[e] = degHotend(e);
      watchmillis[e] = millis();
    } 
  }
#endif 
}

#if (defined (TEMP_RUNAWAY_BED_HYSTERESIS) && TEMP_RUNAWAY_BED_TIMEOUT > 0) || (defined (TEMP_RUNAWAY_EXTRUDER_HYSTERESIS) && TEMP_RUNAWAY_EXTRUDER_TIMEOUT > 0)
void temp_runaway_check(int _heater_id, float _target_temperature, float _current_temperature, float _output, bool _isbed)
{
	float __hysteresis = 0;
	int __timeout = 0;
	bool temp_runaway_check_active = false;
	static float __preheat_start[2] = { 0,0}; //currently just bed and one extruder
	static int __preheat_counter[2] = { 0,0};
	static int __preheat_errors[2] = { 0,0};
		

	if (millis() - temp_runaway_timer[_heater_id] > 2000)
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

		temp_runaway_timer[_heater_id] = millis();
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
				MYSERIAL.print(__preheat_start[_heater_id]);*/
				
				if (_current_temperature - __preheat_start[_heater_id] < 2) {
					__preheat_errors[_heater_id]++;
					/*SERIAL_ECHOPGM(" Preheat errors:");
					MYSERIAL.println(__preheat_errors[_heater_id]);*/
				}
				else {
					//SERIAL_ECHOLNPGM("");
					__preheat_errors[_heater_id] = 0;
				}

				if (__preheat_errors[_heater_id] > ((_isbed) ? 2 : 5)) 
				{
					if (farm_mode) { prusa_statistics(0); }
					temp_runaway_stop(true, _isbed);
					if (farm_mode) { prusa_statistics(91); }
				}
				__preheat_start[_heater_id] = _current_temperature;
				__preheat_counter[_heater_id] = 0;
			}
		}

		if (_current_temperature >= _target_temperature  && temp_runaway_status[_heater_id] == TempRunaway_PREHEAT)
		{
			temp_runaway_status[_heater_id] = TempRunaway_ACTIVE;
			temp_runaway_check_active = false;
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
					{
						if (farm_mode) { prusa_statistics(0); }
						temp_runaway_stop(false, _isbed);
						if (farm_mode) { prusa_statistics(90); }
					}
				}
			}
		}

	}
}

void temp_runaway_stop(bool isPreheat, bool isBed)
{
	cancel_heatup = true;
	quickStop();
	if (card.sdprinting)
	{
		card.sdprinting = false;
		card.closefile();
	}
	// Clean the input command queue 
	// This is necessary, because in command queue there can be commands which would later set heater or bed temperature.
	cmdqueue_reset();
	
	disable_heater();
	disable_x();
	disable_y();
	disable_e0();
	disable_e1();
	disable_e2();
	manage_heater();
	lcd_update(0);
if((eSoundMode==e_SOUND_MODE_LOUD)||(eSoundMode==e_SOUND_MODE_ONCE)||(eSoundMode==e_SOUND_MODE_SILENT))
	WRITE(BEEPER, HIGH);
	delayMicroseconds(500);
	WRITE(BEEPER, LOW);
	delayMicroseconds(100);

	if (isPreheat)
	{
		Stop();
		isBed ? LCD_ALERTMESSAGEPGM("BED PREHEAT ERROR") : LCD_ALERTMESSAGEPGM("PREHEAT ERROR");
		SERIAL_ERROR_START;
		isBed ? SERIAL_ERRORLNPGM(" THERMAL RUNAWAY ( PREHEAT HEATBED)") : SERIAL_ERRORLNPGM(" THERMAL RUNAWAY ( PREHEAT HOTEND)");
		SET_OUTPUT(EXTRUDER_0_AUTO_FAN_PIN);
		SET_OUTPUT(FAN_PIN);
		WRITE(EXTRUDER_0_AUTO_FAN_PIN, 1);
		analogWrite(FAN_PIN, 255);
		fanSpeed = 255;
		delayMicroseconds(2000);
	}
	else
	{
		isBed ? LCD_ALERTMESSAGEPGM("BED THERMAL RUNAWAY") : LCD_ALERTMESSAGEPGM("THERMAL RUNAWAY");
		SERIAL_ERROR_START;
		isBed ? SERIAL_ERRORLNPGM(" HEATBED THERMAL RUNAWAY") : SERIAL_ERRORLNPGM(" HOTEND THERMAL RUNAWAY");
	}
}
#endif


void disable_heater()
{
  setAllTargetHotends(0);
  setTargetBed(0);
  #if defined(TEMP_0_PIN) && TEMP_0_PIN > -1
  target_temperature[0]=0;
  soft_pwm[0]=0;
   #if defined(HEATER_0_PIN) && HEATER_0_PIN > -1  
     WRITE(HEATER_0_PIN,LOW);
   #endif
  #endif
     
  #if defined(TEMP_1_PIN) && TEMP_1_PIN > -1 && EXTRUDERS > 1
    target_temperature[1]=0;
    soft_pwm[1]=0;
    #if defined(HEATER_1_PIN) && HEATER_1_PIN > -1 
      WRITE(HEATER_1_PIN,LOW);
    #endif
  #endif
      
  #if defined(TEMP_2_PIN) && TEMP_2_PIN > -1 && EXTRUDERS > 2
    target_temperature[2]=0;
    soft_pwm[2]=0;
    #if defined(HEATER_2_PIN) && HEATER_2_PIN > -1  
      WRITE(HEATER_2_PIN,LOW);
    #endif
  #endif 

  #if defined(TEMP_BED_PIN) && TEMP_BED_PIN > -1
    target_temperature_bed=0;
    soft_pwm_bed=0;
    #if defined(HEATER_BED_PIN) && HEATER_BED_PIN > -1  
      WRITE(HEATER_BED_PIN,LOW);
    #endif
  #endif 
}

void max_temp_error(uint8_t e) {
  disable_heater();
  if(IsStopped() == false) {
    SERIAL_ERROR_START;
    SERIAL_ERRORLN((int)e);
    SERIAL_ERRORLNPGM(": Extruder switched off. MAXTEMP triggered !");
    LCD_ALERTMESSAGEPGM("Err: MAXTEMP");
  }
  #ifndef BOGUS_TEMPERATURE_FAILSAFE_OVERRIDE
  Stop();
    

    
  #endif
    SET_OUTPUT(EXTRUDER_0_AUTO_FAN_PIN);
    SET_OUTPUT(FAN_PIN);
    SET_OUTPUT(BEEPER);
    WRITE(FAN_PIN, 1);
    WRITE(EXTRUDER_0_AUTO_FAN_PIN, 1);
if((eSoundMode==e_SOUND_MODE_LOUD)||(eSoundMode==e_SOUND_MODE_ONCE)||(eSoundMode==e_SOUND_MODE_SILENT))
    WRITE(BEEPER, 1);
    // fanSpeed will consumed by the check_axes_activity() routine.
    fanSpeed=255;
	if (farm_mode) { prusa_statistics(93); }
}

void min_temp_error(uint8_t e) {
#ifdef DEBUG_DISABLE_MINTEMP
	return;
#endif
//if (current_temperature_ambient < MINTEMP_MINAMBIENT) return;
  disable_heater();
  if(IsStopped() == false) {
    SERIAL_ERROR_START;
    SERIAL_ERRORLN((int)e);
    SERIAL_ERRORLNPGM(": Extruder switched off. MINTEMP triggered !");
    LCD_ALERTMESSAGEPGM("Err: MINTEMP");
  }
  #ifndef BOGUS_TEMPERATURE_FAILSAFE_OVERRIDE
  Stop();
  #endif
  if (farm_mode) { prusa_statistics(92); }

}

void bed_max_temp_error(void) {
#if HEATER_BED_PIN > -1
  WRITE(HEATER_BED_PIN, 0);
#endif
  if(IsStopped() == false) {
    SERIAL_ERROR_START;
    SERIAL_ERRORLNPGM("Temperature heated bed switched off. MAXTEMP triggered !");
    LCD_ALERTMESSAGEPGM("Err: MAXTEMP BED");
  }
  #ifndef BOGUS_TEMPERATURE_FAILSAFE_OVERRIDE
  Stop();
  #endif

}

void bed_min_temp_error(void) {
#ifdef DEBUG_DISABLE_MINTEMP
	return;
#endif
//if (current_temperature_ambient < MINTEMP_MINAMBIENT) return;
#if HEATER_BED_PIN > -1
    WRITE(HEATER_BED_PIN, 0);
#endif
    if(IsStopped() == false) {
        SERIAL_ERROR_START;
        SERIAL_ERRORLNPGM("Temperature heated bed switched off. MINTEMP triggered !");
        LCD_ALERTMESSAGEPGM("Err: MINTEMP BED");
    }
#ifndef BOGUS_TEMPERATURE_FAILSAFE_OVERRIDE
    Stop();
#endif
}

#ifdef HEATER_0_USES_MAX6675
#define MAX6675_HEAT_INTERVAL 250
long max6675_previous_millis = MAX6675_HEAT_INTERVAL;
int max6675_temp = 2000;

int read_max6675()
{
  if (millis() - max6675_previous_millis < MAX6675_HEAT_INTERVAL) 
    return max6675_temp;
  
  max6675_previous_millis = millis();
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



extern "C" {

void adc_ready(void) //callback from adc when sampling finished
{
	current_temperature_raw[0] = adc_values[ADC_PIN_IDX(TEMP_0_PIN)]; //heater
	current_temperature_raw_pinda = adc_values[ADC_PIN_IDX(TEMP_PINDA_PIN)];
	current_temperature_bed_raw = adc_values[ADC_PIN_IDX(TEMP_BED_PIN)];
#ifdef VOLT_PWR_PIN
	current_voltage_raw_pwr = adc_values[ADC_PIN_IDX(VOLT_PWR_PIN)];
#endif
#ifdef AMBIENT_THERMISTOR
	current_temperature_raw_ambient = adc_values[ADC_PIN_IDX(TEMP_AMBIENT_PIN)];
#endif //AMBIENT_THERMISTOR
#ifdef VOLT_BED_PIN
	current_voltage_raw_bed = adc_values[ADC_PIN_IDX(VOLT_BED_PIN)]; // 6->9
#endif
	temp_meas_ready = true;
}

} // extern "C"


// Timer 0 is shared with millies
ISR(TIMER0_COMPB_vect)
{
	static bool _lock = false;
	if (_lock) return;
	_lock = true;
	asm("sei");

	if (!temp_meas_ready) adc_cycle();
	else
	{
		check_max_temp();
		check_min_temp();
	}
	lcd_buttons_update();

  static unsigned char pwm_count = (1 << SOFT_PWM_SCALE);
  static unsigned char soft_pwm_0;
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
  static unsigned char soft_pwm_b;
#ifdef SLOW_PWM_HEATERS
  static unsigned char state_heater_b = 0;
  static unsigned char state_timer_heater_b = 0;
#endif 
#endif
  
#if defined(FILWIDTH_PIN) &&(FILWIDTH_PIN > -1)
  static unsigned long raw_filwidth_value = 0;  //added for filament width sensor
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
#if defined(HEATER_BED_PIN) && HEATER_BED_PIN > -1
    soft_pwm_b = soft_pwm_bed;
    if(soft_pwm_b > 0) WRITE(HEATER_BED_PIN,1); else WRITE(HEATER_BED_PIN,0);
#endif
#ifdef FAN_SOFT_PWM
    soft_pwm_fan = fanSpeedSoftPwm / 2;
    if(soft_pwm_fan > 0) WRITE(FAN_PIN,1); else WRITE(FAN_PIN,0);
#endif
  }
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
#if defined(HEATER_BED_PIN) && HEATER_BED_PIN > -1
  if(soft_pwm_b < pwm_count) WRITE(HEATER_BED_PIN,0);
#endif
#ifdef FAN_SOFT_PWM
  if(soft_pwm_fan < pwm_count) WRITE(FAN_PIN,0);
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
	WRITE(HEATER_BED_PIN, 1);
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
  if (pwm_count == 0){
    soft_pwm_fan = fanSpeedSoftPwm / 2;
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

  
#ifdef BABYSTEPPING
  for(uint8_t axis=0;axis<3;axis++)
  {
    int curTodo=babystepsTodo[axis]; //get rid of volatile for performance
   
    if(curTodo>0)
    {
		asm("cli");
      babystep(axis,/*fwd*/true);
      babystepsTodo[axis]--; //less to do next time
		asm("sei");
    }
    else
    if(curTodo<0)
    {
		asm("cli");
      babystep(axis,/*fwd*/false);
      babystepsTodo[axis]++; //less to do next time
		asm("sei");
    }
  }
#endif //BABYSTEPPING

#if (defined(FANCHECK) && defined(TACH_0) && (TACH_0 > -1))
  check_fans();
#endif //(defined(TACH_0))

	_lock = false;
}

void check_max_temp()
{
//heater
#if HEATER_0_RAW_LO_TEMP > HEATER_0_RAW_HI_TEMP
    if (current_temperature_raw[0] <= maxttemp_raw[0]) {
#else
    if (current_temperature_raw[0] >= maxttemp_raw[0]) {
#endif
        max_temp_error(0);
    }
//bed
#if defined(BED_MAXTEMP) && (TEMP_SENSOR_BED != 0)
#if HEATER_BED_RAW_LO_TEMP > HEATER_BED_RAW_HI_TEMP
    if (current_temperature_bed_raw <= bed_maxttemp_raw) {
#else
    if (current_temperature_bed_raw >= bed_maxttemp_raw) {
#endif
       target_temperature_bed = 0;
       bed_max_temp_error();
    }
#endif

}

void check_min_temp_heater0()
{
//heater
#if HEATER_0_RAW_LO_TEMP > HEATER_0_RAW_HI_TEMP
	if (current_temperature_raw[0] >= minttemp_raw[0]) {
#else
	if (current_temperature_raw[0] <= minttemp_raw[0]) {
#endif
		min_temp_error(0);
	}
}

void check_min_temp_bed()
{
#if HEATER_BED_RAW_LO_TEMP > HEATER_BED_RAW_HI_TEMP
	if (current_temperature_bed_raw >= bed_minttemp_raw) {
#else
	if (current_temperature_bed_raw <= bed_minttemp_raw) {
#endif
		bed_min_temp_error();
	}
}

void check_min_temp()
{
#ifdef AMBIENT_THERMISTOR
	static uint8_t heat_cycles = 0;
	if (current_temperature_raw_ambient > OVERSAMPLENR*MINTEMP_MINAMBIENT_RAW)
	{
		if (READ(HEATER_0_PIN) == HIGH)
		{
//			if ((heat_cycles % 10) == 0)
//				printf_P(PSTR("X%d\n"), heat_cycles);
			if (heat_cycles > 50) //reaction time 5-10s
				check_min_temp_heater0();
			else
				heat_cycles++;
		}
		else
			heat_cycles = 0;
		return;
	}
#endif //AMBIENT_THERMISTOR
	check_min_temp_heater0();
	check_min_temp_bed();
}
 
#if (defined(FANCHECK) && defined(TACH_0) && (TACH_0 > -1))
void check_fans() {
	if (READ(TACH_0) != fan_state[0]) {
		fan_edge_counter[0] ++;
		fan_state[0] = !fan_state[0];
	}
	//if (READ(TACH_1) != fan_state[1]) {
	//	fan_edge_counter[1] ++;
	//	fan_state[1] = !fan_state[1];
	//}
}
#endif //TACH_0

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


