//! @file
//! @date Aug 28, 2019
//! @author mkbel
//! @brief LCD

#include "temperature.h"
#include "ultralcd.h"
#include "Marlin.h"
#include "language.h"
#include "cardreader.h"
#include "fancheck.h"
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

#include "Filament_sensor.h"

#ifdef TMC2130
#include "tmc2130.h"
#endif //TMC2130

#include "sound.h"

#include "mmu2.h"
#include "SpoolJoin.h"

#include "static_assert.h"
#include "first_lay_cal.h"

#include "adc.h"
#include "config.h"

#ifndef LA_NOCOMPAT
#include "la10compat.h"
#endif

#include "Prusa_farm.h"

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

LcdCommands lcd_commands_type = LcdCommands::Idle;
static uint8_t lcd_commands_step = 0;
static bool extraPurgeNeeded = false; ///< lcd_commands - detect if extra purge after MMU-toolchange is necessary or not

CustomMsg custom_message_type = CustomMsg::Status;
uint8_t custom_message_state = 0;

bool isPrintPaused = false;

static ShortTimer display_time; //just timer for showing pid finished message on lcd;
static uint16_t pid_temp = DEFAULT_PID_TEMP;

static float manual_feedrate[] = MANUAL_FEEDRATE;

/* LCD message status */
static LongTimer lcd_status_message_timeout;
static uint8_t lcd_status_message_level;
static char lcd_status_message[LCD_WIDTH + 1];

/* !Configuration settings */

static uint8_t lay1cal_filament = 0;

static const char separator[] PROGMEM = "--------------------";

/** forward declarations **/

static const char* lcd_display_message_fullscreen_nonBlocking_P(const char *msg);
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
static void lcd_menu_xyz_y_min();
static void lcd_menu_xyz_skew();
static void lcd_menu_xyz_offset();
static void lcd_menu_fails_stats_mmu();
static void lcd_menu_fails_stats_mmu_print();
static void lcd_menu_fails_stats_mmu_total();
static void lcd_menu_toolchange_stats_mmu_total();
static void mmu_unload_filament();
static void lcd_v2_calibration();
//static void lcd_menu_show_sensors_state();      // NOT static due to using inside "Marlin_main" module ("manage_inactivity()")

static void mmu_fil_eject_menu();
static void mmu_load_to_nozzle_menu();
static void mmu_loading_test_menu();
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
static void reset_crash_det(uint8_t axis);
static bool lcd_selfcheck_axis_sg(uint8_t axis);
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

static uint8_t  lcd_selftest_screen(TestScreen screen, uint8_t _progress, uint8_t _progress_scale, bool _clear, uint16_t _delay);
static void lcd_selftest_screen_step(uint8_t _row, uint8_t _col, uint8_t _state, const char *_name, const char _indicator);
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
 * @param _fan i fan number 0 means hotend fan, 1 means print fan.
 *
 * @returns a TestError noerror, extruderFan, printFan or swappedFan.
 */
static FanCheck lcd_selftest_fan_auto(uint8_t _fan);
#endif //FANCHECK

#ifdef FILAMENT_SENSOR
#if FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125
static bool lcd_selftest_fsensor();
#elif FILAMENT_SENSOR_TYPE == FSENSOR_IR
static bool selftest_irsensor();
#elif FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG
static bool selftest_irsensor();
static bool lcd_selftest_IRsensor(bool bStandalone=false);
static void lcd_detect_IRsensor();
#endif
#endif //FILAMENT_SENSOR

static void lcd_selftest_error(TestError error, const char *_error_1, const char *_error_2);
static void lcd_colorprint_change();

static void lcd_disable_farm_mode();
static void lcd_set_fan_check();
#ifdef MMU_HAS_CUTTER
static void lcd_cutter_enabled();
#endif
#ifdef SDCARD_SORT_ALPHA
 static void lcd_sort_type_set();
#endif
static void lcd_babystep_z();

static void lcd_sdcard_menu();
static void lcd_sheet_menu();

/* Different types of actions that can be used in menu items. */
static void menu_action_sdfile(const char* filename);
static void menu_action_sddirectory(const char* filename);

#define ENCODER_FEEDRATE_DEADZONE 10

#define STATE_NA 255
#define STATE_OFF 0
#define STATE_ON 1

#if (SDCARDDETECT > 0)
bool lcd_oldcardstatus;
#endif

uint8_t selected_sheet = 0;

bool ignore_click = false;
bool wait_for_unclick;

bool bMain;                                       // flag (i.e. 'fake parameter') for 'lcd_sdcard_menu()' function
bool bSettings;                                   // flag (i.e. 'fake parameter') for 'lcd_hw_setup_menu()' function

const char STR_SEPARATOR[] PROGMEM = "------------";

static void lcd_implementation_drawmenu_sdfile(uint8_t row, const char* longFilename)
{
    uint8_t len = LCD_WIDTH - 1;
    lcd_set_cursor(0, row);
    lcd_print((lcd_encoder == menu_item)?'>':' ');
    lcd_print_pad(longFilename, len);
}

static void lcd_implementation_drawmenu_sddirectory(uint8_t row, const char* longFilename)
{
    uint8_t len = LCD_WIDTH - 2;
    lcd_set_cursor(0, row);
    lcd_print((lcd_encoder == menu_item)?'>':' ');
    lcd_print(LCD_STR_FOLDER[0]);
    lcd_print_pad(longFilename, len);
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
			lcd_consume_click();
			lcd_update_enabled = false;
			menu_action_sddirectory(str_fn);
			lcd_update_enabled = true;
			return menu_item_ret();
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
			menu_clicked = false;
			lcd_consume_click();
			lcd_update_enabled = false;
			menu_action_sdfile(str_fn);
			lcd_update_enabled = true;
			return menu_item_ret();
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
	const char* src = usb_timer.running()?_N("USB"):(IS_SD_PRINTING?_N(" SD"):_N("   "));
	char per[4];
	bool num = IS_SD_PRINTING || (printer_active() && (print_percent_done_normal != PRINT_PERCENT_DONE_INIT));
	if (!num || heating_status != HeatingStatus::NO_HEATING) // either not printing or heating
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
	sprintf_P(per, num?_N("%3d"):_N("---"), calc_percent_done());
	lcd_printf_P(_N("%3S%3s%%"), src, per);
}

// Print extruder status (5 chars total)
// Scenario 1: "F?"
//              There is no filament loaded and no tool change is in progress
// Scenario 2: "F[nr.]"
//              [nr.] ranges from 1 to 5.
//              Shows which filament is loaded. No tool change is in progress
// Scenario 3: "?>[nr.]"
//              [nr.] ranges from 1 to 5.
//              There is no filament currently loaded, but [nr.] is currently being loaded via tool change
// Scenario 4: "[nr.]>?"
//              [nr.] ranges from 1 to 5.
//              This scenario indicates a bug in the firmware if ? is on the right side
// Scenario 5: "[nr1.]>[nr2.]"
//              [nr1.] ranges from 1 to 5.
//              [nr2.] ranges from 1 to 5.
//              Filament [nr1.] was loaded, but [nr2.] is currently being loaded via tool change
// Scenario 6: "?>?"
//              This scenario should not be possible and indicates a bug in the firmware
uint8_t lcdui_print_extruder(void) {
    uint8_t chars = 1;
    lcd_space(1);
    if (MMU2::mmu2.get_current_tool() == MMU2::mmu2.get_tool_change_tool()) {
        lcd_putc('F');
        lcd_putc(MMU2::mmu2.get_current_tool() == (uint8_t)MMU2::FILAMENT_UNKNOWN ? '?' : MMU2::mmu2.get_current_tool() + '1');
        chars += 2;
    } else {
        lcd_putc(MMU2::mmu2.get_current_tool() == (uint8_t)MMU2::FILAMENT_UNKNOWN ? '?' : MMU2::mmu2.get_current_tool() + '1');
        lcd_putc('>');
        lcd_putc(MMU2::mmu2.get_tool_change_tool() == (uint8_t)MMU2::FILAMENT_UNKNOWN ? '?' : MMU2::mmu2.get_tool_change_tool() + '1');
        chars += 3;
    }
    return chars;
}

// Print farm number (5 chars total)
static void lcdui_print_farm(void)
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
    if (printer_active()) {
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
void lcdui_print_status_line(void) {
    if (heating_status != HeatingStatus::NO_HEATING) { // If heating flag, show progress of heating
        heating_status_counter++;
        if (heating_status_counter > 13) {
            heating_status_counter = 0;
        }
        lcd_set_cursor(7, 3);
        lcd_space(13);

        for (uint8_t dots = 0; dots < heating_status_counter; dots++) {
            lcd_putc_at(7 + dots, 3, '.');
        }
        switch (heating_status) {
        case HeatingStatus::EXTRUDER_HEATING:
            lcd_puts_at_P(0, 3, _T(MSG_HEATING));
            break;
        case HeatingStatus::EXTRUDER_HEATING_COMPLETE:
            lcd_puts_at_P(0, 3, _T(MSG_HEATING_COMPLETE));
            heating_status = HeatingStatus::NO_HEATING;
            heating_status_counter = 0;
            break;
        case HeatingStatus::BED_HEATING:
            lcd_puts_at_P(0, 3, _T(MSG_BED_HEATING));
            break;
        case HeatingStatus::BED_HEATING_COMPLETE:
            lcd_puts_at_P(0, 3, _T(MSG_BED_DONE));
            heating_status = HeatingStatus::NO_HEATING;
            heating_status_counter = 0;
            break;
        default:
            break;
        }
    }
    else if ((IS_SD_PRINTING) &&
        (custom_message_type == CustomMsg::Status) &&
        (lcd_status_message_level <= LCD_STATUS_INFO) &&
        lcd_status_message_timeout.expired_cont(LCD_STATUS_INFO_TIMEOUT))
    {
        // If printing from SD, show what we are printing
        const char* longFilenameOLD = (card.longFilename[0] ? card.longFilename : card.filename);
        if( lcd_print_pad(&longFilenameOLD[scrollstuff], LCD_WIDTH) )
        {
            scrollstuff++;
        } else {
            scrollstuff = 0;
        }
    } else { // Otherwise check for other special events
        switch (custom_message_type) {
        case CustomMsg::M117:   // M117 Set the status line message on the LCD
        case CustomMsg::Status: // Nothing special, print status message normally
        case CustomMsg::M0Wait: // M0/M1 Wait command working even from SD
        case CustomMsg::FilamentLoading: // If loading filament, print status
        case CustomMsg::MMUProgress: // MMU Progress Codes
            lcd_print_pad(lcd_status_message, LCD_WIDTH);
        break;
        case CustomMsg::MeshBedLeveling: // If mesh bed leveling in progress, show the status
            if (custom_message_state > 10) {
                lcd_set_cursor(0, 3);
                lcd_space(LCD_WIDTH);
                lcd_puts_at_P(0, 3, _T(MSG_CALIBRATE_Z_AUTO));
                lcd_puts_P(PSTR(" : "));
                lcd_print(custom_message_state - 10);
            } else {
                if (custom_message_state == 3) {
                    lcd_setstatuspgm(MSG_WELCOME);
                    custom_message_type = CustomMsg::Status;
                }
                if (custom_message_state > 3 && custom_message_state <= 10) {
                    lcd_set_cursor(0, 3);
                    lcd_space(19);
                    lcd_puts_at_P(0, 3, _i("Calibration done")); ////MSG_HOMEYZ_DONE c=20
                    custom_message_state--;
                }
            }
            break;
        case CustomMsg::PidCal: // PID tuning in progress
            lcd_print_pad(lcd_status_message, LCD_WIDTH);
            if (pid_cycle <= pid_number_of_cycles && custom_message_state > 0) {
                lcd_set_cursor(10, 3);
                lcd_printf_P(PSTR("%3d/%-3d"), pid_cycle, pid_number_of_cycles);
            }
            break;
        case CustomMsg::TempCal: // PINDA temp calibration in progress
            lcd_set_cursor(0, 3);
            lcd_printf_P(PSTR("%-12.12S%-d/6"), _T(MSG_PINDA_CALIBRATION), custom_message_state);
            break;
        case CustomMsg::TempCompPreheat:              // temp compensation preheat
            lcd_puts_at_P(0, 3, _i("PINDA Heating")); ////MSG_PINDA_PREHEAT c=20
            if (custom_message_state <= PINDA_HEAT_T) {
                lcd_puts_P(PSTR(": "));
                lcd_print(custom_message_state); // seconds
                lcd_print(' ');
            }
            break;
        case CustomMsg::Resuming: // Resuming
            lcd_puts_at_P(0, 3, _T(MSG_RESUMING_PRINT));
            break;
        }
    }
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

    if (MMU2::mmu2.Enabled()) {
        // Print extruder status (5 chars)
        lcd_space(5 - lcdui_print_extruder());
    } else if (farm_mode) {
        // Print farm number (5 chars)
        lcdui_print_farm();
    } else {
        lcd_space(5); // 5 spaces
    }

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
	static uint8_t lcd_status_update_delay = 0;
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

	if (lcd_draw_update) {
		// Update the status screen immediately
		lcd_status_update_delay = 0;
	}

	if (lcd_status_update_delay)
		lcd_status_update_delay--;
	else
	{	// Redraw the main screen every second (see LCD_UPDATE_INTERVAL).
		// This is easier then trying keep track of all things that change on the screen
		lcd_status_update_delay = 10;
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

		prusa_statistics_update_from_status_screen();

		if (lcd_commands_type != LcdCommands::Idle)
			lcd_commands();
	}

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
		&& ( menu_block_mask == MENU_BLOCK_NONE ) // or a serious error blocks entering the menu
	)
	{
		menu_depth = 0; //redundant, as already done in lcd_return_to_status(), just to be sure
		menu_submenu(lcd_main_menu);
		lcd_refresh(); // to maybe revive the LCD if static electricity killed it.
	}
}

void lcd_print_stop_finish();

void lcd_commands()
{
    // printf_P(PSTR("lcd_commands begin, lcd_commands_type=%u, lcd_commands_step=%u\n"), (uint8_t)lcd_commands_type, lcd_commands_step);
    if (planner_aborted) {
        // we are still within an aborted command. do not process any LCD command until we return
        return;
    }

    if (lcd_commands_type == LcdCommands::StopPrint)
    {
        if (!blocks_queued() && !homing_flag)
        {
            custom_message_type = CustomMsg::Status;
            lcd_setstatuspgm(_T(MSG_PRINT_ABORTED));
            lcd_commands_type = LcdCommands::Idle;
            lcd_commands_step = 0;
            lcd_print_stop_finish();
        }
    }

	if (lcd_commands_type == LcdCommands::LongPause)
	{
		if (!blocks_queued() && !homing_flag)
		{
			if (custom_message_type != CustomMsg::M117)
			{
				custom_message_type = CustomMsg::Status;
				lcd_setstatuspgm(_i("Print paused"));////MSG_PRINT_PAUSED c=20
			}
			lcd_commands_type = LcdCommands::Idle;
			lcd_commands_step = 0;
			long_pause();
		}
	}

	if (lcd_commands_type == LcdCommands::Layer1Cal)
	{
		char cmd1[30];
		const uint16_t nozzle_dia = eeprom_read_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM);
		const float extrusion_width = (nozzle_dia + 20)/1000.0f;
		const float layer_height = 0.2f;

		if(lcd_commands_step>1) lcd_timeoutToStatus.start(); //if user dont confirm live adjust Z value by pressing the knob, we are saving last value by timeout to status screen

        if (!blocks_queued() && cmd_buffer_empty() && !saved_printing)
        {
            if (lcd_commands_step == 0)
                lcd_commands_step = 12;
            else
                lcd_commands_step--;

            switch(lcd_commands_step)
            {
            case 12:
                lay1cal_wait_preheat();
                break;
            case 11:
                extraPurgeNeeded = lay1cal_load_filament(cmd1, lay1cal_filament);
                break;
            case 10:
                lcd_clear();
                menu_depth = 0;
                menu_submenu(lcd_babystep_z);
                lay1cal_intro_line(extraPurgeNeeded, layer_height, extrusion_width);
                break;
            case 9:
                lay1cal_before_meander();
                break;
            case 8:
                lay1cal_meander_start(layer_height, extrusion_width);
                break;
            case 7:
                lay1cal_meander(layer_height, extrusion_width);
                break;
            case 6:
                lay1cal_square(0, layer_height, extrusion_width);
                break;
            case 5:
                lay1cal_square(4, layer_height, extrusion_width);
                break;
            case 4:
                lay1cal_square(8, layer_height, extrusion_width);
                break;
            case 3:
                lay1cal_square(12, layer_height, extrusion_width);
                break;
            case 2:
                lay1cal_finish(MMU2::mmu2.Enabled());
                menu_leaving = 1; //if user dont confirm live adjust Z value by pressing the knob, we are saving last value by timeout to status screen
                break;
            case 1:
                lcd_setstatuspgm(MSG_WELCOME);
                lcd_commands_step = 0;
                lcd_commands_type = LcdCommands::Idle;
                if (eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE))
                    lcd_wizard(WizState::RepeatLay1Cal);
                break;
            }
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
			preparePidTuning(); // ensure we don't move to the next step early
			sprintf_P(cmd1, PSTR("M303 E0 S%3u"), pid_temp);
			// setting the correct target temperature (for visualization) is done in PID_autotune
			enquecommand(cmd1);
			lcd_setstatuspgm(_i("PID cal."));////MSG_PID_RUNNING c=20
			lcd_commands_step = 2;
		}
		if (lcd_commands_step == 2 && !pidTuningRunning()) { //saving to eeprom
			custom_message_state = 0;
			lcd_setstatuspgm(_i("PID cal. finished"));////MSG_PID_FINISHED c=20
			setTargetHotend(0);
			if (_Kp != 0 || _Ki != 0 || _Kd != 0) {
				sprintf_P(cmd1, PSTR("M301 P%.2f I%.2f D%.2f"), _Kp, _Ki, _Kd);
				enquecommand(cmd1);
				enquecommand_P(PSTR("M500"));
			}
			else {
				SERIAL_ECHOPGM("Invalid PID cal. results. Not stored to EEPROM.");
			}
			display_time.start();
			lcd_commands_step = 1;
		}
		if ((lcd_commands_step == 1) && display_time.expired(2000)) { //calibration finished message
			lcd_setstatuspgm(MSG_WELCOME);
			custom_message_type = CustomMsg::Status;
			pid_temp = DEFAULT_PID_TEMP;
			lcd_commands_step = 0;
			lcd_commands_type = LcdCommands::Idle;
		}
	}

#ifdef TEMP_MODEL
    if (lcd_commands_type == LcdCommands::TempModel && cmd_buffer_empty())
    {
        switch (lcd_commands_step)
        {
        case 0:
            lcd_commands_step = 3;
            [[fallthrough]];

        case 3:
            temp_model_set_warn_beep(false);
            enquecommand_P(PSTR("M310 A F1"));
            lcd_commands_step = 2;
            break;

        case 2:
            if (temp_model_autotune_result())
                enquecommand_P(PSTR("M500"));
            lcd_commands_step = 1;
            break;

        case 1:
            lcd_commands_step = 0;
            lcd_commands_type = LcdCommands::Idle;
            temp_model_set_warn_beep(true);
            bool res = temp_model_autotune_result();
            if (eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE)) {
                // resume the wizard
                lcd_wizard(res ? WizState::Restore : WizState::Failed);
            }
            break;
        }
    }
