#ifndef MACROS_H
#define MACROS_H

#include <avr/interrupt.h> //for cli() and sei()

#define  FORCE_INLINE __attribute__((always_inline)) inline
#define _UNUSED __attribute__((unused))

#ifndef CRITICAL_SECTION_START
  #define CRITICAL_SECTION_START  unsigned char _sreg = SREG; cli();
  #define CRITICAL_SECTION_END    SREG = _sreg;
#endif //CRITICAL_SECTION_START

// Macros to make a string from a macro
#define STRINGIFY_(M) #M
#define STRINGIFY(M) STRINGIFY_(M)

// Macros for bit masks
#undef _BV
#define _BV(n) (1<<(n))
#define TEST(n,b) (!!((n)&_BV(b)))
#define SET_BIT_TO(N,B,TF) do{ if (TF) SBI(N,B); else CBI(N,B); }while(0)

#ifndef SBI
  #define SBI(A,B) (A |= (1 << (B)))
#endif

#ifndef CBI
  #define CBI(A,B) (A &= ~(1 << (B)))
#endif

#define TBI(N,B) (N ^= _BV(B))


#endif //MACROS_H
