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

Color MirrorMaterial::GetSampleE(const IntersectionInfo& info, Ray& out, float& pdf, float& rpdf, unsigned char& component, bool adjoint) const
{
    //Vector3d normal = info.GetGeometricNormal();
    Vector3d normal = info.GetNormal();
    out.direction = Reflect(info.GetDirection(), normal);
    out.origin = info.GetPosition() + normal*0.0001f;
    out.direction.Normalize();

    pdf = 1;
    rpdf = 1;
    component = 1;

    return Color(1, 1, 1);
}

Color MirrorMaterial::GetSample(const IntersectionInfo& info, Ray& out, bool adjoint) const
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

    out.direction = Reflect(info.GetDirection(), normal);
    out.origin = info.GetPosition() + normal*0.0001f;
    out.direction.Normalize();

    float fac1 = (out.direction*Ns)*(in*Ns);
    float fac2 = (out.direction*Ng)*(in*Ng);

    if(fac1 > 0 || fac2 > 0)
        return Color(0, 0, 0);

    return (adjoint ? abs(out.direction*Ng)/abs(in*Ng) : 1.0f)*Color(1, 1, 1);
}

Color MirrorMaterial::BRDF(const IntersectionInfo& info, const Vector3d& out) const
{
    return Color(0, 0, 0); // The chance that the out, in vectors are reflecant is effectively 0
}

Color MirrorMaterial::ComponentBRDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component) const
{
    assert(component == 1);
    return Color(0, 0, 0);
}

Light* MirrorMaterial::GetLight() const
{
    return light;
}

bool MirrorMaterial::IsSpecular(unsigned char component) const
{
    assert(component == 1);	
    return true;
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

float MirrorMaterial::PDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component, bool adjoint) const
{
    assert(component == 1);
    return 1;
}

void MirrorMaterial::Save(Bytestream& stream) const
{
    stream << (unsigned char) 103;
}

void MirrorMaterial::Load(Bytestream& stream)
{
}