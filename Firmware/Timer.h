/*
 * @file
 * @author Marek Bel
 */

#ifndef TIMER_H
#define TIMER_H

/**
 * @brief simple timer
 *
 * Simple and memory saving implementation. Should handle timer register wrap around well.
 * Resolution is one millisecond. To save memory, doesn't store timer period.
 * If you wish timer which is storing period, derive from this.
 */
template <class T>
class Timer
{
public:
    Timer();
    void start();
    void stop(){m_isRunning = false;}
    bool running(){return m_isRunning;}
    bool expired(T msPeriod);
private:
    bool m_isRunning;
    T m_started;
};

/**
 * @brief Timer unsigned long specialization
 *
 * Maximum period is at least 49 days.
 */
using LongTimer = Timer<unsigned long>;
/**
 * @brief Timer unsigned short specialization
 *
 * Maximum period is at least 65 seconds.
 */
using ShortTimer = Timer<unsigned short>;

#endif /* TIMER_H */
