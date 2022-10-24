/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Scene.h
 * 
 * Declaration of the Scene class.
 */

#pragma once

#define NOMINMAX

#include <string>
#include <unordered_set>
#include <Windows.h>
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
#include "UniformEnvironmentLight.h"
#include "AreaLight.h"
#include "MeshLight.h"
#include "LightPortal.h"

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
        friend void AreaLight::AddToScene(Scene*);
        friend void CsgCuboid::AddToScene(Scene& scene);
        friend void CsgCylinder::AddToScene(Scene& scene);
        friend void CsgSphere::AddToScene(Scene& scene);
        friend void CsgIntersection::AddToScene(Scene& scene);
        friend void MeshLight::AddToScene(Scene* scene);
        friend void SphereLight::AddToScene(Scene* scene);
    };

    // Another one, but for lights
    class LightAdder
    {
        static void AddLight(Scene& scene, Light* light)
        {
            scene.lights.push_back(light);
        }
        friend void AreaLight::AddToScene(Scene*);
        friend void SphereLight::AddToScene(Scene*);
        friend void MeshLight::AddToScene(Scene*);
        friend void LightPortal::AddToScene(Scene*);
        friend void UniformEnvironmentLight::AddToScene(Scene*);
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
        friend void AreaLight::AddToScene(Scene*);
    };

    Scene(std::string file);
    Scene();
    ~Scene();

    void Load(Bytestream& b);
    void Save(Bytestream& b) const;

    void AddModel(Model*);
    void AddLight(Light*);

    void SetCamera(Camera* camera);
    Camera* GetCamera() const;

    void SetPartitioning(SpatialPartitioning* partitioning);

    bool Intersect(const Ray&, double tmax) const;
    std::tuple<double, const Primitive*, const Light*> Intersect(const Ray&) const;

    std::pair<Light*, double> PickLight(double) const;

    BoundingBox GetBoundingBox();

    friend class LightAdder;
    friend class PrimitiveAdder;
    friend class Renderer;
private:
    Camera* camera;
    BoundingBox boundingBox;
    bool calculatedBoundingBox;

    std::vector<Light*> lights;
    std::vector<Model*> models;
    std::vector<const Primitive*> primitives;
    std::unordered_set<Material*> materials;

protected:
    SpatialPartitioning* partitioning;
};

