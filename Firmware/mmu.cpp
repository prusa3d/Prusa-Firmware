//mmu.cpp

#include "planner.h"
#include "language.h"
#include "lcd.h"
#include "uart2.h"
#include "temperature.h"
#include "Configuration_prusa.h"
#include "fsensor.h"
#include "cardreader.h"
#include "ultralcd.h"
#include "sound.h"
#include "printers.h"
#include <avr/pgmspace.h>
#include "io_atmega2560.h"
#include "AutoDeplete.h"
#include "util.h"

#ifdef TMC2130
#include "tmc2130.h"
#endif //TMC2130

#define CHECK_FINDA ((IS_SD_PRINTING || is_usb_printing) && (mcode_in_progress != 600) && !saved_printing && e_active())

#define MMU_TODELAY 100
#define MMU_TIMEOUT 10
#define MMU_IRSENS_TIMEOUT 3000ul
#define MMU_CMD_TIMEOUT 300000ul // 5min timeout for mmu commands (except P0)
static uint8_t mmu_attempt_nr = 0;

#ifdef MMU_HWRESET
#define MMU_RST_PIN 76
#endif //MMU_HWRESET

#ifdef MMU_DEBUG
static const auto DEBUG_PUTCHAR = putchar;
static const auto DEBUG_PUTS_P = puts_P;
static const auto DEBUG_PRINTF_P = printf_P;
#else //MMU_DEBUG
#define DEBUG_PUTCHAR(c)
#define DEBUG_PUTS_P(str)
#define DEBUG_PRINTF_P(__fmt, ...)
#endif //MMU_DEBUG

namespace
{ // MMU2S States
  enum class S : uint_least8_t
  {
    GetActExt,
    GetBN,
    GetVer,
    SetModeInit,
    SetMode,
    Init,
    Disabled,
    Idle,
    Wait,
    Setup
  };
}


const char* mmucmdnames[] = {
  "NO",
  "T0",
  "T1",
  "T2",
  "T3",
  "T4",
  "L0",
  "L1",
  "L2",
  "L3",
  "L4",
  "C0",
  "U0",
  "E0",
  "E1",
  "E2",
  "E3",
  "E4",
  "F0",
  "F1",
  "F2",
  "F3",
  "F4",
  "K0",
  "K1",
  "K2",
  "K3",
  "K4",
  "R0",
  "S3",
  "W0"
  };

bool mmu_enabled = false;
bool mmu_ready = false;
bool mmu_fil_loaded = false; //if true: blocks execution of duplicit T-codes
bool isMMUPrintPaused = false;
int lastLoadedFilament = -10;
uint16_t mmu_power_failures = 0;
void shutdownE0(bool shutdown = true);
void checkIR_SENSOR(void);

uint16_t toolChanges = 0;
uint8_t mmuE0BackupCurrents[2] = {0, 0};
#define TXTimeout 60 //60ms
uint8_t mmu_filament_types[5] = {0, 0, 0, 0, 0};
void mmu_unload_synced(uint16_t _filament_type_speed);

//idler ir sensor
bool mmu_idl_sens = false;
static bool MMU_IRSENS = false;
bool ir_sensor_detected = false;

