//backlight.cpp

#include "backlight.h"
#include <avr/eeprom.h>
#include <Arduino.h>
#include "eeprom.h"
#include "Marlin.h"
#include "pins.h"
#include "fastio.h"
// #include "Timer.h"
// #include "Configuration.h"

int16_t backlightLevel = 0;
int16_t backlightLevel_old = 0;
// uint16_t backlightCounter = 0;

void backlight_update()
{
    if (backlightLevel != backlightLevel_old) //update level
    {
        analogWrite(LCD_BL_PIN, backlightLevel);
        backlightLevel_old = backlightLevel;
        eeprom_update_byte((uint8_t *)EEPROM_BACKLIGHT_LEVEL, backlightLevel);
    }
}

void backlight_init()
{
    SET_OUTPUT(LCD_BL_PIN);
    WRITE(LCD_BL_PIN,0);
    backlightLevel = eeprom_read_byte((uint8_t *)EEPROM_BACKLIGHT_LEVEL);
    backlight_update();
}
