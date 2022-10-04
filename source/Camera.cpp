#include "Camera.h"
#include "ThinLensCamera.h"
#include "PinholeCamera.h"
#include "Bytestream.h"
#include <cassert>

/**
 * Constructor.
 */
Camera::Camera()
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
 */
Camera::Camera(Vector3d aup, Vector3d apos, Vector3d adir, int xres, int yres, double fov) 
    : up(aup), pos(apos), dir(adir), xres(xres), yres(yres)
{
    SetFov(fov);
    dir.Normalize();
    up = (dir^up)^dir;
    up.Normalize();
}

/**
 * Destructor.
 */
Camera::~Camera()
{
}

/**
 * Sets the field of view of the camera.
 * 
 * @param fov The horizontal field of view in degrees.
 */
void Camera::SetFov(double fov)
{
    assert(fov > 0.0 && fov < 180.0);
    halfwidth = tan(3.14159*fov/360.0);
}

/**
 * Returns the area of a pixel on the film plane.
 * 
 * @returns The area of a pixel on the film plane.
 */
double Camera::GetPixelArea() const
{
    return halfwidth*halfwidth*4.0*((double)yres/double(xres))/double(xres*yres);
}

/**
 * Returns the area of the film plane.
 * 
 * @returns The area of the film plane.
 */
double Camera::GetFilmArea() const
{
    return (double)yres/(double)xres*halfwidth*halfwidth*4.0;
}

/**
 * Returns the horizontal resolution of the camera.
 * 
 * @returns The horizontal resolution in pixels of the camera.
 */
int Camera::GetXRes() const
{
    return xres;
}

/**
 * Returns the vertical resolution of the camera.
 * 
 * @returns The vertical resolution in pixels of the camera.
 */
int Camera::GetYRes() const
{
    return yres;
}

/**
 * Creates a camera given an id (see Bytestream.h).
 * 
 * @param id The id of the camera type.
 * @returns A pointer to the created camera.
 */
Camera* Camera::Create(unsigned char id)
{
    switch(id)
    {
    case ID_PINHOLECAMERA:
        return new PinholeCamera;
        break;
    case ID_THINLENSCAMERA:
        return new ThinLensCamera;
        break;
    default:
        __debugbreak();
        return nullptr;
    }
}