#include "TriangleMesh.h"
#include "Matrix3d.h"
#include "GeometricRoutines.h"
#include "Utils.h"
#include "IntersectionInfo.h"
#include "ObjReader.h"
#include "Ray.h"
#include "BoundingBox.h"
#include "Bytestream.h"
#include "Scene.h"
#include <numeric>
#include <unordered_map>

MeshVertex::~MeshVertex()
{
}

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
    return std::abs(((v1->pos-v0->pos)^(v2->pos-v0->pos)).Length())/2;
}

MeshTriangle::MeshTriangle(MeshVertex* _v0, MeshVertex* _v1, MeshVertex* _v2)// : mesh(t)
{
    v0 = _v0;
    v1 = _v1;
    v2 = _v2;
}

MeshTriangle::MeshTriangle()
{
}

MeshTriangle::~MeshTriangle()
{
    delete v0;
    delete v1;
    delete v2;
}

double MeshTriangle::Intersect(const Ray& ray) const
{
    auto [t, u, v] = IntersectTriangle(v0->pos, v1->pos, v2->pos, ray);
    return t;
}


bool MeshTriangle::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
    auto [t, u, v] = IntersectTriangle(v0->pos, v1->pos, v2->pos, ray);
    if(t < 0)
        return false;

    Vector3d E1 = v1->pos-v0->pos, E2 = v2->pos-v0->pos;

    info.direction = ray.direction;
    info.normal = u*(v1->normal-v0->normal) + v*(v2->normal-v0->normal) + v0->normal;

    info.geometricnormal = GetNormal();
    info.normal.Normalize();

    auto posnorm = (info.geometricnormal*info.direction < 0 ? info.geometricnormal*eps : -info.geometricnormal*eps);
    info.position = v0->pos + u*E1 + v*E2 + posnorm;
    info.texpos.x = u;
    info.texpos.y = v;
    info.material = material;

    return true;
}

TriangleMesh::TriangleMesh(const std::string& file, Material* mat)
{
    auto [success, mesh, meshLights] = ReadFromFile(file, mat);
    if(!success)
        __debugbreak();
    *this = *mesh;
}

TriangleMesh::TriangleMesh()
{
}

BoundingBox MeshTriangle::GetBoundingBox() const
{
    Vector3d c1, c2;
    c1.x = min(v0->pos.x, v1->pos.x, v2->pos.x);
    c1.y = min(v0->pos.y, v1->pos.y, v2->pos.y);
    c1.z = min(v0->pos.z, v1->pos.z, v2->pos.z);

    c2.x = max(v0->pos.x, v1->pos.x, v2->pos.x);
    c2.y = max(v0->pos.y, v1->pos.y, v2->pos.y);
    c2.z = max(v0->pos.z, v1->pos.z, v2->pos.z);
    return BoundingBox(c1, c2);
}

TriangleMesh::~TriangleMesh()
{
}

void TriangleMesh::CalculateVertexNormals()
{
    for(auto& p : points)
    {
        auto& tris = static_cast<MeshVertex*>(p)->triangles;
        p->normal = std::accumulate(tris.begin(), tris.end(), Vector3d(0, 0, 0),
                [] (auto a, auto t) { return a + t->GetNormal(); }).Normalized();
    }
}

std::tuple<bool, BoundingBox> MeshTriangle::GetClippedBoundingBox(const BoundingBox& clipbox) const
{
    std::vector<Vector3d> points = { v0->pos, v1->pos, v2->pos };

    ClipPolygonToAAP(0, true, clipbox.c1.x, points); // Left side of the bounding box
    ClipPolygonToAAP(0, false, clipbox.c2.x, points); // Right
    ClipPolygonToAAP(1, true, clipbox.c1.y, points); // Bottom
    ClipPolygonToAAP(1, false, clipbox.c2.y, points); // Top
    ClipPolygonToAAP(2, true, clipbox.c1.z, points); // Front
    ClipPolygonToAAP(2, false, clipbox.c2.z, points); // Back

    BoundingBox resultbox{ { inf, inf, inf }, { -inf, -inf, -inf } };

    for(auto v : points)
    {
        resultbox.c1.x = min(v.x, resultbox.c1.x);
        resultbox.c2.x = max(v.x, resultbox.c2.x);
        resultbox.c1.y = min(v.y, resultbox.c1.y);
        resultbox.c2.y = max(v.y, resultbox.c2.y);
        resultbox.c1.z = min(v.z, resultbox.c1.z);
        resultbox.c2.z = max(v.z, resultbox.c2.z);
    }

    if(points.size() > 2)
        return { true, resultbox };
    else
        return { false, resultbox };
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
    std::unordered_map<Vertex3d*, unsigned int> vertexMemToIndex;
    std::unordered_map<Material*, unsigned int> materialMemToIndex;

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
        points.push_back(static_cast<MeshVertex*>(v));
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