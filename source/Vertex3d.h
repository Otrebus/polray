#pragma once

#define NOMINMAX
#include "Vector3d.h"

class Vertex3d
{
public:
    Vertex3d(double, double, double, double, double, double, double, double);
    Vertex3d(const Vector3d& pos, const Vector3d& norm, const Vector2d& tex);
    Vertex3d(const Vector3d&);
    Vertex3d(double x, double y, double z);
    Vertex3d();
    ~Vertex3d();

    Vector3d pos, normal;
    Vector2d texpos;
};

