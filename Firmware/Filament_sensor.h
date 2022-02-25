#pragma once

#include <inttypes.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

#include "Marlin.h"
#include "ultralcd.h"
#include "menu.h"
#include "cardreader.h"
#include "temperature.h"
#include "cmdqueue.h"
#include "eeprom.h"
#include "pins.h"
#include "fastio.h"
#include "adc.h"
#include "Timer.h"
#include "pat9125.h"

#define FSENSOR_IR 1
#define FSENSOR_IR_ANALOG 2
#define FSENSOR_PAT9125 3

#ifdef FILAMENT_SENSOR
class Filament_sensor {
public:
    virtual void init() = 0;
    virtual void deinit() = 0;
    virtual bool update() = 0;
    virtual bool getFilamentPresent() = 0;
    
    enum class State : uint8_t {
        disabled = 0,
        initializing,
        ready,
        error,
    };
    
    enum class SensorActionOnError : uint8_t {
        _Continue = 0,
        _Pause = 1,
        _Undef = EEPROM_EMPTY_VALUE
    };
    
    void setEnabled(bool enabled) {
        eeprom_update_byte((uint8_t *)EEPROM_FSENSOR, enabled);
        if (enabled) {
            init();
        }
        else {
            deinit();
        }
    }
    
    void setAutoLoadEnabled(bool state, bool updateEEPROM = false) {
        autoLoadEnabled = state;
        if (updateEEPROM) {
            eeprom_update_byte((uint8_t *)EEPROM_FSENS_AUTOLOAD_ENABLED, state);
        }
    }
    
    bool getAutoLoadEnabled() {
        return autoLoadEnabled;
    }
    
    void setRunoutEnabled(bool state, bool updateEEPROM = false) {
        runoutEnabled = state;
        if (updateEEPROM) {
            eeprom_update_byte((uint8_t *)EEPROM_FSENS_RUNOUT_ENABLED, state);
        }
    }
    
    bool getRunoutEnabled() {
        return runoutEnabled;
    }
    
    void setActionOnError(SensorActionOnError state, bool updateEEPROM = false) {
        sensorActionOnError = state;
        if (updateEEPROM) {
            eeprom_update_byte((uint8_t *)EEPROM_FSENSOR_ACTION_NA, (uint8_t)state);
        }
    }
    
    SensorActionOnError getActionOnError() {
        return sensorActionOnError;
    }
    
    bool getFilamentLoadEvent() {
        return postponedLoadEvent;
    }
    
    bool isError() {
        return state == State::error;
    }
    
    bool isReady() {
        return state == State::ready;
    }
    
    bool isEnabled() {
        return state != State::disabled;
    }
    
protected:
    void settings_init() {
        bool enabled = eeprom_read_byte((uint8_t*)EEPROM_FSENSOR);
        if ((state != State::disabled) != enabled) {
            state = enabled ? State::initializing : State::disabled;
        }
        
        autoLoadEnabled = eeprom_read_byte((uint8_t*)EEPROM_FSENS_AUTOLOAD_ENABLED);
        runoutEnabled = eeprom_read_byte((uint8_t*)EEPROM_FSENS_RUNOUT_ENABLED);
        sensorActionOnError = (SensorActionOnError)eeprom_read_byte((uint8_t*)EEPROM_FSENSOR_ACTION_NA);
        if (sensorActionOnError == SensorActionOnError::_Undef) {
            sensorActionOnError = SensorActionOnError::_Continue;
        }
    }
    
    bool checkFilamentEvents() {
        if (state != State::ready)
            return false;
        if (eventBlankingTimer.running() && !eventBlankingTimer.expired(100)) {// event blanking for 100ms
            return false;
        }
        
        bool newFilamentPresent = getFilamentPresent();
        if (oldFilamentPresent != newFilamentPresent) {
            oldFilamentPresent = newFilamentPresent;
            eventBlankingTimer.start();
            if (newFilamentPresent) { //filament insertion
                puts_P(PSTR("filament inserted"));
                triggerFilamentInserted();
                postponedLoadEvent = true;
            }
            else { //filament removal
                puts_P(PSTR("filament removed"));
                triggerFilamentRemoved();
            }
            return true;
        }
        return false;
    };
    
