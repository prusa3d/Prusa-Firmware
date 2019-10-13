//backlight.h
#ifndef _BACKLIGHT_H
#define _BACKLIGHT_H

#include <inttypes.h>

extern int16_t backlightLevel;

extern void backlight_update();
extern void backlight_init();
extern void backlightTimer_reset();


#endif //_BACKLIGHT_H
