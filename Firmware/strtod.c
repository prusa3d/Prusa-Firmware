// Note -- This is a modified stdtod() method, to prevent the catching of uppercase "E", used in 3D printing g-code.


#if !defined(__AVR_TINY__)

#include <avr/pgmspace.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>		/* INFINITY, NAN		*/
#include <stdlib.h>

/* Only GCC 4.2 calls the library function to convert an unsigned long
   to float.  Other GCC-es (including 4.3) use a signed long to float
   conversion along with a large inline code to correct the result.	*/
extern double __floatunsisf(unsigned long);

PROGMEM static const float pwr_p10[6] = {
    1e+1, 1e+2, 1e+4, 1e+8, 1e+16, 1e+32
};
PROGMEM static const float pwr_m10[6] = {
    1e-1, 1e-2, 1e-4, 1e-8, 1e-16, 1e-32
};

/* PSTR() is not used to save 1 byte per string: '\0' at the tail.	*/
PROGMEM static const char pstr_inf[] = { 'I','N','F' };
PROGMEM static const char pstr_inity[] = { 'I','N','I','T','Y' };
PROGMEM static const char pstr_nan[] = { 'N','A','N' };


double strtod_noE(const char* nptr, char** endptr)
{
	union {
		unsigned long u32;
		float flt;
	} x;
	unsigned char c;
	int exp;

	unsigned char flag;
#define FL_MINUS    0x01	/* number is negative	*/
#define FL_ANY	    0x02	/* any digit was readed	*/
#define FL_OVFL	    0x04	/* overflow was		*/
#define FL_DOT	    0x08	/* decimal '.' was	*/
#define FL_MEXP	    0x10	/* exponent 'e' is neg.	*/

	if (endptr)
		*endptr = (char*)nptr;

	do {
		c = *nptr++;
	} while (isspace(c));

	flag = 0;
	if (c == '-') {
		flag = FL_MINUS;
		c = *nptr++;
	}
	else if (c == '+') {
		c = *nptr++;
	}

	if (!strncasecmp_P(nptr - 1, pstr_inf, 3)) {
		nptr += 2;
		if (!strncasecmp_P(nptr, pstr_inity, 5))
			nptr += 5;
		if (endptr)
			*endptr = (char*)nptr;
		return flag & FL_MINUS ? -INFINITY : +INFINITY;
	}

	/* NAN() construction is not realised.
	   Length would be 3 characters only.	*/
	if (!strncasecmp_P(nptr - 1, pstr_nan, 3)) {
		if (endptr)
			*endptr = (char*)nptr + 2;
		return NAN;
	}

	x.u32 = 0;
	exp = 0;
	while (1) {

		c -= '0';

		if (c <= 9) {
			flag |= FL_ANY;
			if (flag & FL_OVFL) {
				if (!(flag & FL_DOT))
					exp += 1;
			}
			else {
				if (flag & FL_DOT)
					exp -= 1;
				/* x.u32 = x.u32 * 10 + c	*/
				x.u32 = (((x.u32 << 2) + x.u32) << 1) + c;
				if (x.u32 >= (ULONG_MAX - 9) / 10)
					flag |= FL_OVFL;
			}

		}
		else if (c == (('.' - '0') & 0xff) && !(flag & FL_DOT)) {
			flag |= FL_DOT;
		}
		else {
			break;
		}
		c = *nptr++;
	}

	// Check for exponent "E", but disable capital E
	if (c == (('e' - '0') & 0xff) /*|| c == (('E' - '0') & 0xff)*/)
	{
		int i;
		c = *nptr++;
		i = 2;
		if (c == '-') {
			flag |= FL_MEXP;
			c = *nptr++;
		}
		else if (c == '+') {
			c = *nptr++;
		}
		else {
			i = 1;
		}
		c -= '0';
		if (c > 9) {
			nptr -= i;
		}
		else {
			i = 0;
			do {
				if (i < 3200)
					i = (((i << 2) + i) << 1) + c;	/* i = 10*i + c	*/
				c = *nptr++ - '0';
			} while (c <= 9);
			if (flag & FL_MEXP)
				i = -i;
			exp += i;
		}
	}

	if ((flag & FL_ANY) && endptr)
		*endptr = (char*)nptr - 1;

	x.flt = __floatunsisf(x.u32);		/* manually	*/
	if ((flag & FL_MINUS) && (flag & FL_ANY))
		x.flt = -x.flt;

	if (x.flt != 0) {
		int pwr;
		if (exp < 0) {
			nptr = (void*)(pwr_m10 + 5);
			exp = -exp;
		}
		else {
			nptr = (void*)(pwr_p10 + 5);
		}
		for (pwr = 32; pwr; pwr >>= 1) {
			for (; exp >= pwr; exp -= pwr) {
				union {
					unsigned long u32;
					float flt;
				} y;
				y.u32 = pgm_read_dword((float*)nptr);
				x.flt *= y.flt;
			}
			nptr -= sizeof(float);
		}
		if (!isfinite(x.flt) || x.flt == 0)
			errno = ERANGE;
	}

	return x.flt;
}

#endif
