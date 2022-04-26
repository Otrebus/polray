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
    AreaLight(const Vector3d& pos, const Vector3d& c1, const Vector3d& c2, const Color& color, std::shared_ptr<Scene> s);
    Color CalculateSurfaceRadiance(const Vector3d&, const IntersectionInfo&);
    //Color SamplePoint (Vector3d& point, Vector3d& normal) const;
    Vector3d GetNormal() const;

    double Pdf(const IntersectionInfo& info, const Vector3d& out) const;

    Color NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info) const;
    Color NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info) const;
    Color DirectHitMIS(const Renderer* renderer, const IntersectionInfo& lastInfo, const IntersectionInfo& thisInfo) const;

    Color SampleRay(Ray& ray, Vector3d& Normal, double& areaPdf, double& anglePdf) const;
    void SamplePoint(Vector3d& point, Vector3d& Normal) const;

    void Save(Bytestream& s) const;
    void Load(Bytestream& s);

    void AddPortal(const Vector3d& pos, const Vector3d& v1, const Vector3d& v2);

    double GetArea() const;
    void AddToScene(std::shared_ptr<Scene> scn);
protected:
    Vector3d pos, c1, c2;
    std::shared_ptr<Scene> scene;
    EmissiveMaterial* material;
    mutable Random r;
};

#endif
