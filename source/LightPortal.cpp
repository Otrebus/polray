/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file LightPortal.cpp
 * 
 * Implementation of the LightPortal class.
 */

#include "Bytestream.h"
#include "EmissiveMaterial.h"
#include "GeometricRoutines.h"
#include "Scene.h"
#include "LightPortal.h"
#include "Utils.h"

/**
 * Checks if and where the given ray intersects the light.
 * 
 * @param ray The ray to check against the light.
 * @returns The distance along the ray that the light source was hit.
 */
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

/**
 * Constructor.
 */
LightPortal::LightPortal()
{
}

/**
 * Destructor.
 */
LightPortal::~LightPortal()
{
}

/**
 * Adds a portal to the set of portals.
 * 
 * @param pos The position of the portal.
 * @param v1 One vector of the parallelogram that forms the portal.
 * @param v2 The other vector of the parallelogram that forms the portal.
 */
void LightPortal::AddPortal(Vector3d pos, Vector3d v1, Vector3d v2)
{
    portals.push_back(Portal(pos, v1, v2));
}

/**
 * Sets the light that is the source of the portal radiance.
 * 
 * @param l The light that shines through the portal.
 */
void LightPortal::SetLight(Light* l)
{
    light = l;
}

/**
 * Checks if and where the given ray intersects the light.
 * 
 * @param ray The ray to check against the light.
 * @returns The distance along the ray that the light source was hit.
 */
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

/**
 * Returns the intersection info of a ray that hit the light.
 * 
 * @param ray The ray that hit the light.
 * @param info The intersection info to fill in.
 * @returns Whether the area light was actually hit by the ray.
 */
bool LightPortal::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
    return light->GenerateIntersectionInfo(ray, info);
}

/**
 * Returns the value of the angle pdf of a ray that was sampled at the light source.
 * 
 * @param info Contains the point that we evaluate the pdf at.
 * @param out The outgoing vector at that point.
 * @returns The value of the angle pdf at the given point and outgoing vector.
 */
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

/**
 * Samples an outgoing ray from the light.
 * 
 * @param rnd The randomizer to sample with.
 * @returns A tuple of the outgoing ray, its sampled color and normal and the area and angle pdfs.
 */
std::tuple<Ray, Color, Normal, AreaPdf, AnglePdf> LightPortal::SampleRay(Randomizer& rnd) const
{
    // Pick the portal that we will sample the light through
    double areaSum = 0;
    for(auto p : portals)
        areaSum += p.GetArea();
    double a = rnd.GetDouble(0, areaSum), s = 0;
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

    double x = rnd.GetDouble(0, 1.f), y = rnd.GetDouble(0, 1.f);
    auto portalPos = portal.pos + portal.v1*x + portal.v2*y + eps*portalNormal;

    // Sample the light and calculate the resulting pdfs
    auto [lightRay, color, normal, lightAreaPdf, lightAnglePdf] = light->SampleRay(rnd);

    auto d = (portalPos-lightRay.origin).Length();
    Ray ray(lightRay.origin, (portalPos-lightRay.origin).Normalized());

    // TODO: this pdf calculation might want to be different for different lights
    // TODO: we're essentially undoing the sampling of the light source here
    auto dirPdf = d*d/(std::abs(portalNormal*ray.direction)*areaSum);

    if(normal*ray.direction < 0)
        return { ray, Color::Black, normal, 0, 0 };
    return { ray, std::abs(normal*ray.direction)*Color(1, 1, 1)/dirPdf, normal, lightAreaPdf, dirPdf };
}

/**
 * Samples a random point of the area light.
 * 
 * @param rnd The randomizer to sample with.
 * @returns A tuple of the point and its normal.
 */
std::tuple<Point, Normal> LightPortal::SamplePoint(Randomizer&) const
{
    // Seems not to be used by any other light
    return { {}, {} };
}

/**
 * Saves the light source to a stream.
 * 
 * @param stream The stream that we serialize to.
 */
void LightPortal::Save(Bytestream& stream) const
{
    stream << ID_LIGHTPORTAL << portals.size();
    for(auto portal : portals)
        stream << portal.pos << portal.v1 << portal.v2;
    light->Save(stream);
}

/**
 * Loads the light source from a stream.
 * 
 * @param stream The stream that we deserialize from.
 */
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

/**
 * Returns the area of the light.
 * 
 * @returns The area of the light.
 */
double LightPortal::GetArea() const
{
    return light->GetArea();
}

/**
 * Returns the radiance that the light emits in every direction.
 * 
 * @returns The emitted radiance.
 */
Color LightPortal::GetIntensity() const
{
    return light->GetIntensity();
}

/**
 * Adds the light to a scene.
 * 
 * @param scn The scene to add to.
 */
void LightPortal::AddToScene(Scene* scn)
{
    light->material->light = this;
    Scene::LightAdder::AddLight(*scn, this);
    light->scene = scn;
}

/**
 * Estimates the integral of the rendering equation in the solid angle area that this light spans
 * on the surface of the given intersection info.
 * 
 * @param renderer The renderer that calculates the next event estimation.
 * @param info The intersection info at the point whose rendering equation integral we calculate.
 * @param rnd The randomizer.
 * @param component The component of the brdf.
 * @returns A tuple of the estimate and the point estimated on the light source.
 */
std::tuple<Color, Point> LightPortal::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Randomizer& rnd, int component) const
{
    auto [color, lightPoint] = light->NextEventEstimation(renderer, info, rnd, component);
    Ray ray(info.position, (lightPoint-info.position).Normalized());

    for(auto p : portals)
        if(p.Intersect(ray) >= 0)
            return { color, lightPoint } ;
    return { Color(0, 0, 0), lightPoint };
}
