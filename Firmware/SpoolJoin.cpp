#include "SpoolJoin.h"
#include "Marlin.h"
#include "eeprom.h"
#include "messages.h"
#include "language.h"

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
    eeprom_write_byte((uint8_t*)EEPROM_SPOOL_JOIN, (uint8_t)status);
}

void SpoolJoin::initSpoolJoinStatus()
{
    EEPROM currentStatus = (EEPROM)eeprom_read_byte((uint8_t*)EEPROM_SPOOL_JOIN);
    if( currentStatus == EEPROM::Empty)
    {
        // By default SpoolJoin is disabled
        updateSpoolJoinStatus(EEPROM::Disabled);
    } else {
        updateSpoolJoinStatus(currentStatus);
    }

    // Useful information to see during bootup
    SERIAL_ECHOPGM("SpoolJoin is ");
    if (isSpoolJoinEnabled())
    {
        SERIAL_ECHOLNRPGM(_O(MSG_ON));
    } else {
        SERIAL_ECHOLNRPGM(_O(MSG_OFF));
    }
}

void SpoolJoin::toggleSpoolJoin()
{
    if (eeprom_read_byte((uint8_t*)EEPROM_SPOOL_JOIN) == (uint8_t)EEPROM::Disabled)
    {
        eeprom_write_byte((uint8_t*)EEPROM_SPOOL_JOIN, (uint8_t)EEPROM::Enabled);
    } else {
        eeprom_write_byte((uint8_t*)EEPROM_SPOOL_JOIN, (uint8_t)EEPROM::Disabled);
    }
}

bool SpoolJoin::isSpoolJoinEnabled()
{
    if(eeprom_read_byte((uint8_t*)EEPROM_SPOOL_JOIN) == (uint8_t)EEPROM::Enabled) {
        return true;
    } else {
        return false;
    }
}

void SpoolJoin::setSlot(uint8_t slot)
{
    currentMMUSlot = slot;
}

uint8_t SpoolJoin::nextSlot()
{
    SERIAL_ECHOPGM("SpoolJoin: ");
    SERIAL_ECHO((int)currentMMUSlot);

    if (currentMMUSlot >= 4) currentMMUSlot = 0;
    else currentMMUSlot++;

    SERIAL_ECHOPGM(" -> ");
    SERIAL_ECHOLN((int)currentMMUSlot);

    return currentMMUSlot;
}

}
