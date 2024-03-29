/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file CookTorrance.cpp
 * 
 * Implementation of the CookTorrance class.
 */

#define NOMINMAX
#include <sstream>
#include "Bytestream.h"
#include "CookTorrance.h"
#include "Sample.h"
#include "IntersectionInfo.h"
#include "Utils.h"
#include "GeometricRoutines.h"

template<typename T> T sq(T x)
{
    return x*x;
}

/**
 * Constructor.
 */
CookTorrance::CookTorrance()
{
    Ks = Color(0, 0, 0);
    ior = 1.5;
    alpha = 0;
}

/**
 * Destructor.
 */
CookTorrance::~CookTorrance()
{
}

double G_p(Vector3d h, Vector3d w, double alpha)
{
    double a = (h*w)/(alpha*std::sqrt(1-(h*w)*(h*w)));
    double l = (-1.0 + std::sqrt(1.0+1.0/(a*a)))/2.0;
    return 1.0/(1.0+l);
}

double D_p(double alpha, double cosn)
{
    return sq(alpha)/(pi*sq((sq(alpha)-1)*cosn*cosn+1));
}

/**
 * Samples the material given a surface intersection.
 * 
 * @param info The intersection info.
 * @param rnd The randomizer to use.
 * @param adjoint Whether we are doing path tracing or light tracing.
 * @returns The sample, containing information of its color, outgoing direction and the
 *          (r)pdf value.
 */
Sample CookTorrance::GetSample(const IntersectionInfo& info, Randomizer& rnd, bool) const
{
    //float df = Kd.GetLuma();
    //float sp = Ks.GetLuma();

    Vector3d N_s = info.normal;
    Vector3d N_g = info.geometricnormal;
    Vector3d in = -info.direction;

    if(in*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    auto N = N_s;

    auto [right, forward] = MakeBasis(N_s);

    auto r1 = rnd.GetDouble(0.0, 2*pi);
    auto r2 = rnd.GetDouble(0.0, 1.0);

    auto t = std::atan(alpha*std::sqrt(r2)/std::sqrt(1-r2));

    Vector3d h_o = forward*std::cos(r1)*std::sin(t) + right*std::sin(r1)*std::sin(t) + N*std::cos(t);
    auto out = Reflect(-in, h_o);
    auto pdf = D_p(alpha, h_o*N)*(h_o*N)/(4.0*(in*h_o));
    return Sample(BRDF(info, out, 1)/pdf, Ray(info.position, out), pdf, pdf, 0, 1);
}

/**
 * Returns the value of the brdf in a certain ingoing/outgoing direction for a brdf component.
 * 
 * @param info The intersection info.
 * @param out The outgoing direction.
 * @param component The component of the brdf.
 * @returns The value of the brdf.
 */
Color CookTorrance::BRDF(const IntersectionInfo& info, const Vector3d& out, int) const
{
    //float df = Kd.GetLuma();
    //float sp = Ks.GetLuma();

    Vector3d N_s = info.normal;
    Vector3d N_g = info.geometricnormal;
    Vector3d in = -info.direction;

    auto h = ((in+out)/2).Normalized();

    if(in*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    auto N = N_s;

    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0) // FIXME: redundant checks
        return Color::Black;

    auto cosn = h*N;
    auto cosv = h*in;
    auto F_0 = sq((1-ior)/(1+ior));
    auto D = D_p(alpha, cosn);
    auto G = 1.0/(1.0 + G_p(h, in, alpha) + G_p(h, out, alpha));
    auto F = F_0 + (1-F_0)*(std::pow(1-cosv, 5));

    return Ks*D*G*F/(4*(out*N)*(in*N));
}

/**
 * Returns the associated light of this material, if any.
 * 
 * @returns The light.
 */
Light* CookTorrance::GetLight() const
{
    return light;
}

/**
 * Reads the properties of the material from a stringstream (from a .mtl file).
 * 
 * @param ss The stringstream.
 */
void CookTorrance::ReadProperties(std::stringstream& ss)
{
    while(!ss.eof())
    {
        std::string line;
        std::string a;
        getline(ss, line);
        std::stringstream ss2(line);
        ss2 >> a;
        transform(a.begin(), a.end(), a.begin(), [](char a) { return (char) tolower(a); });

        if(a == "ks")
            ss2 >> Ks.r >> Ks.g >> Ks.b;
        else if(a == "alpha")
            ss2 >> alpha;
    }
}

/**
 * Calculates the value of the probability distribution function that we use to sample the
 * brdf for a certain surface intersection and outgoing direction.
 * 
 * @param info The information about the incoming ray into the material.
 * @param out The sampled outgoing ray.
 * @param adjoint Whether we are using path tracing or (adjoint) light tracing.
 * @param component The component of the brdf that we sampled.
 * @returns The value of the pdf in the given outgoing direction.
 */
double CookTorrance::PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const
{
    assert(component == 1);

    Vector3d N_s = info.normal;
    Vector3d N_g = info.geometricnormal;
    const Vector3d& in = -info.direction;

    if(in*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0)
        return 0;

    Vector3d normal = adjoint ? N_g : N_s;

    if(component == 1)
        return 0;
    return 1; // TODO: fix
}

/**
 * Saves the material to a bytestream.
 * 
 * @param stream The stream to serialize to.
 */
void CookTorrance::Save(Bytestream& stream) const
{
    stream << (unsigned char) ID_COOKTORRANCEMATERIAL;
    stream << Ks << alpha;
}

/**
 * Loads the material from a bytestream.
 * 
 * @param stream The bytestream to deserialize from.
 */
void CookTorrance::Load(Bytestream& stream)
{
    stream >> Ks >> alpha;
}
