#include "scene.h"
#include "triangle.h"
#include "Random.h"
//#include "RayFactory.h"

//vector<PartRenderer*> PartRenderer::parts;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
Scene::Scene()
{
}

//------------------------------------------------------------------------------
// Constructor, reads a trianglemesh into the scene data from the supplied file
//------------------------------------------------------------------------------
Scene::Scene(string f)
{
	TriangleMesh* tmp = new TriangleMesh;
	tmp->ReadFromFile(f, 0);
	AddModel(tmp);
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
Scene::~Scene()
{

}

/*
void Scene::Build()
{
	tree->Build(primitives);
}

void Scene::Render(ColorBuffer& colBuf)
{
    m_renderer->Render(*camera, colBuf);
}*/

void Scene::AddModel(Model* s)
{
	// It's done this way since every primitive adds to the scene array differently. Triangle meshes
	// for example adds its individual triangles to the scene array to facilitate kd tree building
    models.push_back(s);
	s->AddToScene(*this);
}

void Scene::AddLight(Light* l)
{
    l->AddToScene(shared_from_this());
}

/*void Scene::SetRenderer(Renderer* rnd)
{
	m_renderer = rnd;
}

Renderer* Scene::GetRenderer() const
{
	return m_renderer;
}*/

void Scene::SetCamera(Camera* cam)
{
    camera = cam;
}

Camera* Scene::GetCamera() const
{
    return camera;
}

void Scene::Load(Bytestream& b)
{
    unsigned int nLights;
    unsigned int nModels;

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
        unsigned char id;
        b >> id;
        Light* l = Light::Create(id);
        l->Load(b);
        AddLight(l);
    }
    for(unsigned int i = 0; i < nModels; i++)
    {
        unsigned char id;
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

    for(auto it = lights.begin(); it < lights.end(); it++)
    {
        Light* l = *it;
        l->Save(b);
    }
    for(auto it = models.begin(); it < models.end(); it++)
    {
        Model* m = *it;
        m->Save(b);
    }
}

vector<const Primitive*> Scene::GetPrimitives() const
{
    return primitives;
}

vector<Light*> Scene::GetLights() const
{
    return lights;
}

const EnvironmentLight* Scene::GetEnvironmentLight() const
{
    return envLight;
}

void Scene::SetEnvironmentLight(EnvironmentLight* l)
{
    envLight = l;
}