#endif //TEMP_MODEL

	if (lcd_commands_type == LcdCommands::NozzleCNG)
	{
        if (!blocks_queued() && cmd_buffer_empty() && !saved_printing)
        {
#ifdef TEMP_MODEL
            static bool was_enabled;
#endif //TEMP_MODEL
            switch(lcd_commands_step)
            {
            case 0:
                lcd_commands_step = 3;
                break;
            case 3:
                lcd_update_enabled = false; //hack to avoid lcd_update recursion.
                lcd_show_fullscreen_message_and_wait_P(_T(MSG_NOZZLE_CNG_READ_HELP));
                lcd_update_enabled = true;
                lcd_draw_update = 2; //force lcd clear and update after the stack unwinds.
                enquecommand_P(PSTR("G28 W"));
                enquecommand_P(PSTR("G1 X125 Z200 F1000"));
                enquecommand_P(PSTR("M109 S280"));
#ifdef TEMP_MODEL
                was_enabled = temp_model_enabled();
                temp_model_set_enabled(false);
#endif //TEMP_MODEL
                lcd_commands_step = 2;
                break;
            case 2:
                //|0123456789012456789|
                //|Hotend at 280C!
                //|Nozzle changed and
                //|tightend to specs?
                //| Yes     No
                enquecommand_P(PSTR("M84 XY"));
                lcd_update_enabled = false; //hack to avoid lcd_update recursion.
                if (lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_NOZZLE_CNG_CHANGED), false) == LCD_LEFT_BUTTON_CHOICE) {
                    setTargetHotend(0);
#ifdef TEMP_MODEL
                    temp_model_set_enabled(was_enabled);
#endif //TEMP_MODEL
                    lcd_commands_step = 1;
                }
                lcd_update_enabled = true;
                break;
            case 1:
                lcd_setstatuspgm(MSG_WELCOME);
                lcd_commands_step = 0;
                lcd_commands_type = LcdCommands::Idle;
                break;
            }
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

    SERIAL_ECHOLNRPGM(MSG_OCTOPRINT_PAUSED);
    isPrintPaused = true;

    // return to status is required to continue processing in the main loop!
    lcd_commands_type = LcdCommands::LongPause;
    lcd_return_to_status();
}

//! @brief Send host action "pause"
void lcd_pause_usb_print()
{
    SERIAL_PROTOCOLLNRPGM(MSG_OCTOPRINT_ASK_PAUSE);
}

static void lcd_move_menu_axis();



/* Menu implementation */

