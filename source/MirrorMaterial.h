#pragma once

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

class Ray;

class MirrorMaterial : public Material
{
public:
    MirrorMaterial();
    ~MirrorMaterial();

    Sample GetSample(const IntersectionInfo& info, bool adjoint) const;

    Color BRDF(const IntersectionInfo& info, const Vector3d& out, int component) const;

    Light* GetLight() const;
    
    double PDF(const IntersectionInfo& info, const Vector3d& out, bool adjoint, int component) const;

    void ReadProperties(stringstream& ss);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    static int highestId;
    int id;
    Texture* texture;
    Texture* normalmap;
};

    