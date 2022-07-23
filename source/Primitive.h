#ifndef PRIMITIVE_H
#define PRIMITIVE_H

//#include "Material.h"
//#include "Vector3d.h"
//#include "Ray.h"
//#include "Intersectioninfo.h"
//#include "Boundingbox.h"

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
    virtual bool GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const = 0;

    virtual double Intersect(const Ray& ray) const = 0;
    virtual bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const = 0;

    void SetMaterial(Material* material);
    Material* GetMaterial() const;

    // ugly temporary hack thingy until nlogn kd tree builder is proper [14 years later: HA!]
    mutable int side;

protected:
    Material* material;
};

#endif
