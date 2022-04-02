#ifndef LIGHT_H
#define LIGHT_H

#include "vector3d.h"
#include "color.h"
#include "intersectioninfo.h"
#include <memory>
#include <vector>

#define ID_AREALIGHT ((unsigned char)200)
#define ID_SPHERELIGHT ((unsigned char)201)

class Bytestream;
class Scene;
class Ray;
class Color;
class Renderer;

class LightPortal
{
    LightPortal(Vector3d pos, Vector3d v1, Vector3d v2) : pos(pos), v1(v1), v2(v2) {}
    float GetArea() const { return (v1^v2).GetLength(); }
    Vector3d GetNormal() const { Vector3d n = v1^v2; n.Normalize(); return n; }
    Vector3d pos, v1, v2;
    friend class Light;
    friend class AreaLight;
    friend class SphereLight;
    friend class MeshLight;
};

class Light
{
public:
    Light();
    Light(Color intensity);
    virtual ~Light() {}
	//virtual Color CalculateSurfaceRadiance(const Vector3d&, const IntersectionInfo&) = 0;
	virtual Color SampleRay(Ray& ray, Vector3d& Normal, float& areaPdf, float& anglePdf) const = 0;
  //  virtual void SamplePoint(Vector3d& point, Vector3d& Normal) const = 0;

    virtual float Pdf(const IntersectionInfo& info, const Vector3d& out) const = 0;
    Color GetIntensity() const;

    virtual Color NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, unsigned int component) const = 0;
    virtual Color NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info, unsigned int component) const = 0;
    virtual Color DirectHitMIS(const Renderer* renderer, 
                               const IntersectionInfo& lastInfo, 
                               const IntersectionInfo& thisInfo, 
                               unsigned int component) const = 0;

	//virtual Vector3d SamplePoint (Vector3d& point, Vector3d& normal) const = 0;
    virtual float GetArea() const = 0;
	virtual void AddToScene(std::shared_ptr<Scene>) = 0;

    virtual void Save(Bytestream& s) const = 0;
    virtual void Load(Bytestream& s) = 0;

    static Light* Create(unsigned char);

    void AddPortal(const Vector3d& pos, const Vector3d& v1, const Vector3d& v2);

protected:
	Color intensity_;
    std::vector<LightPortal> portals;
};

#endif