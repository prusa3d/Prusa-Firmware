//! @file
//! @date Aug 28, 2019
//! @author mkbel
//! @brief LCD

#include "temperature.h"
#include "ultralcd.h"
#include "conv2str.h"
#include "fsensor.h"
#include "Marlin.h"
#include "language.h"
#include "cardreader.h"
#include "temperature.h"
#include "stepper.h"
#include "ConfigurationStore.h"
#include "printers.h"
#include <string.h>


#include "lcd.h"
#include "menu.h"

#include "backlight.h"

#include "util.h"
#include "mesh_bed_leveling.h"
#include "mesh_bed_calibration.h"

//#include "Configuration.h"
#include "cmdqueue.h"

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
#include "first_lay_cal.h"

#include "fsensor.h"
#include "adc.h"
#include "config.h"

#ifndef LA_NOCOMPAT
#include "la10compat.h"
#endif


int clock_interval = 0;

static void lcd_sd_updir();
static void lcd_mesh_bed_leveling_settings();
#ifdef LCD_BL_PIN
static void lcd_backlight_menu();
#endif

int8_t ReInitLCD = 0;
uint8_t scrollstuff = 0;

int8_t SilentModeMenu = SILENT_MODE_OFF;
uint8_t SilentModeMenu_MMU = 1; //activate mmu unit stealth mode

int8_t FSensorStateMenu = 1;

#ifdef IR_SENSOR_ANALOG
bool bMenuFSDetect=false;
#endif //IR_SENSOR_ANALOG

LcdCommands lcd_commands_type = LcdCommands::Idle;
static uint8_t lcd_commands_step = 0;

CustomMsg custom_message_type = CustomMsg::Status;
unsigned int custom_message_state = 0;


bool isPrintPaused = false;
uint8_t farm_mode = 0;
int farm_timer = 8;
uint8_t farm_status = 0;
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

static uint8_t lay1cal_filament = 0;


static const char separator[] PROGMEM = "--------------------";

/** forward declarations **/

static const char* lcd_display_message_fullscreen_nonBlocking_P(const char *msg, uint8_t &nlines);
// void copy_and_scalePID_i();
// void copy_and_scalePID_d();

/* Different menus */
//static void lcd_status_screen();                // NOT static due to using inside "Marlin_main" module ("manage_inactivity()")
#if (LANG_MODE != 0)
static void lcd_language_menu();
#endif
static void lcd_main_menu();
static void lcd_tune_menu();
//static void lcd_move_menu();
static void lcd_settings_menu();
static void lcd_calibration_menu();
static void lcd_control_temperature_menu();
#ifdef TMC2130
static void lcd_settings_linearity_correction_menu_save();
#endif
static void prusa_stat_printerstatus(int _status);
static void prusa_stat_farm_number();
static void prusa_stat_diameter();
static void prusa_stat_temperatures();
static void prusa_stat_printinfo();
static void lcd_menu_xyz_y_min();
static void lcd_menu_xyz_skew();
static void lcd_menu_xyz_offset();
static void lcd_menu_fails_stats_mmu();
static void lcd_menu_fails_stats_mmu_print();
static void lcd_menu_fails_stats_mmu_total();
static void mmu_unload_filament();
static void lcd_v2_calibration();
//static void lcd_menu_show_sensors_state();      // NOT static due to using inside "Marlin_main" module ("manage_inactivity()")

static void mmu_fil_eject_menu();
static void mmu_load_to_nozzle_menu();
static void preheat_or_continue();

#ifdef MMU_HAS_CUTTER
static void mmu_cut_filament_menu();
#endif //MMU_HAS_CUTTER

#if defined(TMC2130) || defined(FILAMENT_SENSOR)
static void lcd_menu_fails_stats();
#endif //TMC2130 or FILAMENT_SENSOR

#ifdef TMC2130
static void lcd_belttest_v();
#endif //TMC2130

static void lcd_selftest_v();

#ifdef TMC2130
static void reset_crash_det(unsigned char axis);
static bool lcd_selfcheck_axis_sg(unsigned char axis);
#else
static bool lcd_selfcheck_axis(int _axis, int _travel);
static bool lcd_selfcheck_pulleys(int axis);
#endif //TMC2130
static bool lcd_selfcheck_endstops();

static bool lcd_selfcheck_check_heater(bool _isbed);
enum class TestScreen : uint_least8_t
{
    ExtruderFan,
    PrintFan,
    FansOk,
    EndStops,
    AxisX,
    AxisY,
    AxisZ,
    Bed,
    Hotend,
    HotendOk,
    Fsensor,
    FsensorOk,
    AllCorrect,
    Failed,
    Home,
};

enum class TestError : uint_least8_t
{
    Heater,
    Bed,
    Endstops,
    Motor,
    Endstop,
    PrintFan,
    ExtruderFan,
    Pulley,
    Axis,
    SwappedFan,
    WiringFsensor,
    TriggeringFsensor,
    FsensorLevel
};

static int  lcd_selftest_screen(TestScreen screen, int _progress, int _progress_scale, bool _clear, int _delay);
static void lcd_selftest_screen_step(int _row, int _col, int _state, const char *_name, const char *_indicator);
static bool lcd_selftest_manual_fan_check(int _fan, bool check_opposite,
	bool _default=false);

#ifdef FANCHECK
/** Enumerate for lcd_selftest_fan_auto function.
 */
enum class FanCheck : uint_least8_t {
    Success,
    PrintFan,
    ExtruderFan,
    SwappedFan,
};

/**
 * Try to check fan working and wiring.
 *
 * @param _fan i fan number 0 means extruder fan, 1 means print fan.
 *
 * @returns a TestError noerror, extruderFan, printFan or swappedFan.
 */
static FanCheck lcd_selftest_fan_auto(int _fan);
#endif //FANCHECK

#ifdef PAT9125
static bool lcd_selftest_fsensor();
#endif //PAT9125
static bool selftest_irsensor();
#ifdef IR_SENSOR_ANALOG
static bool lcd_selftest_IRsensor(bool bStandalone=false);
static void lcd_detect_IRsensor();
#endif //IR_SENSOR_ANALOG
static void lcd_selftest_error(TestError error, const char *_error_1, const char *_error_2);
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
#ifdef MMU_HAS_CUTTER
static void lcd_cutter_enabled();
#endif
#ifdef SNMM
static char snmm_stop_print_menu();
#endif //SNMM
#ifdef SDCARD_SORT_ALPHA
 static void lcd_sort_type_set();
#endif
static void lcd_babystep_z();
static void lcd_send_status();
#ifdef FARM_CONNECT_MESSAGE
static void lcd_connect_printer();
#endif //FARM_CONNECT_MESSAGE

//! Beware: has side effects - forces lcd_draw_update to 2, which means clear the display
void lcd_finishstatus();

static void lcd_sdcard_menu();
static void lcd_sheet_menu();

#ifdef DELTA_CALIBRATION_MENU
static void lcd_delta_calibrate_menu();
#endif // DELTA_CALIBRATION_MENU


/* Different types of actions that can be used in menu items. */
static void menu_action_sdfile(const char* filename);
static void menu_action_sddirectory(const char* filename);

#define ENCODER_FEEDRATE_DEADZONE 10

#define STATE_NA 255
#define STATE_OFF 0
#define STATE_ON 1

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

uint8_t selected_sheet = 0;

bool ignore_click = false;
bool wait_for_unclick;

// place-holders for Ki and Kd edits
#ifdef PIDTEMP
// float raw_Ki, raw_Kd;
#endif

bool bMain;                                       // flag (i.e. 'fake parameter') for 'lcd_sdcard_menu()' function
bool bSettings;                                   // flag (i.e. 'fake parameter') for 'lcd_hw_setup_menu()' function



const char STR_SEPARATOR[] PROGMEM = "------------";

static void lcd_implementation_drawmenu_sdfile(uint8_t row, const char* longFilename)
{
    char c;
    uint8_t n = LCD_WIDTH - 1;
    lcd_set_cursor(0, row);
	lcd_print((lcd_encoder == menu_item)?'>':' ');
    while( ((c = *longFilename) != '\0') && (n>0) )
    {
        lcd_print(c);
        longFilename++;
        n--;
    }
    lcd_space(n);
}
static void lcd_implementation_drawmenu_sddirectory(uint8_t row, const char* longFilename)
{
    char c;
    uint8_t n = LCD_WIDTH - 2;
    lcd_set_cursor(0, row);
	lcd_print((lcd_encoder == menu_item)?'>':' ');
	lcd_print(LCD_STR_FOLDER[0]);
    while( ((c = *longFilename) != '\0') && (n>0) )
    {
        lcd_print(c);
        longFilename++;
        n--;
    }
    lcd_space(n);
}



#define MENU_ITEM_SDDIR(str_fn, str_fnl) do { if (menu_item_sddir(str_fn, str_fnl)) return; } while (0)
#define MENU_ITEM_SDFILE(str_fn, str_fnl) do { if (menu_item_sdfile(str_fn, str_fnl)) return; } while (0)


uint8_t menu_item_sddir(const char* str_fn, char* str_fnl)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update)
		{
			lcd_implementation_drawmenu_sddirectory(menu_row, (str_fnl[0] == '\0') ? str_fn : str_fnl);
		}
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			menu_clicked = false;
			lcd_update_enabled = 0;
			menu_action_sddirectory(str_fn);
			lcd_update_enabled = 1;
			/* return */ menu_item_ret();
			return 1;
		}
	}
	menu_item++;
	return 0;
}

static uint8_t menu_item_sdfile(const char* str_fn, char* str_fnl)
{
	if (menu_item == menu_line)
	{
		if (lcd_draw_update)
		{
			lcd_implementation_drawmenu_sdfile(menu_row, (str_fnl[0] == '\0') ? str_fn : str_fnl);
		}
		if (menu_clicked && (lcd_encoder == menu_item))
		{
			lcd_consume_click();
			menu_action_sdfile(str_fn);
			/* return */ menu_item_ret();
			return 1;
		}
	}
	menu_item++;
	return 0;
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
    if (custom_message_type == CustomMsg::MeshBedLeveling)
        lcd_puts_P(_N("Z   --- "));
    else
		lcd_printf_P(_N("Z%6.2f%c"), current_position[Z_AXIS], axis_known_position[Z_AXIS]?' ':'?');
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
	if (!num || heating_status) // either not printing or heating
	{
		const int8_t sheetNR = eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet));
		const int8_t nextSheet = eeprom_next_initialized_sheet(sheetNR);
		if ((nextSheet >= 0) && (sheetNR != nextSheet))
		{
			char sheet[8];
			eeprom_read_block(sheet, EEPROM_Sheets_base->s[sheetNR].name, 7);
			sheet[7] = '\0';
			lcd_printf_P(PSTR("%-7s"),sheet);
			return; //do not also print the percentage
		}
	}
	sprintf_P(per, num?_N("%3hhd"):_N("---"), calc_percent_done());
	lcd_printf_P(_N("%3S%3s%%"), src, per);
}

// Print extruder status (5 chars total)
void lcdui_print_extruder(void)
{
	int chars = 0;
	if (mmu_extruder == tmp_extruder) {
		if (mmu_extruder == MMU_FILAMENT_UNKNOWN) chars = lcd_printf_P(_N(" F?"));
		else chars = lcd_printf_P(_N(" F%u"), mmu_extruder + 1);
	}
	else
	{
		if (mmu_extruder == MMU_FILAMENT_UNKNOWN) chars = lcd_printf_P(_N(" ?>%u"), tmp_extruder + 1);
		else chars = lcd_printf_P(_N(" %u>%u"), mmu_extruder + 1, tmp_extruder + 1);
	}
	lcd_space(5 - chars);
}

// Print farm number (5 chars total)
void lcdui_print_farm(void)
{
	lcd_printf_P(_N(" FRM "));
}

#ifdef CMD_DIAGNOSTICS
// Print CMD queue diagnostic (8 chars total)
void lcdui_print_cmd_diag(void)
{
	lcd_set_cursor(LCD_WIDTH - 8 -1, 2);
	lcd_puts_P(PSTR("      C"));
	lcd_print(buflen);	// number of commands in cmd buffer
	if (buflen < 9) lcd_print(' ');
}
#endif //CMD_DIAGNOSTICS

// Print time (8 chars total)
void lcdui_print_time(void)
{
    //if remaining print time estimation is available print it else print elapsed time
    int chars = 0;
    if (PRINTER_ACTIVE) {
        uint16_t print_t = PRINT_TIME_REMAINING_INIT;
        uint16_t print_tr = PRINT_TIME_REMAINING_INIT;
        uint16_t print_tc = PRINT_TIME_REMAINING_INIT;
        char suff = ' ';
        char suff_doubt = ' ';

#ifdef TMC2130
        if (SilentModeMenu != SILENT_MODE_OFF) {
            if (print_time_remaining_silent != PRINT_TIME_REMAINING_INIT)
                print_tr = print_time_remaining_silent;
//#ifdef CLOCK_INTERVAL_TIME
            if (print_time_to_change_silent != PRINT_TIME_REMAINING_INIT)
                print_tc = print_time_to_change_silent;
//#endif //CLOCK_INTERVAL_TIME
        } else {
#endif //TMC2130
            if (print_time_remaining_normal != PRINT_TIME_REMAINING_INIT)
                print_tr = print_time_remaining_normal;
//#ifdef CLOCK_INTERVAL_TIME
            if (print_time_to_change_normal != PRINT_TIME_REMAINING_INIT)
                print_tc = print_time_to_change_normal;
//#endif //CLOCK_INTERVAL_TIME
#ifdef TMC2130
        }
#endif //TMC2130

//#ifdef CLOCK_INTERVAL_TIME
        if (clock_interval == CLOCK_INTERVAL_TIME*2)
            clock_interval = 0;

        clock_interval++;

        if (print_tc != PRINT_TIME_REMAINING_INIT && clock_interval > CLOCK_INTERVAL_TIME) {
            print_t = print_tc;
            suff = 'C';
        } else
//#endif //CLOCK_INTERVAL_TIME 
        if (print_tr != PRINT_TIME_REMAINING_INIT) {
            print_t = print_tr;
            suff = 'R';
        } else 
            print_t = _millis() / 60000 - starttime / 60000;

        if (feedmultiply != 100 && (print_t == print_tr || print_t == print_tc)) {
            suff_doubt = '?';
            print_t = 100ul * print_t / feedmultiply;
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

//! @Brief Print status line on status screen
void lcdui_print_status_line(void)
{
    if (heating_status) { // If heating flag, show progress of heating
        heating_status_counter++;
        if (heating_status_counter > 13) {
            heating_status_counter = 0;
        }
        lcd_set_cursor(7, 3);
        lcd_space(13);

        for (unsigned int dots = 0; dots < heating_status_counter; dots++) {
            lcd_putc_at(7 + dots, 3, '.');
        }
        switch (heating_status) {
        case 1:
            lcd_puts_at_P(0, 3, _T(MSG_HEATING));
            break;
        case 2:
            lcd_puts_at_P(0, 3, _T(MSG_HEATING_COMPLETE));
            heating_status = 0;
            heating_status_counter = 0;
            break;
        case 3:
            lcd_puts_at_P(0, 3, _T(MSG_BED_HEATING));
            break;
        case 4:
            lcd_puts_at_P(0, 3, _T(MSG_BED_DONE));
            heating_status = 0;
            heating_status_counter = 0;
            break;
        default:
            break;
        }
    }
    else if ((IS_SD_PRINTING) && (custom_message_type == CustomMsg::Status)) { // If printing from SD, show what we are printing
		const char* longFilenameOLD = (card.longFilename[0] ? card.longFilename : card.filename);
        if(strlen(longFilenameOLD) > LCD_WIDTH) {
            int inters = 0;
            int gh = scrollstuff;
            while (((gh - scrollstuff) < LCD_WIDTH) && (inters == 0)) {
                if (longFilenameOLD[gh] == '\0') {
                    lcd_set_cursor(gh - scrollstuff, 3);
                    lcd_print(longFilenameOLD[gh - 1]);
                    scrollstuff = 0;
                    gh = scrollstuff;
                    inters = 1;
                } else {
                    lcd_set_cursor(gh - scrollstuff, 3);
                    lcd_print(longFilenameOLD[gh - 1]);
                    gh++;
                }
            }
            scrollstuff++;
        } else {
            lcd_printf_P(PSTR("%-20s"), longFilenameOLD);
        }
    } else { // Otherwise check for other special events
        switch (custom_message_type) {
        case CustomMsg::MsgUpdate: //Short message even while printing from SD
        case CustomMsg::Status: // Nothing special, print status message normally
        case CustomMsg::M0Wait: // M0/M1 Wait command working even from SD
            lcd_print(lcd_status_message);
        break;
        case CustomMsg::MeshBedLeveling: // If mesh bed leveling in progress, show the status
            if (custom_message_state > 10) {
                lcd_set_cursor(0, 3);
                lcd_space(LCD_WIDTH);
                lcd_puts_at_P(0, 3, _T(MSG_CALIBRATE_Z_AUTO));
                lcd_puts_P(PSTR(" : "));
                lcd_print(custom_message_state-10);
            } else {
                if (custom_message_state == 3)
                {
                    lcd_puts_P(_T(WELCOME_MSG));
                    lcd_setstatuspgm(_T(WELCOME_MSG));
                    custom_message_type = CustomMsg::Status;
                }
                if (custom_message_state > 3 && custom_message_state <= 10 ) {
                    lcd_set_cursor(0, 3);
                    lcd_space(19);
                    lcd_puts_at_P(0, 3, _i("Calibration done"));////MSG_HOMEYZ_DONE c=20
                    custom_message_state--;
                }
            }
            break;
        case CustomMsg::FilamentLoading: // If loading filament, print status
            lcd_print(lcd_status_message);
            break;
        case CustomMsg::PidCal: // PID tuning in progress
            lcd_print(lcd_status_message);
            if (pid_cycle <= pid_number_of_cycles && custom_message_state > 0) {
                lcd_set_cursor(10, 3);
                lcd_print(itostr3(pid_cycle));
                lcd_print('/');
                lcd_print(itostr3left(pid_number_of_cycles));
            }
            break;
        case CustomMsg::TempCal: // PINDA temp calibration in progress
            char statusLine[LCD_WIDTH + 1];
            sprintf_P(statusLine, PSTR("%-20S"), _T(MSG_TEMP_CALIBRATION));
            char progress[4];
            sprintf_P(progress, PSTR("%d/6"), custom_message_state);
            memcpy(statusLine + 12, progress, sizeof(progress) - 1);
            lcd_set_cursor(0, 3);
            lcd_print(statusLine);
            break;
        case CustomMsg::TempCompPreheat: // temp compensation preheat
            lcd_puts_at_P(0, 3, _i("PINDA Heating"));////MSG_PINDA_PREHEAT c=20
            if (custom_message_state <= PINDA_HEAT_T) {
                lcd_puts_P(PSTR(": "));
                lcd_print(custom_message_state); //seconds
                lcd_print(' ');
            }
            break;
        case CustomMsg::Resuming: //Resuming
            lcd_puts_at_P(0, 3, _T(MSG_RESUMING_PRINT));
            break;
        }
    }

    // Fill the rest of line to have nice and clean output
    for(int fillspace = 0; fillspace < LCD_WIDTH; fillspace++)
        if ((lcd_status_message[fillspace] <= 31 ))
            lcd_print(' ');
}

//! @brief Show Status Screen
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |N 000/000D  Z000.0  |
//! |B 000/000D  F100%   |
//! |USB100% T0  t--:--  |
//! |Status line.........|
//! ----------------------
//! N - nozzle temp symbol	LCD_STR_THERMOMETER
//! D - Degree sysmbol		LCD_STR_DEGREE
//! B - bed temp symbol 	LCD_STR_BEDTEMP
//! F - feedrate symbol 	LCD_STR_FEEDRATE
//! t - clock symbol 		LCD_STR_THERMOMETER
//! @endcode
void lcdui_print_status_screen(void)
{

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
void lcd_status_screen()                          // NOT static due to using inside "Marlin_main" module ("manage_inactivity()")
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
				if(loading_flag)
					prusa_statistics(22);
				break;
			case 5:
				if (IS_SD_PRINTING)
					prusa_statistics(20);
				break;
			}
		} // end of farm_mode

		lcd_status_update_delay = 10;   /* redraw the main screen every second. This is easier then trying keep track of all things that change on the screen */
		if (lcd_commands_type != LcdCommands::Idle)
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

	if (current_click
		&& ( menu_block_entering_on_serious_errors == SERIOUS_ERR_NONE ) // or a serious error blocks entering the menu
	)
	{
		menu_depth = 0; //redundant, as already done in lcd_return_to_status(), just to be sure
		menu_submenu(lcd_main_menu);
		lcd_refresh(); // to maybe revive the LCD if static electricity killed it.
	}
}

void lcd_commands()
{
	if (lcd_commands_type == LcdCommands::LongPause)
	{
		if (!blocks_queued() && !homing_flag)
		{
			lcd_setstatuspgm(_i("Print paused"));////MSG_PRINT_PAUSED c=20
            lcd_commands_type = LcdCommands::Idle;
            lcd_commands_step = 0;
            long_pause();
		}
	}


#ifdef SNMM
	if (lcd_commands_type == LcdCommands::Layer1Cal)
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

	if (lcd_commands_type == LcdCommands::Layer1Cal)
	{
		char cmd1[30];

		if(lcd_commands_step>1) lcd_timeoutToStatus.start(); //if user dont confirm live adjust Z value by pressing the knob, we are saving last value by timeout to status screen

        if (!blocks_queued() && cmd_buffer_empty() && !saved_printing)
        {
            switch(lcd_commands_step)
            {
            case 0:
                lcd_commands_step = 11;
                break;
            case 11:
                lay1cal_wait_preheat();
                lcd_commands_step = 10;
                break;
            case 10:
                lay1cal_load_filament(cmd1, lay1cal_filament);
                lcd_commands_step = 9;
                break;
            case 9:
                lcd_clear();
                menu_depth = 0;
                menu_submenu(lcd_babystep_z);
                lay1cal_intro_line();
                lcd_commands_step = 8;
                break;
            case 8:
                lay1cal_before_meander();
                lcd_commands_step = 7;
                break;
            case 7:
                lay1cal_meander(cmd1);
                lcd_commands_step = 6;
                break;
            case 6:
                for (uint8_t i = 0; i < 4; i++)
                {
                    lay1cal_square(cmd1, i);
                }
                lcd_commands_step = 5;
                break;
            case 5:
                for (uint8_t i = 4; i < 8; i++)
                {
                    lay1cal_square(cmd1, i);
                }
                lcd_commands_step = 4;
                break;
            case 4:
                for (uint8_t i = 8; i < 12; i++)
                {
                    lay1cal_square(cmd1, i);
                }
                lcd_commands_step = 3;
                break;
            case 3:
                for (uint8_t i = 12; i < 16; i++)
                {
                    lay1cal_square(cmd1, i);
                }
                lcd_commands_step = 2;
                break;
            case 2:
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
                break;
            case 1:
                lcd_setstatuspgm(_T(WELCOME_MSG));
                lcd_commands_step = 0;
                lcd_commands_type = LcdCommands::Idle;
                if (eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE) == 1)
                {
                    lcd_wizard(WizState::RepeatLay1Cal);
                }
                break;
		}
			}
		}

#endif // not SNMM

	if (lcd_commands_type == LcdCommands::FarmModeConfirm)   /// farm mode confirm
	{

		if (lcd_commands_step == 0) { lcd_commands_step = 6; }

		if (lcd_commands_step == 1 && !blocks_queued())
		{
			lcd_commands_step = 0;
			lcd_commands_type = LcdCommands::Idle;
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
	if (lcd_commands_type == LcdCommands::PidExtruder) {
		char cmd1[30];
		
		if (lcd_commands_step == 0) {
			custom_message_type = CustomMsg::PidCal;
			custom_message_state = 1;
			lcd_draw_update = 3;
			lcd_commands_step = 3;
		}
		if (lcd_commands_step == 3 && !blocks_queued()) { //PID calibration
			strcpy(cmd1, "M303 E0 S");
			strcat(cmd1, ftostr3(pid_temp));
			// setting the correct target temperature (for visualization) is done in PID_autotune
			enquecommand(cmd1);
			lcd_setstatuspgm(_i("PID cal."));////MSG_PID_RUNNING c=20
			lcd_commands_step = 2;
		}
		if (lcd_commands_step == 2 && pid_tuning_finished) { //saving to eeprom
			pid_tuning_finished = false;
			custom_message_state = 0;
			lcd_setstatuspgm(_i("PID cal. finished"));////MSG_PID_FINISHED c=20
			setAllTargetHotends(0);  // reset all hotends temperature including the number displayed on the main screen
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
			display_time = _millis();
			lcd_commands_step = 1;
		}
		if ((lcd_commands_step == 1) && ((_millis()- display_time)>2000)) { //calibration finished message
			lcd_setstatuspgm(_T(WELCOME_MSG));
			custom_message_type = CustomMsg::Status;
			pid_temp = DEFAULT_PID_TEMP;
			lcd_commands_step = 0;
			lcd_commands_type = LcdCommands::Idle;
		}
	}


}

void lcd_return_to_status()
{
	lcd_refresh(); // to maybe revive the LCD if static electricity killed it.
	menu_goto(lcd_status_screen, 0, false, true);
	menu_depth = 0;
    eFilamentAction = FilamentAction::None; // i.e. non-autoLoad
}

//! @brief Pause print, disable nozzle heater, move to park position, send host action "paused"
void lcd_pause_print()
{
    stop_and_save_print_to_ram(0.0, -default_retraction);
    lcd_return_to_status();
    isPrintPaused = true;
    if (LcdCommands::Idle == lcd_commands_type) {
        lcd_commands_type = LcdCommands::LongPause;
    }
    SERIAL_PROTOCOLLNRPGM(MSG_OCTOPRINT_PAUSED);
}

//! @brief Send host action "pause"
void lcd_pause_usb_print()
{
    SERIAL_PROTOCOLLNRPGM(MSG_OCTOPRINT_PAUSE);
}


float move_menu_scale;
static void lcd_move_menu_axis();



/* Menu implementation */

static void lcd_cooldown()
{
  setAllTargetHotends(0);
  setTargetBed(0);
  fanSpeed = 0;
  eFilamentAction = FilamentAction::None;
  lcd_return_to_status();
}

//! @brief append text label with a colon and format it into a fixed size output buffer
//! It would have been much easier if there was a ':' in the labels.
//! But since the texts like Bed, Nozzle and PINDA are used in other places
//! it is better to reuse these texts even though it requires some extra formatting code.
//! @param [in] ipgmLabel pointer to string in PROGMEM
//! @param [out] pointer to string in RAM which will receive the formatted text. Must be allocated to appropriate size
//! @param [in] dstSize allocated length of dst
static void pgmtext_with_colon(const char *ipgmLabel, char *dst, uint8_t dstSize){
    uint8_t i = 0;
    for(; i < dstSize - 2; ++i){ // 2 byte less than buffer, we'd be adding a ':' to the end
        uint8_t b = pgm_read_byte(ipgmLabel + i);
        if( ! b )
            break;
        dst[i] = b;
    }
    dst[i] = ':';               // append the colon
    ++i;
    for(; i < dstSize - 1; ++i) // fill the rest with spaces
        dst[i] = ' ';
    dst[dstSize-1] = '\0';      // terminate the string properly
}

//! @brief Show Extruder Info
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |Nozzle FAN: 0000 RPM|	MSG_NOZZLE_FAN c=10  SPEED c=3
//! |Print FAN:  0000 RPM|	MSG_PRINT_FAN c=10  SPEED c=3
//! |                    |
//! |                    |
//! ----------------------
//! @endcode
//! @todo Positioning of the messages and values on LCD aren't fixed to their exact place. This causes issues with translations.
void lcd_menu_extruder_info()                     // NOT static due to using inside "Marlin_main" module ("manage_inactivity()")
{

    // Display Nozzle fan RPM
    lcd_timeoutToStatus.stop(); //infinite timeout
    lcd_home();
    static const size_t maxChars = 12;
    char nozzle[maxChars], print[maxChars];
    pgmtext_with_colon(_i("Nozzle FAN"), nozzle, maxChars);  ////MSG_NOZZLE_FAN c=10
    pgmtext_with_colon(_i("Print FAN"), print, maxChars);  ////MSG_PRINT_FAN c=10
	lcd_printf_P(_N("%s %4d RPM\n" "%s %4d RPM\n"), nozzle, 60*fan_speed[0], print, 60*fan_speed[1] ); 
    menu_back_if_clicked();
}

static uint16_t __attribute__((noinline)) clamp999(uint16_t v){
    return v > 999 ? 999 : v;
}

//! @brief Show Fails Statistics MMU
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! | Main               |	MSG_MAIN c=18
//! | Last print         |	MSG_LAST_PRINT c=18
//! | Total              |	MSG_TOTAL c=6
//! |                    |
//! ----------------------
//! @endcode
static void lcd_menu_fails_stats_mmu()
{
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));
	MENU_ITEM_SUBMENU_P(_T(MSG_LAST_PRINT), lcd_menu_fails_stats_mmu_print);
	MENU_ITEM_SUBMENU_P(_T(MSG_TOTAL), lcd_menu_fails_stats_mmu_total);
	MENU_END();
}

//! @brief Show Last Print Failures Statistics MMU
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |Last print failures |	MSG_LAST_PRINT_FAILURES c=20
//! | MMU fails       000|	MSG_MMU_FAILS c=15
//! | MMU load fails  000|	MSG_MMU_LOAD_FAILS c=15
//! |                    |
//! ----------------------
//! @endcode
//! @todo Positioning of the messages and values on LCD aren't fixed to their exact place. This causes issues with translations.
static void lcd_menu_fails_stats_mmu_print()
{
	lcd_timeoutToStatus.stop(); //infinite timeout
    lcd_home();
    lcd_printf_P(PSTR("%S\n" " %-16.16S%-3d\n" " %-16.16S%-3d"), 
        _T(MSG_LAST_PRINT_FAILURES),
        _T(MSG_MMU_FAILS), clamp999( eeprom_read_byte((uint8_t*)EEPROM_MMU_FAIL) ),
        _T(MSG_MMU_LOAD_FAILS), clamp999( eeprom_read_byte((uint8_t*)EEPROM_MMU_LOAD_FAIL) ));
    menu_back_if_clicked_fb();
}

//! @brief Show Total Failures Statistics MMU
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |Total failures      |	MSG_TOTAL_FAILURES c=20
//! | MMU fails       000|	MSG_MMU_FAILS c=15
//! | MMU load fails  000|	MSG_MMU_LOAD_FAILS c=15
//! | MMU power fails 000|	MSG_MMU_POWER_FAILS c=15
//! ----------------------
//! @endcode
//! @todo Positioning of the messages and values on LCD aren't fixed to their exact place. This causes issues with translations.
static void lcd_menu_fails_stats_mmu_total()
{
	mmu_command(MmuCmd::S3);
	lcd_timeoutToStatus.stop(); //infinite timeout
    lcd_home();
    lcd_printf_P(PSTR("%S\n" " %-16.16S%-3d\n" " %-16.16S%-3d\n" " %-16.16S%-3d"), 
        _T(MSG_TOTAL_FAILURES),
        _T(MSG_MMU_FAILS), clamp999( eeprom_read_word((uint16_t*)EEPROM_MMU_FAIL_TOT) ),
        _T(MSG_MMU_LOAD_FAILS), clamp999( eeprom_read_word((uint16_t*)EEPROM_MMU_LOAD_FAIL_TOT) ),
        _i("MMU power fails"), clamp999( mmu_power_failures )); ////MSG_MMU_POWER_FAILS c=15
    menu_back_if_clicked_fb();
}

#if defined(TMC2130) && defined(FILAMENT_SENSOR)
static const char failStatsFmt[] PROGMEM = "%S\n" " %-16.16S%-3d\n" " %-16.16S%-3d\n" " %-7.7SX %-3d  Y %-3d";

//! @brief Show Total Failures Statistics MMU
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |Total failures      |	MSG_TOTAL_FAILURES c=20
//! | Power failures  000|	MSG_POWER_FAILURES c=15
//! | Fil. runouts    000|	MSG_FIL_RUNOUTS c=15
//! | Crash   X:000 Y:000|	MSG_CRASH c=7
//! ----------------------
//! @endcode
//! @todo Positioning of the messages and values on LCD aren't fixed to their exact place. This causes issues with translations.
static void lcd_menu_fails_stats_total()
{
	lcd_timeoutToStatus.stop(); //infinite timeout
	lcd_home();
    lcd_printf_P(failStatsFmt, 
        _T(MSG_TOTAL_FAILURES),
        _T(MSG_POWER_FAILURES), clamp999( eeprom_read_word((uint16_t*)EEPROM_POWER_COUNT_TOT) ),
        _T(MSG_FIL_RUNOUTS), clamp999( eeprom_read_word((uint16_t*)EEPROM_FERROR_COUNT_TOT) ),
        _T(MSG_CRASH),
            clamp999( eeprom_read_word((uint16_t*)EEPROM_CRASH_COUNT_X_TOT) ), 
            clamp999( eeprom_read_word((uint16_t*)EEPROM_CRASH_COUNT_Y_TOT) ));
    menu_back_if_clicked_fb();
}

