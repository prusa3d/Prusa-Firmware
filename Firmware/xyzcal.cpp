//xyzcal.cpp - xyz calibration with image processing

#include "Configuration_prusa.h"
#ifdef NEW_XYZCAL

#include "xyzcal.h"
#include <avr/wdt.h>
#include "stepper.h"
#include "temperature.h"
#include "sm4.h"


#define XYZCAL_PINDA_HYST_MIN 20  //50um
#define XYZCAL_PINDA_HYST_MAX 100 //250um
#define XYZCAL_PINDA_HYST_DIF 5   //12.5um

#define ENABLE_FANCHECK_INTERRUPT()  EIMSK |= (1<<7)
#define DISABLE_FANCHECK_INTERRUPT() EIMSK &= ~(1<<7)

#define _PINDA ((READ(Z_MIN_PIN) != Z_MIN_ENDSTOP_INVERTING)?1:0)

#define DBG(args...) printf_P(args)
//#define DBG(args...)
#ifndef _n
#define _n PSTR
#endif //_n

#define _X ((int16_t)count_position[X_AXIS])
#define _Y ((int16_t)count_position[Y_AXIS])
#define _Z ((int16_t)count_position[Z_AXIS])
#define _E ((int16_t)count_position[E_AXIS])

#define _PI 3.14159265F

uint8_t check_pinda_0();
uint8_t check_pinda_1();
void xyzcal_update_pos(uint16_t dx, uint16_t dy, uint16_t dz, uint16_t de);
uint16_t xyzcal_calc_delay(uint16_t nd, uint16_t dd);


void xyzcal_meassure_enter(void)
{
	DBG(_n("xyzcal_meassure_enter\n"));
	disable_heater();
	DISABLE_TEMPERATURE_INTERRUPT();
#if (defined(FANCHECK) && defined(TACH_1) && (TACH_1 >-1))
	DISABLE_FANCHECK_INTERRUPT();
#endif //(defined(FANCHECK) && defined(TACH_1) && (TACH_1 >-1))
	DISABLE_STEPPER_DRIVER_INTERRUPT();
#ifdef WATCHDOG
	wdt_disable();
#endif //WATCHDOG
	sm4_stop_cb = 0;
	sm4_update_pos_cb = xyzcal_update_pos;
	sm4_calc_delay_cb = xyzcal_calc_delay;
}

void xyzcal_meassure_leave(void)
{
	DBG(_n("xyzcal_meassure_leave\n"));
    planner_abort_hard();
	ENABLE_TEMPERATURE_INTERRUPT();
#if (defined(FANCHECK) && defined(TACH_1) && (TACH_1 >-1))
	ENABLE_FANCHECK_INTERRUPT();
#endif //(defined(FANCHECK) && defined(TACH_1) && (TACH_1 >-1))
	ENABLE_STEPPER_DRIVER_INTERRUPT();
#ifdef WATCHDOG
	wdt_enable(WDTO_4S);
#endif //WATCHDOG
	sm4_stop_cb = 0;
	sm4_update_pos_cb = 0;
	sm4_calc_delay_cb = 0;
}


uint8_t check_pinda_0()
{
	return _PINDA?0:1;
}

uint8_t check_pinda_1()
{
	return _PINDA?1:0;
}

uint8_t xyzcal_dm = 0;

void xyzcal_update_pos(uint16_t dx, uint16_t dy, uint16_t dz, uint16_t)
{
//	DBG(_n("xyzcal_update_pos dx=%d dy=%d dz=%d dir=%02x\n"), dx, dy, dz, xyzcal_dm);
	if (xyzcal_dm&1) count_position[0] -= dx; else count_position[0] += dx;
	if (xyzcal_dm&2) count_position[1] -= dy; else count_position[1] += dy;
	if (xyzcal_dm&4) count_position[2] -= dz; else count_position[2] += dz;
//	DBG(_n(" after xyzcal_update_pos x=%ld y=%ld z=%ld\n"), count_position[0], count_position[1], count_position[2]);
}

uint16_t xyzcal_sm4_delay = 0;

//#define SM4_ACCEL_TEST
#ifdef SM4_ACCEL_TEST
uint16_t xyzcal_sm4_v0 = 2000;
uint16_t xyzcal_sm4_vm = 45000;
uint16_t xyzcal_sm4_v = xyzcal_sm4_v0;
uint16_t xyzcal_sm4_ac = 2000;
uint16_t xyzcal_sm4_ac2 = (uint32_t)xyzcal_sm4_ac * 1024 / 10000;
//float xyzcal_sm4_vm = 10000;
#endif //SM4_ACCEL_TEST

