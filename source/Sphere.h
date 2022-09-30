#pragma once

#include "Vector3d.h"
#include "Model.h"
#include "Primitive.h"

class BoundingBox;
class IntersectionInfo;
class Scene;
class Ray;

class Sphere : public Primitive, public Model
{
public:
    Sphere(const Vector3d& position, double radius);
    Sphere(const Vector3d& position, const Vector3d& up, const Vector3d& right, double radius);
    Sphere();
    ~Sphere();

    std::tuple<bool, BoundingBox> GetClippedBoundingBox(const BoundingBox& clipbox) const;
    BoundingBox GetBoundingBox() const;

    double Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

protected:
    friend class Scene;
    virtual void AddToScene(Scene& scene);
    double radius;
    Vector3d position, up, right;
};
