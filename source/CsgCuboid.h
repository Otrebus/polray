/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file CsgCuboid.h
 * 
 * Declaration of the CsgCuboid class.
 */

#pragma once

#include "CsgObject.h"
#include "Matrix3d.h"
#include <memory>

class CsgCuboid : public CsgObject
{
public:
    CsgCuboid(const Vector3d& position, const Vector3d& x, const Vector3d& y, 
              double a, double b, double c);

    std::vector<CsgHit> AllIntersects(const Ray& ray) const;

    virtual BoundingBox GetBoundingBox() const;
    virtual std::tuple<bool, BoundingBox> GetClippedBoundingBox(const BoundingBox& clipbox) const;

    virtual double Intersect(const Ray& ray) const;
    virtual bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    void Translate(const Vector3d& direction);
    void Rotate(const Vector3d& axis, double angle);

    void AddToScene(Scene& scene);
    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    virtual std::unique_ptr<CsgObject> Clone();
private:
    void Precalculate();
    bool SlabsTest(const Ray& inRay, double& tNear, double& tFar, int& axisNear, int& axisFar, int& sideNear, int& sideFar) const;

    Vector3d pos_, x_, y_, z_;
    double a_, b_, c_;

    Vector3d invMatU_, invMatV_, invMatW_;
};
