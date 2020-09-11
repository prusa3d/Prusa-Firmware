#ifndef MACROS_H
#define MACROS_H


#define  FORCE_INLINE __attribute__((always_inline)) inline
#define _UNUSED __attribute__((unused))

#ifndef CRITICAL_SECTION_START
  #define CRITICAL_SECTION_START  unsigned char _sreg = SREG; cli();
  #define CRITICAL_SECTION_END    SREG = _sreg;
#endif //CRITICAL_SECTION_START

// Macros to make a string from a macro
#define STRINGIFY_(M) #M
#define STRINGIFY(M) STRINGIFY_(M)


#endif //MACROS_H
