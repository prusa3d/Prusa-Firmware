#ifndef ULTRALCD_H
#define ULTRALCD_H

#include "Marlin.h"
#include "lcd.h"
#include "conv2str.h"
#include "menu.h"
#include "mesh_bed_calibration.h"
#include "config.h"

#include "config.h"

extern void menu_lcd_longpress_func(void);
extern void menu_lcd_lcdupdate_func(void);

// Call with a false parameter to suppress the LCD update from various places like the planner or the temp control.
void ultralcd_init();
void lcd_setstatus(const char* message);
void lcd_setstatuspgm(const char* message);
//! return to the main status screen and display the alert message
//! Beware - it has sideeffects:
//! - always returns the display to the main status screen
//! - always makes lcd_reset (which is slow and causes flicker)
//! - does not update the message if there is already one (i.e. lcd_status_message_level > 0)
void lcd_setalertstatus(const char* message);
void lcd_setalertstatuspgm(const char* message);
//! only update the alert message on the main status screen
//! has no sideeffects, may be called multiple times
void lcd_updatestatus(const char *message);
void lcd_updatestatuspgm(const char *message);

void lcd_reset_alert_level();
uint8_t get_message_level();
void lcd_adjust_z();
void lcd_pick_babystep();
void lcd_alright();
void show_preheat_nozzle_warning();
void EEPROM_save_B(int pos, int* value);
void EEPROM_read_B(int pos, int* value);
void lcd_wait_interact();
void lcd_loading_filament();
void lcd_change_success();
void lcd_loading_color();
void lcd_sdcard_stop();
void lcd_pause_print();
void lcd_resume_print();
void lcd_print_stop();
void prusa_statistics(int _message, uint8_t _col_nr = 0);
void lcd_confirm_print();
unsigned char lcd_choose_color();
void lcd_load_filament_color_check();
//void lcd_mylang();

extern void lcd_belttest();
extern bool lcd_selftest();

void lcd_menu_statistics(); 

void lcd_status_screen();                         // NOT static due to using inside "Marlin_main" module ("manage_inactivity()")
void lcd_menu_extruder_info();                    // NOT static due to using inside "Marlin_main" module ("manage_inactivity()")
void lcd_menu_show_sensors_state();               // NOT static due to using inside "Marlin_main" module ("manage_inactivity()")

#ifdef TMC2130
bool lcd_crash_detect_enabled();
void lcd_crash_detect_enable();
void lcd_crash_detect_disable();
#endif

extern const char* lcd_display_message_fullscreen_P(const char *msg, uint8_t &nlines);
extern const char* lcd_display_message_fullscreen_P(const char *msg);

extern void lcd_return_to_status();
extern void lcd_wait_for_click();
extern bool lcd_wait_for_click_delay(uint16_t nDelay);
extern void lcd_show_fullscreen_message_and_wait_P(const char *msg);
// 0: no, 1: yes, -1: timeouted
extern int8_t lcd_show_fullscreen_message_yes_no_and_wait_P(const char *msg, bool allow_timeouting = true, bool default_yes = false);
extern int8_t lcd_show_multiscreen_message_two_choices_and_wait_P(const char *msg, bool allow_timeouting, bool default_yes,
        const char *first_choice, const char *second_choice);
extern int8_t lcd_show_multiscreen_message_yes_no_and_wait_P(const char *msg, bool allow_timeouting = true, bool default_yes = false);
// Ask the user to move the Z axis up to the end stoppers and let
// the user confirm that it has been done.

#ifndef TMC2130
extern bool lcd_calibrate_z_end_stop_manual(bool only_z);
#endif

// Show the result of the calibration process on the LCD screen.
  extern void lcd_bed_calibration_show_result(BedSkewOffsetDetectionResultType result, uint8_t point_too_far_mask);

extern void lcd_diag_show_end_stops();


#define LCD_MESSAGEPGM(x) lcd_setstatuspgm(PSTR(x))
#define LCD_ALERTMESSAGEPGM(x) lcd_setalertstatuspgm(PSTR(x))
#define LCD_MESSAGERPGM(x) lcd_setstatuspgm((x))
#define LCD_ALERTMESSAGERPGM(x) lcd_setalertstatuspgm((x))


