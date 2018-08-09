//mmu.cpp

#include "mmu.h"
#include "planner.h"
#include "language.h"
#include "lcd.h"
#include "uart2.h"
#include "temperature.h"
#include "Configuration_prusa.h"


extern const char* lcd_display_message_fullscreen_P(const char *msg);
extern void lcd_return_to_status();

#define MMU_TIMEOUT 100

bool mmu_enabled = false;

uint8_t mmu_extruder = 0;

int8_t mmu_finda = -1;

int16_t mmu_version = -1;


//clear rx buffer
void mmu_clr_rx_buf(void)
{
	while (fgetc(uart2io) >= 0);
}

//send command - puts
int mmu_puts_P(const char* str)
{
	mmu_clr_rx_buf();                          //clear rx buffer
    return fputs_P(str, uart2io);              //send command
}

//send command - printf
int mmu_printf_P(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	mmu_clr_rx_buf();                          //clear rx buffer
	int r = vfprintf_P(uart2io, format, args); //send command
	va_end(args);
	return r;
}

//check 'ok' response
int8_t mmu_rx_ok(void)
{
	return uart2_rx_str_P(PSTR("ok\n"));
}

//check 'start' response
int8_t mmu_rx_start(void)
{
	return uart2_rx_str_P(PSTR("start\n"));
}

//initialize mmu_unit
bool mmu_init(void)
{
	uart2_init();                              //init uart2
	_delay_ms(10);                             //wait 10ms for sure
	if (mmu_reset())                           //reset mmu
	{
		mmu_read_finda();
		mmu_read_version();
		return true;
	}
	return false;
}

bool mmu_reset(void)
{
    mmu_puts_P(PSTR("X0\n"));                  //send command
	unsigned char timeout = 10;                //timeout = 10x100ms
	while ((mmu_rx_start() <= 0) && (--timeout))
		delay_keep_alive(MMU_TIMEOUT);
	mmu_enabled = timeout?true:false;
	return mmu_enabled;
}

int8_t mmu_read_finda(void)
{
    mmu_puts_P(PSTR("P0\n"));
	unsigned char timeout = 10;                //10x100ms
	while ((mmu_rx_ok() <= 0) && (--timeout))
		delay_keep_alive(MMU_TIMEOUT);
	mmu_finda = -1;
	if (timeout)
		fscanf_P(uart2io, PSTR("%hhu"), &mmu_finda);
	return mmu_finda;
}

int16_t mmu_read_version(void)
{
    mmu_puts_P(PSTR("S1\n"));
	unsigned char timeout = 10;                //10x100ms
	while ((mmu_rx_ok() <= 0) && (--timeout))
		delay_keep_alive(MMU_TIMEOUT);
	if (timeout)
		fscanf_P(uart2io, PSTR("%u"), &mmu_version);
	return mmu_version;
}

void extr_mov(float shift, float feed_rate)
{ //move extruder no matter what the current heater temperature is
	set_extrude_min_temp(.0);
	current_position[E_AXIS] += shift;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], feed_rate, active_extruder);
	set_extrude_min_temp(EXTRUDE_MINTEMP);
}


void change_extr(int
#ifdef SNMM
        extr
#endif //SNMM
        ) { //switches multiplexer for extruders
#ifdef SNMM
	st_synchronize();
	delay(100);

	disable_e0();
	disable_e1();
	disable_e2();

	mmu_extruder = extr;

	pinMode(E_MUX0_PIN, OUTPUT);
	pinMode(E_MUX1_PIN, OUTPUT);

	switch (extr) {
	case 1:
		WRITE(E_MUX0_PIN, HIGH);
		WRITE(E_MUX1_PIN, LOW);
		
		break;
	case 2:
		WRITE(E_MUX0_PIN, LOW);
		WRITE(E_MUX1_PIN, HIGH);
		
		break;
	case 3:
		WRITE(E_MUX0_PIN, HIGH);
		WRITE(E_MUX1_PIN, HIGH);
		
		break;
	default:
		WRITE(E_MUX0_PIN, LOW);
		WRITE(E_MUX1_PIN, LOW);
		
		break;
	}
	delay(100);
#endif
}

int get_ext_nr()
{ //reads multiplexer input pins and return current extruder number (counted from 0)
#ifndef SNMM
	return(mmu_extruder); //update needed
#else 
	return(2 * READ(E_MUX1_PIN) + READ(E_MUX0_PIN));
#endif
}


void display_loading()
{
	switch (mmu_extruder) 
	{
	case 1: lcd_display_message_fullscreen_P(_T(MSG_FILAMENT_LOADING_T1)); break;
	case 2: lcd_display_message_fullscreen_P(_T(MSG_FILAMENT_LOADING_T2)); break;
	case 3: lcd_display_message_fullscreen_P(_T(MSG_FILAMENT_LOADING_T3)); break;
	default: lcd_display_message_fullscreen_P(_T(MSG_FILAMENT_LOADING_T0)); break;
	}
}

