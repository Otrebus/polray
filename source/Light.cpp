#include "AreaLight.h"
#include "LightPortal.h"
#include "Light.h"
#include "SphereLight.h"
#include "UniformEnvironmentLight.h"
#include "MeshLight.h"
#include "Bytestream.h"

/**
 * Constructor.
 */
Light::Light()
{
}

/**
 * Constructor.
 * 
 * @param intensity The radiance emitted by the light.
 */
Light::Light(Color intensity) : intensity(intensity)
{
}

/**
 * Creates a light given the id of the light type.
 * 
 * @param c The id of the light (see Bytestream.h).
 * @returns A pointed to the created light.
 */
Light* Light::Create(unsigned char c)
{
    if(c == ID_AREALIGHT)
        return new AreaLight;
    else if(c == ID_SPHERELIGHT)
        return new SphereLight;
    else if(c == ID_LIGHTPORTAL)
        return new LightPortal;
    else if(c == ID_UNIFORMENVIRONMENTLIGHT)
        return new UniformEnvironmentLight;
    else if(c == ID_MESHLIGHT)
        return new MeshLight;
    else
    {
        __debugbreak();
        return new AreaLight; // To satisfy the compiler who thinks the function otherwise might return null
    }
}

/**
 * Returns the radiance emission of the light.
 * 
 * @returns The radiance that the light emits.
 */
Color Light::GetIntensity() const
{
    return intensity;
}