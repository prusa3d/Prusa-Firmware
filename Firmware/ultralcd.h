#ifndef ULTRALCD_H
#define ULTRALCD_H

#include "Marlin.h"
#include "lcd.h"
#include "conv2str.h"
#include "menu.h"
#include "mesh_bed_calibration.h"

extern int lcd_puts_P(const char* str);
extern int lcd_printf_P(const char* format, ...);

extern void menu_lcd_longpress_func(void);
extern void menu_lcd_charsetup_func(void);
extern void menu_lcd_lcdupdate_func(void);

struct EditMenuParentState
{
    //prevMenu and prevEncoderPosition are used to store the previous menu location when editing settings.
    menu_func_t prevMenu;
    uint16_t prevEncoderPosition;
    //Variables used when editing values.
    const char* editLabel;
    void* editValue;
    int32_t minEditValue, maxEditValue;
    // menu_func_t callbackFunc;
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
        //ShortTimer timer;
        char dummy;
    } autoLoadFilamentMenu;
    struct _Lcd_moveMenu
    {
        bool initialized;
        bool endstopsEnabledPrevious;
    } _lcd_moveMenu;
    struct sdcard_menu_t
    {
        uint8_t viewState;
    } sdcard_menu;
    menu_data_edit_t edit_menu;
};

// State of the currently active menu.
// C Union manages sharing of the static memory by all the menus.
extern union MenuData menuData;


  // Call with a false parameter to suppress the LCD update from various places like the planner or the temp control.
  void ultralcd_init();
  void lcd_setstatus(const char* message);
  void lcd_setstatuspgm(const char* message);
  void lcd_setalertstatuspgm(const char* message);
  void lcd_reset_alert_level();
  uint8_t get_message_level();
  void lcd_adjust_z();
  void lcd_pick_babystep();
  void lcd_alright();
  void EEPROM_save_B(int pos, int* value);
  void EEPROM_read_B(int pos, int* value);
  void lcd_wait_interact();
  void lcd_change_filament();
  void lcd_loading_filament();
  void lcd_change_success();
  void lcd_loading_color();
  void lcd_sdcard_stop();
  void lcd_sdcard_pause();
  void lcd_print_stop();
  void prusa_statistics(int _message, uint8_t _col_nr = 0);
  void lcd_confirm_print();
  unsigned char lcd_choose_color();
//void lcd_mylang();

  extern bool lcd_selftest();

  void lcd_menu_statistics(); 

extern const char* lcd_display_message_fullscreen_P(const char *msg, uint8_t &nlines);
extern const char* lcd_display_message_fullscreen_P(const char *msg);

  extern void lcd_wait_for_click();
  extern void lcd_show_fullscreen_message_and_wait_P(const char *msg);
  // 0: no, 1: yes, -1: timeouted
  extern int8_t lcd_show_fullscreen_message_yes_no_and_wait_P(const char *msg, bool allow_timeouting = true, bool default_yes = false);
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
  #define LCD_COMMAND_IDLE 0
  #define LCD_COMMAND_LOAD_FILAMENT 1
  #define LCD_COMMAND_STOP_PRINT 2
  #define LCD_COMMAND_FARM_MODE_CONFIRM 4
  #define LCD_COMMAND_LONG_PAUSE 5
  #define LCD_COMMAND_LONG_PAUSE_RESUME 6
  #define LCD_COMMAND_PID_EXTRUDER 7 
  #define LCD_COMMAND_V2_CAL 8

  extern int lcd_commands_type;
  
  extern uint8_t farm_mode;
  extern int farm_no;
  extern int farm_timer;
  extern int farm_status;
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

  
  extern bool cancel_heatup;
  extern bool isPrintPaused;
      

  void lcd_ignore_click(bool b=true);
  void lcd_commands();
  


void change_extr(int extr);
void extr_adj(int extruder);

void extr_unload_all(); 
void extr_unload_used();
void extr_unload();

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
void adjust_bed_reset();
void lcd_extr_cal_reset();

void lcd_temp_cal_show_result(bool result);
bool lcd_wait_for_pinda(float temp);


union MenuData;

void bowden_menu();
char reset_menu();
char choose_extruder_menu();

void lcd_pinda_calibration_menu();
void lcd_calibrate_pinda();
void lcd_temp_calibration_set();

void display_loading();

#if !SDSORT_USES_RAM
 void lcd_set_degree();
 void lcd_set_progress();
#endif

void lcd_language();

void lcd_wizard();
void lcd_wizard(int state);

#endif //ULTRALCD_H
