/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Color.h
 * 
 * Declaration of the Color class.
 */

#pragma once

#define WIN32_MEAN_AND_LEAN
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ostream>

class Vector3d;

class Color
{
public:
    Color(double, double, double);
    Color(const Vector3d& v);

    explicit Color(int);
    Color();
    int GetInt() const;
    ~Color();
    Color operator+(const Color& v) const;
    Color operator-(const Color& v) const;
    Color operator*(double) const;
    Color operator/(double) const;
    Color operator*(int) const;
    Color operator/(int) const;
    Color operator*(const Color& v) const;
    Color operator+=(const Color& c);
    Color operator*=(const Color& c);
    Color operator/=(double t);
    bool operator==(const Color& c);
    Color operator*=(double t);
    explicit operator bool() const;
    double& operator[](int);
    bool operator!() const;
    bool IsValid() const;
    double GetLuma() const;

    double r, g, b;

    const static Color Identity, Black;
};

Color operator*(double, const Color&);
Color operator*(int, const Color&);
std::ostream& operator << (std::ostream& s, const Color& v);