#ifdef SM4_ACCEL_TEST
uint16_t xyzcal_calc_delay(uint16_t nd, uint16_t dd)
{
	uint16_t del_us = 0;
	if (xyzcal_sm4_v & 0xf000) //>=4096
	{
		del_us = (uint16_t)62500 / (uint16_t)(xyzcal_sm4_v >> 4);
		xyzcal_sm4_v += (xyzcal_sm4_ac2 * del_us + 512) >> 10;
		if (xyzcal_sm4_v > xyzcal_sm4_vm) xyzcal_sm4_v = xyzcal_sm4_vm;
		if (del_us > 25) return del_us - 25;
	}
	else
	{
		del_us = (uint32_t)1000000 / xyzcal_sm4_v;
		xyzcal_sm4_v += ((uint32_t)xyzcal_sm4_ac2 * del_us + 512) >> 10;
		if (xyzcal_sm4_v > xyzcal_sm4_vm) xyzcal_sm4_v = xyzcal_sm4_vm;
		if (del_us > 50) return del_us - 50;
	}

//	uint16_t del_us = (uint16_t)(((float)1000000 / xyzcal_sm4_v) + 0.5);		
//	uint16_t del_us = (uint32_t)1000000 / xyzcal_sm4_v;		
//	uint16_t del_us = 100;		
//	uint16_t del_us = (uint16_t)10000 / xyzcal_sm4_v;
//	v += (ac * del_us + 500) / 1000;
//	xyzcal_sm4_v += (xyzcal_sm4_ac * del_us) / 1000;
//	return xyzcal_sm4_delay;
//	DBG(_n("xyzcal_calc_delay nd=%d dd=%d v=%d  del_us=%d\n"), nd, dd, xyzcal_sm4_v, del_us);
	return 0;
}
#else //SM4_ACCEL_TEST
uint16_t xyzcal_calc_delay(uint16_t, uint16_t)
{
    return xyzcal_sm4_delay;
}
#endif //SM4_ACCEL_TEST

bool xyzcal_lineXYZ_to(int16_t x, int16_t y, int16_t z, uint16_t delay_us, int8_t check_pinda)
{
//	DBG(_n("xyzcal_lineXYZ_to x=%d y=%d z=%d  check=%d\n"), x, y, z, check_pinda);
	x -= (int16_t)count_position[0];
	y -= (int16_t)count_position[1];
	z -= (int16_t)count_position[2];
	xyzcal_dm = ((x<0)?1:0) | ((y<0)?2:0) | ((z<0)?4:0);
	sm4_set_dir_bits(xyzcal_dm);
	sm4_stop_cb = check_pinda?((check_pinda<0)?check_pinda_0:check_pinda_1):0;
	xyzcal_sm4_delay = delay_us;
//	uint32_t u = micros();
	bool ret = sm4_line_xyze_ui(abs(x), abs(y), abs(z), 0)?true:false;
//	u = micros() - u;
	return ret;
}

bool xyzcal_spiral2(int16_t cx, int16_t cy, int16_t z0, int16_t dz, int16_t radius, int16_t rotation, uint16_t delay_us, int8_t check_pinda, uint16_t* pad)
{
	bool ret = false;
	float r = 0; //radius
	uint8_t n = 0; //point number
	uint16_t ad = 0; //angle [deg]
	float ar; //angle [rad]
	uint8_t dad = 0; //delta angle [deg]
	uint8_t dad_min = 4; //delta angle min [deg]
	uint8_t dad_max = 16; //delta angle max [deg]
	uint8_t k = 720 / (dad_max - dad_min); //delta calculation constant
	ad = 0;
	if (pad) ad = *pad % 720;
	DBG(_n("xyzcal_spiral2 cx=%d cy=%d z0=%d dz=%d radius=%d ad=%d\n"), cx, cy, z0, dz, radius, ad);
	for (; ad < 720; ad++)
	{
		if (radius > 0)
		{
			dad = dad_max - (ad / k);
			r = (float)(((uint32_t)ad) * radius) / 720;
		}
		else
		{
			dad = dad_max - ((719 - ad) / k);
			r = (float)(((uint32_t)(719 - ad)) * (-radius)) / 720;
		}
		ar = (ad + rotation)* (float)_PI / 180;
		float _cos = cos(ar);
		float _sin = sin(ar);
		int x = (int)(cx + (_cos * r));
		int y = (int)(cy + (_sin * r));
		int z = (int)(z0 - ((float)((int32_t)dz * ad) / 720));
		if (xyzcal_lineXYZ_to(x, y, z, delay_us, check_pinda))
		{
			ad += dad + 1;
			ret = true;
			break;
		}
		n++;
		ad += dad;
	}
	if (pad) *pad = ad;
	return ret;
}

