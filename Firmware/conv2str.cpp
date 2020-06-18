//conv2str.cpp - Float conversion utilities

#include "conv2str.h"
#include <stdlib.h>


//  convert float to string with +123.4 format
char conv[8];

char *ftostr3(const float &x)
{
  return itostr3((int)x);
}

char *itostr2(const uint8_t &x)
{
  //sprintf(conv,"%5.1f",x);
  int xx = x;
  conv[0] = (xx / 10) % 10 + '0';
  conv[1] = (xx) % 10 + '0';
  conv[2] = 0;
  return conv;
}

// Convert float to string with 123.4 format, dropping sign
char *ftostr31(const float &x)
{
  int xx = x * 10;
  conv[0] = (xx >= 0) ? '+' : '-';
  xx = abs(xx);
  conv[1] = (xx / 1000) % 10 + '0';
  conv[2] = (xx / 100) % 10 + '0';
  conv[3] = (xx / 10) % 10 + '0';
  conv[4] = '.';
  conv[5] = (xx) % 10 + '0';
  conv[6] = 0;
  return conv;
}

// Convert float to string with 123.4 format
char *ftostr31ns(const float &x)
{
  int xx = x * 10;
  //conv[0]=(xx>=0)?'+':'-';
  xx = abs(xx);
  conv[0] = (xx / 1000) % 10 + '0';
  conv[1] = (xx / 100) % 10 + '0';
  conv[2] = (xx / 10) % 10 + '0';
  conv[3] = '.';
  conv[4] = (xx) % 10 + '0';
  conv[5] = 0;
  return conv;
}

char *ftostr32(const float &x)
{
  long xx = x * 100;
  if (xx >= 0)
    conv[0] = (xx / 10000) % 10 + '0';
  else
    conv[0] = '-';
  xx = abs(xx);
  conv[1] = (xx / 1000) % 10 + '0';
  conv[2] = (xx / 100) % 10 + '0';
  conv[3] = '.';
  conv[4] = (xx / 10) % 10 + '0';
  conv[5] = (xx) % 10 + '0';
  conv[6] = 0;
  return conv;
}

//// Convert float to rj string with 123.45 format
char *ftostr32ns(const float &x) {
	long xx = abs(x);
	conv[0] = xx >= 10000 ? (xx / 10000) % 10 + '0' : ' ';
	conv[1] = xx >= 1000 ? (xx / 1000) % 10 + '0' : ' ';
	conv[2] = xx >= 100 ? (xx / 100) % 10 + '0' : '0';
	conv[3] = '.';
	conv[4] = (xx / 10) % 10 + '0';
	conv[5] = xx % 10 + '0';
	return conv;
}


// Convert float to string with 1.234 format
char *ftostr43(const float &x, uint8_t offset)
{
  const size_t maxOffset = sizeof(conv)/sizeof(conv[0]) - 6;
  if (offset>maxOffset) offset = maxOffset;
  long xx = x * 1000;
  if (xx >= 0)
    conv[offset] = (xx / 1000) % 10 + '0';
  else
    conv[offset] = '-';
  xx = abs(xx);
  conv[offset + 1] = '.';
  conv[offset + 2] = (xx / 100) % 10 + '0';
  conv[offset + 3] = (xx / 10) % 10 + '0';
  conv[offset + 4] = (xx) % 10 + '0';
  conv[offset + 5] = 0;
  return conv;
}

//Float to string with 1.23 format
char *ftostr12ns(const float &x)
{
  long xx = x * 100;

  xx = abs(xx);
  conv[0] = (xx / 100) % 10 + '0';
  conv[1] = '.';
  conv[2] = (xx / 10) % 10 + '0';
  conv[3] = (xx) % 10 + '0';
  conv[4] = 0;
  return conv;
}

//Float to string with 1.234 format
char *ftostr13ns(const float &x)
{
    long xx = x * 1000;
    if (xx >= 0)
        conv[0] = ' ';
    else
        conv[0] = '-';
    xx = abs(xx);
    conv[1] = (xx / 1000) % 10 + '0';
    conv[2] = '.';
    conv[3] = (xx / 100) % 10 + '0';
    conv[4] = (xx / 10) % 10 + '0';
    conv[5] = (xx) % 10 + '0';
    conv[6] = 0;
    return conv;
}

