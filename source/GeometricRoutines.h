/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file GeometricRoutines.h
 * 
 * Declaration of functions for various geometric routines.
 */

#pragma once

#define NOMINMAX
#include <vector>

class Vector3d;
class Vector2d;
class Ray;

std::vector<Vector2d> ConvexHull(std::vector<Vector2d> v);
std::vector<Vector3d> ClipPolygonToAAP(int axis, bool side, double position, std::vector<Vector3d>& input);

std::tuple<Vector3d, Vector3d> MakeBasis(const Vector3d& givenVector);

Vector3d Reflect(const Vector3d& incident, const Vector3d& normal);

Vector3d SampleHemisphereCos(double r1, double r2, const Vector3d& apex);
Vector3d SampleHemisphereUniform(double r1, double r2, const Vector3d& apex);
Vector3d SampleSphereUniform(double r1, double r2);

double IntersectSphere(const Vector3d& position, double radius, const Ray& ray);
std::tuple<double, double, double> IntersectTriangle(const Vector3d& v0, const Vector3d& v1, const Vector3d& v2, const Ray& ray);
