#pragma once
#include <stdint.h>
#include "macros.h"

#ifdef __AVR_ATmega2560__

// return the current PC (on AVRs with 22bit PC)
FORCE_INLINE __uint24 GETPC(void)
{
  __uint24 ret;
  asm (
      "rcall .\n"
      "pop %A0\n"
      "pop %B0\n"
      "pop %C0\n"
      : "=&r" (ret)
  );
  return ret;
}

#endif
