#pragma once

#include "Camera.h"
#include <tuple>
#include "Random.h"

class Vector3d;

class ThinLensCamera : public Camera
{
public:
    ThinLensCamera();
    ThinLensCamera(const Vector3d& up, const Vector3d& pos, const Vector3d& dir, int xres, int yres, double fov, double focalLength, double lensRadius);
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
