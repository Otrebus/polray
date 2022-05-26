#define NOMINMAX
#include <sstream>
#include "Bytestream.h"
#include "CookTorrance.h"

using namespace std;

CookTorrance::CookTorrance()
{
    Ks = Color(0, 0, 0);
    ior = 1.5;
    normalmap = 0;
    alpha = 0;
#ifdef DETERMINISTIC
    rnd.Seed(0);
#else
    rnd.Seed(GetTickCount() + int(this));
#endif
}

template<typename T> T sq(T x) {
    return x*x;
}

CookTorrance::~CookTorrance()
{
}

double G_p(Vector3d h, Vector3d w, double alpha) {
    double a = (h*w)/(alpha*std::sqrt(1-(h*w)*(h*w)));
    double l = (-1.0 + std::sqrt(1.0+1.0/(a*a)))/2.0;
    return 1.0/(1.0+l);
}

double D_p(double alpha, double cosn) {
    return sq(alpha)/(M_PI*sq((sq(alpha)-1)*cosn*cosn+1));
}

Sample CookTorrance::GetSample(const IntersectionInfo& info, bool adjoint) const
{
    //float df = Kd.GetMax();
    //float sp = Ks.GetMax();

    Vector3d N_s = info.GetNormal();
    Vector3d N_g = info.GetGeometricNormal();
    Vector3d in = -info.GetDirection();

    if(in*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    auto N = N_s;

    Vector3d right, forward;
    MakeBasis(N_s, right, forward);

    float r1 = rnd.GetDouble(0.0f, 2*F_PI);
    float r2 = rnd.GetDouble(0.0f, 1);

    auto t = std::atan(alpha*std::sqrt(r2)/std::sqrt(1-r2));

    Vector3d h_o = forward*std::cos(r1)*std::sin(t) + right*std::sin(r1)*std::sin(t) + N*std::cos(t);
    auto out = Reflect(-in, h_o);
    auto pdf = D_p(alpha, h_o*N)*(h_o*N)/(4.0*(in*h_o));
    return Sample(BRDF(info, out, 1)/pdf, Ray(info.position, out), pdf, pdf, 0, 1);
}


Color CookTorrance::BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const
{
    assert(component == 1);

    //float df = Kd.GetMax();
    //float sp = Ks.GetMax();

    Vector3d N_s = info.GetNormal();
    Vector3d N_g = info.GetGeometricNormal();
    Vector3d in = -info.GetDirection();

    auto h = ((in+out)/2).Normalized();

    if(in*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    auto N = N_s;

    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0) // FIXME: redundant checks
        return 0;

    if(component == 1)
    {
        auto cosn = h*N;
        auto cosv = h*in;
        auto F_0 = sq((1-ior)/(1+ior));
        auto D = D_p(alpha, cosn);
        auto G = 1.0/(1.0 + G_p(h, in, alpha) + G_p(h, out, alpha));
        auto F = F_0 + (1-F_0)*(std::pow(1-cosv, 5));

        return Ks*D*G*F/(4*(out*N)*(in*N));
    }
}

Light* CookTorrance::GetLight() const
{
    return light;
}

void CookTorrance::ReadProperties(stringstream& ss)
{
    while(!ss.eof())
    {
        string line;
        string a;
        getline(ss, line);
        stringstream ss2(line);
        ss2 >> a;
        transform(a.begin(), a.end(), a.begin(), tolower);

        if(a == "ks")
            ss2 >> Ks.r >> Ks.g >> Ks.b;
        else if(a == "alpha")
            ss2 >> alpha;
    }
}

double CookTorrance::PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const
{
    assert(component == 1);

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

    if(component == 1)
        return 0;

}

void CookTorrance::Save(Bytestream& stream) const
{
    stream << (unsigned char) ID_COOKTORRANCEMATERIAL;
    stream << Ks << alpha;
}

void CookTorrance::Load(Bytestream& stream)
{
    stream >> Ks >> alpha;
}