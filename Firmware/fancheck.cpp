// fan control and check
#include "fancheck.h"
#include "cardreader.h"
#include "ultralcd.h"
#include "sound.h"
#include "messages.h"
#include "temperature.h"
#include "stepper.h"

#define FAN_CHECK_PERIOD 5000 //5s
#define FAN_CHECK_DURATION 100 //100ms

#ifdef FANCHECK
volatile uint8_t fan_check_error = EFCE_OK;
#endif

#if (defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1)
  #ifdef EXTRUDER_ALTFAN_DETECT
  static struct
  {
      uint8_t isAltfan : 1;
      uint8_t altfanOverride : 1;
  } altfanStatus;
  #endif //EXTRUDER_ALTFAN_DETECT

  unsigned long extruder_autofan_last_check = _millis();
  bool fan_measuring = false;
  static uint8_t fanState = 0;
#endif

#if (defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1)
  #if defined(FAN_PIN) && FAN_PIN > -1
    #if EXTRUDER_0_AUTO_FAN_PIN == FAN_PIN
       #error "You cannot set EXTRUDER_0_AUTO_FAN_PIN equal to FAN_PIN"
    #endif
  #endif

void setExtruderAutoFanState(uint8_t state)
{
    //If bit 1 is set (0x02), then the hotend fan speed won't be adjusted according to temperature. Useful for forcing
    //the fan to either On or Off during certain tests/errors.

    fanState = state;
    newFanSpeed = 0;
    if (fanState & 0x01)
    {
#ifdef EXTRUDER_ALTFAN_DETECT
        if (altfanStatus.isAltfan && !altfanStatus.altfanOverride) newFanSpeed = EXTRUDER_ALTFAN_SPEED_SILENT;
        else newFanSpeed = EXTRUDER_AUTO_FAN_SPEED;
#else //EXTRUDER_ALTFAN_DETECT
        newFanSpeed = EXTRUDER_AUTO_FAN_SPEED;
#endif //EXTRUDER_ALTFAN_DETECT
    }
    timer4_set_fan0(newFanSpeed);
}

#if (defined(FANCHECK) && (((defined(TACH_0) && (TACH_0 >-1)) || (defined(TACH_1) && (TACH_1 > -1)))))

void countFanSpeed()
{
    //SERIAL_ECHOPGM("edge counter 1:"); MYSERIAL.println(fan_edge_counter[1]);
    fan_speed[0] = (fan_edge_counter[0] * (float(250) / (_millis() - extruder_autofan_last_check)));
    fan_speed[1] = (fan_edge_counter[1] * (float(250) / (_millis() - extruder_autofan_last_check)));
    /*SERIAL_ECHOPGM("time interval: "); MYSERIAL.println(_millis() - extruder_autofan_last_check);
    SERIAL_ECHOPGM("hotend fan speed:"); MYSERIAL.print(fan_speed[0]); SERIAL_ECHOPGM("; edge counter:"); MYSERIAL.println(fan_edge_counter[0]);
    SERIAL_ECHOPGM("print fan speed:"); MYSERIAL.print(fan_speed[1]); SERIAL_ECHOPGM("; edge counter:"); MYSERIAL.println(fan_edge_counter[1]);
    SERIAL_ECHOLNPGM(" ");*/
    fan_edge_counter[0] = 0;
    fan_edge_counter[1] = 0;
}

//! Prints serialMsg to serial port, displays lcdMsg onto the LCD and beeps.
//! Extracted from fanSpeedError to save some space.
//! @param serialMsg pointer into PROGMEM, this text will be printed to the serial port
//! @param lcdMsg pointer into PROGMEM, this text will be printed onto the LCD
static void fanSpeedErrorBeep(const char *serialMsg, const char *lcdMsg){
    SERIAL_ECHOLNRPGM(serialMsg);
    if (get_message_level() == 0) {
        Sound_MakeCustom(200,0,true);
        LCD_ALERTMESSAGERPGM(lcdMsg);
    }
}

