#ifndef COLOR_H
#define COLOR_H

#define WIN32_MEAN_AND_LEAN
#include <assert.h>
#include <math.h>
#include "Vector3d.h"
#include <stdlib.h>

class Color
{
public:
    Color(double, double, double);
    //Color(const Vector3d&);
    Color(int);
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
    Color operator*=(double t);
    double& operator[](int);
    bool operator!() const;

    bool IsValid() const;

    void SetLuminance(double L);
    double GetLuminance() const;
    double GetIntensity() const;
    double GetAverage() const;
    double GetMax() const;
    double GetSum() const;

    double r;
    double g;
    double b;

    const static Color Identity;
    const static Color Black;

private:
    Color(double); // "Color(double) = delete" in public if using more C++11
};                // compliant compiler. This is to avoid accidentally 
                  // initializing a color with a double, which never makes
                  // sense. Maybe I should remove the int constructor too

Color operator*(double, const Color&);
Color operator*(int, const Color&);
std::ostream& operator << (std::ostream& s, const Color& v);

#endif