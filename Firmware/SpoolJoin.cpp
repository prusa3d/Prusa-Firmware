#include "SpoolJoin.h"
#include "eeprom.h"
#include "Filament_sensor.h"

namespace SpoolJoin {

SpoolJoin spooljoin;

SpoolJoin::SpoolJoin()
    : status(EEPROM::Unknown)
    , currentMMUSlot(0)
{
}

void SpoolJoin::updateSpoolJoinStatus(EEPROM newStatus)
{
    status = newStatus;
    eeprom_write_byte((uint8_t*)EEPROM_AUTO_DEPLETE, (uint8_t)status);
}

void SpoolJoin::initSpoolJoinStatus()
{
    EEPROM currentStatus = (EEPROM)eeprom_read_byte((uint8_t*)EEPROM_AUTO_DEPLETE);
    if( currentStatus == EEPROM::Empty)
    {
        // By default SpoolJoin is disabled
        updateSpoolJoinStatus(EEPROM::Disabled);
    } else {
        updateSpoolJoinStatus(currentStatus);
    }
}

void SpoolJoin::toggleSpoolJoin()
{
    if (eeprom_read_byte((uint8_t*)EEPROM_AUTO_DEPLETE) == (uint8_t)EEPROM::Disabled)
    {
        eeprom_write_byte((uint8_t*)EEPROM_AUTO_DEPLETE, (uint8_t)EEPROM::Enabled);
    } else {
        eeprom_write_byte((uint8_t*)EEPROM_AUTO_DEPLETE, (uint8_t)EEPROM::Disabled);
    }
}

uint8_t SpoolJoin::isSpoolJoinEnabled()
{
    if(eeprom_read_byte((uint8_t*)EEPROM_AUTO_DEPLETE) == (uint8_t)EEPROM::Enabled
#ifdef FILAMENT_SENSOR
    && fsensor.isReady()
#endif
    )
    {
        return 1;
    } else {
        return 0;
    }
}


uint8_t SpoolJoin::nextSlot()
{
    if (currentMMUSlot == 4) currentMMUSlot = 0;
    else currentMMUSlot++;

    return currentMMUSlot;
}

}
