#pragma once

#define WIN32_MEAN_AND_LEAN
#define NOMINMAX

#include <windows.h>
#include "Gfx.h"

extern bool g_isActive;
extern bool g_quitting;
extern Gfx* gfx;
extern LPDIRECTDRAWSURFACE7 bitmapsurface;
extern DDSURFACEDESC2 ddsd;

extern HANDLE bufferMutex;

#define XRES 640
#define YRES 480
//#define DETERMINISTIC

LRESULT WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
