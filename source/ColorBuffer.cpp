#include "ColorBuffer.h"

//------------------------------------------------------------------------------
// Constructor.
//------------------------------------------------------------------------------
ColorBuffer::ColorBuffer(int sizeX, int sizeY) : m_sizeX(sizeX), m_sizeY(sizeY)
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
	m_sizeX = cb.m_sizeX;
	m_sizeY = cb.m_sizeY;
	m_buffer = new Color[m_sizeX*m_sizeY];
	for(int y = 0; y < m_sizeY ; y++)
	{
		for(int x = 0; x < m_sizeX; x++)
		{
			m_buffer[y*m_sizeX + x] = cb.m_buffer[y*m_sizeX + x];
		}
	}
}

//------------------------------------------------------------------------------
// Sets a pixel in the color buffer to a specified color value.
//------------------------------------------------------------------------------
void ColorBuffer::SetPixel(int x, int y, const Color& c)
{
	assert(x >= 0 && y >= 0 && x < m_sizeX && y < m_sizeY);
	m_buffer[y*m_sizeX + x] = c;
}

//------------------------------------------------------------------------------
// Sets a pixel in the color buffer to a specified color value.
//------------------------------------------------------------------------------
void ColorBuffer::SetPixel(int x, int y, float r, float g, float b)
{
	assert(x >= 0 && y >= 0 && x < m_sizeX && y < m_sizeY);
	m_buffer[y*m_sizeX + x] = Color(r, g, b);
}

//------------------------------------------------------------------------------
// Returns the color of a location in the buffer.
//------------------------------------------------------------------------------
Color ColorBuffer::GetPixel(int x, int y) const
{
	assert(x >= 0 && y >= 0 && x < m_sizeX && y < m_sizeY);
	return m_buffer[y*m_sizeX + x];
}

//------------------------------------------------------------------------------
// Returns the width of the buffer.
//------------------------------------------------------------------------------
int ColorBuffer::GetXRes() const
{
	return m_sizeX;
}

//------------------------------------------------------------------------------
// Returns the height of the buffer.
//------------------------------------------------------------------------------
int ColorBuffer::GetYRes() const
{
	return m_sizeY;
}

//------------------------------------------------------------------------------
// Clears the buffer with the specified color.
//------------------------------------------------------------------------------
void ColorBuffer::Clear(const Color& c)
{
	for(int y = 0; y < m_sizeY ; y++)
	{
		for(int x = 0; x < m_sizeX; x++)
		{
			m_buffer[y*m_sizeX + x] = c;
		}
	}
}

//------------------------------------------------------------------------------
// Adds to a color value in the buffer.
//------------------------------------------------------------------------------
void ColorBuffer::AddColor(int x, int y, const Color& c)
{
	assert(x >= 0 && y >= 0 && x < m_sizeX && y < m_sizeY);
	m_buffer[y*m_sizeX + x] += c;
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
	bmi.bmiHeader.biWidth = m_sizeX;
	bmi.bmiHeader.biHeight = -m_sizeY;
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

	for(int b = 0; b < m_sizeY; b++)
		for(int a = 0; a < m_sizeX; a++)
			pBuffer[b*m_sizeX + a] = m_buffer[b*m_sizeX + a].GetInt(); // pBuffer can be 0 after many frames, might want to check that out (maybe forgot to delete something)

	SelectObject(hdc, hbhb);
	::SetTextColor(hdc, 0x00ffffff);
	::SetBkMode(hdc, TRANSPARENT);
	::TextOut(hdc, x, y, (LPCWSTR) text, strlen(text));

	for(int b = 0; b < m_sizeY; b++)
		for(int a = 0; a < m_sizeX; a++)
			m_buffer[b*m_sizeX + a] = pBuffer[b * m_sizeX + a];

	DeleteObject(hbhb);
	ReleaseDC(NULL, hdc);
}

void ColorBuffer::Save(Bytestream& b) const
{
    b << m_sizeX << m_sizeY;
    for(int y = 0; y < m_sizeY; y++) 
    {
        for(int x = 0; x < m_sizeX; x++) 
        {
            Color c = m_buffer[y*m_sizeX + x];
            b << c.r << c.g << c.b;
        }
    }
}

ColorBuffer::ColorBuffer(Bytestream& b)
{
    b >> m_sizeX >> m_sizeY;
    m_buffer = new Color[m_sizeX*m_sizeY];
    for(int y = 0; y < m_sizeY; y++) 
    {
        for(int x = 0; x < m_sizeX; x++) 
        {
            Color c;
            b >> c.r >> c.g >> c.b;
            m_buffer[y*m_sizeX + x] = c;
        }
    }
}