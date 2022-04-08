#include "boundingbox.h"

BoundingBox::BoundingBox(const Vector3d& a, const Vector3d& b) : c1(a), c2(b)
{
}

BoundingBox::~BoundingBox()
{
}

BoundingBox::BoundingBox()
{
}

bool BoundingBox::ContainsPoint(const Vector3d& v)
{
    return (v.x <= c2.x && v.x >= c1.x && v.y <= c2.y && v.y >= c1.y && v.z >= c1.z && v.z <= c2.z);
}

bool BoundingBox::Intersects(const Ray& ray)
{  
    float t1, t2, tfar = numeric_limits<float>::max(), tnear = numeric_limits<float>::min();

    // Check X slab
    for(int u = 0; u < 3; u++)
    {
        if (ray.direction[u] == 0) 
        {  
            if (ray.origin[u] > c2[u] || ray.origin[u] < c1[u]) 
                return false;
        }
        else
        {  
            t1 = (c1[u] - ray.origin[u]) / ray.direction[u];
            t2 = (c2[u] - ray.origin[u]) / ray.direction[u];
            if (t1 > t2)  
                swap(t1, t2);  
            if (t1 > tnear) 
                tnear = t1;  
            if (t2 < tfar) 
                tfar = t2;  
            if (tnear > tfar || tfar < 0.0) 
                return false;
        }
    }
    return true;  
}

bool BoundingBox::Intersect(const Ray& ray, float& tnear, float& tfar) const {  
  
    float t1, t2;
    tfar = numeric_limits<float>::infinity();
    tnear = -numeric_limits<float>::infinity();

    for(int u = 0; u < 3; u++)
    {
        if (ray.direction[u] == 0)
        {  
            if (ray.origin[u] > c2[u] || ray.origin[u] < c1[u]) 
                return false;
        }
        else
        {  
            t1 = (c1[u] - ray.origin[u]) / ray.direction[u];
            t2 = (c2[u] - ray.origin[u]) / ray.direction[u];
            if (t1 > t2)  
                swap(t1, t2);  
            if (t1 > tnear) 
                tnear = t1;  
            if (t2 < tfar) 
                tfar = t2;  
            if (tnear > tfar) 
                return false;
        }  
    }
    return true;
}

BoundingBox& BoundingBox::operator=(const BoundingBox& b)
{
    c1 = b.c1;
    c2 = b.c2;
    return *this;
}

float BoundingBox::GetArea() const
{
    return (c2.z-c1.z)*(c2.y-c1.y)*2 + (c2.x-c1.x)*(c2.z-c1.z)*2 + (c2.x-c1.x)*(c2.y-c1.y)*2;
}

float BoundingBox::GetVolume() const
{
    return (c2.z-c1.z)*(c2.y-c1.y)*(c2.x-c1.x);
}