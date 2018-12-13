//! @file

#include "temperature.h"
#include "ultralcd.h"
#include "fsensor.h"
#include "Marlin.h"
#include "language.h"
#include "cardreader.h"
#include "temperature.h"
#include "stepper.h"
#include "ConfigurationStore.h"
#include <string.h>


#include "lcd.h"
#include "menu.h"

#include "util.h"
#include "mesh_bed_leveling.h"
#include "mesh_bed_calibration.h"

//#include "Configuration.h"
#include "cmdqueue.h"

#include "SdFatUtil.h"

#ifdef FILAMENT_SENSOR
#include "pat9125.h"
#include "fsensor.h"
#endif //FILAMENT_SENSOR

#ifdef TMC2130
#include "tmc2130.h"
#endif //TMC2130

#include "sound.h"

#include "mmu.h"

#include "static_assert.h"


extern bool fans_check_enabled;


int scrollstuff = 0;
char longFilenameOLD[LONG_FILENAME_LENGTH];


static void lcd_sd_updir();

int8_t ReInitLCD = 0;


int8_t SilentModeMenu = SILENT_MODE_OFF;

int8_t FSensorStateMenu = 1;

int8_t CrashDetectMenu = 1;


extern bool fsensor_enable();
extern void fsensor_disable();

#ifdef TMC2130
extern void crashdet_enable();
extern void crashdet_disable();
#endif //TMC2130


#ifdef SDCARD_SORT_ALPHA
bool presort_flag = false;
#endif

int lcd_commands_type = LCD_COMMAND_IDLE;
int lcd_commands_step = 0;

unsigned int custom_message_type = CUSTOM_MSG_TYPE_STATUS;
unsigned int custom_message_state = 0;


bool isPrintPaused = false;
uint8_t farm_mode = 0;
int farm_no = 0;
int farm_timer = 8;
int farm_status = 0;
bool printer_connected = true;

unsigned long display_time; //just timer for showing pid finished message on lcd;
float pid_temp = DEFAULT_PID_TEMP;

static bool forceMenuExpire = false;
static bool lcd_autoDeplete;


static float manual_feedrate[] = MANUAL_FEEDRATE;

/* !Configuration settings */

uint8_t lcd_status_message_level;
char lcd_status_message[LCD_WIDTH + 1] = ""; //////WELCOME!
unsigned char firstrun = 1;


static const char separator[] PROGMEM = "--------------------";

/** forward declarations **/

static const char* lcd_display_message_fullscreen_nonBlocking_P(const char *msg, uint8_t &nlines);
// void copy_and_scalePID_i();
// void copy_and_scalePID_d();

/* Different menus */
static void lcd_status_screen();
static void lcd_language_menu();
static void lcd_main_menu();
static void lcd_tune_menu();
//static void lcd_move_menu();
static void lcd_settings_menu();
static void lcd_calibration_menu();
#ifdef LINEARITY_CORRECTION
static void lcd_settings_menu_back();
#endif //LINEARITY_CORRECTION
static void lcd_control_temperature_menu();
static void lcd_control_temperature_preheat_pla_settings_menu();
static void lcd_control_temperature_preheat_abs_settings_menu();
static void lcd_control_motion_menu();
static void lcd_control_volumetric_menu();
static void lcd_settings_linearity_correction_menu_save();
static void prusa_stat_printerstatus(int _status);
static void prusa_stat_farm_number();
static void prusa_stat_temperatures();
static void prusa_stat_printinfo();
static void lcd_farm_no();
static void lcd_menu_extruder_info();
static void lcd_menu_xyz_y_min();
static void lcd_menu_xyz_skew();
static void lcd_menu_xyz_offset();
#if defined(TMC2130) || defined(FILAMENT_SENSOR)
static void lcd_menu_fails_stats();
#endif //TMC2130 or FILAMENT_SENSOR

static void lcd_selftest_v();

#ifdef TMC2130
static void reset_crash_det(unsigned char axis);
static bool lcd_selfcheck_axis_sg(unsigned char axis);
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
static void lcd_colorprint_change();
#ifdef SNMM
static int get_ext_nr();
#endif //SNMM
#if defined (SNMM) || defined(SNMM_V2)
static void fil_load_menu();
static void fil_unload_menu();
#endif // SNMM || SNMM_V2
static void lcd_disable_farm_mode();
static void lcd_set_fan_check();
static char snmm_stop_print_menu();
#ifdef SDCARD_SORT_ALPHA
 static void lcd_sort_type_set();
#endif
static float count_e(float layer_heigth, float extrusion_width, float extrusion_length);
static void lcd_babystep_z();
static void lcd_send_status();
#ifdef FARM_CONNECT_MESSAGE
static void lcd_connect_printer();
#endif //FARM_CONNECT_MESSAGE

void lcd_finishstatus();

static void lcd_sdcard_menu();

#ifdef DELTA_CALIBRATION_MENU
static void lcd_delta_calibrate_menu();
#endif // DELTA_CALIBRATION_MENU


/* Different types of actions that can be used in menu items. */
static void menu_action_sdfile(const char* filename);
static void menu_action_sddirectory(const char* filename);

#define ENCODER_FEEDRATE_DEADZONE 10


/*
#define MENU_ITEM(type, label, args...) do { \
    if (menu_item == menu_line) { \
      if (lcd_draw_update) { \
        const char* _label_pstr = (label); \
        if (lcd_encoder == menu_item) { \
          lcd_implementation_drawmenu_ ## type ## _selected (menu_row, _label_pstr , ## args ); \
        }else{\
          lcd_implementation_drawmenu_ ## type (menu_row, _label_pstr , ## args ); \
        }\
      }\
      if (menu_clicked && (lcd_encoder == menu_item)) {\
        lcd_quick_feedback(); \
        menu_action_ ## type ( args ); \
        return;\
      }\
    }\
    menu_item++;\
  } while(0)
*/

#if (SDCARDDETECT > 0)
bool lcd_oldcardstatus;
#endif

bool ignore_click = false;
bool wait_for_unclick;

// place-holders for Ki and Kd edits
#ifdef PIDTEMP
// float raw_Ki, raw_Kd;
#endif



const char STR_SEPARATOR[] PROGMEM = "------------";


static void lcd_implementation_drawmenu_sdfile_selected(uint8_t row, char* longFilename)
{
    char c;
    int enc_dif = lcd_encoder_diff;
    uint8_t n = LCD_WIDTH - 1;
    for(int g = 0; g<4;g++){
      lcd_set_cursor(0, g);
    lcd_print(' ');
    }

    lcd_set_cursor(0, row);
    lcd_print('>');
    int i = 1;
    int j = 0;
    char* longFilenameTMP = longFilename;

    while((c = *longFilenameTMP) != '\0')
    {
        lcd_set_cursor(i, row);
        lcd_print(c);
        i++;
        longFilenameTMP++;
        if(i==LCD_WIDTH){
          i=1;
          j++;
          longFilenameTMP = longFilename + j;          
          n = LCD_WIDTH - 1;
          for(int g = 0; g<300 ;g++){
			  manage_heater();
            if(LCD_CLICKED || ( enc_dif != lcd_encoder_diff )){
				longFilenameTMP = longFilename;
				*(longFilenameTMP + LCD_WIDTH - 2) = '\0';
				i = 1;
				j = 0;
				break;
            }else{
				if (j == 1) delay(3);	//wait around 1.2 s to start scrolling text
				delay(1);				//then scroll with redrawing every 300 ms 
            }

          }
        }
    }
    if(c!='\0'){
      lcd_set_cursor(i, row);
        lcd_print(c);
        i++;
    }
    n=n-i+1;
    while(n--)
        lcd_print(' ');
}
static void lcd_implementation_drawmenu_sdfile(uint8_t row, const char* filename, char* longFilename)
{
    char c;
    uint8_t n = LCD_WIDTH - 1;
    lcd_set_cursor(0, row);
    lcd_print(' ');
    if (longFilename[0] != '\0')
    {
        filename = longFilename;
        longFilename[LCD_WIDTH-1] = '\0';
    }
    while( ((c = *filename) != '\0') && (n>0) )
    {
        lcd_print(c);
        filename++;
        n--;
    }
    while(n--)
        lcd_print(' ');
}
static void lcd_implementation_drawmenu_sddirectory_selected(uint8_t row, const char* filename, char* longFilename)
{
    char c;
    uint8_t n = LCD_WIDTH - 2;
    lcd_set_cursor(0, row);
    lcd_print('>');
    lcd_print(LCD_STR_FOLDER[0]);
    if (longFilename[0] != '\0')
    {
        filename = longFilename;
        longFilename[LCD_WIDTH-2] = '\0';
    }
    while( ((c = *filename) != '\0') && (n>0) )
    {
        lcd_print(c);
        filename++;
        n--;
    }
    while(n--)
        lcd_print(' ');
}
static void lcd_implementation_drawmenu_sddirectory(uint8_t row, const char* filename, char* longFilename)
{
    char c;
    uint8_t n = LCD_WIDTH - 2;
    lcd_set_cursor(0, row);
    lcd_print(' ');
    lcd_print(LCD_STR_FOLDER[0]);
    if (longFilename[0] != '\0')
    {
        filename = longFilename;
        longFilename[LCD_WIDTH-2] = '\0';
    }
    while( ((c = *filename) != '\0') && (n>0) )
    {
        lcd_print(c);
        filename++;
        n--;
    }
    while(n--)
        lcd_print(' ');
}



#define MENU_ITEM_SDDIR(str_fn, str_fnl) do { if (menu_item_sddir(str_fn, str_fnl)) return; } while (0)
//#define MENU_ITEM_SDDIR(str, str_fn, str_fnl) MENU_ITEM(sddirectory, str, str_fn, str_fnl)
//extern uint8_t menu_item_sddir(const char* str, const char* str_fn, char* str_fnl);

#define MENU_ITEM_SDFILE(str, str_fn, str_fnl) do { if (menu_item_sdfile(str, str_fn, str_fnl)) return; } while (0)
//#define MENU_ITEM_SDFILE(str, str_fn, str_fnl) MENU_ITEM(sdfile, str, str_fn, str_fnl)
//extern uint8_t menu_item_sdfile(const char* str, const char* str_fn, char* str_fnl);


uint8_t menu_item_sddir(const char* str_fn, char* str_fnl)
{
#ifdef NEW_SD_MENU
//	str_fnl[18] = 0;
//	printf_P(PSTR("menu dir %d '%s' '%s'\n"), menu_row, str_fn, str_fnl);
	if (menu_item == menu_line)
	{
		if (lcd_draw_update)
		{
			lcd_set_cursor(0, menu_row);
			int cnt = lcd_printf_P(PSTR("%c%c%-18s"), (lcd_encoder == menu_item)?'>':' ', LCD_STR_FOLDER[0], str_fnl[0]?str_fnl:str_fn);
//			int cnt = lcd_printf_P(PSTR("%c%c%-18s"), (lcd_encoder == menu_item)?'>':' ', LCD_STR_FOLDER[0], str_fn);
		}
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			uint8_t depth = (uint8_t)card.getWorkDirDepth();
			strcpy(dir_names[depth], str_fn);
//			printf_P(PSTR("%s\n"), dir_names[depth]);
			card.chdir(str_fn);
			lcd_encoder = 0;
			return menu_item_ret();
		}
	}
	menu_item++;
	return 0;
#else //NEW_SD_MENU
	if (menu_item == menu_line)
	{
		if (lcd_draw_update)
		{
			if (lcd_encoder == menu_item)
				lcd_implementation_drawmenu_sddirectory_selected(menu_row, str_fn, str_fnl);
			else
				lcd_implementation_drawmenu_sddirectory(menu_row, str_fn, str_fnl);
		}
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			menu_clicked = false;
			lcd_update_enabled = 0;
			menu_action_sddirectory(str_fn);
			lcd_update_enabled = 1;
			return menu_item_ret();
		}
	}
	menu_item++;
	return 0;

#endif //NEW_SD_MENU
}

static uint8_t menu_item_sdfile(const char*
#ifdef NEW_SD_MENU
        str
#endif //NEW_SD_MENU
         ,const char* str_fn, char* str_fnl)
{
#ifdef NEW_SD_MENU
//	printf_P(PSTR("menu sdfile\n"));
//	str_fnl[19] = 0;
//	printf_P(PSTR("menu file %d '%s' '%s'\n"), menu_row, str_fn, str_fnl);
	if (menu_item == menu_line)
	{
		if (lcd_draw_update)
		{
//			printf_P(PSTR("menu file %d %d '%s'\n"), menu_row, menuData.sdcard_menu.viewState, str_fnl[0]?str_fnl:str_fn);
			lcd_set_cursor(0, menu_row);
/*			if (lcd_encoder == menu_item)
			{
				lcd_printf_P(PSTR("%c%-19s"), (lcd_encoder == menu_item)?'>':' ', (str_fnl[0]?str_fnl:str_fn) + 1);
				if (menuData.sdcard_menu.viewState == 0)
				{
					menuData.sdcard_menu.viewState++;
					lcd_printf_P(PSTR("%c%-19s"), (lcd_encoder == menu_item)?'>':' ', (str_fnl[0]?str_fnl:str_fn) + 1);
				}
				else if (menuData.sdcard_menu.viewState == 1)
				{
					lcd_printf_P(PSTR("%c%-19s"), (lcd_encoder == menu_item)?'>':' ', (str_fnl[0]?str_fnl:str_fn) + 2);
				}
			}
			else*/
			{
				str_fnl[19] = 0;
				lcd_printf_P(PSTR("%c%-19s"), (lcd_encoder == menu_item)?'>':' ', str_fnl[0]?str_fnl:str_fn);
			}

//			int cnt = lcd_printf_P(PSTR("%c%-19s"), (lcd_encoder == menu_item)?'>':' ', str_fnl);
//			int cnt = lcd_printf_P(PSTR("%cTESTIK.gcode"), (lcd_encoder == menu_item)?'>':' ');
		}
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			return menu_item_ret();
		}
	}
	menu_item++;
	return 0;
#else //NEW_SD_MENU
	if (menu_item == menu_line)
	{
		if (lcd_draw_update)
		{
			if (lcd_encoder == menu_item)
				lcd_implementation_drawmenu_sdfile_selected(menu_row, str_fnl);
			else
				lcd_implementation_drawmenu_sdfile(menu_row, str_fn, str_fnl);
		}
		if (menu_clicked && (lcd_encoder == menu_item))
		{
		    lcd_consume_click();
			menu_action_sdfile(str_fn);
			return menu_item_ret();
		}
	}
	menu_item++;
	return 0;
#endif //NEW_SD_MENU
}

// Print temperature (nozzle/bed) (9 chars total)
void lcdui_print_temp(char type, int val_current, int val_target)
{
	int chars = lcd_printf_P(_N("%c%3d/%d%c"), type, val_current, val_target, LCD_STR_DEGREE[0]);
	lcd_space(9 - chars);
}

// Print Z-coordinate (8 chars total)
void lcdui_print_Z_coord(void)
{
    if (custom_message_type == CUSTOM_MSG_TYPE_MESHBL)
        lcd_puts_P(_N("Z   --- "));
    else
		lcd_printf_P(_N("Z%6.2f "), current_position[Z_AXIS]);
}

#ifdef PLANNER_DIAGNOSTICS
// Print planner diagnostics (8 chars total)
void lcdui_print_planner_diag(void)
{
	lcd_set_cursor(LCD_WIDTH - 8-2, 1);
	lcd_print(LCD_STR_FEEDRATE[0]);
	lcd_print(itostr3(feedmultiply));
	lcd_puts_P(PSTR("%  Q"));
	{
		uint8_t queue = planner_queue_min();
		if (queue < (BLOCK_BUFFER_SIZE >> 1))
		lcd_putc('!');
		else
		{
			lcd_putc((char)(queue / 10) + '0');
			queue %= 10;
		}
		lcd_putc((char)queue + '0');
		planner_queue_min_reset();
	}
}
#endif // PLANNER_DIAGNOSTICS

// Print feedrate (8 chars total)
void lcdui_print_feedrate(void)
{
	int chars = lcd_printf_P(_N("%c%3d%%"), LCD_STR_FEEDRATE[0], feedmultiply);
	lcd_space(8 - chars);
}

// Print percent done in form "USB---%", " SD---%", "   ---%" (7 chars total)
void lcdui_print_percent_done(void)
{
	const char* src = is_usb_printing?_N("USB"):(IS_SD_PRINTING?_N(" SD"):_N("   "));
	char per[4];
	bool num = IS_SD_PRINTING || (PRINTER_ACTIVE && (print_percent_done_normal != PRINT_PERCENT_DONE_INIT));
	sprintf_P(per, num?_N("%3hhd"):_N("---"), calc_percent_done());
	lcd_printf_P(_N("%3S%3s%%"), src, per);
}

// Print extruder status (5 chars total)
void lcdui_print_extruder(void)
{
	int chars = 0;
	if (mmu_extruder == tmp_extruder)
		chars = lcd_printf_P(_N(" F%u"), mmu_extruder+1);
	else
		chars = lcd_printf_P(_N(" %u>%u"), mmu_extruder+1, tmp_extruder+1);
	lcd_space(5 - chars);
}

// Print farm number (5 chars total)
void lcdui_print_farm(void)
{
	int chars = lcd_printf_P(_N(" F0  "));
//	lcd_space(5 - chars);
/*
	// Farm number display
	if (farm_mode)
	{
		lcd_set_cursor(6, 2);
		lcd_puts_P(PSTR(" F"));
		lcd_print(farm_no);
		lcd_puts_P(PSTR("  "));
        
        // Beat display
        lcd_set_cursor(LCD_WIDTH - 1, 0);
        if ( (millis() - kicktime) < 60000 ) {
        
            lcd_puts_P(PSTR("L"));
        
        }else{
            lcd_puts_P(PSTR(" "));
        }
        
	}
	else {
#ifdef SNMM
		lcd_puts_P(PSTR(" E"));
		lcd_print(get_ext_nr() + 1);

#else
		lcd_set_cursor(LCD_WIDTH - 8 - 2, 2);
		lcd_puts_P(PSTR(" "));
#endif
	}
*/
}

#ifdef CMD_DIAGNOSTICS
// Print CMD queue diagnostic (8 chars total)
void lcdui_print_cmd_diag(void)
{
	lcd_set_cursor(LCD_WIDTH - 8 -1, 2);
	lcd_puts_P(PSTR("      C"));
	lcd_print(buflen);	// number of commands in cmd buffer
	if (buflen < 9) lcd_puts_P(" ");
}
#endif //CMD_DIAGNOSTICS

// Print time (8 chars total)
void lcdui_print_time(void)
{
	//if remaining print time estimation is available print it else print elapsed time
	uint16_t print_t = 0;
	if (print_time_remaining_normal != PRINT_TIME_REMAINING_INIT)
		print_t = print_time_remaining();
	else if(starttime != 0)
		print_t = millis() / 60000 - starttime / 60000;
	int chars = 0;
	if ((PRINTER_ACTIVE) && ((print_time_remaining_normal != PRINT_TIME_REMAINING_INIT) || (starttime != 0)))
	{
          char suff = ' ';
          char suff_doubt = ' ';
		if (print_time_remaining_normal != PRINT_TIME_REMAINING_INIT)
          {
               suff = 'R';
               if (feedmultiply != 100)
                    suff_doubt = '?';
          }
		if (print_t < 6000) //time<100h
			chars = lcd_printf_P(_N("%c%02u:%02u%c%c"), LCD_STR_CLOCK[0], print_t / 60, print_t % 60, suff, suff_doubt);
		else //time>=100h
			chars = lcd_printf_P(_N("%c%3uh %c%c"), LCD_STR_CLOCK[0], print_t / 60, suff, suff_doubt);
	}
	else
		chars = lcd_printf_P(_N("%c--:--  "), LCD_STR_CLOCK[0]);
	lcd_space(8 - chars);
}

//Print status line on status screen
void lcdui_print_status_line(void)
{
	if (IS_SD_PRINTING)
	{
		if (strcmp(longFilenameOLD, card.longFilename) != 0)
		{
			memset(longFilenameOLD, '\0', strlen(longFilenameOLD));
			sprintf_P(longFilenameOLD, PSTR("%s"), card.longFilename);
			scrollstuff = 0;
		}
	}

	if (heating_status)
	{ // If heating flag, show progress of heating
		heating_status_counter++;
		if (heating_status_counter > 13)
		{
			heating_status_counter = 0;
		}
		lcd_set_cursor(7, 3);
		lcd_puts_P(PSTR("             "));

		for (unsigned int dots = 0; dots < heating_status_counter; dots++)
		{
			lcd_set_cursor(7 + dots, 3);
			lcd_print('.');
		}
		switch (heating_status)
		{
		case 1:
			lcd_set_cursor(0, 3);
			lcd_puts_P(_T(MSG_HEATING));
			break;
		case 2:
			lcd_set_cursor(0, 3);
			lcd_puts_P(_T(MSG_HEATING_COMPLETE));
			heating_status = 0;
			heating_status_counter = 0;
			break;
		case 3:
			lcd_set_cursor(0, 3);
			lcd_puts_P(_T(MSG_BED_HEATING));
			break;
		case 4:
			lcd_set_cursor(0, 3);
			lcd_puts_P(_T(MSG_BED_DONE));
			heating_status = 0;
			heating_status_counter = 0;
			break;
		default:
			break;
		}
	}
	else if ((IS_SD_PRINTING) && (custom_message_type == CUSTOM_MSG_TYPE_STATUS))
	{ // If printing from SD, show what we are printing
		if(strlen(card.longFilename) > LCD_WIDTH)
		{
			int inters = 0;
			int gh = scrollstuff;
			while (((gh - scrollstuff) < LCD_WIDTH) && (inters == 0))
			{
				if (card.longFilename[gh] == '\0')
				{
					lcd_set_cursor(gh - scrollstuff, 3);
					lcd_print(card.longFilename[gh - 1]);
					scrollstuff = 0;
					gh = scrollstuff;
					inters = 1;
				}
				else
				{
					lcd_set_cursor(gh - scrollstuff, 3);
					lcd_print(card.longFilename[gh - 1]);
					gh++;
				}
			}
			scrollstuff++;
		}
		else
		{
			lcd_print(longFilenameOLD);
		}
	}
	else
	{ // Otherwise check for other special events
   		switch (custom_message_type)
		{
		case CUSTOM_MSG_TYPE_STATUS: // Nothing special, print status message normally
			lcd_print(lcd_status_message);
			break;
		case CUSTOM_MSG_TYPE_MESHBL: // If mesh bed leveling in progress, show the status
			if (custom_message_state > 10)
			{
				lcd_set_cursor(0, 3);
				lcd_puts_P(PSTR("                    "));
				lcd_set_cursor(0, 3);
				lcd_puts_P(_T(MSG_CALIBRATE_Z_AUTO));
				lcd_puts_P(PSTR(" : "));
				lcd_print(custom_message_state-10);
			}
			else
			{
				if (custom_message_state == 3)
				{
					lcd_puts_P(_T(WELCOME_MSG));
					lcd_setstatuspgm(_T(WELCOME_MSG));
					custom_message_type = CUSTOM_MSG_TYPE_STATUS;
				}
				if (custom_message_state > 3 && custom_message_state <= 10 )
				{
					lcd_set_cursor(0, 3);
					lcd_puts_P(PSTR("                   "));
					lcd_set_cursor(0, 3);
					lcd_puts_P(_i("Calibration done"));////MSG_HOMEYZ_DONE c=0 r=0
					custom_message_state--;
				}
			}
			break;
		case CUSTOM_MSG_TYPE_F_LOAD: // If loading filament, print status
			lcd_print(lcd_status_message);
			break;
		case CUSTOM_MSG_TYPE_PIDCAL: // PID tuning in progress
			lcd_print(lcd_status_message);
			if (pid_cycle <= pid_number_of_cycles && custom_message_state > 0)
			{
				lcd_set_cursor(10, 3);
				lcd_print(itostr3(pid_cycle));
				lcd_print('/');
				lcd_print(itostr3left(pid_number_of_cycles));
			}
			break;
		case CUSTOM_MSG_TYPE_TEMCAL: // PINDA temp calibration in progress
			{
				char progress[4];
				lcd_set_cursor(0, 3);
				lcd_puts_P(_T(MSG_TEMP_CALIBRATION));
				lcd_set_cursor(12, 3);
				sprintf(progress, "%d/6", custom_message_state);
				lcd_print(progress);
			}
			break;
		case CUSTOM_MSG_TYPE_TEMPRE: // temp compensation preheat
			lcd_set_cursor(0, 3);
			lcd_puts_P(_i("PINDA Heating"));////MSG_PINDA_PREHEAT c=20 r=1
			if (custom_message_state <= PINDA_HEAT_T)
			{
				lcd_puts_P(PSTR(": "));
				lcd_print(custom_message_state); //seconds
				lcd_print(' ');
			}
			break;
		}
	}
    
    // Fill the rest of line to have nice and clean output
	for(int fillspace = 0; fillspace < 20; fillspace++)
		if ((lcd_status_message[fillspace] <= 31 ))
			lcd_print(' ');
}