bool xyzcal_spiral8(int16_t cx, int16_t cy, int16_t z0, int16_t dz, int16_t radius, uint16_t delay_us, int8_t check_pinda, uint16_t* pad)
{
	bool ret = false;
	uint16_t ad = 0;
	if (pad) ad = *pad;
	DBG(_n("xyzcal_spiral8 cx=%d cy=%d z0=%d dz=%d radius=%d ad=%d\n"), cx, cy, z0, dz, radius, ad);
	if (!ret && (ad < 720))
		if ((ret = xyzcal_spiral2(cx, cy, z0 - 0*dz, dz, radius, 0, delay_us, check_pinda, &ad)) != 0)
			ad += 0;
	if (!ret && (ad < 1440))
		if ((ret = xyzcal_spiral2(cx, cy, z0 - 1*dz, dz, -radius, 0, delay_us, check_pinda, &ad)) != 0)
			ad += 720;
	if (!ret && (ad < 2160))
		if ((ret = xyzcal_spiral2(cx, cy, z0 - 2*dz, dz, radius, 180, delay_us, check_pinda, &ad)) != 0)
			ad += 1440;
	if (!ret && (ad < 2880))
		if ((ret = xyzcal_spiral2(cx, cy, z0 - 3*dz, dz, -radius, 180, delay_us, check_pinda, &ad)) != 0)
			ad += 2160;
	if (pad) *pad = ad;
	return ret;
}

#ifdef XYZCAL_MEASSURE_PINDA_HYSTEREZIS
int8_t xyzcal_meassure_pinda_hysterezis(int16_t min_z, int16_t max_z, uint16_t delay_us, uint8_t samples)
{
	DBG(_n("xyzcal_meassure_pinda_hysterezis\n"));
	int8_t ret = -1; // PINDA signal error
	int16_t z = _Z;
	int16_t sum_up = 0;
	int16_t sum_dn = 0;
	int16_t up;
	int16_t dn;
	uint8_t sample;
	xyzcal_lineXYZ_to(_X, _Y, min_z, delay_us, 1);
	xyzcal_lineXYZ_to(_X, _Y, max_z, delay_us, -1);
	if (!_PINDA)
	{
		for (sample = 0; sample < samples; sample++)
		{
			dn = _Z;
			if (!xyzcal_lineXYZ_to(_X, _Y, min_z, delay_us, 1)) break;
			dn = dn - _Z;
			up = _Z;
			if (!xyzcal_lineXYZ_to(_X, _Y, max_z, delay_us, -1)) break;
			up = _Z - up;
			DBG(_n("%d. up=%d dn=%d\n"), sample, up, dn);
			sum_up += up;
			sum_dn += dn;
			if (abs(up - dn) > XYZCAL_PINDA_HYST_DIF)
			{
				ret = -2; // difference between up-dn to high
				break;
			}
		}
		if (sample == samples)
		{
			up = sum_up / samples;
			dn = sum_dn / samples;
			uint16_t hyst = (up + dn) / 2;
			if (abs(up - dn) > XYZCAL_PINDA_HYST_DIF)
				ret = -2; // difference between up-dn to high
			else if ((hyst < XYZCAL_PINDA_HYST_MIN) || (hyst > XYZCAL_PINDA_HYST_MAX))
				ret = -3; // hysterezis out of range
			else
				ret = hyst;
		}
	}
	xyzcal_lineXYZ_to(_X, _Y, z, delay_us, 0);
	return ret;
}
#endif //XYZCAL_MEASSURE_PINDA_HYSTEREZIS


