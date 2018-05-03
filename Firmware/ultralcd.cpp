#include "temperature.h"
#include "ultralcd.h"
#ifdef ULTRA_LCD
#include "MenuStack.h"
#include "Marlin.h"
#include "language.h"
#include "cardreader.h"
#include "temperature.h"
#include "stepper.h"
#include "ConfigurationStore.h"
#include <string.h>
#include "Timer.h"

#include "util.h"
#include "mesh_bed_leveling.h"
//#include "Configuration.h"
#include "cmdqueue.h"

#include "SdFatUtil.h"

#ifdef PAT9125
#include "pat9125.h"
#endif //PAT9125

#ifdef TMC2130
#include "tmc2130.h"
#endif //TMC2130

#define _STRINGIFY(s) #s


int8_t encoderDiff; /* encoderDiff is updated from interrupt context and added to encoderPosition every LCD update */

extern int lcd_change_fil_state;
extern bool fans_check_enabled;
extern bool filament_autoload_enabled;

#ifdef PAT9125
extern bool fsensor_not_responding;
extern bool fsensor_enabled;
#endif //PAT9125

//Function pointer to menu functions.


static void lcd_sd_updir();

struct EditMenuParentState
{
    //prevMenu and prevEncoderPosition are used to store the previous menu location when editing settings.
    menuFunc_t prevMenu;
    uint16_t prevEncoderPosition;
    //Variables used when editing values.
    const char* editLabel;
    void* editValue;
    int32_t minEditValue, maxEditValue;
    // menuFunc_t callbackFunc;
};

union MenuData
{ 
    struct BabyStep
    {
        // 29B total
        int8_t status;
        int babystepMem[3];
        float babystepMemMM[3];
    } babyStep;

    struct SupportMenu
    {
        // 6B+16B=22B total
        int8_t status;
        bool is_flash_air;
        uint8_t ip[4];
        char ip_str[3*4+3+1];
    } supportMenu;

    struct AdjustBed
    {
        // 6+13+16=35B
        // editMenuParentState is used when an edit menu is entered, so it knows
        // the return menu and encoder state.
        struct EditMenuParentState editMenuParentState;
        int8_t status;
        int8_t left;
        int8_t right;
        int8_t front;
        int8_t rear;
        int    left2;
        int    right2;
        int    front2;
        int    rear2;
    } adjustBed;

    struct TuneMenu
    {
        // editMenuParentState is used when an edit menu is entered, so it knows
        // the return menu and encoder state.
        struct EditMenuParentState editMenuParentState;
        // To recognize, whether the menu has been just initialized.
        int8_t  status;
        // Backup of extrudemultiply, to recognize, that the value has been changed and
        // it needs to be applied.
        int16_t extrudemultiply;
    } tuneMenu;

    // editMenuParentState is used when an edit menu is entered, so it knows
    // the return menu and encoder state.
    struct EditMenuParentState editMenuParentState;

    struct AutoLoadFilamentMenu
    {
        //Timer timer;
		char dummy;
    } autoLoadFilamentMenu;
    struct _Lcd_moveMenu
    {
        bool initialized;
        bool endstopsEnabledPrevious;
    } _lcd_moveMenu;
};

// State of the currently active menu.
// C Union manages sharing of the static memory by all the menus.
union MenuData menuData = { 0 };

union Data
{
  byte b[2];
  int value;
};
static MenuStack menuStack;
int8_t ReInitLCD = 0;

int8_t SDscrool = 0;

int8_t SilentModeMenu = SILENT_MODE_OFF;

int8_t FSensorStateMenu = 1;

int8_t CrashDetectMenu = 1;

extern void fsensor_block();
extern void fsensor_unblock();

extern bool fsensor_enable();
extern void fsensor_disable();

#ifdef TMC2130
extern void crashdet_enable();
extern void crashdet_disable();
#endif //TMC2130


#ifdef SNMM
uint8_t snmm_extruder = 0;
#endif

#ifdef SDCARD_SORT_ALPHA
 bool presort_flag = false;
#endif

int lcd_commands_type=LCD_COMMAND_IDLE;
int lcd_commands_step=0;
bool isPrintPaused = false;
uint8_t farm_mode = 0;
int farm_no = 0;
int farm_timer = 8;
int farm_status = 0;
unsigned long allert_timer = millis();
bool printer_connected = true;

unsigned long display_time; //just timer for showing pid finished message on lcd;
float pid_temp = DEFAULT_PID_TEMP;

bool long_press_active = false;
long long_press_timer = millis();
unsigned long button_blanking_time = millis();
bool button_pressed = false;

bool menuExiting = false;

#ifdef FILAMENT_LCD_DISPLAY
unsigned long message_millis = 0;
#endif

#ifdef ULTIPANEL
static float manual_feedrate[] = MANUAL_FEEDRATE;
#endif // ULTIPANEL

/* !Configuration settings */

uint8_t lcd_status_message_level;
char lcd_status_message[LCD_WIDTH + 1] = ""; //////WELCOME!
unsigned char firstrun = 1;

#include "ultralcd_implementation_hitachi_HD44780.h"

/** forward declarations **/

static const char* lcd_display_message_fullscreen_nonBlocking_P(const char *msg, uint8_t &nlines);
// void copy_and_scalePID_i();
// void copy_and_scalePID_d();

/* Different menus */
static void lcd_status_screen();
#ifdef ULTIPANEL
extern bool powersupply;
static void lcd_main_menu();
static void lcd_tune_menu();
static void lcd_prepare_menu();
//static void lcd_move_menu();
static void lcd_settings_menu();
static void lcd_calibration_menu();
static void lcd_language_menu();
static void lcd_control_temperature_menu();
static void lcd_control_temperature_preheat_pla_settings_menu();
static void lcd_control_temperature_preheat_abs_settings_menu();
static void lcd_control_motion_menu();
static void lcd_control_volumetric_menu();
static void lcd_settings_menu_back();

static void prusa_stat_printerstatus(int _status);
static void prusa_stat_farm_number();
static void prusa_stat_temperatures();
static void prusa_stat_printinfo();
static void lcd_farm_no();
static void lcd_menu_extruder_info();
#if defined(TMC2130) || defined(PAT9125)
static void lcd_menu_fails_stats();
#endif //TMC2130 or PAT9125

void lcd_finishstatus();

#ifdef DOGLCD
static void lcd_set_contrast();
#endif
static void lcd_control_retract_menu();
static void lcd_sdcard_menu();

#ifdef DELTA_CALIBRATION_MENU
static void lcd_delta_calibrate_menu();
#endif // DELTA_CALIBRATION_MENU

static void lcd_quick_feedback();//Cause an LCD refresh, and give the user visual or audible feedback that something has happened

/* Different types of actions that can be used in menu items. */
static void menu_action_back(menuFunc_t data = 0);
#define menu_action_back_RAM menu_action_back
static void menu_action_submenu(menuFunc_t data);
static void menu_action_gcode(const char* pgcode);
static void menu_action_function(menuFunc_t data);
static void menu_action_setlang(unsigned char lang);
static void menu_action_sdfile(const char* filename, char* longFilename);
static void menu_action_sddirectory(const char* filename, char* longFilename);
static void menu_action_setting_edit_bool(const char* pstr, bool* ptr);
static void menu_action_setting_edit_wfac(const char* pstr, uint8_t* ptr, uint8_t minValue, uint8_t maxValue);
static void menu_action_setting_edit_mres(const char* pstr, uint8_t* ptr, uint8_t minValue, uint8_t maxValue);
static void menu_action_setting_edit_byte3(const char* pstr, uint8_t* ptr, uint8_t minValue, uint8_t maxValue);
static void menu_action_setting_edit_int3(const char* pstr, int* ptr, int minValue, int maxValue);
static void menu_action_setting_edit_float3(const char* pstr, float* ptr, float minValue, float maxValue);
static void menu_action_setting_edit_float32(const char* pstr, float* ptr, float minValue, float maxValue);
static void menu_action_setting_edit_float43(const char* pstr, float* ptr, float minValue, float maxValue);
static void menu_action_setting_edit_float5(const char* pstr, float* ptr, float minValue, float maxValue);
static void menu_action_setting_edit_float51(const char* pstr, float* ptr, float minValue, float maxValue);
static void menu_action_setting_edit_float52(const char* pstr, float* ptr, float minValue, float maxValue);
static void menu_action_setting_edit_long5(const char* pstr, unsigned long* ptr, unsigned long minValue, unsigned long maxValue);

/*
static void menu_action_setting_edit_callback_bool(const char* pstr, bool* ptr, menuFunc_t callbackFunc);
static void menu_action_setting_edit_callback_int3(const char* pstr, int* ptr, int minValue, int maxValue, menuFunc_t callbackFunc);
static void menu_action_setting_edit_callback_float3(const char* pstr, float* ptr, float minValue, float maxValue, menuFunc_t callbackFunc);
static void menu_action_setting_edit_callback_float32(const char* pstr, float* ptr, float minValue, float maxValue, menuFunc_t callbackFunc);
static void menu_action_setting_edit_callback_float43(const char* pstr, float* ptr, float minValue, float maxValue, menuFunc_t callbackFunc);
static void menu_action_setting_edit_callback_float5(const char* pstr, float* ptr, float minValue, float maxValue, menuFunc_t callbackFunc);
static void menu_action_setting_edit_callback_float51(const char* pstr, float* ptr, float minValue, float maxValue, menuFunc_t callbackFunc);
static void menu_action_setting_edit_callback_float52(const char* pstr, float* ptr, float minValue, float maxValue, menuFunc_t callbackFunc);
static void menu_action_setting_edit_callback_long5(const char* pstr, unsigned long* ptr, unsigned long minValue, unsigned long maxValue, menuFunc_t callbackFunc);
*/

#define ENCODER_FEEDRATE_DEADZONE 10

#if !defined(LCD_I2C_VIKI)
#ifndef ENCODER_STEPS_PER_MENU_ITEM
#define ENCODER_STEPS_PER_MENU_ITEM 5
#endif
#ifndef ENCODER_PULSES_PER_STEP
#define ENCODER_PULSES_PER_STEP 1
#endif
#else
#ifndef ENCODER_STEPS_PER_MENU_ITEM
#define ENCODER_STEPS_PER_MENU_ITEM 2 // VIKI LCD rotary encoder uses a different number of steps per rotation
#endif
#ifndef ENCODER_PULSES_PER_STEP
#define ENCODER_PULSES_PER_STEP 1
#endif
#endif


/* Helper macros for menus */
#define START_MENU() do { \
    if (encoderPosition > 0x8000) encoderPosition = 0; \
    if (encoderPosition / ENCODER_STEPS_PER_MENU_ITEM < currentMenuViewOffset) currentMenuViewOffset = encoderPosition / ENCODER_STEPS_PER_MENU_ITEM;\
    uint8_t _lineNr = currentMenuViewOffset, _menuItemNr; \
    bool wasClicked = LCD_CLICKED;\
    for(uint8_t _drawLineNr = 0; _drawLineNr < LCD_HEIGHT; _drawLineNr++, _lineNr++) { \
      _menuItemNr = 0;

#define MENU_ITEM(type, label, args...) do { \
    if (_menuItemNr == _lineNr) { \
      if (lcdDrawUpdate) { \
        const char* _label_pstr = (label); \
        if ((encoderPosition / ENCODER_STEPS_PER_MENU_ITEM) == _menuItemNr) { \
          lcd_implementation_drawmenu_ ## type ## _selected (_drawLineNr, _label_pstr , ## args ); \
        }else{\
          lcd_implementation_drawmenu_ ## type (_drawLineNr, _label_pstr , ## args ); \
        }\
      }\
      if (wasClicked && (encoderPosition / ENCODER_STEPS_PER_MENU_ITEM) == _menuItemNr) {\
        lcd_quick_feedback(); \
        menu_action_ ## type ( args ); \
        return;\
      }\
    }\
    _menuItemNr++;\
  } while(0)

#define MENU_ITEM_DUMMY() do { _menuItemNr++; } while(0)
#define MENU_ITEM_EDIT(type, label, args...) MENU_ITEM(setting_edit_ ## type, label, (label) , ## args )
#define MENU_ITEM_EDIT_CALLBACK(type, label, args...) MENU_ITEM(setting_edit_callback_ ## type, label, (label) , ## args )
#define END_MENU() \
  if (encoderPosition / ENCODER_STEPS_PER_MENU_ITEM >= _menuItemNr) encoderPosition = _menuItemNr * ENCODER_STEPS_PER_MENU_ITEM - 1; \
  if ((uint8_t)(encoderPosition / ENCODER_STEPS_PER_MENU_ITEM) >= currentMenuViewOffset + LCD_HEIGHT) { currentMenuViewOffset = (encoderPosition / ENCODER_STEPS_PER_MENU_ITEM) - LCD_HEIGHT + 1; lcdDrawUpdate = 1; _lineNr = currentMenuViewOffset - 1; _drawLineNr = -1; } \
  } } while(0)

/** Used variables to keep track of the menu */
#ifndef REPRAPWORLD_KEYPAD
volatile uint8_t buttons;//Contains the bits of the currently pressed buttons.
#else
volatile uint8_t buttons_reprapworld_keypad; // to store the reprapworld_keypad shift register values
#endif
#ifdef LCD_HAS_SLOW_BUTTONS
volatile uint8_t slow_buttons;//Contains the bits of the currently pressed buttons.
#endif
uint8_t currentMenuViewOffset;              /* scroll offset in the current menu */
uint8_t lastEncoderBits;
uint32_t encoderPosition;
#if (SDCARDDETECT > 0)
bool lcd_oldcardstatus;
#endif
#endif //ULTIPANEL

menuFunc_t currentMenu = lcd_status_screen; /* function pointer to the currently active menu */
uint32_t lcd_next_update_millis;
uint8_t lcd_status_update_delay;
bool ignore_click = false;
bool wait_for_unclick;
uint8_t lcdDrawUpdate = 2;                  /* Set to none-zero when the LCD needs to draw, decreased after every draw. Set to 2 in LCD routines so the LCD gets at least 1 full redraw (first redraw is partial) */

// place-holders for Ki and Kd edits
#ifdef PIDTEMP
// float raw_Ki, raw_Kd;
#endif


/**
 * @brief Go to menu
 *
 * In MENU_ITEM(submenu,... ) use MENU_ITEM(back,...) or
 * menu_action_back() and menu_action_submenu() instead, otherwise menuStack will be broken.
 *
 * It is acceptable to call lcd_goto_menu(menu) directly from MENU_ITEM(function,...), if destination menu
 * is the same, from which function was called.
 *
 * @param menu target menu
 * @param encoder position in target menu
 * @param feedback
 *  * true sound feedback (click)
 *  * false no feedback
 * @param reset_menu_state
 *  * true reset menu state global union
 *  * false do not reset menu state global union
 */
static void lcd_goto_menu(menuFunc_t menu, const uint32_t encoder = 0, const bool feedback = true, bool reset_menu_state = true)
{
	asm("cli");
	if (currentMenu != menu)
	{
		currentMenu = menu;
		encoderPosition = encoder;
		asm("sei");
		if (reset_menu_state)
		{
			// Resets the global shared C union.
			// This ensures, that the menu entered will find out, that it shall initialize itself.
			memset(&menuData, 0, sizeof(menuData));
		}
		if (feedback) lcd_quick_feedback();
		// For LCD_PROGRESS_BAR re-initialize the custom characters
#if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
		lcd_set_custom_characters(menu == lcd_status_screen);
#endif
	}
	else
		asm("sei");
}

/* Main status screen. It's up to the implementation specific part to show what is needed. As this is very display dependent */

// Language selection dialog not active.
#define LANGSEL_OFF 0
// Language selection dialog modal, entered from the info screen. This is the case on firmware boot up,
// if the language index stored in the EEPROM is not valid.
#define LANGSEL_MODAL 1
// Language selection dialog entered from the Setup menu.
#define LANGSEL_ACTIVE 2
// Language selection dialog status
unsigned char langsel = LANGSEL_OFF;

void set_language_from_EEPROM() {
  unsigned char eep = eeprom_read_byte((unsigned char*)EEPROM_LANG);
  if (eep < LANG_NUM)
  {
    lang_selected = eep;
    // Language is valid, no need to enter the language selection screen.
    langsel = LANGSEL_OFF;
  }
  else
  {
    lang_selected = LANG_ID_DEFAULT;
    // Invalid language, enter the language selection screen in a modal mode.
    langsel = LANGSEL_MODAL;
  }
}

static void lcd_status_screen()
{
  if (firstrun == 1) 
  {
    firstrun = 0;
    set_language_from_EEPROM();
     
      if(lcd_status_message_level == 0){
          strncpy_P(lcd_status_message, WELCOME_MSG, LCD_WIDTH);
		lcd_finishstatus();
      }
	if (eeprom_read_byte((uint8_t *)EEPROM_TOTALTIME) == 255 && eeprom_read_byte((uint8_t *)EEPROM_TOTALTIME + 1) == 255 && eeprom_read_byte((uint8_t *)EEPROM_TOTALTIME + 2) == 255 && eeprom_read_byte((uint8_t *)EEPROM_TOTALTIME + 3) == 255)
	{
		eeprom_update_dword((uint32_t *)EEPROM_TOTALTIME, 0);
		eeprom_update_dword((uint32_t *)EEPROM_FILAMENTUSED, 0);
	}
	
	if (langsel) {
      //strncpy_P(lcd_status_message, PSTR(">>>>>>>>>>>> PRESS v"), LCD_WIDTH);
      // Entering the language selection screen in a modal mode.
      
    }
  }

  
  if (lcd_status_update_delay)
    lcd_status_update_delay--;
  else
    lcdDrawUpdate = 1;
  if (lcdDrawUpdate)
  {
    ReInitLCD++;


    if (ReInitLCD == 30) {
      lcd_implementation_init( // to maybe revive the LCD if static electricity killed it.
#if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
        currentMenu == lcd_status_screen
#endif
      );
      ReInitLCD = 0 ;
    } else {

      if ((ReInitLCD % 10) == 0) {
        //lcd_implementation_nodisplay();
        lcd_implementation_init_noclear( // to maybe revive the LCD if static electricity killed it.
#if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
          currentMenu == lcd_status_screen
#endif
        );

      }

    }


    //lcd_implementation_display();
    lcd_implementation_status_screen();
    //lcd_implementation_clear();

	if (farm_mode)
	{
		farm_timer--;
		if (farm_timer < 1)
		{
			farm_timer = 10;
			prusa_statistics(0);
		}
		switch (farm_timer)
		{
		case 8:
			prusa_statistics(21);
			break;
		case 5:
			if (IS_SD_PRINTING)
			{
				prusa_statistics(20);
			}
			break;
		}
	} // end of farm_mode





    lcd_status_update_delay = 10;   /* redraw the main screen every second. This is easier then trying keep track of all things that change on the screen */
	if (lcd_commands_type != LCD_COMMAND_IDLE)
	{
		lcd_commands();
	}
	

  } // end of lcdDrawUpdate
#ifdef ULTIPANEL

  bool current_click = LCD_CLICKED;

  if (ignore_click) {
    if (wait_for_unclick) {
      if (!current_click) {
        ignore_click = wait_for_unclick = false;
      }
      else {
        current_click = false;
      }
    }
    else if (current_click) {
      lcd_quick_feedback();
      wait_for_unclick = true;
      current_click = false;
    }
  }


  //if (--langsel ==0) {langsel=1;current_click=true;}

  if (current_click && (lcd_commands_type != LCD_COMMAND_STOP_PRINT)) //click is aborted unless stop print finishes
  {
    menuStack.reset(); //redundant, as already done in lcd_return_to_status(), just to be sure
    menu_action_submenu(lcd_main_menu);
    lcd_implementation_init( // to maybe revive the LCD if static electricity killed it.
#if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
      currentMenu == lcd_status_screen
#endif
    );
#ifdef FILAMENT_LCD_DISPLAY
    message_millis = millis();  // get status message to show up for a while
#endif
  }

#ifdef ULTIPANEL_FEEDMULTIPLY
  // Dead zone at 100% feedrate
  if ((feedmultiply < 100 && (feedmultiply + int(encoderPosition)) > 100) ||
      (feedmultiply > 100 && (feedmultiply + int(encoderPosition)) < 100))
  {
    encoderPosition = 0;
    feedmultiply = 100;
  }

  if (feedmultiply == 100 && int(encoderPosition) > ENCODER_FEEDRATE_DEADZONE)
  {
    feedmultiply += int(encoderPosition) - ENCODER_FEEDRATE_DEADZONE;
    encoderPosition = 0;
  }
  else if (feedmultiply == 100 && int(encoderPosition) < -ENCODER_FEEDRATE_DEADZONE)
  {
    feedmultiply += int(encoderPosition) + ENCODER_FEEDRATE_DEADZONE;
    encoderPosition = 0;
  }
  else if (feedmultiply != 100)
  {
    feedmultiply += int(encoderPosition);
    encoderPosition = 0;
  }
#endif //ULTIPANEL_FEEDMULTIPLY

  if (feedmultiply < 10)
    feedmultiply = 10;
  else if (feedmultiply > 999)
    feedmultiply = 999;
#endif //ULTIPANEL

  /*if (farm_mode && !printer_connected) {
	  lcd.setCursor(0, 3);
	  lcd_printPGM(MSG_PRINTER_DISCONNECTED);
  }*/


//#define FSENS_FACTOR (2580.8/50) //filament sensor factor [steps / encoder counts]
//#define FSENS_FACTOR (2580.8/45.3) //filament sensor factor [steps / encoder counts]
  //lcd.setCursor(0, 3);
  //lcd_implementation_print("                    ");
  //lcd.setCursor(0, 3);
  //lcd_implementation_print(pat9125_x);
  //lcd.setCursor(6, 3);
  //lcd_implementation_print(pat9125_y);
  //lcd.setCursor(12, 3);
  //lcd_implementation_print(pat9125_b);

}

#ifdef ULTIPANEL

