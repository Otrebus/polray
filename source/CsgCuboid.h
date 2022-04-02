#ifndef CSGCUBOID_H
#define CSGCUBOID_H

#include "CsgObject.h"
#include "Matrix3d.h"
#include <memory>

class CsgCuboid : public CsgObject
{
public:
    CsgCuboid(const Vector3d& position, const Vector3d& x, const Vector3d& y, 
              float a, float b, float c);

    bool Intersect(const Ray& ray, std::vector<CsgHit>& intersects) const;

    virtual BoundingBox GetBoundingBox() const;
    virtual bool GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const;

    virtual float Intersect(const Ray& ray) const;
	virtual bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    void SetMaterial(Material* material);
    Material* GetMaterial() const;

    void Translate(const Vector3d& direction);
    void Rotate(const Vector3d& axis, float angle);

    void AddToScene(Scene& scene);
    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    virtual std::unique_ptr<CsgObject> Clone();
private:
    void Precalculate();
    bool SlabsTest(const Ray& inRay, float& tNear, float& tFar, int& axisNear, int& axisFar, int& sideNear, int& sideFar) const;

    Vector3d pos_, x_, y_, z_;
    float a_, b_, c_;

    Vector3d invMatU_, invMatV_, invMatW_;
};

#endif