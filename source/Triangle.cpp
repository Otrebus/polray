#include "Triangle.h"
#include "Scene.h"
#include "Material.h"
#include "Utils.h"

/**
 * Constructor, creates a triangle from three vectors
 * 
 * @param a The first vector.
 * @param b The second vector.
 * @param c The third vector.
 */
Triangle::Triangle(const Vector3d& a, const Vector3d& b, const Vector3d& c) : v0(a), v1(b), v2(c)
{
    Vector3d normal = (b-a)^(c-a);
    normal.Normalize();
    v0.normal = v1.normal = v2.normal = normal;
}

/**
 * Constructor, creates a mesh triangle from three vertices
 */
Triangle::Triangle()
{
}

/**
 * Constructor, creates a triangle from three vertices
 * 
 * @param a The first vector.
 * @param b The second vector.
 * @param c The third vector.
 */
Triangle::Triangle(const Vertex3d& a, const Vertex3d& b, const Vertex3d& c) : v0(a), v1(b), v2(c)
{
}

/**
 * Destructor.
 */
Triangle::~Triangle()
{
}

/**
 * Intersects the MeshTriangle with a ray.
 * 
 * @param ray The ray that hit the triangle.
 * @returns The parametric distance along the ray that the triangle was hit, or -inf if it
 * wasn't hit.
 */
double Triangle::Intersect(const Ray& ray) const
{
    auto [t, u, v] = IntersectTriangle(v0.pos, v1.pos, v2.pos, ray);
    return t;
}

/**
 * Generates information about the intersection of a ray hitting a triangle.
 * 
 * @param clipbox The bounding box that we remove part of the triangle from.
 * @returns A pair indicating whether anything remains of the triangle, and the bounding box of
 *          the clipped resulting mesh triangle.
 */
bool Triangle::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
    auto [t, u, v] = IntersectTriangle(v0.pos, v1.pos, v2.pos, ray);
    if(t < 0)
        return false;
    
    info.direction = ray.direction;

    Vector3d E1 = v1.pos-v0.pos;
    Vector3d E2 = v2.pos-v0.pos;
    
    info.normal = u*(v1.normal-v0.normal) + v*(v2.normal-v0.normal) + v0.normal;
    info.normal.Normalize();

    info.geometricnormal = E1^E2;
    info.geometricnormal.Normalize();

    info.position = v0.pos + u*E1 + v*E2 + (info.geometricnormal*info.direction < 0 ? info.geometricnormal*eps : -info.geometricnormal*eps);
    info.material = material;

    return true;
}

/**
 * Returns the bounding box of the mesh triangle.
 */
BoundingBox Triangle::GetBoundingBox() const
{
    BoundingBox b;
    for(int i = 0; i < 3; i++)
    {
        b.c1[i] = min(v0.pos[i], v1.pos[i], v2.pos[i]);
        b.c2[i] = max(v0.pos[i], v1.pos[i], v2.pos[i]);
    }
    return b;
}

/**
 * Returns the bounding box of a triangle where the triangle has been clipped (in the set
 * difference sense) by the given bounding box, assuming that the triangle is only clipped in
 * such a way that a convex shape results.
 * 
 * @param clipbox The bounding box that we remove part of the triangle from.
 * @returns A pair indicating whether anything remains of the triangle, and the bounding box of
 *          the clipped resulting triangle.
 */
std::tuple<bool, BoundingBox> Triangle::GetClippedBoundingBox(const BoundingBox& clipbox) const
{
    std::vector<Vector3d> points = { v0.pos, v1.pos, v2.pos };

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
 * Adds the triangle to a scene.
 * 
 * @param scene The scene to add to.
 */
void Triangle::AddToScene(Scene& scene)
{
    Scene::PrimitiveAdder::AddPrimitive(scene, this);
    Scene::MaterialAdder::AddMaterial(scene, material);
}

/**
 * Serializes the triangle to a bytestream.
 * 
 * @param stream The bytestream to save to.
 */
void Triangle::Save(Bytestream& stream) const
{
    stream << ID_TRIANGLE << v0 << v1 << v2;
    material->Save(stream);
}

/**
 * Deserializes the triangle from a bytestream.
 * 
 * @param stream The bytestream to load from.
 */
void Triangle::Load(Bytestream& stream)
{
    unsigned char matId;
    stream >> v0 >> v1 >> v2;
    stream >> matId;
    material = Material::Create(matId);
    material->Load(stream);
}