#include "Renderer.h"
#include "Primitive.h"
#include "KDTree.h"
#include "PathTracer.h"
#include "BDPT.h"
#include "RayTracer.h"
#include "LightTracer.h"

#include "Timer.h"
#include "Logger.h"

Renderer::Renderer(std::shared_ptr<Scene> scene)
{
    stopping = false;
    this->scene = scene;
    if(!scene->partitioning)
        scene->partitioning = new KDTree();
    Timer t1;
    t1.Reset();
    scene->partitioning->Build(scene->primitives);
    logger.Box(std::to_string(t1.GetTime()));
    m_lights = scene->GetLights();
}

Renderer::~Renderer()
{
}

//------------------------------------------------------------------------------
// Traces a ray the through scene graph and returns TRUE if ray was NOT 
// intersected
//------------------------------------------------------------------------------
bool Renderer::TraceShadowRay(const Ray& ray, double tmax) const
{
    return !scene->Intersect(ray, tmax);
}

std::shared_ptr<Scene> Renderer::GetScene() const
{
    return scene;
}

void Renderer::Stop()
{
    stopping = true;
}

Renderer* Renderer::Create(unsigned char id, std::shared_ptr<Scene> scn)
{
    switch(id)
    {
    case ID_PATHTRACER:
        return new PathTracer(scn);
        break;
    case ID_LIGHTTRACER:
        return new LightTracer(scn);
        break;
    case ID_RAYTRACER:
        return new RayTracer(scn);
        break;
    case ID_BDPT:
        return new BDPT(scn);
        break;
    default:
        __debugbreak();
        return nullptr;
    }
}