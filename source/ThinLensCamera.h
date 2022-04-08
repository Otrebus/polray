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
    ThinLensCamera(Vector3d up, Vector3d pos, Vector3d dir, int xres, int yres, float fov, float focalLength, float lensRadius);
    ~ThinLensCamera();
    
    Ray GetRayFromPixel(int x, int y, float a, float b, float u, float v) const;
    bool GetPixelFromRay(const Ray& ray, int& x, int& y, float u, float v) const;

    void SampleAperture(Vector3d& pos, float& u, float& v) const;

    void SetFov(float fov);
    void SetFocalLength(float focalLength);
    void SetLensRadius(float lensRadius);

    float GetPixelArea() const;

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    float focalLength;
    float lensRadius;

    mutable Random random;
};

#endif