//! @brief Show Last Print Failures Statistics
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |Last print failures |	MSG_LAST_PRINT_FAILURES c=20
//! | Power failures  000|	MSG_POWER_FAILURES c=15
//! | Fil. runouts    000|	MSG_FIL_RUNOUTS c=15
//! | Crash   X 000 Y 000|	MSG_CRASH c=7
//! ----------------------
//! @endcode
//! @brief Show Last Print Failures Statistics with PAT9125
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |Last print failures |	MSG_LAST_PRINT_FAILURES c=20
//! | Power failures  000|	MSG_POWER_FAILURES c=14
//! | Runouts H 000 S 000|	MSG_RUNOUTS c=7
//! | Crash   X:000 Y:000|	MSG_CRASH c=7
//! ----------------------
//! @endcode

//! @todo Positioning of the messages and values on LCD aren't fixed to their exact place. This causes issues with translations.
static void lcd_menu_fails_stats_print()
{
	lcd_timeoutToStatus.stop(); //infinite timeout
    uint8_t power = eeprom_read_byte((uint8_t*)EEPROM_POWER_COUNT);
    uint8_t filam = eeprom_read_byte((uint8_t*)EEPROM_FERROR_COUNT);
    uint8_t crashX = eeprom_read_byte((uint8_t*)EEPROM_CRASH_COUNT_X);
    uint8_t crashY = eeprom_read_byte((uint8_t*)EEPROM_CRASH_COUNT_Y);
    lcd_home();
#ifndef PAT9125
    lcd_printf_P(failStatsFmt,
        _T(MSG_LAST_PRINT_FAILURES),
        _T(MSG_POWER_FAILURES), power,
        _T(MSG_FIL_RUNOUTS), filam,
        _T(MSG_CRASH), crashX, crashY);
#else
    // On the MK3 include detailed PAT9125 statistics about soft failures
    lcd_printf_P(PSTR("%S\n"
                      " %-16.16S%-3d\n"
                      " %-7.7S H %-3d S %-3d\n"
                      " %-7.7S X %-3d Y %-3d"),
                 _T(MSG_LAST_PRINT_FAILURES),
                 _T(MSG_POWER_FAILURES), power,
                 _i("Runouts"), filam, fsensor_softfail, //MSG_RUNOUTS c=7
                 _T(MSG_CRASH), crashX, crashY);
#endif
    menu_back_if_clicked_fb();
}

//! @brief Open fail statistics menu
//! 
//! This version of function is used, when there is filament sensor,
//! power failure and crash detection.
//! There are Last print and Total menu items.
//! 
//! @code{.unparsed}
//! |01234567890123456789|
//! | Main               |	MSG_MAIN c=18
//! | Last print         |	MSG_LAST_PRINT c=18
//! | Total              |	MSG_TOTAL c=6
//! |                    |
//! ----------------------
//! @endcode

static void lcd_menu_fails_stats()
{
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));
	MENU_ITEM_SUBMENU_P(_T(MSG_LAST_PRINT), lcd_menu_fails_stats_print);
	MENU_ITEM_SUBMENU_P(_T(MSG_TOTAL), lcd_menu_fails_stats_total);
	MENU_END();
}

#elif defined(FILAMENT_SENSOR)
static const char failStatsFmt[] PROGMEM = "%S\n" " %-16.16S%-3d\n" "%S\n" " %-16.16S%-3d\n";
//! 
//! @brief Print last print and total filament run outs
//! 
//! This version of function is used, when there is filament sensor,
//! but no other sensors (e.g. power failure, crash detection).
//! 
//! Example screen:
//! @code{.unparsed}
//! |01234567890123456789|
//! |Last print failures |	MSG_LAST_PRINT_FAILURES c=20
//! | Fil. runouts    000|	MSG_FIL_RUNOUTS c=15
//! |Total failures      |	MSG_TOTAL_FAILURES c=20
//! | Fil. runouts    000|	MSG_FIL_RUNOUTS c=15
//! ----------------------
//! @endcode
//! @todo Positioning of the messages and values on LCD aren't fixed to their exact place. This causes issues with translations.
static void lcd_menu_fails_stats()
{
	lcd_timeoutToStatus.stop(); //infinite timeout
    uint8_t filamentLast = eeprom_read_byte((uint8_t*)EEPROM_FERROR_COUNT);
    uint16_t filamentTotal = clamp999( eeprom_read_word((uint16_t*)EEPROM_FERROR_COUNT_TOT) );
	lcd_home();
	lcd_printf_P(failStatsFmt, 
        _T(MSG_LAST_PRINT_FAILURES),
        _T(MSG_FIL_RUNOUTS), filamentLast,
        _T(MSG_TOTAL_FAILURES),
        _T(MSG_FIL_RUNOUTS), filamentTotal);

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

//! @brief Show Debug Information
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |RAM statistics      |	c=20
//! | SP_min:        0000|	c=14
//! | heap_start:    0000|	c=14
//! | heap_end:      0000|	c=14
//! ----------------------
//! @endcode
//! @todo Positioning of the messages and values on LCD aren't fixed to their exact place. This causes issues with translations.
static void lcd_menu_debug()
{
#ifdef DEBUG_STACK_MONITOR
	lcd_home();
	lcd_printf_P(PSTR("RAM statistics\n"  ////c=20
        " SP_min: 0x%04x\n"   ////c=14
        " heap_start: 0x%04x\n"   ////c=14
        " heap_end: 0x%04x"), SP_min, __malloc_heap_start, __malloc_heap_end);  ////c=14
#endif //DEBUG_STACK_MONITOR

	menu_back_if_clicked_fb();
}
#endif /* DEBUG_BUILD */

//! @brief common line print for lcd_menu_temperatures
//! @param [in] ipgmLabel pointer to string in PROGMEM
//! @param [in] value to be printed behind the label
static void lcd_menu_temperatures_line(const char *ipgmLabel, int value){
    static const size_t maxChars = 15;    
    char tmp[maxChars];
    pgmtext_with_colon(ipgmLabel, tmp, maxChars);
    lcd_printf_P(PSTR(" %s%3d\x01 \n"), tmp, value); // no need to add -14.14 to string alignment
}

//! @brief Show Temperatures
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! | Nozzle:        000D|	MSG_NOZZLE c=14
//! | Bed:           000D|	MSG_BEDc=14
//! | Ambient:       000D|	MSG_AMBIENTc=14
//! | PINDA:         000D|	MSG_PINDA c=14
//! ----------------------
//! D - Degree sysmbol		LCD_STR_DEGREE
//! @endcode
//! @todo Positioning of the messages and values on LCD aren't fixed to their exact place. This causes issues with translations.
static void lcd_menu_temperatures()
{
    lcd_timeoutToStatus.stop(); //infinite timeout
    lcd_home();
    lcd_menu_temperatures_line( _T(MSG_NOZZLE), (int)current_temperature[0] ); ////MSG_NOZZLE
    lcd_menu_temperatures_line( _T(MSG_BED), (int)current_temperature_bed );  ////MSG_BED
#ifdef AMBIENT_THERMISTOR
    lcd_menu_temperatures_line( _i("Ambient"), (int)current_temperature_ambient );  ////MSG_AMBIENT
#endif //AMBIENT_THERMISTOR
#ifdef PINDA_THERMISTOR
    lcd_menu_temperatures_line( _T(MSG_PINDA), (int)current_temperature_pinda );  ////MSG_PINDA
#endif //PINDA_THERMISTOR
    menu_back_if_clicked();
}

#if defined (VOLT_BED_PIN) || defined (VOLT_PWR_PIN) || defined(IR_SENSOR_ANALOG)
#define VOLT_DIV_R1 10000
#define VOLT_DIV_R2 2370
#define VOLT_DIV_FAC ((float)VOLT_DIV_R2 / (VOLT_DIV_R2 + VOLT_DIV_R1))

//! @brief Show Voltages
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |                    |
//! | PWR:         00.0V |	c=12
//! | Bed:         00.0V |	c=12
//! | IR :         00.0V |  c=12 optional
//! ----------------------
//! @endcode
//! @todo Positioning of the messages and values on LCD aren't fixed to their exact place. This causes issues with translations.
static void lcd_menu_voltages()
{
    lcd_timeoutToStatus.stop(); //infinite timeout
    float volt_pwr = VOLT_DIV_REF * ((float)current_voltage_raw_pwr / (1023 * OVERSAMPLENR)) / VOLT_DIV_FAC;
    float volt_bed = VOLT_DIV_REF * ((float)current_voltage_raw_bed / (1023 * OVERSAMPLENR)) / VOLT_DIV_FAC;
    lcd_home();
    lcd_printf_P(PSTR(" PWR:      %4.1fV\n" " BED:      %4.1fV"), volt_pwr, volt_bed);
#ifdef IR_SENSOR_ANALOG
    lcd_printf_P(PSTR("\n IR :       %3.1fV"), Raw2Voltage(current_voltage_raw_IR));
#endif //IR_SENSOR_ANALOG
    menu_back_if_clicked();
}
#endif //defined (VOLT_BED_PIN) || defined (VOLT_PWR_PIN) || defined(IR_SENSOR_ANALOG)

#ifdef TMC2130
//! @brief Show Belt Status
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! | Belt status        |	c=18
//! |  X:            000 |
//! |  Y:            000 |
//! |                    |
//! ----------------------
//! @endcode
//! @todo Positioning of the messages and values on LCD aren't fixed to their exact place. This causes issues with translations.
static void lcd_menu_belt_status()
{
	lcd_home();
    lcd_printf_P(PSTR("%S\n" " X %d\n" " Y %d"), _T(MSG_BELT_STATUS), eeprom_read_word((uint16_t*)(EEPROM_BELTSTATUS_X)), eeprom_read_word((uint16_t*)(EEPROM_BELTSTATUS_Y)));
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

//! @brief Show Preheat Menu
static void lcd_preheat_menu()
{
    eFilamentAction = FilamentAction::Preheat;
    lcd_generic_preheat_menu();
}


#ifdef MENU_DUMP
#include "xflash_dump.h"

static void lcd_dump_memory()
{
    lcd_beeper_quick_feedback();
    xfdump_dump();
    lcd_return_to_status();
}
#endif //MENU_DUMP
#ifdef MENU_SERIAL_DUMP
#include "Dcodes.h"

static void lcd_serial_dump()
{
    serial_dump_and_reset(dump_crash_reason::manual);
}
#endif //MENU_SERIAL_DUMP

#if defined(DEBUG_BUILD) && defined(EMERGENCY_HANDLERS)
#include <avr/wdt.h>

#ifdef WATCHDOG
static void lcd_wdr_crash()
{
    while (1);
}
#endif

static uint8_t lcd_stack_crash_(uint8_t arg, uint32_t sp = 0)
{
    // populate the stack with an increasing value for ease of testing
    volatile uint16_t tmp __attribute__((unused)) = sp;

    _delay(arg);
    uint8_t ret = lcd_stack_crash_(arg, SP);

    // required to avoid tail call elimination and to slow down the stack growth
    _delay(ret);

    return ret;
}

static void lcd_stack_crash()
{
#ifdef WATCHDOG
    wdt_disable();
#endif
    // delay choosen in order to hit the stack-check in the temperature isr reliably
    lcd_stack_crash_(10);
}
#endif


//! @brief Show Support Menu
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! | Main               |	MSG_MAIN c=18
//! | Firmware:          |	c=18
//! |  3.7.2.-2363       |	c=16
//! | prusa3d.com        |	MSG_PRUSA3D
//! | forum.prusa3d.com  |	MSG_PRUSA3D_FORUM
//! | howto.prusa3d.com  |	MSG_PRUSA3D_HOWTO
//! | --------------     |	STR_SEPARATOR
//! | 1_75mm_MK3         |	FILAMENT_SIZE
//! | howto.prusa3d.com  |	ELECTRONICS
//! | howto.prusa3d.com  |	NOZZLE_TYPE
//! | --------------     |	STR_SEPARATOR
//! | Date:              |	c=17
//! | MMM DD YYYY        |	__DATE__
//! | --------------     |	STR_SEPARATOR
//! @endcode
//! 
//! If MMU is connected
//! 
//! 	@code{.unparsed}
//! 	| MMU2 connected     |	c=18
//! 	|  FW: 1.0.6-7064523 |
//! 	@endcode
//! 
//! If MMU is not connected
//! 
//! 	@code{.unparsed}
//! 	| MMU2       N/A     |	c=18
//! 	@endcode
//! 
//! If Flash Air is connected
//! 
//! 	@code{.unparsed}
//! 	| --------------     |	STR_SEPARATOR
//! 	| FlashAir IP Addr:  |	c=18
//! 	|  192.168.1.100     |
//! 	@endcode
//! 
//! @code{.unparsed}
//! | --------------     |	STR_SEPARATOR
//! | XYZ cal. details   |	MSG_XYZ_DETAILS c=18
//! | Extruder info      |	MSG_INFO_EXTRUDER
//! | XYZ cal. details   |	MSG_INFO_SENSORS
//! @endcode
//! 
//! If TMC2130 defined
//! 
//! 	@code{.unparsed}
//! 	| Belt status        |	MSG_BELT_STATUS
//! @endcode
//! 
//! @code{.unparsed}
//! | Temperatures       |	MSG_MENU_TEMPERATURES
//! @endcode
//! 
//! If Voltage Bed and PWR Pin are defined
//! 
//! 	@code{.unparsed}
//! 	| Voltages           |	MSG_MENU_VOLTAGES
//! 	@endcode
//! 
//! 
//! If DEBUG_BUILD is defined
//! 
//! 	@code{.unparsed}
//! 	| Debug              |	c=18
//! 	@endcode
//! ----------------------
//! @endcode
static void lcd_support_menu()
{
	typedef struct
	{	// 22bytes total
		int8_t status;                 // 1byte
		bool is_flash_air;             // 1byte
		uint32_t ip;                   // 4bytes
		char ip_str[IP4_STR_SIZE];     // 16bytes
	} _menu_data_t;
    static_assert(sizeof(menu_data)>= sizeof(_menu_data_t),"_menu_data_t doesn't fit into menu_data");
	_menu_data_t* _md = (_menu_data_t*)&(menu_data[0]);
    if (_md->status == 0 || lcd_draw_update == 2)
	{
        // Menu was entered or SD card status has changed (plugged in or removed).
        // Initialize its status.
        _md->status = 1;
        _md->is_flash_air = card.ToshibaFlashAir_isEnabled();
        if (_md->is_flash_air) {
            card.ToshibaFlashAir_GetIP((uint8_t*)(&_md->ip)); // ip == 0 if it failed
        }
    } else if (_md->is_flash_air && _md->ip == 0 && ++ _md->status == 16)
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
      
  MENU_ITEM_BACK_P(_n("prusa3d.com"));////MSG_PRUSA3D c=18
  MENU_ITEM_BACK_P(_n("forum.prusa3d.com"));////MSG_PRUSA3D_FORUM c=18
  MENU_ITEM_BACK_P(_n("howto.prusa3d.com"));////MSG_PRUSA3D_HOWTO c=18
  MENU_ITEM_BACK_P(STR_SEPARATOR);
  MENU_ITEM_BACK_P(PSTR(FILAMENT_SIZE));
  MENU_ITEM_BACK_P(PSTR(ELECTRONICS));
  MENU_ITEM_BACK_P(PSTR(NOZZLE_TYPE));
  MENU_ITEM_BACK_P(STR_SEPARATOR);
  MENU_ITEM_BACK_P(_i("Date:"));////MSG_DATE c=17
  MENU_ITEM_BACK_P(PSTR(__DATE__));

#ifdef IR_SENSOR_ANALOG
  MENU_ITEM_BACK_P(STR_SEPARATOR);
  MENU_ITEM_BACK_P(PSTR("Fil. sensor v.:"));
  MENU_ITEM_BACK_P(FsensorIRVersionText());
#endif // IR_SENSOR_ANALOG

	MENU_ITEM_BACK_P(STR_SEPARATOR);
	if (mmu_enabled)
	{
		MENU_ITEM_BACK_P(_i("MMU2 connected"));  ////MSG_MMU_CONNECTED c=18
		MENU_ITEM_BACK_P(PSTR(" FW:"));  ////c=17
		if (((menu_item - 1) == menu_line) && lcd_draw_update)
		{
		    lcd_set_cursor(6, menu_row);
			if ((mmu_version > 0) && (mmu_buildnr > 0))
				lcd_printf_P(PSTR("%d.%d.%d-%d"), mmu_version/100, mmu_version%100/10, mmu_version%10, mmu_buildnr);
			else
				lcd_puts_P(_i("unknown"));  ////MSG_UNKNOWN c=13
		}
	}
	else
		MENU_ITEM_BACK_P(PSTR("MMU2       N/A"));


  // Show the FlashAir IP address, if the card is available.
  if (_md->is_flash_air) {
      MENU_ITEM_BACK_P(STR_SEPARATOR);
      MENU_ITEM_BACK_P(PSTR("FlashAir IP Addr:"));  ////MSG_FLASHAIR c=18
      MENU_ITEM_BACK_P(PSTR(" "));
      if (((menu_item - 1) == menu_line) && lcd_draw_update) {
          lcd_set_cursor(2, menu_row);
          ip4_to_str(_md->ip_str, (uint8_t*)(&_md->ip));
          lcd_printf_P(PSTR("%s"), _md->ip_str);
      }
  }
  
  // Show the printer IP address, if it is available.
  if (IP_address) {
      
      MENU_ITEM_BACK_P(STR_SEPARATOR);
      MENU_ITEM_BACK_P(PSTR("Printer IP Addr:"));  ////MSG_PRINTER_IP c=18
      MENU_ITEM_BACK_P(PSTR(" "));
      if (((menu_item - 1) == menu_line) && lcd_draw_update) {
          lcd_set_cursor(2, menu_row);
          ip4_to_str(_md->ip_str, (uint8_t*)(&IP_address));
          lcd_printf_P(PSTR("%s"), _md->ip_str);
      }
  }

  #ifndef MK1BP
  MENU_ITEM_BACK_P(STR_SEPARATOR);
  MENU_ITEM_SUBMENU_P(_i("XYZ cal. details"), lcd_menu_xyz_y_min);////MSG_XYZ_DETAILS c=18
  MENU_ITEM_SUBMENU_P(_i("Extruder info"), lcd_menu_extruder_info);////MSG_INFO_EXTRUDER c=18
  MENU_ITEM_SUBMENU_P(_i("Sensor info"), lcd_menu_show_sensors_state);////MSG_INFO_SENSORS c=18

#ifdef TMC2130
  MENU_ITEM_SUBMENU_P(_T(MSG_BELT_STATUS), lcd_menu_belt_status);////MSG_BELT_STATUS c=18
#endif //TMC2130
    
  MENU_ITEM_SUBMENU_P(_i("Temperatures"), lcd_menu_temperatures);////MSG_MENU_TEMPERATURES c=18

#if defined (VOLT_BED_PIN) || defined (VOLT_PWR_PIN)
  MENU_ITEM_SUBMENU_P(_i("Voltages"), lcd_menu_voltages);////MSG_MENU_VOLTAGES c=18
#endif //defined VOLT_BED_PIN || defined VOLT_PWR_PIN

#ifdef MENU_DUMP
    MENU_ITEM_FUNCTION_P(_i("Dump memory"), lcd_dump_memory);
#endif //MENU_DUMP
#ifdef MENU_SERIAL_DUMP
    if (emergency_serial_dump)
        MENU_ITEM_FUNCTION_P(_i("Dump to serial"), lcd_serial_dump);
#endif
#ifdef DEBUG_BUILD
#ifdef EMERGENCY_HANDLERS
#ifdef WATCHDOG
    MENU_ITEM_FUNCTION_P(PSTR("WDR crash"), lcd_wdr_crash);
#endif //WATCHDOG
    MENU_ITEM_FUNCTION_P(PSTR("Stack crash"), lcd_stack_crash);
#endif //EMERGENCY_HANDLERS
  MENU_ITEM_SUBMENU_P(PSTR("Debug"), lcd_menu_debug);////MSG_DEBUG c=18
#endif /* DEBUG_BUILD */

  #endif //MK1BP

  MENU_END();
}

void lcd_set_fan_check() {
	fans_check_enabled = !fans_check_enabled;
	eeprom_update_byte((unsigned char *)EEPROM_FAN_CHECK_ENABLED, fans_check_enabled);
#ifdef FANCHECK
	if (fans_check_enabled == false) fan_check_error = EFCE_OK; //reset error if fanCheck is disabled during error. Allows resuming print.
#endif //FANCHECK
}

#ifdef MMU_HAS_CUTTER
void lcd_cutter_enabled()
{
    if (EEPROM_MMU_CUTTER_ENABLED_enabled == eeprom_read_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED))
    {
#ifndef MMU_ALWAYS_CUT
        eeprom_update_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED, 0);
    }
#else //MMU_ALWAYS_CUT
        eeprom_update_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED, EEPROM_MMU_CUTTER_ENABLED_always);
    }
    else if (EEPROM_MMU_CUTTER_ENABLED_always == eeprom_read_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED))
    {
        eeprom_update_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED, 0);
    }
#endif //MMU_ALWAYS_CUT
    else
    {
        eeprom_update_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED, EEPROM_MMU_CUTTER_ENABLED_enabled);
    }
}
#endif //MMU_HAS_CUTTER

void lcd_set_filament_autoload() {
     fsensor_autoload_set(!fsensor_autoload_enabled);
}

#if defined(FILAMENT_SENSOR) && defined(PAT9125)
void lcd_set_filament_oq_meass()
{
     fsensor_oq_meassure_set(!fsensor_oq_meassure_enabled);
}
#endif


FilamentAction eFilamentAction=FilamentAction::None; // must be initialized as 'non-autoLoad'
bool bFilamentFirstRun;
bool bFilamentPreheatState;
bool bFilamentAction=false;
static bool bFilamentWaitingFlag=false;

static void mFilamentPrompt()
{
uint8_t nLevel;

lcd_set_cursor(0,0);
lcdui_print_temp(LCD_STR_THERMOMETER[0],(int)degHotend(0),(int)degTargetHotend(0));
lcd_puts_at_P(0,1, _i("Press the knob"));                 ////MSG_PRESS_KNOB c=20
lcd_set_cursor(0,2);
switch(eFilamentAction)
     {
     case FilamentAction::Load:
     case FilamentAction::AutoLoad:
     case FilamentAction::MmuLoad:
          lcd_puts_P(_i("to load filament"));     ////MSG_TO_LOAD_FIL c=20
          break;
     case FilamentAction::UnLoad:
     case FilamentAction::MmuUnLoad:
          lcd_puts_P(_i("to unload filament"));   ////MSG_TO_UNLOAD_FIL c=20
          break;
     case FilamentAction::MmuEject:
     case FilamentAction::MmuCut:
     case FilamentAction::None:
     case FilamentAction::Preheat:
     case FilamentAction::Lay1Cal:
          break;
     }
if(lcd_clicked())
     {
     nLevel=2;
     if(!bFilamentPreheatState)
          {
          nLevel++;
//          setTargetHotend0(0.0);                  // uncoment if return to base-state is required
          }
     menu_back(nLevel);
     switch(eFilamentAction)
          {
          case FilamentAction::AutoLoad:
               eFilamentAction=FilamentAction::None; // i.e. non-autoLoad
               // FALLTHRU
          case FilamentAction::Load:
               loading_flag=true;
               enquecommand_P(PSTR("M701"));      // load filament
               break;
          case FilamentAction::UnLoad:
               enquecommand_P(PSTR("M702"));      // unload filament
               break;
          case FilamentAction::MmuLoad:
          case FilamentAction::MmuUnLoad:
          case FilamentAction::MmuEject:
          case FilamentAction::MmuCut:
          case FilamentAction::None:
          case FilamentAction::Preheat:
          case FilamentAction::Lay1Cal:
               break;
          }
     }
}

void mFilamentItem(uint16_t nTemp, uint16_t nTempBed)
{
    static int nTargetOld;
    static int nTargetBedOld;
    uint8_t nLevel;

    nTargetOld = target_temperature[0];
    nTargetBedOld = target_temperature_bed;
    setTargetHotend0((float )nTemp);
    setTargetBed((float) nTempBed);

    {
        const FilamentAction action = eFilamentAction;
        if (action == FilamentAction::Preheat || action == FilamentAction::Lay1Cal)
        {
            lcd_return_to_status();
            if (action == FilamentAction::Lay1Cal)
            {
                lcd_commands_type = LcdCommands::Layer1Cal;
            }
            else
            {
                raise_z_above(MIN_Z_FOR_PREHEAT);
                if (eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE))
                    lcd_wizard(WizState::LoadFilHot);
            }
            return;
        }
    }

    lcd_timeoutToStatus.stop();

    if (current_temperature[0] > (target_temperature[0] * 0.95))
    {
        switch (eFilamentAction)
        {
        case FilamentAction::Load:
        case FilamentAction::AutoLoad:
        case FilamentAction::UnLoad:
            if (bFilamentWaitingFlag) menu_submenu(mFilamentPrompt);
            else
            {
                nLevel = bFilamentPreheatState ? 1 : 2;
                menu_back(nLevel);
                if ((eFilamentAction == FilamentAction::Load) || (eFilamentAction == FilamentAction::AutoLoad))
                {
                    loading_flag = true;
                    enquecommand_P(PSTR("M701")); // load filament
                    if (eFilamentAction == FilamentAction::AutoLoad) eFilamentAction = FilamentAction::None; // i.e. non-autoLoad
                }
                if (eFilamentAction == FilamentAction::UnLoad)
                enquecommand_P(PSTR("M702")); // unload filament
            }
            break;
        case FilamentAction::MmuLoad:
            nLevel = bFilamentPreheatState ? 1 : 2;
            bFilamentAction = true;
            menu_back(nLevel);
            menu_submenu(mmu_load_to_nozzle_menu);
            break;
        case FilamentAction::MmuUnLoad:
            nLevel = bFilamentPreheatState ? 1 : 2;
            bFilamentAction = true;
            menu_back(nLevel);
            extr_unload();
            break;
        case FilamentAction::MmuEject:
            nLevel = bFilamentPreheatState ? 1 : 2;
            bFilamentAction = true;
            menu_back(nLevel);
            menu_submenu(mmu_fil_eject_menu);
            break;
        case FilamentAction::MmuCut:
#ifdef MMU_HAS_CUTTER
            nLevel=bFilamentPreheatState?1:2;
            bFilamentAction=true;
            menu_back(nLevel);
            menu_submenu(mmu_cut_filament_menu);
#endif //MMU_HAS_CUTTER
            break;
        case FilamentAction::None:
        case FilamentAction::Preheat:
        case FilamentAction::Lay1Cal:
            // handled earlier
            break;
        }
        if (bFilamentWaitingFlag) Sound_MakeSound(e_SOUND_TYPE_StandardPrompt);
        bFilamentWaitingFlag = false;
    }
    else
    {
        if (!bFilamentWaitingFlag || lcd_draw_update)
        {
            // First entry from another menu OR first run after the filament preheat selection. Use
            // bFilamentWaitingFlag to distinguish: this flag is reset exactly once when entering
            // the menu and is used to raise the carriage *once*. In other cases, the LCD has been
            // modified elsewhere and needs to be redrawn in full.

            // reset bFilamentWaitingFlag immediately to avoid re-entry from raise_z_above()!
            bool once = !bFilamentWaitingFlag;
            bFilamentWaitingFlag = true;

            // also force-enable lcd_draw_update (might be 0 when called from outside a menu)
            lcd_draw_update = 1;

            lcd_clear();
            lcd_puts_at_P(0, 3, _T(MSG_CANCEL)); ////MSG_CANCEL

            lcd_set_cursor(0, 1);
            switch (eFilamentAction)
            {
            case FilamentAction::Load:
            case FilamentAction::AutoLoad:
            case FilamentAction::MmuLoad:
                lcd_puts_P(_i("Preheating to load")); ////MSG_PREHEATING_TO_LOAD c=20
                if (once) raise_z_above(MIN_Z_FOR_LOAD);
                break;
            case FilamentAction::UnLoad:
            case FilamentAction::MmuUnLoad:
                lcd_puts_P(_i("Preheating to unload")); ////MSG_PREHEATING_TO_UNLOAD c=20
                if (once) raise_z_above(MIN_Z_FOR_UNLOAD);
                break;
            case FilamentAction::MmuEject:
                lcd_puts_P(_i("Preheating to eject")); ////MSG_PREHEATING_TO_EJECT c=20
                break;
            case FilamentAction::MmuCut:
                lcd_puts_P(_i("Preheating to cut")); ////MSG_PREHEATING_TO_CUT c=20
                break;
            case FilamentAction::None:
            case FilamentAction::Preheat:
            case FilamentAction::Lay1Cal:
                // handled earlier
                break;
            }
        }

        lcd_set_cursor(0, 0);
        lcdui_print_temp(LCD_STR_THERMOMETER[0], (int) degHotend(0), (int) degTargetHotend(0));

        if (lcd_clicked())
        {
            bFilamentWaitingFlag = false;
            if (!bFilamentPreheatState)
            {
                setTargetHotend0(0.0);
                setTargetBed(0.0);
                menu_back();
            }
            else
            {
                setTargetHotend0((float )nTargetOld);
                setTargetBed((float) nTargetBedOld);
            }
            menu_back();
            if (eFilamentAction == FilamentAction::AutoLoad) eFilamentAction = FilamentAction::None; // i.e. non-autoLoad
        }
    }
}

static void mFilamentItem_farm()
{
    bFilamentPreheatState = false;
    mFilamentItem(FARM_PREHEAT_HOTEND_TEMP, FARM_PREHEAT_HPB_TEMP);
}
static void mFilamentItem_farm_nozzle()
{
    bFilamentPreheatState = false;
    mFilamentItem(FARM_PREHEAT_HOTEND_TEMP, 0);
}

static void mFilamentItem_PLA()
{
    bFilamentPreheatState = false;
    mFilamentItem(PLA_PREHEAT_HOTEND_TEMP, PLA_PREHEAT_HPB_TEMP);
}

static void mFilamentItem_PET()
{
    bFilamentPreheatState = false;
    mFilamentItem(PET_PREHEAT_HOTEND_TEMP, PET_PREHEAT_HPB_TEMP);
}

static void mFilamentItem_ASA()
{
    bFilamentPreheatState = false;
    mFilamentItem(ASA_PREHEAT_HOTEND_TEMP, ASA_PREHEAT_HPB_TEMP);
}

static void mFilamentItem_PC()
{
    bFilamentPreheatState = false;
    mFilamentItem(PC_PREHEAT_HOTEND_TEMP, PC_PREHEAT_HPB_TEMP);
}

static void mFilamentItem_ABS()
{
    bFilamentPreheatState = false;
    mFilamentItem(ABS_PREHEAT_HOTEND_TEMP, ABS_PREHEAT_HPB_TEMP);
}

static void mFilamentItem_HIPS()
{
    bFilamentPreheatState = false;
    mFilamentItem(HIPS_PREHEAT_HOTEND_TEMP, HIPS_PREHEAT_HPB_TEMP);
}

static void mFilamentItem_PP()
{
    bFilamentPreheatState = false;
    mFilamentItem(PP_PREHEAT_HOTEND_TEMP, PP_PREHEAT_HPB_TEMP);
}

static void mFilamentItem_FLEX()
{
    bFilamentPreheatState = false;
    mFilamentItem(FLEX_PREHEAT_HOTEND_TEMP, FLEX_PREHEAT_HPB_TEMP);
}

static void mFilamentItem_PVB()
{
    bFilamentPreheatState = false;
    mFilamentItem(PVB_PREHEAT_HOTEND_TEMP, PVB_PREHEAT_HPB_TEMP);
}

void mFilamentBack()
{
    menu_back();
    if (eFilamentAction == FilamentAction::AutoLoad ||
            eFilamentAction == FilamentAction::Preheat ||
            eFilamentAction == FilamentAction::Lay1Cal)
    {
        eFilamentAction = FilamentAction::None; // i.e. non-autoLoad
    }
}

