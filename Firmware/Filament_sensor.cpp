#include <avr/pgmspace.h>
#include <stdio.h>
#include <util/atomic.h>

#include "Filament_sensor.h"
#include "Timer.h"
#include "cardreader.h"
#include "eeprom.h"
#include "menu.h"
#include "planner.h"
#include "temperature.h"
#include "ultralcd.h"

#ifdef FILAMENT_SENSOR
FSensorBlockRunout::FSensorBlockRunout() {
    fsensor.setRunoutEnabled(false); //suppress filament runouts while loading filament.
    fsensor.setAutoLoadEnabled(false); //suppress filament autoloads while loading filament.
#if (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
    fsensor.setJamDetectionEnabled(false); //suppress filament jam detection while loading filament.
#endif //(FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
//    SERIAL_ECHOLNPGM("FSBlockRunout");
}

FSensorBlockRunout::~FSensorBlockRunout() {
    fsensor.settings_init(); // restore filament runout state.
//    SERIAL_ECHOLNPGM("FSUnBlockRunout");
}

# if FILAMENT_SENSOR_TYPE == FSENSOR_IR
IR_sensor fsensor;
# elif FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG
IR_sensor_analog fsensor;
# elif FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125
PAT9125_sensor fsensor;
# endif

#else // FILAMENT_SENSOR
FSensorBlockRunout::FSensorBlockRunout() { }
FSensorBlockRunout::~FSensorBlockRunout() { }
#endif // FILAMENT_SENSOR

void Filament_sensor::setEnabled(bool enabled) {
    eeprom_update_byte((uint8_t *)EEPROM_FSENSOR, enabled);
    if (enabled) {
        fsensor.init();
    } else {
        fsensor.deinit();
    }
}

void Filament_sensor::setAutoLoadEnabled(bool state, bool updateEEPROM) {
    autoLoadEnabled = state;
    if (updateEEPROM) {
        eeprom_update_byte((uint8_t *)EEPROM_FSENS_AUTOLOAD_ENABLED, state);
    }
}

void Filament_sensor::setRunoutEnabled(bool state, bool updateEEPROM) {
    runoutEnabled = state;
    if (updateEEPROM) {
        eeprom_update_byte((uint8_t *)EEPROM_FSENS_RUNOUT_ENABLED, state);
    }
}

void Filament_sensor::setActionOnError(SensorActionOnError state, bool updateEEPROM) {
    sensorActionOnError = state;
    if (updateEEPROM) {
        eeprom_update_byte((uint8_t *)EEPROM_FSENSOR_ACTION_NA, (uint8_t)state);
    }
}

void Filament_sensor::settings_init_common() {
    bool enabled = eeprom_read_byte((uint8_t *)EEPROM_FSENSOR);
    if ((state != State::disabled) != enabled) {
        state = enabled ? State::initializing : State::disabled;
    }

    autoLoadEnabled = eeprom_read_byte((uint8_t *)EEPROM_FSENS_AUTOLOAD_ENABLED);
    runoutEnabled = eeprom_read_byte((uint8_t *)EEPROM_FSENS_RUNOUT_ENABLED);
    sensorActionOnError = (SensorActionOnError)eeprom_read_byte((uint8_t *)EEPROM_FSENSOR_ACTION_NA);
    if (sensorActionOnError == SensorActionOnError::_Undef) {
        sensorActionOnError = SensorActionOnError::_Continue;
    }
}

bool Filament_sensor::checkFilamentEvents() {
    if (state != State::ready)
        return false;
    if (eventBlankingTimer.running() && !eventBlankingTimer.expired(100)) { // event blanking for 100ms
        return false;
    }

    bool newFilamentPresent = fsensor.getFilamentPresent();
    if (oldFilamentPresent != newFilamentPresent) {
        oldFilamentPresent = newFilamentPresent;
        eventBlankingTimer.start();
        if (newFilamentPresent) { // filament insertion
//            puts_P(PSTR("filament inserted"));
            triggerFilamentInserted();
            postponedLoadEvent = true;
        } else { // filament removal
//            puts_P(PSTR("filament removed"));
            triggerFilamentRemoved();
        }
        return true;
    }
    return false;
}

void Filament_sensor::triggerFilamentInserted() {
    if (autoLoadEnabled
        && (eFilamentAction == FilamentAction::None)
        && (! MMU2::mmu2.Enabled() ) // quick and dirty hack to prevent spurious runouts while the MMU is in charge
        && !(
            moves_planned() != 0
            || IS_SD_PRINTING
            || usb_timer.running()
            || (lcd_commands_type == LcdCommands::Layer1Cal)
            || eeprom_read_byte((uint8_t *)EEPROM_WIZARD_ACTIVE)
            )
        ) {
        filAutoLoad();
    }
}

