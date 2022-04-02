#include "Bytestream.h"
#include "EmissiveMaterial.h"
#include "GeometricRoutines.h"
#include "Scene.h"
#include "SphereLight.h"

SphereLight::SphereLight()
{
}

SphereLight::SphereLight(Vector3d pos, float rad, Color str)
    : position_(pos), radius_(rad), Light(str)
{
    r_.Seed(GetTickCount() + int(this));
    material_ = new EmissiveMaterial();
    material_->emissivity = str;
    material_->light = this;
}

SphereLight::~SphereLight()
{
}

float SphereLight::Pdf(const IntersectionInfo& info, const Vector3d& out) const
{
    return abs(out*info.normal)/M_PI;
}

Color SphereLight::SampleRay(Ray& ray, Vector3d& normal, float& areaPdf, float& pdf) const
{
    SamplePoint(ray.origin, normal);
    Vector3d right, forward;
    MakeBasis(normal, right, forward);

    areaPdf = 1/GetArea();

    float r1 = r_.GetFloat(0, 2*M_PI);
    float r2 = r_.GetFloat(0, 0.9999f);
	ray.direction = forward*cos(r1)*sqrt(r2) + right*sin(r1)*sqrt(r2) 
                    + normal * sqrt(1 - r2);
    pdf = abs(ray.direction*normal)/M_PI;

    return Color::Identity*F_PI;
}

void SphereLight::SamplePoint(Vector3d& point, Vector3d& normal) const
{
    float z = r_.GetFloat(-1, 1);
    float r = sqrt(1 - z*z);
    float u = r_.GetFloat(0, 2*M_PI);
    normal = Vector3d(r*cos(u), r*sin(u), z);
    point = position_ + normal*radius_*1.0001f;
}

void SphereLight::SamplePointHemisphere(const Vector3d& apex, Vector3d& point, Vector3d& normal) const
{
    Vector3d right, forward;
    float z = r_.GetFloat(0, 1);
    float r = sqrt(1 - z*z);
    float u = r_.GetFloat(0, 2*M_PI);
    MakeBasis(apex, right, forward);
    normal = right*r*cos(u) + forward*r*sin(u) + apex*z;
    point = position_ + normal*radius_*1.0001f;
}

void SphereLight::Save(Bytestream& s) const
{
    s << ID_SPHERELIGHT << position_.x << position_.y << position_.z
      << radius_ << intensity_;
}
void SphereLight::Load(Bytestream& s)
{
    s >> position_.x >> position_.y >> position_.z 
      >> radius_ >> intensity_;
    material_ = new EmissiveMaterial();
    material_->emissivity = intensity_;
    material_->light = this;
}

float SphereLight::GetArea() const
{
    return 4*M_PI*radius_*radius_;
}

void SphereLight::AddToScene(std::shared_ptr<Scene> scn)
{
    Sphere* s = new Sphere(position_, Vector3d(0, 1, 0), 
                           Vector3d(0, 0, 1), radius_);
    Scene::LightAdder::AddLight(*scn, this);
    scn->AddModel(s);
    s->SetMaterial(material_);
}

Color SphereLight::NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, unsigned int component) const
{
    Vector3d toLight = position_ - info.GetPosition();
    Vector3d normal = info.GetNormal();
    Vector3d lightPoint, lightNormal;
    toLight.Normalize();
    SamplePointHemisphere(-toLight, lightPoint, lightNormal);
    toLight = lightPoint - info.GetPosition();
    float d = toLight.GetLength();
    toLight.Normalize();
    Ray lightRay = Ray(info.GetPosition(), toLight);

    if(toLight*lightNormal < 0)
    {
		if(renderer->TraceShadowRay(lightRay, d))
		{
            float cosphi = abs(normal*toLight);
			float costheta = abs(toLight*lightNormal);
            Color c;
			c = info.GetMaterial()->ComponentBRDF(info, toLight, component)
                *costheta*cosphi*intensity_*GetArea()/(2*d*d);
            return c;
		}
    }
    return Color(0, 0, 0);
}

Color SphereLight::NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info, unsigned int component) const
{
    Vector3d toLight = position_ - info.GetPosition();
    Vector3d normal = info.GetNormal();
    Vector3d lightPoint, lightNormal;
    toLight.Normalize();
    SamplePointHemisphere(-toLight, lightPoint, lightNormal);
    toLight = lightPoint - info.GetPosition();
    float d = toLight.GetLength();
    toLight.Normalize();
    Ray lightRay = Ray(info.GetPosition(), toLight);

    if(toLight*lightNormal < 0)
    {
		if(renderer->TraceShadowRay(lightRay, d))
		{
            float cosphi = abs(normal*toLight);
			float costheta = abs(toLight*lightNormal);
            Color c;
            Material* mat = info.GetMaterial();			
            c = mat->ComponentBRDF(info, toLight, component)
                     *costheta*cosphi*intensity_*GetArea()/(2*d*d);
            float brdfPdf = costheta*mat->PDF(info, toLight, 
                                              component, false)/(d*d);
            float lightPdf = 2.0f/GetArea();
            return c;
		}
    }
    return Color(0, 0, 0);
}


Color SphereLight::DirectHitMIS(const Renderer* renderer, 
                                const IntersectionInfo& lastInfo, 
                                const IntersectionInfo& thisInfo, 
                                unsigned int component) const
{
    Vector3d v = thisInfo.position - lastInfo.position;
    float d = v.GetLength();
    v.Normalize();
    float costheta = abs(v*thisInfo.normal);
    float lightPdf = 2.0f/GetArea();
    Material* mat = lastInfo.GetMaterial();
    float brdfPdf = costheta*mat->PDF(lastInfo, v, component, false)/(d*d);
    return intensity_/(1.0f + lightPdf*lightPdf/(brdfPdf*brdfPdf));
}