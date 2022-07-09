#include "EmissiveMaterial.h"
#include "Utils.h"
#include "Scene.h"
#include "UniformEnvironmentLight.h"
#include "GeometricRoutines.h"


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

void UniformEnvironmentLight::AddToScene(Scene* scn)
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
    return IntersectSphere(position, radius, ray);
}

bool UniformEnvironmentLight::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
    double t = IntersectSphere(position, radius, ray);
    if(t == -inf)
        return false;

    info.direction = ray.direction;
    info.material = material;
    info.normal = position - (ray.origin + ray.direction*t);
    info.normal.Normalize();
    info.position = ray.origin + ray.direction*t + info.normal*eps;
    info.geometricnormal = info.normal;
    return true;
}

Color UniformEnvironmentLight::SampleRay(Ray& ray, Vector3d& normal, double& areaPdf, double& pdf) const
{
    SamplePoint(ray.origin, normal);
    
    areaPdf = 1/GetArea();

    auto [h, A, right, forward, cv] = GetProjectedSceneHull(ray, normal);

    auto n = normal;
    double a = random.GetDouble(0, A);
    double aSum = 0;

    auto pp = ray.origin + normal*(cv*normal);

    for(int i = 0; i < h.size()-2; i++) {
        aSum += std::abs((h[i+1]-h[0])^(h[i+2]-h[0]))/2;
        if(aSum > a) {
            double u = sqrt(random.GetDouble(0, 1)), v = random.GetDouble(0, 1);
            auto e1 = h[i+1] - h[0], e2 = h[i+2] - h[0];

            auto p = h[0] + u*(e1 + v*(e2-e1));
            auto p3 = right*p.x + forward*p.y + pp;
            auto dir = (p3-ray.origin);
            ray.direction = dir.Normalized();
            pdf = dir.GetLengthSquared()/(ray.direction*n)/A;
            return (ray.direction*n)*Color::Identity/pdf;
        }
    }
}

double UniformEnvironmentLight::Pdf(const IntersectionInfo& info, const Vector3d& out) const
{
    auto n = info.normal;
    auto outRay = Ray(info.position, out);
    auto [h, A, right, forward, cv] = GetProjectedSceneHull(outRay, n);

    auto v = n*(cv*n);
    auto r = v.GetLength()/std::abs(out*n);
    return r*r/std::abs(out*n)/A;
}

void UniformEnvironmentLight::SamplePoint(Vector3d& point, Vector3d& normal) const
{
    auto r1 = random.GetDouble(0, 1), r2 = random.GetDouble(0, 1);
    auto pos = SampleSphereUniform(r1, r2);
    normal = -pos;
    point = position + pos*radius + normal*eps;
}

Color UniformEnvironmentLight::GetIntensity() const 
{
    return intensity;
}

void UniformEnvironmentLight::Save(Bytestream& s) const
{
    s << ID_UNIFORMENVIRONMENTLIGHT << position.x << position.y << position.z
      << radius << intensity;
}
void UniformEnvironmentLight::Load(Bytestream& s)
{
    s >> position.x >> position.y >> position.z 
      >> radius >> intensity;
    material = new EmissiveMaterial();
    material->emissivity = intensity;
    material->light = this;
}

Color UniformEnvironmentLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lp, Vector3d& ln, int component) const
{
    Vector3d lightPoint, lightNormal;
    SamplePoint(lightPoint, lightNormal);
    auto toLight = lightPoint - info.GetPosition();
    Ray lightRay = Ray(info.GetPosition(), toLight);
    double d = toLight.GetLength()*(1.-1.e-6);
    toLight.Normalize();

    if(renderer->TraceShadowRay(lightRay, d))
    {
        double cosphi = abs(info.GetNormal()*toLight);
        double costheta = abs(toLight*lightNormal);
        Color c;
        Material* mat = info.GetMaterial();
        c = mat->BRDF(info, toLight, component)*costheta*cosphi*intensity*GetArea()/(d*d);
        lp = lightPoint;
        ln = lightNormal;
        return c;
    }
    return Color(0, 0, 0);
}

std::tuple<std::vector<Vector2d>, double, Vector3d, Vector3d, Vector3d> UniformEnvironmentLight::GetProjectedSceneHull(Ray& ray, Vector3d normal) const
{
    auto bb = scene->GetBoundingBox();
    Vector3d right, forward;

    MakeBasis(normal, right, forward);

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

    auto c = (bb.c2 + bb.c1)/2;
    auto cv = c - ray.origin;
    auto n = normal;

    std::vector<Vector2d> q;
    for(int i = 0; i < 8; i++) {
        auto u = p[i] - ray.origin;
        Vector2d w = Vector2d(u * right, u * forward).Normalized();
        auto r1 = (u - n*(u*n)).GetLength();
        auto r2 = Vector2d(u * right, u * forward).GetLength();
        auto vp = n*(n*u);
        auto v = n*(cv*n);

        auto l1 = v.GetLength();
        auto l2 = u * n;

        auto rp = ((v.GetLength() / (u*n)) * (u - n*(u*n)).GetLength());

        q.push_back(rp*w);
    }

    auto h = convexHull(q);

    double A = 0;
    for(int i = 0; i < h.size(); i++)
        A += h[i].x*h[(i+1)%h.size()].y - h[i].y*h[(i+1)%h.size()].x;
    A *= 0.5;
    return { h, A, right, forward, cv };
}
