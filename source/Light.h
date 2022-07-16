#ifndef LIGHT_H
#define LIGHT_H

#include "vector3d.h"
#include "color.h"
#include "intersectioninfo.h"
#include "EmissiveMaterial.h"
#include "Utils.h"
#include <memory>
#include <vector>

#define ID_AREALIGHT ((unsigned char)200)
#define ID_SPHERELIGHT ((unsigned char)201)
#define ID_LIGHTPORTAL ((unsigned char)202)
#define ID_UNIFORMENVIRONMENTLIGHT ((unsigned char)203)
#define ID_MESHLIGHT ((unsigned char)204)

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
    virtual std::tuple<Ray, Color, Vector3d, AreaPdf, AnglePdf> SampleRay() const = 0;

    virtual double Intersect(const Ray& ray) const = 0;
    virtual bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const = 0;

    virtual double Pdf(const IntersectionInfo& info, const Vector3d& out) const = 0;
    virtual Color GetIntensity() const;

    virtual Color NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lightPoint, Vector3d& lightNormal, int sample) const = 0;

    virtual std::tuple<Vector3d, Vector3d> SamplePoint() const = 0;
    virtual double GetArea() const = 0;
    virtual void AddToScene(Scene* scene) = 0;

    virtual void Save(Bytestream& s) const = 0;
    virtual void Load(Bytestream& s) = 0;

    static Light* Create(unsigned char);

    EmissiveMaterial* material;
    Scene* scene;
protected:
    Color intensity_;
};

#endif