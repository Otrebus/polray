#define NOMINMAX
#include "scene.h"
#include "triangle.h"
#include "Randomizer.h"
#include "Utils.h"
#include "ObjReader.h"

/**
 * Constructor.
 */
Scene::Scene()
{
    calculatedBoundingBox = false;
}

/**
 * Constructor.
 * 
 * @param file The name of the .obj file from which to read the scene.
 */
Scene::Scene(std::string file)
{
	auto [mesh, lghts] = ReadFromFile(file, 0);
	AddModel(mesh);
    for(auto light : lghts)
        AddLight(light);
}

/**
 * Destructor.
 */
Scene::~Scene()
{
}

/**
 * Returns the bounding box of all primitives in the scene.
 * 
 * @returns The bounding box of the scene.
 */
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

/**
 * Adds a model to the scene.
 * 
 * @param model The model to add to the scene.
 */
void Scene::AddModel(Model* model)
{
	// It's done this way since every primitive adds to the scene array differently. Triangle meshes
	// for example adds its individual triangles to the scene array to facilitate kd tree building
    models.push_back(model);
	model->AddToScene(*this);
}

/**
 * Intersects all the objects in the scene with a ray.
 * 
 * @param ray The ray to intersect the scene with.
 * @param tmax The maximum distance we're allowed to record a hit within.
 * @returns True if some object in the scene was intersected.
 */
void Scene::AddLight(Light* l)
{
    l->AddToScene(this);
}

/**
 * Sets the camera which serves as our viewpoint of the scene.
 * 
 * @param camera The camera to use.
 */
void Scene::SetCamera(Camera* camera)
{
    this->camera = camera;
}

/**
 * Sets the spatial partitioning that the scene will use to calculate ray intersections with.
 * 
 * @param partitioning The partitioning that the scene should use.
 */
void Scene::SetPartitioning(SpatialPartitioning* partitioning)
{
    this->partitioning = partitioning;
}

/**
 * Returns the camera using which we render the scene.
 * 
 * @returns The camera.
 */
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

/**
 * Intersects all the objects in the scene with a ray.
 * 
 * @param ray The ray to intersect the scene with.
 * @param tmax The maximum distance we're allowed to record a hit within.
 * @returns True if some object in the scene was intersected.
 */
bool Scene::Intersect(const Ray& ray, double tmax) const
{
    return std::get<0>(partitioning->Intersect(ray, 0, tmax, false)) > 0;
};

/**
 * Intersects all the objects in the scene with a ray.
 * 
 * @param ray The ray to intersect the scene with.
 * @param p The primitive that was hit, if any.
 * @param l The light that was hit, if any.
 * @returns The distance along the ray that the entity was hit.
 */
std::tuple<double, const Primitive*, const Light*> Scene::Intersect(const Ray& ray) const
{
    const Primitive* p = nullptr;
    const Light* l = nullptr;
    auto [primT, prim] = partitioning->Intersect(ray, 0, inf, true);
    p = prim;
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
        return { primT, p, l };
    }
    else if(lightT != -inf)
    {
        p = nullptr;
        return { lightT, p, l };
    }

    p = nullptr;
    l = nullptr;
    return { -inf, p, l };
};

/**
 * Randomly picks a light.
 * 
 * @param r1 A uniformly random number in [0, 1].
 * @returns A pair of the picked light and the probability of having picked that light.
 */
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
