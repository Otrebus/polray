/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file CsgObject.h
 * 
 * Declaration of the CsgObject base class.
 */

#pragma once

#include "IntersectionInfo.h"
#include "Model.h"
#include "Primitive.h"
#include "Ray.h"

#include <vector>
#include <memory>

class CsgObject;

class CsgHit
{
public:
    enum HitType { Enter, Exit };
    IntersectionInfo info;
    double t;
    HitType type;
    const CsgObject* object;
};

class CsgObject : public Model, public Primitive
{
public:
    typedef std::vector<CsgHit> hits;
    virtual ~CsgObject() {}

    virtual hits AllIntersects(const Ray& ray) const = 0;

    virtual BoundingBox GetBoundingBox() const = 0;
    virtual std::tuple<bool, BoundingBox> GetClippedBoundingBox(const BoundingBox& clipbox) const = 0;

    virtual double Intersect(const Ray& ray) const = 0;
    virtual bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const = 0;

    virtual void Translate(const Vector3d& direction) = 0;
    virtual void Rotate(const Vector3d& axis, double angle) = 0;

    virtual std::unique_ptr<CsgObject> Clone() = 0;
protected:
    static Vector3d Multiply(const Vector3d& u, const Vector3d& v, 
                             const Vector3d& w, const Vector3d& x);
};
