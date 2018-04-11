#include "Marlin.h"

#ifdef PAT9125

#include "fsensor.h"
#include "pat9125.h"
#include "stepper.h"
#include "planner.h"
#include "fastio.h"
#include "cmdqueue.h"

//#include "LiquidCrystal_Prusa.h"
//extern LiquidCrystal_Prusa lcd;


#define FSENSOR_ERR_MAX          5  //filament sensor max error count
#define FSENSOR_INT_PIN         63  //filament sensor interrupt pin PK1
#define FSENSOR_INT_PIN_MSK   0x02  //filament sensor interrupt pin mask (bit1)
#define FSENSOR_CHUNK_LEN      280  //filament sensor chunk length in steps

extern void stop_and_save_print_to_ram(float z_move, float e_move);
extern void restore_print_from_ram_and_continue(float e_move);
extern int8_t FSensorStateMenu;

void fsensor_stop_and_save_print()
{
	stop_and_save_print_to_ram(0, 0); //XYZE - no change	
}

void fsensor_restore_print_and_continue()
{
	restore_print_from_ram_and_continue(0); //XYZ = orig, E - no change
}

//uint8_t fsensor_int_pin = FSENSOR_INT_PIN;
uint8_t fsensor_int_pin_old = 0;
int16_t fsensor_chunk_len = FSENSOR_CHUNK_LEN;
bool fsensor_enabled = true;
bool fsensor_not_responding = false;
//bool fsensor_ignore_error = true;
bool fsensor_M600 = false;
uint8_t fsensor_err_cnt = 0;
int16_t fsensor_st_cnt = 0;
uint8_t fsensor_log = 1;

//autoload enable/disable flag
bool fsensor_autoload_enabled = false;
uint16_t fsensor_autoload_y = 0;
uint8_t fsensor_autoload_c = 0;
uint32_t fsensor_autoload_last_millis = 0;
uint8_t fsensor_autoload_sum = 0;

void fsensor_block()
{
	fsensor_enabled = false;
}

void fsensor_unblock() {
	fsensor_enabled = (eeprom_read_byte((uint8_t*)EEPROM_FSENSOR) == 0x01);
}

bool fsensor_enable()
{
//	puts_P(PSTR("fsensor_enable\n"));
	int pat9125 = pat9125_init();
    printf_P(PSTR("PAT9125_init:%d\n"), pat9125);
	if (pat9125)
		fsensor_not_responding = false;
	else
		fsensor_not_responding = true;
	fsensor_enabled = pat9125?true:false;
//	fsensor_ignore_error = true;
	fsensor_M600 = false;
	fsensor_err_cnt = 0;
	eeprom_update_byte((uint8_t*)EEPROM_FSENSOR, fsensor_enabled?0x01:0x00); 
	FSensorStateMenu = fsensor_enabled?1:0;
//	printf_P(PSTR("fsensor_enable - end %d\n"), fsensor_enabled?1:0);
	return fsensor_enabled;
}

void fsensor_disable()
{
//	puts_P(PSTR("fsensor_disable\n"));
	fsensor_enabled = false;
	eeprom_update_byte((uint8_t*)EEPROM_FSENSOR, 0x00); 
	FSensorStateMenu = 0;
}

void pciSetup(byte pin)
{
	*digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin)); // enable pin
	PCIFR |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
	PCICR |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group 
}

void fsensor_setup_interrupt()
{
//	uint8_t fsensor_int_pin = FSENSOR_INT_PIN;
//	uint8_t fsensor_int_pcmsk = digitalPinToPCMSKbit(pin);
//	uint8_t fsensor_int_pcicr = digitalPinToPCICRbit(pin);

	pinMode(FSENSOR_INT_PIN, OUTPUT);
	digitalWrite(FSENSOR_INT_PIN, LOW);
	fsensor_int_pin_old = 0;

	pciSetup(FSENSOR_INT_PIN);
}

