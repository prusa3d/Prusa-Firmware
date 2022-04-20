#pragma once
#include <stdint.h>
#include <stddef.h>

namespace MMU2 {

/// A minimal serial interface for the MMU
class MMU2Serial {
public:
    MMU2Serial() = default;
//    bool available()const;
    void begin(uint32_t baud);
    void close();
    int read();
    void flush();
    size_t write(const uint8_t *buffer, size_t size);
};

extern MMU2Serial mmu2Serial;

} // namespace MMU2
