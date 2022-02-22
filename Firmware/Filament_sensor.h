#pragma once

#include <inttypes.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#include "Marlin.h"
#include "ultralcd.h"
#include "menu.h"
#include "cardreader.h"
#include "temperature.h"
#include "cmdqueue.h"
#include "eeprom.h"
#include "pins.h"
#include "fastio.h"

class Filament_sensor {
public:
    virtual void init() = 0;
    virtual void update() = 0;
    virtual bool getFilamentPresent() = 0;
    
    enum class SensorActionOnError : uint8_t {
        _Continue = 0,
        _Pause = 1,
        _Undef = EEPROM_EMPTY_VALUE
    };
    
    void setAutoLoadEnabled(bool state, bool updateEEPROM = false) {
        autoLoadEnabled = state;
        if (updateEEPROM) {
            eeprom_update_byte((uint8_t *)EEPROM_FSENS_AUTOLOAD_ENABLED, state);
        }
    }
    
    void setRunoutEnabled(bool state, bool updateEEPROM = false) {
        runoutEnabled = state;
        if (updateEEPROM) {
            eeprom_update_byte((uint8_t *)EEPROM_FSENSOR, state);
        }
    }
    
protected:
    void settings_init() {
        autoLoadEnabled = eeprom_read_byte((uint8_t*)EEPROM_FSENS_AUTOLOAD_ENABLED);
        runoutEnabled = eeprom_read_byte((uint8_t*)EEPROM_FSENSOR);
        sensorActionOnError = (SensorActionOnError)eeprom_read_byte((uint8_t*)EEPROM_FSENSOR_ACTION_NA);
        if (sensorActionOnError == SensorActionOnError::_Undef) {
            sensorActionOnError = SensorActionOnError::_Pause;
        }
    }
    
    void checkFilamentEvents() {
        if (!ready)
            return;
        
        bool newFilamentPresent = getFilamentPresent();
        if (oldFilamentPresent != newFilamentPresent) {
            oldFilamentPresent = newFilamentPresent;
            if (newFilamentPresent) { //filament insertion
                puts_P(PSTR("filament inserted"));
                triggerFilamentInserted();
            }
            else { //filament removal
                puts_P(PSTR("filament removed"));
                triggerFilamentRemoved();
            }
        }
    };
    
    void triggerFilamentInserted() {
        if (autoLoadEnabled/*  && (eFilamentAction == FilamentAction::None) */ && !(moves_planned() || IS_SD_PRINTING || usb_timer.running() || (lcd_commands_type == LcdCommands::Layer1Cal) || eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE))) {
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
        if (runoutEnabled/*  && (eFilamentAction == FilamentAction::None) */ && !saved_printing && (moves_planned() || IS_SD_PRINTING || usb_timer.running() || (lcd_commands_type == LcdCommands::Layer1Cal) || eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE))) {
            runoutEnabled = false;
            autoLoadEnabled = false;
            stop_and_save_print_to_ram(0, 0);
            restore_print_from_ram_and_continue(0);
            eeprom_update_byte((uint8_t*)EEPROM_FERROR_COUNT, eeprom_read_byte((uint8_t*)EEPROM_FERROR_COUNT) + 1);
            eeprom_update_word((uint16_t*)EEPROM_FERROR_COUNT_TOT, eeprom_read_word((uint16_t*)EEPROM_FERROR_COUNT_TOT) + 1);
            enquecommand_front_P((PSTR("M600")));
        }
    }
    
    bool autoLoadEnabled;
    bool runoutEnabled;
    bool oldFilamentPresent; //for creating filament presence switching events.
    bool ready;
    SensorActionOnError sensorActionOnError;
};

class IR_sensor: public Filament_sensor {
public:
    void init() {
        SET_INPUT(IR_SENSOR_PIN); //input mode
        WRITE(IR_SENSOR_PIN, 1); //pullup
        settings_init();
    }
    
    void update() {
        if (!ready) {
            ready = true; //the IR sensor gets ready instantly as it's just a gpio read operation.
            oldFilamentPresent = getFilamentPresent(); //initialize the current filament state so that we don't create a switching event right after the sensor is ready.
        }
        checkFilamentEvents();
        ;//
    }
    
    bool getFilamentPresent() {
        return !READ(IR_SENSOR_PIN);
    }
    
    void settings_init() {
        Filament_sensor::settings_init();
    }
protected:
};

class IR_sensor_analog: public IR_sensor {
public:
    void init() {
        IR_sensor::init();
        ;//
    }
    
    void update() {
        IR_sensor::update();
        ;//
    }
    
    void voltUpdate(uint16_t raw) { //to be called from the ADC ISR when a cycle is finished
        voltRaw = raw;
        voltReady = true;
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
private:
    SensorRevision sensorRevision;
    bool voltReady; //this gets set by the adc ISR
    uint16_t voltRaw;
};

extern IR_sensor_analog fsensor;
