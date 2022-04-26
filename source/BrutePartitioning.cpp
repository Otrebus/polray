#include "BrutePartitioning.h"
#include "Utils.h"

void BrutePartitioning::Build(std::vector<const Primitive*> primitives) {
    this->primitives = primitives;
}

double BrutePartitioning::Intersect(const Ray& ray, const Primitive* &primitive, double tmin, double tmax, bool returnPrimitive=true) const {
    bool found = false;
    double mint = inf;
    for(auto p : primitives) {
        double t = p->Intersect(ray);
        if(t > -inf && t < mint && t >= tmin && t <= tmax) {
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
