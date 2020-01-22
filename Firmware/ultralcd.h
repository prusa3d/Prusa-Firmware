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
extern void menu_lcd_charsetup_func(void);
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
void lcd_setalertstatuspgm(const char* message);
//! only update the alert message on the main status screen
//! has no sideeffects, may be called multiple times
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

#if !SDSORT_USES_RAM
void lcd_set_degree();
void lcd_set_progress();
#endif

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

#define VOLT_DIV_REF 5
#if IR_SENSOR_ANALOG
#define IRsensor_Hmin_TRESHOLD (3.0*1023*OVERSAMPLENR/VOLT_DIV_REF) // ~3.0V (0.6*Vcc)
#define IRsensor_Lmax_TRESHOLD (1.5*1023*OVERSAMPLENR/VOLT_DIV_REF) // ~1.5V (0.3*Vcc)
#define IRsensor_Hopen_TRESHOLD (4.6*1023*OVERSAMPLENR/VOLT_DIV_REF) // ~4.6V (N.C. @ Ru~20-50k, Rd'=56k, Ru'=10k)
#define IRsensor_Ldiode_TRESHOLD (0.3*1023*OVERSAMPLENR/VOLT_DIV_REF) // ~0.3V
#endif //IR_SENSOR_ANALOG

#endif //ULTRALCD_H
