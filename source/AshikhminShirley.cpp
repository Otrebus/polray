#include "Bytestream.h"
#include "AshikhminShirley.h"

AshikhminShirley::AshikhminShirley()
{
    Rs = Color(1, 1, 1);
    Rd = Color(1, 1, 1);
    n = 1;
}

AshikhminShirley::~AshikhminShirley()
{
}

void AshikhminShirley::ReadProperties(stringstream& ss)
{
    while(!ss.eof())
    {
        string line;
        string a;
        getline(ss, line);
        stringstream ss2(line);
        ss2 >> a;
        transform(a.begin(), a.end(), a.begin(), tolower);
        if(a == "rd")
            ss2 >> Rd.r >> Rd.g >> Rd.b;
        else if(a == "rs")
            ss2 >> Rs.r >> Rs.g >> Rs.b;
        else if(a == "n")
            ss2 >> n;
    }
}

Sample AshikhminShirley::GetSample(const IntersectionInfo& info, bool adjoint) const
{
    double df = Rd.GetMax();
    double sp = Rs.GetMax();

    double r = rnd.Getdouble(0.0000f, df + sp);

    if(r <= df) // Diffuse bounce
    {
        Vector3d N_g = info.GetGeometricNormal();
        Vector3d N_s = info.GetNormal();
        Vector3d w_i = -info.GetDirection();

        double r1 = rnd.Getdouble(0, 2*F_PI);
        double r2 = rnd.Getdouble(0, 0.9999f);

        if(w_i*N_g < 0)
            N_g = -N_g;

        if(N_g*N_s < 0)
            N_s = -N_s;

        Vector3d N = adjoint ? N_g : N_s;
        Vector3d adjN = adjoint ? N_s : N_g;

        Vector3d dir;
        SampleHemisphereCos(r1, r2, N, dir);
        const Vector3d& w_o = dir;

        double pdf = w_o*N/F_PI*df/(sp+df);
        double rpdf = w_i*adjN/F_PI*df/(sp+df);
        if(rpdf < 0)
            rpdf = 0;
        if(pdf < 0)
            pdf = 0;

        Ray outRay;
        outRay.origin = info.GetPosition();
        outRay.direction = dir;

        if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
        {
            pdf = rpdf = 0;
            return Sample(Color(0, 0, 0), outRay, pdf, rpdf, false);
        }

        // TODO: the below is just the brdf, divided by F_PI, simplify
        Color mod = (28.0f/23.0f)*Rd*(Color::Identity - Rs)*(1-pow(1-abs(N_s*w_i)/2.0f, 5.0f))*(1-pow(1-(N_s*w_o)/2.0f, 5.0f));

        auto color = (adjoint ? abs(w_i*N_s)/abs(w_i*N_g) : 1.0f)*mod/(df/(df+sp));
        return Sample(color, outRay, pdf, rpdf, false);
    }
    else // Specular bounce
    {
        double r1 = rnd.Getdouble(0.0f, 2*F_PI);
        double r2 = acos(pow(rnd.Getdouble(0.0001f, 0.9999f), 1/double(n+1)));

        Vector3d N_g = info.GetGeometricNormal();
        Vector3d N_s = info.GetNormal();
        Vector3d w_i = -info.GetDirection();

        if(w_i*N_g < 0)
            N_g = -N_g;

        if(N_g*N_s < 0)
            N_s = -N_s;

        Vector3d N = adjoint ? N_g : N_s;
        Vector3d adjN = adjoint ? N_s : N_g;

        Vector3d right, forward;
        MakeBasis(N_s, right, forward);
        Vector3d hv = sin(r2)*(forward*cos(r1) + right*sin(r1)) + cos(r2)*N_s;
        Ray outRay;
        outRay = Ray(info.GetPosition(), -w_i + 2*(w_i*hv)*hv);
        outRay.direction.Normalize();
        Vector3d& w_o = outRay.direction;

        double pdf = pow(N_s*hv, n)*(n + 1)/((w_o*hv)*8*F_PI)*sp/(df+sp);
        double rpdf = pdf;
        
        if(w_i*N_s < 0 || w_o*N_s < 0 || w_o*N_g < 0 || w_i*N_g < 0) 
        {
            pdf = rpdf = 0;
            return Sample(Color(0, 0, 0), outRay, pdf, rpdf, false);
        }

        Color fresnel = Rs + (Color::Identity - Rs)*(pow(1-w_o*hv, 5.0f));
        Color mod = abs(N*w_o)*fresnel/(max(N_s*w_i, N_s*w_o));
        auto color = (adjoint ? abs(w_i*N_s)/abs(w_i*N_g) : 1.0f)*mod/(sp/(df+sp));
        return Sample(color, outRay, pdf, rpdf, false);
    }
}

Color AshikhminShirley::BRDF(const IntersectionInfo& info, const Vector3d& out) const
{
    double df = Rd.GetMax();
    double sp = Rs.GetMax();

    Vector3d N_s = info.GetNormal();
    Vector3d N_g = info.GetGeometricNormal();
    Vector3d in = -info.GetDirection();

    if(in*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0) // FIXME: redundant checks
        return 0;

    Vector3d wi = -info.GetDirection();
    Vector3d h = (wi + out);
    h.Normalize();

    auto a = Rd*(28.0f/(23.0f*F_PI))*(Color::Identity-Rs)*(1-pow(1-abs(N_s*out)/2, 5.0f))*(1-pow(1-(abs(N_s*wi))/2, 5.0f));
    auto b = (Rs + (Color::Identity - Rs)*pow(1-out*h, 5.0f))*pow(N_s*h, double(n))*(double(n + 1)/(8*F_PI))/( (h*out)*max(N_s*wi, N_s*out) );
    return a + b;
}

Light* AshikhminShirley::GetLight() const
{
    return 0;
}

double AshikhminShirley::PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint) const
{
    double df = Rd.GetMax();
    double sp = Rs.GetMax();

    Vector3d N_s = info.GetNormal();
    Vector3d N_g = info.GetGeometricNormal();
    Vector3d in = -info.GetDirection();
    Vector3d hv = (in + out);
    hv.Normalize();

    if(in*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0)
        return 0;

    Vector3d normal = adjoint ? N_g : N_s;

    return out*normal/F_PI*df/(df+sp) + pow(N_s*hv, n)*(n + 1)/((in*hv)*8*F_PI)*sp/(df+sp);
}

void AshikhminShirley::Save(Bytestream& stream) const
{
    stream << (unsigned char) ID_ASHIKHMINSHIRLEY;
    stream << Rd << Rs << n;
}
void AshikhminShirley::Load(Bytestream& stream)
{
    stream >> Rd >> Rs >> n;
}
