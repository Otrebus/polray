#pragma once

#include "Light.h"
#include "Ray.h"
#include "Random.h"
#include <memory>
#include <tuple>

class Renderer;
class Scene;
class EmissiveMaterial;

class UniformEnvironmentLight : public Light
{
public:
    UniformEnvironmentLight();
    virtual ~UniformEnvironmentLight() {}
    UniformEnvironmentLight(const Vector3d& position, double radius, const Color& color);

    std::tuple<Ray, Color, Vector3d, AreaPdf, AnglePdf> SampleRay() const;

    double Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    double Pdf(const IntersectionInfo& info, const Vector3d& out) const;
    Color GetIntensity() const;

    Color NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lightPoint, Vector3d& lightNormal, int sample) const;

    std::tuple<Point, Normal> SamplePoint() const;
    double GetArea() const;
    void AddToScene(Scene*);

    void Save(Bytestream& s) const;
    void Load(Bytestream& s);

protected:
    std::tuple<std::vector<Vector2d>, double, Vector3d, Vector3d, Vector3d> GetProjectedSceneHull(Ray& ray, Vector3d normal) const;
    friend class Scene;
    double radius;
    Color intensity;
    Vector3d position;
    mutable Random random;
};

