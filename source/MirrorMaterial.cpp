#include "Bytestream.h"
#include "MirrorMaterial.h"
#include "Sample.h"
#include "Utils.h"
#include "IntersectionInfo.h"
#include "GeometricRoutines.h"
#include <sstream>

/**
 * Constructor.
 */
MirrorMaterial::MirrorMaterial()
{
}

/**
 * Destructor.
 */
MirrorMaterial::~MirrorMaterial()
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
Sample MirrorMaterial::GetSample(const IntersectionInfo& info, Randomizer&, bool adjoint) const
{
    Vector3d Ng, Ns;
    const Vector3d in = info.direction;

    if(in*info.geometricnormal < 0)
    {
        Ng = info.geometricnormal;
        Ns = info.normal;
    }
    else // The surface was hit from behind, so flip all normals
    {
        Ng = -info.geometricnormal;
        Ns = -info.normal;
    }

    Vector3d normal = Ns;

    Ray out;
    out.direction = Reflect(info.direction, normal);
    out.origin = info.position + normal*eps;
    out.direction.Normalize();

    return Sample((adjoint ? abs(out.direction*Ng)/abs(in*Ng) : 1.0)*Color(1, 1, 1), out, 1, 1, true, 1);
}

/**
 * Returns the value of the brdf in a certain ingoing/outgoing direction for a brdf component.
 * 
 * @param info The intersection info.
 * @param out The outgoing direction.
 * @param component The component of the brdf.
 * @returns The value of the brdf.
 */
Color MirrorMaterial::BRDF(const IntersectionInfo&, const Vector3d&, int) const
{
    return Color(0, 0, 0); // The chance that the out, in vectors are reflectant is effectively 0
}

/**
 * Returns the associated light of this material, if any.
 * 
 * @returns The light.
 */
Light* MirrorMaterial::GetLight() const
{
    return light;
}

/**
 * Reads the properties of the material from a stringstream (from a .mtl file).
 * 
 * @param ss The stringstream.
 */
void MirrorMaterial::ReadProperties(std::stringstream& ss)
{
    while(!ss.eof())
    {
        std::string line, s;
        getline(ss, line);
        std::stringstream ss2(line);
        ss2 >> s;
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
double MirrorMaterial::PDF(const IntersectionInfo&, const Vector3d&, bool adjoint, int component) const
{
    return 1;
}

/**
 * Saves the material to a bytestream.
 * 
 * @param stream The stream to serialize to.
 */
void MirrorMaterial::Save(Bytestream& stream) const
{
    stream << (unsigned char) 103;
}

/**
 * Loads the material from a bytestream.
 * 
 * @param stream The bytestream to deserialize from.
 */
void MirrorMaterial::Load(Bytestream&)
{
}