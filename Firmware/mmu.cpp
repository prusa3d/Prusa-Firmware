//! @file

#include "mmu.h"
#include "planner.h"
#include "language.h"
#include "lcd.h"
#include "uart2.h"
#include "temperature.h"
#include "Configuration_prusa.h"
#include "fsensor.h"
#include "cardreader.h"
#include "cmdqueue.h"
#include "stepper.h"
#include "ultralcd.h"
#include "menu.h"
#include "sound.h"
#include "printers.h"
#include <avr/pgmspace.h>
#include "AutoDeplete.h"
#include "fastio.h"
#include "pins.h"
//-//
#include "util.h"

#ifdef TMC2130
#include "tmc2130.h"
#endif //TMC2130

#define MMU_TODELAY 100
#define MMU_TIMEOUT 10
#define MMU_CMD_TIMEOUT 45000ul //45s timeout for mmu commands (except P0)
#define MMU_P0_TIMEOUT 3000ul //timeout for P0 command: 3seconds
#define MMU_MAX_RESEND_ATTEMPTS 2


namespace
{
    enum class S : uint_least8_t
    {
        WaitStealthMode,
        GetFindaInit,
        GetBuildNr,
        GetVersion,
        Init,
        Disabled,
        Idle,
        GetFinda,
        WaitCmd, //!< wait for command response
        Pause,
        GetDrvError, //!< get power failures count
		SwitchMode //switch mmu between stealth and normal mode 
    };
}

bool mmu_enabled = false;
bool mmu_ready = false;
bool mmu_fil_loaded = false; //if true: blocks execution of duplicit T-codes

static S mmu_state = S::Disabled;

MmuCmd mmu_cmd = MmuCmd::None;

//idler ir sensor
static uint8_t mmu_idl_sens = 0;
bool ir_sensor_detected = false; 
static bool mmu_loading_flag = false; //when set to true, we assume that mmu2 unload was finished and loading phase is now performed; printer can send 'A' to mmu2 to abort loading process

uint8_t mmu_extruder = MMU_FILAMENT_UNKNOWN;

//! This variable probably has no meaning and is planed to be removed
uint8_t tmp_extruder = MMU_FILAMENT_UNKNOWN;

int8_t mmu_finda = -1;

int16_t mmu_version = -1;

int16_t mmu_buildnr = -1;

uint32_t mmu_last_request;
uint32_t mmu_last_response;
uint32_t mmu_last_finda_response;
//LongTimer mmu_last_request;
//LongTimer mmu_last_response;
//LongTimer mmu_last_finda_response;

MmuCmd mmu_last_cmd = MmuCmd::None;
uint16_t mmu_power_failures = 0;


#ifdef MMU_DEBUG
static const auto DEBUG_PUTCHAR = putchar;
static const auto DEBUG_PUTS_P = puts_P;
static const auto DEBUG_PRINTF_P = printf_P;
#else //MMU_DEBUG
#define DEBUG_PUTCHAR(c)
#define DEBUG_PUTS_P(str)
#define DEBUG_PRINTF_P( __fmt, ... )
#endif //MMU_DEBUG

#if defined(MMU_FINDA_DEBUG) && defined(MMU_DEBUG)
static const auto FDEBUG_PUTS_P = puts_P;
static const auto FDEBUG_PRINTF_P = printf_P;
#else
#define FDEBUG_PUTS_P(str)
#define FDEBUG_PRINTF_P( __fmt, ... )
#endif //defined(MMU_FINDA_DEBUG) && defined(MMU_DEBUG)


//clear rx buffer
void mmu_clr_rx_buf(void)
{
	while (fgetc(uart2io) >= 0);
}

//send command - puts
int mmu_puts_P(const char* str)
{
	mmu_clr_rx_buf();                          //clear rx buffer
    int r = fputs_P(str, uart2io);             //send command
	mmu_last_request = _millis();
	//mmu_last_request.start();
	return r;
}

//send command - printf
int mmu_printf_P(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	mmu_clr_rx_buf();                          //clear rx buffer
	int r = vfprintf_P(uart2io, format, args); //send command
	va_end(args);
	mmu_last_request = _millis();
	//mmu_last_request.start();
	return r;
}

//check 'ok' response
int8_t mmu_rx_ok(void)
{
	int8_t res = uart2_rx_str_P(PSTR("ok\n"));
	if (res == 1) mmu_last_response = _millis();
	//if (res == 1) mmu_last_response.start();
	return res;
}

//check 'start' response
int8_t mmu_rx_start(void)
{
	int8_t res = uart2_rx_str_P(PSTR("start\n"));
	if (res == 1) mmu_last_response = _millis();
	//if (res == 1) mmu_last_response.start();
	return res;
}

//initialize mmu2 unit - first part - should be done at begining of startup process
void mmu_init(void)
{
#ifdef MMU_HWRESET
	WRITE(MMU_RST_PIN, 1);
	SET_OUTPUT(MMU_RST_PIN);                   //setup reset pin
#endif //MMU_HWRESET
	uart2_init();                              //init uart2
	_delay_ms(10);                             //wait 10ms for sure
	mmu_reset();                               //reset mmu (HW or SW), do not wait for response
	mmu_state = S::Init;
	SET_INPUT(IR_SENSOR_PIN); //input mode
	WRITE(IR_SENSOR_PIN, 1); //pullup
}

//if IR_SENSOR defined, always returns true
//otherwise check for ir sensor and returns true if idler IR sensor was detected, otherwise returns false
bool check_for_ir_sensor() 
{
#ifdef IR_SENSOR
	return true;
#else //IR_SENSOR

	bool detected = false;
	//if IR_SENSOR_PIN input is low and pat9125sensor is not present we detected idler sensor
	if ((READ(IR_SENSOR_PIN) == 0) 
#ifdef PAT9125
		&& fsensor_not_responding
#endif //PAT9125
	) 
	{		
		detected = true;
		//printf_P(PSTR("Idler IR sensor detected\n"));
	}
	else
	{
		//printf_P(PSTR("Idler IR sensor not detected\n"));
	}
	return detected;
#endif //IR_SENSOR
}