void lcdui_print_status_screen(void)
{
//|01234567890123456789|
//|N 000/000D  Z000.0  |
//|B 000/000D  F100%   |
//|USB100% T0  t--:--  |
//|Status line.........|
//----------------------
//N - nozzle temp symbol LCD_STR_THERMOMETER
//B - bed temp symbol LCD_STR_BEDTEMP
//F - feedrate symbol LCD_STR_FEEDRATE
//t - clock symbol LCD_STR_THERMOMETER

    lcd_set_cursor(0, 0); //line 0

    //Print the hotend temperature (9 chars total)
	lcdui_print_temp(LCD_STR_THERMOMETER[0], (int)(degHotend(0) + 0.5), (int)(degTargetHotend(0) + 0.5));

	lcd_space(3); //3 spaces

    //Print Z-coordinate (8 chars total)
	lcdui_print_Z_coord();

    lcd_set_cursor(0, 1); //line 1

	//Print the Bed temperature (9 chars total)
	lcdui_print_temp(LCD_STR_BEDTEMP[0], (int)(degBed() + 0.5), (int)(degTargetBed() + 0.5));

	lcd_space(3); //3 spaces

#ifdef PLANNER_DIAGNOSTICS
	//Print planner diagnostics (8 chars)
	lcdui_print_planner_diag();
#else // PLANNER_DIAGNOSTICS
    //Print Feedrate (8 chars)
	lcdui_print_feedrate();
#endif // PLANNER_DIAGNOSTICS

	lcd_set_cursor(0, 2); //line 2

	//Print SD status (7 chars)
	lcdui_print_percent_done();

	if (mmu_enabled)
		//Print extruder status (5 chars)
		lcdui_print_extruder();
	else if (farm_mode)
		//Print farm number (5 chars)
		lcdui_print_farm();
	else
		lcd_space(5); //5 spaces

#ifdef CMD_DIAGNOSTICS
    //Print cmd queue diagnostics (8chars)
	lcdui_print_cmd_diag();
#else
    //Print time (8chars)
	lcdui_print_time();
#endif //CMD_DIAGNOSTICS

    lcd_set_cursor(0, 3); //line 3

#ifndef DEBUG_DISABLE_LCD_STATUS_LINE
	lcdui_print_status_line();
#endif //DEBUG_DISABLE_LCD_STATUS_LINE

}

// Main status screen. It's up to the implementation specific part to show what is needed. As this is very display dependent
static void lcd_status_screen()
{
	if (firstrun == 1) 
	{
		firstrun = 0;
		if(lcd_status_message_level == 0)
		{
			strncpy_P(lcd_status_message, _T(WELCOME_MSG), LCD_WIDTH);
			lcd_finishstatus();
		}
		if (eeprom_read_byte((uint8_t *)EEPROM_TOTALTIME) == 255 && eeprom_read_byte((uint8_t *)EEPROM_TOTALTIME + 1) == 255 && eeprom_read_byte((uint8_t *)EEPROM_TOTALTIME + 2) == 255 && eeprom_read_byte((uint8_t *)EEPROM_TOTALTIME + 3) == 255)
		{
			eeprom_update_dword((uint32_t *)EEPROM_TOTALTIME, 0);
			eeprom_update_dword((uint32_t *)EEPROM_FILAMENTUSED, 0);
		}
	}

	if (lcd_status_update_delay)
		lcd_status_update_delay--;
	else
		lcd_draw_update = 1;


	if (lcd_draw_update)
	{
		ReInitLCD++;
		if (ReInitLCD == 30)
		{
			lcd_refresh(); // to maybe revive the LCD if static electricity killed it.
			ReInitLCD = 0 ;
		}
		else
		{
			if ((ReInitLCD % 10) == 0)
				lcd_refresh_noclear(); //to maybe revive the LCD if static electricity killed it.
		}

		lcdui_print_status_screen();

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
					prusa_statistics(20);
				break;
			}
		} // end of farm_mode

		lcd_status_update_delay = 10;   /* redraw the main screen every second. This is easier then trying keep track of all things that change on the screen */
		if (lcd_commands_type != LCD_COMMAND_IDLE)
			lcd_commands();
	} // end of lcd_draw_update

	bool current_click = LCD_CLICKED;

	if (ignore_click)
	{
		if (wait_for_unclick)
		{
			if (!current_click)
				ignore_click = wait_for_unclick = false;
			else
				current_click = false;
		}
		else if (current_click)
		{
			lcd_quick_feedback();
			wait_for_unclick = true;
			current_click = false;
		}
	}

	if (current_click && (lcd_commands_type != LCD_COMMAND_STOP_PRINT)) //click is aborted unless stop print finishes
	{
		menu_depth = 0; //redundant, as already done in lcd_return_to_status(), just to be sure
		menu_submenu(lcd_main_menu);
		lcd_refresh(); // to maybe revive the LCD if static electricity killed it.
	}

#ifdef ULTIPANEL_FEEDMULTIPLY
	// Dead zone at 100% feedrate
	if ((feedmultiply < 100 && (feedmultiply + int(lcd_encoder)) > 100) ||
		(feedmultiply > 100 && (feedmultiply + int(lcd_encoder)) < 100))
	{
		lcd_encoder = 0;
		feedmultiply = 100;
	}
	if (feedmultiply == 100 && int(lcd_encoder) > ENCODER_FEEDRATE_DEADZONE)
	{
		feedmultiply += int(lcd_encoder) - ENCODER_FEEDRATE_DEADZONE;
		lcd_encoder = 0;
	}
	else if (feedmultiply == 100 && int(lcd_encoder) < -ENCODER_FEEDRATE_DEADZONE)
	{
		feedmultiply += int(lcd_encoder) + ENCODER_FEEDRATE_DEADZONE;
		lcd_encoder = 0;
	}
	else if (feedmultiply != 100)
	{
		feedmultiply += int(lcd_encoder);
		lcd_encoder = 0;
	}
#endif //ULTIPANEL_FEEDMULTIPLY

	if (feedmultiply < 10)
		feedmultiply = 10;
	else if (feedmultiply > 999)
		feedmultiply = 999;
}


void lcd_commands()
{	
	if (lcd_commands_type == LCD_COMMAND_LONG_PAUSE)
	{
		if (!blocks_queued() && !homing_flag)
		{
			lcd_setstatuspgm(_i("Print paused"));////MSG_PRINT_PAUSED c=20 r=1
			long_pause();
			lcd_commands_type = 0;
			lcd_commands_step = 0;
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

		if (lcd_commands_step>1) lcd_timeoutToStatus.start(); //if user dont confirm live adjust Z value by pressing the knob, we are saving last value by timeout to status screen
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
			enquecommand_P(_T(MSG_M117_V2_CALIBRATION));
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
			lcd_timeoutToStatus.start();
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

			lcd_clear();
			menu_goto(lcd_babystep_z, 0, false, true);


			lcd_commands_step = 8;
		}
		if (lcd_commands_step == 8 && !blocks_queued() && cmd_buffer_empty()) //draw meander
		{
			lcd_timeoutToStatus.start();


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
			lcd_timeoutToStatus.start();
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
			lcd_timeoutToStatus.start();
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
			lcd_timeoutToStatus.start();
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
			lcd_timeoutToStatus.start();
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
			lcd_timeoutToStatus.start();
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
			lcd_timeoutToStatus.start();

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
			lcd_setstatuspgm(_T(WELCOME_MSG));
			lcd_commands_step = 0;
			lcd_commands_type = 0;
			if (eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE) == 1) {
				lcd_wizard(WizState::RepeatLay1Cal);
			}
		}

	}

#else //if not SNMM

	if (lcd_commands_type == LCD_COMMAND_V2_CAL)
	{
		char cmd1[30];
		static uint8_t filament = 0;
		float width = 0.4;
		float length = 20 - width;
		float extr = count_e(0.2, width, length);
		float extr_short_segment = count_e(0.2, width, width);
		if(lcd_commands_step>1) lcd_timeoutToStatus.start(); //if user dont confirm live adjust Z value by pressing the knob, we are saving last value by timeout to status screen

		if (lcd_commands_step == 0 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_commands_step = 10;
		}
		if (lcd_commands_step == 20 && !blocks_queued() && cmd_buffer_empty())
		{
            filament = 0;
            lcd_commands_step = 10;
		}
        if (lcd_commands_step == 21 && !blocks_queued() && cmd_buffer_empty())
        {
            filament = 1;
            lcd_commands_step = 10;
        }
        if (lcd_commands_step == 22 && !blocks_queued() && cmd_buffer_empty())
        {
            filament = 2;
            lcd_commands_step = 10;
        }
        if (lcd_commands_step == 23 && !blocks_queued() && cmd_buffer_empty())
        {
            filament = 3;
            lcd_commands_step = 10;
        }
        if (lcd_commands_step == 24 && !blocks_queued() && cmd_buffer_empty())
        {
            filament = 4;
            lcd_commands_step = 10;
        }

		if (lcd_commands_step == 10)
		{
			enquecommand_P(PSTR("M107"));
			enquecommand_P(PSTR("M104 S" STRINGIFY(PLA_PREHEAT_HOTEND_TEMP)));
			enquecommand_P(PSTR("M140 S" STRINGIFY(PLA_PREHEAT_HPB_TEMP)));
			enquecommand_P(PSTR("M190 S" STRINGIFY(PLA_PREHEAT_HPB_TEMP)));
            enquecommand_P(PSTR("M109 S" STRINGIFY(PLA_PREHEAT_HOTEND_TEMP)));
			enquecommand_P(_T(MSG_M117_V2_CALIBRATION));
			enquecommand_P(PSTR("G28"));
			enquecommand_P(PSTR("G92 E0.0"));

            lcd_commands_step = 9;
		}
        if (lcd_commands_step == 9 && !blocks_queued() && cmd_buffer_empty())
        {
            lcd_clear();
            menu_depth = 0;
            menu_submenu(lcd_babystep_z);

            if (mmu_enabled)
            {
                enquecommand_P(PSTR("M83")); //intro line
                enquecommand_P(PSTR("G1 Y-3.0 F1000.0")); //intro line
                enquecommand_P(PSTR("G1 Z0.4 F1000.0")); //intro line
                strcpy(cmd1, "T");
                strcat(cmd1, itostr3left(filament));
                enquecommand(cmd1);
                enquecommand_P(PSTR("G1 X55.0 E32.0 F1073.0")); //intro line
                enquecommand_P(PSTR("G1 X5.0 E32.0 F1800.0")); //intro line
                enquecommand_P(PSTR("G1 X55.0 E8.0 F2000.0")); //intro line
                enquecommand_P(PSTR("G1 Z0.3 F1000.0")); //intro line
                enquecommand_P(PSTR("G92 E0.0")); //intro line
                enquecommand_P(PSTR("G1 X240.0 E25.0  F2200.0")); //intro line
                enquecommand_P(PSTR("G1 Y-2.0 F1000.0")); //intro line
                enquecommand_P(PSTR("G1 X55.0 E25 F1400.0")); //intro line
                enquecommand_P(PSTR("G1 Z0.20 F1000.0")); //intro line
                enquecommand_P(PSTR("G1 X5.0 E4.0 F1000.0")); //intro line

            } else
            {
                enquecommand_P(PSTR("G1 X60.0 E9.0 F1000.0")); //intro line
                enquecommand_P(PSTR("G1 X100.0 E12.5 F1000.0")); //intro line
            }

            lcd_commands_step = 8;
        }
		if (lcd_commands_step == 8 && !blocks_queued() && cmd_buffer_empty())
		{

			enquecommand_P(PSTR("G92 E0.0"));
			enquecommand_P(PSTR("G21")); //set units to millimeters
			enquecommand_P(PSTR("G90")); //use absolute coordinates
			enquecommand_P(PSTR("M83")); //use relative distances for extrusion
			enquecommand_P(PSTR("G1 E-1.50000 F2100.00000"));
			enquecommand_P(PSTR("G1 Z5 F7200.000"));
			enquecommand_P(PSTR("M204 S1000")); //set acceleration
			enquecommand_P(PSTR("G1 F4000"));
			lcd_commands_step = 7;
		}
		if (lcd_commands_step == 7 && !blocks_queued() && cmd_buffer_empty()) //draw meander
		{
			lcd_timeoutToStatus.start();


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
			enquecommand_P(PSTR("G1 Z0.150 F7200.000"));
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

			lcd_timeoutToStatus.start();

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
			lcd_timeoutToStatus.start();
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
			lcd_timeoutToStatus.start();
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
			lcd_timeoutToStatus.start();
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
			lcd_timeoutToStatus.start();
			enquecommand_P(PSTR("M107")); //turn off printer fan			
			enquecommand_P(PSTR("G1 E-0.07500 F2100.00000")); //retract
			enquecommand_P(PSTR("M104 S0")); // turn off temperature
			enquecommand_P(PSTR("M140 S0")); // turn off heatbed
			enquecommand_P(PSTR("G1 Z10 F1300.000")); //lift Z
			enquecommand_P(PSTR("G1 X10 Y180 F4000")); //Go to parking position
			if (mmu_enabled) enquecommand_P(PSTR("M702 C")); //unload from nozzle
			enquecommand_P(PSTR("M84"));// disable motors
			forceMenuExpire = true; //if user dont confirm live adjust Z value by pressing the knob, we are saving last value by timeout to status screen
			lcd_commands_step = 1;
		}
		if (lcd_commands_step == 1 && !blocks_queued() && cmd_buffer_empty())
		{
			lcd_setstatuspgm(_T(WELCOME_MSG));
			lcd_commands_step = 0;
			lcd_commands_type = 0;			
			if (eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE) == 1) {
				lcd_wizard(WizState::RepeatLay1Cal);
			}
		}

	}

#endif // not SNMM

	if (lcd_commands_type == LCD_COMMAND_STOP_PRINT)   /// stop print
	{
		

		if (lcd_commands_step == 0) 
		{ 
			lcd_commands_step = 6; 
		}

		if (lcd_commands_step == 1 && !blocks_queued())
		{
			lcd_commands_step = 0;
			lcd_commands_type = 0;
			lcd_setstatuspgm(_T(WELCOME_MSG));
			custom_message_type = CUSTOM_MSG_TYPE_STATUS;
			isPrintPaused = false;
		}
		if (lcd_commands_step == 2 && !blocks_queued())
		{
			setTargetBed(0);
			enquecommand_P(PSTR("M104 S0")); //set hotend temp to 0

			manage_heater();
			lcd_setstatuspgm(_T(WELCOME_MSG));
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
			lcd_setstatuspgm(_T(MSG_PLEASE_WAIT));
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
			if (mmu_enabled)
				lcd_commands_step = 8;
			else
				lcd_commands_step = 3;
		}
		if (lcd_commands_step == 5 && !blocks_queued())
		{
			lcd_setstatuspgm(_T(MSG_PRINT_ABORTED));
      // G91: Set to relative positioning.
			enquecommand_P(PSTR("G91"));
      // Lift up.
			enquecommand_P(PSTR("G1 Z15 F1500"));
			if (axis_known_position[X_AXIS] && axis_known_position[Y_AXIS]) lcd_commands_step = 4;
			else lcd_commands_step = 3;
		}
		if (lcd_commands_step == 6 && !blocks_queued())
		{
			lcd_setstatuspgm(_T(MSG_PRINT_ABORTED));
			cancel_heatup = true;
			setTargetBed(0);
			if (mmu_enabled)
				setAllTargetHotends(0);
			manage_heater();
			custom_message_type = CUSTOM_MSG_TYPE_F_LOAD;
			lcd_commands_step = 5;
		}
		if (lcd_commands_step == 7 && !blocks_queued())
		{
			if (mmu_enabled)
				enquecommand_P(PSTR("M702 C")); //current
			else
				switch(snmm_stop_print_menu())
				{
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

		if (lcd_commands_step == 0) { lcd_commands_step = 6; }

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
			custom_message_type = CUSTOM_MSG_TYPE_PIDCAL;
			custom_message_state = 1;
			lcd_draw_update = 3;
			lcd_commands_step = 3;
		}
		if (lcd_commands_step == 3 && !blocks_queued()) { //PID calibration
			strcpy(cmd1, "M303 E0 S");
			strcat(cmd1, ftostr3(pid_temp));
			enquecommand(cmd1);
			lcd_setstatuspgm(_i("PID cal.           "));////MSG_PID_RUNNING c=20 r=1
			lcd_commands_step = 2;
		}
		if (lcd_commands_step == 2 && pid_tuning_finished) { //saving to eeprom
			pid_tuning_finished = false;
			custom_message_state = 0;
			lcd_setstatuspgm(_i("PID cal. finished"));////MSG_PID_FINISHED c=20 r=1
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
			lcd_setstatuspgm(_T(WELCOME_MSG));
			custom_message_type = CUSTOM_MSG_TYPE_STATUS;
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

void lcd_return_to_status()
{
	lcd_refresh(); // to maybe revive the LCD if static electricity killed it.
	menu_goto(lcd_status_screen, 0, false, true);
	menu_depth = 0;
}

//! @brief Pause print, disable nozzle heater, move to park position
void lcd_pause_print()
{
    lcd_return_to_status();
    stop_and_save_print_to_ram(0.0,0.0);
    setAllTargetHotends(0);
    isPrintPaused = true;
    if (LCD_COMMAND_IDLE == lcd_commands_type)
    {
        lcd_commands_type = LCD_COMMAND_LONG_PAUSE;
    }
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
  if (!wizard_active) setTargetBed(PLA_PREHEAT_HPB_TEMP);
  fanSpeed = 0;
  lcd_return_to_status();
  setWatch(); // heater sanity check timer
  if (wizard_active) lcd_wizard(WizState::Unload);
}

void lcd_preheat_abs()
{
  setTargetHotend0(ABS_PREHEAT_HOTEND_TEMP);
  if (!wizard_active) setTargetBed(ABS_PREHEAT_HPB_TEMP);
  fanSpeed = 0;
  lcd_return_to_status();
  setWatch(); // heater sanity check timer
  if (wizard_active) lcd_wizard(WizState::Unload);
}

void lcd_preheat_pp()
{
  setTargetHotend0(PP_PREHEAT_HOTEND_TEMP);
  if (!wizard_active) setTargetBed(PP_PREHEAT_HPB_TEMP);
  fanSpeed = 0;
  lcd_return_to_status();
  setWatch(); // heater sanity check timer
  if (wizard_active) lcd_wizard(WizState::Unload);
}

void lcd_preheat_pet()
{
  setTargetHotend0(PET_PREHEAT_HOTEND_TEMP);
  if (!wizard_active) setTargetBed(PET_PREHEAT_HPB_TEMP);
  fanSpeed = 0;
  lcd_return_to_status();
  setWatch(); // heater sanity check timer
  if (wizard_active) lcd_wizard(WizState::Unload);
}

void lcd_preheat_hips()
{
  setTargetHotend0(HIPS_PREHEAT_HOTEND_TEMP);
  if (!wizard_active) setTargetBed(HIPS_PREHEAT_HPB_TEMP);
  fanSpeed = 0;
  lcd_return_to_status();
  setWatch(); // heater sanity check timer
  if (wizard_active) lcd_wizard(WizState::Unload);
}

void lcd_preheat_flex()
{
  setTargetHotend0(FLEX_PREHEAT_HOTEND_TEMP);
  if (!wizard_active) setTargetBed(FLEX_PREHEAT_HPB_TEMP);
  fanSpeed = 0;
  lcd_return_to_status();
  setWatch(); // heater sanity check timer
  if (wizard_active) lcd_wizard(WizState::Unload);
}


void lcd_cooldown()
{
  setAllTargetHotends(0);
  setTargetBed(0);
  fanSpeed = 0;
  lcd_return_to_status();
}


static void lcd_menu_extruder_info()
{
//|01234567890123456789|
//|Nozzle FAN:      RPM|
//|Print FAN:       RPM|
//|Fil. Xd:    Yd:     |
//|Int:      Shut:     |
//----------------------
	int fan_speed_RPM[2];
	// Display Nozzle fan RPM
	fan_speed_RPM[0] = 60*fan_speed[0];
	fan_speed_RPM[1] = 60*fan_speed[1];

	lcd_timeoutToStatus.stop(); //infinite timeout

	lcd_printf_P(_N(
	  ESC_H(0,0)
	  "%S: %4d RPM\n"
	  "%S:  %4d RPM\n"
	 ),
	 _i("Nozzle FAN"),
	 fan_speed_RPM[0],
	 _i("Print FAN"),
	 fan_speed_RPM[1]
	);

#ifdef FILAMENT_SENSOR
	// Display X and Y difference from Filament sensor    
    // Display Light intensity from Filament sensor
    //  Frame_Avg register represents the average brightness of all pixels within a frame (324 pixels). This
    //  value ranges from 0(darkest) to 255(brightest).
    // Display LASER shutter time from Filament sensor
    //  Shutter register is an index of LASER shutter time. It is automatically controlled by the chip's internal
    //  auto-exposure algorithm. When the chip is tracking on a good reflection surface, the Shutter is small.
    //  When the chip is tracking on a poor reflection surface, the Shutter is large. Value ranges from 0 to 46.
	if (mmu_enabled == false)
	{
		if (!fsensor_enabled)
			lcd_puts_P(_N("Filament sensor\n" "is disabled."));
		else
		{
			if (!moves_planned() && !IS_SD_PRINTING && !is_usb_printing && (lcd_commands_type != LCD_COMMAND_V2_CAL))
				pat9125_update();
			lcd_printf_P(_N(
				"Fil. Xd:%3d Yd:%3d\n"
				"Int: %3d  Shut: %3d"
			),
				pat9125_x, pat9125_y,
				pat9125_b, pat9125_s
			);
		}
	}
#endif //FILAMENT_SENSOR
    
    menu_back_if_clicked();
}

#if defined(TMC2130) && defined(FILAMENT_SENSOR)
static void lcd_menu_fails_stats_total()
{
//01234567890123456789
//Total failures
// Power failures  000
// Filam. runouts  000
// Crash  X 000  Y 000
//////////////////////
	lcd_timeoutToStatus.stop(); //infinite timeout
    uint16_t power = eeprom_read_word((uint16_t*)EEPROM_POWER_COUNT_TOT);
    uint16_t filam = eeprom_read_word((uint16_t*)EEPROM_FERROR_COUNT_TOT);
    uint16_t crashX = eeprom_read_word((uint16_t*)EEPROM_CRASH_COUNT_X_TOT);
    uint16_t crashY = eeprom_read_word((uint16_t*)EEPROM_CRASH_COUNT_Y_TOT);
//	lcd_printf_P(PSTR(ESC_H(0,0) "Total failures" ESC_H(1,1) "Power failures  %-3d" ESC_H(1,2) "Filam. runouts  %-3d" ESC_H(1,3) "Crash  X %-3d  Y %-3d"), power, filam, crashX, crashY);
	lcd_printf_P(PSTR(ESC_H(0,0) "%S" ESC_H(1,1) "%S  %-3d" ESC_H(1,2) "%S  %-3d" ESC_H(1,3) "%S  X %-3d  Y %-3d"), _i("Total failures"), _i("Power failures"), power, _i("Filam. runouts"), filam, _i("Crash"), crashX, crashY);
	menu_back_if_clicked_fb();
}

static void lcd_menu_fails_stats_print()
{
//01234567890123456789
//Last print failures
// Power failures  000
// Filam. runouts  000
// Crash  X 000  Y 000
//////////////////////
	lcd_timeoutToStatus.stop(); //infinite timeout
    uint8_t power = eeprom_read_byte((uint8_t*)EEPROM_POWER_COUNT);
    uint8_t filam = eeprom_read_byte((uint8_t*)EEPROM_FERROR_COUNT);
    uint8_t crashX = eeprom_read_byte((uint8_t*)EEPROM_CRASH_COUNT_X);
    uint8_t crashY = eeprom_read_byte((uint8_t*)EEPROM_CRASH_COUNT_Y);
//	lcd_printf_P(PSTR(ESC_H(0,0) "Last print failures" ESC_H(1,1) "Power failures  %-3d" ESC_H(1,2) "Filam. runouts  %-3d" ESC_H(1,3) "Crash  X %-3d  Y %-3d"), power, filam, crashX, crashY);
	lcd_printf_P(PSTR(ESC_H(0,0) "%S" ESC_H(1,1) "%S  %-3d" ESC_H(1,2) "%S  %-3d" ESC_H(1,3) "%S  X %-3d  Y %-3d"), _i("Last print failures"), _i("Power failures"), power, _i("Filam. runouts"), filam, _i("Crash"), crashX, crashY);
	menu_back_if_clicked_fb();
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
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));
	MENU_ITEM_SUBMENU_P(_i("Last print"), lcd_menu_fails_stats_print);
	MENU_ITEM_SUBMENU_P(_i("Total"), lcd_menu_fails_stats_total);
	MENU_END();
}
#elif defined(FILAMENT_SENSOR)
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
	lcd_timeoutToStatus.stop(); //infinite timeout
    uint8_t filamentLast = eeprom_read_byte((uint8_t*)EEPROM_FERROR_COUNT);
    uint16_t filamentTotal = eeprom_read_word((uint16_t*)EEPROM_FERROR_COUNT_TOT);
    lcd_printf_P(PSTR(ESC_H(0,0) "Last print failures" ESC_H(1,1) "Filam. runouts  %-3d" ESC_H(0,2) "Total failures" ESC_H(1,3) "Filam. runouts  %-3d"), filamentLast, filamentTotal);
    menu_back_if_clicked();
}
#else
static void lcd_menu_fails_stats()
{
	lcd_timeoutToStatus.stop(); //infinite timeout
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));
	MENU_END();
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
	lcd_printf_P(PSTR(ESC_H(1,1) "RAM statistics" ESC_H(5,1) "SP_min: 0x%04x" ESC_H(1,2) "heap_start: 0x%04x" ESC_H(3,3) "heap_end: 0x%04x"), SP_min, __malloc_heap_start, __malloc_heap_end);
#endif //DEBUG_STACK_MONITOR

	menu_back_if_clicked_fb();
}
#endif /* DEBUG_BUILD */

static void lcd_menu_temperatures()
{
	lcd_timeoutToStatus.stop(); //infinite timeout

	lcd_printf_P(PSTR(ESC_H(1,0) "%S:   %d%c" ESC_H(1,1) "%S:      %d%c"), _i("Nozzle"), (int)current_temperature[0], '\x01', _i("Bed"), (int)current_temperature_bed, '\x01');
#ifdef AMBIENT_THERMISTOR
	lcd_printf_P(PSTR(ESC_H(1,2) "%S:  %d%c" ESC_H(1,3) "PINDA:    %d%c"), _i("Ambient"), (int)current_temperature_ambient, '\x01', (int)current_temperature_pinda, '\x01');
#else //AMBIENT_THERMISTOR
	lcd_printf_P(PSTR(ESC_H(1,2) "PINDA:    %d%c"), (int)current_temperature_pinda, '\x01');
#endif //AMBIENT_THERMISTOR

    menu_back_if_clicked();
}

