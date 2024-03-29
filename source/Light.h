/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Light.h
 * 
 * Declaration of the Light base class.
 */

#pragma once

#include "Color.h"
#include "EmissiveMaterial.h"
#include "Utils.h"
#include <memory>

class IntersectionInfo;
class Vector3d;
class Bytestream;
class Scene;
class Ray;
class Color;
class Renderer;

class Light
{
public:
    Light();
    Light(Color intensity);
    virtual ~Light() {}
    virtual std::tuple<Ray, Color, Normal, AreaPdf, AnglePdf> SampleRay(Randomizer& rnd) const = 0;

    virtual double Intersect(const Ray& ray) const = 0;
    virtual bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const = 0;

    virtual double Pdf(const IntersectionInfo& info, const Vector3d& out) const = 0;
    virtual Color GetIntensity() const;

    virtual std::tuple<Color, Point> NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Randomizer& rnd, int component) const = 0;

    virtual double GetArea() const = 0;
    virtual void AddToScene(Scene* scene) = 0;

    virtual void Save(Bytestream& s) const = 0;
    virtual void Load(Bytestream& s) = 0;

    static Light* Create(unsigned char);

    EmissiveMaterial* material;
    Scene* scene;
protected:
    Color intensity;
};
