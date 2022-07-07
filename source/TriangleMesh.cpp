#include "TriangleMesh.h"
#include <stack>
#include <map>
#include "PhongMaterial.h"
#include "EmissiveMaterial.h"
#include "PhongMaterial.h"
#include "LambertianMaterial.h"
#include "MirrorMaterial.h"
#include "DielectricMaterial.h"
#include "AshikhminShirley.h"
#include "Scene.h"
#include <filesystem>
#include "Utils.h"
#include "ObjReader.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

MeshTriangle::MeshTriangle(const Vector3d& a, const Vector3d& b, const Vector3d& c)
{
	v0 = new MeshVertex(a);
	v1 = new MeshVertex(b);
	v2 = new MeshVertex(c);
	v0->normal = v1->normal = v2->normal = (v1->pos-v0->pos)^(v2->pos-v0->pos);
}

Vector3d MeshTriangle::GetNormal() const
{
	Vector3d normal = (v1->pos-v0->pos)^(v2->pos-v0->pos);
	normal.Normalize();
	return normal;
}

double MeshTriangle::GetArea()
{
	return fabsf(((v1->pos-v0->pos)^(v2->pos-v0->pos)).GetLength())/2;
}

MeshVertex::MeshVertex(double a, double b, double c)
{
	pos.x = a;
	pos.y = b;
	pos.z = c;
}

MeshVertex::MeshVertex(const Vector3d& vec)
{
	pos.x = vec.x;
	pos.y = vec.y;
	pos.z = vec.z;
}


MeshTriangle::MeshTriangle(MeshVertex* _v0, MeshVertex* _v1, MeshVertex* _v2)// : mesh(t)
{
	v0 = _v0;
	v1 = _v1;
	v2 = _v2;
}
/*
MeshTriangle::MeshTriangle(TriangleMesh* t, MeshVertex* _v0, MeshVertex* _v1, MeshVertex* _v2, Material* mat)// : mesh(t)
{
//		id = highestid++;
	v0 = _v0;
	v1 = _v1;
	v2 = _v2;
	material = mat;
}*/

MeshTriangle::MeshTriangle()
{
//		id = highestid++;
}

MeshTriangle::~MeshTriangle()
{
}
/*
MeshTriangle::MeshTriangle(Material* mat)
{
	material = mat;
}*/

double MeshTriangle::Intersect(const Ray& ray) const
{
	double u, v, t;
	Vector3d D;
	D.x = ray.direction.x;
	D.y = ray.direction.y;
	D.z = ray.direction.z;

	Vector3d E1 = v1->pos-v0->pos;
	Vector3d E2 = v2->pos-v0->pos;
	Vector3d T = ray.origin - v0->pos;

	Vector3d P = E2^T;
	Vector3d Q = E1^D;

	double det = E2*Q;
	if(!det)
		return -inf;
	u = D*P/det;

	if(u > 1 || u < 0)
		return -inf;

	v = T*Q/det;

	if(u+v > 1 || u < 0 || v < 0)
		return -inf;

	t = E1*P/det;

	return t <= 0 ? -inf : t;
}


bool MeshTriangle::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
	double u, v, t;
	Vector3d D;

	info.direction = ray.direction;

	D.x = ray.direction.x;
	D.y = ray.direction.y;
	D.z = ray.direction.z;

	Vector3d E1 = v1->pos-v0->pos;
	Vector3d E2 = v2->pos-v0->pos;
	Vector3d T = ray.origin - v0->pos;

	Vector3d P = E2^T;
	Vector3d Q = E1^D;

	double det = E2*Q;
	if(!det) // Ray in (almost) the same plane as the triangle
		return false;

	u = D*P/det;

	if(u > 1 || u < 0)
		return false;

	v = T*Q/det;

	if(u+v > 1 || u < 0 || v < 0)
		return false;

	t = E1*P/det;
	if(t <= 0)
		return false;

