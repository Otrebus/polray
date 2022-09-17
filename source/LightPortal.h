#pragma once

#include "Light.h"
#include "Ray.h"
#include "Random.h"
#include <memory>

class Renderer;
class Scene;
class EmissiveMaterial;

class Portal
{
public:
    Portal() {}
    Portal(Vector3d pos, Vector3d v1, Vector3d v2) : pos(pos), v1(v1), v2(v2) {}
    double GetArea() const { return (v1 ^ v2).Length(); }
    double Intersect(const Ray& ray) const;
    Vector3d GetNormal() const { return (v1 ^ v2).Normalized(); }
    Vector3d pos, v1, v2;
};

class LightPortal : public Light
{
public:
    LightPortal();
    ~LightPortal();

    void AddPortal(Vector3d pos, Vector3d v1, Vector3d v2);
    void SetLight(Light* light);

    std::tuple<Ray, Color, Normal, AreaPdf, AnglePdf> SampleRay() const;
    std::tuple<Point, Normal> SamplePoint() const;

    double Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    double Pdf(const IntersectionInfo& info, const Vector3d& out) const;

    std::tuple<Color, Point> NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, int component) const;

    Color GetIntensity() const;

    double GetArea() const;

    void Save(Bytestream& s) const;
    void Load(Bytestream& s);

protected:
    friend class Scene;
    void AddToScene(Scene*);
    Light* light;
    std::vector<Portal> portals;
    mutable Random r;
};
