#pragma once

enum PowerPanicFlag : uint8_t {
    NO_PENDING_RECOVERY = 0,
    PENDING_RECOVERY = 1, // First power panic, print state is saved in EEPROM
    PENDING_RECOVERY_RETRY = 2, // Power outage occured during recovery, print is still saved in EEPROM
};

void uvlo_();
void recover_print(uint8_t automatic); 
void setup_uvlo_interrupt();
