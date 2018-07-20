#ifndef ULTRALCD_H
#define ULTRALCD_H

#include "Marlin.h"
#include "lcd.h"
#include "conv2str.h"

extern int lcd_puts_P(const char* str);
extern int lcd_printf_P(const char* format, ...);

extern void menu_lcd_longpress_func(void);
extern void menu_lcd_charsetup_func(void);
extern void menu_lcd_lcdupdate_func(void);

	static void lcd_language_menu();

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

  static void lcd_selftest_v();
  extern bool lcd_selftest();
  static bool lcd_selfcheck_endstops();

#ifdef TMC2130
  static void reset_crash_det(char axis);
  static bool lcd_selfcheck_axis_sg(char axis);
  static bool lcd_selfcheck_axis(int _axis, int _travel);
#else
  static bool lcd_selfcheck_endstops();
  static bool lcd_selfcheck_axis(int _axis, int _travel);
  static bool lcd_selfcheck_pulleys(int axis);
#endif //TMC2130

  static bool lcd_selfcheck_check_heater(bool _isbed);
  static int  lcd_selftest_screen(int _step, int _progress, int _progress_scale, bool _clear, int _delay);
  static void lcd_selftest_screen_step(int _row, int _col, int _state, const char *_name, const char *_indicator);
  static bool lcd_selftest_manual_fan_check(int _fan, bool check_opposite);
  static bool lcd_selftest_fan_dialog(int _fan);
  static bool lcd_selftest_fsensor(void);
  static void lcd_selftest_error(int _error_no, const char *_error_1, const char *_error_2);
  void lcd_menu_statistics(); 

  extern const char* lcd_display_message_fullscreen_P(const char *msg, uint8_t &nlines);
  inline const char* lcd_display_message_fullscreen_P(const char *msg) 
    { uint8_t nlines; return lcd_display_message_fullscreen_P(msg, nlines); }

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
  extern void lcd_bed_calibration_show_result(uint8_t result, uint8_t point_too_far_mask);

  extern void lcd_diag_show_end_stops();


  static unsigned char blink = 0;	// Variable for visualization of fan rotation in GLCD

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

  extern unsigned long lcd_timeoutToStatus;
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

#if defined (SNMM) || defined (SNMM_V2) 
  extern uint8_t snmm_extruder;
#endif // defined (SNMM) || defined (SNMM_V2)

  extern bool cancel_heatup;
  extern bool isPrintPaused;
      

  void lcd_ignore_click(bool b=true);
  void lcd_commands();
  


void change_extr(int extr);
static void lcd_colorprint_change();
static int get_ext_nr();
void extr_adj(int extruder);
static void extr_adj_0();
static void extr_adj_1();
static void extr_adj_2();
static void extr_adj_3();
static void fil_load_menu();
static void fil_unload_menu();
static void extr_unload_0();
static void extr_unload_1();
static void extr_unload_2();
static void extr_unload_3();
static void lcd_disable_farm_mode();
static void lcd_set_fan_check();
void extr_unload_all(); 
void extr_unload_used();
void extr_unload();
static char snmm_stop_print_menu();
#ifdef SDCARD_SORT_ALPHA
 static void lcd_sort_type_set();
#endif
static float count_e(float layer_heigth, float extrusion_width, float extrusion_length);
static void lcd_babystep_z();

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

static void lcd_send_status();
static void lcd_connect_printer();

#endif //ULTRALCD_H