void lcd_commands()
{	
	if (lcd_commands_type == LCD_COMMAND_LONG_PAUSE)
	{
		if(lcd_commands_step == 0) {
			if (card.sdprinting) {
				card.pauseSDPrint();
				lcd_setstatuspgm(MSG_FINISHING_MOVEMENTS);
				lcdDrawUpdate = 3;
				lcd_commands_step = 1;
			}
			else {
				lcd_commands_type = 0;
			}
		}
		if (lcd_commands_step == 1 && !blocks_queued() && !homing_flag) {
			lcd_setstatuspgm(MSG_PRINT_PAUSED);
			isPrintPaused = true;
			long_pause();
			lcd_commands_type = 0;
			lcd_commands_step = 0;
		}

	}

	if (lcd_commands_type == LCD_COMMAND_LONG_PAUSE_RESUME) {
		char cmd1[30];
		if (lcd_commands_step == 0) {

			lcdDrawUpdate = 3;
			lcd_commands_step = 4;
		}
		if (lcd_commands_step == 1 && !blocks_queued() && cmd_buffer_empty()) {	//recover feedmultiply; cmd_buffer_empty() ensures that card.sdprinting is synchronized with buffered commands and thus print cant be paused until resume is finished
			
			sprintf_P(cmd1, PSTR("M220 S%d"), saved_feedmultiply);
			enquecommand(cmd1);
			isPrintPaused = false;
			pause_time += (millis() - start_pause_print); //accumulate time when print is paused for correct statistics calculation
			card.startFileprint();
			lcd_commands_step = 0;
			lcd_commands_type = 0;
		}
		if (lcd_commands_step == 2 && !blocks_queued()) {	//turn on fan, move Z and unretract
			
			sprintf_P(cmd1, PSTR("M106 S%d"), fanSpeedBckp);
			enquecommand(cmd1);
			strcpy(cmd1, "G1 Z");
			strcat(cmd1, ftostr32(pause_lastpos[Z_AXIS]));
			enquecommand(cmd1);
			
			if (axis_relative_modes[3] == false) {
				enquecommand_P(PSTR("M83")); // set extruder to relative mode
			enquecommand_P(PSTR("G1 E"  STRINGIFY(DEFAULT_RETRACTION))); //unretract
				enquecommand_P(PSTR("M82")); // set extruder to absolute mode
			}
			else {
				enquecommand_P(PSTR("G1 E"  STRINGIFY(DEFAULT_RETRACTION))); //unretract
			}
			
			lcd_commands_step = 1;
		}
		if (lcd_commands_step == 3 && !blocks_queued()) {	//wait for nozzle to reach target temp
			
			strcpy(cmd1, "M109 S");
			strcat(cmd1, ftostr3(HotendTempBckp));
			enquecommand(cmd1);			
			lcd_commands_step = 2;
		}
		if (lcd_commands_step == 4 && !blocks_queued()) {	//set temperature back and move xy
			
			strcpy(cmd1, "M104 S");
			strcat(cmd1, ftostr3(HotendTempBckp));
			enquecommand(cmd1);
			enquecommand_P(PSTR("G90")); //absolute positioning
			strcpy(cmd1, "G1 X");
			strcat(cmd1, ftostr32(pause_lastpos[X_AXIS]));
			strcat(cmd1, " Y");
			strcat(cmd1, ftostr32(pause_lastpos[Y_AXIS]));
			enquecommand(cmd1);
			
			lcd_setstatuspgm(MSG_RESUMING_PRINT);
			lcd_commands_step = 3;
		}
	}

#ifdef SNMM
	if (lcd_commands_type == LCD_COMMAND_V2_CAL)
	{
		char cmd1[30];
		float width = 0.4;
		float length = 20 - width;
		float extr = count_e(0.2, width, length);
		float extr_short_segment = count_e(0.2, width, width);

		if (lcd_commands_step>1) lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS; //if user dont confirm live adjust Z value by pressing the knob, we are saving last value by timeout to status screen
		if (lcd_commands_step == 0)
		{
			lcd_commands_step = 10;
		}
		if (lcd_commands_step == 10 && !blocks_queued() && cmd_buffer_empty())
		{
			enquecommand_P(PSTR("M107"));
			enquecommand_P(PSTR("M104 S" STRINGIFY(PLA_PREHEAT_HOTEND_TEMP)));
			enquecommand_P(PSTR("M140 S" STRINGIFY(PLA_PREHEAT_HPB_TEMP)));
			enquecommand_P(PSTR("M190 S" STRINGIFY(PLA_PREHEAT_HPB_TEMP)));
			enquecommand_P(PSTR("M109 S" STRINGIFY(PLA_PREHEAT_HOTEND_TEMP)));
			enquecommand_P(PSTR("T0"));
			enquecommand_P(MSG_M117_V2_CALIBRATION);
			enquecommand_P(PSTR("G87")); //sets calibration status
			enquecommand_P(PSTR("G28"));
			enquecommand_P(PSTR("G21")); //set units to millimeters
			enquecommand_P(PSTR("G90")); //use absolute coordinates
			enquecommand_P(PSTR("M83")); //use relative distances for extrusion
			enquecommand_P(PSTR("G92 E0"));
			enquecommand_P(PSTR("M203 E100"));
			enquecommand_P(PSTR("M92 E140"));
			lcd_commands_step = 9;
		}
		if (lcd_commands_step == 9 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
			enquecommand_P(PSTR("G1 Z0.250 F7200.000"));
			enquecommand_P(PSTR("G1 X50.0 E80.0 F1000.0"));
			enquecommand_P(PSTR("G1 X160.0 E20.0 F1000.0"));
			enquecommand_P(PSTR("G1 Z0.200 F7200.000"));
			enquecommand_P(PSTR("G1 X220.0 E13 F1000.0"));
			enquecommand_P(PSTR("G1 X240.0 E0 F1000.0"));
			enquecommand_P(PSTR("G92 E0.0"));
			enquecommand_P(PSTR("G21"));
			enquecommand_P(PSTR("G90"));
			enquecommand_P(PSTR("M83"));
			enquecommand_P(PSTR("G1 E-4 F2100.00000"));
			enquecommand_P(PSTR("G1 Z0.150 F7200.000"));
			enquecommand_P(PSTR("M204 S1000"));
			enquecommand_P(PSTR("G1 F4000"));

			lcd_implementation_clear();
			lcd_goto_menu(lcd_babystep_z, 0, false);


			lcd_commands_step = 8;
		}
		if (lcd_commands_step == 8 && !blocks_queued() && cmd_buffer_empty()) //draw meander
		{
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;


			enquecommand_P(PSTR("G1 X50 Y155"));
			enquecommand_P(PSTR("G1 X60 Y155 E4"));
			enquecommand_P(PSTR("G1 F1080"));
			enquecommand_P(PSTR("G1 X75 Y155 E2.5"));
			enquecommand_P(PSTR("G1 X100 Y155 E2"));
			enquecommand_P(PSTR("G1 X200 Y155 E2.62773"));
			enquecommand_P(PSTR("G1 X200 Y135 E0.66174"));
			enquecommand_P(PSTR("G1 X50 Y135 E3.62773"));
			enquecommand_P(PSTR("G1 X50 Y115 E0.49386"));
			enquecommand_P(PSTR("G1 X200 Y115 E3.62773"));
			enquecommand_P(PSTR("G1 X200 Y95 E0.49386"));
			enquecommand_P(PSTR("G1 X50 Y95 E3.62773"));
			enquecommand_P(PSTR("G1 X50 Y75 E0.49386"));
			enquecommand_P(PSTR("G1 X200 Y75 E3.62773"));
			enquecommand_P(PSTR("G1 X200 Y55 E0.49386"));
			enquecommand_P(PSTR("G1 X50 Y55 E3.62773"));

			lcd_commands_step = 7;
		}

		if (lcd_commands_step == 7 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
			strcpy(cmd1, "G1 X50 Y35 E");
			strcat(cmd1, ftostr43(extr));
			enquecommand(cmd1);

			for (int i = 0; i < 4; i++) {
				strcpy(cmd1, "G1 X70 Y");
				strcat(cmd1, ftostr32(35 - i*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 X50 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (i + 1)*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
			}

			lcd_commands_step = 6;
		}

		if (lcd_commands_step == 6 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
			for (int i = 4; i < 8; i++) {
				strcpy(cmd1, "G1 X70 Y");
				strcat(cmd1, ftostr32(35 - i*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 X50 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (i + 1)*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
			}

			lcd_commands_step = 5;
		}

		if (lcd_commands_step == 5 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
			for (int i = 8; i < 12; i++) {
				strcpy(cmd1, "G1 X70 Y");
				strcat(cmd1, ftostr32(35 - i*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 X50 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (i + 1)*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
			}

			lcd_commands_step = 4;
		}

		if (lcd_commands_step == 4 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
			for (int i = 12; i < 16; i++) {
				strcpy(cmd1, "G1 X70 Y");
				strcat(cmd1, ftostr32(35 - i*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 X50 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (i + 1)*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
			}

			lcd_commands_step = 3;
		}

		if (lcd_commands_step == 3 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
			enquecommand_P(PSTR("G1 E-0.07500 F2100.00000"));
			enquecommand_P(PSTR("G4 S0"));
			enquecommand_P(PSTR("G1 E-4 F2100.00000"));
			enquecommand_P(PSTR("G1 Z0.5 F7200.000"));
			enquecommand_P(PSTR("G1 X245 Y1"));
			enquecommand_P(PSTR("G1 X240 E4"));
			enquecommand_P(PSTR("G1 F4000"));
			enquecommand_P(PSTR("G1 X190 E2.7"));
			enquecommand_P(PSTR("G1 F4600"));
			enquecommand_P(PSTR("G1 X110 E2.8"));
			enquecommand_P(PSTR("G1 F5200"));
			enquecommand_P(PSTR("G1 X40 E3"));
			enquecommand_P(PSTR("G1 E-15.0000 F5000"));
			enquecommand_P(PSTR("G1 E-50.0000 F5400"));
			enquecommand_P(PSTR("G1 E-15.0000 F3000"));
			enquecommand_P(PSTR("G1 E-12.0000 F2000"));
			enquecommand_P(PSTR("G1 F1600"));

			lcd_commands_step = 2;
		}
		if (lcd_commands_step == 2 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;

			enquecommand_P(PSTR("G1 X0 Y1 E3.0000"));
			enquecommand_P(PSTR("G1 X50 Y1 E-5.0000"));
			enquecommand_P(PSTR("G1 F2000"));
			enquecommand_P(PSTR("G1 X0 Y1 E5.0000"));
			enquecommand_P(PSTR("G1 X50 Y1 E-5.0000"));
			enquecommand_P(PSTR("G1 F2400"));
			enquecommand_P(PSTR("G1 X0 Y1 E5.0000"));
			enquecommand_P(PSTR("G1 X50 Y1 E-5.0000"));
			enquecommand_P(PSTR("G1 F2400"));
			enquecommand_P(PSTR("G1 X0 Y1 E5.0000"));
			enquecommand_P(PSTR("G1 X50 Y1 E-3.0000"));
			enquecommand_P(PSTR("G4 S0"));
			enquecommand_P(PSTR("M107"));
			enquecommand_P(PSTR("M104 S0"));
			enquecommand_P(PSTR("M140 S0"));
			enquecommand_P(PSTR("G1 X10 Y180 F4000"));
			enquecommand_P(PSTR("G1 Z10 F1300.000"));
			enquecommand_P(PSTR("M84"));

			lcd_commands_step = 1;

		}

		if (lcd_commands_step == 1 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_setstatuspgm(WELCOME_MSG);
			lcd_commands_step = 0;
			lcd_commands_type = 0;
			if (eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE) == 1) {
				lcd_wizard(10);
			}
		}

	}

#else //if not SNMM

	if (lcd_commands_type == LCD_COMMAND_V2_CAL)
	{
		char cmd1[30];
		float width = 0.4;
		float length = 20 - width;
		float extr = count_e(0.2, width, length);
		float extr_short_segment = count_e(0.2, width, width);
		if(lcd_commands_step>1) lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS; //if user dont confirm live adjust Z value by pressing the knob, we are saving last value by timeout to status screen
		if (lcd_commands_step == 0)
		{
			lcd_commands_step = 9;
		}
		if (lcd_commands_step == 9 && !blocks_queued() && cmd_buffer_empty())
		{
			enquecommand_P(PSTR("M107"));
			enquecommand_P(PSTR("M104 S" STRINGIFY(PLA_PREHEAT_HOTEND_TEMP)));
			enquecommand_P(PSTR("M140 S" STRINGIFY(PLA_PREHEAT_HPB_TEMP)));
			enquecommand_P(PSTR("M190 S" STRINGIFY(PLA_PREHEAT_HPB_TEMP)));
			enquecommand_P(PSTR("M109 S" STRINGIFY(PLA_PREHEAT_HOTEND_TEMP)));
			enquecommand_P(MSG_M117_V2_CALIBRATION);
			enquecommand_P(PSTR("G28"));
			enquecommand_P(PSTR("G92 E0.0"));
			lcd_commands_step = 8;
		}
		if (lcd_commands_step == 8 && !blocks_queued() && cmd_buffer_empty())
		{

			lcd_implementation_clear();
			menuStack.reset();
			menu_action_submenu(lcd_babystep_z);
			enquecommand_P(PSTR("G1 X60.0 E9.0 F1000.0")); //intro line
			enquecommand_P(PSTR("G1 X100.0 E12.5 F1000.0")); //intro line			
			enquecommand_P(PSTR("G92 E0.0"));
			enquecommand_P(PSTR("G21")); //set units to millimeters
			enquecommand_P(PSTR("G90")); //use absolute coordinates
			enquecommand_P(PSTR("M83")); //use relative distances for extrusion
			enquecommand_P(PSTR("G1 E-1.50000 F2100.00000"));
			enquecommand_P(PSTR("G1 Z0.150 F7200.000"));
			enquecommand_P(PSTR("M204 S1000")); //set acceleration
			enquecommand_P(PSTR("G1 F4000"));
			lcd_commands_step = 7;
		}
		if (lcd_commands_step == 7 && !blocks_queued() && cmd_buffer_empty()) //draw meander
		{
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;


			//just opposite direction
			/*enquecommand_P(PSTR("G1 X50 Y55"));
			enquecommand_P(PSTR("G1 F1080"));
			enquecommand_P(PSTR("G1 X200 Y55 E3.62773"));
			enquecommand_P(PSTR("G1 X200 Y75 E0.49386"));
			enquecommand_P(PSTR("G1 X50 Y75 E3.62773"));
			enquecommand_P(PSTR("G1 X50 Y95 E0.49386"));
			enquecommand_P(PSTR("G1 X200 Y95 E3.62773"));
			enquecommand_P(PSTR("G1 X200 Y115 E0.49386"));
			enquecommand_P(PSTR("G1 X50 Y115 E3.62773"));
			enquecommand_P(PSTR("G1 X50 Y135 E0.49386"));
			enquecommand_P(PSTR("G1 X200 Y135 E3.62773"));
			enquecommand_P(PSTR("G1 X200 Y155 E0.66174"));
			enquecommand_P(PSTR("G1 X100 Y155 E2.62773"));
			enquecommand_P(PSTR("G1 X75 Y155 E2"));
			enquecommand_P(PSTR("G1 X50 Y155 E2.5"));
			enquecommand_P(PSTR("G1 E - 0.07500 F2100.00000"));*/


			enquecommand_P(PSTR("G1 X50 Y155"));
			enquecommand_P(PSTR("G1 F1080"));
			enquecommand_P(PSTR("G1 X75 Y155 E2.5"));
			enquecommand_P(PSTR("G1 X100 Y155 E2"));
			enquecommand_P(PSTR("G1 X200 Y155 E2.62773"));
			enquecommand_P(PSTR("G1 X200 Y135 E0.66174"));
			enquecommand_P(PSTR("G1 X50 Y135 E3.62773"));
			enquecommand_P(PSTR("G1 X50 Y115 E0.49386"));
			enquecommand_P(PSTR("G1 X200 Y115 E3.62773"));
			enquecommand_P(PSTR("G1 X200 Y95 E0.49386"));
			enquecommand_P(PSTR("G1 X50 Y95 E3.62773"));
			enquecommand_P(PSTR("G1 X50 Y75 E0.49386"));
			enquecommand_P(PSTR("G1 X200 Y75 E3.62773"));
			enquecommand_P(PSTR("G1 X200 Y55 E0.49386"));
			enquecommand_P(PSTR("G1 X50 Y55 E3.62773"));

			strcpy(cmd1, "G1 X50 Y35 E");
			strcat(cmd1, ftostr43(extr));
			enquecommand(cmd1);

			lcd_commands_step = 6;
		}

		if (lcd_commands_step == 6 && !blocks_queued() && cmd_buffer_empty())
		{

			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;

			for (int i = 0; i < 4; i++) {
				strcpy(cmd1, "G1 X70 Y");
				strcat(cmd1, ftostr32(35 - i*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 X50 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (i + 1)*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
			}

			lcd_commands_step = 5;
		}

		if (lcd_commands_step == 5 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
			for (int i = 4; i < 8; i++) {
				strcpy(cmd1, "G1 X70 Y");
				strcat(cmd1, ftostr32(35 - i*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 X50 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (i + 1)*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
			}

			lcd_commands_step = 4;
		}

		if (lcd_commands_step == 4 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
			for (int i = 8; i < 12; i++) {
				strcpy(cmd1, "G1 X70 Y");
				strcat(cmd1, ftostr32(35 - i*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 X50 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (i + 1)*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
			}

			lcd_commands_step = 3;
		}

		if (lcd_commands_step == 3 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
			for (int i = 12; i < 16; i++) {
				strcpy(cmd1, "G1 X70 Y");
				strcat(cmd1, ftostr32(35 - i*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 X50 Y");
				strcat(cmd1, ftostr32(35 - (2 * i + 1)*width));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr));
				enquecommand(cmd1);
				strcpy(cmd1, "G1 Y");
				strcat(cmd1, ftostr32(35 - (i + 1)*width * 2));
				strcat(cmd1, " E");
				strcat(cmd1, ftostr43(extr_short_segment));
				enquecommand(cmd1);
			}

			lcd_commands_step = 2;
		}

		if (lcd_commands_step == 2 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
			enquecommand_P(PSTR("G1 E-0.07500 F2100.00000"));
			enquecommand_P(PSTR("M107")); //turn off printer fan
			enquecommand_P(PSTR("M104 S0")); // turn off temperature
			enquecommand_P(PSTR("M140 S0")); // turn off heatbed
			enquecommand_P(PSTR("G1 Z10 F1300.000"));
			enquecommand_P(PSTR("G1 X10 Y180 F4000")); //home X axis
			enquecommand_P(PSTR("M84"));// disable motors
			lcd_timeoutToStatus = millis() - 1; //if user dont confirm live adjust Z value by pressing the knob, we are saving last value by timeout to status screen
			lcd_commands_step = 1;
		}
		if (lcd_commands_step == 1 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_setstatuspgm(WELCOME_MSG);
			lcd_commands_step = 0;
			lcd_commands_type = 0;			
			if (eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE) == 1) {
				lcd_wizard(10);
			}
		}

	}

#endif // not SNMM

	if (lcd_commands_type == LCD_COMMAND_STOP_PRINT)   /// stop print
	{
		

		if (lcd_commands_step == 0) 
		{ 
			lcd_commands_step = 6; 
			custom_message = true;	
		}

		if (lcd_commands_step == 1 && !blocks_queued())
		{
			lcd_commands_step = 0;
			lcd_commands_type = 0;
			lcd_setstatuspgm(WELCOME_MSG);
			custom_message_type = 0;
			custom_message = false;
			isPrintPaused = false;
		}
		if (lcd_commands_step == 2 && !blocks_queued())
		{
			setTargetBed(0);
			enquecommand_P(PSTR("M104 S0")); //set hotend temp to 0

			manage_heater();
			lcd_setstatuspgm(WELCOME_MSG);
			cancel_heatup = false;
			lcd_commands_step = 1;
		}
		if (lcd_commands_step == 3 && !blocks_queued())
		{
      // M84: Disable steppers.
			enquecommand_P(PSTR("M84"));
			autotempShutdown();
			lcd_commands_step = 2;
		}
		if (lcd_commands_step == 4 && !blocks_queued())
		{
			lcd_setstatuspgm(MSG_PLEASE_WAIT);
      // G90: Absolute positioning.
			enquecommand_P(PSTR("G90"));
      // M83: Set extruder to relative mode.
			enquecommand_P(PSTR("M83"));
			#ifdef X_CANCEL_POS 
			enquecommand_P(PSTR("G1 X"  STRINGIFY(X_CANCEL_POS) " Y" STRINGIFY(Y_CANCEL_POS) " E0 F7000"));
			#else
			enquecommand_P(PSTR("G1 X50 Y" STRINGIFY(Y_MAX_POS) " E0 F7000"));
			#endif
			lcd_ignore_click(false);
			#ifdef SNMM
			lcd_commands_step = 8;
			#else
			lcd_commands_step = 3;
			#endif
		}
		if (lcd_commands_step == 5 && !blocks_queued())
		{
			lcd_setstatuspgm(MSG_PRINT_ABORTED);
      // G91: Set to relative positioning.
			enquecommand_P(PSTR("G91"));
      // Lift up.
			enquecommand_P(PSTR("G1 Z15 F1500"));
			if (axis_known_position[X_AXIS] && axis_known_position[Y_AXIS]) lcd_commands_step = 4;
			else lcd_commands_step = 3;
		}
		if (lcd_commands_step == 6 && !blocks_queued())
		{
			lcd_setstatuspgm(MSG_PRINT_ABORTED);
			cancel_heatup = true;
			setTargetBed(0);
			#ifndef SNMM
			setTargetHotend(0, 0);	//heating when changing filament for multicolor
			setTargetHotend(0, 1);
			setTargetHotend(0, 2);
			#endif
			manage_heater();
			custom_message = true;
			custom_message_type = 2;
			lcd_commands_step = 5;
		}
		if (lcd_commands_step == 7 && !blocks_queued()) {
			switch(snmm_stop_print_menu()) {
				case 0: enquecommand_P(PSTR("M702")); break;//all 
				case 1: enquecommand_P(PSTR("M702 U")); break; //used
				case 2: enquecommand_P(PSTR("M702 C")); break; //current
				default: enquecommand_P(PSTR("M702")); break;
			}
			lcd_commands_step = 3;
		}
		if (lcd_commands_step == 8 && !blocks_queued()) { //step 8 is here for delay (going to next step after execution of all gcodes from step 4)
			lcd_commands_step = 7; 
		}
	}

	if (lcd_commands_type == 3)
	{
		lcd_commands_type = 0;
	}

	if (lcd_commands_type == LCD_COMMAND_FARM_MODE_CONFIRM)   /// farm mode confirm
	{

		if (lcd_commands_step == 0) { lcd_commands_step = 6; custom_message = true; }

		if (lcd_commands_step == 1 && !blocks_queued())
		{
			lcd_confirm_print();
			lcd_commands_step = 0;
			lcd_commands_type = 0;
		}
		if (lcd_commands_step == 2 && !blocks_queued())
		{
			lcd_commands_step = 1;
		}
		if (lcd_commands_step == 3 && !blocks_queued())
		{
			lcd_commands_step = 2;
		}
		if (lcd_commands_step == 4 && !blocks_queued())
		{
			enquecommand_P(PSTR("G90"));
			enquecommand_P(PSTR("G1 X"  STRINGIFY(X_CANCEL_POS) " Y" STRINGIFY(Y_CANCEL_POS) " E0 F7000"));
			lcd_commands_step = 3;
		}
		if (lcd_commands_step == 5 && !blocks_queued())
		{
			lcd_commands_step = 4;
		}
		if (lcd_commands_step == 6 && !blocks_queued())
		{
			enquecommand_P(PSTR("G91"));
			enquecommand_P(PSTR("G1 Z15 F1500"));
			st_synchronize();
			#ifdef SNMM
			lcd_commands_step = 7;
			#else
			lcd_commands_step = 5;
			#endif
		}

	}
	if (lcd_commands_type == LCD_COMMAND_PID_EXTRUDER) {
		char cmd1[30];
		
		if (lcd_commands_step == 0) {
			custom_message_type = 3;
			custom_message_state = 1;
			custom_message = true;
			lcdDrawUpdate = 3;
			lcd_commands_step = 3;
		}
		if (lcd_commands_step == 3 && !blocks_queued()) { //PID calibration
			strcpy(cmd1, "M303 E0 S");
			strcat(cmd1, ftostr3(pid_temp));
			enquecommand(cmd1);
			lcd_setstatuspgm(MSG_PID_RUNNING);
			lcd_commands_step = 2;
		}
		if (lcd_commands_step == 2 && pid_tuning_finished) { //saving to eeprom
			pid_tuning_finished = false;
			custom_message_state = 0;
			lcd_setstatuspgm(MSG_PID_FINISHED);
			if (_Kp != 0 || _Ki != 0 || _Kd != 0) {
			strcpy(cmd1, "M301 P");
			strcat(cmd1, ftostr32(_Kp));
			strcat(cmd1, " I");
			strcat(cmd1, ftostr32(_Ki));
			strcat(cmd1, " D");
			strcat(cmd1, ftostr32(_Kd));
			enquecommand(cmd1);
			enquecommand_P(PSTR("M500"));
			}
			else {
				SERIAL_ECHOPGM("Invalid PID cal. results. Not stored to EEPROM.");
			}
			display_time = millis();
			lcd_commands_step = 1;
		}
		if ((lcd_commands_step == 1) && ((millis()- display_time)>2000)) { //calibration finished message
			lcd_setstatuspgm(WELCOME_MSG);
			custom_message_type = 0;
			custom_message = false;
			pid_temp = DEFAULT_PID_TEMP;
			lcd_commands_step = 0;
			lcd_commands_type = 0;
		}
	}


}

static float count_e(float layer_heigth, float extrusion_width, float extrusion_length) {
	//returns filament length in mm which needs to be extrude to form line with extrusion_length * extrusion_width * layer heigth dimensions
	float extr = extrusion_length * layer_heigth * extrusion_width / (M_PI * pow(1.75, 2) / 4);
	return extr;
}

static void lcd_return_to_status() {
  lcd_implementation_init( // to maybe revive the LCD if static electricity killed it.
#if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
    currentMenu == lcd_status_screen
#endif
  );

    lcd_goto_menu(lcd_status_screen, 0, false);
    menuStack.reset();
}


void lcd_sdcard_pause() {
	lcd_return_to_status();
	lcd_commands_type = LCD_COMMAND_LONG_PAUSE;

}

static void lcd_sdcard_resume() {
	lcd_return_to_status();
	lcd_reset_alert_level(); //for fan speed error
	lcd_commands_type = LCD_COMMAND_LONG_PAUSE_RESUME;
}

float move_menu_scale;
static void lcd_move_menu_axis();



/* Menu implementation */

void lcd_preheat_farm()
{
  setTargetHotend0(FARM_PREHEAT_HOTEND_TEMP);
  setTargetBed(FARM_PREHEAT_HPB_TEMP);
  fanSpeed = 0;
  lcd_return_to_status();
  setWatch(); // heater sanity check timer
}

void lcd_preheat_farm_nozzle()
{
	setTargetHotend0(FARM_PREHEAT_HOTEND_TEMP);
	setTargetBed(0);
	fanSpeed = 0;
	lcd_return_to_status();
	setWatch(); // heater sanity check timer
}

void lcd_preheat_pla()
{
  setTargetHotend0(PLA_PREHEAT_HOTEND_TEMP);
  setTargetBed(PLA_PREHEAT_HPB_TEMP);
  fanSpeed = 0;
  lcd_return_to_status();
  setWatch(); // heater sanity check timer
}

void lcd_preheat_abs()
{
  setTargetHotend0(ABS_PREHEAT_HOTEND_TEMP);
  setTargetBed(ABS_PREHEAT_HPB_TEMP);
  fanSpeed = 0;
  lcd_return_to_status();
  setWatch(); // heater sanity check timer
}

void lcd_preheat_pp()
{
  setTargetHotend0(PP_PREHEAT_HOTEND_TEMP);
  setTargetBed(PP_PREHEAT_HPB_TEMP);
  fanSpeed = 0;
  lcd_return_to_status();
  setWatch(); // heater sanity check timer
}

void lcd_preheat_pet()
{
  setTargetHotend0(PET_PREHEAT_HOTEND_TEMP);
  setTargetBed(PET_PREHEAT_HPB_TEMP);
  fanSpeed = 0;
  lcd_return_to_status();
  setWatch(); // heater sanity check timer
}

void lcd_preheat_hips()
{
  setTargetHotend0(HIPS_PREHEAT_HOTEND_TEMP);
  setTargetBed(HIPS_PREHEAT_HPB_TEMP);
  fanSpeed = 0;
  lcd_return_to_status();
  setWatch(); // heater sanity check timer
}

void lcd_preheat_flex()
{
  setTargetHotend0(FLEX_PREHEAT_HOTEND_TEMP);
  setTargetBed(FLEX_PREHEAT_HPB_TEMP);
  fanSpeed = 0;
  lcd_return_to_status();
  setWatch(); // heater sanity check timer
}


void lcd_cooldown()
{
  setTargetHotend0(0);
  setTargetHotend1(0);
  setTargetHotend2(0);
  setTargetBed(0);
  fanSpeed = 0;
  lcd_return_to_status();
}


static void lcd_menu_extruder_info()
{
    int fan_speed_RPM[2];
    
#ifdef PAT9125
    pat9125_update();
#endif //PAT9125
    
    fan_speed_RPM[0] = 60*fan_speed[0];
    fan_speed_RPM[1] = 60*fan_speed[1];
    
    // Display Nozzle fan RPM
    
    lcd.setCursor(0, 0);
    lcd_printPGM(MSG_INFO_NOZZLE_FAN);
    
    lcd.setCursor(11, 0);
    lcd.print("         ");
    lcd.setCursor(12, 0);
    lcd.print(itostr4(fan_speed_RPM[0]));
    lcd.print(" RPM");
    
    // Display Nozzle fan RPM
    
#if (defined(TACH_1))
    lcd.setCursor(0, 1);
    lcd_printPGM(MSG_INFO_PRINT_FAN);
    
    lcd.setCursor(11, 1);
    lcd.print("         ");
    lcd.setCursor(12, 1);
    lcd.print(itostr4(fan_speed_RPM[1]));
    lcd.print(" RPM");
#endif
    
#ifdef PAT9125
	// Display X and Y difference from Filament sensor    
    lcd.setCursor(0, 2);
    lcd.print("Fil. Xd:");
    lcd.print(itostr3(pat9125_x));
    lcd.print("   ");
    lcd.setCursor(12, 2);
    lcd.print("Yd:");
    lcd.print(itostr3(pat9125_y));
    
    // Display Light intensity from Filament sensor
    /* Frame_Avg register represents the average brightness of all pixels within a frame (324 pixels). This
     value ranges from 0(darkest) to 255(brightest). */
    lcd.setCursor(0, 3);
    
    lcd.print("Int:             ");
    lcd.setCursor(5, 3);
    lcd.print(itostr3(pat9125_b));
    
    // Display LASER shutter time from Filament sensor
    /* Shutter register is an index of LASER shutter time. It is automatically controlled by the chip's internal
     auto-exposure algorithm. When the chip is tracking on a good reflection surface, the Shutter is small.
     When the chip is tracking on a poor reflection surface, the Shutter is large. Value ranges from 0 to
     46. */
    
    lcd.setCursor(10, 3);
    
    lcd.print("Shut:    ");
    lcd.setCursor(15, 3);
    lcd.print(itostr3(pat9125_s));
#endif //PAT9125

    
    if (lcd_clicked())
    {
        lcd_quick_feedback();
        lcd_return_to_status();
    }
}

#if defined(TMC2130) && defined(PAT9125)
static void lcd_menu_fails_stats_total()
{
//01234567890123456789
//Total failures
// Power failures  000
// Filam. runouts  000
// Crash  X 000  Y 000
//////////////////////
    uint16_t power = eeprom_read_word((uint16_t*)EEPROM_POWER_COUNT_TOT);
    uint16_t filam = eeprom_read_word((uint16_t*)EEPROM_FERROR_COUNT_TOT);
    uint16_t crashX = eeprom_read_word((uint16_t*)EEPROM_CRASH_COUNT_X_TOT);
    uint16_t crashY = eeprom_read_word((uint16_t*)EEPROM_CRASH_COUNT_Y_TOT);
	fprintf_P(lcdout, PSTR(ESC_H(0,0)"Total failures"ESC_H(1,1)"Power failures  %-3d"ESC_H(1,2)"Filam. runouts  %-3d"ESC_H(1,3)"Crash  X %-3d  Y %-3d"), power, filam, crashX, crashY);
	if (lcd_clicked())
    {
        lcd_quick_feedback();
        menu_action_back();
    }
}

static void lcd_menu_fails_stats_print()
{
//01234567890123456789
//Last print failures
// Power failures  000
// Filam. runouts  000
// Crash  X 000  Y 000
//////////////////////
    uint8_t power = eeprom_read_byte((uint8_t*)EEPROM_POWER_COUNT);
    uint8_t filam = eeprom_read_byte((uint8_t*)EEPROM_FERROR_COUNT);
    uint8_t crashX = eeprom_read_byte((uint8_t*)EEPROM_CRASH_COUNT_X);
    uint8_t crashY = eeprom_read_byte((uint8_t*)EEPROM_CRASH_COUNT_Y);
	fprintf_P(lcdout, PSTR(ESC_H(0,0)"Last print failures"ESC_H(1,1)"Power failures  %-3d"ESC_H(1,2)"Filam. runouts  %-3d"ESC_H(1,3)"Crash  X %-3d  Y %-3d"), power, filam, crashX, crashY);
	if (lcd_clicked())
    {
        lcd_quick_feedback();
		menu_action_back();
    }    
}
/**
 * @brief Open fail statistics menu
 *
 * This version of function is used, when there is filament sensor,
 * power failure and crash detection.
 * There are Last print and Total menu items.
 */
static void lcd_menu_fails_stats()
{
	START_MENU();
	MENU_ITEM(back, MSG_MAIN, 0);
	MENU_ITEM(submenu, PSTR("Last print"), lcd_menu_fails_stats_print);
	MENU_ITEM(submenu, PSTR("Total"), lcd_menu_fails_stats_total);
	END_MENU();
}
#else if defined(PAT9125)
/**
 * @brief Print last print and total filament run outs
 *
 * This version of function is used, when there is filament sensor,
 * but no other sensors (e.g. power failure, crash detection).
 *
 * Example screen:
 * @code
 * 01234567890123456789
 * Last print failures
 *  Filam. runouts  0
 * Total failures
 *  Filam. runouts  5
 * @endcode
 */
static void lcd_menu_fails_stats()
{
    uint8_t filamentLast = eeprom_read_byte((uint8_t*)EEPROM_FERROR_COUNT);
    uint16_t filamentTotal = eeprom_read_word((uint16_t*)EEPROM_FERROR_COUNT_TOT);
    fprintf_P(lcdout, PSTR(ESC_H(0,0)"Last print failures"ESC_H(1,1)"Filam. runouts  %-3d"ESC_H(0,2)"Total failures"ESC_H(1,3)"Filam. runouts  %-3d"), filamentLast, filamentTotal);
    if (lcd_clicked())
    {
        menu_action_back();
    }
}
#endif //TMC2130


#ifdef DEBUG_BUILD
#ifdef DEBUG_STACK_MONITOR
extern uint16_t SP_min;
extern char* __malloc_heap_start;
extern char* __malloc_heap_end;
#endif //DEBUG_STACK_MONITOR

static void lcd_menu_debug()
{
#ifdef DEBUG_STACK_MONITOR
	fprintf_P(lcdout, PSTR(ESC_H(1,1)"RAM statistics"ESC_H(5,1)"SP_min: 0x%04x"ESC_H(1,2)"heap_start: 0x%04x"ESC_H(3,3)"heap_end: 0x%04x"), SP_min, __malloc_heap_start, __malloc_heap_end);
#endif //DEBUG_STACK_MONITOR

	if (lcd_clicked())
    {
        lcd_quick_feedback();
        lcd_return_to_status();
    }
}
#endif /* DEBUG_BUILD */

static void lcd_menu_temperatures()
{
	fprintf_P(lcdout, PSTR(ESC_H(1,0)"Nozzle:   %d%c" ESC_H(1,1)"Bed:      %d%c"), (int)current_temperature[0], '\x01', (int)current_temperature_bed, '\x01');
#ifdef AMBIENT_THERMISTOR
	fprintf_P(lcdout, PSTR(ESC_H(1,2)"Ambient:  %d%c" ESC_H(1,3)"PINDA:    %d%c"), (int)current_temperature_ambient, '\x01', (int)current_temperature_pinda, '\x01');
#else //AMBIENT_THERMISTOR
	fprintf_P(lcdout, PSTR(ESC_H(1,2)"PINDA:    %d%c"), (int)current_temperature_pinda, '\x01');
#endif //AMBIENT_THERMISTOR

	if (lcd_clicked())
    {
        lcd_quick_feedback();
        lcd_return_to_status();
    }
}

#if defined (VOLT_BED_PIN) || defined (VOLT_PWR_PIN)
#define VOLT_DIV_R1 10000
#define VOLT_DIV_R2 2370
#define VOLT_DIV_FAC ((float)VOLT_DIV_R2 / (VOLT_DIV_R2 + VOLT_DIV_R1))
#define VOLT_DIV_REF 5
static void lcd_menu_voltages()
{
	float volt_pwr = VOLT_DIV_REF * ((float)current_voltage_raw_pwr / (1023 * OVERSAMPLENR)) / VOLT_DIV_FAC;
	//float volt_bed = VOLT_DIV_REF * ((float)current_voltage_raw_bed / (1023 * OVERSAMPLENR)) / VOLT_DIV_FAC;
	//fprintf_P(lcdout, PSTR(ESC_H(1,1)"PWR:      %d.%01dV" ESC_H(1,2)"BED:      %d.%01dV"), (int)volt_pwr, (int)(10*fabs(volt_pwr - (int)volt_pwr)), (int)volt_bed, (int)(10*fabs(volt_bed - (int)volt_bed)));
    fprintf_P(lcdout, PSTR( ESC_H(1,1)"PWR:      %d.%01dV"), (int)volt_pwr, (int)(10*fabs(volt_pwr - (int)volt_pwr))) ;
    if (lcd_clicked())
    {
        lcd_quick_feedback();
        lcd_return_to_status();
    }
}
#endif //defined VOLT_BED_PIN || defined VOLT_PWR_PIN

#ifdef TMC2130
static void lcd_menu_belt_status()
{
    fprintf_P(lcdout, PSTR(ESC_H(1,0) "Belt status" ESC_H(2,1) "X %d" ESC_H(2,2) "Y %d" ), eeprom_read_word((uint16_t*)(EEPROM_BELTSTATUS_X)), eeprom_read_word((uint16_t*)(EEPROM_BELTSTATUS_Y)));
    if (lcd_clicked())
    {
        lcd_quick_feedback();
        lcd_return_to_status();
    }
}
#endif //TMC2130

extern void stop_and_save_print_to_ram(float z_move, float e_move);
extern void restore_print_from_ram_and_continue(float e_move);

static void lcd_menu_test_save()
{
	stop_and_save_print_to_ram(10, -0.8);
}

static void lcd_menu_test_restore()
{
	restore_print_from_ram_and_continue(0.8);
}

static void lcd_preheat_menu()
{
  START_MENU();

  MENU_ITEM(back, MSG_MAIN, 0);

  if (farm_mode) {
	  MENU_ITEM(function, PSTR("farm   -  " STRINGIFY(FARM_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(FARM_PREHEAT_HPB_TEMP)), lcd_preheat_farm);
	  MENU_ITEM(function, PSTR("nozzle -  " STRINGIFY(FARM_PREHEAT_HOTEND_TEMP) "/0"), lcd_preheat_farm_nozzle);
	  MENU_ITEM(function, MSG_COOLDOWN, lcd_cooldown);
	  MENU_ITEM(function, PSTR("ABS    -  " STRINGIFY(ABS_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(ABS_PREHEAT_HPB_TEMP)), lcd_preheat_abs);
  } else {
	  MENU_ITEM(function, PSTR("PLA  -  " STRINGIFY(PLA_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(PLA_PREHEAT_HPB_TEMP)), lcd_preheat_pla);
	  MENU_ITEM(function, PSTR("PET  -  " STRINGIFY(PET_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(PET_PREHEAT_HPB_TEMP)), lcd_preheat_pet);
	  MENU_ITEM(function, PSTR("ABS  -  " STRINGIFY(ABS_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(ABS_PREHEAT_HPB_TEMP)), lcd_preheat_abs);
	  MENU_ITEM(function, PSTR("HIPS -  " STRINGIFY(HIPS_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(HIPS_PREHEAT_HPB_TEMP)), lcd_preheat_hips);
	  MENU_ITEM(function, PSTR("PP   -  " STRINGIFY(PP_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(PP_PREHEAT_HPB_TEMP)), lcd_preheat_pp);
	  MENU_ITEM(function, PSTR("FLEX -  " STRINGIFY(FLEX_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(FLEX_PREHEAT_HPB_TEMP)), lcd_preheat_flex);
	  MENU_ITEM(function, MSG_COOLDOWN, lcd_cooldown);
  }
  

  END_MENU();
}

static void lcd_support_menu()
{
    if (menuData.supportMenu.status == 0 || lcdDrawUpdate == 2) {
        // Menu was entered or SD card status has changed (plugged in or removed).
        // Initialize its status.
        menuData.supportMenu.status = 1;
        menuData.supportMenu.is_flash_air = card.ToshibaFlashAir_isEnabled() && card.ToshibaFlashAir_GetIP(menuData.supportMenu.ip);
        if (menuData.supportMenu.is_flash_air)
            sprintf_P(menuData.supportMenu.ip_str, PSTR("%d.%d.%d.%d"), 
                menuData.supportMenu.ip[0], menuData.supportMenu.ip[1], 
                menuData.supportMenu.ip[2], menuData.supportMenu.ip[3]);
    } else if (menuData.supportMenu.is_flash_air && 
        menuData.supportMenu.ip[0] == 0 && menuData.supportMenu.ip[1] == 0 && 
        menuData.supportMenu.ip[2] == 0 && menuData.supportMenu.ip[3] == 0 &&
        ++ menuData.supportMenu.status == 16) {
        // Waiting for the FlashAir card to get an IP address from a router. Force an update.
        menuData.supportMenu.status = 0;
    }

  START_MENU();

  MENU_ITEM(back, MSG_MAIN, 0);

  MENU_ITEM(back, PSTR("Firmware:"), 0);
  MENU_ITEM(back, PSTR(" " FW_VERSION_FULL), 0);
#if (FW_DEV_VERSION != FW_VERSION_GOLD) && (FW_DEV_VERSION != FW_VERSION_RC)
  MENU_ITEM(back, PSTR(" repo " FW_REPOSITORY), 0);
#endif
  // Ideally this block would be optimized out by the compiler.
/*  const uint8_t fw_string_len = strlen_P(FW_VERSION_STR_P());
  if (fw_string_len < 6) {
      MENU_ITEM(back, PSTR(MSG_FW_VERSION " - " FW_version), 0);
  } else {
      MENU_ITEM(back, PSTR("FW - " FW_version), 0);
  }*/
      
  MENU_ITEM(back, MSG_PRUSA3D, 0);
  MENU_ITEM(back, MSG_PRUSA3D_FORUM, 0);
  MENU_ITEM(back, MSG_PRUSA3D_HOWTO, 0);
  MENU_ITEM(back, PSTR("------------"), 0);
  MENU_ITEM(back, PSTR(FILAMENT_SIZE), 0);
  MENU_ITEM(back, PSTR(ELECTRONICS),0);
  MENU_ITEM(back, PSTR(NOZZLE_TYPE),0);
  MENU_ITEM(back, PSTR("------------"), 0);
  MENU_ITEM(back, MSG_DATE, 0);
  MENU_ITEM(back, PSTR(__DATE__), 0);

  // Show the FlashAir IP address, if the card is available.
  if (menuData.supportMenu.is_flash_air) {
      MENU_ITEM(back, PSTR("------------"), 0);
      MENU_ITEM(back, PSTR("FlashAir IP Addr:"), 0);
      MENU_ITEM(back_RAM, menuData.supportMenu.ip_str, 0);
  }
  #ifndef MK1BP
  MENU_ITEM(back, PSTR("------------"), 0);
  if (!IS_SD_PRINTING && !is_usb_printing && (lcd_commands_type != LCD_COMMAND_V2_CAL)) MENU_ITEM(function, MSG_XYZ_DETAILS, lcd_service_mode_show_result);
  MENU_ITEM(submenu, MSG_INFO_EXTRUDER, lcd_menu_extruder_info);

#ifdef TMC2130
  MENU_ITEM(submenu, MSG_MENU_BELT_STATUS, lcd_menu_belt_status);
#endif //TMC2130
    
  MENU_ITEM(submenu, MSG_MENU_TEMPERATURES, lcd_menu_temperatures);

#if defined (VOLT_BED_PIN) || defined (VOLT_PWR_PIN)
  MENU_ITEM(submenu, MSG_MENU_VOLTAGES, lcd_menu_voltages);
#endif //defined VOLT_BED_PIN || defined VOLT_PWR_PIN

#ifdef DEBUG_BUILD
  MENU_ITEM(submenu, PSTR("Debug"), lcd_menu_debug);
#endif /* DEBUG_BUILD */

  #endif //MK1BP
  END_MENU();
}

void lcd_set_fan_check() {
	fans_check_enabled = !fans_check_enabled;
	eeprom_update_byte((unsigned char *)EEPROM_FAN_CHECK_ENABLED, fans_check_enabled);
	lcd_goto_menu(lcd_settings_menu); //doesn't break menuStack
}

void lcd_set_filament_autoload() {
	filament_autoload_enabled = !filament_autoload_enabled;
	eeprom_update_byte((unsigned char *)EEPROM_FSENS_AUTOLOAD_ENABLED, filament_autoload_enabled);
	lcd_goto_menu(lcd_settings_menu); //doesn't break menuStack
}

void lcd_unLoadFilament()
{

  if (degHotend0() > EXTRUDE_MINTEMP) {
	
	  enquecommand_P(PSTR("M702")); //unload filament

  } else {

    lcd_implementation_clear();
    lcd.setCursor(0, 0);
    lcd_printPGM(MSG_ERROR);
    lcd.setCursor(0, 2);
    lcd_printPGM(MSG_PREHEAT_NOZZLE);

    delay(2000);
    lcd_implementation_clear();
  }

  menu_action_back();
}

void lcd_change_filament() {

  lcd_implementation_clear();

  lcd.setCursor(0, 1);

  lcd_printPGM(MSG_CHANGING_FILAMENT);


}


void lcd_wait_interact() {

  lcd_implementation_clear();

  lcd.setCursor(0, 1);
#ifdef SNMM 
  lcd_printPGM(MSG_PREPARE_FILAMENT);
#else
  lcd_printPGM(MSG_INSERT_FILAMENT);
#endif
  lcd.setCursor(0, 2);
  lcd_printPGM(MSG_PRESS);

}


void lcd_change_success() {

  lcd_implementation_clear();

  lcd.setCursor(0, 2);

  lcd_printPGM(MSG_CHANGE_SUCCESS);


}


void lcd_loading_color() {

  lcd_implementation_clear();

  lcd.setCursor(0, 0);

  lcd_printPGM(MSG_LOADING_COLOR);
  lcd.setCursor(0, 2);
  lcd_printPGM(MSG_PLEASE_WAIT);


  for (int i = 0; i < 20; i++) {

    lcd.setCursor(i, 3);
    lcd.print(".");
    for (int j = 0; j < 10 ; j++) {
      manage_heater();
      manage_inactivity(true);
      delay(85);

    }


  }

}


void lcd_loading_filament() {


  lcd_implementation_clear();

  lcd.setCursor(0, 0);

  lcd_printPGM(MSG_LOADING_FILAMENT);
  lcd.setCursor(0, 2);
  lcd_printPGM(MSG_PLEASE_WAIT);

  for (int i = 0; i < 20; i++) {

    lcd.setCursor(i, 3);
    lcd.print(".");
    for (int j = 0; j < 10 ; j++) {
      manage_heater();
      manage_inactivity(true);
#ifdef SNMM
      delay(153);
#else
	  delay(137);
#endif

    }


  }

}




void lcd_alright() {
  int enc_dif = 0;
  int cursor_pos = 1;




  lcd_implementation_clear();

  lcd.setCursor(0, 0);

  lcd_printPGM(MSG_CORRECTLY);

  lcd.setCursor(1, 1);

  lcd_printPGM(MSG_YES);

  lcd.setCursor(1, 2);

  lcd_printPGM(MSG_NOT_LOADED);


  lcd.setCursor(1, 3);
  lcd_printPGM(MSG_NOT_COLOR);


  lcd.setCursor(0, 1);

  lcd.print(">");


  enc_dif = encoderDiff;

  while (lcd_change_fil_state == 0) {

    manage_heater();
    manage_inactivity(true);

    if ( abs((enc_dif - encoderDiff)) > 4 ) {

      if ( (abs(enc_dif - encoderDiff)) > 1 ) {
        if (enc_dif > encoderDiff ) {
          cursor_pos --;
        }

        if (enc_dif < encoderDiff  ) {
          cursor_pos ++;
        }

        if (cursor_pos > 3) {
          cursor_pos = 3;
        }

        if (cursor_pos < 1) {
          cursor_pos = 1;
        }
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
        lcd.setCursor(0, cursor_pos);
        lcd.print(">");
        enc_dif = encoderDiff;
        delay(100);
      }

    }


    if (lcd_clicked()) {

      lcd_change_fil_state = cursor_pos;
      delay(500);

    }



  };


  lcd_implementation_clear();
  lcd_return_to_status();

}

#ifdef PAT9125
static void lcd_menu_AutoLoadFilament()
{
    if (degHotend0() > EXTRUDE_MINTEMP)
    {
        uint8_t nlines;
        lcd_display_message_fullscreen_nonBlocking_P(MSG_AUTOLOADING_ENABLED,nlines);
    }
    else
    {
		Timer* ptimer = (Timer*)&(menuData.autoLoadFilamentMenu.dummy);
        if (!ptimer->running()) ptimer->start();
        lcd.setCursor(0, 0);
        lcd_printPGM(MSG_ERROR);
        lcd.setCursor(0, 2);
        lcd_printPGM(MSG_PREHEAT_NOZZLE);
        if (ptimer->expired(2000ul)) menu_action_back();
    }
    if (lcd_clicked()) menu_action_back();
}
#endif //PAT9125

static void lcd_LoadFilament()
{
  if (degHotend0() > EXTRUDE_MINTEMP)
  {
      custom_message = true;
      loading_flag = true;
      enquecommand_P(PSTR("M701")); //load filament
      SERIAL_ECHOLN("Loading filament");
      lcd_return_to_status();
  }
  else
  {

    lcd_implementation_clear();
    lcd.setCursor(0, 0);
    lcd_printPGM(MSG_ERROR);
    lcd.setCursor(0, 2);
    lcd_printPGM(MSG_PREHEAT_NOZZLE);
    delay(2000);
    lcd_implementation_clear();
  }
}

void lcd_menu_statistics()
{

	if (IS_SD_PRINTING)
	{
		int _met = total_filament_used / 100000;
		int _cm = (total_filament_used - (_met * 100000))/10;
		
		int _t = (millis() - starttime) / 1000;
		int _h = _t / 3600;
		int _m = (_t - (_h * 3600)) / 60;
		int _s = _t - ((_h * 3600) + (_m * 60));
		
		lcd.setCursor(0, 0);
		lcd_printPGM(MSG_STATS_FILAMENTUSED);

		lcd.setCursor(6, 1);
		lcd.print(itostr3(_met));
		lcd.print("m ");
		lcd.print(ftostr32ns(_cm));
		lcd.print("cm");
		
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_STATS_PRINTTIME);

		lcd.setCursor(8, 3);
		lcd.print(itostr2(_h));
		lcd.print("h ");
		lcd.print(itostr2(_m));
		lcd.print("m ");
		lcd.print(itostr2(_s));
		lcd.print("s");

		if (lcd_clicked())
		{
			lcd_quick_feedback();
			lcd_return_to_status();
		}
	}
	else
	{
		unsigned long _filament = eeprom_read_dword((uint32_t *)EEPROM_FILAMENTUSED);
		unsigned long _time = eeprom_read_dword((uint32_t *)EEPROM_TOTALTIME); //in minutes
		
		uint8_t _hours, _minutes;
		uint32_t _days;

		float _filament_m = (float)_filament;
		int _filament_km = (_filament >= 100000) ? _filament / 100000 : 0;
		if (_filament_km > 0)  _filament_m = _filament - (_filament_km * 100000);

		_days = _time / 1440;
		_hours = (_time - (_days * 1440)) / 60;
		_minutes = _time - ((_days * 1440) + (_hours * 60));

		lcd_implementation_clear();

		lcd.setCursor(0, 0);
		lcd_printPGM(MSG_STATS_TOTALFILAMENT);
		lcd.setCursor(17 - strlen(ftostr32ns(_filament_m)), 1);
		lcd.print(ftostr32ns(_filament_m));

		if (_filament_km > 0)
		{
			lcd.setCursor(17 - strlen(ftostr32ns(_filament_m)) - 3, 1);
			lcd.print("km");
			lcd.setCursor(17 - strlen(ftostr32ns(_filament_m)) - 8, 1);
			lcd.print(itostr4(_filament_km));
		}


		lcd.setCursor(18, 1);
		lcd.print("m");

		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_STATS_TOTALPRINTTIME);;

		lcd.setCursor(18, 3);
		lcd.print("m");
		lcd.setCursor(14, 3);
		lcd.print(itostr3(_minutes));

		lcd.setCursor(14, 3);
		lcd.print(":");

		lcd.setCursor(12, 3);
		lcd.print("h");
		lcd.setCursor(9, 3);
		lcd.print(itostr3(_hours));

		lcd.setCursor(9, 3);
		lcd.print(":");

		lcd.setCursor(7, 3);
		lcd.print("d");
		lcd.setCursor(4, 3);
		lcd.print(itostr3(_days));


		KEEPALIVE_STATE(PAUSED_FOR_USER);
		while (!lcd_clicked())
		{
			manage_heater();
			manage_inactivity(true);
			delay(100);
		}
		KEEPALIVE_STATE(NOT_BUSY);

		lcd_quick_feedback();
		lcd_return_to_status();
	}
}


static void _lcd_move(const char *name, int axis, int min, int max) {
    if (!menuData._lcd_moveMenu.initialized)
    {
        menuData._lcd_moveMenu.endstopsEnabledPrevious = enable_endstops(false);
        menuData._lcd_moveMenu.initialized = true;
    }

	if (encoderPosition != 0) {
    refresh_cmd_timeout();
    if (! planner_queue_full()) {
      current_position[axis] += float((int)encoderPosition) * move_menu_scale;
      if (min_software_endstops && current_position[axis] < min) current_position[axis] = min;
      if (max_software_endstops && current_position[axis] > max) current_position[axis] = max;
      encoderPosition = 0;
      world2machine_clamp(current_position[X_AXIS], current_position[Y_AXIS]);
      plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], manual_feedrate[axis] / 60, active_extruder);
      lcdDrawUpdate = 1;
    }
  }
  if (lcdDrawUpdate) lcd_implementation_drawedit(name, ftostr31(current_position[axis]));
  if (menuExiting || LCD_CLICKED) (void)enable_endstops(menuData._lcd_moveMenu.endstopsEnabledPrevious);
  if (LCD_CLICKED) menu_action_back();
}


static void lcd_move_e()
{
	if (degHotend0() > EXTRUDE_MINTEMP) {
  if (encoderPosition != 0)
  {
    refresh_cmd_timeout();
    if (! planner_queue_full()) {
      current_position[E_AXIS] += float((int)encoderPosition) * move_menu_scale;
      encoderPosition = 0;
      plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], manual_feedrate[E_AXIS] / 60, active_extruder);
      lcdDrawUpdate = 1;
    }
  }
  if (lcdDrawUpdate)
  {
    lcd_implementation_drawedit(PSTR("Extruder"), ftostr31(current_position[E_AXIS]));
  }
  if (LCD_CLICKED) menu_action_back();
}
	else {
		lcd_implementation_clear();
		lcd.setCursor(0, 0);
		lcd_printPGM(MSG_ERROR);
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_PREHEAT_NOZZLE);

		delay(2000);
		lcd_return_to_status();
	}
}

void lcd_service_mode_show_result() {
	float angleDiff;
	lcd_set_custom_characters_degree();
	count_xyz_details();
	angleDiff = eeprom_read_float((float*)(EEPROM_XYZ_CAL_SKEW));
	lcd_update_enable(false);
	lcd_implementation_clear();
	lcd_printPGM(MSG_Y_DISTANCE_FROM_MIN);
	lcd_print_at_PGM(0, 1, MSG_LEFT);
	lcd_print_at_PGM(0, 2, MSG_RIGHT);

	for (int i = 0; i < 2; i++) {
		if(distance_from_min[i] < 200) {
			lcd_print_at_PGM(11, i + 1, PSTR(""));
			lcd.print(distance_from_min[i]);
			lcd_print_at_PGM((distance_from_min[i] < 0) ? 17 : 16, i + 1, PSTR("mm"));		
		} else lcd_print_at_PGM(11, i + 1, PSTR("N/A"));
	}
	delay_keep_alive(500);
	KEEPALIVE_STATE(PAUSED_FOR_USER);
	while (!lcd_clicked()) {
		delay_keep_alive(100);
	}
	delay_keep_alive(500);
	lcd_implementation_clear();
	

	lcd_printPGM(MSG_MEASURED_SKEW);
	if (angleDiff < 100) {
		lcd.setCursor(15, 0);
		lcd.print(angleDiff * 180 / M_PI);
		lcd.print(LCD_STR_DEGREE);
	}else lcd_print_at_PGM(16, 0, PSTR("N/A"));
	lcd_print_at_PGM(0, 1, PSTR("--------------------"));
	lcd_print_at_PGM(0, 2, MSG_SLIGHT_SKEW);
	lcd_print_at_PGM(15, 2, PSTR(""));
	lcd.print(bed_skew_angle_mild * 180 / M_PI);
	lcd.print(LCD_STR_DEGREE);
	lcd_print_at_PGM(0, 3, MSG_SEVERE_SKEW);
	lcd_print_at_PGM(15, 3, PSTR(""));
	lcd.print(bed_skew_angle_extreme * 180 / M_PI);
	lcd.print(LCD_STR_DEGREE);
	delay_keep_alive(500);
	while (!lcd_clicked()) {
		delay_keep_alive(100);
	}
	KEEPALIVE_STATE(NOT_BUSY);
	delay_keep_alive(500);
	lcd_set_custom_characters_arrows();
	lcd_return_to_status();
	lcd_update_enable(true);
	lcd_update(2);
}




// Save a single axis babystep value.
void EEPROM_save_B(int pos, int* value)
{
  union Data data;
  data.value = *value;

  eeprom_update_byte((unsigned char*)pos, data.b[0]);
  eeprom_update_byte((unsigned char*)pos + 1, data.b[1]);
}

// Read a single axis babystep value.
void EEPROM_read_B(int pos, int* value)
{
  union Data data;
  data.b[0] = eeprom_read_byte((unsigned char*)pos);
  data.b[1] = eeprom_read_byte((unsigned char*)pos + 1);
  *value = data.value;
}


static void lcd_move_x() {
  _lcd_move(PSTR("X"), X_AXIS, X_MIN_POS, X_MAX_POS);
}
static void lcd_move_y() {
  _lcd_move(PSTR("Y"), Y_AXIS, Y_MIN_POS, Y_MAX_POS);
}
static void lcd_move_z() {
  _lcd_move(PSTR("Z"), Z_AXIS, Z_MIN_POS, Z_MAX_POS);
}


/**
 * @brief Adjust first layer offset from bed if axis is Z_AXIS
 *
 * If menu is left (button pushed or timed out), value is stored to EEPROM and
 * if the axis is Z_AXIS, CALIBRATION_STATUS_CALIBRATED is also stored.
 * Purpose of this function for other axis then Z is unknown.
 *
 * @param axis AxisEnum X_AXIS Y_AXIS Z_AXIS
 * other value leads to storing Z_AXIS
 * @param msg text to be displayed
 */
static void _lcd_babystep(int axis, const char *msg) 
{
    if (menuData.babyStep.status == 0) {
        // Menu was entered.
        // Initialize its status.
        menuData.babyStep.status = 1;
		check_babystep();

		EEPROM_read_B(EEPROM_BABYSTEP_X, &menuData.babyStep.babystepMem[0]);
        EEPROM_read_B(EEPROM_BABYSTEP_Y, &menuData.babyStep.babystepMem[1]);
        EEPROM_read_B(EEPROM_BABYSTEP_Z, &menuData.babyStep.babystepMem[2]);
		
        menuData.babyStep.babystepMemMM[0] = menuData.babyStep.babystepMem[0]/axis_steps_per_unit[X_AXIS];
        menuData.babyStep.babystepMemMM[1] = menuData.babyStep.babystepMem[1]/axis_steps_per_unit[Y_AXIS];
        menuData.babyStep.babystepMemMM[2] = menuData.babyStep.babystepMem[2]/axis_steps_per_unit[Z_AXIS];
        lcdDrawUpdate = 1;
		//SERIAL_ECHO("Z baby step: ");
		//SERIAL_ECHO(menuData.babyStep.babystepMem[2]);
        // Wait 90 seconds before closing the live adjust dialog.
        lcd_timeoutToStatus = millis() + 90000;
    }

  if (encoderPosition != 0) 
  {
	if (homing_flag) encoderPosition = 0;

    menuData.babyStep.babystepMem[axis] += (int)encoderPosition;
	if (axis == 2) {
		if (menuData.babyStep.babystepMem[axis] < Z_BABYSTEP_MIN) menuData.babyStep.babystepMem[axis] = Z_BABYSTEP_MIN; //-3999 -> -9.99 mm
		else  if (menuData.babyStep.babystepMem[axis] > Z_BABYSTEP_MAX) menuData.babyStep.babystepMem[axis] = Z_BABYSTEP_MAX; //0
		else {
			CRITICAL_SECTION_START
				babystepsTodo[axis] += (int)encoderPosition;
			CRITICAL_SECTION_END		
		}
	}
    menuData.babyStep.babystepMemMM[axis] = menuData.babyStep.babystepMem[axis]/axis_steps_per_unit[axis]; 
	  delay(50);
	  encoderPosition = 0;
    lcdDrawUpdate = 1;
  }
  if (lcdDrawUpdate)
    lcd_implementation_drawedit_2(msg, ftostr13ns(menuData.babyStep.babystepMemMM[axis]));
  if (LCD_CLICKED || menuExiting) {
    // Only update the EEPROM when leaving the menu.
    EEPROM_save_B(
      (axis == X_AXIS) ? EEPROM_BABYSTEP_X : ((axis == Y_AXIS) ? EEPROM_BABYSTEP_Y : EEPROM_BABYSTEP_Z),
      &menuData.babyStep.babystepMem[axis]);

    if(Z_AXIS == axis) calibration_status_store(CALIBRATION_STATUS_CALIBRATED);
  }
  if (LCD_CLICKED) menu_action_back();
}

static void lcd_babystep_x() {
  _lcd_babystep(X_AXIS, (MSG_BABYSTEPPING_X));
}
static void lcd_babystep_y() {
  _lcd_babystep(Y_AXIS, (MSG_BABYSTEPPING_Y));
}
static void lcd_babystep_z() {
	_lcd_babystep(Z_AXIS, (MSG_BABYSTEPPING_Z));
}

static void lcd_adjust_bed();

/**
 * @brief adjust bed reset menu item function
 *
 * To be used as MENU_ITEM(function,...) inside lcd_adjust_bed submenu. In such case lcd_goto_menu usage
 * is correct and doesn't break menuStack.
 * Because we did not leave the menu, the menuData did not reset.
 * Force refresh of the bed leveling data.
 */
static void lcd_adjust_bed_reset()
{
    eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_VALID, 1);
    eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_LEFT , 0);
    eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_RIGHT, 0);
    eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_FRONT, 0);
    eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_REAR , 0);
    lcd_goto_menu(lcd_adjust_bed, 0, false); //doesn't break menuStack
    menuData.adjustBed.status = 0;
}

void adjust_bed_reset() {
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_VALID, 1);
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_LEFT, 0);
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_RIGHT, 0);
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_FRONT, 0);
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_REAR, 0);
	menuData.adjustBed.left = menuData.adjustBed.left2 = 0;
	menuData.adjustBed.right = menuData.adjustBed.right2 = 0;
	menuData.adjustBed.front = menuData.adjustBed.front2 = 0;
	menuData.adjustBed.rear = menuData.adjustBed.rear2 = 0;
}
#define BED_ADJUSTMENT_UM_MAX 50

static void lcd_adjust_bed()
{
    if (menuData.adjustBed.status == 0) {
        // Menu was entered.
        // Initialize its status.
        menuData.adjustBed.status = 1;
        bool valid = false;
        menuData.adjustBed.left  = menuData.adjustBed.left2  = eeprom_read_int8((unsigned char*)EEPROM_BED_CORRECTION_LEFT);
        menuData.adjustBed.right = menuData.adjustBed.right2 = eeprom_read_int8((unsigned char*)EEPROM_BED_CORRECTION_RIGHT);
        menuData.adjustBed.front = menuData.adjustBed.front2 = eeprom_read_int8((unsigned char*)EEPROM_BED_CORRECTION_FRONT);
        menuData.adjustBed.rear  = menuData.adjustBed.rear2  = eeprom_read_int8((unsigned char*)EEPROM_BED_CORRECTION_REAR);
        if (eeprom_read_byte((unsigned char*)EEPROM_BED_CORRECTION_VALID) == 1 && 
            menuData.adjustBed.left  >= -BED_ADJUSTMENT_UM_MAX && menuData.adjustBed.left  <= BED_ADJUSTMENT_UM_MAX &&
            menuData.adjustBed.right >= -BED_ADJUSTMENT_UM_MAX && menuData.adjustBed.right <= BED_ADJUSTMENT_UM_MAX &&
            menuData.adjustBed.front >= -BED_ADJUSTMENT_UM_MAX && menuData.adjustBed.front <= BED_ADJUSTMENT_UM_MAX &&
            menuData.adjustBed.rear  >= -BED_ADJUSTMENT_UM_MAX && menuData.adjustBed.rear  <= BED_ADJUSTMENT_UM_MAX)
            valid = true;
        if (! valid) {
            // Reset the values: simulate an edit.
            menuData.adjustBed.left2  = 0;
            menuData.adjustBed.right2 = 0;
            menuData.adjustBed.front2 = 0;
            menuData.adjustBed.rear2  = 0;
        }
        lcdDrawUpdate = 1;
        eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_VALID, 1);
    }

    if (menuData.adjustBed.left  != menuData.adjustBed.left2)
        eeprom_update_int8((unsigned char*)EEPROM_BED_CORRECTION_LEFT,  menuData.adjustBed.left  = menuData.adjustBed.left2);
    if (menuData.adjustBed.right != menuData.adjustBed.right2)
        eeprom_update_int8((unsigned char*)EEPROM_BED_CORRECTION_RIGHT, menuData.adjustBed.right = menuData.adjustBed.right2);
    if (menuData.adjustBed.front != menuData.adjustBed.front2)
        eeprom_update_int8((unsigned char*)EEPROM_BED_CORRECTION_FRONT, menuData.adjustBed.front = menuData.adjustBed.front2);
    if (menuData.adjustBed.rear  != menuData.adjustBed.rear2)
        eeprom_update_int8((unsigned char*)EEPROM_BED_CORRECTION_REAR,  menuData.adjustBed.rear  = menuData.adjustBed.rear2);

    START_MENU();
    MENU_ITEM(back, MSG_SETTINGS, 0);
    MENU_ITEM_EDIT(int3, MSG_BED_CORRECTION_LEFT,  &menuData.adjustBed.left2,  -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);
    MENU_ITEM_EDIT(int3, MSG_BED_CORRECTION_RIGHT, &menuData.adjustBed.right2, -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);
    MENU_ITEM_EDIT(int3, MSG_BED_CORRECTION_FRONT, &menuData.adjustBed.front2, -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);
    MENU_ITEM_EDIT(int3, MSG_BED_CORRECTION_REAR,  &menuData.adjustBed.rear2,  -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);
    MENU_ITEM(function, MSG_BED_CORRECTION_RESET, lcd_adjust_bed_reset);
    END_MENU();
}

void pid_extruder() {

	lcd_implementation_clear();
	lcd.setCursor(1, 0);
	lcd_printPGM(MSG_SET_TEMPERATURE);
	pid_temp += int(encoderPosition);
	if (pid_temp > HEATER_0_MAXTEMP) pid_temp = HEATER_0_MAXTEMP;
	if (pid_temp < HEATER_0_MINTEMP) pid_temp = HEATER_0_MINTEMP;
	encoderPosition = 0;
	lcd.setCursor(1, 2);
	lcd.print(ftostr3(pid_temp));
	if (lcd_clicked()) {
		lcd_commands_type = LCD_COMMAND_PID_EXTRUDER;
		lcd_return_to_status();
		lcd_update(2);
	}

}

void lcd_adjust_z() {
  int enc_dif = 0;
  int cursor_pos = 1;
  int fsm = 0;




  lcd_implementation_clear();
  lcd.setCursor(0, 0);
  lcd_printPGM(MSG_ADJUSTZ);
  lcd.setCursor(1, 1);
  lcd_printPGM(MSG_YES);

  lcd.setCursor(1, 2);

  lcd_printPGM(MSG_NO);

  lcd.setCursor(0, 1);

  lcd.print(">");


  enc_dif = encoderDiff;

  while (fsm == 0) {

    manage_heater();
    manage_inactivity(true);

    if ( abs((enc_dif - encoderDiff)) > 4 ) {

      if ( (abs(enc_dif - encoderDiff)) > 1 ) {
        if (enc_dif > encoderDiff ) {
          cursor_pos --;
        }

        if (enc_dif < encoderDiff  ) {
          cursor_pos ++;
        }

        if (cursor_pos > 2) {
          cursor_pos = 2;
        }

        if (cursor_pos < 1) {
          cursor_pos = 1;
        }
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, cursor_pos);
        lcd.print(">");
        enc_dif = encoderDiff;
        delay(100);
      }

    }


    if (lcd_clicked()) {
      fsm = cursor_pos;
      if (fsm == 1) {
        int babystepLoadZ = 0;
        EEPROM_read_B(EEPROM_BABYSTEP_Z, &babystepLoadZ);
        CRITICAL_SECTION_START
        babystepsTodo[Z_AXIS] = babystepLoadZ;
        CRITICAL_SECTION_END
      } else {
        int zero = 0;
        EEPROM_save_B(EEPROM_BABYSTEP_X, &zero);
        EEPROM_save_B(EEPROM_BABYSTEP_Y, &zero);
        EEPROM_save_B(EEPROM_BABYSTEP_Z, &zero);
      }
      delay(500);
    }
  };

  lcd_implementation_clear();
  lcd_return_to_status();

}

bool lcd_wait_for_pinda(float temp) {
	lcd_set_custom_characters_degree();
	setTargetHotend(0, 0);
	setTargetBed(0);
	Timer pinda_timeout;
	pinda_timeout.start();
	bool target_temp_reached = true;

	while (current_temperature_pinda > temp){
		lcd_display_message_fullscreen_P(MSG_WAITING_TEMP_PINDA);

		lcd.setCursor(0, 4);
		lcd.print(LCD_STR_THERMOMETER[0]);
		lcd.print(ftostr3(current_temperature_pinda));
		lcd.print("/");
		lcd.print(ftostr3(temp));
		lcd.print(LCD_STR_DEGREE);
		delay_keep_alive(1000);
		serialecho_temperatures();
		if (pinda_timeout.expired(8 * 60 * 1000ul)) { //PINDA cooling from 60 C to 35 C takes about 7 minutes
			target_temp_reached = false;
			break;
		}
	}
	lcd_set_custom_characters_arrows();
	lcd_update_enable(true);
	return(target_temp_reached);
}

void lcd_wait_for_heater() {
	lcd_display_message_fullscreen_P(MSG_WIZARD_HEATING);

		lcd.setCursor(0, 4);
		lcd.print(LCD_STR_THERMOMETER[0]);
		lcd.print(ftostr3(degHotend(active_extruder)));
		lcd.print("/");
		lcd.print(ftostr3(degTargetHotend(active_extruder)));
		lcd.print(LCD_STR_DEGREE);
}

void lcd_wait_for_cool_down() {
	lcd_set_custom_characters_degree();
	setTargetHotend(0,0);
	setTargetBed(0);
	while ((degHotend(0)>MAX_HOTEND_TEMP_CALIBRATION) || (degBed() > MAX_BED_TEMP_CALIBRATION)) {
		lcd_display_message_fullscreen_P(MSG_WAITING_TEMP);

		lcd.setCursor(0, 4);
		lcd.print(LCD_STR_THERMOMETER[0]);
		lcd.print(ftostr3(degHotend(0)));
		lcd.print("/0");		
		lcd.print(LCD_STR_DEGREE);

		lcd.setCursor(9, 4);
		lcd.print(LCD_STR_BEDTEMP[0]);
		lcd.print(ftostr3(degBed()));
		lcd.print("/0");		
		lcd.print(LCD_STR_DEGREE);
		lcd_set_custom_characters();
		delay_keep_alive(1000);
		serialecho_temperatures();
	}
	lcd_set_custom_characters_arrows();
	lcd_update_enable(true);
}

// Lets the user move the Z carriage up to the end stoppers.
// When done, it sets the current Z to Z_MAX_POS and returns true.
// Otherwise the Z calibration is not changed and false is returned.

#ifndef TMC2130
bool lcd_calibrate_z_end_stop_manual(bool only_z)
{
    bool clean_nozzle_asked = false;

    // Don't know where we are. Let's claim we are Z=0, so the soft end stops will not be triggered when moving up.
    current_position[Z_AXIS] = 0;
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);

    // Until confirmed by the confirmation dialog.
    for (;;) {
        unsigned long previous_millis_cmd = millis();
        const char   *msg                 = only_z ? MSG_MOVE_CARRIAGE_TO_THE_TOP_Z : MSG_MOVE_CARRIAGE_TO_THE_TOP;
        const char   *msg_next            = lcd_display_message_fullscreen_P(msg);
        const bool    multi_screen        = msg_next != NULL;
        unsigned long previous_millis_msg = millis();
        // Until the user finishes the z up movement.
        encoderDiff = 0;
        encoderPosition = 0;
        for (;;) {
//          if (millis() - previous_millis_cmd > LCD_TIMEOUT_TO_STATUS)
//             goto canceled;
            manage_heater();
            manage_inactivity(true);
            if (abs(encoderDiff) >= ENCODER_PULSES_PER_STEP) {
                delay(50);
                previous_millis_cmd = millis();
                encoderPosition += abs(encoderDiff / ENCODER_PULSES_PER_STEP);
                encoderDiff = 0;
                if (! planner_queue_full()) {
                    // Only move up, whatever direction the user rotates the encoder.
                    current_position[Z_AXIS] += fabs(encoderPosition);
                    encoderPosition = 0;
                    plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], manual_feedrate[Z_AXIS] / 60, active_extruder);
                }
            }
            if (lcd_clicked()) {
                // Abort a move if in progress.
                planner_abort_hard();
                while (lcd_clicked()) ;
                delay(10);
                while (lcd_clicked()) ;
                break;
            }
            if (multi_screen && millis() - previous_millis_msg > 5000) {
                if (msg_next == NULL)
                    msg_next = msg;
                msg_next = lcd_display_message_fullscreen_P(msg_next);
                previous_millis_msg = millis();
            }
        }

        if (! clean_nozzle_asked) {
            lcd_show_fullscreen_message_and_wait_P(MSG_CONFIRM_NOZZLE_CLEAN);
            clean_nozzle_asked = true;
        }
		

        // Let the user confirm, that the Z carriage is at the top end stoppers.
        int8_t result = lcd_show_fullscreen_message_yes_no_and_wait_P(MSG_CONFIRM_CARRIAGE_AT_THE_TOP, false);
        if (result == -1)
            goto canceled;
        else if (result == 1)
            goto calibrated;
        // otherwise perform another round of the Z up dialog.
    }

