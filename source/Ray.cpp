/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Ray.cpp
 * 
 * Implementation of the Ray class.
 */

#include "Ray.h"
#include "IntersectionInfo.h"

/**
 * Constructor
 * 
 * @param origin The origin of the ray.
 * @param direction The direction of the ray.
 */
Ray::Ray(const Vector3d& origin, const Vector3d& direction) : origin(origin), direction(direction)
{
}

/**
 * Constructor.
 */
Ray::Ray()
{
}

/**
 * Destructor.
 */
Ray::~Ray()
{
}
