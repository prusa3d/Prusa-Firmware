/**
 * @file
 * @author Marek Kuhn
 */

// For now the functions are just COPIED (lots of depencendies in ultralcd.h)

#include "catch.hpp"
#include <iostream>

// Mocking Serial line
std::string SERIAL_BUFFER = "";

//#define SERIAL_ECHO(s) ( SERIAL_BUFFER += s )
/*
#define SERIAL_FLUSH() ( SERIAL_BUFFER.clear() )
#define SERIAL_ECHOLN(s) { 	\
	SERIAL_ECHO(s); 	 	\
	SERIAL_BUFFER += "\n"; 	\
}
*/
void SERIAL_ECHO(std::string s){
	SERIAL_BUFFER += s; 
}

void SERIAL_ECHO(int i){
	SERIAL_BUFFER += std::to_string(i);
}

void SERIAL_ECHOLN(std::string s){
	SERIAL_BUFFER += s + "\n";
}

void SERIAL_FLUSH(){
	SERIAL_BUFFER.clear();
}

std::string itostr3(int i){
	return std::to_string(i);
}

int _millis(){return 10000;}

int farm_no;
int busy_state;
int PAUSED_FOR_USER;
int status_number;
int total_filament_used;
int feedmultiply;
int longFilenameOLD;
int starttime;

int target_temperature[1] = {215};
int current_temperature[1] = {204};
int target_temperature_bed = 60;
int current_temperature_bed = 55;

std::string FW_VERSION;

struct Card {
	int percentDone(){ return 50; }
} card;

void setup_mockups(){
	farm_no = 0;

	busy_state = 0;
	status_number = 0;
	PAUSED_FOR_USER = 0;

	total_filament_used = 0;
	feedmultiply = 0;
	longFilenameOLD = 0;
	starttime = 0;

	FW_VERSION = "3.8.0";
}

/*
void prusa_statistics(int _message, uint8_t _fil_nr) {
#ifdef DEBUG_DISABLE_PRUSA_STATISTICS
	return;
#endif //DEBUG_DISABLE_PRUSA_STATISTICS
	switch (_message)
	{

	case 0: // default message
		if (busy_state == PAUSED_FOR_USER) 
		{
			SERIAL_ECHO("{");
			prusa_stat_printerstatus(15);
			prusa_stat_farm_number();
			prusa_stat_printinfo();
			SERIAL_ECHOLN("}");
			status_number = 15;
		}
		else if (isPrintPaused || card.paused) 
		{
			SERIAL_ECHO("{");
			prusa_stat_printerstatus(14);
			prusa_stat_farm_number();
			prusa_stat_printinfo();
			SERIAL_ECHOLN("}");
			status_number = 14;
		}
		else if (IS_SD_PRINTING)
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
*/
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
		SERIAL_ECHO(_millis() / 1000 - starttime / 1000);
	}
	else
	{
		SERIAL_ECHO(0);
	}
	SERIAL_ECHO("][FWR:");
	SERIAL_ECHO(FW_VERSION);
	SERIAL_ECHO("]");
}


TEST_CASE("Printer status is shown", "[prusa_stats]")
{
	SERIAL_FLUSH();
	prusa_stat_printerstatus(1);
	CHECK(SERIAL_BUFFER.compare("[PRN:1]") == 0);
}

TEST_CASE("Printer info is showsn", "[prusa_stats]")
{
	SERIAL_FLUSH();
	setup_mockups();
	prusa_stat_printinfo();
	CHECK(SERIAL_BUFFER.compare("[TFU:0][PCD:50][FEM:0][FNM:0][TIM:0][FWR:3.8.0]") == 0);
}

TEST_CASE("Printer temperatures are shown", "[prusa_stats]")
{
	SERIAL_FLUSH();
	setup_mockups();
	prusa_stat_temperatures();
	CHECK(SERIAL_BUFFER.compare("[ST0:215][STB:60][AT0:204][ATB:55]") == 0);
}

TEST_CASE("Prusa_statistics Paused test", "[prusa_stats]")
{
	

	CHECK(1 == 1);
}
