#include "ThinLensCamera.h"
#include "Bytestream.h"

//------------------------------------------------------------------------------
// Constructor.
//------------------------------------------------------------------------------
ThinLensCamera::ThinLensCamera()
{
}

//------------------------------------------------------------------------------
// Constructor.
//------------------------------------------------------------------------------
ThinLensCamera::ThinLensCamera(Vector3d up, Vector3d pos, Vector3d dir, int xres, int yres, float fov, float focalLength, float lensRadius) 
    : Camera(up, pos, dir, xres, yres, fov), focalLength(focalLength), lensRadius(lensRadius)
{
    random.Seed(GetTickCount());
}

//------------------------------------------------------------------------------
// Destructor.
//------------------------------------------------------------------------------
ThinLensCamera::~ThinLensCamera()
{
}

//------------------------------------------------------------------------------
// Returns the ray originating on pixel (x, y) with internal pixel coordinate
// (a, b) that was refracted through polar coordinates (u, v) on the lens.
//------------------------------------------------------------------------------
Ray ThinLensCamera::GetRayFromPixel(int x, int y, float a, float b, float u, float v) const
{
    float rx = halfwidth*(2.f*float(x) - float(xres) + (2.f*a)) / float(xres);
    float ry = halfwidth*(2.f*float(y) - float(yres) + (2.f*b)) / float(xres);
    
    Vector3d right = dir^up;
    right.Normalize();
    Vector3d raydir = dir - (up*ry - right*rx);
    raydir.Normalize();
    Ray centerRay = Ray(pos, raydir);

    Vector3d lensPoint = pos+lensRadius*v*(right*cos(u)+up*sin(u));

    centerRay.direction.Normalize();

    Ray outRay = Ray(lensPoint, pos + focalLength*centerRay.direction/(centerRay.direction*dir) - lensPoint);
    outRay.direction.Normalize();

    return outRay;
}

//------------------------------------------------------------------------------
// Returns the pixel coordinates that a ray strikes on the pinhole camera film
// plane. The ray is assumed to always go through the pinhole: the function
// returns false only if the ray does not hit the film plane (screen), or if
// it starts behind the camera or heads away from the camera.
//------------------------------------------------------------------------------
bool ThinLensCamera::GetPixelFromRay(const Ray& ray, int& x, int& y, float u, float v) const
{
    if(ray.direction*dir > 0) // Ray shooting away from camera
        return false;

    if((ray.origin-pos) * dir < 0) // Ray origin behind camera
        return false;

    Vector3d right = dir^up;
    Vector3d lensPoint = pos+lensRadius*v*(right*cos(u)+up*sin(u));

    Vector3d toCam = lensPoint - ray.origin;
    //float camRayLength = toCam.GetLength();
    toCam.Normalize();
    toCam *= focalLength/(-toCam*dir);
    Vector3d focalPoint = lensPoint - toCam;
    Ray centerRay(focalPoint, pos - focalPoint);
    Ray camRay = ray;

    camRay.direction.Normalize();
    centerRay.direction.Normalize();

    float ratio = (float)yres/(float)xres;
    Vector3d left = up^dir;
    left.Normalize();

    Vector3d A = left^up;
    Vector3d B = dir^centerRay.direction;

    float det = centerRay.direction*(A);
    float rx = 1/halfwidth*up*B/det;
    float ry = 1/halfwidth*-left*B/det;

    x = (int)((float)xres*(1.0f - rx)/2.0f);
    y = (int)((float)yres*(ratio - ry)/(ratio*2.0f));

    if(x < 0 || x >= xres || y < 0 || y >= yres)
        return false;

    return true;
}


//------------------------------------------------------------------------------
// Since the aperture is a finite point, this function will always return (0,0).
//------------------------------------------------------------------------------
void ThinLensCamera::SampleAperture(Vector3d& pos, float& u, float& v) const
{
    u = random.GetFloat(0, 2*F_PI);
    v = sqrt(random.GetFloat(0, 1));

    pos = this->pos + lensRadius*v*(up*sin(u) + (dir^up)*cos(u));
}

//------------------------------------------------------------------------------
// Sets the focal length of the camera.
//------------------------------------------------------------------------------
void ThinLensCamera::SetFocalLength(float focalLength)
{
    assert(focalLength > 0);
    this->focalLength = focalLength;
}

//------------------------------------------------------------------------------
// Sets the lens of the camera.
//------------------------------------------------------------------------------
void ThinLensCamera::SetLensRadius(float lensRadius)
{
    assert(lensRadius > 0);
    this->lensRadius = lensRadius;
}

void ThinLensCamera::Save(Bytestream& stream) const
{
    stream << ID_THINLENSCAMERA << pos << dir << up << halfwidth << xres << yres 
           << focalLength << lensRadius;
}

void ThinLensCamera::Load(Bytestream& stream)
{
    stream >> pos >> dir >> up >> halfwidth >> xres >> yres 
           >> focalLength >> lensRadius;
}