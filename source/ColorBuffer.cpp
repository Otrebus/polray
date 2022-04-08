#define NOMINMAX
#include "ColorBuffer.h"
#include <fstream>
#pragma warning(disable:4996)

//------------------------------------------------------------------------------
// Constructor.
//------------------------------------------------------------------------------
ColorBuffer::ColorBuffer(int sizeX, int sizeY) : width(sizeX), height(sizeY)
{
    m_buffer = new Color[sizeX*sizeY];
}

//------------------------------------------------------------------------------
// Destructor.
//------------------------------------------------------------------------------
ColorBuffer::~ColorBuffer()
{
    delete[] m_buffer;
}

//------------------------------------------------------------------------------
// Copy constructor.
//------------------------------------------------------------------------------
ColorBuffer::ColorBuffer(const ColorBuffer& cb)
{
    width = cb.width;
    height = cb.height;
    m_buffer = new Color[width*height];
    for(int y = 0; y < height ; y++)
        for(int x = 0; x < width; x++)
            m_buffer[y*width + x] = cb.m_buffer[y*width + x];
}

//------------------------------------------------------------------------------
// Writes a screenshot to the file with the given name.
//------------------------------------------------------------------------------
void ColorBuffer::Dump(std::string filename) {
    std::ofstream file;
    file.open(filename, std::ios::binary);

    auto pad = (4 - ((width*3)%4))%4; // Each line needs to be padded to a multiple of 4 wide
    auto size = ((width*3) + pad)*height;

    // Write the .bmp-specific stuff here
    file.write("BM", 2);
    int header[] = { size + 54, 0, 54, 40, width, height, 1572865, 0, size, 0, 0, 0, 0 };
    for(auto x : header)
        for(int i = 0; i < 4; i++)
            file.put((0xFF) &(x >> (i*8)));

    // Output the screenshot, bottom-up
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            auto color = GetPixel(x, y);
            for(auto c : { color.b, color.g, color.r })
                file.put(std::max(0, std::min(255, (int)(255*c))));
        }
        for (int n = 0; n < pad; n++)
            file.put(0);
    }
}

//------------------------------------------------------------------------------
// Sets a pixel in the color buffer to a specified color value.
//------------------------------------------------------------------------------
void ColorBuffer::SetPixel(int x, int y, const Color& c)
{
    assert(x >= 0 && y >= 0 && x < width && y < height);
    m_buffer[y*width + x] = c;
}

//------------------------------------------------------------------------------
// Sets a pixel in the color buffer to a specified color value.
//------------------------------------------------------------------------------
void ColorBuffer::SetPixel(int x, int y, float r, float g, float b)
{
    assert(x >= 0 && y >= 0 && x < width && y < height);
    m_buffer[y*width + x] = Color(r, g, b);
}

//------------------------------------------------------------------------------
// Returns the color of a location in the buffer.
//------------------------------------------------------------------------------
Color ColorBuffer::GetPixel(int x, int y) const
{
    assert(x >= 0 && y >= 0 && x < width && y < height);
    return m_buffer[y*width + x];
}

//------------------------------------------------------------------------------
// Returns the width of the buffer.
//------------------------------------------------------------------------------
int ColorBuffer::GetXRes() const
{
    return width;
}

//------------------------------------------------------------------------------
// Returns the height of the buffer.
//------------------------------------------------------------------------------
int ColorBuffer::GetYRes() const
{
    return height;
}

//------------------------------------------------------------------------------
// Clears the buffer with the specified color.
//------------------------------------------------------------------------------
void ColorBuffer::Clear(const Color& c)
{
    for(int y = 0; y < height ; y++)
    {
        for(int x = 0; x < width; x++)
        {
            m_buffer[y*width + x] = c;
        }
    }
}

//------------------------------------------------------------------------------
// Adds to a color value in the buffer.
//------------------------------------------------------------------------------
void ColorBuffer::AddColor(int x, int y, const Color& c)
{
    assert(x >= 0 && y >= 0 && x < width && y < height);
    m_buffer[y*width + x] += c;
}

//------------------------------------------------------------------------------
// Draws text onto the buffer - this function is relatively slow and should only
// be used on a buffer with normalized colors.
//------------------------------------------------------------------------------
void ColorBuffer::PutText(const char* const text, int x, int y)
{
    HDC hdc = CreateCompatibleDC(NULL);

    BITMAPINFO bmi;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB; 
    bmi.bmiHeader.biSizeImage = 0;
    bmi.bmiHeader.biXPelsPerMeter = 0;
    bmi.bmiHeader.biYPelsPerMeter = 0;
    bmi.bmiHeader.biClrUsed = 0;
    bmi.bmiHeader.biClrImportant = 0;

    int* pBuffer;
    HBITMAP hbhb = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pBuffer, NULL, 0);

    for(int b = 0; b < height; b++)
        for(int a = 0; a < width; a++)
            pBuffer[b*width + a] = m_buffer[b*width + a].GetInt(); // pBuffer can be 0 after many frames, might want to check that out (maybe forgot to delete something)

    SelectObject(hdc, hbhb);
    ::SetTextColor(hdc, 0x00ffffff);
    ::SetBkMode(hdc, TRANSPARENT);
    ::TextOut(hdc, x, y, (LPCWSTR) text, strlen(text));

    for(int b = 0; b < height; b++)
        for(int a = 0; a < width; a++)
            m_buffer[b*width + a] = pBuffer[b * width + a];

    DeleteObject(hbhb);
    ReleaseDC(NULL, hdc);
}

void ColorBuffer::Save(Bytestream& b) const
{
    b << width << height;
    for(int y = 0; y < height; y++) 
    {
        for(int x = 0; x < width; x++) 
        {
            Color c = m_buffer[y*width + x];
            b << c.r << c.g << c.b;
        }
    }
}

ColorBuffer::ColorBuffer(Bytestream& b)
{
    b >> width >> height;
    m_buffer = new Color[width*height];
    for(int y = 0; y < height; y++) 
    {
        for(int x = 0; x < width; x++) 
        {
            Color c;
            b >> c.r >> c.g >> c.b;
            m_buffer[y*width + x] = c;
        }
    }
}