void Filament_sensor::triggerFilamentRemoved() {
//    SERIAL_ECHOLNPGM("triggerFilamentRemoved");
    if (runoutEnabled
        && (! MMU2::mmu2.Enabled() ) // quick and dirty hack to prevent spurious runouts just before the toolchange
        && (eFilamentAction == FilamentAction::None)
        && !saved_printing
        && (
            moves_planned() != 0
            || IS_SD_PRINTING
            || usb_timer.running()
            || (lcd_commands_type == LcdCommands::Layer1Cal)
            || eeprom_read_byte((uint8_t *)EEPROM_WIZARD_ACTIVE)
        )
    ){
//        SERIAL_ECHOPGM("runoutEnabled="); SERIAL_ECHOLN((int)runoutEnabled);
//        SERIAL_ECHOPGM("eFilamentAction="); SERIAL_ECHOLN((int)eFilamentAction);
//        SERIAL_ECHOPGM("saved_printing="); SERIAL_ECHOLN((int)saved_printing);
        filRunout();
    }
}

void Filament_sensor::filAutoLoad() {
    eFilamentAction = FilamentAction::AutoLoad;
    if (target_temperature[0] >= EXTRUDE_MINTEMP) {
        bFilamentPreheatState = true;
        menu_submenu(mFilamentItemForce);
    } else {
        menu_submenu(lcd_generic_preheat_menu);
        lcd_timeoutToStatus.start();
    }
}

void Filament_sensor::filRunout() {
//    SERIAL_ECHOLNPGM("filRunout");
    runoutEnabled = false;
    autoLoadEnabled = false;
    stop_and_save_print_to_ram(0, 0);
    restore_print_from_ram_and_continue(0);
    eeprom_increment_byte((uint8_t *)EEPROM_FERROR_COUNT);
    eeprom_increment_word((uint16_t *)EEPROM_FERROR_COUNT_TOT);
    enquecommand_front_P((PSTR("M600")));
}

void Filament_sensor::triggerError() {
    state = State::error;

    /// some message, idk
    ; //
}

#if (FILAMENT_SENSOR_TYPE == FSENSOR_IR) || (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
void IR_sensor::init() {
    if (state == State::error) {
        fsensor.deinit(); // deinit first if there was an error.
    }
//    puts_P(PSTR("fsensor::init()"));
    SET_INPUT(IR_SENSOR_PIN); // input mode
    WRITE(IR_SENSOR_PIN, 1);  // pullup
    settings_init();          // also sets the state to State::initializing
}

void IR_sensor::deinit() {
//    puts_P(PSTR("fsensor::deinit()"));
    SET_INPUT(IR_SENSOR_PIN); // input mode
    WRITE(IR_SENSOR_PIN, 0);  // no pullup
    state = State::disabled;
}

bool IR_sensor::update() {
    switch (state) {
    case State::initializing:
        state = State::ready; // the IR sensor gets ready instantly as it's just a gpio read operation.
        // initialize the current filament state so that we don't create a switching event right after the sensor is ready.
        oldFilamentPresent = fsensor.getFilamentPresent();
        [[fallthrough]];
    case State::ready: {
        postponedLoadEvent = false;
        return checkFilamentEvents();
    } break;
    case State::disabled:
    case State::error:
    default:
        return false;
    }
    return false;
}



#ifdef FSENSOR_PROBING
bool IR_sensor::probeOtherType() { return pat9125_probe(); }
#endif

void IR_sensor::settings_init() { Filament_sensor::settings_init_common(); }

#if (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
void IR_sensor_analog::init() {
    IR_sensor::init();
    IR_sensor::settings_init();
    sensorRevision = (SensorRevision)eeprom_read_byte((uint8_t *)EEPROM_FSENSOR_PCB);
}

