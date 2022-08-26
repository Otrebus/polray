#pragma once

#define WIN32_MEAN_AND_LEAN
#include "ddraw.h"
#include <string.h>
#include <assert.h>
#include <windows.h>
#include <math.h>
#include <string>

using namespace std;

typedef unsigned char uchar;

class Gfx
{
    friend class Rasterizer;
public:
    Gfx();
    ~Gfx();
    bool Initialize(HWND hwnd, bool windowed, int xres, int yres);
    bool SwitchMode(bool windowed);
    bool ChangeDisplayMode(bool, int, int, int);
    void RestoreAllSurfaces();
    void OnWindowMove();

    bool Flip();
    bool Lock();
    bool Unlock();

    bool Text(int xout, int yout, const char *txout, int r, int g, int b);  

    // Plots to the backbuffer
    void Plot(int x, int y, uchar r, uchar g, uchar b);
    void Plot(int x, int y, int color);

    // Plots directly to the client area
    void PutPixel(int x, int y, uchar r, uchar g, uchar b);
    
    void Line(int fromx, int fromy, int tox, int toy, uchar r, uchar g, uchar b);
    void Circle(int x, int y, int radius, uchar r, uchar g, uchar b);
    bool ClearScreen(uchar, uchar, uchar);

    bool IsWindowed() const;
    bool IsReady() const;

    int GetYRes() const;
    int GetXRes() const;

    void DestroySurfaces();
    bool CreateSurfaces(bool, int, int, int);

    string GetLastError() const;

    string					m_lastError;
    bool                    m_isLocked;
    bool                    m_isWindowed;
    int                     m_xres, m_yres;
    HWND                    m_hWnd;
    LPDIRECTDRAW7           m_pDD;
    LPDIRECTDRAWSURFACE7    m_pDDSPrimary; // DirectDraw primary surface
    LPDIRECTDRAWSURFACE7    m_pDDSBack;
    DDSURFACEDESC2          m_ddsd;
    LPRECT                  m_pClientRect;
    LPRECT                  m_pScreenRect;
    LPDIRECTDRAWCLIPPER     m_pClipper;
};