static bool activate_stealth_mode()
{
#ifdef MMU_FORCE_STEALTH_MODE
	return true;
#else
	return (eeprom_read_byte((uint8_t*)EEPROM_MMU_STEALTH) == 1);
#endif
}

//mmu main loop - state machine processing
void mmu_loop(void)
{
	static uint8_t mmu_attempt_nr = 0;
//	printf_P(PSTR("MMU loop, state=%d\n"), (int)mmu_state);
	switch (mmu_state)
	{
	case S::Disabled: //state 0
		return;
	case S::Init: //state 1
		if (mmu_rx_start() > 0)
		{
		    DEBUG_PUTS_P(PSTR("MMU => 'start'"));
		    DEBUG_PUTS_P(PSTR("MMU <= 'S1'"));
		    mmu_puts_P(PSTR("S1\n")); //send 'read version' request
			mmu_state = S::GetVersion;
		}
		else if (_millis() > 30000) //30sec after reset disable mmu
		{
			puts_P(PSTR("MMU not responding - DISABLED"));
			mmu_state = S::Disabled;
		}
		return;
	case S::GetVersion: //state 2
		if (mmu_rx_ok() > 0)
		{
			fscanf_P(uart2io, PSTR("%u"), &mmu_version); //scan version from buffer
			DEBUG_PRINTF_P(PSTR("MMU => '%dok'\n"), mmu_version);
			DEBUG_PUTS_P(PSTR("MMU <= 'S2'"));
			mmu_puts_P(PSTR("S2\n")); //send 'read buildnr' request
			mmu_state = S::GetBuildNr;
		}
		return;
	case S::GetBuildNr: //state 3
		if (mmu_rx_ok() > 0)
		{
			fscanf_P(uart2io, PSTR("%u"), &mmu_buildnr); //scan buildnr from buffer
			DEBUG_PRINTF_P(PSTR("MMU => '%dok'\n"), mmu_buildnr);
			bool version_valid = mmu_check_version();
			if (!version_valid) mmu_show_warning();
			else puts_P(PSTR("MMU version valid"));
			
			if (!activate_stealth_mode())
			{
				FDEBUG_PUTS_P(PSTR("MMU <= 'P0'"));
				mmu_puts_P(PSTR("P0\n")); //send 'read finda' request
				mmu_state = S::GetFindaInit;
			}
			else
			{
				DEBUG_PUTS_P(PSTR("MMU <= 'M1'"));
				mmu_puts_P(PSTR("M1\n")); //set mmu mode to stealth
				mmu_state = S::WaitStealthMode;
			}

		}
		return;
	case S::WaitStealthMode: //state 4
		if (mmu_rx_ok() > 0)
		{
			FDEBUG_PUTS_P(PSTR("MMU <= 'P0'"));
		    mmu_puts_P(PSTR("P0\n")); //send 'read finda' request
			mmu_state = S::GetFindaInit;
		}
		return;
	case S::GetFindaInit: //state 5
		if (mmu_rx_ok() > 0)
		{
			fscanf_P(uart2io, PSTR("%hhu"), &mmu_finda); //scan finda from buffer. MUST BE %hhu!!!
			mmu_last_finda_response = _millis();
			//mmu_last_finda_response.start();
			FDEBUG_PRINTF_P(PSTR("MMU => '%dok'\n"), mmu_finda);
			puts_P(PSTR("MMU - ENABLED"));
			mmu_enabled = true;
            //-//
            // ... PrinterType/Name
            fSetMmuMode(true);
			mmu_state = S::Idle;
		}
		return;
	case S::Idle: //state 6
		if (mmu_cmd != MmuCmd::None) //command request ?
		{
			if ((mmu_cmd >= MmuCmd::T0) && (mmu_cmd <= MmuCmd::T4))
			{
				const uint8_t filament = mmu_cmd - MmuCmd::T0;
				DEBUG_PRINTF_P(PSTR("MMU <= 'T%d'\n"), filament);
				mmu_printf_P(PSTR("T%d\n"), filament);
				mmu_state = S::WaitCmd; // wait for response
				mmu_fil_loaded = true;
				mmu_idl_sens = 1;
			}
			else if ((mmu_cmd >= MmuCmd::L0) && (mmu_cmd <= MmuCmd::L4))
			{
			    const uint8_t filament = mmu_cmd - MmuCmd::L0;
			    DEBUG_PRINTF_P(PSTR("MMU <= 'L%d'\n"), filament);
			    mmu_printf_P(PSTR("L%d\n"), filament);
			    mmu_state = S::WaitCmd; // wait for response
			}
			else if (mmu_cmd == MmuCmd::C0)
			{
			    DEBUG_PRINTF_P(PSTR("MMU <= 'C0'\n"));
				mmu_puts_P(PSTR("C0\n")); //send 'continue loading'
				mmu_state = S::WaitCmd;
				mmu_idl_sens = 1;
			}
			else if (mmu_cmd == MmuCmd::U0)
			{
			    DEBUG_PRINTF_P(PSTR("MMU <= 'U0'\n"));
				mmu_puts_P(PSTR("U0\n")); //send 'unload current filament'
				mmu_fil_loaded = false;
				mmu_state = S::WaitCmd;
			}
			else if ((mmu_cmd >= MmuCmd::E0) && (mmu_cmd <= MmuCmd::E4))
			{
			    const uint8_t filament = mmu_cmd - MmuCmd::E0;
				DEBUG_PRINTF_P(PSTR("MMU <= 'E%d'\n"), filament);
				mmu_printf_P(PSTR("E%d\n"), filament); //send eject filament
				mmu_fil_loaded = false;
				mmu_state = S::WaitCmd;
			}
			else if ((mmu_cmd >= MmuCmd::K0) && (mmu_cmd <= MmuCmd::K4))
            {
                const uint8_t filament = mmu_cmd - MmuCmd::K0;
                DEBUG_PRINTF_P(PSTR("MMU <= 'K%d'\n"), filament);
                mmu_printf_P(PSTR("K%d\n"), filament); //send eject filament
                mmu_fil_loaded = false;
                mmu_state = S::WaitCmd;
            }
			else if (mmu_cmd == MmuCmd::R0)
			{
			    DEBUG_PRINTF_P(PSTR("MMU <= 'R0'\n"));
				mmu_puts_P(PSTR("R0\n")); //send recover after eject
				mmu_state = S::WaitCmd;
			}
			else if (mmu_cmd == MmuCmd::S3)
			{
			    DEBUG_PRINTF_P(PSTR("MMU <= 'S3'\n"));
				mmu_puts_P(PSTR("S3\n")); //send power failures request
				mmu_state = S::GetDrvError;
			}
			else if (mmu_cmd == MmuCmd::W0)
			{
			    DEBUG_PRINTF_P(PSTR("MMU <= 'W0'\n"));
			    mmu_puts_P(PSTR("W0\n"));
			    mmu_state = S::Pause;
			}
			mmu_last_cmd = mmu_cmd;
			mmu_cmd = MmuCmd::None;
		}
		else if ((eeprom_read_byte((uint8_t*)EEPROM_MMU_STEALTH) != SilentModeMenu_MMU) && mmu_ready) {
				DEBUG_PRINTF_P(PSTR("MMU <= 'M%d'\n"), SilentModeMenu_MMU);
				mmu_printf_P(PSTR("M%d\n"), SilentModeMenu_MMU);
				mmu_state = S::SwitchMode;
		}
		else if ((mmu_last_response + 300) < _millis()) //request every 300ms
		//else if (mmu_last_response.expired(300)) //request every 300ms
		{
#ifndef IR_SENSOR
			if(check_for_ir_sensor()) ir_sensor_detected = true;
#endif //IR_SENSOR not defined
			FDEBUG_PUTS_P(PSTR("MMU <= 'P0'"));
		    mmu_puts_P(PSTR("P0\n")); //send 'read finda' request
			mmu_state = S::GetFinda;
		}
		//printf_P(PSTR("MMU loop, state %d\n"), (int)mmu_state);
		//printf_P(PSTR("mmu_last_response %lu\n"), mmu_last_response.elapsed());
		//printf_P(PSTR("mmu_last_finda_response %lu\n"), mmu_last_finda_response.elapsed());
		return;
	case S::GetFinda: //response to command P0 state 7
        if (mmu_idl_sens)
        {
            if (READ(IR_SENSOR_PIN) == 0 && mmu_loading_flag)
            {
#ifdef MMU_DEBUG
                printf_P(PSTR("MMU <= 'A'\n"));
#endif //MMU_DEBUG  
                mmu_puts_P(PSTR("A\n")); //send 'abort' request
                mmu_idl_sens = 0;
                //printf_P(PSTR("MMU IDLER_SENSOR = 0 - ABORT\n"));
            }
            //else
                //printf_P(PSTR("MMU IDLER_SENSOR = 1 - WAIT\n"));
        }
		if (mmu_rx_ok() > 0)
		{
			fscanf_P(uart2io, PSTR("%hhu"), &mmu_finda); //scan finda from buffer. MUST BE %hhu!!!
			mmu_last_finda_response = _millis();
			//mmu_last_finda_response.start();
			FDEBUG_PRINTF_P(PSTR("MMU => '%dok'\n"), mmu_finda);
			//printf_P(PSTR("Eact: %d\n"), int(e_active()));
			if (!mmu_finda && CHECK_FSENSOR && fsensor_enabled) {
				fsensor_checkpoint_print();
				if (mmu_extruder != MMU_FILAMENT_UNKNOWN) // Can't deplete unknown extruder.
                    ad_markDepleted(mmu_extruder);
				if (lcd_autoDepleteEnabled() && !ad_allDepleted() && mmu_extruder != MMU_FILAMENT_UNKNOWN) // Can't auto if F=?
				{
				    enquecommand_front_P(PSTR("M600 AUTO")); //save print and run M600 command
				}
				else
				{
				    enquecommand_front_P(PSTR("M600")); //save print and run M600 command
				}
			}
			mmu_state = S::Idle;
			if (mmu_cmd == MmuCmd::None)
				mmu_ready = true;
		}
		else if ((mmu_last_request + MMU_P0_TIMEOUT) < _millis())
		//else if (mmu_last_request.expired(MMU_P0_TIMEOUT))
		{ //resend request after timeout (30s)
			mmu_state = S::Idle;
		}
		return;
	case S::WaitCmd: //response to mmu commands state 8
        if (mmu_idl_sens)
        {
            if (READ(IR_SENSOR_PIN) == 0 && mmu_loading_flag)
            {
                DEBUG_PRINTF_P(PSTR("MMU <= 'A'\n"));
                mmu_puts_P(PSTR("A\n")); //send 'abort' request
                mmu_idl_sens = 0;
                //printf_P(PSTR("MMU IDLER_SENSOR = 0 - ABORT\n"));
            }
            //else
                //printf_P(PSTR("MMU IDLER_SENSOR = 1 - WAIT\n"));
        }
		if (mmu_rx_ok() > 0)
		{
		    DEBUG_PRINTF_P(PSTR("MMU => 'ok'\n"));
			mmu_attempt_nr = 0;
			mmu_last_cmd = MmuCmd::None;
			mmu_ready = true;
			mmu_state = S::Idle;
		}
		else if ((mmu_last_request + MMU_CMD_TIMEOUT) < _millis())
		//else if (mmu_last_request.expired(MMU_CMD_TIMEOUT))
		{ //resend request after timeout (5 min)
			if (mmu_last_cmd != MmuCmd::None)
			{
				if (mmu_attempt_nr++ < MMU_MAX_RESEND_ATTEMPTS &&
				    mmu_last_cmd >= MmuCmd::T0 && mmu_last_cmd <= MmuCmd::T4)
				{
				    DEBUG_PRINTF_P(PSTR("MMU retry attempt nr. %d\n"), mmu_attempt_nr - 1);
					mmu_cmd = mmu_last_cmd;
				}
				else {
					mmu_cmd = MmuCmd::None;
					mmu_last_cmd = MmuCmd::None; //check
					mmu_attempt_nr = 0;
				}
			}
			mmu_state = S::Idle;
		}
		return;
	case S::Pause: //state 9
        if (mmu_rx_ok() > 0)
        {
            DEBUG_PRINTF_P(PSTR("MMU => 'ok', resume print\n"));
            mmu_attempt_nr = 0;
            mmu_last_cmd = MmuCmd::None;
            mmu_ready = true;
            mmu_state = S::Idle;
            lcd_resume_print();
        }
        if (mmu_cmd != MmuCmd::None)
        {
            mmu_state = S::Idle;
        }
	    return;
	case S::GetDrvError: //state 10
		if (mmu_rx_ok() > 0)
		{
			fscanf_P(uart2io, PSTR("%d"), &mmu_power_failures); //scan power failures
			DEBUG_PRINTF_P(PSTR("MMU => 'ok'\n"));
			mmu_last_cmd = MmuCmd::None;
			mmu_ready = true;
			mmu_state = S::Idle;
		}
		else if ((mmu_last_request + MMU_CMD_TIMEOUT) < _millis())
		//else if (mmu_last_request.expired(MMU_CMD_TIMEOUT))
		{ //timeout 45 s
			mmu_state = S::Idle;
		}
		return;
	case S::SwitchMode: //state 11
		if (mmu_rx_ok() > 0)
		{
			DEBUG_PRINTF_P(PSTR("MMU => 'ok'\n"));
			eeprom_update_byte((uint8_t*)EEPROM_MMU_STEALTH, SilentModeMenu_MMU);
			mmu_state = S::Idle;
		}
		else if ((mmu_last_request + MMU_CMD_TIMEOUT) < _millis())
		//else if (mmu_last_request.expired(MMU_CMD_TIMEOUT))
		{ //timeout 45 s
			mmu_state = S::Idle;
		}
		return;		
	}
}

