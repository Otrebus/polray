#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Vector3d.h"
#include "Vertex3d.h"
#include "Ray.h"
#include "Primitive.h"
#include "Boundingbox.h"
#include "Model.h"

class Scene;

class Triangle : public Primitive, public Model
{
public:
    Triangle(float, float, float, float, float, float, float, float, float);
    Triangle(const Vector3d&, const Vector3d&, const Vector3d&);
    Triangle(const Vertex3d&, const Vertex3d&, const Vertex3d&);
    Triangle();
    ~Triangle();

    BoundingBox GetBoundingBox() const;
    bool GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const;
    
    float Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    void AddToScene(Scene& scene);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    Vertex3d v0, v1, v2;
};

#endif