bool IR_sensor_analog::update() {
    bool event = IR_sensor::update();
    if (state == State::ready) {
        if (getVoltReady()) {
            clearVoltReady();
            uint16_t volt = getVoltRaw();
//            printf_P(PSTR("newVoltRaw:%u\n"), volt / OVERSAMPLENR);

            // detect min-max, some long term sliding window for filtration may be added
            // avoiding floating point operations, thus computing in raw
            if (volt > maxVolt) {
                maxVolt = volt;
            } else if (volt < minVolt) {
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
            if (minVolt >= IRsensor_Ldiode_TRESHOLD && minVolt <= IRsensor_Lmax_TRESHOLD && maxVolt >= IRsensor_Hmin_TRESHOLD &&
                maxVolt <= IRsensor_Hopen_TRESHOLD) {
                IR_ANALOG_Check(SensorRevision::_Old, SensorRevision::_Rev04);
            }
            //! If and only if minVolt is in range <0.0, 0.3> and maxVolt is in range  <4.6, 5.0V>, I'm considering a situation with the old fsensor
            //! Note, we are not relying on one voltage here - getting just +5V can mean an old fsensor or a broken new sensor - that's why
            //! we need to have both voltages detected correctly to allow switching back to the old fsensor.
            else if (minVolt < IRsensor_Ldiode_TRESHOLD && maxVolt > IRsensor_Hopen_TRESHOLD && maxVolt <= IRsensor_VMax_TRESHOLD) {
                IR_ANALOG_Check(SensorRevision::_Rev04, SensorRevision::_Old);
            }

            if (!checkVoltage(volt)) {
                triggerError();
            }
        }
    }

    ; //

    return event;
}

void IR_sensor_analog::voltUpdate(uint16_t raw) { // to be called from the ADC ISR when a cycle is finished
    voltRaw = raw;
    voltReady = true;
}

uint16_t IR_sensor_analog::getVoltRaw() {
    uint16_t ret;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { ret = voltRaw; }
    return ret;
}

const char *IR_sensor_analog::getIRVersionText() {
    switch (sensorRevision) {
    case SensorRevision::_Old:
        return _T(MSG_IR_03_OR_OLDER);
    case SensorRevision::_Rev04:
        return _T(MSG_IR_04_OR_NEWER);
    default:
        return _T(MSG_IR_UNKNOWN);
    }
}

void IR_sensor_analog::setSensorRevision(SensorRevision rev, bool updateEEPROM) {
    sensorRevision = rev;
    if (updateEEPROM) {
        eeprom_update_byte((uint8_t *)EEPROM_FSENSOR_PCB, (uint8_t)rev);
    }
}

bool IR_sensor_analog::checkVoltage(uint16_t raw) {
    if (IRsensor_Lmax_TRESHOLD <= raw && raw <= IRsensor_Hmin_TRESHOLD) {
        /// If the voltage is in forbidden range, the fsensor is ok, but the lever is mounted improperly.
        /// Or the user is so creative so that he can hold a piece of fillament in the hole in such a genius way,
        /// that the IR fsensor reading is within 1.5 and 3V ... this would have been highly unusual
        /// and would have been considered more like a sabotage than normal printer operation
        if (voltageErrorCnt++ > 4) {
            puts_P(PSTR("fsensor in forbidden range 1.5-3V - check sensor"));
            return false;
        }
    } else {
        voltageErrorCnt = 0;
    }
    if (sensorRevision == SensorRevision::_Rev04) {
        /// newer IR sensor cannot normally produce 4.6-5V, this is considered a failure/bad mount
        if (IRsensor_Hopen_TRESHOLD <= raw && raw <= IRsensor_VMax_TRESHOLD) {
            puts_P(PSTR("fsensor v0.4 in fault range 4.6-5V - unconnected"));
            return false;
        }
        /// newer IR sensor cannot normally produce 0-0.3V, this is considered a failure
#if 0 // Disabled as it has to be decided if we gonna use this or not.
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

bool IR_sensor_analog::getVoltReady() const {
    bool ret;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){ ret = voltReady; }
    return ret;
}

void IR_sensor_analog::clearVoltReady(){
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){ voltReady = false; }
}

void IR_sensor_analog::IR_ANALOG_Check(SensorRevision isVersion, SensorRevision switchTo) {
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
                lcd_setstatuspgm(_T(MSG_IR_03_OR_OLDER));
                break;
            case SensorRevision::_Rev04:
                lcd_setstatuspgm(_T(MSG_IR_04_OR_NEWER));
                break;
            default:
                break;
            }
        }
    } else {
        nFSCheckCount = 0;
    }
}
#endif //(FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
#endif //(FILAMENT_SENSOR_TYPE == FSENSOR_IR) || (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)

