//backlight.h
#ifndef _BACKLIGHT_H
#define _BACKLIGHT_H

#include <inttypes.h>

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

extern void backlight_update();
extern void backlight_init();
extern void backlightTimer_reset();
extern void backlight_save();


#endif //_BACKLIGHT_H
