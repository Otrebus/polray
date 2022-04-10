#pragma once

#pragma once

#include "SpatialPartitioning.h"
#include "Ray.h"
#include "Primitive.h"
#include <vector>

class BrutePartitioning : public SpatialPartitioning
{
public:
    void Build(std::vector<const Primitive*>);
    bool Intersect(const Ray&, float tmax) const;
    float Intersect(const Ray&, const Primitive*&) const;
    float Intersect(const Ray& ray, const Primitive* &primitive, float tmin, float tmax) const;

protected:
    std::vector<const Primitive*> primitives;
};
