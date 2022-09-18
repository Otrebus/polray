#pragma once

#include "Camera.h"
#include <tuple>
#include "Randomizer.h"
#include "Bytestream.h"
#include "Vector3d.h"

class Ray;

class Camera
{
public:
    Camera();
    Camera(Vector3d up, Vector3d pos, Vector3d dir, int xres, int yres, double fov);
    virtual ~Camera();
    
    virtual Ray GetRayFromPixel(int x, int y, double a, double b, double u, double v) const = 0;
    virtual std::tuple<bool, int, int> GetPixelFromRay(const Ray& ray, double u, double v) const = 0;
    virtual std::tuple<double, double, Vector3d> SampleAperture() const = 0;

    void SetFov(double fov);

    double GetPixelArea() const;
    double GetFilmArea() const;
    int GetXRes() const;
    int GetYRes() const;

    static Camera* Create(unsigned char n);

    virtual void Save(Bytestream& stream) const = 0;
    virtual void Load(Bytestream& stream) = 0;

    Vector3d up, pos, dir;  // Orientation of the camera
    double halfwidth;       // Size of the film plane (plane is always 1 unit behind the camera)
    int xres, yres;         // Pixel resolution of the film plane
};
