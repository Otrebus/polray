#pragma once

#define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#include <windows.h>

class Timer
{
public:
    Timer();
    ~Timer();

    void Reset();
    double GetTime() const;

private:
    LARGE_INTEGER m_ticksPerSecond;
    LARGE_INTEGER m_startTick;
    LARGE_INTEGER m_dt;
};
