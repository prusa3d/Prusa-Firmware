
#include "xyzcal.h"
#include <avr/wdt.h>
#include "stepper.h"
#include "temperature.h"
#include "sm4.h"
//#include "tmc2130.h"


#define XYZCAL_PINDA_HYST_MIN 20  //50um
#define XYZCAL_PINDA_HYST_MAX 100 //250um
#define XYZCAL_PINDA_HYST_DIF 5   //12.5um

#define ENABLE_FANCHECK_INTERRUPT()  EIMSK |= (1<<7)
#define DISABLE_FANCHECK_INTERRUPT() EIMSK &= ~(1<<7)

#define _PINDA ((READ(Z_MIN_PIN) != Z_MIN_ENDSTOP_INVERTING)?1:0)

#define _PI 3.14159265F

extern long count_position[NUM_AXIS];

void xyzcal_meassure_enter(void)
{
	printf_P(PSTR("xyzcal_meassure_enter\n"));
	disable_heater();
	DISABLE_TEMPERATURE_INTERRUPT();
#if (defined(FANCHECK) && defined(TACH_1) && (TACH_1 >-1))
	DISABLE_FANCHECK_INTERRUPT();
#endif //(defined(FANCHECK) && defined(TACH_1) && (TACH_1 >-1))
	DISABLE_STEPPER_DRIVER_INTERRUPT();
#ifdef WATCHDOG
	wdt_disable();
#endif //WATCHDOG
}

void xyzcal_meassure_leave(void)
{
	printf_P(PSTR("xyzcal_meassure_leave\n"));
    planner_abort_hard();
	ENABLE_TEMPERATURE_INTERRUPT();
#if (defined(FANCHECK) && defined(TACH_1) && (TACH_1 >-1))
	ENABLE_FANCHECK_INTERRUPT();
#endif //(defined(FANCHECK) && defined(TACH_1) && (TACH_1 >-1))
	ENABLE_STEPPER_DRIVER_INTERRUPT();
#ifdef WATCHDOG
	wdt_enable(WDTO_4S);
#endif //WATCHDOG
}

int8_t xyzcal_read_pinda(void)
{
	return ((READ(Z_MIN_PIN) != Z_MIN_ENDSTOP_INVERTING)?1:0);
}

uint16_t xyzcal_stepZ_up_while_on(int16_t max_z, uint16_t delay_us)
{
//	printf_P(PSTR("xyzcal_stepZ_up_while_on %d\n"), max_z);
	if (!xyzcal_read_pinda()) return 0;
	uint16_t steps = 0;
	sm4_set_dir(Z_AXIS, 0);
	while (xyzcal_read_pinda() && (count_position[Z_AXIS] < (long)max_z))
	{
		sm4_do_step(Z_AXIS_MASK);
		delayMicroseconds(delay_us);
		count_position[Z_AXIS]++;
		steps++;
	}
	return steps;
}

uint16_t xyzcal_stepZ_dn_while_off(int16_t min_z, uint16_t delay_us)
{
//	printf_P(PSTR("xyzcal_stepZ_dn_while_off %d\n"), min_z);
	if (xyzcal_read_pinda()) return 0;
	uint16_t steps = 0;
	sm4_set_dir(Z_AXIS, 1);
	while (!xyzcal_read_pinda() && (count_position[Z_AXIS] > (long)min_z))
	{
		sm4_do_step(Z_AXIS_MASK);
		delayMicroseconds(delay_us);
		count_position[Z_AXIS]--;
		steps++;
	}
	return steps;
}

void xyzcal_stepZ_by(int16_t z_delta, uint16_t delay_us)
{
	sm4_set_dir(Z_AXIS, (z_delta < 0)?1:0);
	while (z_delta)
	{
		sm4_do_step(Z_AXIS_MASK);
		delayMicroseconds(delay_us);
		if (z_delta > 0)
		{
			count_position[Z_AXIS]++;
			z_delta--;
		}
		else
		{
			count_position[Z_AXIS]--;
			z_delta++;
		}
	}
}

void xyzcal_stepZ_to(int16_t z_target, uint16_t delay_us)
{
//	printf_P(PSTR("xyzcal_stepZ_to %d\n"), z_target);
	xyzcal_stepZ_by(z_target - count_position[Z_AXIS], delay_us);
}

