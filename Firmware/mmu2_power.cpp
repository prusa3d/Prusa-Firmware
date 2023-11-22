#include "mmu2_power.h"
#include "Configuration_var.h"
#include "pins.h"
#include "fastio.h"
#include <util/delay.h>
#include "mmu2.h"
#include "eeprom.h"

namespace MMU2 {

// sadly, on MK3 we cannot do actual power cycle on HW...
// so we just block the MMU via EEPROM var instead.
void power_on() {
#ifdef MMU_HWRESET
    WRITE(MMU_RST_PIN, 1);
    SET_OUTPUT(MMU_RST_PIN); // setup reset pin
#endif //MMU_HWRESET

    eeprom_update_byte((uint8_t *)EEPROM_MMU_ENABLED, true);

    reset();
}

void power_off() {
    eeprom_update_byte((uint8_t *)EEPROM_MMU_ENABLED, false);
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
