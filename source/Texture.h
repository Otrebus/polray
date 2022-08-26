#pragma once

#include <string>
#include "ddraw.h"
#include "main.h"
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include "color.h"

using namespace std;

class Texture
{
public:
    Texture(const wstring& file);
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
