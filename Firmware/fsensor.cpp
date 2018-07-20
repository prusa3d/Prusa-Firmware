#include "Marlin.h"

#ifdef PAT9125

#include "fsensor.h"
#include "pat9125.h"
#include "stepper.h"
#include "planner.h"
#include "fastio.h"
#include "cmdqueue.h"


const char ERRMSG_PAT9125_NOT_RESP[] PROGMEM = "PAT9125 not responding (%d)!\n";

#define FSENSOR_INT_PIN         63  //filament sensor interrupt pin PK1
#define FSENSOR_INT_PIN_MSK   0x02  //filament sensor interrupt pin mask (bit1)

extern void stop_and_save_print_to_ram(float z_move, float e_move);
extern void restore_print_from_ram_and_continue(float e_move);
extern int8_t FSensorStateMenu;

void fsensor_init(void)
{
	int pat9125 = pat9125_init();
	printf_P(_N("PAT9125_init:%d\n"), pat9125);
	uint8_t fsensor = eeprom_read_byte((uint8_t*)EEPROM_FSENSOR);
     fsensor_autoload_enabled=eeprom_read_byte((uint8_t*)EEPROM_FSENS_AUTOLOAD_ENABLED);
	if (!pat9125)
	{
		fsensor = 0; //disable sensor
		fsensor_not_responding = true;
	}
	else {
		fsensor_not_responding = false;
	}
	puts_P(PSTR("FSensor "));
	if (fsensor)
	{
		puts_P(PSTR("ENABLED\n"));
		fsensor_enable();
	}
	else
	{
		puts_P(PSTR("DISABLED\n"));
		fsensor_disable();
	}
#ifdef DEBUG_DISABLE_FSENSORCHECK
	fsensor_autoload_enabled = false;
	fsensor_disable();
#endif //DEBUG_DISABLE_FSENSORCHECK
}

void fsensor_stop_and_save_print(void)
{
	stop_and_save_print_to_ram(0, 0); //XYZE - no change	
}

void fsensor_restore_print_and_continue(void)
{
	restore_print_from_ram_and_continue(0); //XYZ = orig, E - no change
}

//uint8_t fsensor_int_pin = FSENSOR_INT_PIN;
uint8_t fsensor_int_pin_old = 0;
int16_t fsensor_chunk_len = FSENSOR_CHUNK_LEN;

bool fsensor_enabled = true;
bool fsensor_watch_runout = true;
bool fsensor_not_responding = false;

uint8_t fsensor_err_cnt = 0;
int16_t fsensor_st_cnt = 0;


uint8_t fsensor_log = 1;

//autoload enable/disable flag
bool fsensor_watch_autoload = false;
uint16_t fsensor_autoload_y = 0;
uint8_t fsensor_autoload_c = 0;
uint32_t fsensor_autoload_last_millis = 0;
uint8_t fsensor_autoload_sum = 0;

//filament optical quality meassurement
bool fsensor_oq_meassure = false;
uint8_t  fsensor_oq_skipchunk;
uint32_t fsensor_oq_st_sum;
uint32_t fsensor_oq_yd_sum;
uint16_t fsensor_oq_er_sum;
uint8_t  fsensor_oq_er_max;
uint16_t fsensor_oq_yd_min;
uint16_t fsensor_oq_yd_max;


bool fsensor_enable(void)
{
	int pat9125 = pat9125_init();
    printf_P(PSTR("PAT9125_init:%d\n"), pat9125);
	if (pat9125)
		fsensor_not_responding = false;
	else
		fsensor_not_responding = true;
	fsensor_enabled = pat9125?true:false;
	fsensor_watch_runout = true;
	fsensor_oq_meassure = false;
	fsensor_err_cnt = 0;
	eeprom_update_byte((uint8_t*)EEPROM_FSENSOR, fsensor_enabled?0x01:0x00); 
	FSensorStateMenu = fsensor_enabled?1:0;


	return fsensor_enabled;
}

void fsensor_disable(void)
{
//	puts_P(PSTR("fsensor_disable\n"));
	fsensor_enabled = false;
	eeprom_update_byte((uint8_t*)EEPROM_FSENSOR, 0x00); 
	FSensorStateMenu = 0;
}

void fsensor_autoload_set(bool State)
{
	fsensor_autoload_enabled = State;
	eeprom_update_byte((unsigned char *)EEPROM_FSENS_AUTOLOAD_ENABLED, fsensor_autoload_enabled);
}

void pciSetup(byte pin)
{
	*digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin)); // enable pin
	PCIFR |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
	PCICR |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group 
}

