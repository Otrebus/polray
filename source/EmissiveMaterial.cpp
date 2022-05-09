#include "EmissiveMaterial.h"
#include "Bytestream.h"
#include "Sample.h"

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

Sample EmissiveMaterial::GetSample(const IntersectionInfo& info, bool adjoint) const
{
    auto pdf = 2*F_PI;
    auto rpdf = 2*F_PI;

    auto out = Ray(info.GetPosition(), -info.GetDirection());
    return Sample(Color(0.0, 0.0, 0.0), out, pdf, rpdf, false, 1); // Blackbody
}

Light* EmissiveMaterial::GetLight() const
{
    return light;
}

Color EmissiveMaterial::BRDF(const IntersectionInfo& info, const Vector3d& out, int sample) const
{
    return Color(0.0, 0.0, 0.0); // Blackbody
}

void EmissiveMaterial::ReadProperties(stringstream& ss)
{
}

double EmissiveMaterial::PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int sample) const
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