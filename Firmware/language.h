#ifndef LANGUAGE_H
#define LANGUAGE_H

#define PROTOCOL_VERSION "1.0"

#if MB(ULTIMAKER)|| MB(ULTIMAKER_OLD)|| MB(ULTIMAIN_2)
  #define MACHINE_NAME "Ultimaker"
  #define FIRMWARE_URL "http://firmware.ultimaker.com"
#elif MB(RUMBA)
  #define MACHINE_NAME "Rumba"
  #define FIRMWARE_URL "https://github.com/MarlinFirmware/Marlin"
#elif MB(3DRAG)
  #define MACHINE_NAME "3Drag"
  #define FIRMWARE_URL "http://3dprint.elettronicain.it/"
#elif MB(5DPRINT)
  #define MACHINE_NAME "Makibox"
  #define FIRMWARE_URL "https://github.com/MarlinFirmware/Marlin"
#elif MB(SAV_MKI)
  #define MACHINE_NAME "SAV MkI"
  #define FIRMWARE_URL "https://github.com/fmalpartida/Marlin/tree/SAV-MkI-config"
#else
  #ifdef CUSTOM_MENDEL_NAME
   // #define CUSTOM_MENDEL_NAME CUSTOM_MENDEL_NAME
  #else
    #define MACHINE_NAME "Mendel"
  #endif

// Default firmware set to Mendel
  #define FIRMWARE_URL "https://github.com/MarlinFirmware/Marlin"
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
