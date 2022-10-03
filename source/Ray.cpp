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
