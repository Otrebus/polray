#ifndef COLORBUFFER_H
#define COLORBUFFER_H

#include "Bytestream.h"
#include "Color.h"
#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#include <string>

class ColorBuffer
{
public:
    ColorBuffer(int sizeX, int sizeY);
    ColorBuffer(const ColorBuffer& cb);
    ColorBuffer(Bytestream& b);
    ~ColorBuffer();

    Color GetPixel(int x, int y) const;

    void SetPixel(int x, int y, const Color& color);
    void SetPixel(int x, int y, float r, float g, float b);
    void AddColor(int x, int y, const Color& c);
    void Clear(const Color& c);
    void PutText(const char* const text, int x, int y);

    void Dump(std::string filename);

    int GetXRes() const;
    int GetYRes() const;

    void Save(Bytestream& b) const;
private:
    Color* m_buffer;
    int width, height;
};

#endif