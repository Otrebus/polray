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

void LightPortal::AddPortal(Vector3d pos, Vector3d v1, Vector3d v2)
{
    portals.push_back(Portal(pos, v1, v2));
}

void LightPortal::SetLight(Light* l)
{
    light = l;
}

double LightPortal::Intersect(const Ray& ray) const
{
    for(auto p : portals)
    {
        auto t = p.Intersect(ray);
        if(t != -inf)
        {
            // This doesn't take into account which side of the portal the light is
            // which helps to calculate self-intersections of a light outside the scene.
            // However, if you put the portals + light inside the scene this wouldn't give the correct result
            auto s = light->Intersect(ray);
            if(s != -inf)
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
    for(auto p : portals)
    {
        auto t = p.Intersect(ray);
        if(t != -inf)
        {
            auto portalNormal = p.GetNormal();
            auto lightNormal = info.normal;

            auto portalPoint = ray.origin + t*ray.direction;
            auto lightPoint = info.position;

            double areaSum = 0;
            for(auto pt : portals)
                areaSum += pt.GetArea();

            auto d = (lightPoint - portalPoint).Length();
            return d*d/(std::abs(portalNormal*ray.direction)*areaSum);
        }
    }
    return 0;
}

std::tuple<Ray, Color, Normal, AreaPdf, AnglePdf> LightPortal::SampleRay() const
{
    // Pick the portal that we will sample the light through
    double areaSum = 0;
    for(auto p : portals)
        areaSum += p.GetArea();
    double a = r.GetDouble(0, areaSum), s = 0;
    Portal portal;
    for(auto p : portals)
    {
        s += p.GetArea()*(1+eps);
        if(a < s)
        {
            portal = p;
            break;
        }
    }

    // Sample the point within the portal
    Vector3d portalNormal = portal.v1^portal.v2;
    portalNormal.Normalize();

    double x = r.GetDouble(0, 1.f), y = r.GetDouble(0, 1.f);
    auto portalPos = portal.pos + portal.v1*x + portal.v2*y + (0.0001)*portalNormal;

    // Sample the light and calculate the resulting pdfs
    auto [lightRay, color, normal, lightAreaPdf, lightAnglePdf] = light->SampleRay();

    auto d = (portalPos-lightRay.origin).Length();
    Ray ray(lightRay.origin, (portalPos-lightRay.origin).Normalized());

    // TODO: this pdf calculation might want to be different for different lights
    // TODO: we're essentially undoing the sampling of the light source here
    auto dirPdf = d*d/(std::abs(portalNormal*ray.direction)*areaSum);

    if(normal*ray.direction < 0)
        return { ray, Color::Black, normal, 0, 0 };
    return { ray, std::abs(normal*ray.direction)*Color(1, 1, 1)/dirPdf, normal, lightAreaPdf, dirPdf };
}

std::tuple<Point, Normal> LightPortal::SamplePoint() const
{
    // Seems not to be used by any other light
    return { {}, {} };
}

void LightPortal::Save(Bytestream& stream) const
{
    stream << ID_LIGHTPORTAL;
    stream << portals.size();
    for(auto portal : portals)
        stream << portal.pos << portal.v1 << portal.v2;
    light->Save(stream);
}

void LightPortal::Load(Bytestream& stream)
{
    size_t portalSize;
    stream >> portalSize;
    for(int i = 0; i < portalSize; i++)
    {
        Portal portal;
        stream >> portal.pos >> portal.v1 >> portal.v2;
        portals.push_back(portal);
    }
    
    unsigned char id;
    stream >> id;
    light = Light::Create(id);
    light->Load(stream);
}

double LightPortal::GetArea() const
{
    return light->GetArea();
}

Color LightPortal::GetIntensity() const
{
    return light->GetIntensity();
}

void LightPortal::AddToScene(Scene* scn)
{
    light->material->light = this;
    Scene::LightAdder::AddLight(*scn, this);
    light->scene = scn;
}

std::tuple<Color, Point> LightPortal::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, int component) const
{
    auto [color, lightPoint] = light->NextEventEstimation(renderer, info, component);
    Ray ray(info.position, (lightPoint-info.position).Normalized());

    for(auto p : portals)
        if(p.Intersect(ray) >= 0)
            return { color, lightPoint } ;
    return { Color(0, 0, 0), lightPoint };
}