/*	if(material && material->normalmap)
	{
		Vector2d tex1(u * (v1->texpos.x - v0->texpos.x),
			          u * (v1->texpos.y - v0->texpos.y));
		Vector2d tex2(v * (v2->texpos.x - v0->texpos.x),
			          v * (v2->texpos.y - v0->texpos.y));

		Vector2d txc = Vector2d(v0->texpos.x, v0->texpos.y) + tex1+tex2;
		// If there's a problem with normal mapping not looking right, try normalizing this vector.
		Color nc = material->normalmap->GetTexelBLInterp(txc.x, txc.y);
		double nx = (nc.r - 0.5f)*2.0f;
		double ny = (nc.g - 0.5f)*2.0f;
		double nz = nc.b;

		info.normal = (*tangent)*nx + (*binormal)*ny + (*normal)*nz;
	}*/
	//else
	{
		Vector3d n0 = v0->normal;
		Vector3d n1 = v1->normal;
		Vector3d n2 = v2->normal;

		info.normal = u*(v1->normal-v0->normal) + v*(v2->normal-v0->normal) + v0->normal;
	}

	info.geometricnormal = GetNormal();
	info.normal.Normalize();

	info.position = v0->pos + u*E1 + v*E2 + (info.geometricnormal*info.direction < 0 ? info.geometricnormal*eps : -info.geometricnormal*eps);
	info.texpos.x = u;
	info.texpos.y = v;
	info.material = material;

	return true;
}

TriangleMesh::TriangleMesh(string file, Material* mat)
{
	//logger.File("Reading from file");
	//ReadFromFile(file, mat);
	auto [success, mesh, meshLights] = ReadFromFile(file, mat);
	*this = mesh;
}

TriangleMesh::TriangleMesh()
{
}
/*
int TriangleMesh::GetType()
{
	return type_trianglemesh;
}

void MeshTriangle::ComputeTangentSpaceVectors()
{
}*/

BoundingBox MeshTriangle::GetBoundingBox() const
{
//	return bbox;
	Vector3d c1, c2;
	c1.x = min(v0->pos.x, min(v1->pos.x, v2->pos.x));
	c1.y = min(v0->pos.y, min(v1->pos.y, v2->pos.y));
	c1.z = min(v0->pos.z, min(v1->pos.z, v2->pos.z));

	c2.x = max(v0->pos.x, max(v1->pos.x, v2->pos.x));
	c2.y = max(v0->pos.y, max(v1->pos.y, v2->pos.y));
	c2.z = max(v0->pos.z, max(v1->pos.z, v2->pos.z));
	return BoundingBox(c1, c2);
}

TriangleMesh::~TriangleMesh()
{
}

void TriangleMesh::CalculateVertexNormals()
{
	Vector3d totalnormal(0,0,0);
	Vector3d normal(0, 0, 0);

	for(auto& p : points)
	{
		for(auto& t : p->triangles)
			totalnormal += t->GetNormal();
		totalnormal.Normalize();
		p->normal = totalnormal;
		totalnormal = Vector3d(0,0,0);
	}
}

bool MeshTriangle::GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const
{
	const int positive = 1;
	const int negative = -1;
	vector<Vector3d> points;
	points.push_back(v0->pos);	
	points.push_back(v1->pos);
	points.push_back(v2->pos);

	ClipPolygonToAAP(0, positive, clipbox.c1.x, points); // Left side of the bounding box
	ClipPolygonToAAP(0, negative, clipbox.c2.x, points); // Right
	ClipPolygonToAAP(1, positive, clipbox.c1.y, points); // Bottom
	ClipPolygonToAAP(1, negative, clipbox.c2.y, points); // Top
	ClipPolygonToAAP(2, positive, clipbox.c1.z, points); // Front
	ClipPolygonToAAP(2, negative, clipbox.c2.z, points); // Back

	resultbox.c1.x = numeric_limits<double>::infinity();
	resultbox.c2.x = -numeric_limits<double>::infinity();
	resultbox.c1.y = numeric_limits<double>::infinity();
	resultbox.c2.y = -numeric_limits<double>::infinity();
	resultbox.c1.z = numeric_limits<double>::infinity();
	resultbox.c2.z = -numeric_limits<double>::infinity();

	for(auto v : points)
	{
		resultbox.c1.x = v.x < resultbox.c1.x ? v.x : resultbox.c1.x;
		resultbox.c2.x = v.x > resultbox.c2.x ? v.x : resultbox.c2.x;
		resultbox.c1.y = v.y < resultbox.c1.y ? v.y : resultbox.c1.y;
		resultbox.c2.y = v.y > resultbox.c2.y ? v.y : resultbox.c2.y;
		resultbox.c1.z = v.z < resultbox.c1.z ? v.z : resultbox.c1.z;
		resultbox.c2.z = v.z > resultbox.c2.z ? v.z : resultbox.c2.z;
	}

	if(points.size() > 2)
		return true;
	else
		return false;
}