bool xyzcal_lineXYZ_ui(uint16_t dx, uint16_t dy, uint16_t dz, uint16_t delay_us, int8_t check_pinda)
{
//	printf_P(PSTR("xyzcal_lineXYZ_ui %u %u %u %u %d\n"), dx, dy, dz, delay_us, check_pinda);
	uint16_t d = (uint16_t)(sqrt((float)(((uint32_t)dx)*dx + ((uint32_t)dy*dy) + ((uint32_t)dz*dz))) + 0.5);
//	printf_P(PSTR(" d = %u\n"), d);
	uint16_t cx = d;
	uint16_t cy = d;
	uint16_t cz = d;
	uint16_t nx = dx;
	uint16_t ny = dy;
	uint16_t nz = dz;
	uint8_t msk = 0;
//	int8_t pinda = 0;
	int8_t pinda = xyzcal_read_pinda();
//	printf_P(PSTR(" pinda = %d\n"), pinda);
	while (nx || ny || nz)
	{
		msk = 0;
		if (cx <= dx)
		{
			msk |= 1;
			nx--;
			cx += d;
		}
		if (cy <= dy)
		{
			msk |= 2;
			ny--;
			cy += d;
		}
		if (cz <= dz)
		{
			msk |= 4;
			nz--;
			cz += d;
		}
		cx -= dx;
		cy -= dy;
		cz -= dz;
		sm4_do_step(msk);
		delayMicroseconds(delay_us);
		if (check_pinda)
		{
			pinda = xyzcal_read_pinda();
			if ((check_pinda > 0) && pinda) break;
			if ((check_pinda < 0) && !pinda) break;
		}
//		printf_P(PSTR("%d %d %d %d\n"), nx, ny, nz, delay_us);
	}
	if (sm4_get_dir(X_AXIS)) count_position[X_AXIS] -= (dx - nx);
	else count_position[X_AXIS] += (dx - nx);
	if (sm4_get_dir(Y_AXIS)) count_position[Y_AXIS] -= (dy - ny);
	else count_position[Y_AXIS] += (dy - ny);
	if (sm4_get_dir(Z_AXIS)) count_position[Z_AXIS] -= (dz - nz);
	else count_position[Z_AXIS] += (dz - nz);
	if ((check_pinda > 0) && pinda)
	{
//	int8_t pinda = xyzcal_read_pinda();
//	printf_P(PSTR(" pinda = %d\n"), pinda);
//		printf_P(PSTR("PINDA 0>1\n"));
		return true;
	}
	if ((check_pinda < 0) && !pinda)
	{
//	int8_t pinda = xyzcal_read_pinda();
//	printf_P(PSTR(" pinda = %d\n"), pinda);
//		printf_P(PSTR("PINDA 1>0\n"));
		return true;
	}
	return false;
}

uint8_t check_pinda_cb_0()
{
	return xyzcal_read_pinda()?0:1;
}

uint8_t check_pinda_cb_1()
{
	return xyzcal_read_pinda()?1:0;
}

uint8_t xyzcal_dm = 0;

void xyzcal_update_pos_cb(uint16_t dx, uint16_t dy, uint16_t dz, uint16_t de)
{
	if (dx) count_position[0] += (xyzcal_dm&1)?-dx:dx;
	if (dy) count_position[1] += (xyzcal_dm&2)?-dy:dy;
	if (dz) count_position[2] += (xyzcal_dm&4)?-dz:dz;
}

bool xyzcal_lineXYZ(int16_t x, int16_t y, int16_t z, uint16_t delay_us, int8_t check_pinda)
{
	x -= (int16_t)count_position[0];
	y -= (int16_t)count_position[1];
	z -= (int16_t)count_position[2];
	xyzcal_dm = ((x<0)?1:0) | ((z<0)?2:0) | ((y<0)?4:0);
	sm4_set_dir_bits(xyzcal_dm);
	sm4_line_xyz_ui(x, y, z, delay_us, check_pinda?((check_pinda<0)?check_pinda_cb_0:check_pinda_cb_1):0)?true:false;
}

