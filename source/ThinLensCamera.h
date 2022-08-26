#pragma once

#include <assert.h>
#include <Windows.h>

#include "Camera.h"
#include "Random.h"
#include "Ray.h"
#include "Vector3d.h"

class ThinLensCamera : public Camera
{
public:
    ThinLensCamera();
    ThinLensCamera(Vector3d up, Vector3d pos, Vector3d dir, int xres, int yres, double fov, double focalLength, double lensRadius);
    ~ThinLensCamera();
    
    Ray GetRayFromPixel(int x, int y, double a, double b, double u, double v) const;
    std::tuple<bool, int, int> GetPixelFromRay(const Ray& ray, double u, double v) const;

    std::tuple<double, double, Vector3d> SampleAperture() const;

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    double focalLength;
    double lensRadius;

    mutable Random random;
};
