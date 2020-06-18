//conv2str.h - Float conversion utilities
#ifndef _CONV2STR_H
#define _CONV2STR_H

#include <inttypes.h>


char *itostr2(const uint8_t &x);
char *itostr31(const int &xx);
char *itostr3(const int &xx);
char *itostr3left(const int &xx);
char *itostr4(const int &xx);

char *ftostr3(const float &x);
char *ftostr31ns(const float &x); // float to string without sign character
char *ftostr31(const float &x);
char *ftostr32(const float &x);
char *ftostr32ns(const float &x);
char *ftostr43(const float &x, uint8_t offset = 0);
char *ftostr12ns(const float &x);
char *ftostr13ns(const float &x);
char *ftostr32sp(const float &x); // remove zero-padding from ftostr32
char *ftostr5(const float &x);
char *ftostr51(const float &x);
char *ftostr52(const float &x);


#endif //_CONV2STR_H
