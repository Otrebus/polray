#include "AreaLight.h"
#include "LightPortal.h"
#include "Light.h"
#include "SphereLight.h"
#include "UniformEnvironmentLight.h"
#include "MeshLight.h"
#include <intrin.h>

Light::Light()
{
}

Light::Light(Color intensity) : intensity_(intensity)
{
}

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
    else {
        __debugbreak();
        return new AreaLight; // To satisfy the compiler who thinks the function otherwise might return null
    }
}

Color Light::GetIntensity() const
{
    return intensity_;
}