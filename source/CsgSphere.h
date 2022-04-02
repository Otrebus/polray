#ifndef CSGSPHERE_H
#define CSGSPHERE_H

#include "CsgObject.h"
#include "Matrix3d.h"

class CsgSphere : public CsgObject
{
public:
    CsgSphere(const Vector3d& position, float radius);

    bool Intersect(const Ray& ray, hits& intersects) const;

    virtual BoundingBox GetBoundingBox() const;
    virtual bool GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const;

    virtual float Intersect(const Ray& ray) const;
	virtual bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    //void SetMaterial(Material* material);
    //Material* GetMaterial() const;

    void Translate(const Vector3d& direction);
    void Rotate(const Vector3d& axis, float angle);

    void AddToScene(Scene& scene);
    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    virtual std::unique_ptr<CsgObject> Clone();
private:
    Vector3d pos_;
    float radius_;
};

#endif