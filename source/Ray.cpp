#include "Ray.h"

Ray::Ray(const Vector3d& org, const Vector3d& dir) : origin(org), direction(dir)
{
}

Ray::Ray(const Vector3d& org, const Vector3d& dir, IntersectionInfo ii) : origin(org), direction(dir)
{
}

Ray::Ray()
{
}

Ray::Ray(const Ray& r) : direction(r.direction), origin(r.origin)
{
}

Ray::~Ray()
{
}
