#ifndef UNIFORMENVIRONMENTLIGHT_H
#define UNIFORMENVIRONMENTLIGHT_H

#include "Light.h"
#include "Ray.h"
#include "Random.h"
#include <memory>


class Renderer;
class Scene;
class EmissiveMaterial;

class UniformEnvironmentLight : public Light
{
public:
    UniformEnvironmentLight();
    virtual ~UniformEnvironmentLight() {}
    UniformEnvironmentLight(const Vector3d& position, double radius, const Color& color);

    //virtual Color CalculateSurfaceRadiance(const Vector3d&, const IntersectionInfo&) = 0;
    Color SampleRay(Ray& ray, Vector3d& Normal, double& areaPdf, double& anglePdf) const;
    //  virtual void SamplePoint(Vector3d& point, Vector3d& Normal) const = 0;

    double Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    double Pdf(const IntersectionInfo& info, const Vector3d& out) const;
    Color GetIntensity() const;

    Color NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lightPoint, Vector3d& lightNormal, int sample) const;
    Color NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info, int sample) const;
    Color DirectHitMIS(const Renderer* renderer,
                               const IntersectionInfo& lastInfo,
                               const IntersectionInfo& thisInfo, int component) const;

    void SamplePointHemisphere(const Vector3d& apex, Vector3d& point, Vector3d& normal) const;

    void SamplePoint (Vector3d& point, Vector3d& normal) const;
    double GetArea() const;
    void AddToScene(std::shared_ptr<Scene>);

    void Save(Bytestream& s) const;
    void Load(Bytestream& s);

    static UniformEnvironmentLight* Create(unsigned char);
protected:
    friend class Scene;
    double radius;
    Color intensity;
    Vector3d position;
    std::shared_ptr<Scene> scene;
    mutable Random random;
};

#endif
