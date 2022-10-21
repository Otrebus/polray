/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Gfx.cpp
 * 
 * Implementation of the Gfx class that handles graphics plotting.
 */

#include "gfx.h"
#include <cassert>

/**
 * Constructor
 */
Gfx::Gfx()
{
    m_isLocked = false;
    m_pDD = 0;
    m_pDDSPrimary = m_pDDSBack = 0;
    m_pClipper = 0;
}

/**
 * Destructor
 */
Gfx::~Gfx() 
{
    // The application freezes at any of these if the Directdraw object has been
    // used in another thread apparently.
    if(m_isLocked)
        Unlock();
    m_pDD->RestoreDisplayMode();
        
    if(m_pDD)
        m_pDD->Release();

    // TODO: delete m_pClientRect = new RECT;
    //              m_pScreenRect = new RECT;
}

/**
 * Initializes Directdraw, sets the cooperative mode and the display mode, given
 * a window handle.
 * 
 * @param hWnd The handle of the window
 * @param windowed Whether we are in windowed mode
 * @param xres The horizontal resolution
 * @param yres The vertical resolution
 * @returns True If the initialization succeeded
 */
bool Gfx::Initialize(HWND hWnd, bool windowed, int xres, int yres)
{
    m_hWnd = hWnd;
    m_isWindowed = windowed;
    m_xres = xres;
    m_yres = yres;
    m_isLocked = false;

    if(DirectDrawCreateEx(NULL, (LPVOID*) &m_pDD, IID_IDirectDraw7, NULL) != DD_OK)
    {
        m_lastError = "DirectDrawCreateEx failed in Gfx::Initialize";
        return false;
    }

    if(windowed)
    {
        if(m_pDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL) != DD_OK)
        {
            m_lastError = "SetCooperativeLevel failed in Gfx::Initialize";
            return false;
        }

        OnWindowMove();
        return CreateSurfaces(true, 32, xres, yres);
    }
  
    if(m_pDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) != DD_OK)
    {
        m_lastError = "SetCooperativeLevel failed in Gfx::Initialize";
        m_pDD->Release();
        return false;
    }

    if(m_pDD->SetDisplayMode(xres, yres, 32, 0, 0) != DD_OK)
    {
        m_lastError = "SetDisplayMode failed in Gfx::Initialize";
        m_pDD->Release();
        return false;
    }
    return CreateSurfaces(false, 32, xres, yres);
}

/**
 * Plots a pixel with the given color at the given coordinate on the backbuffer.
 * 
 * @param The x coordinate of the pixel to be plotted.
 * @param The y coordinate of the pixel to be plotted.
 * @param r The red component of the pixel (0-255).
 * @param g The green component of the pixel (0-255).
 * @param b The blue component of the pixel (0-255).
 */
void Gfx::Plot(int x, int y, int r, int g, int b)
{
    assert(m_isLocked);
    if(x < 0 || y < 0 || x >= m_xres || y >= m_yres)
        return;
    ((int*)m_ddsd.lpSurface)[y*m_ddsd.lPitch/sizeof(int) + x] = (int)(b + (g << 8) + (r << 16));
    return;
}

/**
 * Plots a pixel with the given color at the given coordinate on the backbuffer.
 * 
 * @param x The x coordinate of the pixel to be plotted.
 * @param y The y coordinate of the pixel to be plotted.
 * @param color The color of the pixel to be plotted (0xAARRGGBB)
 */
void Gfx::Plot(int x, int y, int color)
{
    assert(m_isLocked);
    if(x < 0 || y < 0 || x >= m_xres || y >= m_yres)
        return;
    ((int*)m_ddsd.lpSurface)[y*m_ddsd.lPitch/sizeof(int) + x] = color;
    return;
}

/**
 * Locks the backbuffer to enable pixel plotting. The backbuffer must be
 * unlocked before flipping surfaces.
 * 
 * @returns True if the locking was successful.
 */