// To be used in lcd_commands_type.
enum class LcdCommands : uint_least8_t
{
	Idle,
	LoadFilament,
	StopPrint,
	FarmModeConfirm,
	LongPause,
	PidExtruder,
	Layer1Cal,
};

extern LcdCommands lcd_commands_type;
extern int8_t FSensorStateMenu;

enum class CustomMsg : uint_least8_t
{
	Status,          //!< status message from lcd_status_message variable
	MeshBedLeveling, //!< Mesh bed leveling in progress
	FilamentLoading, //!< Loading filament in progress
	PidCal,          //!< PID tuning in progress
	TempCal,         //!< PINDA temperature calibration
	TempCompPreheat, //!< Temperature compensation preheat
};

extern CustomMsg custom_message_type;
extern unsigned int custom_message_state;

extern uint8_t farm_mode;
extern int farm_no;
extern int farm_timer;
extern uint8_t farm_status;

#ifdef TMC2130
#define SILENT_MODE_NORMAL 0
#define SILENT_MODE_STEALTH 1
#define SILENT_MODE_OFF SILENT_MODE_NORMAL
#else
#define SILENT_MODE_POWER 0
#define SILENT_MODE_SILENT 1
#define SILENT_MODE_AUTO 2
#define SILENT_MODE_OFF SILENT_MODE_POWER
#endif

#ifdef IR_SENSOR_ANALOG
extern bool bMenuFSDetect;
void printf_IRSensorAnalogBoardChange();
#endif //IR_SENSOR_ANALOG

extern int8_t SilentModeMenu;
extern uint8_t SilentModeMenu_MMU;

extern bool cancel_heatup;
extern bool isPrintPaused;


void lcd_ignore_click(bool b=true);
void lcd_commands();


extern bool bSettings;                            // flag (i.e. 'fake parameter') for 'lcd_hw_setup_menu()' function
void lcd_hw_setup_menu(void);                     // NOT static due to using inside "util" module ("nozzle_diameter_check()")


void change_extr(int extr);

#ifdef SNMM
void extr_unload_all(); 
void extr_unload_used();
#endif //SNMM
void extr_unload();

enum class FilamentAction : uint_least8_t
{
    None, //!< 'none' state is used as flag for (filament) autoLoad (i.e. opposite for 'autoLoad' state)
    Load,
    AutoLoad,
    UnLoad,
    MmuLoad,
    MmuUnLoad,
    MmuEject,
    MmuCut,
    Preheat,
    Lay1Cal,
};

extern FilamentAction eFilamentAction;
extern bool bFilamentFirstRun;
extern bool bFilamentPreheatState;
extern bool bFilamentAction;
void mFilamentItem(uint16_t nTemp,uint16_t nTempBed);
void mFilamentItemForce();
void lcd_generic_preheat_menu();
void unload_filament();

void stack_error();
void lcd_printer_connected();
void lcd_ping();

void lcd_calibrate_extruder();
void lcd_farm_sdcard_menu();

//void getFileDescription(char *name, char *description);

void lcd_farm_sdcard_menu_w();
//void get_description();

void lcd_wait_for_heater();
void lcd_wait_for_cool_down();
void lcd_extr_cal_reset();

void lcd_temp_cal_show_result(bool result);
#ifdef PINDA_THERMISTOR
bool lcd_wait_for_pinda(float temp);
#endif //PINDA_THERMISTOR


void bowden_menu();
char reset_menu();
uint8_t choose_menu_P(const char *header, const char *item, const char *last_item = nullptr);

void lcd_pinda_calibration_menu();
void lcd_calibrate_pinda();
void lcd_temp_calibration_set();

void display_loading();

#if (LANG_MODE != 0)
void lcd_language();
#endif

void lcd_wizard();
bool lcd_autoDepleteEnabled();

//! @brief Wizard state
enum class WizState : uint8_t
{
    Run,            //!< run wizard? Main entry point.
    Restore,        //!< restore calibration status
    Selftest,       //!< self test
    Xyz,            //!< xyz calibration
    Z,              //!< z calibration
    IsFil,          //!< Is filament loaded? First step of 1st layer calibration
    PreheatPla,     //!< waiting for preheat nozzle for PLA
    Preheat,        //!< Preheat for any material
    LoadFilCold,    //!< Load filament for MMU
    LoadFilHot,     //!< Load filament without MMU
    IsPla,          //!< Is PLA filament?
    Lay1CalCold,    //!< First layer calibration, temperature not selected yet
    Lay1CalHot,     //!< First layer calibration, temperature already selected
    RepeatLay1Cal,  //!< Repeat first layer calibration?
    Finish,         //!< Deactivate wizard
};

