#ifndef EMISSIVEMATERIAL_H
#define EMISSIVEMATERIAL_H

#include "vector3d.h"
#include "texture.h"
#include "Ray.h"
#include "GeometricRoutines.h"
#include "Random.h"
#include "Material.h"
#include "Sample.h"

class Light;

class EmissiveMaterial : public Material
{
public:
    EmissiveMaterial();
    ~EmissiveMaterial();

    Sample GetSample(const IntersectionInfo& info, bool adjoint) const;

    Color BRDF(const IntersectionInfo& info, const Vector3d& out) const;

    Light* GetLight() const;

    void ReadProperties(stringstream& ss);

    double PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint) const;

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    mutable Random rnd;

    Texture* texture;
    Texture* normalmap;

    Color emissivity;
};

#endif
    