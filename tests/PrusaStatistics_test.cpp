/**
 * @file
 * @author Marek Kuhn
 */

// For now the functions are just COPIED (lots of depencendies in ultralcd.h)

#include "catch2/catch_test_macros.hpp"
#include <iostream>

static bool VERBOSE_MODE = false;	// If true - output additional info to std:cout

std::string itostr3(int i){
	return std::to_string(i);
}

std::string eeprom_read_word(uint16_t* /*i*/){
	return "eeprom_read";
}

int _millis(){return 10000;}

static int farm_no;
static int busy_state;
static int PAUSED_FOR_USER;
static int status_number;
static int total_filament_used;
static int feedmultiply;
static int longFilenameOLD;
static int starttime;
static int isPrintPaused;
static int IS_SD_PRINTING;
static int farm_status;
static int farm_timer;
static int loading_flag;

static int target_temperature[1];
static int current_temperature[1];
static int target_temperature_bed;
static int current_temperature_bed;

static uint16_t nozzle_diameter;
static uint16_t* EEPROM_NOZZLE_DIAMETER_uM;

static std::string FW_VERSION;

struct Card {
	int paused = 0;
	int percentDone(){ return 50; }
};

static Card card;

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

	isPrintPaused = 0;
	IS_SD_PRINTING = 0;
	farm_status = 0;
	farm_timer = 1;
	loading_flag = 0;

	target_temperature[0] = {215};
	current_temperature[0] = {204};
	target_temperature_bed = 60;
	current_temperature_bed = 55;

	nozzle_diameter = 400;
	EEPROM_NOZZLE_DIAMETER_uM = &nozzle_diameter;

}


// Copy of pre 3.8 version set of functions
namespace old_code
{

// Mocking Serial line
static std::string SERIAL_BUFFER = "";

void SERIAL_ECHO(std::string s){
	SERIAL_BUFFER += s; 
}

void SERIAL_ECHO(int i){
	SERIAL_BUFFER += std::to_string(i);
}

void SERIAL_ECHO(char c){
	SERIAL_BUFFER += char(c);
}

void SERIAL_ECHOLN(std::string s){
	SERIAL_BUFFER += s + "\n";
}

void SERIAL_ECHOLN(char c){
	SERIAL_BUFFER += char(c);
}

void SERIAL_RESET(){
	SERIAL_BUFFER.clear();
}

struct MySerial {
	void print(int i){
		SERIAL_ECHO(i);
	}
	void println(){
		SERIAL_ECHO("\n");
	}
};

static MySerial MYSERIAL;

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

static void prusa_stat_diameter() {
	SERIAL_ECHO("[DIA:");
	SERIAL_ECHO(eeprom_read_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM));
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
     prusa_stat_diameter();
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
			SERIAL_ECHO("{");
			prusa_stat_printerstatus(15);
			prusa_stat_farm_number();
			prusa_stat_printinfo();
			SERIAL_ECHOLN("}");
			status_number = 15;
		}
		else if (isPrintPaused)
		{
			SERIAL_ECHO("{");
			prusa_stat_printerstatus(14);
			prusa_stat_farm_number();
			prusa_stat_printinfo();
			SERIAL_ECHOLN("}");
			status_number = 14;
		}
		else if (IS_SD_PRINTING || loading_flag)
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
			prusa_stat_diameter();
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

		if (IS_SD_PRINTING || loading_flag)
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
}

