#ifndef SPHERE_H
#define SPHERE_H

#include "Vector3d.h"
#include "Ray.h"
#include "Model.h"
#include "Primitive.h"

class BoundingBox;
class IntersectionInfo;
class Scene;

class Sphere : public Primitive, public Model
{
public:
    Sphere(const Vector3d& position, double radius);
    Sphere(const Vector3d& position, const Vector3d& up, const Vector3d& right, double radius);
    Sphere();
    ~Sphere();

    double GetRadius() const;
    void SetRadius(double radius);
    Vector3d GetPosition() const;
    Vector3d SetPosition(Vector3d& position);

    bool GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const;
    BoundingBox GetBoundingBox() const;

    double Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    virtual void AddToScene(Scene& scene);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

protected:
    double radius;
    Vector3d position, up, right;
};

#endif