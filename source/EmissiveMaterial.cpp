#include "EmissiveMaterial.h"
#include "Bytestream.h"

EmissiveMaterial::EmissiveMaterial()
{
#ifdef DETERMINISTIC
    rnd.Seed(0);
#else
    rnd.Seed(GetTickCount() + int(this));
#endif
    texture = 0;
    normalmap = 0;
}

EmissiveMaterial::~EmissiveMaterial()
{
    // Not sure what I meant with this
    if(normalmap)
        delete normalmap;
    if(texture)
        delete texture;
}

Color EmissiveMaterial::GetSampleE(const IntersectionInfo& info, Ray& out, float& pdf, float& rpdf, unsigned char& component, bool adjoint) const
{
    pdf = 2*F_PI;
    rpdf = 2*F_PI;

    component = 1;

    out = Ray(info.GetPosition(), -info.GetDirection());
    return Color(0.0, 0.0, 0.0); // Blackbody
}


Color EmissiveMaterial::GetSample(const IntersectionInfo& info, Ray& out, bool adjoint) const
{
    out = Ray(info.GetPosition(), -info.GetDirection());
    return Color(0.0, 0.0, 0.0); // Blackbody
}

Light* EmissiveMaterial::GetLight() const
{
    return light;
}

Color EmissiveMaterial::BRDF(const IntersectionInfo& info, const Vector3d& out) const
{
    return Color(0.0, 0.0, 0.0); // Blackbody
}

Color EmissiveMaterial::ComponentBRDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component) const
{
    return Color(0.0f, 0.0f, 0.0f);
}

bool EmissiveMaterial::IsSpecular(unsigned char component) const
{
    assert(component == 1);
    return false;
}

void EmissiveMaterial::ReadProperties(stringstream& ss)
{
}

float EmissiveMaterial::PDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component, bool adjoint) const
{
    return 0;
}

void EmissiveMaterial::Save(Bytestream& stream) const
{
    stream << ID_EMISSIVEMATERIAL;
    stream << emissivity;
}

void EmissiveMaterial::Load(Bytestream& stream)
{
    stream >> emissivity;
}