void mmu_reset(void)
{
#ifdef MMU_HWRESET                             //HW - pulse reset pin
	WRITE(MMU_RST_PIN, 0);
	_delay_us(100);
	WRITE(MMU_RST_PIN, 1);
#else                                          //SW - send X0 command
    mmu_puts_P(PSTR("X0\n"));
#endif
}

int8_t mmu_set_filament_type(uint8_t extruder, uint8_t filament)
{
	printf_P(PSTR("MMU <= 'F%d %d'\n"), extruder, filament);
	mmu_printf_P(PSTR("F%d %d\n"), extruder, filament);
	unsigned char timeout = MMU_TIMEOUT;       //10x100ms
	while ((mmu_rx_ok() <= 0) && (--timeout))
		delay_keep_alive(MMU_TODELAY);
	return timeout?1:0;
}

//! @brief Enqueue MMUv2 command
//!
//! Call manage_response() after enqueuing to process command.
//! If T command is enqueued, it disables current for extruder motor if TMC2130 driver present.
//! If T or L command is enqueued, it marks filament loaded in AutoDeplete module.
void mmu_command(MmuCmd cmd)
{
	if ((cmd >= MmuCmd::T0) && (cmd <= MmuCmd::T4))
	{
		//disable extruder motor
#ifdef TMC2130
		tmc2130_set_pwr(E_AXIS, 0);
#endif //TMC2130
		//printf_P(PSTR("E-axis disabled\n"));
		ad_markLoaded(cmd - MmuCmd::T0);
	}
    if ((cmd >= MmuCmd::L0) && (cmd <= MmuCmd::L4))
    {
        ad_markLoaded(cmd - MmuCmd::L0);
    }

	mmu_cmd = cmd;
	mmu_ready = false;
}