static void lcd_cooldown()
{
  setTargetHotend(0);
  setTargetBed(0);
  fanSpeed = 0;
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
//! |Hotend fan:     0000|	MSG_HOTEND_FAN_SPEED c=15
//! |Print fan:      0000|	MSG_PRINT_FAN_SPEED c=15
//! |                    |
//! |                    |
//! ----------------------
//! @endcode
void lcd_menu_extruder_info()                     // NOT static due to using inside "Marlin_main" module ("manage_inactivity()")
{

    lcd_timeoutToStatus.stop(); //infinite timeout
    lcd_home();
	lcd_printf_P(PSTR("%-15.15S%-5d\n" "%-15.15S%-5d\n"), _T(MSG_HOTEND_FAN_SPEED), 60*fan_speed[0], _T(MSG_PRINT_FAN_SPEED), 60*fan_speed[1] ); 
    menu_back_if_clicked();
}

static uint16_t __attribute__((noinline)) clamp999(uint16_t v){
    return v > 999 ? 999 : v;
}

//! @brief Show Fails Statistics MMU
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! | Main               |  MSG_MAIN c=18
//! | Last print         |  MSG_LAST_PRINT c=18
//! | Total              |  MSG_TOTAL c=6
//! | Material changes   |  MSG_MATERIAL_CHANGES c=18
//! ----------------------
//! @endcode
static void lcd_menu_fails_stats_mmu()
{
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));
	MENU_ITEM_SUBMENU_P(_T(MSG_LAST_PRINT), lcd_menu_fails_stats_mmu_print);
	MENU_ITEM_SUBMENU_P(_T(MSG_TOTAL), lcd_menu_fails_stats_mmu_total);
	MENU_ITEM_SUBMENU_P(_T(MSG_MATERIAL_CHANGES), lcd_menu_toolchange_stats_mmu_total);
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
static void lcd_menu_fails_stats_mmu_print() {
    lcd_timeoutToStatus.stop(); //infinite timeout
    lcd_home();
    lcd_printf_P(
        PSTR("%S\n"
             " %-16.16S%-3d\n"
             " %-16.16S%-3d"
        ),
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
static void lcd_menu_fails_stats_mmu_total() {
    lcd_timeoutToStatus.stop(); //infinite timeout
    lcd_home();
    lcd_printf_P(
        PSTR("%S\n"
             " %-16.16S%-3d\n"
             " %-16.16S%-3d\n"
             " %-16.16S%-3d"
        ),
        _T(MSG_TOTAL_FAILURES),
        _T(MSG_MMU_FAILS), clamp999( eeprom_read_word((uint16_t*)EEPROM_MMU_FAIL_TOT) ),
        _T(MSG_MMU_LOAD_FAILS), clamp999( eeprom_read_word((uint16_t*)EEPROM_MMU_LOAD_FAIL_TOT) ),
        _T(MSG_MMU_POWER_FAILS), clamp999( MMU2::mmu2.TMCFailures() ));
    menu_back_if_clicked_fb();
}

//! @brief Show Total Failures Statistics MMU
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |Material changes:   ||
//! |          4294967295|
//! |                    |
//! |                    |
//! ----------------------
//! @endcode
static void lcd_menu_toolchange_stats_mmu_total()
{
    typedef struct
    {
        bool initialized;              // 1byte
    } _menu_data_t;
    static_assert(sizeof(menu_data)>= sizeof(_menu_data_t),"_menu_data_t doesn't fit into menu_data");
    _menu_data_t* _md = (_menu_data_t*)&(menu_data[0]);
    if(!_md->initialized) {
        lcd_clear();
        lcd_puts_P(_T(MSG_MATERIAL_CHANGES)); /// MSG_MATERIAL_CHANGES c=18
        lcd_putc(':');
        lcd_set_cursor(10, 1);
        lcd_print(eeprom_read_dword((uint32_t*)EEPROM_MMU_MATERIAL_CHANGES));
        _md->initialized = true;
    }

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
//! @todo leptun refactor this piece of code please
static void lcd_menu_fails_stats_print()
{
	lcd_timeoutToStatus.stop(); //infinite timeout
    uint8_t power = eeprom_read_byte((uint8_t*)EEPROM_POWER_COUNT);
    uint8_t filam = eeprom_read_byte((uint8_t*)EEPROM_FERROR_COUNT);
    uint8_t crashX = eeprom_read_byte((uint8_t*)EEPROM_CRASH_COUNT_X);
    uint8_t crashY = eeprom_read_byte((uint8_t*)EEPROM_CRASH_COUNT_Y);
    lcd_home();
    lcd_printf_P(failStatsFmt,
        _T(MSG_LAST_PRINT_FAILURES),
        _T(MSG_POWER_FAILURES), power,
        _T(MSG_FIL_RUNOUTS), filam,
        _T(MSG_CRASH), crashX, crashY);
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
//! | Nozzle:        000D|	MSG_NOZZLE c=10
//! | Bed:           000D|	MSG_BED c=13
//! | Ambient:       000D|	MSG_AMBIENTc=14
//! | PINDA:         000D|	MSG_PINDA c=14
//! ----------------------
//! D - Degree sysmbol		LCD_STR_DEGREE
//! @endcode
static void lcd_menu_temperatures()
{
    lcd_timeoutToStatus.stop(); //infinite timeout
    lcd_home();
    lcd_menu_temperatures_line( _T(MSG_NOZZLE), (int)current_temperature[0] );
    lcd_menu_temperatures_line( _T(MSG_BED), (int)current_temperature_bed );
#ifdef AMBIENT_THERMISTOR
    lcd_menu_temperatures_line( _i("Ambient"), (int)current_temperature_ambient );  ////MSG_AMBIENT c=14
#endif //AMBIENT_THERMISTOR
#ifdef PINDA_THERMISTOR
    lcd_menu_temperatures_line(MSG_PINDA, (int)current_temperature_pinda );  ////MSG_PINDA
#endif //PINDA_THERMISTOR
    menu_back_if_clicked();
}

#if defined (VOLT_BED_PIN) || defined (VOLT_PWR_PIN) || (defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG))
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
static void lcd_menu_voltages()
{
    lcd_timeoutToStatus.stop(); //infinite timeout
    float volt_pwr = VOLT_DIV_REF * ((float)current_voltage_raw_pwr / (1023 * OVERSAMPLENR)) / VOLT_DIV_FAC;
    float volt_bed = VOLT_DIV_REF * ((float)current_voltage_raw_bed / (1023 * OVERSAMPLENR)) / VOLT_DIV_FAC;
    lcd_home();
    lcd_printf_P(PSTR(" PWR:      %4.1fV\n" " BED:      %4.1fV"), volt_pwr, volt_bed);
#ifdef IR_SENSOR_ANALOG
    lcd_printf_P(PSTR("\n IR :       %3.1fV"), fsensor.Raw2Voltage(fsensor.getVoltRaw()));
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

#ifdef DEBUG_PULLUP_CRASH
void TestPullupCrash() {
	PORTF |= 0x01;
}
#endif // DEBUG_PULLUP_CRASH

//! @brief Show Support Menu
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! | Main               |	MSG_MAIN c=18
//! | Firmware:          |	c=18
//! |  3.7.2.-2363       |	c=16
//! | prusa3d.com        |	MSG_PRUSA3D
//! | forum.prusa3d.com  |	MSG_PRUSA3D_FORUM
//! | help.prusa3d.com  |	MSG_PRUSA3D_HELP
//! | --------------     |	STR_SEPARATOR
//! | 1_75mm_MK3         |	FILAMENT_SIZE
//! | help.prusa3d.com  |	ELECTRONICS
//! | help.prusa3d.com  |	NOZZLE_TYPE
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
  MENU_ITEM_BACK_P(_n("help.prusa3d.com"));////MSG_PRUSA3D_HELP c=18
  MENU_ITEM_BACK_P(STR_SEPARATOR);
  MENU_ITEM_BACK_P(PSTR(FILAMENT_SIZE));
  MENU_ITEM_BACK_P(PSTR(ELECTRONICS));
  MENU_ITEM_BACK_P(PSTR(NOZZLE_TYPE));
  MENU_ITEM_BACK_P(STR_SEPARATOR);
  MENU_ITEM_BACK_P(_i("Date:"));////MSG_DATE c=17
  MENU_ITEM_BACK_P(PSTR(SOURCE_DATE_EPOCH));

#if defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
  MENU_ITEM_BACK_P(STR_SEPARATOR);
  MENU_ITEM_BACK_P(PSTR("Fil. sensor v.:"));
  MENU_ITEM_BACK_P(fsensor.getIRVersionText());
#endif // IR_SENSOR_ANALOG

	MENU_ITEM_BACK_P(STR_SEPARATOR);
	if (MMU2::mmu2.Enabled())
	{
		MENU_ITEM_BACK_P(_i("MMU2 connected"));  ////MSG_MMU_CONNECTED c=18
		MENU_ITEM_BACK_P(PSTR(" FW:"));  ////c=17
		if (((menu_item - 1) == menu_line) && lcd_draw_update)
		{
			lcd_set_cursor(6, menu_row);
			MMU2::Version mmu_version = MMU2::mmu2.GetMMUFWVersion();
			if (mmu_version.major > 0)
				lcd_printf_P(PSTR("%d.%d.%d"), mmu_version.major, mmu_version.minor, mmu_version.build);
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
          lcd_print(_md->ip_str);
      }
  }
  
  // Show the printer IP address, if it is available.
  if (IP_address) {
      
      MENU_ITEM_BACK_P(STR_SEPARATOR);
      MENU_ITEM_BACK_P(_i("Printer IP Addr:")); ////MSG_PRINTER_IP c=18
      MENU_ITEM_BACK_P(PSTR(" "));
      if (((menu_item - 1) == menu_line) && lcd_draw_update) {
          lcd_set_cursor(2, menu_row);
          ip4_to_str(_md->ip_str, (uint8_t*)(&IP_address));
          lcd_print(_md->ip_str);
      }
  }

  MENU_ITEM_BACK_P(STR_SEPARATOR);
  MENU_ITEM_SUBMENU_P(_i("XYZ cal. details"), lcd_menu_xyz_y_min);////MSG_XYZ_DETAILS c=18
  MENU_ITEM_SUBMENU_P(_i("Extruder info"), lcd_menu_extruder_info);////MSG_INFO_EXTRUDER c=18
  MENU_ITEM_SUBMENU_P(_i("Sensor info"), lcd_menu_show_sensors_state);////MSG_INFO_SENSORS c=18

#ifdef TMC2130
  MENU_ITEM_SUBMENU_P(_T(MSG_BELT_STATUS), lcd_menu_belt_status);
#endif //TMC2130
    
  MENU_ITEM_SUBMENU_P(_i("Temperatures"), lcd_menu_temperatures);////MSG_MENU_TEMPERATURES c=18

#if defined (VOLT_BED_PIN) || defined (VOLT_PWR_PIN)
  MENU_ITEM_SUBMENU_P(_i("Voltages"), lcd_menu_voltages);////MSG_MENU_VOLTAGES c=18
#endif //defined VOLT_BED_PIN || defined VOLT_PWR_PIN

#ifdef MENU_DUMP
    MENU_ITEM_FUNCTION_P(_n("Dump memory"), lcd_dump_memory);
#endif //MENU_DUMP
#ifdef MENU_SERIAL_DUMP
    if (emergency_serial_dump)
        MENU_ITEM_FUNCTION_P(_n("Dump to serial"), lcd_serial_dump);
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

FilamentAction eFilamentAction=FilamentAction::None; // must be initialized as 'non-autoLoad'
bool bFilamentPreheatState;
bool bFilamentAction=false;
static bool bFilamentWaitingFlag=false;

bool shouldPreheatOnlyNozzle() {
    uint8_t eeprom_setting = eeprom_read_byte((uint8_t*)EEPROM_HEAT_BED_ON_LOAD_FILAMENT);
    if (eeprom_setting != 0)
        return false;

    switch(eFilamentAction) {
        case FilamentAction::Load:
        case FilamentAction::AutoLoad:
        case FilamentAction::UnLoad:
        case FilamentAction::MmuLoad:
        case FilamentAction::MmuUnLoad:
        case FilamentAction::MmuEject:
        case FilamentAction::MmuCut:
            return true;
        default:
            return false;
    }
}

void lcd_print_target_temps_first_line(){
    lcd_set_cursor(0, 0);
    lcdui_print_temp(LCD_STR_THERMOMETER[0], (int) degHotend(0), (int) degTargetHotend(0));
    lcd_set_cursor(10, 0);
    int targetBedTemp = (int) degTargetBed();
    if (targetBedTemp) {
        lcdui_print_temp(LCD_STR_BEDTEMP[0], (int) degBed(), targetBedTemp);
    } else {
        lcd_space(10);
    }
}

static void mFilamentPrompt()
{
uint8_t nLevel;

lcd_print_target_temps_first_line();
lcd_puts_at_P(0,1, _i("Press the knob"));                 ////MSG_PRESS_KNOB c=20
lcd_set_cursor(0,2);
switch(eFilamentAction)
     {
     case FilamentAction::Load:
     case FilamentAction::AutoLoad:
     case FilamentAction::MmuLoad:
     case FilamentAction::MmuLoadingTest:
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
    if(lcd_clicked()
#ifdef FILAMENT_SENSOR
/// @todo leptun - add this as a specific retest item
        || (((eFilamentAction == FilamentAction::Load) || (eFilamentAction == FilamentAction::AutoLoad)) && fsensor.getFilamentLoadEvent())
#endif //FILAMENT_SENSOR
    ) {
     nLevel=2;
     if(!bFilamentPreheatState) {
        nLevel++;
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
          case FilamentAction::MmuLoadingTest:
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
    uint8_t nLevel;

    setTargetHotend((float)nTemp);
    if (!shouldPreheatOnlyNozzle()) setTargetBed((float)nTempBed);

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

    if (abs((int)current_temperature[0] - nTemp) > TEMP_HYSTERESIS)
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
        case FilamentAction::MmuLoadingTest:
            nLevel = bFilamentPreheatState ? 1 : 2;
            bFilamentAction = true;
            menu_back(nLevel);
            menu_submenu(mmu_loading_test_menu);
            break;
        case FilamentAction::MmuUnLoad:
            nLevel = bFilamentPreheatState ? 1 : 2;
            bFilamentAction = true;
            menu_back(nLevel);
            MMU2::mmu2.unload();
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
            lcd_puts_at_P(0, 3, _T(MSG_CANCEL)); 

            lcd_set_cursor(0, 1);
            switch (eFilamentAction)
            {
            case FilamentAction::Load:
            case FilamentAction::AutoLoad:
            case FilamentAction::MmuLoad:
            case FilamentAction::MmuLoadingTest:
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

        if (bFilamentWaitingFlag) {
            lcd_print_target_temps_first_line();
        }

        if (lcd_clicked())
        {
            bFilamentWaitingFlag = false;
            if (!bFilamentPreheatState)
            {
                setTargetHotend(0);
                if (!isPrintPaused) setTargetBed(0);
                menu_back();
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

static void mFilamentItem_PA()
{
    bFilamentPreheatState = false;
    mFilamentItem(PA_PREHEAT_HOTEND_TEMP, PA_PREHEAT_HPB_TEMP);
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
        ON_MENU_LEAVE(
            mFilamentBack();
        );
        MENU_ITEM_BACK_P(_T(eFilamentAction == FilamentAction::Lay1Cal ? MSG_BACK : MSG_MAIN));
    }
    if (farm_mode)
    {
        MENU_ITEM_FUNCTION_P(PSTR("farm   -  " STRINGIFY(FARM_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(FARM_PREHEAT_HPB_TEMP)), mFilamentItem_farm);
        MENU_ITEM_FUNCTION_P(PSTR("nozzle -  " STRINGIFY(FARM_PREHEAT_HOTEND_TEMP) "/0"), mFilamentItem_farm_nozzle);
    }
    else
    {
        bool bPreheatOnlyNozzle = shouldPreheatOnlyNozzle();
        MENU_ITEM_SUBMENU_P(bPreheatOnlyNozzle ? PSTR("PLA  -  " STRINGIFY(PLA_PREHEAT_HOTEND_TEMP)) : PSTR("PLA  -  " STRINGIFY(PLA_PREHEAT_HOTEND_TEMP)  "/" STRINGIFY(PLA_PREHEAT_HPB_TEMP)) , mFilamentItem_PLA);
        MENU_ITEM_SUBMENU_P(bPreheatOnlyNozzle ? PSTR("PET  -  " STRINGIFY(PET_PREHEAT_HOTEND_TEMP)) : PSTR("PET  -  " STRINGIFY(PET_PREHEAT_HOTEND_TEMP)  "/" STRINGIFY(PET_PREHEAT_HPB_TEMP)) , mFilamentItem_PET);
        MENU_ITEM_SUBMENU_P(bPreheatOnlyNozzle ? PSTR("ASA  -  " STRINGIFY(ASA_PREHEAT_HOTEND_TEMP)) : PSTR("ASA  -  " STRINGIFY(ASA_PREHEAT_HOTEND_TEMP)  "/" STRINGIFY(ASA_PREHEAT_HPB_TEMP)) , mFilamentItem_ASA);
        MENU_ITEM_SUBMENU_P(bPreheatOnlyNozzle ? PSTR("PC   -  " STRINGIFY(PC_PREHEAT_HOTEND_TEMP))  : PSTR("PC   -  " STRINGIFY(PC_PREHEAT_HOTEND_TEMP)   "/" STRINGIFY(PC_PREHEAT_HPB_TEMP))  , mFilamentItem_PC);
        MENU_ITEM_SUBMENU_P(bPreheatOnlyNozzle ? PSTR("PVB  -  " STRINGIFY(PVB_PREHEAT_HOTEND_TEMP)) : PSTR("PVB  -  " STRINGIFY(PVB_PREHEAT_HOTEND_TEMP)  "/" STRINGIFY(PVB_PREHEAT_HPB_TEMP)) , mFilamentItem_PVB);
        MENU_ITEM_SUBMENU_P(bPreheatOnlyNozzle ? PSTR("PA   -  " STRINGIFY(PA_PREHEAT_HOTEND_TEMP))  : PSTR("PA   -  " STRINGIFY(PA_PREHEAT_HOTEND_TEMP)   "/" STRINGIFY(PA_PREHEAT_HPB_TEMP))  , mFilamentItem_PA);
        MENU_ITEM_SUBMENU_P(bPreheatOnlyNozzle ? PSTR("ABS  -  " STRINGIFY(ABS_PREHEAT_HOTEND_TEMP)) : PSTR("ABS  -  " STRINGIFY(ABS_PREHEAT_HOTEND_TEMP)  "/" STRINGIFY(ABS_PREHEAT_HPB_TEMP)) , mFilamentItem_ABS);
        MENU_ITEM_SUBMENU_P(bPreheatOnlyNozzle ? PSTR("HIPS -  " STRINGIFY(HIPS_PREHEAT_HOTEND_TEMP)): PSTR("HIPS -  " STRINGIFY(HIPS_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(HIPS_PREHEAT_HPB_TEMP)), mFilamentItem_HIPS);
        MENU_ITEM_SUBMENU_P(bPreheatOnlyNozzle ? PSTR("PP   -  " STRINGIFY(PP_PREHEAT_HOTEND_TEMP))  : PSTR("PP   -  " STRINGIFY(PP_PREHEAT_HOTEND_TEMP)   "/" STRINGIFY(PP_PREHEAT_HPB_TEMP))  , mFilamentItem_PP);
        MENU_ITEM_SUBMENU_P(bPreheatOnlyNozzle ? PSTR("FLEX -  " STRINGIFY(FLEX_PREHEAT_HOTEND_TEMP)): PSTR("FLEX -  " STRINGIFY(FLEX_PREHEAT_HOTEND_TEMP) "/" STRINGIFY(FLEX_PREHEAT_HPB_TEMP)), mFilamentItem_FLEX);
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

  lcd_puts_at_P(0, 1, _i("Insert filament"));////MSG_INSERT_FILAMENT c=20
#ifdef FILAMENT_SENSOR
  if (!fsensor.getAutoLoadEnabled())
#endif //FILAMENT_SENSOR
  {
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
  uint16_t slow_seq_time = (FILAMENTCHANGE_FINALFEED * 1000ul) / FILAMENTCHANGE_EFEED_FINAL;
  uint16_t fast_seq_time = (FILAMENTCHANGE_FIRSTFEED * 1000ul) / FILAMENTCHANGE_EFEED_FIRST;
  lcd_loading_progress_bar(slow_seq_time + fast_seq_time); //show progress bar for total time of filament loading fast + slow sequence
}




uint8_t lcd_alright() {
    int8_t enc_dif = 0;
    uint8_t cursor_pos = 1;

    lcd_clear();
    lcd_puts_at_P(0, 0, _i("Changed correctly?"));////MSG_CORRECTLY c=20
    lcd_puts_at_P(1, 1, _T(MSG_YES));
    lcd_puts_at_P(1, 2, _i("Filament not loaded"));////MSG_NOT_LOADED c=19
    lcd_puts_at_P(1, 3, _i("Color not correct"));////MSG_NOT_COLOR c=19
    lcd_putc_at(0, 1, '>');


    enc_dif = lcd_encoder_diff;
    lcd_consume_click();
    while (1)
    {
        manage_heater();
        manage_inactivity(true);

        if (abs(enc_dif - lcd_encoder_diff) >= ENCODER_PULSES_PER_STEP)
        {

            if (enc_dif > lcd_encoder_diff ) {
                // Rotating knob counter clockwise
                cursor_pos--;
            } else if (enc_dif < lcd_encoder_diff) {
                // Rotating knob clockwise
                cursor_pos++;
            }
            if (cursor_pos > 3) {
                cursor_pos = 3;
                Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
            } else if (cursor_pos < 1) {
                cursor_pos = 1;
                Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
            }

            // Update '>' render only
            lcd_puts_at_P(0, 1, PSTR(" \n \n "));
            lcd_putc_at(0, cursor_pos, '>');

            // Consume rotation event and make feedback sound
            enc_dif = lcd_encoder_diff;
            Sound_MakeSound(e_SOUND_TYPE_EncoderMove);
            _delay(100);
        }

        if (lcd_clicked())
        {
            Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
            lcd_clear();
            lcd_return_to_status();
            return cursor_pos;
        }
    };
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
	uint8_t clean = lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_FILAMENT_CLEAN), false, LCD_LEFT_BUTTON_CHOICE);
	while (clean == LCD_MIDDLE_BUTTON_CHOICE) {
		load_filament_final_feed();
		st_synchronize();
		clean = lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_FILAMENT_CLEAN), false, LCD_LEFT_BUTTON_CHOICE);
	}
}

#ifdef FILAMENT_SENSOR
static void lcd_menu_AutoLoadFilament()
{
    lcd_display_message_fullscreen_nonBlocking_P(_i("Autoloading filament is active, just press the knob and insert filament..."));////MSG_AUTOLOADING_ENABLED c=20 r=4
    menu_back_if_clicked();
}
#endif //FILAMENT_SENSOR

static void preheat_or_continue() {
    if (target_temperature[0] >= extrude_min_temp) {
        bFilamentPreheatState = true;
        mFilamentItem(target_temperature[0], target_temperature_bed);
    } else {
        lcd_generic_preheat_menu();
    }
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
			"%10ldh %02dm %02ds"
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
			"%10ldd %02dh %02dm"
            ),
            _i("Total filament"), _filament_m,  ////MSG_TOTAL_FILAMENT c=19
            _i("Total print time"), _days, _hours, _minutes);  ////MSG_TOTAL_PRINT_TIME c=19
        menu_back_if_clicked_fb();
	}
}


static void _lcd_move(const char *name, uint8_t axis, int min, int max)
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
			current_position[axis] += float((int)lcd_encoder);
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
	if ((int)degHotend0() > extrude_min_temp)
	{
		if (lcd_encoder != 0)
		{
			refresh_cmd_timeout();
			if (! planner_queue_full())
			{
				current_position[E_AXIS] += float((int)lcd_encoder);
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
//! |Measured skew :0.00D|	MSG_MEASURED_SKEW c=14
//! | --------------     |	STR_SEPARATOR
//! |Slight skew   :0.12D|	MSG_SLIGHT_SKEW c=14
//! |Severe skew   :0.25D|	MSG_SEVERE_SKEW c=14
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
	 _i("Slight skew"), _deg(bed_skew_angle_mild),  ////MSG_SLIGHT_SKEW c=14
	 _i("Severe skew"), _deg(bed_skew_angle_extreme)  ////MSG_SEVERE_SKEW c=14
	);
	lcd_set_cursor(15, 0);
	if (angleDiff < 100){
		lcd_printf_P(_N("%3.2f\x01"), _deg(angleDiff));
	} else {
		lcd_puts_P(_T(MSG_NA));
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

    for (uint8_t i = 0; i < 2; i++) {
        lcd_set_cursor(0, i + 2);
        lcd_printf_P(PSTR("%c%17.2fmm"), 'X' + i, eeprom_read_float((float*)(EEPROM_BED_CALIBRATION_CENTER+4*i)));
    }
    menu_back_if_clicked();
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
	    if (!calibration_status_get(CALIBRATION_STATUS_LIVE_ADJUST))
			_md->babystepMemZ = 0;

		_md->babystepMemMMZ = _md->babystepMemZ/cs.axis_steps_per_unit[Z_AXIS];
		lcd_draw_update = 1;
		//SERIAL_ECHO("Z baby step: ");
		//SERIAL_ECHO(_md->babystepMem[2]);
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
		menu_draw_float13(_i("Adjusting Z:"), _md->babystepMemMMZ); ////MSG_BABYSTEPPING_Z c=15 // Beware: must include the ':' as its last character
	}
	if (LCD_CLICKED || menu_leaving)
	{
		// Only update the EEPROM when leaving the menu.
          uint8_t active_sheet=eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet));
		eeprom_update_word(reinterpret_cast<uint16_t *>(&(EEPROM_Sheets_base->s[active_sheet].z_offset)),_md->babystepMemZ);

        // NOTE: bed_temp and pinda_temp are not currently read/used anywhere.
		eeprom_update_byte(&(EEPROM_Sheets_base->s[active_sheet].bed_temp),target_temperature_bed);
#ifdef PINDA_THERMISTOR        
		eeprom_update_byte(&(EEPROM_Sheets_base->s[active_sheet].pinda_temp),current_temperature_pinda);
#endif //PINDA_THERMISTOR
		calibration_status_set(CALIBRATION_STATUS_LIVE_ADJUST);
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
	eeprom_adjust_bed_reset();
	_menu_data_adjust_bed_t* _md = (_menu_data_adjust_bed_t*)&(menu_data[0]);
	_md->status = 0;
}

//! @brief Show Bed level correct
//! 
//! @code{.unparsed}
//! |01234567890123456789|
//! |Settings:           |	MSG_SETTINGS
//! |Left side [µm]:     |	MSG_BED_CORRECTION_LEFT
//! |Right side[µm]:     |	MSG_BED_CORRECTION_RIGHT
//! |Front side[µm]:     |	MSG_BED_CORRECTION_FRONT
//! |Rear side [µm]:     |	MSG_BED_CORRECTION_REAR
//! |Reset               |	MSG_BED_CORRECTION_RESET
//! ----------------------
//! @endcode
void lcd_adjust_bed(void)
{
	_menu_data_adjust_bed_t* _md = (_menu_data_adjust_bed_t*)&(menu_data[0]);
    if (_md->status == 0)
	{
        // Menu was entered.
        if (eeprom_read_byte((unsigned char*)EEPROM_BED_CORRECTION_VALID) == 1)
		{
			_md->left = (int8_t)eeprom_read_byte((uint8_t*)EEPROM_BED_CORRECTION_LEFT);
			_md->right = (int8_t)eeprom_read_byte((uint8_t*)EEPROM_BED_CORRECTION_RIGHT);
			_md->front = (int8_t)eeprom_read_byte((uint8_t*)EEPROM_BED_CORRECTION_FRONT);
			_md->rear = (int8_t)eeprom_read_byte((uint8_t*)EEPROM_BED_CORRECTION_REAR);
		}
        _md->status = 1;
    }
    MENU_BEGIN();
	// leaving menu - this condition must be immediately before MENU_ITEM_BACK_P
    ON_MENU_LEAVE(
        eeprom_update_byte((uint8_t*)EEPROM_BED_CORRECTION_LEFT, (uint8_t)_md->left);
        eeprom_update_byte((uint8_t*)EEPROM_BED_CORRECTION_FRONT, (uint8_t)_md->front);
        eeprom_update_byte((uint8_t*)EEPROM_BED_CORRECTION_REAR, (uint8_t)_md->rear);
        eeprom_update_byte((uint8_t*)EEPROM_BED_CORRECTION_RIGHT, (uint8_t)_md->right);
        eeprom_update_byte((uint8_t*)EEPROM_BED_CORRECTION_VALID, 1);
    );
    MENU_ITEM_BACK_P(_T(MSG_BACK));
	MENU_ITEM_EDIT_int3_P(_i("Left side [\xe4m]"),  &_md->left,  -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);////MSG_BED_CORRECTION_LEFT c=14
    MENU_ITEM_EDIT_int3_P(_i("Right side[\xe4m]"), &_md->right, -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);////MSG_BED_CORRECTION_RIGHT c=14
    MENU_ITEM_EDIT_int3_P(_i("Front side[\xe4m]"), &_md->front, -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);////MSG_BED_CORRECTION_FRONT c=14
    MENU_ITEM_EDIT_int3_P(_i("Rear side [\xe4m]"),  &_md->rear,  -BED_ADJUSTMENT_UM_MAX, BED_ADJUSTMENT_UM_MAX);////MSG_BED_CORRECTION_REAR c=14
    MENU_ITEM_FUNCTION_P(_T(MSG_RESET), lcd_adjust_bed_reset);
    MENU_END();
}

//! @brief Show PID Extruder
//! 
//! @code{.unparsed}
//! |01234567890123456789|
//! |Set temperature:    |
//! |                    |
//! | 210                |
//! |                    |
//! ----------------------
//! @endcode
void pid_extruder()
{
	lcd_clear();
	lcd_puts_at_P(0, 0, _i("Set temperature:"));////MSG_SET_TEMPERATURE c=20
	pid_temp += int(lcd_encoder);
	if (pid_temp > HEATER_0_MAXTEMP) pid_temp = HEATER_0_MAXTEMP;
	if (pid_temp < HEATER_0_MINTEMP) pid_temp = HEATER_0_MINTEMP;
	lcd_encoder = 0;
	lcd_set_cursor(1, 2);
	lcd_printf_P(PSTR("%3u"), pid_temp);
	if (lcd_clicked()) {
		lcd_commands_type = LcdCommands::PidExtruder;
		lcd_return_to_status();
		lcd_update(2);
	}

}

#ifdef PINDA_THERMISTOR
bool lcd_wait_for_pinda(float temp) {
	setTargetHotend(0);
	setTargetBed(0);
	LongTimer pinda_timeout;
	pinda_timeout.start();
	bool target_temp_reached = true;

	while (current_temperature_pinda > temp){
		lcd_display_message_fullscreen_P(_i("Waiting for PINDA probe cooling"));////MSG_WAITING_TEMP_PINDA c=20 r=3

		lcd_set_cursor(0, 4);
		lcd_print(LCD_STR_THERMOMETER[0]);
		lcd_printf_P(PSTR("%3d/%3d"), (int16_t)current_temperature_pinda, (int16_t) temp);
		lcd_print(LCD_STR_DEGREE[0]);
		delay_keep_alive(1000);
		serialecho_temperatures();
		if (pinda_timeout.expired(8 * 60 * 1000ul)) { //PINDA cooling from 60 C to 35 C takes about 7 minutes
			target_temp_reached = false;
			break;
		}
	}
	lcd_update_enable(true);
	return target_temp_reached;
}
#endif //PINDA_THERMISTOR

void lcd_wait_for_heater() {
		lcd_display_message_fullscreen_P(_T(MSG_WIZARD_HEATING));
		lcd_set_cursor(0, 4);
		lcd_print(LCD_STR_THERMOMETER[0]);
		lcd_printf_P(PSTR("%3d/%3d"), (int16_t)degHotend(active_extruder), (int16_t) degTargetHotend(active_extruder));
		lcd_print(LCD_STR_DEGREE[0]);
}

void lcd_wait_for_cool_down() {
	setTargetHotend(0);
	setTargetBed(0);
	int fanSpeedBckp = fanSpeed;
	fanSpeed = 255;
	while ((degHotend(0)>MAX_HOTEND_TEMP_CALIBRATION) || (degBed() > MAX_BED_TEMP_CALIBRATION)) {
		lcd_display_message_fullscreen_P(_i("Waiting for nozzle and bed cooling"));////MSG_WAITING_TEMP c=20 r=4

		lcd_set_cursor(0, 4);
		lcd_print(LCD_STR_THERMOMETER[0]);
		lcd_printf_P(PSTR("%3d/0"), (int16_t)degHotend(0));
		lcd_print(LCD_STR_DEGREE[0]);

		lcd_set_cursor(9, 4);
		lcd_print(LCD_STR_BEDTEMP[0]);
		lcd_printf_P(PSTR("%3d/0"), (int16_t)degBed());
		lcd_print(LCD_STR_DEGREE[0]);
		delay_keep_alive(1000);
		serialecho_temperatures();
	}
	fanSpeed = fanSpeedBckp;
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
        const char *msg = only_z
            ? _i("Calibrating Z. Rotate the knob to move the Z carriage up to the end stoppers. Click when done.")////MSG_MOVE_CARRIAGE_TO_THE_TOP_Z c=20 r=8
            : _i("Calibrating XYZ. Rotate the knob to move the Z carriage up to the end stoppers. Click when done.");////MSG_MOVE_CARRIAGE_TO_THE_TOP c=20 r=8
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
                planner_aborted = false;
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
        uint8_t result = lcd_show_fullscreen_message_yes_no_and_wait_P(_i("Are left and right Z~carriages all up?"), false);////MSG_CONFIRM_CARRIAGE_AT_THE_TOP c=20 r=2
        if (result == LCD_BUTTON_TIMEOUT)
            goto canceled;
        else if (result == LCD_LEFT_BUTTON_CHOICE)
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
 * @return rest of the text (to be displayed on next page)
 */
static const char* lcd_display_message_fullscreen_nonBlocking_P(const char *msg)
{
    const char *msgend = msg;
    bool multi_screen = false;
    for (uint8_t row = 0; row < LCD_HEIGHT; ++ row) {
        lcd_set_cursor(0, row);

        // Previous row ended with a complete word, so the first character in the
        // next row is a whitespace. We can skip the whitespace on a new line.
        if (pgm_is_whitespace(msg) && ++msg == nullptr)
        {
            // End of the message.
            break;
        }

        uint8_t linelen = min(strlen_P(msg), LCD_WIDTH);
        const char *msgend2 = msg + linelen;
        msgend = msgend2;
        if (row == 3 && linelen == LCD_WIDTH) {
            // Last line of the display, full line shall be displayed.
            // Find out, whether this message will be split into multiple screens.
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
            else if (c == '\n') {
                // Abort early if '\n' is encontered.
                // This character is used to force the following words to be printed on the next line.
                break;
            }
            lcd_print(c);
        }
    }

    if (multi_screen) {
        // Display the "next screen" indicator character.
        lcd_set_custom_characters_nextpage();
        lcd_set_cursor(19, 3);
        // Display the double down arrow.
        lcd_print(LCD_STR_ARROW_2_DOWN[0]);
    }

    return multi_screen ? msgend : NULL;
}

const char* lcd_display_message_fullscreen_P(const char *msg)
{
    // Disable update of the screen by the usual lcd_update(0) routine.
    lcd_update_enable(false);
    lcd_clear();
    return lcd_display_message_fullscreen_nonBlocking_P(msg);
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
		if (msg_next == NULL) {
			lcd_set_cursor(19, 3);
			// Display the confirm char.
			lcd_print(LCD_STR_CONFIRM[0]);
		}
        // Wait for 5 seconds before displaying the next text.
        for (uint8_t i = 0; i < 100; ++ i) {
            delay_keep_alive(50);
            if (lcd_clicked()) {
				if (msg_next == NULL) {
					KEEPALIVE_STATE(IN_HANDLER);
					lcd_set_custom_characters();
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
//! @param msg Message to show. If NULL, do not clear the screen and handle choice selection only.
//! @param allow_timeouting if true, allows time outing of the screen
//! @param default_selection if 0, 'Yes' choice is selected by default, otherwise 'No' choice is preselected
//! @retval 0 yes choice selected by user
//! @retval 1 no choice selected by user
//! @retval 0xFF button timeout (only possible if allow_timeouting is true)
uint8_t lcd_show_multiscreen_message_yes_no_and_wait_P(const char *msg, bool allow_timeouting, uint8_t default_selection) //currently just max. n*4 + 3 lines supported (set in language header files)
{
    return lcd_show_multiscreen_message_with_choices_and_wait_P(msg, allow_timeouting, default_selection, _T(MSG_YES), _T(MSG_NO), nullptr, 10);
}
//! @brief Show a two-choice prompt on the last line of the LCD
//! @param selected Show first choice as selected if true, the second otherwise
//! @param first_choice text caption of first possible choice
//! @param second_choice text caption of second possible choice
//! @param second_col column on LCD where second choice is rendered.
//! @param third_choice text caption of third, optional, choice.
void lcd_show_choices_prompt_P(uint8_t selected, const char *first_choice, const char *second_choice, uint8_t second_col, const char *third_choice)
{
    lcd_putc_at(0, 3, selected == LCD_LEFT_BUTTON_CHOICE ? '>': ' ');
    lcd_puts_P(first_choice);
    lcd_putc_at(second_col, 3, selected == LCD_MIDDLE_BUTTON_CHOICE ? '>': ' ');
    lcd_puts_P(second_choice);
    if (third_choice) {
        lcd_putc_at(18, 3, selected == LCD_RIGHT_BUTTON_CHOICE ? '>': ' ');
        lcd_puts_P(third_choice);
    }
}

//! @brief Show single or multiple screen message with two possible choices and wait with possible timeout
//! @param msg Message to show. If NULL, do not clear the screen and handle choice selection only.
//! @param allow_timeouting bool, if true, allows time outing of the screen
//! @param default_selection uint8_t, Control which choice is selected first. 0: left most, 1: middle, 2: right most choice. The left most choice is selected by default
//! @param first_choice text caption of first possible choice. Must be in PROGMEM
//! @param second_choice text caption of second possible choice. Must be in PROGMEM
//! @param third_choice text caption of second possible choice. Must be in PROGMEM. When not set to nullptr first_choice and second_choice may not be more than 5 characters long.
//! @param second_col column on LCD where second_choice starts
//! @retval 0 first choice selected by user
//! @retval 1 first choice selected by user
//! @retval 2 third choice selected by user
//! @retval 0xFF button timeout (only possible if allow_timeouting is true)
uint8_t lcd_show_multiscreen_message_with_choices_and_wait_P(
    const char *const msg, bool allow_timeouting, uint8_t default_selection,
    const char *const first_choice, const char *const second_choice, const char *const third_choice,
    uint8_t second_col
) {
    const char *msg_next = msg ? lcd_display_message_fullscreen_P(msg) : NULL;
    bool multi_screen = msg_next != NULL;
    lcd_set_custom_characters_nextpage();

    // Initial status/prompt on single-screen messages
    uint8_t current_selection = default_selection;
    if (!msg_next) {
        lcd_show_choices_prompt_P(current_selection, first_choice, second_choice, second_col, third_choice);
    }
    // Wait for user confirmation or a timeout.
    unsigned long previous_millis_cmd = _millis();
    int8_t enc_dif = lcd_encoder_diff;
    lcd_consume_click();
    KEEPALIVE_STATE(PAUSED_FOR_USER);
    for (;;) {
        for (uint8_t i = 0; i < 100; ++i) {
            delay_keep_alive(50);
            if (allow_timeouting && _millis() - previous_millis_cmd > LCD_TIMEOUT_TO_STATUS) {
                current_selection = LCD_BUTTON_TIMEOUT;
                goto exit;
            }
            if (abs(enc_dif - lcd_encoder_diff) >= ENCODER_PULSES_PER_STEP) {
                if (msg_next == NULL) {
                    if (third_choice) { // third_choice is not nullptr, safe to dereference
                        if (enc_dif > lcd_encoder_diff && current_selection != LCD_LEFT_BUTTON_CHOICE) {
                            // Rotating knob counter clockwise
                            current_selection--;
                        } else if (enc_dif < lcd_encoder_diff && current_selection != LCD_RIGHT_BUTTON_CHOICE) {
                            // Rotating knob clockwise
                            current_selection++;
                        }
                    } else {
                        if (enc_dif > lcd_encoder_diff && current_selection != LCD_LEFT_BUTTON_CHOICE) {
                            // Rotating knob counter clockwise
                            current_selection = LCD_LEFT_BUTTON_CHOICE;
                        } else if (enc_dif < lcd_encoder_diff && current_selection != LCD_MIDDLE_BUTTON_CHOICE) {
                            // Rotating knob clockwise
                            current_selection = LCD_MIDDLE_BUTTON_CHOICE;
                        }
                    }
                    lcd_show_choices_prompt_P(current_selection, first_choice, second_choice, second_col, third_choice);
                    enc_dif = lcd_encoder_diff;
                    Sound_MakeSound(e_SOUND_TYPE_EncoderMove);
                } else {
                    Sound_MakeSound(e_SOUND_TYPE_BlindAlert);
                    break; // turning knob skips waiting loop
                }
            }
            if (lcd_clicked()) {
                Sound_MakeSound(e_SOUND_TYPE_ButtonEcho);
                if (msg_next == NULL) {
                    goto exit;
                } else
                    break;
            }
        }
        if (multi_screen) {
            if (msg_next == NULL) {
                msg_next = msg;
            }
            msg_next = lcd_display_message_fullscreen_P(msg_next);
        }
        if (msg_next == NULL) {
            lcd_show_choices_prompt_P(current_selection, first_choice, second_choice, second_col, third_choice);
        }
    }
exit:
    KEEPALIVE_STATE(IN_HANDLER);
    lcd_set_custom_characters();
    lcd_update_enable(true);
    return current_selection;
}

//! @brief Display and wait for a Yes/No choice using the last line of the LCD
//! @param allow_timeouting if true, allows time outing of the screen
//! @param default_selection if 0, 'Yes' choice is selected by default, otherwise 'No' choice is preselected
//! @retval 0 yes choice selected by user
//! @retval 1 no choice selected by user
//! @retval 0xFF button timeout (only possible if allow_timeouting is true)
uint8_t lcd_show_yes_no_and_wait(bool allow_timeouting, uint8_t default_selection)
{
    return lcd_show_multiscreen_message_yes_no_and_wait_P(NULL, allow_timeouting, default_selection);
}

//! @brief Show single screen message with yes and no possible choices and wait with possible timeout
//! @param msg Message to show. If NULL, do not clear the screen and handle choice selection only.
//! @param allow_timeouting if true, allows time outing of the screen
//! @param default_selection if 0, 'Yes' choice is selected by default, otherwise 'No' choice is preselected
//! @retval 0 yes choice selected by user
//! @retval 1 no choice selected by user
//! @retval 0xFF button timeout (only possible if allow_timeouting is true)
//! @relates lcd_show_yes_no_and_wait
uint8_t lcd_show_fullscreen_message_yes_no_and_wait_P(const char *msg, bool allow_timeouting, uint8_t default_selection)
{
    return lcd_show_multiscreen_message_yes_no_and_wait_P(msg, allow_timeouting, default_selection);
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
            // The left and maybe the center point out of reach. //@todo Why isn't it found in the firmware.map
            msg = _n("XYZ calibration failed. Left front calibration point not reachable.");////MSG_BED_SKEW_OFFSET_DETECTION_FAILED_FRONT_LEFT_FAR c=20 r=8
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
                // The left and maybe the center point out of reach. //@todo Why isn't it found in the firmware.map
                msg = _n("XYZ calibration compromised. Left front calibration point not reachable.");////MSG_BED_SKEW_OFFSET_DETECTION_WARNING_FRONT_LEFT_FAR c=20 r=8
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
		SERIAL_ECHOLNPGM("PINDA calibration done. Continue with pressing the knob.");
		lcd_show_fullscreen_message_and_wait_P(_T(MSG_PINDA_CALIBRATION_DONE));
		eeprom_update_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE, 1);
	}
	else {
		eeprom_update_byte((uint8_t*)EEPROM_CALIBRATION_STATUS_PINDA, 0);
		SERIAL_ECHOLNPGM("PINDA calibration failed. Continue with pressing the knob.");
		lcd_show_fullscreen_message_and_wait_P(_i("PINDA calibration failed"));////MSG_PINDA_CAL_FAILED c=20 r=4
		eeprom_update_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE, 0);
	}
	lcd_update_enable(true);
	lcd_update(2);
}

#ifndef TMC2130
static void lcd_show_end_stops() {
	lcd_puts_at_P(0, 0, (PSTR("End stops diag")));
	lcd_puts_at_P(0, 1, (READ(X_MIN_PIN) ^ (bool)X_MIN_ENDSTOP_INVERTING) ? (PSTR("X1")) : (PSTR("X0")));
	lcd_puts_at_P(0, 2, (READ(Y_MIN_PIN) ^ (bool)Y_MIN_ENDSTOP_INVERTING) ? (PSTR("Y1")) : (PSTR("Y0")));
	lcd_puts_at_P(0, 3, (READ(Z_MIN_PIN) ^ (bool)Z_MIN_ENDSTOP_INVERTING) ? (PSTR("Z1")) : (PSTR("Z0")));
}

static void menu_show_end_stops() {
    lcd_show_end_stops();
    if (LCD_CLICKED) menu_back();
}

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
#endif // not defined TMC2130

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
//! | Int: 000  Xd:+00000|
//! |Shut: 000  Yd:+00000|
//! ----------------------
//! @endcode
static void lcd_show_sensors_state()
{
	//0: N/A; 1: OFF; 2: ON
	uint8_t pinda_state = STATE_NA;
	uint8_t finda_state = STATE_NA;
	uint8_t idler_state = STATE_NA;

	pinda_state = READ(Z_MIN_PIN);
	if (MMU2::mmu2.Enabled())
	{
		finda_state = MMU2::mmu2.FindaDetectsFilament();
	}
	lcd_puts_at_P(0, 0, MSG_PINDA);
	lcd_set_cursor(LCD_WIDTH - 14, 0);
	lcd_print_state(pinda_state);
	
	if (MMU2::mmu2.Enabled())
	{
		lcd_puts_at_P(10, 0, _n("FINDA"));////MSG_FINDA c=5
		lcd_set_cursor(LCD_WIDTH - 3, 0);
		lcd_print_state(finda_state);
	}
#ifdef FILAMENT_SENSOR
	idler_state = fsensor.getFilamentPresent();
	lcd_puts_at_P(0, 1, _T(MSG_FSENSOR));
	lcd_set_cursor(LCD_WIDTH - 3, 1);
	lcd_print_state(idler_state);
#endif //FILAMENT_SENSOR

#if defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
    // Display X and Y difference from Filament sensor
    // Display Light intensity from Filament sensor
    //  Frame_Avg register represents the average brightness of all pixels within a frame (324 pixels). This
    //  value ranges from 0(darkest) to 255(brightest).
    // Display LASER shutter time from Filament sensor
    //  Shutter register is an index of LASER shutter time. It is automatically controlled by the chip's internal
    //  auto-exposure algorithm. When the chip is tracking on a reflective surface, the Shutter is small.
    //  When the chip is tracking on a surface that absorbs IR (or doesn't reflect it), the Shutter is large.
    //  The maximum value of the shutter is 17. The value of 16 seems to be reported as 17 even though the
    //  Brightness value changes correctly as if the shutter changed to 16 (probably some bug with the sensor).
    //  The shutter algorithm tries to keep the B value in the 70-110 range.
    lcd_set_cursor(0, 2);
    lcd_printf_P(_N("B: %3d     Xd:%6d\n"
                    "S: %3d     Yd:%6d"),
                 pat9125_b, pat9125_x,
                 pat9125_s, pat9125_y);
#endif //defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
}

void lcd_menu_show_sensors_state()                // NOT static due to using inside "Marlin_main" module ("manage_inactivity()")
{
	lcd_timeoutToStatus.stop();
	lcd_show_sensors_state();
	menu_back_if_clicked();
}

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

#ifdef SDCARD_SORT_ALPHA
static void lcd_sort_type_set() {
	uint8_t sdSort;
	sdSort = eeprom_read_byte((uint8_t*) EEPROM_SD_SORT);
	switch (sdSort) {
		case SD_SORT_TIME: sdSort = SD_SORT_ALPHA; break;
		case SD_SORT_ALPHA: sdSort = SD_SORT_NONE; break;
		default: sdSort = SD_SORT_TIME;
	}
	eeprom_update_byte((uint8_t*)EEPROM_SD_SORT, sdSort);
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
	if (IS_SD_PRINTING || usb_timer.running() || (lcd_commands_type == LcdCommands::Layer1Cal)) menu_goto(lcd_tune_menu, 9, true, true);
	else menu_goto(lcd_settings_menu, 9, true, true);
}
#endif //TMC2130

#if (LANG_MODE != 0)

void menu_setlang(unsigned char lang)
{
	if (!lang_select(lang))
	{
		if (lcd_show_fullscreen_message_yes_no_and_wait_P(_i("Copy selected language?"), false, LCD_LEFT_BUTTON_CHOICE) == LCD_LEFT_BUTTON_CHOICE)////MSG_COPY_SEL_LANG c=20 r=3
			lang_boot_update_start(lang);
		lcd_update_enable(true);
		lcd_clear();
		menu_goto(lcd_language_menu, 0, true, true);
		lcd_timeoutToStatus.stop(); //infinite timeout
		lcd_draw_update = 2;
	}
}

#ifdef COMMUNITY_LANGUAGE_SUPPORT
#ifdef XFLASH
static void lcd_community_language_menu()
{
	MENU_BEGIN();
	uint8_t cnt = lang_get_count();
	MENU_ITEM_BACK_P(_T(MSG_SELECT_LANGUAGE)); //Back to previous Menu
	for (int i = 8; i < cnt; i++) //all community languages
		if (menu_item_text_P(lang_get_name_by_code(lang_get_code(i))))
		{
			menu_setlang(i);
			return;
		}
	MENU_END();
}
#endif //XFLASH
#endif //COMMUNITY_LANGUAGE_SUPPORT && W52X20CL



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

#ifdef COMMUNITY_LANGUAGE_SUPPORT
#ifdef XFLASH
		MENU_ITEM_SUBMENU_P(_T(MSG_COMMUNITY_MADE), lcd_community_language_menu); 
#endif //XFLASH
#endif //COMMUNITY_LANGUAGE_SUPPORT && W52X20CL

	MENU_END();
}
#endif //(LANG_MODE != 0)


void lcd_mesh_bedleveling()
{
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

void lcd_toshiba_flash_air_compatibility_toggle()
{
   card.ToshibaFlashAir_enable(! card.ToshibaFlashAir_isEnabled());
   eeprom_update_byte((uint8_t*)EEPROM_TOSHIBA_FLASH_AIR_COMPATIBLITY, card.ToshibaFlashAir_isEnabled());
}

//! @brief Continue first layer calibration with previous value or start from zero?
//!
//! @code{.unparsed}
//! |01234567890123456789|
//! |Sheet        Smooth1|  MSG_SHEET c=12, MSG_SHEET_NAME c=7
//! |Z offset:   -1.480mm|  MSG_Z_OFFSET c=11
//! |>Continue           |  MSG_CONTINUE
//! | Reset              |  MSG_RESET
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
            (!calibration_status_get(CALIBRATION_STATUS_LIVE_ADJUST)) ||
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
    lcd_printf_P(_i("Sheet %.7s\nZ offset: %+1.3fmm\n%cContinue\n%cReset"),////MSG_SHEET_OFFSET c=20 r=4
            sheet_name, offset, menuData->reset ? ' ' : '>', menuData->reset ? '>' : ' ');// \n denotes line break, %.7s is replaced by 7 character long sheet name, %+1.3f is replaced by 6 character long floating point number, %c is replaced by > or white space (one character) based on whether first or second option is selected. % denoted place holders can not be reordered. 

}

void lcd_v2_calibration()
{
	if (MMU2::mmu2.Enabled())
	{
	    const uint8_t filament = choose_menu_P(
            _T(MSG_SELECT_FILAMENT),
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
#ifdef FILAMENT_SENSOR
	else if (!eeprom_read_byte((uint8_t*)EEPROM_WIZARD_ACTIVE))
	{
	    bool loaded = false;
	    if (fsensor.isReady())
	    {
	        loaded = fsensor.getFilamentPresent();
	    }
	    else
	    {
	        loaded = !lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_FILAMENT_LOADED), false, LCD_MIDDLE_BUTTON_CHOICE);
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
#endif //FILAMENT_SENSOR

	eFilamentAction = FilamentAction::Lay1Cal;
	menu_goto(lcd_generic_preheat_menu, 0, true, true);
}

void lcd_wizard() {
	bool result = true;
	if (calibration_status_get(CALIBRATION_WIZARD_STEPS)) {
		// calibration already performed: ask before clearing the previous status
		result = !lcd_show_multiscreen_message_yes_no_and_wait_P(_i("Running Wizard will delete current calibration results and start from the beginning. Continue?"), false);////MSG_WIZARD_RERUN c=20 r=7
	}
	if (result) {
		calibration_status_clear(CALIBRATION_WIZARD_STEPS);
		lcd_wizard(WizState::Run);
	} else {
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
		delay_keep_alive(50);
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
    while (fabs(degHotend(0) - degTargetHotend(0)) > TEMP_HYSTERESIS) {
        lcd_display_message_fullscreen_P(_T(MSG_WIZARD_HEATING));

        lcd_set_cursor(0, 4);
	    //Print the hotend temperature (9 chars total)
		lcdui_print_temp(LCD_STR_THERMOMETER[0], (int)(degHotend(0) + 0.5), (int)(degTargetHotend(0) + 0.5));
        delay_keep_alive(1000);
    }
	
}

static void lcd_wizard_load() {
    if (MMU2::mmu2.Enabled()) {
        lcd_show_fullscreen_message_and_wait_P(
            _i("Please insert filament into the first tube of the MMU, then press the knob to load it.")); ////MSG_MMU_INSERT_FILAMENT_FIRST_TUBE c=20 r=6
        // NOTE: a full screen message showing which filament is being inserted
        // is performed by M701. For this reason MSG_LOADING_FILAMENT is not
        // used here when a MMU is used.
    } else {
        lcd_show_fullscreen_message_and_wait_P(
            _i("Please insert filament into the extruder, then press the knob to load it.")); ////MSG_WIZARD_LOAD_FILAMENT c=20 r=6
        lcd_update_enable(false);
        lcd_clear();
        lcd_puts_at_P(0, 2, _T(MSG_LOADING_FILAMENT));
        loading_flag = true;
    }
    gcode_M701(FILAMENTCHANGE_FIRSTFEED, 0);
    //enquecommand_P(PSTR("M701"));
}

static void wizard_lay1cal_message(bool cold)
{
    lcd_show_fullscreen_message_and_wait_P(
            _i("Now I will calibrate distance between tip of the nozzle and heatbed surface.")); ////MSG_WIZARD_V2_CAL c=20 r=8
    if (MMU2::mmu2.Enabled())
    {
        lcd_show_fullscreen_message_and_wait_P(
                _i("Select a filament for the First Layer Calibration and select it in the on-screen menu."));////MSG_SELECT_FIL_1ST_LAYERCAL c=20 r=7
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
	uint8_t wizard_event;
	// Make sure EEPROM_WIZARD_ACTIVE is true if entering using different entry point
	// other than WizState::Run - it is useful for debugging wizard.
	if (state != S::Run) eeprom_update_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 1);
    
    FORCE_BL_ON_START;
	
    while (!end) {
		printf_P(PSTR("Wizard state: %d\n"), (uint8_t)state);
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
				// printer pre-assembled: finish remaining steps
				lcd_show_fullscreen_message_and_wait_P(_T(MSG_WIZARD_WELCOME_SHIPPING));
				state = S::Restore;
			} else {
				// new printer, factory reset or manual invocation
				wizard_event = lcd_show_multiscreen_message_yes_no_and_wait_P(_T(MSG_WIZARD_WELCOME), false, LCD_LEFT_BUTTON_CHOICE);
				if (wizard_event == LCD_LEFT_BUTTON_CHOICE) {
					state = S::Restore;
					eeprom_update_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 1);
				} else {
					// user interrupted
					eeprom_update_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 0);
					end = true;
				}
			}
			break;
		case S::Restore:
			// clear any previous error for make _new_ errors visible
			lcd_reset_alert_level();

			// determine the next step in the required order
			if (!calibration_status_get(CALIBRATION_STATUS_SELFTEST)) {
				state = S::Selftest;
			} else if (!calibration_status_get(CALIBRATION_STATUS_XYZ)) {
				// S::Xyz *includes* S::Z so it needs to come before
				// to avoid repeating Z alignment
				state = S::Xyz;
			} else if (!calibration_status_get(CALIBRATION_STATUS_Z)) {
				state = S::Z;
#ifdef TEMP_MODEL
			} else if (!calibration_status_get(CALIBRATION_STATUS_TEMP_MODEL)) {
				state = S::TempModel;
#endif //TEMP_MODEL
			} else if (!calibration_status_get(CALIBRATION_STATUS_LIVE_ADJUST)) {
				state = S::IsFil;
			} else {
				// all required steps completed, finish successfully
				state = S::Finish;
			}
			break;
		case S::Selftest:
			lcd_show_fullscreen_message_and_wait_P(_i("First, I will run the selftest to check most common assembly problems."));////MSG_WIZARD_SELFTEST c=20 r=8
			wizard_event = lcd_selftest();
			state = (wizard_event ? S::Restore : S::Failed);
			break;
		case S::Xyz:
			lcd_show_fullscreen_message_and_wait_P(_i("I will run xyz calibration now. It will take up to 24 mins."));////MSG_WIZARD_XYZ_CAL c=20 r=8
			wizard_event = gcode_M45(false, 0);
			state = (wizard_event ? S::Restore : S::Failed);
			break;
		case S::Z:
			lcd_show_fullscreen_message_and_wait_P(_i("Please remove shipping helpers first."));////MSG_REMOVE_SHIPPING_HELPERS c=20 r=3
			lcd_show_fullscreen_message_and_wait_P(_i("Now remove the test print from steel sheet."));////MSG_REMOVE_TEST_PRINT c=20 r=4
			wizard_event = lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_STEEL_SHEET_CHECK), false);
			if (wizard_event == LCD_MIDDLE_BUTTON_CHOICE) {
				lcd_show_fullscreen_message_and_wait_P(_T(MSG_PLACE_STEEL_SHEET));
			}
			lcd_show_fullscreen_message_and_wait_P(_i("I will run z calibration now."));////MSG_WIZARD_Z_CAL c=20 r=8
			wizard_event = gcode_M45(true, 0);
			if (!wizard_event) {
				state = S::Failed;
			} else {
				raise_z_above(MIN_Z_FOR_SWAP);
				//current filament needs to be unloaded and then new filament should be loaded
				//start to preheat nozzle for unloading remaining PLA filament
				setTargetHotend(PLA_PREHEAT_HOTEND_TEMP);
				lcd_display_message_fullscreen_P(_i("Now I will preheat nozzle for PLA.")); ////MSG_WIZARD_WILL_PREHEAT c=20 r=4
				wait_preheat();
				//unload current filament
				unload_filament(FILAMENTCHANGE_FINALRETRACT);
				//load filament
				lcd_wizard_load();
				setTargetHotend(0); //we are finished, cooldown nozzle
				state = S::Restore;
			}
			break;
#ifdef TEMP_MODEL
		case S::TempModel:
			lcd_show_fullscreen_message_and_wait_P(_i("Thermal model cal. takes approx. 12 mins. See\nprusa.io/tm-cal"));////MSG_TM_CAL c=20 r=4
			lcd_commands_type = LcdCommands::TempModel;
			end = true; // Leave wizard temporarily for TM cal.
			break;
#endif //TEMP_MODEL
		case S::IsFil:
		    //start to preheat nozzle and bed to save some time later
			setTargetHotend(PLA_PREHEAT_HOTEND_TEMP);
			setTargetBed(PLA_PREHEAT_HPB_TEMP);
			wizard_event = lcd_show_fullscreen_message_yes_no_and_wait_P(_T(MSG_FILAMENT_LOADED), false, LCD_MIDDLE_BUTTON_CHOICE);
			if (wizard_event == LCD_LEFT_BUTTON_CHOICE) {
				state = S::Lay1CalCold;
			} else { // MIDDLE_BUTTON_CHOICE
			    if(MMU2::mmu2.Enabled()) state = S::LoadFilCold;
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
			if (wizard_event == LCD_LEFT_BUTTON_CHOICE)
			{
				lcd_show_fullscreen_message_and_wait_P(_i("Please clean heatbed and then press the knob."));////MSG_WIZARD_CLEAN_HEATBED c=20 r=8
				state = S::Lay1CalCold;
			}
			else
			{
				lcd_show_fullscreen_message_and_wait_P(_i("If you have additional steel sheets, calibrate their presets in Settings - HW Setup - Steel sheets."));////MSG_ADDITIONAL_SHEETS c=20 r=9
				state = S::Restore;
			}
			break;
		case S::Finish:
		case S::Failed:
			eeprom_update_byte((uint8_t*)EEPROM_WIZARD_ACTIVE, 0);
			end = true;
			break;
		}
	}
    
    FORCE_BL_ON_END;
    
    const char *msg = NULL;
	printf_P(_N("Wizard end state: %d\n"), (uint8_t)state);
	switch (state) {
	case S::Run:
		// user interrupted
		msg = _T(MSG_WIZARD_QUIT);
		break;

	case S::Finish:
		// we are successfully finished
		msg = _T(MSG_WIZARD_DONE);
		lcd_reset_alert_level();
		lcd_setstatuspgm(MSG_WELCOME);
		lcd_return_to_status();
		break;

	case S::Failed:
		// aborted due to failure
		msg = _T(MSG_WIZARD_CALIBRATION_FAILED);
		break;

	default:
		// exiting for later re-entry
		break;
	}
	if (msg) {
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

static void fsensor_reinit() {
    fsensor.init();
}

static void lcd_fsensor_enabled_set(void) {
    fsensor.setEnabled(!fsensor.isEnabled());
}

static void lcd_fsensor_runout_set() {
    fsensor.setRunoutEnabled(!fsensor.getRunoutEnabled(), true);
}

static void lcd_fsensor_autoload_set() {
    fsensor.setAutoLoadEnabled(!fsensor.getAutoLoadEnabled(), true);
}

#if FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125
static void lcd_fsensor_jam_detection_set() {
    fsensor.setJamDetectionEnabled(!fsensor.getJamDetectionEnabled(), true);
}
#endif //FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125

static void lcd_fsensor_actionNA_set(void)
{
    Filament_sensor::SensorActionOnError act = fsensor.getActionOnError();
    switch(act) {
        case Filament_sensor::SensorActionOnError::_Continue:
            act = Filament_sensor::SensorActionOnError::_Pause;
            break;
        case Filament_sensor::SensorActionOnError::_Pause:
            act = Filament_sensor::SensorActionOnError::_Continue;
            break;
        default:
            act = Filament_sensor::SensorActionOnError::_Continue;
    }
    fsensor.setActionOnError(act, true);
}

static void lcd_fsensor_settings_menu() {
    MENU_BEGIN();
    MENU_ITEM_BACK_P(_T(MSG_BACK));
    
    MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR), fsensor.isEnabled() ? _T(MSG_ON) : _T(MSG_OFF), lcd_fsensor_enabled_set);
    
    if (fsensor.isEnabled()) {
        if (fsensor.isError()) {
            MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR_RUNOUT), _T(MSG_NA), fsensor_reinit);
            MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR_AUTOLOAD), _T(MSG_NA), fsensor_reinit);
#if defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
            MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR_JAM_DETECTION), _T(MSG_NA), fsensor_reinit);
#endif //defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
        }
        else {
            MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR_RUNOUT), fsensor.getRunoutEnabled() ? _T(MSG_ON) : _T(MSG_OFF), lcd_fsensor_runout_set);
            MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR_AUTOLOAD), fsensor.getAutoLoadEnabled() ? _T(MSG_ON) : _T(MSG_OFF), lcd_fsensor_autoload_set);
