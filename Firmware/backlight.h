//backlight.h
#ifndef _BACKLIGHT_H
#define _BACKLIGHT_H

#include <inttypes.h>
#include "Marlin.h"
#include "pins.h"

enum Backlight_Mode
{
	BACKLIGHT_MODE_BRIGHT  = 0,
	BACKLIGHT_MODE_DIM     = 1,
	BACKLIGHT_MODE_AUTO    = 2,
};

extern int16_t backlightLevel_HIGH;
extern int16_t backlightLevel_LOW;
extern uint8_t backlightMode;
extern bool backlightSupport;
extern int16_t backlightTimer_period;

#ifdef LCD_BL_PIN
    #define FORCE_BL_ON_START   force_bl_on(true)
    #define FORCE_BL_ON_END	    force_bl_on(false)
#else
    #define FORCE_BL_ON_START
    #define FORCE_BL_ON_END
#endif
extern void force_bl_on(bool section_start);
extern void backlight_update();
extern void backlight_init();
extern void backlightTimer_reset();
extern void backlight_save();


#endif //_BACKLIGHT_H
