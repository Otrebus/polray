#pragma once

#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "Primitive.h"
#include "Model.h"
#include "Vertex3d.h"

class MeshTriangle;
class TriangleMesh;
class Matrix3d;

class MeshVertex : public Vertex3d
{
public:
	using Vertex3d::Vertex3d;
	~MeshVertex();

	std::vector<MeshTriangle*> triangles;
};

class MeshTriangle : public Primitive
{
public:
    MeshTriangle(MeshVertex* _v0, MeshVertex* _v1, MeshVertex* _v2);
	MeshTriangle(const Vector3d&, const Vector3d&, const Vector3d&);
	MeshTriangle();
	~MeshTriangle();

    bool GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const;
	BoundingBox GetBoundingBox() const;

    double Intersect(const Ray& ray) const;
	bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    void Transform(const Matrix3d& m);

	double GetArea();
	Vector3d GetNormal() const;

    Vertex3d* v0, *v1, *v2;
};


class TriangleMesh : public Model
{
    friend class MeshLight;
public:
	TriangleMesh();
	TriangleMesh(const std::string&, Material*);
	~TriangleMesh();
	void CalculateVertexNormals();

    void AddToScene(Scene& scene);

    void Transform(const Matrix3d& m);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

//protected:	
	std::vector<MeshTriangle*> triangles;
	std::vector<Vertex3d*> points;
    std::vector<Material*> materials;
};