void xyzcal_scan_pixels_32x32(int16_t cx, int16_t cy, int16_t min_z, int16_t max_z, uint16_t delay_us, uint8_t* pixels)
{
	DBG(_n("xyzcal_scan_pixels_32x32 cx=%d cy=%d min_z=%d max_z=%d\n"), cx, cy, min_z, max_z);
//	xyzcal_lineXYZ_to(cx - 1024, cy - 1024, max_z, 2*delay_us, 0);
//	xyzcal_lineXYZ_to(cx, cy, max_z, delay_us, 0);
	int16_t z = (int16_t)count_position[2];
	xyzcal_lineXYZ_to(cx, cy, z, 2*delay_us, 0);
	for (uint8_t r = 0; r < 32; r++)
	{
//		int8_t _pinda = _PINDA;
		xyzcal_lineXYZ_to((r&1)?(cx+1024):(cx-1024), cy - 1024 + r*64, z, 2*delay_us, 0);
		xyzcal_lineXYZ_to(_X, _Y, min_z, delay_us, 1);
		xyzcal_lineXYZ_to(_X, _Y, max_z, delay_us, -1);
		z = (int16_t)count_position[2];
		sm4_set_dir(X_AXIS, (r&1)?1:0);
		for (uint8_t c = 0; c < 32; c++)
		{
			uint16_t sum = 0;
			int16_t z_sum = 0;
			for (uint8_t i = 0; i < 64; i++)
			{
				int8_t pinda = _PINDA;
				int16_t pix = z - min_z;
				pix += (pinda)?23:-24;
				if (pix < 0) pix = 0;
				if (pix > 255) pix = 255;
				sum += pix;
				z_sum += z;
//				if (_pinda != pinda)
//				{
//					if (pinda)
//						DBG(_n("!1 x=%d z=%d\n"), c*64+i, z+23);
//					else
//						DBG(_n("!0 x=%d z=%d\n"), c*64+i, z-24);
//				}
				sm4_set_dir(Z_AXIS, !pinda);
				if (!pinda)
				{
					if (z > min_z)
					{
						sm4_do_step(Z_AXIS_MASK);
						z--;
					}
				}
				else
				{
					if (z < max_z)
					{
						sm4_do_step(Z_AXIS_MASK);
						z++;
					}
				}
				sm4_do_step(X_AXIS_MASK);
				delayMicroseconds(600);
//				_pinda = pinda;
			}
			sum >>= 6; //div 64
			if (z_sum < 0)
			{
				z_sum = -z_sum;
				z_sum >>= 6; //div 64
				z_sum = -z_sum;
			}
			else
				z_sum >>= 6; //div 64
			if (pixels) pixels[((uint16_t)r<<5) + ((r&1)?(31-c):c)] = sum;
//			DBG(_n("c=%d r=%d l=%d z=%d\n"), c, r, sum, z_sum);
			count_position[0] += (r&1)?-64:64;
			count_position[2] = z;
		}
		if (pixels)
			for (uint8_t c = 0; c < 32; c++)
				DBG(_n("%02x"), pixels[((uint16_t)r<<5) + c]);
		DBG(_n("\n"));
	}
//	xyzcal_lineXYZ_to(cx, cy, z, 2*delay_us, 0);
}

void xyzcal_histo_pixels_32x32(uint8_t* pixels, uint16_t* histo)
{
	for (uint8_t l = 0; l < 16; l++)
		histo[l] = 0;
	for (uint8_t r = 0; r < 32; r++)
		for (uint8_t c = 0; c < 32; c++)
		{
			uint8_t pix = pixels[((uint16_t)r<<5) + c];
			histo[pix >> 4]++;
		}
	for (uint8_t l = 0; l < 16; l++)
		DBG(_n(" %2d %d\n"), l, histo[l]);
}

