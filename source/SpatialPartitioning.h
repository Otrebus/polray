#pragma once

#include "Ray.h"
#include "Primitive.h"
#include <vector>

class SpatialPartitioning
{
public:
    virtual void Build(std::vector<const Primitive*>) = 0;
    virtual float Intersect(const Ray& ray, const Primitive* &primitive, float tmin, float tmax, bool returnPrimitive=true) const = 0;
};
