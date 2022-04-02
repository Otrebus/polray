#include "Camera.h"
#include "ThinLensCamera.h"
#include "PinholeCamera.h"
#include <intrin.h>

//------------------------------------------------------------------------------
// Constructor.
//------------------------------------------------------------------------------
Camera::Camera()
{
}

//------------------------------------------------------------------------------
// Constructor.
//------------------------------------------------------------------------------
Camera::Camera(Vector3d aup, Vector3d apos, Vector3d adir, int xres, int yres, float fov) 
    : up(aup), pos(apos), dir(adir), xres(xres), yres(yres)
{
    SetFov(fov);
    dir.Normalize();
    up = (dir^up)^dir;
    up.Normalize();
}

//------------------------------------------------------------------------------
// Destructor.
//------------------------------------------------------------------------------
Camera::~Camera()
{
}

//------------------------------------------------------------------------------
// Expands the film plane to fit the given field of view argument.
//------------------------------------------------------------------------------
void Camera::SetFov(float fov)
{
    assert(fov > 0.0f && fov < 180.0f);
    halfwidth = tan(3.14159f*fov/360.0f);
}

//------------------------------------------------------------------------------
// Returns the area of a pixel on the film plane.
//------------------------------------------------------------------------------
float Camera::GetPixelArea() const
{
	return halfwidth*halfwidth*4.0f*((float)yres/float(xres))/float(xres*yres);
}

int Camera::GetXRes() const
{
    return xres;
}

int Camera::GetYRes() const
{
    return yres;
}

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
    }
}