void xyzcal_adjust_pixels(uint8_t* pixels, uint16_t* histo)
{
	uint8_t l;
	uint16_t max_c = histo[1];
	uint8_t max_l = 1;
	for (l = 1; l < 16; l++)
	{
		uint16_t c = histo[l];
		if (c > max_c)
		{
			max_c = c;
			max_l = l;
		}
	}
	DBG(_n("max_c=%2d max_l=%d\n"), max_c, max_l);
	for (l = 14; l > 8; l--)
		if (histo[l] >= 10)
			break;
	uint8_t pix_min = 0;
	uint8_t pix_max = l << 4;
	if (histo[0] < (32*32 - 144))
	{
		pix_min = (max_l << 4) / 2;
	}
	uint8_t pix_dif = pix_max - pix_min;
	DBG(_n(" min=%d max=%d dif=%d\n"), pix_min, pix_max, pix_dif);
	for (int16_t i = 0; i < 32*32; i++)
	{
		uint16_t pix = pixels[i];
		if (pix > pix_min) pix -= pix_min;
		else pix = 0;
		pix <<= 8;
		pix /= pix_dif;
//		if (pix < 0) pix = 0;
		if (pix > 255) pix = 255;
		pixels[i] = (uint8_t)pix;
	}
	for (uint8_t r = 0; r < 32; r++)
	{
		for (uint8_t c = 0; c < 32; c++)
			DBG(_n("%02x"), pixels[((uint16_t)r<<5) + c]);
		DBG(_n("\n"));
	}
}

/*
void xyzcal_draw_pattern_12x12_in_32x32(uint8_t* pattern, uint32_t* pixels, int w, int h, uint8_t x, uint8_t y, uint32_t and, uint32_t or)
{
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			int idx = (x + j) + w * (y + i);
			if (pattern[i] & (1 << j))
			{
				pixels[idx] &= and;
				pixels[idx] |= or;
			}
		}
}
*/

int16_t xyzcal_match_pattern_12x12_in_32x32(uint16_t* pattern, uint8_t* pixels, uint8_t c, uint8_t r)
{
	uint8_t thr = 16;
	int16_t match = 0;
	for (uint8_t i = 0; i < 12; i++)
		for (uint8_t j = 0; j < 12; j++)
		{
			if (((i == 0) || (i == 11)) && ((j < 2) || (j >= 10))) continue; //skip corners
			if (((j == 0) || (j == 11)) && ((i < 2) || (i >= 10))) continue;
			uint16_t idx = (c + j) + 32 * (r + i);
			uint8_t val = pixels[idx];
			if (pattern[i] & (1 << j))
			{
				if (val > thr) match ++;
				else match --;
			}
			else
			{
				if (val <= thr) match ++;
				else match --;
			}
		}
	return match;
}

int16_t xyzcal_find_pattern_12x12_in_32x32(uint8_t* pixels, uint16_t* pattern, uint8_t* pc, uint8_t* pr)
{
	uint8_t max_c = 0;
	uint8_t max_r = 0;
	int16_t max_match = 0;
	for (uint8_t r = 0; r < (32 - 12); r++)
		for (uint8_t c = 0; c < (32 - 12); c++)
		{
			int16_t match = xyzcal_match_pattern_12x12_in_32x32(pattern, pixels, c, r);
			if (max_match < match)
			{
				max_c = c;
				max_r = r;
				max_match = match;
			}
		}
	DBG(_n("max_c=%d max_r=%d max_match=%d\n"), max_c, max_r, max_match);
	if (pc) *pc = max_c;
	if (pr) *pr = max_r;
	return max_match;
}

#define MAX_DIAMETR 600
#define XYZCAL_FIND_CENTER_DIAGONAL

