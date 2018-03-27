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
 * Maximum period is at least 49 days. Resolution is one millisecond. To save memory, doesn't store timer period.
 * If you wish timer which is storing period, derive from this. If you need time intervals smaller than 65 seconds
 * consider implementing timer with smaller underlying type.
 */
class Timer
{
public:
    Timer();
    void start();
    void stop(){m_isRunning = false;}
    bool running(){return m_isRunning;}
    bool expired(unsigned long msPeriod);
private:
    bool m_isRunning;
    unsigned long m_started;
};

#endif /* TIMER_H */
