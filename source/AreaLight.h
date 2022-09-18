#pragma once

#include "Light.h"
#include "Randomizer.h"
#include "Vector3d.h"

class Renderer;
class Scene;
class EmissiveMaterial;
class Ray;

class AreaLight : public Light
{
public:
    AreaLight();
    virtual ~AreaLight() {}
    AreaLight(const Vector3d& pos, const Vector3d& c1, const Vector3d& c2, const Color& color);

    Vector3d GetNormal() const;

    double Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    double Pdf(const IntersectionInfo& info, const Vector3d& out) const;

    std::tuple<Color, Point> NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Randomizer&, int component) const;

    std::tuple<Ray, Color, Normal, AreaPdf, AnglePdf> SampleRay(Randomizer& rnd) const;

    void Save(Bytestream& s) const;
    void Load(Bytestream& s);

    double GetArea() const;
protected:
    std::tuple<Point, Normal> SamplePoint(Randomizer& rnd) const;

    friend class Scene;
    void AddToScene(Scene* scene);
    Vector3d pos, c1, c2;
    Randomizer r;
};