void fsensor_autoload_check_start(void)
{
//	puts_P(PSTR("fsensor_autoload_check_start\n"));
	if (!pat9125_update_y()) //update sensor
	{
		puts_P(PSTR("pat9125 not responding (3).\n"));
		fsensor_disable();
		fsensor_not_responding = true;
		fsensor_autoload_enabled = false;
		return;
	}
	fsensor_autoload_y = pat9125_y; //save current y value
	fsensor_autoload_c = 0; //reset number of changes counter
	fsensor_autoload_sum = 0;
	fsensor_autoload_last_millis = millis();
	fsensor_autoload_enabled = true;
	fsensor_err_cnt = 0;
}

void fsensor_autoload_check_stop(void)
{
//	puts_P(PSTR("fsensor_autoload_check_stop\n"));
	fsensor_autoload_sum = 0;
	fsensor_autoload_enabled = false;
	fsensor_err_cnt = 0;
}

bool fsensor_check_autoload(void)
{
	uint8_t fsensor_autoload_c_old = fsensor_autoload_c;
	if ((millis() - fsensor_autoload_last_millis) < 25) return false;
	fsensor_autoload_last_millis = millis();
	if (!pat9125_update_y())
	{
		puts_P(PSTR("pat9125 not responding (2).\n"));
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

ISR(PCINT2_vect)
{
//	puts("PCINT2\n");
	if (!((fsensor_int_pin_old ^ PINK) & FSENSOR_INT_PIN_MSK)) return;
	fsensor_int_pin_old = PINK;
	static bool _lock = false;
	if (_lock) return;
	_lock = true;
//	return;
	int st_cnt = fsensor_st_cnt;
	fsensor_st_cnt = 0;
	sei();
/*	*digitalPinToPCMSK(fsensor_int_pin) &= ~bit(digitalPinToPCMSKbit(fsensor_int_pin));
	digitalWrite(fsensor_int_pin, HIGH);
	*digitalPinToPCMSK(fsensor_int_pin) |= bit(digitalPinToPCMSKbit(fsensor_int_pin));*/
	if (!pat9125_update_y())
	{
//#ifdef DEBUG_FSENSOR_LOG
		puts_P(PSTR("pat9125 not responding (1).\n"));
//#endif //DEBUG_FSENSOR_LOG
		fsensor_disable();
		fsensor_not_responding = true;
	}
	if (st_cnt != 0)
	{
#ifdef DEBUG_FSENSOR_LOG
		if (fsensor_log)
		{
			MYSERIAL.print("cnt=");
			MYSERIAL.print(st_cnt, DEC);
			MYSERIAL.print(" dy=");
			MYSERIAL.print(pat9125_y, DEC);
		}
#endif //DEBUG_FSENSOR_LOG
		if (st_cnt != 0)
		{
			if( (pat9125_y == 0) || ((pat9125_y > 0) && (st_cnt < 0)) || ((pat9125_y < 0) && (st_cnt > 0)))
			{ //invalid movement
				if (st_cnt > 0) //only positive movements
					fsensor_err_cnt++;
#ifdef DEBUG_FSENSOR_LOG
			if (fsensor_log)
			{
				MYSERIAL.print("\tNG ! err=");
				MYSERIAL.println(fsensor_err_cnt, DEC);
			}
#endif //DEBUG_FSENSOR_LOG
			}
			else
			{ //propper movement
				if (fsensor_err_cnt > 0)
					fsensor_err_cnt--;
//					fsensor_err_cnt = 0;
#ifdef DEBUG_FSENSOR_LOG
				if (fsensor_log)
				{
					MYSERIAL.print("\tOK    err=");
					MYSERIAL.println(fsensor_err_cnt, DEC);
				}
#endif //DEBUG_FSENSOR_LOG
			}
		}
		else
		{ //no movement
#ifdef DEBUG_FSENSOR_LOG
		if (fsensor_log)
			MYSERIAL.println("\tOK 0");
#endif //DEBUG_FSENSOR_LOG
		}
	}
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

void fsensor_update()
{
	if (!fsensor_enabled) return;
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
			fsensor_M600 = true;
			fsensor_enabled = false;
		}
	}
}

#endif //PAT9125
