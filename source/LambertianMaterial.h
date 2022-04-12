#ifndef LAMBERTIANMATERIAL_H
#define LAMBERTIANMATERIAL_H

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
#include <functional>
#include "Renderer.h"

class LambertianMaterial : public Material
{

public:
    LambertianMaterial();
    ~LambertianMaterial();

    Sample GetSample(const IntersectionInfo&, bool adjoint) const;
    Color BRDF(const IntersectionInfo& info, const Vector3d& out) const;

    Light* GetLight() const;

    void ReadProperties(stringstream& ss);	
    float PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint) const;

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    mutable Random rnd;

    // Variables for phong lighting
    Color Kd;
    Color emissivity;
};

#endif
    