calibrated:
    // Let the machine think the Z axis is a bit higher than it is, so it will not home into the bed
    // during the search for the induction points.
    current_position[Z_AXIS] = Z_MAX_POS-3.f;
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
    
    
    if(only_z){
        lcd_display_message_fullscreen_P(MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE1);
        lcd_implementation_print_at(0, 3, 1);
        lcd_printPGM(MSG_MEASURE_BED_REFERENCE_HEIGHT_LINE2);
    }else{
		//lcd_show_fullscreen_message_and_wait_P(MSG_PAPER);
        lcd_display_message_fullscreen_P(MSG_FIND_BED_OFFSET_AND_SKEW_LINE1);
        lcd_implementation_print_at(0, 2, 1);
        lcd_printPGM(MSG_FIND_BED_OFFSET_AND_SKEW_LINE2);
    }
    
    
    return true;

canceled:
    return false;
}

#endif // TMC2130

static inline bool pgm_is_whitespace(const char *c_addr)
{
    const char c = pgm_read_byte(c_addr);
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static inline bool pgm_is_interpunction(const char *c_addr)
{
    const char c = pgm_read_byte(c_addr);
    return c == '.' || c == ',' || c == ':'|| c == ';' || c == '?' || c == '!' || c == '/';
}

/**
 * @brief show full screen message
 *
 * This function is non-blocking
 * @param msg message to be displayed from PROGMEM
 * @param nlines
 * @return rest of the text (to be displayed on next page)
 */
static const char* lcd_display_message_fullscreen_nonBlocking_P(const char *msg, uint8_t &nlines)
{
    lcd.setCursor(0, 0);
    const char *msgend = msg;
    uint8_t row = 0;
    bool multi_screen = false;
    for (; row < 4; ++ row) {
        while (pgm_is_whitespace(msg))
            ++ msg;
        if (pgm_read_byte(msg) == 0)
            // End of the message.
            break;
        lcd.setCursor(0, row);
        uint8_t linelen = min(strlen_P(msg), 20);
        const char *msgend2 = msg + linelen;
        msgend = msgend2;
        if (row == 3 && linelen == 20) {
            // Last line of the display, full line shall be displayed.
            // Find out, whether this message will be split into multiple screens.
            while (pgm_is_whitespace(msgend))
                ++ msgend;
            multi_screen = pgm_read_byte(msgend) != 0;
            if (multi_screen)
                msgend = (msgend2 -= 2);
        }
        if (pgm_read_byte(msgend) != 0 && ! pgm_is_whitespace(msgend) && ! pgm_is_interpunction(msgend)) {
            // Splitting a word. Find the start of the current word.
            while (msgend > msg && ! pgm_is_whitespace(msgend - 1))
                 -- msgend;
            if (msgend == msg)
                // Found a single long word, which cannot be split. Just cut it.
                msgend = msgend2;
        }
        for (; msg < msgend; ++ msg) {
            char c = char(pgm_read_byte(msg));
            if (c == '~')
                c = ' ';
            lcd.print(c);
        }
    }

    if (multi_screen) {
        // Display the "next screen" indicator character.
        // lcd_set_custom_characters_arrows();
        lcd_set_custom_characters_nextpage();
        lcd.setCursor(19, 3);
        // Display the down arrow.
        lcd.print(char(1));
    }

    nlines = row;
    return multi_screen ? msgend : NULL;
}

const char* lcd_display_message_fullscreen_P(const char *msg, uint8_t &nlines)
{
    // Disable update of the screen by the usual lcd_update() routine.
    lcd_update_enable(false);
    lcd_implementation_clear();
    return lcd_display_message_fullscreen_nonBlocking_P(msg, nlines);
}


/**
 * @brief show full screen message and wait
 *
 * This function is blocking.
 * @param msg message to be displayed from PROGMEM
 */
void lcd_show_fullscreen_message_and_wait_P(const char *msg)
{
    const char *msg_next = lcd_display_message_fullscreen_P(msg);
    bool multi_screen = msg_next != NULL;
	lcd_set_custom_characters_nextpage();
	KEEPALIVE_STATE(PAUSED_FOR_USER);
	// Until confirmed by a button click.
	for (;;) {
		if (!multi_screen) {
			lcd.setCursor(19, 3);
			// Display the confirm char.
			lcd.print(char(2));
		}
        // Wait for 5 seconds before displaying the next text.
        for (uint8_t i = 0; i < 100; ++ i) {
            delay_keep_alive(50);
            if (lcd_clicked()) {
                while (lcd_clicked()) ;
                delay(10);
                while (lcd_clicked()) ;
				if (msg_next == NULL) {
					KEEPALIVE_STATE(IN_HANDLER);
					lcd_set_custom_characters();
					lcd_update_enable(true);
					lcd_update(2);
					return;
				}
				else {
					break;
				}
            }
        }
        if (multi_screen) {
            if (msg_next == NULL)
                msg_next = msg;
            msg_next = lcd_display_message_fullscreen_P(msg_next);
			if (msg_next == NULL) {

				lcd.setCursor(19, 3);
				// Display the confirm char.
				lcd.print(char(2));
			}
        }
    }
}

void lcd_wait_for_click()
{
	KEEPALIVE_STATE(PAUSED_FOR_USER);
    for (;;) {
        manage_heater();
        manage_inactivity(true);
        if (lcd_clicked()) {
            while (lcd_clicked()) ;
            delay(10);
            while (lcd_clicked()) ;
			KEEPALIVE_STATE(IN_HANDLER);
            return;
        }
    }
}

int8_t lcd_show_multiscreen_message_yes_no_and_wait_P(const char *msg, bool allow_timeouting, bool default_yes) //currently just max. n*4 + 3 lines supported (set in language header files)
{
	const char *msg_next = lcd_display_message_fullscreen_P(msg);
	bool multi_screen = msg_next != NULL;
	bool yes = default_yes ? true : false;

	// Wait for user confirmation or a timeout.
	unsigned long previous_millis_cmd = millis();
	int8_t        enc_dif = encoderDiff;
	//KEEPALIVE_STATE(PAUSED_FOR_USER);
	for (;;) {
		for (uint8_t i = 0; i < 100; ++i) {
			delay_keep_alive(50);
			if (allow_timeouting && millis() - previous_millis_cmd > LCD_TIMEOUT_TO_STATUS)
				return -1;
			manage_heater();
			manage_inactivity(true);

			if (abs(enc_dif - encoderDiff) > 4) {
				if (msg_next == NULL) {
					lcd.setCursor(0, 3);
					if (enc_dif < encoderDiff && yes) {
						lcd_printPGM((PSTR(" ")));
						lcd.setCursor(7, 3);
						lcd_printPGM((PSTR(">")));
						yes = false;
					}
					else if (enc_dif > encoderDiff && !yes) {
						lcd_printPGM((PSTR(">")));
						lcd.setCursor(7, 3);
						lcd_printPGM((PSTR(" ")));
						yes = true;
					}
					enc_dif = encoderDiff;
				}
				else {
					break; //turning knob skips waiting loop
				}
			}
			if (lcd_clicked()) {
				while (lcd_clicked());
				delay(10);
				while (lcd_clicked());
				if (msg_next == NULL) {
					//KEEPALIVE_STATE(IN_HANDLER);
					lcd_set_custom_characters();
					return yes;
				}
				else break;
			}
		}
		if (multi_screen) {
			if (msg_next == NULL) {
				msg_next = msg;
			}
			msg_next = lcd_display_message_fullscreen_P(msg_next);
		}
		if (msg_next == NULL) {
			lcd.setCursor(0, 3);
			if (yes) lcd_printPGM(PSTR(">"));
			lcd.setCursor(1, 3);
			lcd_printPGM(MSG_YES);
			lcd.setCursor(7, 3);
			if (!yes) lcd_printPGM(PSTR(">"));
			lcd.setCursor(8, 3);
			lcd_printPGM(MSG_NO);
		}
	}
}

int8_t lcd_show_fullscreen_message_yes_no_and_wait_P(const char *msg, bool allow_timeouting, bool default_yes)
{

	lcd_display_message_fullscreen_P(msg);
	
	if (default_yes) {
		lcd.setCursor(0, 2);
		lcd_printPGM(PSTR(">"));
		lcd_printPGM(MSG_YES);
		lcd.setCursor(1, 3);
		lcd_printPGM(MSG_NO);
	}
	else {
		lcd.setCursor(1, 2);
		lcd_printPGM(MSG_YES);
		lcd.setCursor(0, 3);
		lcd_printPGM(PSTR(">"));
		lcd_printPGM(MSG_NO);
	}
	bool yes = default_yes ? true : false;

	// Wait for user confirmation or a timeout.
	unsigned long previous_millis_cmd = millis();
	int8_t        enc_dif = encoderDiff;
	KEEPALIVE_STATE(PAUSED_FOR_USER);
	for (;;) {
		if (allow_timeouting && millis() - previous_millis_cmd > LCD_TIMEOUT_TO_STATUS)
			return -1;
		manage_heater();
		manage_inactivity(true);
		if (abs(enc_dif - encoderDiff) > 4) {
			lcd.setCursor(0, 2);
				if (enc_dif < encoderDiff && yes) {
					lcd_printPGM((PSTR(" ")));
					lcd.setCursor(0, 3);
					lcd_printPGM((PSTR(">")));
					yes = false;
				}
				else if (enc_dif > encoderDiff && !yes) {
					lcd_printPGM((PSTR(">")));
					lcd.setCursor(0, 3);
					lcd_printPGM((PSTR(" ")));
					yes = true;
				}
				enc_dif = encoderDiff;
		}
		if (lcd_clicked()) {
			while (lcd_clicked());
			delay(10);
			while (lcd_clicked());
			KEEPALIVE_STATE(IN_HANDLER);
			return yes;
		}
	}
}

void lcd_bed_calibration_show_result(BedSkewOffsetDetectionResultType result, uint8_t point_too_far_mask)
{
    const char *msg = NULL;
    if (result == BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND) {
        lcd_show_fullscreen_message_and_wait_P(MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND);
    } else if (result == BED_SKEW_OFFSET_DETECTION_FITTING_FAILED) {
        if (point_too_far_mask == 0)
            msg = MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED;
        else if (point_too_far_mask == 2 || point_too_far_mask == 7)
            // Only the center point or all the three front points.
            msg = MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR;
        else if (point_too_far_mask & 1 == 0)
            // The right and maybe the center point out of reach.
            msg = MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR;
        else
            // The left and maybe the center point out of reach.
            msg = MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR;
        lcd_show_fullscreen_message_and_wait_P(msg);
    } else {
        if (point_too_far_mask != 0) {
            if (point_too_far_mask == 2 || point_too_far_mask == 7)
                // Only the center point or all the three front points.
                msg = MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR;
            else if (point_too_far_mask & 1 == 0)
                // The right and maybe the center point out of reach.
                msg = MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR;
            else
                // The left and maybe the center point out of reach.
                msg = MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR;
            lcd_show_fullscreen_message_and_wait_P(msg);
        }
        if (point_too_far_mask == 0 || result > 0) {
            switch (result) {
                default:
                    // should not happen
                    msg = MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED;
                    break;
                case BED_SKEW_OFFSET_DETECTION_PERFECT:
                    msg = MSG_BED_SKEW_OFFSET_DETECTION_PERFECT;
                    break;
                case BED_SKEW_OFFSET_DETECTION_SKEW_MILD:
                    msg = MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD;
                    break;
                case BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME:
                    msg = MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME;
                    break;
            }
            lcd_show_fullscreen_message_and_wait_P(msg);
        }
    }
}

void lcd_temp_cal_show_result(bool result) {
	
	custom_message_type = 0;
	custom_message = false;
	disable_x();
	disable_y();
	disable_z();
	disable_e0();
	disable_e1();
	disable_e2();
	setTargetBed(0); //set bed target temperature back to 0

	if (result == true) {
		eeprom_update_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, 1);
		SERIAL_ECHOLNPGM("Temperature calibration done. Continue with pressing the knob.");
		lcd_show_fullscreen_message_and_wait_P(MSG_TEMP_CALIBRATION_DONE);
		temp_cal_active = true;
		eeprom_update_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE, 1);
	}
	else {
		eeprom_update_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, 0);
		SERIAL_ECHOLNPGM("Temperature calibration failed. Continue with pressing the knob.");
		lcd_show_fullscreen_message_and_wait_P(MSG_TEMP_CAL_FAILED);
		temp_cal_active = false;
		eeprom_update_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE, 0);
	}
	lcd_update_enable(true);
	lcd_update(2);
}