#if defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
            MENU_ITEM_TOGGLE_P(_T(MSG_FSENSOR_JAM_DETECTION), fsensor.getJamDetectionEnabled() ? _T(MSG_ON) : _T(MSG_OFF), lcd_fsensor_jam_detection_set);
#endif //defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125)
        }
        
        switch(fsensor.getActionOnError()) {
            case Filament_sensor::SensorActionOnError::_Continue:
                MENU_ITEM_TOGGLE_P(_T(MSG_FS_ACTION), _T(MSG_CONTINUE_SHORT), lcd_fsensor_actionNA_set);
                break;
            case Filament_sensor::SensorActionOnError::_Pause:
                MENU_ITEM_TOGGLE_P(_T(MSG_FS_ACTION), _T(MSG_PAUSE), lcd_fsensor_actionNA_set);
                break;
            default:
                lcd_fsensor_actionNA_set();
        }
    }
    
    MENU_END();
}

#endif //FILAMENT_SENSOR

static void settingsSpoolJoin()
{
    MENU_ITEM_TOGGLE_P(MSG_SPOOL_JOIN, SpoolJoin::spooljoin.isSpoolJoinEnabled() ? _T(MSG_ON) : _T(MSG_OFF), SpoolJoin::spooljoin.toggleSpoolJoin);
}