void fanSpeedError(unsigned char _fan) {
    if (fan_check_error == EFCE_REPORTED) return;
    fan_check_error = EFCE_REPORTED;

    if (IS_SD_PRINTING || usb_timer.running()) {
        // A print is ongoing, pause the print normally
        if(!isPrintPaused) {
            if (usb_timer.running())
                lcd_pause_usb_print();
            else
                lcd_pause_print();
        }
    }
    else {
        // Nothing is going on, but still turn off heaters and report the error
        setTargetHotend(0);
        heating_status = HeatingStatus::NO_HEATING;
    }
    switch (_fan) {
    case 0:	// extracting the same code from case 0 and case 1 into a function saves 72B
        fanSpeedErrorBeep(PSTR("Hotend fan speed is lower than expected"), MSG_FANCHECK_HOTEND);
        break;
    case 1:
        fanSpeedErrorBeep(PSTR("Print fan speed is lower than expected"), MSG_FANCHECK_PRINT);
        break;
    }
}

void checkFanSpeed()
{
    uint8_t max_fan_errors[2];
#ifdef FAN_SOFT_PWM
    max_fan_errors[1] = 3;  // 15 seconds (Print fan)
    max_fan_errors[0] = 2;  // 10 seconds (Hotend fan)
#else //FAN_SOFT_PWM
    max_fan_errors[1] = 15; // 15 seconds (Print fan)
    max_fan_errors[0] = 5;  // 5  seconds (Hotend fan)
#endif //FAN_SOFT_PWM

    if(fans_check_enabled)
        fans_check_enabled = (eeprom_read_byte((uint8_t*)EEPROM_FAN_CHECK_ENABLED) > 0);
    static uint8_t fan_speed_errors[2] = { 0,0 };
#if (defined(FANCHECK) && defined(TACH_0) && (TACH_0 >-1))
    if ((fan_speed[0] < 20) && (current_temperature[0] > EXTRUDER_AUTO_FAN_TEMPERATURE)){ fan_speed_errors[0]++;}
    else fan_speed_errors[0] = 0;
#endif
#if (defined(FANCHECK) && defined(TACH_1) && (TACH_1 >-1))
    if ((fan_speed[1] < 5) && ((blocks_queued() ? block_buffer[block_buffer_tail].fan_speed : fanSpeed) > MIN_PRINT_FAN_SPEED)) fan_speed_errors[1]++;
    else fan_speed_errors[1] = 0;
#endif

    // drop the fan_check_error flag when both fans are ok
    if( fan_speed_errors[0] == 0 && fan_speed_errors[1] == 0 && fan_check_error == EFCE_REPORTED){
        // we may even send some info to the LCD from here
        fan_check_error = EFCE_FIXED;
    }
    if ((fan_check_error == EFCE_FIXED) && !printer_active()){
        fan_check_error = EFCE_OK; //if the issue is fixed while the printer is doing nothing, reenable processing immediately.
        lcd_reset_alert_level(); //for another fan speed error
    }
    if (fans_check_enabled && (fan_check_error == EFCE_OK))
    {
        for (uint8_t fan = 0; fan < 2; fan++)
        {
            if (fan_speed_errors[fan] > max_fan_errors[fan])
            {
                fan_speed_errors[fan] = 0;
                fanSpeedError(fan);
            }
        }
    }
}
#endif //(defined(TACH_0) && TACH_0 >-1) || (defined(TACH_1) && TACH_1 > -1)

#ifdef EXTRUDER_ALTFAN_DETECT
ISR(INT6_vect) {
    fan_edge_counter[0]++;
}

bool extruder_altfan_detect()
{
    // override isAltFan setting for detection
    altfanStatus.isAltfan = 0;

    // During initialisation, use the EEPROM value
    altfanStatus.altfanOverride = eeprom_init_default_byte((uint8_t*)EEPROM_ALTFAN_OVERRIDE, 0);
    setExtruderAutoFanState(3);

    SET_INPUT(TACH_0);
    CRITICAL_SECTION_START;
    EICRB &= ~(1 << ISC61);
    EICRB |= (1 << ISC60);
    EIMSK |= (1 << INT6);
    fan_edge_counter[0] = 0;
    CRITICAL_SECTION_END;
    extruder_autofan_last_check = _millis();

    _delay(1000);

    EIMSK &= ~(1 << INT6);

    countFanSpeed();

    // restore fan state
    altfanStatus.isAltfan = fan_speed[0] > 100;
    setExtruderAutoFanState(1);

    return altfanStatus.isAltfan;
}

void altfanOverride_toggle()
{
    altfanStatus.altfanOverride = !altfanStatus.altfanOverride;
    eeprom_update_byte((uint8_t *)EEPROM_ALTFAN_OVERRIDE, altfanStatus.altfanOverride);
}

bool altfanOverride_get()
{
    return altfanStatus.altfanOverride;
}

#endif //EXTRUDER_ALTFAN_DETECT

