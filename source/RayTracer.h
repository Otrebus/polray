/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file RayTracer.h
 * 
 * Declaration of the RayTracer class.
 */

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