    void triggerFilamentInserted() {
        if (autoLoadEnabled && (eFilamentAction == FilamentAction::None) && !(moves_planned() || IS_SD_PRINTING || usb_timer.running() || (lcd_commands_type == LcdCommands::Layer1Cal) || eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE))) {
            eFilamentAction = FilamentAction::AutoLoad;
            if(target_temperature[0] >= EXTRUDE_MINTEMP){
                bFilamentPreheatState = true;
                menu_submenu(mFilamentItemForce);
            } else {
                menu_submenu(lcd_generic_preheat_menu);
                lcd_timeoutToStatus.start();
            }
        }
    }
    
    void triggerFilamentRemoved() {
        if (runoutEnabled && (eFilamentAction == FilamentAction::None) && !saved_printing && (moves_planned() || IS_SD_PRINTING || usb_timer.running() || (lcd_commands_type == LcdCommands::Layer1Cal) || eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE))) {
            runoutEnabled = false;
            autoLoadEnabled = false;
            stop_and_save_print_to_ram(0, 0);
            restore_print_from_ram_and_continue(0);
            eeprom_update_byte((uint8_t*)EEPROM_FERROR_COUNT, eeprom_read_byte((uint8_t*)EEPROM_FERROR_COUNT) + 1);
            eeprom_update_word((uint16_t*)EEPROM_FERROR_COUNT_TOT, eeprom_read_word((uint16_t*)EEPROM_FERROR_COUNT_TOT) + 1);
            enquecommand_front_P((PSTR("M600")));
        }
    }
    
    void triggerError() {
        // deinit(); //not sure if I should call this here.
        state = State::error;
        
        /// some message, idk
        ;//
    }
    
    State state;
    bool autoLoadEnabled;
    bool runoutEnabled;
    bool oldFilamentPresent; //for creating filament presence switching events.
    bool postponedLoadEvent; //this event lasts exactly one update cycle. It is long enough to be able to do polling for load event.
    ShortTimer eventBlankingTimer;
    SensorActionOnError sensorActionOnError;
};

#if (FILAMENT_SENSOR_TYPE == FSENSOR_IR) || (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
class IR_sensor: public Filament_sensor {
public:
    void init() {
        if (state == State::error) {
            deinit(); //deinit first if there was an error.
        }
        puts_P(PSTR("fsensor::init()"));
        SET_INPUT(IR_SENSOR_PIN); //input mode
        WRITE(IR_SENSOR_PIN, 1); //pullup
        settings_init(); //also sets the state to State::initializing
    }
    
    void deinit() {
        puts_P(PSTR("fsensor::deinit()"));
        SET_INPUT(IR_SENSOR_PIN); //input mode
        WRITE(IR_SENSOR_PIN, 0); //no pullup
        state = State::disabled;
    }
    
    bool update() {
        switch (state) {
            case State::initializing:
                state = State::ready; //the IR sensor gets ready instantly as it's just a gpio read operation.
                oldFilamentPresent = getFilamentPresent(); //initialize the current filament state so that we don't create a switching event right after the sensor is ready.
                // fallthru
            case State::ready: {
                postponedLoadEvent = false;
                bool event = checkFilamentEvents();
                
                ;//
                
                return event;
            } break;
            case State::disabled:
            case State::error:
            default:
                return false;
        }
    }
    
    bool getFilamentPresent() {
        return !READ(IR_SENSOR_PIN);
    }
    
    void settings_init() {
        Filament_sensor::settings_init();
    }
protected:
};

#if (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
class IR_sensor_analog: public IR_sensor {
public:
    void init() {
        IR_sensor::init();
        settings_init();
    }
    
    void deinit() {
        IR_sensor::deinit();
    }
    
    bool update() {
        bool event = IR_sensor::update();
        if (state == State::ready) {
            if (voltReady) {
                voltReady = false;
                uint16_t volt = getVoltRaw();
                printf_P(PSTR("newVoltRaw:%u\n"), volt / OVERSAMPLENR);
                
                // detect min-max, some long term sliding window for filtration may be added
                // avoiding floating point operations, thus computing in raw
                if(volt > maxVolt) {
                    maxVolt = volt;
                }
                else if(volt < minVolt) {
                    minVolt = volt;
                }
                //! The trouble is, I can hold the filament in the hole in such a way, that it creates the exact voltage
                //! to be detected as the new fsensor
                //! We can either fake it by extending the detection window to a looooong time
                //! or do some other countermeasures
                
                //! what we want to detect:
                //! if minvolt gets below ~0.3V, it means there is an old fsensor
                //! if maxvolt gets above 4.6V, it means we either have an old fsensor or broken cables/fsensor
                //! So I'm waiting for a situation, when minVolt gets to range <0, 1.5> and maxVolt gets into range <3.0, 5>
                //! If and only if minVolt is in range <0.3, 1.5> and maxVolt is in range <3.0, 4.6>, I'm considering a situation with the new fsensor
                if(minVolt >= IRsensor_Ldiode_TRESHOLD && minVolt <= IRsensor_Lmax_TRESHOLD && maxVolt >= IRsensor_Hmin_TRESHOLD && maxVolt <= IRsensor_Hopen_TRESHOLD) {
                    IR_ANALOG_Check(SensorRevision::_Old, SensorRevision::_Rev04);
                }
                //! If and only if minVolt is in range <0.0, 0.3> and maxVolt is in range  <4.6, 5.0V>, I'm considering a situation with the old fsensor
                //! Note, we are not relying on one voltage here - getting just +5V can mean an old fsensor or a broken new sensor - that's why
                //! we need to have both voltages detected correctly to allow switching back to the old fsensor.
                else if( minVolt < IRsensor_Ldiode_TRESHOLD && maxVolt > IRsensor_Hopen_TRESHOLD && maxVolt <= IRsensor_VMax_TRESHOLD) {
                    IR_ANALOG_Check(SensorRevision::_Rev04, SensorRevision::_Old);
                }
                
                if (!checkVoltage(volt)) {
                    triggerError();
                }
            }
        }
        
        ;//
        
        return event;
    }
    