#define SETTINGS_SPOOLJOIN \
do\
{\
    settingsSpoolJoin();\
}\
while(0)\

#ifdef MMU_HAS_CUTTER
static void settingsCutter()
{
    if (EEPROM_MMU_CUTTER_ENABLED_enabled == eeprom_read_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED))
    {
        MENU_ITEM_TOGGLE_P(_T(MSG_CUTTER), _T(MSG_ON), lcd_cutter_enabled);
    }
#ifdef MMU_ALWAYS_CUT
    else if (EEPROM_MMU_CUTTER_ENABLED_always == eeprom_read_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED))
    {
        MENU_ITEM_TOGGLE_P(_T(MSG_CUTTER), _T(MSG_ALWAYS), lcd_cutter_enabled);
    }
#endif
    else
    {
        MENU_ITEM_TOGGLE_P(_T(MSG_CUTTER), _T(MSG_OFF), lcd_cutter_enabled);
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

static void mmu_enable_switch()
{
    uint8_t current_state = eeprom_read_byte((uint8_t *)EEPROM_MMU_ENABLED);
    // EEPROM update is handled by the stop and start functions.
    if (current_state)
    {
        MMU2::mmu2.Stop();
    }
    else
    {
        MMU2::mmu2.Start();
    }
}

static void mmu_reset()
{
    MMU2::mmu2.Reset(MMU2::MMU2::ResetForm::Software);
}

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
	if (MMU2::mmu2.Enabled())\
	{\
		if (SilentModeMenu_MMU == 0) MENU_ITEM_TOGGLE_P(_T(MSG_MMU_MODE), _T(MSG_NORMAL), lcd_silent_mode_mmu_set);\
		else MENU_ITEM_TOGGLE_P(_T(MSG_MMU_MODE), _T(MSG_STEALTH), lcd_silent_mode_mmu_set);\
	}\
}\
while (0) 
#else //MMU_FORCE_STEALTH_MODE
#define SETTINGS_MMU_MODE
#endif //MMU_FORCE_STEALTH_MODE

/*
#define SETTINGS_MBL_MODE \
do\
{\
    switch(e_mbl_type)\
    {\
    case e_MBL_FAST:\
        MENU_ITEM_FUNCTION_P(_n("Mode    [Fast]"),mbl_mode_set);\
         break; \
    case e_MBL_OPTIMAL:\
	    MENU_ITEM_FUNCTION_P(_n("Mode [Optimal]"), mbl_mode_set); \
	     break; \
    case e_MBL_PREC:\
	     MENU_ITEM_FUNCTION_P(_n("Mode [Precise]"), mbl_mode_set); \
	     break; \
    default:\
	     MENU_ITEM_FUNCTION_P(_n("Mode [Optimal]"), mbl_mode_set); \
	     break; \
    }\
}\
while (0)
*/

#define SETTINGS_MMU_LOADING_TEST \
do\
{\
    MENU_ITEM_SUBMENU_P(_T(MSG_LOADING_TEST), mmu_loading_test_menu); \
}\
while (0)

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
    switch(oNozzleDiameter)\
    {\
        case ClNozzleDiameter::_Diameter_250: MENU_ITEM_TOGGLE_P(_T(MSG_NOZZLE_DIAMETER), PSTR("0.25"), lcd_nozzle_diameter_cycle); break;\
        case ClNozzleDiameter::_Diameter_Undef: \
        case ClNozzleDiameter::_Diameter_400: MENU_ITEM_TOGGLE_P(_T(MSG_NOZZLE_DIAMETER), PSTR("0.40"), lcd_nozzle_diameter_cycle); break;\
        case ClNozzleDiameter::_Diameter_600: MENU_ITEM_TOGGLE_P(_T(MSG_NOZZLE_DIAMETER), PSTR("0.60"), lcd_nozzle_diameter_cycle); break;\
        case ClNozzleDiameter::_Diameter_800: MENU_ITEM_TOGGLE_P(_T(MSG_NOZZLE_DIAMETER), PSTR("0.80"), lcd_nozzle_diameter_cycle); break;\
    }\
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
              MENU_ITEM_TOGGLE_P(MSG_FIRMWARE, _T(MSG_NONE), lcd_check_version_set);\
              break;\
         case ClCheckVersion::_Warn:\
              MENU_ITEM_TOGGLE_P(MSG_FIRMWARE, _T(MSG_WARN), lcd_check_version_set);\
              break;\
         case ClCheckVersion::_Strict:\
              MENU_ITEM_TOGGLE_P(MSG_FIRMWARE, _T(MSG_STRICT), lcd_check_version_set);\
              break;\
         default:\
              MENU_ITEM_TOGGLE_P(MSG_FIRMWARE, _T(MSG_NONE), lcd_check_version_set);\
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
MENU_END();
}

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

static void nozzle_change()
{
    lcd_commands_type = LcdCommands::NozzleCNG;
    lcd_return_to_status();
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
        _md->experimental_menu_visibility = eeprom_init_default_byte((uint8_t *)EEPROM_EXPERIMENTAL_VISIBILITY, 0);
    }


    MENU_BEGIN();
    MENU_ITEM_BACK_P(_T(bSettings?MSG_SETTINGS:MSG_BACK)); // i.e. default menu-item / menu-item after checking mismatch

    MENU_ITEM_SUBMENU_P(_T(MSG_STEEL_SHEETS), sheets_menu);
    SETTINGS_NOZZLE;
    MENU_ITEM_FUNCTION_P(_T(MSG_NOZZLE_CNG_MENU),nozzle_change);
    MENU_ITEM_SUBMENU_P(_i("Checks"), lcd_checking_menu);  ////MSG_CHECKS c=18

#if defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
    //! Fsensor Detection isn't ready for mmu yet it is temporarily disabled.
    //! @todo Don't forget to remove this as soon Fsensor Detection works with mmu
    if(!MMU2::mmu2.Enabled()) MENU_ITEM_FUNCTION_P(PSTR("Fsensor Detection"), lcd_detect_IRsensor);
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
	SilentModeMenu = eeprom_read_byte((uint8_t*) EEPROM_SILENT);
	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN));

	MENU_ITEM_SUBMENU_P(_i("Temperature"), lcd_control_temperature_menu);////MSG_TEMPERATURE c=18

	if (!printer_active() || isPrintPaused)
    {
	    MENU_ITEM_SUBMENU_P(_i("Move axis"), lcd_move_menu_axis);////MSG_MOVE_AXIS c=18
	    MENU_ITEM_GCODE_P(_i("Disable steppers"), PSTR("M84"));////MSG_DISABLE_STEPPERS c=18
    }

#ifdef FILAMENT_SENSOR
    MENU_ITEM_SUBMENU_P(_T(MSG_FSENSOR), lcd_fsensor_settings_menu);
#endif //FILAMENT_SENSOR

    MENU_ITEM_TOGGLE_P(PSTR("MMU"), eeprom_read_byte((uint8_t *)EEPROM_MMU_ENABLED) ? _T(MSG_ON) : _T(MSG_OFF), mmu_enable_switch);

    if (MMU2::mmu2.Enabled())
    { // Only show menus when communicating with MMU
        SETTINGS_SPOOLJOIN;
        SETTINGS_CUTTER;
        SETTINGS_MMU_MODE;
        SETTINGS_MMU_LOADING_TEST;
    }

    if (eeprom_read_byte((uint8_t *)EEPROM_MMU_ENABLED))
    { // Communication with MMU not required to reset MMU
        MENU_ITEM_FUNCTION_P(PSTR("Reset MMU"), mmu_reset);
    }

	MENU_ITEM_TOGGLE_P(_T(MSG_FANS_CHECK), fans_check_enabled ? _T(MSG_ON) : _T(MSG_OFF), lcd_set_fan_check);

	SETTINGS_SILENT_MODE;

    if(!farm_mode)
    {
        bSettings=true;                              // flag ('fake parameter') for 'lcd_hw_setup_menu()' function
        MENU_ITEM_SUBMENU_P(_T(MSG_HW_SETUP), lcd_hw_setup_menu);
    } else MENU_ITEM_FUNCTION_P(_T(MSG_NOZZLE_CNG_MENU),nozzle_change);

	MENU_ITEM_SUBMENU_P(_T(MSG_MESH_BED_LEVELING), lcd_mesh_bed_leveling_settings);

