#pragma once

#include "Light.h"
#include "Random.h"
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

    std::tuple<Ray, Color, Vector3d, AreaPdf, AnglePdf> SampleRay() const;
    std::tuple<Point, Normal> SamplePoint() const;

    double Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    double Pdf(const IntersectionInfo& info, const Vector3d& out) const;

    std::tuple<Color, Vector3d> NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, int component) const;

    double GetArea() const;
    void AddToScene(Scene*);

    void Save(Bytestream& s) const;
    void Load(Bytestream& s);

protected:
    virtual void SamplePointHemisphere(const Vector3d& apex, Vector3d& point, Vector3d& Normal) const;

    Vector3d position_;
    double radius_;
    mutable Random r_;
};
