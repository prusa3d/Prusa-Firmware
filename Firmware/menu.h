//menu.h
#ifndef _MENU_H
#define _MENU_H

#include <inttypes.h>

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
    void* editValue;
    int32_t minEditValue;
    int32_t maxEditValue;
} menu_data_edit_t;

extern uint8_t menu_data[MENU_DATA_SIZE];

extern uint8_t menu_depth;

extern uint8_t menu_line;
extern uint8_t menu_item;
extern uint8_t menu_row;

//scroll offset in the current menu
extern uint8_t menu_top;

extern uint8_t menu_clicked;

extern uint8_t menu_entering;
extern uint8_t menu_leaving;

//function pointer to the currently active menu
extern menu_func_t menu_menu;



extern void menu_goto(menu_func_t menu, const uint32_t encoder, const bool feedback, bool reset_menu_state);

#define MENU_BEGIN() menu_start(); for(menu_row = 0; menu_row < LCD_HEIGHT; menu_row++, menu_line++) { menu_item = 0;
void menu_start(void);

#define MENU_END() menu_end(); }
extern void menu_end(void);

extern void menu_back(void);
extern void menu_back(uint8_t nLevel);

extern void menu_back_if_clicked(void);

extern void menu_back_if_clicked_fb(void);

extern void menu_submenu(menu_func_t submenu);

extern uint8_t menu_item_ret(void);

//extern int menu_draw_item_printf_P(char type_char, const char* format, ...);


//int menu_draw_item_puts_P_int16(char type_char, const char* str, int16_t val, );

#define MENU_ITEM_DUMMY() menu_item_dummy()
extern void menu_item_dummy(void);

#define MENU_ITEM_TEXT_P(str) do { if (menu_item_text_P(str)) return; } while (0)
extern uint8_t menu_item_text_P(const char* str);

#define MENU_ITEM_SUBMENU_P(str, submenu) do { if (menu_item_submenu_P(str, submenu)) return; } while (0)
extern uint8_t menu_item_submenu_P(const char* str, menu_func_t submenu);

#define MENU_ITEM_BACK_P(str) do { if (menu_item_back_P(str)) return; } while (0)
extern uint8_t menu_item_back_P(const char* str);

#define MENU_ITEM_FUNCTION_P(str, func) do { if (menu_item_function_P(str, func)) return; } while (0)
extern uint8_t menu_item_function_P(const char* str, menu_func_t func);

#define MENU_ITEM_GCODE_P(str, str_gcode) do { if (menu_item_gcode_P(str, str_gcode)) return; } while (0)
extern uint8_t menu_item_gcode_P(const char* str, const char* str_gcode);


extern const char menu_fmt_int3[];

extern const char menu_fmt_float31[];


extern void menu_draw_float31(char chr, const char* str, float val);

extern void menu_draw_float13(char chr, const char* str, float val);


#define MENU_ITEM_EDIT_int3_P(str, pval, minval, maxval) do { if (menu_item_edit_P(str, pval, minval, maxval)) return; } while (0)
//#define MENU_ITEM_EDIT_int3_P(str, pval, minval, maxval) MENU_ITEM_EDIT(int3, str, pval, minval, maxval)
template <typename T>
extern uint8_t menu_item_edit_P(const char* str, T pval, int16_t min_val, int16_t max_val);


#endif //_MENU_H