// Copy of 3.8 version of functions
namespace new_code
{

// Mocking Serial line
static std::string SERIAL_BUFFER = "";

void SERIAL_ECHO(std::string s){
	SERIAL_BUFFER += s; 
}

void SERIAL_ECHO(int i){
	SERIAL_BUFFER += std::to_string(i);
}

void SERIAL_ECHO(char c){
	SERIAL_BUFFER += char(c);
}

void SERIAL_ECHOLN(std::string s){
	SERIAL_BUFFER += s + "\n";
}

void SERIAL_ECHOLN(char c){
	SERIAL_BUFFER += char(c);
	SERIAL_BUFFER += "\n";
}

void SERIAL_RESET(){
	SERIAL_BUFFER.clear();
}

struct MySerial {
	void print(int i){
		SERIAL_ECHO(i);
	}
	void println(){
		SERIAL_ECHO("\n");
	}
};

static MySerial MYSERIAL;

static void prusa_stat_printerstatus(int _status)
{
	SERIAL_ECHO("[PRN:");
	SERIAL_ECHO(_status);
	SERIAL_ECHO(']');
}

static void prusa_stat_farm_number() {
	SERIAL_ECHO("[PFN:");
	SERIAL_ECHO(farm_no);
	SERIAL_ECHO(']');
}

static void prusa_stat_diameter() {
	SERIAL_ECHO("[DIA:");
	SERIAL_ECHO(eeprom_read_word((uint16_t*)EEPROM_NOZZLE_DIAMETER_uM));
	SERIAL_ECHO(']');
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
	SERIAL_ECHO(']');
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
	SERIAL_ECHO(']');
    prusa_stat_diameter();
}

void prusa_statistics_err(char c){
	SERIAL_ECHO("{[ERR:");
	SERIAL_ECHO(c);
	SERIAL_ECHO(']');
	prusa_stat_farm_number();
}

void prusa_statistics_case0(uint8_t statnr){
	SERIAL_ECHO("{");
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
			SERIAL_ECHO("{");
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
	case 4:		// print succesfull
		SERIAL_ECHO("{[RES:1][FIL:");
		MYSERIAL.print(int(_fil_nr));
		SERIAL_ECHO(']');
		prusa_stat_printerstatus(status_number);
		prusa_stat_farm_number();
		farm_timer = 2;
		break;
	case 5:		// print not succesfull
		SERIAL_ECHO("{[RES:0][FIL:");
		MYSERIAL.print(int(_fil_nr));
		SERIAL_ECHO(']');
		prusa_stat_printerstatus(status_number);
		prusa_stat_farm_number();
		farm_timer = 2;
		break;
	case 6:		// print done
		SERIAL_ECHO("{[PRN:8]");
		prusa_stat_farm_number();
		status_number = 8;
		farm_timer = 2;
		break;
	case 7:		// print done - stopped
		SERIAL_ECHO("{[PRN:9]");
		prusa_stat_farm_number();
		status_number = 9;
		farm_timer = 2;
		break;
	case 8:		// printer started
		SERIAL_ECHO("{[PRN:0][PFN:");
		status_number = 0;
		SERIAL_ECHO(farm_no);
		SERIAL_ECHO(']');
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
        SERIAL_ECHO("{[PRN:5]");
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
        SERIAL_ECHO("{[PRN:99]");
        prusa_stat_temperatures();
		SERIAL_ECHO("[PFN:");
		SERIAL_ECHO(farm_no);
		SERIAL_ECHO(']');
            
        break;
	}
	SERIAL_ECHOLN('}');	

}

} // end namespace new

void SERIALS_RESET(){
	old_code::SERIAL_RESET();
	new_code::SERIAL_RESET();
}

std::string SERIALS_SERIALIZE(){
	return old_code::SERIAL_BUFFER + "\n" + new_code::SERIAL_BUFFER;
}
void SERIALS_PRINT(){
	std::cout << "[Printing buffers...] \n";
	std::cout << old_code::SERIAL_BUFFER << "\n";
	std::cout << new_code::SERIAL_BUFFER << "\n";
}

int SERIALS_COMPARE(){
	// Trim the newline at the end

	if(old_code::SERIAL_BUFFER.back() == '\n'){
		old_code::SERIAL_BUFFER.pop_back();
	}
	if(new_code::SERIAL_BUFFER.back() == '\n'){
		new_code::SERIAL_BUFFER.pop_back();
	}

	if(VERBOSE_MODE){
		std::cout << "Comparing: \n";
		std::cout << old_code::SERIAL_BUFFER << "\n";
		std::cout << new_code::SERIAL_BUFFER << "\n";	
	}
	
	return old_code::SERIAL_BUFFER.compare(new_code::SERIAL_BUFFER);
}


// ---------------  TEST CASES ---------------- // 

