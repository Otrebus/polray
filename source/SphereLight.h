/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file SphereLight.h
 * 
 * Declaration of the SphereLight class.
 */

#pragma once

#include "Light.h"
#include "Randomizer.h"
#include <memory>

class Renderer;
class Scene;
class EmissiveMaterial;

class SphereLight : public Light
{
public:
    SphereLight();
    SphereLight(Vector3d position, double radius, Color intensity);
    ~SphereLight();

    std::tuple<Ray, Color, Normal, AreaPdf, AnglePdf> SampleRay(Randomizer&) const;

    double Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    double Pdf(const IntersectionInfo& info, const Vector3d& out) const;

    std::tuple<Color, Point> NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Randomizer& rnd, int component) const;

    double GetArea() const;
    void AddToScene(Scene*);

    void Save(Bytestream& s) const;
    void Load(Bytestream& s);

protected:
    std::tuple<Point, Normal> SamplePoint(Randomizer&) const;
    Vector3d position_;
    double radius_;
};
