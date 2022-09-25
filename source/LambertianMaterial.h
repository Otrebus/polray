#pragma once

#include "Material.h"
#include "Randomizer.h"

class Bytestream;
class Light;
class Sample;
class IntersectionInfo;

class LambertianMaterial : public Material
{

public:
    LambertianMaterial();
    ~LambertianMaterial();

    Sample GetSample(const IntersectionInfo&, Randomizer& random, bool adjoint) const;
    Color BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const;

    Light* GetLight() const;

    void ReadProperties(std::stringstream& ss);	
    double PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const;

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    Color Kd;
};

    