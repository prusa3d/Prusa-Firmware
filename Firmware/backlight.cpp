//backlight.cpp

#include "backlight.h"
#include <avr/eeprom.h>
#include <Arduino.h>
#include "eeprom.h"
#include "Marlin.h"
#include "pins.h"
#include "fastio.h"
#include "Timer.h"
// #include "Configuration.h"

#ifdef LCD_BL_PIN

bool backlightSupport = 0;
int16_t backlightLevel = 0;
int16_t backlightLevel_old = 0;
unsigned long backlightTimer_period = 10000ul;
bool backlightIsDimmed = true;
LongTimer backlightTimer;

static void backlightDim()
{
    // if (backlightIsDimmed) return;
    backlightLevel /= 4; //make the display dimmer.
    backlightIsDimmed = true;
}

static void backlightWake()
{
    // if (!backlightIsDimmed) return;
    backlightLevel = eeprom_read_byte((uint8_t *)EEPROM_BACKLIGHT_LEVEL);
    backlightIsDimmed = false;
}

void backlightTimer_reset() //used for resetting the timer and waking the display
{
    backlightTimer.start();
    if (backlightIsDimmed) backlightWake();
}

void backlight_update()
{
    if (!backlightSupport) return;
    if (backlightTimer.expired(backlightTimer_period)) backlightDim();
    
    if (backlightLevel != backlightLevel_old) //update pwm duty cycle
    {
        analogWrite(LCD_BL_PIN, backlightLevel);
        backlightLevel_old = backlightLevel;
        
        if (!backlightIsDimmed) eeprom_update_byte((uint8_t *)EEPROM_BACKLIGHT_LEVEL, backlightLevel); //update eeprom value
    }
}

void backlight_init()
{
//check for backlight support on lcd
    SET_INPUT(LCD_BL_PIN);
    WRITE(LCD_BL_PIN,HIGH);
    _delay(10);
    backlightSupport = !READ(LCD_BL_PIN);
    if (backlightSupport == 0) return;

//initialize backlight pin
    SET_OUTPUT(LCD_BL_PIN);
    WRITE(LCD_BL_PIN,0);
    backlightTimer_reset(); //initializes eeprom data and starts backlightTimer
    backlight_update(); //actually sets the backlight to the correct level
}

#endif //LCD_BL_PIN