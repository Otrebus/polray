/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file LambertianMaterial.cpp
 * 
 * Implementation of the LambertianMaterial class.
 */

#include <sstream>
#include "Bytestream.h"
#include "LambertianMaterial.h"
#include "GeometricRoutines.h"
#include "Sample.h"
#include "IntersectionInfo.h"
#include "Utils.h"

/**
 * Constructor.
 */
LambertianMaterial::LambertianMaterial()
{
    Kd = Color(1, 1, 1);
}

/**
 * Destructor.
 */
LambertianMaterial::~LambertianMaterial()
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
Sample LambertianMaterial::GetSample(const IntersectionInfo& info, Randomizer& rnd, bool adjoint) const
{
    Ray out;

    double r1 = rnd.GetDouble(0, 1.0);
    double r2 = rnd.GetDouble(0, 1.0);

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
    out.origin = info.position;
    out.direction = dir;

    auto pdf = dir*N/pi;
    auto rpdf = w_i*adjN/pi;

    if(rpdf < 0)
        rpdf = 0;

    if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
        return Sample(Color(0, 0, 0), out, pdf, rpdf, false, 1);

    auto color = adjoint ? Kd*abs(w_i*N_s)/abs(w_i*N_g) : Kd;
    return Sample(color, out, pdf, rpdf, false, 1);
}

/**
 * Returns the value of the brdf in a certain ingoing/outgoing direction for a brdf component.
 * 
 * @param info The intersection info.
 * @param out The outgoing direction.
 * @param component The component of the brdf.
 * @returns The value of the brdf.
 */
Color LambertianMaterial::BRDF(const IntersectionInfo& info, const Vector3d& out, int) const
{
    Vector3d N_s = info.normal;
    Vector3d N_g = info.geometricnormal;

    const Vector3d& w_i = -info.direction;
    const Vector3d& w_o = out;

    if(w_i*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
        return Color(0, 0, 0);

    return Kd/pi;
}

/**
 * Returns the associated light of this material, if any.
 * 
 * @returns The light.
 */
Light* LambertianMaterial::GetLight() const
{
    return light;
}

/**
 * Reads the properties of the material from a stringstream (from a .mtl file).
 * 
 * @param ss The stringstream.
 */
void LambertianMaterial::ReadProperties(std::stringstream& ss)
{
    while(!ss.eof())
    {
        std::string line;
        std::string a;
        getline(ss, line);
        std::stringstream ss2(line);
        ss2 >> a;
        transform(a.begin(), a.end(), a.begin(), [](char a) { return (char) tolower((int)a); });
        if(a == "kd")
            ss2 >> Kd.r >> Kd.g >> Kd.b;
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
double LambertianMaterial::PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int) const
{
    Vector3d N_s = info.normal;
    Vector3d N_g = info.geometricnormal;

    const Vector3d& w_i = -info.direction;
    const Vector3d& w_o = out;

    if(w_i*N_g < 0)
        N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
        return 0;

    return (adjoint ? N_g : N_s)*w_o/pi;
}

/**
 * Saves the material to a bytestream.
 * 
 * @param stream The stream to serialize to.
 */
void LambertianMaterial::Save(Bytestream& stream) const
{
    stream << (unsigned char) ID_LAMBERTIANMATERIAL;
    stream << Kd;
}

/**
 * Loads the material from a bytestream.
 * 
 * @param stream The bytestream to deserialize from.
 */
void LambertianMaterial::Load(Bytestream& stream)
{
    stream >> Kd;
}