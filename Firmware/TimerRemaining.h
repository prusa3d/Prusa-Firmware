/**
 * @file
 * @author Marek Bel
 */

#ifndef TIMERREMAINING_H
#define TIMERREMAINING_H

#include "Timer.h"
#include "Arduino.h"
#include <limits.h>

class TimerRemaining : public LongTimer
{
public:
    /**
     * @brief Time remaining to expiration
     *
     * @param msPeriod timer period in milliseconds
     * @return time remaining to expiration in milliseconds
     * @retval 0 Timer has expired, or was not even started.
     */
    unsigned long remaining(unsigned long msPeriod)
    {
      if (!m_isRunning) return 0;
      if (expired(msPeriod)) return 0;
      const unsigned long now = millis();
      if ((m_started <=  m_started + msPeriod) || (now < m_started))
      {
          return (m_started + msPeriod - now);
      }
      else //(now >= m_started)
      {
          return ULONG_MAX - now + (m_started + msPeriod);
      }
    }
};

#endif // ifndef TIMERREMAINING_H
