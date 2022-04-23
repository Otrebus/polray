#include "BrutePartitioning.h"
#include "Utils.h"

void BrutePartitioning::Build(std::vector<const Primitive*> primitives) {
    this->primitives = primitives;
}

float BrutePartitioning::Intersect(const Ray& ray, const Primitive* &primitive, float tmin, float tmax, bool returnPrimitive=true) const {
    bool found = false;
    float mint = inf;
    for(auto p : primitives) {
        float t = p->Intersect(ray);
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
