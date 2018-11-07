#ifndef _MAIN_H
#define _MAIN_H

#include <inttypes.h>
#include "config.h"
#include "uart.h"

void manual_extruder_selector();

// system state
extern int8_t sys_state;

// signals from interrupt to main loop
extern uint8_t sys_signals;
extern bool load_filament_at_toolChange;
void process_signals();
bool load_filament_withSensor();
bool unload_filament_withSensor();
void fixTheProblem();

extern uint8_t tmc2130_mode;
extern bool fsensor_triggered;

// get state of signal (main loop or interrupt)
#define SIG_GET(id) (sys_signals & (1 << id))
// set state of signal (interrupt only)
#define SIG_SET(id) (sys_signals |= (1 << id))
// get state of signal (main loop only)
#define SIG_CLR(id)                                                                                                    \
    asm("cli");                                                                                                        \
    sys_signals &= ~(1 << id);                                                                                         \
    asm("sei")

typedef enum eFault {FAULT_IDLER_INIT_0, FAULT_IDLER_INIT_1, FAULT_IDLER_INIT_2,
                     FAULT_SELECTOR_INIT_0, FAULT_SELECTOR_INIT_1, FAULT_SELECTOR_INIT_2,
                     FAULT_PULLEY_INIT_0, FAULT_PULLEY_INIT_1, FAULT_PULLEY_INIT_2,
                    } Fault;

void fault_handler(Fault id);

#endif //_MAIN_H
