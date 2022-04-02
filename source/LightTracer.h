#ifndef LIGHTTRACER_H
#define LIGHTTRACER_H

#include "Camera.h"
#include "ColorBuffer.h"
#include "Primitive.h"
#include "Renderer.h"
#include "Random.h"
#include "LightTree.h"

class LightTracer : public Renderer
{
public:
	LightTracer(std::shared_ptr<Scene> scene);
	~LightTracer();

	void Render(Camera& cam, ColorBuffer& colBuf);
	void RenderPart(Camera& cam, ColorBuffer& colBuf) const;
	void Setup(const std::vector<Primitive*>& primitives, const std::vector<Light*>& lights);
	
	void SetSPP(unsigned int samplesperpixel);
	unsigned int GetSPP() const;

    unsigned int GetType() const;

private:
	int m_SPP;
	mutable Random m_random;
};

#endif