static void lcd_show_end_stops() {
	lcd.setCursor(0, 0);
	lcd_printPGM((PSTR("End stops diag")));
	lcd.setCursor(0, 1);
	lcd_printPGM((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING == 1) ? (PSTR("X1")) : (PSTR("X0")));
	lcd.setCursor(0, 2);
	lcd_printPGM((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING == 1) ? (PSTR("Y1")) : (PSTR("Y0")));
	lcd.setCursor(0, 3);
	lcd_printPGM((READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING == 1) ? (PSTR("Z1")) : (PSTR("Z0")));
}

static void menu_show_end_stops() {
    lcd_show_end_stops();
    if (LCD_CLICKED) lcd_goto_menu(lcd_calibration_menu); //doesn't break menuStack
}

// Lets the user move the Z carriage up to the end stoppers.
// When done, it sets the current Z to Z_MAX_POS and returns true.
// Otherwise the Z calibration is not changed and false is returned.
void lcd_diag_show_end_stops()
{
    int enc_dif = encoderDiff;
    lcd_implementation_clear();
    for (;;) {
        manage_heater();
        manage_inactivity(true);
        lcd_show_end_stops();
        if (lcd_clicked()) {
            while (lcd_clicked()) ;
            delay(10);
            while (lcd_clicked()) ;
            break;
        }
    }
    lcd_implementation_clear();
    lcd_return_to_status();
}



void prusa_statistics(int _message, uint8_t _fil_nr) {
#ifdef DEBUG_DISABLE_PRUSA_STATISTICS
	return;
#endif //DEBUG_DISABLE_PRUSA_STATISTICS
	switch (_message)
	{

	case 0: // default message
		if (IS_SD_PRINTING)
		{
			SERIAL_ECHO("{");
			prusa_stat_printerstatus(4);
			prusa_stat_farm_number();
			prusa_stat_printinfo();
			SERIAL_ECHOLN("}");
			status_number = 4;
		}
		else
		{
			SERIAL_ECHO("{");
			prusa_stat_printerstatus(1);
			prusa_stat_farm_number();
			SERIAL_ECHOLN("}");
			status_number = 1;
		}
		break;

	case 1:		// 1 heating
		farm_status = 2;
		SERIAL_ECHO("{");
		prusa_stat_printerstatus(2);
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		status_number = 2;
		farm_timer = 1;
		break;

	case 2:		// heating done
		farm_status = 3;
		SERIAL_ECHO("{");
		prusa_stat_printerstatus(3);
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		status_number = 3;
		farm_timer = 1;

		if (IS_SD_PRINTING)
		{
			farm_status = 4;
			SERIAL_ECHO("{");
			prusa_stat_printerstatus(4);
			prusa_stat_farm_number();
			SERIAL_ECHOLN("}");
			status_number = 4;
		}
		else
		{
			SERIAL_ECHO("{");
			prusa_stat_printerstatus(3);
			prusa_stat_farm_number();
			SERIAL_ECHOLN("}");
			status_number = 3;
		}
		farm_timer = 1;
		break;

	case 3:		// filament change

		break;
	case 4:		// print succesfull
		SERIAL_ECHO("{[RES:1][FIL:");
		MYSERIAL.print(int(_fil_nr));
		SERIAL_ECHO("]");
		prusa_stat_printerstatus(status_number);
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		farm_timer = 2;
		break;
	case 5:		// print not succesfull
		SERIAL_ECHO("{[RES:0][FIL:");
		MYSERIAL.print(int(_fil_nr));
		SERIAL_ECHO("]");
		prusa_stat_printerstatus(status_number);
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		farm_timer = 2;
		break;
	case 6:		// print done
		SERIAL_ECHO("{[PRN:8]");
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		status_number = 8;
		farm_timer = 2;
		break;
	case 7:		// print done - stopped
		SERIAL_ECHO("{[PRN:9]");
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		status_number = 9;
		farm_timer = 2;
		break;
	case 8:		// printer started
		SERIAL_ECHO("{[PRN:0][PFN:");
		status_number = 0;
		SERIAL_ECHO(farm_no);
		SERIAL_ECHOLN("]}");
		farm_timer = 2;
		break;
	case 20:		// echo farm no
		SERIAL_ECHO("{");
		prusa_stat_printerstatus(status_number);
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		farm_timer = 4;
		break;
	case 21: // temperatures
		SERIAL_ECHO("{");
		prusa_stat_temperatures();
		prusa_stat_farm_number();
		prusa_stat_printerstatus(status_number);
		SERIAL_ECHOLN("}");
		break;
    case 22: // waiting for filament change
        SERIAL_ECHO("{[PRN:5]");
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		status_number = 5;
        break;
	
	case 90: // Error - Thermal Runaway
		SERIAL_ECHO("{[ERR:1]");
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		break;
	case 91: // Error - Thermal Runaway Preheat
		SERIAL_ECHO("{[ERR:2]");
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		break;
	case 92: // Error - Min temp
		SERIAL_ECHO("{[ERR:3]");
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		break;
	case 93: // Error - Max temp
		SERIAL_ECHO("{[ERR:4]");
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		break;

    case 99:		// heartbeat
        SERIAL_ECHO("{[PRN:99]");
        prusa_stat_temperatures();
		SERIAL_ECHO("[PFN:");
		SERIAL_ECHO(farm_no);
		SERIAL_ECHO("]");
        SERIAL_ECHOLN("}");
            
        break;
	}

}

static void prusa_stat_printerstatus(int _status)
{
	SERIAL_ECHO("[PRN:");
	SERIAL_ECHO(_status);
	SERIAL_ECHO("]");
}

static void prusa_stat_farm_number() {
	SERIAL_ECHO("[PFN:");
	SERIAL_ECHO(farm_no);
	SERIAL_ECHO("]");
}

static void prusa_stat_temperatures()
{
	SERIAL_ECHO("[ST0:");
	SERIAL_ECHO(target_temperature[0]);
	SERIAL_ECHO("][STB:");
	SERIAL_ECHO(target_temperature_bed);
	SERIAL_ECHO("][AT0:");
	SERIAL_ECHO(current_temperature[0]);
	SERIAL_ECHO("][ATB:");
	SERIAL_ECHO(current_temperature_bed);
	SERIAL_ECHO("]");
}

static void prusa_stat_printinfo()
{
	SERIAL_ECHO("[TFU:");
	SERIAL_ECHO(total_filament_used);
	SERIAL_ECHO("][PCD:");
	SERIAL_ECHO(itostr3(card.percentDone()));
	SERIAL_ECHO("][FEM:");
	SERIAL_ECHO(itostr3(feedmultiply));
	SERIAL_ECHO("][FNM:");
	SERIAL_ECHO(longFilenameOLD);
	SERIAL_ECHO("][TIM:");
	if (starttime != 0)
	{
		SERIAL_ECHO(millis() / 1000 - starttime / 1000);
	}
	else
	{
		SERIAL_ECHO(0);
	}
	SERIAL_ECHO("][FWR:");
	SERIAL_ECHO(FW_VERSION);
	SERIAL_ECHO("]");
}

/*
void lcd_pick_babystep(){
    int enc_dif = 0;
    int cursor_pos = 1;
    int fsm = 0;
    
    
    
    
    lcd_implementation_clear();
    
    lcd.setCursor(0, 0);
    
    lcd_printPGM(MSG_PICK_Z);
    
    
    lcd.setCursor(3, 2);
    
    lcd.print("1");
    
    lcd.setCursor(3, 3);
    
    lcd.print("2");
    
    lcd.setCursor(12, 2);
    
    lcd.print("3");
    
    lcd.setCursor(12, 3);
    
    lcd.print("4");
    
    lcd.setCursor(1, 2);
    
    lcd.print(">");
    
    
    enc_dif = encoderDiff;
    
    while (fsm == 0) {
        
        manage_heater();
        manage_inactivity(true);
        
        if ( abs((enc_dif - encoderDiff)) > 4 ) {
            
            if ( (abs(enc_dif - encoderDiff)) > 1 ) {
                if (enc_dif > encoderDiff ) {
                    cursor_pos --;
                }
                
                if (enc_dif < encoderDiff  ) {
                    cursor_pos ++;
                }
                
                if (cursor_pos > 4) {
                    cursor_pos = 4;
                }
                
                if (cursor_pos < 1) {
                    cursor_pos = 1;
                }

                
                lcd.setCursor(1, 2);
                lcd.print(" ");
                lcd.setCursor(1, 3);
                lcd.print(" ");
                lcd.setCursor(10, 2);
                lcd.print(" ");
                lcd.setCursor(10, 3);
                lcd.print(" ");
                
                if (cursor_pos < 3) {
                    lcd.setCursor(1, cursor_pos+1);
                    lcd.print(">");
                }else{
                    lcd.setCursor(10, cursor_pos-1);
                    lcd.print(">");
                }
                
   
                enc_dif = encoderDiff;
                delay(100);
            }
            
        }
        
        if (lcd_clicked()) {
            fsm = cursor_pos;
            int babyStepZ;
            EEPROM_read_B(EEPROM_BABYSTEP_Z0+((fsm-1)*2),&babyStepZ);
            EEPROM_save_B(EEPROM_BABYSTEP_Z,&babyStepZ);
            calibration_status_store(CALIBRATION_STATUS_CALIBRATED);
            delay(500);
            
        }
    };
    
    lcd_implementation_clear();
    lcd_return_to_status();
}
*/
void lcd_move_menu_axis()
{
	START_MENU();
	MENU_ITEM(back, MSG_SETTINGS, 0);
	MENU_ITEM(submenu, MSG_MOVE_X, lcd_move_x);
	MENU_ITEM(submenu, MSG_MOVE_Y, lcd_move_y);
	MENU_ITEM(submenu, MSG_MOVE_Z, lcd_move_z);
	MENU_ITEM(submenu, MSG_MOVE_E, lcd_move_e);
	END_MENU();
}

static void lcd_move_menu_1mm()
{
  move_menu_scale = 1.0;
  lcd_move_menu_axis();
}


void EEPROM_save(int pos, uint8_t* value, uint8_t size)
{
  do
  {
    eeprom_write_byte((unsigned char*)pos, *value);
    pos++;
    value++;
  } while (--size);
}

void EEPROM_read(int pos, uint8_t* value, uint8_t size)
{
  do
  {
    *value = eeprom_read_byte((unsigned char*)pos);
    pos++;
    value++;
  } while (--size);
}

#ifdef SDCARD_SORT_ALPHA
static void lcd_sort_type_set() {
	uint8_t sdSort;
		EEPROM_read(EEPROM_SD_SORT, (uint8_t*)&sdSort, sizeof(sdSort));
	switch (sdSort) {
		case SD_SORT_TIME: sdSort = SD_SORT_ALPHA; break;
		case SD_SORT_ALPHA: sdSort = SD_SORT_NONE; break;
		default: sdSort = SD_SORT_TIME;
	}
	eeprom_update_byte((unsigned char *)EEPROM_SD_SORT, sdSort);
	presort_flag = true;
	lcd_goto_menu(lcd_settings_menu); //doesn't break menuStack
}
#endif //SDCARD_SORT_ALPHA

#ifdef TMC2130
static void lcd_crash_mode_info()
{
	lcd_update_enable(true);
	static uint32_t tim = 0;
	if ((tim + 1000) < millis())
	{
		fputs_P(MSG_CRASH_DET_ONLY_IN_NORMAL, lcdout);
		tim = millis();
	}
	if (lcd_clicked())
          menu_action_back();
}

static void lcd_crash_mode_info2()
{
	lcd_update_enable(true);
	static uint32_t tim = 0;
	if ((tim + 1000) < millis())
	{
		fputs_P(MSG_CRASH_DET_STEALTH_FORCE_OFF, lcdout);
		tim = millis();
	}
	if (lcd_clicked())
          menu_action_back();
}
#endif //TMC2130

#ifdef PAT9125
static void lcd_filament_autoload_info()
{
    lcd_show_fullscreen_message_and_wait_P(MSG_AUTOLOADING_ONLY_IF_FSENS_ON);
}

static void lcd_fsensor_fail()
{
    lcd_show_fullscreen_message_and_wait_P(MSG_FSENS_NOT_RESPONDING);
}
#endif //PAT9125


static void lcd_silent_mode_set() {
	switch (SilentModeMenu) {
#ifdef TMC2130
	case SILENT_MODE_NORMAL: SilentModeMenu = SILENT_MODE_STEALTH; break;
	case SILENT_MODE_STEALTH: SilentModeMenu = SILENT_MODE_NORMAL; break;
	default: SilentModeMenu = SILENT_MODE_NORMAL; break; // (probably) not needed
#else
	case SILENT_MODE_POWER: SilentModeMenu = SILENT_MODE_SILENT; break;
	case SILENT_MODE_SILENT: SilentModeMenu = SILENT_MODE_AUTO; break;
	case SILENT_MODE_AUTO: SilentModeMenu = SILENT_MODE_POWER; break;
	default: SilentModeMenu = SILENT_MODE_POWER; break; // (probably) not needed
#endif //TMC2130
	}
  eeprom_update_byte((unsigned char *)EEPROM_SILENT, SilentModeMenu);
#ifdef TMC2130
  // Wait until the planner queue is drained and the stepper routine achieves
  // an idle state.
  st_synchronize();
  if (tmc2130_wait_standstill_xy(1000)) {}
//	  MYSERIAL.print("standstill OK");
//  else
//	  MYSERIAL.print("standstill NG!");
  cli();
	tmc2130_mode = (SilentModeMenu != SILENT_MODE_NORMAL)?TMC2130_MODE_SILENT:TMC2130_MODE_NORMAL;
	tmc2130_init();
  // We may have missed a stepper timer interrupt due to the time spent in tmc2130_init.
  // Be safe than sorry, reset the stepper timer before re-enabling interrupts.
  st_reset_timer();
  sei();
#endif //TMC2130
  st_current_init();
#ifdef TMC2130
  if (CrashDetectMenu && (SilentModeMenu != SILENT_MODE_NORMAL))
	  menu_action_submenu(lcd_crash_mode_info2);
#endif //TMC2130
}

#ifdef TMC2130
static void lcd_crash_mode_set()
{
	CrashDetectMenu = !CrashDetectMenu; //set also from crashdet_enable() and crashdet_disable()
    if (CrashDetectMenu==0) {
        crashdet_disable();
    }else{
        crashdet_enable();
    }
	if (IS_SD_PRINTING || is_usb_printing || (lcd_commands_type == LCD_COMMAND_V2_CAL)) lcd_goto_menu(lcd_tune_menu, 9);
	else lcd_goto_menu(lcd_settings_menu, 9);
    
}
#endif //TMC2130
 

static void lcd_set_lang(unsigned char lang) {
  lang_selected = lang;
  firstrun = 1;
  eeprom_update_byte((unsigned char *)EEPROM_LANG, lang);
  /*langsel=0;*/
  if (langsel == LANGSEL_MODAL)
    // From modal mode to an active mode? This forces the menu to return to the setup menu.
    langsel = LANGSEL_ACTIVE;
}

#ifdef PAT9125
static void lcd_fsensor_state_set()
{
	FSensorStateMenu = !FSensorStateMenu; //set also from fsensor_enable() and fsensor_disable()
    if (FSensorStateMenu==0) {
        fsensor_disable();
        if ((filament_autoload_enabled == true)){
            lcd_filament_autoload_info();
        }
    }else{
        fsensor_enable();
        if (fsensor_not_responding)
        {
            lcd_fsensor_fail();
        }
    }
	if (IS_SD_PRINTING || is_usb_printing || (lcd_commands_type == LCD_COMMAND_V2_CAL)) lcd_goto_menu(lcd_tune_menu);
	else lcd_goto_menu(lcd_settings_menu); //doesn't break menuStack
    
}
#endif //PAT9125


#if !SDSORT_USES_RAM
void lcd_set_degree() {
	lcd_set_custom_characters_degree();
}

void lcd_set_progress() {
	lcd_set_custom_characters_progress();
}
#endif

void lcd_force_language_selection() {
  eeprom_update_byte((unsigned char *)EEPROM_LANG, LANG_ID_FORCE_SELECTION);
}

static void lcd_language_menu()
{
  START_MENU();
  if (langsel == LANGSEL_OFF) {
    MENU_ITEM(back, MSG_SETTINGS, 0);
  } else if (langsel == LANGSEL_ACTIVE) {
    MENU_ITEM(back, MSG_WATCH, 0);
  }
  for (int i=0;i<LANG_NUM;i++){
    MENU_ITEM(setlang, MSG_LANGUAGE_NAME_EXPLICIT(i), i);
  }
  END_MENU();
}

void lcd_mesh_bedleveling()
{
	mesh_bed_run_from_menu = true;
	enquecommand_P(PSTR("G80"));
	lcd_return_to_status();
}

void lcd_mesh_calibration()
{
  enquecommand_P(PSTR("M45"));
  lcd_return_to_status();
}

void lcd_mesh_calibration_z()
{
  enquecommand_P(PSTR("M45 Z"));
  lcd_return_to_status();
}

void lcd_pinda_calibration_menu()
{
	START_MENU();
		MENU_ITEM(back, MSG_MENU_CALIBRATION, 0);
		MENU_ITEM(submenu, MSG_CALIBRATE_PINDA, lcd_calibrate_pinda);
	END_MENU();
}

void lcd_temp_calibration_set() {
	temp_cal_active = !temp_cal_active;
	eeprom_update_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE, temp_cal_active);
	st_current_init();
	lcd_goto_menu(lcd_settings_menu); //doesn't break menuStack
}

#ifdef HAS_SECOND_SERIAL_PORT
void lcd_second_serial_set() {
	if(selectedSerialPort == 1) selectedSerialPort = 0;
	else selectedSerialPort = 1;
	eeprom_update_byte((unsigned char *)EEPROM_SECOND_SERIAL_ACTIVE, selectedSerialPort);
	MYSERIAL.begin(BAUDRATE);
	lcd_goto_menu(lcd_settings_menu);//doesn't break menuStack
}
#endif //HAS_SECOND_SERIAL_PORT

void lcd_calibrate_pinda() {
	enquecommand_P(PSTR("G76"));
	lcd_return_to_status();
}

#ifndef SNMM

/*void lcd_calibrate_extruder() {
	
	if (degHotend0() > EXTRUDE_MINTEMP)
	{
		current_position[E_AXIS] = 0;									//set initial position to zero
		plan_set_e_position(current_position[E_AXIS]);
		
		//long steps_start = st_get_position(E_AXIS);

		long steps_final;
		float e_steps_per_unit;
		float feedrate = (180 / axis_steps_per_unit[E_AXIS]) * 1;	//3	//initial automatic extrusion feedrate (depends on current value of axis_steps_per_unit to avoid too fast extrusion)
		float e_shift_calibration = (axis_steps_per_unit[E_AXIS] > 180 ) ? ((180 / axis_steps_per_unit[E_AXIS]) * 70): 70; //length of initial automatic extrusion sequence
		const char   *msg_e_cal_knob = MSG_E_CAL_KNOB;
		const char   *msg_next_e_cal_knob = lcd_display_message_fullscreen_P(msg_e_cal_knob);
		const bool    multi_screen = msg_next_e_cal_knob != NULL;
		unsigned long msg_millis;

		lcd_show_fullscreen_message_and_wait_P(MSG_MARK_FIL);
		lcd_implementation_clear();
		
		
		lcd.setCursor(0, 1); lcd_printPGM(MSG_PLEASE_WAIT);
		current_position[E_AXIS] += e_shift_calibration;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], feedrate, active_extruder);
		st_synchronize();

		lcd_display_message_fullscreen_P(msg_e_cal_knob);
		msg_millis = millis();
		while (!LCD_CLICKED) {
			if (multi_screen && millis() - msg_millis > 5000) {
				if (msg_next_e_cal_knob == NULL)
					msg_next_e_cal_knob = msg_e_cal_knob;
					msg_next_e_cal_knob = lcd_display_message_fullscreen_P(msg_next_e_cal_knob);
					msg_millis = millis();
			}

			//manage_inactivity(true);
			manage_heater();
			if (abs(encoderDiff) >= ENCODER_PULSES_PER_STEP) {						//adjusting mark by knob rotation
				delay_keep_alive(50);
				//previous_millis_cmd = millis();
				encoderPosition += (encoderDiff / ENCODER_PULSES_PER_STEP);
				encoderDiff = 0;
				if (!planner_queue_full()) {
					current_position[E_AXIS] += float(abs((int)encoderPosition)) * 0.01; //0.05
					encoderPosition = 0;
					plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], feedrate, active_extruder);
					
				}
			}	
		}
		
		steps_final = current_position[E_AXIS] * axis_steps_per_unit[E_AXIS];
		//steps_final = st_get_position(E_AXIS);
		lcdDrawUpdate = 1;
		e_steps_per_unit = ((float)(steps_final)) / 100.0f;
		if (e_steps_per_unit < MIN_E_STEPS_PER_UNIT) e_steps_per_unit = MIN_E_STEPS_PER_UNIT;				
		if (e_steps_per_unit > MAX_E_STEPS_PER_UNIT) e_steps_per_unit = MAX_E_STEPS_PER_UNIT;

		lcd_implementation_clear();

		axis_steps_per_unit[E_AXIS] = e_steps_per_unit;
		enquecommand_P(PSTR("M500")); //store settings to eeprom
	
		//lcd_implementation_drawedit(PSTR("Result"), ftostr31(axis_steps_per_unit[E_AXIS]));
		//delay_keep_alive(2000);
		delay_keep_alive(500);
		lcd_show_fullscreen_message_and_wait_P(MSG_CLEAN_NOZZLE_E);
		lcd_update_enable(true);
		lcdDrawUpdate = 2;

	}
	else
	{
		lcd_implementation_clear();
		lcd.setCursor(0, 0);
		lcd_printPGM(MSG_ERROR);
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_PREHEAT_NOZZLE);
		delay(2000);
		lcd_implementation_clear();
	}
	lcd_return_to_status();
}

void lcd_extr_cal_reset() {
	float tmp1[] = DEFAULT_AXIS_STEPS_PER_UNIT;
	axis_steps_per_unit[E_AXIS] = tmp1[3];
	//extrudemultiply = 100;
	enquecommand_P(PSTR("M500"));
}*/

#endif

void lcd_toshiba_flash_air_compatibility_toggle()
{
   card.ToshibaFlashAir_enable(! card.ToshibaFlashAir_isEnabled());
   eeprom_update_byte((uint8_t*)EEPROM_TOSHIBA_FLASH_AIR_COMPATIBLITY, card.ToshibaFlashAir_isEnabled());
}

void lcd_v2_calibration() {
	bool loaded = lcd_show_fullscreen_message_yes_no_and_wait_P(MSG_PLA_FILAMENT_LOADED, false, true);
	if (loaded) {
		lcd_commands_type = LCD_COMMAND_V2_CAL;
	}
	else {
		lcd_display_message_fullscreen_P(MSG_PLEASE_LOAD_PLA);
		for (int i = 0; i < 20; i++) { //wait max. 2s
			delay_keep_alive(100);
			if (lcd_clicked()) {
				while (lcd_clicked());
				delay(10);
				while (lcd_clicked());
				break;
			}
		}
	}
	lcd_return_to_status();
	lcd_update_enable(true);
}

void lcd_wizard() {
	bool result = true;
	if (calibration_status() != CALIBRATION_STATUS_ASSEMBLED) {
		result = lcd_show_multiscreen_message_yes_no_and_wait_P(MSG_WIZARD_RERUN, false, false);
	}
	if (result) {
		calibration_status_store(CALIBRATION_STATUS_ASSEMBLED);
		lcd_wizard(0);
	}
	else {
		lcd_return_to_status();
		lcd_update_enable(true);
		lcd_update(2);
	}
}

void lcd_wizard(int state) {

	bool end = false;
	int wizard_event;
	const char *msg = NULL;
	while (!end) {
		switch (state) {
		case 0: // run wizard?
			wizard_event = lcd_show_multiscreen_message_yes_no_and_wait_P(MSG_WIZARD_WELCOME, false, true);
			if (wizard_event) {
				state = 1;
				eeprom_write_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 1);
			}
			else {
				eeprom_write_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 0);
				end = true;
			}
			break;
		case 1: // restore calibration status
			switch (calibration_status()) {
			case CALIBRATION_STATUS_ASSEMBLED: state = 2; break; //run selftest
			case CALIBRATION_STATUS_XYZ_CALIBRATION: state = 3; break; //run xyz cal.
			case CALIBRATION_STATUS_Z_CALIBRATION: state = 4; break; //run z cal.
			case CALIBRATION_STATUS_LIVE_ADJUST: state = 5; break; //run live adjust
			case CALIBRATION_STATUS_CALIBRATED: end = true; eeprom_write_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 0); break;
			default: state = 2; break; //if calibration status is unknown, run wizard from the beginning
			}
			break; 
		case 2: //selftest
			lcd_show_fullscreen_message_and_wait_P(MSG_WIZARD_SELFTEST);
			wizard_event = lcd_selftest();
			if (wizard_event) {
				calibration_status_store(CALIBRATION_STATUS_XYZ_CALIBRATION);
				state = 3;
			}
			else end = true;
			break;
		case 3: //xyz cal.
			lcd_show_fullscreen_message_and_wait_P(MSG_WIZARD_XYZ_CAL);
			wizard_event = gcode_M45(false, 0);
			if (wizard_event) state = 5;
			else end = true;
			break;
		case 4: //z cal.
			lcd_show_fullscreen_message_and_wait_P(MSG_WIZARD_Z_CAL);
			wizard_event = lcd_show_fullscreen_message_yes_no_and_wait_P(MSG_STEEL_SHEET_CHECK, false, false);
			if (!wizard_event) lcd_show_fullscreen_message_and_wait_P(MSG_PLACE_STEEL_SHEET);
			wizard_event = gcode_M45(true, 0);
			if (wizard_event) state = 11; //shipped, no need to set first layer, go to final message directly
			else end = true;
			break;
		case 5: //is filament loaded?
				//start to preheat nozzle and bed to save some time later
			setTargetHotend(PLA_PREHEAT_HOTEND_TEMP, 0);
			setTargetBed(PLA_PREHEAT_HPB_TEMP);
			wizard_event = lcd_show_fullscreen_message_yes_no_and_wait_P(MSG_WIZARD_FILAMENT_LOADED, false);
			if (wizard_event) state = 8;
			else state = 6;

			break;
		case 6: //waiting for preheat nozzle for PLA;
#ifndef SNMM
			lcd_display_message_fullscreen_P(MSG_WIZARD_WILL_PREHEAT);
			current_position[Z_AXIS] = 100; //move in z axis to make space for loading filament
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], homing_feedrate[Z_AXIS] / 60, active_extruder);
			delay_keep_alive(2000);
			lcd_display_message_fullscreen_P(MSG_WIZARD_HEATING);
			while (abs(degHotend(0) - PLA_PREHEAT_HOTEND_TEMP) > 3) {
				lcd_display_message_fullscreen_P(MSG_WIZARD_HEATING);

				lcd.setCursor(0, 4);
				lcd.print(LCD_STR_THERMOMETER[0]);
				lcd.print(ftostr3(degHotend(0)));
				lcd.print("/");
				lcd.print(PLA_PREHEAT_HOTEND_TEMP);
				lcd.print(LCD_STR_DEGREE);
				lcd_set_custom_characters();
				delay_keep_alive(1000);
			}
#endif //not SNMM
			state = 7;
			break;
		case 7: //load filament 
#ifdef PAT9125
			fsensor_block();
#endif //PAT9125
			lcd_show_fullscreen_message_and_wait_P(MSG_WIZARD_LOAD_FILAMENT);
			lcd_update_enable(false);
			lcd_implementation_clear();
			lcd_print_at_PGM(0, 2, MSG_LOADING_FILAMENT);
#ifdef SNMM
			change_extr(0);
#endif
			gcode_M701();
#ifdef PAT9125
			fsensor_unblock();
