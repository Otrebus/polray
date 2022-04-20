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
    float alpha = 5;
    float r1 = rnd.GetFloat(0, 2*M_PI);
    float r2 = acos(pow(rnd.GetFloat(0.f, 1.f), 1/(alpha+1)));

    Vector3d N_g = info.GetGeometricNormal();
    Vector3d N_s = info.GetNormal();
    
    const Vector3d& w_i = -info.GetDirection();
    
    if(w_i*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    Vector3d N = adjoint ? N_g : N_s;
    Vector3d adjN = adjoint ? N_s : N_g;;

    Vector3d right, forward;
    Vector3d up = adjoint ? N_g : N_s;

    MakeBasis(up, right, forward);
    Vector3d base = forward*cos(r1) + right*sin(r1);

    auto out = Ray(info.GetPosition(), up + base*tanf(r2));
    out.direction.Normalize();

    Vector3d dir = out.direction;
    const Vector3d& w_o = dir;

    auto pdf = dir*up > 0 ? pow(dir*up, alpha)*float(alpha + 1)/(2*F_PI) : 0;
    auto rpdf = w_i*up > 0 ? pow(w_i*up, alpha)*float(alpha + 1)/(2*F_PI) : 0;

    if(pdf < 0)
        pdf = 0;
    if(rpdf < 0)
        rpdf = 0;

    if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
        return Sample(Color(0, 0, 0), out, pdf, rpdf, false);

    auto color = adjoint ? (dir*up)*BRDF(info, dir)*abs(w_i*N_s)/abs(w_i*N_g)/pdf : (dir*up)*BRDF(info, dir)/pdf;
    return Sample(color, out, pdf, rpdf, false);
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
        transform(a.begin(), a.end(), a.begin(), tolower);
        if(a == "kd")
            ss2 >> Kd.r >> Kd.g >> Kd.b;
    }
}

float LambertianMaterial::PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint) const
{
    //Vector3d N_s = info.GetNormal();
    //Vector3d N_g = info.GetGeometricNormal();

    //const Vector3d& w_i = -info.GetDirection();
    //const Vector3d& w_o = out;

    //if(w_i*N_g < 0)
    //    N_g = -N_g;

    //if(N_g*N_s < 0)
    //    N_s = -N_s;

    //if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
    //    return 0;

    //return (adjoint ? N_g : N_s)*w_o/F_PI;

    Vector3d N_s = info.GetNormal();
    Vector3d N_g = info.GetGeometricNormal();
    const Vector3d& in = -info.GetDirection();

    if(in*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0)
        return 0;

    Vector3d normal = adjoint ? N_g : N_s;

    float alpha = 5.0;

    Vector3d up = N_s;
    return out*up > 0 ? pow(out*up, alpha)*float(alpha + 1)/(2*F_PI) : 0;
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