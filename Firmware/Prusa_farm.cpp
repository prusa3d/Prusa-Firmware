#include "Prusa_farm.h"
#include "macros.h"
#include "Marlin.h"
#include "cmdqueue.h"
#include "temperature.h"
#include "cardreader.h"
#include "util.h"
#include "ultralcd.h"
#include "Filament_sensor.h"

#ifdef PRUSA_FARM
uint8_t farm_mode = 0;

static ShortTimer NcTime;
static uint8_t farm_timer = 8;
static uint8_t status_number = 0;
static bool no_response = false;
#ifdef PRUSA_M28
#define CHUNK_SIZE 64 // bytes
#define SAFETY_MARGIN 1
bool prusa_sd_card_upload = false;
char chunk[CHUNK_SIZE+SAFETY_MARGIN];
#endif


static void prusa_statistics_err(char c);
static void prusa_stat_printerstatus(uint8_t _status);
static void prusa_stat_farm_number();
static void prusa_stat_diameter();
static void prusa_stat_temperatures();
static void prusa_stat_printinfo();
static void lcd_send_status();
#ifdef FARM_CONNECT_MESSAGE
static void proc_commands();
static void lcd_connect_printer();
#endif //FARM_CONNECT_MESSAGE
#ifdef PRUSA_M28
static void trace();
#endif


static void prusa_statistics_err(char c) {
    SERIAL_ECHOPGM("{[ERR:");
    SERIAL_ECHO(c);
    SERIAL_ECHO(']');
    prusa_stat_farm_number();
}

static void prusa_statistics_case0(uint8_t statnr) {
    SERIAL_ECHO('{');
    prusa_stat_printerstatus(statnr);
    prusa_stat_farm_number();
    prusa_stat_printinfo();
}

