//menu.h
#ifndef _MENU_H
#define _MENU_H

#include <inttypes.h>
#include "eeprom.h"

#define MENU_DATA_SIZE      32

//Function pointer to menu functions.
typedef void (*menu_func_t)(void);

typedef struct 
{
    menu_func_t menu;
    int8_t position;
} menu_record_t;

typedef struct
{
    //Variables used when editing values.
    const char* editLabel;
    void* editValue; // Pointer to variable which the menu will modify when knob is clicked
    int16_t currentValue; // current value shown on the LCD. Value is not saved until the knob is clicked
    int16_t minEditValue; // Constant set by menu
    int16_t maxEditValue; // Constant set by menu
    uint8_t decimals; // denotes number of decimals places when editing floats
} menu_data_edit_t;

extern uint8_t menu_data[MENU_DATA_SIZE];

extern uint8_t menu_depth;

//! definition of reasons blocking the main menu
//! Use them as bit mask, so that the code may set various errors at the same time
enum ESeriousErrors {
	MENU_BLOCK_NONE                = 0,
	MENU_BLOCK_THERMAL_ERROR       = 0x01,
#ifdef TEMP_MODEL
	MENU_BLOCK_TEMP_MODEL_AUTOTUNE = 0x02,
#endif
}; // and possibly others in the future.

//! this is a flag for disabling entering the main menu and longpress. If this is set to anything !=
//! 0, the only the main status screen will be shown on the LCD and the user will be prevented from
//! entering the menu.
extern uint8_t menu_block_mask;

//! a pair of macros for manipulating menu entry
//! a c++ class would have been better
#define menu_set_block(x) menu_block_mask |= x;
#define menu_unset_block(x) menu_block_mask &= ~x;
#define menu_is_blocked(x) (menu_block_mask & x) != 0

extern uint8_t menu_line;
extern uint8_t menu_item;
extern uint8_t menu_row;

//scroll offset in the current menu
extern uint8_t menu_top;
extern uint8_t menu_clicked;
extern uint8_t menu_leaving;

//function pointer to the currently active menu
extern menu_func_t menu_menu;

extern void menu_data_reset(void);

/// @brief Go to a specific LCD menu
/// @param menu pointer to function which runs the menu. If equal to the currently running menu, nothing happens
/// @param encoder starting menu row position, ranges from 0 to 127
/// @param feedback if true, a feedback sound is played when the menu changes
/// @param reset_menu_state if true, any background menu data is erased
void menu_goto(menu_func_t menu, const int8_t encoder, const bool feedback, bool reset_menu_state);

#define MENU_BEGIN() menu_start(); for(menu_row = 0; menu_row < LCD_HEIGHT; menu_row++, menu_line++) { menu_item = 0;
void menu_start(void);

#define MENU_END() menu_end(); }
extern void menu_end(void);

extern void menu_back(void);
extern void menu_back_no_reset(void);
extern void menu_back(uint8_t nLevel);

extern void menu_back_if_clicked(void);

extern void menu_back_if_clicked_fb(void);

extern void menu_submenu(menu_func_t submenu);
extern void menu_submenu_no_reset(menu_func_t submenu);

extern uint8_t menu_item_ret(void);

//extern int menu_draw_item_printf_P(char type_char, const char* format, ...);


//int menu_draw_item_puts_P_int16(char type_char, const char* str, int16_t val, );

#define MENU_ITEM_DUMMY() menu_item_dummy()
extern void menu_item_dummy(void);

#define MENU_ITEM_TEXT_P(str) do { if (menu_item_text_P(str)) return; } while (0)
extern uint8_t menu_item_text_P(const char* str);

#define MENU_ITEM_SUBMENU_P(str, submenu) do { if (menu_item_submenu_P(str, submenu)) return; } while (0)
extern uint8_t menu_item_submenu_P(const char* str, menu_func_t submenu);

#define MENU_ITEM_SUBMENU_E(sheet, submenu) do { if (menu_item_submenu_E(sheet, submenu)) return; } while (0)
extern uint8_t menu_item_submenu_E(const Sheet &sheet, menu_func_t submenu);

#define MENU_ITEM_FUNCTION_E(sheet, submenu) do { if (menu_item_function_E(sheet, submenu)) return; } while (0)
extern uint8_t menu_item_function_E(const Sheet &sheet, menu_func_t func);

#define MENU_ITEM_BACK_P(str) do { if (menu_item_back_P(str)) return; } while (0)
extern uint8_t menu_item_back_P(const char* str);

// leaving menu - this condition must be immediately before MENU_ITEM_BACK_P
#define ON_MENU_LEAVE(func) do { if (menu_item_leave()){ func } } while (0)
extern bool menu_item_leave();

#define MENU_ITEM_FUNCTION_P(str, func) do { if (menu_item_function_P(str, func)) return; } while (0)
extern uint8_t menu_item_function_P(const char* str, menu_func_t func);

#define MENU_ITEM_FUNCTION_NR_P(str, number, func, fn_par) do { if (menu_item_function_P(str, number, func, fn_par)) return; } while (0)
extern uint8_t menu_item_function_P(const char* str, char number, void (*func)(uint8_t), uint8_t fn_par);