bool xyzcal_lineXYZ_by(int16_t dx, int16_t dy, int16_t dz, uint16_t delay_us, int8_t check_pinda)
{
	if (dx > 0) sm4_set_dir(X_AXIS, 0);
	else if (dx < 0) sm4_set_dir(X_AXIS, 1);
	if (dy > 0) sm4_set_dir(Y_AXIS, 0);
	else if (dy < 0) sm4_set_dir(Y_AXIS, 1);
	if (dz > 0) sm4_set_dir(Z_AXIS, 0);
	else if (dz < 0) sm4_set_dir(Z_AXIS, 1);
	return xyzcal_lineXYZ_ui(abs(dx), abs(dy), abs(dz), delay_us, check_pinda);
}

bool xyzcal_lineXYZ_to(int16_t x, int16_t y, int16_t z, uint16_t delay_us, int8_t check_pinda)
{
//	printf_P(PSTR("  xyzcal_lineXYZ_to %d %d %d %u\n"), x, y, z, delay_us);
	return xyzcal_lineXYZ_by(x - (int16_t)count_position[0], y - (int16_t)count_position[1], z - (int16_t)count_position[2], delay_us, check_pinda);
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
	printf_P(PSTR("xyzcal_spiral2 cx=%d cy=%d z0=%d dz=%d radius=%d ad=%d\n"), cx, cy, z0, dz, radius, ad);
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
	printf_P(PSTR("xyzcal_spiral8 cx=%d cy=%d z0=%d dz=%d radius=%d ad=%d\n"), cx, cy, z0, dz, radius, ad);
	if (!ret && (ad < 720))
		if (ret = xyzcal_spiral2(cx, cy, z0 - 0*dz, dz, radius, 0, delay_us, check_pinda, &ad))
			ad += 0;
	if (!ret && (ad < 1440))
		if (ret = xyzcal_spiral2(cx, cy, z0 - 1*dz, dz, -radius, 0, delay_us, check_pinda, &ad))
			ad += 720;
	if (!ret && (ad < 2160))
		if (ret = xyzcal_spiral2(cx, cy, z0 - 2*dz, dz, radius, 180, delay_us, check_pinda, &ad))
			ad += 1440;
	if (!ret && (ad < 2880))
		if (ret = xyzcal_spiral2(cx, cy, z0 - 3*dz, dz, -radius, 180, delay_us, check_pinda, &ad))
			ad += 2160;
	if (pad) *pad = ad;
	return ret;
}

