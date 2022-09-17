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
#ifdef DETERMINISTIC
    r_.Seed(0);
#endif
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
    return std::max(0.0, (out*info.normal)/pi);
}

std::tuple<Ray, Color, Normal, AreaPdf, AnglePdf> SphereLight::SampleRay() const
{
    Ray ray;

    auto [point, normal] = SamplePoint();
    ray.origin = point;
    auto [rightNode, forward] = MakeBasis(normal);

    double r1 = r_.GetDouble(0, 1), r2 = r_.GetDouble(0, 1);
    ray.direction = SampleHemisphereCos(r1, r2, normal);
    double anglePdf = abs(ray.direction*normal)/pi;
    double areaPdf = 1/GetArea();

    return { ray, Color::Identity*pi, normal, areaPdf, anglePdf };
}

std::tuple<Point, Normal> SphereLight::SamplePoint() const
{
    auto r1 = r_.GetDouble(0, 1), r2 = r_.GetDouble(0, 1);
    auto pos = SampleSphereUniform(r1, r2);

    auto normal = pos;
    auto point = position_ + pos*radius_ + normal*eps;

    return { point, normal };
}

void SphereLight::SamplePointHemisphere(const Vector3d& apex, Vector3d& point, Vector3d& normal) const
{
    Vector3d rightNode, forward;
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
    return 4*pi*radius_*radius_;
}

void SphereLight::AddToScene(Scene* scn)
{
    Sphere* s = new Sphere(position_, Vector3d(0, 1, 0), 
                           Vector3d(0, 0, 1), radius_);
    Scene::LightAdder::AddLight(*scn, this);
    Scene::PrimitiveAdder::AddPrimitive(*scn, s);
    s->SetMaterial(material);
}

std::tuple<Color, Point> SphereLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, int component) const
{
    Vector3d lightPoint, lightNormal;
    Vector3d toLight = position_ - info.position;
    Vector3d normal = info.normal;
    toLight.Normalize();
    SamplePointHemisphere(-toLight, lightPoint, lightNormal);
    lightPoint = lightPoint + lightNormal*eps;
    toLight = lightPoint - info.position;
    double d = toLight.Length();
    toLight.Normalize();
    Ray lightRay = Ray(info.position, toLight);

    if(toLight*lightNormal < 0)
    {
        if(renderer->TraceShadowRay(lightRay, (1-1e-6)*d))
        {
            double cosphi = abs(normal*toLight);
            double costheta = abs(toLight*lightNormal);
            Color c;
            c = info.material->BRDF(info, toLight, component)*costheta*cosphi*intensity_*GetArea()/(2*d*d);
            return { c, lightPoint };
        }
    }
    return { Color(0, 0, 0), lightPoint };
}
