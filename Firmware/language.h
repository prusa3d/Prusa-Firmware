#ifndef LANGUAGE_H
#define LANGUAGE_H

#define PROTOCOL_VERSION "1.0"

#ifdef CUSTOM_MENDEL_NAME
   // #define CUSTOM_MENDEL_NAME CUSTOM_MENDEL_NAME
#else
    #define MACHINE_NAME "Mendel"
#endif



#ifndef MACHINE_UUID
   #define MACHINE_UUID "00000000-0000-0000-0000-000000000000"
#endif

#define MSG_FW_VERSION                   "Firmware"

#define STRINGIFY_(n) #n
#define STRINGIFY(n) STRINGIFY_(n)


// Common serial messages
#define MSG_MARLIN "Marlin"

// Serial Console Messages (do not translate those!)


// LCD Menu Messages
#include "language_all.h"

#endif //__LANGUAGE_H
