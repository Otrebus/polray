#pragma once

#include "SpatialPartitioning.h"
#include <vector>

class Ray;
class Primitive;

class BrutePartitioning : public SpatialPartitioning
{
public:
    void Build(const std::vector<const Primitive*>&);
    std::tuple<double, const Primitive*> Intersect(const Ray& ray, double tmin, double tmax, bool returnPrimitive) const;

protected:
    std::vector<const Primitive*> primitives;
};