TEST_CASE("Serials compare ignore newline at the end", "[helper]")
{
	SERIALS_RESET();
	old_code::SERIAL_BUFFER = "Hello compare me.";
	new_code::SERIAL_BUFFER = "Hello compare me.";
	CHECK(SERIALS_COMPARE() == 0);

	SERIALS_RESET();
	old_code::SERIAL_BUFFER = "Hello compare me.\n";
	new_code::SERIAL_BUFFER = "Hello compare me.";
	CHECK(SERIALS_COMPARE() == 0);

	SERIALS_RESET();
	old_code::SERIAL_BUFFER = "Hello compare me.";
	new_code::SERIAL_BUFFER = "Hello compare me.\n";
	CHECK(SERIALS_COMPARE() == 0);
}

TEST_CASE("Printer status is shown", "[prusa_stats]")
{
	SERIALS_RESET();
	setup_mockups();

	old_code::prusa_stat_printerstatus(1);
	new_code::prusa_stat_printerstatus(1);

	INFO(SERIALS_SERIALIZE());
	CHECK(SERIALS_COMPARE() == 0);
}


TEST_CASE("Printer info is shown", "[prusa_stats]")
{
	SERIALS_RESET();
	setup_mockups();

	old_code::prusa_stat_printinfo();
	new_code::prusa_stat_printinfo();

	INFO(SERIALS_SERIALIZE());
	CHECK(SERIALS_COMPARE() == 0);
}

TEST_CASE("Printer temperatures are shown", "[prusa_stats]")
{
	SERIALS_RESET();
	setup_mockups();

	old_code::prusa_stat_temperatures();
	new_code::prusa_stat_temperatures();
	
	INFO(SERIALS_SERIALIZE());
	CHECK(SERIALS_COMPARE() == 0);
}

TEST_CASE("Prusa_statistics test", "[prusa_stats]")
{
	SERIALS_RESET();
	setup_mockups();

	int test_codes[] = {0,1,2,3,4,5,6,7,8,20,21,22,90,91,92,93,99};
	int size = sizeof(test_codes)/sizeof(test_codes[0]);

	for(int i = 0; i < size; i++){

		if(VERBOSE_MODE){
			std::cout << "Testing prusa_statistics(" << std::to_string(i) << ")\n";	
		}
		
		switch(i)
		{
			case 0: {
				busy_state = 0;
				PAUSED_FOR_USER = 0;
				old_code::prusa_statistics(test_codes[i],0);
				new_code::prusa_statistics(test_codes[i],0);
				CHECK(SERIALS_COMPARE() == 0);
				SERIALS_RESET();

				busy_state = 1;
				PAUSED_FOR_USER = 0;
				isPrintPaused = 1;
				old_code::prusa_statistics(test_codes[i],0);
				new_code::prusa_statistics(test_codes[i],0);	
				CHECK(SERIALS_COMPARE() == 0);
				SERIALS_RESET();

				isPrintPaused = 0;
				IS_SD_PRINTING = 1;
				old_code::prusa_statistics(test_codes[i],0);
				new_code::prusa_statistics(test_codes[i],0);
				CHECK(SERIALS_COMPARE() == 0);
				SERIALS_RESET();	

				busy_state = 1;
				PAUSED_FOR_USER = 0;	
				isPrintPaused = 0;			
				IS_SD_PRINTING = 0;
				loading_flag = 0;
				old_code::prusa_statistics(test_codes[i],0);
				new_code::prusa_statistics(test_codes[i],0);
				CHECK(SERIALS_COMPARE() == 0);
				SERIALS_RESET();	
				break;
			}
			case 2: {
				IS_SD_PRINTING = 1;
				old_code::prusa_statistics(test_codes[i],0);
				new_code::prusa_statistics(test_codes[i],0);	
				CHECK(SERIALS_COMPARE() == 0);
				SERIALS_RESET();	

				IS_SD_PRINTING = 0;
				loading_flag = 0;
				old_code::prusa_statistics(test_codes[i],0);
				new_code::prusa_statistics(test_codes[i],0);	
				CHECK(SERIALS_COMPARE() == 0);
				SERIALS_RESET();					

				break;
			}
			default:{

				old_code::prusa_statistics(test_codes[i],0);
				new_code::prusa_statistics(test_codes[i],0);
				CHECK(SERIALS_COMPARE() == 0);
				SERIALS_RESET();
			}
		}
	}
}
