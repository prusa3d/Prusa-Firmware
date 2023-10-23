#include "mmu2_power.h"
#include "Configuration_var.h"
#include "pins.h"
#include "fastio.h"
#include <util/delay.h>
#include "mmu2.h"

namespace MMU2 {

// On MK3 we cannot do actual power cycle on HW. Instead trigger a hardware reset.
void power_on() {
#ifdef MMU_HWRESET
    WRITE(MMU_RST_PIN, 1);
    SET_OUTPUT(MMU_RST_PIN); // setup reset pin
#endif //MMU_HWRESET

    reset();
}

void power_off() {
}

void reset() {
#ifdef MMU_HWRESET // HW - pulse reset pin
    WRITE(MMU_RST_PIN, 0);
    _delay_us(100);
    WRITE(MMU_RST_PIN, 1);
#else
    mmu2.Reset(MMU2::Software); // @@TODO needs to be redesigned, this power implementation shall not know anything about the MMU itself
#endif
    // otherwise HW reset is not available
}

} // namespace MMU2
