#pragma once

#include "Vector3d.h"

class IntersectionInfo;

class Ray
{
public:
    Ray();
    Ray(const Vector3d&, const Vector3d&);
    ~Ray();

    Vector3d origin;
    Vector3d direction;
};
