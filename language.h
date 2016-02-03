#ifndef LANGUAGE_H
#define LANGUAGE_H

#define LANGUAGE_CONCAT(M)       #M
#define GENERATE_LANGUAGE_INCLUDE(M)  LANGUAGE_CONCAT(language_##M.h)


// NOTE: IF YOU CHANGE LANGUAGE FILES OR MERGE A FILE WITH CHANGES
//
//   ==> ALWAYS TRY TO COMPILE MARLIN WITH/WITHOUT "ULTIPANEL" / "ULTRALCD" / "SDSUPPORT" #define IN "Configuration.h"
//   ==> ALSO TRY ALL AVAILABLE LANGUAGE OPTIONS

// Languages
// en English
// pl Polish
// fr French
// de German
// es Spanish
// ru Russian
// it Italian
// pt Portuguese
// fi Finnish
// an Aragonese
// nl Dutch
// ca Catalan
// eu Basque-Euskera



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
    #define MACHINE_NAME CUSTOM_MENDEL_NAME
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


// Common LCD messages

  /* nothing here yet */

// Common serial messages
#define MSG_MARLIN "Marlin"

// Serial Console Messages (do not translate those!)


// LCD Menu Messages

#include LANGUAGE_INCLUDE

#endif //__LANGUAGE_H