//  convert float to space-padded string with -_23.4_ format
char *ftostr32sp(const float &x) {
  long xx = abs(x * 100);
  uint8_t dig;

  if (x < 0) { // negative val = -_0
    conv[0] = '-';
    dig = (xx / 1000) % 10;
    conv[1] = dig ? '0' + dig : ' ';
  }
  else { // positive val = __0
    dig = (xx / 10000) % 10;
    if (dig) {
      conv[0] = '0' + dig;
      conv[1] = '0' + (xx / 1000) % 10;
    }
    else {
      conv[0] = ' ';
      dig = (xx / 1000) % 10;
      conv[1] = dig ? '0' + dig : ' ';
    }
  }

  conv[2] = '0' + (xx / 100) % 10; // lsd always

  dig = xx % 10;
  if (dig) { // 2 decimal places
    conv[5] = '0' + dig;
    conv[4] = '0' + (xx / 10) % 10;
    conv[3] = '.';
  }
  else { // 1 or 0 decimal place
    dig = (xx / 10) % 10;
    if (dig) {
      conv[4] = '0' + dig;
      conv[3] = '.';
    }
    else {
      conv[3] = conv[4] = ' ';
    }
    conv[5] = ' ';
  }
  conv[6] = '\0';
  return conv;
}

char *itostr31(const int &xx)
{
  conv[0] = (xx >= 0) ? '+' : '-';
  conv[1] = (xx / 1000) % 10 + '0';
  conv[2] = (xx / 100) % 10 + '0';
  conv[3] = (xx / 10) % 10 + '0';
  conv[4] = '.';
  conv[5] = (xx) % 10 + '0';
  conv[6] = 0;
  return conv;
}

// Convert int to rj string with 123 or -12 format
char *itostr3(const int &x)
{
  int xx = x;
  if (xx < 0) {
    conv[0] = '-';
    xx = -xx;
  } else if (xx >= 100)
    conv[0] = (xx / 100) % 10 + '0';
  else
    conv[0] = ' ';
  if (xx >= 10)
    conv[1] = (xx / 10) % 10 + '0';
  else
    conv[1] = ' ';
  conv[2] = (xx) % 10 + '0';
  conv[3] = 0;
  return conv;
}

// Convert int to lj string with 123 format
char *itostr3left(const int &xx)
{
  if (xx >= 100)
  {
    conv[0] = (xx / 100) % 10 + '0';
    conv[1] = (xx / 10) % 10 + '0';
    conv[2] = (xx) % 10 + '0';
    conv[3] = 0;
  }
  else if (xx >= 10)
  {
    conv[0] = (xx / 10) % 10 + '0';
    conv[1] = (xx) % 10 + '0';
    conv[2] = 0;
  }
  else
  {
    conv[0] = (xx) % 10 + '0';
    conv[1] = 0;
  }
  return conv;
}

// Convert int to rj string with 1234 format
char *itostr4(const int &xx) {
  conv[0] = xx >= 1000 ? (xx / 1000) % 10 + '0' : ' ';
  conv[1] = xx >= 100 ? (xx / 100) % 10 + '0' : ' ';
  conv[2] = xx >= 10 ? (xx / 10) % 10 + '0' : ' ';
  conv[3] = xx % 10 + '0';
  conv[4] = 0;
  return conv;
}

// Convert float to rj string with 12345 format
char *ftostr5(const float &x) {
  long xx = abs(x);
  conv[0] = xx >= 10000 ? (xx / 10000) % 10 + '0' : ' ';
  conv[1] = xx >= 1000 ? (xx / 1000) % 10 + '0' : ' ';
  conv[2] = xx >= 100 ? (xx / 100) % 10 + '0' : ' ';
  conv[3] = xx >= 10 ? (xx / 10) % 10 + '0' : ' ';
  conv[4] = xx % 10 + '0';
  conv[5] = 0;
  return conv;
}

// Convert float to string with +1234.5 format
char *ftostr51(const float &x)
{
  long xx = x * 10;
  conv[0] = (xx >= 0) ? '+' : '-';
  xx = abs(xx);
  conv[1] = (xx / 10000) % 10 + '0';
  conv[2] = (xx / 1000) % 10 + '0';
  conv[3] = (xx / 100) % 10 + '0';
  conv[4] = (xx / 10) % 10 + '0';
  conv[5] = '.';
  conv[6] = (xx) % 10 + '0';
  conv[7] = 0;
  return conv;
}

// Convert float to string with +123.45 format
char *ftostr52(const float &x)
{
  long xx = x * 100;
  conv[0] = (xx >= 0) ? '+' : '-';
  xx = abs(xx);
  conv[1] = (xx / 10000) % 10 + '0';
  conv[2] = (xx / 1000) % 10 + '0';
  conv[3] = (xx / 100) % 10 + '0';
  conv[4] = '.';
  conv[5] = (xx / 10) % 10 + '0';
  conv[6] = (xx) % 10 + '0';
  conv[7] = 0;
  return conv;
}