void extr_adj(int extruder) //loading filament for SNMM
{
#ifndef SNMM
    printf_P(PSTR("L%d \n"),extruder);
    fprintf_P(uart2io, PSTR("L%d\n"), extruder);
	
	//show which filament is currently loaded
	
	lcd_update_enable(false);
	lcd_clear();
	lcd_set_cursor(0, 1); lcd_puts_P(_T(MSG_LOADING_FILAMENT));
	//if(strlen(_T(MSG_LOADING_FILAMENT))>18) lcd.setCursor(0, 1);
	//else lcd.print(" ");
	lcd_print(" ");
	lcd_print(mmu_extruder + 1);

	// get response
	manage_response(false, false);

	lcd_update_enable(true);
	
	
	//lcd_return_to_status();
#else

	bool correct;
	max_feedrate[E_AXIS] =80;
	//max_feedrate[E_AXIS] = 50;
	START:
	lcd_clear();
	lcd_set_cursor(0, 0); 
	switch (extruder) {
	case 1: lcd_display_message_fullscreen_P(_T(MSG_FILAMENT_LOADING_T1)); break;
	case 2: lcd_display_message_fullscreen_P(_T(MSG_FILAMENT_LOADING_T2)); break;
	case 3: lcd_display_message_fullscreen_P(_T(MSG_FILAMENT_LOADING_T3)); break;
	default: lcd_display_message_fullscreen_P(_T(MSG_FILAMENT_LOADING_T0)); break;   
	}
	KEEPALIVE_STATE(PAUSED_FOR_USER);
	do{
		extr_mov(0.001,1000);
		delay_keep_alive(2);
	} while (!lcd_clicked());
	//delay_keep_alive(500);
	KEEPALIVE_STATE(IN_HANDLER);
	st_synchronize();
	//correct = lcd_show_fullscreen_message_yes_no_and_wait_P(MSG_FIL_LOADED_CHECK, false);
	//if (!correct) goto	START;
	//extr_mov(BOWDEN_LENGTH/2.f, 500); //dividing by 2 is there because of max. extrusion length limitation (x_max + y_max)
	//extr_mov(BOWDEN_LENGTH/2.f, 500);
	extr_mov(bowden_length[extruder], 500);
	lcd_clear();
	lcd_set_cursor(0, 0); lcd_puts_P(_T(MSG_LOADING_FILAMENT));
	if(strlen(_T(MSG_LOADING_FILAMENT))>18) lcd_set_cursor(0, 1);
	else lcd_print(" ");
	lcd_print(mmu_extruder + 1);
	lcd_set_cursor(0, 2); lcd_puts_P(_T(MSG_PLEASE_WAIT));
	st_synchronize();
	max_feedrate[E_AXIS] = 50;
	lcd_update_enable(true);
	lcd_return_to_status();
	lcdDrawUpdate = 2;
#endif
}


void extr_unload()
{ //unload just current filament for multimaterial printers
#ifdef SNMM
	float tmp_motor[3] = DEFAULT_PWM_MOTOR_CURRENT;
	float tmp_motor_loud[3] = DEFAULT_PWM_MOTOR_CURRENT_LOUD;
	uint8_t SilentMode = eeprom_read_byte((uint8_t*)EEPROM_SILENT);
#endif

	if (degHotend0() > EXTRUDE_MINTEMP)
	{
#ifndef SNMM
		st_synchronize();
		
		//show which filament is currently unloaded
		lcd_update_enable(false);
		lcd_clear();
		lcd_set_cursor(0, 1); lcd_puts_P(_T(MSG_UNLOADING_FILAMENT));
		lcd_print(" ");
		lcd_print(mmu_extruder + 1);

		current_position[E_AXIS] -= 80;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 2500 / 60, active_extruder);
		st_synchronize();
		printf_P(PSTR("U0\n"));
		fprintf_P(uart2io, PSTR("U0\n"));

		// get response
		manage_response(false, true);

		lcd_update_enable(true);
#else //SNMM

		lcd_clear();
		lcd_display_message_fullscreen_P(PSTR(""));
		max_feedrate[E_AXIS] = 50;
		lcd_set_cursor(0, 0); lcd_puts_P(_T(MSG_UNLOADING_FILAMENT));
		lcd_print(" ");
		lcd_print(mmu_extruder + 1);
		lcd_set_cursor(0, 2); lcd_puts_P(_T(MSG_PLEASE_WAIT));
		if (current_position[Z_AXIS] < 15) {
			current_position[Z_AXIS] += 15; //lifting in Z direction to make space for extrusion
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 25, active_extruder);
		}
		
		current_position[E_AXIS] += 10; //extrusion
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 10, active_extruder);
		st_current_set(2, E_MOTOR_HIGH_CURRENT);
		if (current_temperature[0] < 230) { //PLA & all other filaments
			current_position[E_AXIS] += 5.4;
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 2800 / 60, active_extruder);
			current_position[E_AXIS] += 3.2;
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 3000 / 60, active_extruder);
			current_position[E_AXIS] += 3;
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 3400 / 60, active_extruder);
		}
		else { //ABS
			current_position[E_AXIS] += 3.1;
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 2000 / 60, active_extruder);
			current_position[E_AXIS] += 3.1;
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 2500 / 60, active_extruder);
			current_position[E_AXIS] += 4;
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 3000 / 60, active_extruder);
			/*current_position[X_AXIS] += 23; //delay
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 600 / 60, active_extruder); //delay
			current_position[X_AXIS] -= 23; //delay
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 600 / 60, active_extruder); //delay*/
			delay_keep_alive(4700);
		}
	
		max_feedrate[E_AXIS] = 80;
		current_position[E_AXIS] -= (bowden_length[mmu_extruder] + 60 + FIL_LOAD_LENGTH) / 2;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 500, active_extruder);
		current_position[E_AXIS] -= (bowden_length[mmu_extruder] + 60 + FIL_LOAD_LENGTH) / 2;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 500, active_extruder);
		st_synchronize();
		//st_current_init();
		if (SilentMode != SILENT_MODE_OFF) st_current_set(2, tmp_motor[2]); //set back to normal operation currents
		else st_current_set(2, tmp_motor_loud[2]);
		lcd_update_enable(true);
		lcd_return_to_status();
		max_feedrate[E_AXIS] = 50;
