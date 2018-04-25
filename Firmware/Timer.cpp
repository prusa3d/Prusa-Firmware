/**
 * @file
 * @author Marek Bel
 */

#include "Timer.h"
#include "Arduino.h"

/**
 * @brief construct Timer
 *
 * It is guaranteed, that construction is equivalent with zeroing all members.
 * This property can be exploited in MenuData union.
 */
Timer::Timer() : m_isRunning(false), m_started()
{
}

/**
 * @brief Start timer
 */
void Timer::start()
{
    m_started = millis();
    m_isRunning = true;
}

/**
 * @brief Timer has expired
 *
 * Timer is considered expired after msPeriod has passed from time the timer was started.
 * This function must be called at least each (unsigned long maximum value - msPeriod) milliseconds to be sure to
 * catch first expiration.
 * This function is expected to handle wrap around of time register well.
 *
 * @param msPeriod Time interval in milliseconds.
 * @retval true Timer has expired
 * @retval false Timer not expired yet, or is not running, or time window in which is timer considered expired passed.
 */
bool Timer::expired(unsigned long msPeriod)
{
    if (!m_isRunning) return false;
    bool expired = false;
    const unsigned long now = millis();
    if (m_started <=  m_started + msPeriod)
    {
        if ((now >= m_started + msPeriod) || (now < m_started))
        {
            expired = true;
        }
    }
    else
    {
        if ((now >= m_started + msPeriod) && (now < m_started))
        {
            expired = true;
        }
    }
    if (expired) m_isRunning = false;
    return expired;
}
