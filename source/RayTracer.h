#pragma once

#include "Renderer.h"
class Light;

class RayTracer : public Renderer
{
public:
    RayTracer(std::shared_ptr<Scene>);
    ~RayTracer();

    Color TraceRay(const Ray& ray) const;
    bool TraceShadowRay(const Ray& ray, double tmax) const;

    void Render(Camera&, ColorBuffer&);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

//private:

    Color TraceRayRecursive(Ray ray, int bounces, Primitive* ignore, double contribution) const;

    std::vector<Light*> m_lights;
    std::vector<Primitive*> m_primitives;
};
