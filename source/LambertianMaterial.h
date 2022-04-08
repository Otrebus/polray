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

    Color GetSample(const IntersectionInfo&, Ray& out, bool adjoint) const;
    virtual Color GetSampleE(const IntersectionInfo& info, Ray& out, float& pdf, float& rpdf, unsigned char& component, bool adjoint) const;
    Color BRDF(const IntersectionInfo& info, const Vector3d& out) const;
    virtual Color ComponentBRDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component) const;

    Light* GetLight() const;
    bool IsSpecular(unsigned char component) const;

    void ReadProperties(stringstream& ss);	
        virtual float PDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component, bool adjoint) const;

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    mutable Random rnd;

    // Variables for phong lighting
    Color Kd;
    Color emissivity;
};

#endif
    