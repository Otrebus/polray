/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Color.cpp
 * 
 * Implementation of the Color class which represents a quantity of light.
 */

#include "Color.h"
#include "Vector3d.h"
#include "Utils.h"

const Color Color::Identity = Color(1, 1, 1);
const Color Color::Black = Color(0, 0, 0);

/**
 * Constructor.
 */
Color::Color()
{
}

/**
 * Constructor. Turns a binary representation into a color object.
 * 
 * @param c The 0x00RRGGBB integer represenation of the color.
 */
Color::Color(int c)
{
    r = (double)((c >> 16) & 0xFF)/255.0;
    g = (double)((c >> 8) & 0xFF)/255.0;
    b = (double)(c & 0xFF)/255.0;
    assert(r >= 0 && g >= 0 && b >= 0 && r == r && g == g && b == b);
}

/**
 * Constructor.
 * 
 * @param r The red component of the color.
 * @param g The green component of the color.
 * @param b The blue component of the color.
 */
Color::Color(double r, double g, double b) : r(r), g(g), b(b)
{
    assert(IsValid());
}

/**
 * Constructor, turns an vector into a color.
 * 
 * @param v The Vector3d to turn into a color where we assign r=x, g=y, b=y.
 */
Color::Color(const Vector3d& v) : r(v.x), g(v.y), b(v.z)
{
}

/**
 * Destructor.
 */
Color::~Color()
{
}

/**
 * Returns the sum of the color and the given color.
 * 
 * @param v The color to add.
 * @returns The resulting color.
 */
Color Color::operator+(const Color& v) const
{
    return Color(r+v.r, g+v.g, b+v.b);
}

/**
 * Returns the difference of the color and the given color.
 * 
 * @param v The color to subtract with.
 * @returns The resulting color.
 */
Color Color::operator-(const Color& v) const
{
    return Color(r-v.r, g-v.g, b-v.b);
}

/**
 * Returns the component-wise product of the color and the given scalar.
 * 
 * @param t The integer to multiply by.
 * @returns The resulting color.
 */
Color Color::operator*(double t) const
{
    return Color(r*t, g*t, b*t);
}

/**
 * Returns the component-wise product of the color and the given integer.
 * 
 * @param t The integer to multiply by.
 * @returns The resulting color.
 */
Color Color::operator*(int t) const
{
    return Color(r*double(t), g*double(t), b*double(t));
}

/**
 * Returns the component-wise product of a color and scalar.
 * 
 * @param t The scalar to multiply by.
 * @param c The color to multiply by.
 * @returns The resulting color.
 */
Color operator*(double t, const Color& c)
{
    return Color(c.r*t, c.g*t, c.b*t);
}

/**
 * Returns the component-wise product of a color and integer.
 * 
 * @param t The integer to multiply by.
 * @param c The color to multiply by.
 * @returns The resulting color.
 */
Color operator*(int t, const Color& c)
{
    return Color(c.r*double(t), c.g*double(t), c.b*double(t));
}

/**
 * Returns the component-wise division of the color by the given scalar.
 * 
 * @param t The scalar to divide by.
 * @returns The resulting color.
 */
Color Color::operator/(double t) const
{
    return Color(r/t, g/t, b/t);
}

/**
 * Returns the component-wise division of the color by the given integer.
 * 
 * @param t The integer to divide by.
 * @returns The resulting color.
 */
Color Color::operator/(int t) const
{
    return Color(r/double(t), g/double(t), b/double(t));
}

/**
 * Returns the component-wise product of the color by the other color.
 * 
 * @param c The color to multiply by.
 * @returns The resulting color.
 */
Color Color::operator*(const Color& v) const
{
    return Color(r*v.r, g*v.g, b*v.b);
}

/**
 * Adds the other color to the color, component-wise.
 * 
 * @param c The color to add.
 * @returns A reference to the color.
 */
Color Color::operator+=(const Color& c)
{
    r+=c.r, g+=c.g, b+=c.b;
    return *this;
}

/**
 * Multiplies the color by the other color, component-wise.
 * 
 * @param c The color to multiply by.
 * @returns A reference to the color.
 */
Color Color::operator*=(const Color& c)
{
    r *= c.r, g *= c.g, b *= c.b;
    return *this;
}

/**
 * Boolean operator.
 * 
 * @returns True If the color isn't black.
 */
Color::operator bool() const
{
    return !!(*this);
}

/**
 * Multiplies the color by the given scalar.
 * 
 * @param t The scalar to multiply by.
 * @returns A reference to the color.
 */
Color Color::operator*=(double f)
{
    r *= f; g *= f;	b *= f;
    return *this;
}

/**
 * Divides the color by the given scalar.
 * 
 * @param t The scalar to divide by.
 * @returns A reference to the color.
 */
Color Color::operator/=(double t)
{
    r /= t;	g /= t;	b /= t;
    return *this;
}

/**
 * Compares two colors.
 * 
 * @param c The color to compare with.
 * @returns True If the colors match.
 */
bool Color::operator==(const Color& c)
{
    return c.r == r && c.g == g && c.b == b;
}

/**
 * Checks if the color is black.
 * 
 * @returns True if all components are zero.
 */
bool Color::operator!() const
{
    return (r == 0 && g == 0 && b == 0);
}

/**
 * Returns the luma of the color.
 * 
 * @returns The luma of the color.
 */
double Color::GetLuma() const
{
    // Rec. 709 would be 0.2126*r + 0.7152*g + 0.0722*b, below is Rec. 601
    return 0.2989*r + 0.5866*g + 0.1145*b;
}

/**
 * Performs validation on the color.
 * 
 * @returns True if each color component is finite (but not necessarily positive).
 */
bool Color::IsValid() const
{
    return std::isfinite(r) && std::isfinite(g) && std::isfinite(b);
    // && r >= 0 && g >= 0 && b >= 0;
}

/**
 * Streams the string to an ostream.
 * 
 * @param s The ostream to output to.
 * @param c The color to save.
 */
std::ostream& operator<<(std::ostream& s, const Color& c)
{
    return(s << "(" << c.r << "," << c.g << "," << c.b << ")");
}

/**
 * Turns a color into its binary representation.
 * 
 * @returns The 0x00RRGGBB representation of the color.
 */
int Color::GetInt() const
{
    return (int)(b*255) | (int)(g*255) << 8 | (int)(r*255) << 16;
}

/**
 * References a color by its index (r,g,b -> 0,1,2)
 * 
 * @param t The index of the color to return.
 */
double& Color::operator[](int t)
{
    return (&r)[t];
}
