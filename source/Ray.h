#ifndef RAY_H
#define RAY_H

#include "Vector3d.h"
#include "intersectioninfo.h"

class Ray
{
public:
    //Ray(Ray& r);
    Ray();
    Ray(const Vector3d&, const Vector3d&);
    Ray(const Vector3d&, const Vector3d&, IntersectionInfo);
    Ray(const Ray&);
    Ray& operator=(const Ray& r);
    ~Ray();

    double GetRefractiveIndex() const;
    Vector3d GetDirection() const;

    Vector3d origin;
    Vector3d direction;
    //double refractiveindex;
    //IntersectionInfo* previousintersect;
};

class LineSegment
{
public:
    LineSegment(Vector3d&, Vector3d&);
    ~LineSegment();

    bool CutXYPlane(double);
    bool CutYZPlane(double);
    bool CutXZPlane(double);

    Vector3d v0, v1;
};

#endif