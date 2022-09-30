#include "mmu2_serial.h"
#include "uart2.h"

namespace MMU2 {

void MMU2Serial::begin(uint32_t baud){
    uart2_init(baud); // @@TODO we may skip the baud rate setting in case of 8bit FW ... could save some bytes...
}

void MMU2Serial::close() {
    // @@TODO - probably turn off the UART
}

int MMU2Serial::read() {
    return fgetc(uart2io);
}

void MMU2Serial::flush() {
    // @@TODO - clear the output buffer
}

void MMU2Serial::write(const uint8_t *buffer, size_t size) {
    while(size--){
        fputc(*buffer, uart2io);
        ++buffer;
    }
}

MMU2Serial mmu2Serial;

} // namespace MMU2
