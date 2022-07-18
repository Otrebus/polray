#ifndef GEOMETRICROUTINES
#define GEOMETRICROUTINES

#include <vector>
#include "vector3d.h"
#include "Ray.h"

void ClipPolygonToAAP(int axis, bool side, double position, std::vector<Vector3d>& input);
Vector3d Reflect(const Vector3d& incident, const Vector3d& normal);
std::tuple<Vector3d, Vector3d> MakeBasis(const Vector3d& givenVector);
Vector3d SampleHemisphereCos(double r1, double r2, const Vector3d& apex);
Vector3d SampleHemisphereUniform(double r1, double r2, const Vector3d& apex);
Vector3d SampleSphereUniform(double r1, double r2);
double IntersectSphere(const Vector3d& position, double radius, const Ray& ray);
std::vector<Vector2d> convexHull(std::vector<Vector2d> v);

#endif
