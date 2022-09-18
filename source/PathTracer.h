#pragma once

#include "Renderer.h"
#include "Randomizer.h"

class PathTracer : public Renderer
{
public:
    PathTracer(std::shared_ptr<Scene> scene);
    ~PathTracer();

    void Render(Camera& cam, ColorBuffer& colBuf);

    Color TracePath(const Ray& ray);
    Color TracePathPrimitive(const Ray& ray);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

private:
    Randomizer m_random;
};
