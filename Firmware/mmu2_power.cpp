#include "mmu2_power.h"
#include "Configuration_prusa.h"
#include "pins.h"
#include "fastio.h"
#include <util/delay.h>

namespace MMU2 {

// sadly, on MK3 we cannot do this on HW
void power_on() { }

void power_off() { }

void reset() {
#ifdef MMU_HWRESET // HW - pulse reset pin
    WRITE(MMU_RST_PIN, 0);
    _delay_us(100);
    WRITE(MMU_RST_PIN, 1);
#endif
    // otherwise HW reset is not available
}

} // namespace MMU2
