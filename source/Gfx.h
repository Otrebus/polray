/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Gfx.h
 * 
 * Declaration of functions that handle graphics.
 */

#pragma once

#define NOMINMAX
#define WIN32_MEAN_AND_LEAN
#include "ddraw.h"
#include <string>

class Gfx
{
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

    // Plots to the backbuffer
    void Plot(int x, int y, int r, int g, int b);
    void Plot(int x, int y, int color);

    // Plots directly to the client area
    
    bool ClearScreen(int, int, int);

    bool IsWindowed() const;

    void DestroySurfaces();
    bool CreateSurfaces(bool, int, int, int);

    std::string             m_lastError;
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
