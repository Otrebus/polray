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
#else
    r.Seed(GetTickCount() + int(this));
#endif
}

void AreaLight::AddToScene(std::shared_ptr<Scene> scn)
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

    scn->AddModel(tr1);
    scn->AddModel(tr2);

    Scene::LightAdder::AddLight(*scn, this);
}

double AreaLight::GetArea() const
{
    return abs((c1^c2).GetLength());
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
    if(det < 0.0000000001f && det > -0.0000000001f)
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
    Vector3d D = ray.direction;

    Vector3d E1 = c1;
    Vector3d E2 = c2;
    Vector3d T = ray.origin - pos;

    Vector3d P = E2^T;
    Vector3d Q = E1^D;

    double det = E2*Q;
    if(det < 0.0000000001f && det > -0.0000000001f)
        return 0;

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
	info.position = pos + u*E1 + v*E2 + (info.geometricnormal*info.direction < 0 ? info.geometricnormal*0.0001f : -info.geometricnormal*0.0001f);
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

Color AreaLight::SampleRay(Ray& ray, Vector3d& n, double& areaPdf, double& anglePdf) const
{
    Vector3d normal = c1^c2;
    normal.Normalize();
    Vector3d dir;

    double x = r.GetDouble(0, 0.9999f);
    double y = r.GetDouble(0, 0.9999f);

    ray.origin = pos + c1*x + c2*y + 0.0001f*normal;

    Vector3d right, forward;
    MakeBasis(normal, right, forward);

    n = normal;
    areaPdf = 1.0f/GetArea();

    double r1 = r.GetDouble(0, 2*F_PI);
    double r2 = r.GetDouble(0, 0.9999f);
    ray.direction = forward*cos(r1)*sqrt(r2) + right*sin(r1)*sqrt(r2) + normal*sqrt(1-r2);
    anglePdf = abs(ray.direction*normal)/(F_PI);
    return Color(1, 1, 1)*(F_PI);
}

void AreaLight::SamplePoint(Vector3d& point, Vector3d& n) const
{
    double x, y;
    Vector3d normal = c1^c2;
    normal.Normalize();
    Vector3d dir;

    x = r.GetDouble(0, 0.9999f);
    y = r.GetDouble(0, 0.9999f);
    //dir = Vector3d(r.Getdouble(-1, 1), r.Getdouble(-1, 1), r.Getdouble(-1, 1));

    point = pos + c1*x + c2*y + 0.0001f*normal;
    n = normal;
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

Color AreaLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lp, Vector3d& ln) const
{
    Vector3d lightPoint, lightNormal;
    SamplePoint(lightPoint, lightNormal);
    lp = lightPoint;
    ln = lightNormal;

    Vector3d toLight = lightPoint - info.GetPosition();
    double d = toLight.GetLength();
    Vector3d normal = info.GetNormal();

    if(toLight*lightNormal < 0)
    {
        double d = toLight.GetLength();
        toLight.Normalize();

        Ray lightRay = Ray(info.GetPosition(), toLight);

        if(renderer->TraceShadowRay(lightRay, d))
        {
            double cosphi = abs(normal*toLight);
            double costheta = abs(toLight*lightNormal);
            Color c;
            c = info.GetMaterial()->BRDF(info, toLight)
                *costheta*cosphi*intensity_*GetArea()/(d*d);
            return c;
        }
    }
    return Color(0, 0, 0);
}

Color AreaLight::DirectHitMIS(const Renderer* renderer, const IntersectionInfo& lastInfo, const IntersectionInfo& thisInfo) const
{
    Vector3d v = thisInfo.position - lastInfo.position;
    double d = v.GetLength();
    v.Normalize();
    double costheta = abs(v*thisInfo.normal);
    double lightPdf = 1.0f/GetArea();
    Material* mat = lastInfo.GetMaterial();
    double brdfPdf = costheta*mat->PDF(lastInfo, v, false)/(d*d);
    return intensity_/(1.0f + lightPdf*lightPdf/(brdfPdf*brdfPdf));
}

Color AreaLight::NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info) const
{
    Vector3d lightPoint, lightNormal;
    SamplePoint(lightPoint, lightNormal);
    Vector3d toLight = lightPoint - info.GetPosition();
    Vector3d normal = info.GetNormal();

    if(toLight*lightNormal < 0)
    {
        double d = toLight.GetLength();
        toLight.Normalize();
        Ray lightRay = Ray(info.GetPosition(), toLight);

        if(renderer->TraceShadowRay(lightRay, d))
        {
            double cosphi = abs(normal*toLight);
            double costheta = abs(toLight*lightNormal);
            Material* mat = info.GetMaterial();
            Color c = mat->BRDF(info, toLight)
                      *costheta*cosphi*intensity_*GetArea()/(d*d);
            double brdfPdf = costheta*mat->PDF(info, toLight, false)/(d*d);
            double lightPdf = 1.0f/GetArea();
            return c/(1.0f + brdfPdf*brdfPdf/(lightPdf*lightPdf));
        }
    }
    return Color(0, 0, 0);
}
