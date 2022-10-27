/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file IntersectionInfo.h
 * 
 * Declaration of the IntersectionInfo class.
 */

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