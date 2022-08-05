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

Sample LambertianMaterial::GetSample(const IntersectionInfo& info, bool adjoint) const
{
    Ray out;

    double r1 = rnd.GetDouble(0, 1.0);
    double r2 = rnd.GetDouble(0, 1.0);

    Vector3d N_g = info.GetGeometricNormal();
    Vector3d N_s = info.GetNormal();
    
    const Vector3d& w_i = -info.GetDirection();
    
    if(w_i*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    Vector3d N = adjoint ? N_g : N_s;
    Vector3d adjN = adjoint ? N_s : N_g;
    Vector3d dir = SampleHemisphereCos(r1, r2, N);

    const Vector3d& w_o = dir;
    out.origin = info.GetPosition();
    out.direction = dir;

    auto pdf = dir*N/F_PI;
    auto rpdf = w_i*adjN/F_PI;

    if(rpdf < 0)
        rpdf = 0;

    if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
        return Sample(Color(0, 0, 0), out, pdf, rpdf, false, 1);

    auto color = adjoint ? Kd*abs(w_i*N_s)/abs(w_i*N_g) : Kd;
    return Sample(color, out, pdf, rpdf, false, 1);
}

Color LambertianMaterial::BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const
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

Light* LambertianMaterial::GetLight() const
{
    return light;
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
        transform(a.begin(), a.end(), a.begin(), [](char a) { return tolower((int)a); });
        if(a == "kd")
            ss2 >> Kd.r >> Kd.g >> Kd.b;
    }
}

double LambertianMaterial::PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const
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