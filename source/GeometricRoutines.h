#ifndef GEOMETRICROUTINES
#define GEOMETRICROUTINES

#include <vector>
#include "vector3d.h"

void ClipPolygonToAAP(int axis, int side, double position, std::vector<Vector3d>& input);
Vector3d Reflect(const Vector3d& incident, const Vector3d& normal);
double Refract(double n1, double n2, Vector3d& _normal, Vector3d& incident, Vector3d& refraction);
void MakeBasis(const Vector3d& givenVector, Vector3d& v2, Vector3d& v3);
void SampleHemisphereCos(double r1, double r2, const Vector3d& apex, Vector3d& sample);

#endif
