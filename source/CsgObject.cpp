/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file CsgObject.cpp
 * 
 * Implementation of the CsgObject base class used in constructive solid geometry.
 */

#include "CsgObject.h"
#include "Vector3d.h"

Vector3d CsgObject::Multiply(const Vector3d& u, const Vector3d& v, 
                             const Vector3d& w, const Vector3d& x)
{
    return x.x*u + x.y*v + x.z*w;
}
