/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file ThinLensCamera.cpp
 * 
 * Implementation of the ThinLensCamera class.
 */

#include "ThinLensCamera.h"
#include "Bytestream.h"
#include "Utils.h"
#include "Ray.h"

/**
 * Constructor.
 */
ThinLensCamera::ThinLensCamera()
{
}

/**
 * Constructor.
 * 
 * @param aup The up vector.
 * @param apos The position of the camera.
 * @param adir The direction the camera points in.
 * @param xres The horizontal resolution of the camera.
 * @param yres The vertical resolution of the camera.
 * @param fov The horizontal field of view of the camera.
 * @param focalLength The focal length of the camera.
 * @param lensRadius The radius of the lens.
 */
ThinLensCamera::ThinLensCamera(const Vector3d& up, const Vector3d& pos, const Vector3d& dir, int xres, int yres, double fov, double focalLength, double lensRadius) 
    : Camera(up, pos, dir, xres, yres, fov), focalLength(focalLength), lensRadius(lensRadius)
{
    #ifdef DETERMINISTIC
    random.Seed(0);
    #endif
}

/**
 * Destructor.
 */
ThinLensCamera::~ThinLensCamera()
{
}

/**
 * Returns the ray that starts from pixel (x, y) with internal pixel coordinate (a, b)
 * that goes through the camera pinhole.
 * 
 * @param x The horizontal component of the pixel coordinate.
 * @param y The vertical component of the pixel coordinate.
 * @param a The horizontal component of the coordinate inside the pixel.
 * @param b The vertical component of the coordinate inside the pixel.
 * @returns Whether the ray is in front of the camera, and the coordinate on the film plane
 *          that it hits.
 */
Ray ThinLensCamera::GetRayFromPixel(int x, int y, double a, double b, double u, double v) const
{
    double rx = halfwidth*(2.f*double(x) - double(xres) + (2.f*a)) / double(xres);
    double ry = halfwidth*(2.f*double(y) - double(yres) + (2.f*b)) / double(xres);
    
    Vector3d right = dir^up;
    right.Normalize();
    Vector3d raydir = dir - (up*ry - right*rx);
    raydir.Normalize();
    Ray centerRay = Ray(pos, raydir);

    Vector3d lensPoint = SampleAperture(u, v);

    centerRay.direction.Normalize();

    Ray outRay = Ray(lensPoint, pos + focalLength*centerRay.direction/(centerRay.direction*dir) - lensPoint);
    outRay.direction.Normalize();

    return outRay;
}

/**
 * Returns the pixel coordinates that a ray strikes on the camera film plane.
 * 
 * @param ray The ray that strikes the camera.
 * @param u The parametric angle on the lens in [0, 1].
 * @param v The parametric radius on the lens in [0, 1].
 * @returns Whether the ray is in front of the camera, and the coordinate on the film plane
 *          that it hits.
 */
std::tuple<bool, int, int> ThinLensCamera::GetPixelFromRay(const Ray& ray, double u, double v) const
{
    if(ray.direction*dir > 0) // Ray shooting away from camera
        return { false, 0, 0 };

    if((ray.origin-pos) * dir < 0) // Ray origin behind camera
        return { false, 0, 0 };

    Vector3d right = dir^up;
    Vector3d lensPoint = pos+lensRadius*v*(right*cos(u)+up*sin(u));

    Vector3d toCam = lensPoint - ray.origin;
    //double camRayLength = toCam.GetLength();
    toCam.Normalize();
    toCam *= focalLength/(-toCam*dir);
    Vector3d focalPoint = lensPoint - toCam;
    Ray centerRay(focalPoint, pos - focalPoint);
    Ray camRay = ray;

    camRay.direction.Normalize();
    centerRay.direction.Normalize();

    double ratio = (double)yres/(double)xres;
    Vector3d leftNode = up^dir;
    leftNode.Normalize();

    Vector3d A = leftNode^up;
    Vector3d B = dir^centerRay.direction;

    double det = centerRay.direction*(A);
    double rx = 1/halfwidth*up*B/det;
    double ry = 1/halfwidth*-leftNode*B/det;

    double x = ((double)xres*(1.0 - rx)/2.0);
    double y = ((double)yres*(ratio - ry)/(ratio*2.0));

    if(x < 0 || x >= xres || y < 0 || y >= yres)
        return { false, 0, 0 };

    return { true, (int)x, (int)y };
}


/**
 * Samples a point on the aperture.
 * 
 * @returns The parametric coordinates of the sampled point, and the position vector of the point.
 */
Vector3d ThinLensCamera::SampleAperture(double u, double v) const
{
    return this->pos + lensRadius*v*(up*sin(u*2*pi) + (dir^up)*cos(u*2*pi));
}

/**
 * Loads the camera from a bytestream.
 * 
 * @param stream The stream to serialize from.
 */
void ThinLensCamera::Save(Bytestream& stream) const
{
    stream << ID_THINLENSCAMERA << pos << dir << up << halfwidth << xres << yres 
           << focalLength << lensRadius;
}

/**
 * Saves the camera to a bytestream.
 * 
 * @param stream The bytestream to serialize to.
 */
void ThinLensCamera::Load(Bytestream& stream)
{
    stream >> pos >> dir >> up >> halfwidth >> xres >> yres 
           >> focalLength >> lensRadius;
}
