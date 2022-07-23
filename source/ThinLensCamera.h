#ifndef THINLENSCAMERA_H
#define THINLENSCAMERA_H

#include <assert.h>
#include <Windows.h>

#include "Camera.h"
#include "Random.h"
#include "Ray.h"
#include "Vector3d.h"

#ifndef F_PI
#define F_PI 3.14159265
#endif

class ThinLensCamera : public Camera
{
public:
    ThinLensCamera();
    ThinLensCamera(Vector3d up, Vector3d pos, Vector3d dir, int xres, int yres, double fov, double focalLength, double lensRadius);
    ~ThinLensCamera();
    
    Ray GetRayFromPixel(int x, int y, double a, double b, double u, double v) const;
    bool GetPixelFromRay(const Ray& ray, int& x, int& y, double u, double v) const;

    void SampleAperture(Vector3d& pos, double& u, double& v) const;

    void SetFocalLength(double focalLength);
    void SetLensRadius(double lensRadius);

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    double focalLength;
    double lensRadius;

    mutable Random random;
};

#endif
