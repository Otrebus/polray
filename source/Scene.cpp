#define NOMINMAX
#include "scene.h"
#include "triangle.h"
#include "Random.h"
#include "Utils.h"
#include "ObjReader.h"
//#include "RayFactory.h"

//vector<PartRenderer*> PartRenderer::parts;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
Scene::Scene()
{
    calculatedBoundingBox = false;
}

//------------------------------------------------------------------------------
// Constructor, reads a trianglemesh into the scene data from the supplied file
//------------------------------------------------------------------------------
Scene::Scene(std::string f)
{
	auto [success, mesh, lghts] = ReadFromFile(f, 0);
    if(!success)
        __debugbreak();
	AddModel(mesh);
    for(auto light : lghts)
        AddLight(light);
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
Scene::~Scene()
{
}

BoundingBox Scene::GetBoundingBox()
{
    if(calculatedBoundingBox)
        return boundingBox;
    else
    {
        Vector3d m(inf, inf, inf);
        Vector3d M(-inf, -inf, -inf);
        for(auto& p : primitives)
        {
            for(int i = 0; i < 3; i++)
            {
                auto bb = p->GetBoundingBox();
                m[i] = std::min(bb.c1[i], m[i]);
                M[i] = std::max(bb.c2[i], M[i]);
            }
        }
        calculatedBoundingBox = true;
        boundingBox = BoundingBox(m, M);
        return boundingBox;
    }
}

void Scene::AddModel(Model* s)
{
	// It's done this way since every primitive adds to the scene array differently. Triangle meshes
	// for example adds its individual triangles to the scene array to facilitate kd tree building
    models.push_back(s);
	s->AddToScene(*this);
}

void Scene::AddLight(Light* l)
{
    l->AddToScene(this);
}

void Scene::SetCamera(Camera* cam)
{
    camera = cam;
}

void Scene::SetPartitioning(SpatialPartitioning* prt)
{
    this->partitioning = prt;
}

SpatialPartitioning* Scene::GetPartitioning() const
{
    return partitioning;
}

Camera* Scene::GetCamera() const
{
    return camera;
}

void Scene::Load(Bytestream& b)
{
    size_t nLights;
    size_t nModels;

    // Theoretically, all the base classes implementing Save/Load/Create could
    // implement some common interface called Streamable or something, and I
    // could have a single list of objects that implement the Streamable
    // interface
    b >> nLights >> nModels;

    unsigned char id;
    b >> id;
    camera = Camera::Create(id);
    camera->Load(b);

    for(unsigned int i = 0; i < nLights; i++)
    {
        b >> id;
        Light* l = Light::Create(id);
        l->Load(b);
        AddLight(l);
    }
    for(unsigned int i = 0; i < nModels; i++)
    {
        b >> id;
        Model* m = Model::Create(id);
        m->Load(b);
        AddModel(m);
    }
}

void Scene::Save(Bytestream& b) const
{  
    b << lights.size() << models.size();

    camera->Save(b);

    for(auto& light : lights)
        light->Save(b);

    for(auto& model : models)
        model->Save(b);
}

std::vector<const Primitive*> Scene::GetPrimitives() const
{
    return primitives;
}

std::vector<Light*> Scene::GetLights() const
{
    return lights;
}

bool Scene::Intersect(const Ray& ray, double tmax) const {
    const Primitive* dummy;
    return partitioning->Intersect(ray, dummy, 0, tmax, false) > 0;
};

double Scene::Intersect(const Ray& ray, const Primitive*& p, const Light*& l) const {
    p = nullptr;
    l = nullptr;
    auto primT = partitioning->Intersect(ray, p, 0, inf, true);
    double lightT = -inf;
    double minLightT = inf;

    for(auto& light : lights)
    {
        auto t = light->Intersect(ray);

        if(t != -inf && t < minLightT)
        {
            minLightT = t;
            l = light;
        }
    }

    if(minLightT != inf)
        lightT = minLightT;

    if(primT != -inf && (lightT == -inf || primT < lightT))
    {
        l = nullptr;
        return primT;
    }
    else if(lightT != -inf)
    {
        p = nullptr;
        return lightT;
    }

    p = nullptr;
    l = nullptr;
    return -inf;
};

std::pair<Light*, double> Scene::PickLight(double r1) const
{
    auto p = 1.0/lights.size() + eps;
    auto sum = 0.0;
    for(auto light : lights)
    {
        sum += p;
        if(sum > r1)
            return { light, p };
    }
    return { lights[0], 0 }; // Should never happen
}
