#include "AreaLight.h"
#include "Windows.h"
#include "EmissiveMaterial.h"
#include "Renderer.h"
#include "Triangle.h"
#include "Utils.h"
#include "Scene.h"

/**
 * Constructor.
 */
AreaLight::AreaLight()
{
    material = new EmissiveMaterial();
}

/**
 * Constructor.
 * 
 * @param position The position of the area light.
 * @param c1 One component of the area light parallelogram
 * @param c2 The other component of the area light parallelogram
 * @param color The radiance that the light emits in every direction.
 */
AreaLight::AreaLight(const Vector3d& position, const Vector3d& c1, const Vector3d& c2, const Color& color) : pos(position), c1(c1), c2(c2)
{
    material = new EmissiveMaterial();
    intensity_ = color;
#ifdef DETERMINISTIC
    r.Seed(0);
#endif
}

/**
 * Adds the area light to scene.
 * 
 * @param scn The scene to add the light to.
 */
void AreaLight::AddToScene(Scene* scn)
{
    scene = scn;
   
    material->light = this;
    material->emissivity = intensity_;
    Triangle* tr1 = new Triangle(pos, pos + c1, pos + c2);
    Triangle* tr2 = new Triangle(pos + c1 + c2, pos + c2, pos + c1);

    Vector3d normal = c1^c2;
    normal.Normalize();
    tr1->v0.normal = normal;
    tr1->v1.normal = normal;
    tr1->v2.normal = normal;
    tr2->v0.normal = normal;
    tr2->v1.normal = normal;
    tr2->v2.normal = normal;

    tr1->SetMaterial(material);
    tr2->SetMaterial(material);

    Scene::PrimitiveAdder::AddPrimitive(*scene, tr1);
    Scene::PrimitiveAdder::AddPrimitive(*scene, tr2);
    Scene::LightAdder::AddLight(*scene, this);
}

/**
 * Returns the area of the light.
 * 
 * @returns The area of the light.
 */
double AreaLight::GetArea() const
{
    auto area = abs((c1^c2).Length());
    return area;
}

/**
 * Checks if and where the given ray intersects the light.
 * 
 * @param ray The ray to check against the light.
 * @returns The distance along the ray that the light source was hit.
 */
double AreaLight::Intersect(const Ray& ray) const
{
    double u, v, t;
    Vector3d D = ray.direction;

    Vector3d E1 = c1;
    Vector3d E2 = c2;
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
    return t < 0 ? -inf : t;
}

/**
 * Returns the intersection info of a ray that hit the light.
 * 
 * @param ray The ray that hit the light.
 * @param info The intersection info to fill in.
 * @returns Whether the area light was actually hit by the ray.
 */
bool AreaLight::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
    double u, v, t;
    info.direction = ray.direction;
    Vector3d D = ray.direction;

    Vector3d E1 = c1;
    Vector3d E2 = c2;
    Vector3d T = ray.origin - pos;

    Vector3d P = E2^T;
    Vector3d Q = E1^D;

    double det = E2*Q;
    if(!det)
        return false;

    u = D*P/det;

    if(u > 1 || u < 0)
        return false;

    v = T*Q/det;

    if(v > 1 || v < 0)
        return false;

    t = E1*P/det;
    if(t < 0)
        return false;

    info.normal = info.geometricnormal = (c1^c2).Normalized();
    info.position = pos + u*E1 + v*E2 + (info.geometricnormal*info.direction < 0 ? info.geometricnormal*eps : -info.geometricnormal*eps);
    info.texpos.x = u;
    info.texpos.y = v;
    info.material = material;

    return true;
}

/**
 * Returns the value of the angle pdf of a ray that was sampled at the light source.
 * 
 * @param info Contains the point that we evaluate the pdf at.
 * @param out The outgoing vector at that point.
 * @returns The value of the angle pdf at the given point and outgoing vector.
 */
double AreaLight::Pdf(const IntersectionInfo& info, const Vector3d& out) const
{
    Ray ray(info.position, out);
    return ray.direction*GetNormal()/pi;
}

/**
 * Samples an outgoing ray from the light.
 * 
 * @param rnd The randomizer to sample with.
 * @returns A tuple of the outgoing ray, its sampled color and normal and the area and angle pdfs.
 */
std::tuple<Ray, Color, Normal, AreaPdf, AnglePdf> AreaLight::SampleRay(Randomizer& rnd) const
{
    Vector3d normal = c1^c2;
    normal.Normalize();
    Vector3d dir;

    double x = rnd.GetDouble(0, 1);
    double y = rnd.GetDouble(0, 1);

    auto [right, forward] = MakeBasis(normal);

    double r1 = rnd.GetDouble(0, 2*pi);
    double r2 = rnd.GetDouble(0, 1.0);

    Ray ray;
    ray.origin = pos + c1*x + c2*y + eps*normal;
    ray.direction = forward*cos(r1)*sqrt(r2) + right*sin(r1)*sqrt(r2) + normal*sqrt(1-r2);

    double areaPdf = 1.0f/GetArea();
    double anglePdf = abs(ray.direction*normal)/pi;
    Color color = Color(1, 1, 1)*pi;

    return { ray, color, normal, areaPdf, anglePdf };
}

/**
 * Samples a random point of the area light.
 * 
 * @param rnd The randomizer to sample with.
 * @returns A tuple of the point and its normal.
 */
std::tuple<Point, Normal> AreaLight::SamplePoint(Randomizer& rnd) const
{
    double x = rnd.GetDouble(0, 1), y = rnd.GetDouble(0, 1);
    Vector3d normal = c1^c2;
    normal.Normalize();
    Vector3d dir;

    return { pos + c1*x + c2*y + eps*normal, normal };
}

/**
 * Returns the direction the area light is facing.
 */
Vector3d AreaLight::GetNormal() const
{
    Vector3d normal = c1^c2;
    normal.Normalize();
    return normal;
}

/**
 * Saves the light source to a stream.
 * 
 * @param stream The stream that we serialize to.
 */
void AreaLight::Save(Bytestream& stream) const
{
    stream << ID_AREALIGHT << pos << c1 << c2 << intensity_;
}

/**
 * Loads the light source from a stream.
 * 
 * @param stream The stream that we deserialize from.
 */
void AreaLight::Load(Bytestream& stream)
{
    stream >> pos >> c1 >> c2 >> intensity_;
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
std::tuple<Color, Point> AreaLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Randomizer& rnd, int component) const
{
    auto [lightPoint, lightNormal] = SamplePoint(rnd);

    Vector3d toLight = lightPoint - info.position;
    Vector3d normal = info.normal;

    if(toLight*lightNormal < 0)
    {
        double d = toLight.Length();
        toLight.Normalize();

        Ray lightRay = Ray(info.position, toLight);

        if(renderer->TraceShadowRay(lightRay, d*(1-eps)))
        {
            double cosphi = abs(normal*toLight);
            double costheta = abs(toLight*lightNormal);
            Color c(info.material->BRDF(info, toLight, component)*costheta*cosphi*intensity_*GetArea()/(d*d));
            return { c, lightPoint };
        }
    }
    return { Color::Black, lightPoint };
}
