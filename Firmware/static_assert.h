//static_assert.h
//portable solution compatible with C++98

#if (__cplusplus < 201103L) //std < C++11

//source http://www.pixelbeat.org/programming/gcc/STATIC_ASSERT.html
#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)

// These can't be used after statements in c89.
#ifdef __COUNTER__
  #define static_assert(e,m) \
    ;enum { ASSERT_CONCAT(STATIC_ASSERT_, __COUNTER__) = 1/(int)(!!(e)) }
#else
  //This can't be used twice on the same line so ensure if using in headers
  //that the headers are not included twice (by wrapping in #ifndef...#endif)
  //Note it doesn't cause an issue when used on same line of separate modules
  //compiled with gcc -combine -fwhole-program.
  #define static_assert(e,m) \
    ;enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(int)(!!(e)) }
#endif //__COUNTER__

#endif //(__cplusplus < 201103L)