int8_t xyzcal_find_point_center2(uint16_t delay_us)
{
	printf_P(PSTR("xyzcal_find_point_center2\n"));
	int16_t x0 = _X;
	int16_t y0 = _Y;
	int16_t z0 = _Z;
	printf_P(PSTR(" x0=%d\n"), x0);
	printf_P(PSTR(" y0=%d\n"), y0);
	printf_P(PSTR(" z0=%d\n"), z0);

	xyzcal_lineXYZ_to(_X, _Y, z0 + 400, 500, -1);
	xyzcal_lineXYZ_to(_X, _Y, z0 - 400, 500, 1);
	xyzcal_lineXYZ_to(_X, _Y, z0 + 400, 500, -1);
	xyzcal_lineXYZ_to(_X, _Y, z0 - 400, 500, 1);

	z0 = _Z - 20;
	xyzcal_lineXYZ_to(_X, _Y, z0, 500, 0);

//	xyzcal_lineXYZ_to(x0, y0, z0 - 100, 500, 1);
//	z0 = _Z;
//	printf_P(PSTR("  z0=%d\n"), z0);
//	xyzcal_lineXYZ_to(x0, y0, z0 + 100, 500, -1);
//	z0 += _Z;
//	z0 /= 2;
	printf_P(PSTR("   z0=%d\n"), z0);
//	xyzcal_lineXYZ_to(x0, y0, z0 - 100, 500, 1);
//	z0 = _Z - 10;

	int8_t ret = 1;

#ifdef XYZCAL_FIND_CENTER_DIAGONAL
	int32_t xc = 0;
	int32_t yc = 0;
	int16_t ad = 45;
	for (; ad < 360; ad += 90)
	{
		float ar = (float)ad * _PI / 180;
		int16_t x = x0 + MAX_DIAMETR * cos(ar);
		int16_t y = y0 + MAX_DIAMETR * sin(ar);
		if (!xyzcal_lineXYZ_to(x, y, z0, delay_us, -1))
		{
			printf_P(PSTR("ERROR ad=%d\n"), ad);
			ret = 0;
			break;
		}
		xc += _X;
		yc += _Y;
		xyzcal_lineXYZ_to(x0, y0, z0, delay_us, 0);
	}
	if (ret)
	{
		printf_P(PSTR("OK\n"), ad);
		x0 = xc / 4;
		y0 = yc / 4;
		printf_P(PSTR(" x0=%d\n"), x0);
		printf_P(PSTR(" y0=%d\n"), y0);
	}

#else //XYZCAL_FIND_CENTER_DIAGONAL
	xyzcal_lineXYZ_to(x0 - MAX_DIAMETR, y0, z0, delay_us, -1);
	int16_t dx1 = x0 - _X;
	if (dx1 >= MAX_DIAMETR)
	{
		printf_P(PSTR("!!! dx1 = %d\n"), dx1);
		return 0;
	}
	xyzcal_lineXYZ_to(x0, y0, z0, delay_us, 0);
	xyzcal_lineXYZ_to(x0 + MAX_DIAMETR, y0, z0, delay_us, -1);
	int16_t dx2 = _X - x0;
	if (dx2 >= MAX_DIAMETR)
	{
		printf_P(PSTR("!!! dx2 = %d\n"), dx2);
		return 0;
	}
	xyzcal_lineXYZ_to(x0, y0, z0, delay_us, 0);
	xyzcal_lineXYZ_to(x0 , y0 - MAX_DIAMETR, z0, delay_us, -1);
	int16_t dy1 = y0 - _Y;
	if (dy1 >= MAX_DIAMETR)
	{
		printf_P(PSTR("!!! dy1 = %d\n"), dy1);
		return 0;
	}
	xyzcal_lineXYZ_to(x0, y0, z0, delay_us, 0);
	xyzcal_lineXYZ_to(x0, y0 + MAX_DIAMETR, z0, delay_us, -1);
	int16_t dy2 = _Y - y0;
	if (dy2 >= MAX_DIAMETR)
	{
		printf_P(PSTR("!!! dy2 = %d\n"), dy2);
		return 0;
	}
	printf_P(PSTR("dx1=%d\n"), dx1);
	printf_P(PSTR("dx2=%d\n"), dx2);
	printf_P(PSTR("dy1=%d\n"), dy1);
	printf_P(PSTR("dy2=%d\n"), dy2);

	x0 += (dx2 - dx1) / 2;
	y0 += (dy2 - dy1) / 2;

	printf_P(PSTR(" x0=%d\n"), x0);
	printf_P(PSTR(" y0=%d\n"), y0);

#endif //XYZCAL_FIND_CENTER_DIAGONAL

	xyzcal_lineXYZ_to(x0, y0, z0, delay_us, 0);

	return ret;
}

