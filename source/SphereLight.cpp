/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file SphereLight.cpp
 * 
 * Implementation of the SphereLight class.
 */

#include "Bytestream.h"
#include "EmissiveMaterial.h"
#include "GeometricRoutines.h"
#include "Scene.h"
#include "SphereLight.h"
#include "Utils.h"

/**
 * Constructor.
 */
SphereLight::SphereLight()
{
}

/**
 * Constructor.
 * 
 * @param pos The position of the light.
 * @param rad The radius of the light.
 * @param str The intensity of the light.
 */
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

/**
 * Destructor.
 */
SphereLight::~SphereLight()
{
}

/**
 * Checks if and where the given ray intersects the light.
 * 
 * @param ray The ray to check against the light.
 * @returns The distance along the ray that the light source was hit.
 */
double SphereLight::Intersect(const Ray& ray) const
{
    return IntersectSphere(position_, radius_, ray);
}

/**
 * Returns the intersection info of a ray that hit the light.
 * 
 * @param ray The ray that hit the light.
 * @param info The intersection info to fill in.
 * @returns Whether the area light was actually hit by the ray.
 */
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

/**
 * Samples an outgoing ray from the light.
 * 
 * @param rnd The randomizer to sample with.
 * @returns A tuple of the outgoing ray, its sampled color and normal and the area and angle pdfs.
 */
std::tuple<Ray, Color, Normal, AreaPdf, AnglePdf> SphereLight::SampleRay(Randomizer& rnd) const
{
    Ray ray;

    auto [point, normal] = SamplePoint(rnd);
    ray.origin = point;
    auto [right, forward] = MakeBasis(normal);

    double r1 = rnd.GetDouble(0, 1), r2 = rnd.GetDouble(0, 1);
    ray.direction = SampleHemisphereCos(r1, r2, normal);
    double anglePdf = abs(ray.direction*normal)/pi;
    double areaPdf = 1/GetArea();

    return { ray, Color::Identity*pi, normal, areaPdf, anglePdf };
}

std::tuple<Point, Normal> SphereLight::SamplePoint(Randomizer& rnd) const
{
    auto r1 = rnd.GetDouble(0, 1), r2 = rnd.GetDouble(0, 1);
    auto pos = SampleSphereUniform(r1, r2);

    auto normal = pos;
    auto point = position_ + pos*radius_ + normal*eps;

    return { point, normal };
}

/**
 * Saves the light source to a stream.
 * 
 * @param stream The stream that we serialize to.
 */
void SphereLight::Save(Bytestream& stream) const
{
    stream << ID_SPHERELIGHT << position_.x << position_.y << position_.z << radius_ << intensity;
}

/**
 * Loads the light source from a stream.
 * 
 * @param stream The stream that we deserialize from.
 */
void SphereLight::Load(Bytestream& stream)
{
    stream >> position_.x >> position_.y >> position_.z >> radius_ >> intensity;
    material = new EmissiveMaterial();
    material->emissivity = intensity;
    material->light = this;
}

/**
 * Returns the area of the light.
 * 
 * @returns The area of the light.
 */
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
std::tuple<Color, Point> SphereLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Randomizer& rnd, int component) const
{
    Vector3d lightPoint, lightNormal;
    Vector3d toLight = position_ - info.position;
    Vector3d normal = info.normal;
    toLight.Normalize();

    double r1 = rnd.GetDouble(0, 1), r2 = rnd.GetDouble(0, 1);
    lightNormal = SampleHemisphereUniform(r1, r2, -toLight);
    lightPoint = position_ + lightNormal*(radius_+eps);

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
            c = info.material->BRDF(info, toLight, component)*costheta*cosphi*intensity*GetArea()/(2*d*d);
            return { c, lightPoint };
        }
    }
    return { Color(0, 0, 0), lightPoint };
}
