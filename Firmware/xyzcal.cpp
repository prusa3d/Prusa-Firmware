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

#ifndef M_PI
const constexpr float M_PI = 3.1415926535897932384626433832795f;
#endif

const constexpr uint8_t X_PLUS = 0;
const constexpr uint8_t X_MINUS = 1;
const constexpr uint8_t Y_PLUS = 0;
const constexpr uint8_t Y_MINUS = 1;
const constexpr uint8_t Z_PLUS = 0;
const constexpr uint8_t Z_MINUS = 1;

/// Max. jerk in PrusaSlicer, 10000 = 1 mm/s
const constexpr uint16_t MAX_DELAY = 10000;
const constexpr float MIN_SPEED = 0.01f / (MAX_DELAY * 0.000001f);
/// 200 = 50 mm/s
const constexpr uint16_t Z_MIN_DELAY = 200;
const constexpr uint16_t Z_ACCEL = 1000;

/// \returns positive value always
#define ABS(a) \
    ({ __typeof__ (a) _a = (a); \
    _a >= 0 ? _a : (-_a); })

/// \returns maximum of the two
#define MAX(a, b) \
    ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a >= _b ? _a : _b; })

/// \returns minimum of the two
#define MIN(a, b) \
    ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a <= _b ? _a : _b; })

/// swap values
#define SWAP(a, b) \
    ({ __typeof__ (a) c = (a); \
        a = (b); \
        b = c; })

/// Saturates value
/// \returns min if value is less than min
/// \returns max if value is more than min
/// \returns value otherwise
#define CLAMP(value, min, max) \
    ({ __typeof__ (value) a_ = (value); \
		__typeof__ (min) min_ = (min); \
		__typeof__ (max) max_ = (max); \
        ( a_ < min_ ? min_ : (a_ <= max_ ? a_ : max_)); })

/// \returns square of the value
#define SQR(a) \
    ({ __typeof__ (a) a_ = (a); \
        (a_ * a_); })

/// position types
typedef int16_t pos_i16_t;
typedef long pos_i32_t;
typedef float pos_mm_t;
typedef int16_t usteps_t;

uint8_t check_pinda_0();
uint8_t check_pinda_1();
void xyzcal_update_pos(uint16_t dx, uint16_t dy, uint16_t dz, uint16_t de);
uint16_t xyzcal_calc_delay(uint16_t nd, uint16_t dd);

uint8_t round_to_u8(float f){
	return (uint8_t)(f + .5f);
}

uint16_t round_to_u16(float f){
	return (uint16_t)(f + .5f);
}

int16_t round_to_i16(float f){
	return (int16_t)(f + .5f);
}

/// converts millimeters to integer position
pos_i16_t mm_2_pos(pos_mm_t mm){
	return (pos_i16_t)(0.5f + mm * 100);
}

/// converts integer position to millimeters
pos_mm_t pos_2_mm(pos_i16_t pos){
	return pos * 0.01f;
}
pos_mm_t pos_2_mm(float pos){
	return pos * 0.01f;
}

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

/// Moves printer to absolute position [x,y,z] defined in integer position system
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
	//	uint32_t u = _micros();
	bool ret = sm4_line_xyze_ui(abs(x), abs(y), abs(z), 0) ? true : false;
	//	u = _micros() - u;
	return ret;
}

/// Moves printer to absolute position [x,y,z] defined in millimeters
bool xyzcal_lineXYZ_to_float(pos_mm_t x, pos_mm_t y, pos_mm_t z, uint16_t delay_us, int8_t check_pinda){
	return xyzcal_lineXYZ_to(mm_2_pos(x), mm_2_pos(y), mm_2_pos(z), delay_us, check_pinda);
}

bool xyzcal_spiral2(int16_t cx, int16_t cy, int16_t z0, int16_t dz, int16_t radius, int16_t rotation, uint16_t delay_us, int8_t check_pinda, uint16_t* pad)
{
	bool ret = false;
	float r = 0; //radius
	uint16_t ad = 0; //angle [deg]
	float ar; //angle [rad]
	uint8_t dad = 0; //delta angle [deg]
	uint8_t dad_min = 4; //delta angle min [deg]
	uint8_t dad_max = 16; //delta angle max [deg]
	uint8_t k = 720 / (dad_max - dad_min); //delta calculation constant
	ad = 0;
	if (pad) ad = *pad % 720;
	
	DBG(_n("xyzcal_spiral2 cx=%d cy=%d z0=%d dz=%d radius=%d ad=%d\n"), cx, cy, z0, dz, radius, ad);
	// lcd_set_cursor(0, 4);
	// char text[10];
	// snprintf(text, 10, "%4d", z0);
	// lcd_print(text);

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
		ar = (ad + rotation)* (float)M_PI / 180;
		int x = (int)(cx + (cos(ar) * r));
		int y = (int)(cy + (sin(ar) * r));
		int z = (int)(z0 - ((float)((int32_t)dz * ad) / 720));
		if (xyzcal_lineXYZ_to(x, y, z, delay_us, check_pinda))
		{
			ad += dad + 1;
			ret = true;
			break;
		}
		ad += dad;
	}
	if (pad) *pad = ad;
	// if(ret){
	// 	lcd_set_cursor(0, 4);
	// 	lcd_print("         ");
	// }
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

