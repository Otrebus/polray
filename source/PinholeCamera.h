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
    bool GetPixelFromRay(const Ray& ray, int& x, int& y, double u, double v) const;

    void SetFov(double fov);
    void SampleAperture(Vector3d& pos, double& u, double& v) const;

    double GetPixelArea() const;
    double GetFilmArea() const;
    
    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);
};

#endif