#define MENU_ITEM_TOGGLE_P(str, toggle, func) do { if (menu_item_toggle_P(str, toggle, func, 0x02)) return; } while (0)
#define MENU_ITEM_TOGGLE(str, toggle, func) do { if (menu_item_toggle_P(str, toggle, func, 0x00)) return; } while (0)
extern uint8_t menu_item_toggle_P(const char* str, const char* toggle, menu_func_t func, const uint8_t settings);

#define MENU_ITEM_GCODE_P(str, str_gcode) do { if (menu_item_gcode_P(str, str_gcode)) return; } while (0)
extern uint8_t menu_item_gcode_P(const char* str, const char* str_gcode);


extern const char menu_fmt_int3[];

extern const char menu_fmt_float31[];
extern const char menu_fmt_float13[];

extern void menu_draw_float31(const char* str, float val);

extern void menu_draw_float13(const char* str, float val);

struct SheetFormatBuffer
{
    char c[19];
};

extern void menu_format_sheet_E(const Sheet &sheet_E, SheetFormatBuffer &buffer);

static const constexpr int16_t TEMPERATURE_NOZZLE_EDIT_MENU[] PROGMEM = {
    0, // Minimum value
    HEATER_0_MAXTEMP - 10, // Maximum value
    LCD_JUMP_HOTEND_TEMP // Jump value
};

static const constexpr int16_t TEMPERATURE_BED_EDIT_MENU[] PROGMEM = {
    0, // Minimum value
    BED_MAXTEMP - 3, // Maximum value
    LCD_JUMP_BED_TEMP // Jump value
};

static const constexpr int16_t SPEED_EDIT_MENU[] PROGMEM = {
    10, // Minimum value
    999, // Maximum value
    0 // Jump value
};

static const constexpr int16_t FLOW_EDIT_MENU[] PROGMEM = {
    10, // Minimum value
    999, // Maximum value
    0 // Jump value
};

static const constexpr int16_t BACKLIGHT_TIMER_EDIT_MENU[] PROGMEM = {
    1, // Minimum value
    999, // Maximum value
    0 // Jump value
};

#ifdef LA_LIVE_K
static const constexpr int16_t ADJUST_K_EDIT_MENU[] PROGMEM = {
    1, // Minimum value
    999, // Maximum value
    0 // Jump value
};
#endif // LA_LIVE_K

static const constexpr int8_t ADJUST_BED_OFFSET_EDIT_MENU[] PROGMEM = {
    (int8_t)-BED_ADJUSTMENT_UM_MAX, // Minimum value
    (int8_t)BED_ADJUSTMENT_UM_MAX, // Maximum value
    0 // Jump value
};

#ifdef TMC2130
#include "tmc2130.h"
static const constexpr uint8_t TMC_LINEARITY_CORRECTION_EDIT_MENU[] PROGMEM = {
    (uint8_t)(TMC2130_WAVE_FAC1000_MIN-TMC2130_WAVE_FAC1000_STP), // Minimum value
    (uint8_t)(TMC2130_WAVE_FAC1000_MAX), // Maximum value
    0 // Jump value
};
#endif //TMC2130

static const constexpr uint8_t PRINT_FAN_EDIT_MENU[] PROGMEM = {
    0,   // Minimum value
    255, // Maximum value
    LCD_JUMP_FAN_SPEED    // Jump value
};

// The backlight menu has limits which can change at runtime. So we
// cannot save them in PROGMEM. In this case gather the data and 
// place them in an array which the menu_item_edit function understands
#define INIT_ARR(VAR_NAME, ...) uint8_t VAR_NAME[] = {__VA_ARGS__}
#define MENU_ITEM_EDIT_MANUAL_P(str, pval, minval, maxval, jumpval) \
do { \
    INIT_ARR(buf, minval, maxval, jumpval); \
    MENU_ITEM_EDIT_ISPROGMEM_P(str, pval, buf, 0); \
} while (0) \

// NOTE: due to memory restrictions, we're limited to use int16_t for minval and maxval
//       especially when floats are used for pval
template <typename T, typename D>
uint8_t menu_item_edit_P(const char* str, T* const pval, const D* settings, const uint8_t is_progmem = 1, const uint8_t decimals = 3);
#define MENU_ITEM_EDIT_P(str, pval, settings) do { if (menu_item_edit_P(str, pval, settings)) return; } while (0)

// Variant of MENU_ITEM_EDIT_P which gives control over the optional progmem argument
#define MENU_ITEM_EDIT_ISPROGMEM_P(str, pval, settings, is_progmem) do { if (menu_item_edit_P(str, pval, settings, is_progmem)) return; } while (0)

// Variant of MENU_ITEM_EDIT_P whicch gives control of number of decimal places in pval. Only intended for float.
#define MENU_ITEM_EDIT_FLOAT_P(str, pval, settings, decimals) do { if (menu_item_edit_P(str, pval, settings, (uint8_t)decimals)) return; } while (0)

extern void menu_progressbar_init(uint16_t total, const char* title);
extern void menu_progressbar_update(uint16_t newVal);
extern void menu_progressbar_finish(void);

#endif //_MENU_H