/// Accelerate up to max.speed (defined by @min_delay_us)
void accelerate(uint8_t axis, int16_t acc, uint16_t &delay_us, uint16_t min_delay_us){
	sm4_do_step(axis);

	/// keep max speed (avoid extra computation)
	if (acc > 0 && delay_us == min_delay_us){
		delayMicroseconds(delay_us);
		return;
	}

	// v1 = v0 + a * t
	// 0.01 = length of a step
	const float t0 = delay_us * 0.000001f;
	const float v1 = (0.01f / t0 + acc * t0);
	uint16_t t1;
	if (v1 <= 0.16f){ ///< slowest speed convertible to uint16_t delay
		t1 = MAX_DELAY; ///< already too slow so it wants to move back
	} else {
		/// don't exceed max.speed
		t1 = MAX(min_delay_us, round_to_u16(0.01f / v1 * 1000000.f));
	}

	/// make sure delay has changed a bit at least
	if (t1 == delay_us && acc != 0){
		if (acc > 0)
			t1--;
		else
			t1++;
	}
	
	//DBG(_n("%d "), t1);

	delayMicroseconds(t1);
	delay_us = t1;
}

void go_and_stop(uint8_t axis, int16_t dec, uint16_t &delay_us, uint16_t &steps){
	if (steps <= 0 || dec <= 0)
		return;

	/// deceleration distance in steps, s = 1/2 v^2 / a
	uint16_t s = round_to_u16(100 * 0.5f * SQR(0.01f) / (SQR((float)delay_us) * dec));
	if (steps > s){
		/// go steady
		sm4_do_step(axis);
		delayMicroseconds(delay_us);
	} else {
		/// decelerate
		accelerate(axis, -dec, delay_us, delay_us);
	}
	--steps;
}

