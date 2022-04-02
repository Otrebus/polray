#include "AreaLight.h"
#include "Light.h"
#include "SphereLight.h"
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
    else {
        __debugbreak();
        return new AreaLight; // To satisfy the compiler who thinks the function otherwise might return null
    }
}

Color Light::GetIntensity() const
{
    return intensity_;
}