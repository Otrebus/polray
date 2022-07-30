#ifndef PINHOLECAMERA_H
#define PINHOLECAMERA_H

#include "Vector3d.h"
#include "Camera.h"
#include "Ray.h"
#include <assert.h>

class PinholeCamera : public Camera
{
public:
    PinholeCamera();
    PinholeCamera(Vector3d up, Vector3d pos, Vector3d dir, int xres, int yres, double fov);
    ~PinholeCamera();
    
    Ray GetRayFromPixel(int x, int y, double a, double b, double u, double v) const;
    std::tuple<bool, int, int> GetPixelFromRay(const Ray& ray, double u, double v) const;
    std::tuple<double, double, Vector3d> SampleAperture() const;
    
    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);
};

#endif