#endif //PAT9125
			state = 9;
			break;
		case 8:
			wizard_event = lcd_show_fullscreen_message_yes_no_and_wait_P(MSG_WIZARD_PLA_FILAMENT, false, true);
			if (wizard_event) state = 9;
			else end = true;
			break;
		case 9:
			lcd_show_fullscreen_message_and_wait_P(MSG_WIZARD_V2_CAL);
			lcd_show_fullscreen_message_and_wait_P(MSG_WIZARD_V2_CAL_2);
			lcd_commands_type = LCD_COMMAND_V2_CAL;
			end = true;
			break;
		case 10: //repeat first layer cal.?
			wizard_event = lcd_show_multiscreen_message_yes_no_and_wait_P(MSG_WIZARD_REPEAT_V2_CAL, false);
			if (wizard_event) {
				//reset status and live adjust z value in eeprom
				calibration_status_store(CALIBRATION_STATUS_LIVE_ADJUST);
				lcd_show_fullscreen_message_and_wait_P(MSG_WIZARD_CLEAN_HEATBED);
				state = 9;
			}
			else {
				state = 11;
			}
			break;
		case 11: //we are finished
			eeprom_write_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 0);
			end = true;
			break;

		default: break;
		}
	}

	SERIAL_ECHOPGM("State: ");
	MYSERIAL.println(state);
	switch (state) { //final message
	case 0: //user dont want to use wizard
		msg = MSG_WIZARD_QUIT;
		break;

	case 1: //printer was already calibrated
		msg = MSG_WIZARD_DONE;
		break;
	case 2: //selftest
		msg = MSG_WIZARD_CALIBRATION_FAILED;
		break;
	case 3: //xyz cal.
		msg = MSG_WIZARD_CALIBRATION_FAILED;
		break;
	case 4: //z cal.
		msg = MSG_WIZARD_CALIBRATION_FAILED;
		break;
	case 8:
		msg = MSG_WIZARD_INSERT_CORRECT_FILAMENT;
		break;
	case 9: break; //exit wizard for v2 calibration, which is implemted in lcd_commands (we need lcd_update running)
	case 11: //we are finished

		msg = MSG_WIZARD_DONE;
		lcd_reset_alert_level();
		lcd_setstatuspgm(WELCOME_MSG);
		break;

	default:
		msg = MSG_WIZARD_QUIT;
		break;

	}
	if (state != 9) lcd_show_fullscreen_message_and_wait_P(msg);
	lcd_update_enable(true);
	lcd_return_to_status();
	lcd_update(2);
}



static void lcd_settings_menu()
{
  EEPROM_read(EEPROM_SILENT, (uint8_t*)&SilentModeMenu, sizeof(SilentModeMenu));
  START_MENU();

  MENU_ITEM(back, MSG_MAIN, lcd_settings_menu_back);

  MENU_ITEM(submenu, MSG_TEMPERATURE, lcd_control_temperature_menu);
  if (!homing_flag)
  {
	  MENU_ITEM(submenu, MSG_MOVE_AXIS, lcd_move_menu_1mm);
  }
  if (!isPrintPaused)
  {
	  MENU_ITEM(gcode, MSG_DISABLE_STEPPERS, PSTR("M84"));
  }
#ifndef TMC2130
  if (!farm_mode) { //dont show in menu if we are in farm mode
	  switch (SilentModeMenu) {
	  case SILENT_MODE_POWER: MENU_ITEM(function, MSG_SILENT_MODE_OFF, lcd_silent_mode_set); break;
	  case SILENT_MODE_SILENT: MENU_ITEM(function, MSG_SILENT_MODE_ON, lcd_silent_mode_set); break;
	  case SILENT_MODE_AUTO: MENU_ITEM(function, MSG_AUTO_MODE_ON, lcd_silent_mode_set); break;
	  default: MENU_ITEM(function, MSG_SILENT_MODE_OFF, lcd_silent_mode_set); break; // (probably) not needed
	  }
  }
#endif //TMC2130

#ifdef PAT9125
#ifndef DEBUG_DISABLE_FSENSORCHECK
  if (FSensorStateMenu == 0) {
      if (fsensor_not_responding){
          // Filament sensor not working
          MENU_ITEM(function, MSG_FSENSOR_NA, lcd_fsensor_state_set);
          MENU_ITEM(function, MSG_FSENS_AUTOLOAD_NA, lcd_fsensor_fail);
      }
      else{
          // Filament sensor turned off, working, no problems
          MENU_ITEM(function, MSG_FSENSOR_OFF, lcd_fsensor_state_set);
          MENU_ITEM(function, MSG_FSENS_AUTOLOAD_NA, lcd_filament_autoload_info);
      }
  } else {
      // Filament sensor turned on, working, no problems
      MENU_ITEM(function, MSG_FSENSOR_ON, lcd_fsensor_state_set);
      
      if ((filament_autoload_enabled == true)) {
          MENU_ITEM(function, MSG_FSENS_AUTOLOAD_ON, lcd_set_filament_autoload);
      }
      else {
          MENU_ITEM(function, MSG_FSENS_AUTOLOAD_OFF, lcd_set_filament_autoload);
      }
      
  }
#endif //DEBUG_DISABLE_FSENSORCHECK
#endif //PAT9125

  if (fans_check_enabled == true) {
	  MENU_ITEM(function, MSG_FANS_CHECK_ON, lcd_set_fan_check);
  }
  else {
	  MENU_ITEM(function, MSG_FANS_CHECK_OFF, lcd_set_fan_check);
  }

#ifdef TMC2130
  if(!farm_mode)
  {
//*** MaR::180416_01a
    if (SilentModeMenu == SILENT_MODE_NORMAL) MENU_ITEM(function, MSG_STEALTH_MODE_OFF, lcd_silent_mode_set);
    else MENU_ITEM(function, MSG_STEALTH_MODE_ON, lcd_silent_mode_set);
    if (SilentModeMenu == SILENT_MODE_NORMAL)
    {
      if (CrashDetectMenu == 0) MENU_ITEM(function, MSG_CRASHDETECT_OFF, lcd_crash_mode_set);
      else MENU_ITEM(function, MSG_CRASHDETECT_ON, lcd_crash_mode_set);
    }
    else MENU_ITEM(submenu, MSG_CRASHDETECT_NA, lcd_crash_mode_info);
  }
  MENU_ITEM_EDIT(wfac, MSG_EXTRUDER_CORRECTION,  &tmc2130_wave_fac[E_AXIS],  TMC2130_WAVE_FAC1000_MIN-TMC2130_WAVE_FAC1000_STP, TMC2130_WAVE_FAC1000_MAX);
#endif //TMC2130

  if (temp_cal_active == false) {
	  MENU_ITEM(function, MSG_TEMP_CALIBRATION_OFF, lcd_temp_calibration_set);
  }
  else {
	  MENU_ITEM(function, MSG_TEMP_CALIBRATION_ON, lcd_temp_calibration_set);
  }
#ifdef HAS_SECOND_SERIAL_PORT
  if (selectedSerialPort == 0) {
	  MENU_ITEM(function, MSG_SECOND_SERIAL_OFF, lcd_second_serial_set);
  }
  else {
	  MENU_ITEM(function, MSG_SECOND_SERIAL_ON, lcd_second_serial_set);
  }
#endif //HAS_SECOND_SERIAL

  if (!isPrintPaused && !homing_flag)
	{
		MENU_ITEM(submenu, MSG_BABYSTEP_Z, lcd_babystep_z);
	}
	MENU_ITEM(submenu, MSG_LANGUAGE_SELECT, lcd_language_menu);

  if (card.ToshibaFlashAir_isEnabled()) {
    MENU_ITEM(function, MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON, lcd_toshiba_flash_air_compatibility_toggle);
  } else {
    MENU_ITEM(function, MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF, lcd_toshiba_flash_air_compatibility_toggle);
  }

  #ifdef SDCARD_SORT_ALPHA
	  if (!farm_mode) {
	  uint8_t sdSort;
	  EEPROM_read(EEPROM_SD_SORT, (uint8_t*)&sdSort, sizeof(sdSort));
	  switch (sdSort) {
		  case SD_SORT_TIME: MENU_ITEM(function, MSG_SORT_TIME, lcd_sort_type_set); break;
		  case SD_SORT_ALPHA: MENU_ITEM(function, MSG_SORT_ALPHA, lcd_sort_type_set); break;
		  default: MENU_ITEM(function, MSG_SORT_NONE, lcd_sort_type_set);
	  }
  }
  #endif // SDCARD_SORT_ALPHA
    
    if (farm_mode)
    {
        MENU_ITEM(submenu, PSTR("Farm number"), lcd_farm_no);
		MENU_ITEM(function, PSTR("Disable farm mode"), lcd_disable_farm_mode);
    }

	END_MENU();
}

static void lcd_selftest_()
{
	lcd_selftest();
}

#ifdef TMC2130
static void lcd_ustep_linearity_menu_save()
{
    eeprom_update_word((uint16_t*)EEPROM_TMC2130_WAVE_X_FAC, tmc2130_wave_fac[X_AXIS]);
    eeprom_update_word((uint16_t*)EEPROM_TMC2130_WAVE_Y_FAC, tmc2130_wave_fac[Y_AXIS]);
    eeprom_update_word((uint16_t*)EEPROM_TMC2130_WAVE_Z_FAC, tmc2130_wave_fac[Z_AXIS]);
    eeprom_update_word((uint16_t*)EEPROM_TMC2130_WAVE_E_FAC, tmc2130_wave_fac[E_AXIS]);
}
#endif //TMC2130

static void lcd_settings_menu_back()
{
#ifdef TMC2130
    bool changed = false;
    if (tmc2130_wave_fac[E_AXIS] < TMC2130_WAVE_FAC1000_MIN) tmc2130_wave_fac[E_AXIS] = 0;
    changed |= (eeprom_read_word((uint16_t*)EEPROM_TMC2130_WAVE_E_FAC) != tmc2130_wave_fac[E_AXIS]);
    lcd_ustep_linearity_menu_save();
    if (changed) tmc2130_init();
#endif //TMC2130
    currentMenu = lcd_main_menu;
    lcd_main_menu();
}
#ifdef EXPERIMENTAL_FEATURES

static void lcd_experimantal_menu();
static void lcd_homing_accuracy_menu();

static void lcd_accurate_home_set()
{
	tmc2130_home_enabled = tmc2130_home_enabled?0:1;
	eeprom_update_byte((uint8_t*)EEPROM_TMC2130_HOME_ENABLED, tmc2130_home_enabled);
}

static void lcd_homing_accuracy_menu_advanced_reset()
{
	tmc2130_home_bsteps[X_AXIS] = 48;
	tmc2130_home_fsteps[X_AXIS] = 48;
	tmc2130_home_bsteps[Y_AXIS] = 48;
	tmc2130_home_fsteps[Y_AXIS] = 48;
}

static void lcd_homing_accuracy_menu_advanced_save()
{
	eeprom_update_byte((uint8_t*)EEPROM_TMC2130_HOME_X_ORIGIN, tmc2130_home_origin[X_AXIS]);
	eeprom_update_byte((uint8_t*)EEPROM_TMC2130_HOME_X_BSTEPS, tmc2130_home_bsteps[X_AXIS]);
	eeprom_update_byte((uint8_t*)EEPROM_TMC2130_HOME_X_FSTEPS, tmc2130_home_fsteps[X_AXIS]);
	eeprom_update_byte((uint8_t*)EEPROM_TMC2130_HOME_Y_ORIGIN, tmc2130_home_origin[Y_AXIS]);
	eeprom_update_byte((uint8_t*)EEPROM_TMC2130_HOME_Y_BSTEPS, tmc2130_home_bsteps[Y_AXIS]);
	eeprom_update_byte((uint8_t*)EEPROM_TMC2130_HOME_Y_FSTEPS, tmc2130_home_fsteps[Y_AXIS]);
}

static void lcd_homing_accuracy_menu_advanced_back()
{
	lcd_homing_accuracy_menu_advanced_save();
	currentMenu = lcd_homing_accuracy_menu;
	lcd_homing_accuracy_menu();
}

static void lcd_homing_accuracy_menu_advanced()
{
	lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
	START_MENU();
	MENU_ITEM(back, PSTR("Homing accuracy"), lcd_homing_accuracy_menu_advanced_back);
	MENU_ITEM(function, PSTR("Reset def. steps"), lcd_homing_accuracy_menu_advanced_reset);
	MENU_ITEM_EDIT(byte3, PSTR("X-origin"),  &tmc2130_home_origin[X_AXIS],  0, 63);
	MENU_ITEM_EDIT(byte3, PSTR("Y-origin"),  &tmc2130_home_origin[Y_AXIS],  0, 63);
	MENU_ITEM_EDIT(byte3, PSTR("X-bsteps"),  &tmc2130_home_bsteps[X_AXIS],  0, 128);
	MENU_ITEM_EDIT(byte3, PSTR("Y-bsteps"),  &tmc2130_home_bsteps[Y_AXIS],  0, 128);
	MENU_ITEM_EDIT(byte3, PSTR("X-fsteps"),  &tmc2130_home_fsteps[X_AXIS],  0, 128);
	MENU_ITEM_EDIT(byte3, PSTR("Y-fsteps"),  &tmc2130_home_fsteps[Y_AXIS],  0, 128);
	END_MENU();
}

static void lcd_homing_accuracy_menu()
{
	START_MENU();
	MENU_ITEM(back, PSTR("Experimental"), 0);
	MENU_ITEM(function, tmc2130_home_enabled?PSTR("Accur. homing  On"):PSTR("Accur. homing Off"), lcd_accurate_home_set);
    MENU_ITEM(gcode, PSTR("Calibrate X"), PSTR("G28XC"));
    MENU_ITEM(gcode, PSTR("Calibrate Y"), PSTR("G28YC"));
	MENU_ITEM(submenu, PSTR("Advanced"), lcd_homing_accuracy_menu_advanced);
	END_MENU();
}

static void lcd_ustep_resolution_menu_save()
{
	eeprom_update_byte((uint8_t*)EEPROM_TMC2130_X_MRES, tmc2130_mres[X_AXIS]);
	eeprom_update_byte((uint8_t*)EEPROM_TMC2130_Y_MRES, tmc2130_mres[Y_AXIS]);
	eeprom_update_byte((uint8_t*)EEPROM_TMC2130_Z_MRES, tmc2130_mres[Z_AXIS]);
	eeprom_update_byte((uint8_t*)EEPROM_TMC2130_E_MRES, tmc2130_mres[E_AXIS]);
}

static void lcd_ustep_resolution_menu_back()
{
	float tmp1[]=DEFAULT_AXIS_STEPS_PER_UNIT;
	bool changed = false;
	if (tmc2130_mres[X_AXIS] != eeprom_read_byte((uint8_t*)EEPROM_TMC2130_X_MRES))
	{
		axis_steps_per_unit[X_AXIS] = tmp1[X_AXIS] * tmc2130_mres2usteps(tmc2130_mres[X_AXIS]) / TMC2130_USTEPS_XY;
		changed = true;
	}
	if (tmc2130_mres[Y_AXIS] != eeprom_read_byte((uint8_t*)EEPROM_TMC2130_Y_MRES))
	{
		axis_steps_per_unit[Y_AXIS] = tmp1[Y_AXIS] * tmc2130_mres2usteps(tmc2130_mres[Y_AXIS]) / TMC2130_USTEPS_XY;
		changed = true;
	}
	if (tmc2130_mres[Z_AXIS] != eeprom_read_byte((uint8_t*)EEPROM_TMC2130_Z_MRES))
	{
		axis_steps_per_unit[Z_AXIS] = tmp1[Z_AXIS] * tmc2130_mres2usteps(tmc2130_mres[Z_AXIS]) / TMC2130_USTEPS_Z;
		changed = true;
	}
	if (tmc2130_mres[E_AXIS] != eeprom_read_byte((uint8_t*)EEPROM_TMC2130_E_MRES))
	{
		axis_steps_per_unit[E_AXIS] = tmp1[E_AXIS] * tmc2130_mres2usteps(tmc2130_mres[E_AXIS]) / TMC2130_USTEPS_E;
		changed = true;
	}
    if (changed)
	{
		lcd_ustep_resolution_menu_save();
		Config_StoreSettings(EEPROM_OFFSET);
		tmc2130_init();
	}
	currentMenu = lcd_experimantal_menu;
	lcd_experimantal_menu();
}

static void lcd_ustep_resolution_reset_def_xyze()
{
	tmc2130_mres[X_AXIS] = tmc2130_usteps2mres(TMC2130_USTEPS_XY);
	tmc2130_mres[Y_AXIS] = tmc2130_usteps2mres(TMC2130_USTEPS_XY);
	tmc2130_mres[Z_AXIS] = tmc2130_usteps2mres(TMC2130_USTEPS_Z);
	tmc2130_mres[E_AXIS] = tmc2130_usteps2mres(TMC2130_USTEPS_E);
	float tmp1[]=DEFAULT_AXIS_STEPS_PER_UNIT;
	axis_steps_per_unit[X_AXIS] = tmp1[X_AXIS];
	axis_steps_per_unit[Y_AXIS] = tmp1[Y_AXIS];
	axis_steps_per_unit[Z_AXIS] = tmp1[Z_AXIS];
	axis_steps_per_unit[E_AXIS] = tmp1[E_AXIS];
}

static void lcd_ustep_resolution_menu()
{
	lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
	START_MENU();
	MENU_ITEM(back, PSTR("Experimental"), lcd_ustep_resolution_menu_back);
	MENU_ITEM(function, PSTR("Reset defaults"),  lcd_ustep_resolution_reset_def_xyze);
	MENU_ITEM_EDIT(mres, PSTR("X-resolution"),  &tmc2130_mres[X_AXIS],  4, 4);
	MENU_ITEM_EDIT(mres, PSTR("Y-resolution"),  &tmc2130_mres[Y_AXIS],  4, 4);
	MENU_ITEM_EDIT(mres, PSTR("Z-resolution"),  &tmc2130_mres[Z_AXIS],  4, 4);
	MENU_ITEM_EDIT(mres, PSTR("E-resolution"),  &tmc2130_mres[E_AXIS],  2, 5);
	END_MENU();
}



static void lcd_ustep_linearity_menu_back()
{
	bool changed = false;
	if (tmc2130_wave_fac[X_AXIS] < TMC2130_WAVE_FAC1000_MIN) tmc2130_wave_fac[X_AXIS] = 0;
	if (tmc2130_wave_fac[Y_AXIS] < TMC2130_WAVE_FAC1000_MIN) tmc2130_wave_fac[Y_AXIS] = 0;
	if (tmc2130_wave_fac[Z_AXIS] < TMC2130_WAVE_FAC1000_MIN) tmc2130_wave_fac[Z_AXIS] = 0;
	if (tmc2130_wave_fac[E_AXIS] < TMC2130_WAVE_FAC1000_MIN) tmc2130_wave_fac[E_AXIS] = 0;
	changed |= (eeprom_read_word((uint16_t*)EEPROM_TMC2130_WAVE_X_FAC) != tmc2130_wave_fac[X_AXIS]);
	changed |= (eeprom_read_word((uint16_t*)EEPROM_TMC2130_WAVE_Y_FAC) != tmc2130_wave_fac[Y_AXIS]);
	changed |= (eeprom_read_word((uint16_t*)EEPROM_TMC2130_WAVE_Z_FAC) != tmc2130_wave_fac[Z_AXIS]);
	changed |= (eeprom_read_word((uint16_t*)EEPROM_TMC2130_WAVE_E_FAC) != tmc2130_wave_fac[E_AXIS]);
	lcd_ustep_linearity_menu_save();
	if (changed) tmc2130_init();
	currentMenu = lcd_experimantal_menu;
	lcd_experimantal_menu();
}

static void lcd_ustep_linearity_menu_recomended()
{
	tmc2130_wave_fac[X_AXIS] = 220;
	tmc2130_wave_fac[Y_AXIS] = 220;
	tmc2130_wave_fac[Z_AXIS] = 220;
	tmc2130_wave_fac[E_AXIS] = 220;
}

static void lcd_ustep_linearity_menu_reset()
{
	tmc2130_wave_fac[X_AXIS] = 0;
	tmc2130_wave_fac[Y_AXIS] = 0;
	tmc2130_wave_fac[Z_AXIS] = 0;
	tmc2130_wave_fac[E_AXIS] = 0;
}

static void lcd_ustep_linearity_menu()
{
	lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
	START_MENU();
	MENU_ITEM(back, PSTR("Experimental"), lcd_ustep_linearity_menu_back);
	MENU_ITEM(function, PSTR("Reset correction"), lcd_ustep_linearity_menu_reset);
	MENU_ITEM(function, PSTR("Recomended config"), lcd_ustep_linearity_menu_recomended);
	MENU_ITEM_EDIT(wfac, PSTR("X-correction"),  &tmc2130_wave_fac[X_AXIS],  TMC2130_WAVE_FAC1000_MIN-TMC2130_WAVE_FAC1000_STP, TMC2130_WAVE_FAC1000_MAX);
	MENU_ITEM_EDIT(wfac, PSTR("Y-correction"),  &tmc2130_wave_fac[Y_AXIS],  TMC2130_WAVE_FAC1000_MIN-TMC2130_WAVE_FAC1000_STP, TMC2130_WAVE_FAC1000_MAX);
	MENU_ITEM_EDIT(wfac, PSTR("Z-correction"),  &tmc2130_wave_fac[Z_AXIS],  TMC2130_WAVE_FAC1000_MIN-TMC2130_WAVE_FAC1000_STP, TMC2130_WAVE_FAC1000_MAX);
	MENU_ITEM_EDIT(wfac, PSTR("E-correction"),  &tmc2130_wave_fac[E_AXIS],  TMC2130_WAVE_FAC1000_MIN-TMC2130_WAVE_FAC1000_STP, TMC2130_WAVE_FAC1000_MAX);
	END_MENU();
}

static void lcd_experimantal_menu_save_all()
{
	eeprom_update_byte((uint8_t*)EEPROM_TMC2130_HOME_ENABLED, tmc2130_home_enabled);
	lcd_ustep_resolution_menu_save();
	lcd_ustep_linearity_menu_save();
	Config_StoreSettings(EEPROM_OFFSET);
}

static void lcd_experimantal_menu_disable_all()
{
	tmc2130_home_enabled = 0;
	lcd_ustep_resolution_reset_def_xyze();
	lcd_ustep_linearity_menu_reset();
	lcd_experimantal_menu_save_all();
	tmc2130_init();
}

static void lcd_experimantal_menu()
{
	START_MENU();
	MENU_ITEM(back, MSG_MAIN, 0);
	MENU_ITEM(function, PSTR("All Xfeatures off"), lcd_experimantal_menu_disable_all);
	MENU_ITEM(submenu, PSTR("Homing accuracy"), lcd_homing_accuracy_menu);
	MENU_ITEM(submenu, PSTR("uStep resolution"), lcd_ustep_resolution_menu);
	MENU_ITEM(submenu, PSTR("uStep linearity"), lcd_ustep_linearity_menu);
	END_MENU();
}
#endif //EXPERIMENTAL_FEATURES


static void lcd_calibration_menu()
{
  START_MENU();
  MENU_ITEM(back, MSG_MAIN, 0);
  if (!isPrintPaused)
  {
	MENU_ITEM(function, MSG_WIZARD, lcd_wizard);
	MENU_ITEM(submenu, MSG_V2_CALIBRATION, lcd_v2_calibration);
	MENU_ITEM(gcode, MSG_AUTO_HOME, PSTR("G28 W"));
	MENU_ITEM(function, MSG_SELFTEST, lcd_selftest_v);
#ifdef MK1BP
    // MK1
    // "Calibrate Z"
    MENU_ITEM(gcode, MSG_HOMEYZ, PSTR("G28 Z"));
#else //MK1BP
    // MK2
    MENU_ITEM(function, MSG_CALIBRATE_BED, lcd_mesh_calibration);
    // "Calibrate Z" with storing the reference values to EEPROM.
    MENU_ITEM(submenu, MSG_HOMEYZ, lcd_mesh_calibration_z);
#ifndef SNMM
	//MENU_ITEM(function, MSG_CALIBRATE_E, lcd_calibrate_extruder);
#endif
    // "Mesh Bed Leveling"
    MENU_ITEM(submenu, MSG_MESH_BED_LEVELING, lcd_mesh_bedleveling);
	
#endif //MK1BP
    MENU_ITEM(submenu, MSG_BED_CORRECTION_MENU, lcd_adjust_bed);
	MENU_ITEM(submenu, MSG_PID_EXTRUDER, pid_extruder);
    MENU_ITEM(submenu, MSG_SHOW_END_STOPS, menu_show_end_stops);
#ifndef MK1BP
    MENU_ITEM(gcode, MSG_CALIBRATE_BED_RESET, PSTR("M44"));
#endif //MK1BP
#ifndef SNMM
	//MENU_ITEM(function, MSG_RESET_CALIBRATE_E, lcd_extr_cal_reset);
#endif
#ifndef MK1BP
	MENU_ITEM(submenu, MSG_CALIBRATION_PINDA_MENU, lcd_pinda_calibration_menu);
#endif //MK1BP
  }
  
  END_MENU();
}
/*
void lcd_mylang_top(int hlaska) {
    lcd.setCursor(0,0);
    lcd.print("                    ");
    lcd.setCursor(0,0);
    lcd_printPGM(MSG_ALL[hlaska-1][LANGUAGE_SELECT]);   
}

void lcd_mylang_drawmenu(int cursor) {
  int first = 0;
  if (cursor>2) first = cursor-2;
  if (cursor==LANG_NUM) first = LANG_NUM-3;
  lcd.setCursor(0, 1);
  lcd.print("                    ");
  lcd.setCursor(1, 1);
  lcd_printPGM(MSG_ALL[first][LANGUAGE_NAME]);

  lcd.setCursor(0, 2);
  lcd.print("                    ");
  lcd.setCursor(1, 2);
  lcd_printPGM(MSG_ALL[first+1][LANGUAGE_NAME]);

  lcd.setCursor(0, 3);
  lcd.print("                    ");
  lcd.setCursor(1, 3);
  lcd_printPGM(MSG_ALL[first+2][LANGUAGE_NAME]);  
  
  if (cursor==1) lcd.setCursor(0, 1);
  if (cursor>1 && cursor<LANG_NUM) lcd.setCursor(0, 2);
  if (cursor==LANG_NUM) lcd.setCursor(0, 3);

  lcd.print(">");
  
  if (cursor<LANG_NUM-1) {
    lcd.setCursor(19,3);
    lcd.print("\x01");
  }
  if (cursor>2) {
    lcd.setCursor(19,1);
    lcd.print("^");
  }  
}
*/

void lcd_mylang_drawmenu(int cursor) {
  int first = 0;
  if (cursor>3) first = cursor-3;
  if (cursor==LANG_NUM && LANG_NUM>4) first = LANG_NUM-4;
  if (cursor==LANG_NUM && LANG_NUM==4) first = LANG_NUM-4;


  lcd.setCursor(0, 0);
  lcd.print("                    ");
  lcd.setCursor(1, 0);
  lcd_printPGM(MSG_LANGUAGE_NAME_EXPLICIT(first+0));

  lcd.setCursor(0, 1);
  lcd.print("                    ");
  lcd.setCursor(1, 1);
  lcd_printPGM(MSG_LANGUAGE_NAME_EXPLICIT(first+1));

  lcd.setCursor(0, 2);
  lcd.print("                    ");

  if (LANG_NUM > 2){
    lcd.setCursor(1, 2);
    lcd_printPGM(MSG_LANGUAGE_NAME_EXPLICIT(first+2));
  }

  lcd.setCursor(0, 3);
  lcd.print("                    ");
  if (LANG_NUM>3) {
    lcd.setCursor(1, 3);
    lcd_printPGM(MSG_LANGUAGE_NAME_EXPLICIT(first+3));
  }
  
  if (cursor==1) lcd.setCursor(0, 0);
  if (cursor==2) lcd.setCursor(0, 1);
  if (cursor>2) lcd.setCursor(0, 2);
  if (cursor==LANG_NUM && LANG_NUM>3) lcd.setCursor(0, 3);

  lcd.print(">");
  
  if (cursor<LANG_NUM-1 && LANG_NUM>4) {
    lcd.setCursor(19,3);
    lcd.print("\x01");
  }
  if (cursor>3 && LANG_NUM>4) {
    lcd.setCursor(19,0);
    lcd.print("^");
  }  
}
 
void lcd_mylang_drawcursor(int cursor) {
  
  if (cursor==1) lcd.setCursor(0, 1);
  if (cursor>1 && cursor<LANG_NUM) lcd.setCursor(0, 2);
  if (cursor==LANG_NUM) lcd.setCursor(0, 3);

  lcd.print(">");
  
}  

void lcd_mylang() {
  int enc_dif = 0;
  int cursor_pos = 1;
  lang_selected=255;
  int hlaska=1;
  int counter=0;
  lcd_set_custom_characters_arrows();

  lcd_implementation_clear();

  //lcd_mylang_top(hlaska);

  lcd_mylang_drawmenu(cursor_pos);


  enc_dif = encoderDiff;

  while ( (lang_selected == 255)  ) {

    manage_heater();
    manage_inactivity(true);

    if ( abs((enc_dif - encoderDiff)) > 4 ) {

      //if ( (abs(enc_dif - encoderDiff)) > 1 ) {
        if (enc_dif > encoderDiff ) {
          cursor_pos --;
        }

        if (enc_dif < encoderDiff  ) {
          cursor_pos ++;
        }

        if (cursor_pos > LANG_NUM) {
          cursor_pos = LANG_NUM;
        }

        if (cursor_pos < 1) {
          cursor_pos = 1;
        }

        lcd_mylang_drawmenu(cursor_pos);
        enc_dif = encoderDiff;
        delay(100);
      //}

    } else delay(20);


    if (lcd_clicked()) {

      lcd_set_lang(cursor_pos-1);
      delay(500);

    }
    /*
    if (++counter == 80) {
      hlaska++;
      if(hlaska>LANG_NUM) hlaska=1;
      lcd_mylang_top(hlaska);
      lcd_mylang_drawcursor(cursor_pos);
      counter=0;
    }
    */
  };

  if(MYSERIAL.available() > 1){
    lang_selected = 0;
    firstrun = 0;
  }

  lcd_set_custom_characters_degree();
  lcd_implementation_clear();
  lcd_return_to_status();

}

void bowden_menu() {
	int enc_dif = encoderDiff;
	int cursor_pos = 0;
	lcd_implementation_clear();
	lcd.setCursor(0, 0);
	lcd.print(">");
	for (int i = 0; i < 4; i++) {
		lcd.setCursor(1, i);
		lcd.print("Extruder ");
		lcd.print(i);
		lcd.print(": ");
		EEPROM_read_B(EEPROM_BOWDEN_LENGTH + i * 2, &bowden_length[i]);
		lcd.print(bowden_length[i] - 48);

	}
	enc_dif = encoderDiff;

	while (1) {

		manage_heater();
		manage_inactivity(true);

		if (abs((enc_dif - encoderDiff)) > 2) {

			if (enc_dif > encoderDiff) {
					cursor_pos--;
				}

				if (enc_dif < encoderDiff) {
					cursor_pos++;
				}

				if (cursor_pos > 3) {
					cursor_pos = 3;
				}

				if (cursor_pos < 0) {
					cursor_pos = 0;
				}

				lcd.setCursor(0, 0);
				lcd.print(" ");
				lcd.setCursor(0, 1);
				lcd.print(" ");
				lcd.setCursor(0, 2);
				lcd.print(" ");
				lcd.setCursor(0, 3);
				lcd.print(" ");
				lcd.setCursor(0, cursor_pos);
				lcd.print(">");

				enc_dif = encoderDiff;
				delay(100);
		}

		if (lcd_clicked()) {
			while (lcd_clicked());
			delay(10);
			while (lcd_clicked());

			lcd_implementation_clear();
			while (1) {

				manage_heater();
				manage_inactivity(true);

				lcd.setCursor(1, 1);
				lcd.print("Extruder ");
				lcd.print(cursor_pos);
				lcd.print(": ");
				lcd.setCursor(13, 1);
				lcd.print(bowden_length[cursor_pos] - 48);

				if (abs((enc_dif - encoderDiff)) > 2) {
						if (enc_dif > encoderDiff) {
							bowden_length[cursor_pos]--;
							lcd.setCursor(13, 1);
							lcd.print(bowden_length[cursor_pos] - 48);
							enc_dif = encoderDiff;
						}

						if (enc_dif < encoderDiff) {
							bowden_length[cursor_pos]++;
							lcd.setCursor(13, 1);
							lcd.print(bowden_length[cursor_pos] - 48);
							enc_dif = encoderDiff;
						}
				}
				delay(100);
				if (lcd_clicked()) {
					while (lcd_clicked());
					delay(10);
					while (lcd_clicked());
					EEPROM_save_B(EEPROM_BOWDEN_LENGTH + cursor_pos * 2, &bowden_length[cursor_pos]);
					if (lcd_show_fullscreen_message_yes_no_and_wait_P(PSTR("Continue with another bowden?"))) {
						lcd_update_enable(true);
						lcd_implementation_clear();
						enc_dif = encoderDiff;
						lcd.setCursor(0, cursor_pos);
						lcd.print(">");
						for (int i = 0; i < 4; i++) {
							lcd.setCursor(1, i);
							lcd.print("Extruder ");
							lcd.print(i);
							lcd.print(": ");
							EEPROM_read_B(EEPROM_BOWDEN_LENGTH + i * 2, &bowden_length[i]);
							lcd.print(bowden_length[i] - 48);

						}
						break;
					}
					else return;
				}
			}
		}
	}
}

static char snmm_stop_print_menu() { //menu for choosing which filaments will be unloaded in stop print
	lcd_implementation_clear();
	lcd_print_at_PGM(0,0,MSG_UNLOAD_FILAMENT); lcd.print(":");
	lcd.setCursor(0, 1); lcd.print(">");
	lcd_print_at_PGM(1,1,MSG_ALL);
	lcd_print_at_PGM(1,2,MSG_USED);
	lcd_print_at_PGM(1,3,MSG_CURRENT);
	char cursor_pos = 1;
	int enc_dif = 0;
	KEEPALIVE_STATE(PAUSED_FOR_USER);
	while (1) {
		manage_heater();
		manage_inactivity(true);
		if (abs((enc_dif - encoderDiff)) > 4) {

			if ((abs(enc_dif - encoderDiff)) > 1) {
				if (enc_dif > encoderDiff) cursor_pos--;
				if (enc_dif < encoderDiff) cursor_pos++;
				if (cursor_pos > 3) cursor_pos = 3;
				if (cursor_pos < 1) cursor_pos = 1;

				lcd.setCursor(0, 1);
				lcd.print(" ");
				lcd.setCursor(0, 2);
				lcd.print(" ");
				lcd.setCursor(0, 3);
				lcd.print(" ");
				lcd.setCursor(0, cursor_pos);
				lcd.print(">");
				enc_dif = encoderDiff;
				delay(100);
			}
		}
		if (lcd_clicked()) {
			while (lcd_clicked());
			delay(10);
			while (lcd_clicked());
			KEEPALIVE_STATE(IN_HANDLER);
			return(cursor_pos - 1);
		}
	}
	
}