int8_t xyzcal_meassure_pinda_hysterezis(int16_t min_z, int16_t max_z, uint16_t delay_us, uint8_t samples)
{
//	printf_P(PSTR("xyzcal_meassure_pinda_hysterezis\n"));
	int8_t ret = -1; // PINDA signal error
	int16_t z = (int16_t)count_position[Z_AXIS];
	int16_t sum_up = 0;
	int16_t sum_dn = 0;
	int16_t up;
	int16_t dn;
	uint8_t sample;
	dn = xyzcal_stepZ_dn_while_off(min_z, delay_us);
//	printf_P(PSTR("dn=%d\n"), sample, up, dn);
	up = xyzcal_stepZ_up_while_on(max_z, delay_us);
//	printf_P(PSTR("up=%d\n"), sample, up, dn);
	if (!xyzcal_read_pinda())
	{
		for (sample = 0; sample < samples; sample++)
		{
			dn = xyzcal_stepZ_dn_while_off(min_z, 2*delay_us);
			if (!xyzcal_read_pinda()) break;
			up = xyzcal_stepZ_up_while_on(max_z, 2*delay_us);
			if (xyzcal_read_pinda()) break;
//			printf_P(PSTR("%d. up=%d dn=%d\n"), sample, up, dn);
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
	xyzcal_stepZ_to(z, delay_us); //return to original Z position
	return ret;
}

void xyzcal_scan_pixels_32x32(int16_t cx, int16_t cy, int16_t min_z, int16_t max_z, uint16_t delay_us, uint8_t* pixels)
{
	printf_P(PSTR("xyzcal_scan_pixels_32x32 cx=%d cy=%d min_z=%d max_z=%d\n"), cx, cy, min_z, max_z);
//	xyzcal_lineXYZ_to(cx - 1024, cy - 1024, max_z, 2*delay_us, 0);
//	xyzcal_lineXYZ_to(cx, cy, max_z, delay_us, 0);
	int16_t z = (int16_t)count_position[2];
	xyzcal_lineXYZ_to(cx, cy, z, 2*delay_us, 0);
	for (uint8_t r = 0; r < 32; r++)
	{
		int8_t _pinda = xyzcal_read_pinda();
		xyzcal_lineXYZ_to((r&1)?(cx+1024):(cx-1024), cy - 1024 + r*64, z, 2*delay_us, 0);
		xyzcal_stepZ_dn_while_off(min_z, 2*delay_us);
		xyzcal_stepZ_up_while_on(max_z, 2*delay_us);
		z = (int16_t)count_position[2];
		sm4_set_dir(X_AXIS, (r&1)?1:0);
		for (uint8_t c = 0; c < 32; c++)
		{
			uint16_t sum = 0;
			int16_t z_sum = 0;
			for (uint8_t i = 0; i < 64; i++)
			{
				int8_t pinda = xyzcal_read_pinda();
				int16_t pix = z - min_z;
				pix += (pinda)?23:-24;
				if (pix < 0) pix = 0;
				if (pix > 255) pix = 255;
				sum += pix;
				z_sum += z;
//				if (_pinda != pinda)
//				{
//					if (pinda)
//						printf_P(PSTR("!1 x=%d z=%d\n"), c*64+i, z+23);
//					else
//						printf_P(PSTR("!0 x=%d z=%d\n"), c*64+i, z-24);
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
				_pinda = pinda;
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
//			printf_P(PSTR("c=%d r=%d l=%d z=%d\n"), c, r, sum, z_sum);
			count_position[0] += (r&1)?-64:64;
			count_position[2] = z;
		}
		if (pixels)
			for (uint8_t c = 0; c < 32; c++)
				printf_P(PSTR("%02x"), pixels[((uint16_t)r<<5) + c]);
		printf_P(PSTR("\n"));
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
		printf_P(PSTR(" %2d %d\n"), l, histo[l]);
}

void xyzcal_adjust_pixels(uint8_t* pixels, uint16_t* histo)
{
	uint8_t l;
	uint16_t max_c = histo[0];
	uint8_t max_l = 0;
	for (l = 1; l < 16; l++)
	{
		uint16_t c = histo[l];
		if (c > max_c)
		{
			max_c = c;
			max_l = l;
		}
	}
	printf_P(PSTR("max_c=%2d max_l=%d\n"), max_c, max_l);
	for (l = 15; l > 8; l--)
		if (histo[l] >= 10)
			break;
	uint8_t pix_min = (max_l + 3) << 4;
	uint8_t pix_max = l << 4;
	uint8_t pix_dif = pix_max - pix_min;
	printf_P(PSTR(" min=%d max=%d dif=%d\n"), pix_min, pix_max, pix_dif);
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
			printf_P(PSTR("%02x"), pixels[((uint16_t)r<<5) + c]);
		printf_P(PSTR("\n"));
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
	uint8_t thr = 64;
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
//				printf("%2d %2d %d\n", x, y, match8x8(0, pixels, w, h, x, y));
		}
	printf("max_c=%d max_r=%d max_match=%d\n", max_c, max_r, max_match);
	if (pc) *pc = max_c;
	if (pr) *pr = max_r;
	return max_match;
}

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
	int8_t _pinda = xyzcal_read_pinda();
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
			int8_t pinda = xyzcal_read_pinda();
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
					printf_P(PSTR("!1 x=%d y=%d\n"), x, y);
				else
					printf_P(PSTR("!0 x=%d y=%d\n"), x, y);
			}*/
			_pinda = pinda;
//			printf_P(PSTR("x=%d y=%d rx=%d ry=%d\n"), x, y, rx, ry);
		}
		printf_P(PSTR("x_min=%d x_max=%d y_min=%d y_max=%d r_min=%d r_max=%d r_avg=%d\n"), x_min, x_max, y_min, y_max, r_min, r_max, r_sum / 720);
		if ((n > 2) && (n & 1))
		{
			x0 += (x_min + x_max);
			y0 += (y_min + y_max);
			x0 /= 3;
			y0 /= 3;
			int rx = (x_max - x_min) / 2;
			int ry = (y_max - y_min) / 2;
			r = (rx + ry) / 3;//(rx < ry)?rx:ry;
			printf_P(PSTR("x0=%d y0=%d r=%d\n"), x0, y0, r);
		}
	}
	xyzcal_lineXYZ_to(x0, y0, z, 200, 0);
}

