#include "Color.h"

const Color Color::Identity = Color(1, 1, 1);
const Color Color::Black = Color(0, 0, 0);

Color::Color()
{
}

Color::Color(int c)
{
    r = (double)((c >> 16) & 0xFF) / 255.0f;
    g = (double)((c >> 8) & 0xFF) / 255.0f;
    b = (double)(c & 0xFF) / 255.0f;
    assert(r >= 0 && g >= 0 && b >= 0 && r == r && g == g && b == b);
}

int Color::GetInt() const
{
    return (int)(b*255) | (int)(g*255) << 8 | (int)(r*255) << 16;
}


Color::Color(double _r, double _g, double _b) : r(_r), g(_g), b(_b)
{
    //assert(IsValid());
}

Color::~Color()
{
}

Color Color::operator+(const Color& v) const
{
    return Color(r+v.r, g+v.g, b+v.b);
}

Color Color::operator-(const Color& v) const
{
    return Color(r-v.r, g-v.g, b-v.b);
}

Color Color::operator*(double t) const
{
    return Color(r*t, g*t, b*t);
}

Color Color::operator*(int t) const
{
    return Color(r*double(t), g*double(t), b*double(t));
}

Color operator*(double t, const Color& c)
{
    return Color(c.r*t, c.g*t, c.b*t);
}

Color operator*(int t, const Color& c)
{
    return Color(c.r*double(t), c.g*double(t), c.b*double(t));
}

Color Color::operator/(double t) const
{
    return Color(r/t, g/t, b/t);
}

Color Color::operator/(int t) const
{
    return Color(r/double(t), g/double(t), b/double(t));
}

Color Color::operator*(const Color& v) const
{
    return Color(r*v.r, g*v.g, b*v.b);
}


Color Color::operator+=(const Color& c)
{
    r+=c.r, g+=c.g, b+=c.b;
    return *this;
}

Color Color::operator*=(const Color& c)
{
    r *= c.r, g *= c.g, b *= c.b;
    return *this;
}

Color::operator bool() const {
    return !!(*this);
}

Color Color::operator*=(double f)
{
    r *= f; g *= f;	b *= f;
    return *this;
}

Color Color::operator/=(double t)
{
    r /= t;	g /= t;	b /= t;
    return *this;
}

bool Color::operator==(const Color& c)
{
    return c.r == r && c.g == g && c.b == b;
}

bool Color::operator!() const
{
    return (r == 0 && g == 0 && b == 0);
}

double Color::GetIntensity() const
{
    return sqrt(r*r + g*g + b*b);
}

double Color::GetMax() const
{
    return __max(r, __max(g, b));
}

double Color::GetSum() const
{
    return r + g + b;
}

double Color::GetLuminance() const
{
    // Rec. 709 would be 0.2126f*r + 0.7152f*g + 0.0722f*b, below is Rec. 601
    return 0.2989f*r + 0.5866f*g + 0.1145f*b;
}

bool Color::IsValid() const
{
    return std::isfinite(r) && std::isfinite(g) && std::isfinite(b) && r >= 0 && g >= 0 && b >= 0;
}

std::ostream& operator << (std::ostream& s , const Color& v)
{
    return(s << "(" << v.r << "," << v.g << "," << v.b << ")");
}

double& Color::operator[](int t)
{
    return (&r)[t];
}