#pragma once

#include "Camera.h"
#include <tuple>
#include "Randomizer.h"

class Vector3d;

class PinholeCamera : public Camera
{
public:
    PinholeCamera();
    PinholeCamera(const Vector3d& up, const Vector3d& pos, const Vector3d& dir, int xres, int yres, double fov);
    ~PinholeCamera();
    
    Ray GetRayFromPixel(int x, int y, double a, double b, double u, double v) const;
    std::tuple<bool, int, int> GetPixelFromRay(const Ray& ray, double u, double v) const;
    Vector3d SampleAperture(double u, double v) const;
    
    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);
};