void fsensor_autoload_check_start(void)
{
//	puts_P(PSTR("fsensor_autoload_check_start\n"));
	if (!pat9125_update_y()) //update sensor
	{
		printf_P(ERRMSG_PAT9125_NOT_RESP, 3);
		fsensor_disable();
		fsensor_not_responding = true;
		fsensor_watch_autoload = false;
		return;
	}
	fsensor_autoload_y = pat9125_y; //save current y value
	fsensor_autoload_c = 0; //reset number of changes counter
	fsensor_autoload_sum = 0;
	fsensor_autoload_last_millis = millis();
	fsensor_watch_autoload = true;
	fsensor_err_cnt = 0;
}

void fsensor_autoload_check_stop(void)
{
//	puts_P(PSTR("fsensor_autoload_check_stop\n"));
	fsensor_autoload_sum = 0;
	fsensor_watch_autoload = false;
	fsensor_err_cnt = 0;
}

bool fsensor_check_autoload(void)
{
	uint8_t fsensor_autoload_c_old = fsensor_autoload_c;
	if ((millis() - fsensor_autoload_last_millis) < 25) return false;
	fsensor_autoload_last_millis = millis();
	if (!pat9125_update_y())
	{
		printf_P(ERRMSG_PAT9125_NOT_RESP, 2);
		fsensor_disable();
		fsensor_not_responding = true;
		return false; //update sensor
	}
	int16_t dy = fsensor_autoload_y - pat9125_y;
	if (dy) //? y value is different
	{
		if (dy < 0) //? delta-y value is positive (inserting)
		{
			fsensor_autoload_sum -= dy;
			fsensor_autoload_c += 3; //increment change counter by 3
		}
		else if (fsensor_autoload_c > 1)
			fsensor_autoload_c -= 2; //decrement change counter by 2 
		fsensor_autoload_y = pat9125_y; //save current value
	}
	else if (fsensor_autoload_c > 0)
		fsensor_autoload_c--;
	if (fsensor_autoload_c == 0) fsensor_autoload_sum = 0;
//	if (fsensor_autoload_c != fsensor_autoload_c_old)
//		printf_P(PSTR("fsensor_check_autoload dy=%d c=%d sum=%d\n"), dy, fsensor_autoload_c, fsensor_autoload_sum);
	if ((fsensor_autoload_c >= 15) && (fsensor_autoload_sum > 30))
		return true;
	return false;
}

void fsensor_oq_meassure_start(void)
{
	fsensor_oq_skipchunk = 1;
	fsensor_oq_st_sum = 0;
	fsensor_oq_yd_sum = 0;
	fsensor_oq_er_sum = 0;
	fsensor_oq_er_max = 0;
	fsensor_oq_yd_min = FSENSOR_OQ_MAX_YD;
	fsensor_oq_yd_max = 0;
	pat9125_update_y();
	pat9125_y = 0;
	fsensor_watch_runout = false;
	fsensor_oq_meassure = true;
}

void fsensor_oq_meassure_stop(void)
{
	fsensor_oq_meassure = false;
}

bool fsensor_oq_result(void)
{
	printf(_N("fsensor_oq_result\n"));
	if (fsensor_oq_er_sum > FSENSOR_OQ_MAX_ER) return false;
	printf(_N(" er_sum OK\n"));
	uint8_t yd_avg = fsensor_oq_yd_sum * FSENSOR_CHUNK_LEN / fsensor_oq_st_sum;
	if ((yd_avg < FSENSOR_OQ_MIN_YD) || (yd_avg > FSENSOR_OQ_MAX_YD)) return false;
	printf(_N(" yd_avg OK\n"));
	if (fsensor_oq_yd_max > (yd_avg * FSENSOR_OQ_MAX_PD)) return false;
	printf(_N(" yd_max OK\n"));
	if (fsensor_oq_yd_min < (yd_avg / FSENSOR_OQ_MAX_ND)) return false;
	printf(_N(" yd_min OK\n"));
	return true;
}