#if (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
void PAT9125_sensor::init() {
    if (state == State::error) {
        deinit(); // deinit first if there was an error.
    }
//    puts_P(PSTR("fsensor::init()"));

    settings_init(); // also sets the state to State::initializing

    calcChunkSteps(cs.axis_steps_per_unit[E_AXIS]); // for jam detection

    if (!pat9125_init()) {
        deinit();
        triggerError();
        ; //
    }
#ifdef IR_SENSOR_PIN
    else if (!READ(IR_SENSOR_PIN)) {
        ; // MK3 fw on MK3S printer
    }
#endif // IR_SENSOR_PIN
}

void PAT9125_sensor::deinit() {
//    puts_P(PSTR("fsensor::deinit()"));
    ; //
    state = State::disabled;
    filter = 0;
}

bool PAT9125_sensor::update() {
    switch (state) {
    case State::initializing:
        if (!updatePAT9125()) {
            break; // still not stable. Stay in the initialization state.
        }
        oldFilamentPresent =
            getFilamentPresent(); // initialize the current filament state so that we don't create a switching event right after the sensor is ready.
        oldPos = pat9125_y;
        state = State::ready;
        break;
    case State::ready: {
        updatePAT9125();
        postponedLoadEvent = false;
        bool event = checkFilamentEvents();

        ; //

        return event;
    } break;
    case State::disabled:
    case State::error:
    default:
        return false;
    }
    return false;
}

#ifdef FSENSOR_PROBING
bool PAT9125_sensor::probeOtherType() {
    SET_INPUT(IR_SENSOR_PIN); // input mode
    WRITE(IR_SENSOR_PIN, 1);  // pullup
    _delay_us(100); // wait for the pullup to pull the line high (might be needed, not really sure. The internal pullups are quite weak and there might be a
                    // long wire attached).
    bool fsensorDetected = !READ(IR_SENSOR_PIN);
    WRITE(IR_SENSOR_PIN, 0); // no pullup
    return fsensorDetected;
}
#endif

void PAT9125_sensor::setJamDetectionEnabled(bool state, bool updateEEPROM) {
    jamDetection = state;
    oldPos = pat9125_y;
    resetStepCount();
    jamErrCnt = 0;
    if (updateEEPROM) {
        eeprom_update_byte((uint8_t *)EEPROM_FSENSOR_JAM_DETECTION, state);
    }
}

void PAT9125_sensor::settings_init() {
//    puts_P(PSTR("settings_init"));
    Filament_sensor::settings_init_common();
    setJamDetectionEnabled(eeprom_read_byte((uint8_t *)EEPROM_FSENSOR_JAM_DETECTION));
}

int16_t PAT9125_sensor::getStepCount() {
    int16_t ret;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { ret = stepCount; }
    return ret;
}

void PAT9125_sensor::resetStepCount() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { stepCount = 0; }
}

void PAT9125_sensor::filJam() {
    runoutEnabled = false;
    autoLoadEnabled = false;
    jamDetection = false;
    stop_and_save_print_to_ram(0, 0);
    restore_print_from_ram_and_continue(0);
    eeprom_increment_byte((uint8_t *)EEPROM_FERROR_COUNT);
    eeprom_increment_word((uint16_t *)EEPROM_FERROR_COUNT_TOT);
    enquecommand_front_P((PSTR("M600")));
}

bool PAT9125_sensor::updatePAT9125() {
    if (jamDetection) {
        int16_t _stepCount = getStepCount();
        if (abs(_stepCount) >= chunkSteps) { // end of chunk. Check distance
            resetStepCount();
            if (!pat9125_update()) { // get up to date data. reinit on error.
                init();              // try to reinit.
            }
            bool fsDir = (pat9125_y - oldPos) > 0;
            bool stDir = _stepCount > 0;
            if (fsDir != stDir) {
                jamErrCnt++;
            } else if (jamErrCnt) {
                jamErrCnt--;
            }
            oldPos = pat9125_y;
        }
        if (jamErrCnt > 10) {
            jamErrCnt = 0;
            filJam();
        }
    }

    if (!pollingTimer.running() || pollingTimer.expired(pollingPeriod)) {
        pollingTimer.start();
        if (!pat9125_update()) {
            init(); // try to reinit.
        }

        bool present = (pat9125_s < 17) || (pat9125_s >= 17 && pat9125_b >= 50);
        if (present != filterFilPresent) {
            filter++;
        } else if (filter) {
            filter--;
        }
        if (filter >= filterCnt) {
            filter = 0;
            filterFilPresent = present;
        }
    }
    return (filter == 0); // return stability
}
#endif // #if (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
