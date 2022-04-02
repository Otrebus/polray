#ifndef PATHTRACER_H
#define PATHTRACER_H

#include "Renderer.h"
#include "Random.h"
#include "LightTree.h"

class PathTracer : public Renderer
{
public:
	PathTracer(std::shared_ptr<Scene> scene);
	~PathTracer();

	void Render(Camera& cam, ColorBuffer& colBuf);

	void Setup(const std::vector<Primitive*>& primitives, const std::vector<Light*>& lights);
	Color TracePath(const Ray& ray) const;
	//bool TraceShadowRay(const Ray& ray, float tmax) const;
	Color TracePathPrimitive(const Ray& ray) const;
	void SetSPP(unsigned int samplesperpixel);
	unsigned int GetSPP() const;

    unsigned int GetType() const;

private:
	unsigned int m_SPP;
	mutable Random m_random;
};

#endif