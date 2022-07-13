#define NOMINMAX
#include "Bytestream.h"
#include "EmissiveMaterial.h"
#include "GeometricRoutines.h"
#include "Scene.h"
#include "SphereLight.h"
#include "Utils.h"

SphereLight::SphereLight()
{
}

SphereLight::SphereLight(Vector3d pos, double rad, Color str)
    : position_(pos), radius_(rad), Light(str)
{
    r_.Seed(GetTickCount() + int(this));
    material = new EmissiveMaterial();
    material->emissivity = str;
    material->light = this;
}

SphereLight::~SphereLight()
{
}

double SphereLight::Intersect(const Ray& ray) const
{
    return IntersectSphere(position_, radius_, ray);
}

bool SphereLight::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
    double t = IntersectSphere(position_, radius_, ray);
    if(t == -inf)
        return false;

    info.direction = ray.direction;
    info.material = material;
    info.normal = (ray.origin + ray.direction*t) - position_;
    info.normal.Normalize();
    info.position = ray.origin + ray.direction*t + info.normal*eps;
    info.geometricnormal = info.normal;
    return true;
}

double SphereLight::Pdf(const IntersectionInfo& info, const Vector3d& out) const
{
    return std::max(0.0, (out*info.normal)/M_PI);
}

Color SphereLight::SampleRay(Ray& ray, Vector3d& normal, double& areaPdf, double& pdf) const
{
    SamplePoint(ray.origin, normal);
    auto [right, forward] = MakeBasis(normal);

    areaPdf = 1/GetArea();

    double r1 = r_.GetDouble(0, 1), r2 = r_.GetDouble(0, 1);
    ray.direction = SampleHemisphereCos(r1, r2, normal);
    pdf = abs(ray.direction*normal)/M_PI;

    return Color::Identity*F_PI;
}

void SphereLight::SamplePoint(Vector3d& point, Vector3d& normal) const
{
    auto r1 = r_.GetDouble(0, 1), r2 = r_.GetDouble(0, 1);
    auto pos = SampleSphereUniform(r1, r2);
    normal = pos;
    point = position_ + pos*radius_ + normal*eps;
}

void SphereLight::SamplePointHemisphere(const Vector3d& apex, Vector3d& point, Vector3d& normal) const
{
    Vector3d right, forward;
    double r1 = r_.GetDouble(0, 1), r2 = r_.GetDouble(0, 1);
    Vector3d pos;
    pos = SampleHemisphereUniform(r1, r2, apex);
    normal = pos;
    point = position_ + pos*radius_ + normal*eps;
}

void SphereLight::Save(Bytestream& s) const
{
    s << ID_SPHERELIGHT << position_.x << position_.y << position_.z
      << radius_ << intensity_;
}
void SphereLight::Load(Bytestream& s)
{
    s >> position_.x >> position_.y >> position_.z 
      >> radius_ >> intensity_;
    material = new EmissiveMaterial();
    material->emissivity = intensity_;
    material->light = this;
}

double SphereLight::GetArea() const
{
    return 4*M_PI*radius_*radius_;
}

void SphereLight::AddToScene(Scene* scene)
{
    Sphere* s = new Sphere(position_, Vector3d(0, 1, 0), 
                           Vector3d(0, 0, 1), radius_);
    Scene::LightAdder::AddLight(*scene, this);
    Scene::PrimitiveAdder::AddPrimitive(*scene, s);
    s->SetMaterial(material);
}

Color SphereLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lp, Vector3d& ln, int component) const
{
    Vector3d lightPoint, lightNormal;
    Vector3d toLight = position_ - info.GetPosition();
    Vector3d normal = info.GetNormal();
    toLight.Normalize();
    SamplePointHemisphere(-toLight, lightPoint, lightNormal);
    lightPoint = lightPoint + lightNormal*eps;
    lp = lightPoint;
    ln = lightNormal;
    toLight = lightPoint - info.GetPosition();
    double d = toLight.GetLength();
    toLight.Normalize();
    Ray lightRay = Ray(info.GetPosition(), toLight);

    if(toLight*lightNormal < 0)
    {
        if(renderer->TraceShadowRay(lightRay, (1-1e-6)*d))
        {
            double cosphi = abs(normal*toLight);
            double costheta = abs(toLight*lightNormal);
            Color c;
            c = info.GetMaterial()->BRDF(info, toLight, component)
                *costheta*cosphi*intensity_*GetArea()/(2*d*d);
            return c;
        }
    }
    return Color(0, 0, 0);
}
