#include "Marlin.h"

#ifdef PAT9125

#include "fsensor.h"
#include "pat9125.h"
#include "planner.h"

extern void stop_and_save_print_to_ram(float z_move, float e_move);
extern void restore_print_from_ram_and_continue(float e_move);
extern long st_get_position(uint8_t axis);


void fsensor_stop_and_save_print()
{
//	stop_and_save_print_to_ram(10, -0.8); //XY - no change, Z 10mm up, E 0.8mm in
	stop_and_save_print_to_ram(0, 0); //XYZE - no change
}

void fsensor_restore_print_and_continue()
{
	restore_print_from_ram_and_continue(0); //XYZ = orig, E - no change
}


bool fsensor_enabled = true;
bool fsensor_ignore_error = true;
bool fsensor_M600 = false;
long fsensor_prev_pos_e = 0;
uint8_t fsensor_err_cnt = 0;

#define FSENS_ESTEPS 280  //extruder resolution [steps/mm]
//#define FSENS_MINDEL 560  //filament sensor min delta [steps] (3mm)
#define FSENS_MINDEL 280  //filament sensor min delta [steps] (3mm)
#define FSENS_MINFAC 3    //filament sensor minimum factor [count/mm]
//#define FSENS_MAXFAC 50   //filament sensor maximum factor [count/mm]
#define FSENS_MAXFAC 40   //filament sensor maximum factor [count/mm]
//#define FSENS_MAXERR 2    //filament sensor max error count
#define FSENS_MAXERR 5    //filament sensor max error count

extern int8_t FSensorStateMenu;


void fsensor_enable()
{
	MYSERIAL.println("fsensor_enable");
	pat9125_y = 0;
	fsensor_prev_pos_e = st_get_position(E_AXIS);
	fsensor_err_cnt = 0;
	fsensor_enabled = true;
	fsensor_ignore_error = true;
	fsensor_M600 = false;
	eeprom_update_byte((uint8_t*)EEPROM_FSENSOR, 0xFF); 
	FSensorStateMenu = 1;
}

void fsensor_disable()
{
	MYSERIAL.println("fsensor_disable");
	fsensor_enabled = false;
	eeprom_update_byte((uint8_t*)EEPROM_FSENSOR, 0x00); 
	FSensorStateMenu = 0;
}

#include "LiquidCrystal.h"
extern LiquidCrystal lcd;

//bool pat9125_change = ((old_x != pat9125_x) || (old_y != pat9125_y));

uint8_t fsensor_int_pin = 63;
int16_t fsensor_steps_e = 0;
int16_t fsensor_y_old = 0;



void pciSetup(byte pin) 
{
	*digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin)); // enable pin
	PCIFR |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
	PCICR |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group 
}

void fsensor_setup_interrupt()
{
	uint8_t fsensor_int_pin = 63;

	pinMode(fsensor_int_pin, OUTPUT);
	digitalWrite(fsensor_int_pin, HIGH); 

	pciSetup(fsensor_int_pin);
}

void fsensor_interrupt_raise()
{
	digitalWrite(fsensor_int_pin, LOW);
}

ISR(PCINT2_vect)
{
	sei();
	SERIAL_ECHOLNPGM("PCINT2");
	*digitalPinToPCMSK(fsensor_int_pin) &= ~bit(digitalPinToPCMSKbit(fsensor_int_pin));
	digitalWrite(fsensor_int_pin, HIGH);
	*digitalPinToPCMSK(fsensor_int_pin) |= bit(digitalPinToPCMSKbit(fsensor_int_pin));
//	PCIFR |= bit (digitalPinToPCICRbit(fsensor_int_pin)); // clear any outstanding interrupt
//	pat9125_update();
	pat9125_update();
	MYSERIAL.print("steps_e=");
	MYSERIAL.print(fsensor_steps_e, DEC);
	MYSERIAL.print(" dy=");
	MYSERIAL.println(pat9125_y - fsensor_y_old, DEC);
	fsensor_y_old = pat9125_y;
}

void fsensor_st_end_block(block_t* bl)
{
//	return;
	fsensor_steps_e = bl->steps_e;
	digitalWrite(fsensor_int_pin, LOW);
}

void fsensor_st_new_block(block_t* bl)
{
//	return;
//	fsensor_steps_e = bl->steps_e;
//	digitalWrite(fsensor_int_pin, LOW);
}

void fsensor_update()
{
//	return;
	if (!fsensor_enabled) return;
	long pos_e = st_get_position(E_AXIS); //current position
	int old_x = pat9125_x;
	int old_y = pat9125_y;
	pat9125_update();
/*	bool pat9125_change = ((old_x != pat9125_x) || (old_y != pat9125_y));
	static uint32_t checktime = 0;
	pat9125_change |= (millis() - checktime > 250);
#ifdef DEBUG_PAT9125_COUNTERS
	if (pat9125_change)
	{
		lcd.setCursor(2, 3);
		lcd.print(pat9125_x, DEC);
		lcd.print(' ');
		lcd.print(pat9125_y, DEC);
		lcd.print(' ');
		lcd.print(pos_e, DEC);
		lcd.print(' ');
		checktime = millis();
	}
#endif DEBUG_PAT9125_COUNTERS
	return;*/

	long del_e = pos_e - fsensor_prev_pos_e; //delta
	if (abs(del_e) < FSENS_MINDEL) return;
	float de = ((float)del_e / FSENS_ESTEPS);
	int cmin = de * FSENS_MINFAC;
	int cmax = de * FSENS_MAXFAC;
	int cnt = -pat9125_y;
	fsensor_prev_pos_e = pos_e;
	pat9125_y = 0;
	bool err = false;
	if ((del_e > 0) && ((cnt < cmin) || (cnt > cmax))) err = true;
	if ((del_e < 0) && ((cnt > cmin) || (cnt < cmax))) err = true;
	if (err)
		fsensor_err_cnt++;
	else
		fsensor_err_cnt = 0;

/**/
	MYSERIAL.print("pos_e=");
	MYSERIAL.print(pos_e);
	MYSERIAL.print(" de=");
	MYSERIAL.print(de);
	MYSERIAL.print(" cmin=");
	MYSERIAL.print((int)cmin);
	MYSERIAL.print(" cmax=");
	MYSERIAL.print((int)cmax);
	MYSERIAL.print(" cnt=");
	MYSERIAL.print((int)cnt);
	MYSERIAL.print(" err=");
	MYSERIAL.println((int)fsensor_err_cnt);/**/

//	return;

	if (fsensor_err_cnt > FSENS_MAXERR)
	{
		MYSERIAL.println("fsensor_update (fsensor_err_cnt > FSENS_MAXERR)");
		if (fsensor_ignore_error)
		{
			MYSERIAL.println("fsensor_update - error ignored)");
			fsensor_ignore_error = false;
		}
		else
		{
			MYSERIAL.println("fsensor_update - ERROR!!!");
			fsensor_stop_and_save_print();
			enquecommand_front_P((PSTR("M600")));
			fsensor_M600 = true;
			fsensor_enabled = false;
		}
	}
}

#endif //PAT9125
