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
    float Intersect(const Ray& ray, const Primitive* &primitive, float tmin, float tmax, bool returnPrimitive) const;

protected:
    std::vector<const Primitive*> primitives;
};
