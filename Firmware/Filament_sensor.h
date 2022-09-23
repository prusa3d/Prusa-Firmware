#pragma once
#include <inttypes.h>

#include "cmdqueue.h"
#include "pins.h"
#include "fastio.h"
#include "adc.h"
#include "pat9125.h"

#define FSENSOR_IR 1
#define FSENSOR_IR_ANALOG 2
#define FSENSOR_PAT9125 3

/// Can be used to block printer's filament sensor handling - to avoid errorneous injecting of M600
/// while doing a toolchange with the MMU
/// In case of "no filament sensor" these methods default to an empty implementation
class FSensorBlockRunout {
public:
    FSensorBlockRunout();
    ~FSensorBlockRunout();
};

/// Base class Filament sensor
/// 
/// Ideally, there could have been a nice class hierarchy of filament sensor types with common functionality
/// extracted into this base class.
/// But:
/// - virtual methods take more space
/// - we don't need to switch among different filament sensors at runtime
/// Therefore the class hierarchy carefully avoids using virtual methods and doesn't look too fancy.
#ifdef FILAMENT_SENSOR
class Filament_sensor {
public:
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
    
    static void setEnabled(bool enabled);
    
    void setAutoLoadEnabled(bool state, bool updateEEPROM = false);
    bool getAutoLoadEnabled() const { return autoLoadEnabled; }
    
    void setRunoutEnabled(bool state, bool updateEEPROM = false);
    bool getRunoutEnabled() const { return runoutEnabled; }
    
    void setActionOnError(SensorActionOnError state, bool updateEEPROM = false);
    SensorActionOnError getActionOnError() const { return sensorActionOnError; }
    
    bool getFilamentLoadEvent() const { return postponedLoadEvent; }
    
    bool isError() const { return state == State::error; }
    bool isReady() const { return state == State::ready; }
    bool isEnabled() const { return state != State::disabled; }
    
protected:
    void settings_init_common();
    
    bool checkFilamentEvents();
    
    void triggerFilamentInserted();
    
    void triggerFilamentRemoved();
    
    static void filAutoLoad();
    
    void filRunout();
    
    void triggerError();
    
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
    void init();
    void deinit();
    bool update();
    bool getFilamentPresent() const { return !READ(IR_SENSOR_PIN); }
#ifdef FSENSOR_PROBING
    static bool probeOtherType(); //checks if the wrong fsensor type is detected.
#endif
    void settings_init();
};

#if (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
constexpr static uint16_t Voltage2Raw(float V) {
    return (V * 1023 * OVERSAMPLENR / VOLT_DIV_REF ) + 0.5F;
}
constexpr static float Raw2Voltage(uint16_t raw) {
    return VOLT_DIV_REF * (raw / (1023.F * OVERSAMPLENR));
}

class IR_sensor_analog: public IR_sensor {
public:
    void init();
    bool update();
    void voltUpdate(uint16_t raw);
    
    uint16_t __attribute__((noinline)) getVoltRaw();
    
    enum class SensorRevision : uint8_t {
        _Old = 0,
        _Rev04 = 1,
        _Undef = EEPROM_EMPTY_VALUE
    };
    
    SensorRevision getSensorRevision() const { return sensorRevision; }
    
    const char* __attribute__((noinline)) getIRVersionText();
    
    void setSensorRevision(SensorRevision rev, bool updateEEPROM = false);
    
    constexpr static uint16_t IRsensor_Ldiode_TRESHOLD = Voltage2Raw(0.3F); // ~0.3V, raw value=982
    constexpr static uint16_t IRsensor_Lmax_TRESHOLD = Voltage2Raw(1.5F); // ~1.5V (0.3*Vcc), raw value=4910
    constexpr static uint16_t IRsensor_Hmin_TRESHOLD = Voltage2Raw(3.0F); // ~3.0V (0.6*Vcc), raw value=9821
    constexpr static uint16_t IRsensor_Hopen_TRESHOLD = Voltage2Raw(4.6F); // ~4.6V (N.C. @ Ru~20-50k, Rd'=56k, Ru'=10k), raw value=15059
    constexpr static uint16_t IRsensor_VMax_TRESHOLD = Voltage2Raw(5.F); // ~5V, raw value=16368
    
private:
    SensorRevision sensorRevision;
    
    bool voltReady; // set by the adc ISR, therefore avoid accessing the variable directly but use getVoltReady()
    bool getVoltReady()const;
    void clearVoltReady();
    
    uint16_t voltRaw; // set by the adc ISR, therefore avoid accessing the variable directly but use getVoltRaw()
    bool checkVoltage(uint16_t raw);
    
    uint16_t minVolt = Voltage2Raw(6.F);
    uint16_t maxVolt = 0;
    uint16_t nFSCheckCount;
    uint8_t voltageErrorCnt;

    static constexpr uint16_t FS_CHECK_COUNT = 4;
    /// Switching mechanism of the fsensor type.
    /// Called from 2 spots which have a very similar behavior
    /// 1: SensorRevision::_Old -> SensorRevision::_Rev04 and print _i("FS v0.4 or newer")
    /// 2: SensorRevision::_Rev04 -> sensorRevision=SensorRevision::_Old and print _i("FS v0.3 or older")
    void IR_ANALOG_Check(SensorRevision isVersion, SensorRevision switchTo);
};
#endif //(FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
#endif //(FILAMENT_SENSOR_TYPE == FSENSOR_IR) || (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)

#if (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
class PAT9125_sensor: public Filament_sensor {
public:
    void init();
    void deinit();
    bool update();
    bool getFilamentPresent() const { return filterFilPresent; }
#ifdef FSENSOR_PROBING
    bool probeOtherType(); //checks if the wrong fsensor type is detected.
#endif
    
    void setJamDetectionEnabled(bool state, bool updateEEPROM = false);
    bool getJamDetectionEnabled() const { return jamDetection; }
    
    void stStep(bool rev) { //from stepper isr
        stepCount += rev ? -1 : 1;
    }
    
    void settings_init();
private:
    static constexpr uint16_t pollingPeriod = 10; //[ms]
    static constexpr uint8_t filterCnt = 5; //how many checks need to be done in order to determine the filament presence precisely.
    ShortTimer pollingTimer;
    uint8_t filter;
    uint8_t filterFilPresent;
    
    bool jamDetection;
    int16_t oldPos;
    int16_t stepCount;
    int16_t chunkSteps;
    uint8_t jamErrCnt;
    
    constexpr void calcChunkSteps(float u) {
        chunkSteps = (int16_t)(1.25 * u); //[mm]
    }
    
    int16_t getStepCount();
    
    void resetStepCount();
    
    void filJam();
    
    bool updatePAT9125();
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
