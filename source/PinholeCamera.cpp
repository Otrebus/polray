#include "PinholeCamera.h"
#include "Bytestream.h"
#include "Ray.h"

/**
 * Constructor.
 */
PinholeCamera::PinholeCamera()
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
PinholeCamera::PinholeCamera(const Vector3d& up, const Vector3d& pos, const Vector3d& dir, int xres, int yres, double fov) 
    : Camera(up, pos, dir, xres, yres, fov)
{
}

/**
 * Destructor.
 */
PinholeCamera::~PinholeCamera()
{
}

/**
 * Returns the ray that starts from pixel (x, y) with internal pixel coordinate (a, b)
 * that goes through the camera pinhole.
 * 
 * @param x The horizontal component of the pixel coordinate.
 * @param y The vertical component of the pixel coordinate.
 * @param a The horizontal component of the coordinate inside the pixel.
 * @param a The vertical component of the coordinate inside the pixel.
 * @returns Whether the ray is in front of the camera, and the coordinate on the film plane
 *          that it hits.
 */
Ray PinholeCamera::GetRayFromPixel(int x, int y, double a, double b, double, double) const
{
    double rx = halfwidth*(2.f*double(x) - double(xres) + (2.f*a)) / double(xres);
    double ry = halfwidth*(2.f*double(y) - double(yres) + (2.f*b)) / double(xres);
    
    Vector3d leftNode = up^dir;
    leftNode.Normalize();
    Vector3d raydir = dir - (up*ry + leftNode*rx);
    raydir.Normalize();
    return Ray(pos, raydir);
}

/**
 * Returns the pixel coordinates that a ray strikes on the pinhole camera film plane. The ray
 * is assumed to always go through the pinhole: the function returns false only if the ray
 * does not hit the film plane (screen), or if it starts behind the camera or heads away from
 * the camera.
 * 
 * @param ray The ray that strikes the camera.
 * @returns Whether the ray is in front of the camera, and the coordinate on the film plane
 *          that it hits.
 */
std::tuple<bool, int, int> PinholeCamera::GetPixelFromRay(const Ray& ray, double, double) const
{
    if(ray.direction*dir > 0) // Ray shooting away from camera
        return { false, 0, 0 };

    if((ray.origin-pos) * dir < 0) // Ray origin behind camera
        return { false, 0, 0 };

    double ratio = (double)yres/(double)xres;
    Vector3d leftNode = up^dir;
    leftNode.Normalize();

    Vector3d A = leftNode^up;
    Vector3d B = dir^ray.direction;

    double det = ray.direction*(A);
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
Vector3d PinholeCamera::SampleAperture(double, double) const
{
    return this->pos;
}

/**
 * Saves the camera to a bytestream.
 * 
 * @param stream The bytestream to serialize to.
 */
void PinholeCamera::Save(Bytestream& stream) const
{
    stream << ID_PINHOLECAMERA << pos << dir << up << halfwidth << xres << yres;
}

/**
 * Loads the camera from a bytestream.
 * 
 * @param stream The bytestream to deserialize from.
 */
void PinholeCamera::Load(Bytestream& stream)
{
    stream >> pos >> dir >> up >> halfwidth >> xres >> yres;
}