void lcd_generic_preheat_menu()
{
    MENU_BEGIN();
    if (!eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE))
    {
        if (eFilamentAction == FilamentAction::Lay1Cal)
        {
            MENU_ITEM_FUNCTION_P(_T(MSG_BACK), mFilamentBack);
        }
        else
        {
            MENU_ITEM_FUNCTION_P(_T(MSG_MAIN), mFilamentBack);
        }
    }
    if (farm_mode)
    {
        MENU_ITEM_FUNCTION_P(PSTR("farm   -  " STRINGIFY(FARM_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(FARM_PREHEAT_HPB_TEMP)), mFilamentItem_farm);
        MENU_ITEM_FUNCTION_P(PSTR("nozzle -  " STRINGIFY(FARM_PREHEAT_HOTEND_TEMP) "/0"), mFilamentItem_farm_nozzle);
    }
    else
    {
        MENU_ITEM_SUBMENU_P(PSTR("PLA  -  " STRINGIFY(PLA_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(PLA_PREHEAT_HPB_TEMP)),mFilamentItem_PLA);
        MENU_ITEM_SUBMENU_P(PSTR("PET  -  " STRINGIFY(PET_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(PET_PREHEAT_HPB_TEMP)),mFilamentItem_PET);
        MENU_ITEM_SUBMENU_P(PSTR("ASA  -  " STRINGIFY(ASA_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(ASA_PREHEAT_HPB_TEMP)),mFilamentItem_ASA);
        MENU_ITEM_SUBMENU_P(PSTR("PC   -  " STRINGIFY(PC_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(PC_PREHEAT_HPB_TEMP)),mFilamentItem_PC);
        MENU_ITEM_SUBMENU_P(PSTR("PVB  -  " STRINGIFY(PVB_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(PVB_PREHEAT_HPB_TEMP)),mFilamentItem_PVB);
        MENU_ITEM_SUBMENU_P(PSTR("ABS  -  " STRINGIFY(ABS_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(ABS_PREHEAT_HPB_TEMP)),mFilamentItem_ABS);
        MENU_ITEM_SUBMENU_P(PSTR("HIPS -  " STRINGIFY(HIPS_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(HIPS_PREHEAT_HPB_TEMP)),mFilamentItem_HIPS);
        MENU_ITEM_SUBMENU_P(PSTR("PP   -  " STRINGIFY(PP_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(PP_PREHEAT_HPB_TEMP)),mFilamentItem_PP);
        MENU_ITEM_SUBMENU_P(PSTR("FLEX -  " STRINGIFY(FLEX_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(FLEX_PREHEAT_HPB_TEMP)),mFilamentItem_FLEX);
    }
    if (!eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE) && eFilamentAction == FilamentAction::Preheat) MENU_ITEM_FUNCTION_P(_T(MSG_COOLDOWN), lcd_cooldown);
    MENU_END();
}

void mFilamentItemForce()
{
mFilamentItem(target_temperature[0],target_temperature_bed);
}


void lcd_unLoadFilament()
{
     eFilamentAction=FilamentAction::UnLoad;
     preheat_or_continue();
}

static void mmu_unload_filament()
{
    eFilamentAction = FilamentAction::MmuUnLoad;
    preheat_or_continue();
}


void lcd_wait_interact() {

  lcd_clear();

  lcd_set_cursor(0, 1);
#ifdef SNMM 
  lcd_puts_P(_i("Prepare new filament"));////MSG_PREPARE_FILAMENT c=20
#else
  lcd_puts_P(_i("Insert filament"));////MSG_INSERT_FILAMENT c=20
#endif
  if (!fsensor_autoload_enabled) {
	  lcd_puts_at_P(0, 2, _i("and press the knob"));////MSG_PRESS c=20 r=2
  }
}


void lcd_change_success() {

  lcd_clear();

  lcd_puts_at_P(0, 2, _i("Change success!"));////MSG_CHANGE_SUCCESS c=20


}

static void lcd_loading_progress_bar(uint16_t loading_time_ms) { 

	for (uint_least8_t i = 0; i < LCD_WIDTH; i++) {
		lcd_putc_at(i, 3, '.');
		//loading_time_ms/20 delay
		for (uint_least8_t j = 0; j < 5; j++) {
			delay_keep_alive(loading_time_ms / 100);
		}
	}
}


void lcd_loading_color() {
	//we are extruding 25mm with feedrate 200mm/min -> 7.5 seconds for whole action, 0.375 s for one character

  lcd_clear();

  lcd_puts_at_P(0, 0, _i("Loading color"));////MSG_LOADING_COLOR c=20
  lcd_puts_at_P(0, 2, _T(MSG_PLEASE_WAIT));
  lcd_loading_progress_bar((FILAMENTCHANGE_FINALFEED * 1000ul) / FILAMENTCHANGE_EFEED_FINAL); //show progress bar during filament loading slow sequence
}


void lcd_loading_filament() {


  lcd_clear();

  lcd_puts_at_P(0, 0, _T(MSG_LOADING_FILAMENT));
  lcd_puts_at_P(0, 2, _T(MSG_PLEASE_WAIT));
#ifdef SNMM
  for (int i = 0; i < 20; i++) {

    lcd_set_cursor(i, 3);
    lcd_print('.');
    for (int j = 0; j < 10 ; j++) {
      manage_heater();
      manage_inactivity(true);

      _delay(153);
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

  lcd_puts_at_P(0, 0, _i("Changed correctly?"));////MSG_CORRECTLY c=20
  lcd_puts_at_P(1, 1, _T(MSG_YES));
  lcd_puts_at_P(1, 2, _i("Filament not loaded"));////MSG_NOT_LOADED c=19
  lcd_puts_at_P(1, 3, _i("Color not correct"));////MSG_NOT_COLOR c=19
  lcd_putc_at(0, 1, '>');


  enc_dif = lcd_encoder_diff;
  lcd_consume_click();
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
					Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
        }

        if (cursor_pos < 1) {
          cursor_pos = 1;
					Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
        }
        lcd_puts_at_P(0, 1, PSTR(" \n \n "));
        lcd_putc_at(0, cursor_pos, '>');
        enc_dif = lcd_encoder_diff;
				Sound_MakeSound(e_SOUND_TYPE_EncoderMove);
        _delay(100);
      }

    }


    if (lcd_clicked()) {
			Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
      lcd_change_fil_state = cursor_pos;
      _delay(500);

    }



  };


  lcd_clear();
  lcd_return_to_status();

}

void show_preheat_nozzle_warning()
{	
    lcd_clear();
    lcd_puts_at_P(0, 0, _T(MSG_ERROR));
    lcd_puts_at_P(0, 2, _T(MSG_PREHEAT_NOZZLE));
    _delay(2000);
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
     uint8_t nlines;
     lcd_display_message_fullscreen_nonBlocking_P(_i("Autoloading filament is active, just insert filament, press the knob to exit..."),nlines);////MSG_AUTOLOADING_ENABLED c=20 r=4
     menu_back_if_clicked();
}
#endif //FILAMENT_SENSOR

static void preheat_or_continue()
{
    bFilamentFirstRun = false;
    if (target_temperature[0] >= EXTRUDE_MINTEMP)
    {
        bFilamentPreheatState = true;
        mFilamentItem(target_temperature[0], target_temperature_bed);
    }
    else lcd_generic_preheat_menu();
}

static void lcd_LoadFilament()
{
    eFilamentAction = FilamentAction::Load;
    preheat_or_continue();
}


//! @brief Show filament used a print time
//!
//! If printing current print statistics are shown
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |Filament used:      | MSG_FILAMENT_USED c=19
//! |           0000.00m |
//! |Print time:         | MSG_PRINT_TIME c=19
//! |        00h 00m 00s |
//! ----------------------
//! @endcode
//!
//! If not printing, total statistics are shown
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |Total filament:     | MSG_TOTAL_FILAMENT c=19
//! |           0000.00m |
//! |Total print time:   | MSG_TOTAL_PRINT_TIME c=19
//! |        00d 00h 00m |
//! ----------------------
//! @endcode
//! @todo Positioning of the messages and values on LCD aren't fixed to their exact place. This causes issues with translations. Translations missing for "d"days, "h"ours, "m"inutes", "s"seconds".
void lcd_menu_statistics()
{
    lcd_timeoutToStatus.stop(); //infinite timeout
	if (IS_SD_PRINTING)
	{
		const float _met = ((float)total_filament_used) / (100000.f);
		const uint32_t _t = (_millis() - starttime) / 1000ul;
		const uint32_t _h = _t / 3600;
		const uint8_t _m = (_t - (_h * 3600ul)) / 60ul;
		const uint8_t _s = _t - ((_h * 3600ul) + (_m * 60ul));

        lcd_home();
		lcd_printf_P(_N(
			"%S:\n"
			"%18.2fm \n"
			"%S:\n"
			"%10ldh %02hhdm %02hhds"
		    ),
            _i("Filament used"), _met,  ////MSG_FILAMENT_USED c=19
            _i("Print time"), _h, _m, _s);  ////MSG_PRINT_TIME c=19
		menu_back_if_clicked_fb();
	}
	else
	{
		unsigned long _filament = eeprom_read_dword((uint32_t *)EEPROM_FILAMENTUSED);
		unsigned long _time = eeprom_read_dword((uint32_t *)EEPROM_TOTALTIME); //in minutes
		uint8_t _hours, _minutes;
		uint32_t _days;
		float _filament_m = (float)_filament/100;
		_days = _time / 1440;
		_hours = (_time - (_days * 1440)) / 60;
		_minutes = _time - ((_days * 1440) + (_hours * 60));

		lcd_home();
		lcd_printf_P(_N(
			"%S:\n"
			"%18.2fm \n"
			"%S:\n"
			"%10ldd %02hhdh %02hhdm"
            ),
            _i("Total filament"), _filament_m,  ////MSG_TOTAL_FILAMENT c=19
            _i("Total print time"), _days, _hours, _minutes);  ////MSG_TOTAL_PRINT_TIME c=19
        menu_back_if_clicked_fb();
	}
}


static void _lcd_move(const char *name, int axis, int min, int max)
{
    if (homing_flag || mesh_bed_leveling_flag)
    {
        // printer entered a new state where axis move is forbidden
        menu_back();
        return;
    }

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
			plan_buffer_line_curposXYZE(manual_feedrate[axis] / 60);
			lcd_draw_update = 1;
		}
	}
	if (lcd_draw_update)
	{
	    lcd_set_cursor(0, 1);
		menu_draw_float31(name, current_position[axis]);
	}
	if (menu_leaving || LCD_CLICKED) (void)enable_endstops(_md->endstopsEnabledPrevious);
	if (LCD_CLICKED) menu_back();
}


void lcd_move_e()
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
				plan_buffer_line_curposXYZE(manual_feedrate[E_AXIS] / 60);
				lcd_draw_update = 1;
			}
		}
		if (lcd_draw_update)
		{
		    lcd_set_cursor(0, 1);
			// Note: the colon behind the text is necessary to greatly shorten
			// the implementation of menu_draw_float31
			menu_draw_float31(PSTR("Extruder:"), current_position[E_AXIS]);
		}
		if (LCD_CLICKED) menu_back();
	}
	else
	{
		show_preheat_nozzle_warning();
		lcd_return_to_status();
	}
}


//! @brief Show measured Y distance of front calibration points from Y_MIN_POS
//! If those points are detected too close to edge of reachable area, their confidence is lowered.
//! This functionality is applied more often for MK2 printers.
//! @code{.unparsed}
//! |01234567890123456789|
//! |Y distance from min |	MSG_Y_DIST_FROM_MIN
//! | --------------     |	STR_SEPARATOR
//! |Left:        00.00mm|	MSG_LEFT c=10, c=8
//! |Right:       00.00mm|	MSG_RIGHT c=10, c=8
//! ----------------------
//! @endcode
//! @todo Positioning of the messages and values on LCD aren't fixed to their exact place. This causes issues with translations.
static void lcd_menu_xyz_y_min()
{
	float distanceMin[2];
    count_xyz_details(distanceMin);
	lcd_home();
	lcd_printf_P(_N(
	  "%S\n"
	  "%S\n"
	  "%S:\n"
	  "%S:"
	 ),
	 _i("Y distance from min"),  ////MSG_Y_DIST_FROM_MIN c=20
	 separator,
	 _i("Left"),  ////MSG_LEFT c=10
	 _i("Right")  ////MSG_RIGHT c=10
	);
	for (uint8_t i = 0; i < 2; i++)
	{
		lcd_set_cursor(11,2+i);
		if (distanceMin[i] >= 200) lcd_puts_P(_T(MSG_NA));
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

//! @brief Show Measured XYZ Skew
//! 
//! @code{.unparsed}
//! |01234567890123456789|
//! |Measured skew :0.00D|	MSG_MEASURED_SKEW c=14, c=4
//! | --------------     |	STR_SEPARATOR
//! |Slight skew   :0.12D|	MSG_SLIGHT_SKEW c=14, c=4
//! |Severe skew   :0.25D|	MSG_SEVERE_SKEW c=14, c=4
//! ----------------------
//! D - Degree sysmbol		LCD_STR_DEGREE
//! @endcode
//! @todo Positioning of the messages and values on LCD aren't fixed to their exact place. This causes issues with translations.
static void lcd_menu_xyz_skew()
{
    float angleDiff = eeprom_read_float((float*)(EEPROM_XYZ_CAL_SKEW));
	lcd_home();
	lcd_printf_P(_N(
	  "%-14.14S:\n"
	  "%S\n"
	  "%-14.14S:%3.2f\x01\n"
	  "%-14.14S:%3.2f\x01"
	 ),
	 _i("Measured skew"),  ////MSG_MEASURED_SKEW c=14
	 separator,
	 _i("Slight skew"), _deg(bed_skew_angle_mild),  ////MSG_SLIGHT_SKEW c=14, c=4
	 _i("Severe skew"), _deg(bed_skew_angle_extreme)  ////MSG_SEVERE_SKEW c=14, c=4
	);
	if (angleDiff < 100){
		lcd_set_cursor(15,0);
		lcd_printf_P(_N("%3.2f\x01"), _deg(angleDiff));
	}
	else{
		lcd_puts_at_P(15,0, _T(MSG_NA));
	}
    if (lcd_clicked())
        menu_goto(lcd_menu_xyz_offset, 0, true, true);
}
//! @brief Show measured bed offset from expected position
//! 
//! @code{.unparsed}
//! |01234567890123456789|
//! |[0;0] point offset  |	MSG_MEASURED_OFFSET c=20
//! | --------------     |	STR_SEPARATOR
//! |X            00.00mm|	c=10
//! |Y            00.00mm|	c=10
//! ----------------------
//! @endcode
//! @todo Positioning of the messages and values on LCD aren't fixed to their exact place. This causes issues with translations.
static void lcd_menu_xyz_offset()
{
    lcd_puts_at_P(0, 0, _i("[0;0] point offset"));////MSG_MEASURED_OFFSET c=20
    lcd_puts_at_P(0, 1, separator);
    lcd_puts_at_P(0, 2, PSTR("X"));
    lcd_puts_at_P(0, 3, PSTR("Y"));

    float vec_x[2];
    float vec_y[2];
    float cntr[2];
    world2machine_read_valid(vec_x, vec_y, cntr);

    for (uint_least8_t i = 0; i < 2; i++)
    {
        lcd_set_cursor((cntr[i] < 0) ? 13 : 14, i+2);
        lcd_print(cntr[i]);
        lcd_puts_at_P(18, i + 2, PSTR("mm"));
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


// Note: the colon behind the text (X, Y, Z) is necessary to greatly shorten
// the implementation of menu_draw_float31
static void lcd_move_x() {
  _lcd_move(PSTR("X:"), X_AXIS, X_MIN_POS, X_MAX_POS);
}
static void lcd_move_y() {
  _lcd_move(PSTR("Y:"), Y_AXIS, Y_MIN_POS, Y_MAX_POS);
}
static void lcd_move_z() {
  _lcd_move(PSTR("Z:"), Z_AXIS, Z_MIN_POS, Z_MAX_POS);
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
static void lcd_babystep_z()
{
    if (homing_flag || mesh_bed_leveling_flag)
    {
        // printer changed to a new state where live Z is forbidden
        menu_back();
        return;
    }

	typedef struct
	{
		int8_t status;
		int16_t babystepMemZ;
		float babystepMemMMZ;
	} _menu_data_t;
	static_assert(sizeof(menu_data)>= sizeof(_menu_data_t),"_menu_data_t doesn't fit into menu_data");
	_menu_data_t* _md = (_menu_data_t*)&(menu_data[0]);
	if (_md->status == 0)
	{
		// Menu was entered.
		// Initialize its status.
		_md->status = 1;
		check_babystep();
		
		if(!eeprom_is_sheet_initialized(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)))){
			_md->babystepMemZ = 0;
		}
		else{
			_md->babystepMemZ = eeprom_read_word(reinterpret_cast<uint16_t *>(&(EEPROM_Sheets_base->
					s[(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)))].z_offset)));
		}

		// same logic as in babystep_load
	    if (calibration_status() >= CALIBRATION_STATUS_LIVE_ADJUST)
			_md->babystepMemZ = 0;

		_md->babystepMemMMZ = _md->babystepMemZ/cs.axis_steps_per_unit[Z_AXIS];
		lcd_draw_update = 1;
		//SERIAL_ECHO("Z baby step: ");
		//SERIAL_ECHO(_md->babystepMem[2]);
		// Wait 90 seconds before closing the live adjust dialog.
		lcd_timeoutToStatus.start();
	}

	if (lcd_encoder != 0)
	{
		_md->babystepMemZ += (int)lcd_encoder;

        if (_md->babystepMemZ < Z_BABYSTEP_MIN) _md->babystepMemZ = Z_BABYSTEP_MIN; //-3999 -> -9.99 mm
        else if (_md->babystepMemZ > Z_BABYSTEP_MAX) _md->babystepMemZ = Z_BABYSTEP_MAX; //0
        else babystepsTodoZadd(lcd_encoder);

		_md->babystepMemMMZ = _md->babystepMemZ/cs.axis_steps_per_unit[Z_AXIS];
		_delay(50);
		lcd_encoder = 0;
		lcd_draw_update = 1;
	}
	if (lcd_draw_update)
	{
	    SheetFormatBuffer buffer;
	    menu_format_sheet_E(EEPROM_Sheets_base->s[(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)))], buffer);
	    lcd_set_cursor(0, 0);
	    lcd_print(buffer.c);
	    lcd_set_cursor(0, 1);
		menu_draw_float13(_i("Adjusting Z:"), _md->babystepMemMMZ); ////MSG_BABYSTEPPING_Z c=15 Beware: must include the ':' as its last character
	}
	if (LCD_CLICKED || menu_leaving)
	{
		// Only update the EEPROM when leaving the menu.
          uint8_t active_sheet=eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet));
		eeprom_update_word(reinterpret_cast<uint16_t *>(&(EEPROM_Sheets_base->s[active_sheet].z_offset)),_md->babystepMemZ);
		eeprom_update_byte(&(EEPROM_Sheets_base->s[active_sheet].bed_temp),target_temperature_bed);
#ifdef PINDA_THERMISTOR        
		eeprom_update_byte(&(EEPROM_Sheets_base->s[active_sheet].pinda_temp),current_temperature_pinda);
#endif //PINDA_THERMISTOR
		calibration_status_store(CALIBRATION_STATUS_CALIBRATED);
	}
	if (LCD_CLICKED) menu_back();
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

//! @brief Show Bed level correct
//! 
//! @code{.unparsed}
//! |01234567890123456789|
//! |Settings:           |	MSG_SETTINGS
//! |Left side [um]:     |	MSG_BED_CORRECTION_LEFT
//! |Right side[um]:     |	MSG_BED_CORRECTION_RIGHT
//! |Front side[um]:     |	MSG_BED_CORRECTION_FRONT
//! |Rear side [um]:     |	MSG_BED_CORRECTION_REAR
//! |Reset               |	MSG_BED_CORRECTION_RESET
//! ----------------------
//! @endcode
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
    ON_MENU_LEAVE(
        eeprom_update_int8((unsigned char*)EEPROM_BED_CORRECTION_LEFT,  _md->left);
        eeprom_update_int8((unsigned char*)EEPROM_BED_CORRECTION_RIGHT, _md->right);
        eeprom_update_int8((unsigned char*)EEPROM_BED_CORRECTION_FRONT, _md->front);
        eeprom_update_int8((unsigned char*)EEPROM_BED_CORRECTION_REAR,  _md->rear);
        eeprom_update_byte((unsigned char*)EEPROM_BED_CORRECTION_VALID, 1);
    );
    MENU_ITEM_BACK_P(_T(MSG_SETTINGS));
	MENU_ITEM_EDIT_int3_P(_i("Left side [um]"),  &_md->left,  -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);////MSG_BED_CORRECTION_LEFT c=14
    MENU_ITEM_EDIT_int3_P(_i("Right side[um]"), &_md->right, -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);////MSG_BED_CORRECTION_RIGHT c=14
    MENU_ITEM_EDIT_int3_P(_i("Front side[um]"), &_md->front, -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);////MSG_BED_CORRECTION_FRONT c=14
    MENU_ITEM_EDIT_int3_P(_i("Rear side [um]"),  &_md->rear,  -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);////MSG_BED_CORRECTION_REAR c=14
    MENU_ITEM_FUNCTION_P(_T(MSG_RESET), lcd_adjust_bed_reset);////MSG_RESET c=14
    MENU_END();
}

//! @brief Show PID Extruder
//! 
//! @code{.unparsed}
//! |01234567890123456789|
//! |Set temperature:    |	MSG_SET_TEMPERATURE c=20
//! |                    |
//! | 210                |
//! |                    |
//! ----------------------
//! @endcode
void pid_extruder()
{
	lcd_clear();
	lcd_puts_at_P(0, 0, _i("Set temperature:"));////MSG_SET_TEMPERATURE
	pid_temp += int(lcd_encoder);
	if (pid_temp > HEATER_0_MAXTEMP) pid_temp = HEATER_0_MAXTEMP;
	if (pid_temp < HEATER_0_MINTEMP) pid_temp = HEATER_0_MINTEMP;
	lcd_encoder = 0;
	lcd_set_cursor(1, 2);
	lcd_print(ftostr3(pid_temp));
	if (lcd_clicked()) {
		lcd_commands_type = LcdCommands::PidExtruder;
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
  lcd_puts_P(_i("Auto adjust Z?"));////MSG_ADJUSTZ
  lcd_set_cursor(1, 1);
  lcd_puts_P(_T(MSG_YES));

  lcd_set_cursor(1, 2);

  lcd_puts_P(_T(MSG_NO));

  lcd_set_cursor(0, 1);

  lcd_print('>');


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
        lcd_print(' ');
        lcd_set_cursor(0, 2);
        lcd_print(' ');
        lcd_set_cursor(0, cursor_pos);
        lcd_print('>');
        enc_dif = lcd_encoder_diff;
        _delay(100);
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
      _delay(500);
    }
  };

  lcd_clear();
  lcd_return_to_status();

}*/

#ifdef PINDA_THERMISTOR
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
		lcd_print('/');
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
#endif //PINDA_THERMISTOR

void lcd_wait_for_heater() {
		lcd_display_message_fullscreen_P(_T(MSG_WIZARD_HEATING));
		lcd_set_degree();
		lcd_set_cursor(0, 4);
		lcd_print(LCD_STR_THERMOMETER[0]);
		lcd_print(ftostr3(degHotend(active_extruder)));
		lcd_print('/');
		lcd_print(ftostr3(degTargetHotend(active_extruder)));
		lcd_print(LCD_STR_DEGREE);
}

