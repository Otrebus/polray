#ifndef INTERSECTIONINFO_H
#define INTERSECTIONINFO_H

#include "vector3d.h"


class Material;

class IntersectionInfo
{
public:
    IntersectionInfo(Vector3d&, Vector3d&, Vector3d&);
    IntersectionInfo();
    ~IntersectionInfo();

    Vector3d GetNormal() const;
    Vector3d GetPosition() const;
    Vector3d GetRayPosition() const;
    Vector2d GetTexturePosition() const;
    Material* GetMaterial() const;
    Vector3d GetDirection() const;
    Vector3d GetGeometricNormal() const;

    bool specular;

    Vector3d geometricnormal;
    Vector3d direction;
    Vector3d position;
    Vector3d normal;
    Vector2d texpos;
    Vector3d rayposition;
    Material* material;
};

#endif