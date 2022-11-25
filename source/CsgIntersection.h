/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file CsgIntersection.h
 * 
 * Declaration of the CsgIntersection class.
 */

#pragma once

#include "CsgObject.h"
#include <memory>
#include <memory>

class CsgIntersection : public CsgObject
{
public:
    CsgIntersection(CsgObject* a, CsgObject* b);

    hits AllIntersects(const Ray& ray) const;

    virtual BoundingBox GetBoundingBox() const;
    virtual std::tuple<bool, BoundingBox> GetClippedBoundingBox(const BoundingBox& clipbox) const;

    virtual double Intersect(const Ray& ray) const;
    virtual bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    void SetMaterial(Material* material);
    Material* GetMaterial() const;

    void Translate(const Vector3d& direction);
    void Rotate(const Vector3d& axis, double angle);

    void AddToScene(Scene& scene);
    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    virtual std::unique_ptr<CsgObject> Clone();
private:
    std::unique_ptr<CsgObject> objA_, objB_;
};
