#include "Bytestream.h"
#include "EmissiveMaterial.h"
#include "GeometricRoutines.h"
#include "Scene.h"
#include "LightPortal.h"
#include "Utils.h"

double Portal::Intersect(const Ray& ray) const
{
    double u, v, t;
    Vector3d D = ray.direction;

    Vector3d E1 = v1;
    Vector3d E2 = v2;
    Vector3d T = ray.origin - pos;

    Vector3d P = E2^T;
    Vector3d Q = E1^D;

    double det = E2*Q;
    if(!det)
        return -inf;

    u = D*P/det;

    if(u > 1 || u < 0)
        return -inf;

    v = T*Q/det;

    if(v > 1 || v < 0)
        return -inf;

    t = E1*P/det;
    return t <= 0 ? -inf : t;
}

LightPortal::LightPortal()
{
}

LightPortal::~LightPortal()
{
}

void LightPortal::AddPortal(Vector3d pos, Vector3d v1, Vector3d v2) {
    portals.push_back(Portal(pos, v1, v2));
}

void LightPortal::SetLight(Light* l) {
    light = l;
}

double LightPortal::Intersect(const Ray& ray) const
{
    for(auto p : portals) {
        auto t = p.Intersect(ray);
        if(t != -inf) {
            auto s = light->Intersect(ray);
            if(s > t)
                return s;
        }
    }
    return -inf;
}

bool LightPortal::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
    return light->GenerateIntersectionInfo(ray, info);
}

double LightPortal::Pdf(const IntersectionInfo& info, const Vector3d& out) const
{
    Ray ray(info.position, out);
    for(auto p : portals) {
        auto t = p.Intersect(ray);
        if(t != -inf) {
            auto portalNormal = p.GetNormal();
            auto lightNormal = info.GetNormal();

            auto portalPoint = ray.origin + t*ray.direction;
            auto lightPoint = info.position;

            double areaSum = 0;
            for(auto p : portals)
                areaSum += p.GetArea();

            auto d = (lightPoint - portalPoint).GetLength();
            return d*d/(std::abs(portalNormal*ray.direction)*areaSum);
        }
    }
    return 0;
}

Color LightPortal::SampleRay(Ray& ray, Vector3d& normal, double& areaPdf, double& pdf) const
{
    // Pick the portal that we will sample the light through
    double areaSum = 0;
    for(auto p : portals)
        areaSum += p.GetArea();
    double a = r.GetDouble(0, areaSum), s = 0;
    Portal portal;
    for(auto p : portals) {
        s += p.GetArea()*(1+eps);
        if(a < s) {
            portal = p;
            break;
        }
    }

    // Sample the point within the portal
    Vector3d portalNormal = portal.v1^portal.v2;
    portalNormal.Normalize();
    Vector3d dir;
    double x = r.GetDouble(0, 1.f);
    double y = r.GetDouble(0, 1.f);
    auto portalPos = portal.pos + portal.v1*x + portal.v2*y + (0.0001)*portalNormal;

    // Sample the light and calculate the resulting pdfs
    Ray lightRay;
    Vector3d lightNormal;
    double lightAreaPdf, lightAnglePdf;
    auto color = light->SampleRay(lightRay, lightNormal, lightAreaPdf, lightAnglePdf);
    auto d = (portalPos-lightRay.origin).GetLength();
    ray = Ray(lightRay.origin, (portalPos-lightRay.origin).Normalized());

    // TODO: this pdf calculation might want to be different for different lights
    // TODO: we're essentially undoing the sampling of the light source here
    auto dirPdf = d*d/(std::abs(portalNormal*ray.direction)*areaSum);
    areaPdf = lightAreaPdf;
    normal = lightNormal;
    pdf = dirPdf;
    return std::abs(lightNormal*ray.direction)*Color(1, 1, 1)/dirPdf;
}

void LightPortal::SamplePoint(Vector3d& point, Vector3d& normal) const
{
    // Seems not to be used by any other light
}

void LightPortal::Save(Bytestream& s) const
{
    s << ""; // TODO: implement
}

void LightPortal::Load(Bytestream& s)
{
    // TODO: implement
}

double LightPortal::GetArea() const
{
    return light->GetArea();
}

Color LightPortal::GetIntensity() const
{
    return light->GetIntensity();
}

void LightPortal::AddToScene(std::shared_ptr<Scene> scn)
{
    light->material->light = this;
    Scene::LightAdder::AddLight(*scn, this);
}

Color LightPortal::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lp, Vector3d& ln) const
{
    Vector3d lightPoint, lightNormal;
    auto color = light->NextEventEstimation(renderer, info, lightPoint, lightNormal);
    lp = lightPoint;
    ln = lightNormal;
    Ray ray(info.position, (lightPoint-info.position).Normalized());
    double t = -inf;
    for(auto p : portals) {
        t = p.Intersect(ray);
        if(t != -inf)
            break;
    }
    return t == -inf ? Color(0, 0, 0) : color;
}

Color LightPortal::NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info) const
{
    return Color(0, 0, 0); // TODO: implement
}


Color LightPortal::DirectHitMIS(const Renderer* renderer, const IntersectionInfo& lastInfo, const IntersectionInfo& thisInfo) const
{
    return 0; // TODO: implement
}