#include "mmu2_power.h"
#include "Configuration_prusa.h"
#include "pins.h"
#include "fastio.h"
#include <util/delay.h>
#include "mmu2.h"
#include "eeprom.h"

namespace MMU2 {

// sadly, on MK3 we cannot do actual power cycle on HW...
// so we just block the MMU via EEPROM var instead.
void power_on()
{
    if (!eeprom_read_byte((uint8_t *)EEPROM_MMU_ENABLED))
    {
        eeprom_update_byte((uint8_t *)EEPROM_MMU_ENABLED, true);
    }
}

void power_off()
{
    if (eeprom_read_byte((uint8_t *)EEPROM_MMU_ENABLED))
    {
        eeprom_update_byte((uint8_t *)EEPROM_MMU_ENABLED, false);
    }
}

void reset() {
#ifdef MMU_HWRESET // HW - pulse reset pin
    WRITE(MMU_RST_PIN, 0);
    _delay_us(100);
    WRITE(MMU_RST_PIN, 1);
#else
    MMU2::Reset(MMU2::Software);
#endif
    // otherwise HW reset is not available
}

} // namespace MMU2