void lcd_wait_for_cool_down() {
	lcd_set_custom_characters_degree();
	setAllTargetHotends(0);
	setTargetBed(0);
	int fanSpeedBckp = fanSpeed;
	fanSpeed = 255;
	while ((degHotend(0)>MAX_HOTEND_TEMP_CALIBRATION) || (degBed() > MAX_BED_TEMP_CALIBRATION)) {
		lcd_display_message_fullscreen_P(_i("Waiting for nozzle and bed cooling"));////MSG_WAITING_TEMP c=20 r=4

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
	fanSpeed = fanSpeedBckp;
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
    plan_set_position_curposXYZE();

    // Until confirmed by the confirmation dialog.
    for (;;) {
        const char   *msg                 = only_z ? _i("Calibrating Z. Rotate the knob to move the Z carriage up to the end stoppers. Click when done.") : _i("Calibrating XYZ. Rotate the knob to move the Z carriage up to the end stoppers. Click when done.");////MSG_MOVE_CARRIAGE_TO_THE_TOP c=20 r=8////MSG_MOVE_CARRIAGE_TO_THE_TOP_Z c=20 r=8
        const char   *msg_next            = lcd_display_message_fullscreen_P(msg);
        const bool    multi_screen        = msg_next != NULL;
        unsigned long previous_millis_msg = _millis();
        // Until the user finishes the z up movement.
        lcd_encoder_diff = 0;
        lcd_encoder = 0;
        for (;;) {
            manage_heater();
            manage_inactivity(true);
            if (abs(lcd_encoder_diff) >= ENCODER_PULSES_PER_STEP) {
                _delay(50);
                lcd_encoder += abs(lcd_encoder_diff / ENCODER_PULSES_PER_STEP);
                lcd_encoder_diff = 0;
                if (! planner_queue_full()) {
                    // Only move up, whatever direction the user rotates the encoder.
                    current_position[Z_AXIS] += fabs(lcd_encoder);
                    lcd_encoder = 0;
                    plan_buffer_line_curposXYZE(manual_feedrate[Z_AXIS] / 60);
                }
            }
            if (lcd_clicked()) {
                // Abort a move if in progress.
                planner_abort_hard();
                while (lcd_clicked()) ;
                _delay(10);
                while (lcd_clicked()) ;
                break;
            }
            if (multi_screen && _millis() - previous_millis_msg > 5000) {
                if (msg_next == NULL)
                    msg_next = msg;
                msg_next = lcd_display_message_fullscreen_P(msg_next);
                previous_millis_msg = _millis();
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
	if ((PRINTER_TYPE == PRINTER_MK25) || (PRINTER_TYPE == PRINTER_MK2) || (PRINTER_TYPE == PRINTER_MK2_SNMM)) {
		current_position[Z_AXIS] = Z_MAX_POS-3.f;
	}
	else {
		current_position[Z_AXIS] = Z_MAX_POS+4.f;
	}
    plan_set_position_curposXYZE();
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
        uint8_t linelen = min(strlen_P(msg), LCD_WIDTH);
        const char *msgend2 = msg + linelen;
        msgend = msgend2;
        if (row == 3 && linelen == LCD_WIDTH) {
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
	lcd_consume_click();
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
long nTime0 = _millis()/1000;
	lcd_consume_click();
	KEEPALIVE_STATE(PAUSED_FOR_USER);
    for (;;) {
        manage_heater();
        manage_inactivity(true);
        bDelayed = ((_millis()/1000-nTime0) > nDelay);
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
	unsigned long previous_millis_cmd = _millis();
	int8_t        enc_dif = lcd_encoder_diff;
	lcd_consume_click();
	//KEEPALIVE_STATE(PAUSED_FOR_USER);
	for (;;) {
		for (uint8_t i = 0; i < 100; ++i) {
			delay_keep_alive(50);
			if (allow_timeouting && _millis() - previous_millis_cmd > LCD_TIMEOUT_TO_STATUS)
				return -1;
			manage_heater();
			manage_inactivity(true);

			if (abs(enc_dif - lcd_encoder_diff) > 4) {
				if (msg_next == NULL) {
					lcd_set_cursor(0, 3);
					if (enc_dif < lcd_encoder_diff && yes) {
						lcd_print(' ');
						lcd_putc_at(7, 3, '>');
						yes = false;
						Sound_MakeSound(e_SOUND_TYPE_EncoderMove);
					}
					else if (enc_dif > lcd_encoder_diff && !yes) {
						lcd_print('>');
						lcd_putc_at(7, 3, ' ');
						yes = true;
						Sound_MakeSound(e_SOUND_TYPE_EncoderMove);
					}
					enc_dif = lcd_encoder_diff;
				}
				else {
					Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
					break; //turning knob skips waiting loop
				}
			}
			if (lcd_clicked()) {
				Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
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
			if (yes) lcd_print('>');
			lcd_puts_at_P(1, 3, first_choice);
			lcd_set_cursor(7, 3);
			if (!yes) lcd_print('>');
			lcd_puts_at_P(8, 3, second_choice);
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
		lcd_putc_at(0, 2, '>');
		lcd_puts_P(_T(MSG_YES));
		lcd_puts_at_P(1, 3, _T(MSG_NO));
	}
	else {
		lcd_puts_at_P(1, 2, _T(MSG_YES));
		lcd_putc_at(0, 3, '>');
		lcd_puts_P(_T(MSG_NO));
	}
	int8_t retval = default_yes ? true : false;

	// Wait for user confirmation or a timeout.
	unsigned long previous_millis_cmd = _millis();
	int8_t        enc_dif = lcd_encoder_diff;
	lcd_consume_click();
	KEEPALIVE_STATE(PAUSED_FOR_USER);
	for (;;) {
		if (allow_timeouting && _millis() - previous_millis_cmd > LCD_TIMEOUT_TO_STATUS)
		{
		    retval = -1;
		    break;
		}
		manage_heater();
		manage_inactivity(true);
		if (abs(enc_dif - lcd_encoder_diff) > 4) {
			lcd_set_cursor(0, 2);
				if (enc_dif < lcd_encoder_diff && retval) {
					lcd_print(' ');
					lcd_putc_at(0, 3, '>');
					retval = 0;
					Sound_MakeSound(e_SOUND_TYPE_EncoderMove);

				}
				else if (enc_dif > lcd_encoder_diff && !retval) {
					lcd_print('>');
					lcd_putc_at(0, 3, ' ');
					retval = 1;
					Sound_MakeSound(e_SOUND_TYPE_EncoderMove);
				}
				enc_dif = lcd_encoder_diff;
		}
		if (lcd_clicked()) {
			Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
			KEEPALIVE_STATE(IN_HANDLER);
			break;
		}
	}
    lcd_encoder_diff = 0;
    return retval;
}

void lcd_bed_calibration_show_result(BedSkewOffsetDetectionResultType result, uint8_t point_too_far_mask)
{
    const char *msg = NULL;
    if (result == BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND) {
        lcd_show_fullscreen_message_and_wait_P(_i("XYZ calibration failed. Bed calibration point was not found."));////MSG_BED_SKEW_OFFSET_DETECTION_POINT_NOT_FOUND c=20 r=6
    } else if (result == BED_SKEW_OFFSET_DETECTION_FITTING_FAILED) {
        if (point_too_far_mask == 0)
            msg = _T(MSG_BED_SKEW_OFFSET_DETECTION_FITTING_FAILED);
        else if (point_too_far_mask == 2 || point_too_far_mask == 7)
            // Only the center point or all the three front points.
            msg = _i("XYZ calibration failed. Front calibration points not reachable.");////MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_BOTH_FAR c=20 r=6
        else if ((point_too_far_mask & 1) == 0)
            // The right and maybe the center point out of reach.
            msg = _i("XYZ calibration failed. Right front calibration point not reachable.");////MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_RIGHT_FAR c=20 r=6
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
	
	custom_message_type = CustomMsg::Status;
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
		eeprom_update_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE, 1);
	}
	else {
		eeprom_update_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, 0);
		SERIAL_ECHOLNPGM("Temperature calibration failed. Continue with pressing the knob.");
		lcd_show_fullscreen_message_and_wait_P(_i("Temperature calibration failed"));////MSG_TEMP_CAL_FAILED c=20 r=8
		eeprom_update_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE, 0);
	}
	lcd_update_enable(true);
	lcd_update(2);
}

static void lcd_show_end_stops() {
	lcd_puts_at_P(0, 0, (PSTR("End stops diag")));
	lcd_puts_at_P(0, 1, (READ(X_MIN_PIN) ^ (bool)X_MIN_ENDSTOP_INVERTING) ? (PSTR("X1")) : (PSTR("X0")));
	lcd_puts_at_P(0, 2, (READ(Y_MIN_PIN) ^ (bool)Y_MIN_ENDSTOP_INVERTING) ? (PSTR("Y1")) : (PSTR("Y0")));
	lcd_puts_at_P(0, 3, (READ(Z_MIN_PIN) ^ (bool)Z_MIN_ENDSTOP_INVERTING) ? (PSTR("Z1")) : (PSTR("Z0")));
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
	lcd_consume_click();
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

static void lcd_print_state(uint8_t state)
{
	switch (state) {
		case STATE_ON:
			lcd_puts_P(_N("  1"));
		break;
		case STATE_OFF:
			lcd_puts_P(_N("  0"));
		break;
		default: 
			lcd_puts_P(_T(MSG_NA));
		break;
	}
}

//! @brief Show sensor state
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |PINDA N/A  FINDA N/A|  MSG_PINDA c=5 MSG_FINDA c=5
//! |Fil. sensor      N/A|  MSG_FSENSOR 
//! |Xd    000  Yd    000|  MSG_XD
//! |Int   000  Shut  000|  
//! ----------------------
//! @endcode
static void lcd_show_sensors_state()
{
	//0: N/A; 1: OFF; 2: ON
	uint8_t pinda_state = STATE_NA;
	uint8_t finda_state = STATE_NA;
	uint8_t idler_state = STATE_NA;

	pinda_state = READ(Z_MIN_PIN);
	if (mmu_enabled && ((_millis() - mmu_last_finda_response) < 1000ul) )
	{
		finda_state = mmu_finda;
	}
	//lcd_puts_at_P(0, 0, _i("Sensor state"));
	lcd_puts_at_P(0, 0, _T(MSG_PINDA));
	lcd_set_cursor(LCD_WIDTH - 14, 0);
	lcd_print_state(pinda_state);
	
	if (mmu_enabled == true)
	{
		lcd_puts_at_P(10, 0, _n("FINDA"));////MSG_FINDA c=5
		lcd_set_cursor(LCD_WIDTH - 3, 0);
		lcd_print_state(finda_state);
	}
	
	if (ir_sensor_detected) {
		idler_state = !READ(IR_SENSOR_PIN);
		lcd_puts_at_P(0, 1, _i("Fil. sensor"));
		lcd_set_cursor(LCD_WIDTH - 3, 1);
		lcd_print_state(idler_state);
	}
	

#ifdef PAT9125
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
		//if (!fsensor_enabled)
		//	lcd_puts_P(_N("Filament sensor\n" "is disabled."));
		//else
		//{
		if (!moves_planned() && !IS_SD_PRINTING && !is_usb_printing && (lcd_commands_type != LcdCommands::Layer1Cal))
			pat9125_update();
			lcd_set_cursor(0, 2);
			lcd_printf_P(_N(
				"Xd:  %3d  "
				"Yd:  %3d\n" ////c=4
				"Int: %3d  " ////c=4
				"Shut:  %3d"  ////c=4
			),
				pat9125_x, pat9125_y,
				pat9125_b, pat9125_s
			);
		//}
	}
#endif //PAT9125
}

void lcd_menu_show_sensors_state()                // NOT static due to using inside "Marlin_main" module ("manage_inactivity()")
{
	lcd_timeoutToStatus.stop();
	lcd_show_sensors_state();
	if(LCD_CLICKED)
	{
		lcd_timeoutToStatus.start();
		menu_back();
	}
}

void prusa_statistics_err(char c){
	SERIAL_ECHOPGM("{[ERR:");
	SERIAL_ECHO(c);
	SERIAL_ECHO(']');
	prusa_stat_farm_number();
}

static void prusa_statistics_case0(uint8_t statnr){
	SERIAL_ECHO('{');
	prusa_stat_printerstatus(statnr);
	prusa_stat_farm_number();
	prusa_stat_printinfo();
}

void prusa_statistics(int _message, uint8_t _fil_nr) {
#ifdef DEBUG_DISABLE_PRUSA_STATISTICS
	return;
#endif //DEBUG_DISABLE_PRUSA_STATISTICS
	switch (_message)
	{

	case 0: // default message
		if (busy_state == PAUSED_FOR_USER) 
		{   
			prusa_statistics_case0(15);
		}
		else if (isPrintPaused)
		{
			prusa_statistics_case0(14);
		}
		else if (IS_SD_PRINTING || loading_flag)
		{
			prusa_statistics_case0(4);
		}
		else
		{
			SERIAL_ECHO('{');
			prusa_stat_printerstatus(1);
			prusa_stat_farm_number();
			prusa_stat_diameter();
			status_number = 1;
		}
		break;

	case 1:		// 1 heating
		farm_status = 2;
		SERIAL_ECHO('{');
		prusa_stat_printerstatus(2);
		prusa_stat_farm_number();
		status_number = 2;
		farm_timer = 1;
		break;

	case 2:		// heating done
		farm_status = 3;
		SERIAL_ECHO('{');
		prusa_stat_printerstatus(3);
		prusa_stat_farm_number();
		SERIAL_ECHOLN('}');
		status_number = 3;
		farm_timer = 1;

		if (IS_SD_PRINTING || loading_flag)
		{
			farm_status = 4;
			SERIAL_ECHO('{');
			prusa_stat_printerstatus(4);
			prusa_stat_farm_number();
			status_number = 4;
		}
		else
		{
			SERIAL_ECHO('{');
			prusa_stat_printerstatus(3);
			prusa_stat_farm_number();
			status_number = 3;
		}
		farm_timer = 1;
		break;

	case 3:		// filament change
		// must do a return here to prevent doing SERIAL_ECHOLN("}") at the very end of this function
		// saved a considerable amount of FLASH
		return;
		break;
	case 4:		// print succesfull
		SERIAL_ECHOPGM("{[RES:1][FIL:");
		MYSERIAL.print(int(_fil_nr));
		SERIAL_ECHO(']');
		prusa_stat_printerstatus(status_number);
		prusa_stat_farm_number();
		farm_timer = 2;
		break;
	case 5:		// print not succesfull
		SERIAL_ECHOPGM("{[RES:0][FIL:");
		MYSERIAL.print(int(_fil_nr));
		SERIAL_ECHO(']');
		prusa_stat_printerstatus(status_number);
		prusa_stat_farm_number();
		farm_timer = 2;
		break;
	case 6:		// print done
		SERIAL_ECHOPGM("{[PRN:8]");
		prusa_stat_farm_number();
		status_number = 8;
		farm_timer = 2;
		break;
	case 7:		// print done - stopped
		SERIAL_ECHOPGM("{[PRN:9]");
		prusa_stat_farm_number();
		status_number = 9;
		farm_timer = 2;
		break;
	case 8:		// printer started
		SERIAL_ECHOPGM("{[PRN:0]");
		prusa_stat_farm_number();
		status_number = 0;
		farm_timer = 2;
		break;
	case 20:		// echo farm no
		SERIAL_ECHO('{');
		prusa_stat_printerstatus(status_number);
		prusa_stat_farm_number();
		farm_timer = 4;
		break;
	case 21: // temperatures
		SERIAL_ECHO('{');
		prusa_stat_temperatures();
		prusa_stat_farm_number();
		prusa_stat_printerstatus(status_number);
		break;
    case 22: // waiting for filament change
        SERIAL_ECHOPGM("{[PRN:5]");
		prusa_stat_farm_number();
		status_number = 5;
        break;
	
	case 90: // Error - Thermal Runaway
		prusa_statistics_err('1');
		break;
	case 91: // Error - Thermal Runaway Preheat
		prusa_statistics_err('2');
		break;
	case 92: // Error - Min temp
		prusa_statistics_err('3');
		break;
	case 93: // Error - Max temp
		prusa_statistics_err('4');
		break;

    case 99:		// heartbeat
        SERIAL_ECHOPGM("{[PRN:99]");
        prusa_stat_temperatures();
		prusa_stat_farm_number();
        break;
	}
	SERIAL_ECHOLN('}');	

}

static void prusa_stat_printerstatus(int _status)
{
	SERIAL_ECHOPGM("[PRN:");
	SERIAL_ECHO(_status);
	SERIAL_ECHO(']');
}

static void prusa_stat_farm_number() {
	SERIAL_ECHOPGM("[PFN:0]");
}

static void prusa_stat_diameter() {
	SERIAL_ECHOPGM("[DIA:");
	SERIAL_ECHO(eeprom_read_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM));
	SERIAL_ECHO(']');
}

static void prusa_stat_temperatures()
{
	SERIAL_ECHOPGM("[ST0:");
	SERIAL_ECHO(target_temperature[0]);
	SERIAL_ECHOPGM("][STB:");
	SERIAL_ECHO(target_temperature_bed);
	SERIAL_ECHOPGM("][AT0:");
	SERIAL_ECHO(current_temperature[0]);
	SERIAL_ECHOPGM("][ATB:");
	SERIAL_ECHO(current_temperature_bed);
	SERIAL_ECHO(']');
}

static void prusa_stat_printinfo()
{
	SERIAL_ECHOPGM("[TFU:");
	SERIAL_ECHO(total_filament_used);
	SERIAL_ECHOPGM("][PCD:");
	SERIAL_ECHO(itostr3(card.percentDone()));
	SERIAL_ECHOPGM("][FEM:");
	SERIAL_ECHO(itostr3(feedmultiply));
	SERIAL_ECHOPGM("][FNM:");
	SERIAL_ECHO(card.longFilename[0] ? card.longFilename : card.filename);
	SERIAL_ECHOPGM("][TIM:");
	if (starttime != 0)
	{
		SERIAL_ECHO(_millis() / 1000 - starttime / 1000);
	}
	else
	{
		SERIAL_ECHO(0);
	}
	SERIAL_ECHOPGM("][FWR:");
	SERIAL_ECHORPGM(FW_VERSION_STR_P());
	SERIAL_ECHO(']');
     prusa_stat_diameter();
}

/*
void lcd_pick_babystep(){
    int enc_dif = 0;
    int cursor_pos = 1;
    int fsm = 0;
    
    
    
    
    lcd_clear();
    
    lcd_set_cursor(0, 0);
    
    lcd_puts_P(_i("Pick print"));////MSG_PICK_Z
    
    
    lcd_set_cursor(3, 2);
    
    lcd_print('1');
    
    lcd_set_cursor(3, 3);
    
    lcd_print('2');
    
    lcd_set_cursor(12, 2);
    
    lcd_print('3');
    
    lcd_set_cursor(12, 3);
    
    lcd_print('4');
    
    lcd_set_cursor(1, 2);
    
    lcd_print('>');
    
    
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
                lcd_print(' ');
                lcd_set_cursor(1, 3);
                lcd_print(' ');
                lcd_set_cursor(10, 2);
                lcd_print(' ');
                lcd_set_cursor(10, 3);
                lcd_print(' ');
                
                if (cursor_pos < 3) {
                    lcd_set_cursor(1, cursor_pos+1);
                    lcd_print('>');
                }else{
                    lcd_set_cursor(10, cursor_pos-1);
                    lcd_print('>');
                }
                
   
                enc_dif = lcd_encoder_diff;
                _delay(100);
            }
            
        }
        
        if (lcd_clicked()) {
            fsm = cursor_pos;
            int babyStepZ;
            EEPROM_read_B(EEPROM_BABYSTEP_Z0+((fsm-1)*2),&babyStepZ);
            EEPROM_save_B(EEPROM_BABYSTEP_Z,&babyStepZ);
            calibration_status_store(CALIBRATION_STATUS_CALIBRATED);
            _delay(500);
            
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
	MENU_ITEM_SUBMENU_P(_i("Move X"), lcd_move_x);////MSG_MOVE_X c=18
	MENU_ITEM_SUBMENU_P(_i("Move Y"), lcd_move_y);////MSG_MOVE_Y c=18
	MENU_ITEM_SUBMENU_P(_i("Move Z"), lcd_move_z);////MSG_MOVE_Z c=18
	MENU_ITEM_SUBMENU_P(_T(MSG_EXTRUDER), lcd_move_e);
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
	card.presort_flag = true;
}
#endif //SDCARD_SORT_ALPHA

#ifdef TMC2130
static void lcd_crash_mode_info()
{
	lcd_update_enable(true);
	static uint32_t tim = 0;
	if ((tim + 1000) < _millis())
	{
		lcd_clear();
		fputs_P(_i("Crash detection can\nbe turned on only in\nNormal mode"), lcdout);////MSG_CRASH_DET_ONLY_IN_NORMAL c=20 r=4
		tim = _millis();
	}
    menu_back_if_clicked();
}

static void lcd_crash_mode_info2()
{
	lcd_update_enable(true);
	static uint32_t tim = 0;
	if ((tim + 1000) < _millis())
	{
		lcd_clear();
		fputs_P(_i("WARNING:\nCrash detection\ndisabled in\nStealth mode"), lcdout);////MSG_CRASH_DET_STEALTH_FORCE_OFF c=20 r=4
		tim = _millis();
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
	if ((tim + 1000) < _millis())
	{
          lcd_display_message_fullscreen_nonBlocking_P(_i("Autoloading filament available only when filament sensor is turned on..."), nlines); ////MSG_AUTOLOADING_ONLY_IF_FSENS_ON c=20 r=4
		tim = _millis();
	}
    menu_back_if_clicked();
}

static void lcd_fsensor_fail()
{
uint8_t nlines;
	lcd_update_enable(true);
	static uint32_t tim = 0;
	if ((tim + 1000) < _millis())
	{
          lcd_display_message_fullscreen_nonBlocking_P(_i("ERROR: Filament sensor is not responding, please check connection."), nlines);////MSG_FSENS_NOT_RESPONDING c=20 r=4
		tim = _millis();
	}
    menu_back_if_clicked();
}
#endif //FILAMENT_SENSOR

//-//
static void lcd_sound_state_set(void)
{
Sound_CycleState();
}

#ifndef MMU_FORCE_STEALTH_MODE
static void lcd_silent_mode_mmu_set() {
	if (SilentModeMenu_MMU == 1) SilentModeMenu_MMU = 0;
	else SilentModeMenu_MMU = 1;
	//saving to eeprom is done in mmu_loop() after mmu actually switches state and confirms with "ok"
}
#endif //MMU_FORCE_STEALTH_MODE

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
  lcd_display_message_fullscreen_P(_i("Mode change in progress..."));////MSG_MODE_CHANGE_IN_PROGRESS c=20 r=3
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
	tmc2130_init(TMCInitParams(false, FarmOrUserECool()));
  // We may have missed a stepper timer interrupt due to the time spent in tmc2130_init.
  // Be safe than sorry, reset the stepper timer before re-enabling interrupts.
  st_reset_timer();
  sei();
#endif //TMC2130
  st_current_init();
#ifdef TMC2130
  if (lcd_crash_detect_enabled() && (SilentModeMenu != SILENT_MODE_NORMAL))
	  menu_submenu(lcd_crash_mode_info2);
  lcd_encoder_diff=0;                             // reset 'encoder buffer'
#endif //TMC2130
}

#ifdef TMC2130
static void crash_mode_switch()
{
    if (lcd_crash_detect_enabled())
    {
        lcd_crash_detect_disable();
    }
    else
    {
        lcd_crash_detect_enable();
    }
	if (IS_SD_PRINTING || is_usb_printing || (lcd_commands_type == LcdCommands::Layer1Cal)) menu_goto(lcd_tune_menu, 9, true, true);
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

void lcd_set_degree() {
	lcd_set_custom_characters_degree();
}

#if (LANG_MODE != 0)

void menu_setlang(unsigned char lang)
{
	if (!lang_select(lang))
	{
		if (lcd_show_fullscreen_message_yes_no_and_wait_P(_i("Copy selected language?"), false, true))////MSG_COPY_SEL_LANG c=20 r=3
			lang_boot_update_start(lang);
		lcd_update_enable(true);
		lcd_clear();
		menu_goto(lcd_language_menu, 0, true, true);
		lcd_timeoutToStatus.stop(); //infinite timeout
		lcd_draw_update = 2;
	}
}

#ifdef COMMUNITY_LANG_SUPPORT
#ifdef XFLASH
static void lcd_community_language_menu()
{
	MENU_BEGIN();
	uint8_t cnt = lang_get_count();
	MENU_ITEM_BACK_P(_i("Select language")); //Back to previous Menu
	for (int i = 8; i < cnt; i++) //all community languages
		if (menu_item_text_P(lang_get_name_by_code(lang_get_code(i))))
		{
			menu_setlang(i);
			return;
		}
	MENU_END();
}
#endif //XFLASH
#endif //COMMUNITY_LANG_SUPPORT && W52X20CL



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
#ifdef XFLASH
	if (cnt == 2) //display secondary language in case of clear xflash 
	{
		if (menu_item_text_P(lang_get_name_by_code(lang_get_code(1))))
		{
			menu_setlang(1);
			return;
		}
	}
	else
		for (int i = 2; i < 8; i++) //skip seconday language - solved in lang_select (MK3) 'i < 8'  for 7 official languages
#else //XFLASH
		for (int i = 1; i < cnt; i++) //all seconday languages (MK2/25)
#endif //XFLASH
			if (menu_item_text_P(lang_get_name_by_code(lang_get_code(i))))
			{
				menu_setlang(i);
				return;
			}

#ifdef COMMUNITY_LANG_SUPPORT
#ifdef XFLASH
		MENU_ITEM_SUBMENU_P(_T(MSG_COMMUNITY_MADE), lcd_community_language_menu); ////MSG_COMMUNITY_MADE c=18
#endif //XFLASH
#endif //COMMUNITY_LANG_SUPPORT && W52X20CL

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
		MENU_ITEM_SUBMENU_P(_i("Calibrate"), lcd_calibrate_pinda);////MSG_CALIBRATE_PINDA c=17
	MENU_END();
}

void lcd_temp_calibration_set() {
	bool temp_cal_active = eeprom_read_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE);
	temp_cal_active = !temp_cal_active;
	eeprom_update_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE, temp_cal_active);
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
		plan_buffer_line_curposXYZE(feedrate, active_extruder);
		st_synchronize();

		lcd_display_message_fullscreen_P(msg_e_cal_knob);
		msg_millis = _millis();
		while (!LCD_CLICKED) {
			if (multi_screen && _millis() - msg_millis > 5000) {
				if (msg_next_e_cal_knob == NULL)
					msg_next_e_cal_knob = msg_e_cal_knob;
					msg_next_e_cal_knob = lcd_display_message_fullscreen_P(msg_next_e_cal_knob);
					msg_millis = _millis();
			}

			//manage_inactivity(true);
			manage_heater();
			if (abs(lcd_encoder_diff) >= ENCODER_PULSES_PER_STEP) {						//adjusting mark by knob rotation
				delay_keep_alive(50);
				//previous_millis_cmd = _millis();
				lcd_encoder += (lcd_encoder_diff / ENCODER_PULSES_PER_STEP);
				lcd_encoder_diff = 0;
				if (!planner_queue_full()) {
					current_position[E_AXIS] += float(abs((int)lcd_encoder)) * 0.01; //0.05
					lcd_encoder = 0;
					plan_buffer_line_curposXYZE(feedrate, active_extruder);
					
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

//! @brief Continue first layer calibration with previous value or start from zero?
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |Sheet Smooth1 actual|  c=a, c=b, a+b = 13
//! |Z offset: -1.480 mm |  c=a, c=b, a+b = 14
//! |>Continue           |  c=19
//! | Start from zero    |  c=19
//! ----------------------
//! @endcode
void lcd_first_layer_calibration_reset()
{
    typedef struct
    {
        bool reset;
    } MenuData;
    static_assert(sizeof(menu_data)>= sizeof(MenuData),"_menu_data_t doesn't fit into menu_data");
    MenuData* menuData = (MenuData*)&(menu_data[0]);

    if(LCD_CLICKED || !eeprom_is_sheet_initialized(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet))) ||
            (calibration_status() >= CALIBRATION_STATUS_LIVE_ADJUST) ||
            (0 == static_cast<int16_t>(eeprom_read_word(reinterpret_cast<uint16_t*>
            (&EEPROM_Sheets_base->s[(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)))].z_offset)))))
    {
        if (menuData->reset)
        {
            eeprom_update_word(reinterpret_cast<uint16_t*>(&EEPROM_Sheets_base->s[(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)))].z_offset), 0xffff);
        }
        menu_goto(lcd_v2_calibration,0,true,true);
    }

    if (lcd_encoder > 0)
    {
        menuData->reset = true;
        lcd_encoder = 1;
    }
    else if (lcd_encoder < 1)
    {
        menuData->reset = false;
        lcd_encoder = 0;
    }

    char sheet_name[sizeof(Sheet::name)];
    eeprom_read_block(sheet_name, &EEPROM_Sheets_base->s[(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)))].name, sizeof(Sheet::name));
    lcd_set_cursor(0, 0);
    float offset = static_cast<int16_t>(eeprom_read_word(reinterpret_cast<uint16_t*>(&EEPROM_Sheets_base->s[(eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)))].z_offset)))/cs.axis_steps_per_unit[Z_AXIS];
    lcd_printf_P(_i("Sheet %.7s\nZ offset: %+1.3f mm\n%cContinue\n%cStart from zero"), //// \n denotes line break, %.7s is replaced by 7 character long sheet name, %+1.3f is replaced by 6 character long floating point number, %c is replaced by > or white space (one character) based on whether first or second option is selected. % denoted place holders can not be reordered. r=4
            sheet_name, offset, menuData->reset ? ' ' : '>', menuData->reset ? '>' : ' ');

}

void lcd_v2_calibration()
{
	if (mmu_enabled)
	{
	    const uint8_t filament = choose_menu_P(
            _i("Select filament:"), ////MSG_SELECT_FILAMENT c=20
            _T(MSG_FILAMENT),(_T(MSG_CANCEL)+1)); //Hack to reuse MSG but strip 1st char off
	    if (filament < 5)
	    {
	        lay1cal_filament = filament;
	    }
	    else
	    {
	        menu_back();
	        return;
	    }
	}
	else if (!eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE))
	{
	    bool loaded = false;
	    if (fsensor_enabled && ir_sensor_detected)
	    {
	        loaded = (digitalRead(IR_SENSOR_PIN) == 0);
	    }
	    else
	    {
	        loaded = lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_FILAMENT_LOADED), false, true);
	        lcd_update_enabled = true;

	    }


		if (!loaded)
		{
			lcd_display_message_fullscreen_P(_i("Please load filament first."));////MSG_PLEASE_LOAD_PLA c=20 r=4
			lcd_consume_click();
			for (uint_least8_t i = 0; i < 20; i++) { //wait max. 2s
				delay_keep_alive(100);
				if (lcd_clicked()) {
					break;
				}
			}
			lcd_update_enabled = true;
			menu_back();
			return;
		}
	}

	eFilamentAction = FilamentAction::Lay1Cal;
	menu_goto(lcd_generic_preheat_menu, 0, true, true);
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

#if (LANG_MODE != 0)
void lcd_language()
{
	lcd_update_enable(true);
	lcd_clear();
	menu_goto(lcd_language_menu, 0, true, true);
	lcd_timeoutToStatus.stop(); //infinite timeout
	lcd_draw_update = 2;
	while ((menu_menu != lcd_status_screen) && (!lang_is_selected()))
	{
		_delay(50);
		lcd_update(0);
		manage_heater();
		manage_inactivity(true);
	}
	if (lang_is_selected())
		lcd_return_to_status();
	else
		lang_select(LANG_ID_PRI);
}
#endif

static void wait_preheat()
{
    current_position[Z_AXIS] = 100; //move in z axis to make space for loading filament
    plan_buffer_line_curposXYZE(homing_feedrate[Z_AXIS] / 60);
    delay_keep_alive(2000);
    lcd_display_message_fullscreen_P(_T(MSG_WIZARD_HEATING));
	lcd_set_custom_characters();
	while (fabs(degHotend(0) - degTargetHotend(0)) > 3) {
        lcd_display_message_fullscreen_P(_T(MSG_WIZARD_HEATING));

        lcd_set_cursor(0, 4);
	    //Print the hotend temperature (9 chars total)
		lcdui_print_temp(LCD_STR_THERMOMETER[0], (int)(degHotend(0) + 0.5), (int)(degTargetHotend(0) + 0.5));
        delay_keep_alive(1000);
    }
	
}

static void lcd_wizard_load()
{
	if (mmu_enabled)
	{
		lcd_show_fullscreen_message_and_wait_P(_i("Please insert filament into the first tube of the MMU, then press the knob to load it."));////MSG_MMU_INSERT_FILAMENT_FIRST_TUBE c=20 r=6
		tmp_extruder = 0;
	} 
	else
	{
		lcd_show_fullscreen_message_and_wait_P(_i("Please insert filament into the extruder, then press the knob to load it."));////MSG_WIZARD_LOAD_FILAMENT c=20 r=6
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

static void wizard_lay1cal_message(bool cold)
{
    lcd_show_fullscreen_message_and_wait_P(
            _i("Now I will calibrate distance between tip of the nozzle and heatbed surface.")); ////MSG_WIZARD_V2_CAL c=20 r=8
    if (mmu_enabled)
    {
        lcd_show_fullscreen_message_and_wait_P(
                _i("Choose a filament for the First Layer Calibration and select it in the on-screen menu."));////MSG_CHOOSE_FIL_1ST_LAYERCAL c=20 r=7
    }
    else if (cold)
    {
        lcd_show_fullscreen_message_and_wait_P(
                _i("Select temperature which matches your material."));////MSG_SELECT_TEMP_MATCHES_MATERIAL c=20 r=4
    }
    lcd_show_fullscreen_message_and_wait_P(
            _i("The printer will start printing a zig-zag line. Rotate the knob until you reach the optimal height. Check the pictures in the handbook (Calibration chapter).")); ////MSG_WIZARD_V2_CAL_2 c=20 r=12
}

//! @brief Printer first run wizard (Selftest and calibration)
//!
//!
//! First layer calibration with MMU state diagram
//!
//! @startuml
//! [*] --> IsFil
//! IsFil : Is any filament loaded?
//! LoadFilCold : Push the button to start loading Filament 1
//!
//! IsFil       --> Lay1CalCold : yes
//! IsFil       --> LoadFilCold : no
//! LoadFilCold --> Lay1CalCold : click
//! @enduml
//!
//! First layer calibration without MMU state diagram
//!
//! @startuml
//! [*] --> IsFil
//! IsFil : Is filament loaded?
//! Preheat : Select nozle temperature which matches your material.
//! LoadFilHot : Insert filament to extruder and press the knob.
//!
//! IsFil   --> Lay1CalCold : yes
//! IsFil   --> Preheat    : no
//! Preheat --> LoadFilHot : select
//! LoadFilHot --> Lay1CalHot : click
//! @enduml
//!
//! @param state Entry point of the wizard
//!
//!   state                 | description
//!  ---------------------- | ----------------
//! WizState::Run           | Main entry point
//! WizState::RepeatLay1Cal | Entry point after passing 1st layer calibration
//! WizState::LoadFilHot    | Entry point after temporarily left for preheat before load filament
void lcd_wizard(WizState state)
{
    using S = WizState;
	bool end = false;
	int8_t wizard_event;
	const char *msg = NULL;
	// Make sure EEPROM_WIZARD_ACTIVE is true if entering using different entry point
	// other than WizState::Run - it is useful for debugging wizard.
	if (state != S::Run) eeprom_update_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 1);
    
    FORCE_BL_ON_START;
	
    while (!end) {
		printf_P(PSTR("Wizard state: %d\n"), state);
		switch (state) {
		case S::Run: //Run wizard?
			
			// 2019-08-07 brutal hack - solving the "viper" situation.
			// It is caused by the fact, that tmc2130_st_isr makes a crash detection before the printers really starts.
			// And thus it calles stop_and_save_print_to_ram which sets the saved_printing flag.
			// Having this flag set during normal printing is lethal - mesh_plan_buffer_line exist in the middle of planning long travels
			// which results in distorted print.
			// This primarily happens when the printer is new and parked in 0,0
			// So any new printer will fail the first layer calibration unless being reset or the Stop function gets called.
			// We really must find a way to prevent the crash from happening before the printer is started - that would be the correct solution.
			// Btw. the flag may even trigger the viper situation on normal start this way and the user won't be able to find out why.			
			saved_printing = false;
			
			if( eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE)==2){
				lcd_show_fullscreen_message_and_wait_P(_T(MSG_WIZARD_WELCOME_SHIPPING));
				state = S::Restore;
			} else {
				wizard_event = lcd_show_multiscreen_message_yes_no_and_wait_P(_T(MSG_WIZARD_WELCOME), false, true);
				if (wizard_event) {
					state = S::Restore;
					eeprom_update_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 1);
				} else {
					eeprom_update_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 0);
					end = true;
				}
			}
			break;
		case S::Restore:
			switch (calibration_status()) {
			case CALIBRATION_STATUS_ASSEMBLED: state = S::Selftest; break; //run selftest
			case CALIBRATION_STATUS_XYZ_CALIBRATION: state = S::Xyz; break; //run xyz cal.
			case CALIBRATION_STATUS_Z_CALIBRATION: state = S::Z; break; //run z cal.
			case CALIBRATION_STATUS_LIVE_ADJUST: state = S::IsFil; break; //run live adjust
			case CALIBRATION_STATUS_CALIBRATED: end = true; eeprom_update_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 0); break;
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
		case S::Xyz:
			lcd_show_fullscreen_message_and_wait_P(_i("I will run xyz calibration now. It will take approx. 12 mins."));////MSG_WIZARD_XYZ_CAL c=20 r=8
			wizard_event = gcode_M45(false, 0);
			if (wizard_event) state = S::IsFil;
			else end = true;
			break;
		case S::Z:
			lcd_show_fullscreen_message_and_wait_P(_i("Please remove shipping helpers first."));////MSG_REMOVE_SHIPPING_HELPERS c=20 r=3
			lcd_show_fullscreen_message_and_wait_P(_i("Now remove the test print from steel sheet."));////MSG_REMOVE_TEST_PRINT c=20 r=4
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
				unload_filament(true);
				//load filament
				lcd_wizard_load();
				setTargetHotend(0, 0); //we are finished, cooldown nozzle
				state = S::Finish; //shipped, no need to set first layer, go to final message directly
			}
			else end = true;
			break;
		case S::IsFil:
		    //start to preheat nozzle and bed to save some time later
			setTargetHotend(PLA_PREHEAT_HOTEND_TEMP, 0);
			setTargetBed(PLA_PREHEAT_HPB_TEMP);
			if (mmu_enabled)
			{
			    wizard_event = lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_FILAMENT_LOADED), true);
			} else
			{
			    wizard_event = lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_FILAMENT_LOADED), true);
			}
			if (wizard_event) state = S::Lay1CalCold;
			else
			{
			    if(mmu_enabled) state = S::LoadFilCold;
			    else state = S::Preheat;
			}
			break;
		case S::Preheat:
		    menu_goto(lcd_preheat_menu,0,false,true);
		    lcd_show_fullscreen_message_and_wait_P(_i("Select nozzle preheat temperature which matches your material."));////MSG_SEL_PREHEAT_TEMP c=20 r=6
		    end = true; // Leave wizard temporarily for lcd_preheat_menu
		    break;
		case S::LoadFilHot:
		    wait_preheat();
			lcd_wizard_load();
			state = S::Lay1CalHot;
			break;
        case S::LoadFilCold:
            lcd_wizard_load();
            state = S::Lay1CalCold;
            break;
		case S::Lay1CalCold:
            wizard_lay1cal_message(true);
			menu_goto(lcd_v2_calibration,0,false,true);
			end = true; // Leave wizard temporarily for lcd_v2_calibration
			break;
        case S::Lay1CalHot:
            wizard_lay1cal_message(false);
            lcd_commands_type = LcdCommands::Layer1Cal;
            end = true; // Leave wizard temporarily for lcd_v2_calibration
            break;
		case S::RepeatLay1Cal:
			wizard_event = lcd_show_multiscreen_message_yes_no_and_wait_P(_i("Do you want to repeat last step to readjust distance between nozzle and heatbed?"), false);////MSG_WIZARD_REPEAT_V2_CAL c=20 r=7
			if (wizard_event)
			{
				lcd_show_fullscreen_message_and_wait_P(_i("Please clean heatbed and then press the knob."));////MSG_WIZARD_CLEAN_HEATBED c=20 r=8
				state = S::Lay1CalCold;
			}
			else
			{
			    lcd_show_fullscreen_message_and_wait_P(_i("If you have additional steel sheets, calibrate their presets in Settings - HW Setup - Steel sheets."));////MSG_ADDITIONAL_SHEETS c=20 r=9
				state = S::Finish;
			}
			break;
		case S::Finish:
			eeprom_update_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 0);
			end = true;
			break;

		default: break;
		}
	}
    
    FORCE_BL_ON_END;
    
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
	if (!((S::Lay1CalCold == state) || (S::Lay1CalHot == state) || (S::Preheat == state)))
	{
		lcd_show_fullscreen_message_and_wait_P(msg);
	}
	lcd_update_enable(true);
	lcd_update(2);
}

#ifdef TMC2130
void lcd_settings_linearity_correction_menu(void)
{
	MENU_BEGIN();
    ON_MENU_LEAVE(
        lcd_settings_linearity_correction_menu_save();
    );
	MENU_ITEM_BACK_P(_T(MSG_SETTINGS));
#ifdef TMC2130_LINEARITY_CORRECTION_XYZ
	//tmc2130_wave_fac[X_AXIS]

	MENU_ITEM_EDIT_int3_P(_i("X-correct:"),  &tmc2130_wave_fac[X_AXIS],  TMC2130_WAVE_FAC1000_MIN-TMC2130_WAVE_FAC1000_STP, TMC2130_WAVE_FAC1000_MAX);////MSG_X_CORRECTION c=13
	MENU_ITEM_EDIT_int3_P(_i("Y-correct:"),  &tmc2130_wave_fac[Y_AXIS],  TMC2130_WAVE_FAC1000_MIN-TMC2130_WAVE_FAC1000_STP, TMC2130_WAVE_FAC1000_MAX);////MSG_Y_CORRECTION c=13
	MENU_ITEM_EDIT_int3_P(_i("Z-correct:"),  &tmc2130_wave_fac[Z_AXIS],  TMC2130_WAVE_FAC1000_MIN-TMC2130_WAVE_FAC1000_STP, TMC2130_WAVE_FAC1000_MAX);////MSG_Z_CORRECTION c=13
#endif //TMC2130_LINEARITY_CORRECTION_XYZ
	MENU_ITEM_EDIT_int3_P(_i("E-correct:"),  &tmc2130_wave_fac[E_AXIS],  TMC2130_WAVE_FAC1000_MIN-TMC2130_WAVE_FAC1000_STP, TMC2130_WAVE_FAC1000_MAX);////MSG_EXTRUDER_CORRECTION c=13
	MENU_END();
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
            MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR), _T(MSG_NA), lcd_fsensor_state_set);/*////MSG_FSENSOR_NA*/\
            MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR_AUTOLOAD), NULL, lcd_fsensor_fail);\
        }\
        else\
        {\
            /* Filament sensor turned off, working, no problems*/\
            MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR), _T(MSG_OFF), lcd_fsensor_state_set);\
            if (mmu_enabled == false)\
            {\
                MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR_AUTOLOAD), NULL, lcd_filament_autoload_info);\
            }\
        }\
    }\
    else\
    {\
        /* Filament sensor turned on, working, no problems*/\
        MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR), _T(MSG_ON), lcd_fsensor_state_set);\
        if (mmu_enabled == false)\
        {\
            if (fsensor_autoload_enabled)\
                MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR_AUTOLOAD), _T(MSG_ON), lcd_set_filament_autoload);/*////MSG_FSENS_AUTOLOAD_ON c=17*/\
            else\
                MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR_AUTOLOAD), _T(MSG_OFF), lcd_set_filament_autoload);/*////MSG_FSENS_AUTOLOAD_OFF c=17*/\
            /*if (fsensor_oq_meassure_enabled)*/\
                /*MENU_ITEM_FUNCTION_P(_i("F. OQ meass. [on]"), lcd_set_filament_oq_meass);*//*////MSG_FSENS_OQMEASS_ON c=17*/\
            /*else*/\
                /*MENU_ITEM_FUNCTION_P(_i("F. OQ meass.[off]"), lcd_set_filament_oq_meass);*//*////MSG_FSENS_OQMEASS_OFF c=17*/\
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