void xyzcal_scan_pixels_32x32_Zhop(int16_t cx, int16_t cy, int16_t min_z, int16_t max_z, uint16_t delay_us, uint8_t* pixels){
	if (!pixels)
		return;
	int16_t z = _Z;
	int16_t z_trig;
	uint16_t line_buffer[32];
	uint16_t current_delay_us = MAX_DELAY; ///< defines current speed
	xyzcal_lineXYZ_to(cx - 1024, cy - 1024, min_z, delay_us, 0);
	int16_t start_z;
	uint16_t steps_to_go;

	for (uint8_t r = 0; r < 32; r++){ ///< Y axis
		xyzcal_lineXYZ_to(_X, cy - 1024 + r * 64, z, delay_us, 0);
		for (int8_t d = 0; d < 2; ++d){ ///< direction			
			xyzcal_lineXYZ_to((d & 1) ? (cx + 1024) : (cx - 1024), _Y, min_z, delay_us, 0);

			z = _Z;
			sm4_set_dir(X_AXIS, d);
			for (uint8_t c = 0; c < 32; c++){ ///< X axis

				z_trig = min_z;

				/// move up to un-trigger (surpress hysteresis)
				sm4_set_dir(Z_AXIS, Z_PLUS);
				/// speed up from stop, go half the way
				current_delay_us = MAX_DELAY;
				for (start_z = z; z < (max_z + start_z) / 2; ++z){
					if (!_PINDA){
						break;
					}
					accelerate(Z_AXIS_MASK, Z_ACCEL, current_delay_us, Z_MIN_DELAY);
				}

				if(_PINDA){
					uint16_t steps_to_go = MAX(0, max_z - z);
					while (_PINDA && z < max_z){
						go_and_stop(Z_AXIS_MASK, Z_ACCEL, current_delay_us, steps_to_go);
						++z;
					}
				}
				/// slow down to stop
				while (current_delay_us < MAX_DELAY){
					accelerate(Z_AXIS_MASK, -Z_ACCEL, current_delay_us, Z_MIN_DELAY);
					++z;
				}

				/// move down to trigger
				sm4_set_dir(Z_AXIS, Z_MINUS);
				/// speed up
				current_delay_us = MAX_DELAY;
				for (start_z = z; z > (min_z + start_z) / 2; --z){
					if (_PINDA){
						z_trig = z;
						break;
					}
					accelerate(Z_AXIS_MASK, Z_ACCEL, current_delay_us, Z_MIN_DELAY);
				}
				/// slow down
				if(!_PINDA){
					steps_to_go = MAX(0, z - min_z);
					while (!_PINDA && z > min_z){
						go_and_stop(Z_AXIS_MASK, Z_ACCEL, current_delay_us, steps_to_go);
						--z;
					}
					z_trig = z;
				}
				/// slow down to stop
				while (z > min_z && current_delay_us < MAX_DELAY){
					accelerate(Z_AXIS_MASK, -Z_ACCEL, current_delay_us, Z_MIN_DELAY);
					--z;
				}

				count_position[2] = z;
				if (d == 0){
					line_buffer[c] = (uint16_t)(z_trig - min_z);
				} else {
					/// data reversed in X
					// DBG(_n("%04x"), (line_buffer[31 - c] + (z - min_z)) / 2);
					/// save average of both directions
					pixels[(uint16_t)r * 32 + (31 - c)] = (uint8_t)MIN((uint32_t)255, ((uint32_t)line_buffer[31 - c] + (z_trig - min_z)) / 2);
				}

				/// move to the next point and move Z up diagonally (if needed)
				current_delay_us = MAX_DELAY;
				// const int8_t dir = (d & 1) ? -1 : 1;
				const int16_t end_x = ((d & 1) ? 1 : -1) * (64 * (16 - c) - 32) + cx;
				const int16_t length_x = ABS(end_x - _X);
				const int16_t half_x = length_x / 2;
				int16_t x = 0;
				/// don't go up if PINDA not triggered
				const bool up = _PINDA;
				int8_t axis = up ? X_AXIS_MASK | Z_AXIS_MASK : X_AXIS_MASK;

				sm4_set_dir(Z_AXIS, Z_PLUS);
				/// speed up
				for (x = 0; x <= half_x; ++x){
					accelerate(axis, Z_ACCEL, current_delay_us, Z_MIN_DELAY);
					if (up)
						++z;
				}
				/// slow down
				steps_to_go = length_x - x;
				for (; x < length_x; ++x){
					go_and_stop(axis, Z_ACCEL, current_delay_us, steps_to_go);
					if (up)
						++z;
				}
				count_position[0] = end_x;
				count_position[2] = z;
			}
		}
		// DBG(_n("\n\n"));
	}
}

/// Returns rate of match
/// max match = 132, min match = 0
uint8_t xyzcal_match_pattern_12x12_in_32x32(uint16_t* pattern, uint8_t* pixels, uint8_t c, uint8_t r){
	uint8_t thr = 16;
	uint8_t match = 0;
	for (uint8_t i = 0; i < 12; ++i){
		for (uint8_t j = 0; j < 12; ++j){
			/// skip corners (3 pixels in each)
			if (((i == 0) || (i == 11)) && ((j < 2) || (j >= 10))) continue;
			if (((j == 0) || (j == 11)) && ((i < 2) || (i >= 10))) continue;
			const uint16_t idx = (c + j) + 32 * ((uint16_t)r + i);
			const bool high_pix = pixels[idx] > thr;
			const bool high_pat = pattern[i] & (1 << j);
			if (high_pix == high_pat)
				match++;
		}
	}
	return match;
}

/// Searches for best match of pattern by shifting it
/// Returns rate of match and the best location
/// max match = 132, min match = 0
uint8_t xyzcal_find_pattern_12x12_in_32x32(uint8_t* pixels, uint16_t* pattern, uint8_t* pc, uint8_t* pr){
	if (!pixels || !pattern || !pc || !pr)
		return -1;
	uint8_t max_c = 0;
	uint8_t max_r = 0;
	uint8_t max_match = 0;

	// DBG(_n("Matching:\n"));
	/// pixel precision
	for (uint8_t r = 0; r < (32 - 12); ++r){
		for (uint8_t c = 0; c < (32 - 12); ++c){
			const uint8_t match = xyzcal_match_pattern_12x12_in_32x32(pattern, pixels, c, r);
			if (max_match < match){
				max_c = c;
				max_r = r;
				max_match = match;
			}
			// DBG(_n("%d "), match);
		}
		// DBG(_n("\n"));
	}
	DBG(_n("max_c=%d max_r=%d max_match=%d pixel\n"), max_c, max_r, max_match);

	*pc = max_c;
	*pr = max_r;
	return max_match;
}

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