#if defined (VOLT_BED_PIN) || defined (VOLT_PWR_PIN)
#define VOLT_DIV_R1 10000
#define VOLT_DIV_R2 2370
#define VOLT_DIV_FAC ((float)VOLT_DIV_R2 / (VOLT_DIV_R2 + VOLT_DIV_R1))
#define VOLT_DIV_REF 5
static void lcd_menu_voltages()
{
	lcd_timeoutToStatus.stop(); //infinite timeout
	float volt_pwr = VOLT_DIV_REF * ((float)current_voltage_raw_pwr / (1023 * OVERSAMPLENR)) / VOLT_DIV_FAC;
	float volt_bed = VOLT_DIV_REF * ((float)current_voltage_raw_bed / (1023 * OVERSAMPLENR)) / VOLT_DIV_FAC;
	lcd_printf_P(PSTR(ESC_H(1,1)"PWR:      %d.%01dV" ESC_H(1,2)"BED:      %d.%01dV"), (int)volt_pwr, (int)(10*fabs(volt_pwr - (int)volt_pwr)), (int)volt_bed, (int)(10*fabs(volt_bed - (int)volt_bed)));
    menu_back_if_clicked();
}
#endif //defined VOLT_BED_PIN || defined VOLT_PWR_PIN

#ifdef TMC2130
static void lcd_menu_belt_status()
{
    lcd_printf_P(PSTR(ESC_H(1,0) "%S" ESC_H(2,1) "X %d" ESC_H(2,2) "Y %d" ), _i("Belt status"), eeprom_read_word((uint16_t*)(EEPROM_BELTSTATUS_X)), eeprom_read_word((uint16_t*)(EEPROM_BELTSTATUS_Y)));
    menu_back_if_clicked();
}
#endif //TMC2130

#ifdef RESUME_DEBUG 
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
#endif //RESUME_DEBUG 

static void lcd_preheat_menu()
{
  MENU_BEGIN();

  if (!wizard_active) MENU_ITEM_BACK_P(_T(MSG_MAIN));

  if (farm_mode) {
	  MENU_ITEM_FUNCTION_P(PSTR("farm   -  " STRINGIFY(FARM_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(FARM_PREHEAT_HPB_TEMP)), lcd_preheat_farm);
	  MENU_ITEM_FUNCTION_P(PSTR("nozzle -  " STRINGIFY(FARM_PREHEAT_HOTEND_TEMP) "/0"), lcd_preheat_farm_nozzle);
	  MENU_ITEM_FUNCTION_P(_T(MSG_COOLDOWN), lcd_cooldown);
	  MENU_ITEM_FUNCTION_P(PSTR("ABS    -  " STRINGIFY(ABS_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(ABS_PREHEAT_HPB_TEMP)), lcd_preheat_abs);
  } else {
	  MENU_ITEM_FUNCTION_P(PSTR("PLA  -  " STRINGIFY(PLA_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(PLA_PREHEAT_HPB_TEMP)), lcd_preheat_pla);
	  MENU_ITEM_FUNCTION_P(PSTR("PET  -  " STRINGIFY(PET_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(PET_PREHEAT_HPB_TEMP)), lcd_preheat_pet);
	  MENU_ITEM_FUNCTION_P(PSTR("ABS  -  " STRINGIFY(ABS_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(ABS_PREHEAT_HPB_TEMP)), lcd_preheat_abs);
	  MENU_ITEM_FUNCTION_P(PSTR("HIPS -  " STRINGIFY(HIPS_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(HIPS_PREHEAT_HPB_TEMP)), lcd_preheat_hips);
	  MENU_ITEM_FUNCTION_P(PSTR("PP   -  " STRINGIFY(PP_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(PP_PREHEAT_HPB_TEMP)), lcd_preheat_pp);
	  MENU_ITEM_FUNCTION_P(PSTR("FLEX -  " STRINGIFY(FLEX_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(FLEX_PREHEAT_HPB_TEMP)), lcd_preheat_flex);
	  if (!wizard_active) MENU_ITEM_FUNCTION_P(_T(MSG_COOLDOWN), lcd_cooldown);
  }
  

  MENU_END();
}

static void lcd_support_menu()
{
	typedef struct
	{	// 22bytes total
		int8_t status;                 // 1byte
		bool is_flash_air;             // 1byte
		uint8_t ip[4];                 // 4bytes
		char ip_str[3*4+3+1];          // 16bytes
	} _menu_data_t;
    static_assert(sizeof(menu_data)>= sizeof(_menu_data_t),"_menu_data_t doesn't fit into menu_data");
	_menu_data_t* _md = (_menu_data_t*)&(menu_data[0]);
    if (_md->status == 0 || lcd_draw_update == 2)
	{
        // Menu was entered or SD card status has changed (plugged in or removed).
        // Initialize its status.
        _md->status = 1;
        _md->is_flash_air = card.ToshibaFlashAir_isEnabled() && card.ToshibaFlashAir_GetIP(_md->ip);
        if (_md->is_flash_air)
            sprintf_P(_md->ip_str, PSTR("%d.%d.%d.%d"), 
                _md->ip[0], _md->ip[1], 
                _md->ip[2], _md->ip[3]);
    } else if (_md->is_flash_air && 
        _md->ip[0] == 0 && _md->ip[1] == 0 && 
        _md->ip[2] == 0 && _md->ip[3] == 0 &&
        ++ _md->status == 16)
	{
        // Waiting for the FlashAir card to get an IP address from a router. Force an update.
        _md->status = 0;
    }

  MENU_BEGIN();

  MENU_ITEM_BACK_P(_T(MSG_MAIN));

  MENU_ITEM_BACK_P(PSTR("Firmware:"));
  MENU_ITEM_BACK_P(PSTR(" " FW_VERSION_FULL));
#if (FW_DEV_VERSION != FW_VERSION_GOLD) && (FW_DEV_VERSION != FW_VERSION_RC)
  MENU_ITEM_BACK_P(PSTR(" repo " FW_REPOSITORY));
#endif
  // Ideally this block would be optimized out by the compiler.
/*  const uint8_t fw_string_len = strlen_P(FW_VERSION_STR_P());
  if (fw_string_len < 6) {
      MENU_ITEM_BACK_P(PSTR(MSG_FW_VERSION " - " FW_version));
  } else {
      MENU_ITEM_BACK_P(PSTR("FW - " FW_version));
  }*/
      
  MENU_ITEM_BACK_P(_i("prusa3d.com"));////MSG_PRUSA3D c=0 r=0
  MENU_ITEM_BACK_P(_i("forum.prusa3d.com"));////MSG_PRUSA3D_FORUM c=0 r=0
  MENU_ITEM_BACK_P(_i("howto.prusa3d.com"));////MSG_PRUSA3D_HOWTO c=0 r=0
  MENU_ITEM_BACK_P(STR_SEPARATOR);
  MENU_ITEM_BACK_P(PSTR(FILAMENT_SIZE));
  MENU_ITEM_BACK_P(PSTR(ELECTRONICS));
  MENU_ITEM_BACK_P(PSTR(NOZZLE_TYPE));
  MENU_ITEM_BACK_P(STR_SEPARATOR);
  MENU_ITEM_BACK_P(_i("Date:"));////MSG_DATE c=17 r=1
  MENU_ITEM_BACK_P(PSTR(__DATE__));

	MENU_ITEM_BACK_P(STR_SEPARATOR);
	if (mmu_enabled)
	{
		MENU_ITEM_BACK_P(_i("MMU2 connected"));
		MENU_ITEM_BACK_P(PSTR(" FW:"));
		if (((menu_item - 1) == menu_line) && lcd_draw_update)
		{
		    lcd_set_cursor(6, menu_row);
			if ((mmu_version > 0) && (mmu_buildnr > 0))
				lcd_printf_P(PSTR("%d.%d.%d-%d"), mmu_version/100, mmu_version%100/10, mmu_version%10, mmu_buildnr);
			else
				lcd_puts_P(_i("unknown")); 
		}
	}
	else
		MENU_ITEM_BACK_P(PSTR("MMU2       N/A"));


  // Show the FlashAir IP address, if the card is available.
  if (_md->is_flash_air) {
      MENU_ITEM_BACK_P(STR_SEPARATOR);
      MENU_ITEM_BACK_P(PSTR("FlashAir IP Addr:"));
///!      MENU_ITEM(back_RAM, _md->ip_str, 0);
  }

  #ifndef MK1BP
  MENU_ITEM_BACK_P(STR_SEPARATOR);
  MENU_ITEM_SUBMENU_P(_i("XYZ cal. details"), lcd_menu_xyz_y_min);////MSG_XYZ_DETAILS c=19 r=1
  MENU_ITEM_SUBMENU_P(_i("Extruder info"), lcd_menu_extruder_info);////MSG_INFO_EXTRUDER c=15 r=1

#ifdef TMC2130
  MENU_ITEM_SUBMENU_P(_i("Belt status"), lcd_menu_belt_status);////MSG_MENU_BELT_STATUS c=15 r=1
#endif //TMC2130
    
  MENU_ITEM_SUBMENU_P(_i("Temperatures"), lcd_menu_temperatures);////MSG_MENU_TEMPERATURES c=15 r=1

#if defined (VOLT_BED_PIN) || defined (VOLT_PWR_PIN)
  MENU_ITEM_SUBMENU_P(_i("Voltages"), lcd_menu_voltages);////MSG_MENU_VOLTAGES c=15 r=1
#endif //defined VOLT_BED_PIN || defined VOLT_PWR_PIN

#ifdef DEBUG_BUILD
  MENU_ITEM_SUBMENU_P(PSTR("Debug"), lcd_menu_debug);
#endif /* DEBUG_BUILD */

  #endif //MK1BP

  MENU_END();
}

void lcd_set_fan_check() {
	fans_check_enabled = !fans_check_enabled;
	eeprom_update_byte((unsigned char *)EEPROM_FAN_CHECK_ENABLED, fans_check_enabled);
}

void lcd_set_filament_autoload() {
     fsensor_autoload_set(!fsensor_autoload_enabled);
}

void lcd_set_filament_oq_meass()
{
     fsensor_oq_meassure_set(!fsensor_oq_meassure_enabled);
}


void lcd_unLoadFilament()
{

  if (degHotend0() > EXTRUDE_MINTEMP) {
	
	  enquecommand_P(PSTR("M702")); //unload filament

  } else {
	  show_preheat_nozzle_warning();
  }

  menu_back();
}


void lcd_wait_interact() {

  lcd_clear();

  lcd_set_cursor(0, 1);
#ifdef SNMM 
  lcd_puts_P(_i("Prepare new filament"));////MSG_PREPARE_FILAMENT c=20 r=1
#else
  lcd_puts_P(_i("Insert filament"));////MSG_INSERT_FILAMENT c=20 r=0
#endif
  lcd_set_cursor(0, 2);
  lcd_puts_P(_i("and press the knob"));////MSG_PRESS c=20 r=0

}


void lcd_change_success() {

  lcd_clear();

  lcd_set_cursor(0, 2);

  lcd_puts_P(_i("Change success!"));////MSG_CHANGE_SUCCESS c=0 r=0


}

static void lcd_loading_progress_bar(uint16_t loading_time_ms) { 
	
	for (int i = 0; i < 20; i++) {
		lcd_set_cursor(i, 3);
		lcd_print(".");
		//loading_time_ms/20 delay
		for (int j = 0; j < 5; j++) {
			delay_keep_alive(loading_time_ms / 100);
		}
	}
}


void lcd_loading_color() {
	//we are extruding 25mm with feedrate 200mm/min -> 7.5 seconds for whole action, 0.375 s for one character

  lcd_clear();

  lcd_set_cursor(0, 0);

  lcd_puts_P(_i("Loading color"));////MSG_LOADING_COLOR c=0 r=0
  lcd_set_cursor(0, 2);
  lcd_puts_P(_T(MSG_PLEASE_WAIT));
  lcd_loading_progress_bar((FILAMENTCHANGE_FINALFEED * 1000ul) / FILAMENTCHANGE_EFEED_FINAL); //show progress bar during filament loading slow sequence
}


void lcd_loading_filament() {


  lcd_clear();

  lcd_set_cursor(0, 0);

  lcd_puts_P(_T(MSG_LOADING_FILAMENT));
  lcd_set_cursor(0, 2);
  lcd_puts_P(_T(MSG_PLEASE_WAIT));
#ifdef SNMM
  for (int i = 0; i < 20; i++) {

    lcd_set_cursor(i, 3);
    lcd_print(".");
    for (int j = 0; j < 10 ; j++) {
      manage_heater();
      manage_inactivity(true);

      delay(153);
    }


  }
#else //SNMM
  uint16_t slow_seq_time = (FILAMENTCHANGE_FINALFEED * 1000ul) / FILAMENTCHANGE_EFEED_FINAL;
  uint16_t fast_seq_time = (FILAMENTCHANGE_FIRSTFEED * 1000ul) / FILAMENTCHANGE_EFEED_FIRST;
  lcd_loading_progress_bar(slow_seq_time + fast_seq_time); //show progress bar for total time of filament loading fast + slow sequence
#endif //SNMM
}




void lcd_alright() {
  int enc_dif = 0;
  int cursor_pos = 1;




  lcd_clear();

  lcd_set_cursor(0, 0);

  lcd_puts_P(_i("Changed correctly?"));////MSG_CORRECTLY c=20 r=0

  lcd_set_cursor(1, 1);

  lcd_puts_P(_T(MSG_YES));

  lcd_set_cursor(1, 2);

  lcd_puts_P(_i("Filament not loaded"));////MSG_NOT_LOADED c=19 r=0


  lcd_set_cursor(1, 3);
  lcd_puts_P(_i("Color not correct"));////MSG_NOT_COLOR c=0 r=0


  lcd_set_cursor(0, 1);

  lcd_print(">");


  enc_dif = lcd_encoder_diff;

  while (lcd_change_fil_state == 0) {

    manage_heater();
    manage_inactivity(true);

    if ( abs((enc_dif - lcd_encoder_diff)) > 4 ) {

      if ( (abs(enc_dif - lcd_encoder_diff)) > 1 ) {
        if (enc_dif > lcd_encoder_diff ) {
          cursor_pos --;
        }

        if (enc_dif < lcd_encoder_diff  ) {
          cursor_pos ++;
        }

        if (cursor_pos > 3) {
          cursor_pos = 3;
        }

        if (cursor_pos < 1) {
          cursor_pos = 1;
        }
        lcd_set_cursor(0, 1);
        lcd_print(" ");
        lcd_set_cursor(0, 2);
        lcd_print(" ");
        lcd_set_cursor(0, 3);
        lcd_print(" ");
        lcd_set_cursor(0, cursor_pos);
        lcd_print(">");
        enc_dif = lcd_encoder_diff;
        delay(100);
      }

    }


    if (lcd_clicked()) {

      lcd_change_fil_state = cursor_pos;
      delay(500);

    }



  };


  lcd_clear();
  lcd_return_to_status();

}

void show_preheat_nozzle_warning()
{	
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_puts_P(_T(MSG_ERROR));
    lcd_set_cursor(0, 2);
    lcd_puts_P(_T(MSG_PREHEAT_NOZZLE));
    delay(2000);
    lcd_clear();
}

void lcd_load_filament_color_check()
{
	bool clean = lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_FILAMENT_CLEAN), false, true);
	while (!clean) {
		lcd_update_enable(true);
		lcd_update(2);
		load_filament_final_feed();
		st_synchronize();
		clean = lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_FILAMENT_CLEAN), false, true);
	}
}

#ifdef FILAMENT_SENSOR
static void lcd_menu_AutoLoadFilament()
{
    if (degHotend0() > EXTRUDE_MINTEMP)
    {
        uint8_t nlines;
        lcd_display_message_fullscreen_nonBlocking_P(_i("Autoloading filament is active, just press the knob and insert filament..."),nlines);////MSG_AUTOLOADING_ENABLED c=20 r=4
    }
    else
    {
        static_assert(sizeof(menu_data)>=sizeof(ShortTimer), "ShortTimer doesn't fit into menu_data");
		ShortTimer* ptimer = (ShortTimer*)&(menu_data[0]);
        if (!ptimer->running()) ptimer->start();
        lcd_set_cursor(0, 0);
        lcd_puts_P(_T(MSG_ERROR));
        lcd_set_cursor(0, 2);
        lcd_puts_P(_T(MSG_PREHEAT_NOZZLE));
        if (ptimer->expired(2000ul)) menu_back();
    }
    menu_back_if_clicked();
}
#endif //FILAMENT_SENSOR

static void lcd_LoadFilament()
{
  if (degHotend0() > EXTRUDE_MINTEMP)
  {
      custom_message_type = CUSTOM_MSG_TYPE_F_LOAD;
      loading_flag = true;
      enquecommand_P(PSTR("M701")); //load filament
      SERIAL_ECHOLN("Loading filament");
      lcd_return_to_status();
  }
  else
  {
	  show_preheat_nozzle_warning();
  }
}


//! @brief Show filament used a print time
//!
//! If printing current print statistics are shown
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |Filament used:      |
//! |         00.00m     |
//! |Print time:         |
//! |        00h 00m 00s |
//! ----------------------
//! @endcode
//!
//! If not printing, total statistics are shown
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |Total filament :    |
//! |           000.00 m |
//! |Total print time :  |
//! |     00d :00h :00 m |
//! ----------------------
//! @endcode
void lcd_menu_statistics()
{
	if (IS_SD_PRINTING)
	{
		const float _met = ((float)total_filament_used) / (100000.f);
		const uint32_t _t = (millis() - starttime) / 1000ul;
		const int _h = _t / 3600;
		const int _m = (_t - (_h * 3600ul)) / 60ul;
		const int _s = _t - ((_h * 3600ul) + (_m * 60ul));

		lcd_printf_P(_N(
		  ESC_2J
		  "%S:"
		  ESC_H(6,1) "%8.2fm \n"
		  "%S :"
		  ESC_H(8,3) "%2dh %02dm %02ds"
		  ),
		 _i("Filament used"),
		 _met,
		 _i("Print time"),
		 _h, _m, _s
		);
		menu_back_if_clicked_fb();
	}
	else
	{
		unsigned long _filament = eeprom_read_dword((uint32_t *)EEPROM_FILAMENTUSED);
		unsigned long _time = eeprom_read_dword((uint32_t *)EEPROM_TOTALTIME); //in minutes
		uint8_t _hours, _minutes;
		uint32_t _days;
		float _filament_m = (float)_filament/100;
//		int _filament_km = (_filament >= 100000) ? _filament / 100000 : 0;
//		if (_filament_km > 0)  _filament_m = _filament - (_filament_km * 100000);
		_days = _time / 1440;
		_hours = (_time - (_days * 1440)) / 60;
		_minutes = _time - ((_days * 1440) + (_hours * 60));

		lcd_printf_P(_N(
		  ESC_2J
		  "%S :"
		  ESC_H(9,1) "%8.2f m\n"
		  "%S :\n"
		  "%7ldd :%2hhdh :%02hhd m"
		 ),
		 _i("Total filament"),
		 _filament_m,
		 _i("Total print time"),
		 _days, _hours, _minutes
		);
		KEEPALIVE_STATE(PAUSED_FOR_USER);
		while (!lcd_clicked())
		{
			manage_heater();
			manage_inactivity(true);
			delay(100);
		}
		KEEPALIVE_STATE(NOT_BUSY);
		lcd_quick_feedback();
		menu_back();
	}
}


static void _lcd_move(const char *name, int axis, int min, int max)
{
	typedef struct
	{	// 2bytes total
		bool initialized;              // 1byte
		bool endstopsEnabledPrevious;  // 1byte
	} _menu_data_t;
	static_assert(sizeof(menu_data)>= sizeof(_menu_data_t),"_menu_data_t doesn't fit into menu_data");
	_menu_data_t* _md = (_menu_data_t*)&(menu_data[0]);
	if (!_md->initialized)
	{
		_md->endstopsEnabledPrevious = enable_endstops(false);
		_md->initialized = true;
	}
	if (lcd_encoder != 0)
	{
		refresh_cmd_timeout();
		if (! planner_queue_full())
		{
			current_position[axis] += float((int)lcd_encoder) * move_menu_scale;
			if (min_software_endstops && current_position[axis] < min) current_position[axis] = min;
			if (max_software_endstops && current_position[axis] > max) current_position[axis] = max;
			lcd_encoder = 0;
			world2machine_clamp(current_position[X_AXIS], current_position[Y_AXIS]);
			plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], manual_feedrate[axis] / 60, active_extruder);
			lcd_draw_update = 1;
		}
	}
	if (lcd_draw_update)
	{
	    lcd_set_cursor(0, 1);
		menu_draw_float31(' ', name, current_position[axis]);
	}
	if (menu_leaving || LCD_CLICKED) (void)enable_endstops(_md->endstopsEnabledPrevious);
	if (LCD_CLICKED) menu_back();
}


static void lcd_move_e()
{
	if (degHotend0() > EXTRUDE_MINTEMP)
	{
		if (lcd_encoder != 0)
		{
			refresh_cmd_timeout();
			if (! planner_queue_full())
			{
				current_position[E_AXIS] += float((int)lcd_encoder) * move_menu_scale;
				lcd_encoder = 0;
				plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], manual_feedrate[E_AXIS] / 60, active_extruder);
				lcd_draw_update = 1;
			}
		}
		if (lcd_draw_update)
		{
		    lcd_set_cursor(0, 1);
			menu_draw_float31(' ', PSTR("Extruder"), current_position[E_AXIS]);
		}
		if (LCD_CLICKED) menu_back();
	}
	else
	{
		show_preheat_nozzle_warning();
		lcd_return_to_status();
	}
}


//@brief Show measured Y distance of front calibration points from Y_MIN_POS
//If those points are detected too close to edge of reachable area, their confidence is lowered.
//This functionality is applied more often for MK2 printers.
static void lcd_menu_xyz_y_min()
{
//|01234567890123456789|
//|Y distance from min:|
//|--------------------|
//|Left:      N/A      |
//|Right:     N/A      |
//----------------------
	float distanceMin[2];
    count_xyz_details(distanceMin);
	lcd_printf_P(_N(
	  ESC_H(0,0)
	  "%S:\n"
	  "%S\n"
	  "%S:\n"
	  "%S:"
	 ),
	 _i("Y distance from min"),
	 separator,
	 _i("Left"),
	 _i("Right")
	);
	for (uint8_t i = 0; i < 2; i++)
	{
		lcd_set_cursor(11,2+i);
		if (distanceMin[i] >= 200) lcd_puts_P(_N("N/A"));
		else lcd_printf_P(_N("%6.2fmm"), distanceMin[i]);
	}
    if (lcd_clicked())
        menu_goto(lcd_menu_xyz_skew, 0, true, true);
}

//@brief Show measured axis skewness
float _deg(float rad)
{
	return rad * 180 / M_PI;
}

static void lcd_menu_xyz_skew()
{
//|01234567890123456789|
//|Measured skew:  N/A |
//|--------------------|
//|Slight skew:   0.12d|
//|Severe skew:   0.25d|
//----------------------
    float angleDiff = eeprom_read_float((float*)(EEPROM_XYZ_CAL_SKEW));
	lcd_printf_P(_N(
	  ESC_H(0,0)
	  "%S:\n"
	  "%S\n"
	  "%S:  %5.2f\x01\n"
	  "%S:  %5.2f\x01"
	 ),
	 _i("Measured skew"),
	 separator,
	 _i("Slight skew"), _deg(bed_skew_angle_mild),
	 _i("Severe skew"), _deg(bed_skew_angle_extreme)
	);
	if (angleDiff < 100)
		lcd_printf_P(_N(ESC_H(15,0)"%4.2f\x01"), _deg(angleDiff));
	else
		lcd_puts_P(_N(ESC_H(15,0)"N/A"));
    if (lcd_clicked())
        menu_goto(lcd_menu_xyz_offset, 0, true, true);
}
/**
 * @brief Show measured bed offset from expected position
 */
static void lcd_menu_xyz_offset()
{
    lcd_set_cursor(0,0);
    lcd_puts_P(_i("[0;0] point offset"));////MSG_MEASURED_OFFSET c=0 r=0
    lcd_puts_at_P(0, 1, separator);
    lcd_puts_at_P(0, 2, PSTR("X"));
    lcd_puts_at_P(0, 3, PSTR("Y"));

    float vec_x[2];
    float vec_y[2];
    float cntr[2];
    world2machine_read_valid(vec_x, vec_y, cntr);

    for (int i = 0; i < 2; i++)
    {
        lcd_puts_at_P(11, i + 2, PSTR(""));
        lcd_print(cntr[i]);
        lcd_puts_at_P((cntr[i] < 0) ? 17 : 16, i + 2, PSTR("mm"));
    }
    menu_back_if_clicked();
}

// Save a single axis babystep value.
void EEPROM_save_B(int pos, int* value)
{
  eeprom_update_byte((unsigned char*)pos, (unsigned char)((*value) & 0xff));
  eeprom_update_byte((unsigned char*)pos + 1, (unsigned char)((*value) >> 8));
}