static void prusa_stat_printerstatus(uint8_t _status) {
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

static void prusa_stat_temperatures() {
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

static void prusa_stat_printinfo() {
    SERIAL_ECHOPGM("[TFU:");
    SERIAL_ECHO(total_filament_used);
    SERIAL_ECHOPGM("][PCD:");
    SERIAL_ECHO((int)card.percentDone());
    SERIAL_ECHOPGM("][FEM:");
    SERIAL_ECHO(feedmultiply);
    SERIAL_ECHOPGM("][FNM:");
    SERIAL_ECHO(card.longFilename[0] ? card.longFilename : card.filename);
    SERIAL_ECHOPGM("][TIM:");
    if (starttime != 0) {
        SERIAL_ECHO(_millis() / 1000 - starttime / 1000);
    }
    else {
        SERIAL_ECHO(0);
    }
    SERIAL_ECHOPGM("][FWR:");
    SERIAL_ECHORPGM(FW_VERSION_STR_P());
    SERIAL_ECHO(']');
    prusa_stat_diameter();
}

static void lcd_send_status() {
    if (farm_mode && no_response && (NcTime.expired(NC_TIME * 1000))) {
        //send important status messages periodicaly
        prusa_statistics(8);
        NcTime.start();
#ifdef FARM_CONNECT_MESSAGE
        lcd_connect_printer();
#endif //FARM_CONNECT_MESSAGE
    }
}

#ifdef FARM_CONNECT_MESSAGE
static void proc_commands() {
    if (buflen) {
        process_commands();
        if (!cmdbuffer_front_already_processed)
            cmdqueue_pop_front();
        cmdbuffer_front_already_processed = false;
    }
}

static void lcd_connect_printer() {
    lcd_update_enable(false);
    lcd_clear();

    int i = 0;
    int t = 0;
    lcd_puts_at_P(0, 0, PSTR("Connect printer to")); 
    lcd_puts_at_P(0, 1, PSTR("monitoring or hold"));
    lcd_puts_at_P(0, 2, PSTR("the knob to continue"));
    while (no_response) {
        i++;
        t++;
        delay_keep_alive(100);
        proc_commands();
        if (t == 10) {
            prusa_statistics(8);
            t = 0;
        }
        if (READ(BTN_ENC)) { //if button is not pressed
            i = 0; 
            lcd_puts_at_P(0, 3, PSTR("                    "));
        }
        if (i != 0)
            lcd_putc_at((i * 20) / (NC_BUTTON_LONG_PRESS * 10), 3, LCD_STR_SOLID_BLOCK[0]);
        if (i == NC_BUTTON_LONG_PRESS * 10)
            no_response = false;
    }
    lcd_update_enable(true);
    lcd_update(2);
}
#endif //FARM_CONNECT_MESSAGE

#ifdef PRUSA_M28
static void trace() {
    Sound_MakeCustom(25,440,true);
}

void serial_read_stream() {

    setTargetHotend(0);
    setTargetBed(0);

    lcd_clear();
    lcd_puts_P(PSTR(" Upload in progress"));

    // first wait for how many bytes we will receive
    uint32_t bytesToReceive;

    // receive the four bytes
    char bytesToReceiveBuffer[4];
    for (int i=0; i<4; i++) {
        int data;
        while ((data = MYSERIAL.read()) == -1) {};
        bytesToReceiveBuffer[i] = data;

    }

    // make it a uint32
    memcpy(&bytesToReceive, &bytesToReceiveBuffer, 4);

    // we're ready, notify the sender
    MYSERIAL.write('+');

    // lock in the routine
    uint32_t receivedBytes = 0;
    while (prusa_sd_card_upload) {
        int i;
        for (i=0; i<CHUNK_SIZE; i++) {
            int data;

            // check if we're not done
            if (receivedBytes == bytesToReceive) {
                break;
            }

            // read the next byte
            while ((data = MYSERIAL.read()) == -1) {};
            receivedBytes++;

            // save it to the chunk
            chunk[i] = data;
        }

        // write the chunk to SD
        card.write_command_no_newline(&chunk[0]);

        // notify the sender we're ready for more data
        MYSERIAL.write('+');

        // for safety
        manage_heater();

        // check if we're done
        if(receivedBytes == bytesToReceive) {
            trace(); // beep
            card.closefile();
            prusa_sd_card_upload = false;
            SERIAL_PROTOCOLLNRPGM(MSG_FILE_SAVED);
        }
    }
}
#endif //PRUSA_M28


void prusa_statistics(uint8_t _message) {
    const uint8_t _fil_nr = 0;
    if (!farm_mode)
        return;
    
    switch (_message) {
    case 0: // default message
        if (busy_state == PAUSED_FOR_USER) {
            prusa_statistics_case0(15);
        }
        else if (isPrintPaused) {
            prusa_statistics_case0(14);
        }
        else if (IS_SD_PRINTING || loading_flag) {
            prusa_statistics_case0(4);
        }
        else {
            SERIAL_ECHO('{');
            prusa_stat_printerstatus(1);
            prusa_stat_farm_number();
            prusa_stat_diameter();
            status_number = 1;
        }
        break;

    case 1: // 1 heating
        SERIAL_ECHO('{');
        prusa_stat_printerstatus(2);
        prusa_stat_farm_number();
        status_number = 2;
        farm_timer = 1;
        break;

    case 2: // heating done
        SERIAL_ECHO('{');
        prusa_stat_printerstatus(3);
        prusa_stat_farm_number();
        SERIAL_ECHOLN('}');
        status_number = 3;
        farm_timer = 1;

        if (IS_SD_PRINTING || loading_flag) {
            SERIAL_ECHO('{');
            prusa_stat_printerstatus(4);
            prusa_stat_farm_number();
            status_number = 4;
        }
        else {
            SERIAL_ECHO('{');
            prusa_stat_printerstatus(3);
            prusa_stat_farm_number();
            status_number = 3;
        }
        farm_timer = 1;
        break;

    case 3: // filament change
        // must do a return here to prevent doing SERIAL_ECHOLN("}") at the very end of this function
        // saved a considerable amount of FLASH
        return;
        break;
    case 4: // print succesfull
        SERIAL_ECHOPGM("{[RES:1][FIL:");
        MYSERIAL.print(int(_fil_nr));
        SERIAL_ECHO(']');
        prusa_stat_printerstatus(status_number);
        prusa_stat_farm_number();
        farm_timer = 2;
        break;
    case 5: // print not succesfull
        SERIAL_ECHOPGM("{[RES:0][FIL:");
        MYSERIAL.print(int(_fil_nr));
        SERIAL_ECHO(']');
        prusa_stat_printerstatus(status_number);
        prusa_stat_farm_number();
        farm_timer = 2;
        break;
    case 6: // print done
        SERIAL_ECHOPGM("{[PRN:8]");
        prusa_stat_farm_number();
        status_number = 8;
        farm_timer = 2;
        break;
    case 7:        // print done - stopped
        SERIAL_ECHOPGM("{[PRN:9]");
        prusa_stat_farm_number();
        status_number = 9;
        farm_timer = 2;
        break;
    case 8: // printer started
        SERIAL_ECHOPGM("{[PRN:0]");
        prusa_stat_farm_number();
        status_number = 0;
        farm_timer = 2;
        break;
    case 20: // echo farm no
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

    case 99: // heartbeat
        SERIAL_ECHOPGM("{[PRN:99]");
        prusa_stat_temperatures();
        prusa_stat_farm_number();
        break;
    }
    SERIAL_ECHOLN('}');
}

void prusa_statistics_update_from_status_screen() {
    if (farm_mode) {
        farm_timer--;
        if (farm_timer < 1) {
            farm_timer = 10;
            prusa_statistics(0);
        }
        switch (farm_timer) {
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
    }
}

void prusa_statistics_update_from_lcd_update() {
    lcd_send_status();
}

void farm_mode_init() {
    farm_mode = eeprom_init_default_byte((uint8_t*)EEPROM_FARM_MODE, 0); 
    if (farm_mode) {
        no_response = true; //we need confirmation by recieving PRUSA thx
        prusa_statistics(8);
#ifdef HAS_SECOND_SERIAL_PORT
        selectedSerialPort = 1;
#endif //HAS_SECOND_SERIAL_PORT
        MYSERIAL.begin(BAUDRATE);
#ifdef FILAMENT_SENSOR
        //to be converted to Filament_sensor.h...
        //disabled filament autoload (PFW360)
        fsensor.setAutoLoadEnabled(false);
#endif //FILAMENT_SENSOR
        // ~ FanCheck -> on
        eeprom_update_byte((uint8_t*)EEPROM_FAN_CHECK_ENABLED, true);
    }
}

bool farm_prusa_code_seen() {
    if (!farm_mode)
        return false;
    
    if (code_seen_P(PSTR("PRN"))) { // PRUSA PRN
        printf_P(_N("%u"), status_number);
    }
    else if (code_seen_P(PSTR("thx"))) { // PRUSA thx
        no_response = false;
    }
#ifdef PRUSA_M28
    else if (code_seen_P(PSTR("M28"))) { // PRUSA M28
        trace();
        prusa_sd_card_upload = true;
        card.openFileWrite(strchr_pointer+4);
    }
#endif //PRUSA_M28
    else if (code_seen_P(PSTR("fv"))) { // PRUSA fv
        // get file version
#ifdef SDSUPPORT
        card.openFileReadFilteredGcode(strchr_pointer + 3, true);
        while (true) {
            uint16_t readByte = card.getFilteredGcodeChar();
            MYSERIAL.write(readByte);
            if (readByte == '\n') {
                break;
            }
        }
        card.closefile();
#endif // SDSUPPORT
    }
    else {
        return false;
    }
    
    return true;
}

void farm_gcode_g98() {
    farm_mode = 1;
    eeprom_update_byte((unsigned char *)EEPROM_FARM_MODE, farm_mode);
    SilentModeMenu = SILENT_MODE_OFF;
    eeprom_update_byte((unsigned char *)EEPROM_SILENT, SilentModeMenu);
    fCheckModeInit(); // alternatively invoke printer reset
}

void farm_gcode_g99() {
    farm_disable();
    lcd_update(2);
    fCheckModeInit(); // alternatively invoke printer reset
}

void farm_disable() {
    farm_mode = false;
    eeprom_update_byte((uint8_t*)EEPROM_FARM_MODE, farm_mode);
}

#else //PRUSA_FARM

void prusa_statistics(_UNUSED uint8_t message) {
}

void prusa_statistics_update_from_status_screen() {
}

void prusa_statistics_update_from_lcd_update() {
}

void farm_mode_init() {
}

bool farm_prusa_code_seen() {
    return false;
}

void farm_gcode_g98() {
}

void farm_gcode_g99() {
}

void farm_disable() {
}

#endif //PRUSA_FARM

