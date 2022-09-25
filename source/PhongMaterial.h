#pragma once

#include "Material.h"
#include "Randomizer.h"

class Bytestream;
class Light;
class Sample;
class IntersectionInfo;

class PhongMaterial : public Material
{

public:
    PhongMaterial();
    ~PhongMaterial();
    
    Sample GetSample(const IntersectionInfo& info, Randomizer& random, bool adjoint) const;

    Color BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const;

    Light* GetLight() const;
    
    virtual double PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const;

    void ReadProperties(std::stringstream& ss);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    Color Ka, Kd, Ks;
    double alpha;
};
