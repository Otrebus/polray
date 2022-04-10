#include "BrutePartitioning.h"

void BrutePartitioning::Build(std::vector<const Primitive*> primitives) {
    this->primitives = primitives;
}

bool BrutePartitioning::Intersect(const Ray& ray, float tmax) const {
    for(auto& p : primitives) {
        float t = p->Intersect(ray);
        if(t > 0 && t < tmax)
            return true;
    }
    return false;
}

float BrutePartitioning::Intersect(const Ray& r, const Primitive*& p) const {
    return Intersect(r, p, 0, 1e30);
}

float BrutePartitioning::Intersect(const Ray& ray, const Primitive* &primitive, float tmin, float tmax) const {
    bool found = false;
    float mint = tmax;
    for(auto& p : primitives) {
        float t = p->Intersect(ray);
        if(t > 0 && t < mint && t > tmin) {
            found = true;
            primitive = p;
            mint = t;
        }
    }
    if(found)
        return mint;
    else return -1.0f;
}