#include <sstream>
#include "Bytestream.h"
#include "LambertianMaterial.h"

using namespace std;

LambertianMaterial::LambertianMaterial()
{
    Kd = Color(1, 1, 1);
    emissivity = Color(0, 0, 0);
#ifdef DETERMINISTIC
    rnd.Seed(0);
#else
    rnd.Seed(GetTickCount() + int(this));
#endif
}

LambertianMaterial::~LambertianMaterial()
{
}

Color LambertianMaterial::GetSampleE(const IntersectionInfo& info, Ray& out, float& pdf, float& rpdf, unsigned char& component, bool adjoint) const
{
    component = 1;

    float r1 = rnd.GetFloat(0, 2*M_PI);
    float r2 = rnd.GetFloat(0.0001f, 0.9999f);

    Vector3d N_g = info.GetGeometricNormal();
    Vector3d N_s = info.GetNormal();
    
    const Vector3d& w_i = -info.GetDirection();
    
    if(w_i*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    Vector3d N = adjoint ? N_g : N_s;
    Vector3d adjN = adjoint ? N_s : N_g;
    Vector3d dir;

    SampleHemisphereCos(r1, r2, N, dir);

    const Vector3d& w_o = dir;
    out.origin = info.GetPosition();
    out.direction = dir;

    if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
        return Color(0, 0, 0);

    pdf = dir*N/F_PI;
    rpdf = w_i*adjN/F_PI;
    if(rpdf < 0)
        rpdf = 0;

    return adjoint ? Kd*abs(w_i*N_s)/abs(w_i*N_g) : Kd;
}

Color LambertianMaterial::GetSample(const IntersectionInfo& info, Ray& out, bool adjoint) const
{
    float r1 = rnd.GetFloat(0, 2*M_PI);
    float r2 = rnd.GetFloat(0.0001f, 0.9999f);

    Vector3d N_g = info.GetGeometricNormal();
    Vector3d N_s = info.GetNormal();
    
    const Vector3d& w_i = -info.GetDirection();

    if(w_i*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    Vector3d N = adjoint ? N_g : N_s;
    
    Vector3d dir;
    SampleHemisphereCos(r1, r2, N, dir);

    const Vector3d& w_o = dir;

    if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
        return Color(0, 0, 0);

    out.origin = info.GetPosition() + N_g*0.0001f;
    out.direction = dir;
    return adjoint ? Kd*abs(w_i*N_s)/abs(w_i*N_g) : Kd;
}

Color LambertianMaterial::BRDF(const IntersectionInfo& info, const Vector3d& out) const
{
    Vector3d N_s = info.GetNormal();
    Vector3d N_g = info.GetGeometricNormal();

    const Vector3d& w_i = -info.GetDirection();
    const Vector3d& w_o = out;

    if(w_i*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
        return Color(0, 0, 0);

    return Kd/F_PI;
}

Color LambertianMaterial::ComponentBRDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component) const
{
    assert(component == 1);
    Vector3d N_s = info.GetNormal();
    Vector3d N_g = info.GetGeometricNormal();

    const Vector3d& w_i = -info.GetDirection();
    const Vector3d& w_o = out;

    if(w_i*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
        return Color(0, 0, 0);

    return Kd/F_PI;
}

Light* LambertianMaterial::GetLight() const
{
    return light;
}

bool LambertianMaterial::IsSpecular(unsigned char component) const
{
    assert(component == 1);
    return false;
}

void LambertianMaterial::ReadProperties(stringstream& ss)
{
    while(!ss.eof())
    {
        string line;
        string a;
        getline(ss, line);
        stringstream ss2(line);
        ss2 >> a;
        transform(a.begin(), a.end(), a.begin(), tolower);
        if(a == "kd")
            ss2 >> Kd.r >> Kd.g >> Kd.b;
    }
}

float LambertianMaterial::PDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component, bool adjoint) const
{
    assert(component == 1);
    Vector3d N_s = info.GetNormal();
    Vector3d N_g = info.GetGeometricNormal();

    const Vector3d& w_i = -info.GetDirection();
    const Vector3d& w_o = out;

    if(w_i*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
        return 0;

    return (adjoint ? N_g : N_s)*w_o/F_PI;
}

void LambertianMaterial::Save(Bytestream& stream) const
{
    stream << (unsigned char) ID_LAMBERTIANMATERIAL;
    stream << Kd;
}

void LambertianMaterial::Load(Bytestream& stream)
{
    stream >> Kd;
}