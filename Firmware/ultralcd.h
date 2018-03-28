#ifndef ULTRALCD_H
#define ULTRALCD_H

#include "Marlin.h"
#include "mesh_bed_calibration.h"

#ifdef ULTRA_LCD

  void lcd_update(uint8_t lcdDrawUpdateOverride = 0);
  // Call with a false parameter to suppress the LCD update from various places like the planner or the temp control.
  void lcd_update_enable(bool enable);
  void lcd_init();
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
  void lcd_force_language_selection();
  void lcd_sdcard_stop();
  void lcd_sdcard_pause();
  void lcd_print_stop();
  void prusa_statistics(int _message, uint8_t _col_nr = 0);
  void lcd_confirm_print();
  unsigned char lcd_choose_color();
void lcd_mylang();
  bool lcd_detected(void);

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
  static bool lcd_selftest_fsensor();
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
  extern void lcd_bed_calibration_show_result(BedSkewOffsetDetectionResultType result, uint8_t point_too_far_mask);

  extern void lcd_diag_show_end_stops();

#ifdef DOGLCD
  extern int lcd_contrast;
  void lcd_setcontrast(uint8_t value);
#endif

  static unsigned char blink = 0;	// Variable for visualization of fan rotation in GLCD

  #define LCD_MESSAGEPGM(x) lcd_setstatuspgm(PSTR(x))
  #define LCD_ALERTMESSAGEPGM(x) lcd_setalertstatuspgm(PSTR(x))
  #define LCD_MESSAGERPGM(x) lcd_setstatuspgm((x))
  #define LCD_ALERTMESSAGERPGM(x) lcd_setalertstatuspgm((x))

  #define LCD_UPDATE_INTERVAL 100
  #define LCD_TIMEOUT_TO_STATUS 30000

  #ifdef ULTIPANEL
  void lcd_buttons_update();
  extern volatile uint8_t buttons;  //the last checked buttons in a bit array.
  #ifdef REPRAPWORLD_KEYPAD
    extern volatile uint8_t buttons_reprapworld_keypad; // to store the keypad shift register values
  #endif
  #else
  FORCE_INLINE void lcd_buttons_update() {}
  #endif


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
  extern int8_t SilentModeMenu;

#ifdef SNMM
  extern uint8_t snmm_extruder;
#endif // SNMM

  extern bool cancel_heatup;
  extern bool isPrintPaused;
  
  #ifdef FILAMENT_LCD_DISPLAY
        extern unsigned long message_millis;
  #endif
    
  void lcd_buzz(long duration,uint16_t freq);
  bool lcd_clicked();

  void lcd_ignore_click(bool b=true);
  void lcd_commands();
  
  #ifdef NEWPANEL
    #define EN_C (1<<BLEN_C)
    #define EN_B (1<<BLEN_B)
    #define EN_A (1<<BLEN_A)

    #define LCD_CLICKED (buttons&EN_C)
    #ifdef REPRAPWORLD_KEYPAD
  	  #define EN_REPRAPWORLD_KEYPAD_F3 (1<<BLEN_REPRAPWORLD_KEYPAD_F3)
  	  #define EN_REPRAPWORLD_KEYPAD_F2 (1<<BLEN_REPRAPWORLD_KEYPAD_F2)
  	  #define EN_REPRAPWORLD_KEYPAD_F1 (1<<BLEN_REPRAPWORLD_KEYPAD_F1)
  	  #define EN_REPRAPWORLD_KEYPAD_UP (1<<BLEN_REPRAPWORLD_KEYPAD_UP)
  	  #define EN_REPRAPWORLD_KEYPAD_RIGHT (1<<BLEN_REPRAPWORLD_KEYPAD_RIGHT)
  	  #define EN_REPRAPWORLD_KEYPAD_MIDDLE (1<<BLEN_REPRAPWORLD_KEYPAD_MIDDLE)
  	  #define EN_REPRAPWORLD_KEYPAD_DOWN (1<<BLEN_REPRAPWORLD_KEYPAD_DOWN)
  	  #define EN_REPRAPWORLD_KEYPAD_LEFT (1<<BLEN_REPRAPWORLD_KEYPAD_LEFT)

  	  #define LCD_CLICKED ((buttons&EN_C) || (buttons_reprapworld_keypad&EN_REPRAPWORLD_KEYPAD_F1))
  	  #define REPRAPWORLD_KEYPAD_MOVE_Z_UP (buttons_reprapworld_keypad&EN_REPRAPWORLD_KEYPAD_F2)
  	  #define REPRAPWORLD_KEYPAD_MOVE_Z_DOWN (buttons_reprapworld_keypad&EN_REPRAPWORLD_KEYPAD_F3)
  	  #define REPRAPWORLD_KEYPAD_MOVE_X_LEFT (buttons_reprapworld_keypad&EN_REPRAPWORLD_KEYPAD_LEFT)
  	  #define REPRAPWORLD_KEYPAD_MOVE_X_RIGHT (buttons_reprapworld_keypad&EN_REPRAPWORLD_KEYPAD_RIGHT)
  	  #define REPRAPWORLD_KEYPAD_MOVE_Y_DOWN (buttons_reprapworld_keypad&EN_REPRAPWORLD_KEYPAD_DOWN)
  	  #define REPRAPWORLD_KEYPAD_MOVE_Y_UP (buttons_reprapworld_keypad&EN_REPRAPWORLD_KEYPAD_UP)
  	  #define REPRAPWORLD_KEYPAD_MOVE_HOME (buttons_reprapworld_keypad&EN_REPRAPWORLD_KEYPAD_MIDDLE)
    #endif //REPRAPWORLD_KEYPAD
  #else
    //atomic, do not change
    #define B_LE (1<<BL_LE)
    #define B_UP (1<<BL_UP)
    #define B_MI (1<<BL_MI)
    #define B_DW (1<<BL_DW)
    #define B_RI (1<<BL_RI)
    #define B_ST (1<<BL_ST)
    #define EN_B (1<<BLEN_B)
    #define EN_A (1<<BLEN_A)
    
    #define LCD_CLICKED ((buttons&B_MI)||(buttons&B_ST))
  #endif//NEWPANEL

