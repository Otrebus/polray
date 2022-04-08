/*#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"
#include "Ray.h"
#include "Scene.h"

class PointLight : public Light
{
public:
    PointLight(const Vector3d&, const Color&, Scene*);
    ~PointLight();
    //Color CalculateSurfaceRadiance(const Vector3d&, const IntersectionInfo&);

    Color Sample(Vector3d& ray) const;

    //Color SamplePoint (const Vector3d& frompoint, Vector3d& point, Vector3d& normal) const;

    void AddToScene(Scene& scn);
    Vector3d pos;
    Color intensity;
    Scene* scene;
};

#endif*/