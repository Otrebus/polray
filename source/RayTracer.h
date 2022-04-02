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
	bool TraceShadowRay(const Ray& ray, float tmax) const;
	void Build();
	void Render(Camera&, ColorBuffer&);
	void SetSPP(unsigned int);

        virtual unsigned int GetSPP() const;
    virtual unsigned int GetType() const;
//private:

	Color TraceRayRecursive(Ray ray, int bounces, const KDTree& kdt, Primitive* ignore, float contribution) const;

	vector<Light*> m_lights;
	vector<Primitive*> m_primitives;
};

#endif