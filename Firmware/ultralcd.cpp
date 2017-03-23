#include "temperature.h"
#include "ultralcd.h"
#ifdef ULTRA_LCD
#include "Marlin.h"
#include "language.h"
#include "cardreader.h"
#include "temperature.h"
#include "stepper.h"
#include "ConfigurationStore.h"
#include <string.h>

#include "util.h"
#include "mesh_bed_leveling.h"
//#include "Configuration.h"

#include "SdFatUtil.h"

#define _STRINGIFY(s) #s


int8_t encoderDiff; /* encoderDiff is updated from interrupt context and added to encoderPosition every LCD update */

extern int lcd_change_fil_state;

//Function pointer to menu functions.
typedef void (*menuFunc_t)();

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

    // editMenuParentState is used when an edit menu is entered, so it knows
    // the return menu and encoder state.
    struct EditMenuParentState editMenuParentState;
};

// State of the currently active menu.
// C Union manages sharing of the static memory by all the menus.
union MenuData menuData = { 0 };

union Data
{
  byte b[2];
  int value;
};

int8_t ReInitLCD = 0;

int8_t SDscrool = 0;

int8_t SilentModeMenu = 0;

int lcd_commands_type=LCD_COMMAND_IDLE;
int lcd_commands_step=0;
bool isPrintPaused = false;
uint8_t farm_mode = 0;
int farm_no = 0;
int farm_timer = 30;
int farm_status = 0;
unsigned long allert_timer = millis();
bool printer_connected = true;


bool long_press_active = false;
long long_press_timer = millis();
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

#ifdef DOGLCD
#include "dogm_lcd_implementation.h"
#else
#include "ultralcd_implementation_hitachi_HD44780.h"
#endif

/** forward declarations **/

// void copy_and_scalePID_i();
// void copy_and_scalePID_d();

/* Different menus */
static void lcd_status_screen();
#ifdef ULTIPANEL
extern bool powersupply;
static void lcd_main_menu();
static void lcd_tune_menu();
static void lcd_prepare_menu();
static void lcd_move_menu();
static void lcd_settings_menu();
static void lcd_calibration_menu();
static void lcd_language_menu();
static void lcd_control_temperature_menu();
static void lcd_control_temperature_preheat_pla_settings_menu();
static void lcd_control_temperature_preheat_abs_settings_menu();
static void lcd_control_motion_menu();
static void lcd_control_volumetric_menu();

static void prusa_stat_printerstatus(int _status);
static void prusa_stat_farm_number();
static void prusa_stat_temperatures();
static void prusa_stat_printinfo();
static void lcd_farm_no();

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
static void menu_action_back(menuFunc_t data);
#define menu_action_back_RAM menu_action_back
static void menu_action_submenu(menuFunc_t data);
static void menu_action_gcode(const char* pgcode);
static void menu_action_function(menuFunc_t data);
static void menu_action_setlang(unsigned char lang);
static void menu_action_sdfile(const char* filename, char* longFilename);
static void menu_action_sddirectory(const char* filename, char* longFilename);
static void menu_action_setting_edit_bool(const char* pstr, bool* ptr);
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
uint32_t savedEncoderPosition;
#if (SDCARDDETECT > 0)
bool lcd_oldcardstatus;
#endif
#endif //ULTIPANEL

menuFunc_t currentMenu = lcd_status_screen; /* function pointer to the currently active menu */
menuFunc_t savedMenu;
uint32_t lcd_next_update_millis;
uint8_t lcd_status_update_delay;
bool ignore_click = false;
bool wait_for_unclick;
uint8_t lcdDrawUpdate = 2;                  /* Set to none-zero when the LCD needs to draw, decreased after every draw. Set to 2 in LCD routines so the LCD gets at least 1 full redraw (first redraw is partial) */

// place-holders for Ki and Kd edits
#ifdef PIDTEMP
// float raw_Ki, raw_Kd;
#endif

