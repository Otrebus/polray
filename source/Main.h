/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Main.h
 * 
 * Declares the entry point and some global variables.
 */

#pragma once

#define WIN32_MEAN_AND_LEAN
#define NOMINMAX

#include <windows.h>
#include "Gfx.h"

extern bool g_isActive;
extern bool g_quitting;
extern Gfx* gfx;
extern DDSURFACEDESC2 ddsd;

extern HANDLE bufferMutex;

#define XRES 640
#define YRES 480
//#define DETERMINISTIC

LRESULT WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
