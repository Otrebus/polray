#pragma once

#include "Light.h"
#include "Ray.h"
#include "Random.h"
#include <memory>

class Renderer;
class Scene;
class EmissiveMaterial;

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

    Color NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lightPoint, Vector3d& lightNormal, int component) const;

    std::tuple<Point, Normal> SamplePoint() const;
    std::tuple<Ray, Color, Vector3d, AreaPdf, AnglePdf> SampleRay() const;

    void Save(Bytestream& s) const;
    void Load(Bytestream& s);

    double GetArea() const;
protected:
    friend class Scene;
    void AddToScene(Scene* scene);
    Vector3d pos, c1, c2;
    mutable Random r;
};

