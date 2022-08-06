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
ThinLensCamera::ThinLensCamera(Vector3d up, Vector3d pos, Vector3d dir, int xres, int yres, double fov, double focalLength, double lensRadius) 
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
Ray ThinLensCamera::GetRayFromPixel(int x, int y, double a, double b, double u, double v) const
{
    double rx = halfwidth*(2.f*double(x) - double(xres) + (2.f*a)) / double(xres);
    double ry = halfwidth*(2.f*double(y) - double(yres) + (2.f*b)) / double(xres);
    
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
    Vector3d left = up^dir;
    left.Normalize();

    Vector3d A = left^up;
    Vector3d B = dir^centerRay.direction;

    double det = centerRay.direction*(A);
    double rx = 1/halfwidth*up*B/det;
    double ry = 1/halfwidth*-left*B/det;

    double x = ((double)xres*(1.0f - rx)/2.0f);
    double y = ((double)yres*(ratio - ry)/(ratio*2.0f));

    if(x < 0 || x >= xres || y < 0 || y >= yres)
        return { false, 0, 0 };

    return { true, (int)x, (int)y };
}


//------------------------------------------------------------------------------
// Randomly samples the camera aperture
//------------------------------------------------------------------------------
std::tuple<double, double, Vector3d> ThinLensCamera::SampleAperture() const
{
    double u = random.GetDouble(0, 2*F_PI);
    double v = sqrt(random.GetDouble(0, 1));

    Vector3d position = this->pos + lensRadius*v*(up*sin(u) + (dir^up)*cos(u));
    return { u, v, position };
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