/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file SpatialPartitioning.cpp
 * 
 * Declaration of the SpatialPartitioning base class used to speed up ray intersection.
 */

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
