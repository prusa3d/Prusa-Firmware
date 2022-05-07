#pragma once
#include <stdint.h>
#include <stddef.h>

namespace MMU2 {
const uint16_t MMUErrorCodeIndex(uint16_t ec);
void TranslateErr(uint16_t ec, char *dst, size_t dstSize);
}