    void voltUpdate(uint16_t raw) { //to be called from the ADC ISR when a cycle is finished
        voltRaw = raw;
        voltReady = true;
    }
    
    uint16_t getVoltRaw() {
        uint16_t newVoltRaw;
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            newVoltRaw = voltRaw;
        }
        return newVoltRaw;
    }
    
    void settings_init() {
        IR_sensor::settings_init();
        sensorRevision = (SensorRevision)eeprom_read_byte((uint8_t*)EEPROM_FSENSOR_PCB);
    }
    
    enum class SensorRevision : uint8_t {
        _Old = 0,
        _Rev04 = 1,
        _Undef = EEPROM_EMPTY_VALUE
    };
    
    SensorRevision getSensorRevision() {
        return sensorRevision;
    }
    
    const char* getIRVersionText() {
        switch(sensorRevision) {
            case SensorRevision::_Old:
                return _T(MSG_IR_03_OR_OLDER);
            case SensorRevision::_Rev04:
                return _T(MSG_IR_04_OR_NEWER);
            default:
                return _T(MSG_IR_UNKNOWN);
        }
    }
    
    void setSensorRevision(SensorRevision rev, bool updateEEPROM = false) {
        sensorRevision = rev;
        if (updateEEPROM) {
            eeprom_update_byte((uint8_t *)EEPROM_FSENSOR_PCB, (uint8_t)rev);
        }
    }
    
    uint16_t Voltage2Raw(float V) {
        return (V * 1023 * OVERSAMPLENR / VOLT_DIV_REF ) + 0.5F;
    }
    float Raw2Voltage(uint16_t raw) {
        return VOLT_DIV_REF * (raw / (1023.F * OVERSAMPLENR));
    }
    
    bool checkVoltage(uint16_t raw) {
        if(IRsensor_Lmax_TRESHOLD <= raw && raw <= IRsensor_Hmin_TRESHOLD) {
            /// If the voltage is in forbidden range, the fsensor is ok, but the lever is mounted improperly.
            /// Or the user is so creative so that he can hold a piece of fillament in the hole in such a genius way,
            /// that the IR fsensor reading is within 1.5 and 3V ... this would have been highly unusual
            /// and would have been considered more like a sabotage than normal printer operation
            if (voltageErrorCnt++ > 4) {
                puts_P(PSTR("fsensor in forbidden range 1.5-3V - check sensor"));
                return false;
            }
        }
        else {
            voltageErrorCnt = 0;
        }
        if(sensorRevision == SensorRevision::_Rev04) {
            /// newer IR sensor cannot normally produce 4.6-5V, this is considered a failure/bad mount
            if(IRsensor_Hopen_TRESHOLD <= raw && raw <= IRsensor_VMax_TRESHOLD) {
                puts_P(PSTR("fsensor v0.4 in fault range 4.6-5V - unconnected"));
                return false;
            }
            /// newer IR sensor cannot normally produce 0-0.3V, this is considered a failure 
    #if 0	//Disabled as it has to be decided if we gonna use this or not.
            if(IRsensor_Hopen_TRESHOLD <= raw && raw <= IRsensor_VMax_TRESHOLD) {
                puts_P(PSTR("fsensor v0.4 in fault range 0.0-0.3V - wrong IR sensor"));
                return false;
            }
    #endif
        }
        /// If IR sensor is "uknown state" and filament is not loaded > 1.5V return false
    #if 0
    #error "I really think this code can't be enabled anymore because we are constantly checking this voltage."
        if((sensorRevision == SensorRevision::_Undef) && (raw > IRsensor_Lmax_TRESHOLD)) {
            puts_P(PSTR("Unknown IR sensor version and no filament loaded detected."));
            return false;
        }
    #endif
        // otherwise the IR fsensor is considered working correctly
        return true;
    }
    
    // Voltage2Raw is not constexpr :/
    const uint16_t IRsensor_Ldiode_TRESHOLD = Voltage2Raw(0.3f); // ~0.3V, raw value=982
    const uint16_t IRsensor_Lmax_TRESHOLD = Voltage2Raw(1.5f); // ~1.5V (0.3*Vcc), raw value=4910
    const uint16_t IRsensor_Hmin_TRESHOLD = Voltage2Raw(3.0f); // ~3.0V (0.6*Vcc), raw value=9821
    const uint16_t IRsensor_Hopen_TRESHOLD = Voltage2Raw(4.6f); // ~4.6V (N.C. @ Ru~20-50k, Rd'=56k, Ru'=10k), raw value=15059
    const uint16_t IRsensor_VMax_TRESHOLD = Voltage2Raw(5.f); // ~5V, raw value=16368
    