static void lcd_goto_menu(menuFunc_t menu, const uint32_t encoder = 0, const bool feedback = true, bool reset_menu_state = true) {
  if (currentMenu != menu) {
    currentMenu = menu;
    encoderPosition = encoder;
    if (reset_menu_state) {
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
			farm_timer = 180;
			prusa_statistics(0);
		}
		switch (farm_timer)
		{
		case 45:
			prusa_statistics(21);
			break;
		case 10:
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

    lcd_goto_menu(lcd_main_menu);
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

  if (farm_mode && !printer_connected) {
	  lcd.setCursor(0, 3);
	  lcd_printPGM(MSG_PRINTER_DISCONNECTED);
  }

}

#ifdef ULTIPANEL

void lcd_commands()
{
	if (lcd_commands_type == LCD_COMMAND_STOP_PRINT)   /// stop print
	{

		if (lcd_commands_step == 0) { lcd_commands_step = 6; custom_message = true;	}

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
			setTargetHotend(0, 0);
			setTargetHotend(0, 1);
			setTargetHotend(0, 2);
			manage_heater();
			lcd_setstatuspgm(WELCOME_MSG);
			cancel_heatup = false;
			lcd_commands_step = 1;
		}
		if (lcd_commands_step == 3 && !blocks_queued())
		{
      // M84: Disable steppers.
			enquecommand_P(PSTR("M84"));
#ifdef SNMM
			enquecommand_P(PSTR("PRUSA ResF")); //resets flag at the end of the print (used for SNMM)
#endif
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
			lcd_commands_step = 7;
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
			setTargetHotend(0, 0);	//to heating when changing filament for multicolor
			setTargetHotend(0, 1);
			setTargetHotend(0, 2);
			#endif
			manage_heater();
			custom_message = true;
			custom_message_type = 2;
			lcd_commands_step = 5;
		}
		if (lcd_commands_step == 7 && !blocks_queued()) {
			/*ramming();
			st_synchronize();
			change_extr(0);*/
			st_synchronize();
			enquecommand_P(PSTR("M907 E700")); //set extruder current higher
			enquecommand_P(PSTR("M203 E50"));
			st_synchronize();
			if (current_temperature[0] < 230) {
				// PLA
								
				//enquecommand_P(PSTR("G1 E-8 F2100.000000"));
				//enquecommand_P(PSTR("G1 E8 F2100.000000"));
				enquecommand_P(PSTR("G1 E5.4 F2800.000000"));
				enquecommand_P(PSTR("G1 E3.2 F3000.000000"));
				enquecommand_P(PSTR("G1 E3 F3400.000000"));
				enquecommand_P(PSTR("M203 E80"));
				st_synchronize();
				enquecommand_P(PSTR("G1 E-82 F9500.000000"));
				enquecommand_P(PSTR("M203 E50"));
				enquecommand_P(PSTR("G1 E-20 F1200.000000"));
				enquecommand_P(PSTR("G1 E5 F400.000000"));
				enquecommand_P(PSTR("G1 E5 F600.000000"));
				st_synchronize();
				enquecommand_P(PSTR("G1 E-10 F600.000000"));
				enquecommand_P(PSTR("G1 E+10 F600.000000"));
				enquecommand_P(PSTR("G1 E-10 F800.000000"));
				enquecommand_P(PSTR("G1 E+10 F800.000000"));
				enquecommand_P(PSTR("G1 E-10 F800.000000"));
				st_synchronize();
			}else {
				// ABS
				
				//enquecommand_P(PSTR("G1 E-8 F2100.000000"));
				//enquecommand_P(PSTR("G1 E8 F2100.000000"));
				enquecommand_P(PSTR("G1 E3.1 F2000.000000"));
				enquecommand_P(PSTR("G1 E3.1 F2500.000000"));
				enquecommand_P(PSTR("G1 E4 F3000.000000"));
				st_synchronize();
				enquecommand_P(PSTR("G4 P4700"));
				enquecommand_P(PSTR("M203 E80"));
				enquecommand_P(PSTR("G1 E-92 F9900.000000"));
				enquecommand_P(PSTR("M203 E50"));
				enquecommand_P(PSTR("G1 E-5 F800.000000"));
				enquecommand_P(PSTR("G1 E5 F400.000000"));
				st_synchronize();
				enquecommand_P(PSTR("G1 E-5 F600.000000"));
				enquecommand_P(PSTR("G1 E5 F600.000000"));
				enquecommand_P(PSTR("G1 E-5 F600.000000"));
				enquecommand_P(PSTR("G1 E5 F600.000000"));
				enquecommand_P(PSTR("G1 E5 F600.000000"));
				st_synchronize();
			}
			enquecommand_P(PSTR("T0"));
			enquecommand_P(PSTR("M907 E550")); //set extruder current to 500
			//digipot_init();
			
			lcd_commands_step = 3;
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


}

static void lcd_return_to_status() {
  lcd_implementation_init( // to maybe revive the LCD if static electricity killed it.
#if defined(LCD_PROGRESS_BAR) && defined(SDSUPPORT)
    currentMenu == lcd_status_screen
#endif
  );

    lcd_goto_menu(lcd_status_screen, 0, false);
}

static void lcd_sdcard_pause() {
  card.pauseSDPrint();
  isPrintPaused = true;
  lcdDrawUpdate = 3;
}

static void lcd_sdcard_resume() {
	card.startFileprint();
	isPrintPaused = false;
	lcdDrawUpdate = 3;
}

float move_menu_scale;
static void lcd_move_menu_axis();



/* Menu implementation */


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



static void lcd_preheat_menu()
{
  START_MENU();

  MENU_ITEM(back, MSG_MAIN, lcd_main_menu);

  MENU_ITEM(function, PSTR("ABS  -  " STRINGIFY(ABS_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(ABS_PREHEAT_HPB_TEMP)), lcd_preheat_abs);
  MENU_ITEM(function, PSTR("PLA  -  " STRINGIFY(PLA_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(PLA_PREHEAT_HPB_TEMP)), lcd_preheat_pla);
  MENU_ITEM(function, PSTR("PET  -  " STRINGIFY(PET_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(PET_PREHEAT_HPB_TEMP)), lcd_preheat_pet);
  MENU_ITEM(function, PSTR("HIPS -  " STRINGIFY(HIPS_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(HIPS_PREHEAT_HPB_TEMP)), lcd_preheat_hips);
  MENU_ITEM(function, PSTR("PP   -  " STRINGIFY(PP_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(PP_PREHEAT_HPB_TEMP)), lcd_preheat_pp);
  MENU_ITEM(function, PSTR("FLEX -  " STRINGIFY(FLEX_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(FLEX_PREHEAT_HPB_TEMP)), lcd_preheat_flex);

  MENU_ITEM(function, MSG_COOLDOWN, lcd_cooldown);

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

  MENU_ITEM(back, MSG_MAIN, lcd_main_menu);

  // Ideally this block would be optimized out by the compiler.
  const uint8_t fw_string_len = strlen_P(FW_VERSION_STR_P());
  if (fw_string_len < 6) {
      MENU_ITEM(back, PSTR(MSG_FW_VERSION " - " FW_version), lcd_main_menu);
  } else {
      MENU_ITEM(back, PSTR("FW - " FW_version), lcd_main_menu);
  }
      
  MENU_ITEM(back, MSG_PRUSA3D, lcd_main_menu);
  MENU_ITEM(back, MSG_PRUSA3D_FORUM, lcd_main_menu);
  MENU_ITEM(back, MSG_PRUSA3D_HOWTO, lcd_main_menu);
  MENU_ITEM(back, PSTR("------------"), lcd_main_menu);
  MENU_ITEM(back, PSTR(FILAMENT_SIZE), lcd_main_menu);
  MENU_ITEM(back, PSTR(ELECTRONICS),lcd_main_menu);
  MENU_ITEM(back, PSTR(NOZZLE_TYPE),lcd_main_menu);
  MENU_ITEM(back, PSTR("------------"), lcd_main_menu);
  MENU_ITEM(back, PSTR("Date: "), lcd_main_menu);
  MENU_ITEM(back, PSTR(__DATE__), lcd_main_menu);

  // Show the FlashAir IP address, if the card is available.
  if (menuData.supportMenu.is_flash_air) {
      MENU_ITEM(back, PSTR("------------"), lcd_main_menu);
      MENU_ITEM(back, PSTR("FlashAir IP Addr:"), lcd_main_menu);
      MENU_ITEM(back_RAM, menuData.supportMenu.ip_str, lcd_main_menu);
  }

  END_MENU();
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

  lcd_return_to_status();
}

void lcd_change_filament() {

  lcd_implementation_clear();

  lcd.setCursor(0, 1);

  lcd_printPGM(MSG_CHANGING_FILAMENT);


}


void lcd_wait_interact() {

  lcd_implementation_clear();

  lcd.setCursor(0, 1);

  lcd_printPGM(MSG_INSERT_FILAMENT);
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
      delay(110);

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



void lcd_LoadFilament()
{
  if (degHotend0() > EXTRUDE_MINTEMP) 
  {
	  custom_message = true;
	  loading_flag = true;
	  enquecommand_P(PSTR("M701")); //load filament
	  SERIAL_ECHOLN("Loading filament");	    
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



		while (!lcd_clicked())
		{
			manage_heater();
			manage_inactivity(true);
			delay(100);
		}

		lcd_quick_feedback();
		lcd_return_to_status();
	}
}


static void _lcd_move(const char *name, int axis, int min, int max) {
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
  if (LCD_CLICKED) lcd_goto_menu(lcd_move_menu_axis); {
  }
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
  if (LCD_CLICKED) lcd_goto_menu(lcd_move_menu_axis);
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
      (axis == 0) ? EEPROM_BABYSTEP_X : ((axis == 1) ? EEPROM_BABYSTEP_Y : EEPROM_BABYSTEP_Z), 
      &menuData.babyStep.babystepMem[axis]);
  }
  if (LCD_CLICKED) lcd_goto_menu(lcd_main_menu);
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

static void lcd_adjust_bed_reset()
{
    eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_VALID, 1);
    eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_LEFT , 0);
    eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_RIGHT, 0);
    eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_FRONT, 0);
    eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_REAR , 0);
    lcd_goto_menu(lcd_adjust_bed, 0, false);
    // Because we did not leave the menu, the menuData did not reset.
    // Force refresh of the bed leveling data.
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
    MENU_ITEM(back, MSG_SETTINGS, lcd_calibration_menu);
    MENU_ITEM_EDIT(int3, MSG_BED_CORRECTION_LEFT,  &menuData.adjustBed.left2,  -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);
    MENU_ITEM_EDIT(int3, MSG_BED_CORRECTION_RIGHT, &menuData.adjustBed.right2, -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);
    MENU_ITEM_EDIT(int3, MSG_BED_CORRECTION_FRONT, &menuData.adjustBed.front2, -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);
    MENU_ITEM_EDIT(int3, MSG_BED_CORRECTION_REAR,  &menuData.adjustBed.rear2,  -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);
    MENU_ITEM(function, MSG_BED_CORRECTION_RESET, lcd_adjust_bed_reset);
    END_MENU();
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

void lcd_wait_for_cool_down() {
	lcd_set_custom_characters_degree();
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
	}
	lcd_set_custom_characters_arrows();
}

// Lets the user move the Z carriage up to the end stoppers.
// When done, it sets the current Z to Z_MAX_POS and returns true.
// Otherwise the Z calibration is not changed and false is returned.
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
		lcd_show_fullscreen_message_and_wait_P(MSG_PAPER);
        lcd_display_message_fullscreen_P(MSG_FIND_BED_OFFSET_AND_SKEW_LINE1);
        lcd_implementation_print_at(0, 2, 1);
        lcd_printPGM(MSG_FIND_BED_OFFSET_AND_SKEW_LINE2);
    }
    
    
    return true;

canceled:
    return false;
}

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

const char* lcd_display_message_fullscreen_P(const char *msg, uint8_t &nlines)
{
    // Disable update of the screen by the usual lcd_update() routine. 
    lcd_update_enable(false);
    lcd_implementation_clear();
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

void lcd_show_fullscreen_message_and_wait_P(const char *msg)
{
    const char *msg_next = lcd_display_message_fullscreen_P(msg);
    bool multi_screen = msg_next != NULL;

    // Until confirmed by a button click.
    for (;;) {
        // Wait for 5 seconds before displaying the next text.
        for (uint8_t i = 0; i < 100; ++ i) {
            delay_keep_alive(50);
            if (lcd_clicked()) {
                while (lcd_clicked()) ;
                delay(10);
                while (lcd_clicked()) ;
                return;
            }
        }
        if (multi_screen) {
            if (msg_next == NULL)
                msg_next = msg;
            msg_next = lcd_display_message_fullscreen_P(msg_next);
        }
    }
}

void lcd_wait_for_click()
{
    for (;;) {
        manage_heater();
        manage_inactivity(true);
        if (lcd_clicked()) {
            while (lcd_clicked()) ;
            delay(10);
            while (lcd_clicked()) ;
            return;
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
    if (LCD_CLICKED) lcd_goto_menu(lcd_calibration_menu);
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



void prusa_statistics(int _message) {
	

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
		SERIAL_ECHOLN("{[RES:1]");
		prusa_stat_printerstatus(status_number);
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		farm_timer = 2;
		break;
	case 5:		// print not succesfull
		SERIAL_ECHOLN("{[RES:0]");
		prusa_stat_printerstatus(status_number);
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		farm_timer = 2;
		break;
	case 6:		// print done
		SERIAL_ECHOLN("{[PRN:8]");
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		status_number = 8;
		farm_timer = 2;
		break;
	case 7:		// print done - stopped
		SERIAL_ECHOLN("{[PRN:9]");
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
		SERIAL_ECHOLN("{");
		prusa_stat_printerstatus(status_number);
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		farm_timer = 5;
		break;
	case 21: // temperatures
		SERIAL_ECHO("{");
		prusa_stat_temperatures();
		prusa_stat_farm_number();
		prusa_stat_printerstatus(status_number);
		SERIAL_ECHOLN("}");
		break;
    case 22: // waiting for filament change
        SERIAL_ECHOLN("{[PRN:5]");
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		status_number = 5;
        break;
	
	case 90: // Error - Thermal Runaway
		SERIAL_ECHOLN("{[ERR:1]");
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		break;
	case 91: // Error - Thermal Runaway Preheat
		SERIAL_ECHOLN("{[ERR:2]");
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		break;
	case 92: // Error - Min temp
		SERIAL_ECHOLN("{[ERR:3]");
		prusa_stat_farm_number();
		SERIAL_ECHOLN("}");
		break;
	case 93: // Error - Max temp
		SERIAL_ECHOLN("{[ERR:4]");
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
	SERIAL_ECHO(FW_version);
	SERIAL_ECHO("]");
}


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

void lcd_move_menu_axis()
{
	START_MENU();
	MENU_ITEM(back, MSG_SETTINGS, lcd_settings_menu);
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



static void lcd_silent_mode_set() {
  SilentModeMenu = !SilentModeMenu;
  eeprom_update_byte((unsigned char *)EEPROM_SILENT, SilentModeMenu);
  digipot_init();
  lcd_goto_menu(lcd_settings_menu, 7);
}
static void lcd_set_lang(unsigned char lang) {
  lang_selected = lang;
  firstrun = 1;
  eeprom_update_byte((unsigned char *)EEPROM_LANG, lang);
  /*langsel=0;*/
  if (langsel == LANGSEL_MODAL)
    // From modal mode to an active mode? This forces the menu to return to the setup menu.
    langsel = LANGSEL_ACTIVE;
}

void lcd_force_language_selection() {
  eeprom_update_byte((unsigned char *)EEPROM_LANG, LANG_ID_FORCE_SELECTION);
}

static void lcd_language_menu()
{
  START_MENU();
  if (langsel == LANGSEL_OFF) {
    MENU_ITEM(back, MSG_SETTINGS, lcd_settings_menu);
  } else if (langsel == LANGSEL_ACTIVE) {
    MENU_ITEM(back, MSG_WATCH, lcd_status_screen);
  }
  for (int i=0;i<LANG_NUM;i++){
    MENU_ITEM(setlang, MSG_LANGUAGE_NAME_EXPLICIT(i), i);
  }
  END_MENU();
}

void lcd_mesh_bedleveling()
{

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

static void lcd_settings_menu()
{
  EEPROM_read(EEPROM_SILENT, (uint8_t*)&SilentModeMenu, sizeof(SilentModeMenu));
  START_MENU();

  MENU_ITEM(back, MSG_MAIN, lcd_main_menu);

  MENU_ITEM(submenu, MSG_TEMPERATURE, lcd_control_temperature_menu);
  if (!homing_flag)
  {
	  MENU_ITEM(submenu, MSG_MOVE_AXIS, lcd_move_menu_1mm);
  }
  if (!isPrintPaused)
  {
	  MENU_ITEM(gcode, MSG_DISABLE_STEPPERS, PSTR("M84"));
  }

  if ((SilentModeMenu == 0) || (farm_mode) ) {
    MENU_ITEM(function, MSG_SILENT_MODE_OFF, lcd_silent_mode_set);
  } else {
    MENU_ITEM(function, MSG_SILENT_MODE_ON, lcd_silent_mode_set);
  }
  
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
    
    if (farm_mode)
    {
        MENU_ITEM(submenu, PSTR("Farm number"), lcd_farm_no);
		MENU_ITEM(function, PSTR("Disable farm mode"), lcd_disable_farm_mode);
    }

	END_MENU();
}

static void lcd_calibration_menu()
{
  START_MENU();
  MENU_ITEM(back, MSG_MAIN, lcd_main_menu);
  if (!isPrintPaused)
  {
    MENU_ITEM(function, MSG_SELFTEST, lcd_selftest);
#ifndef MESH_BED_LEVELING
    // MK1
    // "Calibrate Z"
    MENU_ITEM(gcode, MSG_HOMEYZ, PSTR("G28 Z"));
#else
    // MK2
MENU_ITEM(function, MSG_CALIBRATE_BED, lcd_mesh_calibration);
    // "Calibrate Z" with storing the reference values to EEPROM.
    MENU_ITEM(submenu, MSG_HOMEYZ, lcd_mesh_calibration_z);
#ifndef SNMM
	//MENU_ITEM(function, MSG_CALIBRATE_E, lcd_calibrate_extruder);
#endif
    // "Mesh Bed Leveling"
    MENU_ITEM(submenu, MSG_MESH_BED_LEVELING, lcd_mesh_bedleveling);
#endif
    MENU_ITEM(gcode, MSG_AUTO_HOME, PSTR("G28 W"));
    MENU_ITEM(submenu, MSG_BED_CORRECTION_MENU, lcd_adjust_bed);
    MENU_ITEM(submenu, MSG_SHOW_END_STOPS, menu_show_end_stops);
    MENU_ITEM(gcode, MSG_CALIBRATE_BED_RESET, PSTR("M44"));
#ifndef SNMM
	//MENU_ITEM(function, MSG_RESET_CALIBRATE_E, lcd_extr_cal_reset);
#endif
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

char reset_menu() {
	int enc_dif = 0;
	char cursor_pos = 0;

	lcd_implementation_clear();

	lcd.setCursor(1, 0);

	lcd_printPGM(PSTR("Language"));
		

	lcd.setCursor(1, 1);

	lcd_printPGM(PSTR("Statistics"));


	lcd.setCursor(1, 2);
	
	lcd_printPGM(PSTR("Shiping prep"));

	lcd.setCursor(1, 3);
	
	lcd_printPGM(PSTR("All data"));

	lcd.setCursor(0, 0);

	lcd.print(">");


	enc_dif = encoderDiff;

	while (1) {

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

		}

		if (lcd_clicked()) {
			while (lcd_clicked());
			delay(10);
			while (lcd_clicked());
			return(cursor_pos);
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
		lcd_goto_menu(lcd_settings_menu);
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

	pinMode(E_MUX0_PIN, OUTPUT);
	pinMode(E_MUX1_PIN, OUTPUT);
	pinMode(E_MUX2_PIN, OUTPUT);
	switch (extr) {
	case 1:
		WRITE(E_MUX0_PIN, HIGH);
		WRITE(E_MUX1_PIN, LOW);
		WRITE(E_MUX2_PIN, LOW);
		
		break;
	case 2:
		WRITE(E_MUX0_PIN, LOW);
		WRITE(E_MUX1_PIN, HIGH);
		WRITE(E_MUX2_PIN, LOW);
		
		break;
	case 3:
		WRITE(E_MUX0_PIN, HIGH);
		WRITE(E_MUX1_PIN, HIGH);
		WRITE(E_MUX2_PIN, LOW);
		
		break;
	default:
		WRITE(E_MUX0_PIN, LOW);
		WRITE(E_MUX1_PIN, LOW);
		WRITE(E_MUX2_PIN, LOW);
		
		break;
	}
	delay(100);
}

static int get_ext_nr() { //reads multiplexer input pins and return current extruder number (counted from 0)
	return(4 * READ(E_MUX2_PIN) + 2 * READ(E_MUX1_PIN) + READ(E_MUX0_PIN));
}


static void extr_adj(int extruder) //loading filament for SNMM
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
			
	do{
		extr_mov(0.001,1000);
		delay_keep_alive(2);
	} while (!lcd_clicked());
	//delay_keep_alive(500);

	st_synchronize();
	//correct = lcd_show_fullscreen_message_yes_no_and_wait_P(MSG_FIL_LOADED_CHECK, false);
	//if (!correct) goto	START;
	//extr_mov(BOWDEN_LENGTH/2.f, 500); //dividing by 2 is there because of max. extrusion length limitation (x_max + y_max)
	//extr_mov(BOWDEN_LENGTH/2.f, 500);
	extr_mov(BOWDEN_LENGTH, 500);
	lcd_implementation_clear();
	lcd.setCursor(0, 1); lcd_printPGM(MSG_PLEASE_WAIT);
	st_synchronize();
	max_feedrate[E_AXIS] = 50;
	lcd_update_enable(true);
	lcd_return_to_status();
	lcdDrawUpdate = 2;
}


static void extr_unload() { //unloads filament
	float tmp_motor[3] = DEFAULT_PWM_MOTOR_CURRENT;
	float tmp_motor_loud[3] = DEFAULT_PWM_MOTOR_CURRENT_LOUD;
	int8_t SilentMode;

	if (degHotend0() > EXTRUDE_MINTEMP) {
		lcd_implementation_clear();
		lcd_display_message_fullscreen_P(PSTR(""));
		max_feedrate[E_AXIS] = 50;
		lcd.setCursor(0, 1); lcd_printPGM(MSG_PLEASE_WAIT);
		current_position[Z_AXIS] += 15; //lifting in Z direction to make space for extrusion
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 25, active_extruder);

		
		current_position[E_AXIS] += 10; //extrusion
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 10, active_extruder);
		digipot_current(2, E_MOTOR_HIGH_CURRENT);
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
		current_position[E_AXIS] -= (BOWDEN_LENGTH + 60 + FIL_LOAD_LENGTH) / 2;   
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 500, active_extruder);
		current_position[E_AXIS] -= (BOWDEN_LENGTH + 60 + FIL_LOAD_LENGTH) / 2;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 500, active_extruder);
		st_synchronize();
		//digipot_init();
		if (SilentMode == 1) digipot_current(2, tmp_motor[2]); //set back to normal operation currents
		else digipot_current(2, tmp_motor_loud[2]);
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
	MENU_ITEM(back, MSG_MAIN, lcd_main_menu);
	MENU_ITEM(function, PSTR("Load filament 1"), extr_adj_0);
	MENU_ITEM(function, PSTR("Load filament 2 "), extr_adj_1);
	MENU_ITEM(function, PSTR("Load filament 3"), extr_adj_2);
	MENU_ITEM(function, PSTR("Load filament 4"), extr_adj_3);
	
	END_MENU();
}


static void fil_unload_menu()
{
	START_MENU();
	MENU_ITEM(back, MSG_MAIN, lcd_main_menu);
	MENU_ITEM(function, PSTR("Unload filament 1"), extr_unload_0);
	MENU_ITEM(function, PSTR("Unload filament 2"), extr_unload_1);
	MENU_ITEM(function, PSTR("Unload filament 3"), extr_unload_2);
	MENU_ITEM(function, PSTR("Unload filament 4"), extr_unload_3);

	END_MENU();
}

static void change_extr_menu(){
	START_MENU();
	MENU_ITEM(back, MSG_MAIN, lcd_main_menu);
	MENU_ITEM(function, PSTR("Extruder 1"), extr_change_0);
	MENU_ITEM(function, PSTR("Extruder 2"), extr_change_1);
	MENU_ITEM(function, PSTR("Extruder 3"), extr_change_2);
	MENU_ITEM(function, PSTR("Extruder 4"), extr_change_3);

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

void lcd_confirm_print()
{
	int enc_dif = 0;
	int cursor_pos = 1;
	int _ret = 0;
	int _t = 0;


	lcd_implementation_clear();

	lcd.setCursor(0, 0);
	lcd.print("Print ok ?");

	do
	{

		if (abs((enc_dif - encoderDiff)) > 2) {
			if (enc_dif > encoderDiff) {
				cursor_pos--;
			}

			if (enc_dif < encoderDiff) {
				cursor_pos++;
			}
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
				prusa_statistics(20);
				prusa_statistics(4);
			}
			if (cursor_pos == 2)
			{
				_ret = 2;
				prusa_statistics(20);
				prusa_statistics(5);
			}
		}

		manage_heater();
		manage_inactivity();

	} while (_ret == 0);

}



static void lcd_main_menu()
{

  SDscrool = 0;
  START_MENU();

  // Majkl superawesome menu

  
 MENU_ITEM(back, MSG_WATCH, lcd_status_screen);
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
        
        MENU_ITEM(back, PSTR("- - - - - - - - -"), lcd_status_screen);
    
        
    }*/
    
    
    
  if ( ( IS_SD_PRINTING || is_usb_printing ) && (current_position[Z_AXIS] < Z_HEIGHT_HIDE_LIVE_ADJUST_MENU) && !homing_flag) 
  {
	MENU_ITEM(submenu, MSG_BABYSTEP_Z, lcd_babystep_z);//8
  }


  if ( moves_planned() || IS_SD_PRINTING || is_usb_printing )
  {
    MENU_ITEM(submenu, MSG_TUNE, lcd_tune_menu);
  } else 
  {
    MENU_ITEM(submenu, MSG_PREHEAT, lcd_preheat_menu);
  }

#ifdef SDSUPPORT
  if (card.cardOK)
  {
    if (card.isFileOpen())
    {
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
	else
	{
		if (!is_usb_printing)
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


  if (IS_SD_PRINTING || is_usb_printing)
  {
	  if (farm_mode)
	  {
		  MENU_ITEM(submenu, PSTR("Farm number"), lcd_farm_no);
	  }
  } 
  else 
  {
	#ifndef SNMM
    MENU_ITEM(function, MSG_LOAD_FILAMENT, lcd_LoadFilament);
    MENU_ITEM(function, MSG_UNLOAD_FILAMENT, lcd_unLoadFilament);
	#endif
	#ifdef SNMM
	MENU_ITEM(submenu, MSG_LOAD_FILAMENT, fil_load_menu);
	MENU_ITEM(submenu, MSG_UNLOAD_FILAMENT, fil_unload_menu);
	MENU_ITEM(submenu, MSG_CHANGE_EXTR, change_extr_menu);
	#endif
	MENU_ITEM(submenu, MSG_SETTINGS, lcd_settings_menu);
    if(!isPrintPaused) MENU_ITEM(submenu, MSG_MENU_CALIBRATION, lcd_calibration_menu);
  }

  if (!is_usb_printing)
  {
	  MENU_ITEM(submenu, MSG_STATISTICS, lcd_menu_statistics);
  }
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

#ifdef SDSUPPORT
static void lcd_autostart_sd()
{
  card.lastnr = 0;
  card.setroot();
  card.checkautostart(true);
}
#endif



static void lcd_silent_mode_set_tune() {
  SilentModeMenu = !SilentModeMenu;
  eeprom_update_byte((unsigned char*)EEPROM_SILENT, SilentModeMenu);
  digipot_init();
  lcd_goto_menu(lcd_tune_menu, 9);
}

static void lcd_tune_menu()
{
  EEPROM_read(EEPROM_SILENT, (uint8_t*)&SilentModeMenu, sizeof(SilentModeMenu));

  

  START_MENU();
  MENU_ITEM(back, MSG_MAIN, lcd_main_menu); //1
  MENU_ITEM_EDIT(int3, MSG_SPEED, &feedmultiply, 10, 999);//2

  MENU_ITEM_EDIT(int3, MSG_NOZZLE, &target_temperature[0], 0, HEATER_0_MAXTEMP - 10);//3
  MENU_ITEM_EDIT(int3, MSG_BED, &target_temperature_bed, 0, BED_MAXTEMP - 10);//4

  MENU_ITEM_EDIT(int3, MSG_FAN_SPEED, &fanSpeed, 0, 255);//5
  MENU_ITEM_EDIT(int3, MSG_FLOW, &extrudemultiply, 10, 999);//6
#ifdef FILAMENTCHANGEENABLE
  MENU_ITEM(gcode, MSG_FILAMENTCHANGE, PSTR("M600"));//7
#endif
  
  if (SilentModeMenu == 0) {
    MENU_ITEM(function, MSG_SILENT_MODE_OFF, lcd_silent_mode_set_tune);
  } else {
    MENU_ITEM(function, MSG_SILENT_MODE_ON, lcd_silent_mode_set_tune);
  }
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
  MENU_ITEM(back, MSG_SETTINGS, lcd_settings_menu);
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
				cancel_heatup = true;
        #ifdef MESH_BED_LEVELING
        mbl.active = false;
        #endif
        // Stop the stoppers, update the position from the stoppers.
        planner_abort_hard();
        // Because the planner_abort_hard() initialized current_position[Z] from the stepper,
        // Z baystep is no more applied. Reset it.
        babystep_reset();
        // Clean the input command queue.
        cmdqueue_reset();
				lcd_setstatuspgm(MSG_PRINT_ABORTED);
				card.sdprinting = false;
				card.closefile();

				stoptime = millis();
				unsigned long t = (stoptime - starttime) / 1000; //time in s
				save_statistics(total_filament_used, t);

				lcd_return_to_status();
				lcd_ignore_click(true);
				lcd_commands_type = LCD_COMMAND_STOP_PRINT;
            
                // Turn off the print fan
                SET_OUTPUT(FAN_PIN);
                WRITE(FAN_PIN, 0);
                fanSpeed=0;
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

	int tempScrool = 0;
  if (lcdDrawUpdate == 0 && LCD_CLICKED == 0)
    //delay(100);
    return; // nothing to do (so don't thrash the SD card)
  uint16_t fileCnt = card.getnrfilenames();

  START_MENU();
  MENU_ITEM(back, MSG_MAIN, lcd_main_menu);
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
		MENU_ITEM(back, MSG_MAIN, lcd_main_menu);
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
    menuData.editMenuParentState.prevMenu = currentMenu; \
    menuData.editMenuParentState.prevEncoderPosition = encoderPosition; \
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

menu_edit_type(int, int3, itostr3, 1)
menu_edit_type(float, float3, ftostr3, 1)
menu_edit_type(float, float32, ftostr32, 100)
menu_edit_type(float, float43, ftostr43, 1000)
menu_edit_type(float, float5, ftostr5, 0.01)
menu_edit_type(float, float51, ftostr51, 10)
menu_edit_type(float, float52, ftostr52, 100)
menu_edit_type(unsigned long, long5, ftostr5, 0.01)

static void lcd_selftest()
{
	int _progress = 0;
	bool _result = false;

	lcd_implementation_clear();
	lcd.setCursor(0, 0); lcd_printPGM(MSG_SELFTEST_START);
	delay(2000);


	_result = lcd_selftest_fan_dialog(1);

	if (_result)
	{
		_result = lcd_selftest_fan_dialog(2);
	}

	if (_result)
	{
		_progress = lcd_selftest_screen(0, _progress, 3, true, 2000);
		_result = lcd_selfcheck_endstops();
	}
		
	if (_result)
	{
		_progress = lcd_selftest_screen(1, _progress, 3, true, 1000);
		_result = lcd_selfcheck_check_heater(false);
	}

	if (_result)
	{
		current_position[Z_AXIS] += 15;									//move Z axis higher to avoid false triggering of Z end stop in case that we are very low - just above heatbed
		_progress = lcd_selftest_screen(2, _progress, 3, true, 2000);
		_result = lcd_selfcheck_axis(X_AXIS, X_MAX_POS);
		
	}

	if (_result)
	{
		_progress = lcd_selftest_screen(2, _progress, 3, true, 0);
		_result = lcd_selfcheck_pulleys(X_AXIS);
	}


	if (_result)
	{
		_progress = lcd_selftest_screen(3, _progress, 3, true, 1500);
		_result = lcd_selfcheck_axis(Y_AXIS, Y_MAX_POS);
	}

	if (_result)
	{
		_progress = lcd_selftest_screen(3, _progress, 3, true, 0);
		_result = lcd_selfcheck_pulleys(Y_AXIS);
	}


	if (_result)
	{
		current_position[X_AXIS] = current_position[X_AXIS] - 3;
		current_position[Y_AXIS] = current_position[Y_AXIS] - 14;
		_progress = lcd_selftest_screen(4, _progress, 3, true, 1500);
		_result = lcd_selfcheck_axis(2, Z_MAX_POS);
		enquecommand_P(PSTR("G28 W"));
	}

	if (_result)
	{
		_progress = lcd_selftest_screen(5, _progress, 3, true, 2000);
		_result = lcd_selfcheck_check_heater(true);
	}
	if (_result)
	{
		_progress = lcd_selftest_screen(6, _progress, 3, true, 5000);
	}
	else
	{
		_progress = lcd_selftest_screen(7, _progress, 3, true, 5000);
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
}

static bool lcd_selfcheck_axis(int _axis, int _travel)
{
	bool _stepdone = false;
	bool _stepresult = false;
	int _progress = 0;
	int _travel_done = 0;
	int _err_endstop = 0;
	int _lcd_refresh = 0;
	_travel = _travel + (_travel / 10);

	do {
		current_position[_axis] = current_position[_axis] - 1;

		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);
		st_synchronize();

		if (READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING == 1 || READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING == 1 || READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING == 1)
		{
			if (_axis == 0)
			{
				_stepresult = (READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING == 1) ? true : false;
				_err_endstop = (READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING == 1) ? 1 : 2;
				
			}
			if (_axis == 1)
			{
				_stepresult = (READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING == 1) ? true : false;
				_err_endstop = (READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING == 1) ? 0 : 2;
				
			}
			if (_axis == 2)
			{
				_stepresult = (READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING == 1) ? true : false;
				_err_endstop = (READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING == 1) ? 0 : 1;
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
			_progress = lcd_selftest_screen(2 + _axis, _progress, 3, false, 0);
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

static bool lcd_selfcheck_pulleys(int axis)
{
	float tmp_motor_loud[3] = DEFAULT_PWM_MOTOR_CURRENT_LOUD;
	float tmp_motor[3] = DEFAULT_PWM_MOTOR_CURRENT;
	float current_position_init;
	float move;
	bool endstop_triggered = false;
	bool result = true;
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
			digipot_current(0, 850); //set motor current higher
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], 200, active_extruder);
			st_synchronize();
			if (SilentModeMenu == 1) digipot_current(0, tmp_motor[0]); //set back to normal operation currents
			else digipot_current(0, tmp_motor_loud[0]); //set motor current back			
			current_position[axis] = current_position[axis] - move;
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], 50, active_extruder);
			st_synchronize();
			if ((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING == 1) || (READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING == 1)) {
				lcd_selftest_error(8, (axis == 0) ? "X" : "Y", "");
				return(false);
			}
		}
		timeout_counter = millis() + 2500;
		endstop_triggered = false;
		manage_inactivity(true);
		while (!endstop_triggered) {
			if ((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING == 1) || (READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING == 1)) {
				endstop_triggered = true;
				if (current_position_init - 1 <= current_position[axis] && current_position_init + 1 >= current_position[axis]) {
					current_position[axis] += 15;
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
}

static bool lcd_selfcheck_endstops()
{
	bool _result = true;

	if (READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING == 1 || READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING == 1 || READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING == 1)
	{
		current_position[0] = (READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING == 1) ? current_position[0] = current_position[0] + 10 : current_position[0];
		current_position[1] = (READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING == 1) ? current_position[1] = current_position[1] + 10 : current_position[1];
		current_position[2] = (READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING == 1) ? current_position[2] = current_position[2] + 10 : current_position[2];
	}
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], manual_feedrate[0] / 60, active_extruder);
	delay(500);

	if (READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING == 1 || READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING == 1 || READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING == 1)
	{
		_result = false;
		String  _error = String((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING == 1) ? "X" : "") +
			String((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING == 1) ? "Y" : "") +
			String((READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING == 1) ? "Z" : "");
		lcd_selftest_error(3, _error.c_str(), "");
	}
	manage_heater();
	manage_inactivity(true);
	return _result;
}

static bool lcd_selfcheck_check_heater(bool _isbed)
{
	int _counter = 0;
	int _progress = 0;
	bool _stepresult = false;
	bool _docycle = true;

	int _checked_snapshot = (_isbed) ? degBed() : degHotend(0);
	int _opposite_snapshot = (_isbed) ? degHotend(0) : degBed();
	int _cycles = (_isbed) ? 120 : 30;

	target_temperature[0] = (_isbed) ? 0 : 100;
	target_temperature_bed = (_isbed) ? 100 : 0;
	manage_heater();
	manage_inactivity(true);

	do {
		_counter++;
		(_counter < _cycles) ? _docycle = true : _docycle = false;

		manage_heater();
		manage_inactivity(true);
		_progress = (_isbed) ? lcd_selftest_screen(5, _progress, 2, false, 400) : lcd_selftest_screen(1, _progress, 2, false, 400);

	} while (_docycle);

	target_temperature[0] = 0;
	target_temperature_bed = 0;
	manage_heater();

	int _checked_result = (_isbed) ? degBed() - _checked_snapshot : degHotend(0) - _checked_snapshot;
	int _opposite_result = (_isbed) ? degHotend(0) - _opposite_snapshot : degBed() - _opposite_snapshot;

	if (_opposite_result < (_isbed) ? 10 : 3)
	{
		if (_checked_result >= (_isbed) ? 3 : 10)
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

static bool lcd_selftest_fan_dialog(int _fan)
{
	bool _result = false;
	int _errno = 0;
	lcd_implementation_clear();

	lcd.setCursor(0, 0); lcd_printPGM(MSG_SELFTEST_FAN);
	switch (_fan)
	{
	case 1:
		// extruder cooling fan
		lcd.setCursor(0, 1); lcd_printPGM(MSG_SELFTEST_EXTRUDER_FAN);
		SET_OUTPUT(EXTRUDER_0_AUTO_FAN_PIN);
		WRITE(EXTRUDER_0_AUTO_FAN_PIN, 1);
		_errno = 7;
		break;
	case 2:
		// object cooling fan
		lcd.setCursor(0, 1); lcd_printPGM(MSG_SELFTEST_COOLING_FAN);
		SET_OUTPUT(FAN_PIN);
		analogWrite(FAN_PIN, 255);
		_errno = 6;
		break;
	}
	delay(500);

	lcd.setCursor(1, 2); lcd_printPGM(MSG_SELFTEST_FAN_YES);
	lcd.setCursor(0, 3); lcd.print(">");
	lcd.setCursor(1, 3); lcd_printPGM(MSG_SELFTEST_FAN_NO);





	int8_t enc_dif = 0;
	bool _response = false;
	do
	{

		switch (_fan)
		{
		case 1:
			// extruder cooling fan
			SET_OUTPUT(EXTRUDER_0_AUTO_FAN_PIN);
			WRITE(EXTRUDER_0_AUTO_FAN_PIN, 1);
			break;
		case 2:
			// object cooling fan
			SET_OUTPUT(FAN_PIN);
			analogWrite(FAN_PIN, 255);
			break;
		}


		if (abs((enc_dif - encoderDiff)) > 2) {
			if (enc_dif > encoderDiff) {
				_result = true;
				lcd.setCursor(0, 2); lcd.print(">");
				lcd.setCursor(1, 2); lcd_printPGM(MSG_SELFTEST_FAN_YES);
				lcd.setCursor(0, 3); lcd.print(" ");
				lcd.setCursor(1, 3); lcd_printPGM(MSG_SELFTEST_FAN_NO);
			}

			if (enc_dif < encoderDiff) {
				_result = false;
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

		if (lcd_clicked())
		{
			_response = true;
		}


	} while (!_response);

	SET_OUTPUT(EXTRUDER_0_AUTO_FAN_PIN);
	WRITE(EXTRUDER_0_AUTO_FAN_PIN, 0);
	SET_OUTPUT(FAN_PIN);
	analogWrite(FAN_PIN, 0);

	fanSpeed = 0;
	manage_heater();

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

	if (_step == -1) lcd_printPGM(MSG_SELFTEST_START);
	if (_step == 0) lcd_printPGM(MSG_SELFTEST_CHECK_ENDSTOPS);
	if (_step == 1) lcd_printPGM(MSG_SELFTEST_CHECK_HOTEND);
	if (_step == 2) lcd_printPGM(MSG_SELFTEST_CHECK_X);
	if (_step == 3) lcd_printPGM(MSG_SELFTEST_CHECK_Y);
	if (_step == 4) lcd_printPGM(MSG_SELFTEST_CHECK_Z);
	if (_step == 5) lcd_printPGM(MSG_SELFTEST_CHECK_BED);
	if (_step == 6) lcd_printPGM(MSG_SELFTEST_CHECK_ALLCORRECT);
	if (_step == 7) lcd_printPGM(MSG_SELFTEST_FAILED);

	lcd.setCursor(0, 1);
	lcd.print("--------------------");

	if (_step != 7)
	{
		_step_block = 1;
		lcd_selftest_screen_step(3, 9, ((_step == _step_block) ? 1 : (_step < _step_block) ? 0 : 2), "Hotend", _indicator);

		_step_block = 2;
		lcd_selftest_screen_step(2, 2, ((_step == _step_block) ? 1 : (_step < _step_block) ? 0 : 2), "X", _indicator);

		_step_block = 3;
		lcd_selftest_screen_step(2, 8, ((_step == _step_block) ? 1 : (_step < _step_block) ? 0 : 2), "Y", _indicator);

		_step_block = 4;
		lcd_selftest_screen_step(2, 14, ((_step == _step_block) ? 1 : (_step < _step_block) ? 0 : 2), "Z", _indicator);

		_step_block = 5;
		lcd_selftest_screen_step(3, 0, ((_step == _step_block) ? 1 : (_step < _step_block) ? 0 : 2), "Bed", _indicator);
	}

	if (_delay > 0) delay(_delay);
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
static void menu_action_back(menuFunc_t data) {
  lcd_goto_menu(data);
}
static void menu_action_submenu(menuFunc_t data) {
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
static void menu_action_sdfile(const char* filename, char* longFilename)
{
  loading_flag = false;
  char cmd[30];
  char* c;
  sprintf_P(cmd, PSTR("M23 %s"), filename);
  for (c = &cmd[4]; *c; c++)
    *c = tolower(*c);
  enquecommand(cmd);
  enquecommand_P(PSTR("M24"));
  lcd_return_to_status();
}
static void menu_action_sddirectory(const char* filename, char* longFilename)
{
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

	  /*if (LCD_CLICKED)*/ lcd_timeoutToStatus = millis() + LCD_TIMEOUT_TO_STATUS;
#endif//ULTIPANEL

#ifdef DOGLCD        // Changes due to different driver architecture of the DOGM display
	  blink++;     // Variable for fan animation and alive dot
	  u8g.firstPage();
	  do
	  {
		  u8g.setFont(u8g_font_6x10_marlin);
		  u8g.setPrintPos(125, 0);
		  if (blink % 2) u8g.setColorIndex(1); else u8g.setColorIndex(0); // Set color for the alive dot
		  u8g.drawPixel(127, 63); // draw alive dot
		  u8g.setColorIndex(1); // black on white
		  (*currentMenu)();
		  if (!lcdDrawUpdate)  break; // Terminate display update, when nothing new to draw. This must be done before the last dogm.next()
	  } while (u8g.nextPage());
#else
	  (*currentMenu)();
#endif

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
		  lcd_return_to_status();
		  lcdDrawUpdate = 2;
	  }
#endif//ULTIPANEL
	  if (lcdDrawUpdate == 2) lcd_implementation_clear();
	  if (lcdDrawUpdate) lcdDrawUpdate--;
	  lcd_next_update_millis = millis() + LCD_UPDATE_INTERVAL;
	  }
	if (!SdFatUtil::test_stack_integrity()) stack_error();
	lcd_ping(); //check that we have received ping command if we are in farm mode
}

void lcd_printer_connected() {
	printer_connected = true;
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
#ifdef NEWPANEL
  uint8_t newbutton = 0;
  if (READ(BTN_EN1) == 0)  newbutton |= EN_A;
  if (READ(BTN_EN2) == 0)  newbutton |= EN_B;
#if BTN_ENC > 0
  if (lcd_update_enabled == true) { //if we are in non-modal mode, long press can be used and short press triggers with button release
	  if (READ(BTN_ENC) == 0) { //button is pressed	  

		  if (button_pressed == false && long_press_active == false) {
			  if (currentMenu != lcd_move_z) {
				  savedMenu = currentMenu;
				  savedEncoderPosition = encoderPosition;
			  }
			  long_press_timer = millis();
			  button_pressed = true;
		  }
		  else {
			  if (millis() - long_press_timer > LONG_PRESS_TIME) { //long press activated
				   
				  long_press_active = true;
				  move_menu_scale = 1.0;
				  lcd_goto_menu(lcd_move_z);
			  }
		  }
	  }
	  else { //button not pressed
		  if (button_pressed) { //button was released
			  if (long_press_active == false) { //button released before long press gets activated
				  if (currentMenu == lcd_move_z) {
					  //return to previously active menu and previous encoder position
					  lcd_goto_menu(savedMenu, savedEncoderPosition);
				  }
				  else {
					  newbutton |= EN_C;
				  }
			  }
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
  return LCD_CLICKED;
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
char *ftostr43(const float &x)
{
  long xx = x * 1000;
  if (xx >= 0)
    conv[0] = (xx / 1000) % 10 + '0';
  else
    conv[0] = '-';
  xx = abs(xx);
  conv[1] = '.';
  conv[2] = (xx / 100) % 10 + '0';
  conv[3] = (xx / 10) % 10 + '0';
  conv[4] = (xx) % 10 + '0';
  conv[5] = 0;
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