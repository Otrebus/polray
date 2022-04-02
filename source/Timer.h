#ifndef TIMER_H
#define TIMER_H

#define WIN32_MEAN_AND_LEAN
#include <windows.h>

class Timer
{
public:
    Timer();
    ~Timer();

    void Reset();
    float GetTime() const;

private:
    LARGE_INTEGER m_ticksPerSecond;
    LARGE_INTEGER m_startTick;
    LARGE_INTEGER m_dt;
};

#endif

