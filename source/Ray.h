#pragma once

#include "Vector3d.h"
#include "intersectioninfo.h"

class Ray
{
public:
    //Ray(Ray& r);
    Ray();
    Ray(const Vector3d&, const Vector3d&);
    Ray(const Vector3d&, const Vector3d&, IntersectionInfo);
    Ray(const Ray&);
    ~Ray();

    Vector3d origin;
    Vector3d direction;
};