static void settingsAutoDeplete()
{
    if (mmu_enabled)
    {
        if (!fsensor_enabled)
        {
            MENU_ITEM_TOGGLE_P(_T(MSG_AUTO_DEPLETE), _T(MSG_NA), NULL);
        }
        else if (lcd_autoDeplete)
        {
            MENU_ITEM_TOGGLE_P(_T(MSG_AUTO_DEPLETE), _T(MSG_ON), auto_deplete_switch);
        }
        else
        {
            MENU_ITEM_TOGGLE_P(_T(MSG_AUTO_DEPLETE), _T(MSG_OFF), auto_deplete_switch);
        }
    }
}

#define SETTINGS_AUTO_DEPLETE \
do\
{\
    settingsAutoDeplete();\
}\
while(0)\

#ifdef MMU_HAS_CUTTER
static void settingsCutter()
{
    if (mmu_enabled)
    {
        if (EEPROM_MMU_CUTTER_ENABLED_enabled == eeprom_read_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED))
        {
            MENU_ITEM_TOGGLE_P(_T(MSG_CUTTER), _T(MSG_ON), lcd_cutter_enabled);
        }
#ifdef MMU_ALWAYS_CUT
        else if (EEPROM_MMU_CUTTER_ENABLED_always == eeprom_read_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED))
        {
            MENU_ITEM_TOGGLE_P(_T(MSG_CUTTER), _i("Always"), lcd_cutter_enabled);
        }
#endif
        else
        {
            MENU_ITEM_TOGGLE_P(_T(MSG_CUTTER), _T(MSG_OFF), lcd_cutter_enabled);
        }
    }
}

#define SETTINGS_CUTTER \
do\
{\
    settingsCutter();\
}\
while(0)
#else
#define SETTINGS_CUTTER
#endif //MMU_HAS_CUTTER

#ifdef TMC2130
#define SETTINGS_SILENT_MODE \
do\
{\
    if(!farm_mode)\
    {\
        if (SilentModeMenu == SILENT_MODE_NORMAL)\
        {\
            MENU_ITEM_TOGGLE_P(_T(MSG_MODE), _T(MSG_NORMAL), lcd_silent_mode_set);\
        }\
        else MENU_ITEM_TOGGLE_P(_T(MSG_MODE), _T(MSG_STEALTH), lcd_silent_mode_set);\
        if (SilentModeMenu == SILENT_MODE_NORMAL)\
        {\
            if (lcd_crash_detect_enabled()) MENU_ITEM_TOGGLE_P(_T(MSG_CRASHDETECT), _T(MSG_ON), crash_mode_switch);\
            else MENU_ITEM_TOGGLE_P(_T(MSG_CRASHDETECT), _T(MSG_OFF), crash_mode_switch);\
        }\
        else MENU_ITEM_TOGGLE_P(_T(MSG_CRASHDETECT), NULL, lcd_crash_mode_info);\
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
            MENU_ITEM_TOGGLE_P(_T(MSG_MODE), _T(MSG_HIGH_POWER), lcd_silent_mode_set);\
            break;\
        case SILENT_MODE_SILENT:\
            MENU_ITEM_TOGGLE_P(_T(MSG_MODE), _T(MSG_SILENT), lcd_silent_mode_set);\
            break;\
        case SILENT_MODE_AUTO:\
            MENU_ITEM_TOGGLE_P(_T(MSG_MODE), _T(MSG_AUTO_POWER), lcd_silent_mode_set);\
            break;\
        default:\
            MENU_ITEM_TOGGLE_P(_T(MSG_MODE), _T(MSG_HIGH_POWER), lcd_silent_mode_set);\
            break; /* (probably) not needed*/\
        }\
    }\
}\
while (0)
#endif //TMC2130

#ifndef MMU_FORCE_STEALTH_MODE
#define SETTINGS_MMU_MODE \
do\
{\
	if (mmu_enabled)\
	{\
		if (SilentModeMenu_MMU == 0) MENU_ITEM_TOGGLE_P(_T(MSG_MMU_MODE), _T(MSG_NORMAL), lcd_silent_mode_mmu_set);\
		else MENU_ITEM_TOGGLE_P(_T(MSG_MMU_MODE), _T(MSG_STEALTH), lcd_silent_mode_mmu_set);\
	}\
}\
while (0) 
#else //MMU_FORCE_STEALTH_MODE
#define SETTINGS_MMU_MODE
#endif //MMU_FORCE_STEALTH_MODE

#ifdef SDCARD_SORT_ALPHA
#define SETTINGS_SD \
do\
{\
    if (card.ToshibaFlashAir_isEnabled())\
        MENU_ITEM_TOGGLE_P(_T(MSG_SD_CARD), _T(MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY), lcd_toshiba_flash_air_compatibility_toggle);\
    else\
        MENU_ITEM_TOGGLE_P(_T(MSG_SD_CARD), _T(MSG_NORMAL), lcd_toshiba_flash_air_compatibility_toggle);\
\
    uint8_t sdSort;\
    EEPROM_read(EEPROM_SD_SORT, (uint8_t*)&sdSort, sizeof(sdSort));\
    switch (sdSort)\
    {\
      case SD_SORT_TIME: MENU_ITEM_TOGGLE_P(_T(MSG_SORT), _T(MSG_SORT_TIME), lcd_sort_type_set); break;\
      case SD_SORT_ALPHA: MENU_ITEM_TOGGLE_P(_T(MSG_SORT), _T(MSG_SORT_ALPHA), lcd_sort_type_set); break;\
      default: MENU_ITEM_TOGGLE_P(_T(MSG_SORT), _T(MSG_NONE), lcd_sort_type_set);\
    }\
}\
while (0)
#else // SDCARD_SORT_ALPHA
#define SETTINGS_SD \
do\
{\
    if (card.ToshibaFlashAir_isEnabled())\
        MENU_ITEM_TOGGLE_P(_T(MSG_SD_CARD), _T(MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY), lcd_toshiba_flash_air_compatibility_toggle);\
    else\
        MENU_ITEM_TOGGLE_P(_T(MSG_SD_CARD), _T(MSG_NORMAL), lcd_toshiba_flash_air_compatibility_toggle);\
}\
while (0)
#endif // SDCARD_SORT_ALPHA

/*
#define SETTINGS_MBL_MODE \
do\
{\
    switch(e_mbl_type)\
    {\
    case e_MBL_FAST:\
        MENU_ITEM_FUNCTION_P(_i("Mode    [Fast]"),mbl_mode_set);\ 
         break; \
    case e_MBL_OPTIMAL:\
	    MENU_ITEM_FUNCTION_P(_i("Mode [Optimal]"), mbl_mode_set); \ 
	     break; \
    case e_MBL_PREC:\
	     MENU_ITEM_FUNCTION_P(_i("Mode [Precise]"), mbl_mode_set); \
	     break; \
    default:\
	     MENU_ITEM_FUNCTION_P(_i("Mode [Optimal]"), mbl_mode_set); \
	     break; \
    }\
}\
while (0)
*/

#define SETTINGS_SOUND \
do\
{\
    switch(eSoundMode)\
    {\
        case e_SOUND_MODE_LOUD:\
            MENU_ITEM_TOGGLE_P(_T(MSG_SOUND), _T(MSG_SOUND_LOUD), lcd_sound_state_set);\
            break;\
        case e_SOUND_MODE_ONCE:\
            MENU_ITEM_TOGGLE_P(_T(MSG_SOUND), _T(MSG_SOUND_ONCE), lcd_sound_state_set);\
            break;\
        case e_SOUND_MODE_SILENT:\
            MENU_ITEM_TOGGLE_P(_T(MSG_SOUND), _T(MSG_SILENT), lcd_sound_state_set);\
            break;\
        case e_SOUND_MODE_BLIND:\
            MENU_ITEM_TOGGLE_P(_T(MSG_SOUND), _T(MSG_SOUND_BLIND), lcd_sound_state_set);\
            break;\
        default:\
            MENU_ITEM_TOGGLE_P(_T(MSG_SOUND), _T(MSG_SOUND_LOUD), lcd_sound_state_set);\
    }\
}\
while (0)

//-//
static void lcd_check_mode_set(void)
{
switch(oCheckMode)
     {
     case ClCheckMode::_None:
          oCheckMode=ClCheckMode::_Warn;
          break;
     case ClCheckMode::_Warn:
          oCheckMode=ClCheckMode::_Strict;
          break;
     case ClCheckMode::_Strict:
          oCheckMode=ClCheckMode::_None;
          break;
     default:
          oCheckMode=ClCheckMode::_None;
     }
eeprom_update_byte((uint8_t*)EEPROM_CHECK_MODE,(uint8_t)oCheckMode);
}

#define SETTINGS_MODE \
do\
{\
    switch(oCheckMode)\
         {\
         case ClCheckMode::_None:\
              MENU_ITEM_TOGGLE_P(_T(MSG_NOZZLE), _T(MSG_NONE), lcd_check_mode_set);\
              break;\
         case ClCheckMode::_Warn:\
              MENU_ITEM_TOGGLE_P(_T(MSG_NOZZLE), _T(MSG_WARN), lcd_check_mode_set);\
              break;\
         case ClCheckMode::_Strict:\
              MENU_ITEM_TOGGLE_P(_T(MSG_NOZZLE), _T(MSG_STRICT), lcd_check_mode_set);\
              break;\
         default:\
              MENU_ITEM_TOGGLE_P(_T(MSG_NOZZLE), _T(MSG_NONE), lcd_check_mode_set);\
         }\
}\
while (0)

static void lcd_nozzle_diameter_cycle(void) {
    uint16_t nDiameter;
    switch(oNozzleDiameter){
    case ClNozzleDiameter::_Diameter_250:
        oNozzleDiameter=ClNozzleDiameter::_Diameter_400;
        nDiameter=400;
        break;
    case ClNozzleDiameter::_Diameter_400:
        oNozzleDiameter=ClNozzleDiameter::_Diameter_600;
        nDiameter=600;
        break;
    case ClNozzleDiameter::_Diameter_600:
        oNozzleDiameter=ClNozzleDiameter::_Diameter_800;
        nDiameter=800;
        break;
    case ClNozzleDiameter::_Diameter_800:
        oNozzleDiameter=ClNozzleDiameter::_Diameter_250;
        nDiameter=250;
        break;
    default:
        oNozzleDiameter=ClNozzleDiameter::_Diameter_400;
        nDiameter=400;
    }
    eeprom_update_byte((uint8_t*)EEPROM_NOZZLE_DIAMETER,(uint8_t)oNozzleDiameter);
    eeprom_update_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM,nDiameter);
}

#define SETTINGS_NOZZLE \
do\
{\
    float fNozzleDiam;\
    switch(oNozzleDiameter)\
    {\
        case ClNozzleDiameter::_Diameter_250: fNozzleDiam = 0.25f; break;\
        case ClNozzleDiameter::_Diameter_400: fNozzleDiam = 0.4f; break;\
        case ClNozzleDiameter::_Diameter_600: fNozzleDiam = 0.6f; break;\
        case ClNozzleDiameter::_Diameter_800: fNozzleDiam = 0.8f; break;\
        default: fNozzleDiam = 0.4f; break;\
    }\
    MENU_ITEM_TOGGLE(_T(MSG_NOZZLE_DIAMETER), ftostr12ns(fNozzleDiam), lcd_nozzle_diameter_cycle);\
}\
while (0)

static void lcd_check_model_set(void)
{
switch(oCheckModel)
     {
     case ClCheckModel::_None:
          oCheckModel=ClCheckModel::_Warn;
          break;
     case ClCheckModel::_Warn:
          oCheckModel=ClCheckModel::_Strict;
          break;
     case ClCheckModel::_Strict:
          oCheckModel=ClCheckModel::_None;
          break;
     default:
          oCheckModel=ClCheckModel::_None;
     }
eeprom_update_byte((uint8_t*)EEPROM_CHECK_MODEL,(uint8_t)oCheckModel);
}

#define SETTINGS_MODEL \
do\
{\
    switch(oCheckModel)\
         {\
         case ClCheckModel::_None:\
              MENU_ITEM_TOGGLE_P(_T(MSG_MODEL), _T(MSG_NONE), lcd_check_model_set);\
              break;\
         case ClCheckModel::_Warn:\
              MENU_ITEM_TOGGLE_P(_T(MSG_MODEL), _T(MSG_WARN), lcd_check_model_set);\
              break;\
         case ClCheckModel::_Strict:\
              MENU_ITEM_TOGGLE_P(_T(MSG_MODEL), _T(MSG_STRICT), lcd_check_model_set);\
              break;\
         default:\
              MENU_ITEM_TOGGLE_P(_T(MSG_MODEL), _T(MSG_NONE), lcd_check_model_set);\
         }\
}\
while (0)

static void lcd_check_version_set(void)
{
switch(oCheckVersion)
     {
     case ClCheckVersion::_None:
          oCheckVersion=ClCheckVersion::_Warn;
          break;
     case ClCheckVersion::_Warn:
          oCheckVersion=ClCheckVersion::_Strict;
          break;
     case ClCheckVersion::_Strict:
          oCheckVersion=ClCheckVersion::_None;
          break;
     default:
          oCheckVersion=ClCheckVersion::_None;
     }
eeprom_update_byte((uint8_t*)EEPROM_CHECK_VERSION,(uint8_t)oCheckVersion);
}

#define SETTINGS_VERSION \
do\
{\
    switch(oCheckVersion)\
         {\
         case ClCheckVersion::_None:\
              MENU_ITEM_TOGGLE_P(_T(MSG_FIRMWARE), _T(MSG_NONE), lcd_check_version_set);\
              break;\
         case ClCheckVersion::_Warn:\
              MENU_ITEM_TOGGLE_P(_T(MSG_FIRMWARE), _T(MSG_WARN), lcd_check_version_set);\
              break;\
         case ClCheckVersion::_Strict:\
              MENU_ITEM_TOGGLE_P(_T(MSG_FIRMWARE), _T(MSG_STRICT), lcd_check_version_set);\
              break;\
         default:\
              MENU_ITEM_TOGGLE_P(_T(MSG_FIRMWARE), _T(MSG_NONE), lcd_check_version_set);\
         }\
}\
while (0)

#if 0 // temporarily unused
static void lcd_check_gcode_set(void)
{
switch(oCheckGcode)
     {
     case ClCheckGcode::_None:
          oCheckGcode=ClCheckGcode::_Warn;
          break;
     case ClCheckGcode::_Warn:
          oCheckGcode=ClCheckGcode::_Strict;
          break;
     case ClCheckGcode::_Strict:
          oCheckGcode=ClCheckGcode::_None;
          break;
     default:
          oCheckGcode=ClCheckGcode::_None;
     }
eeprom_update_byte((uint8_t*)EEPROM_CHECK_GCODE,(uint8_t)oCheckGcode);
}
#endif

#define SETTINGS_GCODE \
do\
{\
    switch(oCheckGcode)\
         {\
         case ClCheckGcode::_None:\
              MENU_ITEM_TOGGLE_P(_T(MSG_GCODE), _T(MSG_NONE), lcd_check_gcode_set);\
              break;\
         case ClCheckGcode::_Warn:\
              MENU_ITEM_TOGGLE_P(_T(MSG_GCODE), _T(MSG_WARN), lcd_check_gcode_set);\
              break;\
         case ClCheckGcode::_Strict:\
              MENU_ITEM_TOGGLE_P(_T(MSG_GCODE), _T(MSG_STRICT), lcd_check_gcode_set);\
              break;\
         default:\
              MENU_ITEM_TOGGLE_P(_T(MSG_GCODE), _T(MSG_NONE), lcd_check_gcode_set);\
         }\
}\
while (0)

static void lcd_checking_menu(void)
{
MENU_BEGIN();
MENU_ITEM_BACK_P(_T(MSG_HW_SETUP));
SETTINGS_MODE;
SETTINGS_MODEL;
SETTINGS_VERSION;
//-// temporarily disabled
//SETTINGS_GCODE;
MENU_END();
}

#ifdef IR_SENSOR_ANALOG
static void lcd_fsensor_actionNA_set(void)
{
switch(oFsensorActionNA)
     {
     case ClFsensorActionNA::_Continue:
          oFsensorActionNA=ClFsensorActionNA::_Pause;
          break;
     case ClFsensorActionNA::_Pause:
          oFsensorActionNA=ClFsensorActionNA::_Continue;
          break;
     default:
          oFsensorActionNA=ClFsensorActionNA::_Continue;
     }
eeprom_update_byte((uint8_t*)EEPROM_FSENSOR_ACTION_NA,(uint8_t)oFsensorActionNA);
}

#define FSENSOR_ACTION_NA \
do\
{\
    switch(oFsensorActionNA)\
         {\
         case ClFsensorActionNA::_Continue:\
              MENU_ITEM_TOGGLE_P(_T(MSG_FS_ACTION), _T(MSG_FS_CONTINUE), lcd_fsensor_actionNA_set);\
              break;\
         case ClFsensorActionNA::_Pause:\
              MENU_ITEM_TOGGLE_P(_T(MSG_FS_ACTION), _T(MSG_FS_PAUSE), lcd_fsensor_actionNA_set);\
              break;\
         default:\
              oFsensorActionNA=ClFsensorActionNA::_Continue;\
         }\
}\
while (0)
#endif //IR_SENSOR_ANALOG

template <uint8_t number>
static void select_sheet_menu()
{
    selected_sheet = number;
    lcd_sheet_menu();
}

static void sheets_menu()
{
    MENU_BEGIN();
    MENU_ITEM_BACK_P(_T(MSG_HW_SETUP));
    MENU_ITEM_SUBMENU_E(EEPROM_Sheets_base->s[0], select_sheet_menu<0>);
    MENU_ITEM_SUBMENU_E(EEPROM_Sheets_base->s[1], select_sheet_menu<1>);
    MENU_ITEM_SUBMENU_E(EEPROM_Sheets_base->s[2], select_sheet_menu<2>);
    MENU_ITEM_SUBMENU_E(EEPROM_Sheets_base->s[3], select_sheet_menu<3>);
    MENU_ITEM_SUBMENU_E(EEPROM_Sheets_base->s[4], select_sheet_menu<4>);
    MENU_ITEM_SUBMENU_E(EEPROM_Sheets_base->s[5], select_sheet_menu<5>);
    MENU_ITEM_SUBMENU_E(EEPROM_Sheets_base->s[6], select_sheet_menu<6>);
    MENU_ITEM_SUBMENU_E(EEPROM_Sheets_base->s[7], select_sheet_menu<7>);
    MENU_END();
}

void lcd_hw_setup_menu(void)                      // can not be "static"
{
    typedef struct
    {// 2bytes total
        int8_t status;
        uint8_t experimental_menu_visibility;
    } _menu_data_t;
    static_assert(sizeof(menu_data)>= sizeof(_menu_data_t),"_menu_data_t doesn't fit into menu_data");
    _menu_data_t* _md = (_menu_data_t*)&(menu_data[0]);

    if (_md->status == 0 || lcd_draw_update)
    {
        _md->status = 1;
        _md->experimental_menu_visibility = eeprom_read_byte((uint8_t *)EEPROM_EXPERIMENTAL_VISIBILITY);
        if (_md->experimental_menu_visibility == EEPROM_EMPTY_VALUE)
        {
            _md->experimental_menu_visibility = 0;
            eeprom_update_byte((uint8_t *)EEPROM_EXPERIMENTAL_VISIBILITY, _md->experimental_menu_visibility);
        }
    }


    MENU_BEGIN();
    MENU_ITEM_BACK_P(_T(bSettings?MSG_SETTINGS:MSG_BACK)); // i.e. default menu-item / menu-item after checking mismatch

    MENU_ITEM_SUBMENU_P(_T(MSG_STEEL_SHEETS), sheets_menu);
    SETTINGS_NOZZLE;
    MENU_ITEM_SUBMENU_P(_i("Checks"), lcd_checking_menu);  ////MSG_CHECKS c=18

#ifdef IR_SENSOR_ANALOG
    FSENSOR_ACTION_NA;
    //! Fsensor Detection isn't ready for mmu yet it is temporarily disabled.
    //! @todo Don't forget to remove this as soon Fsensor Detection works with mmu
    if(!mmu_enabled) MENU_ITEM_FUNCTION_P(PSTR("Fsensor Detection"), lcd_detect_IRsensor);
#endif //IR_SENSOR_ANALOG

    if (_md->experimental_menu_visibility)
    {
        MENU_ITEM_SUBMENU_P(PSTR("Experimental"), lcd_experimental_menu);////MSG_MENU_EXPERIMENTAL c=18
    }

#ifdef PINDA_TEMP_COMP
	//! The SuperPINDA is detected when the PINDA temp is below its defined limit.
	//! This works well on the EINSY board but not on the miniRAMBo board as
	//! as a disconnected SuperPINDA will show higher temps compared to an EINSY board.
	//! 
	//! This menu allows the user to en-/disable the SuperPINDA manualy
	MENU_ITEM_TOGGLE_P(_N("SuperPINDA"), eeprom_read_byte((uint8_t *)EEPROM_PINDA_TEMP_COMPENSATION) ? _T(MSG_YES) : _T(MSG_NO), lcd_pinda_temp_compensation_toggle);
#endif //PINDA_TEMP_COMP

    MENU_END();
}

static void lcd_settings_menu()
{
	EEPROM_read(EEPROM_SILENT, (uint8_t*)&SilentModeMenu, sizeof(SilentModeMenu));
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));

	MENU_ITEM_SUBMENU_P(_i("Temperature"), lcd_control_temperature_menu);////MSG_TEMPERATURE c=18

	if (!PRINTER_ACTIVE || isPrintPaused)
    {
	    MENU_ITEM_SUBMENU_P(_i("Move axis"), lcd_move_menu_1mm);////MSG_MOVE_AXIS c=18
	    MENU_ITEM_GCODE_P(_i("Disable steppers"), PSTR("M84"));////MSG_DISABLE_STEPPERS c=18
    }

	SETTINGS_FILAMENT_SENSOR;

	SETTINGS_AUTO_DEPLETE;

	SETTINGS_CUTTER;

	MENU_ITEM_TOGGLE_P(_T(MSG_FANS_CHECK), fans_check_enabled ? _T(MSG_ON) : _T(MSG_OFF), lcd_set_fan_check);

	SETTINGS_SILENT_MODE;

    if(!farm_mode)
    {
        bSettings=true;                              // flag ('fake parameter') for 'lcd_hw_setup_menu()' function
        MENU_ITEM_SUBMENU_P(_T(MSG_HW_SETUP), lcd_hw_setup_menu);
    }
    
	SETTINGS_MMU_MODE;

	MENU_ITEM_SUBMENU_P(_T(MSG_MESH_BED_LEVELING), lcd_mesh_bed_leveling_settings);

#if defined (TMC2130) && defined (LINEARITY_CORRECTION)
    MENU_ITEM_SUBMENU_P(_i("Lin. correction"), lcd_settings_linearity_correction_menu);////MSG_LIN_CORRECTION c=18
#endif //LINEARITY_CORRECTION && TMC2130
    if(has_temperature_compensation())
    {
	    MENU_ITEM_TOGGLE_P(_T(MSG_TEMP_CALIBRATION), eeprom_read_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE) ? _T(MSG_ON) : _T(MSG_OFF), lcd_temp_calibration_set);
    }

#ifdef HAS_SECOND_SERIAL_PORT
    MENU_ITEM_TOGGLE_P(_T(MSG_RPI_PORT), (selectedSerialPort == 0) ? _T(MSG_OFF) : _T(MSG_ON), lcd_second_serial_set);
#endif //HAS_SECOND_SERIAL

	if (!isPrintPaused && !homing_flag && !mesh_bed_leveling_flag)
		MENU_ITEM_SUBMENU_P(_T(MSG_BABYSTEP_Z), lcd_babystep_z);

#if (LANG_MODE != 0)
	MENU_ITEM_SUBMENU_P(_i("Select language"), lcd_language_menu);////MSG_LANGUAGE_SELECT c=18
#endif //(LANG_MODE != 0)

	SETTINGS_SD;
	SETTINGS_SOUND;

#ifdef LCD_BL_PIN
    if (backlightSupport)
    {
        MENU_ITEM_SUBMENU_P(_T(MSG_BRIGHTNESS), lcd_backlight_menu);
    }
#endif //LCD_BL_PIN

	if (farm_mode)
	{
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

#ifdef TMC2130
static void lcd_settings_linearity_correction_menu_save()
{
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
    if (changed) tmc2130_init(TMCInitParams(false, FarmOrUserECool()));
}
#endif //TMC2130

static void lcd_calibration_menu()
{
  MENU_BEGIN();
  MENU_ITEM_BACK_P(_T(MSG_MAIN));
  if (!isPrintPaused)
  {
	MENU_ITEM_FUNCTION_P(_i("Wizard"), lcd_wizard);////MSG_WIZARD c=17
    if (lcd_commands_type == LcdCommands::Idle)
    {
         MENU_ITEM_SUBMENU_P(_T(MSG_V2_CALIBRATION), lcd_first_layer_calibration_reset);////MSG_V2_CALIBRATION c=18
    }
	MENU_ITEM_GCODE_P(_T(MSG_AUTO_HOME), PSTR("G28 W"));
#ifdef TMC2130
	MENU_ITEM_FUNCTION_P(_i("Belt test"), lcd_belttest_v);////MSG_BELTTEST c=18
#endif //TMC2130
	MENU_ITEM_FUNCTION_P(_i("Selftest"), lcd_selftest_v);////MSG_SELFTEST c=18
#ifdef MK1BP
    // MK1
    // "Calibrate Z"
    MENU_ITEM_GCODE_P(_T(MSG_HOMEYZ), PSTR("G28 Z"));
#else //MK1BP
    // MK2
    MENU_ITEM_FUNCTION_P(_i("Calibrate XYZ"), lcd_mesh_calibration);////MSG_CALIBRATE_BED c=18
    // "Calibrate Z" with storing the reference values to EEPROM.
    MENU_ITEM_SUBMENU_P(_T(MSG_HOMEYZ), lcd_mesh_calibration_z);
#ifndef SNMM
	//MENU_ITEM_FUNCTION_P(_i("Calibrate E"), lcd_calibrate_extruder);////MSG_CALIBRATE_E c=20
#endif
    // "Mesh Bed Leveling"
    MENU_ITEM_SUBMENU_P(_T(MSG_MESH_BED_LEVELING), lcd_mesh_bedleveling);
	
#endif //MK1BP

    MENU_ITEM_SUBMENU_P(_i("Bed level correct"), lcd_adjust_bed);////MSG_BED_CORRECTION_MENU c=18
	MENU_ITEM_SUBMENU_P(_i("PID calibration"), pid_extruder);////MSG_PID_EXTRUDER c=17
#ifndef TMC2130
    MENU_ITEM_SUBMENU_P(_i("Show end stops"), menu_show_end_stops);////MSG_SHOW_END_STOPS c=18
#endif
#ifndef MK1BP
    MENU_ITEM_GCODE_P(_i("Reset XYZ calibr."), PSTR("M44"));////MSG_CALIBRATE_BED_RESET c=18
#endif //MK1BP
#ifndef SNMM
	//MENU_ITEM_FUNCTION_P(MSG_RESET_CALIBRATE_E, lcd_extr_cal_reset);
#endif
#ifndef MK1BP
    if(has_temperature_compensation())
    {
	    MENU_ITEM_SUBMENU_P(_i("Temp. calibration"), lcd_pinda_calibration_menu);////MSG_CALIBRATION_PINDA_MENU c=17
    }
#endif //MK1BP
  }
  
  MENU_END();
}

void bowden_menu() {
	int enc_dif = lcd_encoder_diff;
	int cursor_pos = 0;
	lcd_clear();
	lcd_putc_at(0, 0, '>');
	for (uint_least8_t i = 0; i < 4; i++) {
		lcd_puts_at_P(1, i, PSTR("Extruder "));
		lcd_print(i);
		lcd_print(": ");
		EEPROM_read_B(EEPROM_BOWDEN_LENGTH + i * 2, &bowden_length[i]);
		lcd_print(bowden_length[i] - 48);

	}
	enc_dif = lcd_encoder_diff;
	lcd_consume_click();
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
					Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
				}

				if (cursor_pos < 0) {
					cursor_pos = 0;
					Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
				}

				lcd_puts_at_P(0, 0, PSTR(" \n \n \n "));
				lcd_putc_at(0, cursor_pos, '>');
				Sound_MakeSound(e_SOUND_TYPE_EncoderMove);
				enc_dif = lcd_encoder_diff;
				_delay(100);
		}

		if (lcd_clicked()) {
			Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
			lcd_clear();
			while (1) {

				manage_heater();
				manage_inactivity(true);

				lcd_puts_at_P(1, 1, PSTR("Extruder "));
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
				_delay(100);
				if (lcd_clicked()) {
					Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
					EEPROM_save_B(EEPROM_BOWDEN_LENGTH + cursor_pos * 2, &bowden_length[cursor_pos]);
					if (lcd_show_fullscreen_message_yes_no_and_wait_P(PSTR("Continue with another bowden?"))) {
						lcd_update_enable(true);
						lcd_clear();
						enc_dif = lcd_encoder_diff;
						lcd_putc_at(0, cursor_pos, '>');
						for (uint_least8_t i = 0; i < 4; i++) {
							lcd_puts_at_P(1, i, PSTR("Extruder "));
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

#ifdef SNMM

static char snmm_stop_print_menu() { //menu for choosing which filaments will be unloaded in stop print
	lcd_clear();
	lcd_puts_at_P(0,0,_T(MSG_UNLOAD_FILAMENT)); lcd_print(':');
	lcd_set_cursor(0, 1); lcd_print('>');
	lcd_puts_at_P(1,2,_i("Used during print"));////MSG_USED c=19
	lcd_puts_at_P(1,3,_i("Current"));////MSG_CURRENT c=19
	char cursor_pos = 1;
	int enc_dif = 0;
	KEEPALIVE_STATE(PAUSED_FOR_USER);
	lcd_consume_click();
	while (1) {
		manage_heater();
		manage_inactivity(true);
		if (abs((enc_dif - lcd_encoder_diff)) > 4) {

			if ((abs(enc_dif - lcd_encoder_diff)) > 1) {
				if (enc_dif > lcd_encoder_diff) cursor_pos--;
				if (enc_dif < lcd_encoder_diff) cursor_pos++;
				if (cursor_pos > 3) {
					cursor_pos = 3;
					Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
				}
				if (cursor_pos < 1){
					cursor_pos = 1;
					Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
				}	

				lcd_puts_at_P(0, 1, PSTR(" \n \n "));
				lcd_set_cursor(0, cursor_pos);
				lcd_print('>');
				enc_dif = lcd_encoder_diff;
				Sound_MakeSound(e_SOUND_TYPE_EncoderMove);
				_delay(100);
			}
		}
		if (lcd_clicked()) {
			Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
			KEEPALIVE_STATE(IN_HANDLER);
			return(cursor_pos - 1);
		}
	}
	
}

#endif //SNMM

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
			Sound_MakeSound(e_SOUND_TYPE_EncoderMove);
		}

		if (cursor_pos > 3)
		{		
            cursor_pos = 3;
            if (first < items_no - 3)
            {
                first++;
                lcd_clear();
            } else { // here we are at the very end of the list
				Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
            }
        }

        if (cursor_pos < 1)
        {
            cursor_pos = 1;
            if (first > 0)
            {
                first--;
                lcd_clear();
            } else { // here we are at the very end of the list
				Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
            }
        }

        if (header) lcd_puts_at_P(0,0,header);

        const bool last_visible = (first == items_no - 3);
        const uint_least8_t ordinary_items = (last_item&&last_visible)?2:3;

        for (uint_least8_t i = 0; i < ordinary_items; i++)
        {
            if (item) lcd_puts_at_P(1, i + 1, item);
        }

        for (uint_least8_t i = 0; i < ordinary_items; i++)
        {
            lcd_set_cursor(2 + item_len, i+1);
            lcd_print(first + i + 1);
        }

        if (last_item&&last_visible) lcd_puts_at_P(1, 3, last_item);

        lcd_puts_at_P(0, 1, PSTR(" \n \n "));
        lcd_putc_at(0, cursor_pos, '>');
        _delay(100);

		if (lcd_clicked())
		{
			Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
		    KEEPALIVE_STATE(IN_HANDLER);
			lcd_encoder_diff = 0;
			return(cursor_pos + first - 1);
		}
	}
}

char reset_menu() {
    const uint8_t items_no =
#ifdef SNMM
        6;
#else
        5;
#endif
    static int8_t first = 0;
    int8_t enc_dif = 0;
	char cursor_pos = 0;

    const char *const item[items_no] = {PSTR("Language"), PSTR("Statistics"), PSTR("Shipping prep"), PSTR("Service prep"), PSTR("All Data")
#ifdef SNMM
    , PSTR("Bowden length")
#endif
    };
	
	enc_dif = lcd_encoder_diff;
	lcd_clear();
	lcd_set_cursor(0, 0);
    lcd_putc('>');
	lcd_consume_click();
	while (1) {		

		for (uint_least8_t i = 0; i < 4; i++) {
			lcd_puts_at_P(1, i, item[first + i]);
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
					Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
					if (first < items_no - 4) {
						first++;
						lcd_clear();
					}
				}

				if (cursor_pos < 0) {
					cursor_pos = 0;
					Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
					if (first > 0) {
						first--;
						lcd_clear();
					}
				}
				lcd_puts_at_P(0, 0, PSTR(" \n \n \n "));
				lcd_set_cursor(0, cursor_pos);
				lcd_putc('>');
				Sound_MakeSound(e_SOUND_TYPE_EncoderMove);
				enc_dif = lcd_encoder_diff;
				_delay(100);
			}

		}

		if (lcd_clicked()) {
			Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
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
    MENU_ITEM_FUNCTION_P(_i("Load all"), load_all); ////MSG_LOAD_ALL c=17
    MENU_ITEM_FUNCTION_NR_P(_T(MSG_LOAD_FILAMENT), '1', extr_adj, 0); ////MSG_LOAD_FILAMENT_1 c=16
    MENU_ITEM_FUNCTION_NR_P(_T(MSG_LOAD_FILAMENT), '2', extr_adj, 1); ////MSG_LOAD_FILAMENT_2 c=17
    MENU_ITEM_FUNCTION_NR_P(_T(MSG_LOAD_FILAMENT), '3', extr_adj, 2); ////MSG_LOAD_FILAMENT_3 c=17
    MENU_ITEM_FUNCTION_NR_P(_T(MSG_LOAD_FILAMENT), '4', extr_adj, 3); ////MSG_LOAD_FILAMENT_4 c=17

    if (mmu_enabled)
    {
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_LOAD_FILAMENT), '5', extr_adj, 4);
    }
    MENU_END();
}

