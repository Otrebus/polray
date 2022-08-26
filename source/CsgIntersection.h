#pragma once

#include "CsgObject.h"
#include <memory>
#include <memory>

class CsgIntersection : public CsgObject
{
public:
    CsgIntersection(CsgObject* a, CsgObject* b);

    bool Intersect(const Ray& ray, std::vector<CsgHit>& intersects) const;

    virtual BoundingBox GetBoundingBox() const;
    virtual bool GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const;

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