void checkExtruderAutoFans()
{
#if defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1
    if (!(fanState & 0x02))
    {
        fanState &= ~1;
        fanState |= current_temperature[0] > EXTRUDER_AUTO_FAN_TEMPERATURE;
        fanState |= get_temp_error();
    }
    setExtruderAutoFanState(fanState);
#endif
}

#endif // any extruder auto fan pins set

#if (defined(FANCHECK) && defined(TACH_0) && (TACH_0 > -1))
void readFanTach() {
#ifdef FAN_SOFT_PWM
    if (READ(TACH_0) != fan_state[0]) {
        if(fan_measuring) fan_edge_counter[0] ++;
        fan_state[0] = !fan_state[0];
    }
#else //FAN_SOFT_PWM
    if (READ(TACH_0) != fan_state[0]) {
        fan_edge_counter[0] ++;
        fan_state[0] = !fan_state[0];
    }
#endif
    //if (READ(TACH_1) != fan_state[1]) {
    //	fan_edge_counter[1] ++;
    //	fan_state[1] = !fan_state[1];
    //}
}
#endif //TACH_0

void checkFans()
{
#ifndef DEBUG_DISABLE_FANCHECK
#if (defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1)

#ifdef FAN_SOFT_PWM
#ifdef FANCHECK
  if ((_millis() - extruder_autofan_last_check > FAN_CHECK_PERIOD) && (!fan_measuring)) {
	  extruder_autofan_last_check = _millis();
	  fanSpeedBckp = fanSpeedSoftPwm;

	  if (fanSpeedSoftPwm >= MIN_PRINT_FAN_SPEED) { //if we are in rage where we are doing fan check, set full PWM range for a short time to measure fan RPM by reading tacho signal without modulation by PWM signal
		//  printf_P(PSTR("fanSpeedSoftPwm 1: %d\n"), fanSpeedSoftPwm);
		  fanSpeedSoftPwm = 255;
	  }
	  fan_measuring = true;
  }
  if ((_millis() - extruder_autofan_last_check > FAN_CHECK_DURATION) && (fan_measuring)) {
	  countFanSpeed();
	  checkFanSpeed();
	  //printf_P(PSTR("fanSpeedSoftPwm 1: %d\n"), fanSpeedSoftPwm);
	  fanSpeedSoftPwm = fanSpeedBckp;
	  //printf_P(PSTR("fan PWM: %d; extr fanSpeed measured: %d; print fan speed measured: %d \n"), fanSpeedBckp, fan_speed[0], fan_speed[1]);
	  extruder_autofan_last_check = _millis();
	  fan_measuring = false;
  }
#endif //FANCHECK
  checkExtruderAutoFans();
#else //FAN_SOFT_PWM
  if(_millis() - extruder_autofan_last_check > 1000)  // only need to check fan state very infrequently
  {
#if (defined(FANCHECK) && ((defined(TACH_0) && (TACH_0 >-1)) || (defined(TACH_1) && (TACH_1 > -1))))
	countFanSpeed();
	checkFanSpeed();
#endif //(defined(TACH_0) && TACH_0 >-1) || (defined(TACH_1) && TACH_1 > -1)
    checkExtruderAutoFans();
    extruder_autofan_last_check = _millis();
  }
#endif //FAN_SOFT_PWM

#endif
#endif //DEBUG_DISABLE_FANCHECK
}

void resetFanCheck() {
    fan_measuring = false;
    extruder_autofan_last_check = _millis();
}


void hotendFanSetFullSpeed()
{
#ifdef EXTRUDER_ALTFAN_DETECT
    altfanStatus.altfanOverride = 1; //full speed
#endif //EXTRUDER_ALTFAN_DETECT
    resetFanCheck();
    setExtruderAutoFanState(3);
    SET_OUTPUT(FAN_PIN);
#ifdef FAN_SOFT_PWM
    fanSpeedSoftPwm = 255;
#else //FAN_SOFT_PWM
    analogWrite(FAN_PIN, 255);
#endif //FAN_SOFT_PWM
    fanSpeed = 255;
}

void hotendDefaultAutoFanState()
{
#if (defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1)
#ifdef EXTRUDER_ALTFAN_DETECT
    altfanStatus.altfanOverride = eeprom_read_byte((uint8_t*)EEPROM_ALTFAN_OVERRIDE);
#endif
    resetFanCheck();
    setExtruderAutoFanState(1);
#endif
}