// Read a single axis babystep value.
void EEPROM_read_B(int pos, int* value)
{
  *value = (int)eeprom_read_byte((unsigned char*)pos) | (int)(eeprom_read_byte((unsigned char*)pos + 1) << 8);
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
	typedef struct
	{	// 19bytes total
		int8_t status;                 // 1byte
		int babystepMem[3];            // 6bytes
		float babystepMemMM[3];        // 12bytes
	} _menu_data_t;
	static_assert(sizeof(menu_data)>= sizeof(_menu_data_t),"_menu_data_t doesn't fit into menu_data");
	_menu_data_t* _md = (_menu_data_t*)&(menu_data[0]);
	if (_md->status == 0)
	{
		// Menu was entered.
		// Initialize its status.
		_md->status = 1;
		check_babystep();

		EEPROM_read_B(EEPROM_BABYSTEP_X, &_md->babystepMem[0]);
		EEPROM_read_B(EEPROM_BABYSTEP_Y, &_md->babystepMem[1]);
		EEPROM_read_B(EEPROM_BABYSTEP_Z, &_md->babystepMem[2]);

		// same logic as in babystep_load
	    if (calibration_status() >= CALIBRATION_STATUS_LIVE_ADJUST)
			_md->babystepMem[2] = 0;

		_md->babystepMemMM[0] = _md->babystepMem[0]/cs.axis_steps_per_unit[X_AXIS];
		_md->babystepMemMM[1] = _md->babystepMem[1]/cs.axis_steps_per_unit[Y_AXIS];
		_md->babystepMemMM[2] = _md->babystepMem[2]/cs.axis_steps_per_unit[Z_AXIS];
		lcd_draw_update = 1;
		//SERIAL_ECHO("Z baby step: ");
		//SERIAL_ECHO(_md->babystepMem[2]);
		// Wait 90 seconds before closing the live adjust dialog.
		lcd_timeoutToStatus.start();
	}

	if (lcd_encoder != 0) 
	{
		if (homing_flag) lcd_encoder = 0;
		_md->babystepMem[axis] += (int)lcd_encoder;
		if (axis == 2)
		{
			if (_md->babystepMem[axis] < Z_BABYSTEP_MIN) _md->babystepMem[axis] = Z_BABYSTEP_MIN; //-3999 -> -9.99 mm
			else if (_md->babystepMem[axis] > Z_BABYSTEP_MAX) _md->babystepMem[axis] = Z_BABYSTEP_MAX; //0
			else
			{
				CRITICAL_SECTION_START
				babystepsTodo[axis] += (int)lcd_encoder;
				CRITICAL_SECTION_END		
			}
		}
		_md->babystepMemMM[axis] = _md->babystepMem[axis]/cs.axis_steps_per_unit[axis]; 
		delay(50);
		lcd_encoder = 0;
		lcd_draw_update = 1;
	}
	if (lcd_draw_update)
	{
	    lcd_set_cursor(0, 1);
		menu_draw_float13(' ', msg, _md->babystepMemMM[axis]);
	}
	if (LCD_CLICKED || menu_leaving)
	{
		// Only update the EEPROM when leaving the menu.
		EEPROM_save_B(
		(axis == X_AXIS) ? EEPROM_BABYSTEP_X : ((axis == Y_AXIS) ? EEPROM_BABYSTEP_Y : EEPROM_BABYSTEP_Z),
		&_md->babystepMem[axis]);
		if(Z_AXIS == axis) calibration_status_store(CALIBRATION_STATUS_CALIBRATED);
	}
	if (LCD_CLICKED) menu_back();
}


static void lcd_babystep_z()
{
	_lcd_babystep(Z_AXIS, (_i("Adjusting Z")));////MSG_BABYSTEPPING_Z c=20 r=0
}


typedef struct
{	// 12bytes + 9bytes = 21bytes total
    menu_data_edit_t reserved; //12 bytes reserved for number editing functions
	int8_t status;                   // 1byte
	int16_t left;                    // 2byte
	int16_t right;                   // 2byte
	int16_t front;                   // 2byte
	int16_t rear;                    // 2byte
} _menu_data_adjust_bed_t;
static_assert(sizeof(menu_data)>= sizeof(_menu_data_adjust_bed_t),"_menu_data_adjust_bed_t doesn't fit into menu_data");

void lcd_adjust_bed_reset(void)
{
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_VALID, 1);
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_LEFT , 0);
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_RIGHT, 0);
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_FRONT, 0);
	eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_REAR , 0);
	_menu_data_adjust_bed_t* _md = (_menu_data_adjust_bed_t*)&(menu_data[0]);
	_md->status = 0;
}

#define BED_ADJUSTMENT_UM_MAX 50

void lcd_adjust_bed(void)
{
	_menu_data_adjust_bed_t* _md = (_menu_data_adjust_bed_t*)&(menu_data[0]);
    if (_md->status == 0)
	{
        // Menu was entered.
		_md->left  = 0;
		_md->right = 0;
		_md->front = 0;
		_md->rear  = 0;
        if (eeprom_read_byte((unsigned char*)EEPROM_BED_CORRECTION_VALID) == 1)
		{
			_md->left  = eeprom_read_int8((unsigned char*)EEPROM_BED_CORRECTION_LEFT);
			_md->right = eeprom_read_int8((unsigned char*)EEPROM_BED_CORRECTION_RIGHT);
			_md->front = eeprom_read_int8((unsigned char*)EEPROM_BED_CORRECTION_FRONT);
			_md->rear  = eeprom_read_int8((unsigned char*)EEPROM_BED_CORRECTION_REAR);
		}
        _md->status = 1;
    }
    MENU_BEGIN();
	// leaving menu - this condition must be immediately before MENU_ITEM_BACK_P
	if (((menu_item == menu_line) && menu_clicked && (lcd_encoder == menu_item)) || menu_leaving)
	{
        eeprom_update_int8((unsigned char*)EEPROM_BED_CORRECTION_LEFT,  _md->left);
        eeprom_update_int8((unsigned char*)EEPROM_BED_CORRECTION_RIGHT, _md->right);
        eeprom_update_int8((unsigned char*)EEPROM_BED_CORRECTION_FRONT, _md->front);
        eeprom_update_int8((unsigned char*)EEPROM_BED_CORRECTION_REAR,  _md->rear);
        eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_VALID, 1);
	}
	MENU_ITEM_BACK_P(_T(MSG_SETTINGS));
	MENU_ITEM_EDIT_int3_P(_i("Left side [um]"),  &_md->left,  -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);////MSG_BED_CORRECTION_LEFT c=14 r=1
    MENU_ITEM_EDIT_int3_P(_i("Right side[um]"), &_md->right, -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);////MSG_BED_CORRECTION_RIGHT c=14 r=1
    MENU_ITEM_EDIT_int3_P(_i("Front side[um]"), &_md->front, -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);////MSG_BED_CORRECTION_FRONT c=14 r=1
    MENU_ITEM_EDIT_int3_P(_i("Rear side [um]"),  &_md->rear,  -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);////MSG_BED_CORRECTION_REAR c=14 r=1
    MENU_ITEM_FUNCTION_P(_i("Reset"), lcd_adjust_bed_reset);////MSG_BED_CORRECTION_RESET c=0 r=0
    MENU_END();
}

void pid_extruder()
{
	lcd_clear();
	lcd_set_cursor(1, 0);
	lcd_puts_P(_i("Set temperature:"));////MSG_SET_TEMPERATURE c=19 r=1
	pid_temp += int(lcd_encoder);
	if (pid_temp > HEATER_0_MAXTEMP) pid_temp = HEATER_0_MAXTEMP;
	if (pid_temp < HEATER_0_MINTEMP) pid_temp = HEATER_0_MINTEMP;
	lcd_encoder = 0;
	lcd_set_cursor(1, 2);
	lcd_print(ftostr3(pid_temp));
	if (lcd_clicked()) {
		lcd_commands_type = LCD_COMMAND_PID_EXTRUDER;
		lcd_return_to_status();
		lcd_update(2);
	}

}
/*
void lcd_adjust_z() {
  int enc_dif = 0;
  int cursor_pos = 1;
  int fsm = 0;




  lcd_clear();
  lcd_set_cursor(0, 0);
  lcd_puts_P(_i("Auto adjust Z?"));////MSG_ADJUSTZ c=0 r=0
  lcd_set_cursor(1, 1);
  lcd_puts_P(_T(MSG_YES));

  lcd_set_cursor(1, 2);

  lcd_puts_P(_T(MSG_NO));

  lcd_set_cursor(0, 1);

  lcd_print(">");


  enc_dif = lcd_encoder_diff;

  while (fsm == 0) {

    manage_heater();
    manage_inactivity(true);

    if ( abs((enc_dif - lcd_encoder_diff)) > 4 ) {

      if ( (abs(enc_dif - lcd_encoder_diff)) > 1 ) {
        if (enc_dif > lcd_encoder_diff ) {
          cursor_pos --;
        }

        if (enc_dif < lcd_encoder_diff  ) {
          cursor_pos ++;
        }

        if (cursor_pos > 2) {
          cursor_pos = 2;
        }

        if (cursor_pos < 1) {
          cursor_pos = 1;
        }
        lcd_set_cursor(0, 1);
        lcd_print(" ");
        lcd_set_cursor(0, 2);
        lcd_print(" ");
        lcd_set_cursor(0, cursor_pos);
        lcd_print(">");
        enc_dif = lcd_encoder_diff;
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

  lcd_clear();
  lcd_return_to_status();

}*/

bool lcd_wait_for_pinda(float temp) {
	lcd_set_custom_characters_degree();
	setAllTargetHotends(0);
	setTargetBed(0);
	LongTimer pinda_timeout;
	pinda_timeout.start();
	bool target_temp_reached = true;

	while (current_temperature_pinda > temp){
		lcd_display_message_fullscreen_P(_i("Waiting for PINDA probe cooling"));////MSG_WAITING_TEMP_PINDA c=20 r=3

		lcd_set_cursor(0, 4);
		lcd_print(LCD_STR_THERMOMETER[0]);
		lcd_print(ftostr3(current_temperature_pinda));
		lcd_print("/");
		lcd_print(ftostr3(temp));
		lcd_print(LCD_STR_DEGREE);
		delay_keep_alive(1000);
		serialecho_temperatures();
		if (pinda_timeout.expired(8 * 60 * 1000ul)) { //PINDA cooling from 60 C to 35 C takes about 7 minutes
			target_temp_reached = false;
			break;
		}
	}
	lcd_set_custom_characters_arrows();
	lcd_update_enable(true);
	return target_temp_reached;
}

void lcd_wait_for_heater() {
		lcd_display_message_fullscreen_P(_T(MSG_WIZARD_HEATING));
		lcd_set_degree();
		lcd_set_cursor(0, 4);
		lcd_print(LCD_STR_THERMOMETER[0]);
		lcd_print(ftostr3(degHotend(active_extruder)));
		lcd_print("/");
		lcd_print(ftostr3(degTargetHotend(active_extruder)));
		lcd_print(LCD_STR_DEGREE);
}

void lcd_wait_for_cool_down() {
	lcd_set_custom_characters_degree();
	setAllTargetHotends(0);
	setTargetBed(0);
	while ((degHotend(0)>MAX_HOTEND_TEMP_CALIBRATION) || (degBed() > MAX_BED_TEMP_CALIBRATION)) {
		lcd_display_message_fullscreen_P(_i("Waiting for nozzle and bed cooling"));////MSG_WAITING_TEMP c=20 r=3

		lcd_set_cursor(0, 4);
		lcd_print(LCD_STR_THERMOMETER[0]);
		lcd_print(ftostr3(degHotend(0)));
		lcd_print("/0");		
		lcd_print(LCD_STR_DEGREE);

		lcd_set_cursor(9, 4);
		lcd_print(LCD_STR_BEDTEMP[0]);
		lcd_print(ftostr3(degBed()));
		lcd_print("/0");		
		lcd_print(LCD_STR_DEGREE);
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
    // Don't know where we are. Let's claim we are Z=0, so the soft end stops will not be triggered when moving up.
    current_position[Z_AXIS] = 0;
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);

    // Until confirmed by the confirmation dialog.
    for (;;) {
        unsigned long previous_millis_cmd = millis();
        const char   *msg                 = only_z ? _i("Calibrating Z. Rotate the knob to move the Z carriage up to the end stoppers. Click when done.") : _i("Calibrating XYZ. Rotate the knob to move the Z carriage up to the end stoppers. Click when done.");////MSG_MOVE_CARRIAGE_TO_THE_TOP c=20 r=8////MSG_MOVE_CARRIAGE_TO_THE_TOP_Z c=20 r=8
        const char   *msg_next            = lcd_display_message_fullscreen_P(msg);
        const bool    multi_screen        = msg_next != NULL;
        unsigned long previous_millis_msg = millis();
        // Until the user finishes the z up movement.
        lcd_encoder_diff = 0;
        lcd_encoder = 0;
        for (;;) {
//          if (millis() - previous_millis_cmd > LCD_TIMEOUT_TO_STATUS)
//             goto canceled;
            manage_heater();
            manage_inactivity(true);
            if (abs(lcd_encoder_diff) >= ENCODER_PULSES_PER_STEP) {
                delay(50);
                previous_millis_cmd = millis();
                lcd_encoder += abs(lcd_encoder_diff / ENCODER_PULSES_PER_STEP);
                lcd_encoder_diff = 0;
                if (! planner_queue_full()) {
                    // Only move up, whatever direction the user rotates the encoder.
                    current_position[Z_AXIS] += fabs(lcd_encoder);
                    lcd_encoder = 0;
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
        // Let the user confirm, that the Z carriage is at the top end stoppers.
        int8_t result = lcd_show_fullscreen_message_yes_no_and_wait_P(_i("Are left and right Z~carriages all up?"), false);////MSG_CONFIRM_CARRIAGE_AT_THE_TOP c=20 r=2
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
    lcd_set_cursor(0, 0);
    const char *msgend = msg;
    uint8_t row = 0;
    bool multi_screen = false;
    for (; row < 4; ++ row) {
        while (pgm_is_whitespace(msg))
            ++ msg;
        if (pgm_read_byte(msg) == 0)
            // End of the message.
            break;
        lcd_set_cursor(0, row);
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
            lcd_print(c);
        }
    }

    if (multi_screen) {
        // Display the "next screen" indicator character.
        // lcd_set_custom_characters_arrows();
        lcd_set_custom_characters_nextpage();
        lcd_set_cursor(19, 3);
        // Display the down arrow.
        lcd_print(char(1));
    }

    nlines = row;
    return multi_screen ? msgend : NULL;
}

const char* lcd_display_message_fullscreen_P(const char *msg, uint8_t &nlines)
{
    // Disable update of the screen by the usual lcd_update(0) routine.
    lcd_update_enable(false);
    lcd_clear();
//	uint8_t nlines;
    return lcd_display_message_fullscreen_nonBlocking_P(msg, nlines);
}
const char* lcd_display_message_fullscreen_P(const char *msg) 
{
  uint8_t nlines;
  return lcd_display_message_fullscreen_P(msg, nlines);
}


/**
 * @brief show full screen message and wait
 *
 * This function is blocking.
 * @param msg message to be displayed from PROGMEM
 */
void lcd_show_fullscreen_message_and_wait_P(const char *msg)
{
    LcdUpdateDisabler lcdUpdateDisabler;
    const char *msg_next = lcd_display_message_fullscreen_P(msg);
    bool multi_screen = msg_next != NULL;
	lcd_set_custom_characters_nextpage();
	KEEPALIVE_STATE(PAUSED_FOR_USER);
	// Until confirmed by a button click.
	for (;;) {
		if (!multi_screen) {
			lcd_set_cursor(19, 3);
			// Display the confirm char.
			lcd_print(char(2));
		}
        // Wait for 5 seconds before displaying the next text.
        for (uint8_t i = 0; i < 100; ++ i) {
            delay_keep_alive(50);
            if (lcd_clicked()) {
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

				lcd_set_cursor(19, 3);
				// Display the confirm char.
				lcd_print(char(2));
			}
        }
    }
}

bool lcd_wait_for_click_delay(uint16_t nDelay)
// nDelay :: timeout [s] (0 ~ no timeout)
// true ~ clicked, false ~ delayed
{
bool bDelayed;
long nTime0 = millis()/1000;

	KEEPALIVE_STATE(PAUSED_FOR_USER);
    for (;;) {
        manage_heater();
        manage_inactivity(true);
        bDelayed = ((millis()/1000-nTime0) > nDelay);
        bDelayed = (bDelayed && (nDelay != 0));   // 0 ~ no timeout, always waiting for click
        if (lcd_clicked() || bDelayed) {
			KEEPALIVE_STATE(IN_HANDLER);
            return(!bDelayed);
        }
    }
}

void lcd_wait_for_click()
{
lcd_wait_for_click_delay(0);
}

//! @brief Show multiple screen message with yes and no possible choices and wait with possible timeout
//! @param msg Message to show
//! @param allow_timeouting if true, allows time outing of the screen
//! @param default_yes if true, yes choice is selected by default, otherwise no choice is preselected
//! @retval 1 yes choice selected by user
//! @retval 0 no choice selected by user
//! @retval -1 screen timed out
int8_t lcd_show_multiscreen_message_yes_no_and_wait_P(const char *msg, bool allow_timeouting, bool default_yes) //currently just max. n*4 + 3 lines supported (set in language header files)
{
    return lcd_show_multiscreen_message_two_choices_and_wait_P(msg, allow_timeouting, default_yes, _T(MSG_YES), _T(MSG_NO));
}
//! @brief Show multiple screen message with two possible choices and wait with possible timeout
//! @param msg Message to show
//! @param allow_timeouting if true, allows time outing of the screen
//! @param default_first if true, fist choice is selected by default, otherwise second choice is preselected
//! @param first_choice text caption of first possible choice
//! @param second_choice text caption of second possible choice
//! @retval 1 first choice selected by user
//! @retval 0 second choice selected by user
//! @retval -1 screen timed out
int8_t lcd_show_multiscreen_message_two_choices_and_wait_P(const char *msg, bool allow_timeouting, bool default_first,
        const char *first_choice, const char *second_choice)
{
	const char *msg_next = lcd_display_message_fullscreen_P(msg);
	bool multi_screen = msg_next != NULL;
	bool yes = default_first ? true : false;

	// Wait for user confirmation or a timeout.
	unsigned long previous_millis_cmd = millis();
	int8_t        enc_dif = lcd_encoder_diff;
	//KEEPALIVE_STATE(PAUSED_FOR_USER);
	for (;;) {
		for (uint8_t i = 0; i < 100; ++i) {
			delay_keep_alive(50);
			if (allow_timeouting && millis() - previous_millis_cmd > LCD_TIMEOUT_TO_STATUS)
				return -1;
			manage_heater();
			manage_inactivity(true);

			if (abs(enc_dif - lcd_encoder_diff) > 4) {
				if (msg_next == NULL) {
					lcd_set_cursor(0, 3);
					if (enc_dif < lcd_encoder_diff && yes) {
						lcd_puts_P((PSTR(" ")));
						lcd_set_cursor(7, 3);
						lcd_puts_P((PSTR(">")));
						yes = false;
					}
					else if (enc_dif > lcd_encoder_diff && !yes) {
						lcd_puts_P((PSTR(">")));
						lcd_set_cursor(7, 3);
						lcd_puts_P((PSTR(" ")));
						yes = true;
					}
					enc_dif = lcd_encoder_diff;
				}
				else {
					break; //turning knob skips waiting loop
				}
			}
			if (lcd_clicked()) {
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
			lcd_set_cursor(0, 3);
			if (yes) lcd_puts_P(PSTR(">"));
			lcd_set_cursor(1, 3);
			lcd_puts_P(first_choice);
			lcd_set_cursor(7, 3);
			if (!yes) lcd_puts_P(PSTR(">"));
			lcd_set_cursor(8, 3);
			lcd_puts_P(second_choice);
		}
	}
}

//! @brief Show single screen message with yes and no possible choices and wait with possible timeout
//! @param msg Message to show
//! @param allow_timeouting if true, allows time outing of the screen
//! @param default_yes if true, yes choice is selected by default, otherwise no choice is preselected
//! @retval 1 yes choice selected by user
//! @retval 0 no choice selected by user
//! @retval -1 screen timed out
int8_t lcd_show_fullscreen_message_yes_no_and_wait_P(const char *msg, bool allow_timeouting, bool default_yes)
{

	lcd_display_message_fullscreen_P(msg);
	
	if (default_yes) {
		lcd_set_cursor(0, 2);
		lcd_puts_P(PSTR(">"));
		lcd_puts_P(_T(MSG_YES));
		lcd_set_cursor(1, 3);
		lcd_puts_P(_T(MSG_NO));
	}
	else {
		lcd_set_cursor(1, 2);
		lcd_puts_P(_T(MSG_YES));
		lcd_set_cursor(0, 3);
		lcd_puts_P(PSTR(">"));
		lcd_puts_P(_T(MSG_NO));
	}
	bool yes = default_yes ? true : false;

	// Wait for user confirmation or a timeout.
	unsigned long previous_millis_cmd = millis();
	int8_t        enc_dif = lcd_encoder_diff;
	KEEPALIVE_STATE(PAUSED_FOR_USER);
	for (;;) {
		if (allow_timeouting && millis() - previous_millis_cmd > LCD_TIMEOUT_TO_STATUS)
			return -1;
		manage_heater();
		manage_inactivity(true);
		if (abs(enc_dif - lcd_encoder_diff) > 4) {
			lcd_set_cursor(0, 2);
				if (enc_dif < lcd_encoder_diff && yes) {
					lcd_puts_P((PSTR(" ")));
					lcd_set_cursor(0, 3);
					lcd_puts_P((PSTR(">")));
					yes = false;
				}
				else if (enc_dif > lcd_encoder_diff && !yes) {
					lcd_puts_P((PSTR(">")));
					lcd_set_cursor(0, 3);
					lcd_puts_P((PSTR(" ")));
					yes = true;
				}
				enc_dif = lcd_encoder_diff;
		}
		if (lcd_clicked()) {
			KEEPALIVE_STATE(IN_HANDLER);
			return yes;
		}
	}
}

void lcd_bed_calibration_show_result(BedSkewOffsetDetectionResultType result, uint8_t point_too_far_mask)
{
    const char *msg = NULL;
    if (result == BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND) {
        lcd_show_fullscreen_message_and_wait_P(_i("XYZ calibration failed. Bed calibration point was not found."));////MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND c=20 r=8
    } else if (result == BED_SKEW_OFFSET_DETECTION_FITTING_FAILED) {
        if (point_too_far_mask == 0)
            msg = _T(MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED);
        else if (point_too_far_mask == 2 || point_too_far_mask == 7)
            // Only the center point or all the three front points.
            msg = _i("XYZ calibration failed. Front calibration points not reachable.");////MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR c=20 r=8
        else if ((point_too_far_mask & 1) == 0)
            // The right and maybe the center point out of reach.
            msg = _i("XYZ calibration failed. Right front calibration point not reachable.");////MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR c=20 r=8
        else
            // The left and maybe the center point out of reach.
            msg = _i("XYZ calibration failed. Left front calibration point not reachable.");////MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR c=20 r=8
        lcd_show_fullscreen_message_and_wait_P(msg);
    } else {
        if (point_too_far_mask != 0) {
            if (point_too_far_mask == 2 || point_too_far_mask == 7)
                // Only the center point or all the three front points.
                msg = _i("XYZ calibration compromised. Front calibration points not reachable.");////MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_BOTH_FAR c=20 r=8
            else if ((point_too_far_mask & 1) == 0)
                // The right and maybe the center point out of reach.
                msg = _i("XYZ calibration compromised. Right front calibration point not reachable.");////MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_RIGHT_FAR c=20 r=8
            else
                // The left and maybe the center point out of reach.
                msg = _i("XYZ calibration compromised. Left front calibration point not reachable.");////MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR c=20 r=8
            lcd_show_fullscreen_message_and_wait_P(msg);
        }
        if (point_too_far_mask == 0 || result > 0) {
            switch (result) {
                default:
                    // should not happen
                    msg = _T(MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED);
                    break;
                case BED_SKEW_OFFSET_DETECTION_PERFECT:
                    msg = _i("XYZ calibration ok. X/Y axes are perpendicular. Congratulations!");////MSG_BED_SKEW_OFFSET_DETECTION_PERFECT c=20 r=8
                    break;
                case BED_SKEW_OFFSET_DETECTION_SKEW_MILD:
                    msg = _i("XYZ calibration all right. X/Y axes are slightly skewed. Good job!");////MSG_BED_SKEW_OFFSET_DETECTION_SKEW_MILD c=20 r=8
                    break;
                case BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME:
                    msg = _i("XYZ calibration all right. Skew will be corrected automatically.");////MSG_BED_SKEW_OFFSET_DETECTION_SKEW_EXTREME c=20 r=8
                    break;
            }
            lcd_show_fullscreen_message_and_wait_P(msg);
        }
    }
}

void lcd_temp_cal_show_result(bool result) {
	
	custom_message_type = CUSTOM_MSG_TYPE_STATUS;
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
		lcd_show_fullscreen_message_and_wait_P(_T(MSG_TEMP_CALIBRATION_DONE));
		temp_cal_active = true;
		eeprom_update_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE, 1);
	}
	else {
		eeprom_update_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, 0);
		SERIAL_ECHOLNPGM("Temperature calibration failed. Continue with pressing the knob.");
		lcd_show_fullscreen_message_and_wait_P(_i("Temperature calibration failed"));////MSG_TEMP_CAL_FAILED c=20 r=8
		temp_cal_active = false;
		eeprom_update_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE, 0);
	}
	lcd_update_enable(true);
	lcd_update(2);
}

static void lcd_show_end_stops() {
	lcd_set_cursor(0, 0);
	lcd_puts_P((PSTR("End stops diag")));
	lcd_set_cursor(0, 1);
	lcd_puts_P((READ(X_MIN_PIN) ^ (bool)X_MIN_ENDSTOP_INVERTING) ? (PSTR("X1")) : (PSTR("X0")));
	lcd_set_cursor(0, 2);
	lcd_puts_P((READ(Y_MIN_PIN) ^ (bool)Y_MIN_ENDSTOP_INVERTING) ? (PSTR("Y1")) : (PSTR("Y0")));
	lcd_set_cursor(0, 3);
	lcd_puts_P((READ(Z_MIN_PIN) ^ (bool)Z_MIN_ENDSTOP_INVERTING) ? (PSTR("Z1")) : (PSTR("Z0")));
}

#ifndef TMC2130
static void menu_show_end_stops() {
    lcd_show_end_stops();
    if (LCD_CLICKED) menu_back();
}
#endif // not defined TMC2130

// Lets the user move the Z carriage up to the end stoppers.
// When done, it sets the current Z to Z_MAX_POS and returns true.
// Otherwise the Z calibration is not changed and false is returned.
void lcd_diag_show_end_stops()
{
    lcd_clear();
    for (;;) {
        manage_heater();
        manage_inactivity(true);
        lcd_show_end_stops();
        if (lcd_clicked()) {
            break;
        }
    }
    lcd_clear();
    lcd_return_to_status();
}

#ifdef TMC2130
static void lcd_show_pinda_state()
{
lcd_set_cursor(0, 0);
lcd_puts_P((PSTR("P.I.N.D.A. state")));
lcd_set_cursor(0, 2);
lcd_puts_P(READ(Z_MIN_PIN)?(PSTR("Z1 (LED off)")):(PSTR("Z0 (LED on) "))); // !!! both strings must have same length (due to dynamic refreshing)
}

