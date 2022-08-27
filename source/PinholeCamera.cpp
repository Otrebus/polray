#include "PinholeCamera.h"
#include "Bytestream.h"

//------------------------------------------------------------------------------
// Constructor.
//------------------------------------------------------------------------------
PinholeCamera::PinholeCamera()
{
}

//------------------------------------------------------------------------------
// Constructor.
//------------------------------------------------------------------------------
PinholeCamera::PinholeCamera(const Vector3d& up, const Vector3d& pos, const Vector3d& dir, int xres, int yres, double fov) 
    : Camera(up, pos, dir, xres, yres, fov)
{
}

//------------------------------------------------------------------------------
// Destructor.
//------------------------------------------------------------------------------
PinholeCamera::~PinholeCamera()
{
}

//------------------------------------------------------------------------------
// Returns the ray that starts from pixel (x, y) with internal coordinate (a, b)
// 0 <= a,b <= 1 that goes through the camera pinhole.
//------------------------------------------------------------------------------
Ray PinholeCamera::GetRayFromPixel(int x, int y, double a, double b, double, double) const
{
    double rx = halfwidth*(2.f*double(x) - double(xres) + (2.f*a)) / double(xres);
    double ry = halfwidth*(2.f*double(y) - double(yres) + (2.f*b)) / double(xres);
    
    Vector3d left = up^dir;
    left.Normalize();
    Vector3d raydir = dir - (up*ry + left*rx);
    raydir.Normalize();
    return Ray(pos, raydir);
}

//------------------------------------------------------------------------------
// Returns the pixel coordinates that a ray strikes on the pinhole camera film
// plane. The ray is assumed to always go through the pinhole: the function
// returns false only if the ray does not hit the film plane (screen), or if
// it starts behind the camera or heads away from the camera.
//------------------------------------------------------------------------------
std::tuple<bool, int, int> PinholeCamera::GetPixelFromRay(const Ray& ray, double, double) const
{
    if(ray.direction*dir > 0) // Ray shooting away from camera
        return { false, 0, 0 };

    if((ray.origin-pos) * dir < 0) // Ray origin behind camera
        return { false, 0, 0 };

    double ratio = (double)yres/(double)xres;
    Vector3d left = up^dir;
    left.Normalize();

    Vector3d A = left^up;
    Vector3d B = dir^ray.direction;

    double det = ray.direction*(A);
    double rx = 1/halfwidth*up*B/det;
    double ry = 1/halfwidth*-left*B/det;

    double x = ((double)xres*(1.0f - rx)/2.0f);
    double y = ((double)yres*(ratio - ry)/(ratio*2.0f));

    if(x < 0 || x >= xres || y < 0 || y >= yres)
        return { false, 0, 0 };

    return { true, (int)x, (int)y };
}


//------------------------------------------------------------------------------
// Since the aperture is a finite point, this function will always return (0,0).
//------------------------------------------------------------------------------
std::tuple<double, double, Vector3d> PinholeCamera::SampleAperture() const
{
    return { 0, 0, this->pos };
}

void PinholeCamera::Save(Bytestream& stream) const
{
    stream << ID_PINHOLECAMERA << pos << dir << up << halfwidth << xres << yres;
}

void PinholeCamera::Load(Bytestream& stream)
{
    stream >> pos >> dir >> up >> halfwidth >> xres >> yres;
}