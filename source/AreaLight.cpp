#include "AreaLight.h"
#include "Windows.h"
#include "EmissiveMaterial.h"
#include "Renderer.h"
#include "Triangle.h"
#include "Utils.h"
#include "Scene.h"

AreaLight::AreaLight()
{
    material = new EmissiveMaterial();
}

AreaLight::AreaLight(const Vector3d& position, const Vector3d& corner1, const Vector3d& corner2, const Color& color) : pos(position), c1(corner1), c2(corner2)
{
    material = new EmissiveMaterial();
    intensity_ = color;
#ifdef DETERMINISTIC
    r.Seed(0);
#endif
}

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

double AreaLight::GetArea() const
{
    auto area = abs((c1^c2).GetLength());
    return area;
}

double AreaLight::Intersect(const Ray& ray) const {
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

bool AreaLight::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const {
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

double AreaLight::Pdf(const IntersectionInfo& info, const Vector3d& out) const
{
    Ray ray(info.position, out);
    return ray.direction*GetNormal()/F_PI;
}

std::tuple<Ray, Color, Vector3d, AreaPdf, AnglePdf> AreaLight::SampleRay() const
{
    Vector3d normal = c1^c2;
    normal.Normalize();
    Vector3d dir;

    double x = r.GetDouble(0, 1);
    double y = r.GetDouble(0, 1);

    auto [right, forward] = MakeBasis(normal);

    double r1 = r.GetDouble(0, 2*F_PI);
    double r2 = r.GetDouble(0, 1.0);

    Ray ray;
    ray.origin = pos + c1*x + c2*y + eps*normal;
    ray.direction = forward*cos(r1)*sqrt(r2) + right*sin(r1)*sqrt(r2) + normal*sqrt(1-r2);

    double areaPdf = 1.0f/GetArea();
    double anglePdf = abs(ray.direction*normal)/(F_PI);
    Color color = Color(1, 1, 1)*(F_PI);

    return { ray, color, normal, areaPdf, anglePdf };
}

std::tuple<Point, Normal> AreaLight::SamplePoint() const
{
    double x, y;
    Vector3d normal = c1^c2;
    normal.Normalize();
    Vector3d dir;

    x = r.GetDouble(0, 1);
    y = r.GetDouble(0, 1);

    return { pos + c1*x + c2*y + eps*normal, normal };
}

Vector3d AreaLight::GetNormal() const
{
    Vector3d normal = c1^c2;
    normal.Normalize();
    return normal;
}

void AreaLight::Save(Bytestream& stream) const
{
    stream << ID_AREALIGHT << pos << c1 << c2 << intensity_;
}

void AreaLight::Load(Bytestream& stream)
{
    stream >> pos >> c1 >> c2 >> intensity_;
}

Color AreaLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lp, Vector3d& ln, int component) const
{
    auto [lightPoint, lightNormal] = SamplePoint();
    lp = lightPoint;
    ln = lightNormal;

    Vector3d toLight = lightPoint - info.GetPosition();
    Vector3d normal = info.GetNormal();

    if(toLight*lightNormal < 0)
    {
        double d = toLight.GetLength();
        toLight.Normalize();

        Ray lightRay = Ray(info.GetPosition(), toLight);

        if(renderer->TraceShadowRay(lightRay, d*(1-eps)))
        {
            double cosphi = abs(normal*toLight);
            double costheta = abs(toLight*lightNormal);
            Color c;
            c = info.GetMaterial()->BRDF(info, toLight, component)
                *costheta*cosphi*intensity_*GetArea()/(d*d);
            return c;
        }
    }
    return Color::Black;
}
