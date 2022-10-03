#pragma once

#include "Vector3d.h"

class Vertex3d
{
public:
    Vertex3d(const Vector3d& pos, const Vector3d& norm, const Vector2d& tex);
    Vertex3d(const Vector3d&);
    Vertex3d();
    virtual ~Vertex3d();

    Vector3d pos, normal;
    Vector2d texpos;
};