uint8_t xyzcal_xycoords2point(int16_t x, int16_t y)
{
	uint8_t ix = (x > 10000)?1:0;
	uint8_t iy = (y > 10000)?1:0;
	return iy?(3-ix):ix;
}

//const int16_t PROGMEM xyzcal_point_xcoords[4] = {1200, 22000, 22000, 1200};
//const int16_t PROGMEM xyzcal_point_ycoords[4] = {600, 600, 19800, 19800};
const int16_t PROGMEM xyzcal_point_xcoords[4] = {1200, 22000, 22000, 1200};
const int16_t PROGMEM xyzcal_point_ycoords[4] = {700, 700, 19800, 19800};

const int16_t PROGMEM xyzcal_point_xcoords_[4] = {1131, 21939, 21964, 1122};
const int16_t PROGMEM xyzcal_point_ycoords_[4] = {709, 674, 19883, 19922};

const uint16_t PROGMEM xyzcal_point_pattern[12] = {0x000, 0x0f0, 0x1f8, 0x3fc, 0x7fe, 0x7fe, 0x7fe, 0x7fe, 0x3fc, 0x1f8, 0x0f0, 0x000};
/*
int16_t xyzcal_point2xcoord(uint8_t point)
{
	return xyzcal_point_xcoords[point & 3];
}

int16_t xyzcal_point2ycoord(uint8_t point)
{
	return xyzcal_point_ycoords[point & 3];
}
*/
bool xyzcal_searchZ(void)
{
	printf_P(PSTR("xyzcal_searchZ x=%ld y=%ld z=%ld\n"), count_position[X_AXIS], count_position[Y_AXIS], count_position[Z_AXIS]);
	int16_t x0 = (int16_t)count_position[X_AXIS];
	int16_t y0 = (int16_t)count_position[Y_AXIS];
	int16_t z0 = (int16_t)count_position[Z_AXIS];
//	int16_t min_z = -6000;
//	int16_t dz = 100;
	int16_t z = z0;
	while (z > -2300) //-6mm + 0.25mm
	{
		uint16_t ad = 0;
		if (xyzcal_spiral8(x0, y0, z, 100, 900, 320, 1, &ad)) //dz=100 radius=900 delay=400
		{
			int16_t x_on = (int16_t)count_position[X_AXIS];
			int16_t y_on = (int16_t)count_position[Y_AXIS];
			int16_t z_on = (int16_t)count_position[Z_AXIS];
			printf_P(PSTR(" ON-SIGNAL at x=%d y=%d z=%d ad=%d\n"), x_on, y_on, z_on, ad);
			return true;
		}
		z -= 400;
	}
	printf_P(PSTR("xyzcal_searchZ no signal\n x=%ld y=%ld z=%ld\n"), count_position[X_AXIS], count_position[Y_AXIS], count_position[Z_AXIS]);
	return false;
}