char choose_extruder_menu() {

	int items_no = 4;
	int first = 0;
	int enc_dif = 0;
	char cursor_pos = 1;
	
	enc_dif = encoderDiff;
	lcd_implementation_clear();
	
	lcd_printPGM(MSG_CHOOSE_EXTRUDER);
	lcd.setCursor(0, 1);
	lcd.print(">");
	for (int i = 0; i < 3; i++) {
		lcd_print_at_PGM(1, i + 1, MSG_EXTRUDER);
	}
	KEEPALIVE_STATE(PAUSED_FOR_USER);
	while (1) {

		for (int i = 0; i < 3; i++) {
			lcd.setCursor(2 + strlen_P(MSG_EXTRUDER), i+1);
			lcd.print(first + i + 1);
		}

		manage_heater();
		manage_inactivity(true);

		if (abs((enc_dif - encoderDiff)) > 4) {

			if ((abs(enc_dif - encoderDiff)) > 1) {
				if (enc_dif > encoderDiff) {
					cursor_pos--;
				}

				if (enc_dif < encoderDiff) {
					cursor_pos++;
				}

				if (cursor_pos > 3) {
					cursor_pos = 3;
					if (first < items_no - 3) {
						first++;
						lcd_implementation_clear();
						lcd_printPGM(MSG_CHOOSE_EXTRUDER);
						for (int i = 0; i < 3; i++) {
							lcd_print_at_PGM(1, i + 1, MSG_EXTRUDER);
						}
					}
				}

				if (cursor_pos < 1) {
					cursor_pos = 1;
					if (first > 0) {
						first--;
						lcd_implementation_clear();
						lcd_printPGM(MSG_CHOOSE_EXTRUDER);
						for (int i = 0; i < 3; i++) {
							lcd_print_at_PGM(1, i + 1, MSG_EXTRUDER);
						}
					}
				}
				lcd.setCursor(0, 1);
				lcd.print(" ");
				lcd.setCursor(0, 2);
				lcd.print(" ");
				lcd.setCursor(0, 3);
				lcd.print(" ");
				lcd.setCursor(0, cursor_pos);
				lcd.print(">");
				enc_dif = encoderDiff;
				delay(100);
			}

		}

		if (lcd_clicked()) {
			lcd_update(2);
			while (lcd_clicked());
			delay(10);
			while (lcd_clicked());
			KEEPALIVE_STATE(IN_HANDLER);
			return(cursor_pos + first - 1);
			
		}

	}

}


char reset_menu() {
#ifdef SNMM
	int items_no = 5;
#else
	int items_no = 4;
#endif
	static int first = 0;
	int enc_dif = 0;
	char cursor_pos = 0;
	const char *item [items_no];
	
	item[0] = "Language";
	item[1] = "Statistics";
	item[2] = "Shipping prep";
	item[3] = "All Data";
#ifdef SNMM
	item[4] = "Bowden length";
#endif // SNMM

	enc_dif = encoderDiff;
	lcd_implementation_clear();
	lcd.setCursor(0, 0);
	lcd.print(">");

	while (1) {		

		for (int i = 0; i < 4; i++) {
			lcd.setCursor(1, i);
			lcd.print(item[first + i]);
		}

		manage_heater();
		manage_inactivity(true);

		if (abs((enc_dif - encoderDiff)) > 4) {

			if ((abs(enc_dif - encoderDiff)) > 1) {
				if (enc_dif > encoderDiff) {
					cursor_pos--;
				}

				if (enc_dif < encoderDiff) {
					cursor_pos++;
				}

				if (cursor_pos > 3) {
					cursor_pos = 3;
					if (first < items_no - 4) {
						first++;
						lcd_implementation_clear();
					}
				}

				if (cursor_pos < 0) {
					cursor_pos = 0;
					if (first > 0) {
						first--;
						lcd_implementation_clear();
					}
				}
				lcd.setCursor(0, 0);
				lcd.print(" ");
				lcd.setCursor(0, 1);
				lcd.print(" ");
				lcd.setCursor(0, 2);
				lcd.print(" ");
				lcd.setCursor(0, 3);
				lcd.print(" ");
				lcd.setCursor(0, cursor_pos);
				lcd.print(">");
				enc_dif = encoderDiff;
				delay(100);
			}

		}

		if (lcd_clicked()) {
			while (lcd_clicked());
			delay(10);
			while (lcd_clicked());
			return(cursor_pos + first);
		}

	}

}

static void lcd_disable_farm_mode() {
	int8_t disable = lcd_show_fullscreen_message_yes_no_and_wait_P(PSTR("Disable farm mode?"), true, false); //allow timeouting, default no
	if (disable) {
		enquecommand_P(PSTR("G99"));
		lcd_return_to_status();
	}
	else {
		lcd_goto_menu(lcd_settings_menu); //doesn't break menuStack
	}
	lcd_update_enable(true);
	lcdDrawUpdate = 2;
	
}

static void lcd_ping_allert() {
	if ((abs(millis() - allert_timer)*0.001) > PING_ALLERT_PERIOD) {
		allert_timer = millis();
		SET_OUTPUT(BEEPER);
		for (int i = 0; i < 2; i++) {
			WRITE(BEEPER, HIGH);
			delay(50);
			WRITE(BEEPER, LOW);
			delay(100);
		}
	}

};


#ifdef SNMM

static void extr_mov(float shift, float feed_rate) { //move extruder no matter what the current heater temperature is
	set_extrude_min_temp(.0);
	current_position[E_AXIS] += shift;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], feed_rate, active_extruder);
	set_extrude_min_temp(EXTRUDE_MINTEMP);
}


void change_extr(int extr) { //switches multiplexer for extruders
	st_synchronize();
	delay(100);

	disable_e0();
	disable_e1();
	disable_e2();

	snmm_extruder = extr;

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
}

static int get_ext_nr() { //reads multiplexer input pins and return current extruder number (counted from 0)
	return(2 * READ(E_MUX1_PIN) + READ(E_MUX0_PIN));
}


void display_loading() {
	switch (snmm_extruder) {
	case 1: lcd_display_message_fullscreen_P(MSG_FILAMENT_LOADING_T1); break;
	case 2: lcd_display_message_fullscreen_P(MSG_FILAMENT_LOADING_T2); break;
	case 3: lcd_display_message_fullscreen_P(MSG_FILAMENT_LOADING_T3); break;
	default: lcd_display_message_fullscreen_P(MSG_FILAMENT_LOADING_T0); break;
	}
}

void extr_adj(int extruder) //loading filament for SNMM
{
	bool correct;
	max_feedrate[E_AXIS] =80;
	//max_feedrate[E_AXIS] = 50;
	START:
	lcd_implementation_clear();
	lcd.setCursor(0, 0); 
	switch (extruder) {
	case 1: lcd_display_message_fullscreen_P(MSG_FILAMENT_LOADING_T1); break;
	case 2: lcd_display_message_fullscreen_P(MSG_FILAMENT_LOADING_T2); break;
	case 3: lcd_display_message_fullscreen_P(MSG_FILAMENT_LOADING_T3); break;
	default: lcd_display_message_fullscreen_P(MSG_FILAMENT_LOADING_T0); break;   
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
	lcd_implementation_clear();
	lcd.setCursor(0, 0); lcd_printPGM(MSG_LOADING_FILAMENT);
	if(strlen(MSG_LOADING_FILAMENT)>18) lcd.setCursor(0, 1);
	else lcd.print(" ");
	lcd.print(snmm_extruder + 1);
	lcd.setCursor(0, 2); lcd_printPGM(MSG_PLEASE_WAIT);
	st_synchronize();
	max_feedrate[E_AXIS] = 50;
	lcd_update_enable(true);
	lcd_return_to_status();
	lcdDrawUpdate = 2;
}


void extr_unload() { //unloads filament
	float tmp_motor[3] = DEFAULT_PWM_MOTOR_CURRENT;
	float tmp_motor_loud[3] = DEFAULT_PWM_MOTOR_CURRENT_LOUD;
	uint8_t SilentMode = eeprom_read_byte((uint8_t*)EEPROM_SILENT);

	if (degHotend0() > EXTRUDE_MINTEMP) {
		lcd_implementation_clear();
		lcd_display_message_fullscreen_P(PSTR(""));
		max_feedrate[E_AXIS] = 50;
		lcd.setCursor(0, 0); lcd_printPGM(MSG_UNLOADING_FILAMENT);
		lcd.print(" ");
		lcd.print(snmm_extruder + 1);
		lcd.setCursor(0, 2); lcd_printPGM(MSG_PLEASE_WAIT);
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
		current_position[E_AXIS] -= (bowden_length[snmm_extruder] + 60 + FIL_LOAD_LENGTH) / 2;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 500, active_extruder);
		current_position[E_AXIS] -= (bowden_length[snmm_extruder] + 60 + FIL_LOAD_LENGTH) / 2;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 500, active_extruder);
		st_synchronize();
		//st_current_init();
		if (SilentMode != SILENT_MODE_OFF) st_current_set(2, tmp_motor[2]); //set back to normal operation currents
		else st_current_set(2, tmp_motor_loud[2]);
		lcd_update_enable(true);
		lcd_return_to_status();
		max_feedrate[E_AXIS] = 50;
	}
	else {

		lcd_implementation_clear();
		lcd.setCursor(0, 0);
		lcd_printPGM(MSG_ERROR);
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_PREHEAT_NOZZLE);

		delay(2000);
		lcd_implementation_clear();
	}

	lcd_return_to_status();




}

//wrapper functions for loading filament
static void extr_adj_0(){
	change_extr(0);
	extr_adj(0);
}
static void extr_adj_1() {
	change_extr(1);
	extr_adj(1);
}
static void extr_adj_2() {
	change_extr(2);
	extr_adj(2);
}
static void extr_adj_3() {
	change_extr(3);
	extr_adj(3);
}

static void load_all() {
	for (int i = 0; i < 4; i++) {
		change_extr(i);
		extr_adj(i);
	}
}

//wrapper functions for changing extruders
static void extr_change_0() {
	change_extr(0);
	lcd_return_to_status();
}
static void extr_change_1() {
	change_extr(1);
	lcd_return_to_status();
}
static void extr_change_2() {
	change_extr(2);
	lcd_return_to_status();
}
static void extr_change_3() {
	change_extr(3);
	lcd_return_to_status();
}

//wrapper functions for unloading filament
void extr_unload_all() {
	if (degHotend0() > EXTRUDE_MINTEMP) {
		for (int i = 0; i < 4; i++) {
			change_extr(i);
			extr_unload();
		}
	}
	else {
		lcd_implementation_clear();
		lcd.setCursor(0, 0);
		lcd_printPGM(MSG_ERROR);
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_PREHEAT_NOZZLE);
		delay(2000);
		lcd_implementation_clear();
		lcd_return_to_status();
	}
}

//unloading just used filament (for snmm)

void extr_unload_used() {
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
		lcd_implementation_clear();
		lcd.setCursor(0, 0);
		lcd_printPGM(MSG_ERROR);
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_PREHEAT_NOZZLE);
		delay(2000);
		lcd_implementation_clear();
		lcd_return_to_status();
	}
}



static void extr_unload_0() {
	change_extr(0);
	extr_unload();
}
static void extr_unload_1() {
	change_extr(1);
	extr_unload();
}
static void extr_unload_2() {
	change_extr(2);
	extr_unload();
}
static void extr_unload_3() {
	change_extr(3);
	extr_unload();
}


static void fil_load_menu()
{
	START_MENU();
	MENU_ITEM(back, MSG_MAIN, 0);
	MENU_ITEM(function, MSG_LOAD_ALL, load_all);
	MENU_ITEM(function, MSG_LOAD_FILAMENT_1, extr_adj_0);
	MENU_ITEM(function, MSG_LOAD_FILAMENT_2, extr_adj_1);
	MENU_ITEM(function, MSG_LOAD_FILAMENT_3, extr_adj_2);
	MENU_ITEM(function, MSG_LOAD_FILAMENT_4, extr_adj_3);
	
	END_MENU();
}

static void fil_unload_menu()
{
	START_MENU();
	MENU_ITEM(back, MSG_MAIN, 0);
	MENU_ITEM(function, MSG_UNLOAD_ALL, extr_unload_all);
	MENU_ITEM(function, MSG_UNLOAD_FILAMENT_1, extr_unload_0);
	MENU_ITEM(function, MSG_UNLOAD_FILAMENT_2, extr_unload_1);
	MENU_ITEM(function, MSG_UNLOAD_FILAMENT_3, extr_unload_2);
	MENU_ITEM(function, MSG_UNLOAD_FILAMENT_4, extr_unload_3);

	END_MENU();
}

static void change_extr_menu(){
	START_MENU();
	MENU_ITEM(back, MSG_MAIN, 0);
	MENU_ITEM(function, MSG_EXTRUDER_1, extr_change_0);
	MENU_ITEM(function, MSG_EXTRUDER_2, extr_change_1);
	MENU_ITEM(function, MSG_EXTRUDER_3, extr_change_2);
	MENU_ITEM(function, MSG_EXTRUDER_4, extr_change_3);

	END_MENU();
}

#endif

static void lcd_farm_no()
{
	char step = 0;
	int enc_dif = 0;
	int _farmno = farm_no;
	int _ret = 0;
	lcd_implementation_clear();

	lcd.setCursor(0, 0);
	lcd.print("Farm no");

	do
	{

		if (abs((enc_dif - encoderDiff)) > 2) {
			if (enc_dif > encoderDiff) {
				switch (step) {
				case(0): if (_farmno >= 100) _farmno -= 100; break;
				case(1): if (_farmno % 100 >= 10) _farmno -= 10; break;
				case(2): if (_farmno % 10 >= 1) _farmno--; break;
				default: break;
				}
			}

			if (enc_dif < encoderDiff) {
				switch (step) {
				case(0): if (_farmno < 900) _farmno += 100; break;
				case(1): if (_farmno % 100 < 90) _farmno += 10; break;
				case(2): if (_farmno % 10 <= 8)_farmno++; break;
				default: break;
				}
			}
			enc_dif = 0;
			encoderDiff = 0;
		}

		lcd.setCursor(0, 2);
		if (_farmno < 100) lcd.print("0");
		if (_farmno < 10) lcd.print("0");
		lcd.print(_farmno);
		lcd.print("  ");
		lcd.setCursor(0, 3);
		lcd.print("   ");


		lcd.setCursor(step, 3);
		lcd.print("^");
		delay(100);

		if (lcd_clicked())
		{
			delay(200);
			step++;
			if(step == 3) {
				_ret = 1;
				farm_no = _farmno;
				EEPROM_save_B(EEPROM_FARM_NUMBER, &farm_no);
				prusa_statistics(20);
				lcd_return_to_status();
			}
		}

		manage_heater();
	} while (_ret == 0);

}


unsigned char lcd_choose_color() {
	//function returns index of currently chosen item
	//following part can be modified from 2 to 255 items:
	//-----------------------------------------------------
	unsigned char items_no = 2;
	const char *item[items_no];
	item[0] = "Orange";
	item[1] = "Black";
	//-----------------------------------------------------
	unsigned char active_rows;
	static int first = 0;
	int enc_dif = 0;
	unsigned char cursor_pos = 1;
	enc_dif = encoderDiff;
	lcd_implementation_clear();
	lcd.setCursor(0, 1);
	lcd.print(">");

	active_rows = items_no < 3 ? items_no : 3;

	while (1) {
		lcd_print_at_PGM(0, 0, PSTR("Choose color:"));
		for (int i = 0; i < active_rows; i++) {
			lcd.setCursor(1, i+1);
			lcd.print(item[first + i]);
		}

		manage_heater();
		manage_inactivity(true);
		proc_commands();
		if (abs((enc_dif - encoderDiff)) > 12) {
					
				if (enc_dif > encoderDiff) {
					cursor_pos--;
				}

				if (enc_dif < encoderDiff) {
					cursor_pos++;
				}
				
				if (cursor_pos > active_rows) {
					cursor_pos = active_rows;
					if (first < items_no - active_rows) {
						first++;
						lcd_implementation_clear();
					}
				}

				if (cursor_pos < 1) {
					cursor_pos = 1;
					if (first > 0) {
						first--;
						lcd_implementation_clear();
					}
				}
				lcd.setCursor(0, 1);
				lcd.print(" ");
				lcd.setCursor(0, 2);
				lcd.print(" ");
				lcd.setCursor(0, 3);
				lcd.print(" ");
				lcd.setCursor(0, cursor_pos);
				lcd.print(">");
				enc_dif = encoderDiff;
				delay(100);

		}

		if (lcd_clicked()) {
			while (lcd_clicked());
			delay(10);
			while (lcd_clicked());
			switch(cursor_pos + first - 1) {
			case 0: return 1; break;
			case 1: return 0; break;
			default: return 99; break;
			}
		}

	}

}

void lcd_confirm_print()
{
	uint8_t filament_type;
	int enc_dif = 0;
	int cursor_pos = 1;
	int _ret = 0;
	int _t = 0;

	enc_dif = encoderDiff;
	lcd_implementation_clear();

	lcd.setCursor(0, 0);
	lcd.print("Print ok ?");

	do
	{
		if (abs(enc_dif - encoderDiff) > 12) {
			if (enc_dif > encoderDiff) {
				cursor_pos--;
			}

			if (enc_dif < encoderDiff) {
				cursor_pos++;
			}
			enc_dif = encoderDiff;
		}

		if (cursor_pos > 2) { cursor_pos = 2; }
		if (cursor_pos < 1) { cursor_pos = 1; }

		lcd.setCursor(0, 2); lcd.print("          ");
		lcd.setCursor(0, 3); lcd.print("          ");
		lcd.setCursor(2, 2);
		lcd_printPGM(MSG_YES);
		lcd.setCursor(2, 3);
		lcd_printPGM(MSG_NO);
		lcd.setCursor(0, 1 + cursor_pos);
		lcd.print(">");
		delay(100);

		_t = _t + 1;
		if (_t>100)
		{
			prusa_statistics(99);
			_t = 0;
		}
		if (lcd_clicked())
		{
			if (cursor_pos == 1)
			{
				_ret = 1;
				filament_type = lcd_choose_color();
				prusa_statistics(4, filament_type);
				no_response = true; //we need confirmation by recieving PRUSA thx
				important_status = 4;
				saved_filament_type = filament_type;
				NcTime = millis();
			}
			if (cursor_pos == 2)
			{
				_ret = 2;
				filament_type = lcd_choose_color();
				prusa_statistics(5, filament_type);
				no_response = true; //we need confirmation by recieving PRUSA thx
				important_status = 5;				
				saved_filament_type = filament_type;
				NcTime = millis();
			}
		}

		manage_heater();
		manage_inactivity();
		proc_commands();

	} while (_ret == 0);

}

extern bool saved_printing;

static void lcd_main_menu()
{

  SDscrool = 0;
  START_MENU();

  // Majkl superawesome menu


 MENU_ITEM(back, MSG_WATCH, 0);

#ifdef RESUME_DEBUG 
 if (!saved_printing) 
  MENU_ITEM(function, PSTR("tst - Save"), lcd_menu_test_save);
 else
  MENU_ITEM(function, PSTR("tst - Restore"), lcd_menu_test_restore);
#endif //RESUME_DEBUG 

#ifdef TMC2130_DEBUG
 MENU_ITEM(function, PSTR("recover print"), recover_print);
 MENU_ITEM(function, PSTR("power panic"), uvlo_);
#endif //TMC2130_DEBUG

 /* if (farm_mode && !IS_SD_PRINTING )
    {
    
        int tempScrool = 0;
        if (lcdDrawUpdate == 0 && LCD_CLICKED == 0)
            //delay(100);
            return; // nothing to do (so don't thrash the SD card)
        uint16_t fileCnt = card.getnrfilenames();
        
        card.getWorkDirName();
        if (card.filename[0] == '/')
        {
#if SDCARDDETECT == -1
            MENU_ITEM(function, MSG_REFRESH, lcd_sd_refresh);
#endif
        } else {
            MENU_ITEM(function, PSTR(LCD_STR_FOLDER ".."), lcd_sd_updir);
        }
        
        for (uint16_t i = 0; i < fileCnt; i++)
        {
            if (_menuItemNr == _lineNr)
            {
#ifndef SDCARD_RATHERRECENTFIRST
                card.getfilename(i);
#else
                card.getfilename(fileCnt - 1 - i);
#endif
                if (card.filenameIsDir)
                {
                    MENU_ITEM(sddirectory, MSG_CARD_MENU, card.filename, card.longFilename);
                } else {
                    
                    MENU_ITEM(sdfile, MSG_CARD_MENU, card.filename, card.longFilename);
                    
                    
                    
                    
                }
            } else {
                MENU_ITEM_DUMMY();
            }
        }
        
        MENU_ITEM(back, PSTR("- - - - - - - - -"), 0);
    
        
    }*/
    
  if ( ( IS_SD_PRINTING || is_usb_printing || (lcd_commands_type == LCD_COMMAND_V2_CAL)) && (current_position[Z_AXIS] < Z_HEIGHT_HIDE_LIVE_ADJUST_MENU) && !homing_flag && !mesh_bed_leveling_flag)
  {
	MENU_ITEM(submenu, MSG_BABYSTEP_Z, lcd_babystep_z);//8
  }


  if ( moves_planned() || IS_SD_PRINTING || is_usb_printing || (lcd_commands_type == LCD_COMMAND_V2_CAL))
  {
    MENU_ITEM(submenu, MSG_TUNE, lcd_tune_menu);
  } else 
  {
    MENU_ITEM(submenu, MSG_PREHEAT, lcd_preheat_menu);
  }

#ifdef SDSUPPORT
  if (card.cardOK || lcd_commands_type == LCD_COMMAND_V2_CAL)
  {
    if (card.isFileOpen())
    {
		if (mesh_bed_leveling_flag == false && homing_flag == false) {
			if (card.sdprinting)
			{
				MENU_ITEM(function, MSG_PAUSE_PRINT, lcd_sdcard_pause);
			}
			else
			{
				MENU_ITEM(function, MSG_RESUME_PRINT, lcd_sdcard_resume);
			}
			MENU_ITEM(submenu, MSG_STOP_PRINT, lcd_sdcard_stop);
		}
	}
	else if (lcd_commands_type == LCD_COMMAND_V2_CAL && mesh_bed_leveling_flag == false && homing_flag == false) {
		//MENU_ITEM(submenu, MSG_STOP_PRINT, lcd_sdcard_stop);
	}
	else
	{
		if (!is_usb_printing && (lcd_commands_type != LCD_COMMAND_V2_CAL))
		{
			//if (farm_mode) MENU_ITEM(submenu, MSG_FARM_CARD_MENU, lcd_farm_sdcard_menu);
			/*else*/ MENU_ITEM(submenu, MSG_CARD_MENU, lcd_sdcard_menu);
		}
#if SDCARDDETECT < 1
      MENU_ITEM(gcode, MSG_CNG_SDCARD, PSTR("M21"));  // SD-card changed by user
#endif
    }
	
  } else 
  {
    MENU_ITEM(submenu, MSG_NO_CARD, lcd_sdcard_menu);
#if SDCARDDETECT < 1
    MENU_ITEM(gcode, MSG_INIT_SDCARD, PSTR("M21")); // Manually initialize the SD-card via user interface
#endif
  }
#endif


  if (IS_SD_PRINTING || is_usb_printing || (lcd_commands_type == LCD_COMMAND_V2_CAL))
  {
	  if (farm_mode)
	  {
		  MENU_ITEM(submenu, PSTR("Farm number"), lcd_farm_no);
	  }
  } 
  else 
  {
	#ifndef SNMM
#ifdef PAT9125
	if ( ((filament_autoload_enabled == true) && (fsensor_enabled == true)))
        MENU_ITEM(submenu, MSG_AUTOLOAD_FILAMENT, lcd_menu_AutoLoadFilament);
	else
#endif //PAT9125
		MENU_ITEM(function, MSG_LOAD_FILAMENT, lcd_LoadFilament);
	MENU_ITEM(submenu, MSG_UNLOAD_FILAMENT, lcd_unLoadFilament);
	#endif
	#ifdef SNMM
	MENU_ITEM(submenu, MSG_LOAD_FILAMENT, fil_load_menu);
	MENU_ITEM(submenu, MSG_UNLOAD_FILAMENT, fil_unload_menu);
	MENU_ITEM(submenu, MSG_CHANGE_EXTR, change_extr_menu);
	#endif
	MENU_ITEM(submenu, MSG_SETTINGS, lcd_settings_menu);
    if(!isPrintPaused) MENU_ITEM(submenu, MSG_MENU_CALIBRATION, lcd_calibration_menu);

#ifdef EXPERIMENTAL_FEATURES
	MENU_ITEM(submenu, PSTR("Experimantal"), lcd_experimantal_menu);
#endif //EXPERIMENTAL_FEATURES
  }

  if (!is_usb_printing && (lcd_commands_type != LCD_COMMAND_V2_CAL))
  {
	  MENU_ITEM(submenu, MSG_STATISTICS, lcd_menu_statistics);
  }
    
#if defined(TMC2130) || defined(PAT9125)
  MENU_ITEM(submenu, PSTR("Fail stats"), lcd_menu_fails_stats);
#endif

  MENU_ITEM(submenu, MSG_SUPPORT, lcd_support_menu);

  END_MENU();

}

void stack_error() {
	SET_OUTPUT(BEEPER);
	WRITE(BEEPER, HIGH);
	delay(1000);
	WRITE(BEEPER, LOW);
	lcd_display_message_fullscreen_P(MSG_STACK_ERROR);
	//err_triggered = 1;
	 while (1) delay_keep_alive(1000);
}

#ifdef DEBUG_STEPPER_TIMER_MISSED
bool stepper_timer_overflow_state = false;
uint16_t stepper_timer_overflow_max = 0;
uint16_t stepper_timer_overflow_last = 0;
uint16_t stepper_timer_overflow_cnt = 0;
void stepper_timer_overflow() {
  char msg[28];
  sprintf_P(msg, PSTR("#%d %d max %d"), ++ stepper_timer_overflow_cnt, stepper_timer_overflow_last >> 1, stepper_timer_overflow_max >> 1);
  lcd_setstatus(msg);
  stepper_timer_overflow_state = false;
  if (stepper_timer_overflow_last > stepper_timer_overflow_max)
    stepper_timer_overflow_max = stepper_timer_overflow_last;
  SERIAL_ECHOPGM("Stepper timer overflow: ");
  MYSERIAL.print(msg);
  SERIAL_ECHOLNPGM("");

  WRITE(BEEPER, LOW);
}
#endif /* DEBUG_STEPPER_TIMER_MISSED */

#ifdef SDSUPPORT
static void lcd_autostart_sd()
{
  card.lastnr = 0;
  card.setroot();
  card.checkautostart(true);
}
#endif



static void lcd_silent_mode_set_tune() {
  switch (SilentModeMenu) {
#ifdef TMC2130
	case SILENT_MODE_NORMAL: SilentModeMenu = SILENT_MODE_STEALTH; break;
	case SILENT_MODE_STEALTH: SilentModeMenu = SILENT_MODE_NORMAL; break;
	default: SilentModeMenu = SILENT_MODE_NORMAL; break; // (probably) not needed
#else
	case SILENT_MODE_POWER: SilentModeMenu = SILENT_MODE_SILENT; break;
	case SILENT_MODE_SILENT: SilentModeMenu = SILENT_MODE_AUTO; break;
	case SILENT_MODE_AUTO: SilentModeMenu = SILENT_MODE_POWER; break;
	default: SilentModeMenu = SILENT_MODE_POWER; break; // (probably) not needed
#endif //TMC2130
  }
  eeprom_update_byte((unsigned char *)EEPROM_SILENT, SilentModeMenu);
  st_current_init();
  menu_action_back();
}

static void lcd_colorprint_change() {
	
	enquecommand_P(PSTR("M600"));
	
	custom_message = true;
	custom_message_type = 2; //just print status message
	lcd_setstatuspgm(MSG_FINISHING_MOVEMENTS);
	lcd_return_to_status();
	lcdDrawUpdate = 3;
}

static void lcd_tune_menu()
{
  if (menuData.tuneMenu.status == 0) {
    // Menu was entered. Mark the menu as entered and save the current extrudemultiply value.
    menuData.tuneMenu.status = 1;
    menuData.tuneMenu.extrudemultiply = extrudemultiply;
  } else if (menuData.tuneMenu.extrudemultiply != extrudemultiply) {
    // extrudemultiply has been changed from the child menu. Apply the new value.
    menuData.tuneMenu.extrudemultiply = extrudemultiply;
    calculate_extruder_multipliers();
  }

  EEPROM_read(EEPROM_SILENT, (uint8_t*)&SilentModeMenu, sizeof(SilentModeMenu));



	START_MENU();
	MENU_ITEM(back, MSG_MAIN, 0); //1
	MENU_ITEM_EDIT(int3, MSG_SPEED, &feedmultiply, 10, 999);//2

	MENU_ITEM_EDIT(int3, MSG_NOZZLE, &target_temperature[0], 0, HEATER_0_MAXTEMP - 10);//3
	MENU_ITEM_EDIT(int3, MSG_BED, &target_temperature_bed, 0, BED_MAXTEMP - 10);//4

	MENU_ITEM_EDIT(int3, MSG_FAN_SPEED, &fanSpeed, 0, 255);//5
	MENU_ITEM_EDIT(int3, MSG_FLOW, &extrudemultiply, 10, 999);//6
#ifdef FILAMENTCHANGEENABLE
	MENU_ITEM(function, MSG_FILAMENTCHANGE, lcd_colorprint_change);//7
#endif

#ifndef DEBUG_DISABLE_FSENSORCHECK
#ifdef PAT9125
	if (FSensorStateMenu == 0) {
		MENU_ITEM(function, MSG_FSENSOR_OFF, lcd_fsensor_state_set);
	}
	else {
		MENU_ITEM(function, MSG_FSENSOR_ON, lcd_fsensor_state_set);
	}
#endif //PAT9125
#endif //DEBUG_DISABLE_FSENSORCHECK

#ifdef TMC2130
     if(!farm_mode)
     {
//*** MaR::180416_01b
          if (SilentModeMenu == SILENT_MODE_NORMAL) MENU_ITEM(function, MSG_STEALTH_MODE_OFF, lcd_silent_mode_set);
          else MENU_ITEM(function, MSG_STEALTH_MODE_ON, lcd_silent_mode_set);

          if (SilentModeMenu == SILENT_MODE_NORMAL)
          {
               if (CrashDetectMenu == 0) MENU_ITEM(function, MSG_CRASHDETECT_OFF, lcd_crash_mode_set);
               else MENU_ITEM(function, MSG_CRASHDETECT_ON, lcd_crash_mode_set);
          }
          else MENU_ITEM(submenu, MSG_CRASHDETECT_NA, lcd_crash_mode_info);
     }
#else //TMC2130
	if (!farm_mode) { //dont show in menu if we are in farm mode
		switch (SilentModeMenu) {
		case SILENT_MODE_POWER: MENU_ITEM(function, MSG_SILENT_MODE_OFF, lcd_silent_mode_set); break;
		case SILENT_MODE_SILENT: MENU_ITEM(function, MSG_SILENT_MODE_ON, lcd_silent_mode_set); break;
		case SILENT_MODE_AUTO: MENU_ITEM(function, MSG_AUTO_MODE_ON, lcd_silent_mode_set); break;
		default: MENU_ITEM(function, MSG_SILENT_MODE_OFF, lcd_silent_mode_set); break; // (probably) not needed
		}
	}
#endif //TMC2130
	END_MENU();
}

static void lcd_move_menu_01mm()
{
  move_menu_scale = 0.1;
  lcd_move_menu_axis();
}

static void lcd_control_temperature_menu()
{
#ifdef PIDTEMP
  // set up temp variables - undo the default scaling
//  raw_Ki = unscalePID_i(Ki);
//  raw_Kd = unscalePID_d(Kd);
#endif

  START_MENU();
  MENU_ITEM(back, MSG_SETTINGS, 0);
#if TEMP_SENSOR_0 != 0
  MENU_ITEM_EDIT(int3, MSG_NOZZLE, &target_temperature[0], 0, HEATER_0_MAXTEMP - 10);
#endif
#if TEMP_SENSOR_1 != 0
  MENU_ITEM_EDIT(int3, MSG_NOZZLE1, &target_temperature[1], 0, HEATER_1_MAXTEMP - 10);
#endif
#if TEMP_SENSOR_2 != 0
  MENU_ITEM_EDIT(int3, MSG_NOZZLE2, &target_temperature[2], 0, HEATER_2_MAXTEMP - 10);
#endif
#if TEMP_SENSOR_BED != 0
  MENU_ITEM_EDIT(int3, MSG_BED, &target_temperature_bed, 0, BED_MAXTEMP - 3);
#endif
  MENU_ITEM_EDIT(int3, MSG_FAN_SPEED, &fanSpeed, 0, 255);
#if defined AUTOTEMP && (TEMP_SENSOR_0 != 0)
  MENU_ITEM_EDIT(bool, MSG_AUTOTEMP, &autotemp_enabled);
  MENU_ITEM_EDIT(float3, MSG_MIN, &autotemp_min, 0, HEATER_0_MAXTEMP - 10);
  MENU_ITEM_EDIT(float3, MSG_MAX, &autotemp_max, 0, HEATER_0_MAXTEMP - 10);
  MENU_ITEM_EDIT(float32, MSG_FACTOR, &autotemp_factor, 0.0, 1.0);
#endif

  END_MENU();
}


#if SDCARDDETECT == -1
static void lcd_sd_refresh()
{
  card.initsd();
  currentMenuViewOffset = 0;
}
#endif
static void lcd_sd_updir()
{
  SDscrool = 0;
  card.updir();
  currentMenuViewOffset = 0;
}

void lcd_print_stop() {
	cancel_heatup = true;
#ifdef MESH_BED_LEVELING
	mbl.active = false;
#endif
	// Stop the stoppers, update the position from the stoppers.
	if (mesh_bed_leveling_flag == false && homing_flag == false) {
		planner_abort_hard();
		// Because the planner_abort_hard() initialized current_position[Z] from the stepper,
		// Z baystep is no more applied. Reset it.
		babystep_reset();
	}
	// Clean the input command queue.
	cmdqueue_reset();
	lcd_setstatuspgm(MSG_PRINT_ABORTED);
	lcd_update(2);
	card.sdprinting = false;
	card.closefile();

	stoptime = millis();
	unsigned long t = (stoptime - starttime - pause_time) / 1000; //time in s
	pause_time = 0;
	save_statistics(total_filament_used, t);

	lcd_return_to_status();
	lcd_ignore_click(true);
	lcd_commands_step = 0;
	lcd_commands_type = LCD_COMMAND_STOP_PRINT;

	// Turn off the print fan
	SET_OUTPUT(FAN_PIN);
	WRITE(FAN_PIN, 0);
	fanSpeed = 0;
}

