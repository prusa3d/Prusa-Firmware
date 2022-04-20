#include "mmu2_serial.h"

//@@TODO implement for MK3

namespace MMU2 {

void MMU2Serial::begin(uint32_t baud){ }
void MMU2Serial::close() { }
int MMU2Serial::read() { }
void MMU2Serial::flush() { }
size_t MMU2Serial::write(const uint8_t *buffer, size_t size) { }

MMU2Serial mmu2Serial;

} // namespace MMU2
