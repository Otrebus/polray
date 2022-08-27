#pragma once

#define WIN32_MEAN_AND_LEAN
#define _USE_MATH_DEFINES
#define NOMINMAX

#include "logger.h"
#include "input.h"
#include "logger.h"
#include "Gfx.h"
#include <windows.h>

extern bool g_isActive;
extern bool g_quitting;
extern Gfx* gfx;
extern Input* input;
extern LPDIRECTDRAWSURFACE7 bitmapsurface;
extern DDSURFACEDESC2 ddsd;

extern HANDLE bufferMutex;

#define XRES 320
#define YRES 200
//#define DETERMINISTIC

LRESULT WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
