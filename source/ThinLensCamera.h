/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file ThinLensCamera.h
 * 
 * Declaration of the ThinLensCamera class.
 */

#pragma once

#include "Camera.h"
#include <tuple>
#include "Randomizer.h"

class Vector3d;

class ThinLensCamera : public Camera
{
public:
    ThinLensCamera();
    ThinLensCamera(const Vector3d& up, const Vector3d& pos, const Vector3d& dir, int xres, int yres, double fov, double focalLength, double lensRadius);
    ~ThinLensCamera();
    
    Ray GetRayFromPixel(int x, int y, double a, double b, double u, double v) const;
    std::tuple<bool, int, int> GetPixelFromRay(const Ray& ray, double u, double v) const;

    Vector3d SampleAperture(double u, double v) const;

    void Save(Bytestream& stream) const;
    void Load(Bytestream& stream);

    double focalLength;
    double lensRadius;
};
