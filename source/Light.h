#ifndef LIGHT_H
#define LIGHT_H

#include "vector3d.h"
#include "color.h"
#include "intersectioninfo.h"
#include "EmissiveMaterial.h"
#include <memory>
#include <vector>

#define ID_AREALIGHT ((unsigned char)200)
#define ID_SPHERELIGHT ((unsigned char)201)
#define ID_LIGHTPORTAL ((unsigned char)202)

class Bytestream;
class Scene;
class Ray;
class Color;
class Renderer;

class Light
{
public:
    Light();
    Light(Color intensity);
    virtual ~Light() {}
    //virtual Color CalculateSurfaceRadiance(const Vector3d&, const IntersectionInfo&) = 0;
    virtual Color SampleRay(Ray& ray, Vector3d& Normal, double& areaPdf, double& anglePdf) const = 0;
    //  virtual void SamplePoint(Vector3d& point, Vector3d& Normal) const = 0;

    virtual double Intersect(const Ray& ray) const = 0;
    virtual bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const = 0;

    virtual double Pdf(const IntersectionInfo& info, const Vector3d& out) const = 0;
    virtual Color GetIntensity() const;

    virtual Color NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lightPoint, Vector3d& lightNormal, int sample) const = 0;
    virtual Color NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info, int sample) const = 0;
    virtual Color DirectHitMIS(const Renderer* renderer,
                               const IntersectionInfo& lastInfo,
                               const IntersectionInfo& thisInfo, int component) const = 0;

    virtual void SamplePoint (Vector3d& point, Vector3d& normal) const = 0;
    virtual double GetArea() const = 0;
    virtual void AddToScene(std::shared_ptr<Scene>) = 0;

    virtual void Save(Bytestream& s) const = 0;
    virtual void Load(Bytestream& s) = 0;

    static Light* Create(unsigned char);

    EmissiveMaterial* material;
protected:
    Color intensity_;
};

#endif