static void menu_show_pinda_state()
{
lcd_timeoutToStatus.stop();
lcd_show_pinda_state();
if(LCD_CLICKED)
     {
     lcd_timeoutToStatus.start();
     menu_back();
     }
}
#endif // defined TMC2130



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
    
    
    
    
    lcd_clear();
    
    lcd_set_cursor(0, 0);
    
    lcd_puts_P(_i("Pick print"));////MSG_PICK_Z c=0 r=0
    
    
    lcd_set_cursor(3, 2);
    
    lcd_print("1");
    
    lcd_set_cursor(3, 3);
    
    lcd_print("2");
    
    lcd_set_cursor(12, 2);
    
    lcd_print("3");
    
    lcd_set_cursor(12, 3);
    
    lcd_print("4");
    
    lcd_set_cursor(1, 2);
    
    lcd_print(">");
    
    
    enc_dif = lcd_encoder_diff;
    
    while (fsm == 0) {
        
        manage_heater();
        manage_inactivity(true);
        
        if ( abs((enc_dif - lcd_encoder_diff)) > 4 ) {
            
            if ( (abs(enc_dif - lcd_encoder_diff)) > 1 ) {
                if (enc_dif > lcd_encoder_diff ) {
                    cursor_pos --;
                }
                
                if (enc_dif < lcd_encoder_diff  ) {
                    cursor_pos ++;
                }
                
                if (cursor_pos > 4) {
                    cursor_pos = 4;
                }
                
                if (cursor_pos < 1) {
                    cursor_pos = 1;
                }

                
                lcd_set_cursor(1, 2);
                lcd_print(" ");
                lcd_set_cursor(1, 3);
                lcd_print(" ");
                lcd_set_cursor(10, 2);
                lcd_print(" ");
                lcd_set_cursor(10, 3);
                lcd_print(" ");
                
                if (cursor_pos < 3) {
                    lcd_set_cursor(1, cursor_pos+1);
                    lcd_print(">");
                }else{
                    lcd_set_cursor(10, cursor_pos-1);
                    lcd_print(">");
                }
                
   
                enc_dif = lcd_encoder_diff;
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
    
    lcd_clear();
    lcd_return_to_status();
}
*/
void lcd_move_menu_axis()
{
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_SETTINGS));
	MENU_ITEM_SUBMENU_P(_i("Move X"), lcd_move_x);////MSG_MOVE_X c=0 r=0
	MENU_ITEM_SUBMENU_P(_i("Move Y"), lcd_move_y);////MSG_MOVE_Y c=0 r=0
	MENU_ITEM_SUBMENU_P(_i("Move Z"), lcd_move_z);////MSG_MOVE_Z c=0 r=0
	MENU_ITEM_SUBMENU_P(_i("Extruder"), lcd_move_e);////MSG_MOVE_E c=0 r=0
	MENU_END();
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
}
#endif //SDCARD_SORT_ALPHA

#ifdef TMC2130
static void lcd_crash_mode_info()
{
	lcd_update_enable(true);
	static uint32_t tim = 0;
	if ((tim + 1000) < millis())
	{
		fputs_P(_i("\x1b[2JCrash detection can\x1b[1;0Hbe turned on only in\x1b[2;0HNormal mode"), lcdout);////MSG_CRASH_DET_ONLY_IN_NORMAL c=20 r=4
		tim = millis();
	}
    menu_back_if_clicked();
}

static void lcd_crash_mode_info2()
{
	lcd_update_enable(true);
	static uint32_t tim = 0;
	if ((tim + 1000) < millis())
	{
		fputs_P(_i("\x1b[2JWARNING:\x1b[1;0HCrash detection\x1b[2;0Hdisabled in\x1b[3;0HStealth mode"), lcdout);////MSG_CRASH_DET_STEALTH_FORCE_OFF c=20 r=4
		tim = millis();
	}
    menu_back_if_clicked();
}
#endif //TMC2130

#ifdef FILAMENT_SENSOR
static void lcd_filament_autoload_info()
{
uint8_t nlines;
	lcd_update_enable(true);
	static uint32_t tim = 0;
	if ((tim + 1000) < millis())
	{
          lcd_display_message_fullscreen_nonBlocking_P(_i("Autoloading filament available only when filament sensor is turned on..."), nlines); ////MSG_AUTOLOADING_ONLY_IF_FSENS_ON c=20 r=4
		tim = millis();
	}
    menu_back_if_clicked();
}

static void lcd_fsensor_fail()
{
uint8_t nlines;
	lcd_update_enable(true);
	static uint32_t tim = 0;
	if ((tim + 1000) < millis())
	{
          lcd_display_message_fullscreen_nonBlocking_P(_i("ERROR: Filament sensor is not responding, please check connection."), nlines);////MSG_FSENS_NOT_RESPONDING c=20 r=4
		tim = millis();
	}
    menu_back_if_clicked();
}
#endif //FILAMENT_SENSOR

//-//
static void lcd_sound_state_set(void)
{
Sound_CycleState();
}

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
	update_mode_profile();
	tmc2130_init();
  // We may have missed a stepper timer interrupt due to the time spent in tmc2130_init.
  // Be safe than sorry, reset the stepper timer before re-enabling interrupts.
  st_reset_timer();
  sei();
#endif //TMC2130
  st_current_init();
#ifdef TMC2130
  if (CrashDetectMenu && (SilentModeMenu != SILENT_MODE_NORMAL))
	  menu_submenu(lcd_crash_mode_info2);
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
	if (IS_SD_PRINTING || is_usb_printing || (lcd_commands_type == LCD_COMMAND_V2_CAL)) menu_goto(lcd_tune_menu, 9, true, true);
	else menu_goto(lcd_settings_menu, 9, true, true);
    
}
#endif //TMC2130
 

#ifdef FILAMENT_SENSOR
static void lcd_fsensor_state_set()
{
	FSensorStateMenu = !FSensorStateMenu; //set also from fsensor_enable() and fsensor_disable()
	if (!FSensorStateMenu) {
		fsensor_disable();
		if (fsensor_autoload_enabled && !mmu_enabled)
			menu_submenu(lcd_filament_autoload_info);
	}
	else {
		fsensor_enable();
		if (fsensor_not_responding && !mmu_enabled)
			menu_submenu(lcd_fsensor_fail);
	}
}
#endif //FILAMENT_SENSOR


#if !SDSORT_USES_RAM
void lcd_set_degree() {
	lcd_set_custom_characters_degree();
}

void lcd_set_progress() {
	lcd_set_custom_characters_progress();
}
#endif

#if (LANG_MODE != 0)

void menu_setlang(unsigned char lang)
{
	if (!lang_select(lang))
	{
		if (lcd_show_fullscreen_message_yes_no_and_wait_P(_i("Copy selected language?"), false, true))
			lang_boot_update_start(lang);
		lcd_update_enable(true);
		lcd_clear();
		menu_goto(lcd_language_menu, 0, true, true);
		lcd_timeoutToStatus.stop(); //infinite timeout
		lcd_draw_update = 2;
	}
}

static void lcd_language_menu()
{
	MENU_BEGIN();
	if (lang_is_selected()) MENU_ITEM_BACK_P(_T(MSG_SETTINGS)); //
	if (menu_item_text_P(lang_get_name_by_code(lang_get_code(0)))) //primary language
	{
		menu_setlang(0);
		return;
	}
	uint8_t cnt = lang_get_count();
#ifdef W25X20CL
	if (cnt == 2) //display secondary language in case of clear xflash 
	{
		if (menu_item_text_P(lang_get_name_by_code(lang_get_code(1))))
		{
			menu_setlang(1);
			return;
		}
	}
	else
		for (int i = 2; i < cnt; i++) //skip seconday language - solved in lang_select (MK3)
#else //W25X20CL
		for (int i = 1; i < cnt; i++) //all seconday languages (MK2/25)
#endif //W25X20CL
			if (menu_item_text_P(lang_get_name_by_code(lang_get_code(i))))
			{
				menu_setlang(i);
				return;
			}
	MENU_END();
}
#endif //(LANG_MODE != 0)


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
	MENU_BEGIN();
		MENU_ITEM_BACK_P(_T(MSG_MENU_CALIBRATION));
		MENU_ITEM_SUBMENU_P(_i("Calibrate"), lcd_calibrate_pinda);////MSG_CALIBRATE_PINDA c=17 r=1
	MENU_END();
}

void lcd_temp_calibration_set() {
	temp_cal_active = !temp_cal_active;
	eeprom_update_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE, temp_cal_active);
	st_current_init();
}

#ifdef HAS_SECOND_SERIAL_PORT
void lcd_second_serial_set() {
	if(selectedSerialPort == 1) selectedSerialPort = 0;
	else selectedSerialPort = 1;
	eeprom_update_byte((unsigned char *)EEPROM_SECOND_SERIAL_ACTIVE, selectedSerialPort);
	MYSERIAL.begin(BAUDRATE);
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
		const char   *msg_e_cal_knob = _i("Rotate knob until mark reaches extruder body. Click when done.");////MSG_E_CAL_KNOB c=20 r=8
		const char   *msg_next_e_cal_knob = lcd_display_message_fullscreen_P(msg_e_cal_knob);
		const bool    multi_screen = msg_next_e_cal_knob != NULL;
		unsigned long msg_millis;

		lcd_show_fullscreen_message_and_wait_P(_i("Mark filament 100mm from extruder body. Click when done."));////MSG_MARK_FIL c=20 r=8
		lcd_clear();
		
		
		lcd_set_cursor(0, 1); lcd_puts_P(_T(MSG_PLEASE_WAIT));
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
			if (abs(lcd_encoder_diff) >= ENCODER_PULSES_PER_STEP) {						//adjusting mark by knob rotation
				delay_keep_alive(50);
				//previous_millis_cmd = millis();
				lcd_encoder += (lcd_encoder_diff / ENCODER_PULSES_PER_STEP);
				lcd_encoder_diff = 0;
				if (!planner_queue_full()) {
					current_position[E_AXIS] += float(abs((int)lcd_encoder)) * 0.01; //0.05
					lcd_encoder = 0;
					plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], feedrate, active_extruder);
					
				}
			}	
		}
		
		steps_final = current_position[E_AXIS] * axis_steps_per_unit[E_AXIS];
		//steps_final = st_get_position(E_AXIS);
		lcd_draw_update = 1;
		e_steps_per_unit = ((float)(steps_final)) / 100.0f;
		if (e_steps_per_unit < MIN_E_STEPS_PER_UNIT) e_steps_per_unit = MIN_E_STEPS_PER_UNIT;				
		if (e_steps_per_unit > MAX_E_STEPS_PER_UNIT) e_steps_per_unit = MAX_E_STEPS_PER_UNIT;

		lcd_clear();

		axis_steps_per_unit[E_AXIS] = e_steps_per_unit;
		enquecommand_P(PSTR("M500")); //store settings to eeprom
	
		//lcd_drawedit(PSTR("Result"), ftostr31(axis_steps_per_unit[E_AXIS]));
		//delay_keep_alive(2000);
		delay_keep_alive(500);
		lcd_show_fullscreen_message_and_wait_P(_i("E calibration finished. Please clean the nozzle. Click when done."));////MSG_CLEAN_NOZZLE_E c=20 r=8
		lcd_update_enable(true);
		lcd_draw_update = 2;

	}
	else
	{
		show_preheat_nozzle_warning();
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

void lcd_v2_calibration()
{
	if (mmu_enabled)
	{
	    const uint8_t filament = choose_menu_P(_i("Select PLA filament:"),_T(MSG_FILAMENT),_i("Cancel")); ////c=20 r=1  ////c=19 r=1
	    if (filament < 5)
	    {
	        lcd_commands_step = 20 + filament;
	        lcd_commands_type = LCD_COMMAND_V2_CAL;
	    }
	}
	else
	{
		bool loaded = lcd_show_fullscreen_message_yes_no_and_wait_P(_i("Is PLA filament loaded?"), false, true);////MSG_PLA_FILAMENT_LOADED c=20 r=2
		if (loaded) {
			lcd_commands_type = LCD_COMMAND_V2_CAL;
		}
		else {
			lcd_display_message_fullscreen_P(_i("Please load PLA filament first."));////MSG_PLEASE_LOAD_PLA c=20 r=4
			for (int i = 0; i < 20; i++) { //wait max. 2s
				delay_keep_alive(100);
				if (lcd_clicked()) {
					break;
				}
			}
		}
	}
	lcd_return_to_status();
	lcd_update_enable(true);
}

void lcd_wizard() {
	bool result = true;
	if (calibration_status() != CALIBRATION_STATUS_ASSEMBLED) {
		result = lcd_show_multiscreen_message_yes_no_and_wait_P(_i("Running Wizard will delete current calibration results and start from the beginning. Continue?"), false, false);////MSG_WIZARD_RERUN c=20 r=7
	}
	if (result) {
		calibration_status_store(CALIBRATION_STATUS_ASSEMBLED);
		lcd_wizard(WizState::Run);
	}
	else {
		lcd_return_to_status();
		lcd_update_enable(true);
		lcd_update(2);
	}
}

void lcd_language()
{
	lcd_update_enable(true);
	lcd_clear();
	menu_goto(lcd_language_menu, 0, true, true);
	lcd_timeoutToStatus.stop(); //infinite timeout
	lcd_draw_update = 2;
	while ((menu_menu != lcd_status_screen) && (!lang_is_selected()))
	{
		delay(50);
		lcd_update(0);
		manage_heater();
		manage_inactivity(true);
	}
	if (lang_is_selected())
		lcd_return_to_status();
	else
		lang_select(LANG_ID_PRI);
}

static void wait_preheat()
{
    current_position[Z_AXIS] = 100; //move in z axis to make space for loading filament
    plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], homing_feedrate[Z_AXIS] / 60, active_extruder);
    delay_keep_alive(2000);
    lcd_display_message_fullscreen_P(_T(MSG_WIZARD_HEATING));
	lcd_set_custom_characters();
	while (abs(degHotend(0) - degTargetHotend(0)) > 3) {
        lcd_display_message_fullscreen_P(_T(MSG_WIZARD_HEATING));

        lcd_set_cursor(0, 4);
	    //Print the hotend temperature (9 chars total)
		lcdui_print_temp(LCD_STR_THERMOMETER[0], (int)(degHotend(0) + 0.5), (int)(degTargetHotend(0) + 0.5));
        delay_keep_alive(1000);
    }
	
}

static void lcd_wizard_unload()
{
	if(mmu_enabled)
	{
		int8_t unload = lcd_show_multiscreen_message_two_choices_and_wait_P(
		_i("Use unload to remove filament 1 if it protrudes outside of the rear MMU tube. Use eject if it is hidden in tube.")
		,false, true, _i("Unload"), _i("Eject"));
		if (unload)
		{
			extr_unload_0();
		} 
		else
		{
			mmu_eject_fil_0();
		}
	} 
	else
	{
			unload_filament();
	}
}

static void lcd_wizard_load()
{
	if (mmu_enabled)
	{
		lcd_show_fullscreen_message_and_wait_P(_i("Please insert PLA filament to the first tube of MMU, then press the knob to load it."));////c=20 r=8
	} 
	else
	{
		lcd_show_fullscreen_message_and_wait_P(_i("Please insert PLA filament to the extruder, then press knob to load it."));////MSG_WIZARD_LOAD_FILAMENT c=20 r=8
	}	
	lcd_update_enable(false);
	lcd_clear();
	lcd_puts_at_P(0, 2, _T(MSG_LOADING_FILAMENT));
#ifdef SNMM
	change_extr(0);
#endif
	loading_flag = true;
	gcode_M701();
}

bool lcd_autoDepleteEnabled()
{
    return (lcd_autoDeplete && fsensor_enabled);
}

//! @brief Printer first run wizard (Selftest and calibration)
//!
//!
//! First layer calibration with MMU state diagram
//!
//! @startuml
//! [*] --> IsFil
//! IsFil : Is filament 1 loaded?
//! isPLA : Is filament 1 PLA?
//! unload : Eject or Unload?
//! load : Push the button to start loading PLA Filament 1
//!
//! IsFil --> isPLA   : yes
//! IsFil --> load    : no
//! isPLA --> unload     : no
//! unload --> load      : eject
//! unload --> load      : unload
//! load --> calibration : click
//! isPLA --> calibration : yes
//! @enduml
//!
//! @param state Entry point of the wizard
//!
//!   state                 | description
//!  ---------------------- | ----------------
//! WizState::Run           | Main entry point
//! WizState::RepeatLay1Cal | Entry point after passing 1st layer calibration
void lcd_wizard(WizState state)
{
    using S = WizState;
	bool end = false;
	int wizard_event;
	const char *msg = NULL;
	while (!end) {
		printf_P(PSTR("Wizard state: %d"), state);
		switch (state) {
		case S::Run: //Run wizard?
			wizard_active = true;
			wizard_event = lcd_show_multiscreen_message_yes_no_and_wait_P(_i("Hi, I am your Original Prusa i3 printer. Would you like me to guide you through the setup process?"), false, true);////MSG_WIZARD_WELCOME c=20 r=7
			if (wizard_event) {
				state = S::Restore;
				eeprom_write_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 1);
			}
			else {
				eeprom_write_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 0);
				end = true;
			}
			break;
		case S::Restore: // restore calibration status
			switch (calibration_status()) {
			case CALIBRATION_STATUS_ASSEMBLED: state = S::Selftest; break; //run selftest
			case CALIBRATION_STATUS_XYZ_CALIBRATION: state = S::Xyz; break; //run xyz cal.
			case CALIBRATION_STATUS_Z_CALIBRATION: state = S::Z; break; //run z cal.
			case CALIBRATION_STATUS_LIVE_ADJUST: state = S::IsFil; break; //run live adjust
			case CALIBRATION_STATUS_CALIBRATED: end = true; eeprom_write_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 0); break;
			default: state = S::Selftest; break; //if calibration status is unknown, run wizard from the beginning
			}
			break; 
		case S::Selftest:
			lcd_show_fullscreen_message_and_wait_P(_i("First, I will run the selftest to check most common assembly problems."));////MSG_WIZARD_SELFTEST c=20 r=8
			wizard_event = lcd_selftest();
			if (wizard_event) {
				calibration_status_store(CALIBRATION_STATUS_XYZ_CALIBRATION);
				state = S::Xyz;
			}
			else end = true;
			break;
		case S::Xyz: //xyz calibration
			lcd_show_fullscreen_message_and_wait_P(_i("I will run xyz calibration now. It will take approx. 12 mins."));////MSG_WIZARD_XYZ_CAL c=20 r=8
			wizard_event = gcode_M45(false, 0);
			if (wizard_event) state = S::IsFil;
			else end = true;
			break;
		case S::Z: //z calibration
			lcd_show_fullscreen_message_and_wait_P(_i("Please remove shipping helpers first."));
			lcd_show_fullscreen_message_and_wait_P(_i("Now remove the test print from steel sheet."));
			lcd_show_fullscreen_message_and_wait_P(_i("I will run z calibration now."));////MSG_WIZARD_Z_CAL c=20 r=8
			wizard_event = lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_STEEL_SHEET_CHECK), false, false);
			if (!wizard_event) lcd_show_fullscreen_message_and_wait_P(_T(MSG_PLACE_STEEL_SHEET));
			wizard_event = gcode_M45(true, 0);
			if (wizard_event) {
				//current filament needs to be unloaded and then new filament should be loaded
				//start to preheat nozzle for unloading remaining PLA filament
				setTargetHotend(PLA_PREHEAT_HOTEND_TEMP, 0);
				lcd_display_message_fullscreen_P(_i("Now I will preheat nozzle for PLA."));
				wait_preheat();
				//unload current filament
				lcd_wizard_unload();
				//load filament
				lcd_wizard_load();
				setTargetHotend(0, 0); //we are finished, cooldown nozzle
				state = S::Finish; //shipped, no need to set first layer, go to final message directly
			}
			else end = true;
			break;
		case S::IsFil: //is filament loaded?
				//start to preheat nozzle and bed to save some time later
			setTargetHotend(PLA_PREHEAT_HOTEND_TEMP, 0);
			setTargetBed(PLA_PREHEAT_HPB_TEMP);
			if (mmu_enabled)
			{
			    wizard_event = lcd_show_fullscreen_message_yes_no_and_wait_P(_i("Is filament 1 loaded?"), false);////c=20 r=2
			} else
			{
			    wizard_event = lcd_show_fullscreen_message_yes_no_and_wait_P(_i("Is filament loaded?"), false);////MSG_WIZARD_FILAMENT_LOADED c=20 r=2
			}
			if (wizard_event) state = S::IsPla;
			else
			{
			    if(mmu_enabled) state = S::LoadFil;
			    else state = S::PreheatPla;
			}
			break;
		case S::PreheatPla:
#ifndef SNMM
		    lcd_display_message_fullscreen_P(_i("Now I will preheat nozzle for PLA."));////MSG_WIZARD_WILL_PREHEAT c=20 r=4
		    wait_preheat();
#endif //not SNMM
			state = S::LoadFil;
			break;
		case S::Preheat:
		    menu_goto(lcd_preheat_menu,0,false,true);
		    lcd_show_fullscreen_message_and_wait_P(_i("Select nozzle preheat temperature which matches your material."));
		    end = true; // Leave wizard temporarily for lcd_preheat_menu
		    break;
		case S::Unload:
		    wait_preheat();
			lcd_wizard_unload();
            state = S::LoadFil;
            break;
		case S::LoadFil: //load filament
			lcd_wizard_load();
			state = S::Lay1Cal;
			break;
		case S::IsPla:
			wizard_event = lcd_show_fullscreen_message_yes_no_and_wait_P(_i("Is it PLA filament?"), false, true);////MSG_WIZARD_PLA_FILAMENT c=20 r=2
			if (wizard_event) state = S::Lay1Cal;
			else state = S::Preheat;
			break;
		case S::Lay1Cal:
			lcd_show_fullscreen_message_and_wait_P(_i("Now I will calibrate distance between tip of the nozzle and heatbed surface."));////MSG_WIZARD_V2_CAL c=20 r=8
			lcd_show_fullscreen_message_and_wait_P(_i("I will start to print line and you will gradually lower the nozzle by rotating the knob, until you reach optimal height. Check the pictures in our handbook in chapter Calibration."));////MSG_WIZARD_V2_CAL_2 c=20 r=12
			lcd_commands_type = LCD_COMMAND_V2_CAL;
			lcd_return_to_status();
			end = true;
			break;
		case S::RepeatLay1Cal: //repeat first layer cal.?
			wizard_event = lcd_show_multiscreen_message_yes_no_and_wait_P(_i("Do you want to repeat last step to readjust distance between nozzle and heatbed?"), false);////MSG_WIZARD_REPEAT_V2_CAL c=20 r=7
			if (wizard_event) {
				lcd_show_fullscreen_message_and_wait_P(_i("Please clean heatbed and then press the knob."));////MSG_WIZARD_CLEAN_HEATBED c=20 r=8
				state = S::Lay1Cal;
			}
			else {
				state = S::Finish;
			}
			break;
		case S::Finish: //we are finished
			eeprom_write_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 0);
			end = true;
			break;

		default: break;
		}
	}

	printf_P(_N("Wizard end state: %d\n"), state);
	switch (state) { //final message
	case S::Restore: //printer was already calibrated
		msg = _T(MSG_WIZARD_DONE);
		break;
	case S::Selftest: //selftest
	case S::Xyz: //xyz cal.
	case S::Z: //z cal.
		msg = _T(MSG_WIZARD_CALIBRATION_FAILED);
		break;
	case S::Finish: //we are finished

		msg = _T(MSG_WIZARD_DONE);
		lcd_reset_alert_level();
		lcd_setstatuspgm(_T(WELCOME_MSG));
		lcd_return_to_status(); 
		break;

	default:
		msg = _T(MSG_WIZARD_QUIT);
		break;

	}
	if (!((S::Lay1Cal == state) || (S::Preheat == state))) {
		lcd_show_fullscreen_message_and_wait_P(msg);
		wizard_active = false;
	}
	lcd_update_enable(true);
	lcd_update(2);
}

#ifdef TMC2130
void lcd_settings_linearity_correction_menu(void)
{
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_SETTINGS));
#ifdef TMC2130_LINEARITY_CORRECTION_XYZ
	//tmc2130_wave_fac[X_AXIS]

	MENU_ITEM_EDIT_int3_P(_i("X-correct"),  &tmc2130_wave_fac[X_AXIS],  TMC2130_WAVE_FAC1000_MIN-TMC2130_WAVE_FAC1000_STP, TMC2130_WAVE_FAC1000_MAX);////MSG_EXTRUDER_CORRECTION c=9 r=0
	MENU_ITEM_EDIT_int3_P(_i("Y-correct"),  &tmc2130_wave_fac[Y_AXIS],  TMC2130_WAVE_FAC1000_MIN-TMC2130_WAVE_FAC1000_STP, TMC2130_WAVE_FAC1000_MAX);////MSG_EXTRUDER_CORRECTION c=9 r=0
	MENU_ITEM_EDIT_int3_P(_i("Z-correct"),  &tmc2130_wave_fac[Z_AXIS],  TMC2130_WAVE_FAC1000_MIN-TMC2130_WAVE_FAC1000_STP, TMC2130_WAVE_FAC1000_MAX);////MSG_EXTRUDER_CORRECTION c=9 r=0
#endif //TMC2130_LINEARITY_CORRECTION_XYZ
	MENU_ITEM_EDIT_int3_P(_i("E-correct"),  &tmc2130_wave_fac[E_AXIS],  TMC2130_WAVE_FAC1000_MIN-TMC2130_WAVE_FAC1000_STP, TMC2130_WAVE_FAC1000_MAX);////MSG_EXTRUDER_CORRECTION c=9 r=0
	MENU_END();
	if(menu_leaving)
	{
	    lcd_settings_linearity_correction_menu_save();
	}
}
#endif // TMC2130

