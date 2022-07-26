#include "Main.h"
#include "Draw.h"
#include <sstream>
#include "boundingbox.h"
#include "ColorBuffer.h"
#include <process.h>
#include <string>

#include "Rendering.h"
#include "Random.h"
#include "KDTree.h"
#include "Renderer.h"
#include "Scene.h"
#include "AreaLight.h"
#include "RayTracer.h"
#include "Bytestream.h"

bool g_isActive;
bool g_quitting;
Gfx* gfx;
Input* input;
Logger logger(LOG_FILENAME);
LPDIRECTDRAWSURFACE7 bitmapsurface;
DDSURFACEDESC2 ddsd;

HANDLE bufferMutex;

Rendering* rendering;

//-----------------------------------------------------------------------------
// Entry point.
//-----------------------------------------------------------------------------
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX  wc;
    HWND	    hWnd;
    MSG         msg;
    const int xres = XRES;
    const int yres = YRES;
    bool windowed = true;

    bufferMutex = CreateMutex(0, false, 0);

    gfx = new Gfx();
    input = new Input();

    g_isActive = true;
    g_quitting = false;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = L"Raytracer";
    wc.hIconSm       =  LoadIcon(NULL, IDI_WINLOGO);

    Texture::hInstance = hInstance;

    if (!RegisterClassEx(&wc))
        return 0;

    SetProcessDPIAware();

    hWnd = CreateWindowEx(NULL, L"Raytracer", L"Raytracer", WS_CAPTION | WS_VISIBLE | WS_SYSMENU, 
                          640, 480, xres + 2 * GetSystemMetrics(SM_CXFIXEDFRAME), 
                          yres + GetSystemMetrics(SM_CYCAPTION) + 2 * GetSystemMetrics(SM_CYFIXEDFRAME), 
                          NULL, NULL, hInstance, NULL);	
    if(!hWnd)
    {
        logger.Box("Could not create a window!");
        return 1;
    }

    SetFocus(hWnd);

    if(!input->Initialize(hInstance, hWnd))
    {
        logger.Box("Could not initialize Directinput.");
        return 1;
    }
    if(!gfx->Initialize(hWnd, windowed, xres, yres))
    {
        logger.Box("Could not initialize Directdraw.");
        return 1;
    }

    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    ddsd.dwWidth = XRES;
    ddsd.dwHeight = YRES;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY; 

    if(gfx->m_pDD->CreateSurface(&ddsd, &bitmapsurface, NULL) != DD_OK)
    {
        logger.File("Failed to create texture");
        return false;
    }

     gfx->ClearScreen(255, 255, 255);

     int blah;
     blah = sizeof(std::shared_ptr<Material>);

     std::shared_ptr<Scene> scene;
     std::shared_ptr<Renderer> renderer;
     Bytestream bb;
     bb << (int) 3;
     ifstream ifile("btstrout");
    if (ifile.good()) {
        rendering = new Rendering("btstrout");
    }
    else
    {
        MakeScene(renderer);
        rendering = new Rendering(renderer);
    }
        
     rendering->Start();

    while(!g_quitting)
    {
        if(!g_isActive && !gfx->IsWindowed())
        {
            // Sleep if alt-tabbed out from fullscreen mode
            GetMessage(&msg, 0, 0, 0);
            TranslateMessage(&msg); 
            DispatchMessage(&msg);
        }
        else
        {
            gfx->Lock();
            if(rendering->WasBufferRedrawn())
            {
                ColorBuffer colorbuffer = rendering->GetImage();
                WaitForSingleObject(bufferMutex, INFINITE);
                for(int y = 0; y < YRES; y++)
                {
                    for(int x = 0; x < XRES; x++)
                    {
                        Color color = colorbuffer.GetPixel(x, y);
                        if (color.r || color.g || color.b)
                            color.r = color.r;
                        gfx->Plot(x, y, color.GetInt());
                    }
                }
                ReleaseMutex(bufferMutex);
            }

            gfx->Unlock();
            gfx->Flip();
            while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                if(msg.message == WM_QUIT)
                    g_quitting = true;
                else if(msg.message == WM_KEYDOWN && msg.wParam == 'P')
                    rendering->GetImage().Dump("screenshot.bmp");
                else if(msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
                    g_quitting = true;
                else if(msg.message == WM_KEYDOWN && msg.wParam == VK_F5)
                {
                    if(!gfx->ChangeDisplayMode(windowed=!windowed, 32, xres, yres))
                        return 1;
                    gfx->Flip();
                }
                else if(msg.message == WM_KEYDOWN && msg.wParam == 'S')
                    rendering->SaveRendering("btstrout");
                else
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
        }
        Sleep(1);
    }

    DestroyWindow(hWnd);
}

//------------------------------------------------------------------------------
// Handles window events and notifies the graphics and input systems about them.
//------------------------------------------------------------------------------
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_ERASEBKGND:
        if(!gfx->IsWindowed())
            return 0;
        break;
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE)
            g_isActive = false;
        else 
        {
            g_isActive = true;
            input->Acquire();
        }
        break;
    case WM_MOVE:
        gfx->OnWindowMove();
        break;
    case WM_CREATE:
        break;
    case WM_CLOSE:
        g_quitting = true;
        PostQuitMessage(0);
        break;
    case WM_MOUSEMOVE:
    {
        input->UpdateCursorPos();
        break;
    }
    default:
        break;
    }
    return (DefWindowProc(hWnd, msg, wParam, lParam));
}
