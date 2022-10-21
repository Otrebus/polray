/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Primitive.h
 * 
 * Declaration of the Primitive base class.
 */

#pragma once

#include <tuple>

class Vector3d;
class Ray;
class Material;
class BoundingBox;
class IntersectionInfo;

class Primitive
{
public:
    Primitive();
    virtual ~Primitive();
    
    virtual BoundingBox GetBoundingBox() const = 0;
    virtual std::tuple<bool, BoundingBox> GetClippedBoundingBox(const BoundingBox& clipbox) const = 0;

    virtual double Intersect(const Ray& ray) const = 0;
    virtual bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const = 0;

    void SetMaterial(Material* material);
    Material* GetMaterial() const;

protected:
    Material* material;
};