#ifdef FILAMENT_SENSOR
#define SETTINGS_FILAMENT_SENSOR \
do\
{\
    if (FSensorStateMenu == 0)\
    {\
        if (fsensor_not_responding && (mmu_enabled == false))\
        {\
            /* Filament sensor not working*/\
            MENU_ITEM_FUNCTION_P(_i("Fil. sensor [N/A]"), lcd_fsensor_state_set);/*////MSG_FSENSOR_NA c=0 r=0*/\
            MENU_ITEM_SUBMENU_P(_T(MSG_FSENS_AUTOLOAD_NA), lcd_fsensor_fail);\
        }\
        else\
        {\
            /* Filament sensor turned off, working, no problems*/\
            MENU_ITEM_FUNCTION_P(_T(MSG_FSENSOR_OFF), lcd_fsensor_state_set);\
            if (mmu_enabled == false)\
            {\
                MENU_ITEM_SUBMENU_P(_T(MSG_FSENS_AUTOLOAD_NA), lcd_filament_autoload_info);\
            }\
        }\
    }\
    else\
    {\
        /* Filament sensor turned on, working, no problems*/\
        MENU_ITEM_FUNCTION_P(_T(MSG_FSENSOR_ON), lcd_fsensor_state_set);\
        if (mmu_enabled == false)\
        {\
            if (fsensor_autoload_enabled)\
                MENU_ITEM_FUNCTION_P(_i("F. autoload  [on]"), lcd_set_filament_autoload);/*////MSG_FSENS_AUTOLOAD_ON c=17 r=1*/\
            else\
                MENU_ITEM_FUNCTION_P(_i("F. autoload [off]"), lcd_set_filament_autoload);/*////MSG_FSENS_AUTOLOAD_OFF c=17 r=1*/\
            /*if (fsensor_oq_meassure_enabled)*/\
                /*MENU_ITEM_FUNCTION_P(_i("F. OQ meass. [on]"), lcd_set_filament_oq_meass);*//*////MSG_FSENS_OQMEASS_ON c=17 r=1*/\
            /*else*/\
                /*MENU_ITEM_FUNCTION_P(_i("F. OQ meass.[off]"), lcd_set_filament_oq_meass);*//*////MSG_FSENS_OQMEASS_OFF c=17 r=1*/\
        }\
    }\
}\
while(0)

#else //FILAMENT_SENSOR
#define SETTINGS_FILAMENT_SENSOR do{}while(0)
#endif //FILAMENT_SENSOR

static void auto_deplete_switch()
{
    lcd_autoDeplete = !lcd_autoDeplete;
    eeprom_update_byte((unsigned char *)EEPROM_AUTO_DEPLETE, lcd_autoDeplete);
}

static bool settingsAutoDeplete()
{
    if (mmu_enabled)
    {
        if (!fsensor_enabled)
        {
            if (menu_item_text_P(_i("SpoolJoin   [N/A]"))) return true;
        }
        else if (lcd_autoDeplete)
        {
            if (menu_item_function_P(_i("SpoolJoin    [on]"), auto_deplete_switch)) return true;
        }
        else
        {
            if (menu_item_function_P(_i("SpoolJoin   [off]"), auto_deplete_switch)) return true;
        }
    }
    return false;
}

#define SETTINGS_AUTO_DEPLETE \
do\
{\
    if(settingsAutoDeplete()) return;\
}\
while(0)\

#ifdef TMC2130
#define SETTINGS_SILENT_MODE \
do\
{\
    if(!farm_mode)\
    {\
        if (SilentModeMenu == SILENT_MODE_NORMAL)\
        {\
            MENU_ITEM_FUNCTION_P(_T(MSG_STEALTH_MODE_OFF), lcd_silent_mode_set);\
        }\
        else MENU_ITEM_FUNCTION_P(_T(MSG_STEALTH_MODE_ON), lcd_silent_mode_set);\
        if (SilentModeMenu == SILENT_MODE_NORMAL)\
        {\
            if (CrashDetectMenu == 0)\
            {\
                MENU_ITEM_FUNCTION_P(_T(MSG_CRASHDETECT_OFF), lcd_crash_mode_set);\
            }\
            else MENU_ITEM_FUNCTION_P(_T(MSG_CRASHDETECT_ON), lcd_crash_mode_set);\
        }\
        else MENU_ITEM_SUBMENU_P(_T(MSG_CRASHDETECT_NA), lcd_crash_mode_info);\
    }\
}\
while (0)

#else //TMC2130
#define SETTINGS_SILENT_MODE \
do\
{\
    if(!farm_mode)\
    {\
        switch (SilentModeMenu)\
        {\
        case SILENT_MODE_POWER:\
            MENU_ITEM_FUNCTION_P(_T(MSG_SILENT_MODE_OFF), lcd_silent_mode_set);\
            break;\
        case SILENT_MODE_SILENT:\
            MENU_ITEM_FUNCTION_P(_T(MSG_SILENT_MODE_ON), lcd_silent_mode_set);\
            break;\
        case SILENT_MODE_AUTO:\
            MENU_ITEM_FUNCTION_P(_T(MSG_AUTO_MODE_ON), lcd_silent_mode_set);\
            break;\
        default:\
            MENU_ITEM_FUNCTION_P(_T(MSG_SILENT_MODE_OFF), lcd_silent_mode_set);\
            break; /* (probably) not needed*/\
        }\
    }\
}\
while (0)
#endif //TMC2130

#ifdef SDCARD_SORT_ALPHA
#define SETTINGS_SD \
do\
{\
    if (card.ToshibaFlashAir_isEnabled())\
        MENU_ITEM_FUNCTION_P(_i("SD card [flshAir]"), lcd_toshiba_flash_air_compatibility_toggle);/*////MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON c=19 r=1*/\
    else\
        MENU_ITEM_FUNCTION_P(_i("SD card  [normal]"), lcd_toshiba_flash_air_compatibility_toggle);/*////MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF c=19 r=1*/\
\
    if (!farm_mode)\
    {\
        uint8_t sdSort;\
        EEPROM_read(EEPROM_SD_SORT, (uint8_t*)&sdSort, sizeof(sdSort));\
        switch (sdSort)\
        {\
          case SD_SORT_TIME: MENU_ITEM_FUNCTION_P(_i("Sort:      [time]"), lcd_sort_type_set); break;/*////MSG_SORT_TIME c=17 r=1*/\
          case SD_SORT_ALPHA: MENU_ITEM_FUNCTION_P(_i("Sort:  [alphabet]"), lcd_sort_type_set); break;/*////MSG_SORT_ALPHA c=17 r=1*/\
          default: MENU_ITEM_FUNCTION_P(_i("Sort:      [none]"), lcd_sort_type_set);/*////MSG_SORT_NONE c=17 r=1*/\
        }\
    }\
}\
while (0)
#else // SDCARD_SORT_ALPHA
#define SETTINGS_SD \
do\
{\
    if (card.ToshibaFlashAir_isEnabled())\
        MENU_ITEM_FUNCTION_P(_i("SD card [flshAir]"), lcd_toshiba_flash_air_compatibility_toggle);/*////MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_ON c=19 r=1*/\
    else\
        MENU_ITEM_FUNCTION_P(_i("SD card  [normal]"), lcd_toshiba_flash_air_compatibility_toggle);/*////MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY_OFF c=19 r=1*/\
}\
while (0)
#endif // SDCARD_SORT_ALPHA

#define SETTINGS_SOUND \
do\
{\
    switch(eSoundMode)\
         {\
         case e_SOUND_MODE_LOUD:\
              MENU_ITEM_FUNCTION_P(_i(MSG_SOUND_MODE_LOUD),lcd_sound_state_set);\
              break;\
         case e_SOUND_MODE_ONCE:\
              MENU_ITEM_FUNCTION_P(_i(MSG_SOUND_MODE_ONCE),lcd_sound_state_set);\
              break;\
         case e_SOUND_MODE_SILENT:\
              MENU_ITEM_FUNCTION_P(_i(MSG_SOUND_MODE_SILENT),lcd_sound_state_set);\
              break;\
         case e_SOUND_MODE_MUTE:\
              MENU_ITEM_FUNCTION_P(_i(MSG_SOUND_MODE_MUTE),lcd_sound_state_set);\
              break;\
         default:\
              MENU_ITEM_FUNCTION_P(_i(MSG_SOUND_MODE_LOUD),lcd_sound_state_set);\
         }\
}\
while (0)

static void lcd_settings_menu()
{
	EEPROM_read(EEPROM_SILENT, (uint8_t*)&SilentModeMenu, sizeof(SilentModeMenu));
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));

	MENU_ITEM_SUBMENU_P(_i("Temperature"), lcd_control_temperature_menu);////MSG_TEMPERATURE c=0 r=0
	if (!homing_flag)
	    MENU_ITEM_SUBMENU_P(_i("Move axis"), lcd_move_menu_1mm);////MSG_MOVE_AXIS c=0 r=0
	if (!isPrintPaused)
	    MENU_ITEM_GCODE_P(_i("Disable steppers"), PSTR("M84"));////MSG_DISABLE_STEPPERS c=0 r=0

	SETTINGS_FILAMENT_SENSOR;

	SETTINGS_AUTO_DEPLETE;

	if (fans_check_enabled == true)
		MENU_ITEM_FUNCTION_P(_i("Fans check   [on]"), lcd_set_fan_check);////MSG_FANS_CHECK_ON c=17 r=1
	else
		MENU_ITEM_FUNCTION_P(_i("Fans check  [off]"), lcd_set_fan_check);////MSG_FANS_CHECK_OFF c=17 r=1

	SETTINGS_SILENT_MODE;

#if defined (TMC2130) && defined (LINEARITY_CORRECTION)
    MENU_ITEM_SUBMENU_P(_i("Lin. correction"), lcd_settings_linearity_correction_menu);
#endif //LINEARITY_CORRECTION && TMC2130

  if (temp_cal_active == false)
	  MENU_ITEM_FUNCTION_P(_i("Temp. cal.  [off]"), lcd_temp_calibration_set);////MSG_TEMP_CALIBRATION_OFF c=20 r=1
  else
	  MENU_ITEM_FUNCTION_P(_i("Temp. cal.   [on]"), lcd_temp_calibration_set);////MSG_TEMP_CALIBRATION_ON c=20 r=1

#ifdef HAS_SECOND_SERIAL_PORT
	if (selectedSerialPort == 0)
		MENU_ITEM_FUNCTION_P(_i("RPi port    [off]"), lcd_second_serial_set);////MSG_SECOND_SERIAL_OFF c=17 r=1
	else
		MENU_ITEM_FUNCTION_P(_i("RPi port     [on]"), lcd_second_serial_set);////MSG_SECOND_SERIAL_ON c=17 r=1
#endif //HAS_SECOND_SERIAL

	if (!isPrintPaused && !homing_flag)
		MENU_ITEM_SUBMENU_P(_T(MSG_BABYSTEP_Z), lcd_babystep_z);

#if (LANG_MODE != 0)
	MENU_ITEM_SUBMENU_P(_i("Select language"), lcd_language_menu);////MSG_LANGUAGE_SELECT c=0 r=0
#endif //(LANG_MODE != 0)

	SETTINGS_SD;
	SETTINGS_SOUND;

	if (farm_mode)
	{
		MENU_ITEM_SUBMENU_P(PSTR("Farm number"), lcd_farm_no);
		MENU_ITEM_FUNCTION_P(PSTR("Disable farm mode"), lcd_disable_farm_mode);
	}

	MENU_END();
}

#ifdef TMC2130
static void lcd_ustep_linearity_menu_save()
{
    eeprom_update_byte((uint8_t*)EEPROM_TMC2130_WAVE_X_FAC, tmc2130_wave_fac[X_AXIS]);
    eeprom_update_byte((uint8_t*)EEPROM_TMC2130_WAVE_Y_FAC, tmc2130_wave_fac[Y_AXIS]);
    eeprom_update_byte((uint8_t*)EEPROM_TMC2130_WAVE_Z_FAC, tmc2130_wave_fac[Z_AXIS]);
    eeprom_update_byte((uint8_t*)EEPROM_TMC2130_WAVE_E_FAC, tmc2130_wave_fac[E_AXIS]);
}
#endif //TMC2130


static void lcd_settings_linearity_correction_menu_save()
{
#ifdef TMC2130
    bool changed = false;
    if (tmc2130_wave_fac[X_AXIS] < TMC2130_WAVE_FAC1000_MIN) tmc2130_wave_fac[X_AXIS] = 0;
    if (tmc2130_wave_fac[Y_AXIS] < TMC2130_WAVE_FAC1000_MIN) tmc2130_wave_fac[Y_AXIS] = 0;
    if (tmc2130_wave_fac[Z_AXIS] < TMC2130_WAVE_FAC1000_MIN) tmc2130_wave_fac[Z_AXIS] = 0;
    if (tmc2130_wave_fac[E_AXIS] < TMC2130_WAVE_FAC1000_MIN) tmc2130_wave_fac[E_AXIS] = 0;
    changed |= (eeprom_read_byte((uint8_t*)EEPROM_TMC2130_WAVE_X_FAC) != tmc2130_wave_fac[X_AXIS]);
    changed |= (eeprom_read_byte((uint8_t*)EEPROM_TMC2130_WAVE_Y_FAC) != tmc2130_wave_fac[Y_AXIS]);
    changed |= (eeprom_read_byte((uint8_t*)EEPROM_TMC2130_WAVE_Z_FAC) != tmc2130_wave_fac[Z_AXIS]);
    changed |= (eeprom_read_byte((uint8_t*)EEPROM_TMC2130_WAVE_E_FAC) != tmc2130_wave_fac[E_AXIS]);
    lcd_ustep_linearity_menu_save();
    if (changed) tmc2130_init();
#endif //TMC2130
}


static void lcd_calibration_menu()
{
  MENU_BEGIN();
  MENU_ITEM_BACK_P(_T(MSG_MAIN));
  if (!isPrintPaused)
  {
	MENU_ITEM_FUNCTION_P(_i("Wizard"), lcd_wizard);////MSG_WIZARD c=17 r=1
	MENU_ITEM_SUBMENU_P(_i("First layer cal."), lcd_v2_calibration);////MSG_V2_CALIBRATION c=17 r=1
	MENU_ITEM_GCODE_P(_T(MSG_AUTO_HOME), PSTR("G28 W"));
	MENU_ITEM_FUNCTION_P(_i("Selftest         "), lcd_selftest_v);////MSG_SELFTEST c=0 r=0
#ifdef MK1BP
    // MK1
    // "Calibrate Z"
    MENU_ITEM_GCODE_P(_T(MSG_HOMEYZ), PSTR("G28 Z"));
#else //MK1BP
    // MK2
    MENU_ITEM_FUNCTION_P(_i("Calibrate XYZ"), lcd_mesh_calibration);////MSG_CALIBRATE_BED c=0 r=0
    // "Calibrate Z" with storing the reference values to EEPROM.
    MENU_ITEM_SUBMENU_P(_T(MSG_HOMEYZ), lcd_mesh_calibration_z);
#ifndef SNMM
	//MENU_ITEM_FUNCTION_P(_i("Calibrate E"), lcd_calibrate_extruder);////MSG_CALIBRATE_E c=20 r=1
#endif
    // "Mesh Bed Leveling"
    MENU_ITEM_SUBMENU_P(_i("Mesh Bed Leveling"), lcd_mesh_bedleveling);////MSG_MESH_BED_LEVELING c=0 r=0
	
#endif //MK1BP

    MENU_ITEM_SUBMENU_P(_i("Bed level correct"), lcd_adjust_bed);////MSG_BED_CORRECTION_MENU c=0 r=0
	MENU_ITEM_SUBMENU_P(_i("PID calibration"), pid_extruder);////MSG_PID_EXTRUDER c=17 r=1
#ifdef TMC2130
    MENU_ITEM_SUBMENU_P(_i("Show pinda state"), menu_show_pinda_state);
#else
    MENU_ITEM_SUBMENU_P(_i("Show end stops"), menu_show_end_stops);////MSG_SHOW_END_STOPS c=17 r=1
#endif
#ifndef MK1BP
    MENU_ITEM_GCODE_P(_i("Reset XYZ calibr."), PSTR("M44"));////MSG_CALIBRATE_BED_RESET c=0 r=0
#endif //MK1BP
#ifndef SNMM
	//MENU_ITEM_FUNCTION_P(MSG_RESET_CALIBRATE_E, lcd_extr_cal_reset);
#endif
#ifndef MK1BP
	MENU_ITEM_SUBMENU_P(_i("Temp. calibration"), lcd_pinda_calibration_menu);////MSG_CALIBRATION_PINDA_MENU c=17 r=1
#endif //MK1BP
  }
  
  MENU_END();
}

void bowden_menu() {
	int enc_dif = lcd_encoder_diff;
	int cursor_pos = 0;
	lcd_clear();
	lcd_set_cursor(0, 0);
	lcd_print(">");
	for (int i = 0; i < 4; i++) {
		lcd_set_cursor(1, i);
		lcd_print("Extruder ");
		lcd_print(i);
		lcd_print(": ");
		EEPROM_read_B(EEPROM_BOWDEN_LENGTH + i * 2, &bowden_length[i]);
		lcd_print(bowden_length[i] - 48);

	}
	enc_dif = lcd_encoder_diff;

	while (1) {

		manage_heater();
		manage_inactivity(true);

		if (abs((enc_dif - lcd_encoder_diff)) > 2) {

			if (enc_dif > lcd_encoder_diff) {
					cursor_pos--;
				}

				if (enc_dif < lcd_encoder_diff) {
					cursor_pos++;
				}

				if (cursor_pos > 3) {
					cursor_pos = 3;
				}

				if (cursor_pos < 0) {
					cursor_pos = 0;
				}

				lcd_set_cursor(0, 0);
				lcd_print(" ");
				lcd_set_cursor(0, 1);
				lcd_print(" ");
				lcd_set_cursor(0, 2);
				lcd_print(" ");
				lcd_set_cursor(0, 3);
				lcd_print(" ");
				lcd_set_cursor(0, cursor_pos);
				lcd_print(">");

				enc_dif = lcd_encoder_diff;
				delay(100);
		}

		if (lcd_clicked()) {

			lcd_clear();
			while (1) {

				manage_heater();
				manage_inactivity(true);

				lcd_set_cursor(1, 1);
				lcd_print("Extruder ");
				lcd_print(cursor_pos);
				lcd_print(": ");
				lcd_set_cursor(13, 1);
				lcd_print(bowden_length[cursor_pos] - 48);

				if (abs((enc_dif - lcd_encoder_diff)) > 2) {
						if (enc_dif > lcd_encoder_diff) {
							bowden_length[cursor_pos]--;
							lcd_set_cursor(13, 1);
							lcd_print(bowden_length[cursor_pos] - 48);
							enc_dif = lcd_encoder_diff;
						}

						if (enc_dif < lcd_encoder_diff) {
							bowden_length[cursor_pos]++;
							lcd_set_cursor(13, 1);
							lcd_print(bowden_length[cursor_pos] - 48);
							enc_dif = lcd_encoder_diff;
						}
				}
				delay(100);
				if (lcd_clicked()) {
					EEPROM_save_B(EEPROM_BOWDEN_LENGTH + cursor_pos * 2, &bowden_length[cursor_pos]);
					if (lcd_show_fullscreen_message_yes_no_and_wait_P(PSTR("Continue with another bowden?"))) {
						lcd_update_enable(true);
						lcd_clear();
						enc_dif = lcd_encoder_diff;
						lcd_set_cursor(0, cursor_pos);
						lcd_print(">");
						for (int i = 0; i < 4; i++) {
							lcd_set_cursor(1, i);
							lcd_print("Extruder ");
							lcd_print(i);
							lcd_print(": ");
							EEPROM_read_B(EEPROM_BOWDEN_LENGTH + i * 2, &bowden_length[i]);
							lcd_print(bowden_length[i] - 48);

						}
						break;
					}
					else return;
				}
			}
		}
	}
}

//#ifdef SNMM

static char snmm_stop_print_menu() { //menu for choosing which filaments will be unloaded in stop print
	lcd_clear();
	lcd_puts_at_P(0,0,_T(MSG_UNLOAD_FILAMENT)); lcd_print(":");
	lcd_set_cursor(0, 1); lcd_print(">");
	lcd_puts_at_P(1,2,_i("Used during print"));////MSG_USED c=19 r=1
	lcd_puts_at_P(1,3,_i("Current"));////MSG_CURRENT c=19 r=1
	char cursor_pos = 1;
	int enc_dif = 0;
	KEEPALIVE_STATE(PAUSED_FOR_USER);
	while (1) {
		manage_heater();
		manage_inactivity(true);
		if (abs((enc_dif - lcd_encoder_diff)) > 4) {

			if ((abs(enc_dif - lcd_encoder_diff)) > 1) {
				if (enc_dif > lcd_encoder_diff) cursor_pos--;
				if (enc_dif < lcd_encoder_diff) cursor_pos++;
				if (cursor_pos > 3) cursor_pos = 3;
				if (cursor_pos < 1) cursor_pos = 1;

				lcd_set_cursor(0, 1);
				lcd_print(" ");
				lcd_set_cursor(0, 2);
				lcd_print(" ");
				lcd_set_cursor(0, 3);
				lcd_print(" ");
				lcd_set_cursor(0, cursor_pos);
				lcd_print(">");
				enc_dif = lcd_encoder_diff;
				delay(100);
			}
		}
		if (lcd_clicked()) {
			KEEPALIVE_STATE(IN_HANDLER);
			return(cursor_pos - 1);
		}
	}
	
}

//! @brief Select one of numbered items
//!
//! Create list of items with header. Header can not be selected.
//! Each item has text description passed by function parameter and
//! number. There are 5 numbered items, if mmu_enabled, 4 otherwise.
//! Items are numbered from 1 to 4 or 5. But index returned starts at 0.
//! There can be last item with different text and no number.
//!
//! @param header Header text
//! @param item Item text
//! @param last_item Last item text, or nullptr if there is no Last item
//! @return selected item index, first item index is 0
uint8_t choose_menu_P(const char *header, const char *item, const char *last_item)
{
    //following code should handle 3 to 127 number of items well
    const int8_t items_no = last_item?(mmu_enabled?6:5):(mmu_enabled?5:4);
    const uint8_t item_len = item?strlen_P(item):0;
	int8_t first = 0;
	int8_t enc_dif = lcd_encoder_diff;
	int8_t cursor_pos = 1;
	
	lcd_clear();

	KEEPALIVE_STATE(PAUSED_FOR_USER);
	while (1)
	{
		manage_heater();
		manage_inactivity(true);

		if (abs((enc_dif - lcd_encoder_diff)) > 4)
		{
            if (enc_dif > lcd_encoder_diff)
            {
                cursor_pos--;
            }

            if (enc_dif < lcd_encoder_diff)
            {
                cursor_pos++;
            }
            enc_dif = lcd_encoder_diff;
		}

		if (cursor_pos > 3)
		{
            cursor_pos = 3;
            if (first < items_no - 3)
            {
                first++;
                lcd_clear();
            }
        }

        if (cursor_pos < 1)
        {
            cursor_pos = 1;
            if (first > 0)
            {
                first--;
                lcd_clear();
            }
        }

        if (header) lcd_puts_at_P(0,0,header);

        const bool last_visible = (first == items_no - 3);
        const int8_t ordinary_items = (last_item&&last_visible)?2:3;

        for (int i = 0; i < ordinary_items; i++)
        {
            if (item) lcd_puts_at_P(1, i + 1, item);
        }

        for (int i = 0; i < ordinary_items; i++)
        {
            lcd_set_cursor(2 + item_len, i+1);
            lcd_print(first + i + 1);
        }

        if (last_item&&last_visible) lcd_puts_at_P(1, 3, last_item);

        lcd_set_cursor(0, 1);
        lcd_print(" ");
        lcd_set_cursor(0, 2);
        lcd_print(" ");
        lcd_set_cursor(0, 3);
        lcd_print(" ");
        lcd_set_cursor(0, cursor_pos);
        lcd_print(">");

        delay(100);

		if (lcd_clicked())
		{
		    KEEPALIVE_STATE(IN_HANDLER);
			lcd_encoder_diff = 0;
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

	enc_dif = lcd_encoder_diff;
	lcd_clear();
	lcd_set_cursor(0, 0);
	lcd_print(">");

	while (1) {		

		for (int i = 0; i < 4; i++) {
			lcd_set_cursor(1, i);
			lcd_print(item[first + i]);
		}

		manage_heater();
		manage_inactivity(true);

		if (abs((enc_dif - lcd_encoder_diff)) > 4) {

			if ((abs(enc_dif - lcd_encoder_diff)) > 1) {
				if (enc_dif > lcd_encoder_diff) {
					cursor_pos--;
				}

				if (enc_dif < lcd_encoder_diff) {
					cursor_pos++;
				}

				if (cursor_pos > 3) {
					cursor_pos = 3;
					if (first < items_no - 4) {
						first++;
						lcd_clear();
					}
				}

				if (cursor_pos < 0) {
					cursor_pos = 0;
					if (first > 0) {
						first--;
						lcd_clear();
					}
				}
				lcd_set_cursor(0, 0);
				lcd_print(" ");
				lcd_set_cursor(0, 1);
				lcd_print(" ");
				lcd_set_cursor(0, 2);
				lcd_print(" ");
				lcd_set_cursor(0, 3);
				lcd_print(" ");
				lcd_set_cursor(0, cursor_pos);
				lcd_print(">");
				enc_dif = lcd_encoder_diff;
				delay(100);
			}

		}

		if (lcd_clicked()) {
			return(cursor_pos + first);
		}

	}

}

static void lcd_disable_farm_mode()
{
	int8_t disable = lcd_show_fullscreen_message_yes_no_and_wait_P(PSTR("Disable farm mode?"), true, false); //allow timeouting, default no
	if (disable)
	{
		enquecommand_P(PSTR("G99"));
		lcd_return_to_status();
	}
	lcd_update_enable(true);
	lcd_draw_update = 2;
	
}



static void fil_load_menu()
{
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));
	MENU_ITEM_FUNCTION_P(_i("Load all"), load_all);////MSG_LOAD_ALL c=17 r=0
	MENU_ITEM_FUNCTION_P(_i("Load filament 1"), extr_adj_0);////MSG_LOAD_FILAMENT_1 c=17 r=0
	MENU_ITEM_FUNCTION_P(_i("Load filament 2"), extr_adj_1);////MSG_LOAD_FILAMENT_2 c=17 r=0
	MENU_ITEM_FUNCTION_P(_i("Load filament 3"), extr_adj_2);////MSG_LOAD_FILAMENT_3 c=17 r=0
	MENU_ITEM_FUNCTION_P(_i("Load filament 4"), extr_adj_3);////MSG_LOAD_FILAMENT_4 c=17 r=0

	if (mmu_enabled)
		MENU_ITEM_FUNCTION_P(_i("Load filament 5"), extr_adj_4);

	MENU_END();
}

