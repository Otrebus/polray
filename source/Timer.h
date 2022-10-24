/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Timer.h
 * 
 * Declaration of the Timer class.
 */

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
