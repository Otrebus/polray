#pragma once

#include "Light.h"
#include "Randomizer.h"
#include <tuple>

class Renderer;
class Scene;
class EmissiveMaterial;
class Ray;

class UniformEnvironmentLight : public Light
{
public:
    UniformEnvironmentLight();
    virtual ~UniformEnvironmentLight() {}
    UniformEnvironmentLight(const Vector3d& position, double radius, const Color& color);

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
    std::tuple<std::vector<Vector2d>, double, Vector3d, Vector3d, Vector3d> GetProjectedSceneHull(Ray& ray, Vector3d normal) const;

    friend class Scene;
    double radius;
    Color intensity;
    Vector3d position;
};