#else //no LCD
  FORCE_INLINE void 
  {}
  FORCE_INLINE void lcd_init() {}
  FORCE_INLINE void lcd_setstatus(const char* message) {}
  FORCE_INLINE void lcd_buttons_update() {}
  FORCE_INLINE void lcd_reset_alert_level() {}
  FORCE_INLINE void lcd_buzz(long duration,uint16_t freq) {}
  FORCE_INLINE bool lcd_detected(void) { return true; }

  #define LCD_MESSAGEPGM(x) 
  #define LCD_ALERTMESSAGEPGM(x) 

#endif //ULTRA_LCD

char *itostr2(const uint8_t &x);
char *itostr31(const int &xx);
char *itostr3(const int &xx);
char *itostr3left(const int &xx);
char *itostr4(const int &xx);

char *ftostr3(const float &x);
char *ftostr31ns(const float &x); // float to string without sign character
char *ftostr31(const float &x);
char *ftostr32(const float &x);
char *ftostr32ns(const float &x);
char *ftostr43(const float &x, uint8_t offset = 0);
char *ftostr12ns(const float &x);
char *ftostr13ns(const float &x);
char *ftostr32sp(const float &x); // remove zero-padding from ftostr32
char *ftostr5(const float &x);
char *ftostr51(const float &x);
char *ftostr52(const float &x);


extern void lcd_implementation_clear();
extern void lcd_printPGM(const char* str);
extern void lcd_print_at_PGM(uint8_t x, uint8_t y, const char* str);
extern void lcd_implementation_write(char c);
extern void lcd_implementation_print(const char *str);
extern void lcd_implementation_print(int8_t i);
extern void lcd_implementation_print_at(uint8_t x, uint8_t y, int8_t i);
extern void lcd_implementation_print(int i);
extern void lcd_implementation_print_at(uint8_t x, uint8_t y, int i);
extern void lcd_implementation_print(float f);
extern void lcd_implementation_print_at(uint8_t x, uint8_t y, const char *str);


void change_extr(int extr);
static void lcd_colorprint_change();
static int get_ext_nr();
static void extr_adj(int extruder);
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
static void lcd_ping_allert();
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

union MenuData;

void bowden_menu();
char reset_menu();
char choose_extruder_menu();

void lcd_pinda_calibration_menu();
void lcd_calibrate_pinda();
void lcd_temp_calibration_set();

void display_loading();

void lcd_service_mode_show_result();

#if !SDSORT_USES_RAM
 void lcd_set_degree();
 void lcd_set_progress();
#endif

void lcd_wizard();
void lcd_wizard(int state);

static void lcd_send_status();
static void lcd_connect_printer();

#endif //ULTRALCD_H
