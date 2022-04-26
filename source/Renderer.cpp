#include "Renderer.h"
#include "Primitive.h"
#include "KDTree.h"

Renderer::Renderer(std::shared_ptr<Scene> scene)
{
    stopping = false;
    this->scene = scene;
    if(!scene->partitioning)
        scene->partitioning = new KDTree();
    scene->partitioning->Build(scene->primitives);
    vector<Light*> lights = scene->GetLights();
    lightTree = new LightNode(lights);
    m_lights = lights;
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
    Ray& unconstRay = const_cast<Ray&>(ray);
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