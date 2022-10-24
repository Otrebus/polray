/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Timer.cpp
 * 
 * Implementation of the Timer class.
 */

#include "Timer.h"

/**
 * Constructor.
 */
Timer::Timer()
{
    QueryPerformanceFrequency(&m_ticksPerSecond);
    Reset();
}

/**
 * Destructor.
 */
Timer::~Timer()
{
}

/**
 * Resets the timer.
 */
void Timer::Reset()
{
    QueryPerformanceCounter(&m_startTick);
}

/**
 * Returns the time passed in seconds since the reset.
 */
double Timer::GetTime() const
{
    LARGE_INTEGER thisTick;
    QueryPerformanceCounter(&thisTick);
    return (thisTick.QuadPart - m_startTick.QuadPart)/(double)m_ticksPerSecond.QuadPart;
}