void lcd_wizard(WizState state);

extern void lcd_experimental_toggle();
extern void lcd_experimental_menu();


// hardware ultralcd stuff

#define LCD_UPDATE_INTERVAL    100
#define LCD_TIMEOUT_TO_STATUS 30000ul //!< Generic timeout to status screen in ms, when no user action.
#define LCD_TIMEOUT_TO_STATUS_BABYSTEP_Z 90000ul //!< Specific timeout for lcd_babystep_z screen in ms.



typedef void (*lcd_longpress_func_t)(void);

typedef void (*lcd_lcdupdate_func_t)(void);

//Set to none-zero when the LCD needs to draw, decreased after every draw. Set to 2 in LCD routines so the LCD gets at least 1 full redraw (first redraw is partial)
extern uint8_t lcd_draw_update;

extern int32_t lcd_encoder;

extern uint8_t lcd_encoder_bits;

// lcd_encoder_diff is updated from interrupt context and added to lcd_encoder every LCD update
extern int8_t lcd_encoder_diff;

//the last checked lcd_buttons in a bit array.
extern uint8_t lcd_buttons;

extern uint8_t lcd_button_pressed;

extern uint8_t lcd_update_enabled;

extern LongTimer lcd_timeoutToStatus;

extern uint32_t lcd_next_update_millis;

extern uint8_t lcd_status_update_delay;

extern lcd_longpress_func_t lcd_longpress_func;

extern lcd_lcdupdate_func_t lcd_lcdupdate_func;



extern uint8_t lcd_clicked(void);

extern void lcd_beeper_quick_feedback(void);

//Cause an LCD refresh, and give the user visual or audible feedback that something has happened
extern void lcd_quick_feedback(void);

extern void lcd_update(uint8_t lcdDrawUpdateOverride);

extern void lcd_update_enable(uint8_t enabled);

extern void lcd_buttons_update(void);

extern void lcd_consume_click();

//! @brief Helper class to temporarily disable LCD updates
//!
//! When constructed (on stack), original state state of lcd_update_enabled is stored
//! and LCD updates are disabled.
//! When destroyed (gone out of scope), original state of LCD update is restored.
//! It has zero overhead compared to storing bool saved = lcd_update_enabled
//! and calling lcd_update_enable(false) and lcd_update_enable(saved).
class LcdUpdateDisabler
{
public:
    LcdUpdateDisabler(): m_updateEnabled(lcd_update_enabled)
    {
        lcd_update_enable(false);
    }
    ~LcdUpdateDisabler()
    {
        lcd_update_enable(m_updateEnabled);
    }

private:
    bool m_updateEnabled;
};


////////////////////////////////////
// Setup button and encode mappings for each panel (into 'lcd_buttons' variable
//
// This is just to map common functions (across different panels) onto the same 
// macro name. The mapping is independent of whether the button is directly connected or 
// via a shift/i2c register.

#define BLEN_B 1
#define BLEN_A 0
#define EN_B (1<<BLEN_B) // The two encoder pins are connected through BTN_EN1 and BTN_EN2
#define EN_A (1<<BLEN_A)
#define BLEN_C 2 
#define EN_C (1<<BLEN_C) 

//! @brief Was button clicked?
//!
//! Doesn't consume button click event. See lcd_clicked()
//! for function consuming the event.
//!
//! Generally is used in non-modal menus.
//!
//! @retval 0 button was not clicked
//! @retval 1 button was clicked
#define LCD_CLICKED (lcd_buttons&EN_C)

////////////////////////
// Setup Rotary Encoder Bit Values (for two pin encoders to indicate movement)
// These values are independent of which pins are used for EN_A and EN_B indications
// The rotary encoder part is also independent to the chipset used for the LCD
#define encrot0 0
#define encrot1 2
#define encrot2 3
#define encrot3 1


#endif //ULTRALCD_H
