    //------------------------------------------------------------------------------
// File: Gfx.cpp
//  
// Handles graphics
//------------------------------------------------------------------------------

#include "gfx.h"


//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
Gfx::Gfx()
{
    m_isLocked = false;
    m_pDD = 0;
    m_pDDSPrimary = m_pDDSBack = 0;
    m_pClipper = 0;
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// Initializes Directdraw, sets the cooperative mode and the display mode, given 
// a window handle
//------------------------------------------------------------------------------
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

        // TODO: Replace this with OnWindowMove() and find out just wtf m_pClientRect is supposed to do

        m_pClientRect = new RECT;
        m_pScreenRect = new RECT;
        
        POINT tempPoint;

        GetClientRect(hWnd, m_pClientRect);
        GetClientRect(hWnd, m_pScreenRect);

        tempPoint.x = m_pScreenRect->left;
        tempPoint.y = m_pScreenRect->top;

        ClientToScreen(hWnd,&tempPoint);

        m_pScreenRect->left = tempPoint.x;
        m_pScreenRect->top = tempPoint.y;

        tempPoint.x = m_pScreenRect->right;
        tempPoint.y = m_pScreenRect->bottom;

        ClientToScreen(hWnd,&tempPoint);

        m_pScreenRect->right = tempPoint.x;
        m_pScreenRect->bottom = tempPoint.y;

        return CreateSurfaces(true, 32, xres, yres);
    }
  
    if(m_pDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) != DD_OK)
    {
        m_lastError = "SetCooperativeLevel failed in Gfx::Initialize";
        m_pDD->Release();
        return(false);
    }

    if( m_pDD->SetDisplayMode( xres, yres, 32, 0, 0) != DD_OK )
    {
        m_lastError = "SetDisplayMode failed in Gfx::Initialize";
        m_pDD->Release();
        return false;
    }
    return CreateSurfaces(false, 32, xres, yres);
}

//------------------------------------------------------------------------------
// Plots a pixel with the given color at the given coordinate on the backbuffer
//------------------------------------------------------------------------------
void Gfx::Plot(int x, int y, uchar r, uchar g, uchar b)
{
    assert(m_isLocked);
    if(x < 0 || y < 0 || x >= m_xres || y >= m_yres)
        return;
    ((int*)m_ddsd.lpSurface)[y*m_ddsd.lPitch/sizeof(int) + x] = (int)(b + (g << 8) + (r<< 16));
    return;
}

//------------------------------------------------------------------------------
// Plots a pixel with the given color at the given coordinate directly to the
// device context
//------------------------------------------------------------------------------
void Gfx::PutPixel(int x, int y, uchar r, uchar g, uchar b)
{
    HDC hdc = GetDC(m_hWnd);
    SetPixelV(hdc, x, y, RGB(r, g, b));
}

//------------------------------------------------------------------------------
// Outputs text
//------------------------------------------------------------------------------
bool Gfx::Text(int xout, int yout, const char *txout, int r, int g, int b) 
{ 
    HDC hdc;
    if (m_pDDSBack->GetDC(&hdc) == DD_OK)  
    {  
        SetBkMode(hdc, TRANSPARENT); 
        SetTextColor(hdc, RGB(r, g, b)); 
        TextOut(hdc, xout, yout, (LPCWSTR) txout, (int)strlen(txout)); 
        m_pDDSBack->ReleaseDC(hdc);  
        return true;
    }; 
    return false;
}


//------------------------------------------------------------------------------
// Plots a pixel with the given color at the given coordinate, directly with the
// byte representation of the color
//------------------------------------------------------------------------------
void Gfx::Plot(int x, int y, int color)
{
    assert(m_isLocked);
    if(x < 0 || y < 0 || x >= m_xres || y >= m_yres)
        return;
    ((int*)m_ddsd.lpSurface)[y*m_ddsd.lPitch/sizeof(int) + x] = color;
    return;
}

//------------------------------------------------------------------------------
// Locks the backbuffer to enable pixel plotting. The backbuffer must be
// unlocked before flipping surfaces
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// Unlocks the backbuffer (see Lock())
//------------------------------------------------------------------------------
bool Gfx::Unlock()
{
    assert(m_isLocked);
    if(m_pDDSBack->Unlock(NULL) != DD_OK)
        return false;
    else
        m_isLocked = false;
    return true;
}

//------------------------------------------------------------------------------
// Clears the backbuffer with the given color
//------------------------------------------------------------------------------
bool Gfx::ClearScreen(unsigned char r, unsigned char g, unsigned char b)
{
    HRESULT hresult;
    DDBLTFX ddBltFx;

    ddBltFx.dwSize = sizeof(DDBLTFX);
    ddBltFx.dwFillColor = (DWORD) (b + (g << 8) + (r << 16));

    hresult = m_pDDSBack->Blt( NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddBltFx );
    if(hresult == DDERR_SURFACELOST)
    {
        RestoreAllSurfaces();
        hresult = m_pDDSBack->Blt( NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddBltFx );
    }

    return (hresult==DD_OK);
}

