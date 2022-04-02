#include <intrin.h>
#include "EnvironmentLight.h"
#include "PreethamSky.h"

EnvironmentLight* EnvironmentLight::Create(unsigned char c)
{
    switch(c)
    {
    case ID_PREETHAMSKY:
        return new PreethamSky;
        break;
    default:
        __debugbreak();
    }
}

EnvironmentLight::EnvironmentLight()
{
}

EnvironmentLight::~EnvironmentLight()
{
}