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

/**
 * Destructor.
 */
MeshVertex::~MeshVertex()
{
}

/**
 * Constructor.
 */
MeshTriangle::MeshTriangle()
{
}

/**
 * Constructor, creates a mesh triangle from three vectors
 * 
 * @param a The first vector.
 * @param b The second vector.
 * @param c The third vector.
 */
MeshTriangle::MeshTriangle(const Vector3d& a, const Vector3d& b, const Vector3d& c)
{
    v0 = new MeshVertex(a);
    v1 = new MeshVertex(b);
    v2 = new MeshVertex(c);
    v0->normal = v1->normal = v2->normal = (v1->pos-v0->pos)^(v2->pos-v0->pos);
}

/**
 * Destructor.
 */
MeshTriangle::~MeshTriangle()
{
    delete v0;
    delete v1;
    delete v2;
}

/**
 * Returns the normal of the mesh triangle.
 * 
 * @returns The normal.
 */
Vector3d MeshTriangle::GetNormal() const
{
    Vector3d normal = (v1->pos-v0->pos)^(v2->pos-v0->pos);
    normal.Normalize();
    return normal;
}

/**
 * Returns the area of the mesh triangle.
 * 
 * @returns The area of the mesh triangle.
 */
double MeshTriangle::GetArea()
{
    return std::abs(((v1->pos-v0->pos)^(v2->pos-v0->pos)).Length())/2;
}

/**
 * Constructor, creates a mesh triangle from three mesh vertices.
 * 
 * @param v0 The first mesh vertex.
 * @param v1 The second mesh vertex.
 * @param v2 The third mesh vertex.
 */
MeshTriangle::MeshTriangle(MeshVertex* v0, MeshVertex* v1, MeshVertex* v2) : v0(v0), v1(v1), v2(v2)
{
}

/**
 * Intersects the MeshTriangle with a ray.
 * 
 * @param ray The ray that hit the triangle.
 * @returns The parametric distance along the ray that the triangle was hit, or -inf if it
 * wasn't hit.
 */
double MeshTriangle::Intersect(const Ray& ray) const
{
    auto [t, u, v] = IntersectTriangle(v0->pos, v1->pos, v2->pos, ray);
    return t;
}


/**
 * Generates information about the intersection of a ray hitting a mesh triangle.
 * 
 * @param ray The ray that hit the triangle.
 * @param info The intersection info to fill.
 * @returns Whether the triangle was hit.
 */
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

/**
 * Loads the triangle mesh from a .obj file.
 * 
 * @param fileName The name of file to load.
 * @param mat The material to use for the entire mesh, optionally.
 */
TriangleMesh::TriangleMesh(const std::string& fileName, Material* mat)
{
    auto [mesh, meshLights] = ReadFromFile(fileName, mat);
    *this = *mesh;
}

/**
 * Constructor.
 */
TriangleMesh::TriangleMesh()
{
}

/**
 * Returns the bounding box of the mesh triangle.
 */
BoundingBox MeshTriangle::GetBoundingBox() const
{
    BoundingBox b;
    for(int i = 0; i < 3; i++)
    {
        b.c1[i] = min(v0->pos[i], v1->pos[i], v2->pos[i]);
        b.c2[i] = max(v0->pos[i], v1->pos[i], v2->pos[i]);
    }
    return b;
}

/**
 * Destructor.
 */
TriangleMesh::~TriangleMesh()
{
}

/**
 * Returns the bounding box of a mesh triangle where the triangle has been clipped (in the set
 * difference sense) by the given bounding box, assuming that the triangle is only clipped in
 * such a way that a convex shape results.
 * 
 * @param clipbox The bounding box that we remove part of the triangle from.
 * @returns A pair indicating whether anything remains of the triangle, and the bounding box of
 *          the clipped resulting mehs triangle.
 */
std::tuple<bool, BoundingBox> MeshTriangle::GetClippedBoundingBox(const BoundingBox& clipbox) const
{
    std::vector<Vector3d> points = { v0->pos, v1->pos, v2->pos };

    for(int i = 0; i < 3; i++)
    {
        points = ClipPolygonToAAP(i, true, clipbox.c1[i], points);
        points = ClipPolygonToAAP(i, false, clipbox.c2[i], points);
    }

    BoundingBox resultbox{ { inf, inf, inf }, { -inf, -inf, -inf } };

    for(auto v : points)
    {
        for(int i = 0; i < 3; i++)
        {
            resultbox.c1[i] = min(v[i], resultbox.c1[i]);
            resultbox.c2[i] = max(v[i], resultbox.c2[i]);
        }
    }

    if(points.size() > 2)
        return { true, resultbox };
    else
        return { false, resultbox };
}

/**
 * Adds the contents of the triangle mesh to the scene.
 * 
 * @param scene The scene to add the triangles to.
 */
void TriangleMesh::AddToScene(Scene& scene)
{
    for(auto& t : triangles)
        Scene::PrimitiveAdder::AddPrimitive(scene, t);
}

/**
 * Linearly transforms the triangle mesh according to the transformation matrix m.
 * 
 * @param m The matrix which to transform the triangle mesh with.
 */
void TriangleMesh::Transform(const Matrix3d& m)
{
    // This is the transpose of the inverse used to calculate the transforms of the normals
    Matrix3d nm(
        m(1,1)*m(2,2)-m(1,2)*m(2,1),m(1,2)*m(2,0)-m(1,0)*m(2,2),m(1,0)*m(2,1)-m(1,1)*m(2,0),0,
        m(0,2)*m(2,1)-m(0,1)*m(2,2),m(0,0)*m(2,2)-m(0,2)*m(2,0),m(0,1)*m(2,0)-m(0,0)*m(2,1),0,
        m(0,1)*m(1,2)-m(0,2)*m(1,1),m(0,2)*m(1,0)-m(0,0)*m(1,2),m(0,0)*m(1,1)-m(0,1)*m(1,0),0,
        0,0,0,1);

    for(auto& v : points)
    {
        v->pos = m*v->pos;
        v->normal = nm*v->normal;
        v->normal.Normalize();
    }
}

/**
 * Serializes the triangle mesh to a byte stream.
 * 
 * @param stream The stream to serialize to.
 */
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

/**
 * Deserializes this triangle mesh from a bytestream.
 * 
 * @param stream The bytestream to load from.
 */
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