#pragma once

#include <vector>

class Ray;
class Primitive;

class SpatialPartitioning
{
public:
    virtual void Build(const std::vector<const Primitive*>&) = 0;
    virtual std::tuple<double, const Primitive*> Intersect(const Ray& ray, double tmin, double tmax, bool returnPrimitive) const = 0;
};