#if defined (TMC2130) && defined (LINEARITY_CORRECTION)
    MENU_ITEM_SUBMENU_P(_i("Lin. correction"), lcd_settings_linearity_correction_menu);////MSG_LIN_CORRECTION c=18
#endif //LINEARITY_CORRECTION && TMC2130
#ifdef PINDA_THERMISTOR
    if(has_temperature_compensation())
        MENU_ITEM_TOGGLE_P(_T(MSG_PINDA_CALIBRATION), eeprom_read_byte((unsigned char *)EEPROM_TEMP_CAL_ACTIVE) ? _T(MSG_ON) : _T(MSG_OFF), lcd_temp_calibration_set);
#endif
#ifdef HAS_SECOND_SERIAL_PORT
    MENU_ITEM_TOGGLE_P(_T(MSG_RPI_PORT), (selectedSerialPort == 0) ? _T(MSG_OFF) : _T(MSG_ON), lcd_second_serial_set);
#endif //HAS_SECOND_SERIAL

	if (!isPrintPaused && !homing_flag && !mesh_bed_leveling_flag)
		MENU_ITEM_SUBMENU_P(_T(MSG_BABYSTEP_Z), lcd_babystep_z);

#if (LANG_MODE != 0)
	MENU_ITEM_SUBMENU_P(_T(MSG_SELECT_LANGUAGE), lcd_language_menu);
#endif //(LANG_MODE != 0)

    if (!farm_mode) { //SD related settings are not available in farm mode
        if (card.ToshibaFlashAir_isEnabled())
            MENU_ITEM_TOGGLE_P(_T(MSG_SD_CARD), MSG_TOSHIBA_FLASH_AIR_COMPATIBILITY, lcd_toshiba_flash_air_compatibility_toggle);
        else
            MENU_ITEM_TOGGLE_P(_T(MSG_SD_CARD), _T(MSG_NORMAL), lcd_toshiba_flash_air_compatibility_toggle);
#ifdef SDCARD_SORT_ALPHA
        switch (eeprom_read_byte((uint8_t*) EEPROM_SD_SORT)) {
            case SD_SORT_TIME: MENU_ITEM_TOGGLE_P(_T(MSG_SORT), _T(MSG_SORT_TIME), lcd_sort_type_set); break;
            case SD_SORT_ALPHA: MENU_ITEM_TOGGLE_P(_T(MSG_SORT), _T(MSG_SORT_ALPHA), lcd_sort_type_set); break;
            default: MENU_ITEM_TOGGLE_P(_T(MSG_SORT), _T(MSG_NONE), lcd_sort_type_set);
        }
    }
#endif //SDCARD_SORT_ALPHA

	SETTINGS_SOUND;

#ifdef LCD_BL_PIN
    if (backlightSupport)
    {
        MENU_ITEM_SUBMENU_P(_T(MSG_BRIGHTNESS), lcd_backlight_menu);
    }
#endif //LCD_BL_PIN

    //! Enables/disables the bed heating while heating the nozzle for loading/unloading filament
    MENU_ITEM_TOGGLE_P(_N("HBed on load"), eeprom_read_byte((uint8_t *)EEPROM_HEAT_BED_ON_LOAD_FILAMENT) ? _T(MSG_YES) : _T(MSG_NO), lcd_heat_bed_on_load_toggle); ////MSG_HEAT_BED_ON_LOAD c=12

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
         MENU_ITEM_SUBMENU_P(_T(MSG_V2_CALIBRATION), lcd_first_layer_calibration_reset);
    }
	MENU_ITEM_GCODE_P(_T(MSG_AUTO_HOME), PSTR("G28 W"));
#ifdef TMC2130
	MENU_ITEM_FUNCTION_P(_i("Belt test"), lcd_belttest_v);////MSG_BELTTEST c=18
#endif //TMC2130
	MENU_ITEM_FUNCTION_P(_i("Selftest"), lcd_selftest_v);////MSG_SELFTEST c=18
    // MK2
    MENU_ITEM_FUNCTION_P(_i("Calibrate XYZ"), lcd_mesh_calibration);////MSG_CALIBRATE_BED c=18
    // "Calibrate Z" with storing the reference values to EEPROM.
    MENU_ITEM_SUBMENU_P(_T(MSG_HOMEYZ), lcd_mesh_calibration_z);

    MENU_ITEM_SUBMENU_P(_T(MSG_MESH_BED_LEVELING), lcd_mesh_bedleveling); ////MSG_MESH_BED_LEVELING c=18

    MENU_ITEM_SUBMENU_P(_i("Bed level correct"), lcd_adjust_bed);////MSG_BED_CORRECTION_MENU c=18
	MENU_ITEM_SUBMENU_P(_i("PID calibration"), pid_extruder);////MSG_PID_EXTRUDER c=17
#ifndef TMC2130
    MENU_ITEM_SUBMENU_P(_i("Show end stops"), menu_show_end_stops);////MSG_SHOW_END_STOPS c=18
#endif
    MENU_ITEM_GCODE_P(_i("Reset XYZ calibr."), PSTR("M44"));////MSG_CALIBRATE_BED_RESET c=18
#ifdef PINDA_THERMISTOR
    if(has_temperature_compensation())
	    MENU_ITEM_FUNCTION_P(_T(MSG_PINDA_CALIBRATION), lcd_calibrate_pinda);
#endif
  }
#ifdef TEMP_MODEL
    MENU_ITEM_SUBMENU_P(_n("Thermal Model cal."), lcd_temp_model_cal);
#endif //TEMP_MODEL
  
  MENU_END();
}