void lcd_sdcard_stop()
{
	
	lcd.setCursor(0, 0);
	lcd_printPGM(MSG_STOP_PRINT);
	lcd.setCursor(2, 2);
	lcd_printPGM(MSG_NO);
	lcd.setCursor(2, 3);
	lcd_printPGM(MSG_YES);
	lcd.setCursor(0, 2); lcd.print(" ");
	lcd.setCursor(0, 3); lcd.print(" ");

	if ((int32_t)encoderPosition > 2) { encoderPosition = 2; }
	if ((int32_t)encoderPosition < 1) { encoderPosition = 1; }
	
	lcd.setCursor(0, 1 + encoderPosition);
	lcd.print(">");

	if (lcd_clicked())
	{
		if ((int32_t)encoderPosition == 1)
		{
			lcd_return_to_status();
		}
		if ((int32_t)encoderPosition == 2)
		{
			lcd_print_stop();
		}
	}

}
/*
void getFileDescription(char *name, char *description) {
	// get file description, ie the REAL filenam, ie the second line
	card.openFile(name, true);
	int i = 0;
	// skip the first line (which is the version line)
	while (true) {
		uint16_t readByte = card.get();
		if (readByte == '\n') {
			break;
		}
	}
	// read the second line (which is the description line)
	while (true) {
		uint16_t readByte = card.get();
		if (i == 0) {
			// skip the first '^'
			readByte = card.get();
		}
		description[i] = readByte;
		i++;
		if (readByte == '\n') {
			break;
		}
	}
	card.closefile();
	description[i-1] = 0;
}
*/

void lcd_sdcard_menu()
{
  uint8_t sdSort = eeprom_read_byte((uint8_t*)EEPROM_SD_SORT);
  int tempScrool = 0;
  if (presort_flag == true) {
	  presort_flag = false;
	  card.presort();
  }
  if (lcdDrawUpdate == 0 && LCD_CLICKED == 0)
    //delay(100);
    return; // nothing to do (so don't thrash the SD card)
  uint16_t fileCnt = card.getnrfilenames();

  START_MENU();
  MENU_ITEM(back, MSG_MAIN, 0);
  card.getWorkDirName();
  if (card.filename[0] == '/')
  {
#if SDCARDDETECT == -1
    MENU_ITEM(function, MSG_REFRESH, lcd_sd_refresh);
#endif
  } else {
    MENU_ITEM(function, PSTR(LCD_STR_FOLDER ".."), lcd_sd_updir);
  }

  for (uint16_t i = 0; i < fileCnt; i++)
  {
    if (_menuItemNr == _lineNr)
    {
		const uint16_t nr = ((sdSort == SD_SORT_NONE) || farm_mode || (sdSort == SD_SORT_TIME)) ? (fileCnt - 1 - i) : i;
		/*#ifdef SDCARD_RATHERRECENTFIRST
			#ifndef SDCARD_SORT_ALPHA
				fileCnt - 1 -
			#endif
		#endif
		i;*/
		#ifdef SDCARD_SORT_ALPHA
			if (sdSort == SD_SORT_NONE) card.getfilename(nr);
			else card.getfilename_sorted(nr);
		#else
			 card.getfilename(nr);
		#endif
			
		if (card.filenameIsDir)
			MENU_ITEM(sddirectory, MSG_CARD_MENU, card.filename, card.longFilename);
		else
			MENU_ITEM(sdfile, MSG_CARD_MENU, card.filename, card.longFilename);
    } else {
      MENU_ITEM_DUMMY();
    }
  }
  END_MENU();
}

//char description [10] [31];

/*void get_description() {
	uint16_t fileCnt = card.getnrfilenames();
	for (uint16_t i = 0; i < fileCnt; i++)
	{
		card.getfilename(fileCnt - 1 - i);
		getFileDescription(card.filename, description[i]);
	}
}*/

/*void lcd_farm_sdcard_menu() 
{
	static int i = 0;
	if (i == 0) {
		get_description();
		i++;
	}
		//int j;
		//char description[31];
		int tempScrool = 0;
		if (lcdDrawUpdate == 0 && LCD_CLICKED == 0)
			//delay(100);
			return; // nothing to do (so don't thrash the SD card)
		uint16_t fileCnt = card.getnrfilenames();

		START_MENU();
		MENU_ITEM(back, MSG_MAIN, 0);
		card.getWorkDirName();
		if (card.filename[0] == '/')
		{
#if SDCARDDETECT == -1
			MENU_ITEM(function, MSG_REFRESH, lcd_sd_refresh);
#endif
		}
		else {
			MENU_ITEM(function, PSTR(LCD_STR_FOLDER ".."), lcd_sd_updir);
		}



		for (uint16_t i = 0; i < fileCnt; i++)
		{
			if (_menuItemNr == _lineNr)
			{
#ifndef SDCARD_RATHERRECENTFIRST
				card.getfilename(i);
#else
				card.getfilename(fileCnt - 1 - i);
#endif
				if (card.filenameIsDir)
				{
					MENU_ITEM(sddirectory, MSG_CARD_MENU, card.filename, card.longFilename);
				}
				else {
					
					MENU_ITEM(sdfile, MSG_CARD_MENU, card.filename, description[i]);
				}
			}
			else {
				MENU_ITEM_DUMMY();
			}
		}
		END_MENU();

}*/

#define menu_edit_type(_type, _name, _strFunc, scale) \
  void menu_edit_ ## _name () \
  { \
    if ((int32_t)encoderPosition < 0) encoderPosition = 0; \
    if ((int32_t)encoderPosition > menuData.editMenuParentState.maxEditValue) encoderPosition = menuData.editMenuParentState.maxEditValue; \
    if (lcdDrawUpdate) \
      lcd_implementation_drawedit(menuData.editMenuParentState.editLabel, _strFunc(((_type)((int32_t)encoderPosition + menuData.editMenuParentState.minEditValue)) / scale)); \
    if (LCD_CLICKED) \
    { \
      *((_type*)menuData.editMenuParentState.editValue) = ((_type)((int32_t)encoderPosition + menuData.editMenuParentState.minEditValue)) / scale; \
      lcd_goto_menu(menuData.editMenuParentState.prevMenu, menuData.editMenuParentState.prevEncoderPosition, true, false); \
    } \
  } \
  static void menu_action_setting_edit_ ## _name (const char* pstr, _type* ptr, _type minValue, _type maxValue) \
  { \
    asm("cli"); \
	menuData.editMenuParentState.prevMenu = currentMenu; \
    menuData.editMenuParentState.prevEncoderPosition = encoderPosition; \
    asm("sei"); \
    \
    lcdDrawUpdate = 2; \
    menuData.editMenuParentState.editLabel = pstr; \
    menuData.editMenuParentState.editValue = ptr; \
    menuData.editMenuParentState.minEditValue = minValue * scale; \
    menuData.editMenuParentState.maxEditValue = maxValue * scale - menuData.editMenuParentState.minEditValue; \
    lcd_goto_menu(menu_edit_ ## _name, (*ptr) * scale - menuData.editMenuParentState.minEditValue, true, false); \
    \
  }\
  /*
  void menu_edit_callback_ ## _name () { \
    menu_edit_ ## _name (); \
    if (LCD_CLICKED) (*callbackFunc)(); \
  } \
  static void menu_action_setting_edit_callback_ ## _name (const char* pstr, _type* ptr, _type minValue, _type maxValue, menuFunc_t callback) \
  { \
    menuData.editMenuParentState.prevMenu = currentMenu; \
    menuData.editMenuParentState.prevEncoderPosition = encoderPosition; \
    \
    lcdDrawUpdate = 2; \
    lcd_goto_menu(menu_edit_callback_ ## _name, (*ptr) * scale - menuData.editMenuParentState.minEditValue, true, false); \
    \
    menuData.editMenuParentState.editLabel = pstr; \
    menuData.editMenuParentState.editValue = ptr; \
    menuData.editMenuParentState.minEditValue = minValue * scale; \
    menuData.editMenuParentState.maxEditValue = maxValue * scale - menuData.editMenuParentState.minEditValue; \
    callbackFunc = callback;\
  }
  */


#ifdef TMC2130
extern char conv[8];
// Convert tmc2130 mres to string 
char *mres_to_str3(const uint8_t &x)
{
	return itostr3(256 >> x);
}
menu_edit_type(uint8_t, mres, mres_to_str3, 1)
// Convert tmc2130 wfac to string 
char *wfac_to_str5(const uint8_t &x)
{
	if (x >= TMC2130_WAVE_FAC1000_MIN)
	    {
	    conv[0] = '[';
	    ftostr43(((float)((uint16_t)x + 1000) / 1000), 1);
	    }
	else strcpy_P(conv, MSG_EXTRUDER_CORRECTION_OFF);
	conv[6] = ']';
	conv[7] = ' ';
	conv[8] = 0;
	return conv;
}
menu_edit_type(uint8_t, wfac, wfac_to_str5, 1)
#endif //TMC2130

menu_edit_type(uint8_t, byte3, itostr3, 1)
menu_edit_type(int, int3, itostr3, 1)
menu_edit_type(float, float3, ftostr3, 1)
menu_edit_type(float, float32, ftostr32, 100)
menu_edit_type(float, float43, ftostr43, 1000)
menu_edit_type(float, float5, ftostr5, 0.01)
menu_edit_type(float, float51, ftostr51, 10)
menu_edit_type(float, float52, ftostr52, 100)
menu_edit_type(unsigned long, long5, ftostr5, 0.01)

static void lcd_selftest_v()
{
	(void)lcd_selftest();
}

bool lcd_selftest()
{
	int _progress = 0;
	bool _result = true;
	lcd_wait_for_cool_down();
	lcd_implementation_clear();
	lcd.setCursor(0, 0); lcd_printPGM(MSG_SELFTEST_START);
	#ifdef TMC2130
	  FORCE_HIGH_POWER_START;
	#endif // TMC2130
	delay(2000);
	KEEPALIVE_STATE(IN_HANDLER);

	_progress = lcd_selftest_screen(-1, _progress, 3, true, 2000);
#if (defined(FANCHECK) && defined(TACH_0)) 		
	_result = lcd_selftest_fan_dialog(0);
#else //defined(TACH_0)
	_result = lcd_selftest_manual_fan_check(0, false);
	if (!_result)
	{
		const char *_err;
		lcd_selftest_error(7, _err, _err); //extruder fan not spinning
	}
#endif //defined(TACH_0)
	

	if (_result)
	{
		_progress = lcd_selftest_screen(0, _progress, 3, true, 2000);
#if (defined(FANCHECK) && defined(TACH_1)) 		
		_result = lcd_selftest_fan_dialog(1);
#else //defined(TACH_1)
		_result = lcd_selftest_manual_fan_check(1, false);
		if (!_result)
		{			
			const char *_err;
			lcd_selftest_error(6, _err, _err); //print fan not spinning
		}

#endif //defined(TACH_1)
	}

	if (_result)
	{
		_progress = lcd_selftest_screen(1, _progress, 3, true, 2000);
#ifndef TMC2130
		_result = lcd_selfcheck_endstops();
#else
		_result = true;
#endif
	}
	
	if (_result)
	{
		_progress = lcd_selftest_screen(3, _progress, 3, true, 1000);
		_result = lcd_selfcheck_check_heater(false);
	}


	if (_result)
	{
		//current_position[Z_AXIS] += 15;									//move Z axis higher to avoid false triggering of Z end stop in case that we are very low - just above heatbed
		_progress = lcd_selftest_screen(4, _progress, 3, true, 2000);
#ifdef TMC2130
		_result = lcd_selfcheck_axis_sg(X_AXIS);
#else
		_result = lcd_selfcheck_axis(X_AXIS, X_MAX_POS);
#endif //TMC2130
	}

	if (_result)
	{
		_progress = lcd_selftest_screen(4, _progress, 3, true, 0);

#ifndef TMC2130
		_result = lcd_selfcheck_pulleys(X_AXIS);
#endif
	}


	if (_result)
	{
		_progress = lcd_selftest_screen(5, _progress, 3, true, 1500);
#ifdef TMC2130
		_result = lcd_selfcheck_axis_sg(Y_AXIS);
#else
		_result = lcd_selfcheck_axis(Y_AXIS, Y_MAX_POS);
#endif // TMC2130
	}

	if (_result)
	{
		_progress = lcd_selftest_screen(5, _progress, 3, true, 0);
#ifndef TMC2130
		_result = lcd_selfcheck_pulleys(Y_AXIS);
#endif // TMC2130
	}


	if (_result)
	{
#ifdef TMC2130
		tmc2130_home_exit();
		enable_endstops(false);
		current_position[X_AXIS] = current_position[X_AXIS] + 14;
		current_position[Y_AXIS] = current_position[Y_AXIS] + 12;
#endif

		//homeaxis(X_AXIS);
		//homeaxis(Y_AXIS);
		current_position[Z_AXIS] = current_position[Z_AXIS] + 10;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);
		st_synchronize();
		_progress = lcd_selftest_screen(6, _progress, 3, true, 1500);
		_result = lcd_selfcheck_axis(2, Z_MAX_POS);
		if (eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE) != 1) {
			enquecommand_P(PSTR("G28 W"));
			enquecommand_P(PSTR("G1 Z15 F1000"));
		}
	}

#ifdef TMC2130
	if (_result)
	{
		current_position[Z_AXIS] = current_position[Z_AXIS] + 10;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);
		st_synchronize();
		_progress = lcd_selftest_screen(13, 0, 2, true, 0);
		bool bres = tmc2130_home_calibrate(X_AXIS);
		_progress = lcd_selftest_screen(13, 1, 2, true, 0);
		bres &= tmc2130_home_calibrate(Y_AXIS);
		_progress = lcd_selftest_screen(13, 2, 2, true, 0);
		if (bres)
			eeprom_update_byte((uint8_t*)EEPROM_TMC2130_HOME_ENABLED, 1);
		_result = bres;
	}
#endif //TMC2130

	if (_result)
	{
		_progress = lcd_selftest_screen(7, _progress, 3, true, 2000); //check bed
		_result = lcd_selfcheck_check_heater(true);
	}
	if (_result)
	{
		_progress = lcd_selftest_screen(8, _progress, 3, true, 2000); //bed ok
#ifdef PAT9125
		_progress = lcd_selftest_screen(9, _progress, 3, true, 2000); //check filaments sensor
		_result = lcd_selftest_fsensor();
#endif // PAT9125
	}
	if (_result)
	{
#ifdef PAT9125
		_progress = lcd_selftest_screen(10, _progress, 3, true, 2000); //fil sensor OK
#endif // PAT9125
		_progress = lcd_selftest_screen(11, _progress, 3, true, 5000); //all correct
	}
	else
	{
		_progress = lcd_selftest_screen(12, _progress, 3, true, 5000);
	}
	lcd_reset_alert_level();
	enquecommand_P(PSTR("M84"));
	lcd_implementation_clear();
	lcd_next_update_millis = millis() + LCD_UPDATE_INTERVAL;
	
	if (_result)
	{
		LCD_ALERTMESSAGERPGM(MSG_SELFTEST_OK);
	}
	else
	{
		LCD_ALERTMESSAGERPGM(MSG_SELFTEST_FAILED);
	}
	#ifdef TMC2130
	  FORCE_HIGH_POWER_END;
	#endif // TMC2130
	KEEPALIVE_STATE(NOT_BUSY);
	return(_result);
}

#ifdef TMC2130

static void reset_crash_det(char axis) {
	current_position[axis] += 10;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);
	st_synchronize();
	if (eeprom_read_byte((uint8_t*)EEPROM_CRASH_DET)) tmc2130_sg_stop_on_crash = true;
}

static bool lcd_selfcheck_axis_sg(char axis) {
// each axis length is measured twice	
	float axis_length, current_position_init, current_position_final;
	float measured_axis_length[2];
	float margin = 60;
	float max_error_mm = 5;
	switch (axis) {
	case 0: axis_length = X_MAX_POS; break;
	case 1: axis_length = Y_MAX_POS + 8; break;
	default: axis_length = 210; break;
	}

	tmc2130_sg_stop_on_crash = false;
	tmc2130_home_exit();
	enable_endstops(true);

	if (axis == X_AXIS) { //there is collision between cables and PSU cover in X axis if Z coordinate is too low
		
		current_position[Z_AXIS] += 17;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);
		tmc2130_home_enter(Z_AXIS_MASK);
		st_synchronize();
		tmc2130_home_exit();
	}

// first axis length measurement begin	
	
	current_position[axis] -= (axis_length + margin);
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);

	
	st_synchronize();

	tmc2130_sg_meassure_start(axis);

	current_position_init = st_get_position_mm(axis);

	current_position[axis] += 2 * margin;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);
	st_synchronize();

	current_position[axis] += axis_length;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);

	st_synchronize();

	uint16_t sg1 = tmc2130_sg_meassure_stop();
	printf_P(PSTR("%c AXIS SG1=%d\n"), 'X'+axis, sg1);
	eeprom_write_word(((uint16_t*)((axis == X_AXIS)?EEPROM_BELTSTATUS_X:EEPROM_BELTSTATUS_Y)), sg1);

	current_position_final = st_get_position_mm(axis);
	measured_axis_length[0] = abs(current_position_final - current_position_init);


// first measurement end and second measurement begin	


	current_position[axis] -= margin;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);
	st_synchronize();	

	current_position[axis] -= (axis_length + margin);
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);
		
	st_synchronize();

	current_position_init = st_get_position_mm(axis);

	measured_axis_length[1] = abs(current_position_final - current_position_init);


//end of second measurement, now check for possible errors:

	for(int i = 0; i < 2; i++){ //check if measured axis length corresponds to expected length
		SERIAL_ECHOPGM("Measured axis length:");
		MYSERIAL.println(measured_axis_length[i]);
		if (abs(measured_axis_length[i] - axis_length) > max_error_mm) {
			enable_endstops(false);

			const char *_error_1;
			const char *_error_2;

			if (axis == X_AXIS) _error_1 = "X";
			if (axis == Y_AXIS) _error_1 = "Y";
			if (axis == Z_AXIS) _error_1 = "Z";

			lcd_selftest_error(9, _error_1, _error_2);
			current_position[axis] = 0;
			plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
			reset_crash_det(axis);
			return false;
		}
	}

	SERIAL_ECHOPGM("Axis length difference:");
	MYSERIAL.println(abs(measured_axis_length[0] - measured_axis_length[1]));
	
		if (abs(measured_axis_length[0] - measured_axis_length[1]) > 1) { //check if difference between first and second measurement is low
			//loose pulleys
			const char *_error_1;
			const char *_error_2;

			if (axis == X_AXIS) _error_1 = "X";
			if (axis == Y_AXIS) _error_1 = "Y";
			if (axis == Z_AXIS) _error_1 = "Z";

			lcd_selftest_error(8, _error_1, _error_2);
			current_position[axis] = 0;
			plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
			reset_crash_det(axis);

			return false;
		}
		current_position[axis] = 0;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		reset_crash_det(axis);
		return true;
}
#endif //TMC2130

//#ifndef TMC2130

static bool lcd_selfcheck_axis(int _axis, int _travel)
{
//	printf_P(PSTR("lcd_selfcheck_axis %d, %d\n"), _axis, _travel);
	bool _stepdone = false;
	bool _stepresult = false;
	int _progress = 0;
	int _travel_done = 0;
	int _err_endstop = 0;
	int _lcd_refresh = 0;
	_travel = _travel + (_travel / 10);

	if (_axis == X_AXIS) {
		current_position[Z_AXIS] += 17;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);
	}

	do {
		current_position[_axis] = current_position[_axis] - 1;

		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);
		st_synchronize();
#ifdef TMC2130
		if ((READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING == 1))
#else //TMC2130
		if (((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) ||
			((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1) ||
			((READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING) == 1))
#endif //TMC2130
		{
			if (_axis == 0)
			{
				_stepresult = ((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) ? true : false;
				_err_endstop = ((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1) ? 1 : 2;

			}
			if (_axis == 1)
			{
				_stepresult = ((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1) ? true : false;
				_err_endstop = ((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) ? 0 : 2;

			}
			if (_axis == 2)
			{
				_stepresult = ((READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING) == 1) ? true : false;
				_err_endstop = ((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) ? 0 : 1;
	printf_P(PSTR("lcd_selfcheck_axis %d, %d\n"), _stepresult, _err_endstop);
				/*disable_x();
				disable_y();
				disable_z();*/
			}
			_stepdone = true;
		}

		if (_lcd_refresh < 6)
		{
			_lcd_refresh++;
		}
		else
		{
			_progress = lcd_selftest_screen(4 + _axis, _progress, 3, false, 0);
			_lcd_refresh = 0;
		}

		manage_heater();
		manage_inactivity(true);

		//delay(100);
		(_travel_done <= _travel) ? _travel_done++ : _stepdone = true;

	} while (!_stepdone);


	//current_position[_axis] = current_position[_axis] + 15;
	//plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);

	if (!_stepresult)
	{
		const char *_error_1;
		const char *_error_2;

		if (_axis == X_AXIS) _error_1 = "X";
		if (_axis == Y_AXIS) _error_1 = "Y";
		if (_axis == Z_AXIS) _error_1 = "Z";

		if (_err_endstop == 0) _error_2 = "X";
		if (_err_endstop == 1) _error_2 = "Y";
		if (_err_endstop == 2) _error_2 = "Z";


		if (_travel_done >= _travel)
		{
			lcd_selftest_error(5, _error_1, _error_2);
		}
		else
		{
			lcd_selftest_error(4, _error_1, _error_2);
		}
	}

	return _stepresult;
}

#ifndef TMC2130
static bool lcd_selfcheck_pulleys(int axis)
{
	float tmp_motor_loud[3] = DEFAULT_PWM_MOTOR_CURRENT_LOUD;
	float tmp_motor[3] = DEFAULT_PWM_MOTOR_CURRENT;
	float current_position_init;
	float move;
	bool endstop_triggered = false;
	int i;
	unsigned long timeout_counter;
	refresh_cmd_timeout();
	manage_inactivity(true);

	if (axis == 0) move = 50; //X_AXIS 
	else move = 50; //Y_AXIS

	current_position_init = current_position[axis];

	current_position[axis] += 2;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);
	for (i = 0; i < 5; i++) {
		refresh_cmd_timeout();
		current_position[axis] = current_position[axis] + move;
		st_current_set(0, 850); //set motor current higher
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], 200, active_extruder);
		st_synchronize();
//*** MaR::180416_02
          if (SilentModeMenu != SILENT_MODE_OFF) st_current_set(0, tmp_motor[0]); //set back to normal operation currents
		else st_current_set(0, tmp_motor_loud[0]); //set motor current back			
		current_position[axis] = current_position[axis] - move;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], 50, active_extruder);
		st_synchronize();
		if (((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) ||
			((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1)) {
			lcd_selftest_error(8, (axis == 0) ? "X" : "Y", "");
			return(false);
		}
	}
	timeout_counter = millis() + 2500;
	endstop_triggered = false;
	manage_inactivity(true);
	while (!endstop_triggered) {
		if (((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) ||
			((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1)) {
			endstop_triggered = true;
			if (current_position_init - 1 <= current_position[axis] && current_position_init + 1 >= current_position[axis]) {
				current_position[axis] += (axis == X_AXIS) ? 13 : 9;
				plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);
				st_synchronize();
				return(true);
			}
			else {
				lcd_selftest_error(8, (axis == 0) ? "X" : "Y", "");
				return(false);
			}
		}
		else {
			current_position[axis] -= 1;
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);
			st_synchronize();
			if (millis() > timeout_counter) {
				lcd_selftest_error(8, (axis == 0) ? "X" : "Y", "");
				return(false);
			}
		}
	}
	return(true);
}
#endif TMC2130


static bool lcd_selfcheck_endstops()
{
	bool _result = true;

	if (((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) ||
		((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1) ||
		((READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING) == 1))
	{
		if ((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) current_position[0] += 10;
		if ((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1) current_position[1] += 10;
		if ((READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING) == 1) current_position[2] += 10;
	}
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], manual_feedrate[0] / 60, active_extruder);
	delay(500);

	if (((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) ||
		((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1) ||
		((READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING) == 1))
	{
		_result = false;
		char _error[4] = "";
		if ((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) strcat(_error, "X");
		if ((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1) strcat(_error, "Y");
		if ((READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING) == 1) strcat(_error, "Z");
		lcd_selftest_error(3, _error, "");
	}
	manage_heater();
	manage_inactivity(true);
	return _result;
}
//#endif //not defined TMC2130

static bool lcd_selfcheck_check_heater(bool _isbed)
{
	int _counter = 0;
	int _progress = 0;
	bool _stepresult = false;
	bool _docycle = true;

	int _checked_snapshot = (_isbed) ? degBed() : degHotend(0);
	int _opposite_snapshot = (_isbed) ? degHotend(0) : degBed();
	int _cycles = (_isbed) ? 180 : 60; //~ 90s / 30s

	target_temperature[0] = (_isbed) ? 0 : 200;
	target_temperature_bed = (_isbed) ? 100 : 0;
	manage_heater();
	manage_inactivity(true);
	KEEPALIVE_STATE(NOT_BUSY); //we are sending temperatures on serial line, so no need to send host keepalive messages

	do {
		_counter++;
		_docycle = (_counter < _cycles) ? true : false;

		manage_heater();
		manage_inactivity(true);
		_progress = (_isbed) ? lcd_selftest_screen(7, _progress, 2, false, 400) : lcd_selftest_screen(3, _progress, 2, false, 400);
		/*if (_isbed) {
			MYSERIAL.print("Bed temp:");
			MYSERIAL.println(degBed());
		}
		else {
			MYSERIAL.print("Hotend temp:");
			MYSERIAL.println(degHotend(0));
		}*/
		if(_counter%5 == 0) serialecho_temperatures(); //show temperatures once in two seconds

	} while (_docycle); 

	target_temperature[0] = 0;
	target_temperature_bed = 0;
	manage_heater();

	int _checked_result = (_isbed) ? degBed() - _checked_snapshot : degHotend(0) - _checked_snapshot;
	int _opposite_result = (_isbed) ? degHotend(0) - _opposite_snapshot : degBed() - _opposite_snapshot;
	/*
	MYSERIAL.println("");
	MYSERIAL.print("Checked result:");
	MYSERIAL.println(_checked_result);
	MYSERIAL.print("Opposite result:");
	MYSERIAL.println(_opposite_result);
	*/
	if (_opposite_result < ((_isbed) ? 10 : 3))
	{
		if (_checked_result >= ((_isbed) ? 3 : 10))
		{
			_stepresult = true;
		}
		else
		{
			lcd_selftest_error(1, "", "");
		}
	}
	else
	{
		lcd_selftest_error(2, "", "");
	}

	manage_heater();
	manage_inactivity(true);
	KEEPALIVE_STATE(IN_HANDLER);
	return _stepresult;

}
static void lcd_selftest_error(int _error_no, const char *_error_1, const char *_error_2)
{
	lcd_implementation_quick_feedback();

	target_temperature[0] = 0;
	target_temperature_bed = 0;
	manage_heater();
	manage_inactivity();

	lcd_implementation_clear();

	lcd.setCursor(0, 0);
	lcd_printPGM(MSG_SELFTEST_ERROR);
	lcd.setCursor(0, 1);
	lcd_printPGM(MSG_SELFTEST_PLEASECHECK);

	switch (_error_no)
	{
	case 1:
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_SELFTEST_HEATERTHERMISTOR);
		lcd.setCursor(0, 3);
		lcd_printPGM(MSG_SELFTEST_NOTCONNECTED);
		break;
	case 2:
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_SELFTEST_BEDHEATER);
		lcd.setCursor(0, 3);
		lcd_printPGM(MSG_SELFTEST_WIRINGERROR);
		break;
	case 3:
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_SELFTEST_ENDSTOPS);
		lcd.setCursor(0, 3);
		lcd_printPGM(MSG_SELFTEST_WIRINGERROR);
		lcd.setCursor(17, 3);
		lcd.print(_error_1);
		break;
	case 4:
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_SELFTEST_MOTOR);
		lcd.setCursor(18, 2);
		lcd.print(_error_1);
		lcd.setCursor(0, 3);
		lcd_printPGM(MSG_SELFTEST_ENDSTOP);
		lcd.setCursor(18, 3);
		lcd.print(_error_2);
		break;
	case 5:
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_SELFTEST_ENDSTOP_NOTHIT);
		lcd.setCursor(0, 3);
		lcd_printPGM(MSG_SELFTEST_MOTOR);
		lcd.setCursor(18, 3);
		lcd.print(_error_1);
		break;
	case 6:
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_SELFTEST_COOLING_FAN);
		lcd.setCursor(0, 3);
		lcd_printPGM(MSG_SELFTEST_WIRINGERROR);
		lcd.setCursor(18, 3);
		lcd.print(_error_1);
		break;
	case 7:
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_SELFTEST_EXTRUDER_FAN);
		lcd.setCursor(0, 3);
		lcd_printPGM(MSG_SELFTEST_WIRINGERROR);
		lcd.setCursor(18, 3);
		lcd.print(_error_1);
		break;
	case 8:
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_LOOSE_PULLEY);
		lcd.setCursor(0, 3);
		lcd_printPGM(MSG_SELFTEST_MOTOR);
		lcd.setCursor(18, 3);
		lcd.print(_error_1);
		break;
	case 9:
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_SELFTEST_AXIS_LENGTH);
		lcd.setCursor(0, 3);
		lcd_printPGM(MSG_SELFTEST_AXIS);
		lcd.setCursor(18, 3);
		lcd.print(_error_1);
		break;
	case 10:
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_SELFTEST_FANS);
		lcd.setCursor(0, 3);
		lcd_printPGM(MSG_SELFTEST_SWAPPED);
		lcd.setCursor(18, 3);
		lcd.print(_error_1);
		break;
	case 11: 
		lcd.setCursor(0, 2);
		lcd_printPGM(MSG_FILAMENT_SENSOR);
		lcd.setCursor(0, 3);
		lcd_printPGM(MSG_SELFTEST_WIRINGERROR);
		break;
	}

	delay(1000);
	lcd_implementation_quick_feedback();

	do {
		delay(100);
		manage_heater();
		manage_inactivity();
	} while (!lcd_clicked());

	LCD_ALERTMESSAGERPGM(MSG_SELFTEST_FAILED);
	lcd_return_to_status();

}

#ifdef PAT9125
static bool lcd_selftest_fsensor() {
	fsensor_init();
	if (fsensor_not_responding)
	{
		const char *_err;
		lcd_selftest_error(11, _err, _err);
	}
	return(!fsensor_not_responding);
}
#endif //PAT9125

static bool lcd_selftest_manual_fan_check(int _fan, bool check_opposite)
{

	bool _result = check_opposite;
	lcd_implementation_clear();

	lcd.setCursor(0, 0); lcd_printPGM(MSG_SELFTEST_FAN);
	
	switch (_fan)
	{
	case 0:
		// extruder cooling fan
		lcd.setCursor(0, 1); 
		if(check_opposite == true) lcd_printPGM(MSG_SELFTEST_COOLING_FAN); 
		else lcd_printPGM(MSG_SELFTEST_EXTRUDER_FAN);
		SET_OUTPUT(EXTRUDER_0_AUTO_FAN_PIN);
		WRITE(EXTRUDER_0_AUTO_FAN_PIN, 1);
		break;
	case 1:
		// object cooling fan
		lcd.setCursor(0, 1);
		if (check_opposite == true) lcd_printPGM(MSG_SELFTEST_EXTRUDER_FAN);
		else lcd_printPGM(MSG_SELFTEST_COOLING_FAN);
		SET_OUTPUT(FAN_PIN);
		analogWrite(FAN_PIN, 255);
		break;
	}
	delay(500);

	lcd.setCursor(1, 2); lcd_printPGM(MSG_SELFTEST_FAN_YES);
	lcd.setCursor(0, 3); lcd.print(">");
	lcd.setCursor(1, 3); lcd_printPGM(MSG_SELFTEST_FAN_NO);

	int8_t enc_dif = 0;
	KEEPALIVE_STATE(PAUSED_FOR_USER);

	button_pressed = false; 
	do
	{
		switch (_fan)
		{
		case 0:
			// extruder cooling fan
			SET_OUTPUT(EXTRUDER_0_AUTO_FAN_PIN);
			WRITE(EXTRUDER_0_AUTO_FAN_PIN, 1);
			break;
		case 1:
			// object cooling fan
			SET_OUTPUT(FAN_PIN);
			analogWrite(FAN_PIN, 255);
			break;
		}

		if (abs((enc_dif - encoderDiff)) > 2) {
			if (enc_dif > encoderDiff) {
				_result = !check_opposite;
				lcd.setCursor(0, 2); lcd.print(">");
				lcd.setCursor(1, 2); lcd_printPGM(MSG_SELFTEST_FAN_YES);
				lcd.setCursor(0, 3); lcd.print(" ");
				lcd.setCursor(1, 3); lcd_printPGM(MSG_SELFTEST_FAN_NO);
			}

			if (enc_dif < encoderDiff) {
				_result = check_opposite;
				lcd.setCursor(0, 2); lcd.print(" ");
				lcd.setCursor(1, 2); lcd_printPGM(MSG_SELFTEST_FAN_YES);
				lcd.setCursor(0, 3); lcd.print(">");
				lcd.setCursor(1, 3); lcd_printPGM(MSG_SELFTEST_FAN_NO);
			}
			enc_dif = 0;
			encoderDiff = 0;
		}


		manage_heater();
		delay(100);

	} while (!lcd_clicked());
	KEEPALIVE_STATE(IN_HANDLER);
	SET_OUTPUT(EXTRUDER_0_AUTO_FAN_PIN);
	WRITE(EXTRUDER_0_AUTO_FAN_PIN, 0);
	SET_OUTPUT(FAN_PIN);
	analogWrite(FAN_PIN, 0);

	fanSpeed = 0;
	manage_heater();

	return _result;

}


