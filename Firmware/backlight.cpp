//backlight.cpp

#include "backlight.h"
#include <avr/eeprom.h>
#include <Arduino.h>
#include "eeprom.h"
#include "Marlin.h"
#include "pins.h"
#include "fastio.h"
#include "Timer.h"

#ifdef LCD_BL_PIN

bool backlightSupport = 0; //only if it's true will any of the settings be visible to the user
int16_t backlightLevel_HIGH = 0;
int16_t backlightLevel_LOW = 0;
uint8_t backlightMode = BACKLIGHT_MODE_BRIGHT;
int16_t backlightTimer_period = 10;
LongTimer backlightTimer;

void backlight_save() //saves all backlight data to eeprom.
{
    eeprom_update_byte((uint8_t *)EEPROM_BACKLIGHT_LEVEL_HIGH, (uint8_t)backlightLevel_HIGH);
    eeprom_update_byte((uint8_t *)EEPROM_BACKLIGHT_LEVEL_LOW, (uint8_t)backlightLevel_LOW);
    eeprom_update_byte((uint8_t *)EEPROM_BACKLIGHT_MODE, backlightMode);
    eeprom_update_word((uint16_t *)EEPROM_BACKLIGHT_TIMEOUT, backlightTimer_period);
}

void backlightTimer_reset() //used for resetting the timer and waking the display. Triggered on events such as knob click, rotate and on full screen notifications.
{
    if (!backlightSupport) return;
    backlightTimer.start();
}

void backlight_update()
{
    if (!backlightSupport) return;
    
    if (backlightMode == BACKLIGHT_MODE_AUTO)
    {
        if (backlightTimer.expired((uint32_t)backlightTimer_period * 1000ul)) analogWrite(LCD_BL_PIN, backlightLevel_LOW);
        else if (backlightTimer.running()) analogWrite(LCD_BL_PIN, backlightLevel_HIGH);
        else {/*do nothing*/;} //display is dimmed.
    }
    else if (backlightMode == BACKLIGHT_MODE_DIM) analogWrite(LCD_BL_PIN, backlightLevel_LOW);
    else analogWrite(LCD_BL_PIN, backlightLevel_HIGH);
}

void backlight_init()
{
//check for backlight support on lcd
    SET_INPUT(LCD_BL_PIN);
    WRITE(LCD_BL_PIN,HIGH);
    _delay(10);
    backlightSupport = !READ(LCD_BL_PIN);
    if (!backlightSupport) return;

//initialize backlight
    backlightMode = eeprom_read_byte((uint8_t *)EEPROM_BACKLIGHT_MODE);
    if (backlightMode == 0xFF) //set default values
    {
        backlightMode = BACKLIGHT_MODE_AUTO;
        backlightLevel_HIGH = 130;
        backlightLevel_LOW = 50;
        backlightTimer_period = 10; //in seconds
        backlight_save();
    }
    backlightLevel_HIGH = eeprom_read_byte((uint8_t *)EEPROM_BACKLIGHT_LEVEL_HIGH);
    backlightLevel_LOW = eeprom_read_byte((uint8_t *)EEPROM_BACKLIGHT_LEVEL_LOW);
    backlightTimer_period = eeprom_read_word((uint16_t *)EEPROM_BACKLIGHT_TIMEOUT);
    
    SET_OUTPUT(LCD_BL_PIN);
    backlightTimer_reset();
    backlight_update(); //sets brightness
}

#endif //LCD_BL_PIN