void TriangleMesh::AddToScene(Scene& scene)
{
	for(auto& t : triangles)
		Scene::PrimitiveAdder::AddPrimitive(scene, t);
}


void TriangleMesh::Transform(const Matrix3d& m)
{
    // This is the transpose of the inverse used to calculate the transforms of the normals
    Matrix3d nm(
        m(1,1)*m(2,2)-m(1,2)*m(2,1),m(1,2)*m(2,0)-m(1,0)*m(2,2),m(1,0)*m(2,1)-m(1,1)*m(2,0),0,
        m(0,2)*m(2,1)-m(0,1)*m(2,2),m(0,0)*m(2,2)-m(0,2)*m(2,0),m(0,1)*m(2,0)-m(0,0)*m(2,1),0,
        m(0,1)*m(1,2)-m(0,2)*m(1,1),m(0,2)*m(1,0)-m(0,0)*m(1,2),m(0,0)*m(1,1)-m(0,1)*m(1,0),0,
        0,0,0,1);

    int i = 0;
    for(auto it = points.begin(); it < points.end(); it++)
	{
        i++;
        Vertex3d* v = *it;
        v->pos = m*v->pos;
        v->normal = nm*v->normal;
        v->normal.Normalize();
    }
}

void TriangleMesh::Save(Bytestream& stream) const
{
    // This could also be a hash table, for very large triangle meshes
    map<Vertex3d*, unsigned int> vertexMemToIndex;
    map<Material*, unsigned int> materialMemToIndex;

    stream << (unsigned char)ID_TRIANGLEMESH; 
    stream << materials.size();
    stream << points.size();
    stream << triangles.size();

    for(unsigned int i = 0; i < materials.size(); i++)
    {
        materials[i]->Save(stream);
        materialMemToIndex[materials[i]] = i;
    }
    for(unsigned int i = 0; i < points.size(); i++)
    {
        Vertex3d* v = points[i];
        stream << v->pos.x << v->pos.y << v->pos.z
               << v->normal.x << v->normal.y << v->normal.z
               << v->texpos.x << v->texpos.y;
        vertexMemToIndex[v] = i;
    }
    for(auto it = triangles.begin(); it < triangles.end(); it++)
    {
        unsigned int a, b, c;
        MeshTriangle* v = *it;

        stream << vertexMemToIndex[v->v0] 
               << vertexMemToIndex[v->v1] << vertexMemToIndex[v->v2];
        stream << materialMemToIndex[v->GetMaterial()];
    }
}

void TriangleMesh::Load(Bytestream& stream)
{
    size_t nMats, nPoints, nTriangles;
    stream >> nMats >> nPoints >> nTriangles;

    materials.clear(); points.clear(); triangles.clear();

    for(unsigned int i = 0; i < nMats; i++)
    {
        unsigned char id;
        Material* mat;
        stream >> id;
        mat = Material::Create(id);
        mat->Load(stream);
        materials.push_back(mat);
    }
    for(unsigned int i = 0; i < nPoints; i++)
    {
        Vertex3d* v = new Vertex3d;
        stream >> v->pos.x >> v->pos.y >> v->pos.z 
               >> v->normal.x >> v->normal.y >> v->normal.z 
               >> v->texpos.x >> v->texpos.y;
        points.push_back((MeshVertex*)v);
    }
    for(unsigned int i = 0; i < nTriangles; i++)
    {
        MeshTriangle* v = new MeshTriangle;
        unsigned int n1, n2, n3, m;
        stream >> n1 >> n2 >> n3;
        v->v0 = points[n1]; v->v1 = points[n2]; v->v2 = points[n3];
        stream >> m;
        v->SetMaterial(materials[m]);
        triangles.push_back(v);
    }
}