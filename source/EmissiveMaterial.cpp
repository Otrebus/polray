/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file EmissiveMaterial.cpp
 * 
 * Implementation of the EmissiveMaterial class.
 */

#include "EmissiveMaterial.h"
#include "Bytestream.h"
#include "Utils.h"
#include "Sample.h"
#include "IntersectionInfo.h"

/**
 * Constructor.
 */
EmissiveMaterial::EmissiveMaterial()
{
}

/**
 * Destructor.
 */
EmissiveMaterial::~EmissiveMaterial()
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
Sample EmissiveMaterial::GetSample(const IntersectionInfo& info, Randomizer&, bool) const
{
    auto out = Ray(info.position, -info.direction);
    return Sample(Color(0.0, 0.0, 0.0), out, 2*pi, 2*pi, false, 1);
}

/**
 * Returns the associated light of this material.
 * 
 * @returns The light.
 */
Light* EmissiveMaterial::GetLight() const
{
    return light;
}

/**
 * Returns the value of the brdf in a certain ingoing/outgoing direction for a brdf component.
 * 
 * @param info The intersection info.
 * @param out The outgoing direction.
 * @param component The component of the brdf.
 * @returns The value of the brdf.
 */
Color EmissiveMaterial::BRDF(const IntersectionInfo&, const Vector3d&, int) const
{
    return Color(0.0, 0.0, 0.0); // Blackbody
}

/**
 * Reads the properties of the material from a stringstream (from a .mtl file).
 * 
 * @param ss The stringstream.
 */
void EmissiveMaterial::ReadProperties(std::stringstream&)
{
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
double EmissiveMaterial::PDF(const IntersectionInfo&, const Vector3d&, bool, int) const
{
    return 0;
}

/**
 * Saves the material to a bytestream.
 * 
 * @param stream The stream to serialize to.
 */
void EmissiveMaterial::Save(Bytestream& stream) const
{
    stream << ID_EMISSIVEMATERIAL;
    stream << emissivity;
}

/**
 * Loads the material from a bytestream.
 * 
 * @param stream The bytestream to deserialize from.
 */
void EmissiveMaterial::Load(Bytestream& stream)
{
    stream >> emissivity;
}