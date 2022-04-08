#ifndef COLOR_H
#define COLOR_H

#define WIN32_MEAN_AND_LEAN
#include <assert.h>
#include <math.h>
#include "Vector3d.h"
#include <stdlib.h>
#include <limits>

class Color
{
public:
    Color(float, float, float);
    //Color(const Vector3d&);
    Color(int);
    Color();
    int GetInt() const;
    ~Color();
    Color operator+(const Color& v) const;
    Color operator-(const Color& v) const;
    Color operator*(float) const;
    Color operator/(float) const;
    Color operator*(int) const;
    Color operator/(int) const;
    Color operator*(const Color& v) const;
    Color operator+=(const Color& c);
    Color operator*=(const Color& c);
    Color operator/=(float t);
    Color operator*=(float t);
    float& operator[](int);
    bool operator!() const;

    bool IsValid() const;

    void SetLuminance(float L);
    float GetLuminance() const;
    float GetIntensity() const;
    float GetAverage() const;
    float GetMax() const;
    float GetSum() const;

    void Normalize();

    float r;
    float g;
    float b;

    const static Color Identity;
    const static Color Black;

private:
    Color(float); // "Color(float) = delete" in public if using more C++11
};                // compliant compiler. This is to avoid accidentally 
                  // initializing a color with a float, which never makes
                  // sense. Maybe I should remove the int constructor too

Color operator*(float, const Color&);
Color operator*(int, const Color&);
std::ostream& operator << (std::ostream& s, const Color& v);

#endif