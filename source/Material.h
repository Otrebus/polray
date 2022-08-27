#pragma once

#define NOMINMAX
#include "vector3d.h"
#include "texture.h"
#include "Color.h"
#include "GeometricRoutines.h"
#include "Sample.h"
#include <sstream>

class Light;
class Ray;
class IntersectionInfo;
class Bytestream;

class ShadingState;

class Material
{
public:
    Material();
    virtual ~Material();

    virtual Sample GetSample(const IntersectionInfo& info, bool adjoint) const = 0;

    virtual Color BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const = 0;

    virtual Light* GetLight() const = 0;

    virtual void ReadProperties(std::stringstream& ss) = 0;
    virtual double PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const = 0;

    virtual void Save(Bytestream& stream) const = 0;
    virtual void Load(Bytestream& stream) = 0;

    static Material* Create(unsigned char);

    Light* light;
    static int highestId;
    int id;
    Texture* texture;
    Texture* normalmap;
};
