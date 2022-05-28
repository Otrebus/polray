#define NOMINMAX
#include "AreaLight.h"
#include "Windows.h"
#include "EmissiveMaterial.h"
#include "Renderer.h"
#include "Triangle.h"
#include "Utils.h"
#include "Scene.h"
#include "UniformEnvironmentLight.h"


UniformEnvironmentLight::UniformEnvironmentLight()
{
    material = new EmissiveMaterial();
}

UniformEnvironmentLight::UniformEnvironmentLight(const Vector3d& position, double radius, const Color& color) : position(position), radius(radius), intensity(color)
{
    material = new EmissiveMaterial();
    intensity = color;
#ifdef DETERMINISTIC
    r.Seed(0);
#else
    random.Seed(GetTickCount() + int(this));
#endif
}

void UniformEnvironmentLight::AddToScene(std::shared_ptr<Scene> scn)
{
    material->light = this;
    material->emissivity = intensity;
    Scene::LightAdder::AddLight(*scn, this);
    scene = scn;
}

double UniformEnvironmentLight::GetArea() const
{
    return radius*radius*4*M_PI;
}

double UniformEnvironmentLight::Intersect(const Ray& ray) const
{
    double t;
    Vector3d dir(ray.direction);
    Vector3d vec = ray.origin - position;

    double C = vec*vec - radius*radius;
    double B = 2*(vec*dir);
    double A = dir*dir;

    double D = (B*B/(4*A) - C)/A;

    t = -B/(2*A) - sqrt(D);

    if(D > 0) {
        if(t < eps)
            return -B/(2*A) + sqrt(D) > 0 ? t = -B/(2*A) + sqrt(D) : -inf;
        return t;
    }
    return -inf;
}

bool UniformEnvironmentLight::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
    double t;
    Vector3d dir(ray.direction);
    Vector3d vec = ray.origin - position;

    info.direction = ray.direction;
    info.material = material;

    double C = vec*vec - radius*radius;
    double B = 2*(vec*dir);
    double A = dir*dir;

    double D = (B*B/(4*A) - C)/A;

    t = -B/(2*A) - sqrt(D);

    if(D >= 0)
    {
        if(t < eps)
        {
            t = -B/(2*A) + sqrt(D);
            if(t < eps)
                return false;
        }
        info.normal = position - (ray.origin + ray.direction*t);
        info.normal.Normalize();
        info.position = ray.origin + ray.direction*(t - eps);
        info.geometricnormal = info.normal;
        return true;
    }
    return false;
}

double UniformEnvironmentLight::Pdf(const IntersectionInfo& info, const Vector3d& out) const
{
    return std::max(0.0, (out*info.normal)/M_PI);
}

Color UniformEnvironmentLight::SampleRay(Ray& ray, Vector3d& normal, double& areaPdf, double& pdf) const
{
    SamplePoint(ray.origin, normal);
    Vector3d right, forward;
    MakeBasis(normal, right, forward);

    areaPdf = 1/GetArea();

    auto bb = scene->GetBoundingBox();

    Vector3d p[8] = {
        { bb.c2.x, bb.c1.y, bb.c2.z },
        { bb.c1.x, bb.c1.y, bb.c2.z },
        { bb.c1.x, bb.c1.y, bb.c1.z },
        { bb.c2.x, bb.c1.y, bb.c1.z },
        { bb.c2.x, bb.c2.y, bb.c2.z },
        { bb.c1.x, bb.c2.y, bb.c2.z },
        { bb.c1.x, bb.c2.y, bb.c1.z },
        { bb.c2.x, bb.c2.y, bb.c1.z }
    };

    std::vector<Vector2d> q;
    for(int i = 0; i < 8; i++) {
        auto u = p[i] - ray.origin;
        auto x = (u*right)/right.GetLengthSquared();
        auto y = (u*forward)/forward.GetLengthSquared();
        q.push_back({ x, y });
    }

    auto h = convexHull(q);



    /*std::vector<std::vector<Vector3d>> f {
        { p[0], p[3], p[2], p[1] },
        { p[2], p[6], p[5], p[1] },
        { p[1], p[5], p[4], p[0] },
        { p[0], p[4], p[7], p[3] },
        { p[4], p[5], p[6], p[7] },
        { p[3], p[7], p[6], p[2] },
    };

    std::vector<int> fis;
    for(int i = 0; i < 6; i++) {
        auto w = (f[i][1] - f[i][0]) ^ (f[i][2] - f[i][0]);
        auto c = (bb.c2 + bb.c1)/2;
        if((c - ray.origin) * w < 0) {
            fis.push_back(i);
        }
    }

    for(auto fi : fis) {
        auto face = f[fi];

    }*/

    /*double r1 = random.GetDouble(0, 2*M_PI);
    double r2 = random.GetDouble(0, 1);
    ray.direction = forward*cos(r1)*sqrt(r2) + right*sin(r1)*sqrt(r2) 
                    + normal * sqrt(1-r2);
    pdf = abs(ray.direction*normal)/M_PI;*/

    return Color::Identity*F_PI;
}

void UniformEnvironmentLight::SamplePoint(Vector3d& point, Vector3d& normal) const
{
    double z = random.GetDouble(-1, 1);
    double r = sqrt(1 - z*z);
    double u = random.GetDouble(0, 2*M_PI);
    normal = -Vector3d(r*cos(u), r*sin(u), z);
    point = position - normal*radius*(1.0f-0.001);
}

Color UniformEnvironmentLight::GetIntensity() const 
{
    return intensity;
}

void UniformEnvironmentLight::Save(Bytestream& stream) const
{
}

void UniformEnvironmentLight::Load(Bytestream& stream)
{
}

void UniformEnvironmentLight::SamplePointHemisphere(const Vector3d& apex, Vector3d& point, Vector3d& normal) const
{
    Vector3d right, forward;
    double z = random.GetDouble(0, 1);
    double r = sqrt(1 - z*z);
    double u = random.GetDouble(0, 2*M_PI);
    MakeBasis(apex, right, forward);
    normal = right*r*cos(u) + forward*r*sin(u) + apex*z;
    point = position + normal*radius*1.0001f;
}

Color UniformEnvironmentLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lp, Vector3d& ln, int component) const
{
    Vector3d lightPoint, lightNormal;
    SamplePoint(lightPoint, lightNormal);
    auto toLight = lightPoint - info.GetPosition();
    Ray lightRay = Ray(info.GetPosition(), toLight);
    double d = toLight.GetLength();
    toLight.Normalize();

    if(toLight*lightNormal < 0)
    {
        if(renderer->TraceShadowRay(lightRay, d))
        {
            double cosphi = abs(info.GetNormal()*toLight);
            double costheta = abs(toLight*lightNormal);
            Color c;
            Material* mat = info.GetMaterial();			
            c = mat->BRDF(info, toLight, component)*costheta*cosphi*intensity*GetArea()/(d*d);
            double brdfPdf = costheta*mat->PDF(info, toLight, false, component)/(d*d);
            double lightPdf = 1.0f/GetArea();
            return c;
        }
    }
    return Color(0, 0, 0);
}

Color UniformEnvironmentLight::DirectHitMIS(const Renderer* renderer, const IntersectionInfo& lastInfo, const IntersectionInfo& thisInfo, int component) const
{
    return Color(0, 0, 0);
}

Color UniformEnvironmentLight::NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info, int component) const
{
    return Color(0, 0, 0);
}