static void mmu_load_to_nozzle_menu()
{
    if (bFilamentAction)
    {
        MENU_BEGIN();
        MENU_ITEM_BACK_P(_T(MSG_MAIN));
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_LOAD_FILAMENT), '1', lcd_mmu_load_to_nozzle, 0);
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_LOAD_FILAMENT), '2', lcd_mmu_load_to_nozzle, 1);
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_LOAD_FILAMENT), '3', lcd_mmu_load_to_nozzle, 2);
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_LOAD_FILAMENT), '4', lcd_mmu_load_to_nozzle, 3);
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_LOAD_FILAMENT), '5', lcd_mmu_load_to_nozzle, 4);
        MENU_END();
    }
    else
    {
        eFilamentAction = FilamentAction::MmuLoad;
        preheat_or_continue();
    }
}

static void mmu_eject_filament(uint8_t filament)
{
    menu_back();
    mmu_eject_filament(filament, true);
}

static void mmu_fil_eject_menu()
{
    if (bFilamentAction)
    {
        MENU_BEGIN();
        MENU_ITEM_BACK_P(_T(MSG_MAIN));
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_EJECT_FILAMENT), '1', mmu_eject_filament, 0);
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_EJECT_FILAMENT), '2', mmu_eject_filament, 1);
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_EJECT_FILAMENT), '3', mmu_eject_filament, 2);
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_EJECT_FILAMENT), '4', mmu_eject_filament, 3);
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_EJECT_FILAMENT), '5', mmu_eject_filament, 4);
        MENU_END();
    }
    else
    {
        eFilamentAction = FilamentAction::MmuEject;
        preheat_or_continue();
    }
}

#ifdef MMU_HAS_CUTTER

static void mmu_cut_filament_menu()
{
    if(bFilamentAction)
    {
        MENU_BEGIN();
        MENU_ITEM_BACK_P(_T(MSG_MAIN));
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_CUT_FILAMENT), '1', mmu_cut_filament, 0);
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_CUT_FILAMENT), '2', mmu_cut_filament, 1);
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_CUT_FILAMENT), '3', mmu_cut_filament, 2);
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_CUT_FILAMENT), '4', mmu_cut_filament, 3);
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_CUT_FILAMENT), '5', mmu_cut_filament, 4);
        MENU_END();
    }
    else
    {
        eFilamentAction=FilamentAction::MmuCut;
        bFilamentFirstRun=false;
        if(target_temperature[0]>=EXTRUDE_MINTEMP)
        {
            bFilamentPreheatState=true;
            mFilamentItem(target_temperature[0],target_temperature_bed);
        }
        else lcd_generic_preheat_menu();
    }
}
#endif //MMU_HAS_CUTTER

#ifdef SNMM
static void fil_unload_menu()
{
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));
	MENU_ITEM_FUNCTION_P(_i("Unload all"), extr_unload_all);////MSG_UNLOAD_ALL c=17
	MENU_ITEM_FUNCTION_P(_i("Unload filament 1"), extr_unload_0);////MSG_UNLOAD_FILAMENT_1 c=17
	MENU_ITEM_FUNCTION_P(_i("Unload filament 2"), extr_unload_1);////MSG_UNLOAD_FILAMENT_2 c=17
	MENU_ITEM_FUNCTION_P(_i("Unload filament 3"), extr_unload_2);////MSG_UNLOAD_FILAMENT_3 c=17
	MENU_ITEM_FUNCTION_P(_i("Unload filament 4"), extr_unload_3);////MSG_UNLOAD_FILAMENT_4 c=17

	if (mmu_enabled)
		MENU_ITEM_FUNCTION_P(_i("Unload filament 5"), extr_unload_4);////MSG_UNLOAD_FILAMENT_5 c=17

	MENU_END();
}


static void change_extr_menu(){
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));
	MENU_ITEM_FUNCTION_P(_i("Extruder 1"), extr_change_0);////MSG_EXTRUDER_1 c=17
	MENU_ITEM_FUNCTION_P(_i("Extruder 2"), extr_change_1);////MSG_EXTRUDER_2 c=17
	MENU_ITEM_FUNCTION_P(_i("Extruder 3"), extr_change_2);////MSG_EXTRUDER_3 c=17
	MENU_ITEM_FUNCTION_P(_i("Extruder 4"), extr_change_3);////MSG_EXTRUDER_4 c=17

	MENU_END();
}
#endif //SNMM

// unload filament for single material printer (used in M702 gcode)
// @param automatic: If true, unload_filament is part of a unload+load sequence (M600)
void unload_filament(bool automatic)
{
	custom_message_type = CustomMsg::FilamentLoading;
	lcd_setstatuspgm(_T(MSG_UNLOADING_FILAMENT));

    raise_z_above(automatic? MIN_Z_FOR_SWAP: MIN_Z_FOR_UNLOAD);

	//		extr_unload2();

	current_position[E_AXIS] -= 45;
	plan_buffer_line_curposXYZE(5200 / 60);
	st_synchronize();
	current_position[E_AXIS] -= 15;
	plan_buffer_line_curposXYZE(1000 / 60);
	st_synchronize();
	current_position[E_AXIS] -= 20;
	plan_buffer_line_curposXYZE(1000 / 60);
	st_synchronize();

	lcd_display_message_fullscreen_P(_T(MSG_PULL_OUT_FILAMENT));

	//disable extruder steppers so filament can be removed
	disable_e0();
	disable_e1();
	disable_e2();
	_delay(100);

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
	custom_message_type = CustomMsg::Status;

}

#include "xflash.h"

#ifdef LCD_TEST
static void lcd_test_menu()
{
	XFLASH_SPI_ENTER();
	xflash_enable_wr();
	xflash_chip_erase();
	xflash_disable_wr();
}
#endif //LCD_TEST

static bool fan_error_selftest()
{
#ifdef FANCHECK
    if (!fans_check_enabled) return 0;

    fanSpeed = 255;
#ifdef FAN_SOFT_PWM
	fanSpeedSoftPwm = 255;
#endif //FAN_SOFT_PWM
    manage_heater(); //enables print fan
    setExtruderAutoFanState(3); //force enables the extruder fan
#ifdef FAN_SOFT_PWM
    extruder_autofan_last_check = _millis();
    fan_measuring = true;
#endif //FAN_SOFT_PWM
    _delay(1000); //delay_keep_alive would turn off extruder fan, because temerature is too low (maybe)
    manage_heater();
    fanSpeed = 0;
	setExtruderAutoFanState(1); //releases lock on the extruder fan
#ifdef FAN_SOFT_PWM
    fanSpeedSoftPwm = 0;
#endif //FAN_SOFT_PWM
    manage_heater();
#ifdef TACH_0
    if (fan_speed[0] <= 20) { //extruder fan error
        LCD_ALERTMESSAGERPGM(MSG_FANCHECK_EXTRUDER);
        return 1;
    }
#endif
#ifdef TACH_1
    if (fan_speed[1] <= 20) { //print fan error
        LCD_ALERTMESSAGERPGM(MSG_FANCHECK_PRINT);
        return 1;
    }
#endif
#endif //FANCHECK
    return 0;
}

//! @brief Resume paused print, send host action "resumed"
//! @todo It is not good to call restore_print_from_ram_and_continue() from function called by lcd_update(),
//! as restore_print_from_ram_and_continue() calls lcd_update() internally.
void lcd_resume_print()
{
    lcd_return_to_status();
    lcd_reset_alert_level(); //for fan speed error
    if (fan_error_selftest()) {
        if (is_usb_printing) SERIAL_PROTOCOLLNRPGM(MSG_OCTOPRINT_PAUSED);
        return; //abort if error persists
    }
    cmdqueue_serial_disabled = false;
    lcd_setstatuspgm(_T(MSG_FINISHING_MOVEMENTS));
    st_synchronize();
    custom_message_type = CustomMsg::Resuming;
    isPrintPaused = false;
    restore_print_from_ram_and_continue(default_retraction);
    pause_time += (_millis() - start_pause_print); //accumulate time when print is paused for correct statistics calculation
    refresh_cmd_timeout();
    SERIAL_PROTOCOLLNRPGM(MSG_OCTOPRINT_RESUMED); //resume octoprint
    custom_message_type = CustomMsg::Status;
}

//! @brief Resume paused USB/host print, send host action "resume"
void lcd_resume_usb_print()
{
    SERIAL_PROTOCOLLNRPGM(MSG_OCTOPRINT_RESUME); //resume octoprint
}

static void change_sheet()
{
	eeprom_update_byte(&(EEPROM_Sheets_base->active_sheet), selected_sheet);
    menu_back(3);
}



static void lcd_rename_sheet_menu()
{
    struct MenuData
    {
        bool initialized;
        uint8_t selected;
        char name[sizeof(Sheet::name)];
    };
    static_assert(sizeof(menu_data)>= sizeof(MenuData),"MenuData doesn't fit into menu_data");
    MenuData* menuData = (MenuData*)&(menu_data[0]);

    if (!menuData->initialized)
    {
        eeprom_read_block(menuData->name, EEPROM_Sheets_base->s[selected_sheet].name, sizeof(Sheet::name));
        lcd_encoder = menuData->name[0];
        menuData->initialized = true;
    }
    if (lcd_encoder < '\x20') lcd_encoder = '\x20';
    if (lcd_encoder > '\x7F') lcd_encoder = '\x7F';

    menuData->name[menuData->selected] = lcd_encoder;
    lcd_set_cursor(0,0);
    for (uint_least8_t i = 0; i < sizeof(Sheet::name); ++i)
    {
        lcd_putc(menuData->name[i]);
    }
    lcd_set_cursor(menuData->selected, 1);
    lcd_putc('^');
    if (lcd_clicked())
    {
        if ((menuData->selected + 1u) < sizeof(Sheet::name))
        {
            lcd_encoder = menuData->name[++(menuData->selected)];
        }
        else
        {
            eeprom_update_block(menuData->name,
                EEPROM_Sheets_base->s[selected_sheet].name,
                sizeof(Sheet::name));
            menu_back();
        }
    }
}

static void lcd_reset_sheet()
{
    SheetName sheetName;
    eeprom_default_sheet_name(selected_sheet, sheetName);
	eeprom_update_word(reinterpret_cast<uint16_t *>(&(EEPROM_Sheets_base->s[selected_sheet].z_offset)),EEPROM_EMPTY_VALUE16);
	eeprom_update_block(sheetName.c,EEPROM_Sheets_base->s[selected_sheet].name,sizeof(Sheet::name));
	if (selected_sheet == eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet)))
	{
        eeprom_switch_to_next_sheet();
        if((-1 == eeprom_next_initialized_sheet(0)) && (CALIBRATION_STATUS_CALIBRATED == calibration_status()))
        {
            calibration_status_store(CALIBRATION_STATUS_LIVE_ADJUST);
        }
	}

	menu_back();
}

//! @brief Activate selected_sheet and run first layer calibration
static void activate_calibrate_sheet()
{
    eeprom_update_byte(&(EEPROM_Sheets_base->active_sheet), selected_sheet);
    lcd_first_layer_calibration_reset();
}

static void lcd_sheet_menu()
{
    MENU_BEGIN();
    MENU_ITEM_BACK_P(_T(MSG_STEEL_SHEETS));

	if(eeprom_is_sheet_initialized(selected_sheet)){
	    MENU_ITEM_SUBMENU_P(_i("Select"), change_sheet); ////MSG_SELECT c=18
	}

    if (lcd_commands_type == LcdCommands::Idle)
    {
        MENU_ITEM_SUBMENU_P(_T(MSG_V2_CALIBRATION), activate_calibrate_sheet);////MSG_V2_CALIBRATION c=18
    }
    MENU_ITEM_SUBMENU_P(_i("Rename"), lcd_rename_sheet_menu); ////MSG_RENAME c=18
	MENU_ITEM_FUNCTION_P(_T(MSG_RESET), lcd_reset_sheet); ////MSG_RESET c=14

    MENU_END();
}

//! @brief Show Main Menu
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! | Info screen        | allways
//!
//! | tst - Save         | ifdef RESUME_DEBUG
//! | tst - Restore      | ifdef RESUME_DEBUG
//!
//! | recover print      | ifdef TMC2130_DEBUG
//! | power panic        | ifdef TMC2130_DEBUG
//! 
//! | Live adjust Z      | printing + Z low 
//!
//! | Change filament    | farm mode
//!
//! | Tune               | printing + paused
//! | Pause print        | printing + not paused
//! | Resume print       | printing + paused
//! | Stop print         | printing or paused + NOT MBL
//! | Preheat            | not printing + not paused
//! | Print from SD      | not printing or paused
//!
//! | Switch sheet       | farm mode
//!
//! | AutoLoad filament  | not printing + not mmu or paused
//! | Load filament      | not printing + mmu or paused
//! | Load to nozzle      | not printing + mmu or paused
//! | Unload filament    | not printing or paused
//! | Eject filament     | not printing + mmu or paused
//! | Cut filament       | not printing + mmu or paused + cut atctive
//! | Settings           | not printing or paused
//! | Calibration        | not printing 
//! | Statistics         | not printing
//! | Fail stats         | allways
//! | Fail stats MMU     | mmu
//! | Support            | allways
//! @endcode
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

    if ( ( IS_SD_PRINTING || is_usb_printing || (lcd_commands_type == LcdCommands::Layer1Cal)) && (current_position[Z_AXIS] < Z_HEIGHT_HIDE_LIVE_ADJUST_MENU) && !homing_flag && !mesh_bed_leveling_flag) {
        MENU_ITEM_SUBMENU_P(_T(MSG_BABYSTEP_Z), lcd_babystep_z);//8
    }

    if (farm_mode)
        MENU_ITEM_FUNCTION_P(_T(MSG_FILAMENTCHANGE), lcd_colorprint_change);//8

    if ( moves_planned() || PRINTER_ACTIVE ) {
        MENU_ITEM_SUBMENU_P(_i("Tune"), lcd_tune_menu);////MSG_TUNE c=18
    } else {
        MENU_ITEM_SUBMENU_P(_i("Preheat"), lcd_preheat_menu);////MSG_PREHEAT c=18
    }

    if (mesh_bed_leveling_flag == false && homing_flag == false && !isPrintPaused) {
        if (is_usb_printing) {
            MENU_ITEM_FUNCTION_P(_T(MSG_PAUSE_PRINT), lcd_pause_usb_print);////MSG_PAUSE_PRINT c=18
        } else if (IS_SD_PRINTING) {
            MENU_ITEM_FUNCTION_P(_T(MSG_PAUSE_PRINT), lcd_pause_print);////MSG_PAUSE_PRINT c=18
        }
    }
    if(isPrintPaused)
    {
#ifdef FANCHECK
        if((fan_check_error == EFCE_FIXED) || (fan_check_error == EFCE_OK))
#endif //FANCHECK
        {
            if (is_usb_printing) {
                MENU_ITEM_SUBMENU_P(_T(MSG_RESUME_PRINT), lcd_resume_usb_print);////MSG_RESUME_PRINT c=18
            } else {
                MENU_ITEM_SUBMENU_P(_T(MSG_RESUME_PRINT), lcd_resume_print);////MSG_RESUME_PRINT c=18
            }
        }
    }
    if((IS_SD_PRINTING || is_usb_printing || isPrintPaused) && (custom_message_type != CustomMsg::MeshBedLeveling)) {
        MENU_ITEM_SUBMENU_P(_T(MSG_STOP_PRINT), lcd_sdcard_stop);
    }
#ifdef SDSUPPORT //!@todo SDSUPPORT undefined creates several issues in source code
    if (card.cardOK || lcd_commands_type == LcdCommands::Layer1Cal) {
        if (!card.isFileOpen()) {
            if (!is_usb_printing && (lcd_commands_type != LcdCommands::Layer1Cal)) {
            //if (farm_mode) MENU_ITEM_SUBMENU_P(MSG_FARM_CARD_MENU, lcd_farm_sdcard_menu);
            /*else*/{
                        bMain=true;               // flag ('fake parameter') for 'lcd_sdcard_menu()' function
                        MENU_ITEM_SUBMENU_P(_T(MSG_CARD_MENU), lcd_sdcard_menu);
                    }
        }
#if SDCARDDETECT < 1
        MENU_ITEM_GCODE_P(_i("Change SD card"), PSTR("M21"));  // SD-card changed by user////MSG_CNG_SDCARD
#endif //SDCARDDETECT
        }
    } else {
        bMain=true;                                   // flag (i.e. 'fake parameter') for 'lcd_sdcard_menu()' function
        MENU_ITEM_SUBMENU_P(_i("No SD card"), lcd_sdcard_menu);////MSG_NO_CARD c=18
#if SDCARDDETECT < 1
        MENU_ITEM_GCODE_P(_i("Init. SD card"), PSTR("M21")); // Manually initialize the SD-card via user interface////MSG_INIT_SDCARD
#endif //SDCARDDETECT
    }
#endif //SDSUPPORT

    if(!isPrintPaused && !IS_SD_PRINTING && !is_usb_printing && (lcd_commands_type != LcdCommands::Layer1Cal)) {
        if (!farm_mode) {
            const int8_t sheet = eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet));
            const int8_t nextSheet = eeprom_next_initialized_sheet(sheet);
            if ((nextSheet >= 0) && (sheet != nextSheet)) { // show menu only if we have 2 or more sheets initialized
                MENU_ITEM_FUNCTION_E(EEPROM_Sheets_base->s[sheet], eeprom_switch_to_next_sheet);
            }
        }
    }

    if ( ! ( IS_SD_PRINTING || is_usb_printing || (lcd_commands_type == LcdCommands::Layer1Cal) ) ) {
        if (mmu_enabled) {
            MENU_ITEM_SUBMENU_P(_T(MSG_LOAD_FILAMENT), fil_load_menu);
            MENU_ITEM_SUBMENU_P(_i("Load to nozzle"), mmu_load_to_nozzle_menu);////MSG_LOAD_TO_NOZZLE c=18
//-//          MENU_ITEM_FUNCTION_P(_T(MSG_UNLOAD_FILAMENT), extr_unload);
//bFilamentFirstRun=true;
            MENU_ITEM_SUBMENU_P(_T(MSG_UNLOAD_FILAMENT), mmu_unload_filament);
            MENU_ITEM_SUBMENU_P(_T(MSG_EJECT_FILAMENT), mmu_fil_eject_menu);
#ifdef  MMU_HAS_CUTTER
            MENU_ITEM_SUBMENU_P(_T(MSG_CUT_FILAMENT), mmu_cut_filament_menu);
#endif //MMU_HAS_CUTTER
        } else {
#ifdef SNMM
            MENU_ITEM_SUBMENU_P(_T(MSG_UNLOAD_FILAMENT), fil_unload_menu);
            MENU_ITEM_SUBMENU_P(_i("Change extruder"), change_extr_menu);////MSG_CHANGE_EXTR c=20
#endif
#ifdef FILAMENT_SENSOR
            if ((fsensor_autoload_enabled == true) && (fsensor_enabled == true) && (mmu_enabled == false))
                MENU_ITEM_SUBMENU_P(_i("AutoLoad filament"), lcd_menu_AutoLoadFilament);////MSG_AUTOLOAD_FILAMENT c=18
            else
#endif //FILAMENT_SENSOR
            {
                bFilamentFirstRun=true;
                MENU_ITEM_SUBMENU_P(_T(MSG_LOAD_FILAMENT), lcd_LoadFilament);
            }
            bFilamentFirstRun=true;
            MENU_ITEM_SUBMENU_P(_T(MSG_UNLOAD_FILAMENT), lcd_unLoadFilament);
        }
    MENU_ITEM_SUBMENU_P(_T(MSG_SETTINGS), lcd_settings_menu);
    if(!isPrintPaused) MENU_ITEM_SUBMENU_P(_T(MSG_MENU_CALIBRATION), lcd_calibration_menu);
    }

    if (!is_usb_printing && (lcd_commands_type != LcdCommands::Layer1Cal)) {
        MENU_ITEM_SUBMENU_P(_i("Statistics"), lcd_menu_statistics);////MSG_STATISTICS c=18
    }

#if defined(TMC2130) || defined(FILAMENT_SENSOR)
    MENU_ITEM_SUBMENU_P(_i("Fail stats"), lcd_menu_fails_stats);////MSG_FAIL_STATS c=18
#endif
    if (mmu_enabled) {
        MENU_ITEM_SUBMENU_P(_i("Fail stats MMU"), lcd_menu_fails_stats_mmu);////MSG_MMU_FAIL_STATS c=18
    }
    MENU_ITEM_SUBMENU_P(_i("Support"), lcd_support_menu);////MSG_SUPPORT c=18
#ifdef LCD_TEST
    MENU_ITEM_SUBMENU_P(_i("XFLASH init"), lcd_test_menu);////MSG_SUPPORT
#endif //LCD_TEST

    MENU_END();

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

	custom_message_type = CustomMsg::FilamentLoading; //just print status message
	lcd_setstatuspgm(_T(MSG_FINISHING_MOVEMENTS));
	lcd_return_to_status();
	lcd_draw_update = 3;
}


#ifdef LA_LIVE_K
// @wavexx: looks like there's no generic float editing function in menu.cpp so we
//          redefine our custom handling functions to mimick other tunables
const char menu_fmt_float13off[] PROGMEM = "%c%-13.13S%6.6S";

static void lcd_advance_draw_K(char chr, float val)
{
    if (val <= 0)
        lcd_printf_P(menu_fmt_float13off, chr, MSG_ADVANCE_K, _T(MSG_OFF));
    else
        lcd_printf_P(menu_fmt_float13, chr, MSG_ADVANCE_K, val);
}

static void lcd_advance_edit_K(void)
{
    if (lcd_draw_update)
    {
        if (lcd_encoder < 0) lcd_encoder = 0;
        if (lcd_encoder > 999) lcd_encoder = 999;
        lcd_set_cursor(0, 1);
        lcd_advance_draw_K(' ', 0.01 * lcd_encoder);
    }
    if (LCD_CLICKED)
    {
        extruder_advance_K = 0.01 * lcd_encoder;
        menu_back_no_reset();
    }
}

static uint8_t lcd_advance_K()
{
    if (menu_item == menu_line)
    {
        if (lcd_draw_update)
        {
            lcd_set_cursor(0, menu_row);
            lcd_advance_draw_K((lcd_encoder == menu_item)?'>':' ', extruder_advance_K);
        }
        if (menu_clicked && (lcd_encoder == menu_item))
        {
            menu_submenu_no_reset(lcd_advance_edit_K);
            lcd_encoder = 100. * extruder_advance_K;
            return menu_item_ret();
        }
    }
    menu_item++;
    return 0;
}

#define MENU_ITEM_EDIT_advance_K() do { if (lcd_advance_K()) return; } while (0)
#endif


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
	MENU_ITEM_EDIT_int3_P(_i("Speed"), &feedmultiply, 10, 999);//2////MSG_SPEED c=15

	MENU_ITEM_EDIT_int3_P(_T(MSG_NOZZLE), &target_temperature[0], 0, HEATER_0_MAXTEMP - 10);//3
	MENU_ITEM_EDIT_int3_P(_T(MSG_BED), &target_temperature_bed, 0, BED_MAXTEMP - 10);//4

	MENU_ITEM_EDIT_int3_P(_T(MSG_FAN_SPEED), &fanSpeed, 0, 255);//5
	MENU_ITEM_EDIT_int3_P(_i("Flow"), &extrudemultiply, 10, 999);//6////MSG_FLOW c=15
#ifdef LA_LIVE_K
	MENU_ITEM_EDIT_advance_K();//7
#endif
#ifdef FILAMENTCHANGEENABLE
    if (!farm_mode)
        MENU_ITEM_FUNCTION_P(_T(MSG_FILAMENTCHANGE), lcd_colorprint_change);//8
#endif

#ifdef FILAMENT_SENSOR
	if (FSensorStateMenu == 0) {
          if (fsensor_not_responding && (mmu_enabled == false)) {
               /* Filament sensor not working*/
               MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR), _T(MSG_NA), lcd_fsensor_state_set);
          }
          else {
               /* Filament sensor turned off, working, no problems*/
               MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR), _T(MSG_OFF), lcd_fsensor_state_set);
          }
	}
	else {
		MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR), _T(MSG_ON), lcd_fsensor_state_set);
	}
#ifdef IR_SENSOR_ANALOG
     FSENSOR_ACTION_NA;
#endif //IR_SENSOR_ANALOG
#endif //FILAMENT_SENSOR

	SETTINGS_AUTO_DEPLETE;

	SETTINGS_CUTTER;

	MENU_ITEM_TOGGLE_P(_T(MSG_FANS_CHECK), fans_check_enabled ? _T(MSG_ON) : _T(MSG_OFF), lcd_set_fan_check);


#ifdef TMC2130
     if(!farm_mode)
     {
          if (SilentModeMenu == SILENT_MODE_NORMAL) MENU_ITEM_TOGGLE_P(_T(MSG_MODE), _T(MSG_NORMAL), lcd_silent_mode_set);
          else MENU_ITEM_TOGGLE_P(_T(MSG_MODE), _T(MSG_STEALTH), lcd_silent_mode_set);

          if (SilentModeMenu == SILENT_MODE_NORMAL)
          {
               if (lcd_crash_detect_enabled()) MENU_ITEM_TOGGLE_P(_T(MSG_CRASHDETECT), _T(MSG_ON), crash_mode_switch);
               else MENU_ITEM_TOGGLE_P(_T(MSG_CRASHDETECT), _T(MSG_OFF), crash_mode_switch);
          }
          else MENU_ITEM_TOGGLE_P(_T(MSG_CRASHDETECT), NULL, lcd_crash_mode_info);
     }
#else //TMC2130
	if (!farm_mode) { //dont show in menu if we are in farm mode
		switch (SilentModeMenu) {
		case SILENT_MODE_POWER: MENU_ITEM_TOGGLE_P(_T(MSG_MODE), _T(MSG_HIGH_POWER), lcd_silent_mode_set); break;
		case SILENT_MODE_SILENT: MENU_ITEM_TOGGLE_P(_T(MSG_MODE), _T(MSG_SILENT), lcd_silent_mode_set); break;
		case SILENT_MODE_AUTO: MENU_ITEM_TOGGLE_P(_T(MSG_MODE), _T(MSG_AUTO_POWER), lcd_silent_mode_set); break;
		default: MENU_ITEM_TOGGLE_P(_T(MSG_MODE), _T(MSG_HIGH_POWER), lcd_silent_mode_set); break; // (probably) not needed
		}
	}
#endif //TMC2130
	SETTINGS_MMU_MODE;
    SETTINGS_SOUND;
#ifdef LCD_BL_PIN
    if (backlightSupport)
    {
        MENU_ITEM_SUBMENU_P(_T(MSG_BRIGHTNESS), lcd_backlight_menu);
    }
#endif //LCD_BL_PIN
	MENU_END();
}

static void mbl_magnets_elimination_toggle() {
	bool magnet_elimination = (eeprom_read_byte((uint8_t*)EEPROM_MBL_MAGNET_ELIMINATION) > 0);
	magnet_elimination = !magnet_elimination;
	eeprom_update_byte((uint8_t*)EEPROM_MBL_MAGNET_ELIMINATION, (uint8_t)magnet_elimination);
}

static void mbl_mesh_toggle() {
	uint8_t mesh_nr = eeprom_read_byte((uint8_t*)EEPROM_MBL_POINTS_NR);
	if(mesh_nr == 3) mesh_nr = 7;
	else mesh_nr = 3;
	eeprom_update_byte((uint8_t*)EEPROM_MBL_POINTS_NR, mesh_nr);
}

static void mbl_probe_nr_toggle() {
	mbl_z_probe_nr = eeprom_read_byte((uint8_t*)EEPROM_MBL_PROBE_NR);
	switch (mbl_z_probe_nr) {
		case 1: mbl_z_probe_nr = 3; break;
		case 3: mbl_z_probe_nr = 5; break;
		case 5: mbl_z_probe_nr = 1; break;
		default: mbl_z_probe_nr = 3; break;
	}
	eeprom_update_byte((uint8_t*)EEPROM_MBL_PROBE_NR, mbl_z_probe_nr);
}

static void lcd_mesh_bed_leveling_settings()
{
	
	bool magnet_elimination = (eeprom_read_byte((uint8_t*)EEPROM_MBL_MAGNET_ELIMINATION) > 0);
	uint8_t points_nr = eeprom_read_byte((uint8_t*)EEPROM_MBL_POINTS_NR);
	char sToggle[4]; //enough for nxn format

	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_SETTINGS));
	sToggle[0] = points_nr + '0';
	sToggle[1] = 'x';
	sToggle[2] = points_nr + '0';
	sToggle[3] = 0;
	MENU_ITEM_TOGGLE(_T(MSG_MESH), sToggle, mbl_mesh_toggle);
	sToggle[0] = mbl_z_probe_nr + '0';
	sToggle[1] = 0;
	MENU_ITEM_TOGGLE(_T(MSG_Z_PROBE_NR), sToggle, mbl_probe_nr_toggle);
	MENU_ITEM_TOGGLE_P(_T(MSG_MAGNETS_COMP), (points_nr == 7) ? (magnet_elimination ? _T(MSG_ON): _T(MSG_OFF)) : _T(MSG_NA), mbl_magnets_elimination_toggle);
	MENU_END();
	//SETTINGS_MBL_MODE;
}

