#include "Sphere.h"
#include "Material.h"
#include "BoundingBox.h"
#include "Scene.h"
#include "Utils.h"

/**
 * Constructor.
 * 
 * @param position The position of the sphere.
 * @param radius The radius of the sphere.
 */
Sphere::Sphere(const Vector3d& position, double radius) : position(position), radius(radius)
{
}

/**
 * Constructor, allows to specify an orientation of the sphere for texture mapping purposes.
 * 
 * @param pos The position of the spehre.
 * @param up The up vectof of the sphere.
 * @param right The right vector of the sphere.
 * @param radius The radius of the sphere.
 */
Sphere::Sphere(const Vector3d& pos, const Vector3d& up, const Vector3d& right, double radius) : position(pos), up(up), right(right), radius(radius)
{
    this->up.Normalize();
    this->right = (this->up^right)^this->up;
    this->right.Normalize();
}

/**
 * Constructor.
 */
Sphere::Sphere() : radius(1), position(0, 0, 0)
{
    radius = 1.0;
    position = Vector3d(0.0, 0.0, 0.0);
}

/**
 * Destructor.
 */
Sphere::~Sphere()
{
}

/**
 * Intersects the sphere with a ray.
 * 
 * @param ray The ray to test agains sphere.
 * @returns True if the sphere was hit.
 */
double Sphere::Intersect(const Ray& ray) const
{
    return IntersectSphere(position, radius, ray);
}

/**
 * Generates information about the intersection of a ray hitting a sphere.
 * 
 * @param ray The ray that hit the sphere.
 * @param info The intersection info to fill.
 * @returns True if the sphere was actually hit.
 */
bool Sphere::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
    double t = IntersectSphere(position, radius, ray);
    if(t < 0)
        return false;

    info.direction = ray.direction;
    info.material = material;

    info.normal = (ray.origin + ray.direction*t) - position;
    info.normal.Normalize();
    info.position = ray.origin + ray.direction*t + info.normal*1e-6;

    // Texture coordinates - there are probably better methods than this one
    Vector3d v = info.position - position;
    Vector3d w = (up^v)^up;
    Vector3d forward = up^right;
    v.Normalize();
    w.Normalize();
    forward.Normalize();

    double vcoord = std::acos(up*v) / pi;
    double ucoord;

    // Clamp the coordinates to prevent NaNs, which is one of the reasons this method is inferior
    double wright = w*right > 1 ? 1 : w*right < -1 ? -1 : w*right;

    if(w*forward >= 0)
        ucoord = acos(wright) / (2*pi);
    else
        ucoord = 1.0 - acos(wright) / (2*pi);
        
    info.texpos.x = ucoord;
    info.texpos.y = vcoord;

    info.geometricnormal = info.normal;
    return true;
}

/**
 * Returns the bounding box of the sphere.
 * 
 * @returns The bounding box of the sphere.
 */
BoundingBox Sphere::GetBoundingBox() const
{
    Vector3d c1 = Vector3d(position.x-radius, position.y-radius, position.z-radius);
    Vector3d c2 = Vector3d(position.x+radius, position.y+radius, position.z+radius);
    return BoundingBox(c1, c2);
}

/**
 * Returns the bounding box of a sphere where part of the sphere has been removed by the given
 * bounding box, assuming that the triangle is only clipped in such a way that a convex shape
 * results.
 * 
 * @param clipbox The bounding box that we remove part of the sphere from.
 * @returns A pair indicating whether anything remains of the sphere, and the bounding box of
 *          the clipped resulting sphere.
 */
std::tuple<bool, BoundingBox> Sphere::GetClippedBoundingBox(const BoundingBox&) const
{
    // Just return the unclipped box for now
    return { true, GetBoundingBox() };
}

/**
 * Adds the sphere to a scene.
 * 
 * @param scene The scene to add the sphere to.
 */
void Sphere::AddToScene(Scene& scene)
{
    Scene::PrimitiveAdder::AddPrimitive(scene, this);
    Scene::MaterialAdder::AddMaterial(scene, material);
}

/**
 * Serializes the sphere to a bytestream.
 * 
 * @param stream The bytestream to save to.
 */
void Sphere::Save(Bytestream& stream) const
{
    stream << (unsigned char)ID_SPHERE << position << radius;
    material->Save(stream);
}

/**
 * Deserializes the sphere from a bytestream.
 * 
 * @param stream The bytestream to laod from.
 */
void Sphere::Load(Bytestream& stream)
{
    unsigned char matId;
    stream >> position >> radius;
    stream >> matId;
    material = Material::Create(matId);
    material->Load(stream);
}