#include <sstream>
#include "Bytestream.h"
#include "PhongMaterial.h"

using namespace std;

PhongMaterial::PhongMaterial()
{
    Kd = Color(1, 1, 1);
    Ks = Color(0, 0, 0);
    Ka = Color(0, 0, 0);
    normalmap = 0;
    alpha = 0;
#ifdef DETERMINISTIC
    rnd.Seed(0);
#else
    rnd.Seed(GetTickCount() + int(this));
#endif
}

PhongMaterial::~PhongMaterial()
{
}

Sample PhongMaterial::GetSample(const IntersectionInfo& info, bool adjoint) const
{
float df = Kd.GetMax();
    float sp = Ks.GetMax();
    
    float r = rnd.GetDouble(0, df + sp);
    if(r <= df) // Diffuse bounce
    {
        int component = 1;

        float r1 = rnd.GetDouble(0, 1.0);
        float r2 = rnd.GetDouble(0, 1.0f);

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

        double pdf = w_o*N/F_PI;
        double rpdf = w_i*adjN/F_PI;
        if(rpdf < 0)
            rpdf = 0;
        if(pdf < 0)
            pdf = 0;

        Ray out;
        out.origin = info.GetPosition();
        out.direction = dir;

        if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
            return Sample(Color(0, 0, 0), out, 0, 0, false, 1);

        Color ret = adjoint ? Kd*abs(w_i*N_s)/abs(w_i*N_g) : Kd;
        return Sample(ret/(df/(df+sp)), out, pdf, rpdf, false, 1);
    }
    else // Specular bounce
    {
        int component = 2;
        float r1 = rnd.GetDouble(0.0f, 2*F_PI);
        float r2 = acos(pow(rnd.GetDouble(0, 1), 1/(alpha+1)));

        Vector3d N_g = info.GetGeometricNormal();
        Vector3d N_s = info.GetNormal();
        Vector3d w_i = -info.GetDirection();

        if(w_i*N_g < 0)
            N_g = -N_g;

        if(N_g*N_s < 0)
            N_s = -N_s;

        Vector3d N = adjoint ? N_g : N_s;
        Vector3d adjN = adjoint ? N_s : N_g;

        Vector3d up = Reflect(info.GetDirection(), N_s);
        Vector3d right, forward;

        MakeBasis(up, right, forward);
        Vector3d base = forward*cos(r1) + right*sin(r1);

        auto out = Ray(info.GetPosition(), right*std::sin(r1)*sin(r2) + forward*std::cos(r1)*sin(r2) + up*std::cos(r2));
        out.direction.Normalize();
        Vector3d w_o = out.direction;
                double pdf, rpdf;
        pdf = rpdf = pow(out.direction*up, alpha)*(alpha + 1)/(2*F_PI);
        if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
        {
            return Sample(Color(0, 0, 0), out, 0, 0, false, 2);
        }

        // FIXME these should probably be equal

        //rpdf = pow(-info.GetDirection()*Reflect(-out.direction, N_s), alpha)*(alpha+1)/(2*F_PI);
        Color mod = abs(out.direction*N)*Ks*float(alpha + 2)/float(alpha + 1);

        return Sample((adjoint ? abs((N_s*w_i)/(N_g*w_i)) : 1)*mod/(sp/(df+sp)), out, pdf, rpdf, false, 2);
    }
}

Color PhongMaterial::BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const
{
    assert(component == 1 || component == 2);

    float df = Kd.GetMax();
    float sp = Ks.GetMax();

    Vector3d N_s = info.GetNormal();
    Vector3d N_g = info.GetGeometricNormal();
    const Vector3d& in = -info.GetDirection();

    if(in*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0) // FIXME: redundant checks
        return 0;

    if(component == 1)
    {
        //pdf = out*N_s/F_PI;
        return Kd/F_PI/(df/(df+sp));
    }
    else
    {
        Vector3d reflection = Reflect(info.GetDirection(), N_s);

        if(reflection*out < 0)
            return Color(0, 0, 0);

        return Ks*((alpha + 2)/(2*F_PI))*pow(out*reflection, alpha)/(sp/(df+sp));
    }
}

Light* PhongMaterial::GetLight() const
{
    return light;
}

