#pragma once

#include "SpatialPartitioning.h"
#include <vector>

class Ray;
class Primitive;

class BrutePartitioning : public SpatialPartitioning
{
public:
    void Build(const std::vector<const Primitive*>&);
    double Intersect(const Ray& ray, const Primitive* &primitive, double tmin, double tmax, bool returnPrimitive) const;

protected:
    std::vector<const Primitive*> primitives;
};
