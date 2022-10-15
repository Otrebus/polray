#include "Bytestream.h"
#include "DielectricMaterial.h"
#include <sstream>
#include "Utils.h"
#include "Sample.h"
#include "IntersectionInfo.h"

/**
 * Constructor.
 */
DielectricMaterial::DielectricMaterial()
{
    m_ior = 1.5f;
}

/**
 * Destructor.
 */
DielectricMaterial::~DielectricMaterial()
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
Sample DielectricMaterial::GetSample(const IntersectionInfo& info, Randomizer& rnd, bool adjoint) const
{
    auto pdf = 1;  // This is not true, of course, which is indicated by the
    auto rpdf = 1; // specularity of this brdf

    Vector3d _normal = info.normal;
    Vector3d normal;

    const Vector3d& wi = info.direction;
    const Vector3d& Ng = info.geometricnormal;
    Vector3d& Ns = normal;

    double cosi = _normal*wi*-1;
    double n1, n2;

    if(cosi > 0) // Ray from outside the material, going in
    {
        n1 = 1.0;
        n2 = m_ior;
        normal = _normal;
    }
    else        // Ray from the inside, going out - stuff is flipped
    {           // to make the rest of the calculations still valid
        n1 = m_ior;
        n2 = 1.0;
        normal = -_normal;
        cosi = -cosi;
    }
    
    double d = 1-(n1/n2)*(n1/n2)*(1-cosi*cosi);

    if(d < 0) // Total internal reflection
    {
        Ray out;
        out.direction = Reflect(info.direction, Ns);
        out.direction.Normalize();
        auto wo = out.direction;
        out.origin = info.position;
        auto color = adjoint ? abs((1/(wi*Ng))*(wo*Ng/(1))) * Color::Identity : Color::Identity;
        return Sample(color, out, pdf, rpdf, true, 1);
    }
    Vector3d refraction = wi*(n1/n2) + Ns*(cosi*(n1/n2) - sqrt(d));
    refraction.Normalize();

    double cost = -refraction*normal;
    double Rs = (n1 * cosi - n2 * cost)/(n1 * cosi + n2*cost);
    double Rp = (n1 * cost - n2 * cosi)/(n1 * cost + n2*cosi);
    double R = (Rs*Rs+Rp*Rp)/2.0;

    if(rnd.GetDouble(0, 1) > R) // Refracted
    {
        Ray out;
        out.direction = refraction.Normalized();
        auto wo = out.direction;
        out.origin = info.position + 2*eps*(wo*Ng > 0 ? Ng : -Ng);
        auto color = adjoint ? abs((wi*Ns/(wi*Ng))*(wo*Ng/(wo*Ns))) * Color::Identity : (n1/n2)*(n1/n2)*Color::Identity;
        return Sample(color, out, pdf, rpdf, true, 1);
    }
    else // Reflected
    {
        Ray out;
        out.direction = Reflect(info.direction, Ns).Normalized();
        auto wo = out.direction;
        out.origin = info.position;
        auto color = adjoint ? abs((1/(wi*Ng))*(wo*Ng/(1))) * Color::Identity : Color::Identity;
        return Sample(color, out, pdf, rpdf, true, 1);
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
Color DielectricMaterial::BRDF(const IntersectionInfo&, const Vector3d&, int) const
{
    return Color(0, 0, 0); // The chance that the out, in vectors 
}                          // are reflectant is 0

/**
 * Returns the associated light of this material, if any.
 * 
 * @returns The light.
 */
Light* DielectricMaterial::GetLight() const
{
    return light;
}

/**
 * Reads the properties of the material from a stringstream (from a .mtl file).
 * 
 * @param ss The stringstream.
 */
void DielectricMaterial::ReadProperties(std::stringstream& ss)
{
    while(!ss.eof())
    {
        std::string line;
        getline(ss, line);
        std::stringstream ss2(line);
        ss2 >> m_ior;
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
double DielectricMaterial::PDF(const IntersectionInfo&, const Vector3d&, bool, int) const
{
    return 1;
}

/**
 * Saves the material to a bytestream.
 * 
 * @param stream The stream to serialize to.
 */
void DielectricMaterial::Save(Bytestream& stream) const
{
    stream << (unsigned char) ID_DIELECTRICMATERIAL;
    stream << m_ior;
}

/**
 * Loads the material from a bytestream.
 * 
 * @param stream The bytestream to deserialize from.
 */
void DielectricMaterial::Load(Bytestream& stream)
{
    stream >> m_ior;
}