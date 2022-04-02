#ifndef SPHERE_H
#define SPHERE_H

#include "Vector3d.h"
#include "Ray.h"
#include "Shape.h"
#include "Model.h"
#include "Primitive.h"

class BoundingBox;
class IntersectionInfo;
class Scene;

class Sphere : public Primitive, public Model
{
public:
	Sphere(const Vector3d& position, float radius);
	Sphere(const Vector3d& position, const Vector3d& up, const Vector3d& right, float radius);
    Sphere();
	~Sphere();

	float GetRadius() const;
	void SetRadius(float radius);
	Vector3d GetPosition() const;
    Vector3d SetPosition(Vector3d& position);

	bool GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const;
	BoundingBox GetBoundingBox() const;

	float Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

	virtual void AddToScene(Scene& scene);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

protected:
	float radius;
	Vector3d position, up, right;
};

#endif