#ifdef XYZCAL_FIND_POINT_CENTER
int8_t xyzcal_find_point_center(int16_t x0, int16_t y0, int16_t z0, int16_t min_z, int16_t max_z, uint16_t delay_us, uint8_t turns)
{
	uint8_t n;
	uint16_t ad;
	float ar;
	float _cos;
	float _sin;
	int16_t r_min = 0;
	int16_t r_max = 0;
	int16_t x_min = 0;
	int16_t x_max = 0;
	int16_t y_min = 0;
	int16_t y_max = 0;
	int16_t r = 10;
	int16_t x = x0;
	int16_t y = y0;
	int16_t z = z0;
	int8_t _pinda = _PINDA;
	for (n = 0; n < turns; n++)
	{
		uint32_t r_sum = 0;
		for (ad = 0; ad < 720; ad++)
		{
			ar = ad * _PI / 360;
			_cos = cos(ar);
			_sin = sin(ar);
			x = x0 + (int)(_cos * r);
			y = y0 + (int)(_sin * r);
			xyzcal_lineXYZ_to(x, y, z, 1000, 0);
			int8_t pinda = _PINDA;
			if (pinda)
				r += 1;
			else
			{
				r -= 1;
				ad--;
				r_sum -= r;
			}
			if (ad == 0)
			{
				x_min = x0;
				x_max = x0;
				y_min = y0;
				y_max = y0;
				r_min = r;
				r_max = r;
			}
			else if (pinda)
			{
				if (x_min > x) x_min = (2*x + x_min) / 3;
				if (x_max < x) x_max = (2*x + x_max) / 3;
				if (y_min > y) y_min = (2*y + y_min) / 3;
				if (y_max < y) y_max = (2*y + y_max) / 3;
/*				if (x_min > x) x_min = x;
				if (x_max < x) x_max = x;
				if (y_min > y) y_min = y;
				if (y_max < y) y_max = y;*/
				if (r_min > r) r_min = r;
				if (r_max < r) r_max = r;
			}
			r_sum += r;
/*			if (_pinda != pinda)
			{
				if (pinda)
					DBG(_n("!1 x=%d y=%d\n"), x, y);
				else
					DBG(_n("!0 x=%d y=%d\n"), x, y);
			}*/
			_pinda = pinda;
//			DBG(_n("x=%d y=%d rx=%d ry=%d\n"), x, y, rx, ry);
		}
		DBG(_n("x_min=%d x_max=%d y_min=%d y_max=%d r_min=%d r_max=%d r_avg=%d\n"), x_min, x_max, y_min, y_max, r_min, r_max, r_sum / 720);
		if ((n > 2) && (n & 1))
		{
			x0 += (x_min + x_max);
			y0 += (y_min + y_max);
			x0 /= 3;
			y0 /= 3;
			int rx = (x_max - x_min) / 2;
			int ry = (y_max - y_min) / 2;
			r = (rx + ry) / 3;//(rx < ry)?rx:ry;
			DBG(_n("x0=%d y0=%d r=%d\n"), x0, y0, r);
		}
	}
	xyzcal_lineXYZ_to(x0, y0, z, 200, 0);
}
#endif //XYZCAL_FIND_POINT_CENTER


uint8_t xyzcal_xycoords2point(int16_t x, int16_t y)
{
	uint8_t ix = (x > 10000)?1:0;
	uint8_t iy = (y > 10000)?1:0;
	return iy?(3-ix):ix;
}

//MK3
#if ((MOTHERBOARD == BOARD_EINSY_1_0a))
const int16_t xyzcal_point_xcoords[4] PROGMEM = {1200, 22000, 22000, 1200};
const int16_t xyzcal_point_ycoords[4] PROGMEM = {600, 600, 19800, 19800};
#endif //((MOTHERBOARD == BOARD_EINSY_1_0a))

//MK2.5
#if ((MOTHERBOARD == BOARD_RAMBO_MINI_1_0) || (MOTHERBOARD == BOARD_RAMBO_MINI_1_3))
const int16_t xyzcal_point_xcoords[4] PROGMEM = {1200, 22000, 22000, 1200};
const int16_t xyzcal_point_ycoords[4] PROGMEM = {700, 700, 19800, 19800};
#endif //((MOTHERBOARD == BOARD_RAMBO_MINI_1_0) || (MOTHERBOARD == BOARD_RAMBO_MINI_1_3))

const uint16_t xyzcal_point_pattern[12] PROGMEM = {0x000, 0x0f0, 0x1f8, 0x3fc, 0x7fe, 0x7fe, 0x7fe, 0x7fe, 0x3fc, 0x1f8, 0x0f0, 0x000};

bool xyzcal_searchZ(void)
{
	DBG(_n("xyzcal_searchZ x=%ld y=%ld z=%ld\n"), count_position[X_AXIS], count_position[Y_AXIS], count_position[Z_AXIS]);
	int16_t x0 = _X;
	int16_t y0 = _Y;
	int16_t z0 = _Z;
//	int16_t min_z = -6000;
//	int16_t dz = 100;
	int16_t z = z0;
	while (z > -2300) //-6mm + 0.25mm
	{
		uint16_t ad = 0;
		if (xyzcal_spiral8(x0, y0, z, 100, 900, 320, 1, &ad)) //dz=100 radius=900 delay=400
		{
			int16_t x_on = _X;
			int16_t y_on = _Y;
			int16_t z_on = _Z;
			DBG(_n(" ON-SIGNAL at x=%d y=%d z=%d ad=%d\n"), x_on, y_on, z_on, ad);
			return true;
		}
		z -= 400;
	}
	DBG(_n("xyzcal_searchZ no signal\n x=%ld y=%ld z=%ld\n"), count_position[X_AXIS], count_position[Y_AXIS], count_position[Z_AXIS]);
	return false;
}

