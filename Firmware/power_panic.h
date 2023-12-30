#pragma once

namespace PowerPanic {
enum PowerPanicFlag : uint8_t {
    NO_PENDING_RECOVERY = 0,
    PENDING_RECOVERY = 1, // First power panic, print state is saved in EEPROM
    PENDING_RECOVERY_RETRY = 2, // Power outage occured during recovery, print is still saved in EEPROM
};

// Types of printjobs possible when power panic is triggered
enum PrintType : uint8_t {
    PRINT_TYPE_SD = 0,
    PRINT_TYPE_USB = 1,
    PRINT_TYPE_NONE = 2,
};
} // namespace PowerPanic

void uvlo_();
void recover_print(uint8_t automatic); 
void setup_uvlo_interrupt();
void restore_file_from_sd();
