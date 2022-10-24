/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file BrutePartitioning.h
 * 
 * Declaration of the BrutePartitioning class.
 */

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
