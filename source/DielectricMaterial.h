#ifndef DIELECTRICMATERIAL_H
#define DIELECTRICMATERIAL_H

#include "Material.h"
#include "Vector3d.h"
#include "Color.h"
#include "GeometricRoutines.h"
#include "IntersectionInfo.h"
#include "Ray.h"
#include "Random.h"
#include "Texture.h"
#include <Windows.h>
#include <math.h>
#include "Random.h"
#include "Sample.h"

class Ray;

#define F_PI 3.141592653f

class DielectricMaterial : public Material
{
public:
    DielectricMaterial();
    ~DielectricMaterial();

    Sample GetSample(const IntersectionInfo& info, bool adjoint) const;

    Color BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const;

    Light* GetLight() const;
    
    virtual double PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const;

    void ReadProperties(stringstream& ss);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    double m_ior; // Index of refraction
    mutable Random m_rnd;
};

#endif
    