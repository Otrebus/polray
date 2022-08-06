#ifndef PATHTRACER_H
#define PATHTRACER_H

#include "Renderer.h"
#include "Random.h"

class PathTracer : public Renderer
{
public:
    PathTracer(std::shared_ptr<Scene> scene);
    ~PathTracer();

    void Render(Camera& cam, ColorBuffer& colBuf);

    Color TracePath(const Ray& ray) const;
    Color TracePathPrimitive(const Ray& ray) const;

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

private:
    mutable Random m_random;
};

#endif