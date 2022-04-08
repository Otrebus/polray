#include "AreaLight.h"
#include "Windows.h"
#include "EmissiveMaterial.h"
#include "Renderer.h"
#include "Triangle.h"
#include "Scene.h"

AreaLight::AreaLight()
{
    material = new EmissiveMaterial();
}

AreaLight::AreaLight(const Vector3d& position, const Vector3d& corner1, const Vector3d& corner2, const Color& color, shared_ptr<Scene> scn) : pos(position), c1(corner1), c2(corner2), scene(scn)
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

float AreaLight::GetArea() const
{
    return abs((c1^c2).GetLength());
}

float AreaLight::Pdf(const IntersectionInfo& info, const Vector3d& out) const
{
    Ray ray(info.position, out);
    if(!portals.empty())
    {   // Intersect the portal with the ray - this is almost
        // the same code as ordinary triangle intersection
        LightPortal p = portals.front();
           float u, v, t;
        Vector3d D;

        D.x = ray.direction.x;
        D.y = ray.direction.y;
        D.z = ray.direction.z;

        Vector3d E1 = p.v1;
        Vector3d E2 = p.v2;
        Vector3d T = ray.origin - p.pos;

        Vector3d P = E2^T;
        Vector3d Q = E1^D;

        float det = E2*Q;
        if(det < 0.0000000001f && det > -0.0000000001f)
            return 0;

        u = D*P/det;

        if(u > 1 || u < 0)
            return 0;

        v = T*Q/det;

        if(v > 1 || v < 0)
            return 0;

        t = E1*P/det;
        if(t < 0)
            return 0;
        return (1/p.GetArea())*t*t/(abs(p.GetNormal()*ray.direction));
    }
    return ray.direction*GetNormal()/F_PI;
}

Color AreaLight::SampleRay(Ray& ray, Vector3d& n, float& areaPdf, float& anglePdf) const
{
    Vector3d normal = c1^c2;
    normal.Normalize();
    Vector3d dir;

    float x = r.GetFloat(0, 0.9999f);
    float y = r.GetFloat(0, 0.9999f);

    ray.origin = pos + c1*x + c2*y + 0.0001f*normal;

    Vector3d right, forward;
    MakeBasis(normal, right, forward);

    n = normal;
    areaPdf = 1.0f/GetArea();

    if(!portals.empty())
    {
        LightPortal p = portals.front();
        Vector3d portalNormal = p.v1^p.v2;
        portalNormal.Normalize();
        float x = r.GetFloat(0, 0.9999f);
        float y = r.GetFloat(0, 0.9999f);

        Vector3d portalPos = p.pos + p.v1*x + p.v2*y;

        ray.direction = portalPos - ray.origin;
        float d = ray.direction.GetLength();
        ray.direction.Normalize();
        anglePdf = (1/p.GetArea())*d*d/(abs(p.GetNormal()*ray.direction));
        return Color(1, 1, 1)*abs(ray.direction*normal)/anglePdf;
    }

    float r1 = r.GetFloat(0, 2*F_PI);
    float r2 = r.GetFloat(0, 0.9999f);
     ray.direction =  forward*cos(r1)*sqrt(r2) + right*sin(r1)*sqrt(r2) 
        + normal * sqrt(1-r2);
    anglePdf = abs(ray.direction*normal)/(F_PI);
    return Color(1, 1, 1)*(F_PI);
}

void AreaLight::SamplePoint(Vector3d& point, Vector3d& n) const
{
    float x, y;
    Vector3d normal = c1^c2;
    normal.Normalize();
    Vector3d dir;

    x = r.GetFloat(0, 0.9999f);
    y = r.GetFloat(0, 0.9999f);
    //dir = Vector3d(r.GetFloat(-1, 1), r.GetFloat(-1, 1), r.GetFloat(-1, 1));

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

Color AreaLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, unsigned int component) const
{
    Vector3d lightPoint, lightNormal;
    SamplePoint(lightPoint, lightNormal);
    Vector3d toLight = lightPoint - info.GetPosition();
    float d = toLight.GetLength();
    Vector3d normal = info.GetNormal();

    if(toLight*lightNormal < 0)
    {
           float d = toLight.GetLength();
        toLight.Normalize();

        Ray lightRay = Ray(info.GetPosition(), toLight);

        if(renderer->TraceShadowRay(lightRay, d))
        {
            float cosphi = abs(normal*toLight);
            float costheta = abs(toLight*lightNormal);
            Color c;
            c = info.GetMaterial()->ComponentBRDF(info, toLight, component)
                *costheta*cosphi*intensity_*GetArea()/(d*d);
            return c;
        }
    }
    return Color(0, 0, 0);
}

Color AreaLight::DirectHitMIS(const Renderer* renderer, const IntersectionInfo& lastInfo, const IntersectionInfo& thisInfo, unsigned int component) const
{
    Vector3d v = thisInfo.position - lastInfo.position;
    float d = v.GetLength();
    v.Normalize();
    float costheta = abs(v*thisInfo.normal);
    float lightPdf = 1.0f/GetArea();
    Material* mat = lastInfo.GetMaterial();
    float brdfPdf = costheta*mat->PDF(lastInfo, v, component, false)/(d*d);
    return intensity_/(1.0f + lightPdf*lightPdf/(brdfPdf*brdfPdf));
}

Color AreaLight::NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info, unsigned int component) const
{
    Vector3d lightPoint, lightNormal;
    SamplePoint(lightPoint, lightNormal);
    Vector3d toLight = lightPoint - info.GetPosition();
    Vector3d normal = info.GetNormal();

    if(toLight*lightNormal < 0)
    {
           float d = toLight.GetLength();
        toLight.Normalize();
        Ray lightRay = Ray(info.GetPosition(), toLight);

        if(renderer->TraceShadowRay(lightRay, d))
        {
            float cosphi = abs(normal*toLight);
            float costheta = abs(toLight*lightNormal);
            Material* mat = info.GetMaterial();
            Color c = mat->ComponentBRDF(info, toLight, component)
                      *costheta*cosphi*intensity_*GetArea()/(d*d);
            float brdfPdf = costheta*mat->PDF(info, toLight, 
                                              component, false)/(d*d);
            float lightPdf = 1.0f/GetArea();
            return c/(1.0f + brdfPdf*brdfPdf/(lightPdf*lightPdf));
        }
    }
    return Color(0, 0, 0);
}

void AreaLight::AddPortal(const Vector3d& pos, const Vector3d& v1, const Vector3d& v2)
{
    portals.push_back(LightPortal(pos, v1, v2));
}