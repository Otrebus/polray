#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "Primitive.h"
#include "vertex3d.h"
#include "KDTree.h"
#include "Matrix3d.h"
#include "Model.h"

class MeshTriangle;
class TriangleMesh;

class MeshVertex : public Vertex3d
{
public:
	MeshVertex(const Vector3d& vec);
	MeshVertex(float, float, float);
	~MeshVertex();

	vector<MeshTriangle*> triangles;
    Vertex3d* replacement;
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

    float Intersect(const Ray& ray) const;
	bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    void Transform(const Matrix3d& m);

	float GetArea();
	Vector3d GetNormal() const;

    Vertex3d* v0, *v1, *v2;
};


class TriangleMesh : public Model
{
    friend class MeshLight;
public:
	TriangleMesh();
	TriangleMesh(string, Material*);
	~TriangleMesh();
	void CalculateVertexNormals();
	void RemoveDuplicateVertices(float threshold);
	bool ReadFromFile(string, Material* mat);
	bool ReadMaterialFile(string, map<string, Material*>&);

    void AddToScene(Scene& scene);

    void Transform(const Matrix3d& m);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

protected:	
	vector<MeshTriangle*> triangles;
	vector<MeshVertex*> points;
    vector<Material*> materials;
};

#endif