static bool lcd_selftest_fan_dialog(int _fan)
{
	bool _result = true;
	int _errno = 7;

	switch (_fan) {
	case 0:
		fanSpeed = 0;
		manage_heater();			//turn off fan
		setExtruderAutoFanState(EXTRUDER_0_AUTO_FAN_PIN, 1); //extruder fan
		delay(2000);				//delay_keep_alive would turn off extruder fan, because temerature is too low
		manage_heater();			//count average fan speed from 2s delay and turn off fans
		if (!fan_speed[0]) _result = false;
		//SERIAL_ECHOPGM("Extruder fan speed: ");
		//MYSERIAL.println(fan_speed[0]);
		//SERIAL_ECHOPGM("Print fan speed: ");
		//MYSERIAL.print(fan_speed[1]);
		break;

	case 1:
		//will it work with Thotend > 50 C ?
		fanSpeed = 150;				//print fan
		for (uint8_t i = 0; i < 5; i++) {
			delay_keep_alive(1000);
			lcd.setCursor(18, 3);
			lcd.print("-");
			delay_keep_alive(1000);
			lcd.setCursor(18, 3);
			lcd.print("|");
		}
		fanSpeed = 0;
		manage_heater();			//turn off fan
		manage_inactivity(true);	//to turn off print fan
		if (!fan_speed[1]) {
			_result = false; _errno = 6; //print fan not spinning
		}
		else if (fan_speed[1] < 34) { //fan is spinning, but measured RPM are too low for print fan, it must be left extruder fan
			//check fans manually

			_result = lcd_selftest_manual_fan_check(1, true); //turn on print fan and check that left extruder fan is not spinning
			if (_result) {
				_result = lcd_selftest_manual_fan_check(1, false); //print fan is stil turned on; check that it is spinning
				if (!_result) _errno = 6; //print fan not spinning
			}
			else {
				_errno = 10; //swapped fans
			}
		}

		//SERIAL_ECHOPGM("Extruder fan speed: ");
		//MYSERIAL.println(fan_speed[0]);
		//SERIAL_ECHOPGM("Print fan speed: ");
		//MYSERIAL.println(fan_speed[1]);
		break;
	}
	if (!_result)
	{
		const char *_err;
		lcd_selftest_error(_errno, _err, _err);
	}
	return _result;
}

static int lcd_selftest_screen(int _step, int _progress, int _progress_scale, bool _clear, int _delay)
{

	lcd_next_update_millis = millis() + (LCD_UPDATE_INTERVAL * 10000);

	int _step_block = 0;
	const char *_indicator = (_progress > _progress_scale) ? "-" : "|";

	if (_clear) lcd_implementation_clear();


	lcd.setCursor(0, 0);

	if (_step == -1) lcd_printPGM(MSG_SELFTEST_FAN);
	if (_step == 0) lcd_printPGM(MSG_SELFTEST_FAN);
	if (_step == 1) lcd_printPGM(MSG_SELFTEST_FAN);
	if (_step == 2) lcd_printPGM(MSG_SELFTEST_CHECK_ENDSTOPS);
	if (_step == 3) lcd_printPGM(MSG_SELFTEST_CHECK_HOTEND);
	if (_step == 4) lcd_printPGM(MSG_SELFTEST_CHECK_X);
	if (_step == 5) lcd_printPGM(MSG_SELFTEST_CHECK_Y);
	if (_step == 6) lcd_printPGM(MSG_SELFTEST_CHECK_Z);
	if (_step == 7) lcd_printPGM(MSG_SELFTEST_CHECK_BED);
	if (_step == 8) lcd_printPGM(MSG_SELFTEST_CHECK_BED);
	if (_step == 9) lcd_printPGM(MSG_SELFTEST_CHECK_FSENSOR);
	if (_step == 10) lcd_printPGM(MSG_SELFTEST_CHECK_FSENSOR);
	if (_step == 11) lcd_printPGM(MSG_SELFTEST_CHECK_ALLCORRECT);
	if (_step == 12) lcd_printPGM(MSG_SELFTEST_FAILED);
	if (_step == 13) lcd_printPGM(PSTR("Calibrating home"));

	lcd.setCursor(0, 1);
	lcd.print("--------------------");
	if ((_step >= -1) && (_step <= 1))
	{
		//SERIAL_ECHOLNPGM("Fan test");
		lcd_print_at_PGM(0, 2, MSG_SELFTEST_EXTRUDER_FAN_SPEED);
		lcd.setCursor(18, 2);
		(_step < 0) ? lcd.print(_indicator) : lcd.print("OK");
		lcd_print_at_PGM(0, 3, MSG_SELFTEST_PRINT_FAN_SPEED);
		lcd.setCursor(18, 3);
		(_step < 1) ? lcd.print(_indicator) : lcd.print("OK");
	}
	else if (_step >= 9 && _step <= 10)
	{
		lcd_print_at_PGM(0, 2, MSG_SELFTEST_FILAMENT_SENSOR);
		lcd.setCursor(18, 2);
		(_step == 9) ? lcd.print(_indicator) : lcd.print("OK");
	}
	else if (_step < 9)
	{
		//SERIAL_ECHOLNPGM("Other tests");
		_step_block = 3;
		lcd_selftest_screen_step(3, 9, ((_step == _step_block) ? 1 : (_step < _step_block) ? 0 : 2), "Hotend", _indicator);

		_step_block = 4;
		lcd_selftest_screen_step(2, 2, ((_step == _step_block) ? 1 : (_step < _step_block) ? 0 : 2), "X", _indicator);

		_step_block = 5;
		lcd_selftest_screen_step(2, 8, ((_step == _step_block) ? 1 : (_step < _step_block) ? 0 : 2), "Y", _indicator);

		_step_block = 6;
		lcd_selftest_screen_step(2, 14, ((_step == _step_block) ? 1 : (_step < _step_block) ? 0 : 2), "Z", _indicator);

		_step_block = 7;
		lcd_selftest_screen_step(3, 0, ((_step == _step_block) ? 1 : (_step < _step_block) ? 0 : 2), "Bed", _indicator);
	}

	if (_delay > 0) delay_keep_alive(_delay);
	_progress++;

	return (_progress > _progress_scale * 2) ? 0 : _progress;
}

static void lcd_selftest_screen_step(int _row, int _col, int _state, const char *_name, const char *_indicator)
{
	lcd.setCursor(_col, _row);

	switch (_state)
	{
	case 1:
		lcd.print(_name);
		lcd.setCursor(_col + strlen(_name), _row);
		lcd.print(":");
		lcd.setCursor(_col + strlen(_name) + 1, _row);
		lcd.print(_indicator);
		break;
	case 2:
		lcd.print(_name);
		lcd.setCursor(_col + strlen(_name), _row);
		lcd.print(":");
		lcd.setCursor(_col + strlen(_name) + 1, _row);
		lcd.print("OK");
		break;
	default:
		lcd.print(_name);
	}
}


/** End of menus **/

static void lcd_quick_feedback()
{
  lcdDrawUpdate = 2;
  button_pressed = false;  
  lcd_implementation_quick_feedback();
}

/** Menu action functions **/

/**
 * @brief Go up in menu structure
 * @param data one time action to be done before leaving menu e.g. saving data or 0
 */
static void menu_action_back(menuFunc_t data)
{
    if (data) data();
    MenuStack::Record record = menuStack.pop();
    lcd_goto_menu(record.menu);
    encoderPosition = record.position;
}
/**
 * @brief Go deeper into menu structure
 * @param data nested menu
 */
static void menu_action_submenu(menuFunc_t data) {
  menuStack.push(currentMenu, encoderPosition);
  lcd_goto_menu(data);
}
static void menu_action_gcode(const char* pgcode) {
  enquecommand_P(pgcode);
}
static void menu_action_setlang(unsigned char lang) {
  lcd_set_lang(lang);
}
static void menu_action_function(menuFunc_t data) {
  (*data)();
}

static bool check_file(const char* filename) {
	if (farm_mode) return true;
	bool result = false;
	uint32_t filesize;
	card.openFile((char*)filename, true);
	filesize = card.getFileSize();
	if (filesize > END_FILE_SECTION) {
		card.setIndex(filesize - END_FILE_SECTION);
		
	}
	
		while (!card.eof() && !result) {
		card.sdprinting = true;
		get_command();
		result = check_commands();
		
	}
	card.printingHasFinished();
	strncpy_P(lcd_status_message, WELCOME_MSG, LCD_WIDTH);
	lcd_finishstatus();
	return result;
	
}

static void menu_action_sdfile(const char* filename, char* longFilename)
{
  loading_flag = false;
  char cmd[30];
  char* c;
  bool result = true;
  sprintf_P(cmd, PSTR("M23 %s"), filename);
  for (c = &cmd[4]; *c; c++)
    *c = tolower(*c);

  for (int i = 0; i < 8; i++) {
	  eeprom_write_byte((uint8_t*)EEPROM_FILENAME + i, filename[i]);
  }

  uint8_t depth = (uint8_t)card.getWorkDirDepth();
  eeprom_write_byte((uint8_t*)EEPROM_DIR_DEPTH, depth);

  for (uint8_t i = 0; i < depth; i++) {
	  for (int j = 0; j < 8; j++) {
		  eeprom_write_byte((uint8_t*)EEPROM_DIRS + j + 8 * i, dir_names[i][j]);
	  }
  }
  
  if (!check_file(filename)) {
	  result = lcd_show_fullscreen_message_yes_no_and_wait_P(MSG_FILE_INCOMPLETE, false, false);
	  lcd_update_enable(true);
  }
  if (result) {
	  enquecommand(cmd);
	  enquecommand_P(PSTR("M24"));
  }

  lcd_return_to_status();
}
static void menu_action_sddirectory(const char* filename, char* longFilename)
{
	uint8_t depth = (uint8_t)card.getWorkDirDepth();

	strcpy(dir_names[depth], filename);
	MYSERIAL.println(dir_names[depth]);
  card.chdir(filename);
  encoderPosition = 0;
}
static void menu_action_setting_edit_bool(const char* pstr, bool* ptr)
{
  *ptr = !(*ptr);
}
/*
static void menu_action_setting_edit_callback_bool(const char* pstr, bool* ptr, menuFunc_t callback)
{
  menu_action_setting_edit_bool(pstr, ptr);
  (*callback)();
}
*/
#endif//ULTIPANEL

/** LCD API **/

void lcd_init()
{
  lcd_implementation_init();

#ifdef NEWPANEL
  SET_INPUT(BTN_EN1);
  SET_INPUT(BTN_EN2);
  WRITE(BTN_EN1, HIGH);
  WRITE(BTN_EN2, HIGH);
#if BTN_ENC > 0
  SET_INPUT(BTN_ENC);
  WRITE(BTN_ENC, HIGH);
#endif
#ifdef REPRAPWORLD_KEYPAD
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LD, OUTPUT);
  pinMode(SHIFT_OUT, INPUT);
  WRITE(SHIFT_OUT, HIGH);
  WRITE(SHIFT_LD, HIGH);
#endif
#else  // Not NEWPANEL
#ifdef SR_LCD_2W_NL // Non latching 2 wire shift register
  pinMode (SR_DATA_PIN, OUTPUT);
  pinMode (SR_CLK_PIN, OUTPUT);
#elif defined(SHIFT_CLK)
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LD, OUTPUT);
  pinMode(SHIFT_EN, OUTPUT);
  pinMode(SHIFT_OUT, INPUT);
  WRITE(SHIFT_OUT, HIGH);
  WRITE(SHIFT_LD, HIGH);
  WRITE(SHIFT_EN, LOW);
#else
#ifdef ULTIPANEL
#error ULTIPANEL requires an encoder
#endif
#endif // SR_LCD_2W_NL
#endif//!NEWPANEL

#if defined (SDSUPPORT) && defined(SDCARDDETECT) && (SDCARDDETECT > 0)
  pinMode(SDCARDDETECT, INPUT);
  WRITE(SDCARDDETECT, HIGH);
  lcd_oldcardstatus = IS_SD_INSERTED;
#endif//(SDCARDDETECT > 0)
#ifdef LCD_HAS_SLOW_BUTTONS
  slow_buttons = 0;
#endif
  lcd_buttons_update();
#ifdef ULTIPANEL
  encoderDiff = 0;
#endif
}




//#include <avr/pgmspace.h>

static volatile bool lcd_update_enabled = true;
unsigned long lcd_timeoutToStatus = 0;

void lcd_update_enable(bool enabled)
{
    if (lcd_update_enabled != enabled) {
        lcd_update_enabled = enabled;
        if (enabled) {
            // Reset encoder position. This is equivalent to re-entering a menu.
            encoderPosition = 0;
            encoderDiff = 0;
            // Enabling the normal LCD update procedure.
            // Reset the timeout interval.
            lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
            // Force the keypad update now.
            lcd_next_update_millis = millis() - 1;
            // Full update.
            lcd_implementation_clear();
      #if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
            lcd_set_custom_characters(currentMenu == lcd_status_screen);
      #else
            if (currentMenu == lcd_status_screen)
                lcd_set_custom_characters_degree();
            else
                lcd_set_custom_characters_arrows();
      #endif
            lcd_update(2);
        } else {
            // Clear the LCD always, or let it to the caller?
        }
    }
}

void lcd_update(uint8_t lcdDrawUpdateOverride)
{

	if (lcdDrawUpdate < lcdDrawUpdateOverride)
		lcdDrawUpdate = lcdDrawUpdateOverride;

	if (!lcd_update_enabled)
		return;

#ifdef LCD_HAS_SLOW_BUTTONS
  slow_buttons = lcd_implementation_read_slow_buttons(); // buttons which take too long to read in interrupt context
#endif
  
  lcd_buttons_update();

#if (SDCARDDETECT > 0)
  if ((IS_SD_INSERTED != lcd_oldcardstatus && lcd_detected()))
  {
	  lcdDrawUpdate = 2;
	  lcd_oldcardstatus = IS_SD_INSERTED;
	  lcd_implementation_init( // to maybe revive the LCD if static electricity killed it.
#if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
		  currentMenu == lcd_status_screen
#endif
	  );

	  if (lcd_oldcardstatus)
	  {
		  card.initsd();
		  LCD_MESSAGERPGM(MSG_SD_INSERTED);
		  //get_description();
	  }
	  else
	  {
		  card.release();
		  LCD_MESSAGERPGM(MSG_SD_REMOVED);
	  }
  }
#endif//CARDINSERTED

  if (lcd_next_update_millis < millis())
  {
#ifdef DEBUG_BLINK_ACTIVE
	static bool active_led = false;
	active_led = !active_led;
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, active_led?HIGH:LOW);
#endif //DEBUG_BLINK_ACTIVE

#ifdef ULTIPANEL
#ifdef REPRAPWORLD_KEYPAD
	  if (REPRAPWORLD_KEYPAD_MOVE_Z_UP) {
		  reprapworld_keypad_move_z_up();
	  }
	  if (REPRAPWORLD_KEYPAD_MOVE_Z_DOWN) {
		  reprapworld_keypad_move_z_down();
	  }
	  if (REPRAPWORLD_KEYPAD_MOVE_X_LEFT) {
		  reprapworld_keypad_move_x_left();
	  }
	  if (REPRAPWORLD_KEYPAD_MOVE_X_RIGHT) {
		  reprapworld_keypad_move_x_right();
	  }
	  if (REPRAPWORLD_KEYPAD_MOVE_Y_DOWN) {
		  reprapworld_keypad_move_y_down();
	  }
	  if (REPRAPWORLD_KEYPAD_MOVE_Y_UP) {
		  reprapworld_keypad_move_y_up();
	  }
	  if (REPRAPWORLD_KEYPAD_MOVE_HOME) {
		  reprapworld_keypad_move_home();
	  }
#endif
	  if (abs(encoderDiff) >= ENCODER_PULSES_PER_STEP)
	  {
      if (lcdDrawUpdate == 0)
		    lcdDrawUpdate = 1;
		  encoderPosition += encoderDiff / ENCODER_PULSES_PER_STEP;
		  encoderDiff = 0;
		  lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
	  }

	  if (LCD_CLICKED) lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
#endif//ULTIPANEL

	  (*currentMenu)();

#ifdef LCD_HAS_STATUS_INDICATORS
	  lcd_implementation_update_indicators();
#endif

#ifdef ULTIPANEL
	  if (lcd_timeoutToStatus < millis() && currentMenu != lcd_status_screen)
	  {
      // Exiting a menu. Let's call the menu function the last time with menuExiting flag set to true
      // to give it a chance to save its state.
      // This is useful for example, when the babystep value has to be written into EEPROM.
      if (currentMenu != NULL) {
        menuExiting = true;
        (*currentMenu)();
        menuExiting = false;
      }
	      lcd_implementation_clear();
		  lcd_return_to_status();
		  lcdDrawUpdate = 2;
	  }
#endif//ULTIPANEL
	  if (lcdDrawUpdate == 2) lcd_implementation_clear();
	  if (lcdDrawUpdate) lcdDrawUpdate--;
	  lcd_next_update_millis = millis() + LCD_UPDATE_INTERVAL;
	  }
	if (!SdFatUtil::test_stack_integrity()) stack_error();
#ifdef DEBUG_STEPPER_TIMER_MISSED
  if (stepper_timer_overflow_state) stepper_timer_overflow();
#endif /* DEBUG_STEPPER_TIMER_MISSED */
	lcd_ping(); //check that we have received ping command if we are in farm mode
	lcd_send_status();
	if (lcd_commands_type == LCD_COMMAND_V2_CAL) lcd_commands();
}

void lcd_printer_connected() {
	printer_connected = true;
}

static void lcd_send_status() {
	if (farm_mode && no_response && ((millis() - NcTime) > (NC_TIME * 1000))) {
		//send important status messages periodicaly
		prusa_statistics(important_status, saved_filament_type);
		NcTime = millis();
		lcd_connect_printer();
	}
}

static void lcd_connect_printer() {
	lcd_update_enable(false);
	lcd_implementation_clear();
	
	bool pressed = false;
	int i = 0;
	int t = 0;
	lcd_set_custom_characters_progress();
	lcd_implementation_print_at(0, 0, "Connect printer to"); 
	lcd_implementation_print_at(0, 1, "monitoring or hold");
	lcd_implementation_print_at(0, 2, "the knob to continue");
	while (no_response) {
		i++;
		t++;		
		delay_keep_alive(100);
		proc_commands();
		if (t == 10) {
			prusa_statistics(important_status, saved_filament_type);
			t = 0;
		}
		if (READ(BTN_ENC)) { //if button is not pressed
			i = 0; 
			lcd_implementation_print_at(0, 3, "                    ");
		}
		if (i!=0) lcd_implementation_print_at((i * 20) / (NC_BUTTON_LONG_PRESS * 10), 3, "\x01");
		if (i == NC_BUTTON_LONG_PRESS * 10) {
			no_response = false;
		}
	}
	lcd_set_custom_characters_degree();
	lcd_update_enable(true);
	lcd_update(2);
}

void lcd_ping() { //chceck if printer is connected to monitoring when in farm mode
	if (farm_mode) {
		bool empty = is_buffer_empty();
		if ((millis() - PingTime) * 0.001 > (empty ? PING_TIME : PING_TIME_LONG)) { //if commands buffer is empty use shorter time period
																							  //if there are comamnds in buffer, some long gcodes can delay execution of ping command
																							  //therefore longer period is used
			printer_connected = false;
			//lcd_ping_allert(); //acustic signals
		}
		else {
			lcd_printer_connected();
		}
	}
}
void lcd_ignore_click(bool b)
{
  ignore_click = b;
  wait_for_unclick = false;
}

void lcd_finishstatus() {
  int len = strlen(lcd_status_message);
  if (len > 0) {
    while (len < LCD_WIDTH) {
      lcd_status_message[len++] = ' ';
    }
  }
  lcd_status_message[LCD_WIDTH] = '\0';
#if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
#if PROGRESS_MSG_EXPIRE > 0
  messageTick =
#endif
    progressBarTick = millis();
#endif
  lcdDrawUpdate = 2;

#ifdef FILAMENT_LCD_DISPLAY
  message_millis = millis();  //get status message to show up for a while
#endif
}
void lcd_setstatus(const char* message)
{
  if (lcd_status_message_level > 0)
    return;
  strncpy(lcd_status_message, message, LCD_WIDTH);
  lcd_finishstatus();
}
void lcd_setstatuspgm(const char* message)
{
  if (lcd_status_message_level > 0)
    return;
  strncpy_P(lcd_status_message, message, LCD_WIDTH);
  lcd_status_message[LCD_WIDTH] = 0;
  lcd_finishstatus();
}
void lcd_setalertstatuspgm(const char* message)
{
  lcd_setstatuspgm(message);
  lcd_status_message_level = 1;
#ifdef ULTIPANEL
  lcd_return_to_status();
#endif//ULTIPANEL
}
void lcd_reset_alert_level()
{
  lcd_status_message_level = 0;
}

uint8_t get_message_level()
{
	return lcd_status_message_level;
}
#ifdef DOGLCD
void lcd_setcontrast(uint8_t value)
{
  lcd_contrast = value & 63;
  u8g.setContrast(lcd_contrast);
}
#endif

#ifdef ULTIPANEL
/* Warning: This function is called from interrupt context */
void lcd_buttons_update()
{
	static bool _lock = false;
	if (_lock) return;
	_lock = true;
#ifdef NEWPANEL
  uint8_t newbutton = 0;
  if (READ(BTN_EN1) == 0)  newbutton |= EN_A;
  if (READ(BTN_EN2) == 0)  newbutton |= EN_B;
#if BTN_ENC > 0
  if (lcd_update_enabled == true) { //if we are in non-modal mode, long press can be used and short press triggers with button release
	  if (READ(BTN_ENC) == 0) { //button is pressed	  
		  lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
		  if (millis() > button_blanking_time) {
			  button_blanking_time = millis() + BUTTON_BLANKING_TIME;
			  if (button_pressed == false && long_press_active == false) {
				  long_press_timer = millis();
				  button_pressed = true;
			  }
			  else {
				  if (millis() - long_press_timer > LONG_PRESS_TIME) { //long press activated

					  long_press_active = true;
					  move_menu_scale = 1.0;
					  menu_action_submenu(lcd_move_z);
				  }
			  }
		  }
	  }
	  else { //button not pressed
		  if (button_pressed) { //button was released
			  button_blanking_time = millis() + BUTTON_BLANKING_TIME;

			  if (long_press_active == false) { //button released before long press gets activated
					  newbutton |= EN_C;
			  }
			  else if (currentMenu == lcd_move_z) lcd_quick_feedback(); 
			  //button_pressed is set back to false via lcd_quick_feedback function
		  }
		  else {			  
			  long_press_active = false;
		  }
	  }
  }
  else { //we are in modal mode
	  if (READ(BTN_ENC) == 0)
		  newbutton |= EN_C; 
  }
  
#endif  
  buttons = newbutton;
#ifdef LCD_HAS_SLOW_BUTTONS
  buttons |= slow_buttons;
#endif
#ifdef REPRAPWORLD_KEYPAD
  // for the reprapworld_keypad
  uint8_t newbutton_reprapworld_keypad = 0;
  WRITE(SHIFT_LD, LOW);
  WRITE(SHIFT_LD, HIGH);
  for (int8_t i = 0; i < 8; i++) {
    newbutton_reprapworld_keypad = newbutton_reprapworld_keypad >> 1;
    if (READ(SHIFT_OUT))
      newbutton_reprapworld_keypad |= (1 << 7);
    WRITE(SHIFT_CLK, HIGH);
    WRITE(SHIFT_CLK, LOW);
  }
  buttons_reprapworld_keypad = ~newbutton_reprapworld_keypad; //invert it, because a pressed switch produces a logical 0
#endif
#else   //read it from the shift register
  uint8_t newbutton = 0;
  WRITE(SHIFT_LD, LOW);
  WRITE(SHIFT_LD, HIGH);
  unsigned char tmp_buttons = 0;
  for (int8_t i = 0; i < 8; i++)
  {
    newbutton = newbutton >> 1;
    if (READ(SHIFT_OUT))
      newbutton |= (1 << 7);
    WRITE(SHIFT_CLK, HIGH);
    WRITE(SHIFT_CLK, LOW);
  }
  buttons = ~newbutton; //invert it, because a pressed switch produces a logical 0
#endif//!NEWPANEL

  //manage encoder rotation
  uint8_t enc = 0;
  if (buttons & EN_A) enc |= B01;
  if (buttons & EN_B) enc |= B10;
  if (enc != lastEncoderBits)
  {
    switch (enc)
    {
      case encrot0:
        if (lastEncoderBits == encrot3)
          encoderDiff++;
        else if (lastEncoderBits == encrot1)
          encoderDiff--;
        break;
      case encrot1:
        if (lastEncoderBits == encrot0)
          encoderDiff++;
        else if (lastEncoderBits == encrot2)
          encoderDiff--;
        break;
      case encrot2:
        if (lastEncoderBits == encrot1)
          encoderDiff++;
        else if (lastEncoderBits == encrot3)
          encoderDiff--;
        break;
      case encrot3:
        if (lastEncoderBits == encrot2)
          encoderDiff++;
        else if (lastEncoderBits == encrot0)
          encoderDiff--;
        break;
    }
  }
  lastEncoderBits = enc;
  _lock = false;
}

bool lcd_detected(void)
{
#if (defined(LCD_I2C_TYPE_MCP23017) || defined(LCD_I2C_TYPE_MCP23008)) && defined(DETECT_DEVICE)
  return lcd.LcdDetected() == 1;
#else
  return true;
#endif
}

void lcd_buzz(long duration, uint16_t freq)
{
#ifdef LCD_USE_I2C_BUZZER
  lcd.buzz(duration, freq);
#endif
}

bool lcd_clicked()
{
	bool clicked = LCD_CLICKED;
	if(clicked) button_pressed = false;
    return clicked;
}
#endif//ULTIPANEL

/********************************/
/** Float conversion utilities **/
/********************************/
//  convert float to string with +123.4 format
char conv[8];
char *ftostr3(const float &x)
{
  return itostr3((int)x);
}

char *itostr2(const uint8_t &x)
{
  //sprintf(conv,"%5.1f",x);
  int xx = x;
  conv[0] = (xx / 10) % 10 + '0';
  conv[1] = (xx) % 10 + '0';
  conv[2] = 0;
  return conv;
}

// Convert float to string with 123.4 format, dropping sign
char *ftostr31(const float &x)
{
  int xx = x * 10;
  conv[0] = (xx >= 0) ? '+' : '-';
  xx = abs(xx);
  conv[1] = (xx / 1000) % 10 + '0';
  conv[2] = (xx / 100) % 10 + '0';
  conv[3] = (xx / 10) % 10 + '0';
  conv[4] = '.';
  conv[5] = (xx) % 10 + '0';
  conv[6] = 0;
  return conv;
}

// Convert float to string with 123.4 format
char *ftostr31ns(const float &x)
{
  int xx = x * 10;
  //conv[0]=(xx>=0)?'+':'-';
  xx = abs(xx);
  conv[0] = (xx / 1000) % 10 + '0';
  conv[1] = (xx / 100) % 10 + '0';
  conv[2] = (xx / 10) % 10 + '0';
  conv[3] = '.';
  conv[4] = (xx) % 10 + '0';
  conv[5] = 0;
  return conv;
}

char *ftostr32(const float &x)
{
  long xx = x * 100;
  if (xx >= 0)
    conv[0] = (xx / 10000) % 10 + '0';
  else
    conv[0] = '-';
  xx = abs(xx);
  conv[1] = (xx / 1000) % 10 + '0';
  conv[2] = (xx / 100) % 10 + '0';
  conv[3] = '.';
  conv[4] = (xx / 10) % 10 + '0';
  conv[5] = (xx) % 10 + '0';
  conv[6] = 0;
  return conv;
}

//// Convert float to rj string with 123.45 format
char *ftostr32ns(const float &x) {
	long xx = abs(x);
	conv[0] = xx >= 10000 ? (xx / 10000) % 10 + '0' : ' ';
	conv[1] = xx >= 1000 ? (xx / 1000) % 10 + '0' : ' ';
	conv[2] = xx >= 100 ? (xx / 100) % 10 + '0' : '0';
	conv[3] = '.';
	conv[4] = (xx / 10) % 10 + '0';
	conv[5] = xx % 10 + '0';
	return conv;
}


// Convert float to string with 1.234 format
char *ftostr43(const float &x, uint8_t offset)
{
  const size_t maxOffset = sizeof(conv)/sizeof(conv[0]) - 6;
  if (offset>maxOffset) offset = maxOffset;
  long xx = x * 1000;
  if (xx >= 0)
    conv[offset] = (xx / 1000) % 10 + '0';
  else
    conv[offset] = '-';
  xx = abs(xx);
  conv[offset + 1] = '.';
  conv[offset + 2] = (xx / 100) % 10 + '0';
  conv[offset + 3] = (xx / 10) % 10 + '0';
  conv[offset + 4] = (xx) % 10 + '0';
  conv[offset + 5] = 0;
  return conv;
}

//Float to string with 1.23 format
char *ftostr12ns(const float &x)
{
  long xx = x * 100;

  xx = abs(xx);
  conv[0] = (xx / 100) % 10 + '0';
  conv[1] = '.';
  conv[2] = (xx / 10) % 10 + '0';
  conv[3] = (xx) % 10 + '0';
  conv[4] = 0;
  return conv;
}

//Float to string with 1.234 format
char *ftostr13ns(const float &x)
{
    long xx = x * 1000;
    if (xx >= 0)
        conv[0] = ' ';
    else
        conv[0] = '-';
    xx = abs(xx);
    conv[1] = (xx / 1000) % 10 + '0';
    conv[2] = '.';
    conv[3] = (xx / 100) % 10 + '0';
    conv[4] = (xx / 10) % 10 + '0';
    conv[5] = (xx) % 10 + '0';
    conv[6] = 0;
    return conv;
}

//  convert float to space-padded string with -_23.4_ format
char *ftostr32sp(const float &x) {
  long xx = abs(x * 100);
  uint8_t dig;

  if (x < 0) { // negative val = -_0
    conv[0] = '-';
    dig = (xx / 1000) % 10;
    conv[1] = dig ? '0' + dig : ' ';
  }
  else { // positive val = __0
    dig = (xx / 10000) % 10;
    if (dig) {
      conv[0] = '0' + dig;
      conv[1] = '0' + (xx / 1000) % 10;
    }
    else {
      conv[0] = ' ';
      dig = (xx / 1000) % 10;
      conv[1] = dig ? '0' + dig : ' ';
    }
  }

  conv[2] = '0' + (xx / 100) % 10; // lsd always

  dig = xx % 10;
  if (dig) { // 2 decimal places
    conv[5] = '0' + dig;
    conv[4] = '0' + (xx / 10) % 10;
    conv[3] = '.';
  }
  else { // 1 or 0 decimal place
    dig = (xx / 10) % 10;
    if (dig) {
      conv[4] = '0' + dig;
      conv[3] = '.';
    }
    else {
      conv[3] = conv[4] = ' ';
    }
    conv[5] = ' ';
  }
  conv[6] = '\0';
  return conv;
}

char *itostr31(const int &xx)
{
  conv[0] = (xx >= 0) ? '+' : '-';
  conv[1] = (xx / 1000) % 10 + '0';
  conv[2] = (xx / 100) % 10 + '0';
  conv[3] = (xx / 10) % 10 + '0';
  conv[4] = '.';
  conv[5] = (xx) % 10 + '0';
  conv[6] = 0;
  return conv;
}

// Convert int to rj string with 123 or -12 format
char *itostr3(const int &x)
{
  int xx = x;
  if (xx < 0) {
    conv[0] = '-';
    xx = -xx;
  } else if (xx >= 100)
    conv[0] = (xx / 100) % 10 + '0';
  else
    conv[0] = ' ';
  if (xx >= 10)
    conv[1] = (xx / 10) % 10 + '0';
  else
    conv[1] = ' ';
  conv[2] = (xx) % 10 + '0';
  conv[3] = 0;
  return conv;
}

// Convert int to lj string with 123 format
char *itostr3left(const int &xx)
{
  if (xx >= 100)
  {
    conv[0] = (xx / 100) % 10 + '0';
    conv[1] = (xx / 10) % 10 + '0';
    conv[2] = (xx) % 10 + '0';
    conv[3] = 0;
  }
  else if (xx >= 10)
  {
    conv[0] = (xx / 10) % 10 + '0';
    conv[1] = (xx) % 10 + '0';
    conv[2] = 0;
  }
  else
  {
    conv[0] = (xx) % 10 + '0';
    conv[1] = 0;
  }
  return conv;
}

// Convert int to rj string with 1234 format
char *itostr4(const int &xx) {
  conv[0] = xx >= 1000 ? (xx / 1000) % 10 + '0' : ' ';
  conv[1] = xx >= 100 ? (xx / 100) % 10 + '0' : ' ';
  conv[2] = xx >= 10 ? (xx / 10) % 10 + '0' : ' ';
  conv[3] = xx % 10 + '0';
  conv[4] = 0;
  return conv;
}

// Convert float to rj string with 12345 format
char *ftostr5(const float &x) {
  long xx = abs(x);
  conv[0] = xx >= 10000 ? (xx / 10000) % 10 + '0' : ' ';
  conv[1] = xx >= 1000 ? (xx / 1000) % 10 + '0' : ' ';
  conv[2] = xx >= 100 ? (xx / 100) % 10 + '0' : ' ';
  conv[3] = xx >= 10 ? (xx / 10) % 10 + '0' : ' ';
  conv[4] = xx % 10 + '0';
  conv[5] = 0;
  return conv;
}

// Convert float to string with +1234.5 format
char *ftostr51(const float &x)
{
  long xx = x * 10;
  conv[0] = (xx >= 0) ? '+' : '-';
  xx = abs(xx);
  conv[1] = (xx / 10000) % 10 + '0';
  conv[2] = (xx / 1000) % 10 + '0';
  conv[3] = (xx / 100) % 10 + '0';
  conv[4] = (xx / 10) % 10 + '0';
  conv[5] = '.';
  conv[6] = (xx) % 10 + '0';
  conv[7] = 0;
  return conv;
}

// Convert float to string with +123.45 format
char *ftostr52(const float &x)
{
  long xx = x * 100;
  conv[0] = (xx >= 0) ? '+' : '-';
  xx = abs(xx);
  conv[1] = (xx / 10000) % 10 + '0';
  conv[2] = (xx / 1000) % 10 + '0';
  conv[3] = (xx / 100) % 10 + '0';
  conv[4] = '.';
  conv[5] = (xx / 10) % 10 + '0';
  conv[6] = (xx) % 10 + '0';
  conv[7] = 0;
  return conv;
}

/*
// Callback for after editing PID i value
// grab the PID i value out of the temp variable; scale it; then update the PID driver
void copy_and_scalePID_i()
{
#ifdef PIDTEMP
  Ki = scalePID_i(raw_Ki);
  updatePID();
#endif
}

// Callback for after editing PID d value
// grab the PID d value out of the temp variable; scale it; then update the PID driver
void copy_and_scalePID_d()
{
#ifdef PIDTEMP
  Kd = scalePID_d(raw_Kd);
  updatePID();
#endif
}
*/

#endif //ULTRA_LCD