static void mmu_load_to_nozzle_menu()
{
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));
	MENU_ITEM_FUNCTION_P(_i("Load filament 1"), mmu_load_to_nozzle_0);
	MENU_ITEM_FUNCTION_P(_i("Load filament 2"), mmu_load_to_nozzle_1);
	MENU_ITEM_FUNCTION_P(_i("Load filament 3"), mmu_load_to_nozzle_2);
	MENU_ITEM_FUNCTION_P(_i("Load filament 4"), mmu_load_to_nozzle_3);
	MENU_ITEM_FUNCTION_P(_i("Load filament 5"), mmu_load_to_nozzle_4);
	MENU_END();
}

static void mmu_fil_eject_menu()
{
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));
	MENU_ITEM_FUNCTION_P(_i("Eject filament 1"), mmu_eject_fil_0);
	MENU_ITEM_FUNCTION_P(_i("Eject filament 2"), mmu_eject_fil_1);
	MENU_ITEM_FUNCTION_P(_i("Eject filament 3"), mmu_eject_fil_2);
	MENU_ITEM_FUNCTION_P(_i("Eject filament 4"), mmu_eject_fil_3);
	MENU_ITEM_FUNCTION_P(_i("Eject filament 5"), mmu_eject_fil_4);

	MENU_END();
}

#ifdef SNMM
static void fil_unload_menu()
{
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));
	MENU_ITEM_FUNCTION_P(_i("Unload all"), extr_unload_all);////MSG_UNLOAD_ALL c=17 r=0
	MENU_ITEM_FUNCTION_P(_i("Unload filament 1"), extr_unload_0);////MSG_UNLOAD_FILAMENT_1 c=17 r=0
	MENU_ITEM_FUNCTION_P(_i("Unload filament 2"), extr_unload_1);////MSG_UNLOAD_FILAMENT_2 c=17 r=0
	MENU_ITEM_FUNCTION_P(_i("Unload filament 3"), extr_unload_2);////MSG_UNLOAD_FILAMENT_3 c=17 r=0
	MENU_ITEM_FUNCTION_P(_i("Unload filament 4"), extr_unload_3);////MSG_UNLOAD_FILAMENT_4 c=17 r=0

	if (mmu_enabled)
		MENU_ITEM_FUNCTION_P(_i("Unload filament 5"), extr_unload_4);////MSG_UNLOAD_FILAMENT_5 c=17 r=0

	MENU_END();
}


static void change_extr_menu(){
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));
	MENU_ITEM_FUNCTION_P(_i("Extruder 1"), extr_change_0);////MSG_EXTRUDER_1 c=17 r=1
	MENU_ITEM_FUNCTION_P(_i("Extruder 2"), extr_change_1);////MSG_EXTRUDER_2 c=17 r=1
	MENU_ITEM_FUNCTION_P(_i("Extruder 3"), extr_change_2);////MSG_EXTRUDER_3 c=17 r=1
	MENU_ITEM_FUNCTION_P(_i("Extruder 4"), extr_change_3);////MSG_EXTRUDER_4 c=17 r=1

	MENU_END();
}
#endif //SNMM

//unload filament for single material printer (used in M702 gcode)
void unload_filament()
{
	custom_message_type = CUSTOM_MSG_TYPE_F_LOAD;
	lcd_setstatuspgm(_T(MSG_UNLOADING_FILAMENT));

	//		extr_unload2();

	current_position[E_AXIS] -= 45;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 5200 / 60, active_extruder);
	st_synchronize();
	current_position[E_AXIS] -= 15;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 1000 / 60, active_extruder);
	st_synchronize();
	current_position[E_AXIS] -= 20;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], 1000 / 60, active_extruder);
	st_synchronize();

	lcd_display_message_fullscreen_P(_T(MSG_PULL_OUT_FILAMENT));

	//disable extruder steppers so filament can be removed
	disable_e0();
	disable_e1();
	disable_e2();
	delay(100);

	Sound_MakeSound(e_SOUND_TYPE_StandardPrompt);
	uint8_t counterBeep = 0;
	while (!lcd_clicked() && (counterBeep < 50)) {
		delay_keep_alive(100);
		counterBeep++;
	}
	st_synchronize();
	while (lcd_clicked()) delay_keep_alive(100);

	lcd_update_enable(true);

	lcd_setstatuspgm(_T(WELCOME_MSG));
	custom_message_type = CUSTOM_MSG_TYPE_STATUS;

}

