/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file PhongMaterial.cpp
 * 
 * Implementation of the PhongMaterial class.
 */

#include <sstream>
#include "Bytestream.h"
#include "PhongMaterial.h"
#include "IntersectionInfo.h"
#include "Sample.h"
#include "Utils.h"
#include "GeometricRoutines.h"

/**
 * Constructor.
 */
PhongMaterial::PhongMaterial()
{
    Kd = Color(1, 1, 1);
    Ks = Color(0, 0, 0);
    Ka = Color(0, 0, 0);
    alpha = 0;
}

/**
 * Destructor.
 */
PhongMaterial::~PhongMaterial()
{
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
Sample PhongMaterial::GetSample(const IntersectionInfo& info, Randomizer& rnd, bool adjoint) const
{
    auto df = Kd.GetLuma();
    auto sp = Ks.GetLuma();
    
    auto r = rnd.GetDouble(0, df + sp);
    if(r <= df) // Diffuse bounce
    {
        auto r1 = rnd.GetDouble(0, 1.0);
        auto r2 = rnd.GetDouble(0, 1.0);

        Vector3d N_g = info.geometricnormal;
        Vector3d N_s = info.normal;
    
        const Vector3d& w_i = -info.direction;
    
        if(w_i*N_g < 0)
            N_g = -N_g;

        if(N_g*N_s < 0)
            N_s = -N_s;

        Vector3d N = adjoint ? N_g : N_s;
        Vector3d adjN = adjoint ? N_s : N_g;

        Vector3d dir = SampleHemisphereCos(r1, r2, N);

        const Vector3d& w_o = dir;

        double pdf = w_o*N/pi;
        double rpdf = w_i*adjN/pi;
        if(rpdf < 0)
            rpdf = 0;
        if(pdf < 0)
            pdf = 0;

        Ray out;
        out.origin = info.position;
        out.direction = dir;

        if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
            return Sample(Color(0, 0, 0), out, 0, 0, false, 1);

        Color ret = adjoint ? Kd*abs(w_i*N_s)/abs(w_i*N_g) : Kd;
        return Sample(ret/(df/(df+sp)), out, pdf, rpdf, false, 1);
    }
    else // Specular bounce
    {
        auto r1 = rnd.GetDouble(0.0, 2*pi);
        auto r2 = acos(pow(rnd.GetDouble(0, 1), 1/(alpha+1)));

        Vector3d N_g = info.geometricnormal;
        Vector3d N_s = info.normal;
        Vector3d w_i = -info.direction;

        if(w_i*N_g < 0)
            N_g = -N_g;

        if(N_g*N_s < 0)
            N_s = -N_s;

        Vector3d N = adjoint ? N_g : N_s;
        Vector3d adjN = adjoint ? N_s : N_g;

        Vector3d up = Reflect(info.direction, N_s);

        auto [right, forward] = MakeBasis(up);
        Vector3d base = forward*cos(r1) + right*sin(r1);

        auto out = Ray(info.position, right*std::sin(r1)*sin(r2) + forward*std::cos(r1)*sin(r2) + up*std::cos(r2));
        out.direction.Normalize();
        Vector3d w_o = out.direction;

        double pdf, rpdf;
        pdf = rpdf = pow(out.direction*up, alpha)*(alpha + 1)/(2*pi);
        if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
            return Sample(Color(0, 0, 0), out, 0, 0, false, 2);

        Color mod = abs(out.direction*N)*Ks*float(alpha + 2)/float(alpha + 1);
        return Sample((adjoint ? abs((N_s*w_i)/(N_g*w_i)) : 1)*mod/(sp/(df+sp)), out, pdf, rpdf, false, 2);
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
Color PhongMaterial::BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const
{
    assert(component == 1 || component == 2);

    auto df = Kd.GetLuma();
    auto sp = Ks.GetLuma();

    Vector3d N_s = info.normal;
    Vector3d N_g = info.geometricnormal;
    const Vector3d& in = -info.direction;

    if(in*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0) // FIXME: redundant checks
        return Color::Black;

    if(component == 1)
    {
        //pdf = out*N_s/pi;
        return Kd/pi/(df/(df+sp));
    }
    else
    {
        Vector3d reflection = Reflect(info.direction, N_s);

        if(reflection*out < 0)
            return Color(0, 0, 0);

        return Ks*((alpha + 2)/(2*pi))*pow(out*reflection, alpha)/(sp/(df+sp));
    }
}

/**
 * Returns the associated light of this material, if any.
 * 
 * @returns The light.
 */
Light* PhongMaterial::GetLight() const
{
    return light;
}

/**
 * Reads the properties of the material from a stringstream (from a .mtl file).
 * 
 * @param ss The stringstream.
 */
void PhongMaterial::ReadProperties(std::stringstream& ss)
{
    while(!ss.eof())
    {
        std::string line, a;
        getline(ss, line);
        std::stringstream ss2(line);
        ss2 >> a;
        std::transform(a.begin(), a.end(), a.begin(), [](char a) { return (char) tolower(a); });
        if(a == "kd")
            ss2 >> Kd.r >> Kd.g >> Kd.b;
        else if(a == "ks")
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
double PhongMaterial::PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const
{
    assert(component == 1 || component == 2);

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
        return out*normal/pi;
    else
    {
        Vector3d up = Reflect(info.direction, N_s);
        return out*up > 0 ? pow(out*up, alpha)*float(alpha + 1)/(2*pi) : 0;
    }
}

/**
 * Saves the material to a bytestream.
 * 
 * @param stream The stream to serialize to.
 */
void PhongMaterial::Save(Bytestream& stream) const
{
    stream << (unsigned char) 101;
    stream << Kd << Ks << alpha;
}


/**
 * Loads the material from a bytestream.
 * 
 * @param stream The bytestream to deserialize from.
 */
void PhongMaterial::Load(Bytestream& stream)
{
    stream >> Kd >> Ks >> alpha;
}