#ifdef LCD_BL_PIN
static void backlight_mode_toggle()
{
    switch (backlightMode)
    {
        case BACKLIGHT_MODE_BRIGHT: backlightMode = BACKLIGHT_MODE_DIM; break;
        case BACKLIGHT_MODE_DIM: backlightMode = BACKLIGHT_MODE_AUTO; break;
        case BACKLIGHT_MODE_AUTO: backlightMode = BACKLIGHT_MODE_BRIGHT; break;
        default: backlightMode = BACKLIGHT_MODE_BRIGHT; break;
    }
    backlight_save();
}

static void lcd_backlight_menu()
{
    MENU_BEGIN();
    ON_MENU_LEAVE(
        backlight_save();
    );
    
    MENU_ITEM_BACK_P(_T(MSG_BACK));
    MENU_ITEM_EDIT_int3_P(_T(MSG_BL_HIGH), &backlightLevel_HIGH, backlightLevel_LOW, 255);
    MENU_ITEM_EDIT_int3_P(_T(MSG_BL_LOW), &backlightLevel_LOW, 0, backlightLevel_HIGH);
	MENU_ITEM_TOGGLE_P(_T(MSG_MODE), ((backlightMode==BACKLIGHT_MODE_BRIGHT) ? _T(MSG_BRIGHT) : ((backlightMode==BACKLIGHT_MODE_DIM) ? _T(MSG_DIM) : _T(MSG_AUTO))), backlight_mode_toggle);
    MENU_ITEM_EDIT_int3_P(_T(MSG_TIMEOUT), &backlightTimer_period, 1, 999);
    
    MENU_END();
}
#endif //LCD_BL_PIN

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
  MENU_ITEM_EDIT_int3_P(_i("Nozzle2"), &target_temperature[1], 0, HEATER_1_MAXTEMP - 10);////MSG_NOZZLE1
#endif
#if TEMP_SENSOR_2 != 0
  MENU_ITEM_EDIT_int3_P(_i("Nozzle3"), &target_temperature[2], 0, HEATER_2_MAXTEMP - 10);////MSG_NOZZLE2
#endif
#if TEMP_SENSOR_BED != 0
  MENU_ITEM_EDIT_int3_P(_T(MSG_BED), &target_temperature_bed, 0, BED_MAXTEMP - 3);
#endif
  MENU_ITEM_EDIT_int3_P(_T(MSG_FAN_SPEED), &fanSpeed, 0, 255);
#if defined AUTOTEMP && (TEMP_SENSOR_0 != 0)
//MENU_ITEM_EDIT removed, following code must be redesigned if AUTOTEMP enabled
  MENU_ITEM_EDIT(bool, MSG_AUTOTEMP, &autotemp_enabled);
  MENU_ITEM_EDIT(float3, _i(" \002 Min"), &autotemp_min, 0, HEATER_0_MAXTEMP - 10);////MSG_MIN
  MENU_ITEM_EDIT(float3, _i(" \002 Max"), &autotemp_max, 0, HEATER_0_MAXTEMP - 10);////MSG_MAX
  MENU_ITEM_EDIT(float32, _i(" \002 Fact"), &autotemp_factor, 0.0, 1.0);////MSG_FACTOR
#endif

  MENU_END();
}



static void lcd_sd_refresh()
{
#if SDCARDDETECT == -1
  card.initsd();
#else
  card.presort();
#endif
  menu_top = 0;
  lcd_encoder = 0;
  menu_data_reset(); //Forces reloading of cached variables.
}

static void lcd_sd_updir()
{
  card.updir();
  menu_top = 0;
  lcd_encoder = 0;
  menu_data_reset(); //Forces reloading of cached variables.
}

void lcd_print_stop()
{
    if (!card.sdprinting) {
        SERIAL_ECHOLNRPGM(MSG_OCTOPRINT_CANCEL);   // for Octoprint
    }
    cmdqueue_serial_disabled = false; //for when canceling a print with a fancheck

    CRITICAL_SECTION_START;

    // Clear any saved printing state
    cancel_saved_printing();

    // Abort the planner/queue/sd
    planner_abort_hard();
	cmdqueue_reset();
	card.sdprinting = false;
	card.closefile();
    st_reset_timer();

    CRITICAL_SECTION_END;

#ifdef MESH_BED_LEVELING
    mbl.active = false; //also prevents undoing the mbl compensation a second time in the second planner_abort_hard()
#endif

	lcd_setstatuspgm(_T(MSG_PRINT_ABORTED));
	stoptime = _millis();
	unsigned long t = (stoptime - starttime - pause_time) / 1000; //time in s
	pause_time = 0;
	save_statistics(total_filament_used, t);

    lcd_commands_step = 0;
    lcd_commands_type = LcdCommands::Idle;

    lcd_cooldown(); //turns off heaters and fan; goes to status screen.
    cancel_heatup = true; //unroll temperature wait loop stack.

    current_position[Z_AXIS] += 10; //lift Z.
    plan_buffer_line_curposXYZE(manual_feedrate[Z_AXIS] / 60);

    if (axis_known_position[X_AXIS] && axis_known_position[Y_AXIS]) //if axis are homed, move to parked position.
    {
        current_position[X_AXIS] = X_CANCEL_POS;
        current_position[Y_AXIS] = Y_CANCEL_POS;
        plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
    }
    st_synchronize();

    if (mmu_enabled) extr_unload(); //M702 C

    finishAndDisableSteppers(); //M84

    lcd_setstatuspgm(_T(WELCOME_MSG));
    custom_message_type = CustomMsg::Status;

    planner_abort_hard(); //needs to be done since plan_buffer_line resets waiting_inside_plan_buffer_line_print_aborted to false. Also copies current to destination.
    
    axis_relative_modes = E_AXIS_MASK; //XYZ absolute, E relative
    
    isPrintPaused = false; //clear isPrintPaused flag to allow starting next print after pause->stop scenario.
}

void lcd_sdcard_stop()
{

	lcd_puts_at_P(0, 0, _T(MSG_STOP_PRINT));
	lcd_puts_at_P(2, 2, _T(MSG_NO));
	lcd_puts_at_P(2, 3, _T(MSG_YES));
	lcd_putc_at(0, 2, ' ');
	lcd_putc_at(0, 3, ' ');

	if ((int32_t)lcd_encoder > 2) { lcd_encoder = 2; }
	if ((int32_t)lcd_encoder < 1) { lcd_encoder = 1; }
	
	lcd_putc_at(0, 1 + lcd_encoder, '>');

	if (lcd_clicked())
	{
		Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
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
	enum menuState_t : uint8_t {_uninitialized, _standard, _scrolling};
	typedef struct
	{
		menuState_t menuState = _uninitialized;
		uint8_t offset;
		bool isDir;
		const char* scrollPointer;
		uint16_t selectedFileID;
		uint16_t fileCnt;
		int8_t row;
		uint8_t sdSort;
		ShortTimer lcd_scrollTimer;
	} _menu_data_sdcard_t;
	static_assert(sizeof(menu_data)>= sizeof(_menu_data_sdcard_t),"_menu_data_sdcard_t doesn't fit into menu_data");
	_menu_data_sdcard_t* _md = (_menu_data_sdcard_t*)&(menu_data[0]);
	
	switch(_md->menuState)
	{
		case _uninitialized: //Initialize menu data
		{
			if (card.presort_flag == true) //used to force resorting if sorting type is changed.
			{
				card.presort_flag = false;
				card.presort();
			}
			_md->fileCnt = card.getnrfilenames();
			_md->sdSort = eeprom_read_byte((uint8_t*)EEPROM_SD_SORT);
			_md->menuState = _standard;
		}
		// FALLTHRU
		case _standard: //normal menu structure.
		{
			if (!_md->lcd_scrollTimer.running()) //if the timer is not running, then the menu state was just switched, so redraw the screen.
			{
				_md->lcd_scrollTimer.start();
				lcd_draw_update = 1;
			}
			if (_md->lcd_scrollTimer.expired(500) && (_md->row != -1)) //switch to the scrolling state on timeout if a file/dir is selected.
			{
				_md->menuState = _scrolling;
				_md->offset = 0;
				_md->scrollPointer = NULL;
				_md->lcd_scrollTimer.start();
				lcd_draw_update = 1; //forces last load before switching to scrolling.
			}
			if (lcd_draw_update == 0 && !LCD_CLICKED)
				return; // nothing to do (so don't thrash the SD card)
			
			_md->row = -1; // assume that no SD file/dir is currently selected. Once they are rendered, it will be changed to the correct row for the _scrolling state.
			
			//if we reached this point it means that the encoder moved or clicked or the state is being switched. Reset the scrollTimer.
			_md->lcd_scrollTimer.start();
			
			MENU_BEGIN();
			MENU_ITEM_BACK_P(_T(bMain?MSG_MAIN:MSG_BACK));  // i.e. default menu-item / menu-item after card insertion
			card.getWorkDirName();
			if (card.filename[0] == '/')
			{
#if SDCARDDETECT == -1
				MENU_ITEM_FUNCTION_P(_T(MSG_REFRESH), lcd_sd_refresh);
#else
				if (card.ToshibaFlashAir_isEnabled())
					MENU_ITEM_FUNCTION_P(_T(MSG_REFRESH), lcd_sd_refresh); //show the refresh option if in flashAir mode.
#endif
			}
			else
				MENU_ITEM_FUNCTION_P(PSTR(LCD_STR_FOLDER ".."), lcd_sd_updir); //Show the updir button if in a subdir.

			for (uint16_t i = _md->fileCnt; i-- > 0;) // Every file, from top to bottom.
			{
				if (menu_item == menu_line) //If the file is on the screen.
				{
					//load filename to memory.
#ifdef SDCARD_SORT_ALPHA
					if (_md->sdSort == SD_SORT_NONE)
						card.getfilename(i);
					else
						card.getfilename_sorted(i, _md->sdSort);
#else
					card.getfilename(i);
#endif
					if (lcd_encoder == menu_item) //If the file is selected.
					{
						
						_md->selectedFileID = i;
						_md->isDir = card.filenameIsDir;
						_md->row = menu_row;
					}
					if (card.filenameIsDir)
						MENU_ITEM_SDDIR(card.filename, card.longFilename);
					else
						MENU_ITEM_SDFILE(card.filename, card.longFilename);
				}
				else MENU_ITEM_DUMMY(); //dummy item that just increments the internal menu counters.
			}
			MENU_END();
		} break;
		case _scrolling: //scrolling filename
		{
			const bool rewindFlag = LCD_CLICKED || lcd_draw_update; //flag that says whether the menu should return to _standard state.
			
			if (_md->scrollPointer == NULL)
			{
				//load filename to memory.
#ifdef SDCARD_SORT_ALPHA
				if (_md->sdSort == SD_SORT_NONE)
					card.getfilename(_md->selectedFileID);
				else
					card.getfilename_sorted(_md->selectedFileID, _md->sdSort);
#else
				card.getfilename(_md->selectedFileID);
#endif
				_md->scrollPointer = (card.longFilename[0] == '\0') ? card.filename : card.longFilename;
			}
			
			if (rewindFlag == 1)
				_md->offset = 0; //redraw once again from the beginning.
			if (_md->lcd_scrollTimer.expired(300) || rewindFlag)
			{
				uint8_t i = LCD_WIDTH - ((_md->isDir)?2:1);
				lcd_set_cursor(0, _md->row);
				lcd_print('>');
				if (_md->isDir)
					lcd_print(LCD_STR_FOLDER[0]);
				for (; i != 0; i--)
				{
					const char* c = (_md->scrollPointer + _md->offset + ((LCD_WIDTH - ((_md->isDir)?2:1)) - i));
					lcd_print(c[0]);
					if (c[1])
						_md->lcd_scrollTimer.start();
					else
					{
						_md->lcd_scrollTimer.stop();
						break; //stop at the end of the string
					}
				}
				if (i != 0) //adds spaces if string is incomplete or at the end (instead of null).
				{
					lcd_space(i);
				}
				_md->offset++;
			}
			if (rewindFlag) //go back to sd_menu.
			{
				_md->lcd_scrollTimer.stop(); //forces redraw in _standard state
				_md->menuState = _standard;
			}
		} break;
		default: _md->menuState = _uninitialized; //shouldn't ever happen. Anyways, initialize the menu.
	}
}
#ifdef TMC2130
static void lcd_belttest_v()
{
    lcd_belttest();
    menu_back_if_clicked();
}

void lcd_belttest()
{
    lcd_clear();
	// Belttest requires high power mode. Enable it.
	FORCE_HIGH_POWER_START;
    
    uint16_t   X = eeprom_read_word((uint16_t*)(EEPROM_BELTSTATUS_X));
    uint16_t   Y = eeprom_read_word((uint16_t*)(EEPROM_BELTSTATUS_Y));
	lcd_printf_P(_T(MSG_CHECKING_X));
	lcd_set_cursor(0,1), lcd_printf_P(PSTR("X: %u -> ..."),X);
    KEEPALIVE_STATE(IN_HANDLER);
    
	// N.B: it doesn't make sense to handle !lcd_selfcheck...() because selftest_sg throws its own error screen
	// that clobbers ours, with more info than we could provide. So on fail we just fall through to take us back to status.
    if (lcd_selfcheck_axis_sg(X_AXIS)){
		X = eeprom_read_word((uint16_t*)(EEPROM_BELTSTATUS_X));
		lcd_set_cursor(10,1), lcd_printf_P(PSTR("%u"),X); // Show new X value next to old one.
        lcd_puts_at_P(0,2,_T(MSG_CHECKING_Y));
		lcd_set_cursor(0,3), lcd_printf_P(PSTR("Y: %u -> ..."),Y);
		if (lcd_selfcheck_axis_sg(Y_AXIS))
		{
			Y = eeprom_read_word((uint16_t*)(EEPROM_BELTSTATUS_Y));
			lcd_set_cursor(10,3),lcd_printf_P(PSTR("%u"),Y);
			lcd_set_cursor(19, 3);
			lcd_print(LCD_STR_UPLEVEL);
			lcd_wait_for_click_delay(10);
		}
    }
	
	FORCE_HIGH_POWER_END;
    KEEPALIVE_STATE(NOT_BUSY);
}
#endif //TMC2130

#ifdef IR_SENSOR_ANALOG
// called also from marlin_main.cpp
void printf_IRSensorAnalogBoardChange(){
    printf_P(PSTR("Filament sensor board change detected: revision%S\n"), FsensorIRVersionText());
}

static bool lcd_selftest_IRsensor(bool bStandalone)
{
    bool bPCBrev04;
    uint16_t volt_IR_int;

    volt_IR_int = current_voltage_raw_IR;
    bPCBrev04=(volt_IR_int < IRsensor_Hopen_TRESHOLD);
    printf_P(PSTR("Measured filament sensor high level: %4.2fV\n"), Raw2Voltage(volt_IR_int) );
    if(volt_IR_int < IRsensor_Hmin_TRESHOLD){
        if(!bStandalone)
            lcd_selftest_error(TestError::FsensorLevel,"HIGH","");
        return(false);
    }
    lcd_show_fullscreen_message_and_wait_P(_i("Insert the filament (do not load it) into the extruder and then press the knob."));////MSG_INSERT_FIL c=20 r=6
    volt_IR_int = current_voltage_raw_IR;
    printf_P(PSTR("Measured filament sensor low level: %4.2fV\n"), Raw2Voltage(volt_IR_int));
    if(volt_IR_int > (IRsensor_Lmax_TRESHOLD)){
        if(!bStandalone)
            lcd_selftest_error(TestError::FsensorLevel,"LOW","");
        return(false);
    }
    if((bPCBrev04 ? 1 : 0) != (uint8_t)oFsensorPCB){        // safer then "(uint8_t)bPCBrev04"
        oFsensorPCB=bPCBrev04 ? ClFsensorPCB::_Rev04 : ClFsensorPCB::_Old;
        printf_IRSensorAnalogBoardChange();
        eeprom_update_byte((uint8_t*)EEPROM_FSENSOR_PCB,(uint8_t)oFsensorPCB);
    }
    return(true);
}

static void lcd_detect_IRsensor(){
    bool bAction;
    bool loaded;
    bMenuFSDetect = true;                               // inhibits some code inside "manage_inactivity()"
    /// Check if filament is loaded. If it is loaded stop detection.
    /// @todo Add autodetection with MMU2s
    loaded = ! READ(IR_SENSOR_PIN);
    if(loaded ){
        lcd_show_fullscreen_message_and_wait_P(_i("Please unload the filament first, then repeat this action."));////MSG_UNLOAD_FILAMENT_REPEAT c=20 r=4
        return;
    } else {
        lcd_show_fullscreen_message_and_wait_P(_i("Please check the IR sensor connection, unload filament if present."));////MSG_CHECK_IR_CONNECTION c=20 r=4
        bAction = lcd_selftest_IRsensor(true);
    }
    if(bAction){
        lcd_show_fullscreen_message_and_wait_P(_i("Sensor verified, remove the filament now."));////MSG_FS_VERIFIED c=20 r=3
        // the fsensor board has been successfully identified, any previous "not responding" may be cleared now
        fsensor_not_responding = false;
    } else {
        lcd_show_fullscreen_message_and_wait_P(_i("Verification failed, remove the filament and try again."));////MSG_FIL_FAILED c=20 r=5
        // here it is unclear what to to with the fsensor_not_responding flag
    }
    bMenuFSDetect=false;                              // de-inhibits some code inside "manage_inactivity()"
}
#endif //IR_SENSOR_ANALOG

static void lcd_selftest_v()
{
	(void)lcd_selftest();
}

bool lcd_selftest()
{
	int _progress = 0;
	bool _result = true;
	bool _swapped_fan = false;
#ifdef IR_SENSOR_ANALOG
	//!   Check if IR sensor is in unknown state, if so run Fsensor Detection
	//!   As the Fsensor Detection isn't yet ready for the mmu2s we set temporarily the IR sensor 0.3 or older for mmu2s
	//! @todo Don't forget to remove this as soon Fsensor Detection works with mmu
	if( oFsensorPCB == ClFsensorPCB::_Undef) {
		if (!mmu_enabled) {
			lcd_detect_IRsensor();
		}
		else {
			eeprom_update_byte((uint8_t*)EEPROM_FSENSOR_PCB,0);
		}
	}
#endif //IR_SENSOR_ANALOG
	lcd_wait_for_cool_down();
	lcd_clear();
	lcd_puts_at_P(0, 0, _i("Self test start"));////MSG_SELFTEST_START c=20
	#ifdef TMC2130
	  FORCE_HIGH_POWER_START;
	#endif // TMC2130
	FORCE_BL_ON_START;
	_delay(2000);
	KEEPALIVE_STATE(IN_HANDLER);

	_progress = lcd_selftest_screen(TestScreen::ExtruderFan, _progress, 3, true, 2000);
#if (defined(FANCHECK) && defined(TACH_0))
	switch (lcd_selftest_fan_auto(0)){		// check extruder Fan
		case FanCheck::ExtruderFan:
			_result = false;
			break;
		case FanCheck::SwappedFan:
			_swapped_fan = true;
			// FALLTHRU
		default:
			_result = true;
			break;
	}
#else //defined(TACH_0)
	_result = lcd_selftest_manual_fan_check(0, false);
#endif //defined(TACH_0)
	if (!_result)
	{
		lcd_selftest_error(TestError::ExtruderFan, "", "");
	}

	if (_result)
	{
		_progress = lcd_selftest_screen(TestScreen::PrintFan, _progress, 3, true, 2000);
#if (defined(FANCHECK) && defined(TACH_1))
	switch (lcd_selftest_fan_auto(1)){		// check print fan
		case FanCheck::PrintFan:
			_result = false;
			break;
		case FanCheck::SwappedFan:
			_swapped_fan = true;
			// FALLTHRU
		default:
			_result = true;
			break;
	}
#else //defined(TACH_1)
		_result = lcd_selftest_manual_fan_check(1, false);
#endif //defined(TACH_1)
		if (!_result)
		{
			lcd_selftest_error(TestError::PrintFan, "", ""); //print fan not spinning
		}
	}

	if (_swapped_fan) {
		//turn on print fan and check that left extruder fan is not spinning
		_result = lcd_selftest_manual_fan_check(1, true);
		if (_result) {
			//print fan is stil turned on; check that it is spinning
			_result = lcd_selftest_manual_fan_check(1, false, true);
			if (!_result){
				lcd_selftest_error(TestError::PrintFan, "", "");
			}
		}
		else {
			// fans are swapped
			lcd_selftest_error(TestError::SwappedFan, "", "");
		}
	}

	if (_result)
	{
		_progress = lcd_selftest_screen(TestScreen::FansOk, _progress, 3, true, 2000);
		_result = lcd_selfcheck_endstops(); //With TMC2130, only the Z probe is tested.
	}

	if (_result)
	{
		//current_position[Z_AXIS] += 15;									//move Z axis higher to avoid false triggering of Z end stop in case that we are very low - just above heatbed
		_progress = lcd_selftest_screen(TestScreen::AxisX, _progress, 3, true, 2000);
#ifdef TMC2130
        _result = lcd_selfcheck_axis_sg(X_AXIS);
#else
        _result = lcd_selfcheck_axis(X_AXIS, X_MAX_POS);
#endif //TMC2130
	}




	if (_result)
	{
		_progress = lcd_selftest_screen(TestScreen::AxisX, _progress, 3, true, 0);

#ifndef TMC2130
		_result = lcd_selfcheck_pulleys(X_AXIS);
#endif
	}


	if (_result)
	{
		_progress = lcd_selftest_screen(TestScreen::AxisY, _progress, 3, true, 1500);
#ifdef TMC2130
		_result = lcd_selfcheck_axis_sg(Y_AXIS);
#else
		_result = lcd_selfcheck_axis(Y_AXIS, Y_MAX_POS);
#endif // TMC2130
	}

	if (_result)
	{
		_progress = lcd_selftest_screen(TestScreen::AxisZ, _progress, 3, true, 0);
#ifndef TMC2130
		_result = lcd_selfcheck_pulleys(Y_AXIS);
#endif // TMC2130
	}


	if (_result)
	{
#ifdef TMC2130
		tmc2130_home_exit();
		enable_endstops(false);
#endif

		//homeaxis(X_AXIS);
		//homeaxis(Y_AXIS);
        current_position[X_AXIS] = pgm_read_float(bed_ref_points_4);
		current_position[Y_AXIS] = pgm_read_float(bed_ref_points_4+1);
#ifdef TMC2130
		//current_position[X_AXIS] += 0;
		current_position[Y_AXIS] += 4;
#endif //TMC2130
		current_position[Z_AXIS] = current_position[Z_AXIS] + 10;
		plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
		st_synchronize();
        set_destination_to_current();
		_progress = lcd_selftest_screen(TestScreen::AxisZ, _progress, 3, true, 1500);
#ifdef TMC2130
		homeaxis(Z_AXIS); //In case of failure, the code gets stuck in this function.
#else
        _result = lcd_selfcheck_axis(Z_AXIS, Z_MAX_POS);
#endif //TMC2130

		//raise Z to not damage the bed during and hotend testing
		current_position[Z_AXIS] += 20;
		plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
		st_synchronize();
	}

#ifdef TMC2130
	if (_result)
	{
		current_position[Z_AXIS] = current_position[Z_AXIS] + 10;
		plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
		st_synchronize();
		_progress = lcd_selftest_screen(TestScreen::Home, 0, 2, true, 0);
		bool bres = tmc2130_home_calibrate(X_AXIS);
		_progress = lcd_selftest_screen(TestScreen::Home, 1, 2, true, 0);
		bres &= tmc2130_home_calibrate(Y_AXIS);
		_progress = lcd_selftest_screen(TestScreen::Home, 2, 2, true, 0);
		if (bres)
			eeprom_update_byte((uint8_t*)EEPROM_TMC2130_HOME_ENABLED, 1);
		_result = bres;
	}
#endif //TMC2130

	if (_result)
	{
		_progress = lcd_selftest_screen(TestScreen::Bed, _progress, 3, true, 2000);
		_result = lcd_selfcheck_check_heater(true);
	}

    if (_result)
    {
        _progress = lcd_selftest_screen(TestScreen::Hotend, _progress, 3, true, 1000);
        _result = lcd_selfcheck_check_heater(false);
    }
	if (_result)
	{
		_progress = lcd_selftest_screen(TestScreen::HotendOk, _progress, 3, true, 2000); //nozzle ok
	}
#ifdef FILAMENT_SENSOR
    if (_result)
    {

        if (mmu_enabled)
        {        
			_progress = lcd_selftest_screen(TestScreen::Fsensor, _progress, 3, true, 2000); //check filaments sensor
            _result = selftest_irsensor();
		    if (_result)
			{
				_progress = lcd_selftest_screen(TestScreen::FsensorOk, _progress, 3, true, 2000); //fil sensor OK
			}
        } else
        {
#ifdef PAT9125
			_progress = lcd_selftest_screen(TestScreen::Fsensor, _progress, 3, true, 2000); //check filaments sensor
               _result = lcd_selftest_fsensor();
			if (_result)
			{
				_progress = lcd_selftest_screen(TestScreen::FsensorOk, _progress, 3, true, 2000); //fil sensor OK
			}
#endif //PAT9125
#if 0
	// Intentionally disabled - that's why we moved the detection to runtime by just checking the two voltages.
	// The idea is not to force the user to remove and insert the filament on an assembled printer.
//def IR_SENSOR_ANALOG
			_progress = lcd_selftest_screen(TestScreen::Fsensor, _progress, 3, true, 2000); //check filament sensor
			_result = lcd_selftest_IRsensor();
			if (_result)
			{
				_progress = lcd_selftest_screen(TestScreen::FsensorOk, _progress, 3, true, 2000); //filament sensor OK
			}
#endif //IR_SENSOR_ANALOG
        }
    }
#endif //FILAMENT_SENSOR
	if (_result)
	{
		_progress = lcd_selftest_screen(TestScreen::AllCorrect, _progress, 3, true, 5000); //all correct
	}
	else
	{
		_progress = lcd_selftest_screen(TestScreen::Failed, _progress, 3, true, 5000);
	}
	lcd_reset_alert_level();
	enquecommand_P(PSTR("M84"));
	lcd_update_enable(true);
	
	if (_result)
	{
		LCD_ALERTMESSAGERPGM(_i("Self test OK"));////MSG_SELFTEST_OK c=20
	}
	else
	{
		LCD_ALERTMESSAGERPGM(_T(MSG_SELFTEST_FAILED));
	}
	#ifdef TMC2130
	  FORCE_HIGH_POWER_END;
	#endif // TMC2130
    
    FORCE_BL_ON_END;
	
    KEEPALIVE_STATE(NOT_BUSY);
	return(_result);
}

#ifdef TMC2130

static void reset_crash_det(unsigned char axis) {
	current_position[axis] += 10;
	plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
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


	raise_z_above(MESH_HOME_Z_SEARCH);
	st_synchronize();
	tmc2130_home_enter(1 << axis);

// first axis length measurement begin	
	
	current_position[axis] -= (axis_length + margin);
	plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);

	
	st_synchronize();

	tmc2130_sg_meassure_start(axis);

	current_position_init = st_get_position_mm(axis);

	current_position[axis] += 2 * margin;
	plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
	st_synchronize();

	current_position[axis] += axis_length;
	plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);

	st_synchronize();

	uint16_t sg1 = tmc2130_sg_meassure_stop();
	printf_P(PSTR("%c AXIS SG1=%d\n"), 'X'+axis, sg1);
	eeprom_write_word(((uint16_t*)((axis == X_AXIS)?EEPROM_BELTSTATUS_X:EEPROM_BELTSTATUS_Y)), sg1);

	current_position_final = st_get_position_mm(axis);
	measured_axis_length[0] = fabs(current_position_final - current_position_init);


// first measurement end and second measurement begin	


	current_position[axis] -= margin;
	plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
	st_synchronize();	

	current_position[axis] -= (axis_length + margin);
	plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
		
	st_synchronize();

	current_position_init = st_get_position_mm(axis);

	measured_axis_length[1] = fabs(current_position_final - current_position_init);

	tmc2130_home_exit();

//end of second measurement, now check for possible errors:

	for(uint_least8_t i = 0; i < 2; i++){ //check if measured axis length corresponds to expected length
		printf_P(_N("Measured axis length:%.3f\n"), measured_axis_length[i]);
		if (fabs(measured_axis_length[i] - axis_length) > max_error_mm) {
			enable_endstops(false);

			const char *_error_1;

			if (axis == X_AXIS) _error_1 = "X";
			if (axis == Y_AXIS) _error_1 = "Y";
			if (axis == Z_AXIS) _error_1 = "Z";

			lcd_selftest_error(TestError::Axis, _error_1, "");
			current_position[axis] = 0;
			plan_set_position_curposXYZE();
			reset_crash_det(axis);
			enable_endstops(true);
			endstops_hit_on_purpose();
			return false;
		}
	}

		printf_P(_N("Axis length difference:%.3f\n"), fabs(measured_axis_length[0] - measured_axis_length[1]));
	
		if (fabs(measured_axis_length[0] - measured_axis_length[1]) > 1) { //check if difference between first and second measurement is low
			//loose pulleys
			const char *_error_1;

			if (axis == X_AXIS) _error_1 = "X";
			if (axis == Y_AXIS) _error_1 = "Y";
			if (axis == Z_AXIS) _error_1 = "Z";

			lcd_selftest_error(TestError::Pulley, _error_1, "");
			current_position[axis] = 0;
			plan_set_position_curposXYZE();
			reset_crash_det(axis);
			endstops_hit_on_purpose();
			return false;
		}
		current_position[axis] = 0;
		plan_set_position_curposXYZE();
		reset_crash_det(axis);
		endstops_hit_on_purpose();
		return true;
}
#endif //TMC2130