ISR(PCINT2_vect)
{
	if (!((fsensor_int_pin_old ^ PINK) & FSENSOR_INT_PIN_MSK)) return;
	fsensor_int_pin_old = PINK;
	static bool _lock = false;
	if (_lock) return;
	_lock = true;
	int st_cnt = fsensor_st_cnt;
	fsensor_st_cnt = 0;
	sei();
	uint8_t old_err_cnt = fsensor_err_cnt;
	if (!pat9125_update_y())
	{
		printf_P(ERRMSG_PAT9125_NOT_RESP, 1);
		fsensor_disable();
		fsensor_not_responding = true;
	}
	if (st_cnt != 0)
	{ //movement
		if (st_cnt > 0) //positive movement
		{
			if (pat9125_y <= 0)
			{
				fsensor_err_cnt++;
			}
			else
			{
				if (fsensor_err_cnt)
					fsensor_err_cnt--;
			}
			if (fsensor_oq_meassure)
			{
				if (fsensor_oq_skipchunk)
					fsensor_oq_skipchunk--;
				else
				{
					if (st_cnt == FSENSOR_CHUNK_LEN)
					{
						if (fsensor_oq_yd_min > pat9125_y) fsensor_oq_yd_min = (fsensor_oq_yd_min + pat9125_y) / 2;
						if (fsensor_oq_yd_max < pat9125_y) fsensor_oq_yd_max = (fsensor_oq_yd_max + pat9125_y) / 2;
					}
					fsensor_oq_st_sum += st_cnt;
					fsensor_oq_yd_sum += pat9125_y;
					if (fsensor_err_cnt > old_err_cnt)
						fsensor_oq_er_sum += (fsensor_err_cnt - old_err_cnt);
					if (fsensor_oq_er_max < fsensor_err_cnt)
						fsensor_oq_er_max = fsensor_err_cnt;
				}
			}
		}
		else //negative movement
		{
		}
	}
	else
	{ //no movement
	}

#ifdef DEBUG_FSENSOR_LOG
	if (fsensor_log)
	{
		printf_P(_N("FSENSOR cnt=%d dy=%d err=%d %S\n"), st_cnt, pat9125_y, fsensor_err_cnt, (fsensor_err_cnt > old_err_cnt)?_N("NG!"):_N("OK"));
		printf_P(_N("FSENSOR st_sum=%lu yd_sum=%lu er_sum=%u er_max=%u\n"), fsensor_oq_st_sum, fsensor_oq_yd_sum, fsensor_oq_er_sum, fsensor_oq_er_max);
	}
#endif //DEBUG_FSENSOR_LOG

	pat9125_y = 0;
	_lock = false;
	return;
}

void fsensor_st_block_begin(block_t* bl)
{
	if (!fsensor_enabled) return;
	if (((fsensor_st_cnt > 0) && (bl->direction_bits & 0x8)) || 
		((fsensor_st_cnt < 0) && !(bl->direction_bits & 0x8)))
	{
		if (_READ(63)) _WRITE(63, LOW);
		else _WRITE(63, HIGH);
	}
//		PINK |= FSENSOR_INT_PIN_MSK; //toggle pin
//		_WRITE(fsensor_int_pin, LOW);
}

void fsensor_st_block_chunk(block_t* bl, int cnt)
{
	if (!fsensor_enabled) return;
	fsensor_st_cnt += (bl->direction_bits & 0x8)?-cnt:cnt;
	if ((fsensor_st_cnt >= fsensor_chunk_len) || (fsensor_st_cnt <= -fsensor_chunk_len))
	{
		if (_READ(63)) _WRITE(63, LOW);
		else _WRITE(63, HIGH);
	}
//		PINK |= FSENSOR_INT_PIN_MSK; //toggle pin
//		_WRITE(fsensor_int_pin, LOW);
}

void fsensor_update(void)
{
	if (fsensor_enabled && fsensor_watch_runout)
		if (fsensor_err_cnt > FSENSOR_ERR_MAX)
		{
			fsensor_stop_and_save_print();

			fsensor_err_cnt = 0;

			enquecommand_front_P((PSTR("G1 E-3 F200")));
			process_commands();
			cmdqueue_pop_front();
			st_synchronize();

			enquecommand_front_P((PSTR("G1 E3 F200")));
			process_commands();
			cmdqueue_pop_front();
			st_synchronize();

			if (fsensor_err_cnt == 0)
			{
				fsensor_restore_print_and_continue();
			}
			else
			{
				eeprom_update_byte((uint8_t*)EEPROM_FERROR_COUNT, eeprom_read_byte((uint8_t*)EEPROM_FERROR_COUNT) + 1);
				eeprom_update_word((uint16_t*)EEPROM_FERROR_COUNT_TOT, eeprom_read_word((uint16_t*)EEPROM_FERROR_COUNT_TOT) + 1);
				enquecommand_front_P((PSTR("M600")));
				fsensor_watch_runout = false;
			}
		}
}

void fsensor_setup_interrupt(void)
{
//	uint8_t fsensor_int_pin = FSENSOR_INT_PIN;
//	uint8_t fsensor_int_pcmsk = digitalPinToPCMSKbit(pin);
//	uint8_t fsensor_int_pcicr = digitalPinToPCICRbit(pin);

	pinMode(FSENSOR_INT_PIN, OUTPUT);
	digitalWrite(FSENSOR_INT_PIN, LOW);
	fsensor_int_pin_old = 0;

	pciSetup(FSENSOR_INT_PIN);
}


#endif //PAT9125