//! @brief Select one of numbered items
//!
//! Create list of items with header. Header can not be selected.
//! Each item has text description passed by function parameter and
//! number. There are 5 numbered items, if MMU2::mmu2.Enabled(), 4 otherwise.
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
    const int8_t items_no = last_item?(MMU2::mmu2.Enabled()?6:5):(MMU2::mmu2.Enabled()?5:4);
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
	static int8_t first = 0;
	int8_t enc_dif = 0;
	char cursor_pos = 0;

	const char *const item[] = {
		PSTR("Language"),
		PSTR("Statistics"),
		PSTR("Shipping prep"),
		PSTR("Service prep"),
		PSTR("All Data"),
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
					if (first < (uint8_t)(sizeof(item) / sizeof(item[0])) - 4) {
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
	uint8_t disable = lcd_show_fullscreen_message_yes_no_and_wait_P(PSTR("Disable farm mode?"), true); //allow timeouting, default no
	if (disable == LCD_LEFT_BUTTON_CHOICE)
	{
		enquecommand_P(PSTR("G99"));
		lcd_return_to_status();
	}
	lcd_update_enable(true);
	lcd_draw_update = 2;
	
}

static inline void load_all_wrapper(){
    for(uint8_t i = 0; i < 5; ++i){
        MMU2::mmu2.load_filament(i);
    }
}

static inline void load_filament_wrapper(uint8_t i){
    MMU2::mmu2.load_filament(i);
}

static void mmu_load_filament_menu() {
    MENU_BEGIN();
    MENU_ITEM_BACK_P(_T(MSG_MAIN));
    MENU_ITEM_FUNCTION_P(_T(MSG_LOAD_ALL), load_all_wrapper);
    for (uint8_t i = 0; i < MMU_FILAMENT_COUNT; i++)
        MENU_ITEM_FUNCTION_NR_P(_T(MSG_LOAD_FILAMENT), i + '1', load_filament_wrapper, i); ////MSG_LOAD_FILAMENT c=16
    MENU_END();
}

static inline void lcd_mmu_load_to_nozzle_wrapper(uint8_t index){
    MMU2::mmu2.load_filament_to_nozzle(index);

    // Ask user if the extruded color is correct:
    lcd_return_to_status();
    lcd_load_filament_color_check();
    lcd_setstatuspgm(MSG_WELCOME);
    custom_message_type = CustomMsg::Status;
}

static void mmu_load_to_nozzle_menu() {
    if (bFilamentAction) {
        MENU_BEGIN();
        MENU_ITEM_BACK_P(_T(MSG_MAIN));
        for (uint8_t i = 0; i < MMU_FILAMENT_COUNT; i++)
            MENU_ITEM_FUNCTION_NR_P(_T(MSG_LOAD_FILAMENT), i + '1', lcd_mmu_load_to_nozzle_wrapper, i); ////MSG_LOAD_FILAMENT c=16
        MENU_END();
    } else {
        eFilamentAction = FilamentAction::MmuLoad;
        preheat_or_continue();
    }
}

static void mmu_eject_filament(uint8_t filament) {
    menu_back();
    MMU2::mmu2.eject_filament(filament, true);
}

static void mmu_fil_eject_menu() {
    if (bFilamentAction || (!MMU2::mmu2.FindaDetectsFilament())) {
        MENU_BEGIN();
        MENU_ITEM_BACK_P(_T(MSG_MAIN));
        for (uint8_t i = 0; i < MMU_FILAMENT_COUNT; i++)
            MENU_ITEM_FUNCTION_NR_P(_T(MSG_EJECT_FROM_MMU), i + '1', mmu_eject_filament, i); ////MSG_EJECT_FROM_MMU c=16
        MENU_END();
    } else {
        eFilamentAction = FilamentAction::MmuEject;
        preheat_or_continue();
    }
}

#ifdef MMU_HAS_CUTTER
static inline void mmu_cut_filament_wrapper(uint8_t index){
    MMU2::mmu2.cut_filament(index);
}

static void mmu_cut_filament_menu() {
    if (bFilamentAction || (!MMU2::mmu2.FindaDetectsFilament())) {
        MENU_BEGIN();
        MENU_ITEM_BACK_P(_T(MSG_MAIN));
        for (uint8_t i = 0; i < MMU_FILAMENT_COUNT; i++)
            MENU_ITEM_FUNCTION_NR_P(_T(MSG_CUT_FILAMENT), i + '1', mmu_cut_filament_wrapper, i); ////MSG_CUT_FILAMENT c=16
        MENU_END();
    } else {
        eFilamentAction=FilamentAction::MmuCut;
        preheat_or_continue();
    }
}
#endif //MMU_HAS_CUTTER

static inline void loading_test_all_wrapper(){
    for(uint8_t i = 0; i < 5; ++i){
        MMU2::mmu2.loading_test(i);
    }
}

static inline void loading_test_wrapper(uint8_t i){
    MMU2::mmu2.loading_test(i);
}

static void mmu_loading_test_menu() {
    if (bFilamentAction) {
        MENU_BEGIN();
        MENU_ITEM_BACK_P(_T(MSG_MAIN));
        MENU_ITEM_FUNCTION_P(_T(MSG_LOAD_ALL), loading_test_all_wrapper);
        for (uint8_t i = 0; i < MMU_FILAMENT_COUNT; i++)
            MENU_ITEM_FUNCTION_NR_P(_T(MSG_LOAD_FILAMENT), i + '1', loading_test_wrapper, i); ////MSG_LOAD_FILAMENT c=16
        MENU_END();
    } else {
        eFilamentAction = FilamentAction::MmuLoadingTest;
        preheat_or_continue();
    }
}

/// @brief unload filament for single material printer (used in M600 and M702)
/// @param unloadLength Retract distance for removal (manual reload)
void unload_filament(float unloadLength)
{
	custom_message_type = CustomMsg::FilamentLoading;
	lcd_setstatuspgm(_T(MSG_UNLOADING_FILAMENT));

    FSensorBlockRunout fsBlockRunout;

    // Retract filament
    current_position[E_AXIS] += -FILAMENT_UNLOAD_PURGE_RETRACT;
    plan_buffer_line_curposXYZE(PAUSE_PARK_RETRACT_FEEDRATE);
    st_synchronize();

    // Wait for filament to cool
    delay_keep_alive(FILAMENT_UNLOAD_PURGE_DELAY);

    // Quickly purge
    current_position[E_AXIS] += (FILAMENT_UNLOAD_PURGE_RETRACT + FILAMENT_UNLOAD_PURGE_LENGTH);
    plan_buffer_line_curposXYZE(FILAMENT_UNLOAD_PURGE_FEEDRATE);
    st_synchronize();

    // Configurable length
    current_position[E_AXIS] += unloadLength;
    plan_buffer_line_curposXYZE(FILAMENT_CHANGE_UNLOAD_FEEDRATE);
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

	lcd_setstatuspgm(MSG_WELCOME);
	custom_message_type = CustomMsg::Status;

	eFilamentAction = FilamentAction::None;
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
    setExtruderAutoFanState(3); //force enables the hotend fan
#ifdef FAN_SOFT_PWM
    extruder_autofan_last_check = _millis();
    fan_measuring = true;
#endif //FAN_SOFT_PWM
    _delay(1000); //delay_keep_alive would turn off hotend fan, because temerature is too low (maybe)
    manage_heater();
    fanSpeed = 0;
	setExtruderAutoFanState(1); //releases lock on the hotend fan
#ifdef FAN_SOFT_PWM
    fanSpeedSoftPwm = 0;
#endif //FAN_SOFT_PWM
    manage_heater();
#ifdef TACH_0
    if (fan_speed[0] <= 20) { //hotend fan error
        LCD_ALERTMESSAGERPGM(MSG_FANCHECK_HOTEND);
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

bool resume_print_checks() {
    // reset the lcd status so that a newer error will be shown
    lcd_return_to_status();
    lcd_reset_alert_level();

    // ensure thermal issues (temp or fan) are resolved before we allow to resume
    if (get_temp_error()
#ifdef FANCHECK
        || fan_error_selftest()
#endif
        ) {
        return false; // abort if error persists
    }

    return true;
}

//! @brief Resume paused print, send host action "resumed"
//! @todo It is not good to call restore_print_from_ram_and_continue() from function called by lcd_update(),
//! as restore_print_from_ram_and_continue() calls lcd_update() internally.
void lcd_resume_print()
{
    // reset lcd and ensure we can resume first
    if (!resume_print_checks()) return;

    cmdqueue_serial_disabled = false;
    lcd_setstatuspgm(_T(MSG_FINISHING_MOVEMENTS));
    st_synchronize();
    custom_message_type = CustomMsg::Resuming;
    isPrintPaused = false;

    // resume processing USB commands again and restore hotend fan state (in case the print was
    // stopped due to a thermal error)
    hotendDefaultAutoFanState();
    Stopped = false;

    restore_print_from_ram_and_continue(default_retraction);
    pause_time += (_millis() - start_pause_print); //accumulate time when print is paused for correct statistics calculation
    refresh_cmd_timeout();
    SERIAL_PROTOCOLLNRPGM(MSG_OCTOPRINT_RESUMED); //resume octoprint
    custom_message_type = CustomMsg::Status;
}

//! @brief Resume paused USB/host print, send host action "resume"
void lcd_resume_usb_print()
{
    // reset lcd and ensure we can resume first
    if (!resume_print_checks()) return;

    // resume the usb host
    SERIAL_PROTOCOLLNRPGM(MSG_OCTOPRINT_ASK_RESUME);
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
        if (-1 == eeprom_next_initialized_sheet(0))
            calibration_status_clear(CALIBRATION_STATUS_LIVE_ADJUST);
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
        MENU_ITEM_SUBMENU_P(_T(MSG_V2_CALIBRATION), activate_calibrate_sheet);
    }
    MENU_ITEM_SUBMENU_P(_i("Rename"), lcd_rename_sheet_menu); ////MSG_RENAME c=18
	MENU_ITEM_FUNCTION_P(_T(MSG_RESET), lcd_reset_sheet);

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
//! | Load to nozzle     | not printing + mmu or paused
//! | Unload filament    | not printing or paused
//! | Eject from MMU     | not printing + mmu or paused
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


    MENU_ITEM_BACK_P(_T(MSG_INFO_SCREEN));

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

    if ( ( IS_SD_PRINTING || usb_timer.running() || (lcd_commands_type == LcdCommands::Layer1Cal)) && (current_position[Z_AXIS] < Z_HEIGHT_HIDE_LIVE_ADJUST_MENU) && !homing_flag && !mesh_bed_leveling_flag) {
        MENU_ITEM_SUBMENU_P(_T(MSG_BABYSTEP_Z), lcd_babystep_z);//8
    }

    if (farm_mode)
        MENU_ITEM_FUNCTION_P(_T(MSG_FILAMENTCHANGE), lcd_colorprint_change);//8

    if ( moves_planned() || printer_active() ) {
        MENU_ITEM_SUBMENU_P(_i("Tune"), lcd_tune_menu);////MSG_TUNE c=18
    } else if (!Stopped) {
        MENU_ITEM_SUBMENU_P(_i("Preheat"), lcd_preheat_menu);////MSG_PREHEAT c=18
    }

    if (mesh_bed_leveling_flag == false && homing_flag == false && !isPrintPaused && !processing_tcode) {
        if (usb_timer.running()) {
            MENU_ITEM_FUNCTION_P(_T(MSG_PAUSE_PRINT), lcd_pause_usb_print);
        } else if (IS_SD_PRINTING) {
            MENU_ITEM_FUNCTION_P(_T(MSG_PAUSE_PRINT), lcd_pause_print);
        }
    }
    if(isPrintPaused)
    {
        // only allow resuming if hardware errors (temperature or fan) are cleared
        if(!get_temp_error()
#ifdef FANCHECK
            && ((fan_check_error == EFCE_FIXED) || (fan_check_error == EFCE_OK))
#endif //FANCHECK
           ) {
            if (saved_printing) {
                MENU_ITEM_SUBMENU_P(_T(MSG_RESUME_PRINT), lcd_resume_print);
            } else {
                MENU_ITEM_SUBMENU_P(_T(MSG_RESUME_PRINT), lcd_resume_usb_print);
            }
        }
    }
    if((IS_SD_PRINTING || usb_timer.running() || isPrintPaused) && (custom_message_type != CustomMsg::MeshBedLeveling) && !processing_tcode) {
        MENU_ITEM_SUBMENU_P(_T(MSG_STOP_PRINT), lcd_sdcard_stop);
    }
#ifdef TEMP_MODEL
    else if(Stopped) {
        MENU_ITEM_SUBMENU_P(_T(MSG_TM_ACK_ERROR), lcd_print_stop);
    }
#endif
#ifdef SDSUPPORT //!@todo SDSUPPORT undefined creates several issues in source code
    if (card.cardOK || lcd_commands_type == LcdCommands::Layer1Cal) {
        if (!card.isFileOpen()) {
            if (!usb_timer.running() && (lcd_commands_type != LcdCommands::Layer1Cal)) {
                bMain=true;               // flag ('fake parameter') for 'lcd_sdcard_menu()' function
                MENU_ITEM_SUBMENU_P(_T(MSG_CARD_MENU), lcd_sdcard_menu);
            }
#if SDCARDDETECT < 1
        MENU_ITEM_GCODE_P(_i("Change SD card"), PSTR("M21"));  // SD-card changed by user ////MSG_CNG_SDCARD c=18
#endif //SDCARDDETECT
        }
    } else {
        bMain=true;                                   // flag (i.e. 'fake parameter') for 'lcd_sdcard_menu()' function
        MENU_ITEM_SUBMENU_P(_i("No SD card"), lcd_sdcard_menu); ////MSG_NO_CARD c=18
#if SDCARDDETECT < 1
        MENU_ITEM_GCODE_P(_i("Init. SD card"), PSTR("M21")); // Manually initialize the SD-card via user interface ////MSG_INIT_SDCARD c=18
#endif //SDCARDDETECT
    }
#endif //SDSUPPORT

    if(!isPrintPaused && !IS_SD_PRINTING && !usb_timer.running() && (lcd_commands_type != LcdCommands::Layer1Cal)) {
        if (!farm_mode) {
            const int8_t sheet = eeprom_read_byte(&(EEPROM_Sheets_base->active_sheet));
            const int8_t nextSheet = eeprom_next_initialized_sheet(sheet);
            if ((nextSheet >= 0) && (sheet != nextSheet)) { // show menu only if we have 2 or more sheets initialized
                MENU_ITEM_FUNCTION_E(EEPROM_Sheets_base->s[sheet], eeprom_switch_to_next_sheet);
            }
        }
    }

    if ( ! ( IS_SD_PRINTING || usb_timer.running() || (lcd_commands_type == LcdCommands::Layer1Cal || Stopped) ) ) {
        if (MMU2::mmu2.Enabled()) {
            MENU_ITEM_SUBMENU_P(_T(MSG_LOAD_FILAMENT), mmu_load_filament_menu);
            MENU_ITEM_SUBMENU_P(_i("Load to nozzle"), mmu_load_to_nozzle_menu);////MSG_LOAD_TO_NOZZLE c=18
            MENU_ITEM_SUBMENU_P(_T(MSG_UNLOAD_FILAMENT), mmu_unload_filament);
            MENU_ITEM_SUBMENU_P(_T(MSG_EJECT_FROM_MMU), mmu_fil_eject_menu);
#ifdef  MMU_HAS_CUTTER
            if (eeprom_read_byte((uint8_t*)EEPROM_MMU_CUTTER_ENABLED) != 0) {
                MENU_ITEM_SUBMENU_P(_T(MSG_CUT_FILAMENT), mmu_cut_filament_menu);
            }
#endif //MMU_HAS_CUTTER
        } else {
#ifdef FILAMENT_SENSOR
            if (fsensor.getAutoLoadEnabled()) {
                MENU_ITEM_SUBMENU_P(_i("AutoLoad filament"), lcd_menu_AutoLoadFilament);////MSG_AUTOLOAD_FILAMENT c=18
            }
            else
#endif //FILAMENT_SENSOR
            {
                MENU_ITEM_SUBMENU_P(_T(MSG_LOAD_FILAMENT), lcd_LoadFilament);
            }
            MENU_ITEM_SUBMENU_P(_T(MSG_UNLOAD_FILAMENT), lcd_unLoadFilament);
        }
    MENU_ITEM_SUBMENU_P(_T(MSG_SETTINGS), lcd_settings_menu);
    if(!isPrintPaused) MENU_ITEM_SUBMENU_P(_T(MSG_CALIBRATION), lcd_calibration_menu);
    }

    if (!usb_timer.running() && (lcd_commands_type != LcdCommands::Layer1Cal)) {
        MENU_ITEM_SUBMENU_P(_i("Statistics"), lcd_menu_statistics);////MSG_STATISTICS c=18
    }

#if defined(TMC2130) || defined(FILAMENT_SENSOR)
    MENU_ITEM_SUBMENU_P(_i("Fail stats"), lcd_menu_fails_stats);////MSG_FAIL_STATS c=18
#endif
    if (MMU2::mmu2.Enabled()) {
        MENU_ITEM_SUBMENU_P(_i("Fail stats MMU"), lcd_menu_fails_stats_mmu);////MSG_MMU_FAIL_STATS c=18
    }
    MENU_ITEM_SUBMENU_P(_i("Support"), lcd_support_menu);////MSG_SUPPORT c=18
#ifdef LCD_TEST
    MENU_ITEM_SUBMENU_P(_i("XFLASH init"), lcd_test_menu);////MSG_XFLASH c=18
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

	SilentModeMenu = eeprom_read_byte((uint8_t*) EEPROM_SILENT);

	MENU_BEGIN();
	MENU_ITEM_BACK_P(_T(MSG_MAIN)); //1
	MENU_ITEM_EDIT_int3_P(_i("Speed"), &feedmultiply, 10, 999);//2////MSG_SPEED c=15

	MENU_ITEM_EDIT_int3_P(_T(MSG_NOZZLE), &target_temperature[0], 0, HEATER_0_MAXTEMP - 10);//3
	MENU_ITEM_EDIT_int3_P(_T(MSG_BED), &target_temperature_bed, 0, BED_MAXTEMP - 10);

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
    MENU_ITEM_SUBMENU_P(_T(MSG_FSENSOR), lcd_fsensor_settings_menu);
#endif //FILAMENT_SENSOR

    if (MMU2::mmu2.Enabled())
    {
        SETTINGS_SPOOLJOIN;
        SETTINGS_CUTTER;
    }

	MENU_ITEM_TOGGLE_P(_T(MSG_FANS_CHECK), fans_check_enabled ? _T(MSG_ON) : _T(MSG_OFF), lcd_set_fan_check);


#ifdef TMC2130
    if(!farm_mode)
    {
        if (SilentModeMenu == SILENT_MODE_NORMAL) {
              MENU_ITEM_TOGGLE_P(_T(MSG_MODE), _T(MSG_NORMAL), lcd_silent_mode_set);
            if (lcd_crash_detect_enabled()) {
                MENU_ITEM_TOGGLE_P(_T(MSG_CRASHDETECT), _T(MSG_ON), crash_mode_switch);
            } else {
                MENU_ITEM_TOGGLE_P(_T(MSG_CRASHDETECT), _T(MSG_OFF), crash_mode_switch);
            }
        } else {
            MENU_ITEM_TOGGLE_P(_T(MSG_MODE), _T(MSG_STEALTH), lcd_silent_mode_set);
            MENU_ITEM_TOGGLE_P(_T(MSG_CRASHDETECT), NULL, lcd_crash_mode_info);
        }
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
  MENU_BEGIN();
  MENU_ITEM_BACK_P(_T(MSG_SETTINGS));
#if TEMP_SENSOR_0 != 0
  MENU_ITEM_EDIT_int3_P(_T(MSG_NOZZLE), &target_temperature[0], 0, HEATER_0_MAXTEMP - 10);
#endif
#if TEMP_SENSOR_1 != 0
  MENU_ITEM_EDIT_int3_P(_n("Nozzle2"), &target_temperature[1], 0, HEATER_1_MAXTEMP - 10);
#endif
#if TEMP_SENSOR_2 != 0
  MENU_ITEM_EDIT_int3_P(_n("Nozzle3"), &target_temperature[2], 0, HEATER_2_MAXTEMP - 10);
#endif
#if TEMP_SENSOR_BED != 0
  MENU_ITEM_EDIT_int3_P(_T(MSG_BED), &target_temperature_bed, 0, BED_MAXTEMP - 3);
#endif
  MENU_ITEM_EDIT_int3_P(_T(MSG_FAN_SPEED), &fanSpeed, 0, 255);
#if defined AUTOTEMP && (TEMP_SENSOR_0 != 0)
//MENU_ITEM_EDIT removed, following code must be redesigned if AUTOTEMP enabled
  MENU_ITEM_EDIT(bool, MSG_AUTOTEMP, &autotemp_enabled);
  MENU_ITEM_EDIT(float3, _n(" \xdf Min"), &autotemp_min, 0, HEATER_0_MAXTEMP - 10);
  MENU_ITEM_EDIT(float3, _n(" \xdf Max"), &autotemp_max, 0, HEATER_0_MAXTEMP - 10);
  MENU_ITEM_EDIT(float32, _n(" \xdf Fact"), &autotemp_factor, 0.0, 1.0);
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

// continue stopping the print from the main loop after lcd_print_stop() is called
void lcd_print_stop_finish()
{
    // save printing time
    stoptime = _millis();
    unsigned long t = (stoptime - starttime - pause_time) / 1000; //time in s
    save_statistics(total_filament_used, t);

    // lift Z
    raise_z(10);

    // if axis are homed, move to parking position.
    if (axis_known_position[X_AXIS] && axis_known_position[Y_AXIS]) {
        current_position[X_AXIS] = X_CANCEL_POS;
        current_position[Y_AXIS] = Y_CANCEL_POS;
        plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
    }
    st_synchronize();

    // did we come here from a thermal error?
    if(get_temp_error()) {
        // time to stop the error beep
        WRITE(BEEPER, LOW);
    } else {
        // Turn off the print fan
        fanSpeed = 0;

        // restore the auto hotend state
        hotendDefaultAutoFanState();
    }

    if (MMU2::mmu2.Enabled() && MMU2::mmu2.FindaDetectsFilament())
    {
        if (isPrintPaused)
        {
            // Restore temperature saved in ram after pausing print
            restore_extruder_temperature_from_ram();
        }
        MMU2::mmu2.unload(); // M702
    }

    lcd_cooldown(); //turns off heaters and fan; goes to status screen.

    finishAndDisableSteppers(); //M84
    axis_relative_modes = E_AXIS_MASK; //XYZ absolute, E relative
}

void print_stop(bool interactive)
{
    // UnconditionalStop() will internally cause planner_abort_hard(), meaning we _cannot_ plan any
    // more move in this call! Any further move must happen inside lcd_print_stop_finish(), which is
    // called by the main loop one iteration later.
    UnconditionalStop();

    if (!card.sdprinting) {
        SERIAL_ECHOLNRPGM(MSG_OCTOPRINT_CANCEL); // for Octoprint
    }

#ifdef MESH_BED_LEVELING
    mbl.active = false;
#endif

    // clear any pending paused state immediately
    pause_time = 0;
    isPrintPaused = false;

    if (interactive) {
        // acknowledged by the user from the LCD: resume processing USB commands again
        Stopped = false;
    }

    // return to status is required to continue processing in the main loop!
    lcd_commands_type = LcdCommands::StopPrint;
    lcd_return_to_status();
}

void lcd_print_stop()
{
    print_stop(true);
}

#ifdef TEMP_MODEL
void lcd_temp_model_cal()
{
    lcd_commands_type = LcdCommands::TempModel;
    lcd_return_to_status();
}
#endif //TEMP_MODEL

void lcd_sdcard_stop()
{
    // Show static message
    lcd_puts_at_P(0, 0, _T(MSG_STOP_PRINT));
    lcd_putc_at(0, 1, '\n');

    MENU_BEGIN();
    // Skip first two LCD rows used by static message
    if(menu_row == 0) menu_row = 2;

    // Show No options first, the default selection
    MENU_ITEM_FUNCTION_P(_T(MSG_NO), lcd_return_to_status);
    MENU_ITEM_FUNCTION_P(_T(MSG_YES), lcd_print_stop);
    MENU_END();
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
				lcd_update_enabled = false;
				card.presort();
				lcd_update_enabled = true;
			}
			_md->fileCnt = card.getnrfilenames();
			_md->sdSort = farm_mode ? SD_SORT_NONE : eeprom_read_byte((uint8_t*)EEPROM_SD_SORT);
			_md->menuState = _standard;
			_md->row = -1; // assume that no SD file/dir is currently selected. Once they are rendered, it will be changed to the correct row for the _scrolling state.
		}
		// FALLTHRU
		case _standard: //normal menu structure.
		{
			if (!_md->lcd_scrollTimer.running()) //if the timer is not running, then the menu state was just switched, so redraw the screen.
			{
				_md->lcd_scrollTimer.start();
				lcd_draw_update = 1;
			}
			if ((lcd_draw_update == 0) && _md->lcd_scrollTimer.expired(500) && (_md->row != -1)) //switch to the scrolling state on timeout if a file/dir is selected.
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
				uint8_t len = LCD_WIDTH - ((_md->isDir)? 2 : 1);
				lcd_set_cursor(0, _md->row);
				lcd_print('>');
				if (_md->isDir)
					lcd_print(LCD_STR_FOLDER[0]);

				if( lcd_print_pad(&_md->scrollPointer[_md->offset], len) )
				{
					_md->lcd_scrollTimer.start();
					_md->offset++;
				} else {
					// stop at the end of the string
					_md->lcd_scrollTimer.stop();
				}
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
			lcd_print(LCD_STR_UPLEVEL[0]);
			lcd_wait_for_click_delay(10);
		}
    }
	
	FORCE_HIGH_POWER_END;
    KEEPALIVE_STATE(NOT_BUSY);
}
#endif //TMC2130

#if defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
// called also from marlin_main.cpp
void printf_IRSensorAnalogBoardChange(){
    printf_P(PSTR("Filament sensor board change detected: revision%S\n"), fsensor.getIRVersionText());
}

static bool lcd_selftest_IRsensor(bool bStandalone)
{
    FSensorBlockRunout fsBlockRunout;
    IR_sensor_analog::SensorRevision oldSensorRevision = fsensor.getSensorRevision();
    IR_sensor_analog::SensorRevision newSensorRevision;
    uint16_t volt_IR_int = fsensor.getVoltRaw();
    
    newSensorRevision = (volt_IR_int < fsensor.IRsensor_Hopen_TRESHOLD) ? IR_sensor_analog::SensorRevision::_Rev04 : IR_sensor_analog::SensorRevision::_Old;
    printf_P(PSTR("Measured filament sensor high level: %4.2fV\n"), Raw2Voltage(volt_IR_int) );
    if(volt_IR_int < fsensor.IRsensor_Hmin_TRESHOLD){
        if(!bStandalone)
            lcd_selftest_error(TestError::FsensorLevel,"HIGH","");
        return false;
    }
    lcd_show_fullscreen_message_and_wait_P(_i("Insert the filament (do not load it) into the extruder and then press the knob."));////MSG_INSERT_FIL c=20 r=6
    volt_IR_int = fsensor.getVoltRaw();
    printf_P(PSTR("Measured filament sensor low level: %4.2fV\n"), Raw2Voltage(volt_IR_int));
    if(volt_IR_int > (fsensor.IRsensor_Lmax_TRESHOLD)){
        if(!bStandalone)
            lcd_selftest_error(TestError::FsensorLevel,"LOW","");
        return false;
    }
    if(newSensorRevision != oldSensorRevision) {
        fsensor.setSensorRevision(newSensorRevision, true);
        printf_IRSensorAnalogBoardChange();
    }
    return true;
}

static void lcd_detect_IRsensor(){
    bool bAction;
    bool loaded;
    /// Check if filament is loaded. If it is loaded stop detection.
    /// @todo Add autodetection with MMU2s
    loaded = fsensor.getFilamentPresent();
    if(loaded){
        lcd_show_fullscreen_message_and_wait_P(_i("Please unload the filament first, then repeat this action."));////MSG_UNLOAD_FILAMENT_REPEAT c=20 r=4
        return;
    } else {
        lcd_show_fullscreen_message_and_wait_P(_i("Please check the IR sensor connection, unload filament if present."));////MSG_CHECK_IR_CONNECTION c=20 r=4
        bAction = lcd_selftest_IRsensor(true);
    }
    if(bAction){
        lcd_show_fullscreen_message_and_wait_P(_i("Sensor verified, remove the filament now."));////MSG_FS_VERIFIED c=20 r=3
        fsensor.init();
    } else {
        lcd_show_fullscreen_message_and_wait_P(_i("Verification failed, remove the filament and try again."));////MSG_FIL_FAILED c=20 r=5
    }
}
#endif //IR_SENSOR_ANALOG

static void lcd_selftest_v()
{
	(void)lcd_selftest();
}

bool lcd_selftest()
{
	uint8_t _progress = 0;
	bool _result = true;
	bool _swapped_fan = false;
#if defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
	//!   Check if IR sensor is in unknown state, if so run Fsensor Detection
	//!   As the Fsensor Detection isn't yet ready for the mmu2s we set temporarily the IR sensor 0.3 or older for mmu2s
	//! @todo Don't forget to remove this as soon Fsensor Detection works with mmu
	if(fsensor.getSensorRevision() == IR_sensor_analog::SensorRevision::_Undef) {
		if (!MMU2::mmu2.Enabled()) {
			lcd_detect_IRsensor();
		}
		else {
			fsensor.setSensorRevision(IR_sensor_analog::SensorRevision::_Old, true);
		}
	}
#endif
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
	switch (lcd_selftest_fan_auto(0)){		// check hotend fan
    case FanCheck::SwappedFan:
        _swapped_fan = true; // swapped is merely a hint (checked later)
        // FALLTHRU
    case FanCheck::Success:
        _result = true;
        break;
    default:
        _result = false;
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
        case FanCheck::SwappedFan:
            _swapped_fan = true; // swapped is merely a hint (checked later)
            // FALLTHRU
        case FanCheck::Success:
            _result = true;
            break;
        default:
            _result = false;
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
		//turn on print fan and check that left hotend fan is not spinning
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
		raise_z(10);
        set_destination_to_current();
		_progress = lcd_selftest_screen(TestScreen::AxisZ, _progress, 3, true, 1500);
#ifdef TMC2130
		homeaxis(Z_AXIS); //In case of failure, the code gets stuck in this function.
#else
        _result = lcd_selfcheck_axis(Z_AXIS, Z_MAX_POS);
#endif //TMC2130

		//raise Z to not damage the bed during and hotend testing
		raise_z(20);
	}

#ifdef TMC2130
	if (_result)
	{
		raise_z(10);
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
#if (FILAMENT_SENSOR_TYPE == FSENSOR_IR) || (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
        if (MMU2::mmu2.Enabled())
        {        
			_progress = lcd_selftest_screen(TestScreen::Fsensor, _progress, 3, true, 2000); //check filaments sensor
            _result = selftest_irsensor();
		    if (_result)
			{
				_progress = lcd_selftest_screen(TestScreen::FsensorOk, _progress, 3, true, 2000); //fil sensor OK
			}
        } else
#endif //(FILAMENT_SENSOR_TYPE == FSENSOR_IR) || (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
        {
#if FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125
			_progress = lcd_selftest_screen(TestScreen::Fsensor, _progress, 3, true, 2000); //check filaments sensor
               _result = lcd_selftest_fsensor();
			if (_result)
			{
				_progress = lcd_selftest_screen(TestScreen::FsensorOk, _progress, 3, true, 2000); //fil sensor OK
			}
#endif //FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125
#if 0
	// Intentionally disabled - that's why we moved the detection to runtime by just checking the two voltages.
	// The idea is not to force the user to remove and insert the filament on an assembled printer.
//defined(FILAMENT_SENSOR) && (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
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
		calibration_status_set(CALIBRATION_STATUS_SELFTEST);
		lcd_setstatuspgm(_i("Self test OK"));////MSG_SELFTEST_OK c=20
		lcd_return_to_status();
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

static void reset_crash_det(uint8_t axis) {
	current_position[axis] += 10;
	plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
	st_synchronize();
	if (eeprom_read_byte((uint8_t*)EEPROM_CRASH_DET)) tmc2130_sg_stop_on_crash = true;
}

static bool lcd_selfcheck_axis_sg(uint8_t axis) {
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
	tmc2130_home_enter(1 << axis);

// first axis length measurement begin	
	
	current_position[axis] -= (axis_length + margin);
	plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);

	
	st_synchronize();

	tmc2130_sg_measure_start(axis);

	current_position_init = st_get_position_mm(axis);

	current_position[axis] += 2 * margin;
	plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);
	st_synchronize();

	current_position[axis] += axis_length;
	plan_buffer_line_curposXYZE(manual_feedrate[0] / 60);

	st_synchronize();

	uint16_t sg1 = tmc2130_sg_measure_stop();
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
	uint8_t _progress = 0;
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
	uint8_t _progress = 0;
	int _checked_snapshot = (_isbed) ? degBed() : degHotend(0);
	int _opposite_snapshot = (_isbed) ? degHotend(0) : degBed();
	uint8_t _cycles = (_isbed) ? 180 : 60; //~ 90s / 30s

	target_temperature[0] = (_isbed) ? 0 : 200;
	target_temperature_bed = (_isbed) ? 100 : 0;
#ifdef TEMP_MODEL
	bool tm_was_enabled = temp_model_enabled();
	temp_model_set_enabled(false);
#endif //TEMP_MODEL
	manage_heater();
	manage_inactivity(true);

    for(uint8_t _counter = 0; _counter < _cycles && !Stopped; ++_counter)
    {
		manage_heater();
		manage_inactivity(true);
		_progress = (_isbed?
			lcd_selftest_screen(TestScreen::Bed, _progress, 2, false, 400) :
			lcd_selftest_screen(TestScreen::Hotend, _progress, 2, false, 400));
		/*if (_isbed) {
			MYSERIAL.print("Bed temp:");
			MYSERIAL.println(degBed());
		}
		else {
			MYSERIAL.print("Hotend temp:");
			MYSERIAL.println(degHotend(0));
		}*/
		if(_counter%5 == 0) serialecho_temperatures(); //show temperatures once in two seconds
	}

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

    bool _stepresult = false;
    if (Stopped || _opposite_result < ((_isbed) ? 30 : 9))
    {
        if (!Stopped && _checked_result >= ((_isbed) ? 9 : 30))
            _stepresult = true;
        else
            lcd_selftest_error(TestError::Heater, "", "");
    }
    else
    {
        lcd_selftest_error(TestError::Bed, "", "");
    }

#ifdef TEMP_MODEL
	temp_model_set_enabled(tm_was_enabled);
#endif //TEMP_MODEL
	manage_heater();
	manage_inactivity(true);
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
		lcd_puts_at_P(0, 2, _T(MSG_SELFTEST_PART_FAN));
		lcd_puts_at_P(0, 3, _T(MSG_SELFTEST_WIRINGERROR));
		lcd_set_cursor(18, 3);
		lcd_print(_error_1);
		break;
	case TestError::ExtruderFan:
		lcd_puts_at_P(0, 2, _T(MSG_SELFTEST_HOTEND_FAN));
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
#if FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125
static bool lcd_selftest_fsensor(void)
{
	fsensor.init();
	if (fsensor.isError())
	{
		lcd_selftest_error(TestError::WiringFsensor, "", "");
	}
	return (!fsensor.isError());
}
#endif //FILAMENT_SENSOR_TYPE == FSENSOR_PAT9125

#if (FILAMENT_SENSOR_TYPE == FSENSOR_IR) || (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
//! @brief Self-test of infrared barrier filament sensor mounted on MK3S with MMUv2 printer
//!
//! Test whether sensor is not triggering filament presence when extruder idler is moving without filament.
//!
//! Steps:
//!  * Backup current active extruder temperature
//!  * Pre-heat to PLA extrude temperature.
//!  * Unload filament possibly present.
//!  * Move extruder idler same way as during filament load
//!    and sample the filament sensor.
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
            m_temp(degTargetHotend(active_extruder)){}
        ~TempBackup(){setTargetHotend(m_temp);}
    private:
        float m_temp;
    };
    uint8_t progress;
    {
        TempBackup tempBackup;
        setTargetHotend(ABS_PREHEAT_HOTEND_TEMP);
        progress = lcd_selftest_screen(TestScreen::Fsensor, 0, 1, true, 0);
    }
    progress = lcd_selftest_screen(TestScreen::Fsensor, progress, 1, true, 0);
    MMU2::mmu2.unload();

    for(uint_least8_t i = 0; i < 200; ++i)
    {
        if (0 == (i % 32)) progress = lcd_selftest_screen(TestScreen::Fsensor, progress, 1, true, 0);

//@@TODO        mmu_load_step(false);
        while (blocks_queued())
        {
            if (fsensor.getFilamentPresent())
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
#endif //(FILAMENT_SENSOR_TYPE == FSENSOR_IR) || (FILAMENT_SENSOR_TYPE == FSENSOR_IR_ANALOG)
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
		lcd_puts_at_P(0, 1, check_opposite ? _T(MSG_SELFTEST_PART_FAN) : _T(MSG_SELFTEST_HOTEND_FAN));
		setExtruderAutoFanState(3);
		break;
	case 1:
		// object cooling fan
		lcd_puts_at_P(0, 1, check_opposite ? _T(MSG_SELFTEST_HOTEND_FAN) : _T(MSG_SELFTEST_PART_FAN));
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
// Set print fan speed
static void lcd_selftest_setfan(uint8_t speed) {
    // set the fan speed
    fanSpeed = speed;
#ifdef FAN_SOFT_PWM
    fanSpeedSoftPwm = speed;
#endif
    manage_heater();
}

// Wait for the specified number of seconds while displaying some single-character indicator on the
// screen coordinate col/row, then perform fan measurement
static void lcd_selftest_measure_fans(uint8_t delay, uint8_t col, uint8_t row) {
    // spin-up delay
    static char symbols[] = {'-', '|'};
    static_assert(1000 / sizeof(symbols) * sizeof(symbols) == 1000);
    while(delay--) {
        for(uint8_t i = 0; i != sizeof(symbols); ++i) {
            lcd_putc_at(col, row, symbols[i]);
            delay_keep_alive(1000 / sizeof(symbols));
        }
    }

#ifdef FANCHECK
    extruder_autofan_last_check = _millis();
#endif
    fan_measuring = true;
    while(fan_measuring) {
        delay_keep_alive(100);
    }

    gcode_M123();
}

static FanCheck lcd_selftest_fan_auto(uint8_t _fan)
{
    // speed threshold to differentiate between extruder and print fan
    static const int printFanThr = FANCHECK_AUTO_PRINT_FAN_THRS; // >= FANCHECK_AUTO_PRINT_FAN_THRS RPS

    // speed threshold to mark a fan as failed
    static const int failThr = FANCHECK_AUTO_FAIL_THRS; // < FANCHECK_AUTO_FAIL_THRS RPM would mean either a faulty Noctua, Altfan or print fan

	switch (_fan) {
	case 0:
        setExtruderAutoFanState(3); // hotend fan
        lcd_selftest_setfan(0); // print fan off
        lcd_selftest_measure_fans(2, 18, 2);
        setExtruderAutoFanState(0); // hotend fan off
		if (fan_speed[0] < failThr) {
			return FanCheck::ExtruderFan;
		}
		if (fan_speed[0] >= printFanThr ) {
			return FanCheck::SwappedFan;
		}
		break;

	case 1:
        lcd_selftest_setfan(255);
        lcd_selftest_measure_fans(5, 18, 3);
        lcd_selftest_setfan(0);
        if (fan_speed[1] < failThr) {
            return FanCheck::PrintFan;
        }
		if (fan_speed[1] < printFanThr) {
			return FanCheck::SwappedFan;
		}
	}
	return FanCheck::Success;
}

#endif //FANCHECK

static uint8_t lcd_selftest_screen(TestScreen screen, uint8_t _progress, uint8_t _progress_scale, bool _clear, uint16_t _delay)
{

	lcd_update_enable(false);
	const char _indicator = (_progress >= _progress_scale) ? '-' : '|';
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
		lcd_puts_at_P(0, 2, _T(MSG_HOTEND_FAN_SPEED));
		lcd_set_cursor(18, 2);
		(screen < TestScreen::PrintFan) ? (void)lcd_putc(_indicator) : (void)lcd_puts_P(MSG_OK_CAPS);
		lcd_puts_at_P(0, 3, _T(MSG_PRINT_FAN_SPEED));
		lcd_set_cursor(18, 3);
		(screen < TestScreen::FansOk) ? (void)lcd_putc(_indicator) : (void)lcd_puts_P(MSG_OK_CAPS);
	}
	else if (screen >= TestScreen::Fsensor && screen <= TestScreen::FsensorOk)
	{
		lcd_puts_at_P(0, 2, _T(MSG_SELFTEST_FILAMENT_SENSOR));
		lcd_putc(':');
		lcd_set_cursor(18, 2);
		(screen == TestScreen::Fsensor) ? (void)lcd_putc(_indicator) : (void)lcd_puts_P(MSG_OK_CAPS);
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

static void lcd_selftest_screen_step(uint8_t _row, uint8_t _col, uint8_t _state, const char *_name_PROGMEM, const char _indicator)
{
	lcd_set_cursor(_col, _row);
    uint8_t strlenNameP = strlen_P(_name_PROGMEM);

	switch (_state)
	{
	case 1:
		lcd_puts_P(_name_PROGMEM);
		lcd_putc_at(_col + strlenNameP, _row, ':');
		lcd_putc(_indicator);
		break;
	case 2:
		lcd_puts_P(_name_PROGMEM);
		lcd_putc_at(_col + strlenNameP, _row, ':');
		lcd_puts_P(MSG_OK_CAPS);
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
#ifdef CMDBUFFER_DEBUG
		// Kick watchdog because the file check is very slow
		// with the CMDBUFFER_DEBUG enabled
		manage_heater();
#endif // CMDBUFFER_DEBUG
	}
	
	menu_progressbar_finish();
	
	cmdqueue_serial_disabled = false;
	card.printingHasFinished();

	lcd_setstatuspgm(MSG_WELCOME);
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

  uint8_t depth = card.getWorkDirDepth();
  eeprom_write_byte((uint8_t*)EEPROM_DIR_DEPTH, depth);

  for (uint_least8_t i = 0; i < depth; i++) {
	  for (uint_least8_t j = 0; j < 8; j++) {
		  eeprom_write_byte((uint8_t*)EEPROM_DIRS + j + 8 * i, card.dir_names[i][j]);
	  }
  }
  
  //filename is just a pointer to card.filename, which changes everytime you try to open a file by filename. So you can't use filename directly
  //to open a file. Instead, the cached filename in cmd is used as that one is static for the whole lifetime of this function.
  if (!check_file(cmd + 4)) {
      result = !lcd_show_fullscreen_message_yes_no_and_wait_P(_i("File incomplete. Continue anyway?"), false);////MSG_FILE_INCOMPLETE c=20 r=3
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
    backlight_init();
	lcd_init();
	lcd_refresh();
	lcd_longpress_func = menu_lcd_longpress_func;
	lcd_lcdupdate_func = menu_lcd_lcdupdate_func;
	menu_menu = lcd_status_screen;

  SET_INPUT(BTN_EN1);
  SET_INPUT(BTN_EN2);
  WRITE(BTN_EN1, HIGH);
  WRITE(BTN_EN2, HIGH);
#if BTN_ENC > 0
  SET_INPUT(BTN_ENC);
  WRITE(BTN_ENC, HIGH);
#endif

#if defined (SDSUPPORT) && defined(SDCARDDETECT) && (SDCARDDETECT > 0)
  SET_INPUT(SDCARDDETECT);
  WRITE(SDCARDDETECT, HIGH);
  _delay_ms(1); //wait for the pullups to raise the line
  lcd_oldcardstatus = IS_SD_INSERTED;
#endif//(SDCARDDETECT > 0)
  lcd_encoder_diff = 0;

  // Initialise status line
  strncpy_P(lcd_status_message, MSG_WELCOME, LCD_WIDTH);
}

void lcd_ignore_click(bool b)
{
  ignore_click = b;
  wait_for_unclick = false;
}

static bool lcd_message_check(uint8_t priority)
{
    // regular priority check
    if (priority >= lcd_status_message_level)
        return true;

    // check if we can override an info message yet
    if (lcd_status_message_level == LCD_STATUS_INFO) {
        return lcd_status_message_timeout.expired_cont(LCD_STATUS_INFO_TIMEOUT);
    }

    return false;
}

static void lcd_updatestatus(const char *message, bool progmem = false)
{
    if (progmem)
        strncpy_P(lcd_status_message, message, LCD_WIDTH);
    else
        strncpy(lcd_status_message, message, LCD_WIDTH);

    lcd_status_message[LCD_WIDTH] = 0;

    SERIAL_PROTOCOLLNRPGM(MSG_LCD_STATUS_CHANGED);

    // hack lcd_draw_update to 1, i.e. without clear
    lcd_draw_update = 1;
}

void lcd_setstatus(const char* message)
{
    if (lcd_message_check(LCD_STATUS_NONE))
        lcd_updatestatus(message);
}

void lcd_setstatuspgm(const char* message)
{
    if (lcd_message_check(LCD_STATUS_NONE))
        lcd_updatestatus(message, true);
}

void lcd_setstatus_serial(const char* message)
{
    if (lcd_message_check(LCD_STATUS_NONE))
        lcd_updatestatus(message);
    SERIAL_ECHOLN(message);
}

void lcd_setalertstatus_(const char* message, uint8_t severity, bool progmem)
{
    if (lcd_message_check(severity)) {
        bool same = !(progmem?
            strcmp_P(lcd_status_message, message):
            strcmp(lcd_status_message, message));
        lcd_status_message_timeout.start();
        lcd_status_message_level = severity;
        custom_message_type = CustomMsg::Status;
        custom_message_state = 0;
        if (!same) {
            // do not kick the user out of the menus if the message is unchanged
            lcd_updatestatus(message, progmem);
            lcd_return_to_status();
        }
    }
}

void lcd_setalertstatus(const char* message, uint8_t severity)
{
    lcd_setalertstatus_(message, severity, false);
}

void lcd_setalertstatuspgm(const char* message, uint8_t severity)
{
    lcd_setalertstatus_(message, severity, true);
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
    // Wake up the LCD backlight and,
    // start LCD inactivity timer
    lcd_timeoutToStatus.start();
    if (homing_flag || mesh_bed_leveling_flag || menu_menu == lcd_babystep_z || menu_menu == lcd_move_z || menu_block_mask != MENU_BLOCK_NONE || Stopped)
    {
        // disable longpress during re-entry, while homing, calibration or if a serious error
        lcd_quick_feedback();
        return;
    }
    if (menu_menu == lcd_hw_setup_menu)
    {
        // only toggle the experimental menu visibility flag
        lcd_quick_feedback();
        eeprom_toggle((uint8_t *)EEPROM_EXPERIMENTAL_VISIBILITY);
        return;
    }

    // explicitely listed menus which are allowed to rise the move-z or live-adj-z functions
    // The lists are not the same for both functions, so first decide which function is to be performed
    if ( (moves_planned() || IS_SD_PRINTING || usb_timer.running() )){ // long press as live-adj-z
        if (( current_position[Z_AXIS] < Z_HEIGHT_HIDE_LIVE_ADJUST_MENU ) // only allow live-adj-z up to 2mm of print height
        && ( menu_menu == lcd_status_screen // and in listed menus...
          || menu_menu == lcd_main_menu
          || menu_menu == lcd_tune_menu
          || menu_menu == lcd_support_menu
           )
        ){
            lcd_clear();
            menu_submenu(lcd_babystep_z);
        } else {
            lcd_quick_feedback();
        }
    } else { // long press as move-z
        if (menu_menu == lcd_status_screen
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
            menu_submenu(lcd_move_z);
        } else {
            lcd_quick_feedback();
        }
    }
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

void menu_lcd_lcdupdate_func(void)
{
#if (SDCARDDETECT > 0)
	if ((IS_SD_INSERTED != lcd_oldcardstatus))
	{
		if(menu_menu == lcd_sdcard_menu) {
			// If the user is either inside the submenus
			// 1. 'Print from SD' --> and SD card is removed
			// 2. 'No SD card' --> and SD card is inserted
			//
			// 1. 'Print from SD': We want to back out of this submenu
			//    and instead show the submenu title 'No SD card'.
			//
			// 2. 'No SD card': When the user inserts the SD card we want
			//    to back out of this submenu. Not only to show 
			//    'Print from SD' submenu title but also because the user
			//    will be prompted with another menu with the sorted list of files.
			//    Without backing out of the menu, the list will appear empty and
			//    The user will need to back out of two nested submenus.
			menu_back();
		}
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
			LCD_MESSAGERPGM(MSG_WELCOME);
			bMain=false;                       // flag (i.e. 'fake parameter') for 'lcd_sdcard_menu()' function
			menu_submenu(lcd_sdcard_menu);
			lcd_timeoutToStatus.start();
		}
		else
		{
			card.release();
			LCD_MESSAGERPGM(_i("Card removed"));////MSG_SD_REMOVED c=20
		}
	}
#endif//CARDINSERTED
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
		}

		if (LCD_CLICKED)
		{
			lcd_timeoutToStatus.start();
		}

		(*menu_menu)();

		if (z_menu_expired() || other_menu_expired())
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
	prusa_statistics_update_from_lcd_update();
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

#ifdef PRUSA_SN_SUPPORT
void WorkaroundPrusaSN() {
    const char *SN = PSTR("CZPXInvalidSerialNr");
    for (uint8_t i = 0; i < 20; i++) {
        eeprom_update_byte((uint8_t*)EEPROM_PRUSA_SN + i, pgm_read_byte(SN++));
    }
}
#endif //PRUSA_SN_SUPPORT

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
    
#ifdef DEBUG_PULLUP_CRASH
    MENU_ITEM_FUNCTION_P(_N("Test Pullup Crash"), TestPullupCrash);
#endif // DEBUG_PULLUP_CRASH
    
#ifdef PRUSA_SN_SUPPORT
    MENU_ITEM_FUNCTION_P(_N("Fake serial number"), WorkaroundPrusaSN);////MSG_WORKAROUND_PRUSA_SN c=18
#endif //PRUSA_SN_SUPPORT
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

void lcd_heat_bed_on_load_toggle()
{
    uint8_t value = eeprom_read_byte((uint8_t*)EEPROM_HEAT_BED_ON_LOAD_FILAMENT);
    if (value > 1)
        value = 1;
    else
        value = !value;
    eeprom_update_byte((uint8_t*)EEPROM_HEAT_BED_ON_LOAD_FILAMENT, value);
}