//! @brief Rotate extruder idler to catch filament
//! @par synchronize
//!  * true blocking call
//!  * false non-blocking call
void mmu_load_step(bool synchronize)
{
		current_position[E_AXIS] = current_position[E_AXIS] + MMU_LOAD_FEEDRATE * 0.1;
		plan_buffer_line_curposXYZE(MMU_LOAD_FEEDRATE);
		if (synchronize) st_synchronize();
}

//! @brief Is nozzle hot enough to move extruder wheels and do we have idler sensor?
//!
//! Do load steps only if temperature is higher then min. temp for safe extrusion and
//! idler sensor present.
//! Otherwise "cold extrusion prevented" would be send to serial line periodically
//! and watchdog reset will be triggered by lack of keep_alive processing.
//!
//! @retval true temperature is high enough to move extruder
//! @retval false temperature is not high enough to move extruder, turned
//!         off E-stepper to prevent over-heating and allow filament pull-out if necessary
bool can_extrude()
{
    if ((degHotend(active_extruder) < EXTRUDE_MINTEMP) || !ir_sensor_detected)
    {
        disable_e0();
        delay_keep_alive(100);
        return false;
    }
    return true;
}

static void get_response_print_info(uint8_t move) {
	printf_P(PSTR("mmu_get_response - begin move: "), move);
	switch (move) {
		case MMU_LOAD_MOVE: puts_P(PSTR("load")); break;
		case MMU_UNLOAD_MOVE: puts_P(PSTR("unload")); break;
		case MMU_TCODE_MOVE: puts_P(PSTR("T-code")); break;
		case MMU_NO_MOVE: puts_P(PSTR("no move")); break;
		default: puts_P(PSTR("error: unknown move")); break;
	}
}

