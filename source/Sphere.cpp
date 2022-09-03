#include "Sphere.h"
#include "Material.h"
#include "BoundingBox.h"
#include "Scene.h"
#include "Utils.h"

Sphere::Sphere(const Vector3d& v, double r) : position(v), radius(r)
{
}

Sphere::Sphere(const Vector3d& pos, const Vector3d& u, const Vector3d& ri, double r) : position(pos), up(u), right(ri), radius(r)
{
    up.Normalize();
    right = (up^right)^up;
    right.Normalize();
}

Sphere::Sphere() : radius(1), position(0, 0, 0)
{
    radius = 1.0f;
    position = Vector3d(0.0f, 0.0f, 0.0f);
}

Sphere::~Sphere()
{
}

double Sphere::Intersect(const Ray& ray) const
{
    return IntersectSphere(position, radius, ray);
}

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
        ucoord = 1.0f - acos(wright) / (2*pi);
        
    info.texpos.x = ucoord;
    info.texpos.y = vcoord;

    info.geometricnormal = info.normal;
    return true;
}

BoundingBox Sphere::GetBoundingBox() const
{
    Vector3d c1 = Vector3d(position.x-radius, position.y-radius, position.z-radius);
    Vector3d c2 = Vector3d(position.x+radius, position.y+radius, position.z+radius);
    return BoundingBox(c1, c2);
}

bool Sphere::GetClippedBoundingBox(const BoundingBox&, BoundingBox& resultbox) const
{
    // Just return the unclipped box for now
    resultbox = GetBoundingBox();
    return true;
}

void Sphere::AddToScene(Scene& scene)
{
    Scene::PrimitiveAdder::AddPrimitive(scene, this);
    Scene::MaterialAdder::AddMaterial(scene, material);
}

void Sphere::Save(Bytestream& stream) const
{
    stream << (unsigned char)ID_SPHERE << position << radius;
    material->Save(stream);
}

void Sphere::Load(Bytestream& stream)
{
    unsigned char matId;
    stream >> position >> radius;
    stream >> matId;
    material = Material::Create(matId);
    material->Load(stream);
}