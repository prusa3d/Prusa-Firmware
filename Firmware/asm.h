#pragma once
#include <stdint.h>

#ifdef __AVR_ATmega2560__

// return the current PC (on AVRs with 22bit PC)
static inline void GETPC(uint32_t* v)
{
  uint8_t a, b, c;
  asm
  (
      "rcall .\n"
      "pop %2\n"
      "pop %1\n"
      "pop %0\n"
      : "=r" (a), "=r" (b), "=r" (c)
  );
  ((uint8_t*)v)[0] = a;
  ((uint8_t*)v)[1] = b;
  ((uint8_t*)v)[2] = c;
  ((uint8_t*)v)[3] = 0;
}

#endif
