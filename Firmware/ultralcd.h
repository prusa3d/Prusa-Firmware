#ifndef ULTRALCD_H
#define ULTRALCD_H

#include "mesh_bed_calibration.h"
#include "config.h"

extern void menu_lcd_longpress_func(void);
extern void menu_lcd_lcdupdate_func(void);

// Call with a false parameter to suppress the LCD update from various places like the planner or the temp control.
void ultralcd_init();

//! LCD status severities
#define LCD_STATUS_CRITICAL 3 //< Heater failure
#define LCD_STATUS_ALERT    2 //< Other hardware issue
#define LCD_STATUS_INFO     1 //< Message times out after a while
#define LCD_STATUS_NONE     0 //< No alert message set

#define LCD_STATUS_INFO_TIMEOUT 20000

// Set the current status message (equivalent to LCD_STATUS_NONE)
void lcd_setstatus(const char* message);
void lcd_setstatuspgm(const char* message);
void lcd_setstatus_serial(const char* message);

//! return to the main status screen and display the alert message
//! Beware - it has sideeffects:
//! - always returns the display to the main status screen
//! - always makes lcd_reset (which is slow and causes flicker)
//! - does not update the message if there is one with the same (or higher) severity present
void lcd_setalertstatus(const char* message, uint8_t severity = LCD_STATUS_ALERT);
void lcd_setalertstatuspgm(const char* message, uint8_t severity = LCD_STATUS_ALERT);

//! Get/reset the current alert level
uint8_t get_message_level();
void lcd_reset_alert_level();

void lcd_adjust_z();
void lcd_pick_babystep();
void lcd_alright();
void show_preheat_nozzle_warning();
void lcd_wait_interact();
void lcd_loading_filament();
void lcd_change_success();
void lcd_loading_color();
void lcd_sdcard_stop();
void lcd_pause_print();
void lcd_pause_usb_print();
void lcd_resume_print();
void lcd_print_stop(); // interactive print stop
void print_stop(bool interactive=false);
#ifdef TEMP_MODEL
void lcd_temp_model_cal();
#endif //TEMP_MODEL
void lcd_load_filament_color_check();

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

enum LCDButtonChoice : int_fast8_t {
    LCD_LEFT_BUTTON_CHOICE = 1,
    LCD_MIDDLE_BUTTON_CHOICE = 0,
    LCD_BUTTON_TIMEOUT      = -1,
};

extern const char* lcd_display_message_fullscreen_P(const char *msg);

extern void lcd_return_to_status();
extern void lcd_wait_for_click();
extern bool lcd_wait_for_click_delay(uint16_t nDelay);
extern void lcd_show_fullscreen_message_and_wait_P(const char *msg);
// 0: no, 1: yes, -1: timeouted
extern int8_t lcd_show_yes_no_and_wait(bool allow_timeouting = true, bool default_yes = false);
// 0: no, 1: yes, -1: timeouted
extern int8_t lcd_show_fullscreen_message_yes_no_and_wait_P(const char *msg, bool allow_timeouting = true, bool default_yes = false);
extern int8_t lcd_show_multiscreen_message_two_choices_and_wait_P(const char *msg, bool allow_timeouting, bool default_yes,
        const char *first_choice, const char *second_choice, uint8_t second_col = 7);
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
#ifdef TEMP_MODEL
    TempModel,
#endif //TEMP_MODEL
    NozzleCNG,
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
    M0Wait,          //!< M0/M1 Wait command working even from SD
    M117,            //!< M117 Set the status line message on the LCD
    Resuming,        //!< Resuming message
};

extern CustomMsg custom_message_type;
extern uint8_t custom_message_state;

extern bool UserECoolEnabled();
extern bool FarmOrUserECool();

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

extern uint8_t scrollstuff;


void lcd_ignore_click(bool b=true);
void lcd_commands();


extern bool bSettings;                            // flag (i.e. 'fake parameter') for 'lcd_hw_setup_menu()' function
void lcd_hw_setup_menu(void);                     // NOT static due to using inside "util" module ("nozzle_diameter_check()")

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
extern bool bFilamentPreheatState;
extern bool bFilamentAction;
void mFilamentItem(uint16_t nTemp,uint16_t nTempBed);
void mFilamentItemForce();
void lcd_generic_preheat_menu();
void unload_filament(bool automatic = false);


void lcd_wait_for_heater();
void lcd_wait_for_cool_down();
void lcd_move_e(); // NOT static due to usage in Marlin_main

void lcd_temp_cal_show_result(bool result);
#ifdef PINDA_THERMISTOR
bool lcd_wait_for_pinda(float temp);
#endif //PINDA_THERMISTOR


char reset_menu();
uint8_t choose_menu_P(const char *header, const char *item, const char *last_item = nullptr);

void lcd_calibrate_pinda();
void lcd_temp_calibration_set();

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
#ifdef TEMP_MODEL
    TempModel,      //!< Temp model calibration
#endif //TEMP_MODEL
    IsFil,          //!< Is filament loaded? First step of 1st layer calibration
    Preheat,        //!< Preheat for any material
    LoadFilCold,    //!< Load filament for MMU
    LoadFilHot,     //!< Load filament without MMU
    Lay1CalCold,    //!< First layer calibration, temperature not selected yet
    Lay1CalHot,     //!< First layer calibration, temperature already selected
    RepeatLay1Cal,  //!< Repeat first layer calibration?
    Finish,         //!< Deactivate wizard
};

void lcd_wizard(WizState state);

extern void lcd_experimental_toggle();
extern void lcd_experimental_menu();

#ifdef PINDA_TEMP_COMP
extern void lcd_pinda_temp_compensation_toggle();
#endif //PINDA_TEMP_COMP

#endif //ULTRALCD_H
