#pragma once

#include "Light.h"
#include "Ray.h"
#include "Random.h"
#include <memory>

//#define M_PI 3.14159265

class Renderer;
class Scene;
class EmissiveMaterial;

class Portal
{
public:
    Portal() {}
    Portal(Vector3d pos, Vector3d v1, Vector3d v2) : pos(pos), v1(v1), v2(v2) {}
    double GetArea() const { return (v1 ^ v2).GetLength(); }
    double Intersect(const Ray& ray) const;
    Vector3d GetNormal() const { Vector3d n = v1 ^ v2; n.Normalize(); return n; }
    Vector3d pos, v1, v2;
};

class LightPortal : public Light
{
public:
    LightPortal();
    ~LightPortal();

    void AddPortal(Vector3d pos, Vector3d v1, Vector3d v2);
    void SetLight(Light* light);

    Color SampleRay(Ray& ray, Vector3d& Normal, double& areaPdf, double& pdf) const;
    void SamplePoint(Vector3d& point, Vector3d& Normal) const;

    double Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    double Pdf(const IntersectionInfo& info, const Vector3d& out) const;

    Color NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lightPoint, Vector3d& lightNormal, int component) const;
    Color NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info, int component) const;
    Color DirectHitMIS(const Renderer* renderer, const IntersectionInfo& lastInfo, const IntersectionInfo& thisInfo, int component) const;

    Color GetIntensity() const;

    double GetArea() const;

    void Save(Bytestream& s) const;
    void Load(Bytestream& s);

    static Light* Create(unsigned char);

protected:
    friend class Scene;
    void AddToScene(std::shared_ptr<Scene>);
    Light* light;
    std::vector<Portal> portals;
    std::shared_ptr<Scene> scene_;
    mutable Random r;
};
