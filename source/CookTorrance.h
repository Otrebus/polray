#ifndef COOKTORRANCE_H
#define COOKTORRANCE_H

#define _USE_MATH_DEFINES

#include "Material.h"
#include "Vector3d.h"
#include "Color.h"
#include "GeometricRoutines.h"
#include "IntersectionInfo.h"
#include "Ray.h"
#include "Random.h"
#include <Windows.h>
#include <math.h>
#include <memory>
#include <functional>
#include "Renderer.h"

class CookTorrance : public Material
{

public:
    CookTorrance();
    ~CookTorrance();
    
    Sample GetSample(const IntersectionInfo& info, bool adjoint) const;

    Color BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const;

    Light* GetLight() const;
    
    virtual double PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const;

    void ReadProperties(stringstream& ss);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    Texture* texture;
    Texture* normalmap;

    mutable Random rnd;

    Color Ks;
    double alpha;
    double ior;
};

#endif
    