#pragma once
#include <stdint.h>
#include <stddef.h>

namespace MMU2 {
void TranslateProgress(uint16_t pc, char *dst, size_t dstSize);
}
