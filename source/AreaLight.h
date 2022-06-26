#ifndef AREALIGHT_H
#define AREALIGHT_H

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
    Color CalculateSurfaceRadiance(const Vector3d&, const IntersectionInfo&);
    //Color SamplePoint (Vector3d& point, Vector3d& normal) const;
    Vector3d GetNormal() const;

    double Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    double Pdf(const IntersectionInfo& info, const Vector3d& out) const;

    Color NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lightPoint, Vector3d& lightNormal, int component) const;
    Color NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info, int component) const;
    Color DirectHitMIS(const Renderer* renderer, const IntersectionInfo& lastInfo, const IntersectionInfo& thisInfo, int component) const;

    Color SampleRay(Ray& ray, Vector3d& Normal, double& areaPdf, double& anglePdf) const;
    void SamplePoint(Vector3d& point, Vector3d& Normal) const;

    void Save(Bytestream& s) const;
    void Load(Bytestream& s);

    double GetArea() const;
protected:
    friend class Scene;
    void AddToScene(Scene* scene);
    Vector3d pos, c1, c2;
    mutable Random r;
};

#endif