/// returns value of any location within data
/// uses bilinear interpolation
float get_value(uint8_t * matrix_32x32, float c, float r){
	if (c <= 0 || r <= 0 || c >= 31 || r >= 31)
		return 0;

	/// calculate weights of nearby points
	const float wc1 = c - floor(c);
	const float wr1 = r - floor(r);
	const float wc0 = 1 - wc1;
	const float wr0 = 1 - wr1;

	const float w00 = wc0 * wr0;
	const float w01 = wc0 * wr1;
	const float w10 = wc1 * wr0;
	const float w11 = wc1 * wr1;

	const uint16_t c0 = c;
	const uint16_t c1 = c0 + 1;
	const uint16_t r0 = r;
	const uint16_t r1 = r0 + 1;

	const uint16_t idx00 = c0 + 32 * r0;
	const uint16_t idx01 = c0 + 32 * r1;
	const uint16_t idx10 = c1 + 32 * r0;
	const uint16_t idx11 = c1 + 32 * r1;

	/// bilinear resampling
	return w00 * matrix_32x32[idx00] + w01 * matrix_32x32[idx01] + w10 * matrix_32x32[idx10] + w11 * matrix_32x32[idx11];
}

const constexpr float m_infinity = -1000.f;

/// replaces the highest number by m_infinity
void remove_highest(float *points, const uint8_t num_points){
	if (num_points <= 0)
		return;

	float max = points[0];
	uint8_t max_i = 0;
	for (uint8_t i = 0; i < num_points; ++i){
		if (max < points[i]){
			max = points[i];
			max_i = i;
		}
	}
	points[max_i] = m_infinity;
}

/// return the highest number in the list
float highest(float *points, const uint8_t num_points){
	if (num_points <= 0)
		return 0;

	float max = points[0];
	for (uint8_t i = 0; i < num_points; ++i){
		if (max < points[i]){
			max = points[i];
		}
	}
	return max;
}

/// Searches for circle iteratively
/// Uses points on the perimeter. If point is high it pushes circle out of the center (shift or change of radius),
/// otherwise to the center.
/// Algorithm is stopped after fixed number of iterations. Move is limited to 0.5 px per iteration.
void dynamic_circle(uint8_t *matrix_32x32, float &x, float &y, float &r, uint8_t iterations){
	/// circle of 10.5 diameter has 33 in circumference, don't go much above
	const constexpr uint8_t num_points = 33;
	float points[num_points];
	float pi_2_div_num_points = 2 * M_PI / num_points;
	const constexpr uint8_t target_z = 32; ///< target z height of the circle
	float norm;
	float angle;
	float max_val = 0.5f;
	const uint8_t blocks = 7;
	float shifts_x[blocks];
	float shifts_y[blocks];	
	float shifts_r[blocks];	

	for (int8_t i = iterations; i > 0; --i){
	
		// DBG(_n(" [%f, %f][%f] circle\n"), x, y, r);

		/// read points on the circle
		for (uint8_t p = 0; p < num_points; ++p){
			angle = p * pi_2_div_num_points;
			points[p] = get_value(matrix_32x32, r * cos(angle) + x, r * sin(angle) + y) - target_z;
			// DBG(_n("%f "), points[p]);
		}
		// DBG(_n(" points\n"));

		/// sum blocks
		for (uint8_t j = 0; j < blocks; ++j){
			shifts_x[j] = shifts_y[j] = shifts_r[j] = 0;
			/// first part
			for (uint8_t p = 0; p < num_points * 3 / 4; ++p){
				uint8_t idx = (p + j * num_points / blocks) % num_points;

				angle = idx * pi_2_div_num_points;
				shifts_x[j] += cos(angle) * points[idx];
				shifts_y[j] += sin(angle) * points[idx];
				shifts_r[j] += points[idx];
			}
		}

		/// remove extreme values (slow but simple)
		for (uint8_t j = 0; j < blocks / 2; ++j){
			remove_highest(shifts_x, blocks);
			remove_highest(shifts_y, blocks);
			remove_highest(shifts_r, blocks);
		}

		/// median is the highest now
		norm = 1.f / (32.f * (num_points * 3 / 4));
		x += CLAMP(highest(shifts_x, blocks) * norm, -max_val, max_val);
		y += CLAMP(highest(shifts_y, blocks) * norm, -max_val, max_val);
		r += CLAMP(highest(shifts_r, blocks) * norm, -max_val, max_val);

		r = MAX(2, r);

	}
	DBG(_n(" [%f, %f][%f] final circle\n"), x, y, r);
}

