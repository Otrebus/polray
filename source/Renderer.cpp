#include "Renderer.h"
#include "Primitive.h"
#include "KDTree.h"
#include "PathTracer.h"
#include "BDPT.h"
#include "RayTracer.h"
#include "LightTracer.h"
#include "Timer.h"
#include "Logger.h"

/**
 * Constructor.
 * 
 * @param scene The scene used for rendering.
 */
Renderer::Renderer(std::shared_ptr<Scene> scene)
{
    stopping = false;
    this->scene = scene;
    if(!scene->partitioning)
        scene->partitioning = new KDTree();

    //Timer timer;
    scene->partitioning->Build(scene->primitives);
    //logger.Box(std::to_string(timer.GetTime()));
}

/**
 * Destructor.
 */
Renderer::~Renderer()
{
}

/**
 * Traces a shadow ray through the scene.
 * 
 * @param ray The ray to trace.
 * @param tmax The maximum distance along the ray to search for hits.
 * @returns True if the ray was not intersected.
 */
bool Renderer::TraceShadowRay(const Ray& ray, double tmax) const
{
    return !scene->Intersect(ray, tmax);
}

/**
 * Returns the scene used by the renderer.
 * 
 * @returns The scene used by the renderer.
 */
std::shared_ptr<Scene> Renderer::GetScene() const
{
    return scene;
}

/**
 * Stops rendering.
 */
void Renderer::Stop()
{
    stopping = true;
}

/**
 * Creates a renderer given an id (see Bytestream.h) and a scene.
 * 
 * @returns A pointer to the created renderer.
 */
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
