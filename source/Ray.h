/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Ray.h
 * 
 * Declaration of the Ray class.
 */

#pragma once

#include "Vector3d.h"

class IntersectionInfo;

class Ray
{
public:
    Ray();
    Ray(const Vector3d&, const Vector3d&);
    ~Ray();

    Vector3d origin;
    Vector3d direction;
};
