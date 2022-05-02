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

//#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#define new DEBUG_NEW

#define EXP 1.0 / 2.4
        #define aGamma 0.055
        #define bGamma (1.0 + aGamma)

bool g_isActive;
bool g_quitting;
Gfx* gfx;
Input* input;
Logger logger(LOG_FILENAME);
LPDIRECTDRAWSURFACE7 bitmapsurface;
DDSURFACEDESC2 ddsd;
//ColorBuffer* colorbuffer;
HANDLE bufferMutex;
HANDLE workingMutex;
int nCurrentFrame;
Rendering* rendering;
int frames;

void radixsort(double* fnumbers, double* fsorted, int n)
{
    unsigned int* numbers = (unsigned int*)fnumbers;
    unsigned int* sorted = (unsigned int*)fsorted;
    unsigned const int sHist = 1 << 11;

    unsigned int hR[sHist];
    unsigned int hM[sHist];
    unsigned int hL[sHist];

    memset(hR, 0, sHist*4);
    memset(hM, 0, sHist*4);
    memset(hL, 0, sHist*4);

    const int bitmaskR = 0x7FF;
    const int bitmaskM = bitmaskR << 11;

    // First, create histograms and prepare the doubles for sorting
    for(int i = 0; i < n; i++)
    {
        numbers[i] = numbers[i] ^ (-int(numbers[i] >> 31) | 0x80000000);

        hR[bitmaskR & numbers[i]]++;
        hM[(bitmaskM & numbers[i]) >> 11]++;
        hL[numbers[i] >> 22]++;
    }

    int sumR = 0, sumL = 0, sumM = 0, tmp;
    // Create the offset table with the help of the histogram info
    for(int i = 0; i < sHist; i++)
    {
        tmp = hL[i];
        hL[i] = sumL;
        sumL += tmp;

        tmp = hM[i];
        hM[i] = sumM;
        sumM += tmp;

        tmp = hR[i];
        hR[i] = sumR;
        sumR += tmp;
    }

    for(int i = 0; i < n; i++)
        sorted[hR[bitmaskR & numbers[i]]++] = numbers[i];
    for(int i = 0; i < n; i++)
        numbers[hM[(bitmaskM & sorted[i]) >> 11]++] = sorted[i];
    for(int i=0;i<n;i++)
        sorted[hL[numbers[i] >> 22]++] = numbers[i] ^ (((numbers[i] >> 31) - 1) | 0x80000000);
}

/*
double GetRadixSortTime(int n)
{
    Random r;
    SAHEvent** events;

        events = new SAHEvent*[n];

        for(int i = 0; i < n; i++)
        {
            events[i] = new SAHEvent(0, r.Getdouble(-10, 10), 0);
        }

    KDTree blah;
    int b[3] = {n, n, n};
    double t = timer->GetTime();
    blah.SortEvents(events, n);
    return timer->GetTime() - t;
}*/

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

    /*
    Scene* pScene;

    // Conference camera
    
    Vector3d camerapos(28.3f, 3.0f, 5.8f);
    Vector3d cameradir = Vector3d(7.6f, 24.9f, 1.5f) - camerapos;
    Vector3d cameraup(0, 0, 1.0f);
    
    //PathTracer* confrenderer = new PathTracer();
    //confrenderer->SetSPP(1000);
    Renderer* confrenderer = new RayTracer();
    Vector3d confcamerapos(28.3f, 3.0f, 5.8f);
    Scene conference("conference.obj");
    conference.SetRenderer(confrenderer);
    conference.camera = new Camera(Vector3d(0, 0, 1.0f), Vector3d(28.3f, 3.0f, 5.8f), Vector3d(7.6f, 24.9f, 1.5f) - confcamerapos);
    //PointLight* conferencelight = new PointLight(Vector3d(19.0f, 11.0f, 8.0f), Color(1000.0f, 1000.0f, 1000.0f), &conference);
    AreaLight* conferencelight = new AreaLight(Vector3d(14.0f, 11.0f, 8.5f), Vector3d(0.0f, 3.0f, 0.0f), Vector3d(3.0f, 0.0f, 0.0f), Color(300, 300, 300), &conference);
    conferencelight->AddToScene(conference);
    timer1 = timer->GetTime();
    conference.Build();
    timer1 = timer->GetTime() - timer1;
    pScene = &conference;

    exit(1);
    */

    /*int n = 1000000;
    for(int i = 0; i < 300; i++)
    {
        if(GetRadixSortTime(n) < GetSortTime(n))
            n/=2;
        else
            n*=1.5;
    }*/

/*	stringstream sstr33;
    std::string str33 = sstr33.str();
    sstr33 << "Cutoff: " << n << "\n";
    logger.Box(sstr33.str());
    //MessageBox(hWnd, sstr33.str().c_str(), "0rb0", 0);*/

    //_CrtSetDbgFlag( 
  // 0
//);

    //_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

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

    KDTree::cost_triint = 0;
    KDTree::cost_boxint = 0;

    /*for(int i = 0; i < 10; i++)
    {
        KDTree::cost_triint += KDTree::CalculateCost(Shape::type_meshtriangle, 100000);
        KDTree::cost_boxint += KDTree::CalculateCost(5, 100000);
    }

    KDTree::cost_triint/=10;
    KDTree::cost_boxint/=10;*/
    KDTree::cost_triint = 0.0005f;
    KDTree::cost_boxint = 0.0005f;

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
        //bb.SaveToFile("btstrout");
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
