#pragma once

#include "CsgObject.h"
#include "Matrix3d.h"
#include <memory>

class CsgCylinder : public CsgObject
{
public:
    CsgCylinder(Vector3d& position, Vector3d& dir, 
                double length, double radius);

    bool Intersect(const Ray& ray, hits& intersects) const;

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

    Vector3d pos_;
    Vector3d x_, y_, z_;
    double length_, radius_;

    Vector3d invMatU_, invMatV_, invMatW_;
};