#ifndef TMC2130

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
		plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
	}

	do {
		current_position[_axis] = current_position[_axis] - 1;

		plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
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
			_progress = lcd_selftest_screen(static_cast<TestScreen>(static_cast<int>(TestScreen::AxisX) + _axis), _progress, 3, false, 0);
			_lcd_refresh = 0;
		}

		manage_heater();
		manage_inactivity(true);

		//_delay(100);
		(_travel_done <= _travel) ? _travel_done++ : _stepdone = true;

	} while (!_stepdone);


	//current_position[_axis] = current_position[_axis] + 15;
	//plan_buffer_line_curposXYZE(manual_feedrate[0] / 60, active_extruder);

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
			lcd_selftest_error(TestError::Endstop, _error_1, _error_2);
		}
		else
		{
			lcd_selftest_error(TestError::Motor, _error_1, _error_2);
		}
	}    
	current_position[_axis] = 0; //simulate axis home to avoid negative numbers for axis position, especially Z.
	plan_set_position_curposXYZE();

	return _stepresult;
}

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
	plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
	for (i = 0; i < 5; i++) {
		refresh_cmd_timeout();
		current_position[axis] = current_position[axis] + move;
		st_current_set(0, 850); //set motor current higher
		plan_buffer_line_curposXYZE(200);
		st_synchronize();
          if (SilentModeMenu != SILENT_MODE_OFF) st_current_set(0, tmp_motor[0]); //set back to normal operation currents
		else st_current_set(0, tmp_motor_loud[0]); //set motor current back			
		current_position[axis] = current_position[axis] - move;
		plan_buffer_line_curposXYZE(50);
		st_synchronize();
		if (((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) ||
			((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1)) {
			lcd_selftest_error(TestError::Pulley, (axis == 0) ? "X" : "Y", "");
			return(false);
		}
	}
	timeout_counter = _millis() + 2500;
	endstop_triggered = false;
	manage_inactivity(true);
	while (!endstop_triggered) {
		if (((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) ||
			((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1)) {
			endstop_triggered = true;
			if (current_position_init - 1 <= current_position[axis] && current_position_init + 1 >= current_position[axis]) {
				current_position[axis] += 10;
				plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
				st_synchronize();
				return(true);
			}
			else {
				lcd_selftest_error(TestError::Pulley, (axis == 0) ? "X" : "Y", "");
				return(false);
			}
		}
		else {
			current_position[axis] -= 1;
			plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
			st_synchronize();
			if (_millis() > timeout_counter) {
				lcd_selftest_error(TestError::Pulley, (axis == 0) ? "X" : "Y", "");
				return(false);
			}
		}
	}
	return(true);
}
#endif //not defined TMC2130


static bool lcd_selfcheck_endstops()
{
	bool _result = true;

	if (
	#ifndef TMC2130
		((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) ||
		((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1) ||
	#endif //!TMC2130
		((READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING) == 1))
	{
	#ifndef TMC2130
		if ((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) current_position[0] += 10;
		if ((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1) current_position[1] += 10;
	#endif //!TMC2130
		if ((READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING) == 1) current_position[2] += 10;
	}
	plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
	st_synchronize();

	if (
	#ifndef TMC2130
		((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) ||
		((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1) ||
	#endif //!TMC2130
		((READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING) == 1))
	{
		_result = false;
		char _error[4] = "";
	#ifndef TMC2130
		if ((READ(X_MIN_PIN) ^ X_MIN_ENDSTOP_INVERTING) == 1) strcat(_error, "X");
		if ((READ(Y_MIN_PIN) ^ Y_MIN_ENDSTOP_INVERTING) == 1) strcat(_error, "Y");
	#endif //!TMC2130
		if ((READ(Z_MIN_PIN) ^ Z_MIN_ENDSTOP_INVERTING) == 1) strcat(_error, "Z");
		lcd_selftest_error(TestError::Endstops, _error, "");
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
		_progress = (_isbed) ? lcd_selftest_screen(TestScreen::Bed, _progress, 2, false, 400) : lcd_selftest_screen(TestScreen::Hotend, _progress, 2, false, 400);
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

	if (_opposite_result < ((_isbed) ? 30 : 9))
	{
		if (_checked_result >= ((_isbed) ? 9 : 30))
		{
			_stepresult = true;
		}
		else
		{
			lcd_selftest_error(TestError::Heater, "", "");
		}
	}
	else
	{
		lcd_selftest_error(TestError::Bed, "", "");
	}

	manage_heater();
	manage_inactivity(true);
	KEEPALIVE_STATE(IN_HANDLER);
	return _stepresult;

}
static void lcd_selftest_error(TestError testError, const char *_error_1, const char *_error_2)
{
	lcd_beeper_quick_feedback();
    
    FORCE_BL_ON_END;
    
	target_temperature[0] = 0;
	target_temperature_bed = 0;
	manage_heater();
	manage_inactivity();

	lcd_clear();

	lcd_puts_at_P(0, 0, _i("Selftest error!"));////MSG_SELFTEST_ERROR c=20
	lcd_puts_at_P(0, 1, _i("Please check:"));////MSG_SELFTEST_PLEASECHECK c=20

	switch (testError)
	{
	case TestError::Heater:
		lcd_puts_at_P(0, 2, _i("Heater/Thermistor"));////MSG_SELFTEST_HEATERTHERMISTOR c=20
		lcd_puts_at_P(0, 3, _i("Not connected"));////MSG_SELFTEST_NOTCONNECTED c=20
		break;
	case TestError::Bed:
		lcd_puts_at_P(0, 2, _i("Bed/Heater"));////MSG_SELFTEST_BEDHEATER c=20
		lcd_puts_at_P(0, 3, _T(MSG_SELFTEST_WIRINGERROR));
		break;
	case TestError::Endstops:
		lcd_puts_at_P(0, 2, _i("Endstops"));////MSG_SELFTEST_ENDSTOPS c=20
		lcd_puts_at_P(0, 3, _T(MSG_SELFTEST_WIRINGERROR));
		lcd_set_cursor(18, 3);
		lcd_print(_error_1);
		break;
	case TestError::Motor:
		lcd_puts_at_P(0, 2, _T(MSG_SELFTEST_MOTOR));
		lcd_set_cursor(18, 2);
		lcd_print(_error_1);
		lcd_puts_at_P(0, 3, _i("Endstop"));////MSG_SELFTEST_ENDSTOP c=16
		lcd_set_cursor(18, 3);
		lcd_print(_error_2);
		break;
	case TestError::Endstop:
		lcd_puts_at_P(0, 2, _i("Endstop not hit"));////MSG_SELFTEST_ENDSTOP_NOTHIT c=20
		lcd_puts_at_P(0, 3, _T(MSG_SELFTEST_MOTOR));
		lcd_set_cursor(18, 3);
		lcd_print(_error_1);
		break;
	case TestError::PrintFan:
		lcd_puts_at_P(0, 2, _T(MSG_SELFTEST_COOLING_FAN));
		lcd_puts_at_P(0, 3, _T(MSG_SELFTEST_WIRINGERROR));
		lcd_set_cursor(18, 3);
		lcd_print(_error_1);
		break;
	case TestError::ExtruderFan:
		lcd_puts_at_P(0, 2, _T(MSG_SELFTEST_EXTRUDER_FAN));
		lcd_puts_at_P(0, 3, _T(MSG_SELFTEST_WIRINGERROR));
		lcd_set_cursor(18, 3);
		lcd_print(_error_1);
		break;
	case TestError::Pulley:
		lcd_puts_at_P(0, 2, _i("Loose pulley"));////MSG_LOOSE_PULLEY c=20
		lcd_puts_at_P(0, 3, _T(MSG_SELFTEST_MOTOR));
		lcd_set_cursor(18, 3);
		lcd_print(_error_1);
		break;
	case TestError::Axis:
		lcd_puts_at_P(0, 2, _i("Axis length"));////MSG_SELFTEST_AXIS_LENGTH c=20
		lcd_puts_at_P(0, 3, _i("Axis"));////MSG_SELFTEST_AXIS c=16
		lcd_set_cursor(18, 3);
		lcd_print(_error_1);
		break;
	case TestError::SwappedFan:
		lcd_puts_at_P(0, 2, _i("Front/left fans"));////MSG_SELFTEST_FANS c=20
		lcd_puts_at_P(0, 3, _i("Swapped"));////MSG_SELFTEST_SWAPPED c=16
		lcd_set_cursor(18, 3);
		lcd_print(_error_1);
		break;
	case TestError::WiringFsensor:
		lcd_puts_at_P(0, 2, _T(MSG_SELFTEST_FILAMENT_SENSOR));
		lcd_puts_at_P(0, 3, _T(MSG_SELFTEST_WIRINGERROR));
		break;
	case TestError::TriggeringFsensor:
          lcd_puts_at_P(0, 2, _T(MSG_SELFTEST_FILAMENT_SENSOR));
          lcd_puts_at_P(0, 3, _i("False triggering"));////MSG_FALSE_TRIGGERING c=20
          break;
	case TestError::FsensorLevel:
          lcd_puts_at_P(0, 2, _T(MSG_SELFTEST_FILAMENT_SENSOR));
          lcd_set_cursor(0, 3);
          lcd_printf_P(_i("%s level expected"),_error_1);////MSG_SELFTEST_FS_LEVEL c=20
          break;
	}

	_delay(1000);
	lcd_beeper_quick_feedback();

	do {
		_delay(100);
		manage_heater();
		manage_inactivity();
	} while (!lcd_clicked());

	LCD_ALERTMESSAGERPGM(_T(MSG_SELFTEST_FAILED));
	lcd_return_to_status();

}

#ifdef FILAMENT_SENSOR
#ifdef PAT9125
static bool lcd_selftest_fsensor(void)
{
	fsensor_init();
	if (fsensor_not_responding)
	{
		lcd_selftest_error(TestError::WiringFsensor, "", "");
	}
	return (!fsensor_not_responding);
}
#endif //PAT9125

//! @brief Self-test of infrared barrier filament sensor mounted on MK3S with MMUv2 printer
//!
//! Test whether sensor is not triggering filament presence when extruder idler is moving without filament.
//!
//! Steps:
//!  * Backup current active extruder temperature
//!  * Pre-heat to PLA extrude temperature.
//!  * Unload filament possibly present.
//!  * Move extruder idler same way as during filament load
//!    and sample IR_SENSOR_PIN.
//!  * Check that pin doesn't go low.
//!
//! @retval true passed
//! @retval false failed
static bool selftest_irsensor()
{
    class TempBackup
    {
    public:
        TempBackup():
            m_temp(degTargetHotend(active_extruder)),
            m_extruder(active_extruder){}
        ~TempBackup(){setTargetHotend(m_temp,m_extruder);}
    private:
        float m_temp;
        uint8_t m_extruder;
    };
    uint8_t progress;
    {
        TempBackup tempBackup;
        setTargetHotend(ABS_PREHEAT_HOTEND_TEMP,active_extruder);
        mmu_wait_for_heater_blocking();
        progress = lcd_selftest_screen(TestScreen::Fsensor, 0, 1, true, 0);
        mmu_filament_ramming();
    }
    progress = lcd_selftest_screen(TestScreen::Fsensor, progress, 1, true, 0);
    mmu_command(MmuCmd::U0);
    manage_response(false, false);

    for(uint_least8_t i = 0; i < 200; ++i)
    {
        if (0 == (i % 32)) progress = lcd_selftest_screen(TestScreen::Fsensor, progress, 1, true, 0);

        mmu_load_step(false);
        while (blocks_queued())
        {
            if (READ(IR_SENSOR_PIN) == 0)
            {
                lcd_selftest_error(TestError::TriggeringFsensor, "", "");
                return false;
            }
#ifdef TMC2130
            manage_heater();
            // Vojtech: Don't disable motors inside the planner!
            if (!tmc2130_update_sg())
            {
                manage_inactivity(true);
            }
#else //TMC2130
            manage_heater();
            // Vojtech: Don't disable motors inside the planner!
            manage_inactivity(true);
#endif //TMC2130
        }
    }
    return true;
}
#endif //FILAMENT_SENSOR

static bool lcd_selftest_manual_fan_check(int _fan, bool check_opposite,
	bool _default)
{

	bool _result = check_opposite;
	lcd_clear();

	lcd_puts_at_P(0, 0, _T(MSG_SELFTEST_FAN));

	switch (_fan)
	{
	case 0:
		// extruder cooling fan
		lcd_puts_at_P(0, 1, check_opposite ? _T(MSG_SELFTEST_COOLING_FAN) : _T(MSG_SELFTEST_EXTRUDER_FAN));
		setExtruderAutoFanState(3);
		break;
	case 1:
		// object cooling fan
		lcd_puts_at_P(0, 1, check_opposite ? _T(MSG_SELFTEST_EXTRUDER_FAN) : _T(MSG_SELFTEST_COOLING_FAN));
		SET_OUTPUT(FAN_PIN);
#ifdef FAN_SOFT_PWM
		fanSpeedSoftPwm = 255;
#else //FAN_SOFT_PWM
		analogWrite(FAN_PIN, 255);
#endif //FAN_SOFT_PWM

		break;
	}
	_delay(500);

	lcd_puts_at_P(1, 2, _T(MSG_SELFTEST_FAN_YES));
	lcd_putc_at(0, 3, '>');
	lcd_puts_at_P(1, 3, _T(MSG_SELFTEST_FAN_NO));

	int8_t enc_dif = int(_default)*3;

	KEEPALIVE_STATE(PAUSED_FOR_USER);

	lcd_button_pressed = false;
	do
	{
		if (abs((enc_dif - lcd_encoder_diff)) > 2) {
			if (enc_dif > lcd_encoder_diff) {
				_result = !check_opposite;
				lcd_putc_at(0, 2, '>');
				lcd_puts_at_P(1, 2, _T(MSG_SELFTEST_FAN_YES));
				lcd_putc_at(0, 3, ' ');
				lcd_puts_at_P(1, 3, _T(MSG_SELFTEST_FAN_NO));
			}

			if (enc_dif < lcd_encoder_diff) {
				_result = check_opposite;
				lcd_putc_at(0, 2, ' ');
				lcd_puts_at_P(1, 2, _T(MSG_SELFTEST_FAN_YES));
				lcd_putc_at(0, 3, '>');
				lcd_puts_at_P(1, 3, _T(MSG_SELFTEST_FAN_NO));
			}
			enc_dif = 0;
			lcd_encoder_diff = 0;
		}


		manage_heater();
		_delay(100);

	} while (!lcd_clicked());
	KEEPALIVE_STATE(IN_HANDLER);
	setExtruderAutoFanState(0);
	SET_OUTPUT(FAN_PIN);
#ifdef FAN_SOFT_PWM
	fanSpeedSoftPwm = 0;
#else //FAN_SOFT_PWM
	analogWrite(FAN_PIN, 0);
#endif //FAN_SOFT_PWM
	fanSpeed = 0;
	manage_heater();

	return _result;
}

#ifdef FANCHECK
static FanCheck lcd_selftest_fan_auto(int _fan)
{
	switch (_fan) {
	case 0:
		fanSpeed = 0;
		manage_heater();			//turn off fan
		setExtruderAutoFanState(3); //extruder fan
#ifdef FAN_SOFT_PWM
		extruder_autofan_last_check = _millis();
		fan_measuring = true;
#endif //FAN_SOFT_PWM
		_delay(2000);
		setExtruderAutoFanState(0); //extruder fan
		manage_heater();			//count average fan speed from 2s delay and turn off fans

		puts_P(PSTR("Test 1:"));
		printf_P(PSTR("Print fan speed: %d\n"), fan_speed[1]);
		printf_P(PSTR("Extr fan speed: %d\n"), fan_speed[0]);

		if (fan_speed[0] < 20) { // < 1200 RPM would mean either a faulty Noctua or Altfan
			return FanCheck::ExtruderFan;
		}
#ifdef FAN_SOFT_PWM
		else if (fan_speed[0] > 50 ) { // printerFan is faster
			return FanCheck::SwappedFan;
		}
		break;
#endif

	case 1:
		//will it work with Thotend > 50 C ?
#ifdef FAN_SOFT_PWM
		fanSpeed = 255;
		fanSpeedSoftPwm = 255;
		extruder_autofan_last_check = _millis(); //store time when measurement starts
		fan_measuring = true; //start fan measuring, rest is on manage_heater
#else //FAN_SOFT_PWM
		fanSpeed = 150;				//print fan
#endif //FAN_SOFT_PWM
		for (uint8_t i = 0; i < 5; i++) {
			delay_keep_alive(1000);
			lcd_putc_at(18, 3, '-');
			delay_keep_alive(1000);
			lcd_putc_at(18, 3, '|');
		}
		fanSpeed = 0;

#ifdef FAN_SOFT_PWM
		fanSpeedSoftPwm = 0;
#else //FAN_SOFT_PWM
		manage_heater();			//turn off fan
		manage_inactivity(true);	//to turn off print fan
#endif //FAN_SOFT_PWM
		puts_P(PSTR("Test 2:"));
		printf_P(PSTR("Print fan speed: %d\n"), fan_speed[1]);
		printf_P(PSTR("Extr fan speed: %d\n"), fan_speed[0]);
		if (!fan_speed[1]) {
			return FanCheck::PrintFan;
		}

#ifdef FAN_SOFT_PWM
		fanSpeed = 80;
		fanSpeedSoftPwm = 80;

		for (uint8_t i = 0; i < 5; i++) {
			delay_keep_alive(1000);
			lcd_putc_at(18, 3, '-');
			delay_keep_alive(1000);
			lcd_putc_at(18, 3, '|');
		}
		fanSpeed = 0;

		// noctua speed is between 17 and 24, turbine more then 30
		if (fan_speed[1] < 30) {
			return FanCheck::SwappedFan;
		}
#else
		// fan is spinning, but measured RPM are too low for print fan, it must
		// be left extruder fan
		else if (fan_speed[1] < 34) {
			return FanCheck::SwappedFan;
		}
#endif //FAN_SOFT_PWM
		break;
	}
	return FanCheck::Success;
}

#endif //FANCHECK

static int lcd_selftest_screen(TestScreen screen, int _progress, int _progress_scale, bool _clear, int _delay)
{

	lcd_update_enable(false);

	const char *_indicator = (_progress >= _progress_scale) ? "-" : "|";

	if (_clear) lcd_clear();


	lcd_set_cursor(0, 0);

	if (screen == TestScreen::ExtruderFan) lcd_puts_P(_T(MSG_SELFTEST_FAN));
	if (screen == TestScreen::PrintFan) lcd_puts_P(_T(MSG_SELFTEST_FAN));
	if (screen == TestScreen::FansOk) lcd_puts_P(_T(MSG_SELFTEST_FAN));
	if (screen == TestScreen::EndStops) lcd_puts_P(_i("Checking endstops"));////MSG_SELFTEST_CHECK_ENDSTOPS c=20
	if (screen == TestScreen::AxisX) lcd_puts_P(_T(MSG_CHECKING_X));
	if (screen == TestScreen::AxisY) lcd_puts_P(_T(MSG_CHECKING_Y));
	if (screen == TestScreen::AxisZ) lcd_puts_P(_i("Checking Z axis"));////MSG_SELFTEST_CHECK_Z c=20
	if (screen == TestScreen::Bed) lcd_puts_P(_T(MSG_SELFTEST_CHECK_BED));
	if (screen == TestScreen::Hotend
	    || screen == TestScreen::HotendOk) lcd_puts_P(_i("Checking hotend"));////MSG_SELFTEST_CHECK_HOTEND c=20
	if (screen == TestScreen::Fsensor) lcd_puts_P(_T(MSG_SELFTEST_CHECK_FSENSOR));
	if (screen == TestScreen::FsensorOk) lcd_puts_P(_T(MSG_SELFTEST_CHECK_FSENSOR));
	if (screen == TestScreen::AllCorrect) lcd_puts_P(_i("All correct"));////MSG_SELFTEST_CHECK_ALLCORRECT c=20
	if (screen == TestScreen::Failed) lcd_puts_P(_T(MSG_SELFTEST_FAILED));
	if (screen == TestScreen::Home) lcd_puts_P(_i("Calibrating home"));////MSG_CALIBRATING_HOME c=20

	lcd_puts_at_P(0, 1, separator);
	if ((screen >= TestScreen::ExtruderFan) && (screen <= TestScreen::FansOk))
	{
		//SERIAL_ECHOLNPGM("Fan test");
		lcd_puts_at_P(0, 2, _i("Extruder fan:"));////MSG_SELFTEST_EXTRUDER_FAN_SPEED c=18
		lcd_set_cursor(18, 2);
		(screen < TestScreen::PrintFan) ? lcd_print(_indicator) : lcd_print("OK");
		lcd_puts_at_P(0, 3, _i("Print fan:"));////MSG_SELFTEST_PRINT_FAN_SPEED c=18
		lcd_set_cursor(18, 3);
		(screen < TestScreen::FansOk) ? lcd_print(_indicator) : lcd_print("OK");
	}
	else if (screen >= TestScreen::Fsensor && screen <= TestScreen::FsensorOk)
	{
		lcd_puts_at_P(0, 2, _T(MSG_SELFTEST_FILAMENT_SENSOR));
		lcd_putc(':');
		lcd_set_cursor(18, 2);
		(screen == TestScreen::Fsensor) ? lcd_print(_indicator) : lcd_print("OK");
	}
	else if (screen < TestScreen::Fsensor)
	{
		//SERIAL_ECHOLNPGM("Other tests");

		TestScreen _step_block = TestScreen::AxisX;
		lcd_selftest_screen_step(2, 2, ((screen == _step_block) ? 1 : (screen < _step_block) ? 0 : 2), PSTR("X"), _indicator);

		_step_block = TestScreen::AxisY;
		lcd_selftest_screen_step(2, 8, ((screen == _step_block) ? 1 : (screen < _step_block) ? 0 : 2), PSTR("Y"), _indicator);

		_step_block = TestScreen::AxisZ;
		lcd_selftest_screen_step(2, 14, ((screen == _step_block) ? 1 : (screen < _step_block) ? 0 : 2), PSTR("Z"), _indicator);

		_step_block = TestScreen::Bed;
		lcd_selftest_screen_step(3, 0, ((screen == _step_block) ? 1 : (screen < _step_block) ? 0 : 2), PSTR("Bed"), _indicator);

		_step_block = TestScreen::Hotend;
		lcd_selftest_screen_step(3, 9, ((screen == _step_block) ? 1 : (screen < _step_block) ? 0 : 2), PSTR("Hotend"), _indicator);
	}

	if (_delay > 0) delay_keep_alive(_delay);
	_progress++;

	return (_progress >= _progress_scale * 2) ? 0 : _progress;
}

static void lcd_selftest_screen_step(int _row, int _col, int _state, const char *_name_PROGMEM, const char *_indicator)
{
	lcd_set_cursor(_col, _row);
    uint8_t strlenNameP = strlen_P(_name_PROGMEM);

	switch (_state)
	{
	case 1:
		lcd_puts_P(_name_PROGMEM);
		lcd_putc_at(_col + strlenNameP, _row, ':');
		lcd_set_cursor(_col + strlenNameP + 1, _row);
		lcd_print(_indicator);
		break;
	case 2:
		lcd_puts_P(_name_PROGMEM);
		lcd_putc_at(_col + strlenNameP, _row, ':');
		lcd_puts_at_P(_col + strlenNameP + 1, _row, PSTR("OK"));
		break;
	default:
		lcd_puts_P(_name_PROGMEM);
	}
}


/** End of menus **/

/** Menu action functions **/

static bool check_file(const char* filename) {
	if (farm_mode) return true;
	card.openFileReadFilteredGcode(filename, true);
	bool result = false;
	const uint32_t filesize = card.getFileSize();
	uint32_t startPos = 0;
	const uint16_t bytesToCheck = min(END_FILE_SECTION, filesize);
	if (filesize > END_FILE_SECTION) {
		startPos = filesize - END_FILE_SECTION;
		card.setIndex(startPos);
	}
	cmdqueue_reset();
	cmdqueue_serial_disabled = true;

	menu_progressbar_init(bytesToCheck, _i("Checking file"));////MSG_CHECKING_FILE c=17
	while (!card.eof() && !result) {
		menu_progressbar_update(card.get_sdpos() - startPos);
		card.sdprinting = true;
		get_command();
		result = check_commands();
	}
	
	menu_progressbar_finish();
	
	cmdqueue_serial_disabled = false;
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
  for (uint_least8_t i = 0; i < 8; i++) {
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

  for (uint_least8_t i = 0; i < depth; i++) {
	  for (uint_least8_t j = 0; j < 8; j++) {
		  eeprom_write_byte((uint8_t*)EEPROM_DIRS + j + 8 * i, card.dir_names[i][j]);
	  }
  }
  
  //filename is just a pointer to card.filename, which changes everytime you try to open a file by filename. So you can't use filename directly
  //to open a file. Instead, the cached filename in cmd is used as that one is static for the whole lifetime of this function.
  if (!check_file(cmd + 4)) {
	  result = lcd_show_fullscreen_message_yes_no_and_wait_P(_i("File incomplete. Continue anyway?"), false, false);////MSG_FILE_INCOMPLETE c=20 r=3
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
	card.chdir(filename, true);
	lcd_encoder = 0;
	menu_data_reset(); //Forces reloading of cached variables.
}

/** LCD API **/

void ultralcd_init()
{
    {
        uint8_t autoDepleteRaw = eeprom_read_byte(reinterpret_cast<uint8_t*>(EEPROM_AUTO_DEPLETE));
        if (0xff == autoDepleteRaw) lcd_autoDeplete = false;
        else lcd_autoDeplete = autoDepleteRaw;

    }
    backlight_init();
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
	if (farm_mode && no_response && ((_millis() - NcTime) > (NC_TIME * 1000))) {
		//send important status messages periodicaly
		prusa_statistics(important_status, saved_filament_type);
		NcTime = _millis();
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
		if (i!=0) lcd_puts_at_P((i * 20) / (NC_BUTTON_LONG_PRESS * 10), 3, "\xFF");
		if (i == NC_BUTTON_LONG_PRESS * 10) {
			no_response = false;
		}
	}
	lcd_update_enable(true);
	lcd_update(2);
}
#endif //FARM_CONNECT_MESSAGE

void lcd_ping() { //chceck if printer is connected to monitoring when in farm mode
	if (farm_mode) {
		bool empty = cmd_buffer_empty();
		if ((_millis() - PingTime) * 0.001 > (empty ? PING_TIME : PING_TIME_LONG)) { //if commands buffer is empty use shorter time period
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
  SERIAL_PROTOCOLLNRPGM(MSG_LCD_STATUS_CHANGED);
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
  lcd_updatestatus(message);
}

void lcd_updatestatuspgm(const char *message){
	strncpy_P(lcd_status_message, message, LCD_WIDTH);
	lcd_status_message[LCD_WIDTH] = 0;
	lcd_finishstatus();
	// hack lcd_draw_update to 1, i.e. without clear
	lcd_draw_update = 1;
}

void lcd_setstatuspgm(const char* message)
{
  if (lcd_status_message_level > 0)
    return;
  lcd_updatestatuspgm(message);
}

void lcd_updatestatus(const char *message){
	strncpy(lcd_status_message, message, LCD_WIDTH);
	lcd_status_message[LCD_WIDTH] = 0;
	lcd_finishstatus();
	// hack lcd_draw_update to 1, i.e. without clear
	lcd_draw_update = 1;
}

void lcd_setalertstatuspgm(const char* message)
{
  lcd_setstatuspgm(message);
  lcd_status_message_level = 1;
  lcd_return_to_status();
}

void lcd_setalertstatus(const char* message)
{
  lcd_setstatus(message);
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
	backlight_wake();
    if (homing_flag || mesh_bed_leveling_flag || menu_menu == lcd_babystep_z || menu_menu == lcd_move_z)
    {
        // disable longpress during re-entry, while homing or calibration
        lcd_quick_feedback();
        return;
    }
    if (menu_menu == lcd_hw_setup_menu)
    {
        // only toggle the experimental menu visibility flag
        lcd_quick_feedback();
        lcd_experimental_toggle();
        return;
    }

    // explicitely listed menus which are allowed to rise the move-z or live-adj-z functions
    // The lists are not the same for both functions, so first decide which function is to be performed
    if ( (moves_planned() || IS_SD_PRINTING || is_usb_printing )){ // long press as live-adj-z
        if(( current_position[Z_AXIS] < Z_HEIGHT_HIDE_LIVE_ADJUST_MENU ) // only allow live-adj-z up to 2mm of print height
        && ( menu_menu == lcd_status_screen // and in listed menus...
          || menu_menu == lcd_main_menu
          || menu_menu == lcd_tune_menu
          || menu_menu == lcd_support_menu
           )
        ){
            lcd_clear();
            menu_submenu(lcd_babystep_z);
        } else {
            // otherwise consume the long press as normal click
            if( menu_menu != lcd_status_screen )
                menu_back();
        }
    } else { // long press as move-z
        if(menu_menu == lcd_status_screen
        || menu_menu == lcd_main_menu
        || menu_menu == lcd_preheat_menu
        || menu_menu == lcd_sdcard_menu
        || menu_menu == lcd_settings_menu
        || menu_menu == lcd_control_temperature_menu
#if (LANG_MODE != 0)
        || menu_menu == lcd_language
#endif
        || menu_menu == lcd_support_menu
        ){
            move_menu_scale = 1.0;
            menu_submenu(lcd_move_z);
        } else {
            // otherwise consume the long press as normal click
            if( menu_menu != lcd_status_screen )
                menu_back();
        }
    }
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
		backlight_wake();
		if (lcd_oldcardstatus)
		{
			if (!card.cardOK)
			{
				card.initsd(false); //delay the sorting to the sd menu. Otherwise, removing the SD card while sorting will not menu_back()
				card.presort_flag = true; //force sorting of the SD menu
			}
			LCD_MESSAGERPGM(_T(WELCOME_MSG));
			bMain=false;                       // flag (i.e. 'fake parameter') for 'lcd_sdcard_menu()' function
			menu_submenu(lcd_sdcard_menu);
		}
		else
		{
			if(menu_menu==lcd_sdcard_menu)
				menu_back();
			card.release();
			LCD_MESSAGERPGM(_i("Card removed"));////MSG_SD_REMOVED c=20
		}
	}
#endif//CARDINSERTED
	backlight_update();
	if (lcd_next_update_millis < _millis())
	{
		if (abs(lcd_encoder_diff) >= ENCODER_PULSES_PER_STEP)
		{
			if (lcd_draw_update == 0)
			lcd_draw_update = 1;
			lcd_encoder += lcd_encoder_diff / ENCODER_PULSES_PER_STEP;
			Sound_MakeSound(e_SOUND_TYPE_EncoderMove);
			lcd_encoder_diff = 0;
			lcd_timeoutToStatus.start();
			backlight_wake();
		}

		if (LCD_CLICKED)
		{
			lcd_timeoutToStatus.start();
			backlight_wake();
		}

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
		lcd_next_update_millis = _millis() + LCD_UPDATE_INTERVAL;
	}
	lcd_ping(); //check that we have received ping command if we are in farm mode
	lcd_send_status();
	if (lcd_commands_type == LcdCommands::Layer1Cal) lcd_commands();
}

#ifdef TMC2130
//! @brief Is crash detection enabled?
//!
//! @retval true crash detection enabled
//! @retval false crash detection disabled
bool lcd_crash_detect_enabled()
{
    return eeprom_read_byte((uint8_t*)EEPROM_CRASH_DET);
}

void lcd_crash_detect_enable()
{
    tmc2130_sg_stop_on_crash = true;
    eeprom_update_byte((uint8_t*)EEPROM_CRASH_DET, 0xFF);
}

void lcd_crash_detect_disable()
{
    tmc2130_sg_stop_on_crash = false;
    tmc2130_sg_crash = 0;
    eeprom_update_byte((uint8_t*)EEPROM_CRASH_DET, 0x00);
}
#endif

void lcd_experimental_toggle()
{
    uint8_t oldVal = eeprom_read_byte((uint8_t *)EEPROM_EXPERIMENTAL_VISIBILITY);
    if (oldVal == EEPROM_EMPTY_VALUE)
        oldVal = 0;
    else
        oldVal = !oldVal;
    eeprom_update_byte((uint8_t *)EEPROM_EXPERIMENTAL_VISIBILITY, oldVal);
}

#ifdef TMC2130
void UserECool_toggle(){
    // this is only called when the experimental menu is visible, thus the first condition for enabling of the ECool mode is met in this place
    // The condition is intentionally inverted as we are toggling the state (i.e. if it was enabled, we are disabling the feature and vice versa)
    bool enable = ! UserECoolEnabled();

    eeprom_update_byte((uint8_t *)EEPROM_ECOOL_ENABLE, enable ? EEPROM_ECOOL_MAGIC_NUMBER : EEPROM_EMPTY_VALUE);

    // @@TODO I don't like this - disabling the experimental menu shall disable ECool mode, but it will not reinit the TMC
    // and I don't want to add more code for this experimental feature ... ideally do not reinit the TMC here at all and let the user reset the printer.
    tmc2130_init(TMCInitParams(enable));
}
#endif

/// Enable experimental support for cooler operation of the extruder motor
/// Beware - REQUIRES original Prusa MK3/S/+ extruder motor with adequate maximal current
/// Therefore we don't want to allow general usage of this feature in public as the community likes to
/// change motors for various reasons and unless the motor is rotating, we cannot verify its properties
/// (which would be obviously too late for an improperly sized motor)
/// For farm printing, the cooler E-motor is enabled by default.
bool UserECoolEnabled(){
    // We enable E-cool mode for non-farm prints IFF the experimental menu is visible AND the EEPROM_ECOOL variable has
    // a value of the universal answer to all problems of the universe
    return ( eeprom_read_byte((uint8_t *)EEPROM_ECOOL_ENABLE) == EEPROM_ECOOL_MAGIC_NUMBER ) 
        && ( eeprom_read_byte((uint8_t *)EEPROM_EXPERIMENTAL_VISIBILITY) == 1 );
}

bool FarmOrUserECool(){
    return farm_mode || UserECoolEnabled();
}

void lcd_experimental_menu()
{
    MENU_BEGIN();
    MENU_ITEM_BACK_P(_T(MSG_BACK));

#ifdef EXTRUDER_ALTFAN_DETECT
    MENU_ITEM_TOGGLE_P(_N("ALTFAN det."), altfanOverride_get()?_T(MSG_OFF):_T(MSG_ON), altfanOverride_toggle);////MSG_MENU_ALTFAN c=18
#endif //EXTRUDER_ALTFAN_DETECT
    
#ifdef TMC2130
    MENU_ITEM_TOGGLE_P(_N("E-cool mode"), UserECoolEnabled()?_T(MSG_ON):_T(MSG_OFF), UserECool_toggle);////MSG_MENU_ECOOL c=18
#endif
    MENU_END();
}

#ifdef PINDA_TEMP_COMP
void lcd_pinda_temp_compensation_toggle()
{
	uint8_t pinda_temp_compensation = eeprom_read_byte((uint8_t*)EEPROM_PINDA_TEMP_COMPENSATION);
	if (pinda_temp_compensation == EEPROM_EMPTY_VALUE) // On MK2.5/S the EEPROM_EMPTY_VALUE will be set to 0 during eeprom_init.
		pinda_temp_compensation = 1;                   // But for MK3/S it should be 1 so SuperPINDA is "active"
	else
		pinda_temp_compensation = !pinda_temp_compensation;
	eeprom_update_byte((uint8_t*)EEPROM_PINDA_TEMP_COMPENSATION, pinda_temp_compensation);
	SERIAL_ECHOLNPGM("SuperPINDA:");
	SERIAL_ECHOLN(pinda_temp_compensation);
}
#endif //PINDA_TEMP_COMP