//------------------------------------------------------------------------------
// Switches the front buffer with the back buffer, so the contents of the back
// buffer becomes visible on the screen
//------------------------------------------------------------------------------
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
    return (hresult==DD_OK);
}

//------------------------------------------------------------------------------
// When a window gets issued a WM_MOVE command, this function must be called
// in order to let the graphics system be aware of the new window bounds
//------------------------------------------------------------------------------
void Gfx::OnWindowMove() 
{
    m_pClientRect = new RECT;
    m_pScreenRect = new RECT;
    
    POINT tempPoint;

    GetClientRect(m_hWnd, m_pClientRect); 
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

//------------------------------------------------------------------------------
// Changes the display mode between different resolutions and fullscreen to
// windowed
//------------------------------------------------------------------------------
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

        if( m_pDD->SetDisplayMode(xres, yres, 32, 0, 0) != DD_OK )
        {
            m_lastError = "SetDisplayMode failed in Gfx::ChangeDisplayMode";
            m_pDD->Release();
            return false;
        }
    }
    ShowWindow(m_hWnd, SW_SHOW);

    return CreateSurfaces(windowed, bpp, xres, yres);
}

//------------------------------------------------------------------------------
// Releases all Directdraw surfaces
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// Creates the front buffer and back buffers
//------------------------------------------------------------------------------
bool Gfx::CreateSurfaces(bool windowed, int bpp, int xres, int yres)
{
    if(windowed)
    {
        memset( &m_ddsd, 0, sizeof(m_ddsd) );
        m_ddsd.dwSize = sizeof( m_ddsd );
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

    memset( &m_ddsd, 0, sizeof(m_ddsd) );
    m_ddsd.dwSize = sizeof( m_ddsd );
    m_ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    m_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    m_ddsd.dwBackBufferCount = 1;

    if(m_pDD->CreateSurface( &m_ddsd, &m_pDDSPrimary, NULL ) != DD_OK)
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

//------------------------------------------------------------------------------
// Restores the surfaces that have been lost when other programs have got
// control of the front buffer
//------------------------------------------------------------------------------
void Gfx::RestoreAllSurfaces()
{
    if(m_pDD)
        m_pDD->RestoreAllSurfaces();
}

//------------------------------------------------------------------------------
// Plots a line between two points
//------------------------------------------------------------------------------
void Gfx::Line(int fromX, int fromY, int toX, int toY, uchar r, uchar g, uchar b)
{
    int dY = abs(toY-fromY);
    int dX = abs(toX-fromX);
    bool steep = dY > dX;
    int h = 0, x = 0, ydir = 1, xdir = 1, d;
    
    if(toY < fromY)
        ydir = -1;
    if(toX < fromX)
        xdir = -1;
    if(steep)
    {
        swap(fromX, fromY);
        swap(dX, dY);
    }

    d = -dX;
    for(x = 0; x <= dX; x++)
    {
        if(steep)
            Plot(fromY+h*xdir, fromX+x*ydir, r, g, b);
        else
            Plot(fromX+x*xdir, fromY+h*ydir, r, g, b);
        d+=2*dY;
        if(d>0)
        {
            d-=2*dX;
            h++;
        }
    }
}

//------------------------------------------------------------------------------
// Draws a circle with the given radius and centerpoint. The radius is the
// amount of pixels between the center and the side of the circle, exclusive,
// plus one
//------------------------------------------------------------------------------
void Gfx::Circle(int X, int Y, int R, uchar r, uchar g, uchar b)
{
    assert(R >= 0);

    int d = -(int)R, y = R;
    for(int x = 0; x < y; x++)
    {
        if(d>=0)
            d+=-2*(--y);
        d+=2*x+1;

        Plot(X+x, Y+y, r, g, b); // Lower right
        Plot(X+y, Y+x, r, g, b); // Lower-mid right
        Plot(X+y, Y-x, r, g, b); // Upper-mid right
        Plot(X+x, Y-y, r, g, b); // Upper right
        Plot(X-x, Y+y, r, g, b); // Lower left
        Plot(X-y, Y+x, r, g, b); // Lower-mid left
        Plot(X-y, Y-x, r, g, b); // Upper-mid left
        Plot(X-x, Y-y, r, g, b); // Upper left
    }
}

//------------------------------------------------------------------------------
// Returns true if the application is running in windowed mode
//------------------------------------------------------------------------------
bool Gfx::IsWindowed() const
{
    return m_isWindowed;
}

//------------------------------------------------------------------------------
// Checks if it's possible to draw things at the moment. Will return false
// when the buffers are unable to be restored, eg. due to alt-tabbing
//------------------------------------------------------------------------------
bool Gfx::IsReady() const
{
    if(m_pDDSPrimary->IsLost())
        if(m_pDDSPrimary->Restore() != DD_OK)
            return false;
    return true;
}

int Gfx::GetYRes() const
{
    return m_yres;
}

int Gfx::GetXRes() const
{
    return m_xres;
}

string Gfx::GetLastError() const
{
    return m_lastError;
}

