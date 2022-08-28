#pragma once

#include "Random.h"
#include "Material.h"

class Light;
class Sample;
class Vector3d;

class CookTorrance : public Material
{

public:
    CookTorrance();
    ~CookTorrance();
    
    Sample GetSample(const IntersectionInfo& info, bool adjoint) const;

    Color BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const;

    Light* GetLight() const;
    
    virtual double PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const;

    void ReadProperties(std::stringstream& ss);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    Texture* texture;
    Texture* normalmap;

    mutable Random rnd;

    Color Ks;
    double alpha;
    double ior;
};
