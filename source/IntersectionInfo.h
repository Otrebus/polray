#pragma once

#include "Vector3d.h"

class Material;

class IntersectionInfo
{
public:
    IntersectionInfo(Vector3d&, Vector3d&, Vector3d&);
    IntersectionInfo();
    ~IntersectionInfo();

    bool specular;

    Vector3d geometricnormal;
    Vector3d direction;
    Vector3d position;
    Vector3d normal;
    Vector2d texpos;
    Vector3d rayposition;
    Material* material;
};