static void lcd_farm_no()
{
	char step = 0;
	int enc_dif = 0;
	int _farmno = farm_no;
	int _ret = 0;
	lcd_clear();

	lcd_set_cursor(0, 0);
	lcd_print("Farm no");

	do
	{

		if (abs((enc_dif - lcd_encoder_diff)) > 2) {
			if (enc_dif > lcd_encoder_diff) {
				switch (step) {
				case(0): if (_farmno >= 100) _farmno -= 100; break;
				case(1): if (_farmno % 100 >= 10) _farmno -= 10; break;
				case(2): if (_farmno % 10 >= 1) _farmno--; break;
				default: break;
				}
			}

			if (enc_dif < lcd_encoder_diff) {
				switch (step) {
				case(0): if (_farmno < 900) _farmno += 100; break;
				case(1): if (_farmno % 100 < 90) _farmno += 10; break;
				case(2): if (_farmno % 10 <= 8)_farmno++; break;
				default: break;
				}
			}
			enc_dif = 0;
			lcd_encoder_diff = 0;
		}

		lcd_set_cursor(0, 2);
		if (_farmno < 100) lcd_print("0");
		if (_farmno < 10) lcd_print("0");
		lcd_print(_farmno);
		lcd_print("  ");
		lcd_set_cursor(0, 3);
		lcd_print("   ");


		lcd_set_cursor(step, 3);
		lcd_print("^");
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
	enc_dif = lcd_encoder_diff;
	lcd_clear();
	lcd_set_cursor(0, 1);
	lcd_print(">");

	active_rows = items_no < 3 ? items_no : 3;

	while (1) {
		lcd_puts_at_P(0, 0, PSTR("Choose color:"));
		for (int i = 0; i < active_rows; i++) {
			lcd_set_cursor(1, i+1);
			lcd_print(item[first + i]);
		}

		manage_heater();
		manage_inactivity(true);
		proc_commands();
		if (abs((enc_dif - lcd_encoder_diff)) > 12) {
					
				if (enc_dif > lcd_encoder_diff) {
					cursor_pos--;
				}

				if (enc_dif < lcd_encoder_diff) {
					cursor_pos++;
				}
				
				if (cursor_pos > active_rows) {
					cursor_pos = active_rows;
					if (first < items_no - active_rows) {
						first++;
						lcd_clear();
					}
				}

				if (cursor_pos < 1) {
					cursor_pos = 1;
					if (first > 0) {
						first--;
						lcd_clear();
					}
				}
				lcd_set_cursor(0, 1);
				lcd_print(" ");
				lcd_set_cursor(0, 2);
				lcd_print(" ");
				lcd_set_cursor(0, 3);
				lcd_print(" ");
				lcd_set_cursor(0, cursor_pos);
				lcd_print(">");
				enc_dif = lcd_encoder_diff;
				delay(100);

		}

		if (lcd_clicked()) {
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

	enc_dif = lcd_encoder_diff;
	lcd_clear();

	lcd_set_cursor(0, 0);
	lcd_print("Print ok ?");

	do
	{
		if (abs(enc_dif - lcd_encoder_diff) > 12) {
			if (enc_dif > lcd_encoder_diff) {
				cursor_pos--;
			}

			if (enc_dif < lcd_encoder_diff) {
				cursor_pos++;
			}
			enc_dif = lcd_encoder_diff;
		}

		if (cursor_pos > 2) { cursor_pos = 2; }
		if (cursor_pos < 1) { cursor_pos = 1; }

		lcd_set_cursor(0, 2); lcd_print("          ");
		lcd_set_cursor(0, 3); lcd_print("          ");
		lcd_set_cursor(2, 2);
		lcd_puts_P(_T(MSG_YES));
		lcd_set_cursor(2, 3);
		lcd_puts_P(_T(MSG_NO));
		lcd_set_cursor(0, 1 + cursor_pos);
		lcd_print(">");
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

#include "w25x20cl.h"

#ifdef LCD_TEST
static void lcd_test_menu()
{
	W25X20CL_SPI_ENTER();
	w25x20cl_enable_wr();
	w25x20cl_chip_erase();
	w25x20cl_disable_wr();
}
#endif //LCD_TEST

//! @brief Resume paused print
//! @todo It is not good to call restore_print_from_ram_and_continue() from function called by lcd_update(),
//! as restore_print_from_ram_and_continue() calls lcd_update() internally.
void lcd_resume_print()
{
    lcd_return_to_status();
    lcd_setstatuspgm(_T(MSG_RESUMING_PRINT));
    lcd_reset_alert_level(); //for fan speed error
    restore_print_from_ram_and_continue(0.0);
    pause_time += (millis() - start_pause_print); //accumulate time when print is paused for correct statistics calculation
    refresh_cmd_timeout();
    isPrintPaused = false;
}

static void lcd_main_menu()
{

  MENU_BEGIN();

  // Majkl superawesome menu


 MENU_ITEM_BACK_P(_T(MSG_WATCH));

#ifdef RESUME_DEBUG 
 if (!saved_printing) 
  MENU_ITEM_FUNCTION_P(PSTR("tst - Save"), lcd_menu_test_save);
 else
  MENU_ITEM_FUNCTION_P(PSTR("tst - Restore"), lcd_menu_test_restore);
#endif //RESUME_DEBUG 

#ifdef TMC2130_DEBUG
 MENU_ITEM_FUNCTION_P(PSTR("recover print"), recover_print);
 MENU_ITEM_FUNCTION_P(PSTR("power panic"), uvlo_);
#endif //TMC2130_DEBUG

 /* if (farm_mode && !IS_SD_PRINTING )
    {
    
        int tempScrool = 0;
        if (lcd_draw_update == 0 && LCD_CLICKED == 0)
            //delay(100);
            return; // nothing to do (so don't thrash the SD card)
        uint16_t fileCnt = card.getnrfilenames();
        
        card.getWorkDirName();
        if (card.filename[0] == '/')
        {
#if SDCARDDETECT == -1
            MENU_ITEM_FUNCTION_P(_T(MSG_REFRESH), lcd_sd_refresh);
#endif
        } else {
            MENU_ITEM_FUNCTION_P(PSTR(LCD_STR_FOLDER ".."), lcd_sd_updir);
        }
        
        for (uint16_t i = 0; i < fileCnt; i++)
        {
            if (menu_item == menu_line)
            {
#ifndef SDCARD_RATHERRECENTFIRST
                card.getfilename(i);
#else
                card.getfilename(fileCnt - 1 - i);
#endif
                if (card.filenameIsDir)
                {
                    MENU_ITEM_SDDIR(_T(MSG_CARD_MENU), card.filename, card.longFilename);
                } else {
                    
                    MENU_ITEM_SDFILE(_T(MSG_CARD_MENU), card.filename, card.longFilename);
                    
                    
                    
                    
                }
            } else {
                MENU_ITEM_DUMMY();
            }
        }
        
        MENU_ITEM_BACK_P(PSTR("- - - - - - - - -"));
    
        
    }*/
 
  if ( ( IS_SD_PRINTING || is_usb_printing || (lcd_commands_type == LCD_COMMAND_V2_CAL)) && (current_position[Z_AXIS] < Z_HEIGHT_HIDE_LIVE_ADJUST_MENU) && !homing_flag && !mesh_bed_leveling_flag)
  {
	MENU_ITEM_SUBMENU_P(_T(MSG_BABYSTEP_Z), lcd_babystep_z);//8
  }


  if ( moves_planned() || IS_SD_PRINTING || is_usb_printing || (lcd_commands_type == LCD_COMMAND_V2_CAL))
  {
    MENU_ITEM_SUBMENU_P(_i("Tune"), lcd_tune_menu);////MSG_TUNE c=0 r=0
  } else 
  {
    MENU_ITEM_SUBMENU_P(_i("Preheat"), lcd_preheat_menu);////MSG_PREHEAT c=0 r=0
  }

#ifdef SDSUPPORT
  if (card.cardOK || lcd_commands_type == LCD_COMMAND_V2_CAL)
  {
    if (card.isFileOpen())
    {
		if (mesh_bed_leveling_flag == false && homing_flag == false) {
			if (card.sdprinting)
			{
				MENU_ITEM_FUNCTION_P(_i("Pause print"), lcd_pause_print);////MSG_PAUSE_PRINT c=0 r=0
			}
			else
			{
			    MENU_ITEM_SUBMENU_P(_i("Resume print"), lcd_resume_print);////MSG_RESUME_PRINT c=0 r=0
			}
			MENU_ITEM_SUBMENU_P(_T(MSG_STOP_PRINT), lcd_sdcard_stop);
		}
	}
	else if (lcd_commands_type == LCD_COMMAND_V2_CAL && mesh_bed_leveling_flag == false && homing_flag == false) {
		//MENU_ITEM_SUBMENU_P(_T(MSG_STOP_PRINT), lcd_sdcard_stop);
	}
	else
	{
		if (!is_usb_printing && (lcd_commands_type != LCD_COMMAND_V2_CAL))
		{
			//if (farm_mode) MENU_ITEM_SUBMENU_P(MSG_FARM_CARD_MENU, lcd_farm_sdcard_menu);
			/*else*/ MENU_ITEM_SUBMENU_P(_T(MSG_CARD_MENU), lcd_sdcard_menu);
		}
#if SDCARDDETECT < 1
      MENU_ITEM_GCODE_P(_i("Change SD card"), PSTR("M21"));  // SD-card changed by user////MSG_CNG_SDCARD c=0 r=0
#endif
    }
	
  } else 
  {
    MENU_ITEM_SUBMENU_P(_i("No SD card"), lcd_sdcard_menu);////MSG_NO_CARD c=0 r=0
#if SDCARDDETECT < 1
    MENU_ITEM_GCODE_P(_i("Init. SD card"), PSTR("M21")); // Manually initialize the SD-card via user interface////MSG_INIT_SDCARD c=0 r=0
#endif
  }
#endif


  if (IS_SD_PRINTING || is_usb_printing || (lcd_commands_type == LCD_COMMAND_V2_CAL))
  {
	  if (farm_mode)
	  {
		  MENU_ITEM_SUBMENU_P(PSTR("Farm number"), lcd_farm_no);
	  }
  } 
  else 
  {
	if (mmu_enabled)
	{
		MENU_ITEM_SUBMENU_P(_T(MSG_LOAD_FILAMENT), fil_load_menu);
		MENU_ITEM_SUBMENU_P(_i("Load to nozzle"), mmu_load_to_nozzle_menu);
		MENU_ITEM_SUBMENU_P(_i("Eject filament"), mmu_fil_eject_menu);
		MENU_ITEM_GCODE_P(_T(MSG_UNLOAD_FILAMENT), PSTR("M702 C"));
	}
	else
	{
#ifdef SNMM
		MENU_ITEM_SUBMENU_P(_T(MSG_UNLOAD_FILAMENT), fil_unload_menu);
		MENU_ITEM_SUBMENU_P(_i("Change extruder"), change_extr_menu);////MSG_CHANGE_EXTR c=20 r=1
#endif
#ifdef FILAMENT_SENSOR
		if ((fsensor_autoload_enabled == true) && (fsensor_enabled == true) && (mmu_enabled == false))
			MENU_ITEM_SUBMENU_P(_i("AutoLoad filament"), lcd_menu_AutoLoadFilament);////MSG_AUTOLOAD_FILAMENT c=17 r=0
		else
#endif //FILAMENT_SENSOR
			MENU_ITEM_FUNCTION_P(_T(MSG_LOAD_FILAMENT), lcd_LoadFilament);
		MENU_ITEM_SUBMENU_P(_T(MSG_UNLOAD_FILAMENT), lcd_unLoadFilament);
	}
	MENU_ITEM_SUBMENU_P(_T(MSG_SETTINGS), lcd_settings_menu);
    if(!isPrintPaused) MENU_ITEM_SUBMENU_P(_T(MSG_MENU_CALIBRATION), lcd_calibration_menu);

  }

  if (!is_usb_printing && (lcd_commands_type != LCD_COMMAND_V2_CAL))
  {
	  MENU_ITEM_SUBMENU_P(_i("Statistics  "), lcd_menu_statistics);////MSG_STATISTICS c=0 r=0
  }
    
#if defined(TMC2130) || defined(FILAMENT_SENSOR)
  MENU_ITEM_SUBMENU_P(_i("Fail stats"), lcd_menu_fails_stats);
#endif

  MENU_ITEM_SUBMENU_P(_i("Support"), lcd_support_menu);////MSG_SUPPORT c=0 r=0
#ifdef LCD_TEST
    MENU_ITEM_SUBMENU_P(_i("W25x20CL init"), lcd_test_menu);////MSG_SUPPORT c=0 r=0
#endif //LCD_TEST

  MENU_END();

}

void stack_error() {
	SET_OUTPUT(BEEPER);
if((eSoundMode==e_SOUND_MODE_LOUD)||(eSoundMode==e_SOUND_MODE_ONCE)||(eSoundMode==e_SOUND_MODE_SILENT))
	WRITE(BEEPER, HIGH);
	delay(1000);
	WRITE(BEEPER, LOW);
	lcd_display_message_fullscreen_P(_i("Error - static memory has been overwritten"));////MSG_STACK_ERROR c=20 r=4
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


static void lcd_colorprint_change() {
	
	enquecommand_P(PSTR("M600"));
	
	custom_message_type = CUSTOM_MSG_TYPE_F_LOAD; //just print status message
	lcd_setstatuspgm(_T(MSG_FINISHING_MOVEMENTS));
	lcd_return_to_status();
	lcd_draw_update = 3;
}

static void lcd_tune_menu()
{
	typedef struct
	{
	    menu_data_edit_t reserved; //!< reserved for number editing functions
		int8_t  status; //!< To recognize, whether the menu has been just initialized.
		//! Backup of extrudemultiply, to recognize, that the value has been changed and
		//! it needs to be applied.
		int16_t extrudemultiply;
	} _menu_data_t;
	static_assert(sizeof(menu_data)>= sizeof(_menu_data_t),"_menu_data_t doesn't fit into menu_data");
	_menu_data_t* _md = (_menu_data_t*)&(menu_data[0]);
	if (_md->status == 0)
	{
		// Menu was entered. Mark the menu as entered and save the current extrudemultiply value.
		_md->status = 1;
		_md->extrudemultiply = extrudemultiply;
	}
	else if (_md->extrudemultiply != extrudemultiply)
	{
		// extrudemultiply has been changed from the child menu. Apply the new value.
		_md->extrudemultiply = extrudemultiply;
		calculate_extruder_multipliers();
	}

  EEPROM_read(EEPROM_SILENT, (uint8_t*)&SilentModeMenu, sizeof(SilentModeMenu));



	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN)); //1
	MENU_ITEM_EDIT_int3_P(_i("Speed"), &feedmultiply, 10, 999);//2////MSG_SPEED c=0 r=0

	MENU_ITEM_EDIT_int3_P(_T(MSG_NOZZLE), &target_temperature[0], 0, HEATER_0_MAXTEMP - 10);//3
	MENU_ITEM_EDIT_int3_P(_T(MSG_BED), &target_temperature_bed, 0, BED_MAXTEMP - 10);//4

	MENU_ITEM_EDIT_int3_P(_T(MSG_FAN_SPEED), &fanSpeed, 0, 255);//5
	MENU_ITEM_EDIT_int3_P(_i("Flow"), &extrudemultiply, 10, 999);//6////MSG_FLOW c=0 r=0
#ifdef FILAMENTCHANGEENABLE
	MENU_ITEM_FUNCTION_P(_T(MSG_FILAMENTCHANGE), lcd_colorprint_change);//7
#endif

#ifdef FILAMENT_SENSOR
	if (FSensorStateMenu == 0) {
		MENU_ITEM_FUNCTION_P(_T(MSG_FSENSOR_OFF), lcd_fsensor_state_set);
	}
	else {
		MENU_ITEM_FUNCTION_P(_T(MSG_FSENSOR_ON), lcd_fsensor_state_set);
	}
#endif //FILAMENT_SENSOR

	SETTINGS_AUTO_DEPLETE;

#ifdef TMC2130
     if(!farm_mode)
     {
          if (SilentModeMenu == SILENT_MODE_NORMAL) MENU_ITEM_FUNCTION_P(_T(MSG_STEALTH_MODE_OFF), lcd_silent_mode_set);
          else MENU_ITEM_FUNCTION_P(_T(MSG_STEALTH_MODE_ON), lcd_silent_mode_set);

          if (SilentModeMenu == SILENT_MODE_NORMAL)
          {
               if (CrashDetectMenu == 0) MENU_ITEM_FUNCTION_P(_T(MSG_CRASHDETECT_OFF), lcd_crash_mode_set);
               else MENU_ITEM_FUNCTION_P(_T(MSG_CRASHDETECT_ON), lcd_crash_mode_set);
          }
          else MENU_ITEM_SUBMENU_P(_T(MSG_CRASHDETECT_NA), lcd_crash_mode_info);
     }
#else //TMC2130
	if (!farm_mode) { //dont show in menu if we are in farm mode
		switch (SilentModeMenu) {
		case SILENT_MODE_POWER: MENU_ITEM_FUNCTION_P(_T(MSG_SILENT_MODE_OFF), lcd_silent_mode_set); break;
		case SILENT_MODE_SILENT: MENU_ITEM_FUNCTION_P(_T(MSG_SILENT_MODE_ON), lcd_silent_mode_set); break;
		case SILENT_MODE_AUTO: MENU_ITEM_FUNCTION_P(_T(MSG_AUTO_MODE_ON), lcd_silent_mode_set); break;
		default: MENU_ITEM_FUNCTION_P(_T(MSG_SILENT_MODE_OFF), lcd_silent_mode_set); break; // (probably) not needed
		}
	}
#endif //TMC2130

     switch(eSoundMode)
          {
          case e_SOUND_MODE_LOUD:
               MENU_ITEM_FUNCTION_P(_i(MSG_SOUND_MODE_LOUD),lcd_sound_state_set);
               break;
          case e_SOUND_MODE_ONCE:
               MENU_ITEM_FUNCTION_P(_i(MSG_SOUND_MODE_ONCE),lcd_sound_state_set);
               break;
          case e_SOUND_MODE_SILENT:
               MENU_ITEM_FUNCTION_P(_i(MSG_SOUND_MODE_SILENT),lcd_sound_state_set);
               break;
          case e_SOUND_MODE_MUTE:
               MENU_ITEM_FUNCTION_P(_i(MSG_SOUND_MODE_MUTE),lcd_sound_state_set);
               break;
          default:
               MENU_ITEM_FUNCTION_P(_i(MSG_SOUND_MODE_LOUD),lcd_sound_state_set);
          }

	MENU_END();
}

static void lcd_control_temperature_menu()
{
#ifdef PIDTEMP
  // set up temp variables - undo the default scaling
//  raw_Ki = unscalePID_i(Ki);
//  raw_Kd = unscalePID_d(Kd);
#endif

  MENU_BEGIN();
  MENU_ITEM_BACK_P(_T(MSG_SETTINGS));
#if TEMP_SENSOR_0 != 0
  MENU_ITEM_EDIT_int3_P(_T(MSG_NOZZLE), &target_temperature[0], 0, HEATER_0_MAXTEMP - 10);
#endif
#if TEMP_SENSOR_1 != 0
  MENU_ITEM_EDIT_int3_P(_i("Nozzle2"), &target_temperature[1], 0, HEATER_1_MAXTEMP - 10);////MSG_NOZZLE1 c=0 r=0
#endif
#if TEMP_SENSOR_2 != 0
  MENU_ITEM_EDIT_int3_P(_i("Nozzle3"), &target_temperature[2], 0, HEATER_2_MAXTEMP - 10);////MSG_NOZZLE2 c=0 r=0
#endif
#if TEMP_SENSOR_BED != 0
  MENU_ITEM_EDIT_int3_P(_T(MSG_BED), &target_temperature_bed, 0, BED_MAXTEMP - 3);
#endif
  MENU_ITEM_EDIT_int3_P(_T(MSG_FAN_SPEED), &fanSpeed, 0, 255);
#if defined AUTOTEMP && (TEMP_SENSOR_0 != 0)
//MENU_ITEM_EDIT removed, following code must be redesigned if AUTOTEMP enabled
  MENU_ITEM_EDIT(bool, MSG_AUTOTEMP, &autotemp_enabled);
  MENU_ITEM_EDIT(float3, _i(" \002 Min"), &autotemp_min, 0, HEATER_0_MAXTEMP - 10);////MSG_MIN c=0 r=0
  MENU_ITEM_EDIT(float3, _i(" \002 Max"), &autotemp_max, 0, HEATER_0_MAXTEMP - 10);////MSG_MAX c=0 r=0
  MENU_ITEM_EDIT(float32, _i(" \002 Fact"), &autotemp_factor, 0.0, 1.0);////MSG_FACTOR c=0 r=0
#endif

  MENU_END();
}


#if SDCARDDETECT == -1
static void lcd_sd_refresh()
{
  card.initsd();
  menu_top = 0;
}
#endif
static void lcd_sd_updir()
{
  card.updir();
  menu_top = 0;
}

void lcd_print_stop()
{
	cancel_heatup = true;
#ifdef MESH_BED_LEVELING
	mbl.active = false;
#endif
	// Stop the stoppers, update the position from the stoppers.
	if (mesh_bed_leveling_flag == false && homing_flag == false)
	{
		planner_abort_hard();
		// Because the planner_abort_hard() initialized current_position[Z] from the stepper,
		// Z baystep is no more applied. Reset it.
		babystep_reset();
	}
	// Clean the input command queue.
	cmdqueue_reset();
	lcd_setstatuspgm(_T(MSG_PRINT_ABORTED));
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
	
	lcd_set_cursor(0, 0);
	lcd_puts_P(_T(MSG_STOP_PRINT));
	lcd_set_cursor(2, 2);
	lcd_puts_P(_T(MSG_NO));
	lcd_set_cursor(2, 3);
	lcd_puts_P(_T(MSG_YES));
	lcd_set_cursor(0, 2); lcd_print(" ");
	lcd_set_cursor(0, 3); lcd_print(" ");

	if ((int32_t)lcd_encoder > 2) { lcd_encoder = 2; }
	if ((int32_t)lcd_encoder < 1) { lcd_encoder = 1; }
	
	lcd_set_cursor(0, 1 + lcd_encoder);
	lcd_print(">");

	if (lcd_clicked())
	{
		if ((int32_t)lcd_encoder == 1)
		{
			lcd_return_to_status();
		}
		if ((int32_t)lcd_encoder == 2)
		{
			lcd_print_stop();
		}
	}

}

void lcd_sdcard_menu()
{
  uint8_t sdSort = eeprom_read_byte((uint8_t*)EEPROM_SD_SORT);

  if (presort_flag == true) {
	  presort_flag = false;
	  card.presort();
  }
  if (lcd_draw_update == 0 && LCD_CLICKED == 0)
    //delay(100);
    return; // nothing to do (so don't thrash the SD card)
  uint16_t fileCnt = card.getnrfilenames();


  MENU_BEGIN();
  MENU_ITEM_BACK_P(_T(MSG_MAIN));
  card.getWorkDirName();
  if (card.filename[0] == '/')
  {
#if SDCARDDETECT == -1
    MENU_ITEM_FUNCTION_P(_T(MSG_REFRESH), lcd_sd_refresh);
#endif
  } else {
    MENU_ITEM_FUNCTION_P(PSTR(LCD_STR_FOLDER ".."), lcd_sd_updir);
  }

  for (uint16_t i = 0; i < fileCnt; i++)
  {
    if (menu_item == menu_line)
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
			MENU_ITEM_SDDIR(card.filename, card.longFilename);
		else
			MENU_ITEM_SDFILE(_T(MSG_CARD_MENU), card.filename, card.longFilename);
    } else {
      MENU_ITEM_DUMMY();
    }
  }
  MENU_END();
}

static void lcd_selftest_v()
{
	(void)lcd_selftest();
}

bool lcd_selftest()
{
	int _progress = 0;
	bool _result = true;
	lcd_wait_for_cool_down();
	lcd_clear();
	lcd_set_cursor(0, 0); lcd_puts_P(_i("Self test start  "));////MSG_SELFTEST_START c=20 r=0
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
#ifdef FILAMENT_SENSOR
		if (mmu_enabled == false) {
			_progress = lcd_selftest_screen(9, _progress, 3, true, 2000); //check filaments sensor
			_result = lcd_selftest_fsensor();
		}
#endif // FILAMENT_SENSOR
	}
	if (_result)
	{
#ifdef FILAMENT_SENSOR
		if (mmu_enabled == false)
		{
			_progress = lcd_selftest_screen(10, _progress, 3, true, 2000); //fil sensor OK
		}
#endif // FILAMENT_SENSOR
		_progress = lcd_selftest_screen(11, _progress, 3, true, 5000); //all correct
	}
	else
	{
		_progress = lcd_selftest_screen(12, _progress, 3, true, 5000);
	}
	lcd_reset_alert_level();
	enquecommand_P(PSTR("M84"));
	lcd_update_enable(true);
	
	if (_result)
	{
		LCD_ALERTMESSAGERPGM(_i("Self test OK"));////MSG_SELFTEST_OK c=0 r=0
	}
	else
	{
		LCD_ALERTMESSAGERPGM(_T(MSG_SELFTEST_FAILED));
	}
	#ifdef TMC2130
	  FORCE_HIGH_POWER_END;
	#endif // TMC2130
	KEEPALIVE_STATE(NOT_BUSY);
	return(_result);
}

#ifdef TMC2130

static void reset_crash_det(unsigned char axis) {
	current_position[axis] += 10;
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[3], manual_feedrate[0] / 60, active_extruder);
	st_synchronize();
	if (eeprom_read_byte((uint8_t*)EEPROM_CRASH_DET)) tmc2130_sg_stop_on_crash = true;
}

static bool lcd_selfcheck_axis_sg(unsigned char axis) {
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
		printf_P(_N("Measured axis length:%.3f\n"), measured_axis_length[i]);
		if (abs(measured_axis_length[i] - axis_length) > max_error_mm) {
			enable_endstops(false);

			const char *_error_1;

			if (axis == X_AXIS) _error_1 = "X";
			if (axis == Y_AXIS) _error_1 = "Y";
			if (axis == Z_AXIS) _error_1 = "Z";

			lcd_selftest_error(9, _error_1, NULL);
			current_position[axis] = 0;
			plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
			reset_crash_det(axis);
			return false;
		}
	}

		printf_P(_N("Axis length difference:%.3f\n"), abs(measured_axis_length[0] - measured_axis_length[1]));
	
		if (abs(measured_axis_length[0] - measured_axis_length[1]) > 1) { //check if difference between first and second measurement is low
			//loose pulleys
			const char *_error_1;

			if (axis == X_AXIS) _error_1 = "X";
			if (axis == Y_AXIS) _error_1 = "Y";
			if (axis == Z_AXIS) _error_1 = "Z";

			lcd_selftest_error(8, _error_1, NULL);
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
		if ((READ(Z_MIN_PIN) ^ (bool)Z_MIN_ENDSTOP_INVERTING))
#else //TMC2130
		if ((READ(X_MIN_PIN) ^ (bool)X_MIN_ENDSTOP_INVERTING) ||
			(READ(Y_MIN_PIN) ^ (bool)Y_MIN_ENDSTOP_INVERTING) ||
			(READ(Z_MIN_PIN) ^ (bool)Z_MIN_ENDSTOP_INVERTING))
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
#endif //not defined TMC2130

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
	lcd_beeper_quick_feedback();

	target_temperature[0] = 0;
	target_temperature_bed = 0;
	manage_heater();
	manage_inactivity();

	lcd_clear();

	lcd_set_cursor(0, 0);
	lcd_puts_P(_i("Selftest error !"));////MSG_SELFTEST_ERROR c=0 r=0
	lcd_set_cursor(0, 1);
	lcd_puts_P(_i("Please check :"));////MSG_SELFTEST_PLEASECHECK c=0 r=0

	switch (_error_no)
	{
	case 1:
		lcd_set_cursor(0, 2);
		lcd_puts_P(_i("Heater/Thermistor"));////MSG_SELFTEST_HEATERTHERMISTOR c=0 r=0
		lcd_set_cursor(0, 3);
		lcd_puts_P(_i("Not connected"));////MSG_SELFTEST_NOTCONNECTED c=0 r=0
		break;
	case 2:
		lcd_set_cursor(0, 2);
		lcd_puts_P(_i("Bed / Heater"));////MSG_SELFTEST_BEDHEATER c=0 r=0
		lcd_set_cursor(0, 3);
		lcd_puts_P(_T(MSG_SELFTEST_WIRINGERROR));
		break;
	case 3:
		lcd_set_cursor(0, 2);
		lcd_puts_P(_i("Endstops"));////MSG_SELFTEST_ENDSTOPS c=0 r=0
		lcd_set_cursor(0, 3);
		lcd_puts_P(_T(MSG_SELFTEST_WIRINGERROR));
		lcd_set_cursor(17, 3);
		lcd_print(_error_1);
		break;
	case 4:
		lcd_set_cursor(0, 2);
		lcd_puts_P(_T(MSG_SELFTEST_MOTOR));
		lcd_set_cursor(18, 2);
		lcd_print(_error_1);
		lcd_set_cursor(0, 3);
		lcd_puts_P(_i("Endstop"));////MSG_SELFTEST_ENDSTOP c=0 r=0
		lcd_set_cursor(18, 3);
		lcd_print(_error_2);
		break;
	case 5:
		lcd_set_cursor(0, 2);
		lcd_puts_P(_i("Endstop not hit"));////MSG_SELFTEST_ENDSTOP_NOTHIT c=20 r=1
		lcd_set_cursor(0, 3);
		lcd_puts_P(_T(MSG_SELFTEST_MOTOR));
		lcd_set_cursor(18, 3);
		lcd_print(_error_1);
		break;
	case 6:
		lcd_set_cursor(0, 2);
		lcd_puts_P(_T(MSG_SELFTEST_COOLING_FAN));
		lcd_set_cursor(0, 3);
		lcd_puts_P(_T(MSG_SELFTEST_WIRINGERROR));
		lcd_set_cursor(18, 3);
		lcd_print(_error_1);
		break;
	case 7:
		lcd_set_cursor(0, 2);
		lcd_puts_P(_T(MSG_SELFTEST_EXTRUDER_FAN));
		lcd_set_cursor(0, 3);
		lcd_puts_P(_T(MSG_SELFTEST_WIRINGERROR));
		lcd_set_cursor(18, 3);
		lcd_print(_error_1);
		break;
	case 8:
		lcd_set_cursor(0, 2);
		lcd_puts_P(_i("Loose pulley"));////MSG_LOOSE_PULLEY c=20 r=1
		lcd_set_cursor(0, 3);
		lcd_puts_P(_T(MSG_SELFTEST_MOTOR));
		lcd_set_cursor(18, 3);
		lcd_print(_error_1);
		break;
	case 9:
		lcd_set_cursor(0, 2);
		lcd_puts_P(_i("Axis length"));////MSG_SELFTEST_AXIS_LENGTH c=0 r=0
		lcd_set_cursor(0, 3);
		lcd_puts_P(_i("Axis"));////MSG_SELFTEST_AXIS c=0 r=0
		lcd_set_cursor(18, 3);
		lcd_print(_error_1);
		break;
	case 10:
		lcd_set_cursor(0, 2);
		lcd_puts_P(_i("Front/left fans"));////MSG_SELFTEST_FANS c=0 r=0
		lcd_set_cursor(0, 3);
		lcd_puts_P(_i("Swapped"));////MSG_SELFTEST_SWAPPED c=0 r=0
		lcd_set_cursor(18, 3);
		lcd_print(_error_1);
		break;
	case 11: 
		lcd_set_cursor(0, 2);
		lcd_puts_P(_i("Filament sensor"));////MSG_FILAMENT_SENSOR c=20 r=0
		lcd_set_cursor(0, 3);
		lcd_puts_P(_T(MSG_SELFTEST_WIRINGERROR));
		break;
	}

	delay(1000);
	lcd_beeper_quick_feedback();

	do {
		delay(100);
		manage_heater();
		manage_inactivity();
	} while (!lcd_clicked());

	LCD_ALERTMESSAGERPGM(_T(MSG_SELFTEST_FAILED));
	lcd_return_to_status();

}

#ifdef FILAMENT_SENSOR
static bool lcd_selftest_fsensor(void)
{
	fsensor_init();
	if (fsensor_not_responding)
	{
		lcd_selftest_error(11, NULL, NULL);
	}
	return (!fsensor_not_responding);
}
#endif //FILAMENT_SENSOR

static bool lcd_selftest_manual_fan_check(int _fan, bool check_opposite)
{

	bool _result = check_opposite;
	lcd_clear();

	lcd_set_cursor(0, 0); lcd_puts_P(_T(MSG_SELFTEST_FAN));
	
	switch (_fan)
	{
	case 0:
		// extruder cooling fan
		lcd_set_cursor(0, 1); 
		if(check_opposite == true) lcd_puts_P(_T(MSG_SELFTEST_COOLING_FAN)); 
		else lcd_puts_P(_T(MSG_SELFTEST_EXTRUDER_FAN));
		SET_OUTPUT(EXTRUDER_0_AUTO_FAN_PIN);
		WRITE(EXTRUDER_0_AUTO_FAN_PIN, 1);
		break;
	case 1:
		// object cooling fan
		lcd_set_cursor(0, 1);
		if (check_opposite == true) lcd_puts_P(_T(MSG_SELFTEST_EXTRUDER_FAN));
		else lcd_puts_P(_T(MSG_SELFTEST_COOLING_FAN));
		SET_OUTPUT(FAN_PIN);
		analogWrite(FAN_PIN, 255);
		break;
	}
	delay(500);

	lcd_set_cursor(1, 2); lcd_puts_P(_T(MSG_SELFTEST_FAN_YES));
	lcd_set_cursor(0, 3); lcd_print(">");
	lcd_set_cursor(1, 3); lcd_puts_P(_T(MSG_SELFTEST_FAN_NO));

	int8_t enc_dif = 0;
	KEEPALIVE_STATE(PAUSED_FOR_USER);

	lcd_button_pressed = false; 
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

		if (abs((enc_dif - lcd_encoder_diff)) > 2) {
			if (enc_dif > lcd_encoder_diff) {
				_result = !check_opposite;
				lcd_set_cursor(0, 2); lcd_print(">");
				lcd_set_cursor(1, 2); lcd_puts_P(_T(MSG_SELFTEST_FAN_YES));
				lcd_set_cursor(0, 3); lcd_print(" ");
				lcd_set_cursor(1, 3); lcd_puts_P(_T(MSG_SELFTEST_FAN_NO));
			}

			if (enc_dif < lcd_encoder_diff) {
				_result = check_opposite;
				lcd_set_cursor(0, 2); lcd_print(" ");
				lcd_set_cursor(1, 2); lcd_puts_P(_T(MSG_SELFTEST_FAN_YES));
				lcd_set_cursor(0, 3); lcd_print(">");
				lcd_set_cursor(1, 3); lcd_puts_P(_T(MSG_SELFTEST_FAN_NO));
			}
			enc_dif = 0;
			lcd_encoder_diff = 0;
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
			lcd_set_cursor(18, 3);
			lcd_print("-");
			delay_keep_alive(1000);
			lcd_set_cursor(18, 3);
			lcd_print("|");
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
		lcd_selftest_error(_errno, NULL, NULL);
	}
	return _result;
}

static int lcd_selftest_screen(int _step, int _progress, int _progress_scale, bool _clear, int _delay)
{

    lcd_update_enable(false);

	int _step_block = 0;
	const char *_indicator = (_progress > _progress_scale) ? "-" : "|";

	if (_clear) lcd_clear();


	lcd_set_cursor(0, 0);

	if (_step == -1) lcd_puts_P(_T(MSG_SELFTEST_FAN));
	if (_step == 0) lcd_puts_P(_T(MSG_SELFTEST_FAN));
	if (_step == 1) lcd_puts_P(_T(MSG_SELFTEST_FAN));
	if (_step == 2) lcd_puts_P(_i("Checking endstops"));////MSG_SELFTEST_CHECK_ENDSTOPS c=20 r=0
	if (_step == 3) lcd_puts_P(_i("Checking hotend  "));////MSG_SELFTEST_CHECK_HOTEND c=20 r=0
	if (_step == 4) lcd_puts_P(_i("Checking X axis  "));////MSG_SELFTEST_CHECK_X c=20 r=0
	if (_step == 5) lcd_puts_P(_i("Checking Y axis  "));////MSG_SELFTEST_CHECK_Y c=20 r=0
	if (_step == 6) lcd_puts_P(_i("Checking Z axis  "));////MSG_SELFTEST_CHECK_Z c=20 r=0
	if (_step == 7) lcd_puts_P(_T(MSG_SELFTEST_CHECK_BED));
	if (_step == 8) lcd_puts_P(_T(MSG_SELFTEST_CHECK_BED));
	if (_step == 9) lcd_puts_P(_T(MSG_SELFTEST_CHECK_FSENSOR));
	if (_step == 10) lcd_puts_P(_T(MSG_SELFTEST_CHECK_FSENSOR));
	if (_step == 11) lcd_puts_P(_i("All correct      "));////MSG_SELFTEST_CHECK_ALLCORRECT c=20 r=0
	if (_step == 12) lcd_puts_P(_T(MSG_SELFTEST_FAILED));
	if (_step == 13) lcd_puts_P(PSTR("Calibrating home"));

	lcd_set_cursor(0, 1);
	lcd_puts_P(separator);
	if ((_step >= -1) && (_step <= 1))
	{
		//SERIAL_ECHOLNPGM("Fan test");
		lcd_puts_at_P(0, 2, _i("Extruder fan:"));////MSG_SELFTEST_EXTRUDER_FAN_SPEED c=18 r=0
		lcd_set_cursor(18, 2);
		(_step < 0) ? lcd_print(_indicator) : lcd_print("OK");
		lcd_puts_at_P(0, 3, _i("Print fan:"));////MSG_SELFTEST_PRINT_FAN_SPEED c=18 r=0
		lcd_set_cursor(18, 3);
		(_step < 1) ? lcd_print(_indicator) : lcd_print("OK");
	}
	else if (_step >= 9 && _step <= 10)
	{
		lcd_puts_at_P(0, 2, _i("Filament sensor:"));////MSG_SELFTEST_FILAMENT_SENSOR c=18 r=0
		lcd_set_cursor(18, 2);
		(_step == 9) ? lcd_print(_indicator) : lcd_print("OK");
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
	lcd_set_cursor(_col, _row);

	switch (_state)
	{
	case 1:
		lcd_print(_name);
		lcd_set_cursor(_col + strlen(_name), _row);
		lcd_print(":");
		lcd_set_cursor(_col + strlen(_name) + 1, _row);
		lcd_print(_indicator);
		break;
	case 2:
		lcd_print(_name);
		lcd_set_cursor(_col + strlen(_name), _row);
		lcd_print(":");
		lcd_set_cursor(_col + strlen(_name) + 1, _row);
		lcd_print("OK");
		break;
	default:
		lcd_print(_name);
	}
}


/** End of menus **/

/** Menu action functions **/

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
	strncpy_P(lcd_status_message, _T(WELCOME_MSG), LCD_WIDTH);
	lcd_finishstatus();
	return result;
	
}

static void menu_action_sdfile(const char* filename)
{
  loading_flag = false;
  char cmd[30];
  char* c;
  bool result = true;
  sprintf_P(cmd, PSTR("M23 %s"), filename);
  for (c = &cmd[4]; *c; c++)
    *c = tolower(*c);

  const char end[5] = ".gco";

  //we are storing just first 8 characters of 8.3 filename assuming that extension is always ".gco"
  for (int i = 0; i < 8; i++) {
	  if (strcmp((cmd + i + 4), end) == 0) { 
		  //filename is shorter then 8.3, store '\0' character on position where ".gco" string was found to terminate stored string properly
 		  eeprom_write_byte((uint8_t*)EEPROM_FILENAME + i, '\0');
		  break;
	  }
	  else {
		  eeprom_write_byte((uint8_t*)EEPROM_FILENAME + i, cmd[i + 4]);
	  }
  }

  uint8_t depth = (uint8_t)card.getWorkDirDepth();
  eeprom_write_byte((uint8_t*)EEPROM_DIR_DEPTH, depth);

  for (uint8_t i = 0; i < depth; i++) {
	  for (int j = 0; j < 8; j++) {
		  eeprom_write_byte((uint8_t*)EEPROM_DIRS + j + 8 * i, dir_names[i][j]);
	  }
  }
  
  if (!check_file(filename)) {
	  result = lcd_show_fullscreen_message_yes_no_and_wait_P(_i("File incomplete. Continue anyway?"), false, false);////MSG_FILE_INCOMPLETE c=20 r=2
	  lcd_update_enable(true);
  }
  if (result) {
	  enquecommand(cmd);
	  enquecommand_P(PSTR("M24"));
  }

  lcd_return_to_status();
}

void menu_action_sddirectory(const char* filename)
{
	uint8_t depth = (uint8_t)card.getWorkDirDepth();

	strcpy(dir_names[depth], filename);
	MYSERIAL.println(dir_names[depth]);
  card.chdir(filename);
  lcd_encoder = 0;
}

/** LCD API **/

void ultralcd_init()
{
    {
        uint8_t autoDepleteRaw = eeprom_read_byte(reinterpret_cast<uint8_t*>(EEPROM_AUTO_DEPLETE));
        if (0xff == autoDepleteRaw) lcd_autoDeplete = false;
        else lcd_autoDeplete = autoDepleteRaw;

    }
	lcd_init();
	lcd_refresh();
	lcd_longpress_func = menu_lcd_longpress_func;
	lcd_charsetup_func = menu_lcd_charsetup_func;
	lcd_lcdupdate_func = menu_lcd_lcdupdate_func;
	menu_menu = lcd_status_screen;
	menu_lcd_charsetup_func();

  SET_INPUT(BTN_EN1);
  SET_INPUT(BTN_EN2);
  WRITE(BTN_EN1, HIGH);
  WRITE(BTN_EN2, HIGH);
#if BTN_ENC > 0
  SET_INPUT(BTN_ENC);
  WRITE(BTN_ENC, HIGH);
#endif

#if defined (SDSUPPORT) && defined(SDCARDDETECT) && (SDCARDDETECT > 0)
  pinMode(SDCARDDETECT, INPUT);
  WRITE(SDCARDDETECT, HIGH);
  lcd_oldcardstatus = IS_SD_INSERTED;
#endif//(SDCARDDETECT > 0)
  lcd_encoder_diff = 0;
}





void lcd_printer_connected() {
	printer_connected = true;
}

static void lcd_send_status() {
	if (farm_mode && no_response && ((millis() - NcTime) > (NC_TIME * 1000))) {
		//send important status messages periodicaly
		prusa_statistics(important_status, saved_filament_type);
		NcTime = millis();
#ifdef FARM_CONNECT_MESSAGE
		lcd_connect_printer();
#endif //FARM_CONNECT_MESSAGE
	}
}

#ifdef FARM_CONNECT_MESSAGE
static void lcd_connect_printer() {
	lcd_update_enable(false);
	lcd_clear();
	
	int i = 0;
	int t = 0;
	lcd_set_custom_characters_progress();
	lcd_puts_at_P(0, 0, _i("Connect printer to")); 
	lcd_puts_at_P(0, 1, _i("monitoring or hold"));
	lcd_puts_at_P(0, 2, _i("the knob to continue"));
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
			lcd_puts_at_P(0, 3, PSTR("                    "));
		}
		if (i!=0) lcd_puts_at_P((i * 20) / (NC_BUTTON_LONG_PRESS * 10), 3, "\x01");
		if (i == NC_BUTTON_LONG_PRESS * 10) {
			no_response = false;
		}
	}
	lcd_set_custom_characters_degree();
	lcd_update_enable(true);
	lcd_update(2);
}
#endif //FARM_CONNECT_MESSAGE

void lcd_ping() { //chceck if printer is connected to monitoring when in farm mode
	if (farm_mode) {
		bool empty = is_buffer_empty();
		if ((millis() - PingTime) * 0.001 > (empty ? PING_TIME : PING_TIME_LONG)) { //if commands buffer is empty use shorter time period
																							  //if there are comamnds in buffer, some long gcodes can delay execution of ping command
																							  //therefore longer period is used
			printer_connected = false;
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
  lcd_draw_update = 2;

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
  lcd_return_to_status();
}
void lcd_reset_alert_level()
{
  lcd_status_message_level = 0;
}

uint8_t get_message_level()
{
	return lcd_status_message_level;
}


void menu_lcd_longpress_func(void)
{
	move_menu_scale = 1.0;
	menu_submenu(lcd_move_z);
}

void menu_lcd_charsetup_func(void)
{
	if (menu_menu == lcd_status_screen)
		lcd_set_custom_characters_degree();
	else
		lcd_set_custom_characters_arrows();
}

static inline bool z_menu_expired()
{
    return (menu_menu == lcd_babystep_z
         && lcd_timeoutToStatus.expired(LCD_TIMEOUT_TO_STATUS_BABYSTEP_Z));
}
static inline bool other_menu_expired()
{
    return (menu_menu != lcd_status_screen
            && menu_menu != lcd_babystep_z
            && lcd_timeoutToStatus.expired(LCD_TIMEOUT_TO_STATUS));
}
static inline bool forced_menu_expire()
{
    bool retval = (menu_menu != lcd_status_screen
            && forceMenuExpire);
    forceMenuExpire = false;
    return retval;
}

void menu_lcd_lcdupdate_func(void)
{
#if (SDCARDDETECT > 0)
	if ((IS_SD_INSERTED != lcd_oldcardstatus))
	{
		lcd_draw_update = 2;
		lcd_oldcardstatus = IS_SD_INSERTED;
		lcd_refresh(); // to maybe revive the LCD if static electricity killed it.
		if (lcd_oldcardstatus)
		{
			card.initsd();
			LCD_MESSAGERPGM(_i("Card inserted"));////MSG_SD_INSERTED c=0 r=0
			//get_description();
		}
		else
		{
			card.release();
			LCD_MESSAGERPGM(_i("Card removed"));////MSG_SD_REMOVED c=0 r=0
		}
	}
#endif//CARDINSERTED
	if (lcd_next_update_millis < millis())
	{
		if (abs(lcd_encoder_diff) >= ENCODER_PULSES_PER_STEP)
		{
			if (lcd_draw_update == 0)
			lcd_draw_update = 1;
			lcd_encoder += lcd_encoder_diff / ENCODER_PULSES_PER_STEP;
			lcd_encoder_diff = 0;
			lcd_timeoutToStatus.start();
		}

		if (LCD_CLICKED) lcd_timeoutToStatus.start();

		(*menu_menu)();

		if (z_menu_expired() || other_menu_expired() || forced_menu_expire())
		{
		// Exiting a menu. Let's call the menu function the last time with menu_leaving flag set to true
		// to give it a chance to save its state.
		// This is useful for example, when the babystep value has to be written into EEPROM.
			if (menu_menu != NULL)
			{
				menu_leaving = 1;
				(*menu_menu)();
				menu_leaving = 0;
			}
			lcd_clear();
			lcd_return_to_status();
			lcd_draw_update = 2;
		}
		if (lcd_draw_update == 2) lcd_clear();
		if (lcd_draw_update) lcd_draw_update--;
		lcd_next_update_millis = millis() + LCD_UPDATE_INTERVAL;
	}
	if (!SdFatUtil::test_stack_integrity()) stack_error();
	lcd_ping(); //check that we have received ping command if we are in farm mode
	lcd_send_status();
	if (lcd_commands_type == LCD_COMMAND_V2_CAL) lcd_commands();
}

