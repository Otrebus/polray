//#define _CRTDBG_MAP_ALLOC

#ifndef MAIN_H
#define MAIN_H

#define WIN32_MEAN_AND_LEAN
#define _USE_MATH_DEFINES

#include "logger.h"
#include "timer.h"
#include "input.h"
#include "logger.h"
#include "Gfx.h"
#include <windows.h>
#include "Texture.h"

extern bool g_isActive;
extern bool g_quitting;
extern Gfx* gfx;
extern Input* input;
extern bool* grid;
extern LPDIRECTDRAWSURFACE7 bitmapsurface;
extern DDSURFACEDESC2 ddsd;

extern HANDLE bufferMutex;
extern HANDLE workingMutex;
extern int nCurrentFrame;
extern int frames;

#define XRES 480
#define YRES 320
//#define DETERMINISTIC

LRESULT WINAPI  WndProc(HWND, UINT, WPARAM, LPARAM);

#endif