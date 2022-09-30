#pragma once

#include "CsgObject.h"
#include "Matrix3d.h"

class CsgSphere : public CsgObject
{
public:
    CsgSphere(const Vector3d& position, double radius);

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
    Vector3d pos_;
    double radius_;
};