//if IR_SENSOR defined, always returns true
//otherwise check for ir sensor and returns true if idler IR sensor was detected, otherwise returns false
bool check_for_ir_sensor() 
{
#ifdef IR_SENSOR
	return true;
#else //IR_SENSOR

	bool detected = false;
	//if IR_SENSOR_PIN input is low and pat9125sensor is not present we detected idler sensor
	if (isEXTLoaded 
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

static S mmu_state = S::Disabled; //int8_t mmu_state = 0;
MmuCmd mmu_cmd = MmuCmd::None;
MmuCmd mmu_last_cmd = MmuCmd::None;
uint8_t mmu_extruder = 0;

//! This variable probably has no meaning and is planed to be removed
uint8_t tmp_extruder = 0;
int8_t mmu_finda = -1;
int16_t mmu_version = -1;
int16_t mmu_buildnr = -1;
uint32_t mmu_last_request = 0;
uint32_t mmu_last_response = 0;
uint32_t mmu_last_finda_update = 0;
uint32_t mmu_last_finda_response = 0;

//initialize mmu2 unit - first part - should be done at begining of startup process
void mmu_init(void)
{
#ifdef MMU_HWRESET
  digitalWrite(MMU_RST_PIN, HIGH);
  pinMode(MMU_RST_PIN, OUTPUT); //setup reset pin
#endif                          //MMU_HWRESET
  uart2_init();                 //init uart2
  mmu_reset();                  //reset mmu (HW or SW), do not wait for response
  mmu_state = S::Init;
  PIN_INP(IR_SENSOR_PIN); //input mode
  PIN_SET(IR_SENSOR_PIN); //pullup
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
    shutdownE0();
    delay_keep_alive(100);
    return false;
  }
  shutdownE0(false);
  return true;
}

void mmu_loop(void)
{
  uint8_t filament = 0;
#ifdef MMU_DEBUG
  printf_P(PSTR("MMU loop, state=%d\n"), (int)mmu_state);
#endif //MMU_DEBUG

  cli();
  // Copy volitale vars as local
  unsigned char tData1 = rxData1;
  unsigned char tData2 = rxData2;
  unsigned char tData3 = rxData3;
  unsigned char tData4 = rxData4;
  unsigned char tData5 = rxData5;
  unsigned char tFINDA = rxFINDA;
  bool confPayload = confirmedPayload;
  bool confFINDA = confirmedFINDA;
  if (confPayload) { mmu_last_response = _millis(); confirmedPayload = false; }
  else { tData1 = ' '; tData2 = ' '; tData3 = ' '; tData4 = ' '; tData5 = ' '; }
  if (confFINDA) { mmu_last_response = _millis(); mmu_last_finda_response = _millis(); mmu_finda = tFINDA; confirmedFINDA = false; }
  if (atomic_MMU_IRSENS) { MMU_IRSENS = true; atomic_MMU_IRSENS = false; }
  sei();

  // All Notification & Status Updates between MK3S/MMU2S
  if (mmu_state > S::Disabled)
  {
    // Undates Active Extruder when every MMU is changed. eg. manual ex selection fro feed_filament
    if ((tData1 == 'A') && (tData2 == 'E') && (tData3 < 5)) {
      printf_P(PSTR("MMU2S => MK32S 'OK Active Extruder:%d'\n"), tmp_extruder + 1);
      tmp_extruder = tData3;
      mmu_extruder = tmp_extruder; }
    else if ((tData1 == 'S') && (tData2 == 'E') && (tData3 == 'T') && tData4 == 'U' && tData5 == 'P')
      mmu_state = S::Setup;
    else if ((tData1 == 'Z') && (tData2 == 'Z') && (tData3 == 'Z')) { // Clear MK3 Messages
      //********************
      lcd_setstatuspgm(_T(WELCOME_MSG));
      lcd_return_to_status();
      lcd_update_enable(true); }
    else if ((tData1 == 'Z') && (tData2 == 'Z') && (tData3 == 'R')) { // Advise MMU2S to reconnect
      lcd_setstatuspgm(_T(WELCOME_MSG));
      lcd_return_to_status();
      lcd_update_enable(true);
      tmp_extruder = MMU_FILAMENT_UNKNOWN;
      mmu_extruder = MMU_FILAMENT_UNKNOWN;
      mmu_enabled = false;
      mmu_reset();
      fSetMmuMode(false);
      mmu_state = S::Init; }
    else if ((tData1 == 'Z') && (tData2 == 'L') && (tData3 == '1')) { // MMU Loading Failed
                                             //********************
      lcd_setstatus("MMU Load Failed @MMU"); // 20 Chars
      #ifdef OCTO_NOTIFICATIONS_ON
      printf_P(PSTR("// action:mmuFailedLoadFINDA\n"));
      #endif // OCTO_NOTIFICATIONS_ON
    } 
    else if ((tData1 == 'Z') && (tData2 == 'L') && (tData3 == '2')) { // MMU Loading Failed
                  //********************
      lcd_setstatus("MMU Load Failed @MK3"); // 20 Chars
      #ifdef OCTO_NOTIFICATIONS_ON
      printf_P(PSTR("// action:mmuFailedLoadIR_SENSOR\n"));
      #endif // OCTO_NOTIFICATIONS_ON
      mmu_idl_sens = true; // To cover possible state where this trips off but error still happens from MMU2S
      MMU_IRSENS = false; }
    else if ((tData1 == 'Z') && (tData2 == 'U')) { // MMU Unloading Failed
                                             //********************
      lcd_setstatus(" MMU Unload Failed  "); // 20 Chars
      #ifdef OCTO_NOTIFICATIONS_ON
      printf_P(PSTR("// action:mmuFailedUnload\n"));
      #endif // OCTO_NOTIFICATIONS_ON
      mmu_idl_sens = true;
      MMU_IRSENS = false;
    }
    else if ((tData1 == 'Z') && (tData2 == '1')) { // MMU Filament Loaded
                                             //********************
      lcd_setstatus("ERR: Filament Loaded"); } // 20 Chars
    else if ((tData1 == 'X') && (tData2 == '1')) { // MMU Setup Menu
                                             //********************
      lcd_setstatus("   MMU Setup Menu   "); } // 20 Chars
    else if ((tData1 == 'X') && (tData2 == '2')) { // MMU Adj Bowden Len
                                             //********************
      lcd_setstatus("MMU Adj BowdenLength"); } // 20 Chars
    else if ((tData1 == 'X') && (tData2 == '5')) { // MMU Setup Menu: Unlock EEPROM
                                             //********************
      lcd_setstatus(" MMU Unlock EEPROM  "); } // 20 Chars
    else if ((tData1 == 'X') && (tData2 == '6')) { // MMU Setup Menu: Clr Unlocked EEPROM
                                             //********************
      lcd_setstatus("Clr EEPROM(unlocked)"); } // 20 Chars
    else if ((tData1 == 'X') && (tData2 == '7')) { // MMU Setup Menu: Exit
                                        //********************
      lcd_setstatus("Exit Setup Menu"); // 20 Chars
      lcd_return_to_status();
      lcd_update_enable(true); }
    else if (tData1 == 'B') { // MMU Adj Fsensor to Bondtech Length
      //********************
      lcd_update_enable(false);
      lcd_clear(); //********************
      lcd_set_cursor(0, 0);
      lcd_puts_P(_i("L:  +1mm (Extrude)  "));
      lcd_set_cursor(0, 1);
      lcd_puts_P(_i("M:Unload Save & Exit"));
      lcd_set_cursor(0, 2);
      lcd_puts_P(_i("R:  -1mm (Retract)  "));
      lcd_set_cursor(0, 3);
      lcd_puts_P(_i("Current mm: "));
      lcd_set_cursor(12, 3);
      lcd_print((tData2 << 8) | (tData3)); }
    else if (tData1 == 'V') { // MMU Adj Bowden Len: Loaded Message
      //********************
      lcd_update_enable(false);
      lcd_clear(); //********************
      lcd_set_cursor(0, 0);
      lcd_puts_P(_i("L: +2mm (Extrude)   "));
      lcd_set_cursor(0, 1);
      lcd_puts_P(_i("M:Unload(Save/Retry)"));
      lcd_set_cursor(0, 2);
      lcd_puts_P(_i("R: -2mm (Retract)   "));
      lcd_set_cursor(0, 3);
      lcd_puts_P(_i("Current mm: "));
      lcd_set_cursor(12, 3);
      lcd_print((tData2 << 8) | (tData3)); }
    else if (tData1 == 'W') { // MMU Adj Bowden Len: Unloaded Message
      //********************
      lcd_update_enable(false);
      lcd_clear(); //********************
      lcd_set_cursor(0, 0);
      lcd_puts_P(_i("L:   Save & Exit    "));
      lcd_set_cursor(0, 1);
      lcd_puts_P(_i("M:BowLen Load2Check "));
      lcd_set_cursor(0, 2);
      lcd_puts_P(_i("                    ")); //R:Set BondTech Steps"));
      lcd_set_cursor(0, 3);
      lcd_puts_P(_i("Current mm: "));
      lcd_set_cursor(12, 3);
      lcd_print((tData2 << 8) | (tData3)); }
    else if (tData1 == 'T') { // MMU Report ToolChange Count
      toolChanges = ((tData2 << 8) | (tData3));
      printf_P(PSTR("MMU2S => MK32S '@toolChange:%d'\n"), toolChanges); }
  } // End of mmu_state > S::Disabled

  if (!mmu_finda && CHECK_FSENSOR && fsensor_enabled && mmu_fil_loaded)
  {
    #ifdef OCTO_NOTIFICATIONS_ON
    printf_P(PSTR("// action:m600\n"));
    #endif // OCTO_NOTIFICATIONS_ON
    fsensor_checkpoint_print();
    ad_markDepleted(mmu_extruder);
    if (lcd_autoDepleteEnabled() && !ad_allDepleted()) enquecommand_front_P(PSTR("M600 AUTO")); //save print and run M600 command
    else enquecommand_front_P(PSTR("M600")); //save print and run M600 command
  }

  switch (mmu_state)
  {
  case S::Disabled:
    return;
  case S::Setup:
    return;
  case S::Init:
    if ((tData1 == 'S') && (tData2 == 'T') && (tData3 == 'R'))
    {
      uart2_txPayload((unsigned char *)"S1---");
      mmu_state = S::GetVer;
    }
    else if (mmu_last_response + MMU_CMD_TIMEOUT < _millis())
    { //30sec after reset disable mmu
      puts_P(PSTR("MMU not responding - DISABLED"));
      mmu_state = S::Disabled;
    }       // End of if STR
    return; // Exit method.
  case S::GetVer:
    if ((tData1 == 'O') && (tData2 == 'K'))
    {
      mmu_version = ((tData3 << 8) | (tData4));
      printf_P(PSTR("MMU2S => MK32S 'V:%d'\n"), mmu_version);
      uart2_txPayload((unsigned char *)"S2---");
      mmu_state = S::GetBN;
    }
    return; // Exit method.
  case S::GetBN:
    if ((tData1 == 'O') && (tData2 == 'K'))
    {
      mmu_buildnr = ((tData3 << 8) | (tData4));
      printf_P(PSTR("MMU2S => MK32S 'BN:%d'\n"), mmu_buildnr);
      bool version_valid = mmu_check_version();
      if (!version_valid)
        mmu_show_warning();
      else
        puts_P(PSTR("MMU version valid"));
      uart2_txPayload((unsigned char *)"S3---");
      mmu_state = S::GetActExt;
    }
    return; // Exit method.
  case S::GetActExt:
    if ((tData1 == 'O') && (tData2 == 'K'))
    {
      tmp_extruder = tData3;
      mmu_extruder = tmp_extruder;
      printf_P(PSTR("MMU2S => MK32S 'Active Extruder:%d'\n"), tmp_extruder + 1);
      #ifdef MMU_FORCE_STEALTH_MODE
	    SilentModeMenu_MMU = 1;
      #endif
      unsigned char tempSetMode[5] = {'M', SilentModeMenu_MMU, BLK, BLK, BLK};
      uart2_txPayload(tempSetMode);
      mmu_state = S::SetModeInit;
    }
    return; // Exit method.
  case S::SetModeInit:
    if ((tData1 == 'O') && (tData2 == 'K') && (tData3 == 'M') && (tData4 == SilentModeMenu_MMU))
    {
			eeprom_update_byte((uint8_t*)EEPROM_MMU_STEALTH, SilentModeMenu_MMU);
      puts_P(PSTR("MMU - ENABLED"));
      fSetMmuMode(true);
      mmu_enabled = true;
      mmu_ready = true;
      mmu_state = S::Idle;
    }
    return; // Exit method.
  case S::SetMode:
    if ((tData1 == 'O') && (tData2 == 'K') && (tData3 == 'M') && (tData4 == SilentModeMenu_MMU))
    {
			eeprom_update_byte((uint8_t*)EEPROM_MMU_STEALTH, SilentModeMenu_MMU);
      printf_P(PSTR("MMU2S => MK32S 'Confirm M%d'\n"), SilentModeMenu_MMU);
      mmu_state = S::Idle;
    }
    return; // Exit method.case S::Idle:
  case S::Idle:
    if (mmu_cmd != MmuCmd::None)
    {
      printf_P(PSTR("MK32S => MMU2S Request 'MmuCmd::%s'\n"), mmucmdnames[(uint8_t)mmu_cmd]);
      if ((mmu_cmd >= MmuCmd::T0) && (mmu_cmd <= MmuCmd::T4))
      { // CMD Has a Long execution, ensure to handle this
        filament = mmu_cmd - MmuCmd::T0;
        printf_P(PSTR("MK32S => MMU2S 'T%d'\n"), filament);
        unsigned char tempTxCMD[5] = {'T', (uint8_t)filament, BLK, BLK, BLK};
        uart2_txPayload(tempTxCMD);
        mmu_fil_loaded = true;
        mmu_idl_sens = true;
        MMU_IRSENS = false;
        mmu_state = S::Wait;
      }
      else if ((mmu_cmd >= MmuCmd::L0) && (mmu_cmd <= MmuCmd::L4))
      { // CMD Has a Long execution, ensure to handle this
        filament = mmu_cmd - MmuCmd::L0;
        printf_P(PSTR("MK32S => MMU2S 'L%d'\n"), filament);
        unsigned char tempLxCMD[5] = {'L', (uint8_t)filament, BLK, BLK, BLK};
        uart2_txPayload(tempLxCMD);
        mmu_state = S::Wait;
      }
      else if (mmu_cmd == MmuCmd::C0)
      {
        printf_P(PSTR("MK32S => MMU2S 'C0'\n"));
        uart2_txPayload((unsigned char *)"C0---");
        mmu_state = S::Wait;
      }
      else if (mmu_cmd == MmuCmd::U0)
      { // CMD Has a Long execution, ensure to handle this
        printf_P(PSTR("MK32S => MMU2S 'U0'\n"));
        uart2_txPayload((unsigned char *)"U0---");
        mmu_fil_loaded = false;
        mmu_state = S::Wait;
      }
      else if (((mmu_cmd >= MmuCmd::E0) && (mmu_cmd <= MmuCmd::E4)) || ((mmu_cmd >= MmuCmd::K0) && (mmu_cmd <= MmuCmd::K4)))
      { // CMD Has a Long execution, ensure to handle this
        filament = mmu_cmd - MmuCmd::E0;
        printf_P(PSTR("MK32S => MMU2S 'E%d'\n"), filament);
        unsigned char tempExCMD[5] = {'E', (uint8_t)filament, BLK, BLK, BLK};
        uart2_txPayload(tempExCMD);
        mmu_fil_loaded = false;
        mmu_state = S::Wait;
      }
      else if (mmu_cmd == MmuCmd::R0)
      { // CMD Has a Long execution, ensure to handle this
        printf_P(PSTR("MK32S => MMU2S 'R0'\n"));
        uart2_txPayload((unsigned char *)"R0---");
        mmu_state = S::Wait;
      }
      else if ((mmu_cmd >= MmuCmd::F0) && (mmu_cmd <= MmuCmd::F4))
      {
        uint8_t extruder = mmu_cmd - MmuCmd::F0;
        unsigned char tempTxCMD[5] = {'F', (uint8_t)extruder, mmu_filament_types[extruder], BLK, BLK};
        printf_P(PSTR("MK32S => MMU2S 'F%d %d'\n"), extruder, mmu_filament_types[extruder]);
        uart2_txPayload(tempTxCMD);
        mmu_state = S::Wait;
      }
      mmu_cmd = MmuCmd::None;
    }
    else if ((eeprom_read_byte((uint8_t*)EEPROM_MMU_STEALTH) != SilentModeMenu_MMU) && mmu_ready)
    {
      printf_P(PSTR("MK32S => MMU2S 'M%d'\n"), SilentModeMenu_MMU);
      unsigned char tempSetMode[5] = {'M', SilentModeMenu_MMU, BLK, BLK, BLK};
      uart2_txPayload(tempSetMode);
      mmu_state = S::SetMode;
		}
    else if (((mmu_last_finda_update + 300) < _millis()) && !mmu_idl_sens)
    {
      mmu_last_finda_update = _millis();
      uart2_txPayload((unsigned char *)"P0---");
    }
    return; // Exit method.
  case S::Wait:
    if (tData1 == 'U')
    {
      printf_P(PSTR("MMU2S => MK32S 'Unload Feedrate: %d%d'\n"), tData2, tData3);
      mmu_unload_synced((tData2 << 8) | (tData3));
    }
    else if ((tData1 == 'O') && (tData2 == 'K'))
    {
      printf_P(PSTR("MMU2S => MK32S 'ok'\n"));
      mmu_attempt_nr = 0;
      mmu_last_cmd = MmuCmd::None;
      mmu_ready = true;
      mmu_state = S::Idle;
    }
    return; // Exit method.
  }
} // End of mmu_loop() method.

void mmu_reset(void)
{
  #ifdef MMU_HWRESET //HW - pulse reset pin
  digitalWrite(MMU_RST_PIN, LOW);
  _delay_us(50);
  printf_P(PSTR("MK3S => MMU2S : HWR RESET\n"));
  digitalWrite(MMU_RST_PIN, HIGH);
  #else //SW - send X0 command
  uart2_txPayload((unsigned char *)"X0---");
  #endif
} // End of mmu_reset() method.

void mmu_set_filament_type(uint8_t extruder, uint8_t filament)
{
  mmu_filament_types[extruder] = filament;
  printf_P(PSTR("MK3 => 'F%d %d'\n"), extruder, filament);
  mmu_command(MmuCmd::F0 + extruder);
  manage_response(false, false);
} // End of mmu_set_filament_type() method.

//! @brief Enqueue MMUv2 command
//!
//! Call manage_response() after enqueuing to process command.
//! If T command is enqueued, it disables current for extruder motor if TMC2130 driver present.
//! If T or L command is enqueued, it marks filament loaded in AutoDeplete module.
void mmu_command(MmuCmd cmd)
{
  if ((cmd >= MmuCmd::T0) && (cmd <= MmuCmd::T4)) { ad_markLoaded(cmd - MmuCmd::T0); shutdownE0(); }
  if ((cmd >= MmuCmd::L0) && (cmd <= MmuCmd::L4)) ad_markLoaded(cmd - MmuCmd::L0);
  if ((cmd >= MmuCmd::E0) && (cmd <= MmuCmd::E4)) shutdownE0();
  mmu_last_cmd = mmu_cmd;
  mmu_cmd = cmd;
  mmu_ready = false;
}

void mmu_unload_synced(uint16_t _filament_type_speed)
{
  shutdownE0(false);
  st_synchronize();
  current_position[E_AXIS] -= 20;
  plan_buffer_line_curposXYZE(_filament_type_speed, active_extruder);
  st_synchronize();
  shutdownE0();
}

bool mmu_get_response(void)
{
	KEEPALIVE_STATE(IN_PROCESS);
	while (mmu_cmd != MmuCmd::None) { delay_keep_alive(100); }
	while (!mmu_ready) {
    mmu_loop();
    if (mmu_idl_sens && MMU_IRSENS) {
      for (uint8_t i = 0; i < 75; i++) {
        if (!isEXTLoaded) mmu_load_step();
        else break;
      }
      if (isEXTLoaded) {
        uart2_txPayload((unsigned char *)"IRSEN");
        printf_P(PSTR("MK32S => MMU2S 'Filament seen at extruder'\n"));
        mmu_idl_sens = false;
      }
      MMU_IRSENS = false;
    }
    if (mmu_state == S::Wait && mmu_last_response + MMU_CMD_TIMEOUT > _millis()) delay_keep_alive(100);
    else break;
	}
	return mmu_ready;
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

void manage_response(bool move_axes, bool turn_off_nozzle)
{
	mmu_print_saved = false;
	bool lcd_update_was_enabled = false;
	float hotend_temp_bckp = degTargetHotend(active_extruder);
	float z_position_bckp = current_position[Z_AXIS];
	float x_position_bckp = current_position[X_AXIS];
	float y_position_bckp = current_position[Y_AXIS];
	uint8_t screen = 0; //used for showing multiscreen messages
  //bool mmu_response = mmu_get_response();
	while(!mmu_get_response())
	{
    if (!mmu_print_saved) { //first occurence, we are saving current position, park print head in certain position and disable nozzle heater
      uint8_t mmu_fail = eeprom_read_byte((uint8_t *)EEPROM_MMU_FAIL);
      uint16_t mmu_fail_tot = eeprom_read_word((uint16_t *)EEPROM_MMU_FAIL_TOT);
      shutdownE0();  // Drop E0 Currents to 0.
      if (mmu_fail < 255)
        eeprom_update_byte((uint8_t *)EEPROM_MMU_FAIL, mmu_fail + 1);
      if (mmu_fail_tot < 65535)
        eeprom_update_word((uint16_t *)EEPROM_MMU_FAIL_TOT, mmu_fail_tot + 1);
      if (lcd_update_enabled) {
        lcd_update_was_enabled = true;
        lcd_update_enable(false);
      }
      st_synchronize();
      mmu_print_saved = true;
#ifdef OCTO_NOTIFICATIONS_ON
      printf_P(PSTR("// action:mmuAttention\n"));
#endif // OCTO_NOTIFICATIONS_ON
      printf_P(PSTR("MMU not responding\n"));
      KEEPALIVE_STATE(PAUSED_FOR_USER);
      hotend_temp_bckp = degTargetHotend(active_extruder);
      if (move_axes) {
        z_position_bckp = current_position[Z_AXIS];
        x_position_bckp = current_position[X_AXIS];
        y_position_bckp = current_position[Y_AXIS];

        //lift z
        current_position[Z_AXIS] += Z_PAUSE_LIFT;
        if (current_position[Z_AXIS] > Z_MAX_POS)
          current_position[Z_AXIS] = Z_MAX_POS;
        plan_buffer_line_curposXYZE(15, active_extruder);
        st_synchronize();

        //Move XY to side
        current_position[X_AXIS] = X_PAUSE_POS;
        current_position[Y_AXIS] = Y_PAUSE_POS;
        plan_buffer_line_curposXYZE(50, active_extruder);
        st_synchronize();
      }
      if (turn_off_nozzle) {
        //set nozzle target temperature to 0
        setAllTargetHotends(0);
      }
    }

    //first three lines are used for printing multiscreen message; last line contains measured and target nozzle temperature
    if (screen == 0) { //screen 0
      lcd_display_message_fullscreen_P(_i("MMU needs user attention."));
      screen++;
    }
    else {  //screen 1
      if((degTargetHotend(active_extruder) == 0) && turn_off_nozzle) lcd_display_message_fullscreen_P(_i("Press the knob to resume nozzle temperature."));
      else lcd_display_message_fullscreen_P(_i("Fix the issue and then press button on MMU unit."));
      screen=0;
    }

    lcd_set_degree();
    lcd_set_cursor(0, 4); //line 4
    //Print the hotend temperature (9 chars total) and fill rest of the line with space
    int chars = lcd_printf_P(_N("%c%3d/%d%c"), LCD_STR_THERMOMETER[0],(int)(degHotend(active_extruder) + 0.5), (int)(degTargetHotend(active_extruder) + 0.5), LCD_STR_DEGREE[0]);
    lcd_space(9 - chars);

    // 5 seconds delay
    for (uint8_t r = 0; r < 5; r++)
    {
      for (uint8_t i = 0; i < 10; i++) {
        if (lcd_clicked()) {
          setTargetHotend(hotend_temp_bckp, active_extruder);
          break;
        }
        delay_keep_alive(100);
      }
      //_response = mmu_get_response();
    }
    //_delay(100);
    //mmu_response = mmu_get_response();
	}
  if (mmu_print_saved) {
    printf_P(PSTR("MMU starts responding\n"));
    KEEPALIVE_STATE(IN_HANDLER);
    shutdownE0(false);  // Reset E0 Currents.
    if (turn_off_nozzle)
    {
    lcd_clear();
    setTargetHotend(hotend_temp_bckp, active_extruder);
    if (((degTargetHotend(active_extruder) - degHotend(active_extruder)) > 5)) {
      lcd_display_message_fullscreen_P(_i("MMU OK. Resuming temperature..."));
      delay_keep_alive(3000);
    }
    while ((degTargetHotend(active_extruder) - degHotend(active_extruder)) > 5)
    {
      delay_keep_alive(1000);
      lcd_wait_for_heater();
    }
    }
    if (move_axes) {
      lcd_clear();
      lcd_display_message_fullscreen_P(_i("MMU OK. Resuming position..."));
      current_position[X_AXIS] = x_position_bckp;
      current_position[Y_AXIS] = y_position_bckp;
    plan_buffer_line_curposXYZE(50, active_extruder);
      st_synchronize();
      current_position[Z_AXIS] = z_position_bckp;
    plan_buffer_line_curposXYZE(15, active_extruder);
      st_synchronize();
    }
    else {
      lcd_clear();
      lcd_display_message_fullscreen_P(_i("MMU OK. Resuming..."));
      delay_keep_alive(1000); //delay just for showing MMU OK message for a while in case that there are no xyz movements
    }
  }
	if (lcd_update_was_enabled) lcd_update_enable(true);
  shutdownE0(false);  // Reset E0 Currents. May not be required here as well as after resume code above.
}

void shutdownE0(bool shutdown)
{
  #ifdef TMC2130
  if (shutdown && ((tmc2130_current_h[E_AXIS] != 0) && (tmc2130_current_r[E_AXIS] != 0))) {
      mmuE0BackupCurrents[0] = tmc2130_current_h[E_AXIS];
      mmuE0BackupCurrents[1] = tmc2130_current_r[E_AXIS];
      tmc2130_set_current_h(E_AXIS, 0);
      tmc2130_set_current_r(E_AXIS, 0);
      printf_P(PSTR("E-AXIS Disabled.\n"));
  } else if (!shutdown && ((tmc2130_current_h[E_AXIS] == 0) && (tmc2130_current_r[E_AXIS] == 0))) {
      tmc2130_set_current_h(E_AXIS, mmuE0BackupCurrents[0]);
      tmc2130_set_current_r(E_AXIS, mmuE0BackupCurrents[1]);
      printf_P(PSTR("E-AXIS Enabled.\n"));
  }
  #else
  if (shutdown) disable_e0();
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
  plan_buffer_line_curposXYZE(feedrate / 60, active_extruder);
  st_synchronize();
  #ifdef BONDTECH_MK25S
    current_position[E_AXIS] += 25.4f; //Bondtech MK2.5s 11 mm longer t melt zone
  #elif defined(BONDTECH_MK3S)
    current_position[E_AXIS] += 25.4f; //Bondtech MK3s 11 mm longer t melt zone
  #elif defined(BONDTECH_MOSQUITO)
    current_position[E_AXIS] += 23.4f; //Bondtech Mosquito 9 mm longer t melt zone
  #elif defined(BONDTECH_MOSQUITO_MAGNUM)
    current_position[E_AXIS] += 18.4f; //Bondtech Mosquito Magnum 5 mm longer t melt zone
  #else
	 current_position[E_AXIS] += 14.4f;
  #endif
  feedrate = 871;
  plan_buffer_line_curposXYZE(feedrate / 60, active_extruder);
  st_synchronize();
  current_position[E_AXIS] += 36.0f;
  feedrate = 1393;
  plan_buffer_line_curposXYZE(feedrate / 60, active_extruder);
  st_synchronize();
//Distance through heat block is longer with Mosquito / Mosquito Magnum
  #ifdef BONDTECH_MOSQUITO
    current_position[E_AXIS] += 16.4f; //2mm further through Mosquito heat block
  #elif defined(BONDTECH_MOSQUITO_MAGNUM)
    current_position[E_AXIS] += 21.4f; //7mm further through Mosquito Magnum heat block
  #else
	  current_position[E_AXIS] += 14.4f;
  #endif
  feedrate = 871;
  plan_buffer_line_curposXYZE(feedrate / 60, active_extruder);
  st_synchronize();
	if (!saved_e_relative_mode) axis_relative_modes &= ~E_AXIS_MASK;
}

void mmu_M600_wait_and_beep()
{
  //Beep and wait for user to remove old filament and prepare new filament for load
  long timemark = _millis();
  bool timedout = false;
  shutdownE0();
  float hotend_temp_bckp = degTargetHotend(active_extruder);
  KEEPALIVE_STATE(PAUSED_FOR_USER);

  int counterBeep = 0;
  lcd_display_message_fullscreen_P(_i("Remove ejected filament and press the knob to start loading new filament."));
  bool bFirst = true;

  while (!lcd_clicked())
  {
    manage_heater();
    manage_inactivity(true);
    if (!timedout && timemark + MMU_CMD_TIMEOUT < _millis()) { setAllTargetHotends(0); timedout = true; }
    #if BEEPER > 0
    if (counterBeep == 500)
      counterBeep = 0;
    SET_OUTPUT(BEEPER);
    if (counterBeep == 0)
    {
      if ((eSoundMode == e_SOUND_MODE_LOUD) || ((eSoundMode == e_SOUND_MODE_ONCE) && bFirst))
      {
        bFirst = false;
        WRITE(BEEPER, HIGH);
      }
    }
    if (counterBeep == 20)
      WRITE(BEEPER, LOW);

    counterBeep++;
    #endif //BEEPER > 0

    delay_keep_alive(4);
  }
  setTargetHotend(hotend_temp_bckp, active_extruder);
  lcd_clear();
  while ((degTargetHotend(active_extruder) - degHotend(active_extruder)) > 5)
  {
    delay_keep_alive(1000);
    lcd_wait_for_heater();
  }
  WRITE(BEEPER, LOW);
}

//! @brief load filament for mmu v2
//! @par nozzle_temp nozzle temperature to load filament
void mmu_M600_load_filament(bool automatic, float nozzle_temp)
{
  tmp_extruder = mmu_extruder;
  if (!automatic)
  {
#ifdef MMU_M600_SWITCH_EXTRUDER
    bool yes = lcd_show_fullscreen_message_yes_no_and_wait_P(_i("Do you want to switch extruder?"), false);
    if (yes)
      tmp_extruder = choose_extruder_menu();
#endif //MMU_M600_SWITCH_EXTRUDER
  }
  else
  {
    tmp_extruder = ad_getAlternative(tmp_extruder);
  }
  lcd_update_enable(false);
  lcd_clear();
  lcd_set_cursor(0, 1);
  lcd_puts_P(_T(MSG_LOADING_FILAMENT));
  lcd_print(" ");
  lcd_print(tmp_extruder + 1);

  setTargetHotend(nozzle_temp, active_extruder);
  mmu_wait_for_heater_blocking();

  mmu_command(MmuCmd::T0 + tmp_extruder);
  manage_response(false, true);
  mmu_continue_loading();
  mmu_extruder = tmp_extruder; //filament change is finished
  mmu_load_to_nozzle();
  load_filament_final_feed();
  st_synchronize();
}

void extr_adj(uint8_t extruder) //loading filament for SNMM
{
  MmuCmd cmd = MmuCmd::L0 + extruder;
  if (extruder > (MmuCmd::L4 - MmuCmd::L0))
  {
    printf_P(PSTR("Filament out of range %d \n"), extruder);
    return;
  }
  mmu_command(cmd);

  //show which filament is currently loaded

  lcd_update_enable(false);
  lcd_clear();
  lcd_set_cursor(0, 1);
  lcd_puts_P(_T(MSG_LOADING_FILAMENT));
  //if(strlen(_T(MSG_LOADING_FILAMENT))>18) lcd.setCursor(0, 1);
  //else lcd.print(" ");
  lcd_print(" ");
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
        {1.0, 1000.0 / 60},
        {1.0, 1500.0 / 60},
        {2.0, 2000.0 / 60},
        {1.5, 3000.0 / 60},
        {2.5, 4000.0 / 60},
        {-15.0, 5000.0 / 60},
        {-14.0, 1200.0 / 60},
        {-6.0, 600.0 / 60},
        {10.0, 700.0 / 60},
        {-10.0, 400.0 / 60},
        {-50.0, 2000.0/60}
};

//! @brief Unload sequence to optimize shape of the tip of the unloaded filament
static void filament_ramming()
{
  for (uint8_t i = 0; i < (sizeof(ramming_sequence) / sizeof(E_step)); ++i)
  {
    current_position[E_AXIS] += pgm_read_float(&(ramming_sequence[i].extrude));
    plan_buffer_line_curposXYZE(pgm_read_float(&(ramming_sequence[i].feed_rate)), active_extruder);
    st_synchronize();
  }
}

//! @brief Unload sequence to optimize shape of the tip of the unloaded filament
void mmu_filament_ramming()
{
  for (uint8_t i = 0; i < (sizeof(ramming_sequence) / sizeof(E_step)); ++i)
  {
    current_position[E_AXIS] += pgm_read_float(&(ramming_sequence[i].extrude));
    plan_buffer_line_curposXYZE(pgm_read_float(&(ramming_sequence[i].feed_rate)), active_extruder);
    st_synchronize();
  }
}

//! @brief Rotate extruder idler to catch filament
//! @par synchronize
//!  * true blocking call
//!  * false non-blocking call
void mmu_load_step(bool synchronize)
{
  shutdownE0(false);
  current_position[E_AXIS] = current_position[E_AXIS] + MMU_LOAD_FEEDRATE * 0.1;
  plan_buffer_line_curposXYZE(MMU_LOAD_FEEDRATE, active_extruder);
  if (synchronize)
    st_synchronize();
}

//-//
void extr_unload_()
{
  //if(bFilamentAction)
  if (0)
  {
    bFilamentAction = false;
    extr_unload();
  }
  else
  {
    eFilamentAction = FilamentAction::MmuUnLoad;
    bFilamentFirstRun = false;
    if (target_temperature[active_extruder] >= EXTRUDE_MINTEMP)
    {
      bFilamentPreheatState = true;
      mFilamentItem(target_temperature[active_extruder], target_temperature_bed);
    }
    else
      lcd_generic_preheat_menu();
  }
}

//! @brief show which filament is currently unloaded
void extr_unload_view()
{
  lcd_clear();
  lcd_set_cursor(0, 1);
  lcd_puts_P(_T(MSG_UNLOADING_FILAMENT));
  lcd_print(" ");
  if (mmu_extruder == MMU_FILAMENT_UNKNOWN)
    lcd_print(" ");
  else
    lcd_print(mmu_extruder + 1);
}

void extr_unload()
{ //unload just current filament for multimaterial printers

  if (degHotend0() > EXTRUDE_MINTEMP)
  {
    st_synchronize();
		//show which filament is currently unloaded
    lcd_setstatuspgm(_T(WELCOME_MSG));
    lcd_return_to_status();
    lcd_update_enable(true);
    char msg[20];
    sprintf_P(msg, PSTR("MMU Unloading Fil:%d"), (mmu_extruder + 1));
                       //********************
    lcd_setstatus(msg); // 20 Chars
    mmu_filament_ramming();
    mmu_command(MmuCmd::U0);
    // get response
    manage_response(false, true);
    lcd_setstatuspgm(_T(WELCOME_MSG));
    lcd_return_to_status();
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
  kill(_i("Please update firmware in your MMU2. Waiting for reset."));
}

void lcd_mmu_load_to_nozzle(uint8_t filament_nr)
{
  menu_back();
  bFilamentAction = false; // NOT in "mmu_load_to_nozzle_menu()"
  if (degHotend0() > EXTRUDE_MINTEMP)
  {
    tmp_extruder = filament_nr;
    lcd_setstatuspgm(_T(WELCOME_MSG));
    lcd_return_to_status();
    lcd_update_enable(true);
    char msg[20];
    sprintf_P(msg, PSTR("MMU Loading Ext:%d"), (tmp_extruder + 1));
                      //********************
    lcd_setstatus(msg); // 20 Chars 
    if (isEXTLoaded) mmu_filament_ramming();
    mmu_command(MmuCmd::T0 + tmp_extruder);
    manage_response(true, true);
    mmu_continue_loading();
    mmu_extruder = tmp_extruder; //filament change is finished
    mmu_load_to_nozzle();
    load_filament_final_feed();
    st_synchronize();
    custom_message_type = CustomMsg::FilamentLoading;
    lcd_setstatuspgm(_T(MSG_LOADING_FILAMENT));
    lcd_return_to_status();
    lcd_update_enable(true);
    lcd_load_filament_color_check();
    lcd_setstatuspgm(_T(WELCOME_MSG));
    custom_message_type = CustomMsg::Status;
  }
  else
  {
    show_preheat_nozzle_warning();
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
    #ifdef BONDTECH_MK25S //feed to melt zone
      current_position[E_AXIS] += 71; //Bondtech_V6 71mm from drive gear to melt zone
    #elif defined(BONDTECH_MK3S)
      current_position[E_AXIS] += 71; //Bondtech_V6 71mm from drive gear to melt zone
    #elif defined(BONDTECH_MOSQUITO)
      current_position[E_AXIS] += 70; //Bondtech_Mosquito 70mm from drive gear to melt zone
    #elif defined(BONDTECH_MOSQUITO_MAGNUM)
      current_position[E_AXIS] += 62; //Bondtech_Mosquito_Magnum 62mm from drive gear to melt zone
    #else
      current_position[E_AXIS] += 60;
    #endif
    plan_buffer_line_curposXYZE(MMU_LOAD_FEEDRATE, active_extruder);

    #ifdef BONDTECH_MK25S//pull back to 8 mm below drive gear to check if filament path was blocked
      current_position[E_AXIS] -= 63; // Pull back 63mm, 8 mm below drive gear
    #elif defined(BONDTECH_MK3S)
      current_position[E_AXIS] -= 63; // Pull back 63mm, 8 mm below drive gear
    #elif defined(BONDTECH_MOSQUITO)
      current_position[E_AXIS] -= 62; // Pull back 62mm, 8 mm below drive gear
    #elif defined(BONDTECH_MOSQUITO_MAGNUM)
      current_position[E_AXIS] -= 54; // Pull back 54mm, 8 mm below drive gear
    #else
      current_position[E_AXIS] -= 52;
    #endif
    plan_buffer_line_curposXYZE(MMU_LOAD_FEEDRATE, active_extruder);
    st_synchronize();

    uint_least8_t filament_detected_count = 0;
    const float e_increment = 0.2;
    const uint_least8_t steps = 6.0 / e_increment;
    DEBUG_PUTS_P(PSTR("MMU can_load:"));
    for(uint_least8_t i = 0; i < steps; ++i)
    {
        current_position[E_AXIS] -= e_increment;
        plan_buffer_line_curposXYZE(MMU_LOAD_FEEDRATE, active_extruder);
        st_synchronize();
        if(isEXTLoaded) ++filament_detected_count;
    }
    if (filament_detected_count > steps - 4) return true;
    else return false;
}

static void increment_load_fail()
{
    uint8_t mmu_load_fail = eeprom_read_byte((uint8_t*)EEPROM_MMU_LOAD_FAIL);
    uint16_t mmu_load_fail_tot = eeprom_read_word((uint16_t*)EEPROM_MMU_LOAD_FAIL_TOT);
    if(mmu_load_fail < 255) eeprom_update_byte((uint8_t*)EEPROM_MMU_LOAD_FAIL, mmu_load_fail + 1);
    if(mmu_load_fail_tot < 65535) eeprom_update_word((uint16_t*)EEPROM_MMU_LOAD_FAIL_TOT, mmu_load_fail_tot + 1);
}

//! @brief Fits filament tip into heatbreak?
//!
//! If PTFE tube is jammed, this causes filament to be unloaded and no longer
//! being detected by the pulley IR sensor.
//! @retval true Fits
//! @retval false Doesn't fit
void mmu_continue_loading(void)
{
  if (!ir_sensor_detected || mmu_filament_types[mmu_extruder] == 1) {
    mmu_command(MmuCmd::C0);
    manage_response(true, true);
    return;
  }
  mmu_command(MmuCmd::C0);
  manage_response(true, true);
  bool success = can_load();

  enum class Ls : uint_least8_t
  {
      Enter,
      Retry,
      Unload,
  };
  Ls state = Ls::Enter;

  while (!success)
  {
    switch (state)
    {
    case Ls::Enter:
        increment_load_fail();
        state = Ls::Unload;
        break;
    case Ls::Retry:
        #ifdef MMU_HAS_CUTTER
        if (1 == eeprom_read_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED))
        {
            mmu_command(MmuCmd::K0 + tmp_extruder);
            manage_response(true, true, MMU_UNLOAD_MOVE);
        }
        #endif //MMU_HAS_CUTTER
        mmu_command(MmuCmd::T0 + tmp_extruder);
        manage_response(true, true);
        mmu_command(MmuCmd::C0);
        manage_response(true, true);
        success = can_load();
        state = Ls::Unload;
        break;
    case Ls::Unload:
        printf_P(PSTR("Jam, Malformed Tip or Clog.\n"));
        #ifdef OCTO_NOTIFICATIONS_ON
        printf_P(PSTR("// action:jamDetected\n"));
        #endif // OCTO_NOTIFICATIONS_ON
        stop_and_save_print_to_ram(0, 0);

        //lift z
        current_position[Z_AXIS] += Z_PAUSE_LIFT;
        if (current_position[Z_AXIS] > Z_MAX_POS) current_position[Z_AXIS] = Z_MAX_POS;
        plan_buffer_line_curposXYZE(15, active_extruder);
        st_synchronize();

        //Move XY to side
        current_position[X_AXIS] = X_PAUSE_POS;
        current_position[Y_AXIS] = Y_PAUSE_POS;
        plan_buffer_line_curposXYZE(50, active_extruder);
        st_synchronize();

        mmu_command(MmuCmd::U0);
        manage_response(false, true);

        setAllTargetHotends(0);
        lcd_update_enable(false);
        lcd_clear(); //********************
        lcd_set_cursor(0, 0);
        lcd_puts_P(_i("                    "));
        lcd_set_cursor(0, 1);
        lcd_puts_P(_i(" Heatbreak Jam/Clog "));
        lcd_set_cursor(0, 2);
        lcd_puts_P(_i("Click wheel to retry"));
        lcd_set_cursor(0, 3);
        lcd_puts_P(_i("                    "));
        marlin_wait_for_click();
        lcd_setstatuspgm(_T(WELCOME_MSG));
        lcd_return_to_status();
        lcd_update_enable(true);
        restore_print_from_ram_and_continue(0);
        state = Ls::Retry;
        break;
    }
  }
}

void mmu_eject_filament(uint8_t filament, bool recover)
{
  if (filament < 5)
  {
    if (degHotend0() <= EXTRUDE_MINTEMP && isEXTLoaded) {
      show_preheat_nozzle_warning();
      return;
    }
    else {
      st_synchronize();
      LcdUpdateDisabler disableLcdUpdate;
      lcd_clear();
      lcd_set_cursor(0, 1);
      lcd_puts_P(_i("Ejecting filament"));
      if (isEXTLoaded && can_extrude()) filament_ramming();
      mmu_command(MmuCmd::E0 + filament);
      manage_response(false, false);
      if (recover)
      {
        lcd_show_fullscreen_message_and_wait_P(_i("Please remove filament and then press the knob."));
        mmu_command(MmuCmd::R0);
        manage_response(false, false);
      }
    }
  }
  else
  {
    puts_P(PSTR("Filament nr out of range!"));
  }
}