bool xyzcal_scan_and_process(void)
{
	DBG(_n("sizeof(block_buffer)=%d\n"), sizeof(block_t)*BLOCK_BUFFER_SIZE);
//	DBG(_n("sizeof(pixels)=%d\n"), 32*32);
//	DBG(_n("sizeof(histo)=%d\n"), 2*16);
//	DBG(_n("sizeof(pattern)=%d\n"), 2*12);
	DBG(_n("sizeof(total)=%d\n"), 32*32+2*16+2*12);
	bool ret = false;
	int16_t x = _X;
	int16_t y = _Y;
	int16_t z = _Z;

	uint8_t* pixels = (uint8_t*)block_buffer;
	xyzcal_scan_pixels_32x32(x, y, z - 72, 2400, 200, pixels);

	uint16_t* histo = (uint16_t*)(pixels + 32*32);
	xyzcal_histo_pixels_32x32(pixels, histo);

	xyzcal_adjust_pixels(pixels, histo);

	uint16_t* pattern = (uint16_t*)(histo + 2*16);
	for (uint8_t i = 0; i < 12; i++)
	{
		pattern[i] = pgm_read_word((uint16_t*)(xyzcal_point_pattern + i));
//		DBG(_n(" pattern[%d]=%d\n"), i, pattern[i]);
	}
	uint8_t c = 0;
	uint8_t r = 0;
	if (xyzcal_find_pattern_12x12_in_32x32(pixels, pattern, &c, &r) > 66) //total pixels=144, corner=12 (1/2 = 66)
	{
		DBG(_n(" pattern found at %d %d\n"), c, r);
		c += 6;
		r += 6;
		x += ((int16_t)c - 16) << 6;
		y += ((int16_t)r - 16) << 6;
		DBG(_n(" x=%d y=%d z=%d\n"), x, y, z);
		xyzcal_lineXYZ_to(x, y, z, 200, 0);
		ret = true;
	}
	for (uint16_t i = 0; i < sizeof(block_t)*BLOCK_BUFFER_SIZE; i++)
		pixels[i] = 0;
	return ret;
}

bool xyzcal_find_bed_induction_sensor_point_xy(void)
{
	DBG(_n("xyzcal_find_bed_induction_sensor_point_xy x=%ld y=%ld z=%ld\n"), count_position[X_AXIS], count_position[Y_AXIS], count_position[Z_AXIS]);
	bool ret = false;
	st_synchronize();
	int16_t x = _X;
	int16_t y = _Y;
	int16_t z = _Z;
	uint8_t point = xyzcal_xycoords2point(x, y);
	x = pgm_read_word((uint16_t*)(xyzcal_point_xcoords + point));
	y = pgm_read_word((uint16_t*)(xyzcal_point_ycoords + point));
	DBG(_n("point=%d x=%d y=%d z=%d\n"), point, x, y, z);
	xyzcal_meassure_enter();
	xyzcal_lineXYZ_to(x, y, z, 200, 0);
	if (xyzcal_searchZ())
	{
		int16_t z = _Z;
		xyzcal_lineXYZ_to(x, y, z, 200, 0);
		if (xyzcal_scan_and_process())
		{
			if (xyzcal_find_point_center2(500))
			{
				uint32_t x_avg = 0;
				uint32_t y_avg = 0;
				uint8_t n; for (n = 0; n < 4; n++)
				{
					if (!xyzcal_find_point_center2(1000)) break;
					x_avg += _X;
					y_avg += _Y;	
				}
				if (n == 4)
				{
					xyzcal_lineXYZ_to(x_avg >> 2, y_avg >> 2, _Z, 200, 0);
					ret = true;
				}
			}
		}
	}
	xyzcal_meassure_leave();
	return ret;
}


#endif //NEW_XYZCAL