bool mmu_get_response(uint8_t move)
{

	get_response_print_info(move);
	KEEPALIVE_STATE(IN_PROCESS);
	while (mmu_cmd != MmuCmd::None)
	{
		delay_keep_alive(100);
	}

	while (!mmu_ready)
	{
		if ((mmu_state != S::WaitCmd) && (mmu_last_cmd == MmuCmd::None))
			break;

		switch (move) {
			case MMU_LOAD_MOVE:
			    mmu_loading_flag = true;
				if (can_extrude()) mmu_load_step();
				//don't rely on "ok" signal from mmu unit; if filament detected by idler sensor during loading stop loading movements to prevent infinite loading
				if (READ(IR_SENSOR_PIN) == 0) move = MMU_NO_MOVE;
				break;
			case MMU_UNLOAD_MOVE:
				if (READ(IR_SENSOR_PIN) == 0) //filament is still detected by idler sensor, printer helps with unlading 
				{
				    if (can_extrude())
				    {
                        puts_P(PSTR("Unload 1"));
                        current_position[E_AXIS] = current_position[E_AXIS] - MMU_LOAD_FEEDRATE * MMU_LOAD_TIME_MS*0.001;
                        plan_buffer_line_curposXYZE(MMU_LOAD_FEEDRATE);
                        st_synchronize();
				    }
				}
				else //filament was unloaded from idler, no additional movements needed 
				{ 
					puts_P(PSTR("Unloading finished 1"));
					disable_e0(); //turn off E-stepper to prevent overheating and alow filament pull-out if necessary
					move = MMU_NO_MOVE;
				}
				break;
			case MMU_TCODE_MOVE: //first do unload and then continue with infinite loading movements
				if (READ(IR_SENSOR_PIN) == 0) //filament detected by idler sensor, we must unload first 
				{
                    if (can_extrude())
                    {
                        puts_P(PSTR("Unload 2"));
                        current_position[E_AXIS] = current_position[E_AXIS] - MMU_LOAD_FEEDRATE * MMU_LOAD_TIME_MS*0.001;
                        plan_buffer_line_curposXYZE(MMU_LOAD_FEEDRATE);
                        st_synchronize();
                    }
				}
				else //delay to allow mmu unit to pull out filament from bondtech gears and then start with infinite loading 
				{ 
					puts_P(PSTR("Unloading finished 2"));
					disable_e0(); //turn off E-stepper to prevent overheating and alow filament pull-out if necessary
					delay_keep_alive(MMU_LOAD_TIME_MS);
					move = MMU_LOAD_MOVE;
					get_response_print_info(move);
				}
				break;
			case MMU_NO_MOVE:
			default: 
				delay_keep_alive(100);
				break;
		}
	}
	printf_P(PSTR("mmu_get_response() returning: %d\n"), mmu_ready);
	bool ret = mmu_ready;
	mmu_ready = false;
//	printf_P(PSTR("mmu_get_response - end %d\n"), ret?1:0);
	return ret;
}

//! @brief Wait for active extruder to reach temperature set
//!
//! This function is blocking and showing lcd_wait_for_heater() screen
//! which is constantly updated with nozzle temperature.
void mmu_wait_for_heater_blocking()
{
    while ((degTargetHotend(active_extruder) - degHotend(active_extruder)) > 5)
    {
        delay_keep_alive(1000);
        lcd_wait_for_heater();
    }
}

void manage_response(bool move_axes, bool turn_off_nozzle, uint8_t move)
{
	bool response = false;
	mmu_print_saved = false;
	bool lcd_update_was_enabled = false;
	float hotend_temp_bckp = degTargetHotend(active_extruder);
	float z_position_bckp = current_position[Z_AXIS];
	float x_position_bckp = current_position[X_AXIS];
	float y_position_bckp = current_position[Y_AXIS];	
	uint8_t screen = 0; //used for showing multiscreen messages
	mmu_loading_flag = false;
	while(!response)
	{
		  response = mmu_get_response(move); //wait for "ok" from mmu
		  if (!response) { //no "ok" was received in reserved time frame, user will fix the issue on mmu unit
			  if (!mmu_print_saved) { //first occurence, we are saving current position, park print head in certain position and disable nozzle heater
				  
				  uint8_t mmu_fail = eeprom_read_byte((uint8_t*)EEPROM_MMU_FAIL);
				  uint16_t mmu_fail_tot = eeprom_read_word((uint16_t*)EEPROM_MMU_FAIL_TOT);
				  if(mmu_fail < 255) eeprom_update_byte((uint8_t*)EEPROM_MMU_FAIL, mmu_fail + 1);
				  if(mmu_fail_tot < 65535) eeprom_update_word((uint16_t*)EEPROM_MMU_FAIL_TOT, mmu_fail_tot + 1);

				  if (lcd_update_enabled) {
					  lcd_update_was_enabled = true;
					  lcd_update_enable(false);
				  }
				  st_synchronize();
				  mmu_print_saved = true;
				  puts_P(PSTR("MMU not responding"));
				  KEEPALIVE_STATE(PAUSED_FOR_USER);
				  hotend_temp_bckp = degTargetHotend(active_extruder);
				  if (move_axes) {
					  z_position_bckp = current_position[Z_AXIS];
					  x_position_bckp = current_position[X_AXIS];
					  y_position_bckp = current_position[Y_AXIS];
				  
					  //lift z
					  current_position[Z_AXIS] += Z_PAUSE_LIFT;
					  if (current_position[Z_AXIS] > Z_MAX_POS) current_position[Z_AXIS] = Z_MAX_POS;
					  plan_buffer_line_curposXYZE(15);
					  st_synchronize();
					  					  
					  //Move XY to side
					  current_position[X_AXIS] = X_PAUSE_POS;
					  current_position[Y_AXIS] = Y_PAUSE_POS;
					  plan_buffer_line_curposXYZE(50);
					  st_synchronize();
				  }
				  if (turn_off_nozzle) {
					  //set nozzle target temperature to 0
					  setAllTargetHotends(0);
				  }
				  disable_e0(); //turn off E-stepper to prevent overheating and alow filament pull-out if necessary
			  }

			  //first three lines are used for printing multiscreen message; last line contains measured and target nozzle temperature
			  if (screen == 0) { //screen 0
				  lcd_display_message_fullscreen_P(_i("MMU needs user attention."));////MSG_MMU_USER_ATTENTION c=20 r=3
				  screen++;
			  }
			  else {  //screen 1
			      //printf_P(PSTR("active ex: %d deg: %d turn_off_nozzle: %d\n"),active_extruder ,degTargetHotend(active_extruder), (int)turn_off_nozzle);
				  if((degTargetHotend(active_extruder) == 0) && turn_off_nozzle) lcd_display_message_fullscreen_P(_i("Press the knob to resume nozzle temperature."));////MSG_RESUME_NOZZLE_TEMP c=20 r=4
				  else lcd_display_message_fullscreen_P(_i("Fix the issue and then press button on MMU unit."));////MSG_MMU_FIX_ISSUE c=20 r=4
				  screen=0;
			  }

			  //5 seconds delay
			  for (uint8_t i = 0; i < 5; i++) {
				  if (lcd_clicked()) {
					  setTargetHotend(hotend_temp_bckp, active_extruder);
					 /// mmu_cmd = mmu_last_cmd;
					  break;
				  }		  

				  //Print the hotend temperature (9 chars total) and fill rest of the line with space
				  lcd_set_cursor(0, 4); //line 4
				  int chars = lcd_printf_P(_N("%c%3d/%d%c"), LCD_STR_THERMOMETER[0],(int)(degHotend(active_extruder) + 0.5), (int)(degTargetHotend(active_extruder) + 0.5), LCD_STR_DEGREE[0]);
				  lcd_space(9 - chars);
				  delay_keep_alive(1000);
			  }
		  }
		  else if (mmu_print_saved) {
			  puts_P(PSTR("MMU starts responding"));
			  KEEPALIVE_STATE(IN_HANDLER);
			  mmu_loading_flag = false;
			  if (turn_off_nozzle) 
			  {
				lcd_clear();
				setTargetHotend(hotend_temp_bckp, active_extruder);
				if (((degTargetHotend(active_extruder) - degHotend(active_extruder)) > 5)) {
					lcd_display_message_fullscreen_P(_i("MMU OK. Resuming temperature...")); ////MSG_MMU_OK_RESUMING_TEMPERATURE c=20 r=4
					delay_keep_alive(3000);
				}
				mmu_wait_for_heater_blocking();
			  }			  
			  if (move_axes) {
				  lcd_clear();
				  lcd_display_message_fullscreen_P(_i("MMU OK. Resuming position...")); ////MSG_MMU_OK_RESUMING_POSITION c=20 r=4
				  current_position[X_AXIS] = x_position_bckp;
				  current_position[Y_AXIS] = y_position_bckp;
				  plan_buffer_line_curposXYZE(50);
				  st_synchronize();
				  current_position[Z_AXIS] = z_position_bckp;
				  plan_buffer_line_curposXYZE(15);
				  st_synchronize();
			  }
			  else {
				  lcd_clear();
				  lcd_display_message_fullscreen_P(_i("MMU OK. Resuming...")); ////MSG_MMU_OK_RESUMING c=20 r=4
				  delay_keep_alive(1000); //delay just for showing MMU OK message for a while in case that there are no xyz movements
			  }
		  }
	}
	if (lcd_update_was_enabled) lcd_update_enable(true);
#ifdef TMC2130
			//enable extruder motor (disabled in mmu_command, start of T-code processing)
			tmc2130_set_pwr(E_AXIS, 1);
			//printf_P(PSTR("E-axis enabled\n"));
#endif //TMC2130
}

