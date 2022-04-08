#include "Renderer.h"
#include "Primitive.h"

Renderer::Renderer(std::shared_ptr<Scene> scene)
{
    stopping = false;
    this->scene = scene;
    m_tree.Build(scene->GetPrimitives());
    vector<Light*> lights = scene->GetLights();
    lightTree = new LightNode(lights);
    m_lights = lights;
}

Renderer::~Renderer()
{
}

bool Renderer::ShootRay(const Ray& ray, IntersectionInfo& info) const
{
    const Primitive* primitiveHit;
    if(m_tree.Intersect(ray, primitiveHit) >= 0)
    {
        primitiveHit->GenerateIntersectionInfo(ray, info);
        return true;
    }
    else return false;
}

//------------------------------------------------------------------------------
// Traces a ray the through scene graph and returns TRUE if ray was NOT 
// intersected
//------------------------------------------------------------------------------
bool Renderer::TraceShadowRay(const Ray& ray, float tmax) const
{
    /*Ray& unconstRay = const_cast<Ray&>(ray);
    const Primitive* dummy;
    float result = m_tree.Intersect(ray, dummy);
    //if(dummy)dummy->Intersect(ray);
    if(result < tmax*0.999f && result >= 0)
        return false;
    return true;*/
    Ray& unconstRay = const_cast<Ray&>(ray);
    return !m_tree.Intersect(ray, tmax);
}

std::shared_ptr<Scene> Renderer::GetScene() const
{
    return scene;
}

void Renderer::Stop()
{
    stopping = true;
}