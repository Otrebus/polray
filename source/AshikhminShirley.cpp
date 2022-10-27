/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file AhikhminShirley.cpp
 * 
 * Implementation of the AshikhminShirley class.
 */

#include "Bytestream.h"
#include "AshikhminShirley.h"
#include "Utils.h"
#include "Sample.h"
#include "IntersectionInfo.h"
#include "GeometricRoutines.h"

/**
 * Constructor.
 */
AshikhminShirley::AshikhminShirley()
{
    Rs = Color(1, 1, 1);
    Rd = Color(1, 1, 1);
    n = 1;
}

/**
 * Destructor.
 */
AshikhminShirley::~AshikhminShirley()
{
}

/**
 * Reads the properties of the material from a stringstream (from a .mtl file).
 * 
 * @param ss The stringstream.
 */
void AshikhminShirley::ReadProperties(std::stringstream& ss)
{
    while(!ss.eof())
    {
        std::string line;
        std::string a;
        getline(ss, line);
        std::stringstream ss2(line);
        ss2 >> a;
        transform(a.begin(), a.end(), a.begin(), [](char a) { return (char) tolower(a); });
        if(a == "rd")
            ss2 >> Rd.r >> Rd.g >> Rd.b;
        else if(a == "rs")
            ss2 >> Rs.r >> Rs.g >> Rs.b;
        else if(a == "n")
            ss2 >> n;
    }
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
Sample AshikhminShirley::GetSample(const IntersectionInfo& info, Randomizer& rnd, bool adjoint) const
{
    double df = Rd.GetLuma();
    double sp = Rs.GetLuma();

    double r = rnd.GetDouble(0.0, df + sp);

    Vector3d N_g = info.geometricnormal;
    Vector3d N_s = info.normal;
    Vector3d w_i = -info.direction;

    if(w_i*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    Vector3d N = adjoint ? N_g : N_s;
    Vector3d adjN = adjoint ? N_s : N_g;

    Ray outRay;
    Vector3d& w_o = outRay.direction;

    if(r <= df) // Diffuse bounce
    {
        double r1 = rnd.GetDouble(0, 1.0);
        double r2 = rnd.GetDouble(0, 1.0);

        auto dir = SampleHemisphereCos(r1, r2, N);

        outRay.origin = info.position;
        outRay.direction = dir;

        if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
            return Sample(Color(0, 0, 0), outRay, 0, 0, false, 1);

        // TODO: the below is just the brdf, multiplied by pi, simplify
        Color mod = (28.0/23.0)*Rd*(Color::Identity - Rs)*(1-pow(1-abs(N_s*w_i)/2.0, 5.0))*(1-pow(1-(N_s*w_o)/2.0, 5.0));

        auto color = (adjoint ? abs(w_i*N_s)/abs(w_i*N_g) : 1.0)*mod/(df/(df+sp));
        double pdf = std::max(0.0, w_o*N/pi);
        double rpdf = std::max(0.0, w_i*adjN/pi);
        return Sample(color, outRay, pdf, rpdf, false, 1);
    }
    else // Specular bounce
    {
        double r1 = rnd.GetDouble(0.0, 2*pi);
        double r2 = acos(pow(rnd.GetDouble(0, 1.0), 1/double(n+1)));

        auto [right, forward] = MakeBasis(N_s);
        Vector3d hv = sin(r2)*(forward*cos(r1) + right*sin(r1)) + cos(r2)*N_s;

        outRay = Ray(info.position, -w_i + 2*(w_i*hv)*hv);
        outRay.direction.Normalize();

        if(w_i*N_s < 0 || w_o*N_s < 0 || w_o*N_g < 0 || w_i*N_g < 0) 
            return Sample(Color(0, 0, 0), outRay, 0, 0, false, 2);

        Color fresnel = Rs + (Color::Identity - Rs)*(pow(1-w_o*hv, 5.0));
        Color mod = abs(N*w_o)*fresnel/(max(N_s*w_i, N_s*w_o));
        auto color = (adjoint ? abs(w_i*N_s)/abs(w_i*N_g) : 1.0f)*mod/(sp/(df+sp));
        double pdf = pow(N_s*hv, n)*(n + 1)/((w_i*hv)*8*pi);
        double rpdf = pdf;
        return Sample(color, outRay, pdf, rpdf, false, 2);
    }
}

/**
 * Returns the value of the brdf in a certain ingoing/outgoing direction for a brdf component.
 * 
 * @param info The intersection info.
 * @param out The outgoing direction.
 * @param component The component of the brdf.
 * @returns The value of the brdf.
 */
Color AshikhminShirley::BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const
{
    double df = Rd.GetLuma();
    double sp = Rs.GetLuma();

    Vector3d N_s = info.normal;
    Vector3d N_g = info.geometricnormal;
    Vector3d in = -info.direction;

    if(in*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0) // FIXME: redundant checks
        return Color(0);

    Vector3d wi = -info.direction;
    Vector3d h = (wi + out);
    h.Normalize();

    if(component == 1)
        return Rd*(28.0/(23.0*pi))*(Color::Identity-Rs)*(1-pow(1-abs(N_s*out)/2, 5.0))*(1-pow(1-abs(N_s*wi)/2, 5.0))/(df/(df+sp));
    else
        return (Rs + (Color::Identity - Rs)*pow(1-out*h, 5.0))*pow(N_s*h, double(n))*(double(n + 1)/(8*pi))/( (h*out)*max(N_s*wi, N_s*out) )/(sp/(df+sp));
}

Light* AshikhminShirley::GetLight() const
{
    return nullptr;
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
double AshikhminShirley::PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const
{
    Vector3d N_s = info.normal;
    Vector3d N_g = info.geometricnormal;

    Vector3d in = -info.direction;

    Vector3d hv = (in + out);
    hv.Normalize();

    if(in*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0)
        return 0;

    Vector3d normal = adjoint ? N_g : N_s;

    if(component == 1)
        return out*normal/pi;
    else
        return pow(normal*hv, n)*(n + 1)/((in*hv)*8*pi);
}

/**
 * Saves the material to a bytestream.
 * 
 * @param stream The stream to serialize to.
 */
void AshikhminShirley::Save(Bytestream& stream) const
{
    stream << (unsigned char) ID_ASHIKHMINSHIRLEY;
    stream << Rd << Rs << n;
}

/**
 * Loads the material from a bytestream.
 * 
 * @param stream The bytestream to deserialize from.
 */
void AshikhminShirley::Load(Bytestream& stream)
{
    stream >> Rd >> Rs >> n;
}