//! @brief load filament to nozzle of multimaterial printer
//!
//! This function is used only only after T? (user select filament) and M600 (change filament).
//! It is not used after T0 .. T4 command (select filament), in such case, gcode is responsible for loading
//! filament to nozzle.
//!
void mmu_load_to_nozzle()
{
	st_synchronize();
	
	const bool saved_e_relative_mode = axis_relative_modes & E_AXIS_MASK;
	if (!saved_e_relative_mode) axis_relative_modes |= E_AXIS_MASK;
	if (ir_sensor_detected)
	{
		current_position[E_AXIS] += 3.0f;
	}
	else
	{
		current_position[E_AXIS] += 7.2f;
	}
    float feedrate = 562;
	plan_buffer_line_curposXYZE(feedrate / 60);
    st_synchronize();
	current_position[E_AXIS] += 14.4f;
	feedrate = 871;
	plan_buffer_line_curposXYZE(feedrate / 60);
    st_synchronize();
	current_position[E_AXIS] += 36.0f;
	feedrate = 1393;
	plan_buffer_line_curposXYZE(feedrate / 60);
    st_synchronize();
	current_position[E_AXIS] += 14.4f;
	feedrate = 871;
	plan_buffer_line_curposXYZE(feedrate / 60);
    st_synchronize();
	if (!saved_e_relative_mode) axis_relative_modes &= ~E_AXIS_MASK;
}

void mmu_M600_wait_and_beep() {
		//Beep and wait for user to remove old filament and prepare new filament for load

		KEEPALIVE_STATE(PAUSED_FOR_USER);

		int counterBeep = 0;
		lcd_display_message_fullscreen_P(_i("Remove old filament and press the knob to start loading new filament.")); ////MSG_REMOVE_OLD_FILAMENT c=20 r=5
		bool bFirst=true;

		while (!lcd_clicked()){
			manage_heater();
			manage_inactivity(true);

			#if BEEPER > 0
			if (counterBeep == 500) {
				counterBeep = 0;
			}
			SET_OUTPUT(BEEPER);
			if (counterBeep == 0) {
				if((eSoundMode==e_SOUND_MODE_BLIND)|| (eSoundMode==e_SOUND_MODE_LOUD)||((eSoundMode==e_SOUND_MODE_ONCE)&&bFirst))
				{
					bFirst=false;
					WRITE(BEEPER, HIGH);
				}
			}
			if (counterBeep == 20) {
				WRITE(BEEPER, LOW);
			}
				
			counterBeep++;
			#endif //BEEPER > 0

			delay_keep_alive(4);
		}
		WRITE(BEEPER, LOW);
}

//! @brief load filament for mmu v2
//! @par nozzle_temp nozzle temperature to load filament
void mmu_M600_load_filament(bool automatic, float nozzle_temp)
{ 
    tmp_extruder = mmu_extruder;
    if (automatic) {
        tmp_extruder = ad_getAlternative(tmp_extruder);
    }
    lcd_update_enable(false);
    lcd_clear();
    lcd_puts_at_P(0, 1, _T(MSG_LOADING_FILAMENT));
    lcd_print(' ');
    lcd_print(tmp_extruder + 1);

    //printf_P(PSTR("T code: %d \n"), tmp_extruder);
    //mmu_printf_P(PSTR("T%d\n"), tmp_extruder);
    setTargetHotend(nozzle_temp,active_extruder);
    mmu_wait_for_heater_blocking();

    mmu_command(MmuCmd::T0 + tmp_extruder);

    manage_response(false, true, MMU_LOAD_MOVE);
    mmu_continue_loading(usb_timer.running() || (lcd_commands_type == LcdCommands::Layer1Cal));
    mmu_extruder = tmp_extruder; //filament change is finished

    mmu_load_to_nozzle();
    load_filament_final_feed();
    st_synchronize();
}

