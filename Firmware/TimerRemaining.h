/**
 * @file
 * @author Marek Bel
 */

#ifndef TIMERREMAINING_H
#define TIMERREMAINING_H

#include "Timer.h"
#include "Arduino.h"
#include "system_timer.h"
#include <limits.h>

class TimerRemaining : public LongTimer
{
public:
    TimerRemaining() : m_period(){}
    void start() = delete;
    bool expired(unsigned long msPeriod) = delete;
    /**
     * @brief Start timer
     * @param msPeriod Time to expire in milliseconds
     */
    void start(unsigned long msPeriod)
    {
        m_period = msPeriod;
        LongTimer::start();
    }
    /**
     * @brief Time remaining to expiration
     *
     * @param msPeriod timer period in milliseconds
     * @return time remaining to expiration in milliseconds
     * @retval 0 Timer has expired, or was not even started.
     */
    unsigned long remaining()
    {
      if (!running()) return 0;
      if (expired()) return 0;
      const unsigned long now = _millis();
      return (started() + m_period - now);
    }
    /**
     * @brief Timer has expired.
     * @retval true Timer has expired.
     * @retval false Timer has not expired.
     */
    bool expired()
    {
        return LongTimer::expired(m_period);
    }
private:
    unsigned long m_period; //!< Timer period in milliseconds.
};

#endif // ifndef TIMERREMAINING_H
