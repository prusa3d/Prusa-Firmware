#pragma once
#include <stdint.h>
#include <stddef.h>

namespace MMU2 {
void TranslateErr(uint16_t ec, char *dst, size_t dstSize);
}