/// Prints matrix in hex to debug output (serial line)
void print_image(uint8_t *matrix_32x32){
	for (uint8_t y = 0; y < 32; ++y){
		const uint16_t idx_y = y * 32;
		for (uint8_t x = 0; x < 32; ++x){
			DBG(_n("%02x"), matrix_32x32[idx_y + x]);
		}
		DBG(_n("\n"));
	}
	DBG(_n("\n"));
}

/// scans area around the current head location and
/// searches for the center of the calibration pin
bool xyzcal_scan_and_process(void){
	DBG(_n("sizeof(block_buffer)=%d\n"), sizeof(block_t)*BLOCK_BUFFER_SIZE);
	bool ret = false;
	int16_t x = _X;
	int16_t y = _Y;
	int16_t z = _Z;

	uint8_t *matrix32 = (uint8_t *)block_buffer;
	uint16_t *pattern = (uint16_t *)(matrix32 + 32 * 32);

	xyzcal_scan_pixels_32x32_Zhop(x, y, z - 72, 2400, 200, matrix32);
	print_image(matrix32);

	for (uint8_t i = 0; i < 12; i++){
		pattern[i] = pgm_read_word((uint16_t*)(xyzcal_point_pattern + i));
//		DBG(_n(" pattern[%d]=%d\n"), i, pattern[i]);
	}
	
	/// SEARCH FOR BINARY CIRCLE
	uint8_t uc = 0;
	uint8_t ur = 0;
	/// max match = 132, 1/2 good = 66, 2/3 good = 88
	if (xyzcal_find_pattern_12x12_in_32x32(matrix32, pattern, &uc, &ur) >= 88){
		/// find precise circle
		/// move to the center of the pattern (+5.5)
		float xf = uc + 5.5f;
		float yf = ur + 5.5f;
		float radius = 5; ///< default radius
		const uint8_t iterations = 20;
		dynamic_circle(matrix32, xf, yf, radius, iterations);
		if (ABS(xf - (uc + 5.5f)) > 3 || ABS(yf - (ur + 5.5f)) > 3 || ABS(radius - 5) > 3){
			DBG(_n(" [%f %f][%f] mm divergence\n"), xf - (uc + 5.5f), yf - (ur + 5.5f), radius - 5);
			/// dynamic algorithm diverged, use original position instead
			xf = uc + 5.5f;
			yf = ur + 5.5f;
		}

		/// move to the center of area and convert to position
		xf = (float)x + (xf - 15.5f) * 64;
		yf = (float)y + (yf - 15.5f) * 64;
		DBG(_n(" [%f %f] mm pattern center\n"), pos_2_mm(xf), pos_2_mm(yf));
		x = round_to_i16(xf);
		y = round_to_i16(yf);
		xyzcal_lineXYZ_to(x, y, z, 200, 0);
		ret = true;
	}

	/// wipe buffer
	for (uint16_t i = 0; i < sizeof(block_t)*BLOCK_BUFFER_SIZE; i++)
		matrix32[i] = 0;
	return ret;
}

bool xyzcal_find_bed_induction_sensor_point_xy(void){
	bool ret = false;

	DBG(_n("xyzcal_find_bed_induction_sensor_point_xy x=%ld y=%ld z=%ld\n"), count_position[X_AXIS], count_position[Y_AXIS], count_position[Z_AXIS]);
	st_synchronize();
	pos_i16_t x = _X;
	pos_i16_t y = _Y;
	pos_i16_t z = _Z;

	uint8_t point = xyzcal_xycoords2point(x, y);
	x = pgm_read_word((uint16_t *)(xyzcal_point_xcoords + point));
	y = pgm_read_word((uint16_t *)(xyzcal_point_ycoords + point));
	DBG(_n("point=%d x=%d y=%d z=%d\n"), point, x, y, z);
	xyzcal_meassure_enter();
	xyzcal_lineXYZ_to(x, y, z, 200, 0);

	if (xyzcal_searchZ()){
		int16_t z = _Z;
		xyzcal_lineXYZ_to(x, y, z, 200, 0);
		ret = xyzcal_scan_and_process();
	}
	xyzcal_meassure_leave();
	return ret;
}

#endif //NEW_XYZCAL
