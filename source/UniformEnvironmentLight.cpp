#include "EmissiveMaterial.h"
#include "Utils.h"
#include "Scene.h"
#include "UniformEnvironmentLight.h"
#include "GeometricRoutines.h"

/**
 * Constructor.
 */
UniformEnvironmentLight::UniformEnvironmentLight()
{
    material = new EmissiveMaterial();
}

/**
 * Constructor.
 * 
 * @param position The position of the light.
 * @param radius The radius of the light.
 * @param color The radiance that the light emits in every direction.
 */
UniformEnvironmentLight::UniformEnvironmentLight(const Vector3d& position, double radius, const Color& color) : position(position), radius(radius)
{
    material = new EmissiveMaterial();
    intensity = color;
}

/**
 * Adds the light to the scene.
 * 
 * @param scn The scene to add to.
 */
void UniformEnvironmentLight::AddToScene(Scene* scn)
{
    material->light = this;
    material->emissivity = intensity;
    Scene::LightAdder::AddLight(*scn, this);
    scene = scn;
}

/**
 * Returns the area of the light.
 * 
 * @returns The area of the light.
 */
double UniformEnvironmentLight::GetArea() const
{
    return radius*radius*4*pi;
}

/**
 * Checks if and where the given ray intersects the light.
 * 
 * @param ray The ray to check against the light.
 * @returns The distance along the ray that the light source was hit.
 */
double UniformEnvironmentLight::Intersect(const Ray& ray) const
{
    return IntersectSphere(position, radius, ray);
}

/**
 * Returns the intersection info of a ray that hit the light.
 * 
 * @param ray The ray that hit the light.
 * @param info The intersection info to fill in.
 * @returns Whether the area light was actually hit by the ray.
 */
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

/**
 * Samples an outgoing ray from the light.
 * 
 * @param rnd The randomizer to sample with.
 * @returns A tuple of the outgoing ray, its sampled color and normal and the area and angle pdfs.
 */
std::tuple<Ray, Color, Normal, AreaPdf, AnglePdf> UniformEnvironmentLight::SampleRay(Randomizer& rnd) const
{
    Ray ray;
    auto [origin, normal] = SamplePoint(rnd);
    ray.origin = origin;
    
    double areaPdf = 1/GetArea();

    auto [h, A, right, forward, cv] = GetProjectedSceneHull(ray, normal);

    auto n = normal;
    double a = rnd.GetDouble(0, A);
    double aSum = 0;

    auto pp = ray.origin + normal*(cv*normal);

    for(int i = 0; i < h.size()-2; i++)
    {
        aSum += std::abs((h[i+1]-h[0])^(h[i+2]-h[0]))/2;
        if(aSum > a)
        {
            double u = sqrt(rnd.GetDouble(0, 1)), v = rnd.GetDouble(0, 1);
            auto e1 = h[i+1] - h[0], e2 = h[i+2] - h[0];

            auto p = h[0] + u*(e1 + v*(e2-e1));
            auto p3 = right*p.x + forward*p.y + pp;
            auto dir = (p3-ray.origin);
            ray.direction = dir.Normalized();
            double anglePdf = dir.Length2()/(ray.direction*n)/A;
            return { ray, (ray.direction*n)*Color::Identity/anglePdf, normal, areaPdf, anglePdf };
        }
    }
    return { ray, Color::Identity, Vector3d(1, 1, 1), 1, 1 }; // Should never happen
}

/**
 * Returns the value of the angle pdf of a ray that was sampled at the light source.
 * 
 * @param info Contains the point that we evaluate the pdf at.
 * @param out The outgoing vector at that point.
 * @returns The value of the angle pdf at the given point and outgoing vector.
 */
double UniformEnvironmentLight::Pdf(const IntersectionInfo& info, const Vector3d& out) const
{
    auto n = info.normal;
    auto outRay = Ray(info.position, out);
    auto [h, A, right, forward, cv] = GetProjectedSceneHull(outRay, n);

    auto v = n*(cv*n);
    auto r = v.Length()/std::abs(out*n);
    return r*r/std::abs(out*n)/A;
}

/**
 * Samples a random point of the area light.
 * 
 * @param rnd The randomizer to sample with.
 * @returns A tuple of the point and its normal.
 */
std::tuple<Point, Normal> UniformEnvironmentLight::SamplePoint(Randomizer& rnd) const
{
    auto r1 = rnd.GetDouble(0, 1), r2 = rnd.GetDouble(0, 1);
    auto pos = SampleSphereUniform(r1, r2);
    auto normal = -pos;
    auto point = position + pos*radius + normal*eps;
    return { point, normal };
}

/**
 * Saves the light source to a stream.
 * 
 * @param stream The stream that we serialize to.
 */
void UniformEnvironmentLight::Save(Bytestream& stream) const
{
    stream << ID_UNIFORMENVIRONMENTLIGHT << position.x << position.y << position.z << radius << intensity;
}

/**
 * Loads the light source from a stream.
 * 
 * @param stream The stream that we deserialize from.
 */
void UniformEnvironmentLight::Load(Bytestream& stream)
{
    stream >> position.x >> position.y >> position.z >> radius >> intensity;
    material = new EmissiveMaterial();
    material->emissivity = intensity;
    material->light = this;
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
std::tuple<Color, Point> UniformEnvironmentLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Randomizer& rnd, int component) const
{
    auto [lightPoint, lightNormal] = SamplePoint(rnd);
    auto toLight = lightPoint - info.position;
    Ray lightRay = Ray(info.position, toLight);
    double d = toLight.Length()*(1.-1.e-6);
    toLight.Normalize();

    if(renderer->TraceShadowRay(lightRay, d))
    {
        double cosphi = abs(info.normal*toLight);
        double costheta = abs(toLight*lightNormal);
        Material* mat = info.material;
        Color c = mat->BRDF(info, toLight, component)*costheta*cosphi*intensity*GetArea()/(d*d);
        return { c, lightPoint };
    }
    return { Color(0, 0, 0), lightPoint };
}

/**
 * Returns the scene hull projected on a plane which runs through the center of the bounding box
 * of the scene, and is perpendicular against the origin of the given ray towards that center.
 * 
 * @param ray The ray from whose origin we project the scene.
 * @param normal The normal of the environment light at the point of the origin.
 * @returns A tuple of the convex hull, its area, the basis of the coordinates of the convex hull,
 *          and the vector between the ray origin and the bounding box center
 */
std::tuple<std::vector<Vector2d>, double, Vector3d, Vector3d, Vector3d> UniformEnvironmentLight::GetProjectedSceneHull(Ray& ray, Vector3d normal) const
{
    auto bb = scene->GetBoundingBox();

    auto [right, forward] = MakeBasis(normal);

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
    for(int i = 0; i < 8; i++)
    {
        auto u = p[i] - ray.origin;
        Vector2d w = Vector2d(u*right, u*forward).Normalized();
        auto vp = n*(n*u);
        auto v = n*(cv*n);
        auto rp = ((v.Length()/(u*n))*(u - n*(u*n)).Length());

        q.push_back(rp*w);
    }

    auto h = ConvexHull(q);

    double A = 0;
    for(int i = 0; i < h.size(); i++)
        A += h[i].x*h[(i+1)%h.size()].y - h[i].y*h[(i+1)%h.size()].x;
    A *= 0.5;
    return { h, A, right, forward, cv };
}
