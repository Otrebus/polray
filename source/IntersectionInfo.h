#pragma once

#include "Vector3d.h"

class Material;

class IntersectionInfo
{
public:
    IntersectionInfo();
    ~IntersectionInfo();

    Vector3d geometricnormal;
    Vector3d direction;
    Vector3d position;
    Vector3d normal;
    Vector2d texpos;
    Material* material;
};