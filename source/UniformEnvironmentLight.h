#ifndef UNIFORMENVIRONMENTLIGHT_H
#define UNIFORMENVIRONMENTLIGHT_H

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

    Color SampleRay(Ray& ray, Vector3d& Normal, double& areaPdf, double& anglePdf) const;
     
    Vector2d point_on_triangle(Vector2d, Vector2d, Vector2d) const;
    double Intersect(const Ray& ray) const;
    bool GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const;

    double Pdf(const IntersectionInfo& info, const Vector3d& out) const;
    Color GetIntensity() const;

    Color NextEventEstimation(const Renderer* renderer, const IntersectionInfo& info, Vector3d& lightPoint, Vector3d& lightNormal, int sample) const;
    Color NextEventEstimationMIS(const Renderer* renderer, const IntersectionInfo& info, int sample) const;
    Color DirectHitMIS(const Renderer* renderer,
                               const IntersectionInfo& lastInfo,
                               const IntersectionInfo& thisInfo, int component) const;


    void SamplePoint (Vector3d& point, Vector3d& normal) const;
    double GetArea() const;
    void AddToScene(std::shared_ptr<Scene>);

    void Save(Bytestream& s) const;
    void Load(Bytestream& s);

    static UniformEnvironmentLight* Create(unsigned char);
protected:

    std::tuple<std::vector<Vector2d>, double, Vector3d, Vector3d, Vector3d> GetProjectedSceneHull(Ray& ray, Vector3d normal) const;
    friend class Scene;
    double radius;
    Color intensity;
    Vector3d position;
    std::shared_ptr<Scene> scene;
    mutable Random random;
};

#endif
