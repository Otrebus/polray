#include "EmissiveMaterial.h"
#include "Bytestream.h"
#include "Sample.h"
#include "Utils.h"

EmissiveMaterial::EmissiveMaterial()
{
#ifdef DETERMINISTIC
    rnd.Seed(0);
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

Sample EmissiveMaterial::GetSample(const IntersectionInfo& info, bool) const
{
    auto pdf = 2*pi;
    auto rpdf = 2*pi;

    auto out = Ray(info.GetPosition(), -info.GetDirection());
    return Sample(Color(0.0, 0.0, 0.0), out, pdf, rpdf, false, 1); // Blackbody
}

Light* EmissiveMaterial::GetLight() const
{
    return light;
}

Color EmissiveMaterial::BRDF(const IntersectionInfo&, const Vector3d&, int) const
{
    return Color(0.0, 0.0, 0.0); // Blackbody
}

void EmissiveMaterial::ReadProperties(std::stringstream&)
{
}

double EmissiveMaterial::PDF(const IntersectionInfo&, const Vector3d&, bool, int) const
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