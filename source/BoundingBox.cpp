/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file BoundingBox.cpp
 * 
 * Implementation of the BoundingBox class.
 */

#include "BoundingBox.h"
#include "Utils.h"
#include "Ray.h"

/**
 * Constructor.
 * 
 * @param c1 The minimal corner of the bounding box.
 * @param c2 The maximal corner of the bounding box.
 */
BoundingBox::BoundingBox(const Vector3d& c1, const Vector3d& c2) : c1(c1), c2(c2)
{
}

/**
 * Destructor.
 */
BoundingBox::~BoundingBox()
{
}

/**
 * Constructor.
 */
BoundingBox::BoundingBox()
{
}

/**
 * Checks if a ray intersects with the bounding box.
 * 
 * @param ray The ray to intersect with.
 * @param tnear The closest distance along the ray to count as an intersection.
 * @param tfar The farthest distance along the ray to count as an intersection.
 * @returns True if there's an intersection in a distance along the ray in [tnear, tfar].
 */
bool BoundingBox::Intersect(const Ray& ray, double& tnear, double& tfar) const
{  
  
    double t1, t2;
    tfar = inf;
    tnear = -inf;

    for(int u = 0; u < 3; u++)
    {
        if(ray.direction[u] == 0)
        {  
            if(ray.origin[u] > c2[u] || ray.origin[u] < c1[u]) 
                return false;
        }
        else
        {  
            t1 = (c1[u] - ray.origin[u]) / ray.direction[u];
            t2 = (c2[u] - ray.origin[u]) / ray.direction[u];
            if(t1 > t2)  
                std::swap(t1, t2);  
            if(t1 > tnear) 
                tnear = t1;  
            if(t2 < tfar) 
                tfar = t2;  
            if(tnear > tfar) 
                return false;
        }  
    }
    return true;
}
