#include "Texture.h"
#include "Logger.h"
#include "ddraw.h"
#include "Gfx.h"
#include "Color.h"
#include "Main.h"

HINSTANCE Texture::hInstance = 0;

Texture::Texture(int width, int height) : width(width), height(height)
{
    texture = new int[width*height];
}

Texture::Texture(const std::wstring& file)
{
    HDC hSrcDC;
    HDC hDestDC;
    HBITMAP hbitmap;      
    BITMAP bmp;           
    LPDIRECTDRAWSURFACE7    surface;
    DDSURFACEDESC2          ddsd2;	

    memset(&ddsd2, 0, sizeof(ddsd2));
    ddsd2.dwSize = sizeof(ddsd2);
    ddsd2.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    ddsd2.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;  

    texture = 0;

    // first load the bitmap resource
    hbitmap = (HBITMAP)LoadImage(hInstance, file.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if(hbitmap == NULL)
        logger.Box("Could not open file.");

    // create a DC for the bitmap to use
    if((hSrcDC = CreateCompatibleDC(NULL)) == NULL)
        logger.Box("Could not create a device context.");

    // select the bitmap into the DC
    if(SelectObject(hSrcDC, hbitmap) == NULL)
    {
        logger.Box("Could not select the bitmap into the DC.");
        DeleteDC(hSrcDC);
    }

    // get image dimensions
    if(GetObject(hbitmap, sizeof(BITMAP), &bmp) == 0)
    {
        logger.Box("Could not get image dimensions.");
        DeleteDC(hSrcDC);
    }

    ddsd2.dwWidth = bmp.bmWidth;
    ddsd2.dwHeight = bmp.bmHeight;

    if(gfx->m_pDD->CreateSurface(&ddsd2, &surface, NULL) != DD_OK)
    logger.File("Failed to create texture");
     
    // retrieve surface DC
    if(FAILED(surface->GetDC(&hDestDC)))
    {
        logger.Box("Could not retrieve surface DC.");
        DeleteDC(hSrcDC);
    }

    // copy image from one DC to the other
    if(BitBlt(hDestDC, 0, 0, bmp.bmWidth, bmp.bmHeight, hSrcDC, 0, 0,
            SRCCOPY) == NULL)
    {
        logger.Box("Could not copy image between device contexts.");
        surface->ReleaseDC(hDestDC);
        DeleteDC(hSrcDC);
    }

    // kill the device contexts
    surface->ReleaseDC(hDestDC);
    DeleteDC(hSrcDC);

    surface->Lock(0, &ddsd2, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, 0);
    
    // Now after all that hassle, copy the contents of the ddraw surface to our own buffer.
    // Some day I might change this to copy directly from the BITMAP object when I learn how it works better.
    texture = new int[ddsd2.dwWidth*ddsd2.dwHeight];
    height = bmp.bmHeight;
    width = bmp.bmWidth;

    for(int x = 0; x < width; x++)
        for(int y = 0; y < height; y++)
            texture[x+y*width] = ((int*)ddsd2.lpSurface)[x + y*ddsd2.lPitch/sizeof(int)];

    surface->Unlock(NULL);
    surface->Release();
}

Texture::Texture(Texture& source)
{
    width = source.width;
    height = source.height;

    texture = new int[width*height];

    for(int x = 0; x < width; x++)
        for(int y = 0; y < width; y++)
            texture[x + y*width] = source.texture[x + y*width];
}

Color Texture::GetTexel(int x, int y) const
{
    if(x < 0 || x >= width || y < 0 || y >= height)
        return Color(1, 0, 1);
    else
        return Color(texture[x + width*y]);
}

void Texture::SetTexel(int x, int y, int color)
{
    assert(!(x < 0 || x >= width || y < 0 || y >= height));
    texture[x + width*y] = color;
}

Color Texture::GetTexelBLInterp(double fx, double fy) const
{
    int x = (int) fx, y = (int) fy;
    if(x < 0 || x > width - 1 || y < 0 || y > height - 1)
        return Color(1, 0, 1);

    double dx = x - std::floor(fx), dy = y - std::floor(fy);
    double dxu = 1 - dx, dyu = 1 - dy;
    return (GetTexel(x, y)*dxu + GetTexel(x + 1, y)*dx)*dyu + (GetTexel(x, y+1)*dxu + GetTexel(x+1,y+1)*dx)*dy;
}
    

Texture::~Texture()
{
    if(texture)
        delete[] texture;
}

int Texture::GetWidth() const
{
    return width;
}

int Texture::GetHeight() const
{
    return height;
}