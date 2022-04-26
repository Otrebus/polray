#pragma once

#include "Ray.h"
#include "Primitive.h"
#include <vector>

class SpatialPartitioning
{
public:
    virtual void Build(std::vector<const Primitive*>) = 0;
    virtual double Intersect(const Ray& ray, const Primitive* &primitive, double tmin, double tmax, bool returnPrimitive=true) const = 0;
};