void extr_adj(uint8_t extruder) //loading filament into the MMU unit
{
    MmuCmd cmd = MmuCmd::L0 + extruder;
    if (extruder > (MmuCmd::L4 - MmuCmd::L0))
    {
        printf_P(PSTR("Filament out of range %d \n"),extruder);
        return;
    }
    mmu_command(cmd);
	
	//show which filament is currently loaded
	
	lcd_update_enable(false);
	lcd_clear();
	lcd_puts_at_P(0, 1, _T(MSG_LOADING_FILAMENT));
	//if(strlen(_T(MSG_LOADING_FILAMENT))>18) lcd.setCursor(0, 1);
	//else lcd.print(" ");
	lcd_print(' ');
	lcd_print(extruder + 1);

	// get response
	manage_response(false, false);

	lcd_update_enable(true);
	
	
	//lcd_return_to_status();
}

struct E_step
{
    float extrude;   //!< extrude distance in mm
    float feed_rate; //!< feed rate in mm/s
};
static const E_step ramming_sequence[] PROGMEM =
{
    {1.0,   1000.0/60},
    {1.0,   1500.0/60},
    {2.0,   2000.0/60},
    {1.5,   3000.0/60},
    {2.5,   4000.0/60},
    {-15.0, 5000.0/60},
    {-14.0, 1200.0/60},
    {-6.0,  600.0/60},
    {10.0,  700.0/60},
    {-10.0, 400.0/60},
    {-50.0, 2000.0/60},
};

//! @brief Unload sequence to optimize shape of the tip of the unloaded filament
void mmu_filament_ramming()
{
    for(uint8_t i = 0; i < (sizeof(ramming_sequence)/sizeof(E_step));++i)
    {
        current_position[E_AXIS] += pgm_read_float(&(ramming_sequence[i].extrude));
        plan_buffer_line_curposXYZE(pgm_read_float(&(ramming_sequence[i].feed_rate)));
        st_synchronize();
    }
}


//! @brief show which filament is currently unloaded
void extr_unload_view()
{
    lcd_clear();
    lcd_puts_at_P(0, 1, _T(MSG_UNLOADING_FILAMENT));
    lcd_print(' ');
    if (mmu_extruder == MMU_FILAMENT_UNKNOWN) lcd_print(' ');
    else lcd_print(mmu_extruder + 1);
}

void extr_unload()
{ //unload just current filament for multimaterial printers
	if (degHotend0() > EXTRUDE_MINTEMP)
	{
		st_synchronize();

        menu_submenu(extr_unload_view);

		mmu_filament_ramming();

		mmu_command(MmuCmd::U0);
		// get response
		manage_response(false, true, MMU_UNLOAD_MOVE);

        menu_back();
	}
	else
	{
		show_preheat_nozzle_warning();
	}
}

void load_all()
{
	enquecommand_P(PSTR("M701 E0"));
	enquecommand_P(PSTR("M701 E1"));
	enquecommand_P(PSTR("M701 E2"));
	enquecommand_P(PSTR("M701 E3"));
	enquecommand_P(PSTR("M701 E4"));
}

bool mmu_check_version()
{
	return (mmu_buildnr >= MMU_REQUIRED_FW_BUILDNR);
}

void mmu_show_warning()
{
	printf_P(PSTR("MMU2 firmware version invalid. Required version: build number %d or higher."), MMU_REQUIRED_FW_BUILDNR);
	kill(_i("Please update firmware in your MMU2. Waiting for reset.")); ////MSG_UPDATE_MMU2_FW c=20 r=4
}

void lcd_mmu_load_to_nozzle(uint8_t filament_nr)
{
    menu_back();
    bFilamentAction = false;                            // NOT in "mmu_load_to_nozzle_menu()"
    if (degHotend0() > EXTRUDE_MINTEMP)
    {
        tmp_extruder = filament_nr;
        lcd_update_enable(false);
        lcd_clear();
        lcd_puts_at_P(0, 1, _T(MSG_LOADING_FILAMENT));
        lcd_print(' ');
        lcd_print(tmp_extruder + 1);
        mmu_command(MmuCmd::T0 + tmp_extruder);
        manage_response(true, true, MMU_TCODE_MOVE);
        mmu_continue_loading(false);
        mmu_extruder = tmp_extruder; //filament change is finished
        raise_z_above(MIN_Z_FOR_LOAD, false);
        mmu_load_to_nozzle();
        load_filament_final_feed();
        st_synchronize();
        custom_message_type = CustomMsg::FilamentLoading;
        lcd_setstatuspgm(_T(MSG_LOADING_FILAMENT));
        lcd_return_to_status();
        lcd_update_enable(true);
        lcd_load_filament_color_check();
        lcd_setstatuspgm(MSG_WELCOME);
        custom_message_type = CustomMsg::Status;
    }
    else
    {
        show_preheat_nozzle_warning();
    }
}

#ifdef MMU_HAS_CUTTER
void mmu_cut_filament(uint8_t filament_nr)
{
    menu_back();
    bFilamentAction=false;                            // NOT in "mmu_load_to_nozzle_menu()"
    if (degHotend0() > EXTRUDE_MINTEMP)
    {
        LcdUpdateDisabler disableLcdUpdate;
        lcd_clear();
        lcd_puts_at_P(0, 1, _i("Cutting filament")); ////MSG_MMU_CUTTING_FIL c=18
        lcd_print(' ');
        lcd_print(filament_nr + 1);
        mmu_filament_ramming();
        mmu_command(MmuCmd::K0 + filament_nr);
        manage_response(false, false, MMU_UNLOAD_MOVE);
    }
    else
    {
        show_preheat_nozzle_warning();
    }
}
#endif //MMU_HAS_CUTTER

