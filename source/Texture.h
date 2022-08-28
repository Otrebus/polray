#pragma once

#include <string>
#define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#include <windows.h>

class Color;

class Texture
{
public:
    Texture(const std::wstring& file);
    Texture(int, int);
    ~Texture();
    Texture(Texture&);
    Color GetTexel(int x, int y) const;
    void SetTexel(int x, int y, int color);
    Color GetTexelBLInterp(double x, double y) const;

    int GetHeight() const;
    int GetWidth() const;
    static HINSTANCE hInstance;

protected:
    int* texture;
    int height, width;
};
