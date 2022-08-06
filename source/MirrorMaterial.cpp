#include "Bytestream.h"
#include "MirrorMaterial.h"

#include <sstream>

using namespace std;

MirrorMaterial::MirrorMaterial()
{
}

MirrorMaterial::~MirrorMaterial()
{
}

Sample MirrorMaterial::GetSample(const IntersectionInfo& info, bool adjoint) const
{
	Vector3d Ng, Ns;
    const Vector3d in = info.GetDirection();

    if(in*info.GetGeometricNormal() < 0)
    {
        Ng = info.GetGeometricNormal();
        Ns = info.GetNormal();
    }
    else // The surface was hit from behind, so flip all normals
    {
        Ng = -info.GetGeometricNormal();
        Ns = -info.GetNormal();
    }

    Vector3d normal = Ns;

    Ray out;
	out.direction = Reflect(info.GetDirection(), normal);
	out.origin = info.GetPosition() + normal*0.0001f;
	out.direction.Normalize();

    return Sample((adjoint ? abs(out.direction*Ng)/abs(in*Ng) : 1.0f)*Color(1, 1, 1), out, 1, 1, true, 1);
}

Color MirrorMaterial::BRDF(const IntersectionInfo&, const Vector3d&, int) const
{
    return Color(0, 0, 0); // The chance that the out, in vectors are reflectant is effectively 0
}

Light* MirrorMaterial::GetLight() const
{
    return light;
}

void MirrorMaterial::ReadProperties(stringstream& ss)
{
    while(!ss.eof())
    {
        string line, s;
        getline(ss, line);
        stringstream ss2(line);
        ss2 >> s;
    }
}

double MirrorMaterial::PDF(const IntersectionInfo&, const Vector3d&, bool, int) const
{
    return 1;
}

void MirrorMaterial::Save(Bytestream& stream) const
{
    stream << (unsigned char) 103;
}

void MirrorMaterial::Load(Bytestream&)
{
}