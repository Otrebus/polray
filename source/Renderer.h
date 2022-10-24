/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Renderer.h
 * 
 * Declaration of the Renderer base class.
 */

#pragma once

#include <vector>
#include "KDTree.h"
#include "ColorBuffer.h"
#include "Camera.h"
#include "Scene.h"
#include "Randomizer.h"

class Ray;
class Primitive;
class Light;
class Scene;

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

    static Renderer* Create(unsigned char, std::shared_ptr<Scene> scn);
protected:
    std::shared_ptr<Scene> scene;

    bool stopping;

    mutable Randomizer m_random;
    std::vector<Light*> m_lights;
};