void mmu_eject_filament(uint8_t filament, bool recover)
{
//-//
bFilamentAction=false;                            // NOT in "mmu_fil_eject_menu()"
	if (filament < 5) 
	{

		if (degHotend0() > EXTRUDE_MINTEMP)
		{
			st_synchronize();

			{
			    LcdUpdateDisabler disableLcdUpdate;
                lcd_clear();
                lcd_puts_at_P(0, 1, _i("Ejecting filament")); ////MSG_EJECTING_FILAMENT c=20
                mmu_filament_ramming();
                mmu_command(MmuCmd::E0 + filament);
                manage_response(false, false, MMU_UNLOAD_MOVE);
                if (recover)
                {
                    lcd_show_fullscreen_message_and_wait_P(_i("Please remove filament and then press the knob.")); ////MSG_EJECT_REMOVE c=20 r=4
                    mmu_command(MmuCmd::R0);
                    manage_response(false, false);
                }

            }
		}
		else
		{
			show_preheat_nozzle_warning();
		}
	}
	else
	{
		puts_P(PSTR("Filament nr out of range!"));
	}
}

//! @brief Fits filament tip into heatbreak?
//!
//! If PTFE tube is jammed, this causes filament to be unloaded and no longer
//! being detected by the pulley IR sensor.
//! @retval true Fits
//! @retval false Doesn't fit
static bool can_load()
{
    current_position[E_AXIS] += 60;
    plan_buffer_line_curposXYZE(MMU_LOAD_FEEDRATE);
    current_position[E_AXIS] -= 52;
    plan_buffer_line_curposXYZE(MMU_LOAD_FEEDRATE);
    st_synchronize();

    uint_least8_t filament_detected_count = 0;
    const float e_increment = 0.2;
    const uint_least8_t steps = 6.0 / e_increment;
    DEBUG_PUTS_P(PSTR("MMU can_load:"));
    for(uint_least8_t i = 0; i < steps; ++i)
    {
        current_position[E_AXIS] -= e_increment;
        plan_buffer_line_curposXYZE(MMU_LOAD_FEEDRATE);
        st_synchronize();
        if(0 == READ(IR_SENSOR_PIN))
        {
            ++filament_detected_count;
            DEBUG_PUTCHAR('O');
        }
        else
        {
            DEBUG_PUTCHAR('o');
        }
    }
    if (filament_detected_count > steps - 4)
    {
        DEBUG_PUTS_P(PSTR(" succeeded."));
        return true;
    }
    else
    {
        DEBUG_PUTS_P(PSTR(" failed."));
        return false;
    }
}

//! @brief load more
//!
//! Try to feed more filament from MMU if it is not detected by filament sensor.
//! @retval true Success, filament detected by IR sensor
//! @retval false Failed, filament not detected by IR sensor after maximum number of attempts
static bool load_more()
{
    for (uint8_t i = 0; i < MMU_IDLER_SENSOR_ATTEMPTS_NR; i++)
    {
        if (READ(IR_SENSOR_PIN) == 0) return true;
        DEBUG_PRINTF_P(PSTR("Additional load attempt nr. %d\n"), i);
        mmu_command(MmuCmd::C0);
        manage_response(true, true, MMU_LOAD_MOVE);
    }
    return false;
}

static void increment_load_fail()
{
    uint8_t mmu_load_fail = eeprom_read_byte((uint8_t*)EEPROM_MMU_LOAD_FAIL);
    uint16_t mmu_load_fail_tot = eeprom_read_word((uint16_t*)EEPROM_MMU_LOAD_FAIL_TOT);
    if(mmu_load_fail < 255) eeprom_update_byte((uint8_t*)EEPROM_MMU_LOAD_FAIL, mmu_load_fail + 1);
    if(mmu_load_fail_tot < 65535) eeprom_update_word((uint16_t*)EEPROM_MMU_LOAD_FAIL_TOT, mmu_load_fail_tot + 1);
}

//! @brief continue loading filament
//! @par blocking
//!  * true blocking - do not return until successful load
//!  * false non-blocking - pause print and return on load failure
//!
//! @startuml
//! [*] --> [*] : !ir_sensor_detected /\n send MmuCmd::C0
//! [*] --> LoadMore
//! LoadMore --> [*] : filament \ndetected
//! LoadMore --> Retry : !filament detected /\n increment load fail
//! Retry --> [*] : filament \ndetected
//! Retry --> Unload : !filament \ndetected
//! Unload --> [*] : non-blocking
//! Unload --> Retry : button \nclicked
//!
//! Retry : Cut filament if enabled
//! Retry : repeat last T-code
//! Unload : unload filament
//! Unload : pause print
//! Unload : show error message
//!
//! @enduml
void mmu_continue_loading(bool blocking)
{
	if (!ir_sensor_detected)
	{
	    mmu_command(MmuCmd::C0);
	    return;
	}

    bool success = load_more();
    if (success) success = can_load();

    enum class Ls : uint_least8_t
    {
        Enter,
        Retry,
        Unload,
    };
    Ls state = Ls::Enter;

    const uint_least8_t max_retry = 3;
    uint_least8_t retry = 0;

    while (!success)
    {
        switch (state)
        {
        case Ls::Enter:
            increment_load_fail();
            // FALLTHRU
        case Ls::Retry:
            ++retry; // overflow not handled, as it is not dangerous.
            if (retry >= max_retry)
            {
                state = Ls::Unload;
#ifdef MMU_HAS_CUTTER
                if (1 == eeprom_read_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED))
                {
                    mmu_command(MmuCmd::K0 + tmp_extruder);
                    manage_response(true, true, MMU_UNLOAD_MOVE);
                }
#endif //MMU_HAS_CUTTER
            }
            mmu_command(MmuCmd::T0 + tmp_extruder);
            manage_response(true, true, MMU_TCODE_MOVE);
            success = load_more();
            if (success) success = can_load();

            break;
        case Ls::Unload:
            stop_and_save_print_to_ram(0, 0);
            long_pause();

            mmu_command(MmuCmd::U0);
            manage_response(false, true, MMU_UNLOAD_MOVE);

            setAllTargetHotends(0);
            lcd_setstatuspgm(_i("MMU load failed"));////MSG_MMU_LOAD_FAILED c=20

            if (blocking)
            {
                marlin_wait_for_click();
                st_synchronize();
                restore_print_from_ram_and_continue(0);
                state = Ls::Retry;
            }
            else
            {
                mmu_fil_loaded = false; //so we can retry same T-code again
                isPrintPaused = true;
                mmu_command(MmuCmd::W0);
                return;
            }
            break;
        }
    }
}
