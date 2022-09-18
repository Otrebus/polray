#include "EmissiveMaterial.h"
#include "Bytestream.h"
#include "Utils.h"
#include "Sample.h"
#include "IntersectionInfo.h"

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
}

Sample EmissiveMaterial::GetSample(const IntersectionInfo& info, Randomizer&, bool) const
{
    auto out = Ray(info.position, -info.direction);
    return Sample(Color(0.0, 0.0, 0.0), out, 2*pi, 2*pi, false, 1);
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