void PhongMaterial::ReadProperties(stringstream& ss)
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
        else if(a == "ks")
            ss2 >> Ks.r >> Ks.g >> Ks.b;
        else if(a == "alpha")
            ss2 >> alpha;
    }
}

double PhongMaterial::PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const
{
    assert(component == 1 || component == 2);

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
        return out*normal/F_PI;
    else
    {
        Vector3d up = Reflect(info.GetDirection(), N_s);
        float asdf = out*up;
        return out*up > 0 ? pow(out*up, alpha)*float(alpha + 1)/(2*F_PI) : 0;
    }
}

void PhongMaterial::Save(Bytestream& stream) const
{
    stream << (unsigned char) 101;
    stream << Kd << Ks << alpha;
}

void PhongMaterial::Load(Bytestream& stream)
{
    stream >> Kd >> Ks >> alpha;
}

//
//#include <sstream>
//#include "Bytestream.h"
//#include "PhongMaterial.h"
//
//using namespace std;
//
//PhongMaterial::PhongMaterial()
//{
//    Kd = Color(1, 1, 1);
//    Ks = Color(0, 0, 0);
//    Ka = Color(0, 0, 0);
//    normalmap = 0;
//    alpha = 0;
//#ifdef DETERMINISTIC
//    rnd.Seed(0);
//#else
//    rnd.Seed(GetTickCount() + int(this));
//#endif
//}
//
//double arvo_p(Vector3d N, Vector3d w_i, int n) {
//    auto c = std::abs(N*w_i);
//    auto s = std::sqrt(1-c*c);
//    int k = 0;
//    double T_k, A;
//    if(n%2 == 0) {
//        A = M_PI - std::acos(c);
//        k = 1;
//        T_k = s;
//    } else {
//        A = M_PI/2;
//        k = 0;
//        T_k = M_PI/2;
//    }
//    double t = 0;
//    while(k <= n-1) {
//        t += T_k;
//        k += 2;
//        T_k = s*s*(double(k-1)/double(k))*T_k;
//    }
//    return 2*(A + c*t)/(n+1);
//}
//
//PhongMaterial::~PhongMaterial()
//{
//}
//
//Sample PhongMaterial::GetSample(const IntersectionInfo& info, bool adjoint) const
//{
//    assert(std::abs(info.GetNormal().GetLength() - 1) < 0.001);
//
//    float df = Kd.GetMax();
//    float sp = Ks.GetMax();
//    
//    float r = rnd.GetDouble(0, df + sp);
//    if(r <= df) // Diffuse bounce
//    {
//        int component = 1;
//
//        float r1 = rnd.GetDouble(0, 1);
//        float r2 = rnd.GetDouble(0, 1);
//
//        Vector3d N_g = info.GetGeometricNormal();
//        Vector3d N_s = info.GetNormal();
//    
//        const Vector3d& w_i = -info.GetDirection();
//    
//        if(w_i*N_g < 0)
//            N_g = -N_g;
//
//        if(N_g*N_s < 0)
//            N_s = -N_s;
//
//        Vector3d N = adjoint ? N_g : N_s;
//        Vector3d adjN = adjoint ? N_s : N_g;
//
//        Vector3d dir;
//        SampleHemisphereCos(r1, r2, N, dir);
//
//        const Vector3d& w_o = dir;
//
//        double pdf = w_o*N/F_PI;
//        double rpdf = w_i*adjN/F_PI;
//        if(rpdf < 0)
//            rpdf = 0;
//        if(pdf < 0)
//            pdf = 0;
//
//        Ray out;
//        out.origin = info.GetPosition();
//        out.direction = dir;
//
//        if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
//            return Sample(Color(0, 0, 0), out, 0, 0, false, 1);
//
//        Color ret = adjoint ? Kd*abs(w_i*N_s)/abs(w_i*N_g) : Kd;
//        return Sample(ret/(df/(df+sp)), out, pdf, rpdf, false, 1);
//    }
//    else // Specular bounce
//    {
//        int component = 2;
//        
//        Vector3d N_g = info.GetGeometricNormal();
//        Vector3d N_s = info.GetNormal();
//        Vector3d w_i = -info.GetDirection();
//
//        if(w_i*N_g < 0)
//            N_g = -N_g;
//
//        if(N_g*N_s < 0)
//            N_s = -N_s;
//
//        Vector3d N = adjoint ? N_g : N_s;
//        Vector3d adjN = adjoint ? N_s : N_g;
//
//        Vector3d up = Reflect(info.GetDirection(), N);
//        Vector3d right, forward;
//        MakeBasis(up, right, forward);
//
//        Vector3d w_o = -N_s;
//
//        while(w_o*N < 0) {
//            float r1 = rnd.GetDouble(0.0f, 2*F_PI);
//            float r2 = acos(pow(rnd.GetDouble(0, 1), 1/(alpha+1)));
//
//            w_o = right*std::sin(r1)*sin(r2) + forward*std::cos(r1)*sin(r2) + up*std::cos(r2);
//        }
//
//        auto out = Ray(info.GetPosition(), w_o);
//        auto up2 = Reflect(-out.direction, N);
//
//        double pdf, rpdf;
//        pdf = pow(w_o*up, alpha)/arvo_p(N, w_i, alpha+0.01);
//        rpdf = pow(w_i*up2, alpha)/arvo_p(N, w_o, alpha+0.01);
//
//        Color mod = arvo_p(N, w_i, alpha+0.01)*Ks*abs(out.direction*N)*float(alpha + 2)/(2*F_PI);
//
//        return Sample((adjoint ? abs((N_s*w_i)/(N_g*w_i)) : 1)*mod/(sp/(df+sp)), out, pdf, rpdf, false, 2);
//    }
//}
//
//Color PhongMaterial::BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const
//{
//    assert(std::abs(info.GetNormal().GetLength() - 1) < 0.001);
//    assert(std::abs(out.GetLength() - 1) < 0.001);
//    assert(component == 1 || component == 2);
//
//    float df = Kd.GetMax();
//    float sp = Ks.GetMax();
//
//    Vector3d N_s = info.GetNormal();
//    Vector3d N_g = info.GetGeometricNormal();
//    const Vector3d& in = -info.GetDirection();
//
//    if(in*N_g < 0)
//        N_g = -N_g;
//
//    if(N_g*N_s < 0)
//        N_s = -N_s;
//
//    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0) // FIXME: redundant checks
//        return 0;
//
//    if(component == 1)
//    {
//        //pdf = out*N_s/F_PI;
//        return Kd/F_PI/(df/(df+sp));
//    }
//    else
//    {
//        Vector3d reflection = Reflect(info.GetDirection(), N_s);
//
//        if(reflection*out < 0)
//            return Color(0, 0, 0);
//
//        return Ks*float(alpha + 2)/(2*F_PI)*pow(out*reflection, alpha)/(sp/(df+sp));
//    }
//}
//
//Light* PhongMaterial::GetLight() const
//{
//    return light;
//}
//
//void PhongMaterial::ReadProperties(stringstream& ss)
//{
//    while(!ss.eof())
//    {
//        string line;
//        string a;
//        getline(ss, line);
//        stringstream ss2(line);
//        ss2 >> a;
//        transform(a.begin(), a.end(), a.begin(), tolower);
//        if(a == "kd")
//            ss2 >> Kd.r >> Kd.g >> Kd.b;
//        else if(a == "ks")
//            ss2 >> Ks.r >> Ks.g >> Ks.b;
//        else if(a == "alpha")
//            ss2 >> alpha;
//    }
//}
//
//double PhongMaterial::PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const
//{
//    assert(component == 1 || component == 2);
//
//    Vector3d N_s = info.GetNormal();
//    Vector3d N_g = info.GetGeometricNormal();
//    const Vector3d& in = -info.GetDirection();
//
//    if(in*N_g < 0)
//        N_g = -N_g;
//
//    if(N_g*N_s < 0)
//        N_s = -N_s;
//
//    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0)
//        return 0;
//
//    Vector3d N = adjoint ? N_g : N_s;
//
//    if(component == 1)
//        return out*N/F_PI;
//    else
//    {
//        Vector3d up = Reflect(-in, N);
//        return out*up > 0 ? pow(out*up, alpha)/arvo_p(N, out, alpha+0.001) : 0;
//    }
//}
//
//void PhongMaterial::Save(Bytestream& stream) const
//{
//    stream << (unsigned char) 101;
//    stream << Kd << Ks << alpha;
//}
//
//void PhongMaterial::Load(Bytestream& stream)
//{
//    stream >> Kd >> Ks >> alpha;
//}
//




