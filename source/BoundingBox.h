#pragma once

#include "Vector3D.h"

class Ray;

class BoundingBox
{
public:
    BoundingBox(const Vector3d& x, const Vector3d& y);
    BoundingBox();
    ~BoundingBox();
    bool Intersect(const Ray& ray, double& tnear, double& tfar) const;
    Vector3d c1, c2;
};
