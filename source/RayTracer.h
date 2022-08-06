#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "Renderer.h"
#include "Light.h"

class RayTracer : public Renderer
{
public:
    RayTracer(std::shared_ptr<Scene>);
    ~RayTracer();

    void Setup(const vector<Primitive*>& primitives, const vector<Light*>& lights);
    Color TraceRay(const Ray& ray) const;
    bool TraceShadowRay(const Ray& ray, double tmax) const;
    void Build();
    void Render(Camera&, ColorBuffer&);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

//private:

    Color TraceRayRecursive(Ray ray, int bounces, Primitive* ignore, double contribution) const;

    vector<Light*> m_lights;
    vector<Primitive*> m_primitives;
};

#endif