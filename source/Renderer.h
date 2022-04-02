#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "KDTree.h"
#include "ColorBuffer.h"
#include "Camera.h"
#include "Scene.h"
#include "LightTree.h"

class Ray;
class Primitive;
class Light;

using namespace std;

class Renderer
{
public:
	Renderer(std::shared_ptr<Scene> scene);
	virtual ~Renderer();

	//virtual void Setup(const std::vector<Primitive*>& primitives, const std::vector<Light*>& lights) = 0;
	virtual void Render(Camera& cam, ColorBuffer& colBuf) = 0;
	virtual bool TraceShadowRay(const Ray& ray, float tmax) const;
	virtual bool ShootRay(const Ray& ray, IntersectionInfo& info) const;

    virtual unsigned int GetSPP() const = 0;
    virtual void SetSPP(unsigned int) = 0;
    virtual unsigned int GetType() const = 0;
    std::shared_ptr<Scene> GetScene() const;

    void Stop();

    static const unsigned int typePathTracer = 1;
    static const unsigned int typeLightTracer = 2;
    static const unsigned int typeBDPT = 3;
protected:
	KDTree m_tree;
    std::shared_ptr<Scene> scene;

    bool stopping;

  	int m_SPP;
	mutable Random m_random;
	vector<Light*> m_lights;
    LightNode* lightTree;
};

#endif