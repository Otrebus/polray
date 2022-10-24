/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file ColorBuffer.h
 * 
 * Declaration of the ColorBuffer class.
 */

#pragma once

#include "Color.h"
#define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#include <Windows.h>
#include <string>

class Bytestream;

class ColorBuffer
{
public:
    ColorBuffer(int sizeX, int sizeY);
    ColorBuffer(int sizeX, int sizeY, Color c);
    ColorBuffer(const ColorBuffer& cb);
    ColorBuffer(Bytestream& b);
    ~ColorBuffer();

    Color GetPixel(int x, int y) const;

    void SetPixel(int x, int y, const Color& color);
    void SetPixel(int x, int y, double r, double g, double b);
    void AddColor(int x, int y, const Color& c);
    void Clear(const Color& c);

    void Dump(std::string filename);

    int GetXRes() const;
    int GetYRes() const;

    void Save(Bytestream& b) const;
private:
    Color* m_buffer;
    int width, height;
};