bool Gfx::Lock()
{
    assert(!m_isLocked);
    HRESULT hresult;
    memset(&m_ddsd, 0, sizeof(m_ddsd));
    m_ddsd.dwSize = sizeof(m_ddsd);
    hresult = m_pDDSBack->Lock(NULL, &m_ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
    if(hresult == DDERR_SURFACELOST)
    {
        RestoreAllSurfaces();
        hresult = m_pDDSBack->Lock(NULL, &m_ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
    }
    if(hresult != DD_OK)
        m_lastError = "Failed to lock the back surface.";
    else
        m_isLocked = true;
    return (hresult == DD_OK);
}

/**
 * Unlocks the backbuffer (see Lock()).
 * 
 * @returns True if the unlocking was successful.
 */
bool Gfx::Unlock()
{
    assert(m_isLocked);
    if(m_pDDSBack->Unlock(NULL) != DD_OK)
        return false;
    else
        m_isLocked = false;
    return true;
}

/**
 * Clears the backbuffer with the given color.
 * 
 * @param r The red component of the color.
 * @param g The green component of the color.
 * @param b The blue component of the color.
 * @returns True if the clearing was successful.
 */
bool Gfx::ClearScreen(int r, int g, int b)
{
    HRESULT hresult;
    DDBLTFX ddBltFx;

    ddBltFx.dwSize = sizeof(DDBLTFX);
    ddBltFx.dwFillColor = (DWORD) (b + (g << 8) + (r << 16));

    hresult = m_pDDSBack->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddBltFx);
    if(hresult == DDERR_SURFACELOST)
    {
        RestoreAllSurfaces();
        hresult = m_pDDSBack->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddBltFx);
    }

    return hresult == DD_OK;
}

/**
 * Switches the front and back buffers, so the contents of the back buffer become visible
 * on the screen.
 * 
 * @returns True if the flip was successful.
 */
bool Gfx::Flip()
{
    assert(!(m_isLocked && m_isWindowed));
    HRESULT hresult;
    DDBLTFX ddbltfx;
    memset(&ddbltfx, 0, sizeof(ddbltfx));
    ddbltfx.dwSize=sizeof(ddbltfx);

    if(m_isWindowed)
    {
        hresult = m_pDDSPrimary->Blt(m_pScreenRect, m_pDDSBack, NULL, DDBLTFAST_WAIT, &ddbltfx);
        if(hresult == DDERR_SURFACELOST)
        {
            RestoreAllSurfaces();
            hresult = m_pDDSPrimary->Blt(m_pScreenRect, m_pDDSBack, NULL, DDBLTFAST_WAIT, &ddbltfx);
        }
    }
    else 
    {
        m_pDDSPrimary->Blt(0, m_pDDSBack, 0, DDBLTFAST_WAIT, &ddbltfx);
        hresult = m_pDDSPrimary->Flip(NULL, DDFLIP_WAIT);
        if(hresult == DDERR_SURFACELOST)
        {
            RestoreAllSurfaces();
            hresult = m_pDDSPrimary->Flip(NULL, DDFLIP_WAIT);
        }
    }
    return hresult == DD_OK;
}

/**
 * When a window gets issue a WM_MOVE command, this function must be called in order to let the
 * graphics system be aware of the new window bounds.
 */
void Gfx::OnWindowMove() 
{
    if(m_pScreenRect)
        delete m_pScreenRect;
 
    m_pScreenRect = new RECT;
    
    POINT tempPoint;

    GetClientRect(m_hWnd, m_pScreenRect);

    tempPoint.x = m_pScreenRect->left;
    tempPoint.y = m_pScreenRect->top;

    ClientToScreen(m_hWnd, &tempPoint);

    m_pScreenRect->left = tempPoint.x;
    m_pScreenRect->top = tempPoint.y;

    tempPoint.x = m_pScreenRect->right;
    tempPoint.y = m_pScreenRect->bottom;

    ClientToScreen(m_hWnd, &tempPoint);

    m_pScreenRect->right = tempPoint.x;
    m_pScreenRect->bottom = tempPoint.y;
}


/**
 * Changes the display mode between different resolutions and fullscreen to windowed
 * 
 * @param windowed Whether the application should be windowed mode
 * @param bpp The number of bits per pixel
 * @param xres The horizontal resolution
 * @param yres The vertical resolution
 * @returns True if the resolution change was successful
 */
