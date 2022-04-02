#ifndef CSGOBJECT_H
#define CSGOBJECT_H

#include "IntersectionInfo.h"
#include "Model.h"
#include "Primitive.h"
#include "Ray.h"

#include <vector>
#include <memory>

class CsgHit
{
public:
    enum HitType { Enter, Exit };
    IntersectionInfo info;
    float t;
    HitType type;
};

class CsgObject : public Model, public Primitive
{
public:
    typedef std::vector<CsgHit> hits;
    virtual ~CsgObject() {}

    virtual bool Intersect(const Ray& ray, hits& intersects) const = 0;

    virtual BoundingBox GetBoundingBox() const = 0;
    virtual bool GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const = 0;

    virtual float Intersect(const Ray& ray) const = 0;
	virtual bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const = 0;

    virtual void Translate(const Vector3d& direction) = 0;
    virtual void Rotate(const Vector3d& axis, float angle) = 0;

    virtual std::unique_ptr<CsgObject> Clone() = 0;
protected:
    static Vector3d Multiply(const Vector3d& u, const Vector3d& v, 
                             const Vector3d& w, const Vector3d& x);
};

#endif