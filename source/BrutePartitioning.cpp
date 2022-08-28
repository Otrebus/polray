#include "BrutePartitioning.h"
#include "Utils.h"
#include "Primitive.h"

void BrutePartitioning::Build(const std::vector<const Primitive*>& shapes)
{
    primitives = shapes;
}

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