bool Gfx::ChangeDisplayMode(bool windowed, int bpp, int xres, int yres)
{
    m_xres = xres;
    m_yres = yres;
    m_isWindowed = windowed;
    ShowWindow(m_hWnd, SW_HIDE);
    DestroySurfaces();
    if(windowed)
    {
        ShowCursor(true);
        m_pDD->RestoreDisplayMode();
        m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL);
        SetWindowLong(m_hWnd, GWL_STYLE, WS_CAPTION | WS_VISIBLE | WS_SYSMENU);
        MoveWindow(m_hWnd, 100, 100, xres + 2 * GetSystemMetrics(SM_CXFIXEDFRAME), 
                   yres + GetSystemMetrics(SM_CYCAPTION) + 2 * GetSystemMetrics(SM_CYFIXEDFRAME), true);
        SetFocus(m_hWnd);
    }
    else
    {
        ShowCursor(false);
        SetWindowLong(m_hWnd, GWL_STYLE, WS_EX_TOPMOST);
        MoveWindow(m_hWnd, 0, 0, xres, yres, true);
        if(m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_MULTITHREADED | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) != DD_OK)
        {
            m_lastError = "SetCooperativeLevel failed in Gfx::ChangeDisplayMode";
            m_pDD->Release();
            return(false);
        }

        if(m_pDD->SetDisplayMode(xres, yres, 32, 0, 0) != DD_OK)
        {
            m_lastError = "SetDisplayMode failed in Gfx::ChangeDisplayMode";
            m_pDD->Release();
            return false;
        }
    }
    ShowWindow(m_hWnd, SW_SHOW);

    return CreateSurfaces(windowed, bpp, xres, yres);
}

/**
 * Releases all DirectDraw surfaces.
 */
void Gfx::DestroySurfaces()
{
    if(m_pDDSBack)
    {
        m_pDDSBack->Release();
        m_pDDSBack = 0;
    }
    if(m_pDDSPrimary)
    {
        if(m_isWindowed)
        {
            m_pDDSPrimary->SetClipper(NULL);
            m_pClipper->Release();
            m_pClipper = 0;
        }

        m_pDDSPrimary->Release();
        m_pDDSPrimary = 0;
    }
}

/**
 * Creates the front buffer and back buffers.
 * 
 * @param windowed Whether the application is in windowed mode
 * @param xres The horizontal resolution of the application
 * @param yres The vertical resolution of the application
 * @returns True if the surfaces were created successfully
 */
bool Gfx::CreateSurfaces(bool windowed, int, int xres, int yres)
{
    if(windowed)
    {
        memset(&m_ddsd, 0, sizeof(m_ddsd));
        m_ddsd.dwSize = sizeof(m_ddsd);
        m_ddsd.dwFlags = DDSD_CAPS;
        m_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY;

        if(m_pDD->CreateSurface(&m_ddsd, &m_pDDSPrimary, NULL) != DD_OK)
        {
            m_lastError = "Failed to create primary surface (windowed)";
            return false;
        }

        m_ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
        m_ddsd.dwWidth = xres;
        m_ddsd.dwHeight = yres;
        m_ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;  

        if(m_pDD->CreateSurface(&m_ddsd, &m_pDDSBack, NULL) != DD_OK)
        {
            m_lastError = "Failed to create backbuffer (windowed)";
            return false;
        }

        m_pDD->CreateClipper(0,&m_pClipper,0);
        m_pClipper->SetHWnd(0, m_hWnd);
        m_pDDSPrimary->SetClipper(m_pClipper);
        return true;
    }

    memset(&m_ddsd, 0, sizeof(m_ddsd));
    m_ddsd.dwSize = sizeof(m_ddsd);
    m_ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    m_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    m_ddsd.dwBackBufferCount = 1;

    if(m_pDD->CreateSurface(&m_ddsd, &m_pDDSPrimary, NULL) != DD_OK)
    {
        m_lastError = "Failed to create primary surface (fullscreen)";
        m_pDD->Release();
        return false;
    }

    m_ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

    if(m_pDDSPrimary->GetAttachedSurface(&m_ddsd.ddsCaps, &m_pDDSBack) != DD_OK)
    {
        m_lastError = "Failed to create backbuffer (fullscreen)";
        m_pDDSPrimary->Release();
        m_pDD->Release();
        return false;
    }
    return true;
}

/**
 * Restores the surfaces that where lost when other programs got control of the front buffer
 */
void Gfx::RestoreAllSurfaces()
{
    if(m_pDD)
        m_pDD->RestoreAllSurfaces();
}

/**
 * Checks if the application is running in windowed mode.
 * 
 * @returns True if the application is running windowed mode.
 */
bool Gfx::IsWindowed() const
{
    return m_isWindowed;
}
