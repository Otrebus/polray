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
        double t;
        Vector3d dir(ray.direction);
        Vector3d vec = ray.origin - position_;

        double C = vec*vec - radius_*radius_;
        double B = 2*(vec*dir);
        double A = dir*dir;

        double D = (B*B/(4*A) - C)/A;

        t = -B/(2*A) - sqrt(D);
            
        if(D > 0)
        {
            if(t < eps)
            {
                return -B/(2*A) + sqrt(D) > 0 ? t = -B/(2*A) + sqrt(D) : -inf;
            }
            return t;
        }
        return -inf;
}

bool SphereLight::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
    double t;
    Vector3d dir(ray.direction);
    Vector3d vec = ray.origin - position_;

    info.direction = ray.direction;

    info.material = 0;

    double C = vec*vec - radius_*radius_;
    double B = 2*(vec*dir);
    double A = dir*dir;

    double D = (B*B/(4*A) - C)/A;

    t = -B/(2*A) - sqrt(D);

    if(D >= 0)
    {
        if(t < eps)
        {
            t = -B/(2*A) + sqrt(D);
            if(t < 0)
                return false;
        }
        info.normal = (ray.origin + ray.direction*t) - position_;
        info.normal.Normalize();
        info.position = ray.origin + ray.direction*(t - eps);

        info.material = material;
        info.geometricnormal = info.normal;
        return true;
    }
    return false;
}

double SphereLight::Pdf(const IntersectionInfo& info, const Vector3d& out) const
{
    return abs(out*info.normal)/M_PI;
}

Color SphereLight::SampleRay(Ray& ray, Vector3d& normal, double& areaPdf, double& pdf) const
{
    SamplePoint(ray.origin, normal);
    Vector3d right, forward;
    MakeBasis(normal, right, forward);

    areaPdf = 1/GetArea();

    double r1 = r_.GetDouble(0, 2*M_PI);
    double r2 = r_.GetDouble(0, 0.9999f);
    ray.direction = forward*cos(r1)*sqrt(r2) + right*sin(r1)*sqrt(r2) 
                    + normal * sqrt(1 - r2);
    pdf = abs(ray.direction*normal)/M_PI;

    return Color::Identity*F_PI;
}

void SphereLight::SamplePoint(Vector3d& point, Vector3d& normal) const
{
    double z = r_.GetDouble(-1, 1);
    double r = sqrt(1 - z*z);
    double u = r_.GetDouble(0, 2*M_PI);
    normal = Vector3d(r*cos(u), r*sin(u), z);
    point = position_ + normal*radius_*1.0001f;
}

void SphereLight::SamplePointHemisphere(const Vector3d& apex, Vector3d& point, Vector3d& normal) const
{
    Vector3d right, forward;
    double z = r_.GetDouble(0, 1);
    double r = sqrt(1 - z*z);
    double u = r_.GetDouble(0, 2*M_PI);
    MakeBasis(apex, right, forward);
    normal = right*r*cos(u) + forward*r*sin(u) + apex*z;
    point = position_ + normal*radius_*1.0001f;
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

void SphereLight::AddToScene(std::shared_ptr<Scene> scn)
{
    Sphere* s = new Sphere(position_, Vector3d(0, 1, 0), 
                           Vector3d(0, 0, 1), radius_);
    Scene::LightAdder::AddLight(*scn, this);
    scn->AddModel(s);
    s->SetMaterial(material);
}

Color SphereLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lp, Vector3d& ln) const
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
            c = info.GetMaterial()->BRDF(info, toLight)
                *costheta*cosphi*intensity_*GetArea()/(2*d*d);
            return c;
        }
    }
    return Color(0, 0, 0);
}

Color SphereLight::NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info) const
{
    Vector3d toLight = position_ - info.GetPosition();
    Vector3d normal = info.GetNormal();
    Vector3d lightPoint, lightNormal;
    toLight.Normalize();
    SamplePointHemisphere(-toLight, lightPoint, lightNormal);
    toLight = lightPoint - info.GetPosition();
    double d = toLight.GetLength();
    toLight.Normalize();
    Ray lightRay = Ray(info.GetPosition(), toLight);

    if(toLight*lightNormal < 0)
    {
        if(renderer->TraceShadowRay(lightRay, d))
        {
            double cosphi = abs(normal*toLight);
            double costheta = abs(toLight*lightNormal);
            Color c;
            Material* mat = info.GetMaterial();			
            c = mat->BRDF(info, toLight)*costheta*cosphi*intensity_*GetArea()/(2*d*d);
            double brdfPdf = costheta*mat->PDF(info, toLight, false)/(d*d);
            double lightPdf = 2.0f/GetArea();
            return c;
        }
    }
    return Color(0, 0, 0);
}


Color SphereLight::DirectHitMIS(const Renderer* renderer, 
                                const IntersectionInfo& lastInfo, 
                                const IntersectionInfo& thisInfo) const
{
    Vector3d v = thisInfo.position - lastInfo.position;
    double d = v.GetLength();
    v.Normalize();
    double costheta = abs(v*thisInfo.normal);
    double lightPdf = 2.0f/GetArea();
    Material* mat = lastInfo.GetMaterial();
    double brdfPdf = costheta*mat->PDF(lastInfo, v, false)/(d*d);
    return intensity_/(1.0f + lightPdf*lightPdf/(brdfPdf*brdfPdf));
}