#include "BrutePartitioning.h"
#include "Utils.h"
#include "Primitive.h"

/**
 * Adds the shapes to the BrutePartitioning.
 * 
 * @param shapes The shapes to add.
 */
void BrutePartitioning::Build(const std::vector<const Primitive*>& shapes)
{
    primitives = shapes;
}

/**
 * Intersects the primitives with a ray, returning the closest primitive hit, if specified.
 * 
 * @param ray The ray to intersect with.
 * @param primitive The intersected triangle.
 * @param tmin The smallest parametric distance along the ray to find intersections.
 * @param tmax The greatest parametric distance along the ray to find intersections.
 * @param returnPrimitive Whether to find the smallest parametric distance along the ray that the
                          primitive was intersected and return the primitive that was
                          intersected, or just reporting any parametric distance and returning no
                          primitive.
 * @returns The parametric distance along the ray that the intersection happened, or -inf if
 *          no intersection happened.
 */
double BrutePartitioning::Intersect(const Ray& ray, const Primitive* &primitive, double tmin, double tmax, bool returnPrimitive = true) const
{
    bool found = false;
    double mint = inf;

    for(auto p : primitives)
    {
        double t = p->Intersect(ray);
        if(t > -inf && t < mint && t >= tmin && t <= tmax)
        {
            if(!returnPrimitive)
                return true;
            found = true;
            primitive = p;
            mint = t;
        }
    }
    if(found)
        return mint;
    return -inf;
}
