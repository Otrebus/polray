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

extern float timer1, timer2, timer3;

extern Timer* timer;
extern Timer* totaltime;
extern bool g_isActive;
extern bool g_quitting;
extern Gfx* gfx;
extern Input* input;
extern bool* grid;
extern Rasterizer* rasterizer;
extern int* texture;
extern LPDIRECTDRAWSURFACE7 bitmapsurface;
extern DDSURFACEDESC2 ddsd;
//extern int* ptr;
extern int currenttick;
extern int numintersects;
extern int numcalls;
extern int nodes;
extern int nodetriangles;
extern float rendertime;
extern float setuptime;
extern int maxdepth;
extern bool mouseclicked;
extern HANDLE bufferMutex;
extern HANDLE workingMutex;
extern int nCurrentFrame;
extern int frames;

#define MAX_DEPTH 50
#define XRES 640
#define YRES 480
//#define DETERMINISTIC

LRESULT WINAPI  WndProc(HWND, UINT, WPARAM, LPARAM);

#endif