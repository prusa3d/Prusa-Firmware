// motion.h

#ifndef _MOTION_h
#define _MOTION_h

#include "config.h"
#include <inttypes.h>
#include <stdbool.h>


extern int8_t filament_type[EXTRUDERS];
extern const int IDLER_PARKING_STEPS;
extern const int BOWDEN_LENGTH;
extern const int STEPS_MK3FSensor_To_Bondtech;
extern const int FILAMENT_PARKING_STEPS;

void home(bool doToolSync = false);
void engage_filament_pulley(bool engage);
void reset_engage_filament_pulley(bool previouslyEngaged);

void load_filament_into_extruder();

void set_positions(int _current_extruder, int _next_extruder);

void init_Pulley();

void move_idler(int steps, uint16_t speed = MAX_SPEED_IDL);
void move_selector(int steps, uint16_t speed = MAX_SPEED_SEL);
void move_pulley(int steps, uint16_t speed = MAX_SPEED_PUL);

void eject_filament(int extruder);
void recover_after_eject();

enum MotReturn {MR_Success, MR_FailedAndRehomed, MR_Failed};
MotReturn homeSelectorSmooth();
MotReturn moveSmooth(uint8_t axis, int steps, int speed,bool rehomeOnFail = true,
                     bool withStallDetection = true, float ACC = ACC_NORMAL, bool withFindaDetection = false, bool disengageAtEnd = false);
MotReturn homeIdlerSmooth();
MotReturn homeSelectorSmooth();
#endif
