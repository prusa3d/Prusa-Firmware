//! @file
//! @author: Marek Bel
//! @date Jan 3, 2019

#include "AutoDeplete.h"
#include "assert.h"

//! @brief bit field marking depleted filaments
//!
//! binary 1 marks filament as depleted
//! Zero initialized value means, that no filament is depleted.
static uint8_t depleted;
static const uint8_t filamentCount = 5;

//! @return binary 1 for all filaments
//! @par fCount number of filaments
static constexpr uint8_t allDepleted(uint8_t fCount)
{
    return fCount == 1 ? 1 : ((1 << (fCount - 1)) | allDepleted(fCount - 1));
}

//! @brief Is filament available for printing?
//! @par filament Filament number to be checked
//! @retval true Filament is available for printing.
//! @retval false Filament is not available for printing.
static bool loaded(uint8_t filament)
{
    if (depleted & (1 << filament)) return false;
    return true;
}

//! @brief Mark filament as not available for printing.
//! @par filament filament to be marked
void ad_markDepleted(uint8_t filament)
{
    assert(filament < filamentCount);
    if (filament < filamentCount)
    {
        depleted |= 1 << filament;
    }
}

//! @brief Mark filament as available for printing.
//! @par filament filament to be marked
void ad_markLoaded(uint8_t filament)
{
    assert(filament < filamentCount);
    if (filament < filamentCount)
    {
        depleted &= ~(1 << filament);
    }
}

//! @brief Get alternative filament, which is not depleted
//! @par filament filament
//! @return Filament, if it is depleted, returns next available,
//! if all filaments are depleted, returns filament function parameter.
uint8_t ad_getAlternative(uint8_t filament)
{
    assert(filament < filamentCount);
    for (uint8_t i = 0; i<filamentCount; ++i)
    {
        uint8_t nextFilament = (filament + i) % filamentCount;
        if (loaded(nextFilament)) return nextFilament;
    }
    return filament;
}

//! @brief Are all filaments depleted?
//! @retval true All filaments are depleted.
//! @retval false All filaments are not depleted.
bool ad_allDepleted()
{
    if (allDepleted(filamentCount) == depleted)
    {
        return true;
    }
    return false;
}
