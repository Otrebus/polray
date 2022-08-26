#pragma once

#include "SpatialPartitioning.h"
#include "Ray.h"
#include "Primitive.h"
#include <vector>

class BrutePartitioning : public SpatialPartitioning
{
public:
    void Build(std::vector<const Primitive*>);
    double Intersect(const Ray& ray, const Primitive* &primitive, double tmin, double tmax, bool returnPrimitive) const;

protected:
    std::vector<const Primitive*> primitives;
};
