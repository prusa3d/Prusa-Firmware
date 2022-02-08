#ifndef SPEED_LOOKUPTABLE_H
#define SPEED_LOOKUPTABLE_H

#include "Marlin.h"

extern const uint16_t speed_lookuptable_fast[256][2] PROGMEM;
extern const uint16_t speed_lookuptable_slow[256][2] PROGMEM;

#ifndef _NO_ASM

// return ((x * y) >> 8) with rounding when shifting right
FORCE_INLINE uint16_t MUL8x16R8(uint8_t x, uint16_t y) {
    uint16_t out;
    __asm__ (
    // %0 out
    // %1 x
    // %2 y
    // uint8_t: %An or %n
    // uint16_t: %Bn %An
    // __uint24: %Cn %Bn %An
    // uint32_t: %Dn %Cn %Bn %An
    //
    //
    //    B2 A2 *
    //       A1
    //---------
    // B0 A0 RR
    "mul %B2, %A1" "\n\t"
    "movw %0, r0" "\n\t"
    "mul %A2, %A1" "\n\t"
    "lsl r0" "\n\t"         //push MSB to carry for rounding
    "adc %A0, r1" "\n\t"    //add with carry (for rounding)
    "clr r1" "\n\t"         //make r1 __zero_reg__ again
    "adc %B0, r1" "\n\t"    //propagate carry of addition (add 0 with carry)
    : "=&r" (out)
    : "r" (x), "r" (y)
    : "r0", "r1"            //clobbers: Technically these are either scratch registers or always 0 registers, but I'm making sure the compiler knows just in case.
    );
    return out;
}

// return ((x * y) >> 24) with rounding when shifting right
FORCE_INLINE uint16_t MUL24x24R24(__uint24 x, __uint24 y) {
    uint16_t out;
    __asm__ (
    // %0 out
    // %1 x
    // %2 y
    // uint8_t: %An or %n
    // uint16_t: %Bn %An
    // __uint24: %Cn %Bn %An
    // uint32_t: %Dn %Cn %Bn %An
    //
    //
    //          C2 B2 A2 *
    //          C1 B1 A1
    //------------------
    // -- B0 A0 RR RR RR
    "clr r26 \n\t"
    "mul %A1, %B2 \n\t"
    "mov r27, r1 \n\t"
    "mul %B1, %C2 \n\t"
    "movw %A0, r0 \n\t"
    "mul %C1, %C2 \n\t"
    "add %B0, r0 \n\t"
    "mul %C1, %B2 \n\t"
    "add %A0, r0 \n\t"
    "adc %B0, r1 \n\t"
    "mul %A1, %C2 \n\t"
    "add r27, r0 \n\t"
    "adc %A0, r1 \n\t"
    "adc %B0, r26 \n\t"
    "mul %B1, %B2 \n\t"
    "add r27, r0 \n\t"
    "adc %A0, r1 \n\t"
    "adc %B0, r26 \n\t"
    "mul %C1, %A2 \n\t"
    "add r27, r0 \n\t"
    "adc %A0, r1 \n\t"
    "adc %B0, r26 \n\t"
    "mul %B1, %A2 \n\t"
    "add r27, r1 \n\t"
    "adc %A0, r26 \n\t"
    "adc %B0, r26 \n\t"
    "lsl r27 \n\t"
    "adc %A0, r26 \n\t"
    "adc %B0, r26 \n\t"
    "clr r1 \n\t"
    : "=&r" (out)
    : "r" (x), "r" (y)
    : "r0", "r1", "r26" , "r27" //clobbers: Technically these are either scratch registers or always 0 registers, but I'm making sure the compiler knows just in case. R26 is __zero_reg__, R27 is a temporary register.
    );
    return out;
}

#else //_NO_ASM

FORCE_INLINE uint16_t MUL8x16R8(uint8_t charIn1, uint16_t intIn2)
{
    return ((uint32_t)charIn1 * (uint32_t)intIn2) >> 8;
}

FORCE_INLINE uint16_t MUL24x24R24(uint32_t longIn1, uint32_t longIn2)
{
    return ((uint64_t)longIn1 * (uint64_t)longIn2) >> 24;
}

#endif //_NO_ASM


FORCE_INLINE unsigned short calc_timer(uint16_t step_rate, uint8_t& step_loops) {
  uint16_t timer;
  if(step_rate > MAX_STEP_FREQUENCY) step_rate = MAX_STEP_FREQUENCY;

  if(step_rate > 20000) { // If steprate > 20kHz >> step 4 times
    step_rate = (step_rate >> 2)&0x3fff;
    step_loops = 4;
  }
  else if(step_rate > 10000) { // If steprate > 10kHz >> step 2 times
    step_rate = (step_rate >> 1)&0x7fff;
    step_loops = 2;
  }
  else {
    step_loops = 1;
  }

  if(step_rate < (F_CPU/500000)) step_rate = (F_CPU/500000);
  step_rate -= (F_CPU/500000); // Correct for minimal speed
  if(step_rate >= (8*256)){ // higher step rate
    unsigned short table_address = (unsigned short)&speed_lookuptable_fast[(unsigned char)(step_rate>>8)][0];
    unsigned char tmp_step_rate = (step_rate & 0x00ff);
    uint16_t gain = (uint16_t)pgm_read_word_near(table_address+2);
    timer = (unsigned short)pgm_read_word_near(table_address) - MUL8x16R8(tmp_step_rate, gain);
  }
  else { // lower step rates
    unsigned short table_address = (unsigned short)&speed_lookuptable_slow[0][0];
    table_address += ((step_rate)>>1) & 0xfffc;
    timer = (unsigned short)pgm_read_word_near(table_address);
    timer -= (((unsigned short)pgm_read_word_near(table_address+2) * (unsigned char)(step_rate & 0x0007))>>3);
  }
  if(timer < 100) { timer = 100; }//(20kHz this should never happen)////MSG_STEPPER_TOO_HIGH c=0 r=0
  return timer;
}

#endif
