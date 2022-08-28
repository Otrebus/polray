#pragma once

class Vector3d;
#include "Texture.h"

class Cubemap
{
public:
    Cubemap(Texture, Texture, Texture, Texture, Texture, Texture);
    ~Cubemap();
    
    int GetTexel(const Vector3d&) const;
protected:
    Texture m_north, m_south, m_west, m_east, m_top, m_bottom;
};
