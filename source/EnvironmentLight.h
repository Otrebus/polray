#ifndef ENVIRONMENT_LIGHT
#define ENVIRONMENT_LIGHT

#include "Vector3d.h"
#include "Light.h"

#define ID_PREETHAMSKY ((unsigned char)50)

class Bytestream;
class Color;
class Vector3d;

class EnvironmentLight
{
public: 
    EnvironmentLight();
    virtual ~EnvironmentLight();

    virtual Color GetRadiance(const Vector3d& dir) const = 0;

    static EnvironmentLight* Create(unsigned char n);

    virtual void Save(Bytestream& stream) const = 0;
    virtual void Load(Bytestream& stream) = 0;
};

#endif
