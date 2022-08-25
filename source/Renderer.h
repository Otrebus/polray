#pragma once

#include <vector>
#include "KDTree.h"
#include "ColorBuffer.h"
#include "Camera.h"
#include "Scene.h"

class Ray;
class Primitive;
class Light;

using namespace std;

class Renderer
{
public:
    Renderer(std::shared_ptr<Scene> scene);
    virtual ~Renderer();

    virtual void Render(Camera& cam, ColorBuffer& colBuf) = 0;
    virtual bool TraceShadowRay(const Ray& ray, double tmax) const;

    std::shared_ptr<Scene> GetScene() const;

    void Stop();
    
    virtual void Save(Bytestream& stream) const = 0;
    virtual void Load(Bytestream& stream) = 0;

    static Renderer* Create(unsigned char, shared_ptr<Scene> scn);
protected:
    std::shared_ptr<Scene> scene;

    bool stopping;

    mutable Random m_random;
    vector<Light*> m_lights;
};
