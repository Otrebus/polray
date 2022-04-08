#ifndef EMISSIVEMATERIAL_H
#define EMISSIVEMATERIAL_H

#include "vector3d.h"
#include "texture.h"
#include "Ray.h"
#include "GeometricRoutines.h"
#include "Random.h"
#include "Material.h"

class Light;

class EmissiveMaterial : public Material
{
public:
    EmissiveMaterial();
    ~EmissiveMaterial();

    Color GetSample(const IntersectionInfo& info, Ray& out, bool adjoint) const;
    Color GetSampleE(const IntersectionInfo& info, Ray& out, float& pdf, float& rpdf, unsigned char& component, bool adjoint) const;

    Color BRDF(const IntersectionInfo& info, const Vector3d& out) const;
    Color ComponentBRDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component) const;

    Light* GetLight() const;
    bool IsSpecular(unsigned char component) const;

    void ReadProperties(stringstream& ss);

virtual float PDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component, bool adjoint) const;

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    mutable Random rnd;

    Texture* texture;
    Texture* normalmap;

    Color emissivity;
};

#endif
    