#endif //SNMM
	}
	else
	{
		lcd_clear();
		lcd_set_cursor(0, 0);
		lcd_puts_P(_T(MSG_ERROR));
		lcd_set_cursor(0, 2);
		lcd_puts_P(_T(MSG_PREHEAT_NOZZLE));
		delay(2000);
		lcd_clear();
	}
	//lcd_return_to_status();
}

//wrapper functions for loading filament
void extr_adj_0()
{
#ifndef SNMM
	enquecommand_P(PSTR("M701 E0"));
#else
	change_extr(0);
	extr_adj(0);
#endif
}

void extr_adj_1()
{
#ifndef SNMM
	enquecommand_P(PSTR("M701 E1"));
#else
	change_extr(1);
	extr_adj(1);
#endif
}

void extr_adj_2()
{
#ifndef SNMM
	enquecommand_P(PSTR("M701 E2"));
#else
	change_extr(2);
	extr_adj(2);
#endif
}

void extr_adj_3()
{
#ifndef SNMM
	enquecommand_P(PSTR("M701 E3"));
#else
	change_extr(3);
	extr_adj(3);
#endif
}

void extr_adj_4()
{
#ifndef SNMM
	enquecommand_P(PSTR("M701 E4"));
#else
	change_extr(4);
	extr_adj(4);
#endif
}

void load_all()
{
#ifndef SNMM
	enquecommand_P(PSTR("M701 E0"));
	enquecommand_P(PSTR("M701 E1"));
	enquecommand_P(PSTR("M701 E2"));
	enquecommand_P(PSTR("M701 E3"));
	enquecommand_P(PSTR("M701 E4"));
#else
	for (int i = 0; i < 4; i++)
	{
		change_extr(i);
		extr_adj(i);
	}
#endif
}

//wrapper functions for changing extruders
void extr_change_0()
{
	change_extr(0);
	lcd_return_to_status();
}

void extr_change_1()
{
	change_extr(1);
	lcd_return_to_status();
}

void extr_change_2()
{
	change_extr(2);
	lcd_return_to_status();
}

void extr_change_3()
{
	change_extr(3);
	lcd_return_to_status();
}

//wrapper functions for unloading filament
void extr_unload_all()
{
	if (degHotend0() > EXTRUDE_MINTEMP)
	{
		for (int i = 0; i < 4; i++)
		{
			change_extr(i);
			extr_unload();
		}
	}
	else
	{
		lcd_clear();
		lcd_set_cursor(0, 0);
		lcd_puts_P(_T(MSG_ERROR));
		lcd_set_cursor(0, 2);
		lcd_puts_P(_T(MSG_PREHEAT_NOZZLE));
		delay(2000);
		lcd_clear();
		lcd_return_to_status();
	}
}

//unloading just used filament (for snmm)
void extr_unload_used()
{
	if (degHotend0() > EXTRUDE_MINTEMP) {
		for (int i = 0; i < 4; i++) {
			if (snmm_filaments_used & (1 << i)) {
				change_extr(i);
				extr_unload();
			}
		}
		snmm_filaments_used = 0;
	}
	else {
		lcd_clear();
		lcd_set_cursor(0, 0);
		lcd_puts_P(_T(MSG_ERROR));
		lcd_set_cursor(0, 2);
		lcd_puts_P(_T(MSG_PREHEAT_NOZZLE));
		delay(2000);
		lcd_clear();
		lcd_return_to_status();
	}
}

void extr_unload_0()
{
	change_extr(0);
	extr_unload();
}

void extr_unload_1()
{
	change_extr(1);
	extr_unload();
}

void extr_unload_2()
{
	change_extr(2);
	extr_unload();
}

void extr_unload_3()
{
	change_extr(3);
	extr_unload();
}

void extr_unload_4()
{
	change_extr(4);
	extr_unload();
}
