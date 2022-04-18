#ifndef CAMERA_H
#define CAMERA_H

#include "vector3d.h"
#include "Ray.h"
#include <assert.h>

#define ID_PINHOLECAMERA ((unsigned char)199)
#define ID_THINLENSCAMERA ((unsigned char)198)

class Bytestream;

class Camera
{
public:
    Camera();
    Camera(Vector3d up, Vector3d pos, Vector3d dir, int xres, int yres, float fov);
    virtual ~Camera();
    
    virtual Ray GetRayFromPixel(int x, int y, float a, float b, float u, float v) const = 0;
    virtual bool GetPixelFromRay(const Ray& ray, int& x, int& y, float u, float v) const = 0;
    virtual void SampleAperture(Vector3d& pos, float& u, float& v) const = 0;

    void SetFov(float fov);

    float GetPixelArea() const;
    float GetFilmArea() const;
    int GetXRes() const;
    int GetYRes() const;

    static Camera* Create(unsigned char n);

    virtual void Save(Bytestream& stream) const = 0;
    virtual void Load(Bytestream& stream) = 0;

    Vector3d up, pos, dir;  // Orientation of the camera
    float halfwidth;        // Size of the film plane (plane is always 1 unit behind the camera)
    int xres, yres;         // Pixel resolution of the film plane
};

#endif