#define NOMINMAX
#include "ColorBuffer.h"
#include "Bytestream.h"
#include "Utils.h"
#include <fstream>
#pragma warning(disable:4996)

/**
 * Constructor.
 * 
 * @param sizeX The horizontal resolution of the buffer.
 * @param sizey The vertical resolution of the buffer.
 */
ColorBuffer::ColorBuffer(int sizeX, int sizeY) : width(sizeX), height(sizeY)
{
    m_buffer = new Color[sizeX*sizeY];
}

/**
 * Constructor. Creates a color buffer filled with a color.
 * 
 * @param sizeX The horizontal resolution of the buffer.
 * @param sizey The vertical resolution of the buffer.
 * @param c The color to fill the color buffer with.
 */
ColorBuffer::ColorBuffer(int sizeX, int sizeY, Color c) : width(sizeX), height(sizeY)
{
    m_buffer = new Color[sizeX*sizeY];
    Clear(c);
}

/**
 * Constructor. Creates the color buffer from a bytestream.
 * 
 * @param b The bytestream to deserialize.
 */
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

/**
 * Destructor.
 */
ColorBuffer::~ColorBuffer()
{
    delete[] m_buffer;
}

/**
 * Copy constructor.
 * 
 * @param cb The color buffer to copy.
 */
ColorBuffer::ColorBuffer(const ColorBuffer& cb)
{
    width = cb.width;
    height = cb.height;
    m_buffer = new Color[width*height];
    for(int y = 0; y < height ; y++)
        for(int x = 0; x < width; x++)
            m_buffer[y*width + x] = cb.m_buffer[y*width + x];
}

/**
 * Writes a screenshot to the file with the given name.
 * 
 * @param filename The filename to serialize into.
 */
void ColorBuffer::Dump(std::string filename)
{
    std::ofstream file;
    file.open(filename, std::ios::binary);

    auto pad = (4 - width*3%4)%4; // Each line needs to be padded to a multiple of 4 wide
    auto size = (width*3 + pad)*height;

    // Write the .bmp-specific stuff here
    file.write("BM", 2);
    int header[] = { size + 54, 0, 54, 40, width, height, 1572865, 0, size, 0, 0, 0, 0 };
    for(auto x : header)
        for(int i = 0; i < 4; i++)
            file.put(0xFF &(x >> (i*8)));

    // Output the screenshot, bottom-up
    for(int y = height - 1; y >= 0; y--)
    {
        for(int x = 0; x < width; x++)
        {
            auto color = GetPixel(x, y);
            for(auto c : { color.b, color.g, color.r })
                file.put((char) max(0, min(255, (int)(255*c))));
        }
        for(int n = 0; n < pad; n++)
            file.put(0);
    }
}

/**
 * Sets a pixel to a certain color.
 * 
 * @param x The x coordinate of the pixel.
 * @param y The y coordinate of the pixel.
 * @param c The color to set the pixel to.
 */
void ColorBuffer::SetPixel(int x, int y, const Color& c)
{
    assert(x >= 0 && y >= 0 && x < width && y < height);
    m_buffer[y*width + x] = c;
}

/**
 * Sets a pixel to a certain color.
 * 
 * @param x The x coordinate of the pixel.
 * @param y The y coordinate of the pixel.
 * @param r The red component of the color to set the pixel to.
 * @param g The green component of the color to set the pixel to.
 * @param b The blue component of the color to set the pixel to.
 */
void ColorBuffer::SetPixel(int x, int y, double r, double g, double b)
{
    assert(x >= 0 && y >= 0 && x < width && y < height);
    m_buffer[y*width + x] = Color(r, g, b);
}

/**
 * Sets a pixel to a certain color.
 * 
 * @param x The x coordinate of the pixel.
 * @param y The y coordinate of the pixel.
 * @returns The color of the pixel
 */
Color ColorBuffer::GetPixel(int x, int y) const
{
    assert(x >= 0 && y >= 0 && x < width && y < height);
    return m_buffer[y*width + x];
}

/**
 * Returns the horizontal resolution of the buffer.
 *
 * @returns The width of the buffer, in pixels
 */
int ColorBuffer::GetXRes() const
{
    return width;
}

/**
 * Returns the vertical resolution of the buffer.
 *
 * @returns The height of the buffer, in pixels
 */
int ColorBuffer::GetYRes() const
{
    return height;
}

/**
 * Clears the buffer with the specified color.
 *
 * @param c The color to use to clear the buffer.
 */
void ColorBuffer::Clear(const Color& c)
{
    for(int y = 0; y < height ; y++)
        for(int x = 0; x < width; x++)
            m_buffer[y*width + x] = c;
}

/**
 * Add a color to a pixel in the buffer.
 *
 * @param x The x coordinate of the pixel.
 * @param y The y coordinate of the pixel.
 * @param c The color to use to clear the buffer.
 */
void ColorBuffer::AddColor(int x, int y, const Color& c)
{
    assert(x >= 0 && y >= 0 && x < width && y < height);
    m_buffer[y*width + x] += c;
}

/**
 * Serializes the color buffer to a bytestream.
 *
 * @param b The bytestream to stream to.
 */
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