//
//
//
//
//
//
//
//#include <sstream>
//#include "Bytestream.h"
//#include "PhongMaterial.h"
//
//using namespace std;
//
//PhongMaterial::PhongMaterial()
//{
//    Kd = Color(1, 1, 1);
//    Ks = Color(0, 0, 0);
//    Ka = Color(0, 0, 0);
//    normalmap = 0;
//    alpha = 0;
//#ifdef DETERMINISTIC
//    rnd.Seed(0);
//#else
//    rnd.Seed(GetTickCount() + int(this));
//#endif
//}
//
//double arvo_m(Vector3d N, Vector3d w_i, int n) {
//    double c = std::abs(N*w_i);
//    double s = std::sqrt(1-c*c);
//    double k = 0;
//    double T_k, A;
//    if(n%2 == 0) {
//        A = M_PI/2;
//        k = 0;
//        T_k = M_PI/2;
//    } else {
//        A = M_PI - std::acos(c);
//        k = 1;
//        T_k = s;
//    }
//    double t = 0;
//    while(k <= n-2) {
//        t += T_k;
//        k += 2;
//        T_k = s*s*((k-1)/k)*T_k;
//    }
//    return 2*(T_k + A*c + c*c*t)/(n+2);
//}
//
//double arvo_p(Vector3d N, Vector3d w_i, int n) {
//    auto c = std::abs(N*w_i);
//    auto s = std::sqrt(1-c*c);
//    int k = 0;
//    double T_k, A;
//    if(n%2 == 0) {
//        A = M_PI - std::acos(c);
//        k = 1;
//        T_k = s;
//    } else {
//        A = M_PI/2;
//        k = 0;
//        T_k = M_PI/2;
//    }
//    double t = 0;
//    while(k <= n-1) {
//        t += T_k;
//        k += 2;
//        T_k = s*s*(double(k-1)/double(k))*T_k;
//    }
//    return 2*(A + c*t)/(n+1);
//}
//
//PhongMaterial::~PhongMaterial()
//{
//}
//
//Sample PhongMaterial::GetSample(const IntersectionInfo& info, bool adjoint) const
//{
//    float df = Kd.GetMax();
//    float sp = Ks.GetMax();
//    
//    float r = rnd.GetDouble(0, df + sp);
//    if(r <= df) // Diffuse bounce
//    {
//        int component = 1;
//
//        float r1 = rnd.GetDouble(0, 1);
//        float r2 = rnd.GetDouble(0, 1);
//
//        Vector3d N_g = info.GetGeometricNormal();
//        Vector3d N_s = info.GetNormal();
//    
//        const Vector3d& w_i = -info.GetDirection();
//    
//        if(w_i*N_g < 0)
//            N_g = -N_g;
//
//        if(N_g*N_s < 0)
//            N_s = -N_s;
//
//        Vector3d N = adjoint ? N_g : N_s;
//        Vector3d adjN = adjoint ? N_s : N_g;
//
//        Vector3d dir;
//        SampleHemisphereCos(r1, r2, N, dir);
//
//        const Vector3d& w_o = dir;
//
//        double pdf = w_o*N/F_PI;
//        double rpdf = w_i*adjN/F_PI;
//        if(rpdf < 0)
//            rpdf = 0;
//        if(pdf < 0)
//            pdf = 0;
//
//        Ray out;
//        out.origin = info.GetPosition();
//        out.direction = dir;
//
//        if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
//            return Sample(Color(0, 0, 0), out, 0, 0, false, 1);
//
//        Color ret = adjoint ? Kd*abs(w_i*N_s)/abs(w_i*N_g) : Kd;
//        return Sample(ret/(df/(df+sp)), out, pdf, rpdf, false, 1);
//    }
//    else // Specular bounce
//    {
//        int component = 2;
//        float r1 = rnd.GetDouble(0.0f, 2*F_PI);
//        float r2 = rnd.GetDouble(0, 1);
//
//        Vector3d N_g = info.GetGeometricNormal();
//        Vector3d N_s = info.GetNormal();
//        Vector3d w_i = -info.GetDirection();
//
//        if(w_i*N_g < 0)
//            N_g = -N_g;
//
//        if(N_g*N_s < 0)
//            N_s = -N_s;
//
//        Vector3d N = adjoint ? N_g : N_s;
//        Vector3d adjN = adjoint ? N_s : N_g;
//
//        Vector3d up = Reflect(info.GetDirection(), N_s);
//        
//        Vector3d right, forward;
//
//        MakeBasis(N_s, right, forward);
//
//        auto out = Ray(info.GetPosition(), right*sqrt(1-r2*r2)*cos(r1) + forward*sqrt(1-r2*r2)*sin(r1) + N_s*r2);
//
//        Vector3d w_o = out.direction;
//
//        double pdf, rpdf;
//        pdf = 1/(2*M_PI);
//        rpdf = 1/(2*M_PI);
//
//        if(out.direction*up < 0 || w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
//        {
//            return Sample(Color(0, 0, 0), out, pdf, rpdf, false, 2);
//        } 
//
//        Color mod = Ks*abs(out.direction*N)*float(alpha + 2)*pow(up*out.direction, alpha);
//
//        return Sample((adjoint ? abs((N_s*w_i)/(N_g*w_i)) : 1)*mod/(sp/(df+sp)), out, pdf, rpdf, false, 2);
//    }
//}
//
//Color PhongMaterial::BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const
//{
//    assert(component == 1 || component == 2);
//
//    float df = Kd.GetMax();
//    float sp = Ks.GetMax();
//
//    Vector3d N_s = info.GetNormal();
//    Vector3d N_g = info.GetGeometricNormal();
//    const Vector3d& in = -info.GetDirection();
//
//    if(in*N_g < 0)
//        N_g = -N_g;
//
//    if(N_g*N_s < 0)
//        N_s = -N_s;
//
//    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0) // FIXME: redundant checks
//        return 0;
//
//    if(component == 1)
//    {
//        //pdf = out*N_s/F_PI;
//        return Kd/F_PI/(df/(df+sp));
//    }
//    else
//    {
//        Vector3d reflection = Reflect(info.GetDirection(), N_s);
//
//        if(reflection*out < 0)
//            return Color(0, 0, 0);
//
//        return Ks*float(alpha + 2)/(2*F_PI)*pow(out*reflection, alpha)/(sp/(df+sp));
//    }
//}
//
//Light* PhongMaterial::GetLight() const
//{
//    return light;
//}
//
//void PhongMaterial::ReadProperties(stringstream& ss)
//{
//    while(!ss.eof())
//    {
//        string line;
//        string a;
//        getline(ss, line);
//        stringstream ss2(line);
//        ss2 >> a;
//        transform(a.begin(), a.end(), a.begin(), tolower);
//        if(a == "kd")
//            ss2 >> Kd.r >> Kd.g >> Kd.b;
//        else if(a == "ks")
//            ss2 >> Ks.r >> Ks.g >> Ks.b;
//        else if(a == "alpha")
//            ss2 >> alpha;
//    }
//}
//
//double PhongMaterial::PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const
//{
//    assert(component == 1 || component == 2);
//
//    Vector3d N_s = info.GetNormal();
//    Vector3d N_g = info.GetGeometricNormal();
//    const Vector3d& in = -info.GetDirection();
//
//    if(in*N_g < 0)
//        N_g = -N_g;
//
//    if(N_g*N_s < 0)
//        N_s = -N_s;
//
//    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0)
//        return 0;
//
//    Vector3d normal = adjoint ? N_g : N_s;
//
//    if(component == 1)
//        return out*normal/F_PI;
//    else
//    {
//        Vector3d up = Reflect(info.GetDirection(), N_s);
//        float asdf = out*up;
//        return 1/(2*M_PI);
//    }
//}
//
//void PhongMaterial::Save(Bytestream& stream) const
//{
//    stream << (unsigned char) 101;
//    stream << Kd << Ks << alpha;
//}
//
//void PhongMaterial::Load(Bytestream& stream)
//{
//    stream >> Kd >> Ks >> alpha;
//}
