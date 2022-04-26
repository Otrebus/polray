#ifndef SPHERELIGHT_H
#define SPHERELIGHT_H

#include "Light.h"
#include "Ray.h"
#include "Random.h"
#include <memory>

//#define M_PI 3.14159265

class Renderer;
class Scene;
class EmissiveMaterial;

class SphereLight : public Light
{
public:
    SphereLight();
    SphereLight(Vector3d position, double radius, Color intensity);
    ~SphereLight();

    virtual Color SampleRay(Ray& ray, Vector3d& Normal, double& areaPdf, double& pdf) const;
    virtual void SamplePoint(Vector3d& point, Vector3d& Normal) const;

    virtual double Pdf(const IntersectionInfo& info, const Vector3d& out) const;

    virtual Color NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info) const;
    virtual Color NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info) const;
    virtual Color DirectHitMIS(const Renderer* renderer, const IntersectionInfo& lastInfo, const IntersectionInfo& thisInfo) const;

    virtual double GetArea() const;
    virtual void AddToScene(std::shared_ptr<Scene>);

    virtual void Save(Bytestream& s) const;
    virtual void Load(Bytestream& s);

    static Light* Create(unsigned char);

protected:
    virtual void SamplePointHemisphere(const Vector3d& apex, Vector3d& point, Vector3d& Normal) const;

    Vector3d position_;
    double radius_;
    std::shared_ptr<Scene> scene_;
    EmissiveMaterial* material_;
    mutable Random r_;
};

#endif
