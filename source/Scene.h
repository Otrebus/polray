#pragma once

#include <type_traits>
#include <string>
#include <unordered_set>
#include <Windows.h>
#include "EnvironmentLight.h"
#include "Renderer.h"
#include "Camera.h"
#include "TriangleMesh.h"
#include "ColorBuffer.h"
#include "Bytestream.h"
#include "Model.h"

#include "SpatialPartitioning.h"
#include "CsgIntersection.h"
#include "CsgSphere.h"
#include "CsgCylinder.h"
#include "CsgCuboid.h"
#include "Sphere.h"
#include "TriangleMesh.h"
#include "Triangle.h"
#include "SphereLight.h"
#include "AreaLight.h"
#include "MeshLight.h"

using namespace std;

class MeshLight;
class EnvironmentLight;
class SphereLight;
class Light;
class KDTree;

class Scene : public std::enable_shared_from_this<Scene>
{
public:
    // Attorney idiom for making sure only certain classes can add primitives
    // to the scene. Essentially, this is a selective friends functionality
    class PrimitiveAdder
    {
        static void AddPrimitive(Scene& scene, Primitive* primitive)
        {
            scene.primitives.push_back(primitive);
        }
        friend void Sphere::AddToScene(Scene& scene);
        friend void TriangleMesh::AddToScene(Scene& scene);
        friend void Triangle::AddToScene(Scene& scene);
        friend void AreaLight::AddToScene(std::shared_ptr<Scene>);
        friend void CsgCuboid::AddToScene(Scene& scene);
        friend void CsgCylinder::AddToScene(Scene& scene);
        friend void CsgSphere::AddToScene(Scene& scene);
        friend void CsgIntersection::AddToScene(Scene& scene);
    };

    // Another one, but for lights
    class LightAdder
    {
        static void AddLight(Scene& scene, Light* light)
        {
            scene.lights.push_back(light);
        }
        friend void AreaLight::AddToScene(std::shared_ptr<Scene>);
        friend void SphereLight::AddToScene(std::shared_ptr<Scene>);
        friend void MeshLight::AddToScene(std::shared_ptr<Scene>);
    };

    // And a last one, for materials
    class MaterialAdder
    {
        static void AddMaterial(Scene& scene, Material* material)
        {
            scene.materials.insert(material);
        }
        friend void Sphere::AddToScene(Scene& scene);
        friend void TriangleMesh::AddToScene(Scene& scene);
        friend void Triangle::AddToScene(Scene& scene);
        friend void AreaLight::AddToScene(std::shared_ptr<Scene>);
    };

	Scene(string file);
	Scene();
	~Scene();

    void Load(Bytestream& b);
    void Save(Bytestream& b) const;

	void Build();

	void AddModel(Model*);
    void AddLight(Light*);

    void SetCamera(Camera* camera);
    Camera* GetCamera() const;

    vector<const Primitive*> GetPrimitives() const;
    vector<Light*> GetLights() const;

    const EnvironmentLight* GetEnvironmentLight() const;
    void SetEnvironmentLight(EnvironmentLight* light);

    void SetPartitioning(SpatialPartitioning* partitioning);
    SpatialPartitioning* GetPartitioning() const;

    bool Intersect(const Ray&, float tmax) const;
    float Intersect(const Ray&, const Primitive*&) const;

    friend class LightAdder;
    friend class PrimitiveAdder;
    friend class Renderer;
private:
	Camera* camera;

	vector<Light*> lights;
    vector<Model*> models;
    vector<const Primitive*> primitives;
    unordered_set<Material*> materials;
    EnvironmentLight* envLight;

protected:
    SpatialPartitioning* partitioning;
};