bool xyzcal_scan_and_process(void)
{
	printf_P(PSTR("sizeof(block_buffer)=%d\n"), sizeof(block_t)*BLOCK_BUFFER_SIZE);
//	printf_P(PSTR("sizeof(pixels)=%d\n"), 32*32);
//	printf_P(PSTR("sizeof(histo)=%d\n"), 2*16);
//	printf_P(PSTR("sizeof(pattern)=%d\n"), 2*12);
	printf_P(PSTR("sizeof(total)=%d\n"), 32*32+2*16+2*12);
	bool ret = false;
	int16_t x = (int16_t)count_position[X_AXIS];
	int16_t y = (int16_t)count_position[Y_AXIS];
	int16_t z = (int16_t)count_position[Z_AXIS];

	uint8_t* pixels = (uint8_t*)block_buffer;
	xyzcal_scan_pixels_32x32(x, y, z - 128, 2400, 200, pixels);

	uint16_t* histo = (uint16_t*)(pixels + 32*32);
	xyzcal_histo_pixels_32x32(pixels, histo);

	xyzcal_adjust_pixels(pixels, histo);

	uint16_t* pattern = (uint16_t*)(histo + 2*16);
	for (uint8_t i = 0; i < 12; i++)
	{
		pattern[i] = pgm_read_word_far((uint16_t*)(xyzcal_point_pattern + i));
//		printf_P(PSTR(" pattern[%d]=%d\n"), i, pattern[i]);
	}
	uint8_t c = 0;
	uint8_t r = 0;
	if (xyzcal_find_pattern_12x12_in_32x32(pixels, pattern, &c, &r) > 66) //total pixels=144, corner=12 (1/2 = 66)
	{
		printf_P(PSTR(" pattern found at %d %d\n"), c, r);
		c += 6;
		r += 6;
		x += ((int16_t)c - 16) << 6;
		y += ((int16_t)r - 16) << 6;
		printf_P(PSTR(" x=%d y=%d z=%d\n"), x, y, z);
		xyzcal_lineXYZ_to(x, y, z, 200, 0);
		ret = true;
	}
	for (uint16_t i = 0; i < sizeof(block_t)*BLOCK_BUFFER_SIZE; i++)
		pixels[i] = 0;
	return ret;
}

bool xyzcal_find_bed_induction_sensor_point_xy(void)
{
	printf_P(PSTR("xyzcal_find_bed_induction_sensor_point_xy x=%ld y=%ld z=%ld\n"), count_position[X_AXIS], count_position[Y_AXIS], count_position[Z_AXIS]);
	bool ret = false;
	st_synchronize();
	int16_t x = (int16_t)count_position[X_AXIS];
	int16_t y = (int16_t)count_position[Y_AXIS];
	int16_t z = (int16_t)count_position[Z_AXIS];
	uint8_t point = xyzcal_xycoords2point(x, y);
	x = pgm_read_word_far((uint16_t*)(xyzcal_point_xcoords + point));
	y = pgm_read_word_far((uint16_t*)(xyzcal_point_ycoords + point));
	printf_P(PSTR("point=%d x=%d y=%d z=%d\n"), point, x, y, z);
	xyzcal_meassure_enter();
	xyzcal_lineXYZ_to(x, y, z, 200, 0);
	if (xyzcal_searchZ())
	{
		int16_t z = (int16_t)count_position[Z_AXIS];
		xyzcal_lineXYZ_to(x, y, z, 200, 0);
		if (xyzcal_scan_and_process())
		{
			ret = true;
		}
/*
		x = pgm_read_word_far((uint16_t*)(xyzcal_point_xcoords_ + point));
		y = pgm_read_word_far((uint16_t*)(xyzcal_point_ycoords_ + point));
		printf_P(PSTR("point=%d x=%d y=%d z=%d\n"), point, x, y, z);
		xyzcal_lineXYZ_to(x, y, z, 200, 0);
		xyzcal_stepZ_dn_while_off(-2400, 500);
		xyzcal_stepZ_up_while_on(800, 500);
		ret = true;*/
	}
	xyzcal_meassure_leave();
	return ret;
}

bool xyzcal_improve_bed_induction_sensor_point(void)
{
	printf_P(PSTR("xyzcal_improve_bed_induction_sensor_point x=%ld y=%ld z=%ld\n"), count_position[X_AXIS], count_position[Y_AXIS], count_position[Z_AXIS]);
	st_synchronize();
	xyzcal_meassure_enter();
	xyzcal_meassure_leave();
	return true;
}

bool xyzcal_improve_bed_induction_sensor_point2(bool lift_z_on_min_y)
{
	printf_P(PSTR("xyzcal_improve_bed_induction_sensor_point2 x=%ld y=%ld z=%ld lift_z_on_min_y=%d\n"), count_position[X_AXIS], count_position[Y_AXIS], count_position[Z_AXIS], lift_z_on_min_y?1:0);
	return true;
}

bool xyzcal_improve_bed_induction_sensor_point3(void)
{
	printf_P(PSTR("xyzcal_improve_bed_induction_sensor_point3 x=%ld y=%ld z=%ld\n"), count_position[X_AXIS], count_position[Y_AXIS], count_position[Z_AXIS]);
	return true;
}

