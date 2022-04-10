#pragma once

#include "Ray.h"
#include "Primitive.h"
#include <vector>

class SpatialPartitioning
{
public:
    virtual void Build(std::vector<const Primitive*>) = 0;
    virtual bool Intersect(const Ray&, float tmax) const = 0;
    virtual float Intersect(const Ray&, const Primitive*&) const = 0;
    virtual float Intersect(const Ray& ray, const Primitive* &primitive, float tmin, float tmax) const = 0;
};
