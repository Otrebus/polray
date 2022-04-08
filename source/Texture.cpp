#include "Texture.h"

HINSTANCE Texture::hInstance = 0;

Texture::Texture(int width, int height) : m_width(width), m_height(height)
{
    m_texture = new int[m_width*m_height];
    m_fHeight = (float)height;
    m_fWidth = (float)width;
}

Texture::Texture(const wstring& file)
{
    HDC hSrcDC;
    HDC hDestDC;
    HBITMAP hbitmap;      
    BITMAP bmp;           
    LPDIRECTDRAWSURFACE7    surface;
    DDSURFACEDESC2          ddsd;	

    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;  

    m_texture = 0;

    // first load the bitmap resource

    if ((hbitmap = (HBITMAP)LoadImage(hInstance, file.c_str(),
                                    IMAGE_BITMAP, 0, 0,
                                    LR_LOADFROMFILE)) == NULL)
    {
        logger.Box("Could not open file.");
    }

    
  // create a DC for the bitmap to use
  if ((hSrcDC = CreateCompatibleDC(NULL)) == NULL)
  {
      logger.Box("Could not create a device context.");
  }

  // select the bitmap into the DC
  if (SelectObject(hSrcDC, hbitmap) == NULL)
  {
    logger.Box("Could not select the bitmap into the DC.");
    DeleteDC(hSrcDC);
  }

  // get image dimensions
  if (GetObject(hbitmap, sizeof(BITMAP), &bmp) == 0)
  {
      logger.Box("Could not get image dimensions.");
    DeleteDC(hSrcDC);
  }

  ddsd.dwWidth = bmp.bmWidth;
  ddsd.dwHeight = bmp.bmHeight;

    if(gfx->m_pDD->CreateSurface(&ddsd, &surface, NULL) != DD_OK)
       logger.File("Failed to create texture");
     
  // retrieve surface DC
  if (FAILED(surface->GetDC(&hDestDC)))
  {
      logger.Box("Could not retrieve surface DC.");
    DeleteDC(hSrcDC);
  }

  // copy image from one DC to the other
  if (BitBlt(hDestDC, 0, 0, bmp.bmWidth, bmp.bmHeight, hSrcDC, 0, 0,
             SRCCOPY) == NULL)
  {
      logger.Box("Could not copy image between device contexts.");
    surface->ReleaseDC(hDestDC);
    DeleteDC(hSrcDC);
  }

  // kill the device contexts
  surface->ReleaseDC(hDestDC);
  DeleteDC(hSrcDC);

     

    surface->Lock(0, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, 0);
    
    // Now after all that hassle, copy the contents of the ddraw surface to our own buffer.
    // Some day I might change this to copy directly from the BITMAP object when I learn how it works better.

    m_texture = new int[ddsd.dwWidth*ddsd.dwHeight];
    m_height = bmp.bmHeight;
    m_width = bmp.bmWidth;
    m_fHeight = (float)m_height;
    m_fWidth = (float)m_width;

    for(int x = 0; x < m_width; x++)
    {
        for(int y = 0; y < m_height; y++)
        {
            m_texture[x+y*m_width] = ((int*)ddsd.lpSurface)[x + y*ddsd.lPitch/sizeof(int)];
        }
    }

    surface->Unlock(NULL);
    surface->Release();
}

Texture::Texture(Texture& source)
{
    m_width = source.m_width;
    m_height = source.m_height;
    m_fWidth = source.m_fWidth;
    m_fHeight = source.m_fHeight;

    m_texture = new int[m_width*m_height];

    for(int x = 0; x < m_width; x++)
    {
        for(int y = 0; y < m_width; y++)
        {
            m_texture[x + y*m_width] = source.m_texture[x + y*m_width];
        }
    }
}

Color Texture::GetTexel(int x, int y) const
{
    if(x < 0 || x >= m_width || y < 0 || y >= m_height)
        return Color(1, 0, 1);
    else
        return m_texture[x + m_width*y];
}

void Texture::SetTexel(int x, int y, int color)
{
    assert(!(x < 0 || x >= m_width || y < 0 || y >= m_height));
    m_texture[x + m_width*y] = color;
}

Color Texture::GetTexelBLInterp(float fx, float fy) const
{
    int x = (int) fx;
    int y = (int) fy;
    if(x < 0 || x > m_fWidth - 1 || y < 0 || y > m_fHeight - 1)
    {
        return Color(1, 0, 1);
    }
    float dx = x - floorf(fx);
    float dy = y - floorf(fy);
    float dxu = 1 - dx;
    float dyu = 1 - dy;
    return (GetTexel(x, y)*dxu + GetTexel(x + 1, y)*dx)*dyu + (GetTexel(x, y+1)*dxu + GetTexel(x+1,y+1)*dx)*dy;
}
    

Texture::~Texture()
{
    if(m_texture)
        delete[] m_texture;
}

int Texture::GetWidth() const
{
    return m_width;
}

int Texture::GetHeight() const
{
    return m_height;
}