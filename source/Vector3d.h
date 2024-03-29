/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Vector3d.h
 * 
 * Declaration of the Vector3d class.
 */

#pragma once

#include <cmath>
#include <sstream>

class Matrix3d;

class Vector3d
{
public:
    Vector3d(double x, double y, double z);
    ~Vector3d();
    Vector3d();

    Vector3d& operator=(const Vector3d&);
    Vector3d operator+(const Vector3d&) const;
    Vector3d operator-(const Vector3d&) const;
    Vector3d operator-() const;

    Vector3d operator+=(const Vector3d& v);
    Vector3d operator-=(const Vector3d& v);

    Vector3d operator*=(const Matrix3d& m);
    bool operator!() const;
    bool operator!=(const Vector3d&) const;
    bool operator==(const Vector3d&) const;
    double operator*(const Vector3d&) const;
    Vector3d operator^(const Vector3d&)	const;
    Vector3d operator/(double) const;
    Vector3d operator/=(double);
    Vector3d operator*=(double);
    Vector3d operator*(double) const;
    double& operator[](int);
    double operator[](int) const;
    
    double Length() const;
    double Length2() const;
    void Normalize();
    Vector3d Normalized() const;
    bool IsValid() const;

    double x, y, z;
};

class Vector2d
{
public:
    Vector2d(double x, double y);
    ~Vector2d();
    Vector2d();

    Vector2d& operator=(const Vector2d&);
    Vector2d operator+(const Vector2d&) const;
    Vector2d operator-(const Vector2d&) const;
    Vector2d operator*(double) const;
    double operator^(const Vector2d&) const;
    bool operator<(const Vector2d&) const;

    double operator*(const Vector2d&) const;

    double Length() const;
    void Normalize();
    Vector2d Normalized() const;

    double x, y;
};

Vector3d operator*(double t, const Vector3d& v);
Vector2d operator*(double t, const Vector2d& v);
std::ostream& operator<<(std::ostream& str, const Vector3d& v);