private:
    SensorRevision sensorRevision;
    volatile bool voltReady; //this gets set by the adc ISR
    volatile uint16_t voltRaw;
    uint16_t minVolt = Voltage2Raw(6.f);
    uint16_t maxVolt = 0;
    uint16_t nFSCheckCount;
    uint8_t voltageErrorCnt;

    static constexpr uint16_t FS_CHECK_COUNT = 4;
    /// Switching mechanism of the fsensor type.
    /// Called from 2 spots which have a very similar behavior
    /// 1: SensorRevision::_Old -> SensorRevision::_Rev04 and print _i("FS v0.4 or newer")
    /// 2: SensorRevision::_Rev04 -> sensorRevision=SensorRevision::_Old and print _i("FS v0.3 or older")
    void IR_ANALOG_Check(SensorRevision isVersion, SensorRevision switchTo) {
        bool bTemp = (!CHECK_ALL_HEATERS);
        bTemp = bTemp && (menu_menu == lcd_status_screen);
        bTemp = bTemp && ((sensorRevision == isVersion) || (sensorRevision == SensorRevision::_Undef));
        bTemp = bTemp && (state == State::ready);
        if (bTemp) {
            nFSCheckCount++;
            if (nFSCheckCount > FS_CHECK_COUNT) {
                nFSCheckCount = 0; // not necessary
                setSensorRevision(switchTo, true);
                printf_IRSensorAnalogBoardChange();
                switch (switchTo) {
                    case SensorRevision::_Old:
                        lcd_setstatuspgm(_T(MSG_FS_V_03_OR_OLDER)); ////MSG_FS_V_03_OR_OLDER c=18
                        break;
                    case SensorRevision::_Rev04:
                        lcd_setstatuspgm(_T(MSG_FS_V_04_OR_NEWER)); ////MSG_FS_V_04_OR_NEWER c=18
                        break;
                    default:
                        break;
                }
            }
        }
        else {
            nFSCheckCount = 0;
        }
    }
};
#endif //(FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
#endif //(FILAMENT_SENSOR_TYPE == FSENSOR_IR) || (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)

#if (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
class PAT9125_sensor: public Filament_sensor {
public:
    void init() {
        if (state == State::error) {
            deinit(); //deinit first if there was an error.
        }
        puts_P(PSTR("fsensor::init()"));
        ;//
        settings_init(); //also sets the state to State::initializing
    }
    
    void deinit() {
        puts_P(PSTR("fsensor::deinit()"));
        ;//
        state = State::disabled;
    }
    
    bool update() {
        switch (state) {
            case State::initializing:
                // state = State::ready; //the IR sensor gets ready instantly as it's just a gpio read operation.
                oldFilamentPresent = getFilamentPresent(); //initialize the current filament state so that we don't create a switching event right after the sensor is ready.
                // fallthru
            case State::ready: {
                postponedLoadEvent = false;
                bool event = checkFilamentEvents();
                
                ;//
                
                return event;
            } break;
            case State::disabled:
            case State::error:
            default:
                return false;
        }
    }
    
    bool getFilamentPresent() {
        return false;///
    }
    
    void settings_init() {
        Filament_sensor::settings_init();
    }
protected:
};
#endif //(FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)

#if FILAMENT_SENSOR_TYPE == FSENSOR_IR
extern IR_sensor fsensor;
#elif FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG
extern IR_sensor_analog fsensor;
#elif FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125
extern PAT9125_sensor fsensor;
#endif

#endif //FILAMENT_SENSOR
