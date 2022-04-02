/*#ifndef SHAPE_H
#define SHAPE_H

#include "material.h"
#include "Vector3d.h"
#include "Ray.h"
#include "intersectioninfo.h"
#include "boundingbox.h"
#include "GeometricRoutines.h"

#define TYPE_MESHTRIANGLE 4

class Shape
{
public:
	Shape();
	virtual ~Shape();
	virtual BoundingBox GetBoundingBox() = 0;
	virtual float Intersect(const Ray& ray) = 0;
	virtual bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) = 0;
	virtual int GetType() = 0;
	virtual void ComputeTangentSpaceVectors() = 0;
	virtual bool GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) = 0;
	virtual void AddToScene(Scene& scene) = 0;
	static const int type_triangle = 0;
	static const int type_sphere = 1;
	static const int type_trianglemesh = 2;
	static const int type_meshtriangle = 3;

	Material* material;

	// ugly temporary hack thingy until nlogn kd tree builder is proper
	int side;
};

#endif*/