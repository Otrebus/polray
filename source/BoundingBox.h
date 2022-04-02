#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "vector3d.h"
#include "ray.h"
#include <limits>
#include <algorithm>

using namespace std;

class BoundingBox
{
public:
	BoundingBox(const Vector3d& x, const Vector3d& y);
	BoundingBox();
	float GetArea() const;
	float GetVolume() const;
	BoundingBox& operator=(const BoundingBox&);
	~BoundingBox();
	bool ContainsPoint(const Vector3d& v);
	bool Intersect(const Ray& ray, float& tnear, float& tfar) const;
	bool Intersects(const Ray& r);
	Vector3d c1, c2;
};

#endif